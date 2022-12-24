#pragma once
#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

enum class CameraMoveDir { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

//TODO remove
constexpr float PI = 3.1415926f;

//TODO add descr
class Camera
{
public:
	//Default camera:
	// - positioned at the origin;
	// - looking at the -Z axis;
	// - has vertical field of view of 45 deg.
	Camera();

	Mat44f getViewMatrix() const;
	Vec3f getPosition() const;
	Vec3f getForwardDir() const;
	Vec3f getRightDir() const;
	Vec3f getUpDir() const;
	float getVerticalFOV() const;
	bool isActive() const;

	void move(const CameraMoveDir& direction, float deltaTime);
	void rotate(float xDelta, float yDelta);
	void zoom(float fovDelta);
	void zoomIn();
	void zoomOut();
	void setVerticalFOV(float fovY);
	void setPosition(const Vec3f& pos);
	void speedUp();
	void speedDown();
	bool toggleActive();

private:
	//Camera position in world coordinates. Gives the origin of the camera coordinate system.
	Vec3f pos;

	//We store 2 out of the 3 basis vectors for the camera coordinate system. We store these to avoid
	//repeated recomputation. However, the vectors themselves are derived from the angles below.
	Vec3f forward;
	Vec3f right;

	//The current orientation of the camera, given in (modified) spherical coordinates.
	//The two angles correspond to the direction of the unit-length forward vector of the camera.
	//See reference on spherical coordinates: https://mathworld.wolfram.com/SphericalCoordinates.html
	//We modify the framework to account for the fact that in OpenGL, the y axis points upward.
	//Thus, a point with yaw = 0 and pitch = 0 will lie on the positive z axis (ordinarily, it would lie on the positive x axis).

	float yaw;//This is the azimuthal angle in radians (thehta in the reference). It ranges from 0 to 2*PI.
			  //However, it is defined to be an angle in the xz plane, starting from z and moving towards x (because in OpenGL, the y axis points upward).
	float pitch;//This is the colatitude angle in radians (phi in the reference). It ranges from 0 to PI.
				//However, it is defined to be an angle in the yz plnae, starting from y and moving towards z (because in OpenGL, the y axis points upward).

	//A vertical field of view angle in radians. Used for the perspective projection.
	float fovY;

	float moveSpeed; //Given in units per second
	bool bActive;


	//Camera class constants
	static constexpr Vec3f WORLD_FORWARD = { 0.0f,0.0f,-1.0f };
	static constexpr Vec3f WORLD_UP = { 0.0f,1.0f,0.0f };
	static constexpr Vec3f WORLD_RIGHT = { 1.0f,0.0f,0.0f };

	static constexpr float DEF_MOVEMENT_SPEED = 5.0f;//units per second
	static constexpr float MOVEMENT_SPEED_UPDATE = 2.0f;//units per second^2 (acceleration)
	static constexpr float MIN_MOVEMENT_SPEED = 1.0f;
	static constexpr float MAX_MOVEMENT_SPEED = 9.0f;

	static constexpr float DEF_MOUSE_SENSITIVITY = 0.002f;//radians per pixel
	static constexpr float DEF_ZOOM_SENSITIVITY = 0.02f;

	static constexpr float MIN_PITCH = 0.3490658f;//20 deg
	static constexpr float MAX_PITCH = 2.7925268f;//160 deg
	static constexpr float MIN_FOVY = PI / 12.0f;//15 deg
	static constexpr float MAX_FOVY = PI / 4.0f;//45 deg
};

inline Camera::Camera() :pos({ 0.0f,0.0f,0.0f }), forward({ 0.0f,0.0f,-1.0f }), right({ 1.0f,0.0f,0.0f }), yaw(PI), pitch(PI / 2.0f), fovY(PI / 4.0f),
moveSpeed(DEF_MOVEMENT_SPEED), bActive(false)
{
}

inline Vec3f Camera::getPosition() const
{
	return pos;
}

inline Vec3f Camera::getForwardDir() const
{
	return forward;
}

inline Vec3f Camera::getRightDir() const
{
	return right;
}

inline Vec3f Camera::getUpDir() const
{
	return cross(right, forward);
}

inline float Camera::getVerticalFOV() const
{
	return fovY;
}

inline bool Camera::isActive() const
{
	return bActive;
}

inline void Camera::setVerticalFOV(float fov)
{
	fovY = fov;
}

inline void Camera::setPosition(const Vec3f& position)
{
	pos = position;
}

inline bool Camera::toggleActive()
{
	bActive = !bActive;
	return bActive;
}

inline void Camera::zoomIn()
{
	fovY = MIN_FOVY;
}

inline void Camera::zoomOut()
{
	fovY = MAX_FOVY;
}