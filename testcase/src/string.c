#include "trap.h"
#include <string.h>

char *s[] = {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	", World!\n",
	"Hello, World!\n",
	"#####"
};

char str1[] = "Hello";
//char str[20] = "!!!#################"; // memset到第3个就不行了.
char str[400];

int main() {
	nemu_assert(strcmp(s[0], "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == 0); // OK
	//nemu_assert(strcmp(s[0], s[1]) == -1); // OK
	// nemu_assert(strcmp(s[0] + 1, s[1] + 1) == -1); // OK
	// nemu_assert(strcmp(s[0] + 2, s[1] + 2) == -1); // OK
	// nemu_assert(strcmp(s[0] + 3, s[1] + 3) == -1); // OK

	// nemu_assert(strcmp( strcat(strcpy(str, str1), s[3]), s[4]) == 0); // OK

	// memcmp没问题. 是memset的问题.
	// stos的操作数大小设置错了.
	// nemu_assert(memcmp(/*"#####"*/memset(str, '#', 5), s[5], 5) == 0);

	// nemu_assert(memcmp("12345678910", "12345678910", 11) == 0);

	// memset(str, 0, 50);
	// nemu_assert(strcmp(memset(str, '#', 5), s[5]) == 0);

	return 0;
}
