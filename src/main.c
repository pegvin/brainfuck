#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brainfuck.h"

int main(void) {
	struct BfContext ctx;

	if (Bf_InitializeContext(&ctx) != 0) {
		printf("Error: Initialize_BfContext(...) returned Non-Zero value\n");
		return 1;
	}

	const char str[] = "+[>[<-[]>+[>+++>[+++++++++++>][>]-[<]>-]]++++++++++<]>>>>>>----.<<+++.<-..+++.<-.>>>.<<.+++.------.>-.<<+.<.";
	size_t len = strlen(str);

	int Result = Bf_Execute(&ctx, str, len);
	if (Result != 0) {
		printf("Error: Bf_Execute(...) %s\n", Bf_StrError(Result));
		return 1;
	}

	return 0;
}

