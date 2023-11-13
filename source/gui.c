/*
 *	gui.c
 *	Function to create and update mbuddy GUI
 *
 *	Created by zuidec on 11/11/23
 */

#include <bits/stdint-uintn.h>
#include <curses.h>
#include <ncursesw/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "gui.h"

#define BLACK           (0)
#define BLUE            (1)
#define GREEN           (2)
#define CYAN            (3)
#define RED             (4)
#define MAGENTA         (5)
#define BROWN           (6)
#define WHITE           (7)
#define B_BLACK         (8)
#define B_BLUE          (9)
#define B_GREEN         (10)
#define B_CYAN          (11)
#define B_RED           (12)
#define B_MAGENTA       (13)
#define YELLOW          (14)
#define B_WHITE         (15)

#define STATUS_RED      (3)
#define STATUS_GREEN    (1)
#define STATUS_NORM     (2)
#define MAIN_WINDOW_START_LN    (2)
#define INPUT_INDEX_OFFSET      (4)
WIN_PARAMS main_params, input_params, status_params;
WINDOW* main_window;
WINDOW* input_window;
WINDOW* status_window; 

int main_window_index = MAIN_INDEX_OFFSET;
int input_box_index = INPUT_INDEX_OFFSET;

static WINDOW* init_window(WIN_PARAMS* params);
static void init_window_params(void);
static void init_colors(void);
static void set_status_colors(void);
static void reset_status_colors(void);
static const char* inttostr(int number); 
static void chtype_to_str(int* ch, char* str, int size);

static void init_window_params(void)  {

    // Get max screen dimensions
    int max_y, max_x; 
    max_y = getmaxy(stdscr);
    max_x = getmaxx(stdscr);

    // Set each window parameter based on max screen size
    main_params.max_y      = max_y;
    main_params.max_x      = max_x;
    main_params.width      = max_x- COLUMN_OFFSET;
    main_params.height     = max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT;
    main_params.start_x    = COLUMN_OFFSET;
    main_params.start_y    = 0;

    input_params.max_y     = max_y;
    input_params.max_x     = max_x;
    input_params.width     = max_x - COLUMN_OFFSET;
    input_params.height    = INPUT_BOX_HEIGHT;
    input_params.start_x   = COLUMN_OFFSET;
    input_params.start_y   = max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT;

    status_params.max_y    = max_y;
    status_params.max_x    = max_x;
    status_params.width    = max_x;
    status_params.height   = STATUS_BAR_HEIGHT;
    status_params.start_x  = NO_COLUMN_OFFSET; 
    status_params.start_y  = max_y - STATUS_BAR_HEIGHT;
}

static WINDOW* init_window(WIN_PARAMS* params)   {
    WINDOW* window;

    // Use window parameters to create and return a pointer to a new windown
    window = newwin(params->height, params->width, params->start_y, params->start_x);
    if(params->height!=STATUS_BAR_HEIGHT)   {
        box(window, 0, 0); 
    }
    wrefresh(window);

    return window;
}

void init_gui(void)  {

    // Initialize the screen, allow CTRL-C to escape, disable echo
    initscr();
    cbreak();
    noecho();

    // Initialize colors if terminal supports them
    if(has_colors())    {
        use_default_colors();
        start_color();
        init_colors();
    }

    // Initialize window parameters to default parameters
    init_window_params();

    // Then initialize the windows themselves
    main_window = init_window(&main_params);
    input_window = init_window(&input_params);
    status_window= init_window(&status_params);

    wclear(status_window);
    wrefresh(status_window);

    wbkgdset(status_window, COLOR_PAIR(STATUS_NORM));

    // Allow keypad in the input window
    keypad(input_window, true);
    
    // Make sure wgetch calls dont block
    nodelay(input_window,true);
    /*
    wmove(main_window, 1, 1);
    for(unsigned int i=0; i<32000; i++)    {
        wattr_on(main_window, COLOR_PAIR(i), NULL);
        char buf[8] = {'\0'};
        sprintf(&buf[0], "%i ",i);
        update_main_window(&buf[0],strlen(&buf[0]) );
        wattr_off(main_window, COLOR_PAIR(i), NULL);
        //wrefresh(main_window);
    }
    */
}

void update_status_bar(status_bar_t* status_bar) {

    char msg[64] = {' '};
    
    // Clear then reprint data
    wclear(status_window);
    sprintf(&msg[0], "(F1) Exit  (F2) Port: %s  (F3) Baudrate: %i", status_bar->port, status_bar->baudrate);
    mvwprintw(status_window, 1, COLUMN_OFFSET, "%s", msg);
    memset(msg, '\0', sizeof(msg));

    // Set color of connection based on status
    if(status_bar->is_connected)    {
        sprintf(&msg[0], "Connection OK  ");
        wattr_on(status_window, COLOR_PAIR(STATUS_GREEN), NULL);
        mvwprintw(status_window,1, status_params.width - (strlen(msg)), "%s", &msg[0]);
        wattr_off(status_window, COLOR_PAIR(STATUS_GREEN), NULL);
    }
    else    {
        sprintf(&msg[0], "Connection FAIL  ");
        wattr_on(status_window, COLOR_PAIR(STATUS_RED), NULL);
        mvwprintw(status_window,1, status_params.width - (strlen(msg)), "%s", &msg[0]);
        wattr_off(status_window, COLOR_PAIR(STATUS_RED), NULL);
    }

    // Put cursor back into input window and refresh
    wmove(input_window, 1, input_box_index);
    wrefresh(status_window);
}

