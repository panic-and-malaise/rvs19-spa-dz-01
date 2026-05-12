#pragma once

#include "vec3d.hpp"

struct Mat4;

struct Tri {
public:
	Tri();
	Tri(Vec3D v1, Vec3D v2, Vec3D v3);
	Tri(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D vn1, Vec3D vn2, Vec3D vn3);
	Tri(Vec3D v1, Vec3D v2, Vec3D v3, Vec3D vt1, Vec3D vt2, Vec3D vt3, Vec3D vn1, Vec3D vn2, Vec3D vn3);

	const Vec3D* get_vertices() const;
	const Vec3D* get_normals() const;
	const Vec3D* get_uv() const;

	const Vec3D* get_vertex_colors() const;
	void set_vertex_color(const Vec3D rgb1, const Vec3D rgb2, const Vec3D rgb3);

	void multiply_with_matrix(const Mat4& mat);
	static void multiply_with_matrix(const Tri& in, Tri& out, const Mat4& mat);

	Vec3D normal() const;

	void translate(const Vec3D& trans);

	void normalize_to_viewport(const float viewport_width, const float viewport_heigth);
	void scale_uv_to_texture(const int uv_x, const int uv_y);

	static int triangles_to_clip(Vec3D plane_point, Vec3D plane_normal, const Tri& input, Tri& out1, Tri& out2);
private:
	Vec3D vertices[3];
	Vec3D normals[3];
	Vec3D uv[3];
	Vec3D vertex_colors[3];
};
