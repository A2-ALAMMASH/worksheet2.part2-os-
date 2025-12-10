#include "../drivers/framebuffer.h"
#include "../drivers/interrupts.h"
#include "../drivers/hardware_interrupt_enabler.h"
#include "io.h"

#define SERIAL_PORT 0x3F8

static void serial_init(void)
{
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
}

static int serial_is_transmit_empty(void)
{
    return inb(SERIAL_PORT + 5) & 0x20;
}

static void serial_write_char(char c)
{
    while (!serial_is_transmit_empty())
    {
    }
    outb(SERIAL_PORT, (unsigned char)c);
}

static void serial_write_string(const char *str)
{
    char c;
    while ((c = *str) != '\0')
    {
        serial_write_char(c);
        str++;
    }
}

static void serial_write_dec(int value)
{
    char buffer[12];
    int i = 0;
    int is_negative = 0;
    unsigned int v;

    if (value == 0)
    {
        serial_write_char('0');
        return;
    }

    if (value < 0)
    {
        is_negative = 1;
        v = (unsigned int)(-value);
    }
    else
    {
        v = (unsigned int)value;
    }

    while (v > 0 && i < 11)
    {
        unsigned int digit = v % 10;
        buffer[i] = (char)('0' + digit);
        v /= 10;
        i++;
    }

    if (is_negative && i < 11)
    {
        buffer[i] = '-';
        i++;
    }

    while (i > 0)
    {
        i--;
        serial_write_char(buffer[i]);
    }
}

int sum_of_three(int a, int b, int c)
{
    return a + b + c;
}

int max_two(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

int square(int x)
{
    return x * x;
}

void kernel_main(void)
{
    serial_init();
    serial_write_string("OS kernel is starting\r\n");

    fb_set_color(FB_COLOR_LIGHT_GREY, FB_COLOR_BLACK);
    fb_clear();

    interrupts_install_idt();
    enable_hardware_interrupts();

    serial_write_string("Interrupts enabled\r\n");

    fb_move(0, 0);
    fb_write_string("OS kernel is running.");

    serial_write_string("Framebuffer initialized\r\n");

    fb_move(0, 2);
    int s = sum_of_three(1, 2, 3);
    fb_write_string("sum_of_three(1,2,3) = ");
    fb_write_dec(s);
    serial_write_string("sum_of_three(1,2,3) = ");
    serial_write_dec(s);
    serial_write_string("\r\n");

    fb_move(0, 3);
    int m = max_two(10, 4);
    fb_write_string("max_two(10,4) = ");
    fb_write_dec(m);
    serial_write_string("max_two(10,4) = ");
    serial_write_dec(m);
    serial_write_string("\r\n");

    fb_move(0, 4);
    int sq = square(7);
    fb_write_string("square(7) = ");
    fb_write_dec(sq);
    serial_write_string("square(7) = ");
    serial_write_dec(sq);
    serial_write_string("\r\n");

    fb_move(0, 6);
    fb_write_string("Kernel ready. Type on the keyboard.");
    serial_write_string("Kernel ready. Type on the keyboard.\r\n");

    while (1)
    {
    }
}