void update_input_box(char* data)   {
    
    werase(input_window);
    box(input_window, 0, 0); 
    mvwprintw(input_window, 1, COLUMN_OFFSET, "> %s", data);
}

void move_input_cursor(int distance)    {
    input_box_index+=distance;
    wmove(input_window, 1, input_box_index);
}

int get_input_box_width(void)  {
    return input_params.width-1-(WINDOW_PADDING);
}

int get_input_box_char(void)   {
    return  wgetch(input_window);
}

bool new_input_box_char(void)   {
    int ch = wgetch(input_window); 
    if(ch==ERR) {
        return false;
    }
    ungetch(ch);
    return true;
}

void update_main_window(const char *data, int size)  {
    
    // Get the available height/width for printing
    int window_height = main_params.height-MAIN_INDEX_OFFSET; 
    int window_width = main_params.width - WINDOW_PADDING;
    
    // Make a temporary buffer to store each line as it's moved
    chtype current_line[window_width];
    memset(&current_line[0], '\0', window_width);

    int bytes_written = 0;

    // Loop while there is still data to output
    while(bytes_written < size) {
        
        // Stay within this loop as long as the end of line isn't reached
        while(main_window_index < window_width && bytes_written < size)  {
            
            // Shift everything if a newline character is received
            if(data[bytes_written]=='\n')   {
                
                // Reset window index and increment the write index
                main_window_index = MAIN_INDEX_OFFSET; 
                bytes_written++;
                
                for(int i=2; i<= window_height; i++)    {
                    // Grab one line at a time, copy it, shift it up, then move
                    // down, etc.. until all lines are shifted
                    mvwinchnstr(main_window,i, MAIN_INDEX_OFFSET, &current_line[0], window_width);
                    wmove(main_window, i-1, MAIN_INDEX_OFFSET);
                    waddchnstr(main_window, &current_line[0], window_width);
                    memset(&current_line[0], '\0', window_width);
                }

                // Clear the last line before printing more characters
                for(int i=MAIN_INDEX_OFFSET; i< window_width; i++)    {
                    mvwprintw(main_window, window_height, i, " ");
                }
            }
            
            // Write one character at a time to the bottom line
            else    {
                mvwprintw(main_window, window_height, main_window_index,"%c", data[bytes_written]);
                main_window_index++;
                bytes_written++;
            }
        }
        
        if(main_window_index==window_width) {
            // Reset window index and increment the write index
            main_window_index = MAIN_INDEX_OFFSET; 
            
            for(int i=2; i<= window_height; i++)    {
                // Grab one line at a time, copy it, shift it up, then move
                // down, etc.. until all lines are shifted
                mvwinchnstr(main_window,i, MAIN_INDEX_OFFSET, &current_line[0], window_width);
                wmove(main_window, i-1, MAIN_INDEX_OFFSET);
                waddchnstr(main_window, &current_line[0], window_width);
                memset(&current_line[0], '\0', window_width);
            }

            // Clear the last line before printing more characters
            for(int i=MAIN_INDEX_OFFSET; i< window_width; i++)    {
                mvwprintw(main_window, window_height, i, " ");
            }
        }
    }
    // Move the cursor back to the input window and refresh
    wmove(input_window, 1, input_box_index);
    wrefresh(main_window);
}

bool screen_size_changed(void)  {
    
    int y, x;
    getmaxyx(stdscr, y, x);
    
    if(main_params.max_y != y || main_params.max_x != x)    {
        return true;
    }
 
    return false;
}

static void init_colors(void)  {
    
    // Hypotheticall initializes all combos of the 256 colors FG + BG,
    // but COLOR_PAIR() can only go to 255
    for(int j=-1; j< 256; j++)   {

        for(int i=0; i< 256; i++)   {

            init_extended_pair((i+(256*(j+1))), i, j);
        }
    }
    init_extended_pair(STATUS_NORM, 231, 238);
    init_extended_pair(STATUS_RED,  196, 238);
    init_extended_pair(STATUS_GREEN, 46, 238);
}

static void set_status_colors(void) {

    // Set status bar color pairs
    init_pair(STATUS_NORM, 231, 238);
    init_pair(STATUS_RED, 160, 238);
    init_pair(STATUS_GREEN, 46, 238);
}

static void reset_status_colors(void) {

    // Set status bar color pairs
    init_pair(STATUS_NORM, STATUS_NORM, -1);
    init_pair(STATUS_RED, STATUS_RED, -1);
    init_pair(STATUS_GREEN, STATUS_GREEN, -1);
}

static const char* inttostr(int number)    {
    static char str[16] = {'\0'};
    sprintf(&str[0], "%i", number);
    return str;
}

static void chtype_to_str(int* ch, char* str, int size) {
    for (int i=0; i< size; i++) {
        str[i] = ch[i] & A_CHARTEXT;
    }
}
