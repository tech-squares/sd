/* SD -- square dance caller's helper.

    Copyright (C) 1990  William B. Ackerman.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    This is for version 27. */

/* This defines the following functions:
   clear_screen
   writestuff
   newline
   doublespace_file
   exit_program
   fail
   fail2
   specialfail
   string_copy
   display_initial_history
   write_history_line
   warn
   find_proper_call_list
   assoc
   find_calldef
   clear_people
   rotperson
   rotcw
   rotccw
   clear_person
   copy_person
   copy_rot
   swap_people
   install_person
   install_rot
   process_final_concepts
   fix_n_results
   divided_setup_move
   overlapped_setup_move

and the following external variables:
   longjmp_buffer
   longjmp_ptr
   history
   history_ptr
   written_history_items
   written_history_nopic
   last_magic_diamond
   error_message1
   error_message2
   collision_person1
   collision_person2
   enable_file_writing
   selector_names
*/

#include <string.h>
#include "sd.h"



/*  This is the line length beyond which we will take pity on
   whatever device has to print the result, and break the text line.
   It is presumably smaller than our internal text capacity. */
# define MAX_PRINT_LENGTH 59

/* These variables are external. */

real_jmp_buf longjmp_buffer;
real_jmp_buf *longjmp_ptr;
configuration history[100];
int history_ptr;

/* This tells how much of the history text written to the UI is "safe".  If this
   is positive, the history items up to that number, inclusive, have valid
   "text_line" fields, and have had their text written to the UI.  Furthermore,
   the local variable "text_line_count" is >= any of those "text_line" fields,
   and each "text_line" field shows the number of lines of text that were
   written to the UI when that history item was written.  This variable is
   -1 when none of the history is safe or the state of the text in the UI
   is unknown. */
int written_history_items;
/* When written_history_items is positive, this tells how many of the initial lines
   did not have pictures forced on (other than having been drawn as a natural consequence
   of the "draw_pic" flag being on.  If this number ever becomes greater than
   written_history_items, that's OK.  It just means that none of the lines had
   forced pictures. */
int written_history_nopic;

parse_block *last_magic_diamond;
char error_message1[MAX_ERR_LENGTH];
char error_message2[MAX_ERR_LENGTH];
unsigned int collision_person1;
unsigned int collision_person2;
long_boolean enable_file_writing;

/* These variables are used by the text-packing stuff. */

static char *destcurr;
static char lastchar;
static char *lastblank;
static char current_line[MAX_TEXT_LINE_LENGTH];
static int text_line_count = 0;



static void open_text_line(void)
{
   destcurr = current_line;
   lastchar = ' ';
   lastblank = (char *) 0;
}


extern void clear_screen(void)
{
   written_history_items = -1;
   text_line_count = 0;
   uims_reduce_line_count(0);
   open_text_line();
}


static void writechar(char src)
{
   char save_buffer[MAX_TEXT_LINE_LENGTH];
   char *p;
   char *q;

   *destcurr = (lastchar = src);
   if (src == ' ' && destcurr != current_line) lastblank = destcurr;

   if (destcurr < &current_line[MAX_PRINT_LENGTH])
      destcurr++;
   else {
      /* Line overflow.  Try to write everything up to the last
         blank, then fill next line with everything since that blank. */
      if (lastblank) {
         p = lastblank+1;
         q = save_buffer;
         while (p <= destcurr) *q++ = *p++;
         *q = '\0';
         destcurr = lastblank;
      }
      else {
         /* Must break a word. */
         save_buffer[0] = *destcurr;
         save_buffer[1] = '\0';
      }

      newline();
      writestuff("   ");
      writestuff(save_buffer);
   }
}


extern void newline(void)
{
   *destcurr = '\0';

   if (enable_file_writing)
      write_file(current_line);

   text_line_count++;
   uims_add_new_line(current_line);
   open_text_line();
}



extern void writestuff(char s[])
{
   char *f = s;
   while (*f) writechar(*f++);
}



static void writestuff_with_fraction(char s[], long_boolean do_number, int num)
{
   if (do_number) {
      char *f = s;
      while (*f) {
         if (f[0] == 'N' && f[1] == '/' && f[2] == '4') {
            if ((num & 0xFFFF) == 2)
               writestuff("1/2");
            else {
               writechar('0' + (num & 0xFFFF));
               writestuff("/4");
            }
            writestuff(&f[3]);
            return;
         }
         else
            writechar(*f++);
      }
   }
   else {
      writestuff(s);
   }
}


extern void doublespace_file(void)
{
   write_file("");
}



extern void exit_program(int code)
{
   uims_terminate();
   final_exit(code);
}


extern void fail(char s[])
{
   (void) strncpy(error_message1, s, MAX_ERR_LENGTH);
   error_message1[MAX_ERR_LENGTH-1] = '\0';
   error_message2[0] = '\0';
   longjmp(longjmp_ptr->the_buf, 1);
}


extern void fail2(char s1[], char s2[])
{
   (void) strncpy(error_message1, s1, MAX_ERR_LENGTH);
   error_message1[MAX_ERR_LENGTH-1] = '\0';
   (void) strncpy(error_message2, s2, MAX_ERR_LENGTH);
   error_message2[MAX_ERR_LENGTH-1] = '\0';
   longjmp(longjmp_ptr->the_buf, 2);
}


extern void specialfail(char s[])
{
   (void) strncpy(error_message1, s, MAX_ERR_LENGTH);
   error_message1[MAX_ERR_LENGTH-1] = '\0';
   error_message2[0] = '\0';
   longjmp(longjmp_ptr->the_buf, 4);
}


extern void string_copy(char **dest, char src[])
{
   char *f = src;
   char *t = *dest;

   while (*t++ = *f++);
   *dest = t-1;
}


/* BEWARE!!  These two lists are keyed to the definition of "selector_kind" in sd.h,
   and to the necessary stuff in SDUI. */
/* This one is external, so that the user interface can use it for popup text. */
char *selector_names[] = {
   "???",
   "boys",
   "girls",
   "heads",
   "sides",
   "head corners",
   "side corners",
   "head boys",
   "head girls",
   "side boys",
   "side girls",
   "centers",
   "ends",
   "leads",
   "trailers",
   "beaux",
   "belles",
   "center 2",
   "center 6",
   "outer 2",
   "outer 6",
   "near line",
   "far line",
   "near column",
   "far column",
   "near box",
   "far box",
   "everyone",
   "no one",
   NULL};         /* The X11 interface uses this when making the popup text. */

static char *selector_singular[] = {
   "???",
   "boy",
   "girl",
   "head",
   "side",
   "head corner",
   "side corner",
   "head boy",
   "head girl",
   "side boy",
   "side girl",
   "center",
   "end",
   "lead",
   "trailer",
   "beau",
   "belle",
   "center 2",
   "center 6",
   "outer 2",
   "outer 6",
   "near line",
   "far line",
   "near column",
   "far column",
   "near box",
   "far box",
   "everyone",
   "no one"};

/* BEWARE!!  These strings are keyed to the definitions of "warn__<whatever>" in sd.h . */
static char *warning_strings[] = {
   /*  warn__do_your_part        */   "Do your part.",
   /*  warn__tbonephantom        */   "This is a T-bone phantom setup call.  Everyone will do their own part.",
   /*  warn__ends_work_to_spots  */   "Ends work to same spots.",
   /*  warn__awkward_centers     */   "Awkward for centers.",
   /*  warn__bad_concept_level   */   "This concept is not allowed at this level.",
   /*  warn__not_funny           */   "That wasn't funny.",
   /*  warn__hard_funny          */   "Very difficult funny concept.",
   /*  warn__unusual             */   "This is an unusual setup for this call.",
   /*  warn__rear_back           */   "Rear back from the handhold.",
   /*  warn__awful_rear_back     */   "Rear back from the handhold -- this is very unusual.",
   /*  warn__excess_split        */   "Split concept seems to be superfluous here.",
   /*  warn__lineconc_perp       */   "Ends should opt for setup perpendicular to their original line.",
   /*  warn__dmdconc_perp        */   "Ends should opt for setup perpendicular to their original diamond points.",
   /*  warn__lineconc_par        */   "Ends should opt for setup parallel to their original line -- concentric rule does not apply.",
   /*  warn__dmdconc_par         */   "Ends should opt for setup parallel to their original diamond points -- concentric rule does not apply.",
   /*  warn__xclineconc_perp     */   "New ends should opt for setup perpendicular to their original (center) line.",
   /*  warn__xcdmdconc_perp      */   "New ends should opt for setup perpendicular to their original (center) diamond points.",
   /*  warn__ctrstand_endscpls   */   "Centers work in tandem, ends as couples.",
   /*  warn__ctrscpls_endstand   */   "Centers work as couples, ends in tandem.",
   /*  warn__each2x2             */   "Each 2x2.",
   /*  warn__each1x4             */   "Each 1x4.",
   /*  warn__each1x2             */   "Each 1x2.",
   /*  warn__take_right_hands    */   "Take right hands.",
   /*  warn__ctrs_are_dmd        */   "The centers are the diamond.",
   /*  warn__full_pgram          */   "Completely offset parallelogram.",
   /*  warn__offset_gone         */   "The offset goes away.",
   /*  warn__overlap_gone        */   "The overlap goes away.",
   /*  warn__to_o_spots          */   "Go back to 'O' spots.",
   /*  warn__to_x_spots          */   "Go back to butterfly spots.",
   /*  warn__some_rear_back      */   "Some people rear back.",
   /*  warn__not_tbone_person    */   "Work with the person to whom you are not T-boned.",
   /*  warn__check_c1_phan       */   "Check a 'C1 phantom' setup.",
   /*  warn__check_dmd_qtag      */   "Fudge to a diamond/quarter-tag setup.",
   /*  warn__check_2x4           */   "Check a 2x4 setup.",
   /*  warn__check_pgram         */   "Opt for a parallelogram."};

static char *ordinals[] = {"1st", "2nd", "3rd", "4th", "5th"};

/* This variable is shared by "print_recurse", which is subordinate
   to "write_history_line". */

static parse_block *static_cptr;

/* Bits that go into argument to print_recurse. */
#define PRINT_RECURSE_STAR 01
#define PRINT_RECURSE_TAGREACT 02
#define PRINT_RECURSE_TAGENDING 04

