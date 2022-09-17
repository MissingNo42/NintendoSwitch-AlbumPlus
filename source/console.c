#include "console.h"


static color * compute_color(color * c) {
    double h = c->seek * 6 / (double)c->sampling;
    double x = h - ((u8)h >> 1 << 1);
    x = ((x < 1) ? x: 2 - x) * 255;
    u8 r = (x - (u8)x >= .5) ? x + .5: x;
    if      (h < 1) c->r = 0xff, c->g = r,    c->b = 0;
    else if (h < 2) c->r = r,    c->g = 0xff, c->b = 0;
    else if (h < 3) c->r = 0,    c->g = 0xff, c->b = r;
    else if (h < 4) c->r = 0,    c->g = r,    c->b = 0xff;
    else if (h < 5) c->r = r,    c->g = 0,    c->b = 0xff;
    else            c->r = 0xff, c->g = 0,    c->b = r;
    return c;
}


color init_color(u16 sampling, u16 start, u16 end, u16 seek, ColorIterOption option) {
    if (!end) end = sampling - 1;
    color c = {.sampling = sampling ? sampling: 0x600, .seek = seek ? seek: ((option & ColorIterOption_Reverse) ? end: start), .start = start, .end = end ? end: sampling - 1, .op = option};
    return *compute_color(&c);
}


color * next_color(color * c) {
    if (c->seek == c->start) {
        if (c->op & ColorIterOption_Mirror) c->op &= ~ColorIterOption_Reverse;
        else if (c->op & ColorIterOption_Reverse) { c->seek = c->end; goto end; }
    } else if (c->seek == c->end) {
        if (c->op & ColorIterOption_Mirror) c->op |= ColorIterOption_Reverse;
        else if (!(c->op & ColorIterOption_Reverse)) { c->seek = c->start; goto end; }
    }
    c->seek += (c->op & ColorIterOption_Reverse) ? -1: 1;
    end:
    return compute_color(c);
}


void progressbar(u8 size, u8 padding, u64 value, u64 max) {
    // st line
    console_fg(220, 220, 220);
    for (u8 i = 0; i < padding; i++) putchar(' ');
    putchar('\xda');
    for (u8 i = 0; i < size; i++) putchar('\xc4');
    puts("\xbf");

    // sd line
    for (u8 i = 0; i < padding; i++) putchar(' ');
    putchar('\xb3');
    
    if (size) {
    
        u16 hs = (double)value / (double)max * (size << 1); // num of half block
        if (value != max && hs == size << 1) hs--;          // round down 100% if value != max
        if (value && !hs) hs++;                             // round up 0% if value != 0
        u8 sz = hs >> 1;                                    // num of full block
        bool half = hs & 1;                                 // true if an half block remains
    
        color ci = init_color(size, 0, 0, 0, 0);
        if (hs) {
            print("\x1b[38;2;%hhu;%hhu;%hhum%c", ci.r, ci.g, ci.b, (half && !sz) ? '\xdd': '\xdb');
            
            for (u8 i = 0; i < sz + (u8)half - 1; i++) {
                next_color(&ci);                
                print("\x1b[38;2;%hhu;%hhu;%hhum%c", ci.r, ci.g, ci.b, (half && i == sz - 1) ? '\xdd': '\xdb');
            }
            console_fg(220, 220, 220);
        }
        for (u8 i = sz + (u8)half; i < size; i++) putchar(' ');
    }
    puts("\xb3");

    // td line
    for (u8 i = 0; i < padding; i++) putchar(' ');
    putchar('\xc0');
    for (u8 i = 0; i < size; i++) putchar('\xc4');
    puts("\xd9");

}


/*
void progressbar(u8 size, u8 padding, u64 value, u64 max) {
    
        u8 sectiond[6],
           section  = (size - 1) / 6,
           sectionb = (size - 1) % 6;

        for (u8 i = 0; i < 6; i++) sectiond[i] = section + ((sectionb / (i + 1) > 0) ? 1: 0) + (i > 0 ? sectiond[i - 1]: 0);

        float r = 255, g = 0, b = 0;
        if (hs) {
            print("\x1b[38;2;%hhu;%hhu;%hhum%c", (u8)r, (u8)g, (u8)b, (half && !sz) ? '\xdd': '\xdb');
            
            for (u8 i = 0; i < sz + (u8)half - 1; i++) {
                if (i < sectiond[0]) g += 255.0 / sectiond[0];
                else if (i < sectiond[1]) g = 255, r -= 255.0 / (sectiond[1] - sectiond[0]);
                else if (i < sectiond[2]) r = 0,   b += 255.0 / (sectiond[2] - sectiond[1]);
                else if (i < sectiond[3]) b = 255, g -= 255.0 / (sectiond[3] - sectiond[2]);
                else if (i < sectiond[4]) g = 0,   r += 255.0 / (sectiond[4] - sectiond[3]);
                else                      r = 255, b -= 255.0 / (sectiond[5] - sectiond[4]);
                
                print("\x1b[38;2;%hhu;%hhu;%hhum%c", (u8)r, (u8)g, (u8)b, (half && i == sz - 1) ? '\xdd': '\xdb');
            }
}*/
