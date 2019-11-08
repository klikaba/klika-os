#include <serial.h>
#include <stdint.h>
#include <x86.h>

#define COM1_PORT 0x3F8
#define COM2_PORT 0x2F8

void init_kernel_serial() {
	// Init COM1 and COM2
	outp(COM1_PORT + 1, 0x00);    // Disable all interrupts
	outp(COM1_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outp(COM1_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outp(COM1_PORT + 1, 0x00);    //                  (hi byte)
	outp(COM1_PORT + 3, 0x02);    // 7 bits, no parity, one stop bit
	outp(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outp(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	outp(COM2_PORT + 1, 0x00);    // Disable all interrupts
	outp(COM2_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outp(COM2_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outp(COM2_PORT + 1, 0x00);    //                  (hi byte)
	outp(COM2_PORT + 3, 0x02);    // 7 bits, no parity, one stop bit
	outp(COM2_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outp(COM2_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

static inline int serial_transmit_empty(uint16_t port) {
	return inp(port + 5) & 0x20;
}

void serial_write_com(int com, unsigned char data)
{
	uint16_t port;
	switch (com) {
		case 1:
			port = COM1_PORT;
			break;
		case 2:
			port = COM2_PORT;
			break;
		// TODO: COM3 and COM4
		default:
			return;
	}

	while (serial_transmit_empty(port) == 0)
		__builtin_ia32_pause();
	outp(port, data);
}
