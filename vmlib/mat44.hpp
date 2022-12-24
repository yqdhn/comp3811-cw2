#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*(const Mat44f& aLeft, const Mat44f& aRight ) noexcept
{
	return Mat44f{
		// 1
		aLeft(0,0) * aRight(0,0) + aLeft(0,1) * aRight(1,0) + aLeft(0,2) * aRight(2,0) + aLeft(0,3) * aRight(3,0),
		aLeft(0,0) * aRight(0,1) + aLeft(0,1) * aRight(1,1) + aLeft(0,2) * aRight(2,1) + aLeft(0,3) * aRight(3,1),
		aLeft(0,0) * aRight(0,2) + aLeft(0,1) * aRight(1,2) + aLeft(0,2) * aRight(2,2) + aLeft(0,3) * aRight(3,2),
		aLeft(0,0) * aRight(0,3) + aLeft(0,1) * aRight(1,3) + aLeft(0,2) * aRight(2,3) + aLeft(0,3) * aRight(3,3),
		// 2
		aLeft(1,0) * aRight(0,0) + aLeft(1,1) * aRight(1,0) + aLeft(1,2) * aRight(2,0) + aLeft(1,3) * aRight(3,0),
		aLeft(1,0) * aRight(0,1) + aLeft(1,1) * aRight(1,1) + aLeft(1,2) * aRight(2,1) + aLeft(1,3) * aRight(3,1),
		aLeft(1,0) * aRight(0,2) + aLeft(1,1) * aRight(1,2) + aLeft(1,2) * aRight(2,2) + aLeft(1,3) * aRight(3,2),
		aLeft(1,0) * aRight(0,3) + aLeft(1,1) * aRight(1,3) + aLeft(1,2) * aRight(2,3) + aLeft(1,3) * aRight(3,3),
		// 3
		aLeft(2,0) * aRight(0,0) + aLeft(2,1) * aRight(1,0) + aLeft(2,2) * aRight(2,0) + aLeft(2,3) * aRight(3,0),
		aLeft(2,0) * aRight(0,1) + aLeft(2,1) * aRight(1,1) + aLeft(2,2) * aRight(2,1) + aLeft(2,3) * aRight(3,1),
		aLeft(2,0) * aRight(0,2) + aLeft(2,1) * aRight(1,2) + aLeft(2,2) * aRight(2,2) + aLeft(2,3) * aRight(3,2),
		aLeft(2,0) * aRight(0,3) + aLeft(2,1) * aRight(1,3) + aLeft(2,2) * aRight(2,3) + aLeft(2,3) * aRight(3,3),
		// 4
		aLeft(3,0) * aRight(0,0) + aLeft(3,1) * aRight(1,0) + aLeft(3,2) * aRight(2,0) + aLeft(3,3) * aRight(3,0),
		aLeft(3,0) * aRight(0,1) + aLeft(3,1) * aRight(1,1) + aLeft(3,2) * aRight(2,1) + aLeft(3,3) * aRight(3,1),
		aLeft(3,0) * aRight(0,2) + aLeft(3,1) * aRight(1,2) + aLeft(3,2) * aRight(2,2) + aLeft(3,3) * aRight(3,2),
		aLeft(3,0) * aRight(0,3) + aLeft(3,1) * aRight(1,3) + aLeft(3,2) * aRight(2,3) + aLeft(3,3) * aRight(3,3),
	};
}

constexpr
Vec4f operator*(const Mat44f& aLeft, const Vec4f& aRight ) noexcept
{
	return Vec4f{
		aLeft(0,0) * aRight.x + aLeft(0,1) * aRight.y + aLeft(0,2) * aRight.z + aLeft(0,3) * aRight.w,
		aLeft(1,0) * aRight.x + aLeft(1,1) * aRight.y + aLeft(1,2) * aRight.z + aLeft(1,3) * aRight.w,
		aLeft(2,0) * aRight.x + aLeft(2,1) * aRight.y + aLeft(2,2) * aRight.z + aLeft(2,3) * aRight.w,
		aLeft(3,0) * aRight.x + aLeft(3,1) * aRight.y + aLeft(3,2) * aRight.z + aLeft(3,3) * aRight.w,
	};
}

