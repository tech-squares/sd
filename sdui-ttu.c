/*
 * sdui-ttu.c - helper functions for sdui-tty interface to use the Unix
 * "curses" mechanism.
 * Time-stamp: <93/11/27 11:06:39 gildea>
 * Copyright 1993 Stephen Gildea
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose is hereby granted without fee, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * The authors make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * By Stephen Gildea <gildea@lcs.mit.edu> January 1993
 * Certain editorial comments by William Ackerman <wba@apollo.com> July 1993
 *
 */


/* The "NO_CURSES" compile-time switch will shut off all attempts to use the "curses"
   system service, including the inclusion of the "curses.h" include file.  (Whew!)
   In this case, the program will ignore the "-no_cursor" run-time switch, and will
   always act as though that switch had been given. */

#ifndef NO_CURSES
#include <curses.h>
#else
#include <stdio.h>
#endif
#include <termios.h>   /* We use this stuff if "-no_cursor" was specified. */
#include <unistd.h>    /* This too. */
#include <signal.h>
#include <string.h>

#include "sdui-ttu.h"







extern int elide_blanks;











static int no_line_delete = 0;

#ifdef NO_CURSES
static int no_cursor = 1;
#else
static int no_cursor = 0;
#endif

static int curses_initialized = 0;

static int current_tty_mode = 0;

static void csetmode(int mode)             /* 1 means raw, no echo, one character at a time;
                                                0 means normal. */
{
    static cc_t orig_eof = '\004';
    struct termios term;
    int fd;

    if (mode == current_tty_mode) return;

    fd = fileno(stdin);

    (void) tcgetattr(fd, &term);
    if (mode == 1) {
         orig_eof = term.c_cc[VEOF]; /* VMIN may clobber */
	term.c_cc[VMIN] = 1;	/* 1 char at a time */
	term.c_cc[VTIME] = 0;	/* no time limit on input */
	term.c_lflag &= ~(ICANON|ECHO);
    } else {
	term.c_cc[VEOF] = orig_eof;
	term.c_lflag |= ICANON|ECHO;
    }
    (void) tcsetattr(fd, TCSADRAIN, &term);
    current_tty_mode = mode;
}


/*
 * The main program calls this before doing anything else, so we can
 * supply additional command line arguments.
 * Note: If we are writing a call list, the program will
 * exit before doing anything else with the user interface, but this
 * must be made anyway.
 */   


extern void ttu_process_command_line(int *argcp, char **argv)
{
   int i;
   int argno = 1;

   while (argno < (*argcp)) {
      if (strcmp(argv[argno], "-no_line_delete") == 0)
         no_line_delete = 1;
      else if (strcmp(argv[argno], "-no_cursor") == 0)
         no_cursor = 1;
      else if (strcmp(argv[argno], "-ignoreblanks") == 0)
         elide_blanks = 1;
      else if (strcmp(argv[argno], "-no_graphics") == 0) ;   /* ignore this */
      else if (strcmp(argv[argno], "-lines") == 0 && argno+1 < (*argcp)) {   /* ignore this */
         (*argcp) -= 2;      /* Remove two arguments from the list. */
         for (i=argno+1; i<=(*argcp); i++) argv[i-1] = argv[i+1];
         continue;
      }
      else {
         argno++;
         continue;
      }

      (*argcp)--;      /* Remove this argument from the list. */
      for (i=argno+1; i<=(*argcp); i++) argv[i-1] = argv[i];
   }
}


extern void ttu_display_help(void)
{
   printf("-no_line_delete             do not use the \"line delete\" function for screen management\n");
   printf("-no_cursor                  do not use screen management functions at all\n");
   printf("-ignoreblanks               allow user to omit spaces when typing in\n");
}

