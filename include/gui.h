/*
 *  gui.h
 *  Definitions to handle mbuddy GUI
 *
 *  Created by zuidec on 
 */

#ifndef GUI_H
#define GUI_H    // BEGIN GUI_H

/*
 *  Includes
 */

#include <ncurses.h>


/*
 *  Defines
 */

#define INPUT_BOX_HEIGHT    (3)
#define COLUMN_OFFSET       (2)
#define NO_COLUMN_OFFSET    (0)
#define STATUS_BAR_HEIGHT   (2)
#define MAX_ARGS            (5)


/*
 *  Structs. unions, etc..
 */

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


/*
 *  Function prototypes
 */

WINDOW* init_window(WIN_PARAMS* params);
void init_window_params(WIN_PARAMS* main_params, WIN_PARAMS* input_params, WIN_PARAMS* status_params);
void update_status_bar(status_bar_t* status_bar, WIN_PARAMS* status_params);
void update_input_box(WINDOW* input_box, char* data);
int get_input(char* buffer);

#endif  // END GUI_H

