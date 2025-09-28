#include <ncurses.h>
#include <signal.h>

typedef char* string;
static volatile int stop = 1;

void exit_curse(){
  stop = 0;
}

void text_inputer(){
  char c;
  while(1){
    c = getch();
    if(c==27){
      noecho();
      break;
    }
  }
}

int start_y = 0 , start_x = 0;
int chat_cursor = 2;
int main(){
  initscr();
  cbreak();
  keypad(stdscr, true);
  noecho();
  getyx(stdscr, start_y, start_x);
  WINDOW* chat_list = newwin(LINES, (int)COLS * 20 / 100, start_y, start_x);
  WINDOW* chat_search = newwin((int)LINES * 7 / 100, ((int)COLS * 20 / 100) - 2,  (int)LINES * 5 / 100 , (int)COLS * 1 / 100); //  Don't think I can finish this feature anytime soon
  WINDOW* text_input_box = newwin((int)LINES * 7 / 100, ((int)COLS * 80 / 100),  (int)LINES * 95 / 100 , (int)COLS * 20 / 100);
  refresh();
  box(chat_list, 0, 0);
  mvwprintw(chat_list, 1, 1, "Chat");
  wrefresh(chat_list);
  refresh();
  box(text_input_box, 0, 0);
  wrefresh(text_input_box);
  //signal(SIGINT, exit_curse);
  while(stop){
    int mode = getch();
    switch (mode){
      case 'f':
        echo();
        box(chat_search, 0, 0);
        wrefresh(chat_search);
        char c;
        move(3, chat_cursor);
        text_inputer();
        break;
      
      case 't':
        echo();
        move(((int)LINES * 95 / 100) + 1 , ((int)COLS * 20 / 100)+1);
        text_inputer();
        break;
      
      case KEY_RESIZE:
        getyx(stdscr, start_y, start_x);
        refresh();
        wresize(chat_list, start_y, start_x);
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
  endwin();
  return 0;
}
