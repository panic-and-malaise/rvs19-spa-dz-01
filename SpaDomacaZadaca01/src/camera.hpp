#pragma once

#include "vec3d.hpp"

# define PI 3.14159265358979323846f

class Camera {
public:
	Vec3D position;

	Camera();
	Camera(const Vec3D& start_pos);

	float get_yaw() const;
	void set_yaw(const float radians);

	float get_pitch() const;
	void set_pitch(const float radians);

	float get_roll() const;
	void set_roll(const float radians);
private:
	float yaw;
	float pitch;
	float roll;

	float snap_to_360(const float radians) const;
};