static void print_recurse(int print_recurse_arg)
{
   long_boolean use_left_name = FALSE;
   long_boolean use_cross_name = FALSE;
   long_boolean comma_after_next_concept = FALSE;

   while (static_cptr) {
      int i;
      concept_kind k;
      concept_descriptor *item;
      int next_recurse_arg;
      parse_block *saved_cptr;

      item = static_cptr->concept;
      k = item->kind;

      if (k == concept_comment) {
         comment_block *fubb;

         fubb = (comment_block *) static_cptr->call;
         writestuff("{ ");
         writestuff(fubb->txt);
         writestuff(" } ");
         static_cptr = static_cptr->next;
      }
      else if (k > marker_end_of_list) {
         /* This is a concept. */

         int index = static_cptr->number;
         long_boolean request_final_space = FALSE;
         long_boolean you_owe_me_a_number = FALSE;
         long_boolean request_comma_after_next_concept = FALSE;
         concept_kind kk = k;
         parse_block *cc = static_cptr;

         /* We turn off the funny name elision if concepts are present,
            except for "cross" and "left", since we want "[cross flip] your neighbor"
            and "[LEFT tag the star] your neighbor" to work. */

         for (;;) {
            if (kk <= marker_end_of_list) {
               break;
            }
            else if (kk != concept_left && kk != concept_cross) {
               print_recurse_arg &= ~(PRINT_RECURSE_TAGREACT | PRINT_RECURSE_TAGENDING);
               break;
            }
            cc = cc->next;
            if (!cc) break;
            kk = cc->concept->kind;
         }

         if (concept_table[k].concept_prop & CONCPROP__USE_SELECTOR) {
            selector_kind selector = static_cptr->selector;

            if (k == concept_some_vs_others) {
               writestuff("OWN the ");
               writestuff(selector_names[selector]);
               writestuff(", ");
            }
            else if (k == concept_selbasedtrngl) {
               writestuff(selector_singular[selector]);
            }
            else {
               writestuff(selector_names[selector]);
               writestuff(" ");
            }
         }

         /* These concepts are missing the word "are" in the menu, because otherwise the menu would
            be excessively wide, so we put the word back in. */
         if (k == concept_some_are_frac_tandem || k == concept_some_are_tandem)
            writestuff("ARE ");

         if ((concept_table[k].concept_prop & (CONCPROP__USE_NUMBER | CONCPROP__USE_TWO_NUMBERS)) &&
                     k != concept_nth_part && k != concept_replace_nth_part) {
            if (k == concept_frac_stable || k == concept_frac_tandem || k == concept_some_are_frac_tandem ||
                     k == concept_phantom_frac_tandem || k == concept_so_and_so_frac_stable || k == concept_gruesome_frac_tandem) {
               you_owe_me_a_number = TRUE;
            }
            else {
               char nn[3];
               nn[0] = '0' + (index & 0xFFFF);
               nn[1] = '\0';
               writestuff(nn);
               if (concept_table[k].concept_prop & CONCPROP__USE_TWO_NUMBERS) {
                  nn[0] = '/';
                  nn[1] = '0' + (index >> 16);
                  nn[2] = '\0';
                  writestuff(nn);
               }
               writestuff(" ");
            }
         }

         saved_cptr = static_cptr;
         static_cptr = static_cptr->next;    /* Now it points to the thing after this concept. */

         if (concept_table[k].concept_prop & CONCPROP__SECOND_CALL) {
            parse_block *subsidiary_ptr = saved_cptr->subsidiary_root;

            if (k == concept_centers_and_ends) {
               if ((i = item->value.arg1) == 2)
                  writestuff("CENTER 6 ");
               else if (i)
                  writestuff("CENTER 2 ");
               else
                  writestuff("CENTERS ");
            }
            else if (k == concept_some_vs_others)
               ;
            else if (k == concept_sequential) {
               writestuff("(");
            }
            else if (k == concept_callrigger) {
               writestuff("ENDS BEGIN A [");
            }
            else if (k == concept_replace_nth_part) {
               writestuff("DELAY: ");
               if (!static_cptr || !subsidiary_ptr) {
                  if (saved_cptr->concept->value.arg1)
                     writestuff("(interrupting after the ");
                  else
                     writestuff("(replacing the ");
                  writestuff(ordinals[index-1]);
                  writestuff(" part) ");
               }
            }
            else {
               writestuff(item->name);
               writestuff(" ");
            }

            print_recurse(0);

            if (k == concept_callrigger) {
               writestuff("]-RIGGER");             /* We want to print this even if input is incomplete. */
               if (!subsidiary_ptr) break;         /* Can happen if echoing incomplete input. */
               request_final_space = TRUE;
            }
            else {
               if (!subsidiary_ptr) break;         /* Can happen if echoing incomplete input. */

               request_final_space = TRUE;

               if (k == concept_centers_and_ends)
                  writestuff(" WHILE THE ENDS");
               else if (k == concept_on_your_own || k == concept_interlace)
                  writestuff(" AND");
               else if (k == concept_replace_nth_part) {
                  if (saved_cptr->concept->value.arg1)
                     writestuff(" BUT INTERRUPT AFTER THE ");
                  else
                     writestuff(" BUT REPLACE THE ");
                  writestuff(ordinals[index-1]);
                  writestuff(" PART WITH A [");
                  request_final_space = FALSE;
               }
               else if (k == concept_sequential)
                  writestuff(" ;");
               else
                  writestuff(" BY");
            }

            static_cptr = subsidiary_ptr;
         }
         else if (k == concept_so_and_so_only) {
            writestuff("ONLY");
            request_final_space = TRUE;
         }
         else if (k == concept_selbasedtrngl) {
            writestuff(&item->name[9]);      /* Strip off the "so-and-so". */
            request_final_space = TRUE;
         }
         else if (k == concept_standard) {
            writestuff("ARE STANDARD IN");
            request_final_space = TRUE;
         }
         else if (k == concept_some_are_frac_tandem || k == concept_so_and_so_frac_stable ||
                  k == concept_some_are_tandem || k == concept_so_and_so_stable || k == concept_so_and_so_begin) {
            writestuff_with_fraction(&item->name[10], you_owe_me_a_number, index);      /* Strip off the "so-and-so ". */
            writestuff(",");
            request_final_space = TRUE;
         }
         else if (k == concept_frac_stable) {
            writestuff_with_fraction(item->name, you_owe_me_a_number, index);
            request_final_space = TRUE;
         }
         else if (k == concept_double_offset) {
            int scarg1 = saved_cptr->concept->value.arg1;
            if (scarg1 == 0)
               writestuff("ARE CENTERS OF A DOUBLE-OFFSET 1/4 TAG");
            else if (scarg1 == 1)
               writestuff("ARE CENTERS OF A DOUBLE-OFFSET 3/4 TAG");
            else if (scarg1 == 3)
               writestuff("ARE CENTERS OF DOUBLE-OFFSET DIAMONDS");
            else
               writestuff("ARE CENTERS OF A DOUBLE-OFFSET QUARTER-SOMETHING");
            request_final_space = TRUE;
         }
         else if (static_cptr && (k == concept_left || k == concept_cross || k == concept_single)) {

            /* These concepts want to take special action if there are no following concepts and
               certain escape characters are found in the name of the following call. */

            final_set finaljunk;
            callspec_block *target_call;
            parse_block *tptr;
            
            /* Skip all final concepts, then demand that what remains is a marker (as opposed to a serious
                concept), and that a real call has been entered, and that its name starts with "@g". */
            tptr = process_final_concepts(static_cptr, FALSE, &finaljunk);

            if (tptr) {
               target_call = tptr->call;
   
               if ((tptr->concept->kind <= marker_end_of_list) && target_call && (target_call->name[0] == '@')) {
                  if (k == concept_left) {
                     /* See if this is a call whose name naturally changes when the "left" concept is used. */
                     if (target_call->name[1] == 'g') {
                        use_left_name = TRUE;
                     }
                     else {
                        writestuff(item->name);
                        request_final_space = TRUE;
                     }
                  }
                  else if (k == concept_cross) {
                     /* See if this is a call that wants the "cross" modifier to be moved inside its name. */
                     if (target_call->name[1] == 'i') {
                        use_cross_name = TRUE;
                     }
                     else {
                        writestuff(item->name);
                        request_final_space = TRUE;
                     }
                  }
                  else {
                     /* See if this is a call that wants the "single" concept to be given as "single file" instead. */
                     if (target_call->name[1] == 'h') {
                        writestuff("SINGLE FILE");
                        request_final_space = TRUE;
                     }
                     else {
                        writestuff(item->name);
                        request_final_space = TRUE;
                     }
                  }
               }
               else {
                  writestuff(item->name);
                  request_final_space = TRUE;
               }
            }
            else {
               writestuff(item->name);
               request_final_space = TRUE;
            }
         }
         else if (k == concept_fractional)
            /* Already printed the fraction; that's all we want. */
            ;
         else if (k == concept_nth_part) {
            writestuff("DO THE ");
            writestuff(ordinals[index-1]);
            writestuff(" PART");
            request_comma_after_next_concept = TRUE;
            request_final_space = TRUE;
         }
         else if (k == concept_replace_nth_part) {
            writestuff("DELAY -- REPLACE THE ");
            writestuff(ordinals[index-1]);
            writestuff(" PART");
            request_comma_after_next_concept = TRUE;
            request_final_space = TRUE;
         }
         else if ((k == concept_meta) && saved_cptr->concept->value.arg1 == 3) {
            writestuff("START");
            request_comma_after_next_concept = TRUE;
            request_final_space = TRUE;
         }
         else {
            writestuff_with_fraction(item->name, you_owe_me_a_number, index);      /* Strip off the "so-and-so are ". */
            request_final_space = TRUE;
         }

         if (comma_after_next_concept)
            writestuff(", ");
         else if (request_final_space)
            writestuff(" ");

         comma_after_next_concept = request_comma_after_next_concept;
      }
      else {
         /* This is a "marker", so it has a call, perhaps with a selector and/or number.
            The call may be null if we are printing a partially entered line.  Beware. */

         parse_block *save_cptr;
         parse_block *subsidiary_ptr;
         parse_block *search;
         selector_kind i16junk;
         int i17junk;
         callspec_block *localcall;
         long_boolean pending_subst, subst_in_use;

         i16junk = static_cptr->selector;
         i17junk = static_cptr->number;
         localcall = static_cptr->call;

         save_cptr = static_cptr;
         subst_in_use = FALSE;
         next_recurse_arg = 0;

         if (k == concept_another_call_next_mod) {
            search = save_cptr->next;
            while (search) {
               subsidiary_ptr = search->subsidiary_root;
               if (subsidiary_ptr && ( (search->concept->kind == concept_another_call_next_mod) ||
                                       (search->concept->kind == concept_another_call_next_modreact) ||
                                       (search->concept->kind == concept_another_call_next_modtag))) {
                  if (search->concept->kind == concept_another_call_next_modreact) next_recurse_arg = PRINT_RECURSE_TAGREACT;
                  else if (search->concept->kind == concept_another_call_next_modtag) next_recurse_arg = PRINT_RECURSE_TAGENDING;
                  subst_in_use = TRUE;
                  static_cptr = subsidiary_ptr;
                  break;
               }
               search = search->next;
            }
         }
   
         pending_subst = subst_in_use;

         /* Now "subst_in_use" is on if there is a replacement call that goes in naturally.  During the
            scan of the name, we will try to fit that replacement into the name of the call as directed
            by atsign-escapes.  If we succeed at this, we will clear "pending_subst".
            In addition to all of this, there may be any number of forcible replacements. */

         if (localcall) {      /* Call = NIL means we are echoing input and user hasn't entered call yet. */
            char *np;

            if (enable_file_writing) localcall->age = global_age;
            np = localcall->name;

            /* Skip any "@g", "@h", or "@i" marker (we already acted on it.) */
            if ((*np == '@') && ((np[1] == 'g') || (np[1] == 'h') || (np[1] == 'i'))) np += 2;

            while (*np) {
               if (*np == '@') {
                  if (np[1] == '6' || np[1] == 'k') {
                     if (lastchar != ' ' && lastchar != '[') writestuff(" ");
                     if (np[1] == '6')
                        writestuff(selector_names[i16junk]);
                     else
                        writestuff(selector_singular[i16junk]);
                     if (np[2] && np[2] != ' ' && np[2] != ']')
                        writestuff(" ");
                     np += 2;       /* skip the digit */
                  }
                  else if (np[1] == '9' || np[1] == 'a' || np[1] == 'b') {
                     char nn[2];

                     if (lastchar != ' ' && lastchar != '[') writestuff(" ");
                     nn[0] = '0' + i17junk;
                     nn[1] = '\0';
                     if (np[1] == '9')
                        writestuff(nn);
                     else if (i17junk == 2)
                        writestuff("1/2");
                     else if ((i17junk == 4) && (np[1] == 'a'))
                        writestuff("full");
                     else {
                        writestuff(nn);
                        writestuff("/4");
                     }

                     np += 2;       /* skip the digit */
                  }
                  else if (np[1] == 'e') {
                     if (use_left_name) {
                        np += 2;
                        while (*np != '@') np++;
                        if (lastchar == ']') writestuff(" ");
                        writestuff("left");
                     }
                     np += 2;
                  }
                  else if (np[1] == 'j') {
                     if (!use_cross_name) {
                        np += 2;
                        while (*np != '@') np++;
                     }
                     np += 2;
                  }
                  else if (np[1] == 'l')
                     np += 2;
                  else if (np[1] == 'c') {
                     if (print_recurse_arg & (PRINT_RECURSE_TAGREACT | PRINT_RECURSE_TAGENDING)) {
                        np += 2;
                        while (*np != '@') np++;
                     }
                     else {
                        if ((lastchar == ' ' || lastchar == '[') && (np[2] == ' ')) np++;
                     }
                     np += 2;
                  }
                  else {
                     char savec = np[1];
      
                     if (subst_in_use) {
                        if (np[1] == '2' || np[1] == '4') {
                           np += 2;
                           while (*np != '@') np++;
                        }
                     }
                     else {
                        if (np[1] == '7') {
                           np += 2;
                           while (*np != '@') np++;
                        }
                     }
      
                     if (pending_subst && savec != '4' && savec != '7') {
                        if (lastchar != ' ' && lastchar != '[') writestuff(" ");
                        writestuff("[");
                        print_recurse(next_recurse_arg | PRINT_RECURSE_STAR);
                        writestuff("]");
      
                        pending_subst = FALSE;
                     }
      
                     np += 2;        /* skip the digit */
                  }
               }
               else {
                  char c = *np++;

                  if (lastchar == ']' && c != ' ' && c != ']')
                     writestuff(" ");

                  if ((lastchar != ' ' && lastchar != '[') || c != ' ') writechar(c);
               }
            }

            if (lastchar == ']' && *np && *np != ' ' && *np != ']')
               writestuff(" ");
         }
         else if (print_recurse_arg & PRINT_RECURSE_STAR) {
            writestuff("*");
         }

         /* Now if "pending_subst" is still on, we have to do by hand what should have been
            a natural replacement.  In any case, we have to find forcible replacements and
            report them. */
   
         if (k == concept_another_call_next_mod) {
            int first_replace = 0;

            search = save_cptr->next;
            while (search) {
               subsidiary_ptr = search->subsidiary_root;
               if (subsidiary_ptr) {
                  static_cptr = subsidiary_ptr;
   
                  if (  (search->concept->kind == concept_another_call_next_mod) ||
                        (search->concept->kind == concept_another_call_next_modreact) ||
                        (search->concept->kind == concept_another_call_next_modtag)) {
                     /* This is a natural replacement.  It may already have been taken care of. */
                     if (pending_subst) {
                        writestuff(" [modification: ");
                        print_recurse(PRINT_RECURSE_STAR);
                        writestuff("]");
                     }
                  }
                  else {
                     /* This is a forced replacement.  Need to check for case of replacing
                        one star turn with another. */
                     localcall = search->call;

                     if ((!(first_replace++)) && static_cptr &&
                           (localcall->callflags & cflag__is_star_call) &&
                                 ((static_cptr->concept->kind == marker_end_of_list) ||
                                 (static_cptr->concept->kind == concept_another_call_next_mod)) &&
                           (static_cptr->call) &&
                           (static_cptr->call->callflags & cflag__is_star_call)) {
                        writestuff(" BUT [");
                     }
                     else {
                        if (first_replace == 1)
                           writestuff(" BUT REPLACE ");
                        else
                           writestuff(" AND REPLACE ");
                        writestuff(localcall->name);
                        writestuff(" WITH [");
                     }

                     print_recurse(PRINT_RECURSE_STAR);
                     writestuff("]");
                  }
               }
               search = search->next;
            }
         }

         break;
      }

      if (k == concept_sequential) {
         print_recurse(PRINT_RECURSE_STAR);
         writestuff(")");
         return;
      }
      else if (k == concept_replace_nth_part) {
         print_recurse(PRINT_RECURSE_STAR);
         writestuff("]");
         return;
      }
   }
}


