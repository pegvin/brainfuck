#ifndef BRAINFUCK_BRAINFUCK_H_INCLUDED_
#define BRAINFUCK_BRAINFUCK_H_INCLUDED_ 1
#pragma once

#include <stdint.h>
#include <stdlib.h>

#define TAPE_SIZE 30000

struct BfContext {
	uint8_t Tape[TAPE_SIZE];
	int64_t Pointer;
};

// Initializes a Brainfuck Context
int Bf_InitializeContext(struct BfContext* ctx);

// Executes Brainfuck code "str" of length "len" in "ctx" Context
int Bf_Execute(struct BfContext* ctx, const char* const str, int64_t len);

// Compiles Brainfuck code `inp` of length `inpLen` and stores it in `out` and stores length of `out` in `outLen`.
// Note: The function will allocate the memory for you.
int Bf_Compile(const char* inp, int64_t inpLen, char** out, int64_t* outLen);

// returns a Human readable string for corresponding error code returned by Bf_Execute
const char* const Bf_StrError(int err);

#endif // BRAINFUCK_BRAINFUCK_H_INCLUDED_

