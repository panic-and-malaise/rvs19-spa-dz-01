#include "tri.hpp"
#include "vec3d.hpp"
#include "matrix.hpp"

#include <cstddef>

Tri::Tri() : vertices{}, normals{}, uv{}, vertex_colors{} {}

Tri::Tri(Vec3D v1, Vec3D v2, Vec3D v3) : Tri() {
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
}

Tri::Tri(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D vn1, Vec3D vn2, Vec3D vn3) : Tri(v1, v2, v3) {
	normals[0] = vn1;
	normals[1] = vn2;
	normals[2] = vn3;
}

Tri::Tri(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D vt1, Vec3D vt2, Vec3D vt3, Vec3D vn1, Vec3D vn2, Vec3D vn3) : Tri(v1, v2, v3, vn1, vn2, vn3) {
	uv[0] = vt1;
	uv[1] = vt2;
	uv[2] = vt3;
}

const Vec3D* Tri::get_vertices() const {
	return vertices;
}

const Vec3D* Tri::get_uv() const {
	return uv;
}

const Vec3D* Tri::get_normals() const {
	return normals;
}

const Vec3D* Tri::get_vertex_colors() const {
	return vertex_colors;
}

void Tri::set_vertex_color(const Vec3D rgb1, const Vec3D rgb2, const Vec3D rgb3) {
	vertex_colors[0] = rgb1;
	vertex_colors[1] = rgb2;
	vertex_colors[2] = rgb3;
}

void Tri::multiply_with_matrix(const Mat4& mat) {
	Mat4::multiply_matrix_vector(get_vertices()[0], vertices[0], mat);
	Mat4::multiply_matrix_vector(get_vertices()[1], vertices[1], mat);
	Mat4::multiply_matrix_vector(get_vertices()[2], vertices[2], mat);
}

void Tri::multiply_with_matrix(const Tri& in, Tri& out, const Mat4& mat) {
	Mat4::multiply_matrix_vector(in.get_vertices()[0], out.vertices[0], mat);
	Mat4::multiply_matrix_vector(in.get_vertices()[1], out.vertices[1], mat);
	Mat4::multiply_matrix_vector(in.get_vertices()[2], out.vertices[2], mat);
}

Vec3D Tri::normal() const {
	Vec3D a {
		vertices[1].x - vertices[0].x,
		vertices[1].y - vertices[0].y,
		vertices[1].z - vertices[0].z
	};

	Vec3D b {
		vertices[2].x - vertices[0].x,
		vertices[2].y - vertices[0].y,
		vertices[2].z - vertices[0].z
	};

	Vec3D normal = Vec3D::cross_product(a, b);
	normal.normalize();

	return normal;
}

void Tri::translate(const Vec3D& trans) {
	vertices[0] = vertices[0] + trans;
	vertices[1] = vertices[1] + trans;
	vertices[2] = vertices[2] + trans;
}

void Tri::normalize_to_viewport(const float viewport_width, const float viewport_heigth) {
	vertices[0].x += 1.0f;
	vertices[0].y += 1.0f;
	vertices[0].x *= 0.5f * viewport_width;
	vertices[0].y *= 0.5f * viewport_heigth;

	vertices[1].x += 1.0f;
	vertices[1].y += 1.0f;
	vertices[1].x *= 0.5f * viewport_width;
	vertices[1].y *= 0.5f * viewport_heigth;

	vertices[2].x += 1.0f;
	vertices[2].y += 1.0f;
	vertices[2].x *= 0.5f * viewport_width;
	vertices[2].y *= 0.5f * viewport_heigth;
}

void Tri::scale_uv_to_texture(const int uv_x, const int uv_y) {
	uv[0].x *= uv_x;
	uv[0].y *= uv_y;

	uv[1].x *= uv_x;
	uv[1].y *= uv_y;

	uv[2].x *= uv_x;
	uv[2].y *= uv_y;
}

// Return value specifies how many of the output Tris will be filled
int Tri::triangles_to_clip(Vec3D plane_point, Vec3D plane_normal, const Tri& input, Tri& out1, Tri& out2) {
	plane_normal.normalize();

	// Signed shortest distance from point to plane
	#define dist(point) (plane_normal.x * point.x + plane_normal.y * point.y + plane_normal.z * point.z - Vec3D::dot_product(plane_normal, plane_point));

	const Vec3D* inside_points[3];
	const Vec3D* outside_points[3];

	size_t inside_point_count = 0;
	size_t outside_point_count = 0;

	float distance1 = dist(input.get_vertices()[0]);
	float distance2 = dist(input.get_vertices()[1]);
	float distance3 = dist(input.get_vertices()[2]);

	if (distance1 >= 0.0f) {
		inside_points[inside_point_count++] = &input.get_vertices()[0];
	} else {
		outside_points[outside_point_count++] = &input.get_vertices()[0];
	}
	if (distance2 >= 0.0f) {
		inside_points[inside_point_count++] = &input.get_vertices()[1];
	} else {
		outside_points[outside_point_count++] = &input.get_vertices()[1];
	}
	if (distance3 >= 0.0f) {
		inside_points[inside_point_count++] = &input.get_vertices()[2];
	} else {
		outside_points[outside_point_count++] = &input.get_vertices()[2];
	}

	if (inside_point_count == 0) {
		return 0;
	} else if (inside_point_count == 3) {
		out1 = input;
		return 1;
	} else if (inside_point_count == 1 && outside_point_count == 2){
		out1 = input;
		out1.vertices[0] = *inside_points[0];

		out1.vertices[1] = Vec3D::vector_cross_section_with_plane(*inside_points[0], *outside_points[0], plane_point, plane_normal);
		out1.vertices[2] = Vec3D::vector_cross_section_with_plane(*inside_points[0], *outside_points[1], plane_point, plane_normal);

		return 1;
	} else if (inside_point_count == 2 && outside_point_count == 1) {
		out1 = input;
		out2 = input;

		out1.vertices[0] = *inside_points[0];
		out1.vertices[1] = *inside_points[1];
		out1.vertices[2] = Vec3D::vector_cross_section_with_plane(*inside_points[0], *outside_points[0], plane_point, plane_normal);

		out2.vertices[0] = *inside_points[1];
		out2.vertices[1] = out1.vertices[2];
		out2.vertices[2] = Vec3D::vector_cross_section_with_plane(*inside_points[1], *outside_points[0], plane_point, plane_normal);

		return 2;
	}

	return 0;
}
