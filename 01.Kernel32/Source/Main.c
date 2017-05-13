#include "Types.h"

void PrintString (int x, int y, const char *str);

void Main (void) {
	PrintString (0, 3, "C language Kernel Started~!!");

	while (1);
}

void PrintString (int x, int y, const char *str) {
	CharStruct *screen = (CharStruct *) 0xB8000;
	int i = 0;

	screen += (y * 80) + x;
	while (str[i] != 0) {
		screen[i].character = str[i];
		i++;
	}
}
