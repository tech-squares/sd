/* SD -- square dance caller's helper.

    Copyright (C) 1990-2002  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 34. */

/* This defines the following external variables:
   special_magic
   special_interlocked
   mark_end_of_list
   marker_decline
   marker_concept_mod
   marker_concept_comment
   marker_concept_supercall
   main_call_lists
   number_of_calls
   calling_level
   unsealed_concept_descriptor_table
   concept_fixer_table
   nice_setup_thing_4x4
   nice_setup_thing_3x4
   nice_setup_thing_2x8
   nice_setup_thing_2x6
   nice_setup_thing_1x12
   nice_setup_thing_1x16
   nice_setup_thing_3dmd
   nice_setup_thing_4dmd
   nice_setup_thing_4x6
*/

#ifdef WIN32
#define SDLIB_API __declspec(dllexport)
#else
#define SDLIB_API
#endif

#include "sd.h"


/* The number (typically 4), appearing just before the level in the items below, is the "concparseflags" word.
  To save space, it is entered numerically rather than symbolically.  Here are the meanings of the various bits:
      Y - If the parse turns out to be ambiguous, don't use this one -- yield to the other one.
      D - Parse directly.  It directs the parser to allow this concept (and similar concepts)
         and the following call to be typed on one line.  One needs to be very careful
         about avoiding ambiguity when setting this flag.
      F - This seems to mean put a comma after this concept, as in
         "boys are stable, swing thru". */

#define Y CONCPARSE_YIELD_IF_AMB
#define D CONCPARSE_PARSE_DIRECT
#define L CONCPARSE_PARSE_L_TYPE
#define F CONCPARSE_PARSE_F_TYPE
#define G CONCPARSE_PARSE_G_TYPE


concept_descriptor centers_concept = {
   "centers????",
   concept_centers_or_ends,
   TRUE,
   l_mainstream,
   UC_none,
   {selector_centers, FALSE}};

concept_descriptor special_magic          = {"MAGIC DIAMOND,",       concept_magic,             L+D, l_c1, UC_none, {1}};
concept_descriptor special_interlocked    = {"INTERLOCKED DIAMOND,", concept_interlocked,       L+D, l_c1, UC_none, {1}};

concept_descriptor mark_end_of_list       = {"????",                 marker_end_of_list,            0, l_dontshow, UC_none};
concept_descriptor marker_decline         = {"decline???",           concept_mod_declined,          0, l_dontshow, UC_none};
concept_descriptor marker_concept_mod     = {">>MODIFIED BY<<",      concept_another_call_next_mod, 0, l_dontshow, UC_none, {0, 0}};
concept_descriptor marker_concept_comment = {">>COMMENT<<",          concept_comment,               0, l_dontshow, UC_none};
concept_descriptor marker_concept_supercall = {">>SUPER<<",          concept_supercall,             0, l_dontshow, UC_none};



call_with_name **main_call_lists[call_list_extent];
int number_of_calls[call_list_extent];
dance_level calling_level;


const concept_descriptor *concept_descriptor_table;

