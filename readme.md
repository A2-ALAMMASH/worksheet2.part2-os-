# Worksheet 2 – Part 2  
Interrupts, PIC Remapping, Keyboard Driver, and Serial Output

## 1. Overview
This project extends the kernel from **Part 1** by adding full interrupt support, including:

- IDT (Interrupt Descriptor Table)
- PIC remapping
- Hardware interrupt enabling (STI)
- Keyboard interrupt handler (IRQ1)
- Serial port output (COM1) for debugging
- Circular input buffer for keyboard input

The kernel now receives hardware interrupts, handles keyboard input, and prints to both framebuffer and serial.

---

## 2. Project Structure
![alt text](<Screenshot 2025-12-10 103438.png>)


---

## 3. What Was Added in Part 2

###  1. Interrupt Descriptor Table (IDT)
- 256-entry IDT table  
- Keyboard interrupt mapped to entry **33**  
- Installed using `interrupts_install_idt()`  

###  2. PIC Remapping
- PIC1 mapped to 0x20  
- PIC2 mapped to 0x28  
- Allows clean IRQ handling without clashing with CPU exceptions.

###  3. Keyboard Driver
- Reads scan codes from port **0x60**
- Converts scan codes to ASCII
- Supports:
  - normal keys  
  - newline  
  - backspace  
- Stores input in a circular buffer  
- Prints characters to both framebuffer and serial  

###  4. Serial Port (COM1)
Used for debugging when running QEMU with `-nographic` or `curses`.  
Functions:
- `serial_init()`
- `serial_write_char()`
- `serial_write_string()`
- `serial_write_dec()`

###  5. Hardware Interrupts Enabled
From `hardware_interrupt_enabler.s`:
- Executes the `sti` instruction
- Activates hardware interrupts globally

---

## 4. Kernel Behavior in Part 2

When booted, the kernel:

1. Initializes framebuffer + clears screen  
2. Initializes serial port  
3. Installs IDT  
4. Remaps PIC  
5. Enables hardware interrupts  
6. Prints arithmetic test results  
7. Prints: Kernel ready. Type on the keyboard.
8. Enters an infinite loop while handling keyboard interrupts in the background.

---

## 5. Building the OS

make
Produces:

iso/boot/kernel.elf
os.iso

Clean:

make clean

## 6. Running the OS
The Makefile uses a QEMU mode suitable for SSH/terminal:

make run

Output example:

OS kernel is starting

Interrupts enabled

Framebuffer initialized

sum_of_three(1,2,3) = 6

max_two(10,4) = 10

square(7) = 49

sKernel ready. Type on the keyboard.

Keyboard interrupts work on systems with GUI QEMU.

On SSH servers (csctcloud), keyboard scanning is limited due to -nographic.

## 7. Screenshots
![alt text](<Screenshot 2025-12-10 104944.png>)

![alt text](<Screenshot 2025-12-10 105019.png>)

![alt text](<Screenshot 2025-12-10 101929.png>)

## 8. Summary

Part 2 adds low-level OS features:

Real hardware interrupt support

IDT setup

PIC remapping

Keyboard driver

Serial debugging output

The kernel now handles asynchronous hardware events and interactive input, completing all requirements for Worksheet 2 – Part 2.