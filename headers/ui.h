// ============================================================
//  ui.h  —  ASCII box drawing, menus, input helpers (inline)
// ============================================================
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cctype>
#ifdef _WIN32
#include <conio.h>
#endif
#include "colors.h"

// ── Box Drawing Characters (UTF-8 encoded) ───────────────────
inline const std::string BOX_TL  = "\xE2\x95\x94"; // ╔
inline const std::string BOX_TR  = "\xE2\x95\x97"; // ╗
inline const std::string BOX_BL  = "\xE2\x95\x9A"; // ╚
inline const std::string BOX_BR  = "\xE2\x95\x9D"; // ╝
inline const std::string BOX_H   = "\xE2\x95\x90"; // ═
inline const std::string BOX_V   = "\xE2\x95\x91"; // ║
inline const std::string BOX_ML  = "\xE2\x95\xA0"; // ╠
inline const std::string BOX_MR  = "\xE2\x95\xA3"; // ╣
inline const std::string TH_H    = "\xE2\x94\x80"; // ─
inline const std::string TH_V    = "\xE2\x94\x82"; // │
inline const std::string TH_L    = "\xE2\x94\x9C"; // ├
inline const std::string TH_R    = "\xE2\x94\xA4"; // ┤
inline const std::string TH_TL   = "\xE2\x94\x8C"; // ┌
inline const std::string TH_TR   = "\xE2\x94\x90"; // ┐
inline const std::string TH_BL   = "\xE2\x94\x94"; // └
inline const std::string TH_BR   = "\xE2\x94\x98"; // ┘
inline const std::string TH_TM   = "\xE2\x94\xAC"; // ┬
inline const std::string TH_BM   = "\xE2\x94\xB4"; // ┴
inline const std::string TH_CR   = "\xE2\x94\xBC"; // ┼
inline const std::string BLOCK   = "\xE2\x96\x88"; // █
inline const std::string SHADE   = "\xE2\x96\x91"; // ░
inline const std::string STAR    = "\xE2\x98\x85"; // ★

static const int W = 68; // Default box width

// ── Utility ───────────────────────────────────────────────────

