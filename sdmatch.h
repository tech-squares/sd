/*
 * sdmatch.h - header file for command matcher
 *
 * Copyright (C) 1993 Alan Snyder
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose is hereby granted without fee, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * The author makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" WITHOUT ANY
 * WARRANTY, without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  
 *
 */


/* This is provided by sdmatch.c */
extern call_list_kind *call_menu_ptr;


/* The following items are PROVIDED by the main program (that is, the user
   interface part of it) and IMPORTED by sdmatch.c */

extern int num_command_commands;
extern Cstring command_commands[];

extern int number_of_resolve_commands;
extern Cstring resolve_command_strings[];


/*
 * A match_result describes the result of matching a string against
 * a set of acceptable commands. A match_result is effectively a
 * sequence of values to be returned to the main program. A sequence
 * is required because the main program asks for information in
 * bits and pieces.  For example, the call "touch 1/4" first returns
 * "touch <N/4>" and then returns "1".  This structure for the
 * interaction between the main program and the UI reflects the
 * design of the original Domain Dialog UI.
 *
 */

typedef struct glozk {
   concept_descriptor *this_modifier;
   struct glozk *next;            /* next concept, or, if this is end mark, points to substitution list */
   struct glozk *gc_ptr;          /* used for reclaiming dead blocks */
} modifier_block;

typedef struct zilch {
   long_boolean valid;       /* set to TRUE if a match was found */
   long_boolean exact;       /* set to TRUE if an exact match was found */
   uims_reply kind;
   int index;
   selector_kind who;        /* matches <anyone> */
   direction_kind where;     /* matches <direction> */
   uint32 tagger;            /* matches <atc> */
   Const struct zilch *modifier_parent;
   modifier_block *newmodifiers;   /* has "left", "magic", etc. modifiers. */
   concept_descriptor *current_modifier;
   long_boolean need_big_menu;
   uint32 number_fields;     /* matches all the number fields */
   int howmanynumbers;       /* tells how many there are */
   int space_ok;             /* space is a legitimate next input character */
   int callflags1;           /* contains the "yield_if_ambiguous" flag */
   int yielding_matches;     /* how many matches have that flag on */
} match_result;

typedef void (*show_function)(char *user_input, Const char *extension, Const match_result *mr);

enum {
    match_startup_commands = -1,
    match_resolve_commands = -2,
    match_selectors = -3,
    match_directions = -4,
    match_taggers = -8      /* This is the lowest of 4 numbers. */
};

extern void
matcher_initialize(long_boolean show_commands_last);

extern void
matcher_setup_call_menu(call_list_kind cl, callspec_block *call_name_list[]);
    
extern int
match_user_input(
    char *user_input,
    int which_commands,
    match_result *mr,
    char *extension,
    show_function sf,
    long_boolean show_verify);
