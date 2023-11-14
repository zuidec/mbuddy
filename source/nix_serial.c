/*
 *	nix_serial.c
 *	Library to access serial linux serial ports
 *
 *	Created by zuidec on 11/11/23
 */

#include <stdio.h>
#include <string.h>

// Needed to access serial devices
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

// Needed for select() timeout 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include "nix_serial.h"


int init_serial_port(const char* port, int baudrate) {

    printd("Opening serial port...");
    int serial_port = open(port, O_RDWR | O_NOCTTY);
    printd(" done!\n");
    if(serial_port < 0) {
        // Do something about the error
            printd("\nError number %i: %s\n",errno, strerror(errno));
            return -1;
    }

    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0)   {
        // Handle error
            printd("\nError number %i: %s\n",errno, strerror(errno));
            return -1;
    }

    // Disable parity and set to 1 stop bit with 8 bits data
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // Disable hardware flow control
    tty.c_cflag &= ~CRTSCTS;

    // Set readable and set CLOCAL to prevent carrier detect signals
    tty.c_cflag |= CREAD | CLOCAL;

    // Disable canonical mode (canonical mode only makes data available when
    // a newline is received. Also disable echo mode
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;

    // Disable special signal chars
    tty.c_lflag &= ~ISIG;

    // Disable software flow control
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Disable processing so we get raw data
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // Disable processing of output data
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    // Set timeout to 1ms to prevent blocking
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    printd("Setting baudrate %i... ", baudrate);
    // Set baudrate
    cfsetspeed(&tty, baudrate);
    printd("set!\nSaving tty attributes... ");
    // Save the settings and check for error
    if(tcsetattr(serial_port, TCSANOW, &tty) != 0)  {
        // Handle error 
            printd("\nError number %i: %s\n",errno, strerror(errno));
            return -1;
    }
    printd("set!\n");
    return serial_port;
}

void close_serial_port(serial_handle_t serial)  {
    close(serial);
}

int serial_print(serial_handle_t serial, const char* data, int data_size)  {
    return write(serial, data, data_size);
}

int serial_println(serial_handle_t serial, const char* data, int data_size)    {
    int bytes_sent =0;

    return bytes_sent;
}

int serial_read(serial_handle_t serial, char* buffer, int buffer_size)  {
    return read(serial, buffer, buffer_size);
}

int serial_readln(serial_handle_t serial, char* buffer, int buffer_size)  {
    int bytes_received = 0;

    return bytes_received;
}

bool serial_data_available(serial_handle_t serial)  {
    fd_set set;
    struct timeval timeout;
    int return_value;

    FD_ZERO(&set);
    FD_SET(serial, &set);

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100;

    return_value = select(serial + 1, &set, NULL, NULL, &timeout);

    if(return_value <= 0)   {
        return false;
    }
    else    {
        return true;
    }
    
}
