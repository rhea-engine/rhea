/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_USER_H
#define FORTYTWO_USER_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Avatar.h>

// class representing the information and resources of a user
// This is not the full account data but everything that is
// needed for displaying and working of the world and login
// 
// it does not include the user's login credentials
// or the user's general data

class User : public rhea::Unit
{
	MECLASS(User, rhea::Unit)
public:
	User(std::string name, std::string type) :
		m_allowed_to_login(true),
		m_creation_time(0),
		m_last_login(0),
		rhea::Unit(name, type)
	{}
	User(std::string name) :
		User(name, "//meworla/user")
	{}

	// get the avatar with the given name
	virtual rhea::Avatar* getAvatar(std::string name);

	// act on all avatars
	virtual void Avatars(std::function<bool(rhea::Avatar*)> handler);

	// get the display name
	std::string getDisplayName() const { return m_display_name; }

	// set the display name
	void setDisplayName(std::string name) { m_display_name = name; }

	// get the last login time
	time_t getLastLoginTime() const { return m_last_login; }

	// set the last login time
	void setLastLoginTime(time_t time) { m_last_login = time; }

	// get the time the login was created
	time_t getCreationTime() const { return m_creation_time; }

	// set the creation time
	void setCreationTime(time_t time) { m_creation_time = time; }

	// get the global name tag of the user
	std::string getGlobalTag() const { return m_global_tag; }

	// is the account allowed to login?
	bool isAllowedToLogin() const { return m_allowed_to_login; }

	void ready() override;

protected:

	// info about the user
	std::string m_display_name;
	time_t m_last_login;
	time_t m_creation_time;
	std::string m_global_tag;
	bool m_allowed_to_login;
};

#endif // !FORTYTWO_USER_H
