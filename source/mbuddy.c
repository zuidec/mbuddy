/*
 *	mbuddy.c
 *  Entry point for mbuddy serial monitor program
 *
 *	Created by zuidec on 11/11/23
 */

#include <curses.h>
#include <ncursesw/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "nix_serial.h"

#define DEFAULT_BAUDRATE    (115200)
#define DEFAULT_PORT        ("/dev/ttyUSB0")
#define SERIAL_BUFFER_SIZE  (256)

static void print_usage(void);
static bool is_backspace(int key);
static bool is_special_key(int key);
static bool exit_key_pressed(void);
static void serial_update(serial_handle_t serial_port);
static void refresh_serial_status(serial_handle_t* serial_port,status_bar_t* status);
static void update_user_input(serial_handle_t serial_port,status_bar_t* status); 

const char* cmd         = "mbuddy";
char* exit_msg          = "Press F1 to exit";
int input_index         = 0;
int input_size          = 0;
char* input_data;

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

    serial_handle_t  serial_port = serial_port_init(port, baudrate);
    if(serial_port >= 0) {
        status.is_connected = true;
    }
    else    {
        status.is_connected = false;
        serial_port_close(serial_port);
    }
    update_status_bar(&status);    

    // Update the input box
    update_input_box("");
    input_size = get_input_box_width(); 
    input_data = malloc(input_size);
    memset(&input_data[0], '\0', input_size);

/*
 *  Main program loop
 */
    while(!exit_key_pressed())   {

        refresh_serial_status(&serial_port, &status);
        update_user_input(serial_port ,&status); 
        if(status.is_connected) {
            serial_update(serial_port);
        } 
        //update_main_window("It's working!\0\n", strlen("It's working!\0\n")+2);
    }

    serial_port_close(serial_port);
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

static bool exit_key_pressed(void)  {
    int key = 0;

    // Check the next available char if available and see if its the exit key
    if(new_input_box_char())    {
        key = peek_input_box_char();
        if(key==KEY_F(1))   {
            return true;
        }
    }

    // Else
    return false;
}

static void serial_update(serial_handle_t serial_port)    {
                
    int bytes_read = 0;
    char serial_input_buffer[SERIAL_BUFFER_SIZE];
    memset(&serial_input_buffer[0],'\0', SERIAL_BUFFER_SIZE);

    if(serial_data_available(serial_port))   {

        bytes_read = serial_read(serial_port, &serial_input_buffer[0], SERIAL_BUFFER_SIZE);
        
        update_main_window(&serial_input_buffer[0], bytes_read);
        bytes_read = 0;
        memset(&serial_input_buffer[0],'\0', SERIAL_BUFFER_SIZE);
    }
}

static void update_user_input(serial_handle_t serial_port, status_bar_t* status)  {

    int ch = 0;

    // First check if there is a new character available
    if(new_input_box_char())    {
        ch = get_input_box_char();
        if(input_index < input_size && !is_special_key(ch))   {
            input_data[input_index] = ch;
            input_index++;
            move_input_cursor(1);
            update_input_box(&input_data[0]);
        }
        
        // Remove last character if backspace is pressed
        if(is_backspace(ch) && input_index > 0) {
           
            // Decrement the index and replace that character with a null
            input_index--;
            input_data[input_index] = '\0';
            move_input_cursor(-1);
            update_input_box(&input_data[0]);

        }
        
        // Send data to the main window and to the serial device when enter is pressed
        if(ch=='\n')   {
            input_data[input_index] = '\n';

            // Only attempt a print if serial device is connected
            if(status->is_connected)    {
                serial_print(serial_port, &input_data[0], input_index+1);
            }

            // Change text color and add >>> to distinguish input from output
            // and print to main window
            main_window_attron(COLOR_PAIR(50));
            update_main_window(">>> ", strlen(">>> "));
            update_main_window(&input_data[0], input_index+1);
            main_window_attroff(COLOR_PAIR(50));
            
            // Clear the buffer, input box, and reset the index
            memset(&input_data[0], '\0', input_size);
            update_input_box(&input_data[0]);
            move_input_cursor(0-input_index);
            input_index = 0;
        }
    }
}


static void refresh_serial_status(serial_handle_t* serial_port,status_bar_t* status)    {
    
    if(status->is_connected == false)   {
        *serial_port = serial_port_init(status->port, status->baudrate);
        if(*serial_port >= 0)   {
            status->is_connected = true;
            update_status_bar(status);
        }
        else    {
            serial_port_close(*serial_port);
        }
    }
    else if(status->is_connected ==true)    {
        if(!serial_is_connected(*serial_port))   {
            status->is_connected = false;
            update_status_bar(status);
            serial_port_close(*serial_port);
        }
    }


}
