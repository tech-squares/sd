/* 
 * sdui-tty.c - SD TTY User Interface
 * Originally for Macintosh.  Unix version by gildea.
 * Time-stamp: <94/11/10 17:17:53 wba>
 * Copyright (c) 1990-1994 Stephen Gildea, William B. Ackerman, and
 *   Alan Snyder
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose is hereby granted without fee, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * The authors make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * By Alan Snyder, December 1992.
 *
 * This version of the Sd UI does completing reads instead of menus.
 * Type '?' to see possibilities.
 *    At the "More" prompt, type SPC to see the next page, DEL to stop.
 * Type SPC to complete the current word.
 * Type TAB to complete as much as possible.
 * Type Control-U to clear the line.
 *
 * For use with version 31 of the Sd program.
 * Based on sdui-x11.c 1.10
 *
 * The version of this file is as shown immediately below.  This string
 * gets displayed at program startup, as the "ui" part of the complete
 * version.
 */

#define UI_VERSION_STRING "1.7"

/* See comments in sdmain.c regarding this string. */
static char *id="@(#)$He" "ader: Sd: sdui-tty.c "
   UI_VERSION_STRING
   "  wba@apollo.hp.com  28 May 95 $";

/* This file defines the following functions:
   uims_process_command_line
   uims_display_help
   uims_version_string
   uims_preinitialize
   uims_create_menu
   uims_postinitialize
   uims_get_startup_command
   uims_get_call_command
   uims_get_resolve_command
   uims_do_comment_popup
   uims_do_outfile_popup
   uims_do_header_popup
   uims_do_getout_popup
   uims_do_abort_popup
   uims_do_neglect_popup
   uims_do_selector_popup
   uims_do_direction_popup
   uims_do_tagger_popup
   uims_do_circcer_popup
   uims_get_number_fields
   uims_do_modifier_popup
   uims_add_new_line
   uims_reduce_line_count
   uims_begin_search
   uims_begin_reconcile_history
   uims_end_reconcile_history
   uims_update_resolve_menu
   uims_terminate
   uims_database_tick_max
   uims_database_tick
   uims_database_tick_end
   uims_database_error
   uims_bad_argument

and the following data that are used by sdmatch.c :

   num_command_commands
   command_commands
   number_of_resolve_commands
   resolve_command_strings
*/

#ifndef THINK_C
#define UNIX_STYLE
#endif

/* For "sprintf" and some IO stuff (fflush, printf, stdout) that we use
   during the "database tick" printing before the actual IO package is started.
   During normal operation, we don't do any IO at all in this file. */
#include <stdio.h>
/* For "strlen". */
#include <string.h>
/* For "isprint". */
#include <ctype.h>
/* For "atoi". */
#include <stdlib.h>

#include "sd.h"
#include "sdmatch.h"
#include "sdui-ttu.h"

#define DEL 0x7F
#define INPUT_TEXTLINE_SIZE 200

/*
 * The total version string looks something like
 * "1.4:db1.5:ui0.6tty"
 * We return the "0.6tty" part.
 */

Private char version_mem[12];

extern char *
uims_version_string(void)
{
    (void) sprintf(version_mem, "%stty", UI_VERSION_STRING);
    return version_mem;
}

/*
 * User Input functions
 */

/* not passed as args so refresh_input can be called from signal handler */
Private char user_input[INPUT_TEXTLINE_SIZE+1];
Private int user_input_size;
Private char *user_input_prompt;
Private char *function_key_expansion;

void
refresh_input(void)
{
    user_input[0] = '\0';
    user_input_size = 0;
    function_key_expansion = (char *) 0;
    clear_line(); /* clear the current line */
    put_line(user_input_prompt);
}

Private void
get_string_input(char prompt[], char dest[])
{
    put_line(prompt);
    get_string(dest);
}


