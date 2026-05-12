#include "camera.hpp"

#include <cstdlib>

Camera::Camera() : position{ 0.0f, 0.0f, 0.0f }, yaw(0.0f), pitch(0.0f), roll(0.0f) {}

Camera::Camera(const Vec3D& start_pos) : Camera() {
	position = start_pos;
}

float Camera::get_yaw() const {
	return yaw;
}

void Camera::set_yaw(const float radians) {
	yaw = radians;
}

float Camera::get_pitch() const {
	return pitch;
}

void Camera::set_pitch(const float radians) {
	constexpr float max_pitch = 89.0f * (PI / 180.0f); // Just slighly under 90 degrees
	pitch = radians;

	if (pitch > max_pitch)
		pitch = max_pitch;
	if (pitch < -max_pitch)
		pitch = -max_pitch;
}

float Camera::get_roll() const {
	return roll;
}

void Camera::set_roll(const float radians) {
	roll = radians;
}

float Camera::snap_to_360(const float radians) const {
	return radians;
	float out = radians;

	constexpr float CIRCLE_SIZE = PI;

	if (std::abs(radians) > CIRCLE_SIZE) {
		out = std::abs(radians) - CIRCLE_SIZE;
	} else if (std::abs(radians) > PI/2) {
		out = -radians;
	}

	return out;
}
