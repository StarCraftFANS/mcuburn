#include <errno.h>
#include <sys/poll.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <ncurses.h>
#include <termios.h>
#include <unistd.h>
#include <proto.h>
#include <commport.h>
#include <mcutarget.h>
#define DEVNAME "/dev/ttyS0"
#define GETCH_BLOCK(w)	 ({ timeout ((w) ? -1 : 0); })
#define MAX(a,b) (a)>(b)?(a):(b)
#define BUTTON_READ   4
#define BUTTON_TARGET 4
#define BUTTON_WRITE  7
#define BUTTON_QUIT  10
#define RBUTTON_VERIFY 7
#define RBUTTON_LOCK  10
#define MINX  18
#define MINY  10
static char *serdev = DEVNAME;
#define ESCKEY  0x1b
#define SER_BLACK	(COLOR_BLACK)
#define SER_DARK_RED  (COLOR_RED)
#define SER_RED       (COLOR_RED | A_BOLD)
#define SER_GREEN     (COLOR_GREEN | A_BOLD)
#define SER_ORANGE    (COLOR_YELLOW)
#define SER_YELLOW    (COLOR_YELLOW | A_BOLD)
#define SER_MARIN     (COLOR_BLUE)
#define SER_BLUE      (COLOR_BLUE | A_BOLD)
#define SER_MAGENTA   (COLOR_MAGENTA)
#define SER_DARK_CYAN (COLOR_CYAN)
#define SER_CYAN      (COLOR_CYAN | A_BOLD)
#define SER_GREY      (COLOR_WHITE)
#define SER_GRAY      (SER_GREY)
#define SER_WHITE     (COLOR_WHITE | A_BOLD)
#define ROWS     4
#define COLUMNS  80
#define DISLEN (ROWS*COLUMNS)
enum {
  DC_DEFAULT,
  DC_BACK,
  DC_TEXT,
  DC_PROMPT,
  DC_CBAR_FRAME,
  DC_CBAR_LOCK,
  DC_CBAR_UNLOCK,
  DC_CBAR_VERIFY,
  DC_CBAR_NOVERIFY,
  DC_CBAR_EMPTY,
  DC_CBAR_LABEL,
  DC_CBAR_FOCUS_LABEL,
  DC_FOCUS,
  DC_ANY_1,
  DC_ANY_2,
  DC_ANY_3,
  DC_ANY_4,
  DC_LAST
};
static int dc_fg[DC_LAST] = { 0 };
static int dc_attrib[DC_LAST] = { 0 };
static int dc_char[DC_LAST] = { 0 };
static int ser_do_color = 1;
static const char *prompt="mcuProgrammer\0";
char disbuf[DISLEN];
WINDOW *swin,*pwin;






static int lrcorner,maxx,maxy;
static int state=0;
static int ser_dc(int);
static WINDOW *open_display();
static int ser_resize (void)
{
  struct winsize winsz = { 0, };
  if (ioctl (fileno (stdout), TIOCGWINSZ, &winsz) >= 0 &&
      winsz.ws_row && winsz.ws_col)
    {
      keypad (pwin, FALSE);
      leaveok (pwin, FALSE);

      endwin ();
      
      maxx = MAX (2, winsz.ws_col);
      maxy = MAX (2, winsz.ws_row);
      
      /* humpf, i don't get it, if only the number of rows change,
       * ncurses will segfault shortly after (could trigger that with mc as well).
       */
      resizeterm (maxy + 1, maxx + 1);
      resizeterm (maxy, maxx);
      
      open_display ();
      
      if (maxx < MINX ||
	  maxy < MINY)
	beep (); // mixer_abort (ERR_WINSIZE, "");
    }
}


static int quitkeypressed(int *keyp)
{
int finishpoll=0;
int count=1;        
struct pollfd *fds=NULL;
fds=(struct pollfd *)calloc(count+1,sizeof(struct pollfd));
if(fds == NULL)
{
*keyp=getch();
if(*keyp == 0x1b) 
        return 1;
else 
         return 0;
}
fds->fd=fileno(stdin);
fds->events=POLLIN;
finishpoll=poll(fds,count+1,-1);
if(finishpoll < 0 || errno == EINTR)
        finishpoll=0;
if(finishpoll != 0)
{
    if (fds->revents & POLLIN) {
      *keyp = getch ();
      finishpoll--;
    }
  } else {
    *keyp = 0;
  }

  finishpoll=0;
  free(fds);
  if(*keyp == 0x1b)
  return 1;
  return 0;
}
static int ser_dc(int n)
{
  if (ser_do_color)
    attrset (COLOR_PAIR (n) | (dc_fg[n] & 0xfffffff0));
  else
    attrset (dc_attrib[n]);
  
  return dc_char[n];
}