Private int
get_char_input(void)
{
   int c;

   start_expand:

   if (function_key_expansion) {
      c = *function_key_expansion++;
      if (c) return c;
      else function_key_expansion = (char *) 0;
   }

#ifdef THINK_C			/* Unix or DOS provides its own cursor */
   putchar('_'); /* a cursor */
   c = get_char();
   putchar('\b'); putchar(' '); putchar('\b'); /* erase the cursor */
#else
   c = get_char();
#endif

   if (c == 129)
      function_key_expansion = "heads start\n";                /* F1 */
   else if (c == 130)
      function_key_expansion = "two calls in succession\n";    /* F2 */
   else if (c == 131)
      function_key_expansion = "pick random call\n";           /* F3 */
   else if (c == 132)
      function_key_expansion = "resolve\n";                    /* F4 */
   else if (c == 133)
      function_key_expansion = "refresh display\n";            /* F5 */
   else if (c == 134)
      function_key_expansion = "simple modifications\n";       /* F6 */
   else if (c == 135)
      function_key_expansion = "toggle concept levels\n";      /* F7 */
   else if (c == 136)
      function_key_expansion = "<anything>";                   /* F8 */
   else if (c == 137)
      function_key_expansion = "undo last call\n";             /* F9 */
   else if (c == 138)
      function_key_expansion = "end this sequence\n";          /* F10 */

   else if (c == 161)
      function_key_expansion = "sides start\n";                /* sF1 */
   else if (c == 162)
      function_key_expansion = "twice\n";                      /* sF2 */
   else if (c == 163)
      function_key_expansion = "normalize\n";                  /* sF3 */
   else if (c == 164)
      function_key_expansion = "reconcile\n";                  /* sF4 */
   else if (c == 165)
      function_key_expansion = "keep picture\n";               /* sF5 */
   else if (c == 166)
      function_key_expansion = "allow modifications\n";        /* sF6 */
   else if (c == 167)
      function_key_expansion = "toggle active phantoms\n";     /* sF7 */
   else if (c == 168)
      function_key_expansion = "<concept>";                    /* sF8 */
   else if (c == 169)
      function_key_expansion = "abort the search\n";           /* sF9 */
   else if (c == 170)
      function_key_expansion = "change output file\n";         /* sF10 */

   else if (c >= 128)
      c = ' ';

   if (function_key_expansion)
      goto start_expand;

   return c;

}
  
/* This tells how many of the last lines currently on the screen contain
   the text that the main program thinks comprise the transcript.  That is,
   if we count this far up from the bottom of the screen (well, from the
   cursor, to be precise), we will get the line that the main program thinks
   is the first line of the transcript.  This variable will often be greater
   than the main program's belief of the transcript length, because we are
   fooling around with input lines (prompts, error messages, etc.) at the
   bottom of the screen.

   The main program typically updates the transcript in a "clean" way by
   calling "uims_reduce_line_count" to erase some number of lines at the
   end of the transcript, followed by a rewrite of new material.  We compare
   what "uims_reduce_line_count" tells us against the value of this variable
   to find out how many lines to actually erase.  That way, we erase the input
   lines and produce a truly clean transcript, on devices (like VT-100's)
   capable of doing so.  If the display device can't erase lines, the user
   will see the last few lines of the transcript overlapping from one
   command to the next.  This is the appropriate behavior for a printing
   terminal or computer emulation of same. */

Private int current_text_line;

/*
 * Throw away all but the first n lines of the text output.
 * n = 0 means to erase the entire buffer.
 */

Private void
text_output_trim(int n)
{
    if (current_text_line > n)
        erase_last_n(current_text_line-n);

    current_text_line = n;
}

/*
 * end of text output stuff
 */

Private char *call_menu_prompts[NUM_CALL_LIST_KINDS];

/*
 * The main program calls this before doing anything else, so we can
 * supply additional command line arguments.
 * Note: If we are writing a call list, the program will
 * exit before doing anything else with the user interface, but this
 * must be made anyway.
 */
 
extern void
uims_process_command_line(int *argcp, char ***argvp)
{
#ifdef UNIX_STYLE
   ttu_process_command_line(argcp, *argvp);
#else
   *argcp = ccommand(argvp); /* pop up window to get command line arguments */
#endif
}

extern void uims_display_help(void)
{
#ifdef UNIX_STYLE
   ttu_display_help();
#else
   printf("\nIn addition, the usual X window system flags are supported.\n");
#endif
}

/*
 * The main program calls this before any of the call menus are
 * created, that is, before any calls to uims_create_menu.
 */
 
extern void
uims_preinitialize(void)
{
    current_text_line = 0;
    ttu_initialize();
}

/*
 * Create a menu containing number_of_calls[cl] items.
 * Use the "menu_names" array to create a
 * title line for the menu.  The string is in static storage.
 * 
 * This will be called once for each value in the enumeration call_list_kind.
 */