inline std::string repeatStr(const std::string& s, int n) {
    std::string r;
    r.reserve(s.size() * n);
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

// Visual (display) length: skips ANSI escape codes,
// counts multi-byte UTF-8 sequences as 1 character each.
inline int visLen(const std::string& s) {
    int len = 0;
    size_t i = 0;
    while (i < s.size()) {
        // ANSI escape sequence: ESC [ ... m
        if (s[i] == '\033' && i + 1 < s.size() && s[i+1] == '[') {
            i += 2;
            while (i < s.size() && s[i] != 'm') ++i;
            if (i < s.size()) ++i; // skip 'm'
        } else {
            unsigned char c = (unsigned char)s[i];
            if      (c < 0x80)           { ++len; ++i; }
            else if ((c & 0xE0) == 0xC0) { ++len; i += 2; }
            else if ((c & 0xF0) == 0xE0) { ++len; i += 3; }
            else if ((c & 0xF8) == 0xF0) { ++len; i += 4; }
            else                          { ++i; }
        }
    }
    return len;
}

// ── Screen Helpers ────────────────────────────────────────────

inline void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

inline void pause() {
    std::cout << "\n" C_DIM "  Press any key to continue..." C_RESET;
#ifdef _WIN32
    _getch();
#else
    std::string ignored;
    std::getline(std::cin, ignored);
#endif
    std::cout << "\n";
}

// ── Status Messages ───────────────────────────────────────────

inline void printSuccess(const std::string& msg) {
    std::cout << C_BOLD C_BR_GREEN "  [ OK ]  " C_RESET C_GREEN << msg << C_RESET "\n";
}
inline void printError(const std::string& msg) {
    std::cout << C_BOLD C_BR_RED   " [ERROR]  " C_RESET C_RED   << msg << C_RESET "\n";
}
inline void printWarning(const std::string& msg) {
    std::cout << C_BOLD C_BR_YELLOW " [WARN]  " C_RESET C_YELLOW << msg << C_RESET "\n";
}
inline void printInfo(const std::string& msg) {
    std::cout << C_BOLD C_BR_CYAN  " [INFO]  " C_RESET C_CYAN   << msg << C_RESET "\n";
}

// ── Core Box Drawing ──────────────────────────────────────────

inline void drawTopBorder(int w = W) {
    std::cout << C_CYAN << BOX_TL << repeatStr(BOX_H, w - 2) << BOX_TR << C_RESET "\n";
}
inline void drawBottomBorder(int w = W) {
    std::cout << C_CYAN << BOX_BL << repeatStr(BOX_H, w - 2) << BOX_BR << C_RESET "\n";
}
inline void drawMidBorder(int w = W) {
    std::cout << C_CYAN << BOX_ML << repeatStr(BOX_H, w - 2) << BOX_MR << C_RESET "\n";
}

// Draw a single box row.
// content    : text to place inside (may include ANSI codes)
// contentVis : the VISUAL width of content (without escape codes)
// align      : "left" | "center" | "right"
inline void drawBoxRow(const std::string& content, int contentVis,
                       int w = W, const std::string& align = "left") {
    int inner = w - 4;          // usable inner width (║ sp ... sp ║)
    int pad   = inner - contentVis;
    if (pad < 0) pad = 0;
    int lp = (align == "center") ? pad / 2 : (align == "right") ? pad : 0;
    int rp = pad - lp;
    std::cout << C_CYAN << BOX_V << C_RESET
              << " " << std::string(lp, ' ') << content << std::string(rp, ' ') << " "
              << C_CYAN << BOX_V << C_RESET "\n";
}

inline void drawEmptyRow(int w = W) { drawBoxRow("", 0, w); }

// ── High-Level UI Components ──────────────────────────────────

inline void printBanner() {
    clearScreen();
    std::cout << "\n";
    std::cout << C_BOLD C_BR_CYAN
              "   \xE2\x95\xA6  \xE2\x95\xA6  \xE2\x95\x94\xE2\x95\x90\xE2\x95\x97  \xE2\x95\x94\xE2\x95\xA6\xE2\x95\x97  \xE2\x95\xA6  \xE2\x95\x94\xE2\x95\x97\xE2\x95\x94  \xE2\x95\x94\xE2\x95\x90\xE2\x95\x97\n"
              "   \xE2\x95\x9A\xE2\x95\x97\xE2\x95\x94\xE2\x95\x9D  \xE2\x95\x91 \xE2\x95\x91  \xE2\x95\x91 \xE2\x95\x91 \xE2\x95\x91  \xE2\x95\x91\xE2\x95\x91\xE2\x95\x91  \xE2\x95\x91 \xE2\x95\xA6\n"
              "    \xE2\x95\x9A\xE2\x95\x9D   \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x9D  \xE2\x95\xA9 \xE2\x95\xA9 \xE2\x95\xA9  \xE2\x95\x9D\xE2\x95\x9A\xE2\x95\x9D  \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x9D\n" C_RESET;
    std::cout << C_BOLD C_BR_YELLOW
              "    \xE2\x95\x94\xE2\x95\x90\xE2\x95\x97  \xE2\x95\xA6 \xE2\x95\xA6  \xE2\x95\x94\xE2\x95\x90\xE2\x95\x97  \xE2\x95\x94\xE2\x95\xA6\xE2\x95\x97  \xE2\x95\x94\xE2\x95\x90\xE2\x95\x97  \xE2\x95\x94\xE2\x95\xA6\xE2\x95\x97\n"
              "    \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x97  \xE2\x95\x9A\xE2\x95\xA6\xE2\x95\x9D  \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x97  \xE2\x95\x91 \xE2\x95\x91  \xE2\x95\x91\xE2\x95\xA3   \xE2\x95\x91 \xE2\x95\x91\xE2\x95\x91\n"
              "    \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x9D   \xE2\x95\xA9   \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x9D   \xE2\x95\xA9   \xE2\x95\x9A\xE2\x95\x90\xE2\x95\x9D  \xE2\x95\xA9 \xE2\x95\xA9\n" C_RESET;
    std::cout << C_DIM C_WHITE
              "       \xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\n"
              "           Secure  \xC2\xB7  Transparent  \xC2\xB7  Fair\n"
              "       \xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\n" C_RESET "\n";
}

// Title box: top border + centered bold yellow title + mid border
inline void drawTitleBox(const std::string& title, int w = W) {
    std::string styled = C_BOLD C_BR_YELLOW + title + C_RESET;
    drawTopBorder(w);
    drawBoxRow(styled, (int)title.size(), w, "center");
    drawMidBorder(w);
}

// Numbered menu list (renders between current border and next border)
inline void drawMenu(const std::vector<std::string>& opts, int w = W) {
    drawEmptyRow(w);
    for (size_t i = 0; i < opts.size(); ++i) {
        bool isExit = (opts[i].find("Exit") != std::string::npos ||
                       opts[i].find("Back") != std::string::npos ||
                       opts[i].find("Logout") != std::string::npos);
        std::string numStr  = "  [" + std::to_string(i + 1) + "]  ";
        std::string lineCol = isExit
            ? std::string(C_DIM C_RED) + numStr + C_RESET C_RED + opts[i] + C_RESET
            : std::string(C_BR_CYAN)   + numStr + C_RESET C_BR_WHITE + opts[i] + C_RESET;
        int vl = (int)numStr.size() + (int)opts[i].size();
        drawBoxRow(lineCol, vl, w);
    }
    drawEmptyRow(w);
}

// ── Input Helpers ─────────────────────────────────────────────

// Password input — echoes '*' for each character
inline std::string getMaskedInput(const std::string& prompt) {
    std::cout << C_BR_CYAN "  -> " C_RESET << prompt;
    std::string pwd;
#ifdef _WIN32
    int ch;
    while (true) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') break;
        if (ch == '\b' || ch == 127) {
            if (!pwd.empty()) { pwd.pop_back(); std::cout << "\b \b"; }
        } else if (ch >= 32 && ch <= 126) {
            pwd += (char)ch;
            std::cout << '*';
        }
    }
#else
    // MinGW uses _getch(); the fallback keeps development builds portable.
    std::getline(std::cin, pwd);
#endif
    std::cout << "\n";
    return pwd;
}

