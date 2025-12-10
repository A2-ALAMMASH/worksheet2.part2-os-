#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

enum {
    FB_COLOR_BLACK = 0,
    FB_COLOR_BLUE = 1,
    FB_COLOR_GREEN = 2,
    FB_COLOR_CYAN = 3,
    FB_COLOR_RED = 4,
    FB_COLOR_MAGENTA = 5,
    FB_COLOR_BROWN = 6,
    FB_COLOR_LIGHT_GREY = 7,
    FB_COLOR_DARK_GREY = 8,
    FB_COLOR_LIGHT_BLUE = 9,
    FB_COLOR_LIGHT_GREEN = 10,
    FB_COLOR_LIGHT_CYAN = 11,
    FB_COLOR_LIGHT_RED = 12,
    FB_COLOR_LIGHT_MAGENTA = 13,
    FB_COLOR_LIGHT_BROWN = 14,
    FB_COLOR_WHITE = 15
};

void fb_set_color(unsigned char fg, unsigned char bg);
void fb_clear(void);
void fb_move(unsigned short x, unsigned short y);
void fb_print(const char *str);
void fb_print_hex(unsigned int value);
void fb_print_dec(int value);
void fb_write_char(char c);
void fb_backspace(void);
void fb_newline(void);
void fb_write_string(const char *str);
void fb_write_dec(int value);

#endif