extern void uims_create_menu(call_list_kind cl, callspec_block *call_name_list[])
{
   call_menu_prompts[cl] = (char *) get_mem(50);  /* *** Too lazy to compute it. */
   matcher_setup_call_menu(cl, call_name_list);

   if (cl == call_list_any)
      /* The menu name here is "(any setup)".  That may be a sensible
         name for a menu, but it isn't helpful as a prompt.  So we
         just use a vanilla prompt. */
      call_menu_prompts[cl] = "--> ";
   else
      (void) sprintf(call_menu_prompts[cl], "(%s)--> ", menu_names[cl]);
}


/* The main program calls this after all the call menus have been created,
   after all calls to uims_create_menu.
   This performs any final initialization required by the interface package.

   It must also perform any required setup of the concept menu.  The
   concepts are not presented procedurally.  Instead, they can be found
   in the external array concept_descriptor_table+general_concept_offset.
   The number of concepts in that list is general_concept_size.  For each
   i, the field concept_descriptor_table[i].name has the text that we
   should display for the user.
*/

extern void
uims_postinitialize(void)
{
    call_menu_prompts[call_list_empty] = "--> ";   /* This prompt should never be used. */
    matcher_initialize(TRUE); /* want commands last in menu */
#if defined(UNIX_STYLE) && !defined(MSDOS)
    initialize_signal_handlers();
#endif
    resolver_is_unwieldy = TRUE;   /* Sorry about that. */
}

Private void
pack_and_echo_character(char c)
{
   /* Really should handle error better -- ring the bell,
      but this is called inside a loop. */

   if (user_input_size < INPUT_TEXTLINE_SIZE) {
      user_input[user_input_size++] = c;
      user_input[user_input_size] = '\0';
      put_char(c);
   }
}

/* This tells how many more lines of matches (the stuff we print in response
   to a question mark) we can print before we have to say "--More--" to
   get permission from the user to continue.  If verify_has_stopped goes on, the
   user has given a negative reply to one of our queries, and so we don't
   print any more stuff. */
Private int match_counter;
long_boolean verify_has_stopped;

/* This is what we reset the counter to whenever the user confirms.  That
   is, it is the number of lines we print per "screenful".  On a VT-100-like
   ("dumb") terminal, we will actually make it a screenful.  On a printing
   device or a workstation, we don't need to do the hold-screen stuff, because
   the device can handle output intelligently (on a printing device, it does
   this by letting us look at the paper that is spewing out on the floor;
   on a workstation we know that the window system provides real (infinite,
   of course) scrolling).  But on printing devices or workstations we still
   do the output in screenful-like blocks, because the user may not want
   to see an enormous amount of output. */
Private int match_lines;

Private void
start_matches(void)
{
   /*
    * Find the number of lines on the screen.
    * Used only for grouping "screenfuls" with "--more--" stuff
    * when showing long lists of choices after '?' is typed.
    * In other cases, we just assume the output mechanism can intelligently
    * handle what we send to it.
    */

   match_lines = get_lines_for_more();
   match_counter = match_lines-1; /* last line used for "--More--" prompt */
   verify_has_stopped = FALSE;
}

Private int
prompt_for_more_output(void)
{
    put_line("--More--");

    for (;;) {
        char c = get_char_input();
        clear_line();   /* Erase the "more" line; next item goes on that line. */

        switch (c) {
          case '\r':
          case '\n': match_counter = 1; /* show one more line */
                     return TRUE;  /* keep going */

          case '\b':
          case DEL:
          case 'q':
          case 'Q':  return FALSE; /* stop showing */

          case ' ':  return TRUE;  /* keep going */

          default:   put_line("Type Space to see more, Return for next line, Delete to stop:  --More--");
        }
    }
}

Private void
show_match(char *user_input_str, Const char *extension, Const match_result *mr)
{
   char temp[200];
   char c;
   Const char *p;
   char *q;
   char *z;

   if (verify_has_stopped) return;  /* Showing has been turned off. */

   if (match_counter <= 0) {
      match_counter = match_lines - 1;
      if (!prompt_for_more_output()) {
         match_counter = -1;   /* Turn it off. */
         verify_has_stopped = TRUE;
         return;
      }
   }
   match_counter--;
   put_line(user_input_str);
   put_line((char *) extension);
   put_line("\n");
   current_text_line++;
}

