#include "vec3d.hpp"

#include <cmath>

Vec3D Vec3D::operator+(const Vec3D& other) const {
	return Vec3D{
		x + other.x,
		y + other.y,
		z + other.z
	};
}

Vec3D Vec3D::operator-(const Vec3D& other) const {
	return Vec3D{
		x - other.x,
		y - other.y,
		z - other.z
	};
}

void Vec3D::operator+=(const Vec3D& other) {
	x += other.x;
	y += other.y;
	z += other.z;
}

Vec3D Vec3D::operator*(const float scalar) const {
	return Vec3D{
		x * scalar,
		y * scalar,
		z * scalar,
	};
}

void Vec3D::normalize() {
	float length = std::sqrt(x*x + y*y + z*z);

	x /= length;
	y /= length;
	z /= length;
}

// Best to avoid entirely when possible! Square roots are expensive and take dozens of cycles, even on new CPUs.
float Vec3D::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

float Vec3D::length_squared() const {
    return x*x + y*y + z*z;
}

float Vec3D::dot_product(const Vec3D& a, const Vec3D& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3D Vec3D::cross_product(const Vec3D& a, const Vec3D& b) {
	return Vec3D{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

Vec3D Vec3D::vector_cross_section_with_plane(Vec3D vector_start, Vec3D vector_end, Vec3D plane_point, Vec3D plane_normal) {
	plane_normal.normalize();

	float plane_D = -dot_product(plane_normal, plane_point);
	float ad = dot_product(vector_start, plane_normal);
	float bd = dot_product(vector_end, plane_normal);
	
	float t = (-plane_D - ad) / (bd - ad);

	Vec3D line = vector_end - vector_start;
	Vec3D line_intersection = line * t;

	return (vector_start + line_intersection);
}
