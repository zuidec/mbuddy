#ifndef NIX_SERIAL_H
#define NIX_SERIAL_H

#include <stdbool.h>

#ifdef  DEBUG
#define printd(...)      printf(__VA_ARGS__)
#else
#define printd(...)      /*printf(__VA_ARGS__)*/
#endif

typedef int serial_handle_t;

serial_handle_t serial_port_init(const char* port, int baudrate);
void serial_port_close(serial_handle_t serial);
bool serial_is_connected(serial_handle_t serial);
int serial_print(serial_handle_t serial, const char* data, int data_size);
int serial_println(serial_handle_t serial, const char* data, int data_size);
int serial_read(serial_handle_t serial, char* buffer, int buffer_size);
int serial_readln(serial_handle_t serial, char* buffer, int buffer_size);
bool serial_data_available(serial_handle_t serial);
void serial_update_baudrate(serial_handle_t serial, int baudrate);

#endif