/*
 * The searching functions indicate their results by setting the variables
 * last_match and exact_match.  Last_match indicates the last matching item
 * (if there is one or more matching item).  Exact_match is set if
 * an item matches the input string exactly.  Hopefully, if there is more than
 * one item that matches exactly, they are all equivalent.
 */

Private match_result user_match;



/* BEWARE!!  These two lists must stay in step. */

#define NUM_SPECIAL_COMMANDS 4
#define SPECIAL_COMMAND_SIMPLE_MODS 0
#define SPECIAL_COMMAND_ALLOW_MODS 1
#define SPECIAL_COMMAND_TOGGLE_CONCEPT_LEVELS 2
#define SPECIAL_COMMAND_TOGGLE_ACTIVE_PHANTOMS 3

int num_command_commands = 19;          /* The number of items in the tables, independent of NUM_COMMAND_KINDS. */

Cstring command_commands[] = {
    "simple modifications",
    "allow modifications",
    "toggle concept levels",
    "toggle active phantoms",
    "exit the program",
    "quit the program",
    "undo last call",
    "discard entered concepts",
    "abort this sequence",
    "insert a comment",
    "change output file",
    "change header comment",
    "end this sequence",
    "resolve",
    "reconcile",
    "pick random call",
    "normalize",
    "keep picture",
    "refresh display"
};

static command_kind command_command_values[] = {
   (command_kind) -1,    /* The first 4 are special. */
   (command_kind) -1,
   (command_kind) -1,
   (command_kind) -1,
   command_quit,
   command_quit,
   command_undo,
   command_erase,
   command_abort,
   command_create_comment,
   command_change_outfile,
   command_change_header,
   command_getout,
   command_resolve,
   command_reconcile,
   command_anything,
   command_nice_setup,
   command_save_pic,
   command_refresh
};


/* BEWARE!!  These two lists must stay in step. */

int number_of_resolve_commands = 10;    /* The number of items in the tables, independent of NUM_RESOLVE_COMMAND_KINDS. */

Cstring resolve_command_strings[] = {
    "abort the search",
    "exit the search",
    "quit the search",
    "undo the search",
    "find another",
    "next",
    "previous",
    "accept current choice",
    "raise reconcile point",
    "lower reconcile point"
};

static resolve_command_kind resolve_command_values[] = {
   resolve_command_abort,
   resolve_command_abort,
   resolve_command_abort,
   resolve_command_abort,
   resolve_command_find_another,
   resolve_command_goto_next,
   resolve_command_goto_previous,
   resolve_command_accept,
   resolve_command_raise_rec_point,
   resolve_command_lower_rec_point
};






/* result is indicated in user_match */

