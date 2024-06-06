#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long size_t;
typedef long ptrdiff_t;

#define OFFSETOF(type, member) ((size_t) & (((type*)0)->member))

#ifdef __cplusplus
};
#endif

#endif
