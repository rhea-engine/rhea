#include "Graphics/Camera.h"

#include "Camera.h"

namespace Rhea
{
	void Camera::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (m_status == Status::SETUP) {
			m_status = Status::LOADED;			
			return handler(ReqError::OK);
		}
		else {
			// TODO load myself with other means
			printf("Camera::load without setup not implemented\n");
		}
	}

	Unit* Camera::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Camera(name, type);
		return ret;
	}
}