/*
 * sdui-ttu.h - header file for sdui-ttu.c
 * Time-stamp: <93/06/25 19:39:06 gildea>
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
 */

/* This is a variable in "sdutil.c" that we can set if we want to
   draw pretty graphical characters for the arrows that show
   which way a person is facing.  It must point to a string in stable storage. */
extern char *ui_directions;

/* Print the help message appropriate for the switches that we support. */
extern void ttu_display_help();

/* Initialize this package. */
extern void ttu_initialize();

/* Terminate this package. */
extern void ttu_terminate();

/* Get number of lines to use for "more" processing.  This number is
   not used for any other purpose -- the rest of the program is not concerned
   with the "screen" size. */

extern int get_lines_for_more();

/* Clear the current line, leave cursor at left edge. */
extern void clear_line();

/* Backspace the cursor and clear the rest of the line, presumably
   erasing the last character. */
extern void rubout();

/* Move cursor up "n" lines and then clear rest of screen. */
extern void erase_last_n(int n);

/* Write a line.  The text may or may not have a newline at the end. */
/* This may or may not be after a prompt and/or echoed user input. */
extern void put_line(char the_line[]);

/* Write a single character on the current output line. */
extern void put_char(int c);
