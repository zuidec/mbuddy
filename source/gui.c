/*
 *	gui.c
 *	Function to create and update mbuddy GUI
 *
 *	 Created by zuidec on 11/11/23
 */

#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "gui.h"

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