Private void get_user_input(char *prompt, int which)
{
   char extended_input[200];
   char *p;
   char c;
   int matches;

   user_match.valid = FALSE;
   user_input_prompt = prompt;
   user_input[0] = '\0';
   user_input_size = 0;
   function_key_expansion = (char *) 0;
   put_line(user_input_prompt);

   for (;;) {
      /* At this point we always have the concatenation of "user_input_prompt"
         and "user_input" displayed on the current line. */

      c = get_char_input();

      if ((c == '\b') || (c == DEL)) {
         if (user_input_size > 0) {
            user_input_size--;
            user_input[user_input_size] = '\0';
            function_key_expansion = (char *) 0;
            rubout(); /* Update the display. */
         }
         continue;
      }
      else if (c == '?' || c == '!') {
         put_line ("\n");
         current_text_line++;
         start_matches();
         (void) match_user_input(user_input, which, (match_result *) 0, (char *) 0, show_match, c == '?');
         put_line ("\n");     /* Write a blank line. */
         current_text_line++;
         put_line(user_input_prompt);   /* Redisplay the current line. */
         put_line(user_input);
         continue;
      }

      if (c == ' ') {
         /* extend only to one space, inclusive */
         matches = match_user_input(user_input, which, &user_match, extended_input, (show_function) 0, FALSE);
         p = extended_input;
         if (*p) {
            while (*p) {
               if (*p != ' ')
                  pack_and_echo_character(*p++);
               else
                  goto foobar;
            }
            continue;   /* Do *not* pack the character. */

            foobar: ;
            pack_and_echo_character(c);
         }
         else if (user_match.space_ok && matches > 1)
            pack_and_echo_character(c);
         else if (diagnostic_mode)
            goto diagnostic_error;
         else
            bell();
      }
      else if ((c == '\n') || (c == '\r')) {
         matches = match_user_input(user_input, which, &user_match, extended_input, (show_function) 0, FALSE);

         /* We forbid a match consisting of two or more "direct parse" concepts, such as "grand cross".
            Direct parse concepts may only be stacked if they are followed by a call.
            The "newmodifiers" field indicates that direct parse concepts were stacked. */

         if (  (matches == 1 || matches - user_match.yielding_matches == 1 || user_match.exact)
                              &&
              (!user_match.newmodifiers || user_match.kind == ui_call_select)) {
            p = extended_input;
            while (*p)
               pack_and_echo_character(*p++);

            put_line("\n");
            /* Include the input line in our count, so we will erase it
               if we are trying to make the VT-100 screen look nice. */
            current_text_line++;

            return;
         }

         if (diagnostic_mode)
            goto diagnostic_error;

         /* Tell how bad it is, then redisplay current line. */
	   if (matches > 0) {
            char tempstuff[200];

            (void) sprintf(tempstuff, "  (%d matches, type ? for list)\n", matches);
            put_line(tempstuff);
         }
         else {
            put_line("  (no matches)\n");
         }

         put_line(user_input_prompt);
         put_line(user_input);
         current_text_line++;   /* Count that line for erasure. */
      }
      else if (c == '\t' || c == '\033') {
         (void) match_user_input(user_input, which, &user_match, extended_input, (show_function) 0, FALSE);
         p = extended_input;
         if (*p) {
            while (*p)
               pack_and_echo_character(*p++);
         }
         else if (diagnostic_mode)
            goto diagnostic_error;
         else
            bell();
      }
      else if (c == ('U'&'\037')) { /* C-u: kill line */
         user_input[0] = '\0';
         user_input_size = 0;
         function_key_expansion = (char *) 0;
         clear_line();           /* Clear the current line */
         put_line(user_input_prompt);    /* Redisplay the prompt. */
      }
      else if (isprint(c))
         pack_and_echo_character(c);
      else if (diagnostic_mode)
         goto diagnostic_error;
      else
         bell();
   }

   diagnostic_error:

   uims_terminate();
   (void) fputs("\nParsing error during diagnostic.\n", stdout);
   (void) fputs("\nParsing error during diagnostic.\n", stderr);
   final_exit(1);
}



static char *banner_prompts[] = {
    (char *) 0,
    "[AP] ",
    "[all concepts] ",
    "[all concepts,AP] ",
    "[simple modifications] ",
    "[AP,simple modifications] ",
    "[all concepts,simple modifications] ",
    "[all concepts,AP,simple modifications] ",
    "[all modifications] ",
    "[AP,all modifications] ",
    "[all concepts,all modifications] ",
    "[all concepts,AP,all modifications] "};



extern uims_reply uims_get_startup_command(void)
{
   get_user_input("Enter startup command> ", (int) match_startup_commands);
   uims_menu_index = user_match.index;
   return user_match.kind;
}


