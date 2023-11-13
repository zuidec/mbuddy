/*
 *	mbuddy.c
 *  Entry point for mbuddy serial monitor program
 *
 *	Created by zuidec on 11/11/23
 */

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "nix_serial.h"

#define DEFAULT_BAUDRATE    (115200)
#define DEFAULT_PORT        ("/dev/ttyUSB0")

static void print_usage(void);
static bool is_backspace(int key);
static bool is_special_key(int key);

const char* cmd = "mbuddy";
char* exit_msg = "Press F1 to exit";

int main(int argc, char *argv[]) {
    
    // Print usage and exit if too many args
    if(argc > MAX_ARGS) {
        printf("Error: too many arguments\n");
        print_usage();
        return 0;
    }
    
    char* port = DEFAULT_PORT;
    int baudrate = DEFAULT_BAUDRATE;

    // Get command line arguments, skip if no arguments
    if(argc > 1) {
        for(int i = 1; i < argc; i++)   {
            // Show usage if passed -h or --help
            if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))  {
                print_usage();
                return 0;
            }
            // Store the port name if passed
            else if(!strcmp(argv[i], "-p") && argc >= i+1)   {
                port = argv[i+1];

            }
            // Store the baudrate if passed
            else if(!strcmp(argv[i], "-b") && argc >= i+1)   {
                baudrate = atoi(argv[i+1]);
            }
        }
    }

    init_gui();

    // Initialize status bar settings
    status_bar_t status = {port, baudrate, exit_msg, false};

    serial_handle_t  serial_port = init_serial_port(port, baudrate);
    if(serial_port >= 0) {
        status.is_connected = true;
    }
    update_status_bar(&status);    

    // Update the input box
    update_input_box("");
    
    // Set up the entry buffer and indexing
    int input_size = get_input_box_width();
    char input_data[input_size];
    memset(&input_data[0], '\0', input_size);
    int input_index = 0;
    char serial_input_buffer[256];
    int ch = 0;
    int bytes_read = 0;

    while(ch != KEY_F(1))   {

        if(new_input_box_char())    {
            ch = get_input_box_char();
        if(input_index < input_size && !is_special_key(ch))   {
            input_data[input_index] = ch;
            input_index++;
            move_input_cursor(1);
            update_input_box(&input_data[0]);
        }
        
        if(is_backspace(ch) && input_index > 0) {
            
            input_index--;
            input_data[input_index] = '\0';
            move_input_cursor(-1);
            update_input_box(&input_data[0]);

        }
        
        bytes_read = serial_read(serial_port, &serial_input_buffer[0], sizeof(serial_input_buffer));
        if(bytes_read > 0)  {
        //    update_main_window(&serial_input_buffer[0], bytes_read);
            bytes_read = 0;
        }
    }
    }

    close_serial_port(serial_port);
    endwin(); 
    return 0;
}

static void print_usage(void)   {

    printf("Usage: %s [-p port] [-b baudrate]\n", cmd);
    printf("\tExample: %s /dev/ttyUSB0 115200\n\n", cmd);
    printf("\t-p port\t\t\tDefault is %s\n", DEFAULT_PORT);
    printf("\t-b baudrate\t\tDefault is %i\n", DEFAULT_BAUDRATE); 
}

static bool is_backspace(int key)  {
    
    if(key==KEY_BACKSPACE) {
        return true;
    }
    if(key==8) {
        return true;
    }

    return false;
}

static bool is_special_key(int key) {
    if(key>=32 && key<=127) {
        return false;
    }
    
    return true;
 }

