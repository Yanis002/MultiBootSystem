#ifndef _WCHAR_H_
#define _WCHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"

int fwide(FILE* stream, int mode);

#ifdef __cplusplus
};
#endif

#endif