concept_descriptor unsealed_concept_descriptor_table[] = {
   {"AS COUPLES",                            concept_tandem,                    D, l_a1, UC_cpl,         {0, 0,                    0x000, tandem_key_cpls}},
   {"TANDEM",                                concept_tandem,                    D, l_c1, UC_tnd,         {0, 0,                    0x000, tandem_key_tand}},
   {"SIAMESE",                               concept_tandem,                    D, l_c1, UC_none,         {0, 0,                    0x000, tandem_key_siam}},
   {"COUPLES OF 3",                          concept_tandem,                    D, l_a1, UC_none,         {0, 0,                    0x000, tandem_key_cpls3}},
   {"TANDEMS OF 3",                          concept_tandem,                    D, l_c1, UC_none,         {0, 0,                    0x000, tandem_key_tand3}},
   {"SIAMESE OF 3",                          concept_tandem,                    D, l_c1, UC_none,         {0, 0,                    0x000, tandem_key_siam3}},
   {"COUPLES OF 4",                          concept_tandem,                    D, l_a1, UC_none,         {0, 0,                    0x000, tandem_key_cpls4}},
   {"TANDEMS OF 4",                          concept_tandem,                    D, l_c1, UC_none,         {0, 0,                    0x000, tandem_key_tand4}},
   {"SIAMESE OF 4",                          concept_tandem,                    D, l_c1, UC_none,         {0, 0,                    0x000, tandem_key_siam4}},
   {"BOXES ARE SOLID",                       concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_box}},
   {"DIAMONDS ARE SOLID",                    concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_diamond}},
   {"SKEW",                                  concept_tandem,                    D, l_c4a, UC_none,        {0, 0,                    0x000, tandem_key_skew}},
   {"GRUESOME AS COUPLES",                   concept_gruesome_tandem,           D, l_c4a, UC_none,        {0, CONCPROP__NEEDK_2X8,  0x002, tandem_key_cpls}},
   {"GRUESOME TANDEM",                       concept_gruesome_tandem,           D, l_c4a, UC_none,        {0, CONCPROP__NEEDK_2X8,  0x002, tandem_key_tand}},
   {"@6 ARE AS COUPLES",                     concept_some_are_tandem,         F+D, l_a1, UC_none,         {0, 0,                    0x100, tandem_key_cpls}},
   {"@6 ARE TANDEM",                         concept_some_are_tandem,         F+D, l_c1, UC_none,         {0, 0,                    0x100, tandem_key_tand}},
   {"@6 ARE COUPLES OF 3",                   concept_some_are_tandem,         F+D, l_c1, UC_none,         {0, 0,                    0x100, tandem_key_cpls3}},
   {"@6 ARE TANDEMS OF 3",                   concept_some_are_tandem,         F+D, l_c1, UC_none,         {0, 0,                    0x100, tandem_key_tand3}},
   {"INSIDE TRIANGLES ARE SOLID",            concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_inside_tgls}},
   {"OUTSIDE TRIANGLES ARE SOLID",           concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_outside_tgls}},
   {"IN POINT TRIANGLES ARE SOLID",          concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_inpoint_tgls}},
   {"OUT POINT TRIANGLES ARE SOLID",         concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_outpoint_tgls}},
   {"WAVE-BASED TRIANGLES ARE SOLID",        concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_wave_tgls}},
   {"TANDEM-BASED TRIANGLES ARE SOLID",      concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_tand_tgls}},
   {"@k-BASED TRIANGLES ARE SOLID",          concept_some_are_tandem,         F+D, l_c2, UC_none,         {0, 0,                    0x100, tandem_key_anyone_tgls}},
   {"3X1 TRIANGLES ARE SOLID",               concept_tandem,                  F+D, l_c2, UC_none,         {0, 0,                    0x000, tandem_key_3x1tgls}},
   {"Y's ARE SOLID",                         concept_tandem,                  F+D, l_c4, UC_none,         {0, 0,                    0x000, tandem_key_ys}},
   {"COUPLES @b TWOSOME",                    concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_cpls}},
   {"TANDEM @b TWOSOME",                     concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_tand}},
   {"SIAMESE @b TWOSOME",                    concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_siam}},
   {"COUPLES OF 3 @b THREESOME",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_cpls3}},
   {"TANDEMS OF 3 @b THREESOME",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_tand3}},
   {"SIAMESE OF 3 @b THREESOME",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_siam3}},
   {"COUPLES OF 4 @b FOURSOME",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_cpls4}},
   {"TANDEMS OF 4 @b FOURSOME",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_tand4}},
   {"SIAMESE OF 4 @b FOURSOME",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_siam4}},
   {"BOXES ARE SOLID @b BOXSOME",            concept_frac_tandem,             F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_box}},
   {"DIAMONDS ARE SOLID @b DIAMONDSOME",     concept_frac_tandem,             F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_diamond}},
   {"SKEW @b TWOSOME",                       concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_skew}},
   {"GRUESOME AS COUPLES @b TWOSOME",        concept_gruesome_frac_tandem,      D, l_c4, UC_none,         {0, CONCPROP__NEEDK_2X8,  0x022, tandem_key_cpls}},
   {"GRUESOME TANDEM @b TWOSOME",            concept_gruesome_frac_tandem,      D, l_c4, UC_none,         {0, CONCPROP__NEEDK_2X8,  0x022, tandem_key_tand}},
   {"@6 ARE COUPLES @b TWOSOME",             concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x120, tandem_key_cpls}},
   {"@6 ARE TANDEM @b TWOSOME",              concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x120, tandem_key_tand}},
   {"@6 ARE COUPLES OF 3 @b THREESOME",      concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x120, tandem_key_cpls3}},
   {"@6 ARE TANDEMS OF 3 @b THREESOME",      concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x120, tandem_key_tand3}},
   {"INSIDE TRIANGLES ARE SOLID @b THREESOME",       concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_inside_tgls}},
   {"OUTSIDE TRIANGLES ARE SOLID @b THREESOME",      concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_outside_tgls}},
   {"IN POINT TRIANGLES ARE SOLID @b THREESOME",     concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_inpoint_tgls}},
   {"OUT POINT TRIANGLES ARE SOLID @b THREESOME",    concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_outpoint_tgls}},
   {"WAVE-BASED TRIANGLES ARE SOLID @b THREESOME",   concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_wave_tgls}},
   {"TANDEM-BASED TRIANGLES ARE SOLID @b THREESOME", concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_tand_tgls}},
   {"@k-BASED TRIANGLES ARE SOLID @b THREESOME", concept_some_are_frac_tandem,F+D, l_c4, UC_none,         {0, 0,                    0x120, tandem_key_anyone_tgls}},
   {"3X1 TRIANGLES ARE SOLID @b TRIANGLESOME",       concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_3x1tgls}},
   {"Y's ARE SOLID @b Y-SOME",                       concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x020, tandem_key_ys}},
   {"GRUESOME TWOSOME",                      concept_gruesome_tandem,           D, l_c4a, UC_none,        {0, CONCPROP__NEEDK_2X8,  0x013, tandem_key_cpls}},
   {"COUPLES TWOSOME",                       concept_tandem,                    D, l_c3, UC_cpl2s,         {0, 0,                    0x010, tandem_key_cpls}},
   {"TANDEM TWOSOME",                        concept_tandem,                    D, l_c3, UC_tnd2s,         {0, 0,                    0x010, tandem_key_tand}},
   {"SIAMESE TWOSOME",                       concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_siam}},
   {"COUPLES THREESOME",                     concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_cpls3}},
   {"TANDEM THREESOME",                      concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_tand3}},
   {"SIAMESE THREESOME",                     concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_siam3}},
   {"COUPLES FOURSOME",                      concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_cpls4}},
   {"TANDEM FOURSOME",                       concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_tand4}},
   {"SIAMESE FOURSOME",                      concept_tandem,                    D, l_c3, UC_none,         {0, 0,                    0x010, tandem_key_siam4}},
   {"BOXSOME",                               concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_box}},
   {"DIAMONDSOME",                           concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_diamond}},
   {"SKEWSOME",                              concept_tandem,                    D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_skew}},
   {"GRUESOME COUPLES TWOSOME",              concept_gruesome_tandem,           D, l_c4a, UC_none,        {0, CONCPROP__NEEDK_2X8,  0x012, tandem_key_cpls}},
   {"GRUESOME TANDEM TWOSOME",               concept_gruesome_tandem,           D, l_c4a, UC_none,        {0, CONCPROP__NEEDK_2X8,  0x012, tandem_key_tand}},
   {"@6 ARE COUPLES TWOSOME",                concept_some_are_tandem,         F+D, l_c3, UC_none,         {0, 0,                    0x110, tandem_key_cpls}},
   {"@6 ARE TANDEM TWOSOME",                 concept_some_are_tandem,         F+D, l_c3, UC_none,         {0, 0,                    0x110, tandem_key_tand}},
   {"@6 ARE COUPLES THREESOME",              concept_some_are_tandem,         F+D, l_c3, UC_none,         {0, 0,                    0x110, tandem_key_cpls3}},
   {"@6 ARE TANDEM THREESOME",               concept_some_are_tandem,         F+D, l_c3, UC_none,         {0, 0,                    0x110, tandem_key_tand3}},
   {"INSIDE TRIANGLES ARE THREESOME",        concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_inside_tgls}},
   {"OUTSIDE TRIANGLES ARE THREESOME",       concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_outside_tgls}},
   {"IN POINT TRIANGLES ARE THREESOME",      concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_inpoint_tgls}},
   {"OUT POINT TRIANGLES ARE THREESOME",     concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_outpoint_tgls}},
   {"WAVE-BASED TRIANGLES ARE THREESOME",    concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_wave_tgls}},
   {"TANDEM-BASED TRIANGLES ARE THREESOME",  concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_tand_tgls}},
   {"@k-BASED TRIANGLES ARE THREESOME",      concept_some_are_tandem,         F+D, l_c4a, UC_none,        {0, 0,                    0x110, tandem_key_anyone_tgls}},
   {"3X1 TRIANGLES ARE TRIANGLESOME",        concept_tandem,                  F+D, l_c4a, UC_none,        {0, 0,                    0x010, tandem_key_3x1tgls}},
   {"Y-SOME",                                concept_tandem,                  F+D, l_c4, UC_none,         {0, 0,                    0x010, tandem_key_ys}},
   {"COUPLES TWOSOME @b SOLID",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_cpls}},
   {"TANDEM TWOSOME @b SOLID",               concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_tand}},
   {"SIAMESE TWOSOME @b SOLID",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_siam}},
   {"COUPLES THREESOME @b SOLID",            concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_cpls3}},
   {"TANDEM THREESOME @b SOLID",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_tand3}},
   {"SIAMESE THREESOME @b SOLID",            concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_siam3}},
   {"COUPLES FOURSOME @b SOLID",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_cpls4}},
   {"TANDEM FOURSOME @b SOLID",              concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_tand4}},
   {"SIAMESE FOURSOME @b SOLID",             concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_siam4}},
   {"BOXSOME @b SOLID",                      concept_frac_tandem,             F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_box}},
   {"DIAMONDSOME @b SOLID",                  concept_frac_tandem,             F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_diamond}},
   {"SKEWSOME @b SOLID",                     concept_frac_tandem,               D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_skew}},
   {"GRUESOME TWOSOME @b SOLID",             concept_gruesome_frac_tandem,      D, l_c4, UC_none,         {0, CONCPROP__NEEDK_2X8,  0x032, tandem_key_cpls}},
   {"GRUESOME TANDEM TWOSOME @b SOLID",      concept_gruesome_frac_tandem,      D, l_c4, UC_none,         {0, CONCPROP__NEEDK_2X8,  0x032, tandem_key_tand}},
   {"@6 ARE COUPLES TWOSOME @b SOLID",       concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x130, tandem_key_cpls}},
   {"@6 ARE TANDEM TWOSOME @b SOLID",        concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x130, tandem_key_tand}},
   {"@6 ARE COUPLES THREESOME @b SOLID",     concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x130, tandem_key_cpls3}},
   {"@6 ARE TANDEM THREESOME @b SOLID",      concept_some_are_frac_tandem,    F+D, l_c4, UC_none,         {0, 0,                    0x130, tandem_key_tand3}},
   {"INSIDE TRIANGLES ARE THREESOME @b SOLID",       concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_inside_tgls}},
   {"OUTSIDE TRIANGLES ARE THREESOME @b SOLID",      concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_outside_tgls}},
   {"IN POINT TRIANGLES ARE THREESOME @b SOLID",     concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_inpoint_tgls}},
   {"OUT POINT TRIANGLES ARE THREESOME @b SOLID",    concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_outpoint_tgls}},
   {"WAVE-BASED TRIANGLES ARE THREESOME @b SOLID",   concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_wave_tgls}},
   {"TANDEM-BASED TRIANGLES ARE THREESOME @b SOLID", concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_tand_tgls}},
   {"@k-BASED TRIANGLES ARE THREESOME @b SOLID", concept_some_are_frac_tandem,F+D, l_c4, UC_none,         {0, 0,                    0x130, tandem_key_anyone_tgls}},
   {"3X1 TRIANGLES ARE TRIANGLESOME @b SOLID",       concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_3x1tgls}},
   {"Y-SOME @b SOLID",                               concept_frac_tandem,     F+D, l_c4, UC_none,         {0, 0,                    0x030, tandem_key_ys}},
   {"SPLIT PHANTOM COLUMNS",                 concept_do_phantom_2x4,            D, l_c3a, UC_spc, {phantest_impossible,    0, MPKIND__SPLIT, 0}},
   {"SPLIT PHANTOM LINES",                   concept_do_phantom_2x4,            D, l_c3a, UC_spl, {phantest_impossible,    1, MPKIND__SPLIT, 0}},
   {"SPLIT PHANTOM WAVES",                   concept_do_phantom_2x4,            D, l_c3a, UC_spw, {phantest_impossible,    3, MPKIND__SPLIT, 0}},
   {"SPLIT PHANTOM BOXES",                   concept_do_phantom_boxes,          D, l_c3, UC_spb,  {phantest_impossible,    0, MPKIND__SPLIT}},
   {"SPLIT PHANTOM DIAMONDS",                concept_do_phantom_diamonds,       D, l_c3, UC_spd, {phantest_impossible,    CMD_MISC__VERIFY_DMD_LIKE, MPKIND__SPLIT}},
   {"SPLIT PHANTOM DIAMOND SPOTS",           concept_do_phantom_diamonds,       D, l_c3, UC_spds, {phantest_impossible,    0, MPKIND__SPLIT}},
   {"SPLIT PHANTOM 1/4 TAGS",                concept_do_phantom_diamonds,       D, l_c3, UC_sp1, {phantest_impossible,    CMD_MISC__VERIFY_1_4_TAG, MPKIND__SPLIT}},
   {"SPLIT PHANTOM 3/4 TAGS",                concept_do_phantom_diamonds,       D, l_c3, UC_sp3, {phantest_impossible,    CMD_MISC__VERIFY_3_4_TAG, MPKIND__SPLIT}},
   {"SPLIT PHANTOM 1/4 LINES",               concept_do_phantom_diamonds,       D, l_c3, UC_none, {phantest_impossible,    CMD_MISC__VERIFY_REAL_1_4_LINE, MPKIND__SPLIT}},
   {"SPLIT PHANTOM 3/4 LINES",               concept_do_phantom_diamonds,       D, l_c3, UC_none, {phantest_impossible,    CMD_MISC__VERIFY_REAL_3_4_LINE, MPKIND__SPLIT}},
   {"SPLIT PHANTOM GENERAL 1/4 TAGS",        concept_do_phantom_diamonds,       D, l_c3, UC_spgt, {phantest_impossible,    CMD_MISC__VERIFY_QTAG_LIKE, MPKIND__SPLIT}},
   {"INTERLOCKED PHANTOM COLUMNS",           concept_do_phantom_2x4,            D, l_c4a, UC_ipc, {phantest_impossible,    0, MPKIND__INTLK, 0}},
   {"INTERLOCKED PHANTOM LINES",             concept_do_phantom_2x4,            D, l_c4a, UC_ipl, {phantest_impossible,    1, MPKIND__INTLK, 0}},
   {"INTERLOCKED PHANTOM WAVES",             concept_do_phantom_2x4,            D, l_c4a, UC_ipw, {phantest_impossible,    3, MPKIND__INTLK, 0}},
   {"INTERLOCKED PHANTOM BOXES",             concept_do_phantom_boxes,          D, l_c4, UC_ipb,  {phantest_impossible,    0, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM DIAMONDS",          concept_do_phantom_diamonds,       D, l_c4, UC_ipd, {phantest_impossible,    CMD_MISC__VERIFY_DMD_LIKE, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM DIAMOND SPOTS",     concept_do_phantom_diamonds,       D, l_c4, UC_ipds, {phantest_impossible,    0, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM 1/4 TAGS",          concept_do_phantom_diamonds,       D, l_c4, UC_ip1, {phantest_impossible,    CMD_MISC__VERIFY_1_4_TAG, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM 3/4 TAGS",          concept_do_phantom_diamonds,       D, l_c4, UC_ip3, {phantest_impossible,    CMD_MISC__VERIFY_3_4_TAG, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM 1/4 LINES",         concept_do_phantom_diamonds,       D, l_c4, UC_none, {phantest_impossible,    CMD_MISC__VERIFY_REAL_1_4_LINE, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM 3/4 LINES",         concept_do_phantom_diamonds,       D, l_c4, UC_none, {phantest_impossible,    CMD_MISC__VERIFY_REAL_3_4_LINE, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM GENERAL 1/4 TAGS",  concept_do_phantom_diamonds,       D, l_c4, UC_ipgt, {phantest_impossible,    CMD_MISC__VERIFY_QTAG_LIKE, MPKIND__INTLK}},
   {"PHANTOM COLUMNS",                       concept_do_phantom_2x4,            D, l_c3, UC_pc,  {phantest_first_or_both, 0, MPKIND__CONCPHAN, 0}},
   {"PHANTOM LINES",                         concept_do_phantom_2x4,            D, l_c3, UC_pl,  {phantest_first_or_both, 1, MPKIND__CONCPHAN, 0}},
   {"PHANTOM WAVES",                         concept_do_phantom_2x4,            D, l_c3, UC_pw,  {phantest_first_or_both, 3, MPKIND__CONCPHAN, 0}},
   {"PHANTOM BOXES",                         concept_do_phantom_boxes,          D, l_c4a, UC_pb, {phantest_first_or_both, 0, MPKIND__CONCPHAN}},
   {"PHANTOM DIAMONDS",                      concept_do_phantom_diamonds,       D, l_c4, UC_pd, {phantest_first_or_both, CMD_MISC__VERIFY_DMD_LIKE, MPKIND__CONCPHAN}},
   {"PHANTOM DIAMOND SPOTS",                 concept_do_phantom_diamonds,       D, l_c4, UC_pds, {phantest_first_or_both, 0, MPKIND__CONCPHAN}},
   {"PHANTOM 1/4 TAGS",                      concept_do_phantom_diamonds,       D, l_c4, UC_p1, {phantest_first_or_both, CMD_MISC__VERIFY_1_4_TAG, MPKIND__CONCPHAN}},
   {"PHANTOM 3/4 TAGS",                      concept_do_phantom_diamonds,       D, l_c4, UC_p3, {phantest_first_or_both, CMD_MISC__VERIFY_3_4_TAG, MPKIND__CONCPHAN}},
   {"PHANTOM 1/4 LINES",                     concept_do_phantom_diamonds,       D, l_c4, UC_none, {phantest_first_or_both, CMD_MISC__VERIFY_REAL_1_4_LINE, MPKIND__CONCPHAN}},
   {"PHANTOM 3/4 LINES",                     concept_do_phantom_diamonds,       D, l_c4, UC_none, {phantest_first_or_both, CMD_MISC__VERIFY_REAL_3_4_LINE, MPKIND__CONCPHAN}},
   {"PHANTOM GENERAL 1/4 TAGS",              concept_do_phantom_diamonds,       D, l_c4, UC_pgt, {phantest_first_or_both, CMD_MISC__VERIFY_QTAG_LIKE, MPKIND__CONCPHAN}},
   {"12 MATRIX SPLIT PHANTOM COLUMNS",       concept_triple_twin_nomystic,      D, l_c3x, UC_none, {0, CONCPROP__NEEDK_3X4, 8, phantest_impossible, MPKIND__SPLIT}},
   {"12 MATRIX SPLIT PHANTOM LINES",         concept_triple_twin_nomystic,      D, l_c3x, UC_none, {1, CONCPROP__NEEDK_3X4, 8, phantest_impossible, MPKIND__SPLIT}},
   {"12 MATRIX INTERLOCKED PHANTOM COLUMNS", concept_triple_twin_nomystic,      D, l_c3x, UC_none, {0, CONCPROP__NEEDK_3X4, 8, phantest_impossible, MPKIND__INTLK}},
   {"12 MATRIX INTERLOCKED PHANTOM LINES",   concept_triple_twin_nomystic,      D, l_c3x, UC_none, {1, CONCPROP__NEEDK_3X4, 8, phantest_impossible, MPKIND__INTLK}},
   {"12 MATRIX PHANTOM COLUMNS",             concept_triple_twin_nomystic,      D, l_c3x, UC_none, {0, CONCPROP__NEEDK_3X4, 8, phantest_first_or_both, MPKIND__CONCPHAN}},
   {"12 MATRIX PHANTOM LINES",               concept_triple_twin_nomystic,      D, l_c3x, UC_none, {1, CONCPROP__NEEDK_3X4, 8, phantest_first_or_both, MPKIND__CONCPHAN}},
   {"SPLIT PHANTOM COLUMNS OF 6",            concept_triple_twin,               D, l_c3x, UC_none, {0, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__SPLIT}},
   {"SPLIT PHANTOM LINES OF 6",              concept_triple_twin,               D, l_c3x, UC_none, {1, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__SPLIT}},
   {"SPLIT PHANTOM WAVES OF 6",              concept_triple_twin,               D, l_c3x, UC_none, {3, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__SPLIT}},
   {"INTERLOCKED PHANTOM COLUMNS OF 6",      concept_triple_twin,               D, l_c3x, UC_none, {0, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM LINES OF 6",        concept_triple_twin,               D, l_c3x, UC_none, {1, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__INTLK}},
   {"INTERLOCKED PHANTOM WAVES OF 6",        concept_triple_twin,               D, l_c3x, UC_none, {3, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__INTLK}},
   {"PHANTOM COLUMNS OF 6",                  concept_triple_twin,               D, l_c3x, UC_none, {0, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__CONCPHAN}},
   {"PHANTOM LINES OF 6",                    concept_triple_twin,               D, l_c3x, UC_none, {1, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__CONCPHAN}},
   {"PHANTOM WAVES OF 6",                    concept_triple_twin,               D, l_c3x, UC_none, {3, CONCPROP__NEEDK_4X6, 3, phantest_ok, MPKIND__CONCPHAN}},
   {"DIVIDED COLUMNS",                       concept_triple_twin_nomystic,      D, l_c4, UC_none, {0, CONCPROP__NEEDK_2X8, 10, phantest_impossible}},
   {"DIVIDED LINES",                         concept_triple_twin_nomystic,      D, l_c4, UC_none, {1, CONCPROP__NEEDK_2X8, 10, phantest_impossible}},
   {"DIVIDED WAVES",                         concept_triple_twin_nomystic,      D, l_c4, UC_none, {3, CONCPROP__NEEDK_2X8, 10, phantest_impossible}},
   {"12 MATRIX DIVIDED COLUMNS",             concept_triple_twin_nomystic,      D, l_c4, UC_none, {0, CONCPROP__NEEDK_2X6, 9, phantest_impossible}},
   {"12 MATRIX DIVIDED LINES",               concept_triple_twin_nomystic,      D, l_c4, UC_none, {1, CONCPROP__NEEDK_2X6, 9, phantest_impossible}},
   {"12 MATRIX DIVIDED WAVES",               concept_triple_twin_nomystic,      D, l_c4, UC_none, {3, CONCPROP__NEEDK_2X6, 9, phantest_impossible}},
   {"TWIN PHANTOM TIDAL COLUMNS",            concept_triple_twin_nomystic,      D, l_c3, UC_pc8, {0, CONCPROP__NEEDK_2X8, 7, phantest_impossible}},
   {"TWIN PHANTOM TIDAL LINES",              concept_triple_twin_nomystic,      D, l_c3, UC_pl8, {1, CONCPROP__NEEDK_2X8, 7, phantest_impossible}},
   {"TWIN PHANTOM TIDAL WAVES",              concept_triple_twin_nomystic,      D, l_c3, UC_none, {3, CONCPROP__NEEDK_2X8, 7, phantest_impossible}},
   {"TWIN PHANTOM COLUMNS OF 6",             concept_triple_twin_nomystic,      D, l_c3, UC_pc6, {0, CONCPROP__NEEDK_2X6, 6, phantest_impossible}},
   {"TWIN PHANTOM LINES OF 6",               concept_triple_twin_nomystic,      D, l_c3, UC_pl6, {1, CONCPROP__NEEDK_2X6, 6, phantest_impossible}},
   {"TWIN PHANTOM WAVES OF 6",               concept_triple_twin_nomystic,      D, l_c3, UC_none, {3, CONCPROP__NEEDK_2X6, 6, phantest_impossible}},
   {"TRIPLE COLUMNS",                        concept_triple_lines,              D, l_c2, UC_tc, {2}},
   {"TRIPLE LINES",                          concept_triple_lines,              D, l_c2, UC_tl, {1}},
   {"TRIPLE WAVES",                          concept_triple_lines,              D, l_c2, UC_none, {3}},
   {"TRIPLE BOXES",                          concept_triple_boxes,              D, l_c1, UC_tb, {MPKIND__SPLIT}},
   {"TRIPLE DIAMONDS",                       concept_triple_diamonds,           D, l_c3a, UC_td, {0, CMD_MISC__VERIFY_DMD_LIKE}},
   {"TRIPLE DIAMOND SPOTS",                  concept_triple_diamonds,           D, l_c3a, UC_none, {0, 0}},
   {"TRIPLE 1/4 TAGS",                       concept_triple_diamonds,           D, l_c3x, UC_none, {0, CMD_MISC__VERIFY_1_4_TAG}},
   {"TRIPLE 3/4 TAGS",                       concept_triple_diamonds,           D, l_c3x, UC_none, {0, CMD_MISC__VERIFY_3_4_TAG}},
   {"TRIPLE 1/4 LINES",                      concept_triple_diamonds,           D, l_c3x, UC_none, {0, CMD_MISC__VERIFY_REAL_1_4_LINE}},
   {"TRIPLE 3/4 LINES",                      concept_triple_diamonds,           D, l_c3x, UC_none, {0, CMD_MISC__VERIFY_REAL_3_4_LINE}},
   {"TRIPLE GENERAL 1/4 TAGS",               concept_triple_diamonds,           D, l_c3x, UC_none, {0, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"TRIPLE Z's",                            concept_misc_distort,              D, l_c4, UC_none,         {0, CONCPROP__NEEDK_3X6, 0, 3}},
   {"TRIPLE 1X4s",                           concept_triple_lines,              D, l_c3, UC_none, {0}},
   {"TRIPLE COLUMNS OF 6",                   concept_triple_twin_nomystic,      D, l_c3x, UC_none, {0, CONCPROP__NEEDK_3X6, 1, phantest_ok}},
   {"TRIPLE LINES OF 6",                     concept_triple_twin_nomystic,      D, l_c3x, UC_none, {1, CONCPROP__NEEDK_3X6, 1, phantest_ok}},
   {"TRIPLE WAVES OF 6",                     concept_triple_twin_nomystic,      D, l_c3x, UC_none, {3, CONCPROP__NEEDK_3X6, 1, phantest_ok}},
   {"TRIPLE TIDAL COLUMNS",                  concept_triple_twin_nomystic,      D, l_c3, UC_none, {0, CONCPROP__NEEDK_3X8, 5, phantest_impossible}},
   {"TRIPLE TIDAL LINES",                    concept_triple_twin_nomystic,      D, l_c3, UC_none, {1, CONCPROP__NEEDK_3X8, 5, phantest_impossible}},
   {"TRIPLE TIDAL WAVES",                    concept_triple_twin_nomystic,      D, l_c3, UC_none, {3, CONCPROP__NEEDK_3X8, 5, phantest_impossible}},
   {"TRIPLE DIAGONAL COLUMNS",               concept_triple_diag,               D, l_c4, UC_none, {0}},
   {"TRIPLE DIAGONAL LINES",                 concept_triple_diag,               D, l_c4, UC_none, {1}},
   {"TRIPLE DIAGONAL WAVES",                 concept_triple_diag,               D, l_c4, UC_none, {3}},
   {"TRIPLE TWIN COLUMNS",                   concept_triple_twin,               D, l_c4a, UC_trtc, {0, CONCPROP__NEEDK_4X6, 0, phantest_not_just_centers}},
   {"TRIPLE TWIN LINES",                     concept_triple_twin,               D, l_c4a, UC_trtl, {1, CONCPROP__NEEDK_4X6, 0, phantest_not_just_centers}},
   {"TRIPLE TWIN WAVES",                     concept_triple_twin,               D, l_c4a, UC_none, {3, CONCPROP__NEEDK_4X6, 0, phantest_not_just_centers}},
   {"TRIPLE TWIN COLUMNS OF 3",              concept_triple_twin,               D, l_c4a, UC_none, {0, CONCPROP__NEEDK_3X6, 4, phantest_not_just_centers}},
   {"TRIPLE TWIN LINES OF 3",                concept_triple_twin,               D, l_c4a, UC_none, {1, CONCPROP__NEEDK_3X6, 4, phantest_not_just_centers}},
   {"TRIPLE TWIN WAVES OF 3",                concept_triple_twin,               D, l_c4a, UC_none, {3, CONCPROP__NEEDK_3X6, 4, phantest_not_just_centers}},
   {"QUADRUPLE COLUMNS",                     concept_quad_lines,                D, l_c4a, UC_none, {0, 0}},
   {"QUADRUPLE LINES",                       concept_quad_lines,                D, l_c4a, UC_none, {1, 0}},
   {"QUADRUPLE WAVES",                       concept_quad_lines,                D, l_c4a, UC_none, {3, 0}},
   {"QUADRUPLE BOXES",                       concept_quad_boxes,                D, l_c4a, UC_none, {MPKIND__SPLIT}},
   {"QUADRUPLE DIAMONDS",                    concept_quad_diamonds,             D, l_c4a, UC_qd, {0, CMD_MISC__VERIFY_DMD_LIKE}},
   {"QUADRUPLE DIAMOND SPOTS",               concept_quad_diamonds,             D, l_c4a, UC_none, {0, 0}},
   {"QUADRUPLE 1/4 TAGS",                    concept_quad_diamonds,             D, l_c4a, UC_none, {0, CMD_MISC__VERIFY_1_4_TAG}},
   {"QUADRUPLE 3/4 TAGS",                    concept_quad_diamonds,             D, l_c4a, UC_none, {0, CMD_MISC__VERIFY_3_4_TAG}},
   {"QUADRUPLE 1/4 LINES",                   concept_quad_diamonds,             D, l_c4a, UC_none, {0, CMD_MISC__VERIFY_REAL_1_4_LINE}},
   {"QUADRUPLE 3/4 LINES",                   concept_quad_diamonds,             D, l_c4a, UC_none, {0, CMD_MISC__VERIFY_REAL_3_4_LINE}},
   {"QUADRUPLE GENERAL 1/4 TAGS",            concept_quad_diamonds,             D, l_c4a, UC_none, {0, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"QUADRUPLE COLUMNS OF 3",                concept_quad_lines_of_3,           D, l_c4a, UC_none, {0, 1}},
   {"QUADRUPLE LINES OF 3",                  concept_quad_lines_of_3,           D, l_c4a, UC_none, {1, 1}},
   {"QUADRUPLE WAVES OF 3",                  concept_quad_lines_of_3,           D, l_c4a, UC_none, {3, 1}},
   {"QUADRUPLE COLUMNS OF 6",                concept_triple_twin,               D, l_c3x, UC_none, {0, CONCPROP__NEEDK_4X6, 2, phantest_ok}},
   {"QUADRUPLE LINES OF 6",                  concept_triple_twin,               D, l_c3x, UC_none, {1, CONCPROP__NEEDK_4X6, 2, phantest_ok}},
   {"QUADRUPLE WAVES OF 6",                  concept_triple_twin,               D, l_c3x, UC_none, {3, CONCPROP__NEEDK_4X6, 2, phantest_ok}},
   {"TRIPLE COLUMNS WORKING TOGETHER",       concept_multiple_lines_tog_std,    D, l_c4a, UC_tcwt, {10, CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE COLUMNS WORKING APART",          concept_multiple_lines_tog_std,    D, l_c4a, UC_tcwa, {11, CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE COLUMNS WORKING RIGHT",          concept_multiple_lines_tog,        D, l_c4a, UC_tcwr, {2,  CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE COLUMNS WORKING LEFT",           concept_multiple_lines_tog,        D, l_c4a, UC_tcwl, {0,  CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE COLUMNS WORKING CLOCKWISE",      concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE COLUMNS WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,   D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_TRIPLE_1X4, 0, 3}},
   {"TRIPLE LINES WORKING TOGETHER",         concept_multiple_lines_tog_std,    D, l_c4a, UC_tlwt, {10, CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE LINES WORKING APART",            concept_multiple_lines_tog_std,    D, l_c4a, UC_tlwa, {11, CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE LINES WORKING FORWARD",          concept_multiple_lines_tog,        D, l_c3, UC_tlwf,  {0,  CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE LINES WORKING BACKWARD",         concept_multiple_lines_tog,        D, l_c3, UC_tlwb,  {2,  CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE LINES WORKING CLOCKWISE",        concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE LINES WORKING COUNTERCLOCKWISE", concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_TRIPLE_1X4, 1, 3}},
   {"TRIPLE WAVES WORKING TOGETHER",         concept_multiple_lines_tog_std,    D, l_c4a, UC_none, {10, CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE WAVES WORKING APART",            concept_multiple_lines_tog_std,    D, l_c4a, UC_none, {11, CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE WAVES WORKING FORWARD",          concept_multiple_lines_tog,        D, l_c3, UC_none,  {0,  CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE WAVES WORKING BACKWARD",         concept_multiple_lines_tog,        D, l_c3, UC_none,  {2,  CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE WAVES WORKING CLOCKWISE",        concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE WAVES WORKING COUNTERCLOCKWISE", concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_TRIPLE_1X4, 3, 3}},
   {"TRIPLE BOXES WORKING TOGETHER",         concept_triple_boxes_together,     D, l_c4a, UC_tbwt, {6, 0}},
   {"TRIPLE BOXES WORKING APART",            concept_triple_boxes_together,     D, l_c4a, UC_tbwa, {7, 0}},
   {"TRIPLE BOXES WORKING FORWARD",          concept_triple_boxes_together,     D, l_c4a, UC_tbwf, {0, 0}},
   {"TRIPLE BOXES WORKING BACKWARD",         concept_triple_boxes_together,     D, l_c4a, UC_tbwb, {2, 0}},
   {"TRIPLE BOXES WORKING RIGHT",            concept_triple_boxes_together,     D, l_c4a, UC_tbwr, {3, 0}},
   {"TRIPLE BOXES WORKING LEFT",             concept_triple_boxes_together,     D, l_c4a, UC_tbwl, {1, 0}},
   {"TRIPLE BOXES WORKING CLOCKWISE",        concept_triple_boxes_together,     D, l_c4, UC_none, {8, 0}},
   {"TRIPLE BOXES WORKING COUNTERCLOCKWISE", concept_triple_boxes_together,     D, l_c4, UC_none, {9, 0}},
   {"TRIPLE DIAMONDS WORKING TOGETHER",      concept_triple_diamonds_together,  D, l_c4a, UC_tdwt, {0}},
   {"TRIPLE 1/4 TAGS WORKING TOGETHER",      concept_triple_diamonds_together,  D, l_c4a, UC_none, {0}},
   {"TRIPLE 1/4 TAGS WORKING RIGHT",         concept_triple_diamonds_together,  D, l_c4a, UC_none, {1}},
   {"TRIPLE 1/4 TAGS WORKING LEFT",          concept_triple_diamonds_together,  D, l_c4a, UC_none, {2}},
   {"TRIPLE 1/4 LINES WORKING TOGETHER",     concept_triple_diamonds_together,  D, l_c4a, UC_none, {0}},
   {"TRIPLE 1/4 LINES WORKING RIGHT",        concept_triple_diamonds_together,  D, l_c4a, UC_none, {1}},
   {"TRIPLE 1/4 LINES WORKING LEFT",         concept_triple_diamonds_together,  D, l_c4a, UC_none, {2}},
   {"TRIPLE TIDAL COLUMNS WORKING RIGHT",    concept_triple_1x8_tog,            D, l_c4, UC_none, {2, 0}},
   {"TRIPLE TIDAL COLUMNS WORKING LEFT",     concept_triple_1x8_tog,            D, l_c4, UC_none, {0, 0}},
   {"TRIPLE TIDAL LINES WORKING FORWARD",    concept_triple_1x8_tog,            D, l_c4, UC_none, {0, 1}},
   {"TRIPLE TIDAL LINES WORKING BACKWARD",   concept_triple_1x8_tog,            D, l_c4, UC_none, {2, 1}},
   {"TRIPLE TIDAL WAVES WORKING FORWARD",    concept_triple_1x8_tog,            D, l_c4, UC_none, {0, 3}},
   {"TRIPLE TIDAL WAVES WORKING BACKWARD",   concept_triple_1x8_tog,            D, l_c4, UC_none, {2, 3}},
   {"TRIPLE DIAGONAL COLUMNS WORKING RIGHT", concept_triple_diag_together,      D, l_c4, UC_none, {3, 0}},
   {"TRIPLE DIAGONAL COLUMNS WORKING LEFT",  concept_triple_diag_together,      D, l_c4, UC_none, {1, 0}},
   {"TRIPLE DIAGONAL LINES WORKING FORWARD", concept_triple_diag_together,      D, l_c4, UC_none, {0, 1}},
   {"TRIPLE DIAGONAL LINES WORKING BACKWARD",concept_triple_diag_together,      D, l_c4, UC_none, {2, 1}},
   {"TRIPLE DIAGONAL WAVES WORKING FORWARD", concept_triple_diag_together,      D, l_c4, UC_none, {0, 3}},
   {"TRIPLE DIAGONAL WAVES WORKING BACKWARD",concept_triple_diag_together,      D, l_c4, UC_none, {2, 3}},
   {"QUADRUPLE COLUMNS WORKING TOGETHER",    concept_multiple_lines_tog_std,    D, l_c4a, UC_qcwt, {10, CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING APART",       concept_multiple_lines_tog_std,    D, l_c4a, UC_qcwa, {11, CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING RIGHT",       concept_multiple_lines_tog,        D, l_c4a, UC_qcwr, {2,  CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING LEFT",        concept_multiple_lines_tog,        D, l_c4a, UC_qcwl, {0,  CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING CLOCKWISE",   concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING TOWARD THE CENTER",concept_multiple_lines_tog_std,D,l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE COLUMNS WORKING INWARD",      concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 0, 4}},
   {"QUADRUPLE LINES WORKING TOGETHER",      concept_multiple_lines_tog_std,    D, l_c4a, UC_qlwt, {10, CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING APART",         concept_multiple_lines_tog_std,    D, l_c4a, UC_qlwa, {11, CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING FORWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_qlwf, {0,  CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING BACKWARD",      concept_multiple_lines_tog,        D, l_c4a, UC_qlwb, {2,  CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING CLOCKWISE",     concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,  D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING TOWARD THE CENTER",concept_multiple_lines_tog_std, D, l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE LINES WORKING INWARD",        concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 1, 4}},
   {"QUADRUPLE WAVES WORKING TOGETHER",      concept_multiple_lines_tog_std,    D, l_c4a, UC_none, {10, CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING APART",         concept_multiple_lines_tog_std,    D, l_c4a, UC_none, {11, CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING FORWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING BACKWARD",      concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING CLOCKWISE",     concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,  D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING TOWARD THE CENTER",concept_multiple_lines_tog_std, D, l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE WAVES WORKING INWARD",        concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {12, CONCPROP__NEEDK_QUAD_1X4, 3, 4}},
   {"QUADRUPLE BOXES WORKING TOGETHER",      concept_quad_boxes_together,       D, l_c4a, UC_qbwt, {6, 0}},
   {"QUADRUPLE BOXES WORKING APART",         concept_quad_boxes_together,       D, l_c4a, UC_qbwa, {7, 0}},
   {"QUADRUPLE BOXES WORKING FORWARD",       concept_quad_boxes_together,       D, l_c4a, UC_qbwf, {0, 0}},
   {"QUADRUPLE BOXES WORKING BACKWARD",      concept_quad_boxes_together,       D, l_c4a, UC_qbwb, {2, 0}},
   {"QUADRUPLE BOXES WORKING RIGHT",         concept_quad_boxes_together,       D, l_c4a, UC_qbwr, {3, 0}},
   {"QUADRUPLE BOXES WORKING LEFT",          concept_quad_boxes_together,       D, l_c4a, UC_qbwl, {1, 0}},
   {"QUADRUPLE BOXES WORKING CLOCKWISE",     concept_quad_boxes_together,       D, l_c4, UC_none,  {8, 0}},
   {"QUADRUPLE BOXES WORKING COUNTERCLOCKWISE",concept_quad_boxes_together,     D, l_c4, UC_none,  {9, 0}},
   {"QUADRUPLE BOXES WORKING TOWARD THE CENTER",concept_quad_boxes_together,    D, l_c4, UC_none,  {12, 0}},
   {"QUADRUPLE BOXES WORKING INWARD",        concept_quad_boxes_together,       D, l_c4, UC_none,  {12, 0}},
   {"QUADRUPLE DIAMONDS WORKING TOGETHER",   concept_quad_diamonds_together,    D, l_c4a, UC_qdwt, {0, 0}},
   {"QUADRUPLE DIAMONDS WORKING TOWARD THE CENTER",concept_quad_diamonds_together, D, l_c4, UC_none, {12, 0}},
   {"QUADRUPLE DIAMONDS WORKING INWARD",     concept_quad_diamonds_together,    D, l_c4, UC_none, {12, 0}},
   {"QUADRUPLE 1/4 TAGS WORKING TOGETHER",   concept_quad_diamonds_together,    D, l_c4a, UC_none, {0}},
   {"QUADRUPLE 1/4 TAGS WORKING RIGHT",      concept_quad_diamonds_together,    D, l_c4a, UC_none, {1}},
   {"QUADRUPLE 1/4 TAGS WORKING LEFT",       concept_quad_diamonds_together,    D, l_c4a, UC_none, {2}},
   {"QUADRUPLE 1/4 TAGS WORKING TOWARD THE CENTER",concept_quad_diamonds_together,D, l_c4a, UC_none, {12}},
   {"QUADRUPLE 1/4 TAGS WORKING INWARD",     concept_quad_diamonds_together,    D, l_c4a, UC_none, {12}},
   {"QUADRUPLE 1/4 LINES WORKING TOGETHER",  concept_quad_diamonds_together,    D, l_c4a, UC_none, {0}},
   {"QUADRUPLE 1/4 LINES WORKING RIGHT",     concept_quad_diamonds_together,    D, l_c4a, UC_none, {1}},
   {"QUADRUPLE 1/4 LINES WORKING LEFT",      concept_quad_diamonds_together,    D, l_c4a, UC_none, {2}},
   {"QUADRUPLE 1/4 LINES WORKING TOWARD THE CENTER",concept_quad_diamonds_together,D, l_c4a, UC_none, {12}},
   {"QUADRUPLE 1/4 LINES WORKING INWARD",    concept_quad_diamonds_together,    D, l_c4a, UC_none, {12}},
   {"QUINTUPLE COLUMNS WORKING RIGHT",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X5,      0, 5}},
   {"QUINTUPLE COLUMNS WORKING LEFT",        concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X5,      0, 5}},
   {"QUINTUPLE COLUMNS WORKING CLOCKWISE",   concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X5,      0, 5}},
   {"QUINTUPLE COLUMNS WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X5,      0, 5}},
   {"QUINTUPLE LINES WORKING FORWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X5,      1, 5}},
   {"QUINTUPLE LINES WORKING BACKWARD",      concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X5,      1, 5}},
   {"QUINTUPLE LINES WORKING CLOCKWISE",     concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X5,      1, 5}},
   {"QUINTUPLE LINES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,  D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X5,      1, 5}},
   {"QUINTUPLE WAVES WORKING FORWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X5,      3, 5}},
   {"QUINTUPLE WAVES WORKING BACKWARD",      concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X5,      3, 5}},
   {"QUINTUPLE WAVES WORKING CLOCKWISE",     concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X5,      3, 5}},
   {"QUINTUPLE WAVES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,  D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X5,      3, 5}},
   {"SEXTUPLE COLUMNS WORKING RIGHT",        concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X6,      0, 6}},
   {"SEXTUPLE COLUMNS WORKING LEFT",         concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X6,      0, 6}},
   {"SEXTUPLE COLUMNS WORKING CLOCKWISE",    concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X6,      0, 6}},
   {"SEXTUPLE COLUMNS WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std, D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X6,      0, 6}},
   {"SEXTUPLE LINES WORKING FORWARD",        concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X6,      1, 6}},
   {"SEXTUPLE LINES WORKING BACKWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X6,      1, 6}},
   {"SEXTUPLE LINES WORKING CLOCKWISE",      concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X6,      1, 6}},
   {"SEXTUPLE LINES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,   D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X6,      1, 6}},
   {"SEXTUPLE WAVES WORKING FORWARD",        concept_multiple_lines_tog,        D, l_c4a, UC_none, {0,  CONCPROP__NEEDK_4X6,      3, 6}},
   {"SEXTUPLE WAVES WORKING BACKWARD",       concept_multiple_lines_tog,        D, l_c4a, UC_none, {2,  CONCPROP__NEEDK_4X6,      3, 6}},
   {"SEXTUPLE WAVES WORKING CLOCKWISE",      concept_multiple_lines_tog_std,    D, l_c4, UC_none,  {8,  CONCPROP__NEEDK_4X6,      3, 6}},
   {"SEXTUPLE WAVES WORKING COUNTERCLOCKWISE",concept_multiple_lines_tog_std,   D, l_c4, UC_none,  {9,  CONCPROP__NEEDK_4X6,      3, 6}},
   {"CENTER TRIPLE COLUMN",                  concept_in_out_std,                D, l_c2, UC_none, {0, CONCPROP__NEEDK_CTR_1X4, 0}},
   {"OUTSIDE TRIPLE COLUMNS",                concept_in_out_std,                D, l_c2, UC_none, {8+0, CONCPROP__NEEDK_END_1X4, 0}},
   {"CENTER TRIPLE LINE",                    concept_in_out_std,                D, l_c2, UC_none, {1, CONCPROP__NEEDK_CTR_1X4, 0}},
   {"OUTSIDE TRIPLE LINES",                  concept_in_out_std,                D, l_c2, UC_none, {8+1, CONCPROP__NEEDK_END_1X4, 0}},
   {"CENTER TRIPLE WAVE",                    concept_in_out_std,                D, l_c2, UC_none, {3, CONCPROP__NEEDK_CTR_1X4, 0}},
   {"OUTSIDE TRIPLE WAVES",                  concept_in_out_std,                D, l_c2, UC_none, {8+3, CONCPROP__NEEDK_END_1X4, 0}},
   {"CENTER TRIPLE BOX",                     concept_in_out_nostd,              D, l_c1, UC_none, {4,   CONCPROP__NEEDK_CTR_2X2, 0}},
   {"OUTSIDE TRIPLE BOXES",                  concept_in_out_nostd,              D, l_c1, UC_none, {8+4, CONCPROP__NEEDK_END_2X2, 0}},
   {"CENTER TRIPLE DIAMOND",                 concept_in_out_nostd,              D, l_c3a, UC_none, {5, CONCPROP__NEEDK_CTR_DMD, 0}},
   {"OUTSIDE TRIPLE DIAMONDS",               concept_in_out_nostd,              D, l_c3a, UC_none, {8+5, CONCPROP__NEEDK_END_DMD, 0}},
   {"CENTER Z",                              concept_in_out_nostd,              D, l_c3a, UC_none, {6, CONCPROP__NEEDK_3X6, 0}},
   {"OUTSIDE TRIPLE Z's",                    concept_in_out_nostd,              D, l_c3a, UC_none, {8+6, CONCPROP__NEEDK_3X6, 0}},
   {"CENTER TIDAL COLUMN",                   concept_in_out_nostd,              D, l_c4a, UC_none, {80+0, CONCPROP__NEEDK_3X8, 0}},
   {"CENTER TIDAL LINE",                     concept_in_out_nostd,              D, l_c4a, UC_none, {80+1, CONCPROP__NEEDK_3X8, 0}},
   {"CENTER TIDAL WAVE",                     concept_in_out_nostd,              D, l_c4a, UC_none, {80+3, CONCPROP__NEEDK_3X8, 0}},
   {"CENTER TRIPLE TWIN COLUMNS",            concept_in_out_std,                D, l_c4a, UC_none, {32+0, CONCPROP__NEEDK_4X6, 0}},
   {"OUTSIDE TRIPLE TWIN COLUMNS",           concept_in_out_std,                D, l_c4a, UC_none, {32+8+0, CONCPROP__NEEDK_4X6, 0}},
   {"CENTER TRIPLE TWIN LINES",              concept_in_out_std,                D, l_c4a, UC_none, {32+1, CONCPROP__NEEDK_4X6, 0}},
   {"OUTSIDE TRIPLE TWIN LINES",             concept_in_out_std,                D, l_c4a, UC_none, {32+8+1, CONCPROP__NEEDK_4X6, 0}},
   {"CENTER TRIPLE TWIN WAVES",              concept_in_out_std,                D, l_c4a, UC_none, {32+3, CONCPROP__NEEDK_4X6, 0}},
   {"OUTSIDE TRIPLE TWIN WAVES",             concept_in_out_std,                D, l_c4a, UC_none, {32+8+3, CONCPROP__NEEDK_4X6, 0}},
   {"CENTER TRIPLE TWIN COLUMNS OF 3",       concept_in_out_std,                D, l_c4a, UC_none, {48+0, CONCPROP__NEEDK_3X6, 0}},
   {"OUTSIDE TRIPLE TWIN COLUMNS OF 3",      concept_in_out_std,                D, l_c4a, UC_none, {48+8+0, CONCPROP__NEEDK_3X6, 0}},
   {"CENTER TRIPLE TWIN LINES OF 3",         concept_in_out_std,                D, l_c4a, UC_none, {48+1, CONCPROP__NEEDK_3X6, 0}},
   {"OUTSIDE TRIPLE TWIN LINES OF 3",        concept_in_out_std,                D, l_c4a, UC_none, {48+8+1, CONCPROP__NEEDK_3X6, 0}},
   {"CENTER TRIPLE TWIN WAVES OF 3",         concept_in_out_std,                D, l_c4a, UC_none, {48+3, CONCPROP__NEEDK_3X6, 0}},
   {"OUTSIDE TRIPLE TWIN WAVES OF 3",        concept_in_out_std,                D, l_c4a, UC_none, {48+8+3, CONCPROP__NEEDK_3X6, 0}},
   {"CENTER PHANTOM COLUMNS",                concept_in_out_std,                D, l_c3a, UC_none, {16+0,   CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"OUTSIDE PHANTOM COLUMNS",               concept_in_out_std,                D, l_c3a, UC_none, {16+8+0, CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"CENTER PHANTOM LINES",                  concept_in_out_std,                D, l_c3, UC_none,  {16+1, CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"OUTSIDE PHANTOM LINES",                 concept_in_out_std,                D, l_c3, UC_none,  {16+8+1, CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"CENTER PHANTOM WAVES",                  concept_in_out_std,                D, l_c3, UC_none,  {16+3, CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"OUTSIDE PHANTOM WAVES",                 concept_in_out_std,                D, l_c3, UC_none,  {16+8+3, CONCPROP__NEEDK_QUAD_1X4, 0}},
   {"CENTER PHANTOM BOXES",                  concept_in_out_nostd,              D, l_c4a, UC_none, {16+4, CONCPROP__NEEDK_2X8, 0}},
   {"OUTSIDE PHANTOM BOXES",                 concept_in_out_nostd,              D, l_c4a, UC_none, {16+8+4, CONCPROP__NEEDK_2X8, 0}},
   {"CENTER PHANTOM DIAMONDS",               concept_in_out_nostd,              D, l_c4a, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"OUTSIDE PHANTOM DIAMONDS",              concept_in_out_nostd,              D, l_c4a, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"CENTER PHANTOM DIAMOND SPOTS",          concept_in_out_nostd,              D, l_c4a, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"OUTSIDE PHANTOM DIAMOND SPOTS",         concept_in_out_nostd,              D, l_c4a, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"CENTER PHANTOM GENERAL 1/4 TAGS",       concept_in_out_nostd,              D, l_c4a, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"OUTSIDE PHANTOM GENERAL 1/4 TAGS",      concept_in_out_nostd,              D, l_c4a, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"12 MATRIX CENTER PHANTOM COLUMNS",      concept_in_out_std,                D, l_c3x, UC_none, {64+0,   0, 0}},
   {"12 MATRIX OUTSIDE PHANTOM COLUMNS",     concept_in_out_std,                D, l_c3x, UC_none, {64+8+0, 0, 0}},
   {"12 MATRIX CENTER PHANTOM LINES",        concept_in_out_std,                D, l_c3x, UC_none, {64+1, 0, 0}},
   {"12 MATRIX OUTSIDE PHANTOM LINES",       concept_in_out_std,                D, l_c3x, UC_none, {64+8+1, 0, 0}},
   {"TWIN PHANTOM DIAMONDS",                 concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINDMD, CMD_MISC__VERIFY_DMD_LIKE, 0}},
   {"TWIN PHANTOM DIAMOND SPOTS",            concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_4X6, 0, 0}},
   {"TWIN PHANTOM 1/4 TAGS",                 concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINQTAG, CMD_MISC__VERIFY_1_4_TAG, 0}},
   {"TWIN PHANTOM 3/4 TAGS",                 concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINQTAG, CMD_MISC__VERIFY_3_4_TAG, 0}},
   {"TWIN PHANTOM 1/4 LINES",                concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINQTAG, CMD_MISC__VERIFY_REAL_1_4_LINE, 0}},
   {"TWIN PHANTOM 3/4 LINES",                concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINQTAG, CMD_MISC__VERIFY_REAL_3_4_LINE, 0}},
   {"TWIN PHANTOM GENERAL 1/4 TAGS",         concept_do_divided_diamonds,       D, l_c3x, UC_none, {phantest_impossible, CONCPROP__NEEDK_TWINQTAG, CMD_MISC__VERIFY_QTAG_LIKE, 0}},
   {"TWIN PHANTOM POINT-TO-POINT DIAMONDS",  concept_do_divided_diamonds,       D, l_c4, UC_none,  {phantest_impossible, CONCPROP__NEEDK_TWINDMD, CMD_MISC__VERIFY_DMD_LIKE, 1}},
   {"TWIN PHANTOM POINT-TO-POINT DIAMOND SPOTS", concept_do_divided_diamonds,   D, l_c4, UC_none,  {phantest_impossible, CONCPROP__NEEDK_4X6, 0, 1}},

   {"CRAZY PHANTOM COLUMNS",                 concept_phan_crazy,                D, l_c4, UC_none, {0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"REVERSE CRAZY PHANTOM COLUMNS",         concept_phan_crazy,                D, l_c4, UC_none, {8+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"@a CRAZY PHANTOM COLUMNS",              concept_frac_phan_crazy,           D, l_c4, UC_none, {16+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"@a REVERSE CRAZY PHANTOM COLUMNS",      concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"CRAZY PHANTOM LINES",                   concept_phan_crazy,                D, l_c4, UC_none, {1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"REVERSE CRAZY PHANTOM LINES",           concept_phan_crazy,                D, l_c4, UC_none, {8+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"@a CRAZY PHANTOM LINES",                concept_frac_phan_crazy,           D, l_c4, UC_none, {16+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"@a REVERSE CRAZY PHANTOM LINES",        concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"CRAZY PHANTOM WAVES",                   concept_phan_crazy,                D, l_c4, UC_none, {3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"REVERSE CRAZY PHANTOM WAVES",           concept_phan_crazy,                D, l_c4, UC_none, {8+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"@a CRAZY PHANTOM WAVES",                concept_frac_phan_crazy,           D, l_c4, UC_none, {16+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"@a REVERSE CRAZY PHANTOM WAVES",        concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"CRAZY OFFSET COLUMNS",                  concept_phan_crazy,                D, l_c4, UC_none, {64+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"REVERSE CRAZY OFFSET COLUMNS",          concept_phan_crazy,                D, l_c4, UC_none, {64+8+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"@a CRAZY OFFSET COLUMNS",               concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"@a REVERSE CRAZY OFFSET COLUMNS",       concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+8+0, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_COLS}},
   {"CRAZY OFFSET LINES",                    concept_phan_crazy,                D, l_c4, UC_none, {64+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"REVERSE CRAZY OFFSET LINES",            concept_phan_crazy,                D, l_c4, UC_none, {64+8+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"@a CRAZY OFFSET LINES",                 concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"@a REVERSE CRAZY OFFSET LINES",         concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+8+1, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_LINES}},
   {"CRAZY OFFSET WAVES",                    concept_phan_crazy,                D, l_c4, UC_none, {64+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"REVERSE CRAZY OFFSET WAVES",            concept_phan_crazy,                D, l_c4, UC_none, {64+8+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"@a CRAZY OFFSET WAVES",                 concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"@a REVERSE CRAZY OFFSET WAVES",         concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+8+3, CONCPROP__NEEDK_4X4, CMD_MISC__VERIFY_WAVES}},
   {"CRAZY PHANTOM BOXES",                   concept_phan_crazy,                D, l_c4, UC_none, {4,      CONCPROP__NEEDK_2X8, 0}},
   {"REVERSE CRAZY PHANTOM BOXES",           concept_phan_crazy,                D, l_c4, UC_none, {8+4,    CONCPROP__NEEDK_2X8, 0}},
   {"@a CRAZY PHANTOM BOXES",                concept_frac_phan_crazy,           D, l_c4, UC_none, {16+4,   CONCPROP__NEEDK_2X8, 0}},
   {"@a REVERSE CRAZY PHANTOM BOXES",        concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+4, CONCPROP__NEEDK_2X8, 0}},
   {"CRAZY DIAGONAL BOXES",                  concept_phan_crazy,                D, l_c4, UC_none, {64+4, CONCPROP__NEEDK_4X4, 0}},
   {"REVERSE CRAZY DIAGONAL BOXES",          concept_phan_crazy,                D, l_c4, UC_none, {64+8+4, CONCPROP__NEEDK_4X4, 0}},
   {"@a CRAZY DIAGONAL BOXES",               concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+4, CONCPROP__NEEDK_4X4, 0}},
   {"@a REVERSE CRAZY DIAGONAL BOXES",       concept_frac_phan_crazy,           D, l_c4, UC_none, {64+16+8+4, CONCPROP__NEEDK_4X4, 0}},
   {"CRAZY PHANTOM DIAMONDS",                concept_phan_crazy,                D, l_c4, UC_none, {5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"REVERSE CRAZY PHANTOM DIAMONDS",        concept_phan_crazy,                D, l_c4, UC_none, {8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"@a CRAZY PHANTOM DIAMONDS",             concept_frac_phan_crazy,           D, l_c4, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"@a REVERSE CRAZY PHANTOM DIAMONDS",     concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_DMD_LIKE}},
   {"CRAZY PHANTOM DIAMOND SPOTS",           concept_phan_crazy,                D, l_c4, UC_none, {5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"REVERSE CRAZY PHANTOM DIAMOND SPOTS",   concept_phan_crazy,                D, l_c4, UC_none, {8+5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"@a CRAZY PHANTOM DIAMOND SPOTS",        concept_frac_phan_crazy,           D, l_c4, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"@a REVERSE CRAZY PHANTOM DIAMOND SPOTS",concept_frac_phan_crazy,           D, l_c4, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, 0}},
   {"CRAZY PHANTOM GENERAL 1/4 TAGS",        concept_phan_crazy,                D, l_c4, UC_none, {5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"REVERSE CRAZY PHANTOM GENERAL 1/4 TAGS",concept_phan_crazy,                D, l_c4, UC_none, {8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"@a CRAZY PHANTOM GENERAL 1/4 TAGS",     concept_frac_phan_crazy,           D, l_c4, UC_none, {16+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"@a REVERSE CRAZY PHANTOM GENERAL 1/4 TAGS",concept_frac_phan_crazy,        D, l_c4, UC_none, {16+8+5, CONCPROP__NEEDK_4D_4PTPD, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"BIG BLOCK",                             concept_do_phantom_2x4,            D, l_c3a, UC_none,        {phantest_only_one, 1, MPKIND__STAG, 0}},
   {"STAIRSTEP LINES",                       concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 1, MPKIND__NONE, spcmap_ladder}},
   {"LADDER LINES",                          concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 1, MPKIND__NONE, spcmap_stairst}},
   {"OFFSET LINES",                          concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 1, 0, 0}},
   {"OFFSET LINE",                           concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 1, 0, 1}},
   {"Z LINES",                               concept_distorted,                 D, l_c4a, UC_none,        {disttest_z, 1, 0, 0}},
   {"DISTORTED LINES",                       concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 1, 0, 0}},
   {"DIAGONAL LINE",                         concept_single_diagonal,           D, l_c1, UC_none,         {LOOKUP_DIAG_CLW+1}},
   {"@6 IN YOUR DIAGONAL LINE",              concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIAG_CLW+1}},
   {"@6 IN YOUR OFFSET LINE",                concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_OFFS_CLW+1}},
   {"@6 IN YOUR STAGGERED LINE",             concept_so_and_so_only,            D, l_c4a, UC_none,        {selective_key_disc_dist, 0, LOOKUP_STAG_CLW+1}},
   {"DIAGONAL LINES",                        concept_double_diagonal,           D, l_c4a, UC_none,        {1, 0}},
   {"DIAGONAL LINES OF 3",                   concept_double_diagonal,           D, l_c1, UC_none,         {1, 1}},
   {"STAGGERED LINES OF 3",                  concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 64+1, 0, 0}},
   {"LINES OF 3",                            concept_distorted,                 D, l_c1, UC_none,         {disttest_any, 32+1}},
   {"OFFSET TIDAL LINE",                     concept_distorted,                 D, l_c3, UC_none,         {disttest_offset, 8+1}},
   {"DISTORTED TIDAL LINE",                  concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 8+1}},
   {"DISTORTED LINE OF 6",                   concept_double_diagonal,           D, l_c3, UC_none,         {1, 2}},
   {"BENT LINES",                            concept_dblbent,                   D, l_c4, UC_none,         {1}},
   {"DOUBLE BENT TIDAL LINE",                concept_dblbent,                   D, l_c4, UC_none,         {8+1}},
   {"@6 IN YOUR DOUBLE BENT LINE",           concept_so_and_so_only,            D, l_c4, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DBL_BENT+1}},
   {"@6 IN YOUR DISTORTED LINE",             concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, 1}},
   {"PHANTOM BIG BLOCK LINES",               concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 1, MPKIND__STAG, 0}},
   {"PHANTOM STAIRSTEP LINES",               concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 1, MPKIND__NONE, spcmap_ladder}},
   {"PHANTOM LADDER LINES",                  concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 1, MPKIND__NONE, spcmap_stairst}},
   {"PHANTOM OFFSET LINES",                  concept_do_phantom_2x4,            D, l_c4a, UC_pofl,        {phantest_both, 1, MPKIND__OFFS_BOTH_FULL, 0}},
   {"PARALLELOGRAM",                         concept_parallelogram,             D, l_c2, UC_none,         {0}},
   {"PARALLELOGRAM DIAMONDS",                concept_parallelogram,             D, l_c3a, UC_none,        {1}},
   {"OFFSET 1/4 TAG",                        concept_distorted,                 D, l_c4, UC_none,         {disttest_any, 256, CMD_MISC__VERIFY_1_4_TAG}},
   {"OFFSET 3/4 TAG",                        concept_distorted,                 D, l_c4, UC_none,         {disttest_any, 256, CMD_MISC__VERIFY_3_4_TAG}},
   {"STAGGER DIAMONDS",                      concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 1, CMD_MISC__VERIFY_DMD_LIKE}},
   {"STAGGER 1/4 TAG",                       concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_1_4_TAG}},
   {"STAGGER 3/4 TAG",                       concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_3_4_TAG}},
   {"STAGGER 1/4 LINE",                      concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_REAL_1_4_LINE}},
   {"STAGGER 3/4 LINE",                      concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_REAL_3_4_LINE}},
   {"STAGGER GENERAL 1/4 TAG",               concept_do_phantom_stag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"DIAGONAL DIAMONDS",                     concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 1, CMD_MISC__VERIFY_DMD_LIKE}},
   {"DIAGONAL 1/4 TAG",                      concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_1_4_TAG}},
   {"DIAGONAL 3/4 TAG",                      concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_3_4_TAG}},
   {"DIAGONAL 1/4 LINE",                     concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_REAL_1_4_LINE}},
   {"DIAGONAL 3/4 LINE",                     concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_REAL_3_4_LINE}},
   {"DIAGONAL GENERAL 1/4 TAG",              concept_do_phantom_diag_qtg,       D, l_c4, UC_none, {phantest_only_one, 0, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"DISTORTED DIAMONDS",                    concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_DMD_LIKE}},
   {"DISTORTED 1/4 TAG",                     concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_1_4_TAG}},
   {"DISTORTED 3/4 TAG",                     concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_3_4_TAG}},
   {"DISTORTED 1/4 LINE",                    concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_REAL_1_4_LINE}},
   {"DISTORTED 3/4 LINE",                    concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_REAL_3_4_LINE}},
   {"DISTORTED GENERAL 1/4 TAG",             concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, CMD_MISC__VERIFY_QTAG_LIKE}},
   {"DISTORTED DIAMOND SPOTS",               concept_distorted,                 D, l_c3x, UC_none, {disttest_any, 16, 0}},
   {"BIG BLOCK WAVES",                       concept_do_phantom_2x4,            D, l_c3a, UC_none,        {phantest_only_one, 3, MPKIND__STAG, 0}},
   {"STAIRSTEP WAVES",                       concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 3, MPKIND__NONE, spcmap_ladder}},
   {"LADDER WAVES",                          concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 3, MPKIND__NONE, spcmap_stairst}},
   {"OFFSET WAVES",                          concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 3, 0, 0}},
   {"OFFSET WAVE",                           concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 3, 0, 1}},
   {"Z WAVES",                               concept_distorted,                 D, l_c4a, UC_none,        {disttest_z, 3, 0, 0}},
   {"DISTORTED WAVES",                       concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 3, 0, 0}},
   {"DIAGONAL WAVE",                         concept_single_diagonal,           D, l_c1, UC_none,         {LOOKUP_DIAG_CLW+3}},
   {"@6 IN YOUR DIAGONAL WAVE",              concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIAG_CLW+3}},
   {"@6 IN YOUR OFFSET WAVE",                concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_OFFS_CLW+3}},
   {"@6 IN YOUR STAGGERED WAVE",             concept_so_and_so_only,            D, l_c4a, UC_none,        {selective_key_disc_dist, 0, LOOKUP_STAG_CLW+3}},
   {"DIAGONAL WAVES",                        concept_double_diagonal,           D, l_c4a, UC_none,        {3, 0}},
   {"DIAGONAL WAVES OF 3",                   concept_double_diagonal,           D, l_c1, UC_none,         {3, 1}},
   {"STAGGERED WAVES OF 3",                  concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 64+3, 0, 0}},
   {"WAVES OF 3",                            concept_distorted,                 D, l_c1, UC_none,         {disttest_any, 32+3}},
   {"OFFSET TIDAL WAVE",                     concept_distorted,                 D, l_c3, UC_none,         {disttest_offset, 8+3}},
   {"DISTORTED TIDAL WAVE",                  concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 8+3}},
   {"DISTORTED WAVE OF 6",                   concept_double_diagonal,           D, l_c3, UC_none,         {3, 2}},
   {"BENT WAVES",                            concept_dblbent,                   D, l_c4, UC_none,         {3}},
   {"DOUBLE BENT TIDAL WAVE",                concept_dblbent,                   D, l_c4, UC_none,         {8+3}},
   {"@6 IN YOUR DOUBLE BENT WAVE",           concept_so_and_so_only,            D, l_c4, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DBL_BENT+3}},
   {"@6 IN YOUR DISTORTED WAVE",             concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, 3}},
   {"PHANTOM BIG BLOCK WAVES",               concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 3, MPKIND__STAG, 0}},
   {"PHANTOM STAIRSTEP WAVES",               concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 3, MPKIND__NONE, spcmap_ladder}},
   {"PHANTOM LADDER WAVES",                  concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 3, MPKIND__NONE, spcmap_stairst}},
   {"PHANTOM OFFSET WAVES",                  concept_do_phantom_2x4,            D, l_c4a, UC_none,        {phantest_both, 3, MPKIND__OFFS_BOTH_FULL, 0}},
   {"@6 IN YOUR DISTORTED BOX",              concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIST_BOX}},
   {"@6 IN YOUR DIAGONAL BOX",               concept_so_and_so_only,            D, l_c3, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIAG_BOX}},
   {"@6 IN YOUR STAGGERED BOX",              concept_so_and_so_only,            D, l_c4a, UC_none,        {selective_key_disc_dist, 0, LOOKUP_STAG_BOX}},
   {"@6 IN YOUR DISTORTED DIAMOND",          concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIST_DMD+7}},
   {"@6 IN YOUR Z",                          concept_so_and_so_only,            D, l_c3, UC_none,         {selective_key_disc_dist, 0, LOOKUP_Z}},
   {"STAGGER",                               concept_do_phantom_2x4,            D, l_c2, UC_none,         {phantest_only_one, 0, MPKIND__STAG, 0}},
   {"STAIRSTEP COLUMNS",                     concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 0, MPKIND__NONE, spcmap_stairst}},
   {"LADDER COLUMNS",                        concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_only_one, 0, MPKIND__NONE, spcmap_ladder}},
   {"OFFSET COLUMNS",                        concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 0, 0, 0}},
   {"OFFSET COLUMN",                         concept_distorted,                 D, l_c2, UC_none,         {disttest_offset, 0, 0, 1}},
   {"Z COLUMNS",                             concept_distorted,                 D, l_c4a, UC_none,        {disttest_z, 0, 0, 0}},
   {"DISTORTED COLUMNS",                     concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 0, 0, 0}},
   {"DIAGONAL COLUMN",                       concept_single_diagonal,           D, l_c1, UC_none,         {LOOKUP_DIAG_CLW+2}},
   {"@6 IN YOUR DIAGONAL COLUMN",            concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DIAG_CLW+2}},
   {"@6 IN YOUR OFFSET COLUMN",              concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, LOOKUP_OFFS_CLW+2}},
   {"@6 IN YOUR STAGGERED COLUMN",           concept_so_and_so_only,            D, l_c4a, UC_none,        {selective_key_disc_dist, 0, LOOKUP_STAG_CLW+2}},
   {"DIAGONAL COLUMNS",                      concept_double_diagonal,           D, l_c4a, UC_none,        {0, 0}},
   {"DIAGONAL COLUMNS OF 3",                 concept_double_diagonal,           D, l_c1, UC_none,         {0, 1}},
   {"STAGGERED COLUMNS OF 3",                concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 64+0, 0, 0}},
   {"COLUMNS OF 3",                          concept_distorted,                 D, l_c1, UC_none,         {disttest_any, 32+0}},
   {"OFFSET TIDAL COLUMN",                   concept_distorted,                 D, l_c3, UC_none,         {disttest_offset, 8+0}},
   {"DISTORTED TIDAL COLUMN",                concept_distorted,                 D, l_c3, UC_none,         {disttest_any, 8+0}},
   {"DISTORTED COLUMN OF 6",                 concept_double_diagonal,           D, l_c3, UC_none,         {0, 2}},
   {"BENT COLUMNS",                          concept_dblbent,                   D, l_c4, UC_none,         {0}},
   {"DOUBLE BENT TIDAL COLUMN",              concept_dblbent,                   D, l_c4, UC_none,         {8+0}},
   {"@6 IN YOUR DOUBLE BENT COLUMN",         concept_so_and_so_only,            D, l_c4, UC_none,         {selective_key_disc_dist, 0, LOOKUP_DBL_BENT+2}},
   {"@6 IN YOUR DISTORTED COLUMN",           concept_so_and_so_only,            D, l_c1, UC_none,         {selective_key_disc_dist, 0, 2}},
   {"PHANTOM STAGGER COLUMNS",               concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 0, MPKIND__STAG, 0}},
   {"PHANTOM STAIRSTEP COLUMNS",             concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 0, MPKIND__NONE, spcmap_stairst}},
   {"PHANTOM LADDER COLUMNS",                concept_do_phantom_2x4,            D, l_c4, UC_none,         {phantest_both, 0, MPKIND__NONE, spcmap_ladder}},
   {"PHANTOM OFFSET COLUMNS",                concept_do_phantom_2x4,            D, l_c4a, UC_pofc,        {phantest_both, 0, MPKIND__OFFS_BOTH_FULL, 0}},
   {"O",                                     concept_do_phantom_2x4,            D, l_c1, UC_none,         {phantest_only_first_one, 0, MPKIND__O_SPOTS, 0}},
   {"BUTTERFLY",                             concept_do_phantom_2x4,            D, l_c1, UC_none,         {phantest_only_second_one, 0, MPKIND__X_SPOTS, 0}},
   {"PHANTOM BUTTERFLY OR O",                concept_do_phantom_2x4,            D, l_c4a, UC_pob,        {phantest_both, 0, MPKIND__NONE, spcmap_but_o}},
   {"BENT BOXES",                            concept_dblbent,                   D, l_c4, UC_none,         {16+4}},
   {"SPLIT",                                 concept_split,                   L+D, l_mainstream, UC_none},
   {"ONCE REMOVED",                          concept_once_removed,              D, l_c2, UC_none,         {0}},
   {"TWICE REMOVED",                         concept_once_removed,              D, l_c3x, UC_none,         {2}},
   {"THRICE REMOVED",                        concept_once_removed,              D, l_c3x, UC_none,         {3}},
   {"ONCE REMOVED DIAMONDS",                 concept_once_removed,              D, l_c3a, UC_none,        {1}},
   {"MAGIC",                                 concept_magic,                   L+D, l_c1, UC_magic},
   {"DIAGONAL BOX",                          concept_do_both_boxes,             D, l_c3, UC_none,         {spcmap_2x4_diagonal, 97, FALSE}},
   {"TRAPEZOID",                             concept_do_both_boxes,             D, l_c3, UC_none,         {spcmap_2x4_trapezoid, 97, FALSE}},
   {"OVERLAPPED DIAMONDS",                   concept_overlapped_diamond,        D, l_c4, UC_none,         {0}},
   {"OVERLAPPED LINES",                      concept_overlapped_diamond,        D, l_c4, UC_none,         {1}},
   {"OVERLAPPED WAVES",                      concept_overlapped_diamond,        D, l_c4, UC_none,         {3}},
   {"INTERLOCKED PARALLELOGRAM",             concept_do_both_boxes,             D, l_c3x, UC_none,        {spcmap_2x4_int_pgram, 97, TRUE}},
   {"INTERLOCKED BOXES",                     concept_misc_distort,              D, l_c3x, UC_none,        {3, 0, 0}},
   {"TWIN PARALLELOGRAMS",                   concept_misc_distort,              D, l_c3x, UC_none,        {2, 0, 0}},
   {"Z",                                     concept_misc_distort,              D, l_c3, UC_none,         {0, 0, 0, 1}},
   {"EACH Z",                                concept_misc_distort,              D, l_c3, UC_none,         {0, 0, 0, 2}},
   {"INTERLOCKED Z's",                       concept_misc_distort,              D, l_c4a, UC_none,        {0, 0, 8, 2}},
   {"Z DIAMOND",                             concept_misc_distort,              D, l_c4, UC_none,         {6, 0, 0, 1}},
   {"Z DIAMONDS",                            concept_misc_distort,              D, l_c4, UC_none,         {6, 0, 0, 2}},
   {"JAY",                                   concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 0,  0x0000}},
   {"BACK-TO-FRONT JAY",                     concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 8,  0x0000}},
   {"BACK-TO-BACK JAY",                      concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 16, 0x0000}},
   {"FRONT JAY",                             concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 0,  0x0000}},
   {"BACK JAY",                              concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 0,  0xAAAA}},
   {"LEFT JAY",                              concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 0,  0x5555}},
   {"RIGHT JAY",                             concept_misc_distort,              D, l_c3a, UC_none,        {1, 0, 0,  0xFFFF}},
   {"FACING PARALLELOGRAM",                  concept_misc_distort,              D, l_c3a, UC_none,        {4, 0, 0}},
   {"BACK-TO-FRONT PARALLELOGRAM",           concept_misc_distort,              D, l_c3a, UC_none,        {4, 0, 8}},
   {"BACK-TO-BACK PARALLELOGRAM",            concept_misc_distort,              D, l_c3a, UC_none,        {4, 0, 16}},
   {"BLOCKS",                                concept_do_phantom_2x2,            D, l_c1, UC_none,         {spcmap_blocks, phantest_2x2_only_two}},
   {"IN YOUR BLOCKS",                        concept_do_phantom_2x2,            D, l_c1, UC_none,         {spcmap_blocks, phantest_2x2_only_two}},
   {"4 PHANTOM INTERLOCKED BLOCKS",          concept_do_phantom_2x2,            D, l_c4a, UC_pibl,        {spcmap_blocks, phantest_2x2_both}},
   {"TRIANGULAR BOXES",                      concept_triangular_boxes,          D, l_c4, UC_none,         {phantest_2x2_only_two, 0}},
   {"4 PHANTOM TRIANGULAR BOXES",            concept_triangular_boxes,          D, l_c4, UC_none,         {phantest_2x2_both, CONCPROP__NEEDK_4X4}},
   {"DISTORTED BLOCKS",                      concept_misc_distort,              D, l_c4a, UC_none,        {5, 0, 0}},
   {"LEFT",                                  concept_left,                    L+D, l_mainstream, UC_left},
   {"REVERSE",                               concept_reverse,                 L+D, l_mainstream, UC_none},
   {"CROSS",                                 concept_cross,                   L+D, l_mainstream, UC_cross},
   {"SINGLE",                                concept_single,                  L+D, l_mainstream, UC_none},
   {"SINGLE FILE",                           concept_singlefile,              L+D, l_mainstream, UC_none},  /* Not C3?  No, mainstream, for dixie style. */
   {"GRAND",                                 concept_grand,                   L+D, l_plus, UC_grand},
   {"MIRROR",                                concept_mirror,                    D, l_c3, UC_none},
   {"TRIANGLE",                              concept_triangle,                  D, l_c1, UC_none},
   {"LEADING TRIANGLE",                      concept_leadtriangle,              D, l_c3x, UC_none},
   {"DIAMOND",                               concept_diamond,                   D, l_c3x, UC_none},
   {"INTERLOCKED",                           concept_interlocked,             L+D, l_c1, UC_intlk},
   {"12 MATRIX",                             concept_12_matrix,                 D, l_c3x, UC_none},
   {"16 MATRIX",                             concept_16_matrix,                 D, l_c3x, UC_none},
   {"PHANTOM",                               concept_c1_phantom,              L+D, l_c1, UC_phan},
   {"FUNNY",                                 concept_funny,                     D, l_c2, UC_none},
   {"MATRIX",                                concept_matrix,                  G+D, l_c4, UC_none},
   {"ASSUME WAVES",                          concept_assume_waves,              D, l_c3a, UC_none,        {cr_wave_only,  0, 0}},
   {"ASSUME MINIWAVES",                      concept_assume_waves,              D, l_c3a, UC_none,        {cr_miniwaves,  0, 0}},
   {"ASSUME COUPLES",                        concept_assume_waves,              D, l_c3a, UC_none,        {cr_couples_only,0, 0}},
   {"ASSUME TWO-FACED LINES",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_2fl_only,   0, 0}},
   {"ASSUME ONE-FACED LINES",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_1fl_only,   0, 0}},
   {"ASSUME INVERTED LINES",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_magic_only, 0, 0}},
   {"ASSUME NORMAL BOXES",                   concept_assume_waves,              D, l_c3a, UC_none,        {cr_wave_only,  2, 0}},
   {"ASSUME INVERTED BOXES",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_magic_only, 2, 0}},
   {"ASSUME NORMAL COLUMNS",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_wave_only,  1, 0}},
   {"ASSUME MAGIC COLUMNS",                  concept_assume_waves,              D, l_c3a, UC_none,        {cr_magic_only, 1, 0}},
   {"ASSUME EIGHT CHAIN",                    concept_assume_waves,              D, l_c3a, UC_none,        {cr_li_lo,      1, 1}},
   {"ASSUME TRADE BY",                       concept_assume_waves,              D, l_c3a, UC_none,        {cr_li_lo,      1, 2}},
   {"ASSUME DPT",                            concept_assume_waves,              D, l_c3a, UC_none,        {cr_2fl_only,   1, 1}},
   {"ASSUME CDPT",                           concept_assume_waves,              D, l_c3a, UC_none,        {cr_2fl_only,   1, 2}},
   {"ASSUME FACING LINES",                   concept_assume_waves,              D, l_c3a, UC_none,        {cr_li_lo,      0, 1}},
   {"ASSUME BACK-TO-BACK LINES",             concept_assume_waves,              D, l_c3a, UC_none,        {cr_li_lo,      0, 2}},
   {"ASSUME GENERAL DIAMONDS",               concept_assume_waves,              D, l_c3a, UC_none,        {cr_diamond_like,0, 0}},
   {"ASSUME GENERAL 1/4 TAGS",               concept_assume_waves,              D, l_c3a, UC_none,        {cr_qtag_like,  0, 0}},
   {"ASSUME 1/4 TAGS",                       concept_assume_waves,              D, l_c3a, UC_none,        {cr_real_1_4_tag, 0, 0}},
   {"ASSUME RIGHT 1/4 TAGS",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_jleft,      0, 2}},
   {"ASSUME LEFT 1/4 TAGS",                  concept_assume_waves,              D, l_c3a, UC_none,        {cr_jright,     0, 2}},
   {"ASSUME 3/4 TAGS",                       concept_assume_waves,              D, l_c3a, UC_none,        {cr_real_3_4_tag, 0, 0}},
   {"ASSUME LEFT 3/4 TAGS",                  concept_assume_waves,              D, l_c3a, UC_none,        {cr_jleft,      0, 1}},
   {"ASSUME RIGHT 3/4 TAGS",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_jright,     0, 1}},
   {"ASSUME 1/4 LINES",                      concept_assume_waves,              D, l_c3a, UC_none,        {cr_real_1_4_line, 0, 0}},
   {"ASSUME RIGHT 1/4 LINES",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijleft,     0, 2}},
   {"ASSUME LEFT 1/4 LINES",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijright,    0, 2}},
   {"ASSUME 3/4 LINES",                      concept_assume_waves,              D, l_c3a, UC_none,        {cr_real_3_4_line, 0, 0}},
   {"ASSUME LEFT 3/4 LINES",                 concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijleft,     0, 1}},
   {"ASSUME RIGHT 3/4 LINES",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijright,    0, 1}},
   {"ASSUME NORMAL DIAMONDS",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_jright,     4, 0}},
   {"ASSUME FACING DIAMONDS",                concept_assume_waves,              D, l_c3a, UC_none,        {cr_jleft,      4, 0}},
   {"ASSUME NORMAL INTERLOCKED DIAMONDS",    concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijright,    4, 0}},
   {"ASSUME FACING INTERLOCKED DIAMONDS",    concept_assume_waves,              D, l_c3a, UC_none,        {cr_ijleft,     4, 0}},
   {"ASSUME NORMAL CASTS",                   concept_assume_waves,              D, l_c3a, UC_none,        {cr_alwaysfail, 0, 0}},
   {"WITH ACTIVE PHANTOMS",                  concept_active_phantoms,           D, l_c3a, UC_none},
   {"INVERT",                                concept_snag_mystic,             L+D, l_c4, UC_none,         {CMD_MISC2__SAID_INVERT}},
   {"CENTRAL",                               concept_central,                   D, l_c3, UC_none,         {CMD_MISC2__DO_CENTRAL}},
   {"INVERT CENTRAL",                        concept_central,                   D, l_c4, UC_none,         {CMD_MISC2__DO_CENTRAL | CMD_MISC2__INVERT_CENTRAL}},
   {"SNAG",                                  concept_snag_mystic,               D, l_c4, UC_none,         {CMD_MISC2__CENTRAL_SNAG}},
   {"INVERT SNAG",                           concept_snag_mystic,             F+D, l_c4, UC_none,         {CMD_MISC2__CENTRAL_SNAG | CMD_MISC2__INVERT_SNAG}},
   {"SNAG THE @6",                           concept_so_and_so_only,          L+D, l_c4, UC_none,         {selective_key_snag_anyone, 1, 0}},
   {"MYSTIC",                                concept_snag_mystic,               D, l_c4, UC_none,         {CMD_MISC2__CENTRAL_MYSTIC}},
   {"INVERT MYSTIC",                         concept_snag_mystic,             F+D, l_c4, UC_none,         {CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__INVERT_MYSTIC}},
   {"FAN",                                   concept_fan,                     L+D, l_c3, UC_none},
   {"YOYO",                                  concept_yoyo,                    L+D, l_c4, UC_none},
   {"FRACTAL",                               concept_fractal,                 L+D, l_c4, UC_none},
   {"STRAIGHT",                              concept_straight,                L+D, l_c4, UC_none},
   {"TWISTED",                               concept_twisted,                 L+D, l_c4, UC_none},
   {"@6 ARE STANDARD IN",                    concept_standard,                L+D, l_c4a, UC_none,        {0}},
   {"STABLE",                                concept_stable,                    D, l_c3a, UC_none,        {FALSE, FALSE}},
   {"@6 ARE STABLE",                         concept_so_and_so_stable,        F+D, l_c3a, UC_none,        {TRUE,  FALSE}},
   {"@b STABLE",                             concept_frac_stable,               D, l_c4, UC_none,         {FALSE, TRUE}},
   {"@6 ARE @b STABLE",                      concept_so_and_so_frac_stable,   F+D, l_c4, UC_none,         {TRUE,  TRUE}},
   {"EMULATE",                               concept_emulate,                   D, l_c4, UC_none},
   {"TRACE",                                 concept_trace,                     0, l_c3x, UC_none},
   {"OUTERACTING",                           concept_outeracting,               D, l_c4, UC_none},
   {"STRETCH",                               concept_old_stretch,               D, l_c1, UC_none},
   {"STRETCHED SETUP",                       concept_new_stretch,               D, l_c2, UC_none,         {16}},
   {"STRETCHED BOX",                         concept_new_stretch,               D, l_c2, UC_none,         {18}},
   {"STRETCHED LINE",                        concept_new_stretch,               D, l_c2, UC_none,         {1}},
   {"STRETCHED WAVE",                        concept_new_stretch,               D, l_c2, UC_none,         {3}},
   {"STRETCHED COLUMN",                      concept_new_stretch,               D, l_c2, UC_none,         {4}},
   {"STRETCHED DIAMOND",                     concept_new_stretch,               D, l_c3a, UC_none,        {19}},
   {"FERRIS",                                concept_ferris,                    D, l_c3x, UC_none,        {0, 0}},
   {"RELEASE",                               concept_ferris,                    D, l_c3a, UC_none,        {1, /*CONCPROP__NEEDK_3X4*/0}},
   {"CENTERS AND ENDS",                      concept_centers_and_ends,          0, l_mainstream, UC_none, {selector_centers, FALSE}},
   {"CENTER 6/OUTER 2",                      concept_centers_and_ends,          0, l_mainstream, UC_none, {selector_center6, FALSE}},
   {"CENTER 2/OUTER 6",                      concept_centers_and_ends,          0, l_mainstream, UC_none, {selector_center2, FALSE}},
   {"ENDS CONCENTRIC",                       concept_centers_or_ends,           D, l_c1, UC_none,         {selector_ends,    TRUE}},
   {"OUTER 2 CONCENTRIC",                    concept_centers_or_ends,           D, l_c1, UC_none,         {selector_outer2,  TRUE}},
   {"OUTER 6 CONCENTRIC",                    concept_centers_or_ends,           D, l_c1, UC_none,         {selector_outer6,  TRUE}},
   {"CENTERS AND ENDS CONCENTRIC",           concept_centers_and_ends,          0, l_c1, UC_none,         {selector_centers, TRUE}},
   {"CHECKPOINT",                            concept_checkpoint,                0, l_c2, UC_none,         {0}},
   {"REVERSE CHECKPOINT",                    concept_checkpoint,                0, l_c3, UC_none,         {1}},
   {"CHECKERBOARD",                          concept_checkerboard,              D, l_c3a, UC_none,        {s1x4, 0}},
   {"CHECKERBOX",                            concept_checkerboard,              D, l_c3a, UC_none,        {s2x2, 0}},
   {"CHECKERDIAMOND",                        concept_checkerboard,              D, l_c4a, UC_none,        {sdmd, 0}},
   {"@6 PREFERRED FOR TRADE, CHECKERBOARD",  concept_sel_checkerboard,          D, l_c3a, UC_none,        {s1x4, 8}},
   {"@6 PREFERRED FOR TRADE, CHECKERBOX",    concept_sel_checkerboard,          D, l_c3a, UC_none,        {s2x2, 8}},
   {"@6 PREFERRED FOR TRADE, CHECKERDIAMOND",concept_sel_checkerboard,          D, l_c4a, UC_none,        {sdmd, 8}},
   {"ORBITBOARD",                            concept_checkerboard,              D, l_c4, UC_none,         {s1x4, 2}},
   {"ORBITBOX",                              concept_checkerboard,              D, l_c4, UC_none,         {s2x2, 2}},
   {"ORBITDIAMOND",                          concept_checkerboard,              D, l_c4, UC_none,         {sdmd, 2}},
   {"TWIN ORBITBOARD",                       concept_checkerboard,              D, l_c4, UC_none,         {s1x4, 3}},
   {"TWIN ORBITBOX",                         concept_checkerboard,              D, l_c4, UC_none,         {s2x2, 3}},
   {"TWIN ORBITDIAMOND",                     concept_checkerboard,              D, l_c4, UC_none,         {sdmd, 3}},
   {"SHADOW LINE",                           concept_checkerboard,              D, l_c4a, UC_none,        {s1x4, 1}},
   {"SHADOW BOX",                            concept_checkerboard,              D, l_c4a, UC_none,        {s2x2, 1}},
   {"SHADOW DIAMOND",                        concept_checkerboard,              D, l_c4a, UC_none,        {sdmd, 1}},
   {"ANCHOR THE @6",                         concept_anchor,                  F+D, l_c4, UC_none},
   {"@6",                                    concept_so_and_so_only,          Y+D, l_mainstream, UC_none, {selective_key_plain, 0, 0}},
   {"@6 (while the others)",                 concept_some_vs_others,            0, l_mainstream, UC_none, {selective_key_plain, 1, 0}},
   {"@6 DISCONNECTED",                       concept_so_and_so_only,            D, l_c2, UC_none,         {selective_key_disc_dist, 0, 0}},
   {"@6 DISCONNECTED (while the others)",    concept_some_vs_others,            0, l_c2, UC_none,         {selective_key_disc_dist, 1, 0}},
   {"@6 DO YOUR PART",                       concept_so_and_so_only,          F+D, l_mainstream, UC_none, {selective_key_dyp, 0, 0}},
   {"@6 DO YOUR PART (while the others)",    concept_some_vs_others,            F, l_mainstream, UC_none, {selective_key_dyp, 1, 0}},
   {"ON YOUR OWN",                           concept_on_your_own,               0, l_c4a, UC_none},
   {"OWN THE @6",                            concept_some_vs_others,            F, l_c3a, UC_none,        {selective_key_own, 1, 0}},
   {"IGNORE THE @6",                         concept_so_and_so_only,          F+D, l_c1, UC_none,         {selective_key_ignore, 0, 0}},
   {"@6 WORK",                               concept_so_and_so_only,          L+D, l_c1, UC_none,         {selective_key_work_concept, 1, 0}},
   {"@6 LEAD FOR A",                         concept_so_and_so_only,            D, l_mainstream, UC_none, {selective_key_lead_for_a, 0, 0}},
   {"two calls in succession",               concept_sequential,                0, l_mainstream, UC_none},
   {"FOLLOW IT BY",                          concept_special_sequential,        0, l_c2, UC_none,         {0}},
   {"PRECEDE IT BY",                         concept_special_sequential,        0, l_c2, UC_none,         {1}},
   {"ADD",                                   concept_special_sequential,        0, l_c2, UC_none,         {0}},
   {"USE",                                   concept_special_sequential,        0, l_c2, UC_none,         {4}},
   {"CRAZY",                                 concept_crazy,                     D, l_c2, UC_none,         {0, FALSE}},
   {"REVERSE CRAZY",                         concept_crazy,                     D, l_c2, UC_none,         {1, FALSE}},
   {"@a CRAZY",                              concept_frac_crazy,                D, l_c2, UC_none,         {0, TRUE}},
   {"@a REVERSE CRAZY",                      concept_frac_crazy,                D, l_c2, UC_none,         {1, TRUE}},
   {"RANDOM",                                concept_meta,                    G+D, l_c3, UC_none,         {meta_key_random}},
   {"REVERSE RANDOM",                        concept_meta,                    G+D, l_c3x, UC_none,        {meta_key_rev_random}},
   {"ODDLY",                                 concept_meta,                    G+D, l_c4, UC_none,         {meta_key_random}},
   {"EVENLY",                                concept_meta,                    G+D, l_c4, UC_none,         {meta_key_rev_random}},
   {"PIECEWISE",                             concept_meta,                    G+D, l_c3x, UC_none,        {meta_key_piecewise}},
   {"@9/@9",                                 concept_fractional,              F+D, l_mainstream, UC_none, {0}},
   {"1-@9/@9",                               concept_fractional,              F+D, l_mainstream, UC_none, {2}},
   {"TWICE",              concept_n_times_const, F+D, l_mainstream, UC_none, {0, 2}},
   {"THRICE",             concept_n_times_const, F+D, l_mainstream, UC_none, {0, 3}},
   {"@9 TIMES",           concept_n_times,    F+D, l_mainstream, UC_none, {1}},
   {"DO THE LAST @9/@9",  concept_fractional, F+D, l_mainstream, UC_none, {1}},
   {"REVERSE ORDER",      concept_meta,       D, l_mainstream, UC_none, {meta_key_revorder}},
   {"INTERLACE",          concept_interlace,  0, l_c3x, UC_none},
   {"INITIALLY",          concept_meta,     G+D, l_c3a, UC_none,     {meta_key_initially}},
   {"FINALLY",            concept_meta,     G+D, l_c3a, UC_none,     {meta_key_finally}},
   {"INITIALLY AND FINALLY", concept_meta,  G+D, l_c3a, UC_none,     {meta_key_initially_and_finally}},
   {"FINISH",             concept_meta,       D, l_c1, UC_none,      {meta_key_finish}},
   {"LIKE A",             concept_meta,       D, l_c1, UC_none,      {meta_key_like_a}},
   {"LIKE AN",            concept_meta,       D, l_c1, UC_none,      {meta_key_like_a}},
   {"ECHO",               concept_meta,     G+D, l_c4, UC_none,      {meta_key_echo}},
   {"REVERSE ECHO",       concept_meta,     G+D, l_c4, UC_none,      {meta_key_rev_echo}},
   {"SHIFTY",             concept_meta,       D, l_c4, UC_none,      {meta_key_shift_n, 1}},
   {"SHIFT @9",           concept_meta_one_arg,F+D,l_c4, UC_none,    {meta_key_shift_n}},
   {"SHIFT 1/2",          concept_meta,     F+D, l_c4, UC_none,      {meta_key_shift_half, 0}},
   {"SHIFT @9-1/2",       concept_meta_one_arg,F+D,l_c4, UC_none,    {meta_key_shift_half}},
   {"@6 START",           concept_so_and_so_begin,F+D,l_c1, UC_none, {0}},
   {"SKIP THE @u PART",   concept_meta_one_arg,F+D,l_c1, UC_none,    {meta_key_skip_nth_part}},
   {"DO THE @u PART",     concept_meta_one_arg,L+D,l_c1, UC_none,    {meta_key_nth_part_work}},
   {"SECONDLY",           concept_meta,        L+D,l_c4, UC_none,    {meta_key_nth_part_work, 2}},
   {"THIRDLY",            concept_meta,        L+D,l_c4, UC_none,    {meta_key_nth_part_work, 3}},
   {"FIRST @9/@9",        concept_meta_two_args,L+D,l_c1, UC_none,   {meta_key_first_frac_work, 0}},
   {"MIDDLE @9/@9",       concept_meta_two_args,L+D,l_c1, UC_none,   {meta_key_first_frac_work, 2}},
   {"LAST @9/@9",         concept_meta_two_args,L+D,l_c1, UC_none,   {meta_key_first_frac_work, 1}},
   {"DO THE LAST PART",   concept_meta,     G+D, l_c3a, UC_none,     {meta_key_finally}},
   {"REPLACE THE @u PART",                   concept_replace_nth_part,          0, l_c1, UC_none,         {8}},
   {"USE FOR THE @u PART",                   concept_special_sequential_num,    0, l_c1, UC_none,         {3}},
   {"REPLACE THE LAST PART",                 concept_replace_last_part,         0, l_c1, UC_none,         {0}},
   {"START WITH",                            concept_special_sequential,        0, l_c2, UC_none,         {2}},
   {"INTERRUPT AFTER THE @u PART",           concept_replace_nth_part,          0, l_c1, UC_none,         {9}},
   {"INTERRUPT BEFORE THE LAST PART",        concept_replace_last_part,         0, l_c1, UC_none,         {1}},
   {"INTERRUPT AFTER @9/@9",                 concept_interrupt_at_fraction,     0, l_c1, UC_none,         {2}},
   {"SANDWICH",                              concept_sandwich,                  0, l_c3, UC_none,         {3}},
   {"OMIT",                                  concept_omit,                      D, l_c3, UC_none},
   {"IN A 1/4 TAG",                          concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_REAL_1_4_TAG,  CONCPROP__NEEDK_4DMD}},
   {"IN A 3/4 TAG",                          concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_REAL_3_4_TAG,  CONCPROP__NEEDK_4DMD}},
   {"IN A 1/4 LINE",                         concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_REAL_1_4_LINE, CONCPROP__NEEDK_4DMD}},
   {"IN A 3/4 LINE",                         concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_REAL_3_4_LINE, CONCPROP__NEEDK_4DMD}},
   {"IN POINT-TO-POINT DIAMONDS",            concept_tandem_in_setup,           D, l_c3, UC_none,         {0, CONCPROP__NEEDK_DBLX}},
   {"IN A TALL 6",                           concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_TALL6, CONCPROP__NEEDK_DEEPXWV}},
   {"IN A TIDAL LINE",                       concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_LINES, CONCPROP__NEEDK_1X16}},
   {"IN A TIDAL COLUMN",                     concept_tandem_in_setup,           D, l_c3, UC_none,         {CMD_MISC__VERIFY_COLS, CONCPROP__NEEDK_2X8}},
   {"INSIDE TRIANGLES",                      concept_randomtrngl,               D, l_c1, UC_none,         {2}},
   {"INSIDE INTERLOCKED TRIANGLES",          concept_randomtrngl,               D, l_c2, UC_none,         {0102}},
   {"OUTSIDE TRIANGLES",                     concept_randomtrngl,               D, l_c1, UC_none,         {3}},
   {"IN POINT TRIANGLES",                    concept_randomtrngl,               D, l_c1, UC_none,         {5}},
   {"OUT POINT TRIANGLES",                   concept_randomtrngl,               D, l_c1, UC_none,         {4}},
   {"TALL 6",                                concept_randomtrngl,               D, l_c3x, UC_none,        {0}},
   {"SHORT 6",                               concept_randomtrngl,               D, l_c3x, UC_none,        {1}},
   {"WAVE-BASED TRIANGLES",                  concept_randomtrngl,               D, l_c1, UC_none,         {6}},
   {"TANDEM-BASED TRIANGLES",                concept_randomtrngl,               D, l_c1, UC_none,         {7}},
   {"@k-BASED TRIANGLES",                    concept_selbasedtrngl,             D, l_c1, UC_none,         {20}},
   {"MINI-BUTTERFLY",                        concept_mini_but_o,                D, l_c3, UC_none, {0}},
   {"MINI-O",                                concept_mini_but_o,                D, l_c3, UC_none, {1}},
   {"CONCENTRIC",         concept_concentric, D, l_c1, UC_none,      {schema_concentric}},
   {"CROSS CONCENTRIC",   concept_concentric, D, l_c2, UC_none,      {schema_cross_concentric}},
   {"SINGLE CONCENTRIC",  concept_concentric, D, l_c4, UC_none,      {schema_single_concentric}},
   {"SINGLE CROSS CONCENTRIC",               concept_concentric,                D, l_c4, UC_none,         {schema_single_cross_concentric}},
   {"GRAND SINGLE CONCENTRIC",               concept_concentric,                D, l_c4, UC_none,         {schema_grand_single_concentric}},
   {"GRAND SINGLE CROSS CONCENTRIC",         concept_concentric,                D, l_c4, UC_none,         {schema_grand_single_cross_concentric}},
   {"CONCENTRIC TRIPLE BOXES",               concept_triple_boxes,              D, l_c4a, UC_none, {MPKIND__CONCPHAN}},
   {"CONCENTRIC QUADRUPLE BOXES",            concept_quad_boxes,                D, l_c4a, UC_none, {MPKIND__CONCPHAN}},
   {"CONCENTRIC DIAMONDS",                   concept_concentric,                D, l_c1, UC_none,         {schema_concentric_diamonds}},
   {"CROSS CONCENTRIC DIAMONDS",             concept_concentric,                D, l_c2, UC_none,         {schema_cross_concentric_diamonds}},
   {"CONCENTRIC Z's",                        concept_concentric,                D, l_c3a, UC_none,        {schema_concentric_zs}},
   {"CROSS CONCENTRIC Z's",                  concept_concentric,                D, l_c3a, UC_none,        {schema_cross_concentric_zs}},
   {"GRAND WORKING FORWARD",                 concept_grand_working,             D, l_c4a, UC_none,        {0}},
   {"GRAND WORKING BACKWARD",                concept_grand_working,             D, l_c4a, UC_none,        {2}},
   {"GRAND WORKING LEFT",                    concept_grand_working,             D, l_c4a, UC_none,        {1}},
   {"GRAND WORKING RIGHT",                   concept_grand_working,             D, l_c4a, UC_none,        {3}},
   {"GRAND WORKING AS CENTERS",              concept_grand_working,             D, l_c4a, UC_none,        {10}},
   {"GRAND WORKING AS ENDS",                 concept_grand_working,             D, l_c4a, UC_none,        {11}},
   {"GRAND WORKING CLOCKWISE",               concept_grand_working,             D, l_c4a, UC_none,        {8}},
   {"GRAND WORKING COUNTERCLOCKWISE",        concept_grand_working,             D, l_c4a, UC_none,        {9}},
   {"@6 ARE CENTERS OF A DOUBLE-OFFSET 1/4 TAG",concept_double_offset,          D, l_c4, UC_none,         {0}},
   {"@6 ARE CENTERS OF A DOUBLE-OFFSET 3/4 TAG",concept_double_offset,          D, l_c4, UC_none,         {1}},
   {"@6 ARE CENTERS OF DOUBLE-OFFSET DIAMONDS",concept_double_offset,           D, l_c4, UC_none,         {3}},
   {"@6 ARE CENTERS OF DOUBLE-OFFSET DIAMOND SPOTS",concept_double_offset,      D, l_c4, UC_none,         {2}},
   {"INRIGGER",                   concept_rigger, D, l_c3x, UC_none, {2}},
   {"OUTRIGGER",                  concept_rigger, D, l_c3x, UC_none, {0}},
   {"LEFTRIGGER",                 concept_rigger, D, l_c3x, UC_none, {1}},
   {"RIGHTRIGGER",                concept_rigger, D, l_c3x, UC_none, {3}},
   {"BACKRIGGER",                 concept_rigger, D, l_c3x, UC_none, {16}},
   {"FRONTRIGGER",                concept_rigger, D, l_c3x, UC_none, {18}},
   {"RIGHT WING",                 concept_wing,   D, l_c3x, UC_none, {0}},
   {"LEFT WING",                  concept_wing,   D, l_c3x, UC_none, {1}},
   {"COMMON POINT GALAXY",   concept_common_spot, D, l_c4, UC_none,  {   1, 0}},
   {"COMMON SPOT COLUMNS",   concept_common_spot, D, l_c4, UC_none,  {   2, 0}},
   {"COMMON POINT DIAMONDS", concept_common_spot, D, l_c4, UC_none,  {   4, 0}},
   {"COMMON SPOT DIAMONDS",  concept_common_spot, D, l_c4, UC_none,  {   4, 0}},
   {"COMMON SPOT POINT-TO-POINT DIAMONDS",  concept_common_spot, D, l_c4, UC_none,  {0x400, 0}},
   {"COMMON POINT HOURGLASS",concept_common_spot, D, l_c4, UC_none,  {0x80, 0}},
   {"COMMON SPOT HOURGLASS", concept_common_spot, D, l_c4, UC_none,  {0x80, 0}},
   {"COMMON END LINES",      concept_common_spot, D, l_c4, UC_none,  {0x10, 0}},
   {"COMMON END WAVES",      concept_common_spot, D, l_c4, UC_none,  {0x10, CMD_MISC__VERIFY_WAVES}},
   {"COMMON CENTER LINES",   concept_common_spot, D, l_c4, UC_none,  {0x20, 0}},
   {"COMMON CENTER WAVES",   concept_common_spot, D, l_c4, UC_none,  {0x20, CMD_MISC__VERIFY_WAVES}},
   {"COMMON SPOT LINES",     concept_common_spot, D, l_c4, UC_none,  {0x78, 0}},
   {"COMMON SPOT WAVES",     concept_common_spot, D, l_c4, UC_none,  {0x70, CMD_MISC__VERIFY_WAVES}},
   {"COMMON SPOT TWO-FACED LINES",concept_common_spot,D,l_c4, UC_none,{8,   CMD_MISC__VERIFY_2FL}},
   {"COMMON SPOT 1/4 TAGS",  concept_common_spot, D, l_c4, UC_none,  {0x200, 0}},
   {"COMMON SPOT 1/4 LINES", concept_common_spot, D, l_c4, UC_none,  {0x100, 0}},
   {"1X2",                                   concept_1x2,                     L+D, l_c3, UC_none},
   {"2X1",                                   concept_2x1,                     L+D, l_c3, UC_none},
   {"2X2",                                   concept_2x2,                     L+D, l_c3a, UC_none},
   {"1X3",                                   concept_1x3,                     L+D, l_c2, UC_none},
   {"3X1",                                   concept_3x1,                     L+D, l_c2, UC_none},
   {"3X3",                                   concept_3x3,                     L+D, l_c3x, UC_3x3},
   {"4X4",                                   concept_4x4,                     L+D, l_c3x, UC_4x4},
   {"5X5",                                   concept_5x5,                     L+D, l_c3x, UC_none},
   {"6X6",                                   concept_6x6,                     L+D, l_c3x, UC_none},
   {"7X7",                                   concept_7x7,                     L+D, l_c3x, UC_none},
   {"8X8",                                   concept_8x8,                     L+D, l_c3x, UC_none},
   {"ALL 4 COUPLES",                         concept_all_8,                     D, l_a2, UC_none,         {0}},
   {"ALL 8",                                 concept_all_8,                     D, l_a2, UC_none,         {1}},
   {"ALL 8 (diamonds)",                      concept_all_8,                     D, l_a2, UC_none,         {2}},
   {"REVERT",                                concept_revert,                  L+D, l_c3x, UC_none,        {INHERITFLAGRVRTK_REVERT}},
   {"REFLECTED",                             concept_revert,                  L+D, l_c3, UC_none,         {INHERITFLAGRVRTK_REFLECT}},
   {"REVERT AND THEN REFLECT",               concept_revert,                  L+D, l_c3x, UC_none,        {INHERITFLAGRVRTK_RVF}},
   {"REFLECT AND THEN REVERT",               concept_revert,                  L+D, l_c3x, UC_none,        {INHERITFLAGRVRTK_RFV}},
   {"REVERT, THEN REFLECT, THEN REVERT",     concept_revert,                  L+D, l_c3x, UC_none,        {INHERITFLAGRVRTK_RVFV}},
   {"REFLECT, THEN REVERT, THEN REFLECT",    concept_revert,                  L+D, l_c3x, UC_none,        {INHERITFLAGRVRTK_RFVF}},
   {"FAST",                                  concept_fast,                    L+D, l_c4, UC_none},
   {"CENTERS",                               concept_centers_or_ends,           D, l_mainstream, UC_none, {selector_centers, FALSE}},
   {"ENDS",                                  concept_centers_or_ends,           D, l_mainstream, UC_none, {selector_ends, FALSE}},
   {"EACH 1X4",                              concept_each_1x4,                  D, l_mainstream, UC_none, {0, 0}},
   {"EACH LINE",                             concept_each_1x4,                  D, l_mainstream, UC_none, {1, 0}},
   {"EACH COLUMN",                           concept_each_1x4,                  D, l_mainstream, UC_none, {2, 0}},
   {"EACH WAVE",                             concept_each_1x4,                  D, l_mainstream, UC_none, {3, 0}},
   {"EACH BOX",                              concept_each_1x4,                  D, l_mainstream, UC_none, {0, 2}},
   {"EACH DIAMOND",                          concept_each_1x4,                  D, l_mainstream, UC_none, {0, 1}},
   {"1x10 MATRIX",                           concept_create_matrix,             D, l_c3x, UC_none,        {s1x10, CONCPROP__NEEDK_1X10}},
   {"1X12 MATRIX",                           concept_create_matrix,             D, l_c3x, UC_none,        {s1x12, CONCPROP__NEEDK_1X12}},
   {"1X16 MATRIX",                           concept_create_matrix,             D, l_c3x, UC_none,        {s1x16, CONCPROP__NEEDK_1X16}},
   {"2X5 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s2x5,  0}},  /* If it isn't already in a 2x5, we lose. */
   {"2X6 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s2x6,  CONCPROP__NEEDK_2X6}},
   {"2X8 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_2x8matrix,        {s2x8,  CONCPROP__NEEDK_2X8}},
   {"2X12 MATRIX",                           concept_create_matrix,             D, l_c3x, UC_none,        {s2x12,  CONCPROP__NEEDK_2X12}},
   {"3X4 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s3x4,  CONCPROP__NEEDK_3X4}},
   {"4X4 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s4x4,  CONCPROP__NEEDK_4X4}},
   {"4X5 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s4x5,  CONCPROP__NEEDK_4X5}},
   {"3X6 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s3x6,  CONCPROP__NEEDK_3X6}},
   {"3X8 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s3x8,  CONCPROP__NEEDK_3X8}},
   {"4X6 MATRIX",                            concept_create_matrix,             D, l_c3x, UC_none,        {s4x6,  CONCPROP__NEEDK_4X6}},
   {"16 MATRIX OF PARALLEL DIAMONDS",        concept_create_matrix,             D, l_c3x, UC_none,        {s4dmd, CONCPROP__NEEDK_4D_4PTPD}},
   {"OFFSET SPLIT PHANTOM BOXES",            concept_distorted,                 D, l_c4, UC_none,         {disttest_offset, 128}},
   {"DRAG THE @6",                           concept_drag,                    F+D, l_c4, UC_none},
   {"DIAGNOSE",                              concept_diagnose,                  D, l_mainstream, UC_none},  /* Only appears if "-diagnostic" given. */
   {"???",                                   marker_end_of_list}};


const concept_fixer_thing concept_fixer_table[] = {
   {0, FINAL__SPLIT,      UC_pl, UC_spl},    // SPLIT + PHANTOM LINES
   {INHERITFLAG_INTLK, 0, UC_pl, UC_ipl},    // INTERLOCKED + PHANTOM LINES
   {0, FINAL__SPLIT,      UC_pw, UC_spw},    // SPLIT + PHANTOM WAVES
   {INHERITFLAG_INTLK, 0, UC_pw, UC_ipw},    // INTERLOCKED + PHANTOM WAVES
   {0, FINAL__SPLIT,      UC_pc, UC_spc},    // SPLIT + PHANTOM COLUMNS
   {INHERITFLAG_INTLK, 0, UC_pc, UC_ipc},    // INTERLOCKED + PHANTOM COLUMNS
   {0, FINAL__SPLIT,      UC_pb, UC_spb},    // SPLIT + PHANTOM BOXES
   {INHERITFLAG_INTLK, 0, UC_pb, UC_ipb},    // INTERLOCKED + PHANTOM BOXES
   {0, FINAL__SPLIT,      UC_pd, UC_spd},    // SPLIT + PHANTOM DIAMONDS
   {INHERITFLAG_INTLK, 0, UC_pd, UC_ipd},    // INTERLOCKED + PHANTOM DIAMONDS
   {0, FINAL__SPLIT,      UC_pds, UC_spds},  // SPLIT + PHANTOM DIAMOND SPOTS
   {INHERITFLAG_INTLK, 0, UC_pds, UC_ipds},  // INTERLOCKED + PHANTOM DIAMOND SPOTS
   {0, FINAL__SPLIT,      UC_p1, UC_sp1},    // SPLIT + PHANTOM 1/4 TAGS
   {INHERITFLAG_INTLK, 0, UC_p1, UC_ip1},    // INTERLOCKED + PHANTOM 1/4 TAGS
   {0, FINAL__SPLIT,      UC_p3, UC_sp3},    // SPLIT + PHANTOM 3/4 TAGS
   {INHERITFLAG_INTLK, 0, UC_p3, UC_ip3},    // INTERLOCKED + PHANTOM 3/4 TAGS
   {0, FINAL__SPLIT,      UC_pgt, UC_spgt},  // SPLIT + PHANTOM GENERAL 1/4 TAGS
   {INHERITFLAG_INTLK, 0, UC_pgt, UC_ipgt},  // INTERLOCKED + PHANTOM GENERAL 1/4 TAGS
   {0, 0, UC_none, UC_none}};

static const useful_concept_enum nice_setup_concept_4x4[] = {
   UC_spl,     // split phantom lines
   UC_ipl,     // interlocked phantom lines
   UC_qlwf,    // quadruple lines working forward
   UC_qlwb,    // quadruple lines working backward
   UC_spc,     // split phantom columns
   UC_ipc,     // interlocked phantom columns
   UC_qcwr,    // quadruple columns working right
   UC_qcwl,    // quadruple columns working left
   UC_pofl,    // phantom offset lines
   UC_pob,     // phantom "O" or butterfly
   UC_pofc,    // phantom offset columns
   UC_pibl,    // 4 phantom interlocked blocks

   /* We suspect that the following concepts are less useful than those above,
      because, unless people are situated in certain ways, they won't lead to
      *any* legal calls, and we're too lazy to look at the setup in more
      detail to decide whether they can be used.  Actually, the task of making
      searches restrict their attention to plausible things is very difficult.
      We accept the fact that most of the things we try are pointless. */

   UC_cpl,     // phantom as couples (of course, we will need to put in "phantom")
   UC_tnd,     // phantom tandem
   UC_4x4,     // 4x4
   UC_none};

static const useful_concept_enum nice_setup_concept_3x4[] = {
   UC_tl,      // triple lines
   UC_tc,      // triple columns
   UC_tlwf,    // triple lines working forward
   UC_tlwb,    // triple lines working backward
   UC_tcwr,    // triple columns working right
   UC_tcwl,    // triple columns working left
   UC_3x3,     // 3x3
   UC_none};

static const useful_concept_enum nice_setup_concept_2x8[] = {
   UC_spb,     // split phantom boxes
   UC_ipb,     // interlocked phantom boxes
   UC_qbwf,    // quadruple boxes working forward
   UC_qbwb,    // quadruple boxes working backward
   UC_qbwr,    // quadruple boxes working right
   UC_qbwl,    // quadruple boxes working left
   UC_qbwt,    // quadruple boxes working together
   UC_qbwa,    // quadruple boxes working apart
   UC_pl8,     // twin phantom tidal lines
   UC_pc8,     // twin phantom tidal columns
   // The less likely ones:
   UC_4x4,     // 4x4
   UC_cpl,     // (2x8 matrix) as couples
   UC_tnd,     // (2x8 matrix) tandem
   UC_cpl2s,   // (2x8 matrix) couples twosome
   UC_tnd2s,   // (2x8 matrix) tandem twosome
   UC_none};

static const useful_concept_enum nice_setup_concept_2x6[] = {
   UC_tb,      // triple boxes
   UC_tbwf,    // triple boxes working forward
   UC_tbwb,    // triple boxes working backward
   UC_tbwr,    // triple boxes working right
   UC_tbwl,    // triple boxes working left
   UC_tbwt,    // triple boxes working together
   UC_tbwa,    // triple boxes working apart
   UC_pl6,     // twin phantom lines of 6
   UC_pc6,     // twin phantom columns of 6
   UC_3x3,     // 3x3
   // Let's give these extra probability.
   UC_tb,      // triple boxes
   UC_tb,      // triple boxes
   UC_tbwt,    // triple boxes working together
   UC_tbwt,    // triple boxes working together
   // These are actually 2x8 concepts, but they could still be helpful.
   UC_spb,     // split phantom boxes
   UC_qbwt,    // quadruple boxes working together
   UC_pl8,     // twin phantom tidal lines
   UC_pc8,     // phantom tidal columns
   UC_none};

static const useful_concept_enum nice_setup_concept_1x12[] = {
   UC_tlwt,    // end-to-end triple lines working together
   UC_tlwa,    // end-to-end triple lines working apart
   UC_tcwt,    // end-to-end triple columns working together
   UC_tcwa,    // end-to-end triple columns working apart
   UC_none};

static const useful_concept_enum nice_setup_concept_1x16[] = {
   UC_spl,      // end-to-end split phantom lines
   UC_ipl,      // end-to-end interlocked phantom lines
   UC_qlwt,     // end-to-end quadruple lines working together
   UC_qlwa,     // end-to-end quadruple lines working apart
   UC_spc,      // end-to-end split phantom columns
   UC_ipc,      // end-to-end interlocked phantom columns
   UC_qcwt,     // end-to-end quadruple columns working together
   UC_qcwa,     // end-to-end quadruple columns working apart
   UC_none};

static const useful_concept_enum nice_setup_concept_3dmd[] = {
   UC_td,      // triple diamonds
   UC_tdwt,    // triple diamonds working together
   UC_none};

static const useful_concept_enum nice_setup_concept_4dmd[] = {
   UC_spd,     // split phantom diamonds
   UC_ipd,     // interlocked phantom diamonds
   UC_qd,      // quadruple diamonds
   UC_qdwt,    // quadruple diamonds working together
   UC_none};

static const useful_concept_enum nice_setup_concept_4x6[] = {
   UC_trtl,     // triple twin lines
   UC_trtc,     // triple twin columns
   UC_none};

static nice_setup_thing nice_setup_thing_4x4 = {
   nice_setup_concept_4x4,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_4x4)};

static nice_setup_thing nice_setup_thing_3x4 = {
   nice_setup_concept_3x4,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_3x4)};

static nice_setup_thing nice_setup_thing_2x8 = {
   nice_setup_concept_2x8,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_2x8)};

static nice_setup_thing nice_setup_thing_2x6 = {
   nice_setup_concept_2x6,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_2x6)};

static nice_setup_thing nice_setup_thing_1x12 = {
   nice_setup_concept_1x12,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_1x12)};

static nice_setup_thing nice_setup_thing_1x16 = {
   nice_setup_concept_1x16,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_1x16)};

static nice_setup_thing nice_setup_thing_3dmd = {
   nice_setup_concept_3dmd,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_3dmd)};

