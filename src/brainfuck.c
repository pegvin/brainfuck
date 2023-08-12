#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#include "brainfuck.h"

enum Bf_Errors {
	Bf_TAPE_OUT_OF_BOUND_ERROR = 1,
	Bf_UNKNOWN_ERROR,
};

int Bf_InitializeContext(struct BfContext* ctx) {
	if (ctx == NULL) return 1;

	bzero(ctx->Tape, sizeof(ctx->Tape));
	ctx->Pointer = &ctx->Tape[0];

	return 0;
}

int Bf_Execute(struct BfContext* ctx, const char* const str, size_t len) {

	return 0;
}

const char* const Bf_StrError(int err) {
	switch (err) {
		case Bf_TAPE_OUT_OF_BOUND_ERROR:
			return "pointer out of bound";

		case Bf_UNKNOWN_ERROR:
		default:
			return "unknown error";
	}
}

