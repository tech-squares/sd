/* 
 *  sdui-mac.c - SD Macintosh User Interface (main entry points)
 *
 *  Copyright (C) 1992, 1993 Alan Snyder
 *
 *  Permission to use, copy, modify, and distribute this software for
 *  any purpose is hereby granted without fee, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *  The author makes no representations about the suitability of this
 *  software for any purpose.  It is provided "as is" WITHOUT ANY
 *  WARRANTY, without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  
 *
 *  For use with version 30 of the Sd program.
 *
 */


/*
 *  The version of this file is as shown immediately below.  This string
 *  gets displayed at program startup, as the "ui" part of the complete
 *  version.
 *
 */

static char *sdui_version = "2.2";

/* This file defines the following functions:
   uims_process_command_line
   uims_version_string
   uims_preinitialize
   uims_add_call_to_menu
   uims_finish_call_menu
   uims_postinitialize
   uims_get_command
   uims_do_comment_popup
   uims_do_outfile_popup
   uims_do_getout_popup
   uims_do_abort_popup
   uims_do_neglect_popup
   uims_do_selector_popup
   uims_do_direction_popup
   uims_get_number_fields
   uims_do_modifier_popup
   uims_add_new_line
   uims_reduce_line_count
   uims_update_resolve_menu
   uims_terminate
*/

#include "macguts.h"
#include <stdio.h> /* for sprintf */
#include <string.h> /* for strcpy */

static char *call_menu_names[NUM_CALL_LIST_KINDS];

/*
 * The total version string looks something like
 * "1.4:db1.5:ui0.6tty"
 * We return the "0.6tty" part.
 */

static char version_mem[12];

extern char *
uims_version_string(void)
{
    (void) sprintf(version_mem, "%smac", sdui_version);
    return version_mem;
}

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
   mac_initialize();
   *argcp = getcommand(argvp); /* pop up window to get command line arguments */
}

/*
 * The main program calls this before any of the call menus are
 * created, that is, before any calls to uims_add_call_to_menu
 * or uims_finish_call_menu.
 */
 
extern void
uims_preinitialize(void)
{
}

/*
 * We have been given the name of one call (call number
 * call_menu_index, from 0 to number_of_calls[cl]) to be added to the
 * call menu cl (enumerated over the type call_list_kind.)
 * The string is guaranteed to be in stable storage.
 */
 
extern void
uims_add_call_to_menu(call_list_kind cl, int call_menu_index, char name[])
{
    matcher_add_call_to_menu(cl, call_menu_index, name);
}

/*
 * Create a menu containing number_of_calls[cl] items, which are the
 * items whose text strings were previously transmitted by the calls
 * to uims_add_call_to_menu.  Use the "menu_name" argument to create a
 * title line for the menu.  The string is in static storage.
 * 
 * This will be called once for each value in the enumeration call_list_kind.
 */

extern void
uims_finish_call_menu(call_list_kind cl, char menu_name[])
{
   call_menu_names[cl] = menu_name;
}

/* The main program calls this after all the call menus have been created,
   after all calls to uims_add_call_to_menu and uims_finish_call_menu.
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
    call_menu_names[call_list_empty] = "";
    matcher_initialize(TRUE); /* want commands last in menu */
}

static match_result user_match;

/* BEWARE!!  The first part of this list is keyed to the definition of
   "command_kind" in sd.h .
   There are NUM_COMMAND_KINDS items in that part.  The rest of it
   corresponds to the special commands defined in sdmatch.h .  There
   are NUM_SPECIAL_COMMANDS of those items. */

static char *command_list[] = {
    "quit the program",      /* This is different from sdtty. */
    "undo last call",
    "end this sequence",     /* This is different from sdtty. */
    "insert a comment ...",
    "save as ...",
    "*",                     /* "end this sequence" is not used on the Mac. */
    "resolve ...",
    "reconcile ...",
    "pick random call ...",
    "normalize ...",
#ifdef NEGLECT
    "show neglected calls ...",
#endif
    "keep picture",
    "refresh display",
/* The following items are the special ones. */
    "modify next call",      /* This is different from sdtty. */
    "toggle concept levels"
};

#define NUM_SPECIAL_COMMANDS 2
#define SPECIAL_COMMAND_ALLOW_MODS 0
#define SPECIAL_COMMAND_TOGGLE_CONCEPT_LEVELS 1



/* result is indicated in user_match */

