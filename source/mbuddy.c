/*
 *	mbuddy.c
 *  Entry point for mbuddy serial monitor program
 *
 *	Created by zuidec on 11/11/23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "nix_serial.h"

#define DEFAULT_BAUDRATE    (115200)
#define DEFAULT_PORT        ("dev/ttyUSB0")

static void print_usage(void);
static bool is_backspace(int code);

const char* cmd = "mbuddy";

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

    // Create param structs for the GUI windows
    WIN_PARAMS main_params, input_params, status_params;
    
    // Initialize the screen, allow CTRL-C to escape, disable echo
    initscr();
    cbreak();
    noecho();
    //start_color();
    
    // Initialize window parameters to default parameters
    init_window_params(&main_params, &input_params,&status_params);
    // Then initialize the windows themselves
    WINDOW* main_window = init_window(&main_params);
    WINDOW* input_window = init_window(&input_params);
    WINDOW* status_bar = init_window(&status_params);
    
    // Allow keypad in the input window
    keypad(input_window, true);

    // Update the status bar
    status_bar_t status = {port, baudrate, status_bar, "Press F1 to exit"};
    update_status_bar(&status, &status_params);
    
    for(int i =0; i< argc; i++) {
        mvwprintw(main_window,i+2, 2, "Argument %i is %s", i, argv[i]);
        wrefresh(main_window);
    }
    
    mvwprintw(input_window, 1, COLUMN_OFFSET, "> ");
    wrefresh(input_window);

    int input_size = input_params.width-1-(COLUMN_OFFSET * 3);
    char input_data[input_size];
    memset(&input_data[0], '\0', input_size);
    int input_index = 0;
    uint32_t ch = 0;

    while((ch = wgetch(input_window)) != KEY_F(1))   {

        if(input_index < input_size && !is_backspace(ch))   {
            input_data[input_index] = ch;
            input_index++;
            int y, x;
            getyx(input_window,y,x);
            wmove(input_window,y, x++); 
            update_input_box(input_window, &input_data[0]);
        }
        
        if(is_backspace(ch) && input_index > 0) {
            
            input_index--;
            input_data[input_index] = '\0';
            int y, x;
            getyx(input_window,y,x);
            wmove(input_window,y, x--);
            update_input_box(input_window, &input_data[0]);

        }
    }

    endwin(); 
    return 0;
}

int get_input(char* buffer) {
    int bytes_read = 0;

    return bytes_read;
}

static void print_usage(void)   {

    printf("Usage: %s [-p port] [-b baudrate]\n", cmd);
    printf("\tExample: %s /dev/ttyUSB0 115200\n\n", cmd);
    printf("\t-p port\t\t\tDefault is %s\n", DEFAULT_PORT);
    printf("\t-b baudrate\t\tDefault is %i\n", DEFAULT_BAUDRATE); 
}

static bool is_backspace(int code)  {
    
    if(code==KEY_BACKSPACE) {
        return true;
    }
    if(code==8) {
        return true;
    }

    return false;
}
