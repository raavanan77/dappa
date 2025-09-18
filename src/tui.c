#include <ncurses.h>

int main(){
  initscr();
  printw("Hello Vi!");
  refresh();
  getch();
  endwin();

  return 0;
}