static void
get_user_command(int which)
{
    char user_input[200];
    char extended_input[200];
    char *p;
    int c;
    int matches;
    static match_result menu_match; /* for commands entered via a menu */
    
    input_clear();
    user_input[0] = 0;
    user_match.valid = FALSE;
    match_update(which);

    for (;;) {
        c = get_user_input(user_input, &menu_match);
        if (menu_match.valid) {
            user_match = menu_match;
            return;
        }
        matches = match_user_input(user_input, which, &user_match, command_list, NUM_COMMAND_KINDS+NUM_SPECIAL_COMMANDS, extended_input, (show_function) 0, FALSE);
        if (c == ' ') {
            /* extend only to one space, inclusive */
            p = extended_input;
            while (*p)
                if (*p++ == ' ') *p = '\0';
            }
        input_extend(extended_input);
        if (c == '\n') {
            if ((matches == 1) || user_match.exact) {
                return;
            }
            input_error(matches);
        }
        else {
            if (extended_input[0] == '\0') {
                /* Did not extend. Input is either wrong or ambiguous. */
                if ((c == ' ')  && (matches > 1)) {
                    /* Space did not extend because of ambiguity.
                     * Try treating Space as an ordinary character.
                     * If it matches, accept it.  Otherwise, complain.
                     */
                    if (user_match.space_ok)
                        input_extend(" ");
                    else
                        input_error(matches);
                }
                else
                    input_error(matches);
            }
        }
    }
}

call_list_kind uims_current_call_menu;

static uims_reply
get_call_command(call_list_kind *call_menu)
{
    check_menu:

    if (allowing_modifications)
        *call_menu = call_list_any;

    uims_current_call_menu = *call_menu;
    input_set_prompt("Enter concept or call", call_menu_names[*call_menu]);
    get_user_command((int) *call_menu);
    uims_menu_index = user_match.index;
    
    /*
     * User can type "modify next command", but sd doesn't want to hear about it.
     * It just wants to see the global variable changed.
     *
     */

    if (user_match.kind == ui_command_select && uims_menu_index >= NUM_COMMAND_KINDS) {
        if (uims_menu_index == NUM_COMMAND_KINDS + SPECIAL_COMMAND_ALLOW_MODS) {
            /* Increment "allowing_modifications" up to a maximum of 2. */
            if (allowing_modifications != 2) allowing_modifications++;
            /* This must be called to get the little checkmark in the menus correct */
            update_modification_state(allowing_modifications);
        }
        else {   /* Must be SPECIAL_COMMAND_TOGGLE_CONCEPT_LEVELS. */
            allowing_all_concepts = !allowing_all_concepts;
            /* update checkmark in the menus */
            SetItemMark(sequence_menu, anyConceptCommand, allowing_all_concepts ? 022 : 0);
        }
        goto check_menu;
    }
       
    if (!dirty) {
        if ((user_match.kind == ui_concept_select) ||
            (user_match.kind == ui_call_select) ||
            ((user_match.kind == ui_command_select) &&
             ((uims_menu_index == command_undo) ||
              (uims_menu_index == command_create_comment) ||
              (uims_menu_index == command_save_pic))))
            dirty = TRUE;
    }
    return user_match.kind;
}

extern uims_reply
uims_get_command(mode_kind mode, call_list_kind *call_menu)
{
    if (mode == mode_startup) {
        return get_startup_command();
    }
    else if (mode == mode_resolve) {
        return get_resolve_command();
    }
    else {
        return get_call_command(call_menu);
    }
}

extern int
uims_do_comment_popup(char dest[])
{
    return get_popup_string("Enter comment:", dest);
}

#ifdef NEGLECT
extern int
uims_do_neglect_popup(char dest[])
{
    strcpy(dest, "0"); /* cause command to abort */
    return POPUP_ACCEPT_WITH_STRING;
}
#endif

extern int
uims_do_modifier_popup(char callname[], modify_popup_kind kind)
{
   switch (kind) {
     case modify_popup_any:
        return mac_modifier_any_popup(callname);
        break;
     case modify_popup_only_tag:
       return mac_modifier_tag_popup(callname);
       break;
     case modify_popup_only_scoot:
       return mac_modifier_scoot_popup(callname);
       break;
     default:
       return POPUP_DECLINE;
   }
}

extern int
uims_do_selector_popup(void)
{
    int n;

    if (user_match.valid && (user_match.who > selector_uninitialized)) {
        n = (int) user_match.who;
        user_match.who = selector_uninitialized;
        return n;
    }
    else {
        return mac_do_selector_popup();
    }
}    

extern int
uims_do_direction_popup(void)
{
    int n;

    if (user_match.valid && (user_match.where > direction_uninitialized)) {
        n = (int) user_match.where;
        user_match.where = direction_uninitialized;
        return n;
    }
    else {
        return mac_do_direction_popup();
    }
}    


extern unsigned int
uims_get_number_fields(int nnumbers)
{
   int i;
   unsigned int number_list = 0;

   for (i=0 ; i<nnumbers ; i++) {
      unsigned int this_num;

      if (user_match.valid && (user_match.howmanynumbers >= 1)) {
          this_num = user_match.number_fields & 0xF;
          user_match.number_fields >>= 4;
          user_match.howmanynumbers--;
      }
      else {
          this_num =  mac_do_quantifier_popup();
      }

      if (this_num == 0) return 0;    /* User waved the mouse away. */
      number_list |= (this_num << (i*4));
   }

   return number_list;
}

/*
 * add a line to the text output area.
 * the_line does not have the trailing Newline in it and
 * is volitile, so we must copy it if we need it to stay around.
 */

extern void
uims_add_new_line(char the_line[])
{
    text_output_add_line(the_line);
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
}