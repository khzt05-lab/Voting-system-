// ============================================================
//  colors.h  —  ANSI / VT100 color macros for the console
// ============================================================
#pragma once
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

// ── Enable ANSI escape sequences on Windows 10+ ──────────────
inline void enableANSI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
    SetConsoleOutputCP(65001);   // UTF-8 output
    SetConsoleCP(65001);          // UTF-8 input
#endif
}

// ── Reset & Text Effects ─────────────────────────────────────
#define C_RESET      "\033[0m"
#define C_BOLD       "\033[1m"
#define C_DIM        "\033[2m"
#define C_ITALIC     "\033[3m"
#define C_ULINE      "\033[4m"
#define C_BLINK      "\033[5m"
#define C_INVERT     "\033[7m"

// ── Foreground Colors ─────────────────────────────────────────
#define C_BLACK      "\033[30m"
#define C_RED        "\033[31m"
#define C_GREEN      "\033[32m"
#define C_YELLOW     "\033[33m"
#define C_BLUE       "\033[34m"
#define C_MAGENTA    "\033[35m"
#define C_CYAN       "\033[36m"
#define C_WHITE      "\033[37m"

// Bright / intense foreground
#define C_BR_BLACK   "\033[90m"
#define C_BR_RED     "\033[91m"
#define C_BR_GREEN   "\033[92m"
#define C_BR_YELLOW  "\033[93m"
#define C_BR_BLUE    "\033[94m"
#define C_BR_MAGENTA "\033[95m"
#define C_BR_CYAN    "\033[96m"
#define C_BR_WHITE   "\033[97m"

// ── Background Colors ─────────────────────────────────────────
#define C_BG_BLACK   "\033[40m"
#define C_BG_RED     "\033[41m"
#define C_BG_GREEN   "\033[42m"
#define C_BG_YELLOW  "\033[43m"
#define C_BG_BLUE    "\033[44m"
#define C_BG_MAGENTA "\033[45m"
#define C_BG_CYAN    "\033[46m"
#define C_BG_WHITE   "\033[47m"

// ── Inline Helpers ────────────────────────────────────────────
inline std::string colorize(const std::string& text, const char* color) {
    return std::string(color) + text + C_RESET;
}

inline std::string boldColor(const std::string& text, const char* color) {
    return std::string(C_BOLD) + color + text + C_RESET;
}