extern long_boolean uims_get_call_command(call_list_kind *call_menu, uims_reply *reply_p)
{
   char prompt_buffer[200];
   char *prompt_ptr;
   int banner_mode;

   call_menu_ptr = call_menu;

   check_menu:

   if (allowing_modifications)
       *call_menu_ptr = call_list_any;

   prompt_ptr = prompt_buffer;

   /* Put any necessary special things into the prompt. */

   banner_mode = (allowing_modifications << 2) |
                 (allowing_all_concepts ? 2 : 0) |
                 (using_active_phantoms ? 1 : 0);

   if (banner_mode != 0)
      (void) sprintf(prompt_ptr, "%s%s", banner_prompts[banner_mode], call_menu_prompts[*call_menu_ptr]);
   else
      prompt_ptr = call_menu_prompts[*call_menu_ptr];

   get_user_input(prompt_ptr, (int) *call_menu_ptr);

   *reply_p = user_match.kind;

   uims_menu_index = user_match.index;

   if (user_match.kind == ui_command_select) {
      if (user_match.index < NUM_SPECIAL_COMMANDS) {
         if (user_match.index == SPECIAL_COMMAND_SIMPLE_MODS) {
             /* Increment "allowing_modifications" up to a maximum of 2. */
             if (allowing_modifications != 2) allowing_modifications++;
         }
         else if (user_match.index == SPECIAL_COMMAND_ALLOW_MODS) {
             allowing_modifications = 2;
         }
         else if (user_match.index == SPECIAL_COMMAND_TOGGLE_CONCEPT_LEVELS) {
             allowing_all_concepts = !allowing_all_concepts;
         }
         else {   /* Must be SPECIAL_COMMAND_TOGGLE_ACTIVE_PHANTOMS. */
             using_active_phantoms = !using_active_phantoms;
         }

         goto check_menu;
      }

      /* Translate the command. */
      uims_menu_index = command_command_values[user_match.index];
   }

   if (user_match.kind == ui_call_select) {
      modifier_block *mods;
      callspec_block *save_call = main_call_lists[parse_state.call_list_to_use][uims_menu_index];

      for (mods = user_match.newmodifiers ; mods ; mods = mods->next) {
         concept_descriptor *cp = mods->this_modifier;
         uint32 concept_number_fields = 0;
         int howmanynumbers = 0;
         uint32 props = concept_table[cp->kind].concept_prop;

         if (props & CONCPROP__USE_NUMBER)
            howmanynumbers = 1;
         if (props & CONCPROP__USE_TWO_NUMBERS)
            howmanynumbers = 2;

         if (howmanynumbers != 0) {
            if ((concept_number_fields = uims_get_number_fields(howmanynumbers)) == 0)
               return TRUE;           /* User waved the mouse away. */
         }

         if (deposit_concept(cp, concept_number_fields)) return TRUE;
      }

      if (deposit_call(save_call)) return TRUE;
   }
   else if (user_match.kind == ui_concept_select) {
      concept_descriptor *cp = &concept_descriptor_table[uims_menu_index];
      uint32 concept_number_fields = 0;
      int howmanynumbers = 0;
      uint32 props = concept_table[cp->kind].concept_prop;

      if (props & CONCPROP__USE_NUMBER)
         howmanynumbers = 1;
      if (props & CONCPROP__USE_TWO_NUMBERS)
         howmanynumbers = 2;

      if (howmanynumbers != 0) {
         if ((concept_number_fields = uims_get_number_fields(howmanynumbers)) == 0)
            return TRUE;           /* User waved the mouse away. */
      }

      if (deposit_concept(cp, concept_number_fields)) return TRUE;
   }

   return FALSE;
}


extern uims_reply uims_get_resolve_command(void)
{
   get_user_input("Enter resolve command> ", (int) match_resolve_commands);
   uims_menu_index = resolve_command_values[user_match.index];
   return user_match.kind;
}


Private int get_popup_string(char prompt[], char dest[])
{
    char buffer[200];

    (void) sprintf(buffer, "%s: ", prompt);
    get_string_input(buffer, dest);
    return POPUP_ACCEPT_WITH_STRING;
}

extern int uims_do_comment_popup(char dest[])
{
    return get_popup_string("Enter comment", dest);
}

extern int uims_do_outfile_popup(char dest[])
{
    char buffer[MAX_TEXT_LINE_LENGTH];
    (void) sprintf(buffer, "Sequence output file is \"%s\".\n", outfile_string);

    put_line(buffer);
    current_text_line++;
    return get_popup_string("Enter new file name", dest);
}

extern int uims_do_header_popup(char dest[])
{
   if (header_comment[0]) {
      char buffer[MAX_TEXT_LINE_LENGTH];
      (void) sprintf(buffer, "Current header comment is \"%s\".\n", header_comment);
      put_line(buffer);
      current_text_line++;
   }
   return get_popup_string("Enter new comment", dest);
}

extern int uims_do_getout_popup(char dest[])
{
    put_line("Specify text label for sequence.\n");
    current_text_line++;
    return get_popup_string("Enter label", dest);
}

#ifdef NEGLECT
extern int uims_do_neglect_popup(char dest[])
{
    put_line("Specify integer percentage of neglected calls.\n");
    current_text_line++;
    return get_popup_string("Enter percentage", dest);
}
#endif

