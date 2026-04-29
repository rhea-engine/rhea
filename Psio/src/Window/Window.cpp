/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Window/Window.h>
#include <Psio/Input/Input.h>

namespace psio
{
	void Window::setInput(Input* input)
	{
		m_input = input;
	}

	void Window::addInputEvent(std::shared_ptr<InputEvent> evt)
	{
		if (m_input)
			m_input->addEvent(evt);
	}

	void Window::onKeyDown(unsigned int key)
	{
		
	}
}