/* These static variables are used by printperson. */

static char peoplenames[] = "1234";
static char directions[] = "B?B>B?B<B?B?B?B?B^B?BVB?B?B?B?B?G?G>G?G<G?G?G?G?G^G?GVG?G?G?G?G?";
static char personbuffer[] = " ZZZ";

static void printperson(int x)
{
   int i;

   if ((x & 01000) == 0) {
      writestuff("    ");
   }
   else {
      i = 2 * (x & 017);
      if ((x & 0100) != 0) i = i+32;
      personbuffer[1] = peoplenames[(x >> 7) & 3];
      personbuffer[2] = directions[i];
      personbuffer[3] = directions[i+1];
      writestuff(personbuffer);
   }
}

/* These static variables are used by printsetup/print_4_person_setup/do_write/do_write4/do_write4_small. */

static int offs, roti, ri, modulus, personstart;
static setup *printarg;

static void do_write4_small(char s[])
{
   char c;

   offs = (((roti >> 1) & 1) * (modulus / 2)) - modulus;
   for (;;) {
      if (!(c=(*s++))) return;
      else if (c == '@') newline();
      else if (c == ' ') writestuff(" ");
      else if (c >= 'a' && c <= 'l')
         printperson(rotperson(printarg->people[personstart + ((c-'a'-offs) % modulus)].id1, ri));
      else writestuff("?");
   }
}


static void do_write(char s[])
{
   char c;

   for (;;) {
      if (!(c=(*s++))) return;
      else if (c == '@') newline();
      else if (c == ' ') writestuff(" ");
      else if (c >= 'a' && c <= 'x')
         printperson(rotperson(printarg->people[(c-'a'-offs) % modulus].id1, roti));
      else writestuff("?");
   }
}


static void print_4_person_setup(int ps, small_setup *s, int elong)
{
   roti = (s->srotation & 3);
   ri = roti * 011;
   personstart = ps;
   
   switch (s->skind) {
      case s_1x2:
         modulus = 2;
         newline();
         if (roti & 1)
            do_write4_small("a@b@");
         else
            do_write4_small("a  b@");
         break;
      case s2x2:
         modulus = 4;
         newline();
         if (roti & 1) {
            if (elong < 0)
               do_write4_small("da@cb@");
            else if ((roti+elong) & 1)
               do_write4_small("da@@@cb@");
            else
               do_write4_small("d    a@c    b@");
         }
         else {
            if (elong < 0)
               do_write4_small("ab@dc@");
            else if ((roti+elong) & 1)
               do_write4_small("ab@@@dc@");
            else
               do_write4_small("a    b@d    c@");
         }
         break;
      case s_star:
         modulus = 4;
         newline();
         if (roti & 1)
            do_write4_small("   a@d  b@   c@");
         else
            do_write4_small("   b@a  c@   d@");
         break;
      case s1x4:
         modulus = 4;
         newline();
         if (roti & 1)
            do_write4_small("a@b@d@c@");
         else
            do_write4_small("a  b  d  c@");
         break;
      case sdmd:
         modulus = 4;
         newline();
         if (roti & 1)
            do_write4_small("   a@@d  b@@   c@");
         else
            do_write4_small("     b@a      c@     d@");
         break;
      case s_2x3:
         modulus = 6;
         newline();
         if (roti & 1)
            do_write4_small("f  a@e  b@d  c@");
         else
            do_write4_small("a  b  c@f  e  d@");
         break;
      case s_short6:
         modulus = 6;
         newline();
         if (roti & 1)
            do_write4_small("   fa@e      b@   dc@");
         else
            do_write4_small("   b@a  c@f  d@   e@");
         break;
      case s_bone6:
         modulus = 6;
         newline();
         if (roti & 1)
            do_write4_small("ea@  f@  c@db@");
         else
            do_write4_small("a        b@    fc@e        d@");
         break;
      case s_1x6:
         modulus = 6;
         newline();
         if (roti & 1)
            do_write4_small("a@b@c@f@e@d@");
         else
            do_write4_small("a  b  c  f  e  d@");
         break;
      default:
         writestuff(" ????");
   }
}

