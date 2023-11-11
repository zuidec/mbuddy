#ifndef NIX_SERIAL_H
#define NIX_SERIAL_H

#ifdef  DEBUG
#define printd printf
#else
#define printd /*printf*/
#endif

typedef int serial_handle_t;

serial_handle_t init_serial_port(const char* port, int baudrate);
void close_serial_port(serial_handle_t serial);
int serial_print(serial_handle_t serial, const char* data, int data_size);
int serial_println(serial_handle_t serial, const char* data, int data_size);
int serial_read(serial_handle_t serial, char* buffer, int buffer_size);
int serial_readln(serial_handle_t serial, char* buffer, int buffer_size);

#endif

