#ifndef POSEIDON_CAMERA_H
#define POSEIDON_CAMERA_H

#include "Hippios/Renderer.h"
#include "Hippios/GraphicsBuffer.h"

#include "Node.h"

#include "Node.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/Ray.h"

namespace Poseidon
{
	static const float DEFAULT_ORTHOSIZE = 20.0f;
	static const float DEFAULT_NEARCLIP = 0.1f;
	static const float DEFAULT_FARCLIP = 1000.0f;
	static const float DEFAULT_CAMERA_FOV = 45.0f;

	class Viewport;

	class Camera : public Node
	{
	protected:
		Hippios::Renderer* m_renderer;

		float rot = 0.01f;

		mutable Matrix4 m_projection;
		mutable Matrix4 m_projection_rs;
		/// Pre-calced view matrix
		mutable Matrix4 m_view_matrix;
		/// orthographic mode
		bool m_orthographic = false;

		bool m_reflect = false;
		Matrix4 m_reflect_matrix;

		static Matrix4 makeViewMatrix(const Vector3& position, const Quaternion& orientation,
			const Matrix4* reflectMatrix = 0);

		/// Zoom.
		float m_zoom = 1.0f;
		
		/// Near clip distance.
		float nearClip_;
		/// Far clip distance.
		float farClip_;
		/// Field of view.
		float fov_;
		/// Orthographic view size.
		float m_ortho_size;
		/// Aspect ratio.
		float aspectRatio_;

		Vector3 m_projection_offset;

	public:
		Camera(Hippios::Renderer* renderer);
		bool onUpdate() { return false; };
		
		const Matrix4& getProjectionMatrixRS(void) const;
		const Matrix4& getViewMatrix(void) const;		

		void updateProjection() const;
		void updateView(void) const;
		void setProjectionOffset(Vector3 offset);

		void setOrthographic(bool enabled);
		void setOrthographicSize(float size);
		void setAspectRatio(float ratio) { aspectRatio_ = ratio; }

		bool render(Viewport*, Camera*) { return true; }

		Ray getPickingRay(float scr_x, float scr_y, int viewport_width, int viewport_height);
	};
}

#endif // !POSEIDON_CAMERA_H
