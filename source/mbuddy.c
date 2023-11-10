#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define INPUT_BOX_HEIGHT    (3)
#define COLUMN_OFFSET       (2)
#define NO_COLUMN_OFFSET    (0)
#define STATUS_BAR_HEIGHT   (2)



typedef struct _win_border_struct   {
    chtype ls, rs, ts, bs,  // Character to use on sides: left side, right side, top side, bottom side
           tl, tr, bl, br;  // Character to use on corners: top left, top right, bottom left, bottom right
} WIN_BORDER;

typedef struct WIN_PARAMS   {
    int height, width;
    int x_pos, y_pos;
} WIN_PARAMS;

typedef struct serial_t {
    int baudrate;
    char* device;
} serial_t;

WINDOW* init_window(WIN_PARAMS* params);
void init_window_params(WIN_PARAMS* main_params, WIN_PARAMS* input_params, WIN_PARAMS* status_params);
int get_input(char* buffer);

int main(int argc, char *argv[]) {
    
    WIN_PARAMS main_params, input_params, status_params;
    int ch; 

    initscr();
    cbreak();
    noecho();
    //start_color();

    init_window_params(&main_params, &input_params,&status_params);
    WINDOW* main_window = init_window(&main_params);
    WINDOW* input_window = init_window(&input_params);
    WINDOW* status_bar = init_window(&status_params);
    
    keypad(input_window, true);

    wclear(status_bar);
    mvwprintw(status_bar, 1, COLUMN_OFFSET, "Press F1 to exit");
    wrefresh(status_bar);

    for(int i =0; i< argc; i++) {
        mvwprintw(main_window,i+2, 2, "Argument %i is %s", i, argv[i]);
        wrefresh(main_window);
    }
    
    mvwprintw(input_window, 1, COLUMN_OFFSET, "> ");
    wrefresh(input_window);

    while((ch = wgetch(input_window)) != KEY_F(1))   {
        wprintw(input_window, "%c", ch);
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

int get_input(char* buffer) {
    int bytes_read = 0;

    return bytes_read;
}
