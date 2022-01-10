#pragma once

#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include "tgaimage.h"
#include "tiny_obj_loader.h"
#include "geometry.h"

class Draw
{
public:
	bool load(const std::string& obj) {
		if (!_reader.ParseFromFile(obj)) {
			if (!_reader.Error().empty()) {
				std::cerr << "obj read: " << _reader.Error();
			}
			return false;
		}
		if (!_reader.Warning().empty()) {
			std::cout << "obj read: " << _reader.Warning();
		}
		return true;
	}

	void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
		bool steep = false;
		//x，y轴互换，使其变成斜率在0-1之间
		if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}

		//终点x值小于起点，交换变为base case
		if (x0 > x1) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int dx = x1 - x0;
		int dy = y1 - y0;

		//直线斜率
		float k = std::abs(dy / float(dx));

		float d = 0;
		int y = y0;

		for (int x = x0; x <= x1; x++) {
			//画出像素点
			if (steep)
				image.set(y, x, color);
			else
				image.set(x, y, color);
			//加上直线斜率
			d += k;
			//选择上边的像素
			if (d > 0.5f) {
				y += (y1 > y0 ? 1 : -1);
				d -= 1.0f;
			}
		}
	}

	void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
		if (t0.y == t1.y && t0.y == t2.y) return; // i dont care about degenerate triangles
		if (t0.y > t1.y) std::swap(t0, t1);
		if (t0.y > t2.y) std::swap(t0, t2);
		if (t1.y > t2.y) std::swap(t1, t2);
		int total_height = t2.y - t0.y;
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1.y - t0.y || t1.y == t0.y;
			int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
			Vec2i A = t0 + (t2 - t0) * alpha;
			Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
			if (A.x > B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j++) {
				image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
			}
		}
	}

	void render() {
		static const int WIDTH = 1024;
		static const int HEIGHT = 1024;
		static const TGAColor ORANGE = TGAColor(255, 165, 0, 255);
		TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);

		auto& attrib = _reader.GetAttrib();
		auto& shapes = _reader.GetShapes();

		Vec3f light_dir(0, 0, -1);
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
				// Loop over vertices in the face.
				Vec2i screen_coords[3];
				Vec3f world_coords[3];
				for (size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx0 = shapes[s].mesh.indices[index_offset + v];
					tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset + (v + 1) % fv];
					int x0 = (attrib.vertices[3 * size_t(idx0.vertex_index) + 0]) * 16 + WIDTH / 2.;
					int y0 = (attrib.vertices[3 * size_t(idx0.vertex_index) + 1]) * 16 + HEIGHT / 2.;

					screen_coords[v] = Vec2i(x0, y0);
					world_coords[v] = Vec3f(attrib.vertices[3 * size_t(idx0.vertex_index) + 0],
						attrib.vertices[3 * size_t(idx0.vertex_index) + 1],
						attrib.vertices[3 * size_t(idx0.vertex_index) + 2]);
				}

				Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
				n.normalize();
				float intensity = n * light_dir;
				if (intensity > 0) {
					triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, ORANGE* intensity);
				}

				index_offset += fv;
			}
		}
		image.write_tga_file("output.tga");
	}

private:
	tinyobj::ObjReader _reader;
};