static int drawframe(char *ch)
{
  int i;
  ser_dc(DC_PROMPT);
  mvaddstr(1,2,prompt);
  ser_dc(DC_TEXT);
  mvaddstr(BUTTON_READ,1,"READ ");
  mvaddstr(BUTTON_WRITE,1,"WRITE");
  mvaddstr(BUTTON_QUIT,1,"QUIT ");
  mvaddstr(BUTTON_TARGET,9,"TARGET");
  mvaddstr(RBUTTON_VERIFY,9,"VERIFY");
  mvaddstr(RBUTTON_LOCK,9,"LOCK  ");
  mvaddstr(13,10,ch);
  ser_dc(DC_CBAR_VERIFY);
  mvaddch(RBUTTON_VERIFY,15,'X');
  mvaddch(RBUTTON_LOCK,15,' ');
}
static void ser_clear(int full_redraw)
{
        int x,y;
   int f=full_redraw?0:1;        
   ser_dc(DC_PROMPT);
  if (full_redraw)
    clearok (pwin, TRUE);

  /* buggy ncurses doesn't really write spaces with the specified
   * color into the screen on clear () or erase ()
   */
  for (x = f; x < maxx - f; x++)
    for (y = f; y < maxy - f; y++)
      mvaddch (y, x, ' ');
}
ser_init_dc (int c,
	       int n,
	       int f,
	       int b,
	       int a)
{
  dc_fg[n] = f;
  dc_attrib[n] = a;
  dc_char[n] = c;
  if (n > 0)
    init_pair (n, dc_fg[n] & 0xf, b & 0x0f);
}
void ser_init_draw_contexts (void)
{
  start_color ();
  
  ser_init_dc ('.', DC_BACK, SER_WHITE, SER_BLACK, A_NORMAL);
  ser_init_dc ('.', DC_TEXT, SER_YELLOW, SER_BLUE, A_BOLD);
  ser_init_dc ('.', DC_PROMPT, SER_WHITE, SER_BLACK, A_BOLD|A_UNDERLINE);
  ser_init_dc ('.', DC_CBAR_FRAME, SER_CYAN, SER_BLACK, A_BOLD);
  ser_init_dc ('L', DC_CBAR_LOCK, SER_DARK_CYAN, SER_BLACK, A_NORMAL);
  ser_init_dc ('U', DC_CBAR_UNLOCK, SER_WHITE, SER_GREEN, A_BOLD);
  ser_init_dc ('v', DC_CBAR_VERIFY, SER_DARK_RED, SER_BLUE, A_BOLD);
  ser_init_dc ('-', DC_CBAR_NOVERIFY, SER_GRAY, SER_BLACK, A_NORMAL);
  ser_init_dc (' ', DC_CBAR_EMPTY, SER_GRAY, SER_BLACK, A_DIM);
  ser_init_dc ('.', DC_CBAR_LABEL, SER_WHITE, SER_BLUE, A_REVERSE | A_BOLD);
  ser_init_dc ('.', DC_CBAR_FOCUS_LABEL, SER_RED, SER_BLUE, A_REVERSE | A_BOLD);
  ser_init_dc ('.', DC_FOCUS, SER_RED, SER_BLACK, A_BOLD);
  ser_init_dc (ACS_CKBOARD, DC_ANY_1, SER_WHITE, SER_WHITE, A_BOLD);
  ser_init_dc (ACS_CKBOARD, DC_ANY_2, SER_GREEN, SER_GREEN, A_BOLD);
  ser_init_dc (ACS_CKBOARD, DC_ANY_3, SER_RED, SER_RED, A_BOLD);
  ser_init_dc ('.', DC_ANY_4, SER_WHITE, SER_BLUE, A_BOLD);
}


static WINDOW *
open_display ()
{
        struct winsize winsz;
  setlocale(LC_CTYPE,"");       
  pwin = initscr ();
  curs_set(0);
  lrcorner=tigetflag("xenl") != 1 && tigetflag("am") != 1;
    ser_do_color = has_colors ();
  ser_init_draw_contexts ();
  cbreak ();
  noecho ();
  nonl ();
  leaveok(pwin,TRUE);
  //intrflush (pwin, FALSE);
  keypad (pwin, TRUE);
  GETCH_BLOCK(1);
  getmaxyx(pwin,maxx,maxy);
if (ioctl (fileno (stdout), TIOCGWINSZ, &winsz) >= 0 &&
    winsz.ws_row && winsz.ws_col)
    {
   maxx = MAX (2, winsz.ws_col);
   maxy = MAX (2, winsz.ws_row);
   }
  ser_clear(TRUE);
  return pwin;
}

int
main (int argc, char **argv)
{
          mcudev_t  *mcu;         
          int key=0;
        int index=0;
  int getdata;
  int fd = -1;
  char ch='?';
  mcu=(mcudev_t *)malloc(sizeof(struct mcudev));
  mcu->image=(image_t *)malloc(sizeof(struct image_s));
  memset(mcu->image->Filename,0,255);
  strcpy(mcu->image->Filename,argv[1]);
  fd = serial_open (serdev);
  if (fd < -1)
    {
      fprintf (stderr, "Cannot open device\n");
      return fd;
    }
  if(serial_setup(fd)< 0)
               fprintf(stderr,"Error in configuring serial device\n");
  open_display ();
  memset(disbuf,0,DISLEN);
  do
    {
      drawframe(disbuf);      
      refresh();
      sendcommand(fd,key,mcu);      
      processdata(disbuf,DISLEN);
      drawframe(disbuf);      
      refresh();
    }while(!quitkeypressed(&key));
  endwin ();
  serial_close (fd);
  free(mcu->image);
  mcu->image=NULL;  
  free(mcu);
  return 0;
}


void 
progressbar()
{

}
