/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "AuthBackendFile.h"
#include <Orion/FileStream.h>
#include <Psio/Log.h>
#include <nlohmann/json.hpp>

void AuthBackendFile::setup(rhea::Application* app)
{
	m_app = app;
	loadUsers();
}

void AuthBackendFile::LookupByUserID(std::string user_id, DBLookupHandler handler)
{
	for (auto& u : m_store.m_users) {
		if (u.user_id == user_id) {
			handler(u);
			return;
		}
	}
	handler(UserInfo());
}

void AuthBackendFile::LookupByUsername(std::string username, DBLookupHandler handler)
{
	for (auto& u : m_store.m_users) {
		if (u.username == username) {
			handler(u);
			return;
		}
	}
	handler(UserInfo());
}

void AuthBackendFile::LookupByExternalID(std::string ext_id, DBLookupHandler handler)
{
	for (auto& u : m_store.m_users) {
		if (u.ext_id == ext_id) {
			handler(u);
			return;
		}
	}
	handler(UserInfo());
}

void AuthBackendFile::LookupBySessionKey(std::string session_id, DBLookupHandler handler)
{
	for (auto& u : m_store.m_users) {
		if (u.session_key == session_id) {
			handler(u);
			return;
		}
	}
	handler(UserInfo());
}

void AuthBackendFile::SetSessionKey(std::string user_id, std::string session_id, std::function<void(int err)> handler)
{
	for (auto& u : m_store.m_users) {
		if (u.user_id == user_id) {
			u.session_key = session_id;
			saveUsers();
			handler(0);
			return;
		}
	}
	handler(-1);
}

void AuthBackendFile::CreateUser(UserInfo info, std::vector<std::string> secrets, std::function<void(std::string, int err)> handler)
{
	// safety check
	if (info.username == "") {
		handler("", -1);
		return;
	}
	// does username already exist
	for (auto& u : m_store.m_users) {
		if (u.username == info.username) {
			handler("", -2);
			return;
		}
	}

	if (secrets.size() == 0) {
		m_store.m_users.push_back({ info.user_id, info.username, info.ext_id, info.name, "" });
	}
	else {
		m_store.m_users.push_back({ info.user_id, info.username, info.ext_id, info.name, secrets[0] });
	}
	saveUsers();
	handler(info.user_id, 0);
}

void AuthBackendFile::loadUsers()
{
	// read from auth file
	std::string cred_path = m_app->getFS()->getFs()->getAppDataDir();
	cred_path += string("/auth.json");

	if (m_app->getFS()->getFs()->fileExists(cred_path))
	{
		// file exists		
		std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(cred_path);
		std::unique_ptr<psio::IOBuffer> buf = std::make_unique<psio::IOBuffer>(64 * 1024);
		if (stream->Read(buf.get())) {
			const char* ptr = buf->reader();
			size_t len = buf->Remaining();
			try {
				nlohmann::json auths = nlohmann::json::parse(ptr, ptr + len, nullptr, false);
				LOG(LOG_INFO) << "Loaded " << auths.size() << " users from auth file";
				for (auto& auth : auths) {
					m_store.m_users.push_back({ auth["user_id"], auth["username"], auth["ext_id"], auth["name"], auth["password"] });
				}
			}
			catch (nlohmann::json::out_of_range& e) {
				LOG(LOG_ERROR) << "Error parsing auth file: " << e.what();
			}
		}
	}
}

void AuthBackendFile::saveUsers()
{
	std::string cred_path = m_app->getFS()->getFs()->getAppDataDir();
	cred_path += string("/auth.json");

	std::string s = m_store.toJSON();
	std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(cred_path);
	stream->SetMode(psio::Fs::WRITE);
	stream->Open();
	stream->Write(s.c_str(), s.size());
	stream->Close();
}

// json function for userinfo

void to_json(nlohmann::json& j, const UserInfo& p) {
	j = nlohmann::json{
		{"user_id", p.user_id},
		{"username", p.user_id},
		{"name", p.name},
		{"password", p.password_hash},
		{"ext_id", p.ext_id},
		{"sessionkey", p.session_key}
	};
}

void from_json(const nlohmann::json& j, UserInfo& p) {
    j.at("name").get_to(p.name);
	j.at("ext_id").get_to(p.ext_id);
	j.at("user_id").get_to(p.user_id);
	j.at("username").get_to(p.user_id);
	j.at("password").get_to(p.password_hash);
	j.at("sessionkey").get_to(p.session_key);
}

std::string AuthBackendFile::UserStore::toJSON()
{
	nlohmann::json j = m_users;
	return j.dump(4);
}