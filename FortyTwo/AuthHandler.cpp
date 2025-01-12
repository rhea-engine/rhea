#include "AuthHandler.h"
#include <Rhea/UnitTree.h>
#include <Rhea/Fabrics/Core/Item.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessagePosition.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include <Psio/Log.h>
#include <Rhea/Fabrics/Network/ServerService.h>
#include "World.h"
#include <Hermes/Messages/MessageEnterFailed.h>
#include <Hermes/Messages/MessageLogin.h>
#include "Accounts/AccountService.h"
#include "Authorization/AuthorizationService.h"
#include <Hermes/Messages/MessageCreateAvatar.h>

void AuthHandler::Initialize()
{
	// auth state
	m_hermes->onLogin([&](std::string method, std::string username, std::vector<std::string> secrets) {
		Login(method, username, secrets);
		});

	m_hermes->onCreateUser(std::bind(&AuthHandler::CreateUser, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
}

void AuthHandler::CreateUser(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)
{
	AuthorizationService* auth_srv = doCast<AuthorizationService>(m_app->getService("authorization"));
	if (!auth_srv) {
		LOG(LOG_ERROR) << "Configuration error: Authorization service not found";
		// Send auth failure
		m_state = CONN_LOGIN;
		m_hermes->AuthFailure([&](int result) {
			m_hermes->LoginRetry();
			},
			std::vector<std::string>());
		return;
	}
	UserInfo user_info;
	user_info.username = username;
	user_info.user_id = "";
	if (name != "")
		user_info.name = name;
	else
		user_info.name = username;
	user_info.ext_id = ext_id;
	auth_srv->CreateUser(method, user_info, secrets, [&, user_info, secrets](std::string user_id, int err) {
		if (!err) {
			// success

			// create the account for the user
			AccountService* acc_srv = doCast<AccountService>(m_app->getService("accounts"));
			if (!acc_srv) {
				LOG(LOG_ERROR) << "Configuration error: Account service not found";
				// Send auth failure
				m_state = CONN_LOGIN;
				m_hermes->AuthFailure([&](int result) {
					m_hermes->LoginRetry();
					},
					std::vector<std::string>());
				return;
			}

			// log the user in
			Login(method, user_info.username, secrets);
		}
		else {
			// Send auth failure
			m_state = CONN_LOGIN;
			m_hermes->AuthFailure([&](int result) {
				m_hermes->LoginRetry();
				},
				std::vector<std::string>());
		}
		});
}

void AuthHandler::Login(std::string method, std::string username, std::vector<std::string> secrets) {

	// login message received - try to log the user in
	m_world = (World*)m_app->getUnitTree()->findByType("//meworla/world");
	if (m_world) {
		m_username = username;
		AuthorizationService* auth_srv = doCast<AuthorizationService>(m_app->getService("authorization"));
		if (!auth_srv) {
			LOG(LOG_ERROR) << "Configuration error: Authorization service not found";
			// Send auth failure
			m_state = CONN_LOGIN;
			m_hermes->AuthFailure([&](int result) {
				m_hermes->LoginRetry();
				},
				std::vector<std::string>());
			return;
		}
		auth_srv->getUserId(method, m_username, secrets, [&](AuthInfo auth_info) {

			if (auth_info.user_id == "") {
				LOG(LOG_ERROR) << "User " << m_username << " is not registered";
				// Send auth failure
				m_state = CONN_LOGIN;
				m_hermes->AuthFailure([&](int result) {
					m_hermes->LoginRetry();
					},
					auth_srv->getMethods());
				return;
			}
			AccountService* acc_srv = doCast<AccountService>(m_app->getService("accounts"));
			if (!acc_srv) {
				LOG(LOG_ERROR) << "Configuration error: Account service not found";
				m_user = nullptr;
			}
			else {
				// get the user
				acc_srv->GetAccount(auth_info, [&](User* user, int err) {
					if (!err && user) {
						m_user = user;

						if (m_on_succes_handler)
							m_on_succes_handler(user);

						// login succesful - send auth succcess
						m_hermes->AuthSuccess([&](int result) {
							if (result < -1 && m_error_handler)
								m_error_handler(result);
							});
					}
					else {
						LOG(LOG_ERROR) << "User with id " << auth_info.user_id << " is not registered and could not be created.";
						// Send auth failure
						m_state = CONN_LOGIN;
						m_hermes->AuthFailure([&](int result) {
							m_hermes->LoginRetry();
							},
							auth_srv->getMethods());
					}
					});
			}
			});
	}
	else {
		LOG(LOG_ERROR) << "Configuration error: World not found";
	}
}
