#include "framebuffer.h"

#define FB_WIDTH 80
#define FB_HEIGHT 25
#define FB_ADDRESS 0x000B8000

static volatile unsigned char *fb = (unsigned char *)FB_ADDRESS;
static unsigned short cursor_x = 0;
static unsigned short cursor_y = 0;
static unsigned char fb_color = 0x07;

static inline void outb(unsigned short port, unsigned char data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static void fb_update_cursor(void) {
    unsigned short pos = (unsigned short)(cursor_y * FB_WIDTH + cursor_x);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

static void fb_write_cell(unsigned short x, unsigned short y, char c, unsigned char color) {
    unsigned int index = (y * FB_WIDTH + x) * 2;
    fb[index] = (unsigned char)c;
    fb[index + 1] = color;
}

void fb_set_color(unsigned char fg, unsigned char bg) {
    fb_color = (unsigned char)((bg << 4) | (fg & 0x0F));
}

void fb_clear(void) {
    unsigned short y;
    unsigned short x;
    for (y = 0; y < FB_HEIGHT; y++) {
        for (x = 0; x < FB_WIDTH; x++) {
            fb_write_cell(x, y, ' ', fb_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    fb_update_cursor();
}

void fb_move(unsigned short x, unsigned short y) {
    if (x >= FB_WIDTH) {
        x = (unsigned short)(FB_WIDTH - 1);
    }
    if (y >= FB_HEIGHT) {
        y = (unsigned short)(FB_HEIGHT - 1);
    }
    cursor_x = x;
    cursor_y = y;
    fb_update_cursor();
}

static void fb_scroll_if_needed(void) {
    unsigned short y;
    unsigned short x;
    if (cursor_y < FB_HEIGHT) {
        return;
    }
    for (y = 1; y < FB_HEIGHT; y++) {
        for (x = 0; x < FB_WIDTH; x++) {
            unsigned int from_index = (y * FB_WIDTH + x) * 2;
            unsigned int to_index = ((y - 1) * FB_WIDTH + x) * 2;
            fb[to_index] = fb[from_index];
            fb[to_index + 1] = fb[from_index + 1];
        }
    }
    for (x = 0; x < FB_WIDTH; x++) {
        fb_write_cell(x, (unsigned short)(FB_HEIGHT - 1), ' ', fb_color);
    }
    cursor_y = (unsigned short)(FB_HEIGHT - 1);
}

static void fb_put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        fb_scroll_if_needed();
        fb_update_cursor();
        return;
    }
    fb_write_cell(cursor_x, cursor_y, c, fb_color);
    cursor_x++;
    if (cursor_x >= FB_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        fb_scroll_if_needed();
    }
    fb_update_cursor();
}

void fb_write_char(char c) {
    fb_put_char(c);
}

void fb_backspace(void) {
    if (cursor_x == 0 && cursor_y == 0) {
        return;
    }
    if (cursor_x == 0) {
        cursor_y--;
        cursor_x = (unsigned short)(FB_WIDTH - 1);
    } else {
        cursor_x--;
    }
    fb_write_cell(cursor_x, cursor_y, ' ', fb_color);
    fb_update_cursor();
}

void fb_newline(void) {
    fb_put_char('\n');
}

void fb_print(const char *str) {
    char c;
    while ((c = *str) != '\0') {
        fb_put_char(c);
        str++;
    }
}

void fb_write_string(const char *str) {
    fb_print(str);
}

void fb_print_hex(unsigned int value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[9];
    int i;
    for (i = 7; i >= 0; i--) {
        unsigned int nibble = value & 0x0F;
        buffer[i] = hex_chars[nibble];
        value >>= 4;
    }
    buffer[8] = '\0';
    fb_print("0x");
    fb_print(buffer);
}

void fb_print_dec(int value) {
    char buffer[12];
    int i = 0;
    int is_negative = 0;
    unsigned int v;
    if (value == 0) {
        fb_put_char('0');
        return;
    }
    if (value < 0) {
        is_negative = 1;
        v = (unsigned int)(-value);
    } else {
        v = (unsigned int)value;
    }
    while (v > 0 && i < 11) {
        unsigned int digit = v % 10;
        buffer[i] = (char)('0' + digit);
        v /= 10;
        i++;
    }
    if (is_negative && i < 11) {
        buffer[i] = '-';
        i++;
    }
    while (i > 0) {
        i--;
        fb_put_char(buffer[i]);
    }
}

void fb_write_dec(int value) {
    fb_print_dec(value);
}
