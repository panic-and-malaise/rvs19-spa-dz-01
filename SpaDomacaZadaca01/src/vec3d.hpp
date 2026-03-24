#pragma once

struct Vec3D {
	float x{}, y{}, z{};

	Vec3D operator+(const Vec3D& other) const;
	Vec3D operator-(const Vec3D& other) const;
	void operator+=(const Vec3D& other);

	Vec3D operator*(const float scalar) const;

	void normalize();

	float length() const;
	float length_squared() const;

	static float dot_product(const Vec3D& a, const Vec3D& b);
	static Vec3D cross_product(const Vec3D& a, const Vec3D& b);

	static Vec3D vector_cross_section_with_plane(const Vec3D vector_start, const Vec3D vector_end, const Vec3D plane_point, const Vec3D plane_normal);
};
