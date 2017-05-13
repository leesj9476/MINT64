#include <stdio.h>
#include <fcntl.h>

#define SECTORBYTE 512

int main (int argc, char **argv) {
	int target_fd;
	int source_fd;


	if (argc < 3) {
		fprintf(stderr, "[Error] ./ImageMaker BootLoader.bin Kernel32.bin\n");
		exit(-1);
	}

	target_fd = open("Disk.img", O_RDWR | O_TRUNC | O_CREAT | O_BINARY, S_IREAD | S_IWRITE);
	if (target_fd == -1) {
		fprintf(stderr, "[Error] Disk.img open fail\n");
		exit(-1);
	}

	source_fd = open(argv[1], O_RDONLY | O_BINARY)
	if (source_fd == -1) {
		fprintf(stderr, "[Error] %s open fail\n", argv[1]);
		exit(-1);
	}



}