Private int confirm(char *question)
{
    char c;

    for (;;) {
        put_line(question);
        c = get_char_input();
        if ((c=='n') || (c=='N')) {
            put_line("no\n");
            current_text_line++;
            return POPUP_DECLINE;
        }
        if ((c=='y') || (c=='Y')) {
            put_line("yes\n");
            current_text_line++;
            return POPUP_ACCEPT;
        }
        put_char(c);

        if (diagnostic_mode) {
            uims_terminate();
            (void) fputs("\nParsing error during diagnostic.\n", stdout);
            (void) fputs("\nParsing error during diagnostic.\n", stderr);
            final_exit(1);
        }

        put_line("\n");
        put_line("Answer y or n\n");
        current_text_line += 2;
        bell();
    }
}

extern int
uims_do_abort_popup(void)
{
    put_line("The current sequence will be aborted.\n");
    current_text_line++;
    return confirm("Do you really want to abort it? ");
}

extern int uims_do_modifier_popup(Cstring callname, modify_popup_kind kind)
{
    char *line_format = "Internal error: unknown modifier kind.\n";
    char tempstuff[200];

    switch (kind) {
        case modify_popup_any:
            line_format = "The \"%s\" can be replaced.\n";
            break;
        case modify_popup_only_tag:
            line_format = "The \"%s\" can be replaced with a tagging call.\n";
            break;
        case modify_popup_only_circ:
            line_format = "The \"%s\" can be replaced with a modified circulate-like call.\n";
            break;
    }

    sprintf(tempstuff, line_format, callname);
    put_line(tempstuff);
    current_text_line++;
    return confirm("Do you want to replace it? ");
}

/*
 * UIMS_BEGIN_SEARCH is called at the beginning of each search mode
 * command (resolve, reconcile, nice setup, pick random call).
 */

extern void
uims_begin_search(search_kind goal)
{
}

/*
 * UIMS_BEGIN_RECONCILE_HISTORY is called at the beginning of a reconcile,
 * after UIMS_BEGIN_SEARCH,
 * and whenever the current reconcile point changes.  CURRENTPOINT is the
 * current reconcile point and MAXPOINT is the maximum possible reconcile
 * point.  This call is followed by calls to UIMS_REDUCE_LINE_COUNT
 * and UIMS_ADD_NEW_LINE that
 * display the current sequence with the reconcile point indicated.  These
 * calls are followed by a call to UIMS_END_RECONCILE_HISTORY.
 * Return TRUE to cause sd to forget its cached history output; do this
 * if the reconcile history is written to a separate window.
 */

extern int
uims_begin_reconcile_history(int currentpoint, int maxpoint)
{
    return FALSE;
}

/*
 * Return TRUE to cause sd to forget its cached history output.
 */

extern int
uims_end_reconcile_history(void)
{
    put_line("\n");
    return FALSE;
}

extern void
uims_update_resolve_menu(search_kind goal, int cur, int max, resolver_display_state state)
{
    char title[MAX_TEXT_LINE_LENGTH];

    create_resolve_menu_title(goal, cur, max, state, title);
    put_line(title);
    put_line("\n");
    current_text_line++;
}

extern int uims_do_selector_popup(void)
{
   uint32 retval;

   if (interactivity == interactivity_verify) {
      if (result_for_verify->who == selector_uninitialized) {
         verify_used_selector = 1;
         return (int) selector_for_initialize;
      }
      else
         return result_for_verify->who;
   }
   else if (user_match.valid && (user_match.who > selector_uninitialized)) {
      retval = (int) user_match.who;
      user_match.who = selector_uninitialized;
      return retval;
   }
   else {
      match_result saved_match = user_match;
      get_user_input("Enter who> ", (int) match_selectors);
      retval = user_match.index+1;      /* We skip the zeroth selector, which is selector_uninitialized. */
      user_match = saved_match;
      return retval;
   }
}

extern int uims_do_direction_popup(void)
{
   int n;

   if (user_match.valid && (user_match.where > direction_uninitialized)) {
      n = (int) user_match.where;
      user_match.where = direction_uninitialized;
      return n;
   }
   else {
      int retval;
      match_result saved_match = user_match;
      get_user_input("Enter direction> ", (int) match_directions);
      retval = user_match.index+1;      /* We skip the zeroth direction, which is direction_uninitialized. */
      user_match = saved_match;
      return retval;
   }
}    

