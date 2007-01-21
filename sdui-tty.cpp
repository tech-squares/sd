/*
 * sdui-tty.c - SD TTY User Interface
 * Originally for Macintosh.  Unix version by gildea.
 * Time-stamp: <96/05/22 17:17:53 wba>
 * Copyright (c) 1990-1994 Stephen Gildea, William B. Ackerman, and
 *   Alan Snyder
 *
 * Copyright (c) 1994-2006 William B. Ackerman
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
 * For use with version 34 of the Sd program.
 *
 * The version of this file is as shown immediately below.  This string
 * gets displayed at program startup, as the "ui" part of the complete
 * version.
 */

#define UI_VERSION_STRING "1.13"
#define UI_TIME_STAMP "wba@alum.mit.edu  5 Jul 2005 $"

/* This file defines all the functions in class iofull
   except for

   iofull::display_help
   iofull::help_manual
   iofull::help_faq
   iofull::final_initialize

and the following other variables:

   screen_height
   no_cursor
   no_console
   no_line_delete
*/


// For "sprintf" and some IO stuff (fflush, printf, stdout) that we use
// during the "database tick" printing before the actual IO package is started.
// During normal operation, we don't do any IO at all in this file.
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

extern void exit(int code);

#include "sd.h"
#include "paths.h"

// See comments in sdmain.cpp regarding this string.
static const char id[] = "@(#)$He" "ader: Sd: sdui-tty.c " UI_VERSION_STRING "  " UI_TIME_STAMP;


#define DEL 0x7F

// The total version string looks something like
// "1.4:db1.5:ui0.6tty"
// We return the "0.6tty" part.

static char journal_name[MAX_TEXT_LINE_LENGTH];
static FILE *journal_file = (FILE *) 0;
int sdtty_screen_height = 0;  // The "lines" option may set this to something.
                              // Otherwise, any subsystem that sees the value zero
                              // will initialize it to whatever value it thinks best,
                              // perhaps by interrogating the OS.
bool sdtty_no_console = false;
bool sdtty_no_line_delete = false;

char *iofull::version_string()
{
   return UI_VERSION_STRING "tty";
}

static resolver_display_state resolver_happiness = resolver_display_failed;


int main(int argc, char *argv[])
{
   // In Sdtty, the defaults are reverse video (white-on-black) and pastel colors.

   ui_options.reverse_video = true;
   ui_options.pastel_color = true;

   // Initialize all the callbacks that sdlib will need.
   iofull ggg;
   gg = &ggg;

   return sdmain(argc, argv);
}


// This array is the same as GLOB_full_input, but has the original capitalization
// as typed by the user.  GLOB_full_input is converted to all lower case for
// ease of searching.
static char user_input[INPUT_TEXTLINE_SIZE+1];
static char *user_input_prompt;
static char *function_key_expansion;

