#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

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

int Bf_Compile(const char* inp, int64_t inpLen, char** out, int64_t* outLen) {
	if (inp == NULL)
		return Bf_UNKNOWN_ERROR;

	if (inpLen < 1) return 0;

	int64_t outStrSize = 0;
	int64_t outStrLen = 0;
	char* outStr = NULL;

	#define D_WRITE(...) printf(__VA_ARGS__)

	D_WRITE(
		"#include <unistd.h>\n\n"
		"int main(void) {\n"
		"\tunsigned char data[30000];\n"
		"\tunsigned int dataPtr = 0;\n"
		"\tfor (int i = 0; i < 30000; i++) {\n\t\tdata[i] = 0;\n\t}\n"
	);

	uint64_t Line = 1;
	uint64_t Col = 0;
	int didArithOccur = 0;

	for (int64_t i = 0; i < inpLen; i++) {
		char c = inp[i];
		if (c == '\n') {
			Line++;
			Col = 0;
			continue;
		} else {
			Col++;
		}

		switch (c) {
			case '>': {
				D_WRITE("\tdataPtr += 1;\n");
				break;
			}
			case '<': {
				D_WRITE("\tdataPtr -= 1;\n");
				break;
			}
			case '+': {
				D_WRITE("\tdata[dataPtr] += 1;\n");
				didArithOccur = 1;
				break;
			}
			case '-': {
				D_WRITE("\tdata[dataPtr] -= 1;\n");
				didArithOccur = 1;
				break;
			}
			case '.': {
				D_WRITE("\twrite(STDOUT_FILENO, &data[dataPtr], 1);\n");
				break;
			}
			case ',': {
				D_WRITE("\tread(STDIN_FILENO, &data[dataPtr], 1);\n");
				break;
			}
			case '[': {
				int64_t NxtClosingBracket = i + 1;
				int64_t BracketDepth = 0;
				int64_t LineOff = 0;
				while (1) {
					if (NxtClosingBracket >= inpLen) {
						break;
					} else if (inp[NxtClosingBracket] == '[') {
						BracketDepth++;
					} else if (inp[NxtClosingBracket] == ']') {
						if (BracketDepth == 0) break;
						else BracketDepth--;
					} else if (inp[NxtClosingBracket] == '\n') {
						LineOff++;
					}

					NxtClosingBracket++;
				}
				if (
					NxtClosingBracket < inpLen    &&
					inp[NxtClosingBracket] == ']' &&
					BracketDepth == 0
				) {
					if (didArithOccur == 0) {
						i = NxtClosingBracket + 1;
					} else {
						D_WRITE(
							"\tif (data[dataPtr] == 0) {\n"
							"\t\t goto CmdAfter_CB_%ld;\n"
							"\t}\n"
							"\nCmdAfter_OB_%ld:\n",
							NxtClosingBracket, i
						);
					}
				} else {
					return Bf_UNMATCHED_CLOSE_BRACKET;
				}
				break;
			}
			case ']': {
				int64_t PrvOpeningBracket = i - 1;
				int64_t BracketDepth = 0;
				int64_t LineOff = 0;
				while (1) {
					if (PrvOpeningBracket <= 0) break;
					else if (inp[PrvOpeningBracket] == ']') BracketDepth++;
					else if (inp[PrvOpeningBracket] == '[') {
						if (BracketDepth == 0) break;
						else BracketDepth--;
					} else if (inp[PrvOpeningBracket] == '\n') {
						LineOff++;
					}

					PrvOpeningBracket--;
				}
				if (
					PrvOpeningBracket > -1        &&
					inp[PrvOpeningBracket] == '[' &&
					BracketDepth == 0
				) {
					int64_t ColOff = 0;
					for (
						ColOff = 0;
						PrvOpeningBracket + ColOff >= 0 && inp[PrvOpeningBracket + ColOff] != '\n';
						ColOff--
					);
					ColOff = ColOff > 0 ? ColOff : (ColOff * -1);

					D_WRITE(
						"\tif (data[dataPtr] != 0) {\n"
						"\t\t goto CmdAfter_OB_%ld;\n"
						"\t}\n"
						"\nCmdAfter_CB_%ld:\n",
						PrvOpeningBracket, i
					);
				} else {
					return Bf_UNMATCHED_OPEN_BRACKET;
				}
				break;
			}
		}
	}

	D_WRITE("\treturn 0;\n}\n");

	#undef D_WRITE

	if (outStrLen < outStrSize) {
		outStr = realloc(outStr, outStrLen + 1);
	}

	*out = outStr;
	*outLen = outStrLen;
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