// Validated integer input in range [minV, maxV]
inline int getIntInput(const std::string& prompt, int minV, int maxV) {
    while (true) {
        std::cout << C_BR_CYAN "  -> " C_RESET << prompt;
        std::string line;
        std::getline(std::cin, line);
        // Trim
        size_t s = line.find_first_not_of(" \t");
        if (s == std::string::npos) { printError("Input cannot be empty."); continue; }
        size_t e = line.find_last_not_of(" \t");
        line = line.substr(s, e - s + 1);
        // Validate
        bool ok = true;
        for (size_t k = 0; k < line.size(); ++k)
            if (k == 0 && line[k] == '-') continue;
            else if (!std::isdigit((unsigned char)line[k])) { ok = false; break; }
        if (!ok) { printError("Please enter a valid number."); continue; }
        int val = 0;
        try { val = std::stoi(line); }
        catch (...) { printError("Number out of range."); continue; }
        if (val < minV || val > maxV) {
            printError("Enter a number between " + std::to_string(minV)
                       + " and " + std::to_string(maxV) + ".");
            continue;
        }
        return val;
    }
}

// Non-empty trimmed string input
inline std::string getStringInput(const std::string& prompt) {
    while (true) {
        std::cout << C_BR_CYAN "  -> " C_RESET << prompt;
        std::string val;
        std::getline(std::cin, val);
        size_t s = val.find_first_not_of(" \t");
        if (s == std::string::npos) { printError("Input cannot be empty."); continue; }
        size_t e = val.find_last_not_of(" \t");
        return val.substr(s, e - s + 1);
    }
}

// ── Results Bar Chart Row ─────────────────────────────────────

inline void printBarRow(const std::string& label, int votes, int maxVotes,
                        double pct, int barWidth = 28) {
    int filled = (maxVotes > 0) ? (votes * barWidth / maxVotes) : 0;
    std::string bar = std::string(C_BR_GREEN)  + repeatStr(BLOCK, filled)
                    + std::string(C_DIM C_WHITE) + repeatStr(SHADE, barWidth - filled)
                    + std::string(C_RESET);
    // Label padded to 22 chars
    std::string lbl = label.size() > 22 ? label.substr(0, 22) : label;
    while ((int)lbl.size() < 22) lbl += ' ';

    std::cout << "  " C_BR_WHITE << lbl << C_RESET " "
              << bar << " "
              << C_BR_YELLOW << std::setw(3) << votes << " votes  "
              << C_MAGENTA   << std::fixed << std::setprecision(1) << pct << "%"
              << C_RESET "\n";
}
