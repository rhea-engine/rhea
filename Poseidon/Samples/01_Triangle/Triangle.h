/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Application.h"
#include "Poseidon/FrameRenderer.h"
#include "Hippios/VertexBuffer.h"

class TriangleApp : public rhea::Application
{
protected:
	poseidon::FrameRenderer* m_scene = 0;

	hippios::vertexVec vertices;
public:
	void setup();
	void initialize();
};