extern void ttu_initialize(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      initscr();    /* Initialize "curses". */
      noecho();     /* Don't echo; we will do it ourselves. */
      cbreak();     /* Give us each keystroke; don't wait for entire line. */
      scrollok(stdscr, 1);    /* Permit screen to scroll when we write beyond the bottom.
                                 Of course, that means we lose a line at the top, but
                                 finite-memory dumb terminals such as VT-100's are like that.
                                 A consequence of this lost line is that "erase_last_n" may
                                 get unhappy and just erase the entire screen. */
   
      /* Tell the "curses" system whether we desire to use the line insert/delete
         capabilities of the output device.  The documentation says that, under
         some circumstances the use of this capability can be annoying to the user.
         We let the user decide.
   
         Note that the second argument to "idlok" is defined in the documentation
         as being of type "bool", and must be given as TRUE or FALSE.  (The HP-UX
         documentation specifically says that you must use TRUE or FALSE, not 1 or 0,
         even though an examination of the file /usr/include/curses.h shows these
         constants defined as 1 and 0.)  Apparently the convention, used by
         C programmers throughout the rest of the known universe, that boolean
         conditions are expressed as integers, with zero denoting falsehood and
         any nonzero value denoting truth, wasn't good enough for the designers
         of "curses", so they added a new data type and constants just for us.
         In choosing the names of the type and the constants, they also seem to
         have felt secure in the assumption that *NO ONE ELSE*, either end-user or
         another subsystem designer, would ever conceivably want to use such obscure
         names as "bool", "TRUE", and "FALSE".
         
         Note that, in the spirit of encapsulation, we use a conditional
         expression to turn our ordinary 1/0 condition into the required
         TRUE/FALSE values.  We have no choice, since we aren't really allowed
         to look at the include file.  We rest assured and confident that the
         compiler will not be confused by this and will generate optimal code.
         Right? */
   
      idlok(stdscr, no_line_delete ? FALSE : TRUE);
      keypad(stdscr, TRUE);
      curses_initialized = 1;
   }
#endif
}

extern void ttu_terminate(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      if (curses_initialized) {
         endwin();
      }
   }
   else
      csetmode(0);   /* Restore normal input mode. */
#else
   csetmode(0);   /* Restore normal input mode. */
#endif
}


extern int get_lines_for_more(void)
{
#ifndef NO_CURSES
   if (!no_cursor)
      return LINES;
   else
      return 24;
#else
   return 24;
#endif
}


#ifndef NO_CURSES
static void getyx_correctly(int *y, int *x)
{
   int localy, localx;

/*******************************************************
    WARNING WARNING WARNING WARNING WARNING WARNING

We are about to call the curses service known as "getyx".
This service VIOLATES REFERENTIAL TRANSPARENCY!!!  That
is, it violates the principle that, if you don't see a
variable on the left-hand side of an assignment, or with
an ampersand in front of it, you KNOW FOR CERTAIN that
that variable isn't modified.  The ability to know, simply
by looking at a program, what variables get clobbered, or
what other side-effects might occur, is, of course, an
extremely important aspect of good programming, as any
programmer with a multi-cellular brain can tell you.

In fact, more generally, it is critically important to
be able to tell, straightforwardly and trivially, what a
program will do simply by looking at it.  This property is
known as the "WYSIWYG" ("what you see is what you get")
property of programming languages.  (This is not to be
confused with the WYSIWYG property of editors or the
WYSIWYG property of text formatters, which are also very
desirable properties but unrelated to the WYSIWYG property
of programming languages.)

A common way that programming languages can fail to obey
the WYSIWYG property is to provide a method for cloaking
the true linguistic text of the program by means of a
text transformation mechanism such as a macro processor.
Real programmers, needless to say, NEVER use macro
processors for this purpose, even if doing so might make
the program more "convenient" to write.  (The purpose
of a program is not to be convenient to write.  The purpose
is to behave correctly, and obeying the WYSIWYG property
is the single most important thing that a program can do
in furtherance of this goal.)

The manner in which the "getyx" service violates the
WYSIWYG property is, not surprisingly, through a macro.
Someone thought that typing two ampersands was too much
trouble, and that he or she could make the use of this
service more "convenient" by making it a macro that clobbers
the variables named in the macro call.

Furthermore, even the convention that macros will always
be in upper case, a convention that most C programmers
adhere to in recognition of the extremely dangerous
nature of macros, isn't adhered to in this case.

Some operating system vendors' manuals actually describe
this situation as though they think it is a good thing.

For example, the HP-UX manual says

     "This is a macro, so no ampersand is necessary"

        [What does "not necessary" mean in the context
        of programming system documentation?  Is it
        optional?  Is it forbidden?  Furthermore, what
        is the significance of the "so" conjunction in
        that sentence?  Does it logically follow from
        the fact that something is a macro that ampersands
        are never necessary?  Am I missing something?]

and the AT&T System V Release 4 manual says

     "Note that all of these interfaces [getyx and
     several others] are macros and that "&" is not
     necessary before the variables y and x"

and the SunOS 4.1 manual says

     "This is implemented as a macro, so no "&" is
     necessary before the variables"

whereas the correct way to document this would have been

     "This is a macro, and, to save you the two seconds
     that it would take to type two ampersands, we have
     violated referential transparency in it.  To further
     confuse you, we have put it in lower case.  After
     you finish rewriting whatever static analysis tools
     you use at your place of employment so that they
     won't be fooled by the apparent right-hand-side
     usage of what might appear to be an uninitialized
     variable, please make good use of whatever time
     remains of the two seconds that we have saved you."

*******************************************************/


   /* WARNING!!!! LOCALY AND LOCALX MODIFIED BY THIS STATEMENT!!!!! */
                                   /* DANGER BELOW!! */
   /* DANGER TO THE RIGHT!! */ getyx(stdscr, localy, localx) /* DANGER TO THE LEFT!! */
                                   /* DANGER ABOVE!! */

   /* Could we have said just "getyx(stdscr, *y, *x)" ????
      Probably, but who knows what's going in inside that macro.
      We'd better play safe. */

   /* By the way, do we need a semicolon here?  (Note that we
      didn't provide one.)  Exactly what form does this macro
      take?  Does it have a semicolon at the end?  Does it use
      a semicolon between its two assignments, or a comma, or
      what?  No fair peeking at the definition!  Does this
      macro invocation yield a value from its assignment?
      Is it a free-standing statement?  What is it? */

   /* The suspense is killing me!  Here comes the semicolon. */
   ;

   *y = localy;   
   *x = localx;   
}
#endif


