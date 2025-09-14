#pragma once

int lexer_init(const char *path);
int parser_parse(const char *src);
int typecheck_file(const char *path);
void runtime_init();
int net_register(const char *controller_addr, void *caps);