static void printsetup(setup *x)
{
   printarg = x;
   modulus = setup_limits[x->kind]+1;
   roti = (x->rotation & 3) * 011;
   offs = ((x->rotation & 2) * (modulus / 4)) - modulus;
   
   newline();

   switch (x->kind) {
      case s2x4:
         if (x->rotation & 1)
            do_write("h  a@@g  b@@f  c@@e  d");
         else
            do_write("a  b  c  d@@h  g  f  e");
         break;
      case s1x8:
         if (x->rotation & 1)
            do_write("a@b@d@c@g@h@f@e");
         else
            do_write("a b d c g h f e");
         break;
      case s1x10:
         if (x->rotation & 1)
            do_write("a@b@c@d@e@j@i@h@g@f");
         else
            do_write("a b c d e j i h g f");
         break;
      case s1x12:
         if (x->rotation & 1)
            do_write("a@b@c@d@e@f@l@k@j@i@h@g");
         else
            do_write("a b c d e f l k j i h g");
         break;
      case s1x14:
         if (x->rotation & 1)
            do_write("a@b@c@d@e@f@g@n@m@l@k@j@i@h");
         else
            /* Sorry, they won't fit unless we run them together like this. */
            do_write("abcdefgnmlkjih");
         break;
      case s1x16:
         if (x->rotation & 1)
            do_write("a@b@c@d@e@f@g@h@p@o@n@m@l@k@j@i");
         else
            /* Sorry, they won't fit unless we run them together like this. */
            do_write("abcdefghponmlkji");
         break;
      case s_crosswave:
         if (x->rotation & 1)
            do_write("      a@      b@@ghdc@@      f@      e");
         else
            do_write("          c@          d@ab        fe@          h@          g");
         break;
      case s_qtag:
         if ((x->people[0].id1 & x->people[1].id1 & x->people[4].id1 & x->people[5].id1 & 1) &&
               (x->people[2].id1 & x->people[3].id1 & x->people[6].id1 & x->people[7].id1 & 010)) {
            /* People are in diamond-like orientation. */
            if (x->rotation & 1)
               do_write("      g@f        a@      h@@      d@e        b@      c");
            else {
               do_write("   a     b@@g h d c@@   f     e");
            }
         }
         else {
            /* People are not.  Probably 1/4-tag-like orientation. */
            if (x->rotation & 1)
               do_write("      g@f  h  a@e  d  b@      c");
            else {
               do_write("      a  b@@g  h  d  c@@      f  e");
            }
         }
         break;
      case s_bone:
         if (x->rotation & 1)
            do_write("fa@  g@  h@  d@  c@eb");
         else
            do_write("a                   b@    g h d c@f                   e");
         break;
      case s_rigger:
         if (x->rotation & 1)
            do_write("  g@  h@fa@eb@  d@  c");
         else
            do_write("        a b@gh         dc@        f e");
         break;
      case s_spindle:
         if (x->rotation & 1)
            do_write("  h@ga@fb@ec@  d");
         else
            do_write("    a b c@h              d@    g f e");
         break;
      case s_ptpd:
         if (x->rotation & 1)
            do_write("  a@@db@@  c@@  g@@fh@@  e");
         else
            do_write("    b           h@a    c   g    e@    d           f");
         break;
      case s_hrglass:
         if (x->rotation & 1)
            do_write("     g@f      a@   hd@e      b@     c");
         else
            do_write("  a  b@     d@g       c@     h@  f  e");
         break;
      case s_galaxy:
         offs = ((x->rotation & 3) * (modulus / 4)) - modulus;
         do_write("     c@   bd@a      e@   hf@     g");
         break;
      case s2x6:
         if (x->rotation & 1)
            do_write("l  a@@k  b@@j  c@@i  d@@h  e@@g  f");
         else
            do_write("a  b  c  d  e  f@@l  k  j  i  h  g");
         break;
      case s3x4:
         if (x->rotation & 1)
            do_write("j  k  a@@i  l  b@@h  f  c@@g  e  d");
         else
            do_write("a  b  c  d@@k  l  f  e@@j  i  h  g");
         break;
      case s_3x1dmd:
         if (x->rotation & 1)
            do_write("      a@@      b@@      c@h        d@      g@@      f@@      e");
         else
            do_write("             d@@a b c g f e@@             h");
         break;
      case s_3dmd:
         if (x->rotation & 1)
            do_write("      j@i        a@      k@@      l@h        b@      f@@      e@g        c@      d");
         else
            do_write("   a      b      c@@j k l f e d@@   i      h      g");
         break;
      case s_wingedstar:
         if (x->rotation & 1)
            do_write("   a@@   b@@   c@h  d@   g@@   f@@   e");
         else
            do_write("             d@a b c  g f e@             h");
         break;
      case s_wingedstar12:
         if (x->rotation & 1)
            do_write("   a@@   b@@   c@l  d@   e@   k@j  f@   i@@   h@@   g");
         else
            do_write("             d       f@a b c  e k  i h g@             l       j");
         break;
      case s_4dmd:
         if (x->rotation & 1)
            do_write("      m@l        a@      n@@      o@k        b@      p@@      h@j        c@      g@@      f@i        d@      e");
         else
            do_write("   a      b      c      d@@m n o p h g f e@@   l      k      j      i");
         break;
      case s2x8:
         if (x->rotation & 1)
            do_write("p  a@@o  b@@n  c@@m  d@@l  e@@k  f@@j  g@@i  h");
         else
            do_write("a  b  c  d  e  f  g  h@@p  o  n  m  l  k  j  i");
         break;
      case s4x4:
         offs = ((x->rotation & 3) * (modulus / 4)) - modulus;
         do_write("m  n  o  a@@k  p  d  b@@j  l  h  c@@i  g  f  e");
         break;
      case s_bigblob:
         offs = ((x->rotation & 3) * (modulus / 4)) - modulus;
         do_write("            a  b@@      v  w  c  d@@t  u  x  f  e  g@@s  q  r  l  i  h@@      p  o  k  j@@            n  m");
         break;
      case s_c1phan:
         offs = ((x->rotation & 3) * (modulus / 4)) - modulus;
         do_write("   b        e@a  c  h  f@   d        g@@   o        l@n  p  k  i@   m        j");
         break;
      case s4x6:
         if (x->rotation & 1)
            do_write("r  s  l  a@@q  t  k  b@@p  u  j  c@@o  v  i  d@@n  w  h  e@@m  x  g  f");
         else
            do_write("a  b  c  d  e  f@@l  k  j  i  h  g@@s  t  u  v  w  x@@r  q  p  o  n  m");
         break;
      case s_normal_concentric:
         writestuff(" centers:");
         newline();
         print_4_person_setup(0, &(x->inner), -1);
         writestuff(" ends:");
         newline();
         print_4_person_setup(12, &(x->outer), x->outer_elongation);
         break;
      default:
         writestuff("???? UNKNOWN SETUP ????");
   }

   newline();
   newline();
}


/* Clear the screen and display the initial part of the history.
   This attempts to re-use material already displayed on the screen.
   The "num_pics" argument tells how many of the last history items
   are to have pictures forced, so we can tell exactly what items
   have pictures. */
extern void display_initial_history(int upper_limit, int num_pics)
{
   int j, startpoint, compilerbug;

   /* See if we can re-use some of the safely written history. */
   /* First, cut down overly optimistic estimates. */
   if (written_history_items > upper_limit) written_history_items = upper_limit;
   /* And normalize the "nopic" number. */
   if (written_history_nopic > written_history_items) written_history_nopic = written_history_items;

   /* Check whether pictures are faithful.  If any item in the written history has its
      "picture forced" property (as determined by written_history_nopic)
      different from what we want that property to be (as determined by upper_limit-num_pics),
      and that item didn't have the draw_pic flag on, that item needs to be rewritten.
      We cut written_history_items down to below that item if such is the case. */

   for (j=1; j<=written_history_items; j++) {
      compilerbug = ((int) ((unsigned int) (written_history_nopic-j)) ^
                 ((unsigned int) (upper_limit-num_pics-j)));
      if (compilerbug < 0 && ~history[j].draw_pic) {
         written_history_items = j-1;
         break;
      }
   }

   if (written_history_items > 0) {
      /* We win.  Back up the text line count to the right place, and rewrite the rest. */

      text_line_count = history[written_history_items].text_line;
      uims_reduce_line_count(text_line_count);
      open_text_line();
      startpoint = written_history_items+1;
   }
   else {
      /* We lose, there is nothing we can use. */
      clear_screen();
      startpoint = 1;
   }

   for (j=startpoint; j<=upper_limit-num_pics; j++) write_history_line(j, (char *) 0, FALSE, file_write_no);

   /* Now write stuff with forced pictures. */

   for (j=upper_limit-num_pics+1; j<=upper_limit; j++) {
      if (j >= startpoint) write_history_line(j, (char *) 0, TRUE, file_write_no);
   }

   written_history_items = upper_limit;   /* This stuff is now safe. */
   written_history_nopic = written_history_items-num_pics;
}



extern void write_history_line(int history_index, char *header, long_boolean picture, file_write_flag write_to_file)
{
   int index, w;

   if (write_to_file == file_write_double)
      doublespace_file();

   index = history[history_index].centersp;

   if (header) {
      writestuff(header);
   }

   if (index != 0) {
      if (startinfolist[index].into_the_middle) goto morefinal;
      writestuff(startinfolist[index].name);
      goto final;
   }

   static_cptr = history[history_index].command_root;
   
   /* Need to check for the special case of starting a sequence with heads or sides.
      If this is the first line of the history, and we started with heads of sides,
      change the name of this concept from "centers" to the appropriate thing. */

   if (history_index == 2 && static_cptr->concept->kind == concept_centers_or_ends && static_cptr->concept->value.arg1 == 0) {
      index = history[1].centersp;
      if (startinfolist[index].into_the_middle) {
         writestuff(startinfolist[index].name);
         writestuff(" ");
         static_cptr = static_cptr->next;
      }
   }
   
   print_recurse(0);
   
   final:

   newline();

   morefinal:

   /* Check for warnings to print. */
   /* Do not double space them, even if writing final output. */

   for (w=0; w<64; w++) {
      if (((1 << (w & 0x1f)) & history[history_index].warnings.bits[w>>5]) != 0) {
         writestuff("  Warning:  ");
         writestuff(warning_strings[w]);
         newline();
      }
   }

   if (picture || history[history_index].draw_pic) {
      printsetup(&history[history_index].state);
   }

   /* Record that this history item has been written to the UI. */
   history[history_index].text_line = text_line_count;
}


