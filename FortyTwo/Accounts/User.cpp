#include "User.h"

Rhea::Avatar* User::getAvatar(std::string name)
{
	Rhea::Avatar* avatar = doCast<Rhea::Avatar>(getChild(name));
	return avatar;
}

void User::Avatars(std::function<bool(Rhea::Avatar*)> handler)
{
	children([handler](std::string& name, Rhea::Unit* unit)->bool {
		Rhea::Avatar* avatar = doCast<Rhea::Avatar>(unit);
		if (avatar)
			return handler(avatar);
		else
			return true;
		});
}
