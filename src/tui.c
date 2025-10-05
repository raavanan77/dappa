#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

typedef char* string;
static volatile int stop = 1;

// Text input function
int text_inputer(string data){
    char c;
    while(stop){
        c = getch();
        if( c == 27 || c == '\n'){
            noecho();
            break;
        }
        //else{
        //    strcpy(data, &c); //, sizeof(char));
        //}
    }
    return 0;
}

// Block cursor (insert mode style)
void cursor_block() {
        printf("\033[2 q");
        fflush(stdout);
}

// Underline cursor (normal mode style)
void cursor_underline() {
        printf("\033[4 q");
        fflush(stdout);
}

// Beam/line cursor
void cursor_beam() {
        printf("\033[6 q");
        fflush(stdout);
}

void exit_curse(){
    stop = 0;
    cursor_block();
    //exit(1);
}

int start_y = 0 , start_x = 0;
int chat_cursor = 2;
int main(){
    initscr();
    cbreak();
    keypad(stdscr, true);
    noecho();
    getyx(stdscr, start_y, start_x);
    int input_cursor[2] = {((int)LINES * 95 / 100) , ((int)COLS * 20 / 100) + 1};
    WINDOW* chat_list = newwin(LINES - 1, (int)COLS * 20 / 100, start_y, start_x);
    WINDOW* chat_search = newwin((int)LINES * 7 / 100 - 1, ((int)COLS * 20 / 100) - 2,    (int)LINES * 5 / 100 - 1, (int)COLS * 1 / 100); //    Don't think I can finish this feature anytime soon
    WINDOW* text_input_box = newwin((int)LINES * 7 / 100 , ((int)COLS * 80 / 100),    (int)LINES * 93 / 100 , (int)COLS * 20 / 100);
    WINDOW* command = newwin(((int)LINES * 5 / 100) + 2, COLS, LINES - 1, 0);
    refresh();
    box(chat_list, 0, 0);
    mvwprintw(chat_list, 1, 1, "Chat");
    wrefresh(chat_list);
    refresh();
    box(text_input_box, 0, 0);
    wrefresh(text_input_box);
    while(stop){
        signal(SIGINT, exit_curse);
        move(input_cursor[0], input_cursor[1]);
        int mode = getch();
        switch (mode){
            case 'f':
                string searchinput;
                echo();
                box(chat_search, 0, 0);
                wrefresh(chat_search);
                char c;
                move(2, chat_cursor);
                text_inputer(searchinput);
                noecho();
                break;
            case 't':
                string msgdata;
                echo();
                cursor_beam();
                move(input_cursor[0], input_cursor[1]);
                int rc = text_inputer(msgdata);
                if(rc == 0){
                    wclear(text_input_box);
                    box(text_input_box, 0, 0);
                    wrefresh(text_input_box);
                }
                cursor_block();
                noecho();
                break;
            case ':':
                string cmdfromuser;
                echo();
                move(LINES - 1, 0);
                wrefresh(command);
                int rcw = text_inputer(cmdfromuser);
                if(rcw == 0){
                    wclear(command);
                    box(text_input_box, 0, 0);
                    wrefresh(command);
                    wrefresh(text_input_box);
                }
                noecho();
                break;
            case KEY_RESIZE:
                getyx(stdscr, start_y, start_x);
                refresh();
                wresize(chat_list, LINES, COLS);
                wrefresh(chat_list);
                wrefresh(chat_search);
                wrefresh(text_input_box);
                break;
            case 27:
                stop = 0;
                break;
            default:
                noecho();
                break;
        }
        if(stop == 0) break;
    }
    cursor_block();
    endwin();
    return 0;
}
