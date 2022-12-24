#include"camera.hpp"
#include<cmath>
#include<stdio.h>

Mat44f Camera::getViewMatrix() const
{
	//Implementation follows the approach in: https://learnopengl.com/Getting-started/Camera

	//Form an orthonormal basis of vectors pointing to the right, upwards, and backwards of the camera (we want a right-handed coordinate system
	//and the camera is looking at the -Z direction).
	Vec3f backward = -forward;
	Vec3f up = getUpDir();

	//The transformation is composed of a translation, followed by a rotation.
	//View mat = rotation * translation, 
	//where:
	// - translation - move the camera position at the origin (i.e. translate by -cameraPos);
	// - rotation - a coordinate transformation from the current coordinate system to the one with basis given by <right,up,backward>
	//              (i.e. the three vectors are the rows of the matrix).

	//After performing the multiplication, the final matrix takes the form:
	return Mat44f{
		right.x,    right.y,    right.z,    -dot(right,pos),
		up.x,       up.y,       up.z,       -dot(up,pos),
		backward.x, backward.y, backward.z, -dot(backward,pos),
		0.0f,       0.0f,       0.0f,        1.0f
	};
}

void Camera::move(const CameraMoveDir& direction, float deltaTime)
{
	float speed = deltaTime * moveSpeed;
	switch (direction)
	{
		//Forward/backward and left/right movement happen only in the xz plane (horizontal plane). 
		//To achieve this effect, we project the camera axes to the xz plane (i.e. remove the component parallel to the plane normal, so 
		//only the component parallel to the plane remains).
		case CameraMoveDir::FORWARD: pos += normalize((forward - projectOnToUnit(forward,WORLD_UP))) * speed; break;
		case CameraMoveDir::BACKWARD: pos -= normalize((forward - projectOnToUnit(forward, WORLD_UP))) * speed; break;
		case CameraMoveDir::LEFT: pos -= normalize((right - projectOnToUnit(right, WORLD_UP))) * speed; break;
		case CameraMoveDir::RIGHT: pos += normalize((right - projectOnToUnit(right, WORLD_UP))) * speed; break;
		case CameraMoveDir::UP: pos += WORLD_UP * speed; break;//Going along the world up direction feels more natural.
		case CameraMoveDir::DOWN: pos -= WORLD_UP * speed; break;
	}
}

void Camera::rotate(float xDelta, float yDelta)
{
	yaw += xDelta * DEF_MOUSE_SENSITIVITY;//in radians
	pitch += yDelta * DEF_MOUSE_SENSITIVITY;//in radians

	pitch = std::fmaxf(std::fminf(pitch, MAX_PITCH), MIN_PITCH);//Clamp pitch

	//Convert spherical to cartesian coordinates for the forward vector.
	//Furmulas in: https://mathworld.wolfram.com/SphericalCoordinates.html
	//However, we have performed the following remapping to account for the fact that in OpenGL the y axis points upward:
	//x -> z
	//y -> x
	//z -> y
	float sP = std::sin(pitch);
	forward.x = sP * std::sin(yaw);
	forward.y = std::cos(pitch);
	forward.z = sP * std::cos(yaw);

	//Update the right vector, 
	right = normalize(cross(forward, WORLD_UP));
}

void Camera::zoom(float fovDelta)
{
	fovY += fovDelta* DEF_ZOOM_SENSITIVITY;
	fovY = std::fmaxf(std::fminf(fovY, MAX_FOVY), MIN_FOVY);//clamp
}

void Camera::speedUp()
{
	moveSpeed += MOVEMENT_SPEED_UPDATE;
	moveSpeed = std::fmaxf(std::fminf(moveSpeed, MAX_MOVEMENT_SPEED), MIN_MOVEMENT_SPEED); //clamp
}

void Camera::speedDown()
{
	moveSpeed -= MOVEMENT_SPEED_UPDATE;
	moveSpeed = std::fmaxf(std::fminf(moveSpeed, MAX_MOVEMENT_SPEED), MIN_MOVEMENT_SPEED); //clamp
}
