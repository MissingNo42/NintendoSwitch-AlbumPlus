#pragma once

// Include the most common headers from the C standard library
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

#define abs(x) (x < 0) ? -x: x;

#define putsb(s) fputs(s, stdout);
#define print(...) printf(__VA_ARGS__)
#define cprint(c, ...) { putsb(c); printf(__VA_ARGS__); }

#define debug(c) {Result res = c; print("S %X | D %02X| M %02X | V %04X : %s\n", R_SUCCEEDED(res), R_DESCRIPTION(res), R_MODULE(res), R_VALUE(res), #c);}
#define debugv(c) {print("S %X | D %02X| M %02X | V %04X", R_SUCCEEDED(c), R_DESCRIPTION(c), R_MODULE(c), R_VALUE(c));}

#define CONSOLE_FG(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define CONSOLE_BG(r, g, b) "\x1b[48;2;" #r ";" #g ";" #b "m"

#define console_fg(r, g, b) putsb(CONSOLE_FG(r, g, b))
#define console_bg(r, g, b) putsb(CONSOLE_BG(r, g, b))

#define reset_fg() console_fg(255, 255, 255)
#define reset_bg() console_bg(51, 51, 51)


typedef enum {
    ColorIterOption_Cycle   = 0, // restart from start when reaches end
    ColorIterOption_Mirror  = 1, // restart from end (towards start) when reaches end (and reverse)

    ColorIterOption_Reverse = 2, // progress from end to start instead of start to end
} ColorIterOption;

typedef struct {
    u16 sampling, seek, start, end;
    u8 r, g, b;
    ColorIterOption op;
} color;


/**
 * @brief Initialize a color iterator.
 * @note start and end are used to iterate on a limited part of the rainbow colors, default values means all rainbow (= full cycle)
 * @note end must be upper than start (or 0), means that start must be lower than sampling - 1
 * @note sampling = 0 is a shortcut to default sampling = 0x600
 * @note end      = 0 is a shortcut to default end      = sampling - 1
 * @note 0 <= start <= seek <= end < sampling <= 0x600    and    start < end
 * @param[in] sampling number of color computed over 1 full cycle (sampling <= 0x600, default 0x600)
 * @param[in] start    start color (which hue is start / sampling), 0 default (pur red)
 * @param[in] end      end   color (which hue is end   / sampling), (sampling - 1 or 0) default (~pur red)
 * @param[in] seek     current (init) color (which hue is seek / sampling), 0 (start) default
 * @param[in] option   defines iteration option (flag), see enum.
 */
color init_color(u16 sampling, u16 start, u16 end, u16 seek, ColorIterOption option);
color * next_color(color * c);

void progressbar(u8 size, u8 padding, u64 value, u64 max);