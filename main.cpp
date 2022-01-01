#include <math.h>

#ifdef _WIN32
#include <direct.h>
#define ch_dir _chdir
#elif __APPLE__
#include <unistd.h>
#define ch_dir chdir
#elif __linux__
#include <unistd.h>
#define ch_dir chdir
#endif

#include "tgaimage.h"

const TGAColor green = TGAColor(0, 255, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

int main(int argc, char** argv) {
	std::string dir = CWD;
	(void)ch_dir((dir + "/bin").c_str());

    TGAImage image(100, 100, TGAImage::RGB);
	line(50, 50, 80, 90, image, green);
	line(50, 50, -80, 40, image, green);
	line(50, 50, -80, -40, image, green);
	line(50, 50, 80, -40, image, green);
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}