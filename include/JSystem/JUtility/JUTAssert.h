#ifndef JUTASSERT_H
#define JUTASSERT_H

#include "dolphin/os.h"
#include "dolphin/types.h"
#include "stdarg.h"

#define JUT_ASSERT(LINE, COND)                                                                     \
    (void)((COND) || (JUTAssertion::showAssert(JUTAssertion::getSDevice(), __FILE__, LINE, #COND), \
                      OSPanic(__FILE__, LINE, "Halt"), 0));

// Favored by JAI (JAudio)
#define JUT_ASSERT_MSG(LINE, COND, MSG)                         \
    if (!(COND)) {                                              \
        OSReport(MSG);                                          \
        if (!(COND)) {                                          \
            JUTAssertion::showAssert(3, __FILE__, LINE, #COND); \
            OSPanic(__FILE__, LINE, "Halt");                    \
        }                                                       \
    }

#define JUT_PANIC(LINE)                                                        \
    JUTAssertion::showAssert(JUTAssertion::getSDevice(), __FILE__, LINE, "0"); \
    OSPanic(__FILE__, LINE, "Halt");

#define JUT_WARN(LINE, ...) JUTAssertion::setWarningMessage_f(JUTAssertion::getSDevice(), __FILE__, LINE, __VA_ARGS__);

#define JUT_LOG(LINE, ...) JUTAssertion::setLogMessage_f(JUTAssertion::getSDevice(), __FILE__, LINE, __VA_ARGS__)

#define JUT_CONFIRM(LINE, COND) JUTAssertion::setConfirmMessage(JUTAssertion::getSDevice(), __FILE__, LINE, COND, #COND)

namespace JUTAssertion {
    u32 getSDevice();
    void showAssert(u32 device, const char* file, int line, const char* assertion);
    void setWarningMessage_f(u32 device, char* file, int line, const char* fmt, ...);
    void setLogMessage_f(u32 device, char* file, int line, const char* fmt, ...);
    void setConfirmMessage(u32 device, char* file, int line, bool cond, const char* msg);
    void create();
    u32 flush_subroutine();
    void flushMessage();
    void flushMessage_dbPrint();
    void setVisible(bool);
    void setMessageCount(int);
}; // namespace JUTAssertion

extern "C" {
void setWarningMessage_f_va(u32 device, const char* file, int line, const char* msg, va_list args);
void setLogMessage_f_va(u32 device, const char* file, int line, const char* msg, va_list args);
}

#endif /* JUTASSERT_H */