extern void warn(int w)
{
   history[history_ptr+1].warnings.bits[w>>5] |= 1 << (w & 0x1f);
}


extern call_list_kind find_proper_call_list(setup *s)
{
   if (s->kind == s1x8) {
      if      ((s->people[0].id1 & 017) == 010 &&
               (s->people[1].id1 & 017) == 012 &&
               (s->people[2].id1 & 017) == 012 &&
               (s->people[3].id1 & 017) == 010 &&
               (s->people[4].id1 & 017) == 012 &&
               (s->people[5].id1 & 017) == 010 &&
               (s->people[6].id1 & 017) == 010 &&
               (s->people[7].id1 & 017) == 012)
         return(call_list_1x8);
      else if ((s->people[0].id1 & 017) == 012 &&
               (s->people[1].id1 & 017) == 010 &&
               (s->people[2].id1 & 017) == 010 &&
               (s->people[3].id1 & 017) == 012 &&
               (s->people[4].id1 & 017) == 010 &&
               (s->people[5].id1 & 017) == 012 &&
               (s->people[6].id1 & 017) == 012 &&
               (s->people[7].id1 & 017) == 010)
         return(call_list_l1x8);
      else if ((s->people[0].id1 & 015) == 1 &&
               (s->people[1].id1 & 015) == 1 &&
               (s->people[2].id1 & 015) == 1 &&
               (s->people[3].id1 & 015) == 1 &&
               (s->people[4].id1 & 015) == 1 &&
               (s->people[5].id1 & 015) == 1 &&
               (s->people[6].id1 & 015) == 1 &&
               (s->people[7].id1 & 015) == 1)
         return(call_list_gcol);
   }
   else if (s->kind == s2x4) {
      if      ((s->people[0].id1 & 017) == 1 &&
               (s->people[1].id1 & 017) == 1 &&
               (s->people[2].id1 & 017) == 3 &&
               (s->people[3].id1 & 017) == 3 &&
               (s->people[4].id1 & 017) == 3 &&
               (s->people[5].id1 & 017) == 3 &&
               (s->people[6].id1 & 017) == 1 &&
               (s->people[7].id1 & 017) == 1)
         return(call_list_dpt);
      else if ((s->people[0].id1 & 017) == 3 &&
               (s->people[1].id1 & 017) == 3 &&
               (s->people[2].id1 & 017) == 1 &&
               (s->people[3].id1 & 017) == 1 &&
               (s->people[4].id1 & 017) == 1 &&
               (s->people[5].id1 & 017) == 1 &&
               (s->people[6].id1 & 017) == 3 &&
               (s->people[7].id1 & 017) == 3)
         return(call_list_cdpt);
      else if ((s->people[0].id1 & 017) == 1 &&
               (s->people[1].id1 & 017) == 1 &&
               (s->people[2].id1 & 017) == 1 &&
               (s->people[3].id1 & 017) == 1 &&
               (s->people[4].id1 & 017) == 3 &&
               (s->people[5].id1 & 017) == 3 &&
               (s->people[6].id1 & 017) == 3 &&
               (s->people[7].id1 & 017) == 3)
         return(call_list_rcol);
      else if ((s->people[0].id1 & 017) == 3 &&
               (s->people[1].id1 & 017) == 3 &&
               (s->people[2].id1 & 017) == 3 &&
               (s->people[3].id1 & 017) == 3 &&
               (s->people[4].id1 & 017) == 1 &&
               (s->people[5].id1 & 017) == 1 &&
               (s->people[6].id1 & 017) == 1 &&
               (s->people[7].id1 & 017) == 1)
         return(call_list_lcol);
      else if ((s->people[0].id1 & 017) == 1 &&
               (s->people[1].id1 & 017) == 3 &&
               (s->people[2].id1 & 017) == 1 &&
               (s->people[3].id1 & 017) == 3 &&
               (s->people[4].id1 & 017) == 3 &&
               (s->people[5].id1 & 017) == 1 &&
               (s->people[6].id1 & 017) == 3 &&
               (s->people[7].id1 & 017) == 1)
         return(call_list_8ch);
      else if ((s->people[0].id1 & 017) == 3 &&
               (s->people[1].id1 & 017) == 1 &&
               (s->people[2].id1 & 017) == 3 &&
               (s->people[3].id1 & 017) == 1 &&
               (s->people[4].id1 & 017) == 1 &&
               (s->people[5].id1 & 017) == 3 &&
               (s->people[6].id1 & 017) == 1 &&
               (s->people[7].id1 & 017) == 3)
         return(call_list_tby);
      else if ((s->people[0].id1 & 017) == 012 &&
               (s->people[1].id1 & 017) == 012 &&
               (s->people[2].id1 & 017) == 012 &&
               (s->people[3].id1 & 017) == 012 &&
               (s->people[4].id1 & 017) == 010 &&
               (s->people[5].id1 & 017) == 010 &&
               (s->people[6].id1 & 017) == 010 &&
               (s->people[7].id1 & 017) == 010)
         return(call_list_lin);
      else if ((s->people[0].id1 & 017) == 010 &&
               (s->people[1].id1 & 017) == 010 &&
               (s->people[2].id1 & 017) == 010 &&
               (s->people[3].id1 & 017) == 010 &&
               (s->people[4].id1 & 017) == 012 &&
               (s->people[5].id1 & 017) == 012 &&
               (s->people[6].id1 & 017) == 012 &&
               (s->people[7].id1 & 017) == 012)
         return(call_list_lout);
      else if ((s->people[0].id1 & 017) == 010 &&
               (s->people[1].id1 & 017) == 012 &&
               (s->people[2].id1 & 017) == 010 &&
               (s->people[3].id1 & 017) == 012 &&
               (s->people[4].id1 & 017) == 012 &&
               (s->people[5].id1 & 017) == 010 &&
               (s->people[6].id1 & 017) == 012 &&
               (s->people[7].id1 & 017) == 010)
         return(call_list_rwv);
      else if ((s->people[0].id1 & 017) == 012 &&
               (s->people[1].id1 & 017) == 010 &&
               (s->people[2].id1 & 017) == 012 &&
               (s->people[3].id1 & 017) == 010 &&
               (s->people[4].id1 & 017) == 010 &&
               (s->people[5].id1 & 017) == 012 &&
               (s->people[6].id1 & 017) == 010 &&
               (s->people[7].id1 & 017) == 012)
         return(call_list_lwv);
      else if ((s->people[0].id1 & 017) == 010 &&
               (s->people[1].id1 & 017) == 010 &&
               (s->people[2].id1 & 017) == 012 &&
               (s->people[3].id1 & 017) == 012 &&
               (s->people[4].id1 & 017) == 012 &&
               (s->people[5].id1 & 017) == 012 &&
               (s->people[6].id1 & 017) == 010 &&
               (s->people[7].id1 & 017) == 010)
         return(call_list_r2fl);
      else if ((s->people[0].id1 & 017) == 012 &&
               (s->people[1].id1 & 017) == 012 &&
               (s->people[2].id1 & 017) == 010 &&
               (s->people[3].id1 & 017) == 010 &&
               (s->people[4].id1 & 017) == 010 &&
               (s->people[5].id1 & 017) == 010 &&
               (s->people[6].id1 & 017) == 012 &&
               (s->people[7].id1 & 017) == 012)
         return(call_list_l2fl);
   }
   else if (s->kind == s_qtag)
      return(call_list_qtag);

   return(call_list_any);
}


