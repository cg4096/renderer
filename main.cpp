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

#include "draw.hpp"

int main(int argc, char** argv) {
	std::string dir = CWD;
	(void)ch_dir((dir + "/bin").c_str());

	Draw draw;
	draw.load("utah-teapot.obj");
	draw.render();
	
    return 0;
}