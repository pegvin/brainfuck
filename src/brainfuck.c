#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "brainfuck.h"

enum Bf_Errors {
	Bf_TAPE_OUT_OF_BOUND_ERROR = 1,
	Bf_UNMATCHED_OPEN_BRACKET,
	Bf_UNMATCHED_CLOSE_BRACKET,
	Bf_UNKNOWN_ERROR,
};

int Bf_InitializeContext(struct BfContext* ctx) {
	if (ctx == NULL) return 1;

	memset(ctx->Tape, 0, sizeof(ctx->Tape));
	ctx->Pointer = 0;

	return 0;
}

int Bf_Execute(struct BfContext* ctx, const char* const str, int64_t len) {
	if (ctx == NULL || str == NULL)
		return Bf_UNKNOWN_ERROR;

	if (len < 1) return 0;

	int64_t Position = 0;
	while (Position < len && str[Position] != '\0') {
		char c = str[Position];
		switch (c) {
			case '>': ctx->Pointer++; break;
			case '<': ctx->Pointer--; break;
			case '+': ctx->Tape[ctx->Pointer]++; break;
			case '-': ctx->Tape[ctx->Pointer]--; break;
			case '.': write(STDOUT_FILENO, &ctx->Tape[ctx->Pointer], 1); break;
			case ',': read(STDIN_FILENO, &ctx->Tape[ctx->Pointer], 1); break;
			case '[': {
				if (ctx->Tape[ctx->Pointer] == 0) {
					int64_t NxtClosingBracket = Position + 1;
					int64_t BracketDepth = 0;
					while (1) {
						if (NxtClosingBracket >= len) break;
						else if (str[NxtClosingBracket] == '\0') break;
						else if (str[NxtClosingBracket] == '[') BracketDepth++;
						else if (str[NxtClosingBracket] == ']') {
							if (BracketDepth == 0) break;
							else BracketDepth--;
						}

						NxtClosingBracket++;
					}
					if (
						NxtClosingBracket < len       &&
						str[NxtClosingBracket] == ']' &&
						BracketDepth == 0
					) {
						Position = NxtClosingBracket;
					} else {
						return Bf_UNMATCHED_CLOSE_BRACKET;
					}
				}
				break;
			}
			case ']': {
				if (ctx->Tape[ctx->Pointer] != 0) {
					int64_t PrvOpeningBracket = Position - 1;
					int64_t BracketDepth = 0;
					while (1) {
						if (PrvOpeningBracket <= 0) break;
						else if (str[PrvOpeningBracket] == ']') BracketDepth++;
						else if (str[PrvOpeningBracket] == '[') {
							if (BracketDepth == 0) break;
							else BracketDepth--;
						}

						PrvOpeningBracket--;
					}
					if (
						PrvOpeningBracket > -1        &&
						str[PrvOpeningBracket] == '[' &&
						BracketDepth == 0
					) {
						Position = PrvOpeningBracket;
					} else {
						return Bf_UNMATCHED_OPEN_BRACKET;
					}
				}
				break;
			}

			default:
				break;
		}
		if (ctx->Pointer < 0 || ctx->Pointer > sizeof(ctx->Tape)) {
			return Bf_TAPE_OUT_OF_BOUND_ERROR;
		}

		Position++;
	}

	return 0;
}

const char* const Bf_StrError(int err) {
	switch (err) {
		case Bf_TAPE_OUT_OF_BOUND_ERROR:
			return "pointer out of bound";

		case Bf_UNMATCHED_OPEN_BRACKET:
			return "matching '[' not found";

		case Bf_UNMATCHED_CLOSE_BRACKET:
			return "matching ']' not found";

		case Bf_UNKNOWN_ERROR:
		default:
			return "unknown error";
	}
}