static nice_setup_thing nice_setup_thing_4dmd = {
   nice_setup_concept_4dmd,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_4dmd)};

static nice_setup_thing nice_setup_thing_4x6 = {
   nice_setup_concept_4x6,
   (useful_concept_enum *) 0,     /* Will be filled in during initialization. */
   sizeof(nice_setup_concept_4x6)};


/* This array tracks the enumeration "nice_start_kind". */
nice_setup_info_item nice_setup_info[] = {
   {s4x4,   &nice_setup_thing_4x4,  (useful_concept_enum *) 0, 0},
   {s3x4,   &nice_setup_thing_3x4,  (useful_concept_enum *) 0, 0},
   {s2x8,   &nice_setup_thing_2x8,  (useful_concept_enum *) 0, 0},
   {s2x6,   &nice_setup_thing_2x6,  (useful_concept_enum *) 0, 0},
   {s1x10,  &nice_setup_thing_1x12, (useful_concept_enum *) 0, 0},  /* Note overuse. */
   {s1x12,  &nice_setup_thing_1x12, (useful_concept_enum *) 0, 0},
   {s1x14,  &nice_setup_thing_1x16, (useful_concept_enum *) 0, 0},  /* Note overuse. */
   {s1x16,  &nice_setup_thing_1x16, (useful_concept_enum *) 0, 0},
   {s3dmd,  &nice_setup_thing_3dmd, (useful_concept_enum *) 0, 0},
   {s4dmd,  &nice_setup_thing_4dmd, (useful_concept_enum *) 0, 0},
   {s4x6,   &nice_setup_thing_4x6,  (useful_concept_enum *) 0, 0}
};