extern callarray *assoc(begin_kind key, setup *ss, callarray *spec)
{
   callarray *p;
   int t, u, i, k, mask;

   p = spec;
   for (;;) {
      if (p == 0) goto good;
      if (p->start_setup == key) {
         if (p->qualifier == sq_none) goto good;
         /* Can't be bothered to figure out what setup to create when
            calling this during initialization, so we send nil. */
         if (!ss) goto good;

         /* Note that we have to examine setups larger than the setup the
            qualifier is officially defined for.  If a qualifier were defined
            as being legal only on 1x4's (so that, in the database, we only had
            specifications of the sort "setup 1x4 1x4 qualifier wave_only") we could
            still find ourselves here with ss->kind equal to s2x4.  Why?  Because
            the setup could be a 2x4 and the splitter could be trying to decide
            whether to split the setup into parallel 1x4's.  This happens when
            trying to figure out whether to split a 2x4 into 1x4's or 2x2's for
            the call "recycle". */

         k = 0;   /* Many tests will find these values useful. */
         i = 2;

         switch(p->qualifier) {
            case sq_wave_only:                    /* 1x4 or 2x4 - wave */
               switch (ss->kind) {
                  case s1x4:
                     if (t = ss->people[0].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[1].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[2].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[3].id1) { k |=  t; i &=  t; }
                     if (!(k & ~i & 2)) goto good;
                     goto bad;
                  case s2x4:
                     if (t = ss->people[0].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[1].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[2].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[3].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[4].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[5].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[6].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[7].id1) { k |=  t; i &=  t; }
                     if (!(k & ~i & 2)) goto good;
                     goto bad;
                  default:
                     goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_2fl_only:                     /* 1x4 or 2x4 - 2FL */
               switch (ss->kind) {
                  case s1x4:
                     if (t = ss->people[0].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[1].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[2].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[3].id1) { k |= ~t; i &= ~t; }
                     if (!(k & ~i & 2)) goto good;
                     goto bad;
                  case s2x4:
                     if (t = ss->people[0].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[1].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[2].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[3].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[4].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[5].id1) { k |= ~t; i &= ~t; }
                     if (t = ss->people[6].id1) { k |=  t; i &=  t; }
                     if (t = ss->people[7].id1) { k |=  t; i &=  t; }
                     if (!(k & ~i & 2)) goto good;
                     goto bad;
                  default:
                     goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_miniwaves:                    /* miniwaves everywhere */
               switch (ss->kind) {
                  case s_1x2:
                     if ((t = ss->people[0].id1) & (u = ss->people[1].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     goto bad;
                  case s1x4:
                     if ((t = ss->people[0].id1) & (u = ss->people[1].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[3].id1) & (u = ss->people[2].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     goto bad;
                  case s1x8:
                     if ((t = ss->people[0].id1) & (u = ss->people[1].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[3].id1) & (u = ss->people[2].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[6].id1) & (u = ss->people[7].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[5].id1) & (u = ss->people[4].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     goto bad;
                  case s2x4:
                     if ((t = ss->people[0].id1) & (u = ss->people[1].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[2].id1) & (u = ss->people[3].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[5].id1) & (u = ss->people[4].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[7].id1) & (u = ss->people[6].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     k = 1;
                     i = 2;
                     if ((t = ss->people[0].id1) & (u = ss->people[7].id1)) { k &= t&u; i &= t^u; }
                     if ((t = ss->people[1].id1) & (u = ss->people[6].id1)) { k &= t&u; i &= t^u; }
                     if ((t = ss->people[2].id1) & (u = ss->people[5].id1)) { k &= t&u; i &= t^u; }
                     if ((t = ss->people[3].id1) & (u = ss->people[4].id1)) { k &= t&u; i &= t^u; }
                     if ((i & 2) && (k & 1)) goto good;
                     goto bad;
                  case s2x2:
                     if ((t = ss->people[0].id1) & (u = ss->people[1].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[3].id1) & (u = ss->people[2].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     k = 1;
                     i = 2;
                     if ((t = ss->people[0].id1) & (u = ss->people[3].id1)) { k &= t&u; i &= t^u; }
                     if ((t = ss->people[1].id1) & (u = ss->people[2].id1)) { k &= t&u; i &= t^u; }
                     if ((i & 2) && (k & 1)) goto good;
                     goto bad;
                  case sdmd:
                     k = 1;
                     i = 2;
                     if ((t = ss->people[1].id1) & (u = ss->people[3].id1)) { k &= t&u; i &= t^u; }
                     if ((i & 2) && (k & 1)) goto good;
                     goto bad;
                  case s_trngl:
                     if ((t = ss->people[1].id1) & (u = ss->people[2].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     goto bad;
                  case s_qtag:
                     if ((t = ss->people[6].id1) & (u = ss->people[7].id1)) { k |= t|u; i &= t^u; }
                     if ((t = ss->people[3].id1) & (u = ss->people[2].id1)) { k |= t|u; i &= t^u; }
                     if ((i & 2) && !(k & 1)) goto good;
                     goto bad;
                  default:
                     goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_rwave_only:
               if (ss->kind == s_1x2) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_south))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s1x4) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_south))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s1x8) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_south))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s2x4) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_north))
                  goto good;
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_west))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s2x2) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_north))
                  goto good;
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_west))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == sdmd) {
                  if ((!(t = ss->people[1].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_west))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s_trngl) {
                  if ((!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_south))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s_qtag) {
                  if ((!(t = ss->people[2].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_south))
                  goto good;
                  goto bad;
               }
               else {
                  goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_lwave_only:
               if (ss->kind == s_1x2) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_north))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s1x4) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_north))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s1x8) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_north))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s2x4) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_south))
                  goto good;
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[4].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[5].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_east))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s2x2) {
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_south))
                  goto good;
                  if ((!(t = ss->people[0].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[1].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_east) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_east))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == sdmd) {
                  if ((!(t = ss->people[1].id1 & d_mask) || t == d_west) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_east))
                  goto good;
                  goto bad;
               }
               if (ss->kind == s_trngl) {
                  if ((!(t = ss->people[1].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[2].id1 & d_mask) || t == d_north))
                  goto good;
                  goto bad;
               }
               else if (ss->kind == s_qtag) {
                  if ((!(t = ss->people[2].id1 & d_mask) || t == d_north) &&
                      (!(t = ss->people[3].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[6].id1 & d_mask) || t == d_south) &&
                      (!(t = ss->people[7].id1 & d_mask) || t == d_north))
                  goto good;
                  goto bad;
               }
               else {
                  goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_ctrwv_end2fl:
               /* Note that this qualifier is kind of strict.  We won't permit the call "with
                  confidence" do be done unless everyone can trivially determine which
                  part to do. */
               if (ss->kind == s_crosswave) {
                  if (((ss->people[0].id1 ^ ss->people[1].id1) & d_mask) == 0 &&
                      ((ss->people[4].id1 ^ ss->people[5].id1) & d_mask) == 0 &&
                      ((ss->people[2].id1 | ss->people[3].id1) == 0 || ((ss->people[2].id1 ^ ss->people[3].id1) & d_mask) == 2) &&
                      ((ss->people[6].id1 | ss->people[7].id1) == 0 || ((ss->people[6].id1 ^ ss->people[7].id1) & d_mask) == 2))
                  goto good;
                  goto bad;
               }
               else {
                  goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_ctr2fl_endwv:
               /* Note that this qualifier is kind of strict.  We won't permit the call "with
                  confidence" do be done unless everyone can trivially determine which
                  part to do. */
               if (ss->kind == s_crosswave) {
                  if (((ss->people[2].id1 ^ ss->people[3].id1) & d_mask) == 0 &&
                      ((ss->people[6].id1 ^ ss->people[7].id1) & d_mask) == 0 &&
                      ((ss->people[0].id1 | ss->people[1].id1) == 0 || ((ss->people[0].id1 ^ ss->people[1].id1) & d_mask) == 2) &&
                      ((ss->people[4].id1 | ss->people[5].id1) == 0 || ((ss->people[4].id1 ^ ss->people[5].id1) & d_mask) == 2))
                  goto good;
                  goto bad;
               }
               else {
                  goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_true_Z:                    /* 2x3, 3x4, or 2x6 - real Z spots occupied, so can do Z axle */
               mask = 0;

               for (i=0, k=1; i<=setup_limits[ss->kind]; i++, k<<=1) {
                  if (ss->people[i].id1) mask |= k;
               }

               if (ss->kind == s_2x3) {
                  if (mask == 033 || mask == 066) goto good;
                  goto bad;
               }
               else if (ss->kind == s3x4) {
                  if (mask == 0xEBA || mask == 0xD75) goto good;
                  goto bad;
               }
               else if (ss->kind == s2x6) {
                  if (mask == 03333 || mask == 06666) goto good;
                  goto bad;
               }
               else {
                  goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_3_4_tag:                      /* dmd or qtag - is a 3/4 tag, i.e. points looking out */
               switch (ss->kind) {
                  case sdmd:
                     if ((!(t = ss->people[0].id1 & d_mask) || t == d_west) &&
                         (!(t = ss->people[2].id1 & d_mask) || t == d_east))
                     goto good;
                     goto bad;
                  case s_qtag:
                     if ((!(t = ss->people[0].id1 & d_mask) || t == d_north) &&
                         (!(t = ss->people[1].id1 & d_mask) || t == d_north) &&
                         (!(t = ss->people[4].id1 & d_mask) || t == d_south) &&
                         (!(t = ss->people[5].id1 & d_mask) || t == d_south))
                     goto good;
                     goto bad;
                  default:
                     goto good;                 /* We don't understand the setup -- we'd better accept it. */
               }
            case sq_dmd_same_pt:                   /* dmd or pdmd - centers would circulate to same point */
               if (((ss->people[1].id1 & 01011) == d_east) &&                        /* faces either east or west */
                   (!((ss->people[3].id1 ^ ss->people[1].id1) & d_mask)))   /* and both face same way */
                  goto good;
               goto bad;
            case sq_dmd_facing:                    /* dmd or pdmd - diamond is fully occupied and fully facing */
               if ((ss->people[0].id1 & d_mask) == d_north &&
                   (ss->people[1].id1 & d_mask) == d_west &&
                   (ss->people[2].id1 & d_mask) == d_south &&
                   (ss->people[3].id1 & d_mask) == d_east)
                  goto good;
               if ((ss->people[0].id1 & d_mask) == d_south &&
                   (ss->people[1].id1 & d_mask) == d_east &&
                   (ss->people[2].id1 & d_mask) == d_north &&
                   (ss->people[3].id1 & d_mask) == d_west)
                  goto good;
               goto bad;
         }
      }
      bad:
      p = p->next;
   }

   good:
   return(p);
}


extern unsigned int find_calldef(
   callarray *tdef,
   setup *scopy,
   int real_index,
   int real_direction,
   int northified_index)
{
   unsigned short *calldef_array;
   predptr_pair *predlistptr;
   unsigned int z;

   if (tdef->callarray_flags & CAF__PREDS) {
      predlistptr = tdef->stuff.prd.predlist;
      while (predlistptr != (predptr_pair *) 0) {
         if ((*(predlistptr->pred))(scopy, real_index, real_direction, northified_index)) {
            calldef_array = predlistptr->arr;
            goto got_it;
         }
         predlistptr = predlistptr->next;
      }
      fail(tdef->stuff.prd.errmsg);
   }
   else
      calldef_array = tdef->stuff.def;

got_it:

   z = calldef_array[northified_index];
   if (!z) {
      collision_person1 = scopy->people[real_index].id1;
      error_message1[0] = '\0';
      error_message2[0] = '\0';
      longjmp(longjmp_ptr->the_buf, 6);
   }

   return(z);
}


extern void clear_people(setup *z)
{
   int i;

   for (i=0; i<MAX_PEOPLE; i++) {
      z->people[i].id1 = 0;
      z->people[i].id2 = 0;
   }
}


extern unsigned int rotperson(unsigned int n, int amount)
{
   if (n == 0) return(0); else return((n + amount) & ~064);
}


extern unsigned int rotcw(unsigned int n)
{
   if (n == 0) return(0); else return((n + 011) & ~064);
}


extern unsigned int rotccw(unsigned int n)
{
   if (n == 0) return(0); else return((n + 033) & ~064);
}


extern void clear_person(setup *resultpeople, int resultplace)
{
   resultpeople->people[resultplace].id1 = 0;
   resultpeople->people[resultplace].id2 = 0;
}


extern unsigned int copy_person(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace)
{
   resultpeople->people[resultplace] = sourcepeople->people[sourceplace];
   return(resultpeople->people[resultplace].id1);
}


extern unsigned int copy_rot(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace, int rotamount)
{
   unsigned int newperson = sourcepeople->people[sourceplace].id1;

   if (newperson) newperson = (newperson + rotamount) & ~064;
   resultpeople->people[resultplace].id2 = sourcepeople->people[sourceplace].id2;
   return(resultpeople->people[resultplace].id1 = newperson);
}


extern void swap_people(setup *ss, int oneplace, int otherplace)
{
   personrec temp = ss->people[otherplace];
   ss->people[otherplace] = ss->people[oneplace];
   ss->people[oneplace] = temp;
}


