#pragma once
#include <log.h>
int snprintf(const char *fmt, int n, ...);
#define printf(fmt, ...) infof(fmt, __VA_ARGS__)
