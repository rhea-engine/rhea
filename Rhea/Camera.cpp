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

	bool Camera::getGraphicsCamera()
	{
		auto it = m_elements.find("graphics");
		if (it != m_elements.end()) {
			m_graphics_camera = static_cast<GraphicsCamera*>(it->second);
			return true;
		}
		return false;
	}

	void Camera::setPosition(Poseidon::Position& position)
	{
		if (!m_graphics_camera)
			if (!getGraphicsCamera())
				return;
		m_graphics_camera->setPosition(position);
	}

	void Camera::setPosition(Poseidon::Vector3 position)
	{
		if (!m_graphics_camera)
			if (!getGraphicsCamera())
				return;
		m_graphics_camera->setPosition(position);
	}

	void Camera::setLookAt(Poseidon::Vector3 look_at)
	{
		if (!m_graphics_camera)
			if (!getGraphicsCamera())
				return;
		m_graphics_camera->lookAt(look_at);
	}

	Unit* Camera::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Camera(name, type);
		return ret;
	}
}