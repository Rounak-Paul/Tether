#pragma once
#include "../include/ast.h"

// A minimal interpreter API for the prototype language.
// It supports executing a parsed AST and calling the `main` function.

int interp_execute(AST *ast); // returns exit code