extern void clear_line(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      int y, x;

      getyx_correctly(&y, &x);
      move(y, 0);
      clrtoeol();
      refresh();
   }
   else {
      /* We may be on a printing terminal, so we can't erase anything.  Just
         print "XXX" at the end of the line that we want to erase, and start
         a new line.  This will happen if the user types "C-U", or after any
         "--More--" line.  We can't make the "--More--" line go away completely,
         leaving a seamless transcript.  This is the best we can do. */
      printf(" XXX\n");
   }
#else
      printf(" XXX\n");
#endif
}

extern void rubout(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      int y, x;
      getyx_correctly(&y, &x);
      if (x > 0) move(y, x-1);
      clrtoeol();
      refresh();
   }
   else {
      printf("\b \b");    /* We hope that this works. */
   }
#else
   printf("\b \b");    /* We hope that this works. */
#endif
}

extern void erase_last_n(int n)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      int y, x;

      getyx_correctly(&y, &x);
      if (y >= n)
         move(y-n, 0);
      else
         /* We would have to erase more than is on the screen.
            This normally means that stuff scrolled off the top and
            got lost. Just home the cursor and clear the whole screen.
            User will get updated text only.  If that isn't
            satisfactory, user can ask for a refresh.  Of course, if
            the sequence is too long to fit on the screen, stuff will
            scroll off the top during the refresh, but there's only
            so much we can do. */
         move(0, 0); 
   
      clrtobot();
      refresh();
   }
#endif
}

extern void put_line(char the_line[])
{
#ifndef NO_CURSES
   if (!no_cursor) {
      addstr(the_line);
      refresh();
   }
   else {
      (void) fputs(the_line, stdout);
   }
#else
   (void) fputs(the_line, stdout);
#endif
}

extern void put_char(int c)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      addch(c);
      refresh();
   }
   else {
      (void) putchar(c);
   }
#else
   (void) putchar(c);
#endif
}


extern int get_char(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      int c = getch();      /* A "curses" call. */
      /* Handle function keys. */
      return c >= 0410 ? c-0410+128 : c;
   }
   else {
      csetmode(1);         /* Raw, no echo, single-character mode. */
      return getchar();    /* A "stdio" call. */
   }
#else
   csetmode(1);         /* Raw, no echo, single-character mode. */
   return getchar();    /* A "stdio" call. */
#endif
}


extern void get_string(char *dest)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      echo();
      getstr(dest);
      noecho();
   }
   else {
      csetmode(0);         /* Regular full-line mode with system echo. */
      gets(dest);
   }
#else
   csetmode(0);         /* Regular full-line mode with system echo. */
   gets(dest);
#endif
}


extern void bell(void)
{
#ifndef NO_CURSES
   if (!no_cursor) {
      beep();
   }
   else {
      (void) putchar('\007');
   }
#else
   (void) putchar('\007');
#endif
}

/*
 * signal handling
 */

/* ARGSUSED */
static void stop_handler(int n)
{
/*
    if (current_tty_mode != 0) {
	csetmode(0);
	current_tty_mode = 1;
    }
    signal(SIGTSTP, SIG_DFL);
    kill(0, SIGTSTP);
*/
}

/* ARGSUSED */
static void cont_handler(int n)
{
    refresh_input();
/*
    if (current_tty_mode != 0) {
	csetmode(current_tty_mode);
    }
    initialize_signal_handlers();
*/
}

void initialize_signal_handlers(void)
{
    signal(SIGTSTP, stop_handler);
    signal(SIGCONT, cont_handler);
}
