#ifndef LOGMA_H
#define LOGMA_H

#define LOGMA_LIMITS 2048

/*
 * CSI = '\033['
 * format: CSI + int(code) + 'm'
 *
 * Fore.YELLOW        = "\033[33m"
 * Fore.RED           = "\033[31m"
 * Fore.LIGHTBLACK_EX = "\033[90m"
 * Style.DIM          = "\033[2m"
 * Style.RESET_ALL    = "\033[0m"
 */

#define FORE_YELLOW        "\033[33m"
#define FORE_RED           "\033[31m"
#define FORE_LIGHTBLACK_EX "\033[90m"
#define FORE_RESET         "\033[39m"
#define STYLE_DIM          "\033[2m"
#define STYLE_NORMAL       "\033[22m"
#define STYLE_RESET_ALL    "\033[0m"

static bool log_verbose = false;
static bool log_disable = false;

void log_setVerbosity(bool b);
void log_disableCompletely(bool b);

void log_info(const char* _Format, ...);
void log_warn(const char* _Format, ...);
void log_fatal(const char* _Format, ...);
void log_debug(const char* _Format, ...);
void log_debug_raw(const char* _Format, ...);

#endif //LOGMA_H
