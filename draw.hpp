#pragma once

#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include "tgaimage.h"
#include "tiny_obj_loader.h"

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

	void render() {
		static const int WIDTH = 1024;
		static const int HEIGHT = 1024;
		static const TGAColor ORANGE = TGAColor(255, 165, 0, 255);
		TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);

		auto& attrib = _reader.GetAttrib();
		auto& shapes = _reader.GetShapes();

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					tinyobj::index_t idx0 = shapes[s].mesh.indices[index_offset + v];
					tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset + (v + 1) % fv];
					int x0 = (attrib.vertices[3 * size_t(idx0.vertex_index) + 0]) * 16 + WIDTH / 2.;
					int y0 = (attrib.vertices[3 * size_t(idx0.vertex_index) + 1]) * 16 + HEIGHT / 2.;
					int x1 = (attrib.vertices[3 * size_t(idx1.vertex_index) + 0]) * 16 + WIDTH / 2.;
					int y1 = (attrib.vertices[3 * size_t(idx1.vertex_index) + 1]) * 16 + HEIGHT / 2.;
					line(x0, y0, x1, y1, image, ORANGE);
				}
				index_offset += fv;
			}
		}
		//image.flip_vertically();
		image.write_tga_file("output.tga");
	}

private:
	tinyobj::ObjReader _reader;
};