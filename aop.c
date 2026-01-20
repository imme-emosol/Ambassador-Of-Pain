/* Free Software under the terms of GNU GPL */
/* Raffael Himmelreich     <raffi@raffi.at> */
/* Clifford Wolf     <clifford@clifford.at> */
/* immeëmosol    <p+6199ab0d6c@willfris.nl> */

#include <curses.h>
#include <unistd.h>
#include <string.h>
#define MSG_SIZE (55)

int main(int argc, char *argv[]) { enum { INT_CRASH=-1, INT_NEWLVL=-2
    , INT_ERREAD=-10, INT_FINISH=-11, INT_SHOWUS=-12, INT_DIDDIE=-13, INT_DOSTOP=-15, };
  int state = INT_NEWLVL, input, loc = 0, level = 1, life = 5, score = 0, dir = 0, levels = argc >= 2 ? argc - 1 : 11;
  const int cols = 81, locations = cols * 25;
  static const struct { int key, delta, cost, state, level; } keys[] = { {0,0,0,0,0}
    , {' ',0,0,0}, {KEY_ENTER,0,0,0}, {'q',0,0,-15,0}, {KEY_BACKSPACE,0,0,INT_DOSTOP,0}
    , {'p',0,0,-2,-1}, {KEY_PPAGE,0,0,-2,-1}, {'n',0,0,-2,1}, {KEY_NPAGE,0,0,-2,1}
    , {'k',-cols,-77,0,0}, {KEY_UP,-cols,-77,0,0}, {'j',cols,-77,0,0}, {KEY_DOWN,cols,-77,0,0}
    , {'h',-1,-77,0,0}, {KEY_LEFT,-1,-77,0,0}, {'l',1,-77,0,0}, {KEY_RIGHT,1,-77,0,0}, };
  struct { char curr, prev; int x, y; } map[locations] = {};
  struct { char * f, a[2][MSG_SIZE]; } msg[] = { {"Can't open level file '%s'.\n",{"",}}
    , {"Well done! (%s points)\n",{"",}}, {"Usage: %s [aop-level-01.txt [..] ]\n",{"",}}
    , {"Sucker! (%s points)\n",{"",}}, {"Lifes: %d, Points: %d ",{"",}}, {"Bye. (%s points)\n",{"",}}, };
  snprintf(msg[2].a[0], MSG_SIZE, argv[0]); FILE * file;
  if (1 < argc && '-' == *argv[1]) state = INT_SHOWUS;
  initscr(), cbreak(), noecho(), curs_set(0), keypad(stdscr, TRUE), start_color();
  nodelay(stdscr, 1), init_pair(1, 255 < COLORS ? 226 : COLOR_RED, COLOR_BLACK);
  while (refresh(), usleep(keys[dir].delta < 5 ? 50000 : 100000), INT_NEWLVL <= state) {
    if (INT_CRASH >= state) {
      if (INT_NEWLVL >= state) {
        if (1 > level || levels < level) level = 1;
        if (argc >= 2) snprintf(msg[0].a[0], MSG_SIZE, argv[level]);
        else snprintf(msg[0].a[0], MSG_SIZE, "/usr/local/share/aop/aop-level-%02d.txt", level);
        if (!(file = fopen(msg[0].a[0], "r"))) {state = INT_ERREAD; break;}
        memset(map, ' ', locations);
        for (loc = 0; loc < locations && 0 < fread(&map[loc].curr, 1, 1, file); loc++) {
          map[loc].x = loc % cols, map[loc].y = loc / cols; }
        fclose(file), score += 700000 + level * 373737; }
      for (loc = locations; 0 < --loc && '%' != map[loc].curr;);
      if (0 >= loc) loc = 3 + 2 * cols;
      for (state = locations - 1; mvaddch(map[state].y, map[state].x, map[state].curr), 0 < --state;);
      dir = 0;    }
    for (state = getch(), input = sizeof(keys)/sizeof(keys[0]); 0 < input && state != keys[input].key; input--);
    state = 0, dir = 0 != input ? input : dir;
    mvaddch(map[loc].y, map[loc].x, map[loc].prev ? map[loc].prev : ' ');
    switch (map[loc += keys[dir].delta].curr) {
    case '0': life++, map[loc].curr = map[loc].prev, map[loc].prev = '\0';
    case '%':
    case ' ':
      score += keys[input].cost + keys[dir].delta < 5 ? INT_CRASH : INT_NEWLVL;
      level += keys[dir].level ? keys[dir].level : 0;
      state = keys[dir].state ? keys[dir].state : state;
      attron(COLOR_PAIR(1)), mvaddch(map[loc].y, map[loc].x, 'O');
      mvprintw(0, 0, msg[4].f, life, score), attroff(COLOR_PAIR(1));
      break;
    case '@': state = levels <= level++ ? INT_FINISH: INT_NEWLVL;
      break;
    default: loc -= keys[dir].delta, state = INT_CRASH, map[loc].prev = map[loc].curr, map[loc].curr = '0';
      for (state = 3; INT_CRASH < --state; mvaddch(map[loc].y, map[loc].x, state % 2 ? map[loc].curr : ' '), refresh(), usleep(125000));
      if (0 >= --life) state = INT_DIDDIE; }}
  if ((state = -1 * (10 + state)) % 2) snprintf(msg[state].a[0], MSG_SIZE, "%d", score);
  return endwin(), printf(msg[state].f, msg[state].a[0]), 1+state % 2; }

/* Yup - This are 64 lines of C code.  ;-) */
