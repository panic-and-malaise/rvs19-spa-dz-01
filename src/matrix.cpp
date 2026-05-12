#include "matrix.hpp"
#include "tri.hpp"
#include "vec3d.hpp"

#include <math.h>

float* Mat4::operator[](const int index) {
	return m[index];
}

const float* Mat4::operator[](const int index) const {
	return m[index];
}

Mat4 Mat4::identity_matrix() {
	return Mat4{{
		{ 1.0f,  0.0f,  0.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f,  0.0f },
		{ 0.0f,  0.0f,  1.0f,  0.0f },
		{ 0.0f,  0.0f,  0.0f,  1.0f }
	}};
}

float Mat4::at(const int row, const int column) const {
	return m[row][column];
}

float& Mat4::at(const int row, const int column) {
	return m[row][column];
}

Mat4 Mat4::from_vec3d(const Vec3D& vec) {
	return Mat4{{
		{ 1.0f,  0.0f,  0.0f,  0.0f },
		{ 0.0f,  1.0f,  0.0f,  0.0f },
		{ 0.0f,  0.0f,  1.0f,  0.0f },
		{ vec.x, vec.y, vec.z, 1.0f }
	}};
}

Mat4 Mat4::operator*(const Mat4& other) {
	Mat4 result{};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			result.at(row, col) =
				at(row, 0) * other.at(0, col) +
				at(row, 1) * other.at(1, col) +
				at(row, 2) * other.at(2, col) +
				at(row, 3) * other.at(3, col);
		}
	}

	return result;
}

void Mat4::multiply_matrix_vector(const Vec3D& in, Vec3D& out, const Mat4& mat) {
	out.x  = in.x * mat.at(0, 0) + in.y * mat.at(1, 0) + in.z * mat.at(2, 0) + mat.at(3, 0);
	out.y  = in.x * mat.at(0, 1) + in.y * mat.at(1, 1) + in.z * mat.at(2, 1) + mat.at(3, 1);
	out.z  = in.x * mat.at(0, 2) + in.y * mat.at(1, 2) + in.z * mat.at(2, 2) + mat.at(3, 2);
	float w = in.x * mat.at(0, 3) + in.y * mat.at(1, 3) + in.z * mat.at(2, 3) + mat.at(3, 3);

	// Normalize
	if (w != 0.0f) {
		out.x /= w;
		out.y /= w;
		out.z /= w;
	}
}

Mat4 Mat4::rotation_matrix_x(float angle) {
	return {{
		{1, 0, 0, 0},
		{0, cosf(angle) , sinf(angle), 0},
		{0, -sinf(angle), cosf(angle), 0},
		{0, 0, 0, 1}
	}};
}


Mat4 Mat4::rotation_matrix_y(float angle) {
	return {{
		{ cosf(angle),  0, -sinf(angle), 0 },
		{ 0, 	 1, 0, 	  0 },
		{ sinf(angle), 0, cosf(angle), 0 },
		{ 0, 	 0, 0, 	  1 }
	}};
}

Mat4 Mat4::rotation_matrix_z(float angle) {
	return {{
		{cosf(angle) , sinf(angle), 0, 0},
		{-sinf(angle), cosf(angle), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	}};
}

Mat4 Mat4::quick_inverse(const Mat4& matrix) {
	Mat4 inverse = {{
		{ matrix[0][0], matrix[1][0], matrix[2][0], 0.0f },
		{ matrix[0][1], matrix[1][1], matrix[2][1], 0.0f },
		{ matrix[0][2], matrix[1][2], matrix[2][2], 0.0f },
		{ 0.0f, 0.0f, 0.0f, 						1.0f }
	}};

	inverse[3][0] = -(matrix[3][0] * inverse[0][0] + matrix[3][1] * inverse[1][0] + matrix[3][2] * inverse[2][0]);
	inverse[3][1] = -(matrix[3][0] * inverse[0][1] + matrix[3][1] * inverse[1][1] + matrix[3][2] * inverse[2][1]);
	inverse[3][2] = -(matrix[3][0] * inverse[0][2] + matrix[3][1] * inverse[1][2] + matrix[3][2] * inverse[2][2]);

	return inverse;
}

Mat4 Mat4::point_at(const Vec3D& in, const Vec3D& target, const Vec3D& up) {
	Vec3D new_forward = target - in;
	new_forward.normalize();

	Vec3D a = new_forward * Vec3D::dot_product(up, new_forward);
	Vec3D new_up = up - a;
	new_up.normalize();

	Vec3D new_right = Vec3D::cross_product(new_forward, new_up);

	Mat4 point_at_matrix = {{
		{ new_right.x,   new_right.y, 	new_right.z,   0.0f },
		{ new_up.x, 	 new_up.y, 		new_up.z, 	   0.0f },
		{ new_forward.x, new_forward.y, new_forward.z, 0.0f },
		{ in.x, 		 in.y, 			in.z, 		   1.0f }
	}};

	return point_at_matrix;
}

Mat4 Mat4::look_at(const Vec3D& in, const Vec3D& target, const Vec3D& up) {
	Mat4 look_at_matrix = quick_inverse(point_at(in, target, up));
	return look_at_matrix;
}
