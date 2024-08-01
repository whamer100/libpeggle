#include "logma.h"

#include <Windows.h>
#include <cstdio>

void log_setVerbosity(const bool b) {
    log_verbose = b;
    // so funny thing, we dont actually have to change this ever
    log_debug("Debug logging enabled.\n");
    // when its disabled, nothing gets printed :)
}

void log_disableCompletely(const bool b) {
    log_disable = b;
    // it doesnt print anything since, yknow, it's disabled
}

#ifdef __DEBUG__
#define DEBUG_FIX() setbuf(stdout, 0)
#else
#define DEBUG_FIX()
#endif

// i didnt want to have to write duplicate code
#define impl_log(_fmt, _Format, ...) \
DEBUG_FIX();\
va_list args;\
char buf[LOGMA_LIMITS] = {0};\
\
va_start(args, _Format);\
vsprintf_s(buf, LOGMA_LIMITS, _Format, args);\
va_end(args);\
\
printf(_fmt, buf);\


void log_info(const char* _Format, ...) {
    if (log_disable) return;

    impl_log("[INFO] %s" STYLE_RESET_ALL, _Format);
}
void log_warn(const char* _Format, ...) {
    if (log_disable) return;

    impl_log(FORE_YELLOW "[WARN] %s" STYLE_RESET_ALL, _Format);
}
void log_fatal(const char* _Format, ...) {
    if (log_disable) return;

    impl_log(FORE_RED "[FATAL] %s" STYLE_RESET_ALL, _Format);
}
void log_debug(const char* _Format, ...) {
    if (log_disable || !log_verbose) return;

    impl_log(FORE_LIGHTBLACK_EX STYLE_DIM "[DEBUG] %s" STYLE_RESET_ALL, _Format);
}
void log_debug_raw(const char* _Format, ...) {
    if (log_disable || !log_verbose) return;

    impl_log(FORE_LIGHTBLACK_EX STYLE_DIM "%s" STYLE_RESET_ALL, _Format);
}