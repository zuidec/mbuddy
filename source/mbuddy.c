#include <bits/stdint-uintn.h>
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "nix_serial.h"

#define INPUT_BOX_HEIGHT    (3)
#define COLUMN_OFFSET       (2)
#define NO_COLUMN_OFFSET    (0)
#define STATUS_BAR_HEIGHT   (2)
#define MAX_ARGS            (5)
#define DEFAULT_BAUDRATE    (115200)
#define DEFAULT_PORT        ("dev/ttyUSB0")

const char* cmd = "mbuddy";

typedef struct _win_border_struct   {
    chtype ls, rs, ts, bs,  // Character to use on sides: left side, right side, top side, bottom side
           tl, tr, bl, br;  // Character to use on corners: top left, top right, bottom left, bottom right
} WIN_BORDER;

typedef struct WIN_PARAMS   {
    int height, width;
    int x_pos, y_pos;
} WIN_PARAMS;

typedef struct status_bar_t    {
    char* port;
    int baudrate;
    WINDOW* window;
    char* exit_msg;
} status_bar_t;

WINDOW* init_window(WIN_PARAMS* params);
void init_window_params(WIN_PARAMS* main_params, WIN_PARAMS* input_params, WIN_PARAMS* status_params);
void update_status_bar(status_bar_t* status_bar, WIN_PARAMS* status_params);
void update_input_box(WINDOW* input_box, char* data);
int get_input(char* buffer);
static void print_usage(void);
static bool is_backspace(int code);

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


void init_window_params(WIN_PARAMS* main_params, WIN_PARAMS* input_params, WIN_PARAMS* status_params)  {
    int max_y, max_x; 
    max_y = getmaxy(stdscr);
    max_x = getmaxx(stdscr);

    main_params->width      = max_x- COLUMN_OFFSET;
    main_params->height     = (max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT - 1);
    main_params->x_pos      = COLUMN_OFFSET;
    main_params->y_pos      = 0;

    input_params->width     = max_x - COLUMN_OFFSET;
    input_params->height    = INPUT_BOX_HEIGHT;
    input_params->x_pos     = COLUMN_OFFSET;
    input_params->y_pos     = max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT -1;

    status_params->width    = max_x -1;
    status_params->height   = STATUS_BAR_HEIGHT;
    status_params->x_pos    = NO_COLUMN_OFFSET; 
    status_params->y_pos    = max_y - STATUS_BAR_HEIGHT - 1;
}

WINDOW* init_window( WIN_PARAMS* params)   {
    WINDOW* window;
    // newwin( height, width, y, x )
    window = newwin(params->height, params->width, params->y_pos, params->x_pos);
    box( window, 0, 0); 
    wrefresh(window);

    return window;
}

void update_status_bar(status_bar_t* status_bar, WIN_PARAMS* status_params) {

    char msg[64] = {' '};
    sprintf(&msg[0], "Port: %s  Baudrate: %i", status_bar->port, status_bar->baudrate);
    // Clear then reprint data
    wclear(status_bar->window);
    mvwprintw(status_bar->window, 1, COLUMN_OFFSET, "Press F1 to exit");
    mvwprintw(status_bar->window, 1, (status_params->width - strlen(msg)), "%s", &msg[0]);
    wrefresh(status_bar->window);
}

void update_input_box(WINDOW* input_box, char* data)   {
    
    werase(input_box);
    box(input_box, 0, 0); 
    mvwprintw(input_box, 1, COLUMN_OFFSET, "> %s", data);
}

int get_input(char* buffer) {
    int bytes_read = 0;

    return bytes_read;
}

static void print_usage(void)   {

    printf("Usage: %s [-p port] [-b baudrate]\n", cmd);
    printf("\tExample: %s /dev/ttyUSB0 115200\n", cmd);
    printf("\t-p port\t\t\tDefault is /dev/ttyUSB0\n");
    printf("\t-b baudrate\t\tDefault is 115200\n"); 
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