extern void install_person(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace)
{
   unsigned int newperson = sourcepeople->people[sourceplace].id1;

   if (resultpeople->people[resultplace].id1 == 0)
      resultpeople->people[resultplace] = sourcepeople->people[sourceplace];
   else if (newperson) {
      collision_person1 = resultpeople->people[resultplace].id1;
      collision_person2 = newperson;
      error_message1[0] = '\0';
      error_message2[0] = '\0';
      longjmp(longjmp_ptr->the_buf, 3);
   }
}


extern void install_rot(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace, int rotamount)
{
   unsigned int newperson = sourcepeople->people[sourceplace].id1;

   if (newperson) {
      if (resultpeople->people[resultplace].id1 == 0) {
         resultpeople->people[resultplace].id1 = (newperson + rotamount) & ~064;
         resultpeople->people[resultplace].id2 = sourcepeople->people[sourceplace].id2;
      }
      else {
         collision_person1 = resultpeople->people[resultplace].id1;
         collision_person2 = newperson;
         error_message1[0] = '\0';
         error_message2[0] = '\0';
         longjmp(longjmp_ptr->the_buf, 3);
      }
   }
}


/* Take a concept pointer and scan for all "final" concepts,
   returning an updated concept pointer and a mask of all such concepts found.
   "Final" concepts are those that modify the execution of a call but
   do not cause it to be executed in a virtual or distorted setup.
   This has a side-effect that is occasionally used:  When it passes over
   any "magic" or "interlocked" concept, it drops a pointer to where the
   last such occurred into the external variable "last_magic_diamond". */

extern parse_block *process_final_concepts(
   parse_block *cptr,
   long_boolean check_errors,
   final_set *final_concepts)
{
   parse_block *tptr = cptr;

   *final_concepts = 0;

   while (tptr) {
      final_set bit_to_set = 0;

      switch (tptr->concept->kind) {
         case concept_comment:
            break;               /* Need to skip these. */
         case concept_triangle:
            bit_to_set = FINAL__TRIANGLE; break;
         case concept_magic:
            last_magic_diamond = tptr;
            if (check_errors && (*final_concepts & (FINAL__SINGLE | FINAL__DIAMOND)))
               fail("Modifiers specified in illegal order.");
            bit_to_set = FINAL__MAGIC; break;
         case concept_grand:
            if (check_errors && (*final_concepts & FINAL__SINGLE))
               fail("Modifiers specified in illegal order.");
            bit_to_set = FINAL__GRAND; break;
         case concept_cross:
            bit_to_set = FINAL__CROSS; break;
         case concept_single:
            bit_to_set = FINAL__SINGLE; break;
         case concept_interlocked:
            last_magic_diamond = tptr;
            if (check_errors && (*final_concepts & (FINAL__SINGLE | FINAL__DIAMOND)))
               fail("Modifiers specified in illegal order.");
            bit_to_set = FINAL__INTERLOCKED; break;
         case concept_split:
            bit_to_set = FINAL__SPLIT; break;
         case concept_reverse:
            bit_to_set = FINAL__REVERSE; break;
         case concept_left:
            bit_to_set = FINAL__LEFT; break;
         case concept_12_matrix:
            bit_to_set = FINAL__12_MATRIX; break;
         case concept_diamond:
            if (check_errors && (*final_concepts & FINAL__SINGLE))
               fail("Modifiers specified in illegal order.");
            bit_to_set = FINAL__DIAMOND; break;
         case concept_funny:
            bit_to_set = FINAL__FUNNY; break;
         default:
            goto exit5;
      }

      if (check_errors && (*final_concepts & bit_to_set))
         fail("Redundant call modifier.");
      *final_concepts |= bit_to_set;

      tptr = tptr->next;
   }
   exit5:

   return(tptr);
}


/* Prepare several setups to be assembled into one, by making them all have
   the same kind and rotation. */
extern long_boolean fix_n_results(int arity, setup z[])
{
   int i;
   long_boolean lineflag = FALSE;
   long_boolean miniflag = FALSE;
   setup_kind kk = nothing;
   int rr = -1;

   /* There are 3 things that make this task nontrivial.  First, some setups could
      be "nothing", in which case we turn them into the same type of setup as
      their neighbors, with no people.  Second, some types of "grand working"
      can leave setups confused about whether they are lines or diamonds,
      because only the ends/points are occupied.  We turn those setups into
      whatever matches their more-fully-occupied neighbors.  (If completely in
      doubt, we opt for 1x4's.)  Third, some 1x4's may have been shrunk to 1x2's.
      This can happen in some cases of "triple box patch the so-and-so", which lead
      to triple lines in which the outer lines collapsed from 1x4's to 1x2's while
      the center line is fully occupied.  In this case we repopulate the outer lines
      to 1x4's. */

   for (i=0; i<arity; i++) {
      if (z[i].kind != nothing) {
         canonicalize_rotation(&z[i]);

         if (z[i].kind == s_1x2)
            miniflag = TRUE;
         else if ((z[i].kind == s1x4 || z[i].kind == sdmd) && (z[i].people[1].id1 | z[i].people[3].id1) == 0)
            lineflag = TRUE;
         else {
            if (kk == nothing) kk = z[i].kind;
            if (kk != z[i].kind) goto lose;
         }

         if (rr < 0) rr = z[i].rotation;
         if (rr != z[i].rotation) goto lose;
      }
   }

   if (kk == nothing) {
      if (lineflag) kk = s1x4;
      else if (miniflag) kk = s_1x2;
      else return(TRUE);
   }
   
   /* If something wasn't sure whether it was points of a diamond or
      ends of a 1x4, that's OK if something else had a clue. */
   if (lineflag && kk != s1x4 && kk != sdmd) goto lose;

   /* If something was a 1x2, that's OK if something else was a 1x4. */
   if (miniflag && kk != s1x4 && kk != s_1x2) goto lose;

   for (i=0; i<arity; i++) {
      if (z[i].kind == nothing)
         clear_people(&z[i]);
      else if (z[i].kind == s_1x2 && kk == s1x4) {
         /* We have to expand a 1x2 to the center spots of a 1x4. */
         (void) copy_person(&z[i], 3, &z[i], 1);
         clear_person(&z[i], 2);
         (void) copy_person(&z[i], 1, &z[i], 0);
         clear_person(&z[i], 0);
      }

      z[i].kind = kk;
      z[i].rotation = rr;
   }

   return(FALSE);

   lose:

   fail("This is a ridiculously inconsistent shape or orientation changer!!");
   /* NOTREACHED */
}


static void innards(
   setup *ss,
   parse_block *parseptr,
   callspec_block *callspec,
   final_set final_concepts,
   map_thing *maps,
   long_boolean recompute_id,
   setup *a1,
   setup *a2,
   setup *a3,
   setup *a4,
   setup *result)
{
   int i, r;
   map_thing *final_map;
   map_hunk *hunk;
   setup *x[4];
   setup z[4];

   int finalsetupflags = 0;
   setup_kind kn = maps->inner_kind;
   int rot = maps->rot;
   int vert = maps->vert;
   int arity = maps->arity;

   clear_people(result);
   
   x[0] = a1;
   x[1] = a2;
   x[2] = a3;
   x[3] = a4;
   
   for (i=0; i<arity; i++) {
      /* It is clearly too late to expand the matrix -- that can't be what is wanted. */
      x[i]->setupflags = (ss->setupflags & ~SETUPFLAG__OFFSET_Z) | SETUPFLAG__DISTORTED | SETUPFLAG__NO_EXPAND_MATRIX;
      x[i]->kind = kn;
      x[i]->rotation = 0;
      if (recompute_id) update_id_bits(x[i]);
      move(x[i], parseptr, callspec, final_concepts, FALSE, &z[i]);
      finalsetupflags |= z[i].setupflags;
   }

   if (fix_n_results(arity, z)) {
      result->kind = nothing;
      return;
   }
   
   /* Set the final setupflags to the OR of everything that happened.
      The PAR_CONC_END flag doesn't matter --- if the result is a 2x2
      begin done around the outside, the procedure that called us
      (basic_move) knows what is happening and will fix that bit. */

   result->setupflags = finalsetupflags;

   /* Some maps (the ones used in "triangle peel and trail") do not want the result
      to be reassembled, so we get out now.  These maps are indicated by arity = 1
      and map3[1] nonzero. */

   if ((arity == 1) && (maps->map3[1])) {
      *result = z[0];
      goto getout;
   }

   /* See if we can put things back with the same map we used before. */

   if (z[0].kind == kn && (z[0].rotation&3) == 0) {
      final_map = maps;
      result->rotation = 0;
      goto finish;
   }

   /* If this is a special map that flips the second setup upside-down, do so. */
   if (rot == 2) {
      z[1].rotation += 2;
      canonicalize_rotation(&z[1]);
   }
   else if (rot == 3) {    /* or the first setup */
      z[0].rotation += 2;
      canonicalize_rotation(&z[0]);
   }

   z[0].rotation += (rot & 1) + vert;
   z[1].rotation += (rot & 1) + vert;
   z[2].rotation += (rot & 1) + vert;
   z[3].rotation += (rot & 1) + vert;

   /* Do various special things. */

   switch (maps->map_kind) {
      case MPKIND__4_QUADRANTS:
         /* These particular maps misrepresent the rotation of subsetups 2 and 4, so
            we have to repair things when a shape-changer is called. */
         z[1].rotation += 2;
         z[3].rotation += 2;
         break;
      case MPKIND__DMD_STUFF:
         /* These particular maps misrepresent the rotation of subsetup 2, so
            we have to repair things when a shape-changer is called. */
         z[1].rotation += 2;
         break;
      case MPKIND__O_SPOTS:
         warn(warn__to_o_spots);
         break;
      case MPKIND__X_SPOTS:
         warn(warn__to_x_spots);
         break;
      case MPKIND__NONE:
         fail("Can't do shape changer with complex line/box/column/diamond identification concept.");
   }

   for (i=0; i<arity; i++)
      canonicalize_rotation(&z[i]);

   final_map = 0;
   hunk = map_lists[z[0].kind][arity-1];
   if (hunk) final_map = (*hunk)[maps->map_kind][(z[0].rotation & 1)];

   if (z[0].rotation & 2) {
      if (final_map == &map_s6_trngl) final_map = &map_b6_trngl;
      else final_map = 0;        /* Raise an error. */
   }

   if ((ss->setupflags & SETUPFLAG__OFFSET_Z) && final_map && (maps->map_kind == MPKIND__OFFS_L_HALF || maps->map_kind == MPKIND__OFFS_R_HALF)) {
      if (final_map->outer_kind == s2x6) warn(warn__check_pgram);
      else final_map = 0;        /* Raise an error. */
   }

   if (!final_map) {
      if (arity == 1)
         fail("Don't know how far to re-offset this.");
      else
         fail("Can't do shape changer with complex line/box/column/diamond identification concept.");
   }

   result->rotation = z[0].rotation;
   if ((z[0].rotation & 1) && (final_map->rot & 1))
      result->rotation = 0;

   result->rotation -= vert;

   /* For single arity maps, nonzero map3 item means to give warning. */
   if ((arity == 1) && (final_map->map3[0])) warn(warn__offset_gone);
   /* For triple arity maps, nonzero map4 item means to give warning. */
   if ((arity == 3) && (final_map->map4[0])) warn(warn__overlap_gone);

   /* If this is a special map that expects the second setup to have been flipped upside-down, do so. */
   if (final_map->rot == 2) {
      z[1].rotation += 2;
      canonicalize_rotation(&z[1]);
   }
   else if (final_map->rot == 3) {    /* or the first setup */
      z[0].rotation += 2;
      canonicalize_rotation(&z[0]);
   }

   finish:

   if (arity != final_map->arity) fail("Confused about number of setups to divide into.");

   rot = final_map->rot;
   r = rot * 011;

   for (i=0; i<=setup_limits[final_map->inner_kind]; i++) {
      int t;

      if (rot & 1) {
         install_rot(result, final_map->map1[i], &z[0], i, r);
         if (maps->map_kind == MPKIND__4_QUADRANTS) {
            install_person(result, final_map->map2[i], &z[1], i);
            install_rot(result, final_map->map3[i], &z[2], i, 011);
            install_person(result, final_map->map4[i], &z[3], i);
         }
         else if (maps->map_kind == MPKIND__DMD_STUFF) {
            install_person(result, final_map->map2[i], &z[1], i);
         }
         else {
            if (final_map->arity >= 2) install_rot(result, final_map->map2[i], &z[1], i, 011);
            if (final_map->arity >= 3) install_rot(result, final_map->map3[i], &z[2], i, 011);
            if (final_map->arity == 4) install_rot(result, final_map->map4[i], &z[3], i, 011);
         }
      }
      else {
         t = final_map->map1[i];

         if (t >= 0)
            install_person(result, t, &z[0], i);
         else if (z[0].people[i].id1 & BIT_PERSON)
            fail("This would go into an excessively large matrix.");

         if (maps->map_kind == MPKIND__4_QUADRANTS) {
            install_rot(result, final_map->map2[i], &z[1], i, 011);
            install_person(result, final_map->map3[i], &z[2], i);
            install_rot(result, final_map->map4[i], &z[3], i, 011);
         }
         else if (maps->map_kind == MPKIND__DMD_STUFF) {
            install_rot(result, final_map->map2[i], &z[1], i, 011);
         }
         else {
            if (final_map->arity >= 2) {
               t = final_map->map2[i];
   
               if (t >= 0)
                  install_rot(result, t, &z[1], i, r);
               else if (z[1].people[i].id1 & BIT_PERSON)
                  fail("This would go into an excessively large matrix.");
            }
   
            if (final_map->arity >= 3) {
               t = final_map->map3[i];
      
               if (t >= 0)
                  install_person(result, t, &z[2], i);
               else if (z[2].people[i].id1 & BIT_PERSON)
                  fail("This would go into an excessively large matrix.");
            }
   
            if (final_map->arity == 4) {
               t = final_map->map4[i];
      
               if (t >= 0)
                  install_person(result, t, &z[3], i);
               else if (z[3].people[i].id1 & BIT_PERSON)
                  fail("This would go into an excessively large matrix.");
            }
         }
      }
   }

   result->kind = final_map->outer_kind;

   getout:

   canonicalize_rotation(result);
   reinstate_rotation(ss, result);
}


