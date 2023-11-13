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

#include <ncursesw/ncurses.h>


/*
 *  Defines
 */

#define INPUT_BOX_HEIGHT    (3)
#define COLUMN_OFFSET       (2)
#define NO_COLUMN_OFFSET    (0)
#define WINDOW_PADDING      (4)
#define MAIN_INDEX_OFFSET   (2)
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
    int start_y, start_x;
    int max_y, max_x;
} WIN_PARAMS;

typedef struct status_bar_t    {
    char* port;
    int baudrate;
    char* exit_msg;
    bool is_connected;
} status_bar_t;


/*
 *  Function prototypes
 */

void init_gui(void);
void close_gui(void);
void update_status_bar(status_bar_t* status_bar);
void update_input_box(char* data);
void move_input_cursor(int distance);
int get_input(char* buffer);
int get_input_box_width(void);
int get_input_box_char(void);
bool new_input_box_char(void);
bool screen_size_changed(void);
void update_main_window(const char* data, int size);
void clear_main(void);

#endif  // END GUI_H