// Functions:

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	return Mat44f{
		1.f, 0.f, 0.f, 0.f,
		0,cos(aAngle),-sin(aAngle),0,
		0,sin(aAngle),cos(aAngle),0,
		0.f, 0.f, 0.f, 1.f
	};
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	return Mat44f{
		cos(aAngle), 0.f, sin(aAngle), 0.f,
		0.f , 1.f, 0.f, 0.f,
		-sin(aAngle), 0.f, cos(aAngle), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	return Mat44f{
		cos(aAngle), -sin(aAngle), 0.f, 0.f,
		sin(aAngle), cos(aAngle), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_translation(const Vec3f& aTranslation ) noexcept
{
	return Mat44f{
		1.f, 0.f, 0.f, aTranslation.x,
		0.f, 1.f, 0.f, aTranslation.y,
		0.f, 0.f, 1.f, aTranslation.z,
		0.f, 0.f, 0.f, 1.f
	};
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	return Mat44f{
		aSX, 0.f, 0.f, 0.f,
		0.f, aSY, 0.f, 0.f,
		0.f, 0.f, aSZ, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

//NOTE: this is perspective projection for a symmetric frustum
inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float s = 1 / tan(aFovInRadians * 0.5f);
	float a = -(aFar + aNear) / (aFar - aNear);
	float b = -2 * (aFar * aNear) / (aFar - aNear);
	return Mat44f{
		s / aAspect, 0.f, 0.f, 0.f,
		0.f, s, 0.f, 0.f,
		0.f, 0.f, a, b,
		0.f, 0.f, -1.f, 0.f
	};
}

//Compute the matrix to perform a conversion from world coordinates to camera coordinates.
//Input:
// 'cameraPos' - The camera position.
// 'forwardDir' - The direction which the camera is looking at.
// 'up' - Some vector pointing upwards (need not be orthogonal to the other two). Often some world "up" direction is used as it is assumed that the camera
//        will not be positioned upside-down.
inline Mat44f make_view_matrix(const Vec3f& cameraPos, const Vec3f& forwardDir, const Vec3f& up)
{
	//Implementation follows the approach in: https://learnopengl.com/Getting-started/Camera
	
	//Ensure that the forward and up vectors are normalized.
	Vec3f forward = -normalize(forwardDir);
	Vec3f upN = normalize(up);

	//Compute the vector pointing to the left of the camera.
	Vec3f left = cross(up, forward);

	//Compute the new Up vector that together with the left and forward vectors, forms an orthonormal basis.
	upN = cross(forward, left);

	//The transformation is composed of a translation, followed by a rotation.
	//View mat = rotation * translation, 
	//where:
	// - translation - move the camera position at the origin (i.e. translate by -cameraPos);
	// - rotation - a coordinate transformation from the current coordinate system to the one with basis given by <left,up,forward>
	//              (i.e. the three vectors are the rows of the matrix).
	
	//After performing the multiplication, the final matrix takes the form:
	return Mat44f{
		left.x,    left.y,    left.z,    -dot(left,cameraPos),
		upN.x,     upN.y,     upN.z,     -dot(upN,cameraPos),
		forward.x, forward.y, forward.z, -dot(forward,cameraPos),
		0.0f,      0.0f,      0.0f,       1.0f
	};
}

//Compute a "LookAt" matrix which converts the current coordinates to camera space, where we know the camera position and an object the camera is looking at.
//Input:
// 'cameraPos' - The position of the camera.
// 'poi' - Point of Interest. A point in space the camera is looking at. (Used to fetch forward direction)
// 'up' - A vector pointing upwards. Usually some world up direction is used, as it is assumed that the camera will not be positioned upside-down.
inline Mat44f make_lookat(const Vec3f& cameraPos, const Vec3f& poi, const Vec3f& up)
{
	return make_view_matrix(cameraPos, poi - cameraPos, up);
}

#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