extern int uims_do_tagger_popup(int tagger_class)
{
   uint32 retval;
   int j = 0;

   if (interactivity == interactivity_verify) {
      user_match.tagger = result_for_verify->tagger;
      if (user_match.tagger == 0) user_match.tagger = 1;
   }
   else if (!user_match.valid || (user_match.tagger <= 0)) {
      match_result saved_match = user_match;
      get_user_input("Enter tagging call> ", ((int) match_taggers) + tagger_class);
      saved_match.tagger = user_match.tagger;
      user_match = saved_match;
   }

   while ((user_match.tagger & 0xFF000000UL) == 0) {
      user_match.tagger <<= 8;
      j++;
   }

   retval = user_match.tagger >> 24;
   user_match.tagger &= 0x00FFFFFF;
   while (j-- != 0) user_match.tagger >>= 8;   /* Shift it back. */
   return retval;
}


extern int uims_do_circcer_popup(void)
{
   uint32 retval;

   if (interactivity == interactivity_verify) {
      retval = result_for_verify->circcer;
      if (retval == 0) retval = 1;
   }
   else if (user_match.valid && (user_match.circcer > 0)) {
      retval = user_match.circcer;
      user_match.circcer = 0;
   }
   else {
      match_result saved_match = user_match;
      get_user_input("Enter circulate replacement> ", (int) match_circcer);
      retval = user_match.circcer;
      user_match = saved_match;
   }

   return retval;
}


extern uint32 uims_get_number_fields(int nnumbers)
{
   int i;
   uint32 number_fields;
   int howmanynumbers;
   uint32 number_list = 0;
   long_boolean valid = 1;

   if (interactivity == interactivity_verify) {
      number_fields = result_for_verify->number_fields;
      howmanynumbers = result_for_verify->howmanynumbers;
   }
   else {
      number_fields = user_match.number_fields;
      howmanynumbers = user_match.howmanynumbers;
      valid = user_match.valid;
   }

   for (i=0 ; i<nnumbers ; i++) {
      uint32 this_num;

      if (valid && (howmanynumbers >= 1)) {
         this_num = number_fields & 0xF;
         number_fields >>= 4;
         howmanynumbers--;
      }
      else {
         if (interactivity == interactivity_verify) {
            this_num = number_for_initialize;
            verify_used_number = 1;
         }
         else {
            char buffer[200];
            get_string_input("How many? ", buffer);
            this_num = atoi(buffer);
         }
      }

      if (this_num == 0 || this_num > 8) return 0;    /* User gave bad answer. */
      number_list |= (this_num << (i*4));
   }

   return number_list;
}


/*
 * add a line to the text output area.
 * the_line does not have the trailing Newline in it and
 * is volatile, so we must copy it if we need it to stay around.
 */

extern void
uims_add_new_line(char the_line[])
{
    current_text_line++;
    put_line(the_line);
    put_line("\n");
}

/*
 * Throw away all but the first n lines of the text output.
 * n = 0 means to erase the entire buffer.
 */

extern void
uims_reduce_line_count(int n)
{
    text_output_trim(n);
}

extern void
uims_terminate(void)
{
    ttu_terminate();
}

/*
 * The following three functions allow the UI to put up a progress
 * indicator while the call database is being read (and processed).
 *
 * uims_database_tick_max is called before reading the database
 * with the number of ticks that will be sent.
 * uims_database_tick is called repeatedly with the number of new
 * ticks to add.
 */

Private int db_tick_max;
Private int db_tick_cur;	/* goes from 0 to db_tick_max */

#define TICK_STEPS 52
Private int tick_displayed;	/* goes from 0 to TICK_STEPS */

extern void uims_database_tick_max(int n)
{
   db_tick_max = n;
   db_tick_cur = 0;
   printf("Sd: reading database...");
   fflush(stdout);
   tick_displayed = 0;
}

extern void uims_database_tick(int n)
{
    int tick_new;

    db_tick_cur += n;
    tick_new = TICK_STEPS*db_tick_cur/db_tick_max;
    while (tick_displayed < tick_new) {
	printf(".");
	tick_displayed++;
    }
    fflush(stdout);
}

extern void uims_database_tick_end(void)
{
    printf("done\n");
}

extern void uims_database_error(Cstring message, Cstring call_name)
{
   print_line(message);
   if (call_name) {
      print_line("  While reading this call from the database:");
      print_line(call_name);
   }
}

extern void uims_bad_argument(Cstring s1, Cstring s2, Cstring s3)
{
   if (s1) print_line(s1);
   if (s2) print_line(s2);
   if (s3) print_line(s3);
   print_line("Use the -help flag for help.");
   exit_program(1);
}