extern void divided_setup_move(
   setup *ss,
   parse_block *parseptr,
   callspec_block *callspec,
   final_set final_concepts,
   map_thing *maps,
   phantest_kind phancontrol,
   long_boolean recompute_id,
   setup *result)
{
   int i, mm, v1flag, v2flag, v3flag, v4flag;
   setup a1, a2, a3, a4;

   setup_kind kn = maps->inner_kind;
   int rot = maps->rot;
   int arity = maps->arity;
   
   v1flag = 0;
   v2flag = 0;
   v3flag = 0;
   v4flag = 0;

   for (i=0; i<=setup_limits[kn]; i++) {
      setup tstuff;
      clear_people(&tstuff);

      mm = maps->map1[i];
      if (mm >= 0)
         tstuff.people[0] = ss->people[mm];
      v1flag |= tstuff.people[0].id1;

      if (arity >= 2) {
         mm = maps->map2[i];
         if (mm >= 0)
            tstuff.people[1] = ss->people[mm];
         v2flag |= tstuff.people[1].id1;
      }

      if (arity >= 3) {
         mm = maps->map3[i];
         if (mm >= 0)
            tstuff.people[2] = ss->people[mm];
         v3flag |= tstuff.people[2].id1;
      }

      if (arity == 4) {
         mm = maps->map4[i];
         if (mm >= 0)
            tstuff.people[3] = ss->people[mm];
         v4flag |= tstuff.people[3].id1;
      }

      if (rot & 1) {
         /* Rotation is odd.  3 is a special case. */
         (void) copy_rot(&a1, i, &tstuff, 0, (rot==3 ? 011 : 033));
         if (maps->map_kind == MPKIND__4_QUADRANTS) {
            (void) copy_person(&a2, i, &tstuff, 1);
            (void) copy_rot(&a3, i, &tstuff, 2, 033);
            (void) copy_person(&a4, i, &tstuff, 3);
         }
         else if (maps->map_kind == MPKIND__DMD_STUFF) {
            (void) copy_person(&a2, i, &tstuff, 1);
         }
         else {
            if (arity >= 2) (void) copy_rot(&a2, i, &tstuff, 1, 033);
            if (arity >= 3) (void) copy_rot(&a3, i, &tstuff, 2, 033);
            if (arity == 4) (void) copy_rot(&a4, i, &tstuff, 3, 033);
         }
      }
      else {
         /* Rotation is even.  2 is a special case. */
         (void) copy_person(&a1, i, &tstuff, 0);
         if (maps->map_kind == MPKIND__4_QUADRANTS) {
            (void) copy_rot(&a2, i, &tstuff, 1, 033);
            (void) copy_person(&a3, i, &tstuff, 2);
            (void) copy_rot(&a4, i, &tstuff, 3, 033);
         }
         else if (maps->map_kind == MPKIND__DMD_STUFF) {
            (void) copy_rot(&a2, i, &tstuff, 1, 033);
         }
         else {
            if (arity >= 2) {
               if (rot == 2) {
                  (void) copy_rot(&a2, i, &tstuff, 1, 022);
               }
               else {
                  (void) copy_person(&a2, i, &tstuff, 1);
               }
            }
            if (arity >= 3) (void) copy_person(&a3, i, &tstuff, 2);
            if (arity == 4) (void) copy_person(&a4, i, &tstuff, 3);
         }
      }
   }

   switch (phancontrol) {
      case phantest_impossible:
         if (!(v1flag && v2flag))
            fail("This is impossible in a symmetric setup!!!!");
         break;
      case phantest_both:
         if (!(v1flag && v2flag))
            /* Only one of the two setups is occupied. */
            fail("Don't use phantom concept if you don't mean it.");
         break;
      case phantest_only_one:
         if (v1flag && v2flag) fail("Can't find the setup to work in.");
         break;
      case phantest_only_first_one:
         if (v2flag) fail("Not in correct setup.");
         break;
      case phantest_only_second_one:
         if (v1flag) fail("Not in correct setup.");
         break;
      case phantest_first_or_both:
         if (!v1flag)
            fail("Don't use phantom concept if you don't mean it.");
         break;
      case phantest_2x2_both:
         /* Test for "C1" blocks. */
         if (!((v1flag | v3flag) && (v2flag | v4flag)))
            fail("Don't use phantom concept if you don't mean it.");
         break;
      case phantest_not_just_centers:
         if (!(v1flag && v3flag))
            fail("Don't use phantom concept if you don't mean it.");
         break;
      case phantest_2x2_only_two:
         /* Test for NOT "C1" blocks. */
         if ((v1flag | v3flag) && (v2flag | v4flag)) fail("Not in blocks.");
         break;
   }

   innards(ss, parseptr, callspec,
      final_concepts, maps, recompute_id,
      &a1, &a2, &a3, &a4, result);
}


extern void overlapped_setup_move(setup *s, map_thing *maps,
   int m1, int m2, int m3, parse_block *parseptr, setup *result)
{
   int i, j;
   setup a1, a2, a3;

   setup_kind kn = maps->inner_kind;
   int rot = maps->rot;
   int arity = maps->arity;

   if (arity >= 4) fail("Can't handle this many overlapped setups.");

   for (i=0, j=1; i<=setup_limits[kn]; i++, j<<=1) {
      setup tstuff;

      tstuff.people[0] = s->people[maps->map1[i]];
      if (arity >= 2) tstuff.people[1] = s->people[maps->map2[i]];
      if (arity == 3) tstuff.people[2] = s->people[maps->map3[i]];

      if (rot & 1) {
         /* Rotation is odd.  3 is a special case. */
         if (rot == 3) tstuff.people[0].id1 = rotcw(tstuff.people[0].id1); else tstuff.people[0].id1 = rotccw(tstuff.people[0].id1);
         if (arity >= 2) tstuff.people[1].id1 = rotccw(tstuff.people[1].id1);
         if (arity == 3) tstuff.people[2].id1 = rotccw(tstuff.people[2].id1);
      }
      else {
         /* Rotation is even.  2 is a special case. */
         if (rot == 2) tstuff.people[1].id1 = rotperson(tstuff.people[1].id1, 022);
      }

      if (j & m1)
         (void) copy_person(&a1, i, &tstuff, 0);
      else
         clear_person(&a1, i);

      if (arity >= 2) {
         if (j & m2)
            (void) copy_person(&a2, i, &tstuff, 1);
         else
            clear_person(&a2, i);
      }

      if (arity >= 3) {
         if (j & m3)
            (void) copy_person(&a3, i, &tstuff, 2);
         else
            clear_person(&a3, i);
      }
   }

   innards(s, parseptr, NULLCALLSPEC, 0, maps,
      FALSE, &a1, &a2, &a3, &a3, result);
/* Note: we pass garbage    ^^^   for a4. */

}
