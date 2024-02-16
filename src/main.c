#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "brainfuck.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <filepath>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		printf("Error: fopen(...) %s\n", strerror(errno));
		return 1;
	}
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f) + 1;
	fseek(f, 0, SEEK_SET);

	char* str = malloc(len);
	memset(str, 0, len);
	if (str == NULL) {
		printf("Error: malloc(...) %s\n", strerror(errno));
		return 1;
	}
	if (fread(str, 1, len - 1, f) != len - 1) {
		printf("Error: fread(...) %s\n", strerror(errno));
		return 1;
	}
	fclose(f);

	struct BfContext ctx;

	if (Bf_InitializeContext(&ctx) != 0) {
		printf("Error: Initialize_BfContext(...) returned Non-Zero value\n");
		return 1;
	}

	int Result = Bf_Execute(&ctx, str, len);
	if (Result != 0) {
		printf("Error: Bf_Execute(...) %s\n", Bf_StrError(Result));
		return 1;
	}

	free(str);
	return 0;
}

