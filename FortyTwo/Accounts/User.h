#ifndef FORTYTWO_USER_H
#define FORTYTWO_USER_H

#include <Rhea/Unit.h>
#include <Rhea/Fabrics/Core/Avatar.h>

// class representing the information and resources of a user
// This is not the full account data but everything that is
// needed for displaying and working of the world and login
// 
// it does not include the user's login credentials
// or the user's general data

class User : public Rhea::Unit
{
	MECLASS(User, Rhea::Unit)
public:
	User(std::string name, std::string type) :
		m_allowed_to_login(true),
		m_creation_time(0),
		m_last_login(0),
		Rhea::Unit(name, type)
	{}
	User(std::string name) :
		User(name, "//meworla/user")
	{}

	// get the avatar with the given name
	virtual Rhea::Avatar* getAvatar(std::string name);

	// act on all avatars
	virtual void Avatars(std::function<bool(Rhea::Avatar*)> handler);

	// get the last login time
	time_t getLastLogin() const { return m_last_login; }

	// get the time the login was created
	time_t getCreationTime() const { return m_creation_time; }

	// get the global name tag of the user
	std::string getGlobalTag() const { return m_global_tag; }

	// is the account allowed to login?
	bool isAllowedToLogin() const { return m_allowed_to_login; }

protected:

	// info about the user
	time_t m_last_login;
	time_t m_creation_time;
	std::string m_global_tag;
	bool m_allowed_to_login;
};

#endif // !FORTYTWO_USER_H
