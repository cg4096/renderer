#include <math.h>
#include <direct.h>
#include "tgaimage.h"

const TGAColor green = TGAColor(0, 255, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	bool steep = false;
	//x��y�ụ����ʹ����б����0-1֮��
 	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
 		std::swap(x0, y0);
 		std::swap(x1, y1);
 		steep = true;
 	}

	//�յ�xֵС����㣬������Ϊbase case
 	if (x0 > x1) {
 		std::swap(x0, x1);
 		std::swap(y0, y1);
 	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	//ֱ��б��
	float k = std::abs(dy / float(dx));

	float d = 0;
	int y = y0;

	for (int x = x0; x <= x1; x++) {
		//�������ص�
		if (steep)
			image.set(y, x, color);
		else
			image.set(x, y, color);
		//����ֱ��б��
		d += k;
		//ѡ���ϱߵ�����
		if (d > 0.5f) {
			y += (y1 > y0 ? 1 : -1);
			d -= 1.0f;
		}
	}
}

int main(int argc, char** argv) {
	std::string dir = CWD;
	(void)_chdir((dir + "/bin").c_str());

    TGAImage image(100, 100, TGAImage::RGB);
	line(50, 50, 80, 90, image, green);
	line(50, 50, -80, 40, image, green);
	line(50, 50, -80, -40, image, green);
	line(50, 50, 80, -40, image, green);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}