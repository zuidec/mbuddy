/*
 *	gui.c
 *	Function to create and update mbuddy GUI
 *
 *	 Created by zuidec on 11/11/23
 */

#include <bits/stdint-uintn.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "gui.h"

#define BLACK       (0)
#define BLUE        (1)
#define GREEN       (2)
#define CYAN        (3)
//#define RED         (4)
#define MAGENTA     (5)
#define BROWN       (6)
#define WHITE       (7)
#define B_BLACK     (8)
#define B_BLUE      (9)
#define B_GREEN     (10)
#define B_CYAN      (11)
#define B_RED       (12)
#define B_MAGENTA   (13)
#define YELLOW      (14)
#define B_WHITE     (15)

WIN_PARAMS main_params, input_params, status_params;
WINDOW* main_window;
WINDOW* input_window;
WINDOW* status_window; 

static WINDOW* init_window(WIN_PARAMS* params);
static void init_window_params(void);

static void init_window_params(void)  {
    int max_y, max_x; 
    max_y = getmaxy(stdscr);
    max_x = getmaxx(stdscr);

    main_params.max_y      = max_y;
    main_params.max_x      = max_x;
    main_params.width      = max_x- COLUMN_OFFSET;
    main_params.height     = (max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT - 1);
    main_params.x_pos      = COLUMN_OFFSET;
    main_params.y_pos      = 0;

    input_params.max_y     = max_y;
    input_params.max_x     = max_x;
    input_params.width     = max_x - COLUMN_OFFSET;
    input_params.height    = INPUT_BOX_HEIGHT;
    input_params.x_pos     = COLUMN_OFFSET;
    input_params.y_pos     = max_y - INPUT_BOX_HEIGHT - STATUS_BAR_HEIGHT -1;

    status_params.max_y    = max_y;
    status_params.max_x    = max_x;
    status_params.width    = max_x -1;
    status_params.height   = STATUS_BAR_HEIGHT;
    status_params.x_pos    = NO_COLUMN_OFFSET; 
    status_params.y_pos    = max_y - STATUS_BAR_HEIGHT - 1;
}

static WINDOW* init_window(WIN_PARAMS* params)   {
    WINDOW* window;
    window = newwin(params->height, params->width, params->y_pos, params->x_pos);
    box( window, 0, 0); 
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

    wmove(main_window, 1, 1);
    for(int i=0; i<256; i++)    {
        wattron(main_window, COLOR_PAIR(i));
        wprintw(main_window, "%i ", i);
        wattron(main_window, COLOR_PAIR(i));
        wrefresh(main_window);
    }

    // Allow keypad in the input window
    keypad(input_window, true);

}

void update_status_bar(status_bar_t* status_bar) {

    char msg[64] = {' '};
    
    // Clear then reprint data
    wclear(status_window);
    mvwprintw(status_window, 1, COLUMN_OFFSET, "Press F1 to exit");

    if(status_bar->is_connected)    {
        wattron(status_window, COLOR_PAIR(46));
        sprintf(&msg[0], "Connection OK Port: %s  Baudrate: %i", status_bar->port, status_bar->baudrate);
        mvwprintw(status_window, 1, (status_params.width - strlen(msg)), "%s", &msg[0]);
        wattroff(status_window, COLOR_PAIR(46));
    }
    else    {
        wattron(status_window, COLOR_PAIR(160));
        sprintf(&msg[0], "Connection FAIL Port: %s  Baudrate: %i", status_bar->port, status_bar->baudrate);
        mvwprintw(status_window, 1, (status_params.width - strlen(msg)), "%s", &msg[0]);
        wattroff(status_window, COLOR_PAIR(160));
    }
    wrefresh(status_window);
}

void update_input_box(char* data)   {
    
    werase(input_window);
    box(input_window, 0, 0); 
    mvwprintw(input_window, 1, COLUMN_OFFSET, "> %s", data);
}

void move_input_cursor(int distance)    {
    int y, x;
    getyx(input_window, y, x);
    wmove(input_window, y, x+distance);
}

int get_input_box_width(void)  {
    return input_params.width-1-(COLUMN_OFFSET * 3);
}

int get_input_box_char(void)   {
    return wgetch(input_window);
}
bool screen_size_changed(void)  {
    
    int y, x;
    getmaxyx(stdscr, y, x);
    
    if(main_params.max_y != y || main_params.max_x != x)    {
        return true;
    }
 
    return false;
}

void init_colors(void)  {
 /*   init_pair(BLACK ,COLOR_BLACK , -1);
    init_pair(BLUE , COLOR_BLUE, -1);
    init_pair(GREEN , COLOR_GREEN, -1);
    init_pair(CYAN , COLOR_CYAN, -1);
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(MAGENTA , COLOR_MAGENTA, -1);
    init_pair(WHITE , COLOR_WHITE, -1);
    init_pair(YELLOW , COLOR_YELLOW, -1);*/

    for(int i=0; i< 256; i++)   {
        init_pair(i, i, -1);
    }
}
