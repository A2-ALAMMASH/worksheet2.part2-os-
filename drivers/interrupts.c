#include "interrupts.h"
#include "pic.h"
#include "../source/io.h"
#include "framebuffer.h"
#include "keyboard.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256
#define INTERRUPTS_KEYBOARD 33
#define INPUT_BUFFER_SIZE 256
#define SERIAL_PORT 0x3F8

static u8int input_buffer[INPUT_BUFFER_SIZE];
static u32int buffer_write_index = 0;
static u32int buffer_read_index  = 0;

struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
struct IDT idt;

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

void interrupts_init_descriptor(s32int index, u32int address)
{
    idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF;
    idt_descriptors[index].offset_low  = address & 0xFFFF;
    idt_descriptors[index].segment_selector = 0x08;
    idt_descriptors[index].reserved = 0x00;
    idt_descriptors[index].type_and_attr =
        (0x01 << 7) |
        (0x00 << 6) |
        (0x00 << 5) |
        0x0E;
}

void interrupts_install_idt()
{
    interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (u32int)interrupt_handler_33);

    idt.address = (u32int)&idt_descriptors;
    idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;

    load_idt((u32int)&idt);

    pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);

    outb(0x21, inb(0x21) & ~(1 << 1));
}

void interrupt_handler(struct cpu_state cpu,
                       u32int interrupt,
                       struct stack_state stack)
{
    if (interrupt == INTERRUPTS_KEYBOARD)
    {
        while (inb(0x64) & 1)
        {
            u8int scan_code = keyboard_read_scan_code();
            u8int ascii = keyboard_scan_code_to_ascii(scan_code);

            if (ascii == 0)
            {
                continue;
            }

            if (ascii == '\b')
            {
                if (buffer_write_index != buffer_read_index)
                {
                    if (buffer_write_index == 0)
                    {
                        buffer_write_index = INPUT_BUFFER_SIZE - 1;
                    }
                    else
                    {
                        buffer_write_index--;
                    }
                }

                fb_backspace();
                serial_write_string("\b \b");
            }
            else
            {
                input_buffer[buffer_write_index] = ascii;
                buffer_write_index = (buffer_write_index + 1) % INPUT_BUFFER_SIZE;

                if (buffer_write_index == buffer_read_index)
                {
                    buffer_read_index = (buffer_read_index + 1) % INPUT_BUFFER_SIZE;
                }

                if (ascii == '\n')
                {
                    fb_newline();
                    serial_write_string("\r\n");
                }
                else
                {
                    fb_write_char(ascii);
                    serial_write_char(ascii);
                }
            }
        }

        pic_acknowledge(interrupt);
    }
}

int getc(void)
{
    if (buffer_read_index == buffer_write_index)
    {
        return -1;
    }

    u8int c = input_buffer[buffer_read_index];
    buffer_read_index = (buffer_read_index + 1) % INPUT_BUFFER_SIZE;
    return (int)c;
}

int readline(char* buffer, int size)
{
    int count = 0;

    if (size <= 0)
    {
        return 0;
    }

    while (1)
    {
        int ch = getc();
        if (ch == -1)
        {
            continue;
        }

        if (ch == '\n')
        {
            break;
        }

        if (count < size - 1)
        {
            buffer[count++] = (char)ch;
        }
    }

    buffer[count] = '\0';
    return count;
}
