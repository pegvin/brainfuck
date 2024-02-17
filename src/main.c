#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "brainfuck.h"

void PrintUsage(const char* filePath) {
	printf("Usage: %s execute/compile <filepath>\n", filePath);
}

int main(int argc, char** argv) {
	if (argc < 3) {
		PrintUsage(argv[0]);
		return 1;
	}

	enum Command { Cmd_Exec, Cmd_Comp };

	enum Command cmd;

	if (strncmp("compile", argv[1], 7) == 0) {
		cmd = Cmd_Comp;
	} else if (strncmp("execute", argv[1], 7) == 0) {
		cmd = Cmd_Exec;
	} else {
		PrintUsage(argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[2], "r");
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

	switch (cmd) {
		case Cmd_Exec: {
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
			break;
		}
		case Cmd_Comp: {
			char* out = NULL;
			int64_t outLen = 0;
			int Result = Bf_Compile(str, len, &out, &outLen);
			if (Result != 0) {
				printf("Error: Bf_Compile(...) %s\n", Bf_StrError(Result));
				return 1;
			}
			break;
		}
	}

	free(str);
	return 0;
}

