#include "Math/Math.h"
#include "Camera.h"
#include "Node.h"
#include "Position.h"

using namespace DirectX;

namespace Poseidon
{
	Camera::Camera(Hippios::Renderer* renderer) : m_renderer(renderer), nearClip_(DEFAULT_NEARCLIP),
		farClip_(DEFAULT_FARCLIP),
		fov_(DEFAULT_CAMERA_FOV),
		orthoSize_(DEFAULT_ORTHOSIZE),
		aspectRatio_(1.0f),
		m_projection_offset(Vector3::ZERO)
	{
		createPosition(1);
	}

	const Matrix4& Camera::getProjectionMatrixRS(void) const
	{
		updateProjection();

		// if we use direct3d we have to convert to left-hand coord pj matrix
		m_projection_rs = m_projection;

		
		// Convert depth range from [-1,+1] to [0,1]
	/*	m_projection_rs[2][0] = (m_projection_rs[2][0] + m_projection_rs[3][0]) / 2;
		m_projection_rs[2][1] = (m_projection_rs[2][1] + m_projection_rs[3][1]) / 2;
		m_projection_rs[2][2] = (m_projection_rs[2][2] + m_projection_rs[3][2]) / 2;
		m_projection_rs[2][3] = (m_projection_rs[2][3] + m_projection_rs[3][3]) / 2;
		*/
		/*
		// Convert right-handed to left-handed
		m_projection_rs[0][2] = -m_projection_rs[0][2];
		m_projection_rs[1][2] = -m_projection_rs[1][2];
		m_projection_rs[2][2] = -m_projection_rs[2][2];
		m_projection_rs[3][2] = -m_projection_rs[3][2];
		*/

		return m_projection_rs;
	}
	//-----------------------------------------------------------------------
	const Matrix4& Camera::getViewMatrix(void) const
	{
		updateView();

		return m_view_matrix;
	}
	   
	void Camera::updateProjection() const
	{
		// Start from a zero matrix in case it was custom previously
		m_projection = Matrix4::ZERO;

		if (!m_orthographic)
		{
			float h = (1.0f / tanf(fov_ * M_DEGTORAD * 0.5f)) * m_zoom;
			float w = h / aspectRatio_;
			float q = farClip_ / (farClip_ - nearClip_);
			float r = -q * nearClip_;

			m_projection[0][0] = w;
			m_projection[0][2] = m_projection_offset.x * 2.0f;
			m_projection[1][1] = h;
			m_projection[1][2] = m_projection_offset.y * 2.0f;
			m_projection[2][2] = q;
			m_projection[2][3] = r;
			m_projection[3][2] = 1.0f;
		//	projNearClip_ = nearClip_;
		//	projFarClip_ = farClip_;
		}
		else
		{
			float h = (1.0f / (orthoSize_ * 0.5f)) * m_zoom;
			float w = h / aspectRatio_;
			float q = 1.0f / farClip_;
			float r = 0.0f;

			m_projection[0][0] = w;
			m_projection[0][3] = m_projection_offset.x * 2.0f;
			m_projection[1][1] = h;
			m_projection[1][3] = m_projection_offset.y * 2.0f;
			m_projection[2][2] = q;
			m_projection[2][3] = r;
			m_projection[3][3] = 1.0f;
			// Near clip does not affect depth accuracy in ortho projection, so let it stay 0 to avoid problems with shader depth parameters
		//	projNearClip_ = 0.0f;
		//	projFarClip_ = farClip_;
		}

	}

	Matrix4 Camera::makeViewMatrix(const Vector3& position, const Quaternion& orientation,
		const Matrix4* reflectMatrix)
	{
		// This is most efficiently done using 3x3 Matrices
		Matrix3 rot;
		orientation.toRotationMatrix(rot);

		// Make the translation relative to new axes
		Matrix3 rotT = rot.Transpose();
		Vector3 trans = -rotT * position;

		// Make final matrix
		Matrix4 viewMatrix = Matrix4::IDENTITY;
		viewMatrix = rotT; // fills upper 3x3
		viewMatrix[0][3] = trans.x;
		viewMatrix[1][3] = trans.y;
		viewMatrix[2][3] = trans.z;

		// Deal with reflections
		if (reflectMatrix)
		{
			viewMatrix = viewMatrix * (*reflectMatrix);
		}

		return viewMatrix;

	}

	void Camera::updateView(void) const
	{
		// ----------------------
		// Update the view matrix
		// ----------------------

		// Get orientation from quaternion

		//if (!mCustom_view_matrix)
		{
			Matrix3 rot;
			const Quaternion& orientation = m_derived_position->get_rotation();  //getOrientationForViewUpdate();
			const Vector3& position = m_derived_position->get_worldspace();  //getPositionForViewUpdate();

			m_view_matrix = makeViewMatrix(position, orientation, m_reflect ? &m_reflect_matrix : 0);
		}
	}
}
