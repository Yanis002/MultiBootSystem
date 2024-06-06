#ifndef _STDARG_H_
#define _STDARG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];
typedef __va_list va_list;

extern void __builtin_va_info(void*);

void* __va_arg(va_list v_list, int type);

#ifdef __MWERKS__
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_arg(ap, t) (*((t*)__va_arg(ap, _var_arg_typeof(t))))
#else
#define va_start(ap, fmt) 0
#define va_arg(ap, t) 0
#endif

#define va_end(ap) (void)0

#ifdef __cplusplus
};
#endif

#endif
