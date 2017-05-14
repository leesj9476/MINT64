#include <stdio.h>
#include <fcntl.h>

#define SECTORBYTE 512

int CopyFile (int, int);
int AdjustSectorSize (int, int);

int main (int argc, char **argv) {
	int target_fd;
	int source_fd, source_size;
	int bootloader_sector_count;
	int kerner32_sector_count;


	if (argc < 3) {
		fprintf(stderr, "[Error] ./ImageMaker BootLoader.bin Kernel32.bin\n");
		exit(-1);
	}

	// Make Disk.img file
	target_fd = open("Disk.img", O_RDWR | O_TRUNC | O_CREAT | O_BINARY, S_IREAD | S_IWRITE);
	if (target_fd == -1) {
		fprintf(stderr, "[Error] Disk.img open fail\n");
		exit(-1);
	}

	// Open BootLoader.bin file
	printf("[Info] Copy boot loader to image file\n");
	source_fd = open(argv[1], O_RDONLY | O_BINARY)
	if (source_fd == -1) {
		fprintf(stderr, "[Error] %s open fail\n", argv[1]);
		exit(-1);
	}

	// Copy BootLoader.bin to Disk.img and fill with 0
	// to adjust sector size (512 byte)
	source_size = CopyFile(target_fd, source_fd);
	bootloader_sector_count = AdjustSectorSize(target_fd, source_size);
	close(source_fd);

	printf("[Info] %s size = [%d] and sector count = [%d]\n", 
			argv[1], source_size, bootloader_sector_count);

	// Open Kernel32.bin file
	printf("[Info] Copy protected mode kernel to image file\n");
	source_fd = open(argv[2], O_RDONLY | O_BINARY);
	if (source_fd == -1) {
		fprintf(stderr, "[Error] %s open fail\n", argv[2]);
		exit(-1);
	}

	// Do same action using Kernel32.bin instead of BootLoader.bin
	source_size = CopyFile(target_fd, source_fd);
	kernel32_sector_count = AdjustSectorSize(target_fd, source_size);
	close(source_fd);

	printf("[Info] %s size = [%d] and sector count = [%d]\n",
			argv[2], source_size, kernel32_sector_count);

	// TODO: Update 5th byte using kerner32_sector_count

}

// input: file descriptor of target and source file
// output: size of source file (byte)
int CopyFile(int target_fd, int source_fd) {
	int read_byte, write_byte;
	int source_file_size = 0;
	char buffer[SECTORBYTE];

	while (1) {
		read_byte = read(source_fd, (void *)buffer, SECTORBYTE);
		if (read_byte == -1) {
			fprintf(stderr, "[Error] source file read fail\n");
			exit(-1);
		}
		else if (read_byte == 0)
			break;

		write_byte = write(target_fd, (const void *)buffer, SECTORBYTE);
		if (read_byte != write_byte) {
			fprintf(stderr, "[Error] read byte != write byte\n");
			exit(-1);
		}

		source_file_size += read_byte;
	}

	return source_file_size;
}

// input: file descriptor of target file and size of that
// output: the number of sectors of adjusted file
int AdjustSectorSize (int target_fd, int source_size) {
	int adjust_size;
	int sector_count;
	int write_byte;
	char char_zero = 0x00;

	adjust_byte = SECTORBYTE - (source_size % SECTORBYTE);
	sector_count = (source_size + adjust_byte) >> 9; // divided by 512

	if (adjust_byte != 0) {
		printf("[Info] file size [%d] and fill [%d] byte\n", source_size, adjust_byte);
		while (adjust_byte != 0) {
			write_byte = write(target_fd, (void *)(&char_zero), 1);
			if (write_byte != 1) {
				fprintf(stderr, "[Error] fill target file with 0x00 fail\n");
				exit(-1);
			}

			adjust_byte--;
		}
	}
	else
		printf("[Info] file is aligned by 512 byte\n");

	return sector_count;
}
