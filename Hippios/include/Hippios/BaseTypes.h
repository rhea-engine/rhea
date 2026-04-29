/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_BASETYPES_H
#define HIPPIOS_BASETYPES_H

namespace hippios
{
	struct GPUHandle {};

	enum class ComparisonFunction
	{
		COMPARE_NEVER = 0,
		COMPARE_LESS = 1,
		COMPARE_EQUAL = 2,
		COMPARE_LESS_EQUAL = 3,
		COMPARE_GREATER = 4,
		COMPARE_NOT_EQUAL = 5,
		COMPARE_GREATER_EQUAL = 6,
		COMPARE_ALWAYS = 7
	};

	enum class WindowBackend
	{
		Unknown = 0,
		Win32,
		X11,
		Wayland,
		SDL,
		GLFW
	};

	struct NativeWindow
	{
		void* native_win_handle = nullptr;
		int width = -1;
		int height = -1;
	};

	struct ViewportDesc
	{
		float x;
		float y;
		float w;
		float h;
		float min_depth;
		float max_depth;
	};

	enum class ShaderStage
	{
		Vertex,
		Fragment,
		Geometry,
		Compute
	};
}
#endif // !HIPPIOS_BASETYPES_H
