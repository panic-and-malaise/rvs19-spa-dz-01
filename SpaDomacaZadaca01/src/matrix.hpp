#pragma once

struct Vec3D;

struct Mat4 {
	float m[4][4];

	float* operator[](const int index);
	const float* operator[](const int index) const;

	static Mat4 identity_matrix();

	float at(const int row, const int column) const;
	float& at(const int row, const int column);

	static Mat4 from_vec3d(const Vec3D& vec);

	Mat4 operator*(const Mat4& other);

	static void multiply_matrix_vector(const Vec3D& in, Vec3D& out, const Mat4& mat);

	static Mat4 rotation_matrix_x(float angle);
	static Mat4 rotation_matrix_y(float angle);
	static Mat4 rotation_matrix_z(float angle);

	static Mat4 quick_inverse(const Mat4& matrix);

	static Mat4 point_at(const Vec3D& in, const Vec3D& target, const Vec3D& up);
	static Mat4 look_at(const Vec3D& in, const Vec3D& target, const Vec3D& up);
};
