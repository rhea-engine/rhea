#ifndef POSEIDON_CAMERA_H
#define POSEIDON_CAMERA_H

#include "Hippios/Renderer.h"
#include "Hippios/GraphicsBuffer.h"

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/Ray.h"

namespace poseidon
{
	static const float DEFAULT_ORTHOSIZE = 20.0f;
	static const float DEFAULT_NEARCLIP = 0.1f;
	static const float DEFAULT_FARCLIP = 1000.0f;
	static const float DEFAULT_CAMERA_FOV = 45.0f;

	class Viewport;

	class Camera
	{
	public:
		Camera(hippios::Renderer* renderer);
		bool onUpdate() { return false; };
		
		const math::Matrix4& getProjectionMatrixRS(void) const;
		const math::Matrix4& getViewMatrix(void) const;		

		void setProjectionOffset(math::Vector3 offset);
		void setPosition(const math::Vector3& pos);
		const math::Vector3& getPosition() const;
		void setOrientation(const math::Quaternion& rot);
		const math::Quaternion& getOrientation() const;
		void setNearClip(float value);
		void setFarClip(float value);
		void setFov(float value);
		void setOrthographic(bool enabled);
		void setOrthographicSize(float size);
		void setAspectRatio(float ratio) { aspectRatio_ = ratio; }

		bool render(Viewport*, Camera*) { return true; }

		math::Ray getPickingRay(float scr_x, float scr_y, int viewport_width, int viewport_height);

	private:
		void updateProjection() const;
		void updateView() const;

	protected:
		hippios::Renderer* m_renderer;

		math::Vector3 m_position = math::Vector3::ZERO;
		math::Quaternion m_orientation = math::Quaternion(1.f, 0.f, 0.f, 0.f);


		float rot = 0.01f; // TODO: needed?

		mutable math::Matrix4 m_projection;
		mutable math::Matrix4 m_projection_rs;
		/// Pre-calced view matrix
		mutable math::Matrix4 m_view_matrix;
		/// orthographic mode
		bool m_orthographic = false;

		bool m_reflect = false;
		math::Matrix4 m_reflect_matrix;

		static math::Matrix4 makeViewMatrix(const math::Vector3& position, const math::Quaternion& orientation,	const math::Matrix4* reflectMatrix = 0);

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

		math::Vector3 m_projection_offset;
	};
}

#endif // !POSEIDON_CAMERA_H