void refresh_input()
{
   erase_matcher_input();
   user_input[0] = '\0';
   function_key_expansion = (char *) 0;
   clear_line();
   put_line(user_input_prompt);
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
   calling "reduce_line_count" to erase some number of lines at the
   end of the transcript, followed by a rewrite of new material.  We compare
   what "reduce_line_count" tells us against the value of this variable
   to find out how many lines to actually erase.  That way, we erase the input
   lines and produce a truly clean transcript, on devices (like VT-100's)
   capable of doing so.  If the display device can't erase lines, the user
   will see the last few lines of the transcript overlapping from one
   command to the next.  This is the appropriate behavior for a printing
   terminal or computer emulation of same. */

static int current_text_line;

static char *call_menu_prompts[call_list_extent];

// For the "alternate_glyphs_1" command-line switch.
static char alt1_names1[] = "        ";
static char alt1_names2[] = "1P2R3O4C";


static void get_string_input(char prompt[], char dest[], int max)
{
   put_line(prompt);
   get_string(dest, max);
   current_text_line++;
}



/*
 * The main program calls this before doing anything else, so we can
 * supply additional command line arguments.
 * Note: If we are writing a call list, the program will
 * exit before doing anything else with the user interface, but this
 * must be made anyway.
 */

void iofull::process_command_line(int *argcp, char ***argvp)
{
   int argno = 1;
   char **argv = *argvp;
   journal_name[0] = '\0';

   while (argno < (*argcp)) {
      int i;

      if (strcmp(argv[argno], "-no_line_delete") == 0)
         sdtty_no_line_delete = true;
      else if (strcmp(argv[argno], "-no_cursor") == 0)
         ;     // This is obsolete.
      else if (strcmp(argv[argno], "-no_console") == 0)
         sdtty_no_console = true;
      else if (strcmp(argv[argno], "-alternate_glyphs_1") == 0) {
         ui_options.pn1 = alt1_names1;
         ui_options.pn2 = alt1_names2;
      }
      else if (strcmp(argv[argno], "-lines") == 0 && argno+1 < (*argcp)) {
         sdtty_screen_height = atoi(argv[argno+1]);
         goto remove_two;
      }
      else if (strcmp(argv[argno], "-journal") == 0 && argno+1 < (*argcp)) {
         strcpy(journal_name, argv[argno+1]);
         journal_file = fopen(argv[argno+1], "w");

         if (!journal_file) {
            printf("Can't open journal file\n");
            perror(argv[argno+1]);
            general_final_exit(1);
         }

         goto remove_two;
      }
      else if (strcmp(argv[argno], "-maximize") == 0)
         {}
      else if (strcmp(argv[argno], "-window_size") == 0 && argno+1 < (*argcp))
         goto remove_two;
      else if (strcmp(argv[argno], "-session") == 0 && argno+1 < (*argcp)) {
         argno += 2;
         continue;
      }
      else if (strcmp(argv[argno], "-resolve_test") == 0 && argno+1 < (*argcp)) {
         argno += 2;
         continue;
      }
      else {
         argno++;
         continue;
      }

      (*argcp)--;      // Remove this argument from the list.
      for (i=argno+1; i<=(*argcp); i++) argv[i-1] = argv[i];
      continue;

      remove_two:

      (*argcp) -= 2;   // Remove two arguments from the list.
      for (i=argno+1; i<=(*argcp); i++) argv[i-1] = argv[i+1];
      continue;
   }
}


static bool really_open_session()
{
   Cstring session_error_msg;

   // Process the session list.  If user has specified a
   // session number from the command line, we don't print
   // stuff or query anyone, but we still go through the
   // file.

   if (get_first_session_line()) goto no_session;

   // If user gave a session specification
   // in the command line, we don't query about the session.

   if (ui_options.force_session == -1000000) {
      char line[MAX_FILENAME_LENGTH];

      put_line("Do you want to use one of the following sessions?\n\n");

      while (get_next_session_line(line)) {
         put_line(line);
         put_line("\n");
      }

      put_line("Enter the number of the desired session\n");
      put_line("   (or a negative number to delete that session):  ");

      get_string(line, MAX_FILENAME_LENGTH);
      if (!line[0] || line[0] == '\r' || line[0] == '\n')
         goto no_session;

      if (!sscanf(line, "%d", &session_index)) {
         session_index = 0;         // User typed garbage -- exit the program immediately.
         return true;
      }
   }
   else {
      while (get_next_session_line((char *) 0));   // Need to scan the file anyway.
      session_index = ui_options.force_session;
   }

   if (session_index < 0)
      return true;    // Exit the program immediately.  Deletion will take place.

   {
      int session_info = process_session_info(&session_error_msg);

      if (session_info & 2) {
         put_line(session_error_msg);
         put_line("\n");
      }

      if (session_info & 1) {
         // We are not using a session, either because the user selected
         // "no session", or because of some error in processing the
         // selected session.
         goto no_session;
      }
   }

   goto really_do_it;

   no_session:

   session_index = 0;
   sequence_number = -1;

 really_do_it:

   return false;
}


static int db_tick_max;
static int db_tick_cur;   /* goes from 0 to db_tick_max */

#define TICK_STEPS 52
static int tick_displayed;   /* goes from 0 to TICK_STEPS */

static bool ttu_is_initialized = false;

bool iofull::init_step(init_callback_state s, int n)
{
   // Some operations (e.g. get_session_info) can return information
   // in the boolean return value.  The others don't care.

   char line[MAX_FILENAME_LENGTH];
   int size;

   switch (s) {

   case get_session_info:
      current_text_line = 0;
      if (!ttu_is_initialized) ttu_initialize();
      ttu_is_initialized = true;
      return really_open_session();

   case final_level_query:
      // The level never got specified, either from a command line argument
      // or from the session file.  Perhaps the program was invoked under
      // a window-ish OS in which one clicks on icons rather than typing
      // a command line.  In that case, we need to query the user for the
      // level.

      calling_level = l_mainstream;   // Default in case we fail.
      put_line("Enter the level: ");

      get_string(line, MAX_FILENAME_LENGTH);

      size = strlen(line);

      while (size > 0 && (line[size-1] == '\n' || line[size-1] == '\r'))
         line[--size] = '\000';

      parse_level(line);

      strncat(outfile_string, filename_strings[calling_level], MAX_FILENAME_LENGTH);
      break;

   case init_database1:
      // The level has been chosen.  We are about to open the database.
      call_menu_prompts[call_list_empty] = "--> ";   // This prompt should never be used.
      break;

   case init_database2:
      break;

   case calibrate_tick:
      db_tick_max = n;
      db_tick_cur = 0;
      put_line("Sd: reading database...");
      tick_displayed = 0;
      break;

   case do_tick:
      db_tick_cur += n;
      {
         int tick_new = (TICK_STEPS*db_tick_cur)/db_tick_max;
         while (tick_displayed < tick_new) {
            put_line(".");
            tick_displayed++;
         }
      }

      break;
   case tick_end:
      put_line("done\n");
      break;

   case do_accelerator:
      break;
   }

   return false;
}



/*
 * Create a menu containing number_of_calls[cl] items.
 * Use the "menu_names" array to create a
 * title line for the menu.  The string is in static storage.
 *
 * This will be called once for each value in the enumeration call_list_kind.
 */

void iofull::create_menu(call_list_kind cl)
{
   call_menu_prompts[cl] = (char *) get_mem(50);  /* *** Too lazy to compute it. */
   matcher_setup_call_menu(cl);

   if (cl == call_list_any)
      /* The menu name here is "(any setup)".  That may be a sensible
         name for a menu, but it isn't helpful as a prompt.  So we
         just use a vanilla prompt. */
      call_menu_prompts[cl] = "--> ";
   else
      sprintf(call_menu_prompts[cl], "(%s)--> ", menu_names[cl]);
}





/* The main program calls this after all the call menus have been created,
   after all calls to create_menu.
   This performs any final initialization required by the interface package.
*/




void iofull::set_window_title(char s[])
{
   char full_text[MAX_TEXT_LINE_LENGTH];

   if (journal_name[0]) {
      sprintf(full_text, "Sdtty %s {%s}", s, journal_name);
   }
   else {
      sprintf(full_text, "Sdtty %s", s);
   }

   ttu_set_window_title(full_text);
}


static void uims_bell()
{
   if (!ui_options.no_sound) ttu_bell();
}


static void pack_and_echo_character(char c)
{
   /* Really should handle error better -- ring the bell,
      but this is called inside a loop. */

   if (GLOB_user_input_size < INPUT_TEXTLINE_SIZE) {
      user_input[GLOB_user_input_size] = c;
      GLOB_user_input[GLOB_user_input_size++] = tolower(c);
      user_input[GLOB_user_input_size] = '\0';
      GLOB_user_input[GLOB_user_input_size] = '\0';
      put_char(c);
   }
}

// This tells how many more lines of matches (the stuff we print in response
// to a question mark) we can print before we have to say "--More--" to
// get permission from the user to continue.  If showing_has_stopped goes on, the
// user has given a negative reply to one of our queries, and so we don't
// print any more stuff.

// Specifically, this number, minus "text_line_count" is how many lines
// we have left on the current screenful.  When text_line_count reaches
// match_counter, we have run out of space and need to query the user.
static int match_counter;

// This is what we reset the counter to whenever the user confirms.  That is,
// it is the number of lines we print per "screenful".  On a VT-100-like
// ("dumb") terminal, we will actually make it a screenful.  On a printing
// device or a workstation, we don't need to do the hold-screen stuff,
// because the device can handle output intelligently (on a printing device,
// it does this by letting us look at the paper that is spewing out on the
// floor; on a workstation we know that the window system provides real
// (infinite, of course) scrolling).  But on printing devices or workstations
// we still do the output in screenful-like blocks, because the user may not
// want to see an enormous amount of output all at once.  This way, a
// workstation user sees things presented one screenful at a time, and only
// needs to scroll back if she wants to look at earlier screenfuls.
// This also caters to those people unfortunate enough to have to use those
// pathetic excuses for workstations (HP-UX and SunOS come to mind) that don't
// allow infinite scrolling.

static int match_lines;


static bool prompt_for_more_output()
{
   put_line("--More--");

   for (;;) {
      int c = get_char();
      clear_line();    // Erase the "more" line; next item goes on that line.

      switch (c) {
      case '\r':
      case '\n':
         match_counter = text_line_count+1; // Show one more line,
         return true;       // but otherwise keep going.
      case '\b':
      case DEL:
      case EKEY+14:    // The "delete" key on a PC.
      case 'q':
      case 'Q':
         return false; // Stop showing.
      case ' ':
         return true;  // Keep going.
      default:
         put_line("Type Space to see more, Return for next line, Delete to stop:  --More--");
      }
   }
}

void iofull::show_match()
{
   if (showing_has_stopped) return;  // Showing has been turned off.

   if (match_counter <= text_line_count) {
      match_counter = text_line_count+match_lines-2;
      if (!prompt_for_more_output()) {
         match_counter = text_line_count-1;   // Turn it off.
         showing_has_stopped = true;
         return;
      }
   }

   if (GLOB_match.indent) writestuff("   ");
   writestuff(GLOB_user_input);
   writestuff(GLOB_full_extension);
   newline();
}


static bool get_user_input(char *prompt, int which)
{
   char *p;
   char c;
   int nc;
   int matches;

   user_match.valid = false;
   user_input_prompt = prompt;
   erase_matcher_input();
   user_input[0] = '\0';
   function_key_expansion = (char *) 0;
   put_line(user_input_prompt);

   for (;;) {
      /* At this point we always have the concatenation of "user_input_prompt"
         and "user_input" displayed on the current line. */

      start_expand:

      if (function_key_expansion) {
         c = *function_key_expansion++;
         if (c) goto got_char;
         else function_key_expansion = (char *) 0;
      }

      nc = get_char();

      if (nc >= 128) {
         modifier_block *keyptr;
         int chars_deleted;
         int which_target = which;

         if (which_target > 0) which_target = 0;

         if (nc < FCN_KEY_TAB_LOW || nc > FCN_KEY_TAB_LAST)
            continue;      /* Ignore this key. */

         if (nc == CLOSEPROGRAMKEY) {
            // User clicked the "X" in the upper-right corner, or used the system menu
            // to close the program, or perhaps used the task manager.  In any case,
            // don't close without querying the user.
            if (which_target == match_startup_commands || gg->do_abort_popup() == POPUP_ACCEPT)
               general_final_exit(0);
         }

         keyptr = fcn_key_table_normal[nc-FCN_KEY_TAB_LOW];

         // Check for special bindings.
         // These always come from the main binding table, even if
         // we are doing something else, like a resolve.

         if (keyptr && keyptr->index < 0) {
            switch (keyptr->index) {
            case special_index_deleteline:
               erase_matcher_input();
               strcpy(user_input, GLOB_user_input);
               function_key_expansion = (char *) 0;
               clear_line();                   // Clear the current line.
               put_line(user_input_prompt);    // Redisplay the prompt.
               continue;
            case special_index_deleteword:
               chars_deleted = delete_matcher_word();
               while (chars_deleted-- > 0) rubout();
               strcpy(user_input, GLOB_user_input);
               function_key_expansion = (char *) 0;
               continue;
            case special_index_quote_anything:
               function_key_expansion = "<anything>";
               goto do_character;
            default:
               continue;    // Ignore all others.
            }
         }

         if (which_target == match_startup_commands)
            keyptr = fcn_key_table_start[nc-FCN_KEY_TAB_LOW];
         else if (which_target == match_resolve_commands)
            keyptr = fcn_key_table_resolve[nc-FCN_KEY_TAB_LOW];
         else if (which_target == 0)
            keyptr = fcn_key_table_normal[nc-FCN_KEY_TAB_LOW];
         else
            continue;

         if (!keyptr) {
            // If user hits alt-F4 and there is no binding for it, we handle it in
            // the usual way anyway.  This makes the behavior similar to Sd, where
            // the system automatically provides that action.
            if (nc == AFKEY+4) {
               if (which_target == match_startup_commands || gg->do_abort_popup() == POPUP_ACCEPT)
                  general_final_exit(0);
            }

            continue;   /* No binding for this key; ignore it. */
         }

         // If we get here, we have a function key to process from the table.

         char linebuff[INPUT_TEXTLINE_SIZE+1];
         if (process_accel_or_abbrev(*keyptr, linebuff)) {
            put_line(linebuff);
            put_line("\n");

            if (journal_file) {
               fputs(linebuff, journal_file);
               fputc('\n', journal_file);
            }

            current_text_line++;
            return false;
         }

         continue;   // Couldn't be processed; ignore the key press.
      }

   do_character:

      c = nc;

      if (function_key_expansion)
         goto start_expand;

   got_char:

      if ((c == '\b') || (c == DEL)) {
         if (GLOB_user_input_size > 0) {
            GLOB_user_input_size--;
            user_input[GLOB_user_input_size] = '\0';
            GLOB_user_input[GLOB_user_input_size] = '\0';
            function_key_expansion = (char *) 0;
            rubout();    // Update the display with the character erased.
         }
         continue;
      }
      else if (c == '?' || c == '!') {
         put_char(c);
         put_line("\n");
         current_text_line++;
         match_lines = get_lines_for_more();
         if (match_lines < 2) match_lines = 25;  // The system is screwed up.
         if (ui_options.diagnostic_mode) match_lines = 1000000;
         match_counter = text_line_count+match_lines-2;   // Count for for "--More--" prompt.
         showing_has_stopped = false;
         (void) match_user_input(which, true, c == '?', false);
         put_line("\n");     // Write a blank line.
         current_text_line++;
         put_line(user_input_prompt);   /* Redisplay the current line. */
         put_line(user_input);
         continue;
      }
      else if (c == ' ' || c == '-') {
         // Extend only to one space or hyphen, inclusive.
         matches = match_user_input(which, false, false, true);
         p = GLOB_echo_stuff;

         if (*p) {
            while (*p) {
               if (*p != ' ' && *p != '-')
                  pack_and_echo_character(*p++);
               else {
                  pack_and_echo_character(c);
                  goto foobar;
               }
            }
            continue;   // Do *not* pack the character.

            foobar: ;
         }
         else if (GLOB_space_ok && matches > 1)
            pack_and_echo_character(c);
         else if (ui_options.diagnostic_mode)
            goto diagnostic_error;
         else
            uims_bell();
      }
      else if ((c == '\n') || (c == '\r')) {

         // Look for abbreviations.

         abbrev_block *asearch = (abbrev_block *) 0;

         if (which == match_startup_commands)
            asearch = abbrev_table_start;
         else if (which == match_resolve_commands)
            asearch = abbrev_table_resolve;
         else if (which >= 0)
            asearch = abbrev_table_normal;

         for ( ; asearch ; asearch = asearch->next) {
            if (!strcmp(asearch->key, GLOB_user_input)) {
               char linebuff[INPUT_TEXTLINE_SIZE+1];
               if (process_accel_or_abbrev(asearch->value, linebuff)) {
                  refresh_input();
                  put_line(linebuff);
                  put_line("\n");

                  if (journal_file) {
                     fputs(linebuff, journal_file);
                     fputc('\n', journal_file);
                  }

                  current_text_line++;
                  return false;
               }
               break;   // Couldn't be processed.  Stop.  No other abbreviations will match.
            }
         }

         matches = match_user_input(which, false, false, true);
         user_match = GLOB_match;

         if (!strcmp(GLOB_user_input, "help")) {
            put_line("\n");
            user_match.match.kind = ui_help_simple;
            current_text_line++;
            return true;
         }

         /* We forbid a match consisting of two or more "direct parse" concepts, such as
            "grand cross".  Direct parse concepts may only be stacked if they are followed
            by a call.  The "match.next" field indicates that direct parse concepts
            were stacked. */

         if ((matches == 1 || matches - GLOB_yielding_matches == 1 || user_match.exact) &&
             ((!user_match.match.packed_next_conc_or_subcall &&
               !user_match.match.packed_secondary_subcall) ||
              user_match.match.kind == ui_call_select ||
              user_match.match.kind == ui_concept_select)) {

            p = GLOB_echo_stuff;
            while (*p)
               pack_and_echo_character(*p++);

            put_line("\n");

            if (journal_file) {
               fputs(GLOB_user_input, journal_file);
               fputc('\n', journal_file);
            }

            current_text_line++;
            return false;
         }

         if (ui_options.diagnostic_mode)
            goto diagnostic_error;

         /* Tell how bad it is, then redisplay current line. */
         if (matches > 0) {
            char tempstuff[200];

            sprintf(tempstuff, "  (%d matches, type ! or ? for list)\n", matches);
            put_line(tempstuff);
         }
         else
            put_line("  (no matches)\n");

         put_line(user_input_prompt);
         put_line(user_input);
         current_text_line++;   /* Count that line for erasure. */
      }
      else if (c == '\t' || c == '\033') {
         (void) match_user_input(which, false, false, true);
         user_match = GLOB_match;
         p = GLOB_echo_stuff;

         if (*p) {
            while (*p)
               pack_and_echo_character(*p++);
         }
         else if (ui_options.diagnostic_mode)
            goto diagnostic_error;
         else
            uims_bell();
      }
      else if (isprint(c))
         pack_and_echo_character(c);
      else if (ui_options.diagnostic_mode)
         goto diagnostic_error;
      else
         uims_bell();
   }

   diagnostic_error:

   (void) fputs("\nParsing error during diagnostic.\n", stdout);
   (void) fputs("\nParsing error during diagnostic.\n", stderr);
   general_final_exit(1);
   return false;
}




static char *banner_prompts0[] = {
    "",
    "simple modifications",
    "all modifications",
    "??"};

static char *banner_prompts1[] = {
    "",
    "all concepts",
    "??",
    "??"};

static char *banner_prompts2[] = {
    "",
    "AP",
    "??",
    "??"};

static char *banner_prompts3[] = {
    "",
    "SC",
    "RSC",
    "??"};

static char *banner_prompts4[] = {
    "",
    "MG",
    "??",
    "??"};




uims_reply iofull::get_startup_command()
{
   for (;;) {
      if (!get_user_input("Enter startup command> ", (int) match_startup_commands))
         break;

      /* ****** remember to remove "start_select_help" in sd.h */

      writestuff("The program wants you to start a sequence.  Type, for example, "
                 "'heads start', and press Enter.  Then type a call, such as "
                 "'pass the ocean', and press Enter again.");
      newline();
   }

   uims_menu_index = user_match.match.index;

   if (user_match.match.kind == ui_start_select) {
      /* Translate the command. */
      uims_menu_index = (int) startup_command_values[user_match.match.index];
   }

   return user_match.match.kind;
}



// This returns true if it fails, e.g. the user waves the mouse away.
bool iofull::get_call_command(uims_reply *reply_p)
{
   char prompt_buffer[200];
   char *prompt_ptr;
   bool retval = false;

   if (allowing_modifications != 0)
      parse_state.call_list_to_use = call_list_any;

   prompt_ptr = prompt_buffer;
   prompt_buffer[0] = '\0';

   // Put any necessary special things into the prompt.

   int banner_mode =
      (allowing_minigrand ? 256 : 0) |
      (ui_options.singing_call_mode << 6) |
      (using_active_phantoms ? 16 : 0) |
      (allowing_all_concepts ? 4 : 0) |
      (allowing_modifications);

   if (banner_mode != 0) {
      int i;
      int comma = 0;

      (void) strcat(prompt_buffer, "[");

      for (i=0 ; i<5 ; i++,banner_mode>>=2) {
         if (banner_mode&3) {

            if (comma) (void) strcat(prompt_buffer, ",");

            if (i==0) {
               (void) strcat(prompt_buffer, banner_prompts0[banner_mode&3]);
            }

            if (i==1) {
               (void) strcat(prompt_buffer, banner_prompts1[banner_mode&3]);
            }

            if (i==2) {
               (void) strcat(prompt_buffer, banner_prompts2[banner_mode&3]);
            }

            if (i==3) {
               (void) strcat(prompt_buffer, banner_prompts3[banner_mode&3]);
            }

            if (i==4) {
               (void) strcat(prompt_buffer, banner_prompts4[banner_mode&3]);
            }

            comma |= banner_mode&3;
         }
      }

      (void) strcat(prompt_buffer, "] ");
      (void) strcat(prompt_buffer, call_menu_prompts[parse_state.call_list_to_use]);
   }
   else
      prompt_ptr = call_menu_prompts[parse_state.call_list_to_use];

   if (get_user_input(prompt_ptr, (int) parse_state.call_list_to_use)) {
      // User typed "help".
      *reply_p = ui_command_select;
      uims_menu_index = command_help;
      return false;
   }

   *reply_p = user_match.match.kind;

   uims_menu_index = user_match.match.index;

   if (user_match.match.index < 0)
      /* Special encoding from a function key. */
      uims_menu_index = -1-user_match.match.index;
   else if (user_match.match.kind == ui_command_select) {
      /* Translate the command. */
      uims_menu_index = (int) command_command_values[user_match.match.index];
   }
   else {
      call_conc_option_state save_stuff = user_match.match.call_conc_options;
      there_is_a_call = false;
      retval = deposit_call_tree(&user_match.match, (parse_block *) 0, 2);
      user_match.match.call_conc_options = save_stuff;
      if (there_is_a_call) {
         parse_state.topcallflags1 = the_topcallflags;
         *reply_p = ui_call_select;
      }
   }

   return retval;
}


uims_reply iofull::get_resolve_command()
{
   for (;;) {
      if (!get_user_input("Enter search command> ", (int) match_resolve_commands))
         break;

      if (resolver_happiness == resolver_display_failed)
         writestuff("The program is trying to resolve, but has failed to find anything."
                    "  You can type 'find another' to keep trying.");
      else
         writestuff("The program is searching for resolves.  If you like the currently "
                    "displayed resolve, you can type 'accept' and press Enter."
                    "  If not, you can type 'find another'.");

      newline();
   }

   if (user_match.match.index < 0)
      uims_menu_index = -1-user_match.match.index;   /* Special encoding from a function key. */
   else
      uims_menu_index = (int) resolve_command_values[user_match.match.index];

   return user_match.match.kind;
}


static popup_return get_popup_string(char prompt[], char dest[])
{
    char buffer[200];

    sprintf(buffer, "%s: ", prompt);
    get_string_input(buffer, dest, 200);
    return POPUP_ACCEPT_WITH_STRING;
}

popup_return iofull::do_comment_popup(char dest[])
{
   popup_return retval = get_popup_string("Enter comment", dest);

   if (retval != POPUP_ACCEPT_WITH_STRING) {
      if (journal_file) {
         fputs(dest, journal_file);
         fputc('\n', journal_file);
      }
   }

   return retval;
}

popup_return iofull::do_outfile_popup(char dest[])
{
   writestuff("Current sequence output file is \"");
   writestuff(outfile_string);
   writestuff("\".");
   newline();

   return get_popup_string("Enter new file name (or '+' to base it on today's date)", dest);
}

popup_return iofull::do_header_popup(char dest[])
{
   if (header_comment[0]) {
      writestuff("Current title is \"");
      writestuff(header_comment);
      writestuff("\".");
      newline();
   }
   return get_popup_string("Enter new title", dest);
}

popup_return iofull::do_getout_popup(char dest[])
{
   if (header_comment[0]) {
      writestuff("Session title is \"");
      writestuff(header_comment);
      writestuff("\".");
      newline();
      writestuff("You can give an additional comment for just this sequence.");
      newline();
   }
   else {
      writestuff("Type comment for this sequence, if desired.");
      newline();
   }

   return get_popup_string("Enter comment", dest);
}

static int confirm(char *question)
{
   for (;;) {
      put_line(question);
      put_line(" ");
      char c = get_char();
      if ((c=='n') || (c=='N')) {
         put_line("no\n");
         current_text_line++;
         if (journal_file) fputc('n', journal_file);
         return POPUP_DECLINE;
      }
      if ((c=='y') || (c=='Y')) {
         put_line("yes\n");
         current_text_line++;
         if (journal_file) fputc('y', journal_file);
         return POPUP_ACCEPT;
      }

      if (c >= 0) put_char(c);

      if (ui_options.diagnostic_mode) {
         (void) fputs("\nParsing error during diagnostic.\n", stdout);
         (void) fputs("\nParsing error during diagnostic.\n", stderr);
         general_final_exit(1);
      }

      put_line("\n");
      put_line("Answer y or n\n");
      current_text_line += 2;
      uims_bell();
   }
}

int iofull::yesnoconfirm(char * /*title*/, char *line1, char *line2,
                         bool /*excl*/, bool /*info*/)
{
   if (line1) {
      writestuff(line1);
      newline();
   }

   return confirm(line2);
}

int iofull::do_abort_popup()
{
   return yesnoconfirm("Confirmation", "The current sequence will be aborted.",
                       "Do you really want to abort it?", true, false);
}

void iofull::update_resolve_menu(command_kind goal, int cur, int max,
                                 resolver_display_state state)
{
   char title[MAX_TEXT_LINE_LENGTH];

   resolver_happiness = state;

   create_resolve_menu_title(goal, cur, max, state, title);
   writestuff(title);
   newline();
}

int iofull::do_selector_popup()
{
   match_result saved_match = user_match;

   for (;;) {
      if (!get_user_input("Enter who> ", (int) match_selectors))
         break;

      writestuff("The program wants you to type a person designator.  "
                 "Try typing something like 'boys' and pressing Enter.");
      newline();
   }

   // We skip the zeroth selector, which is selector_uninitialized.
   int retval = user_match.match.index+1;
   user_match = saved_match;
   return retval;
}

int iofull::do_direction_popup()
{
   match_result saved_match = user_match;

   for (;;) {
      if (!get_user_input("Enter direction> ", (int) match_directions))
         break;

      writestuff("The program wants you to type a direction.  "
                 "Try typing something like 'right' and pressing Enter.");
      newline();
   }

   // We skip the zeroth direction, which is direction_uninitialized.
   int retval = user_match.match.index+1;
   user_match = saved_match;

   return retval;
}

int iofull::do_tagger_popup(int tagger_class)
{
   match_result saved_match = user_match;

   for (;;) {
      if (!get_user_input("Enter tagging call> ", ((int) match_taggers) + tagger_class))
         break;

      writestuff("The program wants you to type an 'ATC' (tagging) call.  "
                 "Try typing something like 'vertical tag' and pressing Enter.");
      newline();
   }

   saved_match.match.call_conc_options.tagger = user_match.match.call_conc_options.tagger;
   user_match = saved_match;
   int retval = user_match.match.call_conc_options.tagger;
   user_match.match.call_conc_options.tagger = 0;
   return retval;
}


int iofull::do_circcer_popup()
{
   uint32 retval;

   if (interactivity == interactivity_verify) {
      retval = verify_options.circcer;
      if (retval == 0) retval = 1;
   }
   else if (!user_match.valid || (user_match.match.call_conc_options.circcer == 0)) {
      match_result saved_match = user_match;

      for (;;) {
         if (!get_user_input("Enter circulate replacement> ", (int) match_circcer))
            break;

         writestuff("The program wants you to type a circulating call as part of "
                    "a call like 'in roll motivate'.  "
                    "Try typing something like 'in roll circulate' and pressing Enter.");
         newline();
      }

      retval = user_match.match.call_conc_options.circcer;
      user_match = saved_match;
   }
   else {
      retval = user_match.match.call_conc_options.circcer;
      user_match.match.call_conc_options.circcer = 0;
   }

   return retval;
}


uint32 iofull::get_number_fields(int nnumbers, bool odd_number_only, bool forbid_zero)
{
   int i;
   uint32 number_fields = user_match.match.call_conc_options.number_fields;
   int howmanynumbers = user_match.match.call_conc_options.howmanynumbers;
   uint32 number_list = 0;

   for (i=0 ; i<nnumbers ; i++) {
      uint32 this_num;

      if (!user_match.valid || (howmanynumbers <= 0)) {
         for (;;) {
            char buffer[200];
            get_string_input("How many? ", buffer, 200);
            if (buffer[0] == '!' || buffer[0] == '?') {
               writestuff("Type a number between 0 and 36");
               newline();
            }
            else if (!buffer[0]) return ~0UL;
            else {
               this_num = atoi(buffer);
               break;
            }
         }
      }
      else {
         this_num = number_fields & NUMBER_FIELD_MASK;
         number_fields >>= BITS_PER_NUMBER_FIELD;
         howmanynumbers--;
      }

      if (odd_number_only && !(this_num & 1)) return ~0UL;
      if (forbid_zero && this_num == 0) return ~0UL;
      if (this_num >= NUM_CARDINALS) return ~0UL;    // User gave bad answer.
      number_list |= (this_num << (i*BITS_PER_NUMBER_FIELD));
   }

   return number_list;
}


/*
 * add a line to the text output area.
 * the_line does not have the trailing Newline in it and
 * is volatile, so we must copy it if we need it to stay around.
 */

void iofull::add_new_line(const char the_line[], uint32 drawing_picture)
{
    put_line(the_line);
    put_line("\n");
    current_text_line++;
}

// Make everything before the most recent n lines
// not subject to erasure.  Used to protect the
// display of the transcript of preceding sequences.
void iofull::no_erase_before_n(int n)
{
   current_text_line = n;
}

// Throw away all but the first n lines of the text output.
// N = 0 means to erase the entire buffer.
void iofull::reduce_line_count(int n)
{
   if (current_text_line > n)
      erase_last_n(current_text_line-n);

   current_text_line = n;
}


bool iofull::choose_font() { return false; }
bool iofull::print_this() { return false; }
bool iofull::print_any() { return false; }



void iofull::bad_argument(Cstring s1, Cstring s2, Cstring s3)
{
   if (s2 && s2[0]) {
      fprintf(stderr, "%s: %s\n", s1, s2);
   }
   else {
      fprintf(stderr, "%s\n", s1);
   }

   if (s3) fprintf(stderr, "%s\n", s3);
   fprintf(stderr, "%s", "Use the -help flag for help.\n");
   general_final_exit(1);
}


void iofull::fatal_error_exit(int code, Cstring s1, Cstring s2)
{
   if (s2 && s2[0])
      fprintf(stderr, "%s: %s\n", s1, s2);
   else
      fprintf(stderr, "%s\n", s1);

   session_index = 0;  // Prevent attempts to update session file.
   general_final_exit(code);
}


void iofull::serious_error_print(Cstring s1)
{
   fprintf(stderr, "%s\n", s1);
   fprintf(stderr, "Press 'enter' to resume.\n");
   get_char();
}


void iofull::terminate(int code)
{
   if (journal_file) (void) fclose(journal_file);
   if (ttu_is_initialized) ttu_terminate();
   exit(code);
}
