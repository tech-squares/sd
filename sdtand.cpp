/* SD -- square dance caller's helper.

    Copyright (C) 1990-1999  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 32. */

/* This file contains stuff for tandem and as-couples moves. */

/* This defines the following functions:
   tandem_couples_move
   initialize_tandem_tables
*/

#ifdef WIN32
#define SDLIB_API __declspec(dllexport)
#else
#define SDLIB_API
#endif

#include "sd.h"


typedef struct {
   personrec real_saved_people[8][MAX_PEOPLE];
   int saved_originals[MAX_PEOPLE];
   setup virtual_setup;
   setup virtual_result;
   int vertical_people[MAX_PEOPLE];    /* 1 if original people were near/far; 0 if lateral */
   uint32 single_mask;
   long_boolean no_unit_symmetry;
   long_boolean phantom_pairing_ok;
   int np;
} tandrec;


typedef struct {
   int maps[24];
   uint32 ilatmask;           /* lateral pairs in inside numbering --
                                 only "1" bits used! (except triangles) */
   uint32 olatmask;
   int limit;
   int rot;
   uint32 insinglemask;       /* relative to insetup numbering, those people that are
                                 NOT paired -- only alternate bits used! */
   uint32 outsinglemask;      /* relative to outsetup numbering, those people that are
                                 NOT paired */
   uint32 outunusedmask;
   setup_kind insetup;
   setup_kind outsetup;
} tm_thing;


Private tm_thing maps_isearch_twosome[] = {

/*         map1                              map2                map3  map4   ilatmask olatmask    limit rot            insetup outsetup */


   {{7, 6, 4, 5,                     0, 1, 3, 2},                                0,     0000,         4, 0,  0,  0, 0,  s1x4,  s2x4},            /* "2x4_4" - see below */
   {{0, 2, 5, 7,                     1, 3, 4, 6},                             0x55,     0xFF,         4, 0,  0,  0, 0,  s2x2,  s2x4},
   {{2, 5, 7, 0,                     3, 4, 6, 1},                                0,     0xFF,         4, 1,  0,  0, 0,  s2x2,  s2x4},
   {{3, 2,                           0, 1},                                      0,     0000,         2, 0,  0,  0, 0,  s1x2,  s2x2},
   {{0, 3,                           1, 2},                                      0,      0xF,         2, 1,  0,  0, 0,  s1x2,  s2x2},
   {{0, 3,                           1, 2},                                    0x5,      0xF,         2, 0,  0,  0, 0,  s1x2,  s1x4},
   {{0,                              1},                                       0x1,     0003,         1, 0,  0,  0, 0,  s1x1,  s1x2},
   {{0,                              1},                                         0,     0003,         1, 1,  0,  0, 0,  s1x1,  s1x2},
   {{0, 3, 5, 6,                     1, 2, 4, 7},                             0x55,     0xFF,         4, 0,  0,  0, 0,  s1x4,  s1x8},
   {{0, 2, 4, 7, 9, 11,              1, 3, 5, 6, 8, 10},                     0x555,    0xFFF,         6, 0,  0,  0, 0,  s1x6,  s1x12},
   {{0, 2, 6, 4, 9, 11, 15, 13,      1, 3, 7, 5, 8, 10, 14, 12},            0x5555,   0xFFFF,         8, 0,  0,  0, 0,  s1x8,  s1x16},
   {{15, 14, 12, 13, 8, 9, 11, 10,   0, 1, 3, 2, 7, 6, 4, 5},                    0,     0000,         8, 0,  0,  0, 0,  s1x8,  s2x8},
   {{11, 10, 9, 6, 7, 8,             0, 1, 2, 5, 4, 3},                          0,     0000,         6, 0,  0,  0, 0,  s1x6,  s2x6},

   {{10, 15, 3, 1, 4, 5, 6, 8,       12, 13, 14, 0, 2, 7, 11, 9},                0,     0000,         8, 0,  0,  0, 0,  s2x4,  s4x4},
   {{14, 3, 7, 5, 8, 9, 10, 12,      0, 1, 2, 4, 6, 11, 15, 13},                 0,   0xFFFF,         8, 1,  0,  0, 0,  s2x4,  s4x4},

   /* When the map following this one gets fixed and uncommented, this one will have to appear first, of course.  Actually, it's
      trickier than that.  The whole issue of 3x4 vs. qtag operation needs to be straightened out. */
   {{7, 22, 15, 20, 18, 11, 2, 9,    6, 23, 14, 21, 19, 10, 3, 8},               0, 0xFCCFCC,         8, 1,  0,  0, 0,  s_qtag,s4x6},
   {{11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17, 
                      0, 1, 2, 3, 4, 5, 23, 22, 21, 20, 19, 18},                 0,     0000,        12, 0,  0,  0, 0,  s2x6,  s4x6},
   /* This is for everyone as couples in a 3x4, making virtual columns of 6. */
   {{2, 5, 7, 9, 10, 0,              3, 4, 6, 8, 11, 1},                         0,   0x0FFF,         6, 1,  0,  0, 0,  s2x3,  s3x4},
   /* This is for various people as couples in a 1/4 tag, making virtual columns of 6. */
   {{1, 3, 4, 5, 6, 0,               -1, 2, -1, -1, 7, -1},                      0,     0xCC,         6, 1,  0,  0, 0,  s2x3,  s_qtag},

   /* There is an issue involving the order of the two pairs of items that follow.  In the order shown, (3x4 matrix stuff before c1phan),
      the program will opt for a 3x4 if we say (normal columns; centers trail off) centers are as couples, circulate.  In the other
      order, it would opt for C1 phantoms.  We believe that having them arranged in 3 definite lines, from which, for example, we
      could have the very center 2 trade, is better. */

   /* Next two are for various people as couples in a 3x4 matrix, making virtual columns of 6. */
   {{2, 5, 7, 8, 11, 0,              -1, -1, 6, -1, -1, 1},                      0,   0x00C3,         6, 1,  0,  0, 0,  s2x3,  s3x4},
   {{2, 5, 7, 9, 11, 1,              3, -1, -1, 8, -1, -1},                      0,   0x030C,         6, 1,  0,  0, 0,  s2x3,  s3x4},

   /* Next two are for various people as couples in a C1 phantom, making virtual columns of 6. */
   {{3, 7, 5, 9, 15, 13,             1, -1, -1, 11, -1, -1},                     0,     0000,         6, 0,  0,  0, 0,  s2x3,  s_c1phan},
   {{0, 2, 6, 8, 10, 12,             -1, -1, 4, -1, -1, 14},                     0,     0000,         6, 0,  0,  0, 0,  s2x3,  s_c1phan},

   {{0, 2, 4, 6, 9, 11, 13, 15,      1, 3, 5, 7, 8, 10, 12, 14},            0x5555,   0xFFFF,         8, 0,  0,  0, 0,  s2x4,  s2x8},
   {{0, 2, 4, 6, 9, 11, 13, 15, 17, 19, 20, 22, 
                      1, 3, 5, 7, 8, 10, 12, 14, 16, 18, 21, 23},           0x555555, 0xFFFFFF,      12, 0,  0,  0, 0,  s3x4,  s3x8},
   {{2, 3, 5, 6, 7, 0,               -1, -1, 4, -1, -1, 1},                      0,     0x33,         6, 1,  0,  0, 0,  s_2x1dmd, s_crosswave},
   {{0, 1, 3, 4, 5, 6,               -1, -1, 2, -1, -1, 7},                      0,        0,         6, 0,  0,  0, 0,  s_1x2dmd, s_crosswave},
   {{6, 7, 0, 2, 3, 5,               -1, -1, 1, -1, -1, 4},                  0x410,     0x33,         6, 0,  0,  0, 0,  s_1x2dmd, s_rigger},
   {{0, 3, 2, 5, 7, 6,               1, -1, -1, 4, -1, -1},                      0,     0x33,         6, 1,  0,  0, 0,  s_2x1dmd, s_hrglass},
   {{0, 2, 3, 5, 6, 7,               1, -1, -1, 4, -1, -1},                  0x041,     0x33,         6, 0,  0,  0, 0,  s_2x1dmd, s3x1dmd},
   {{0, 1, 3, 4, 6, 7,               -1, 2, -1, -1, 5, -1},                  0x104,     0x66,         6, 0,  0,  0, 0,  s_2x1dmd, s3x1dmd},
   {{6, 7, 0, 2, 3, 5,               -1, -1, 1, -1, -1, 4},                  0x410,     0x33,         6, 0,  0,  0, 0,  s_2x1dmd, s_qtag},
   {{2, 4, 5, 0,                     -1, 3, -1, 1},                              0,      033,         4, 1,  0,  0, 0,  sdmd, s_2x1dmd},
   {{0, 2, 4, 5,                     1, -1, 3, -1},                           0x11,      033,         4, 0,  0,  0, 0,  sdmd, s_1x2dmd},
   /* Next one is for centers in tandem in lines, making a virtual bone6. */
   {{0, 3, 5, 4, 7, 6,               -1, -1, 2, -1, -1, 1},                      0,     0000,         6, 0,  0,  0, 0,  s_bone6, s2x4},
   /* Next two are for certain ends in tandem in an H, making a virtual bone6. */
   {{10, 3, 5, 6, 9, 11,             0, -1, -1, 4, -1, -1},                      0,     0000,         6, 0,  0,  0, 0,  s_bone6, s3x4},
   {{0, 4, 5, 6, 9, 11,              -1, 3, -1, -1, 10, -1},                     0,     0000,         6, 0,  0,  0, 0,  s_bone6, s3x4},
   /* Next one is for ends in tandem in lines, making a virtual short6. */
   {{2, 4, 5, 6, 7, 1,               -1, 3, -1, -1, 0, -1},                  0x104,     0000,         6, 1,  0,  0, 0,  s_short6, s2x4},
   /* Next two are for certain center column people in tandem in a 1/4 tag, making a virtual short6. */
   {{3, 2, 4, 5, 6, 0,               1, -1, -1, 7, -1, -1},                  0x041,     0000,         6, 1,  0,  0, 0,  s_short6, s_qtag},
   {{1, 2, 4, 5, 6, 7,               -1, -1, 3, -1, -1, 0},                  0x410,     0000,         6, 1,  0,  0, 0,  s_short6, s_qtag},
   /* Next two are for certain center column people in tandem in a spindle, making a virtual short6. */
   {{2, 3, 5, 6, 7, 0,               -1, -1, 4, -1, -1, 1},                      0,     0x33,         6, 1,  0,  0, 0,  s_short6, s_spindle},
   {{1, 3, 4, 6, 7, 0,               2, -1, -1, 5, -1, -1},                      0,     0x66,         6, 1,  0,  0, 0,  s_short6, s_spindle},
   /* Next three are for various people in tandem in columns of 8, making virtual columns of 6. */
   {{0, 2, 3, 5, 6, 7,               1, -1, -1, 4, -1, -1},                  0x041,     0063,         6, 0,  0,  0, 0,  s2x3,  s2x4},
   {{0, 1, 3, 4, 6, 7,               -1, 2, -1, -1, 5, -1},                  0x104,     0x66,         6, 0,  0,  0, 0,  s2x3,  s2x4},
   {{0, 1, 2, 4, 5, 7,               -1, -1, 3, -1, -1, 6},                  0x410,     0xCC,         6, 0,  0,  0, 0,  s2x3,  s2x4},
   /* Next three are for various people in tandem in a rigger/ptpd/bone, making a virtual line of 6. */
   {{6, 7, 5, 2, 3, 4,               -1, -1, 0, -1, -1, 1},                      0,     0000,         6, 0,  0,  0, 0,  s1x6,  s_rigger},
   {{0, 3, 2, 4, 5, 6,               -1, 1, -1, -1, 7, -1},                      0,     0000,         6, 0,  0,  0, 0,  s1x6,  s_ptpd},
   {{5, 6, 7, 4, 2, 3,               0, -1, -1, 1, -1, -1},                      0,     0000,         6, 0,  0,  0, 0,  s1x6,  s_bone},
   {{0, 1, 3, 4, 5, 6,               -1, -1, 2, -1, -1, 7},                  0x410,     0xCC,         6, 0,  0,  0, 0,  s_bone6,s_bone},
   {{0, 2, 5, 7, 9, 11, 12, 14,      1, 3, 4, 6, 8, 10, 13, 15},            0x5555,   0xFFFF,         8, 0,  0,  0, 0,  s_qtag,s4dmd},
   {{0, 3, 5, 6,                     1, 2, 4, 7},                                0,     0xFF,         4, 1,  0,  0, 0,  sdmd,  s_qtag},
   {{0, 7, 2, 4, 5, 6,               -1, 1, -1, -1, 3, -1},                      0,     0000,         6, 0,  0,  0, 0,  s_2x1dmd, s_galaxy},
   {{2, 1, 4, 6, 7, 0,               -1, 3, -1, -1, 5, -1},                      0,     0xAA,         6, 1,  0,  0, 0,  s_2x1dmd, s_galaxy},
   {{3, 7, 9, 13,                    1, 5, 11, 15},                           0x44,   0xA0A0,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{0, 6, 10, 12,                   2, 4, 8, 14},                            0x11,   0x0505,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},

   {{0, 7, 9, 12,                    2, 5, 11, 14},                           0x05,   0x00A5,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},     /* these 4 are unsymmetrical */
   {{3, 6, 10, 13,                   1, 4, 8, 15},                            0x50,   0xA500,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{3, 7, 10, 12,                   1, 5, 8, 14},                            0x14,   0x05A0,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{0, 6, 9, 13,                    2, 4, 11, 15},                           0x41,   0xA005,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},

   {{15, 3, 5, 9,                    13, 1, 7, 11},                           0x44,   0x0A0A,         4, 0,  0,  0, 0,  s2x2,  s4x4},         /* These do C1-phantom-like stuff from fudgy 4x4 */
   {{10, 3, 7, 6,                    15, 14, 2, 11},                          0x11,   0x8484,         4, 0,  0,  0, 0,  s2x2,  s4x4},         /* They must follow the pair just above. */

/* These two are new */
   {{11, 2, 7, 9,                    1, 3, 5, 8},                             0x44,    01414,         4, 0,  0,  0, 0,  s2x2,  s3x4},         /* These do C1-phantom-like stuff from fudgy 3x4 */
   {{0, 5, 7, 8,                     1, 2, 6, 11},                            0x11,    00303,         4, 0,  0,  0, 0,  s2x2,  s3x4},

   {{1, 3, 4, 5, 6, 0,               -1, 2, -1, -1, 7, -1},                      0,     0xCC,         6, 1,  0,  0, 0,  s_short6, s_rigger},
   {{6, 0, 3, 5,                     7, 1, 2, 4},                             0x55,     0xFF,         4, 0,  0,  0, 0,  sdmd,  s_rigger},
   {{6, 5, 3, 4,                     7, 0, 2, 1},                             0x11,     0xCC,         4, 0,  0,  0, 0,  s1x4,  s_rigger},     /* must be after "2x4_4" */
   {{5, 6, 4, 3,                     0, 7, 1, 2},                             0x44,     0xCC,         4, 0,  0,  0, 0,  s1x4,  s_bone},
   {{0, 3, 5, 6,                     1, 2, 4, 7},                             0x11,     0063,         4, 0,  0,  0, 0,  sdmd,  s_crosswave},  /* crosswave*/  
   {{7, 1, 4, 6,                     0, 2, 3, 5},                             0x44,     0146,         4, 0,  0,  0, 0,  sdmd,  s2x4},         /* must be after "2x4_4" */
   /* Next one is for so-and-so in tandem in a bone6, making a virtual line of 4. */
   {{4, 5, 3, 2,                     0, -1, 1, -1},                              0,     0000,         4, 0,  0,  0, 0,  s1x4,  s_bone6},
   /* Next one is for so-and-so in tandem in a short6, making a virtual line of 4. */
   {{1, 0, 4, 5,                     -1, 2, -1, 3},                              0,     0055,         4, 1,  0,  0, 0,  s1x4,  s_short6},
   {{1, 3, 4, 5,                     -1, 2, -1, 0},                           0x44,     0000,         4, 1,  0,  0, 0,  sdmd,  s_short6},
   {{5, 1, 3, 4,                     0, -1, 2, -1},                              0,     0000,         4, 0,  0,  0, 0,  sdmd,  s2x3},
   /* Next three are for so-and-so as couples in a line of 8, making a virtual line of 6. */
   {{0, 1, 3, 4, 5, 6,               -1, -1, 2, -1, -1, 7},                  0x410,     0xCC,         6, 0,  0,  0, 0,  s1x6,  s1x8},
   {{0, 1, 2, 4, 7, 6,               -1, 3, -1, -1, 5, -1},                  0x104,     0xAA,         6, 0,  0,  0, 0,  s1x6,  s1x8},
   {{0, 3, 2, 5, 7, 6,               1, -1, -1, 4, -1, -1},                  0x041,     0x33,         6, 0,  0,  0, 0,  s1x6,  s1x8},
   /* Next two are for so-and-so as couples in a line of 6, making a virtual line of 4. */
   {{0, 1, 3, 5,                     -1, 2, -1, 4},                           0x44,     0066,         4, 0,  0,  0, 0,  s1x4,  s1x6},
   {{0, 2, 4, 5,                     1, -1, 3, -1},                           0x11,     0033,         4, 0,  0,  0, 0,  s1x4,  s1x6},
   /* Next 4 are for so-and-so in tandem from a column of 6, making a virtual column of 4.  The first two are the real maps,
      and the other two take care of the reorientation that sometimes happens when coming out of a 2x2. */
   {{0, 1, 3, 5,                     -1, 2, -1, 4},                           0x44,     0066,         4, 0,  0,  0, 0,  s2x2,  s2x3},
   {{0, 2, 4, 5,                     1, -1, 3, -1},                           0x11,     0033,         4, 0,  0,  0, 0,  s2x2,  s2x3},
   {{1, 3, 5, 0,                     2, -1, 4, -1},                              0,     0066,         4, 1,  0,  0, 0,  s2x2,  s2x3},
   {{2, 4, 5, 0,                     -1, 3, -1, 1},                              0,     0033,         4, 1,  0,  0, 0,  s2x2,  s2x3},
   {{0, 2, 4, 7, 9, 11,              1, 3, 5, 6, 8, 10},                     0x555,   0x0FFF,         6, 0,  0,  0, 0,  s2x3,  s2x6},
   /* The two maps just below must be after the map just above. */
   {{-2, 7, 6, -2, 12, 15,           -2, 2, 5, -2, 17, 16},                  02020,  0x18060,         6, 0,  0,  0, 0,  s2x3,  s4x5},
   {{9, 7, -2, 18, 12, -2,           8, 2, -2, 19, 17, -2},                  00101,  0xC0300,         6, 0,  0,  0, 0,  s2x3,  s4x5},

   {{2, 0,                           3, 1},                                    0x4,      0xC,         2, 1,  0,  0, 0,  s1x2,  s_trngl4},
   {{1, 3,                           0, 2},                                    0x1,      0xC,         2, 3,  0,  0, 0,  s1x2,  s_trngl4},
   {{2, 1, 0,                        3, -1, -1},                                 0,      0xC,         3, 1,  0,  0, 0,  s1x3,  s_trngl4},
   {{0, 1, 3,                        -1, -1, 2},                                 0,      0xC,         3, 3,  0,  0, 0,  s1x3,  s_trngl4},
   {{0, 3, 2,                        -1, 1, -1},                                 0,     0000,         3, 0,  0,  0, 0,  s1x3,  sdmd},

   /* This map must be very late, after the two that do 2x4->4x4
      and the one that does 2x4->2x8. */
   {{3, 5, 14, 8, 9, 12, 7, 2,      1, 4, 15, 10, 11, 13, 6, 0},            0x4141,   0xF0F0,         8, 1,  0,  0, 0,  s2x4,  sdeepbigqtg},

   {{9, 8, 23, 22, 14, 15, 18, 19,       2, 3, 6, 7, 21, 20, 11, 10},            0,     0000,         8, 0,  0,  0, 0,  s_rigger, s4x6},

   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};


Private tm_thing maps_isearch_threesome[] = {

/*   map1                  map2                  map3                 map4    ilatmask olatmask    limit rot            insetup outsetup */
   {{0,                    1,                    2},                           0x1,       07,         1, 0,  0,  0, 0,  s1x1,  s1x3},
   {{0,                    1,                    2},                           0x0,       07,         1, 1,  0,  0, 0,  s1x1,  s1x3},
   {{0, 5,                 1, 4,                 2, 3},                        0x5,      077,         2, 0,  0,  0, 0,  s1x2,  s1x6},
   {{0, 5,                 1, 4,                 2, 3},                          0,      077,         2, 1,  0,  0, 0,  s1x2,  s2x3},
   {{0, 3, 8, 11,          1, 4, 7, 10,          2, 5, 6, 9},                 0x55,    07777,         4, 0,  0,  0, 0,  s2x2,  s2x6},
   {{3, 8, 11, 0,          4, 7, 10, 1,          5, 6, 9, 2},                    0,    07777,         4, 1,  0,  0, 0,  s2x2,  s2x6},
   {{0, 3, 6, 7,           1, -1, 5, -1,         2, -1, 4, -1},               0x11,     0x77,         4, 0,  0,  0, 0,  sdmd,  s1x3dmd},
   {{3, 6, 7, 0,           -1, 5, -1, 1,         -1, 4, -1, 2},                  0,     0x77,         4, 1,  0,  0, 0,  sdmd,  s3x1dmd},
   {{7, 0, 3, 6,           -1, 1, -1, 5,         -1, 2, -1, 4},               0x44,     0x77,         4, 0,  0,  0, 0,  sdmd,  s_spindle},
   {{0, 3, 6, 7,           1, -1, 5, -1,         2, -1, 4, -1},                  0,     0x77,         4, 1,  0,  0, 0,  sdmd,  s_323},
   {{0, 5, 8, 11,          1, -1, 7, -1,         2, -1, 6, -1},                  0,     0707,         4, 1,  0,  0, 0,  sdmd,  s3dmd},
   {{0, 5, 8, 9,           1, 4, 7, 10,          2, 3, 6, 11},                   0,    07777,         4, 1,  0,  0, 0,  sdmd,  s3dmd},
   {{0, 3, 8, 11,          1, 4, 7, 10,          2, 5, 6, 9},                 0x55,    07777,         4, 0,  0,  0, 0,  s1x4,  s1x12},
   {{0, 2, 7, 6,           1, -1, 5, -1,         3, -1, 4, -1},               0x11,     0xBB,         4, 0,  0,  0, 0,  s1x4,  s1x8},
   {{0, 1, 4, 6,           -1, 3, -1, 7,         -1, 2, -1, 5},               0x44,     0xEE,         4, 0,  0,  0, 0,  s1x4,  s1x8},
   {{3, 8, 21, 14, 17, 18, 11, 0,         4, 7, 22, 13, 16, 19, 10, 1,
                                          5, 6, 23, 12, 15, 20, 9, 2},           0,0x0FFFFFF,         8, 1,  0,  0, 0,  s2x4,  s4x6},
   {{19, 18, 16, 17, 12, 13, 15, 14,      20, 21, 23, 22, 8, 9, 11, 10,
                                          0, 1, 3, 2, 7, 6, 4, 5},               0,     0000,         8, 0,  0,  0, 0,  s1x8,  s3x8},
   {{9, 8, 6, 7,           10, 11, 4, 5,         0, 1, 3, 2},                    0,     0000,         4, 0,  0,  0, 0,  s1x4,  s3x4},
   {{9, 11, 6, 5,          10, -1, 4, -1,        0, -1, 3, -1},                  0,     0000,         4, 0,  0,  0, 0,  s1x4,  s3x4},
   {{0, 3, 6, 7,           1, -1, 5, -1,         2, -1, 4, -1},               0x11,     0x77,         4, 0,  0,  0, 0,  s2x2,  s2x4},
   {{0, 1, 4, 7,           -1, 2, -1, 6,         -1, 3, -1, 5},               0x44,     0xEE,         4, 0,  0,  0, 0,  s2x2,  s2x4},
   {{3, 6, 7, 0,           -1, 5, -1, 1,         -1, 4, -1, 2},                  0,     0x77,         4, 1,  0,  0, 0,  s2x2,  s2x4},
   {{1, 4, 7, 0,           2, -1, 6, -1,         3, -1, 5, -1},                  0,     0xEE,         4, 1,  0,  0, 0,  s2x2,  s2x4},
   {{6, 5, 2, 4,           -1, 7, -1, 3,         -1, 0, -1, 1},                  0,     0000,         4, 0,  0,  0, 0,  s1x4,  s_qtag},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

static tm_thing maps_isearch_foursome[] = {

/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{0, 6,             1, 7,             3, 5,             2, 4},              0x5,    0x0FF,         2, 0,  0,  0, 0,  s1x2,  s1x8},
   {{0, 7,             1, 6,             2, 5,             3, 4},                0,    0x0FF,         2, 1,  0,  0, 0,  s1x2,  s2x4},
   {{0, 4, 11, 15,     1, 5, 10, 14,     2, 6, 9, 13,      3, 7, 8, 12},      0x55,  0x0FFFF,         4, 0,  0,  0, 0,  s2x2,  s2x8},
   {{4, 11, 15, 0,     5, 10, 14, 1,     6, 9, 13, 2,      7, 8, 12, 3},         0,  0x0FFFF,         4, 1,  0,  0, 0,  s2x2,  s2x8},
   {{0, 7, 11, 12,     1, 6, 10, 13,     2, 5, 9, 14,      3, 4, 8, 15},         0,  0x0FFFF,         4, 1,  0,  0, 0,  sdmd,  s4dmd},
   {{0, 4, 11, 15,     1, 5, 10, 14,     2, 6, 9, 13,      3, 7, 8, 12},      0x55,  0x0FFFF,         4, 0,  0,  0, 0,  s1x4,  s1x16},
   {{17, 16, 15, 12, 13, 14,         18, 19, 20, 23, 22, 21,
         11, 10, 9, 6, 7, 8,                 0, 1, 2, 5, 4, 3},                  0,     0000,         6, 0,  0,  0, 0,  s1x6,  s4x6},
   {{8, 6, 4, 5,       9, 11, 2, 7,      10, 15, 1, 3,     12, 13, 0, 14},       0,     0000,         4, 0,  0,  0, 0,  s1x4,  s4x4},
   {{12, 10, 8, 9,     13, 15, 6, 11,    14, 3, 5, 7,      0, 1, 4, 2},          0,   0xFFFF,         4, 1,  0,  0, 0,  s1x4,  s4x4},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

static tm_thing maps_isearch_sixsome[] = {
/*   maps
           ilatmask olatmask    limit rot            insetup outsetup */
   {{0, 11,   1, 10,   2, 9,    3, 8,    4, 7,    5, 6},
             0x5,    0x0FFF,       2, 0,  0,  0, 0,  s1x2,  s1x12},
   {{0, 11,   1, 10,   2, 9,    3, 8,    4, 7,    5, 6},
               0,    0x0FFF,       2, 1,  0,  0, 0,  s1x2,  s2x6},
   {{0, 6, 17, 23,   1, 7, 16, 22,   2, 8, 15, 21,    3, 9, 14, 20,    4, 10, 13, 19,    5, 11, 12, 18},
            0x55, 0x0FFFFFF,       4, 0,  0,  0, 0,  s2x2,  s2x12},
   {{0, 11, 17, 18,  1, 10, 16, 19,  2, 9, 15, 20,    3, 8, 14, 21,    4, 7, 13, 22,     5, 6, 12, 23},
               0, 0x0FFFFFF,       4, 1,  0,  0, 0,  s1x4,  s4x6},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

static tm_thing maps_isearch_eightsome[] = {
/*   maps
           ilatmask olatmask    limit rot            insetup outsetup */
   {{0, 15,   1, 14,   2, 13,   3, 12,   4, 11,   5, 10,   6, 9,   7, 8},
             0x5,   0x0FFFF,       2, 0,  0,  0, 0,  s1x2,  s1x16},
   {{0, 15,   1, 14,   2, 13,   3, 12,   4, 11,   5, 10,   6, 9,   7, 8},
               0,   0x0FFFF,       2, 1,  0,  0, 0,  s1x2,  s2x8},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

static tm_thing maps_isearch_boxsome[] = {

/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{0,                1,                3,                2},                   1,      0xF,         1, 0,  0,  0, 0,  s1x1,  s2x2},
   {{3,                0,                2,                1},                   0,        0,         1, 0,  0,  0, 0,  s1x1,  s2x2},
   {{0, 2,             1, 3,             7, 5,             6, 4},              0x5,     0xFF,         2, 0,  0,  0, 0,  s1x2,  s2x4},
   {{7, 5,             0, 2,             6, 4,             1, 3},                0,        0,         2, 0,  0,  0, 0,  s1x2,  s2x4},
   {{0, 2, 6, 4,       1, 3, 7, 5,       15, 13, 9, 11,    14, 12, 8, 10},    0x55,   0xFFFF,         4, 0,  0,  0, 0,  s1x4,  s2x8},
   {{15, 13, 9, 11,    0, 2, 6, 4,       14, 12, 8, 10,    1, 3, 7, 5},          0,        0,         4, 0,  0,  0, 0,  s1x4,  s2x8},
   {{12, 14, 7, 9,     13, 0, 2, 11,     10, 3, 5, 8,      15, 1, 4, 6},      0x55,   0xFFFF,         4, 0,  0,  0, 0,  s2x2,  s4x4},
   {{10, 3, 5, 8,      12, 14, 7, 9,     15, 1, 4, 6,      13, 0, 2, 11},        0,        0,         4, 0,  0,  0, 0,  s2x2,  s4x4},
   {{11, 2, 7, 20,     10, 3, 6, 21,     18, 9, 22, 15,    19, 8, 23, 14},    0x55,0x0FCCFCC,         4, 0,  0,  0, 0,  sdmd,  s4x6},
   {{18, 9, 22, 15,    11, 2, 7, 20,     19, 8, 23, 14,    10, 3, 6, 21},        0,        0,         4, 0,  0,  0, 0,  sdmd,  s4x6},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};


Private tm_thing maps_isearch_dmdsome[] = {

/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{0,                1,                3,                2},                   1,      0xF,         1, 0,  0,  0, 0,  s1x1,  sdmd},
   {{0,                1,                3,                2},                   0,      0xF,         1, 1,  0,  0, 0,  s1x1,  sdmd},
   {{0, 6,             1, 7,             3, 5,             2, 4},              0x5,     0xFF,         2, 0,  0,  0, 0,  s1x2,  s_ptpd},
   {{5, 4,             6, 3,             7, 2,             0, 1},                0,        0,         2, 0,  0,  0, 0,  s1x2,  s_qtag},
   {{11, 10, 8, 9,     12, 14, 5, 7,     13, 15, 4, 6,     0, 1, 3, 2},          0,        0,         4, 0,  0,  0, 0,  s1x4,  s4dmd},
   {{12, 14, 5, 7,     0, 1, 3, 2,       11, 10, 8, 9,     13, 15, 4, 6},     0x55,   0xFFFF,         4, 0,  0,  0, 0,  s1x4,  s4ptpd},
   {{-2, 4, -2, 5,     -2, 3, -2, 6,     -2, 2, -2, 7,     -2, 1, -2, 0},     0x55,        0,         4, 1,  0,  0, 0,  sdmd,  s_qtag},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};


Private tm_thing maps_isearch_tglsome[] = {

/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{6, 0, 2, 4,       -1, 7, -1, 3,     -1, 5, -1, 1},                       0xC4,     0xBB,         4, 0,  0,  0, 0,  s1x4,  s_rigger},
   {{1, 2, 5, 6,       0, -1, 4, -1,     3, -1, 7, -1},                       0x31,     0xBB,         4, 0,  0,  0, 0,  s1x4,  s_ptpd},
   {{0, 3, 4, 7,       -1, 2, -1, 6,     -1, 1, -1, 5},                       0x4C,     0xEE,         4, 0,  0,  0, 0,  s1x4,  s_ptpd},

   {{7, 1, 3, 5,       0, -1, 4, -1,     6, -1, 2, -1},                       0x20,        0,         4, 0,  0,  0, 0,  s2x2,  s_qtag},
   {{0, 2, 4, 6,       -1, 1, -1, 5,     -1, 3, -1, 7},                       0x80,        0,         4, 0,  0,  0, 0,  s2x2,  s_qtag},

   {{1, 3, 5, 7,       -1, 4, -1, 0,     -1, 2, -1, 6},                       0x4C,        0,         4, 1,  0,  0, 0,  s2x2,  s_qtag},
   {{2, 4, 6, 0,       1, -1, 5, -1,     3, -1, 7, -1},                       0x31,        0,         4, 1,  0,  0, 0,  s2x2,  s_qtag},

   {{5, 7, 1, 3,       6, -1, 2, -1,     0, -1, 4, -1},                       0x13,     0x77,         4, 0,  0,  0, 0,  s1x4,  s_bone},
   {{0, 1, 4, 5,       7, -1, 3, -1,     6, -1, 2, -1},                       0x31,     0xDD,         4, 0,  0,  0, 0,  sdmd,  s_spindle},
   {{5, 3, 1, 7,       6, -1, 2, -1,     0, -1, 4, -1},                       0x13,     0x77,         4, 0,  0,  0, 0,  sdmd,  s_dhrglass},
   {{6, 0, 2, 4,       -1, 3, -1, 7,     -1, 1, -1, 5},                       0x08,        0,         4, 0,  0,  0, 0,  sdmd,  s_hrglass},
   {{0, 3, 4, 7,       6, -1, 2, -1,     5, -1, 1, -1},                       0x31,     0x77,         4, 0,  0,  0, 0,  sdmd,  s_hrglass},

   {{0, 3, 4, 7,       -1, 2, -1, 6,     -1, 1, -1, 5},                       0x80,        0,         4, 0,  0,  0, 0,  sdmd,  s_galaxy},
   {{2, 5, 6, 1,       -1, 4, -1, 0,     -1, 3, -1, 7},                       0x08,     0xBB,         4, 1,  0,  0, 0,  sdmd,  s_galaxy},

   {{1, 7, 9, 15,      -1, 11, -1, 3,    -1, 5, -1, 13},                      0x08,   0x0000,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{0, 4, 8, 12,      -1, 2, -1, 10,    -1, 6, -1, 14},                      0xC4,   0x5454,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{3, 5, 11, 13,     7, -1, 15, -1,    1, -1, 9, -1},                       0x13,   0x8A8A,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{0, 4, 8, 12,      14, -1, 6, -1,    2, -1, 10, -1},                      0x02,   0x0000,         4, 0,  0,  0, 0,  s2x2,  s_c1phan},
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};


Private tm_thing maps_isearch_3x1tglsome[] = {
/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{9, 3,             10, 4,            0, 6,             11, 5},             0x7,    07171,         2, 0,  0,  0, 0,  s1x2,  s3x4},
   {{0, 4,             7, 3,             5, 1,             6, 2},              0xD,     0xFF,         2, 0,  0,  0, 0,  s1x2,  s_qtag},
   {{9, 3,             10, 4,            11, 5,            1, 7},              0x8,     0000,         2, 0,  0,  0, 0,  s1x2,  s2x6},
   {{0, 6,             1, 7,             2, 8,             10, 4},             0x2,     0000,         2, 0,  0,  0, 0,  s1x2,  s2x6},

   {{9, 6, 21, 18,     10, 7, 22, 19,    11, 8, 23, 20,    1, 4, 13, 16},      0x0A,    0000,         4, 0,  0,  0, 0,  s2x2,  s4x6},
   {{0, 3, 12, 15,     1, 4, 13, 16,     2, 5, 14, 17,     10, 7, 22, 19},     0xA0,    0000,         4, 0,  0,  0, 0,  s2x2,  s4x6},

   {{19, 3, 7, 15,     20, 23, 8, 11,    0, 16, 12, 4,     21, 22, 9, 10},     0x7D,   0xF99F99,      4, 0,  0,  0, 0,  s1x4,  s3x8},
   {{1, 17, 13, 5,     21, 22, 9, 10,    18, 2, 6, 14,     20, 23, 8, 11},     0xD7,   0xF66F66,      4, 0,  0,  0, 0,  s1x4,  s3x8},

   {{0, 18, 12, 6,     1, 19, 13, 7,     2, 20, 14, 8,     22, 4, 10, 16},     0x28,   0000,          4, 0,  0,  0, 0,  s1x4,  s2x12},
   {{21, 3, 9, 15,     22, 4, 10, 16,    23, 5, 11, 17,    1, 19, 13, 7},      0x82,   0000,          4, 0,  0,  0, 0,  s1x4,  s2x12},

   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

Private tm_thing maps_isearch_ysome[] = {
/*   map1              map2              map3              map4               ilatmask olatmask    limit rot            insetup outsetup */
   {{5, 1,             0, 4,             6, 2,             7, 3},              0x7,     0xFF,         2, 0,  0,  0, 0,  s1x2,  s_bone},
   {{0, 4,             5, 1,             7, 3,             6, 2},              0xD,     0xFF,         2, 0,  0,  0, 0,  s1x2,  s_rigger},
   {{15, 7,            13, 5,            3, 11,            1, 9},              0x8,     0000,         2, 0,  0,  0, 0,  s1x2,  s_c1phan},
   {{0, 8,             2, 10,            14, 6,            12, 4},             0x2,     0000,         2, 0,  0,  0, 0,  s1x2,  s_c1phan},
/*   Taken out -- we rotate the setup instead.
   {{3, 11,            1, 9,             7, 15,            5, 13},             0x8,   0xAAAA,         2, 1,  0,  0, 0,  s1x2,  s_c1phan},
   {{4, 12,            6, 14,            2, 10,            0, 8},              0x2,   0x5555,         2, 1,  0,  0, 0,  s1x2,  s_c1phan},
*/
   {{0}, 0, 0, 0, 0,  0,  0, 0, nothing, nothing}};

typedef struct {
  setup_kind testkind;
  uint32 testval;
  uint32 fixup;   /* High bit means phantom pairing is OK. */
  warning_index warning;
} siamese_item;

siamese_item siamese_table_of_2[] = {
   {s2x4,        0x00FF0000UL, 0x99UL,   warn__ctrstand_endscpls},
   {s2x4,        0x00990066UL, 0x99UL,   warn__ctrstand_endscpls},
   {s2x4,        0x000000FFUL, 0x66UL,   warn__ctrscpls_endstand},
   {s2x4,        0x00660099UL, 0x66UL,   warn__ctrscpls_endstand},
   {s2x4,        0x00F0000FUL, 0x0FUL,   warn__none},  // unsymm
   {s2x4,        0x000F00F0UL, 0xF0UL,   warn__none},  // unsymm
   {s1x8,        0x00CC0033UL, 0x33UL,   warn__ctrstand_endscpls},
   {s1x8,        0x003300CCUL, 0xCCUL,   warn__ctrscpls_endstand},
   {s2x4,        0x003300CCUL, 0xCCUL,   warn__none},
   {s2x4,        0x00CC0033UL, 0x33UL,   warn__none},
   {s_trngl4,    0x000F0000UL, 0x03UL,   warn__none},
   {s_trngl4,    0x0000000FUL, 0x0CUL,   warn__none},
   {s_c1phan,    0x0000AAAAUL, 0xA0A0UL, warn__none},
   {s_c1phan,    0x00005555UL, 0x0505UL, warn__none},
   {s_c1phan,    0xAAAA0000UL, 0x0A0AUL, warn__none},
   {s_c1phan,    0x55550000UL, 0x5050UL, warn__none},
   {s_c1phan,    0x00005AA5UL, 0x00A5UL, warn__none},  /* These 8 are unsymmetrical. */
   {s_c1phan,    0x0000A55AUL, 0xA500UL, warn__none},
   {s_c1phan,    0x000055AAUL, 0x05A0UL, warn__none},
   {s_c1phan,    0x0000AA55UL, 0xA005UL, warn__none},
   {s_c1phan,    0x5AA50000UL, 0x5A00UL, warn__none},
   {s_c1phan,    0xA55A0000UL, 0x005AUL, warn__none},
   {s_c1phan,    0x55AA0000UL, 0x500AUL, warn__none},
   {s_c1phan,    0xAA550000UL, 0x0A50UL, warn__none},

   {s4x4,        0x0000AAAAUL, 0x0A0AUL, warn__none},
   {s4x4,        0x0000CCCCUL, 0x8484UL, warn__none},
   {s4x4,        0xAAAA0000UL, 0xA0A0UL, warn__none},
   {s4x4,        0xCCCC0000UL, 0x4848UL, warn__none},

   {s4x5,        0x000390E4UL, 0x18060UL, warn__none},
   {s4x5,        0x000E1384UL, 0xC0300UL, warn__none},
   {s4x5,        0x90E40000UL, 0x21084UL, warn__none},
   {s4x5,        0x13840000UL, 0x21084UL, warn__none},

   {s4x4,        0x30304141UL, 0x80004141UL, warn__none},
   {s4x4,        0x41413030UL, 0x80003030UL, warn__none},
   {s4x4,        0x03031414UL, 0x80000303UL, warn__none},
   {s4x4,        0x14140303UL, 0x80001414UL, warn__none},

   {s3x4,        0x09E70000UL, 0x0924UL, warn__none},
   {s3x4,        0x000009E7UL, 0x00C3UL, warn__none},
   {s3x4,        0x0BAE0000UL, 0x08A2UL, warn__none},
   {s3x4,        0x00000BAEUL, 0x030CUL, warn__none},

   {s_qtag,      0x003300CCUL, 0xCCUL,   warn__ctrscpls_endstand},
   {s_qtag,      0x00CC0033UL, 0x33UL,   warn__ctrstand_endscpls},
   {s4dmd,       0x0F0FF0F0UL, 0xF0F0UL, warn__ctrscpls_endstand},
   {s4dmd,       0xF0F00F0FUL, 0x0F0FUL, warn__ctrstand_endscpls},
   {s_rigger,    0x00FF0000UL, 0x33UL,   warn__ctrscpls_endstand},
   {s_rigger,    0x00CC0033UL, 0x33UL,   warn__ctrscpls_endstand},
   {s_rigger,    0x000000FFUL, 0xCCUL,   warn__ctrstand_endscpls},
   {s_rigger,    0x003300CCUL, 0xCCUL,   warn__ctrstand_endscpls},
   {s_bone,      0x00FF0000UL, 0x33UL,   warn__ctrstand_endscpls},
   {s_bone,      0x000000FFUL, 0xCCUL,   warn__ctrscpls_endstand},
   {s_crosswave, 0x00FF0000UL, 0xCCUL,   warn__ctrscpls_endstand},
   {s_crosswave, 0x000000FFUL, 0x33UL,   warn__ctrstand_endscpls},
   {sdeepbigqtg, 0xFFFF0000UL, 0x0F0FUL, warn__none},
   {sdeepbigqtg, 0x0000FFFFUL, 0xF0F0UL, warn__none},
   {sdeepbigqtg, 0x3535CACAUL, 0xC5C5UL, warn__none},
   {sdeepbigqtg, 0xCACA3535UL, 0x3A3AUL, warn__none},
   {sdeepbigqtg, 0xC5C53A3AUL, 0x3535UL, warn__none},
   {sdeepbigqtg, 0x3A3AC5C5UL, 0xCACAUL, warn__none},
   {nothing,     0,            0,        warn__none}};

siamese_item siamese_table_of_3[] = {
   {s2x6,        0x01C70E38UL, 0xE38UL,  warn__none},
   {s2x6,        0x0E3801C7UL, 0x1C7UL,  warn__none},
   {nothing,     0,            0,        warn__none}};

siamese_item siamese_table_of_4[] = {
   {s2x8,        0x0F0FF0F0UL, 0xF0F0UL, warn__none},
   {s2x8,        0xF0F00F0FUL, 0x0F0FUL, warn__none},
   {nothing,     0,            0,        warn__none}};

Private void initialize_one_table(tm_thing *map_start, int np)
{
   tm_thing *map_search;

   for (map_search = map_start; map_search->outsetup != nothing; map_search++) {
      int i, j;
      uint32 m;
      uint32 imask = 0;
      uint32 omask = 0;
      uint32 osidemask = 0;

      /* All 1's for people in outer setup. */
      uint32 alloutmask = (1 << (setup_attrs[map_search->outsetup].setup_limits+1))-1;

      for (i=0, m=1; i<map_search->limit; i++, m<<=2) {
         if (map_search->maps[i] == -2) continue;

         alloutmask &= ~(1 << map_search->maps[i]);

         if (map_search->maps[i+map_search->limit] < 0) {
            imask |= m;
            omask |= 1 << map_search->maps[i];
         }
         else {
            for (j=1 ; j<np ; j++)
               alloutmask &= ~(1 << map_search->maps[i+(map_search->limit*j)]);

            if (((map_search->ilatmask >> (i*2)) ^ map_search->rot) & 1) {
               for (j=0 ; j<np ; j++)
                  osidemask |= 1 << map_search->maps[i+(map_search->limit*j)];
            }
         }
      }

      map_search->insinglemask = imask;
      map_search->outsinglemask = omask;
      map_search->outunusedmask = alloutmask;

      /* We can't encode the virtual person number in the required 3-bit field if this is > 8. */
      if (map_search->limit != setup_attrs[map_search->insetup].setup_limits+1)
         (*the_callback_block.uims_database_error_fn)("Tandem table initialization failed: limit wrong.\n", (Cstring) 0);
      if (map_search->olatmask != osidemask)
         (*the_callback_block.uims_database_error_fn)("Tandem table initialization failed: Smask.\n", (Cstring) 0);
   }
}


extern void initialize_tandem_tables(void)
{
   initialize_one_table(maps_isearch_twosome, 2);
   initialize_one_table(maps_isearch_threesome, 3);
   initialize_one_table(maps_isearch_foursome, 4);
   initialize_one_table(maps_isearch_sixsome, 6);
   initialize_one_table(maps_isearch_eightsome, 8);
   initialize_one_table(maps_isearch_boxsome, 4);
   initialize_one_table(maps_isearch_dmdsome, 4);
   initialize_one_table(maps_isearch_tglsome, 3);
   initialize_one_table(maps_isearch_3x1tglsome, 4);
   initialize_one_table(maps_isearch_ysome, 4);
}


Private void unpack_us(
   tm_thing *map_ptr,
   uint32 orbitmask,
   tandrec *tandstuff,
   setup *result)
{
   int i;
   uint32 m, o, r;

   result->kind = map_ptr->outsetup;
   result->rotation = tandstuff->virtual_result.rotation - map_ptr->rot;
   result->result_flags = tandstuff->virtual_result.result_flags;
   r = map_ptr->rot*011;

   for (i=0, m=map_ptr->insinglemask, o=orbitmask; i<map_ptr->limit; i++, m>>=2, o>>=2) {
      uint32 z = rotperson(tandstuff->virtual_result.people[i].id1, r);

      if (z != 0) {
         int ii = (z >> 6) & 7;

         if (m & 1) {
            /* Unpack single person. */

            personrec f = tandstuff->real_saved_people[0][ii];
            if (f.id1) f.id1 = (f.id1 & ~(ROLL_MASK|STABLE_MASK|077)) | (z & (ROLL_MASK|STABLE_MASK|013));
            result->people[map_ptr->maps[i]] = f;
         }
         else {
            int j;
            personrec fb[8];

            /* Unpack tandem/couples person. */
   
            for (j=0 ; j<tandstuff->np ; j++) {
               fb[j] = tandstuff->real_saved_people[j][ii];
               if (fb[j].id1) fb[j].id1 =
                                 (fb[j].id1 & ~(ROLL_MASK|STABLE_MASK|077)) |
                                 (z & (ROLL_MASK|STABLE_MASK|013));
            }

            if (map_ptr->maps[i+map_ptr->limit] < 0)
               fail("This would go to an impossible setup.");

            if (((o + (map_ptr->rot&1) + 1) & 2) && !tandstuff->no_unit_symmetry) {
               for (j=0 ; j<tandstuff->np ; j++)
                  result->people[map_ptr->maps[i+(map_ptr->limit*j)]] = fb[tandstuff->np-j-1];
            }
            else {
               for (j=0 ; j<tandstuff->np ; j++)
                  result->people[map_ptr->maps[i+(map_ptr->limit*j)]] = fb[j];
            }
         }
      }
   }

   canonicalize_rotation(result);
}




/* Lat = 0 means the people we collapsed, relative to the incoming geometry, were one
   behind the other.  Lat = 1 means they were lateral.  "Incoming geometry" does
   not include the incoming rotation field, since we ignore it.  We are not responsible
   for the rotation field of the incoming setup.

  The canonical storage of the real people, while we are doing the virtual call, is as follows:

   Real_saved_people[0] gets person on left (lat=1) near person (lat=0).
   Real_saved_people[last] gets person on right (lat=1) or far person (lat=0). */

// Returns TRUE if it found people facing the wrong way.  This can happen if we are
// trying siamese and we shouldn't be.

Private long_boolean pack_us(
   personrec *s,
   tm_thing *map_ptr,
   int fraction,
   long_boolean twosome,
   int key,
   tandrec *tandstuff)
{
   int i, j;
   uint32 m, sgl;
   int virt_index = -1;

   tandstuff->virtual_setup.rotation = map_ptr->rot & 1;
   tandstuff->virtual_setup.kind = map_ptr->insetup;

   for (i=0, m=map_ptr->ilatmask, sgl=map_ptr->insinglemask; i<map_ptr->limit; i++, m>>=2, sgl>>=2) {
      personrec fb[8];
      personrec *ptr = &tandstuff->virtual_setup.people[i];
      uint32 vp1, vp2;
      int vert = (1 + map_ptr->rot + m) & 3;

      fb[0] = s[map_ptr->maps[i]];

      if (!tandstuff->no_unit_symmetry) vert &= 1;

      if (sgl & 1) {
         vp1 = fb[0].id1;
         vp2 = fb[0].id2;
         fb[1].id1 = ~0UL;
      }
      else {
         uint32 orpeople = 0;
         uint32 andpeople = ~0;

         for (j=1 ; j<tandstuff->np ; j++) fb[j] = s[map_ptr->maps[i+(map_ptr->limit*j)]];

         for (j=0 ; j<tandstuff->np ; j++) {
            orpeople |= fb[j].id1;
            andpeople &= fb[j].id1;
         }

         if (key == tandem_key_skew) {
            /* If this is skew/skewsome, we require people paired in the appropriate way.
               This means [fb[0], fb[3]] must match, [fb[1], fb[2]] must match, and
               [fb[0], fb[1]] must not match. */

            if ((((fb[0].id1 ^ fb[3].id1) |
                  (fb[1].id1 ^ fb[2].id1) |
                  (~(fb[0].id1 ^ fb[1].id1))) & BIT_PERSON))
               fail("Can't find skew people.");
         }
         else if (!(tandstuff->virtual_setup.cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) &&
                  !tandstuff->phantom_pairing_ok) {

            /* Otherwise, we usually forbid phantoms unless some phantom concept was used
               (either something like "phantom tandem" or some other phantom concept such
               as "split phantom lines").  If we get here, such a concept was not used.
               We forbid a live person paired with a phantom.  Additionally, we forbid
               ANY PERSON AT ALL to be a phantom, even if paired with another phantom,
               except in the special case of a virtual 2x3. */

            if (!(andpeople & BIT_PERSON)) {
               if (orpeople ||
                   (tandstuff->virtual_setup.kind != s2x3 &&
                    key != tandem_key_siam))
                  fail("Use \"phantom\" concept in front of this concept.");
            }
         }

         if (orpeople) {
            if (twosome >= 2 && (orpeople & STABLE_MASK))
               fail("Sorry, can't nest fractional stable/twosome.");

            vp1 = ~0UL;
            vp2 = ~0UL;

            /* Create the virtual person.  When both people are present, anding
               the real peoples' id2 bits gets the right bits.  For example,
               the virtual person will be a boy and able to do a tandem star thru
               if both real people were boys.  Remove the identity field (700 bits)
               from id1 and replace with a virtual person indicator.  Check that
               direction, roll, and stability parts of id1 are consistent. */

            for (j=0 ; j<tandstuff->np ; j++) {
               if (fb[j].id1) {
                  vp1 &= fb[j].id1;
                  vp2 &= fb[j].id2;

                  /* If they have different fractional stability states,
                     just clear them -- they can't do it. */
                  if ((fb[j].id1 ^ orpeople) & STABLE_MASK) vp1 &= ~STABLE_MASK;
                  /* If they have different roll states, just clear them -- they can't roll. */
                  if ((fb[j].id1 ^ orpeople) & ROLL_MASK) vp1 &= ~ROLL_MASK;
                  /* Check that all real people face the same way. */
                  if ((fb[j].id1 ^ orpeople) & 077)
                     return TRUE;
               }
            }
         }
         else
            vp1 = 0;
      }

      if (vp1) {

         /* Create a virtual person number.  Only 8 are allowed, because of the tightness
            in the person representation.  That shouldn't be a problem, since each
            virtual person came from a group of real people that had at least one
            live person.  Unless active phantoms have been created, that is. */

         if ((++virt_index) >= 8) fail("Sorry, too many tandem or as couples people.");

         ptr->id1 = (vp1 & ~0700) | (virt_index << 6) | BIT_TANDVIRT;
         ptr->id2 = vp2;

         if (!(sgl & 1)) {
            if (twosome >= 2)
               ptr->id1 |= STABLE_ENAB | (STABLE_RBIT * fraction);

            /* 1 if original people were near/far; 0 if lateral */
            tandstuff->vertical_people[virt_index] = vert;
         }

         if (map_ptr->rot & 1)   /* Compensate for setup rotation. */
            ptr->id1 = rotccw(ptr->id1);

         for (j=0 ; j<tandstuff->np ; j++)
            tandstuff->real_saved_people[j][virt_index] = fb[j];

         tandstuff->saved_originals[virt_index] = ptr->id1 + tandstuff->virtual_setup.rotation;
      }
      else {
         ptr->id1 = 0;
         ptr->id2 = 0;
      }
   }

   return FALSE;
}



extern void tandem_couples_move(
   setup *ss,
   selector_kind selector,
   int twosome,           /* solid=0 / twosome=1 / solid-to-twosome=2 / twosome-to-solid=3 */
   int fraction,          /* number, if doing fractional twosome/solid */
   int phantom,           /* normal=0 phantom=1 general-gruesome=2 gruesome-with-wave-check=3 */
   tandem_key key,
   uint32 mxn_bits,
   long_boolean phantom_pairing_ok,
   setup *result)
{
   selector_kind saved_selector;
   tandrec tandstuff;
   tm_thing *map;
   tm_thing *map_search;
   uint32 nsmask, ewmask, allmask, special_mask;
   int i, np;
   uint32 jbit;
   uint32 hmask, hmask2;
   uint32 orbitmask;
   uint32 sglmask, sglmask2;
   uint32 livemask, livemask2;
   long_boolean fractional = FALSE;
   long_boolean dead_conc = FALSE;
   tm_thing *our_map_table;

   special_mask = 0;
   tandstuff.single_mask = 0;
   tandstuff.no_unit_symmetry = FALSE;
   tandstuff.phantom_pairing_ok = phantom_pairing_ok;
   clear_people(result);

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_MASK)
      remove_z_distortion(ss);

   if (mxn_bits != 0) {
      uint32 directions = 0;
      uint32 livemask = 0;
      tandem_key transformed_key = key;

      if (key == tandem_key_tand3 || key == tandem_key_tand4)
         transformed_key = tandem_key_tand;
      else if (key == tandem_key_cpls3 || key == tandem_key_cpls4)
         transformed_key = tandem_key_cpls;
      else if (key == tandem_key_siam3 || key == tandem_key_siam4)
         transformed_key = tandem_key_siam;

      if ((transformed_key & ~1) != 0) fail("Can't do this combination of concepts.");

      switch (mxn_bits) {
      case INHERITFLAGNXNK_3X3:
         np = 3;
         our_map_table = maps_isearch_threesome;
         goto foobarves;
      case INHERITFLAGNXNK_4X4:
         np = 4;
         our_map_table = maps_isearch_foursome;
         goto foobarves;
      case INHERITFLAGNXNK_6X6:
         np = 6;
         our_map_table = maps_isearch_sixsome;
         goto foobarves;
      case INHERITFLAGNXNK_8X8:
         np = 8;
         our_map_table = maps_isearch_eightsome;
         goto foobarves;
      }

      for (i=0; i<=setup_attrs[ss->kind].setup_limits; i++) {
         uint32 t = ss->people[i].id1;
         directions <<= 2;
         livemask <<= 2;
         if (t) { livemask |= 3 ; directions |= t & 3; }
      }

      if (mxn_bits == INHERITFLAGMXNK_2X1 || mxn_bits == INHERITFLAGMXNK_1X2) {
         np = 2;
         our_map_table = maps_isearch_twosome;

         if (ss->kind == s2x3 || ss->kind == s1x6) {
            if (transformed_key == tandem_key_tand) directions ^= 0x555;

            if (((directions ^ 0x0A8) & livemask) == 0 ||
                ((directions ^ 0xA02) & livemask) == 0)
               special_mask |= 044;

            if (((directions ^ 0x2A0) & livemask) == 0 ||
                ((directions ^ 0x80A) & livemask) == 0)
               special_mask |= 011;

            if (mxn_bits == INHERITFLAGMXNK_2X1 && ((directions ^ 0x02A) & livemask) == 0)
               special_mask |= 011;
   
            if (mxn_bits == INHERITFLAGMXNK_2X1 && ((directions ^ 0xA80) & livemask) == 0)
               special_mask |= 044;
   
            if (mxn_bits == INHERITFLAGMXNK_1X2 && ((directions ^ 0x02A) & livemask) == 0)
               special_mask |= 044;
   
            if (mxn_bits == INHERITFLAGMXNK_1X2 && ((directions ^ 0xA80) & livemask) == 0)
               special_mask |= 011;
   
            if (special_mask != 011 && special_mask != 044) special_mask = 0;
         }
      }
      else if (mxn_bits == INHERITFLAGMXNK_3X1 || mxn_bits == INHERITFLAGMXNK_1X3) {
         np = 3;
         our_map_table = maps_isearch_threesome;
         if (transformed_key == tandem_key_tand) directions ^= 0x5555;

         if (ss->kind == s2x4) {
            if (((directions ^ 0x02A8) & livemask) == 0 ||
                ((directions ^ 0xa802) & livemask) == 0)
               special_mask |= 0x88;
   
            if (((directions ^ 0x2A80) & livemask) == 0 ||
                ((directions ^ 0x802A) & livemask) == 0)
               special_mask |= 0x11;
   
            if (mxn_bits == INHERITFLAGMXNK_3X1 && ((directions ^ 0x00AA) & livemask) == 0)
               special_mask |= 0x11;
   
            if (mxn_bits == INHERITFLAGMXNK_3X1 && ((directions ^ 0xAA00) & livemask) == 0)
               special_mask |= 0x88;
   
            if (mxn_bits == INHERITFLAGMXNK_1X3 && ((directions ^ 0x00AA) & livemask) == 0)
               special_mask |= 0x88;
   
            if (mxn_bits == INHERITFLAGMXNK_1X3 && ((directions ^ 0xAA00) & livemask) == 0)
               special_mask |= 0x11;
   
            if (special_mask != 0x11 && special_mask != 0x88) special_mask = 0;
         }
         else if (ss->kind == s1x8) {
            if (((directions ^ 0x08A2) & livemask) == 0 ||
                ((directions ^ 0xA208) & livemask) == 0)
               special_mask |= 0x44;

            if (((directions ^ 0x2A80) & livemask) == 0 ||
                ((directions ^ 0x802A) & livemask) == 0)
               special_mask |= 0x11;

            if (mxn_bits == INHERITFLAGMXNK_3X1 && ((directions ^ 0x00AA) & livemask) == 0)
               special_mask |= 0x11;
   
            if (mxn_bits == INHERITFLAGMXNK_3X1 && ((directions ^ 0xAA00) & livemask) == 0)
               special_mask |= 0x44;
   
            if (mxn_bits == INHERITFLAGMXNK_1X3 && ((directions ^ 0x00AA) & livemask) == 0)
               special_mask |= 0x44;
   
            if (mxn_bits == INHERITFLAGMXNK_1X3 && ((directions ^ 0xAA00) & livemask) == 0)
               special_mask |= 0x11;
   
            if (special_mask != 0x11 && special_mask != 0x44) special_mask = 0;
         }
         else if (ss->kind == s3x1dmd) {
            if (((directions ^ 0x00A8) & 0xFCFC & livemask) == 0 ||
                ((directions ^ 0xA800) & 0xFCFC & livemask) == 0)
               special_mask |= 0x88;
         }
      }
      else
         fail("Can't do this combination of concepts.");

      if (!special_mask) fail("Can't find 3x3/3x1/2x1 people.");

   foobarves:

      /* This will make it look like "as couples" or "tandem", as needed,
         for swapping masks, but won't trip the assumption transformation stuff. */
      // It will also turn "threesome" into "twosome", as required by the rest of the code.
      key = (tandem_key) (transformed_key | 64);
   }
   else if (key == tandem_key_ys) {
      np = 4;
      our_map_table = maps_isearch_ysome;
      tandstuff.no_unit_symmetry = TRUE;
   }
   else if (key == tandem_key_3x1tgls) {
      np = 4;
      our_map_table = maps_isearch_3x1tglsome;
      tandstuff.no_unit_symmetry = TRUE;
   }
   else if (key >= tandem_key_outpoint_tgls) {
      np = 3;
      our_map_table = maps_isearch_tglsome;
      tandstuff.no_unit_symmetry = TRUE;
      if (key == tandem_key_outside_tgls)
         selector = selector_outer6;
      else if (key == tandem_key_inside_tgls)
         selector = selector_center6;
      else if (key == tandem_key_wave_tgls || key == tandem_key_tand_tgls) {
         if (ss->kind == s_hrglass) {
            uint32 tbonetest =
               ss->people[0].id1 | ss->people[1].id1 |
               ss->people[4].id1 | ss->people[5].id1;

            if ((tbonetest & 011) == 011 || ((key ^ tbonetest) & 1))
               fail("Can't find the indicated triangles.");

            special_mask = 0x44;   /* The triangles have to be these. */
         }
         else if (ss->kind == s_bone) {
            uint32 tbonetest =
               ss->people[0].id1 | ss->people[1].id1 |
               ss->people[4].id1 | ss->people[5].id1;

            if ((tbonetest & 011) == 011 || !((key ^ tbonetest) & 1))
               fail("Can't find the indicated triangles.");

            special_mask = 0x88;   /* The triangles have to be these. */
         }
         else if (ss->kind == s_rigger) {
            uint32 tbonetest =
               ss->people[0].id1 | ss->people[1].id1 |
               ss->people[4].id1 | ss->people[5].id1;

            if ((tbonetest & 011) == 011 || !((key ^ tbonetest) & 1))
               fail("Can't find the indicated triangles.");

            special_mask = 0x44;   /* The triangles have to be these. */
         }
         else if (ss->kind == s_dhrglass) {
            uint32 tbonetest = ss->people[0].id1 | ss->people[1].id1 | ss->people[4].id1 | ss->people[5].id1;

            if ((tbonetest & 011) == 011 || !((key ^ tbonetest) & 1))
               fail("Can't find the indicated triangles.");

            special_mask = 0x88;   /* The triangles have to be these. */
         }
         else if (ss->kind == s_galaxy) {
            uint32 tbonetest = ss->people[1].id1 | ss->people[3].id1 | ss->people[5].id1 | ss->people[7].id1;

            if ((tbonetest & 011) == 011)
               fail("Can't find the indicated triangles.");

            if ((key ^ tbonetest) & 1)
               special_mask = 0x44;
            else
               special_mask = 0x11;
         }
         else if (ss->kind == s_c1phan) {
            uint32 tbonetest = 0;
            int t = key;

            for (i=0; i<16; i++) tbonetest |= ss->people[i].id1;

            if ((tbonetest & 010) == 0) t ^= 1;
            else if ((tbonetest & 1) != 0)
               fail("Can't find the indicated triangles.");

            if (t&1) special_mask = 0x2121;
            else     special_mask = 0x1212;
         }
         else
            fail("Can't find these triangles.");
      }
      else if ((key == tandem_key_outpoint_tgls || key == tandem_key_inpoint_tgls) &&
               ss->kind == s_qtag) {
         if (key == tandem_key_inpoint_tgls) {
            if (     (ss->people[0].id1 & d_mask) == d_east &&
                     (ss->people[1].id1 & d_mask) != d_west &&
                     (ss->people[4].id1 & d_mask) == d_west &&
                     (ss->people[5].id1 & d_mask) != d_east)
               special_mask = 0x22;
            else if ((ss->people[0].id1 & d_mask) != d_east &&
                     (ss->people[1].id1 & d_mask) == d_west &&
                     (ss->people[4].id1 & d_mask) != d_west &&
                     (ss->people[5].id1 & d_mask) == d_east)
               special_mask = 0x11;
         }
         else {
            if (     (ss->people[0].id1 & d_mask) != d_west &&
                     (ss->people[1].id1 & d_mask) == d_east &&
                     (ss->people[4].id1 & d_mask) != d_east &&
                     (ss->people[5].id1 & d_mask) == d_west)
               special_mask = 0x11;
            else if ((ss->people[0].id1 & d_mask) == d_west &&
                     (ss->people[1].id1 & d_mask) != d_east &&
                     (ss->people[4].id1 & d_mask) == d_east &&
                     (ss->people[5].id1 & d_mask) != d_west)
               special_mask = 0x22;
         }

         if (special_mask == 0)
            fail("Can't find designated point.");
      }
      else if (key != tandem_key_anyone_tgls || ss->kind != s_c1phan)
         /* For <anyone>-based triangles in C1-phantom,
            we just use whatever selector was given. */
         fail("Can't find these triangles.");
   }
   else if (key == tandem_key_diamond) {
      np = 4;
      our_map_table = maps_isearch_dmdsome;
   }
   else if (key == tandem_key_box || key == tandem_key_skew) {
      np = 4;
      our_map_table = maps_isearch_boxsome;
   }
   else if (key >= 8) {
      np = 4;
      our_map_table = maps_isearch_foursome;
   }
   else if (key >= 4) {
      np = 3;
      our_map_table = maps_isearch_threesome;
   }
   else {
      np = 2;
      our_map_table = maps_isearch_twosome;
   }

   if (setup_attrs[ss->kind].setup_limits < 0) fail("Can't do tandem/couples concept from this position.");

   /* We use the phantom indicator to forbid an already-distorted setup.
      The act of forgiving phantom pairing is based on the setting of the
      CMD_MISC__PHANTOMS bit in the incoming setup, not on the phantom indicator. */

   if ((ss->cmd.cmd_misc_flags & CMD_MISC__DISTORTED) && (phantom != 0))
      fail("Can't specify phantom tandem/couples in virtual or distorted setup.");

   /* Find out who is selected, if this is a "so-and-so are tandem". */
   saved_selector = current_options.who;

   if (selector != selector_uninitialized)
      current_options.who = selector;

   nsmask = 0;
   ewmask = 0;
   allmask = 0;

   for (i=0, jbit=1; i<=setup_attrs[ss->kind].setup_limits; i++, jbit<<=1) {
      uint32 p = ss->people[i].id1;
      if (p) {
         allmask |= jbit;
         /* We allow a "special" mask to override the selector. */
         if (     (selector != selector_uninitialized && !selectp(ss, i)) ||
                  (jbit & special_mask) != 0)
            tandstuff.single_mask |= jbit;
         else {
            if (p & 1)
               ewmask |= jbit;
            else
               nsmask |= jbit;
         }
      }
   }
   
   current_options.who = saved_selector;

   if (!allmask) {
      result->result_flags = 0;
      result->kind = nothing;
      return;
   }

   if (twosome >= 2) fractional = TRUE;

   if (fractional && fraction > 4)
      fail("Can't do fractional twosome more than 4/4.");

   uint32 siamese_fixup(0);
   warning_index siamese_warning = warn__none;
   siamese_item *ptr;
   long_boolean doing_siamese(FALSE);
   uint32 saveew, savens;

   if (key == tandem_key_box || key == tandem_key_skew) {
      ewmask = allmask;
      nsmask = 0;
   }
   else if (key == tandem_key_diamond) {
      if (ss->kind == s_ptpd || ss->kind == s3ptpd || ss->kind == s4ptpd || ss->kind == sdmd) {
         ewmask = allmask;
         nsmask = 0;
      }
      else {
         ewmask = 0;
         nsmask = allmask;
      }
   }
   else if (tandstuff.no_unit_symmetry) {
      if (key == tandem_key_anyone_tgls) {
         /* This was <anyone>-based triangles.  The setup must have been a C1-phantom.
            The current mask shows just the base.  Expand it to include the apex. */
         ewmask |= nsmask;     /* Get the base bits regardless of facing direction. */
         if (allmask == 0xAAAA) {
            tandstuff.single_mask &= 0x7777;
            if (ewmask == 0x0A0A) {
               ewmask |= 0x8888;
               nsmask = 0;
            }
            else if (ewmask == 0xA0A0) {
               ewmask |= 0x8888;
               nsmask = ewmask;
               ewmask = 0;
            }
            else
               fail("Can't find these triangles.");
         }
         else if (allmask == 0x5555) {
            tandstuff.single_mask &= 0xBBBB;
            if (ewmask == 0x0505) {
               ewmask |= 0x4444;
               nsmask = ewmask;
               ewmask = 0;
            }
            else if (ewmask == 0x5050) {
               ewmask |= 0x4444;
               nsmask = 0;
            }
            else
               fail("Can't find these triangles.");
         }
         else
            fail("Can't find these triangles.");
      }
      else if (key == tandem_key_3x1tgls) {
         if (ss->kind == s2x6 || ss->kind == s4x6 || ss->kind == s2x12) {
            ewmask = 0;
            nsmask = allmask;
         }
         else {
            ewmask = allmask;
            nsmask = 0;
         }
      }
      else if (key == tandem_key_ys) {
         if (ss->kind == s_c1phan) {
            if (!((ss->people[0].id1 ^ ss->people[1].id1 ^ ss->people[4].id1 ^ ss->people[5].id1) & 2)) {
               ss->rotation++;
               canonicalize_rotation(ss);
               ss->rotation--;   /* Since we won't look at the ss->rotation again until the end,
                                    this will have the effect of rotating it back.
                                    Note that allmask, if it is acceptable at all, is still correct
                                    for the new rotation. */
            }
            ewmask = 0;
            nsmask = allmask;
         }
         else {
            ewmask = allmask;
            nsmask = 0;
         }
      }
      else if (ss->kind == s_galaxy) {
         if (special_mask == 0x44) {
            ewmask = allmask;
            nsmask = 0;
         }
         else if (special_mask == 0x11) {
            ewmask = 0;
            nsmask = allmask;
         }
         else
            fail("Can't find these triangles.");
      }
      else if (special_mask == 0x44 || special_mask == 0x1212 || special_mask == 0x22 || special_mask == 0x11) {
         ewmask = 0;
         nsmask = allmask;
      }
      else if (special_mask == 0x2121) {
         ewmask = allmask;
         nsmask = 0;
      }
      else if (ss->kind == s_c1phan)
         fail("Sorry, don't know who is lateral.");
      else {
         ewmask = allmask;
         nsmask = 0;
      }
   }
   else if (key & 2) {

      // Siamese.  Usually, we will insist on a true siamese separation.
      // But, if we fail, it may be because we are doing things "random
      // siamese" or whatever.  In that case, we will allow a pure
      // couples or tandem separation.

      doing_siamese = TRUE;
      saveew = ewmask;
      savens = nsmask;

      if (key == tandem_key_siam3)
         ptr = siamese_table_of_3;
      else if (key == tandem_key_siam4)
         ptr = siamese_table_of_4;
      else
         ptr = siamese_table_of_2;

      for (; ptr->testkind != nothing; ptr++) {
         if (ptr->testkind == ss->kind &&
             ((((ewmask << 16) | nsmask) ^ ptr->testval) & ((allmask << 16) | allmask)) == 0) {
            siamese_warning = ptr->warning;
            siamese_fixup = ptr->fixup & 0xFFFFFF;
            goto try_siamese;   // We seem to have a match.  However, it still might be wrong.
         }
      }

      goto siamese_failed;
   }
   else if (key & 1) {
      /* Couples -- swap masks.  Tandem -- do nothing. */
      uint32 temp(ewmask);
      ewmask = nsmask;
      nsmask = temp;
   }

 try_this:

   /* Now ewmask and nsmask have the info about who is paired with whom. */
   ewmask &= ~tandstuff.single_mask;         /* Clear out unpaired people. */
   nsmask &= ~tandstuff.single_mask;

   map_search = our_map_table;
   while (map_search->outsetup != nothing) {
      if ((map_search->outsetup == ss->kind) &&
            (map_search->outsinglemask == tandstuff.single_mask) &&
            (!(allmask & map_search->outunusedmask)) &&
            (!(ewmask & (~map_search->olatmask))) &&
            (!(nsmask & map_search->olatmask))) {
         map = map_search;
         goto fooy;
      }
      map_search++;
   }

   if (!doing_siamese)
      fail("Can't do this tandem or couples call in this setup or with these people selected.");

   goto siamese_failed;

 try_siamese:

   ewmask ^= (siamese_fixup & allmask);
   nsmask ^= (siamese_fixup & allmask);
   if (ptr->fixup & 0x80000000UL) tandstuff.phantom_pairing_ok = TRUE;
   goto try_this;

 fooy:

   /* We also use the subtle aspects of the phantom indicator to tell what kind
      of setup we allow, and whether pairings must be parallel to the long axis. */

   if (phantom == 1) {
      if (ss->kind != s2x8 && ss->kind != s4x4 && ss->kind != s3x4 && ss->kind != s2x6 &&
          ss->kind != s4x6 && ss->kind != s1x12 && ss->kind != s2x12 && ss->kind != s1x16 &&
          ss->kind != s3dmd && ss->kind != s_323 &&
          ss->kind != s4dmd && ss->kind != s3x8)
         fail("Can't do couples or tandem concepts in this setup.");
   }
   else if (phantom >= 2) {
      if (ss->kind != s2x8 || map->insetup != s2x4)
         fail("Can't do gruesome concept in this setup.");
   }

   tandstuff.np = np;
   tandstuff.virtual_setup.cmd = ss->cmd;
   tandstuff.virtual_setup.cmd.cmd_assume.assumption = cr_none;

   /* There are a small number of assumptions that we can transform. */

   if (ss->cmd.cmd_assume.assump_col == 4) {
      if (ss->cmd.cmd_assume.assumption == cr_ijright)
         tandstuff.virtual_setup.cmd.cmd_assume.assumption = cr_jright;
      else if (ss->cmd.cmd_assume.assumption == cr_ijleft)
         tandstuff.virtual_setup.cmd.cmd_assume.assumption = cr_jleft;
   }

   if (ss->cmd.cmd_assume.assumption == cr_jright || ss->cmd.cmd_assume.assumption == cr_jleft)
      fail("Couples or tandem concept is inconsistent with phantom facing direction.");

   if (key == 1) {
      if (ss->cmd.cmd_assume.assumption == cr_wave_only)
         fail("Couples or tandem concept is inconsistent with phantom facing direction.");
      else if (ss->cmd.cmd_assume.assumption == cr_li_lo) {
         if (ss->kind == s2x2 || ss->kind == s2x4)
            tandstuff.virtual_setup.cmd.cmd_assume.assumption = ss->cmd.cmd_assume.assumption;
      }
      else if (ss->cmd.cmd_assume.assumption == cr_1fl_only) {
         if (ss->kind == s1x4 || ss->kind == s2x4)
            tandstuff.virtual_setup.cmd.cmd_assume.assumption = cr_couples_only;
      }
      else if (ss->cmd.cmd_assume.assumption == cr_2fl_only) {
         if (ss->kind == s1x4 || ss->kind == s2x4)
            tandstuff.virtual_setup.cmd.cmd_assume.assumption = cr_wave_only;
      }
   }

   /*
   tandstuff.virtual_setup.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
   */
   if (phantom == 3) tandstuff.virtual_setup.cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

   if (!pack_us(ss->people, map, fraction, twosome, key, &tandstuff))
      goto got_good_separation;

   // Or failure to pack people properly may just be because we are taking
   // "siamese" too seriously.

   if (!doing_siamese)
      fail("People not facing same way for tandem or as couples.");

 siamese_failed:

   // We allow pointless siamese if this is a part of a call that is being done
   // "piecewise" or "random" or whatever.
   if (!(ss->cmd.cmd_frac_flags & CMD_FRAC_BREAKING_UP))
      fail("Can't do Siamese in this setup.");

   nsmask = savens;
   ewmask = saveew;


   if (key & 2) {
      key = (tandem_key) (key & ~2);    // Turn off siamese -- it's now effectively as couples
      key = (tandem_key) (key | 1);
      uint32 temp(ewmask);
      ewmask = nsmask;
      nsmask = temp;
   }
   else if (key & 1)
      key = (tandem_key) (key & ~1);    // Try tandem
   else
      fail("Can't do this tandem or couples call in this setup or with these people selected.");

   goto try_this;

 got_good_separation:

   warn(siamese_warning);
   update_id_bits(&tandstuff.virtual_setup);
   impose_assumption_and_move(&tandstuff.virtual_setup, &tandstuff.virtual_result);

   /* If this is a concentric setup with dead outsides, we can still handle it.
      We have to remember to put dead outsides back when we are done, in order
      to make gluing illegal. */

   if (tandstuff.virtual_result.kind == s_dead_concentric) {
      dead_conc = TRUE;
      tandstuff.virtual_result.kind = tandstuff.virtual_result.inner.skind;
      tandstuff.virtual_result.rotation += tandstuff.virtual_result.inner.srotation;
   }

   if (setup_attrs[tandstuff.virtual_result.kind].setup_limits < 0)
      fail("Don't recognize ending position from this tandem or as couples call.");

   sglmask2 = 0;    /* Bits appear here in pairs!  Both are duplicates. */
   livemask2 = 0;   /* Bits appear here in pairs!  Both are duplicates. */
   orbitmask = 0;   /* Bits appear here in pairs! */

   /* Compute orbitmask, livemask2, and sglmask2.
      Since we are synthesizing bit masks, we scan in reverse order to make things easier. */

   for (i=setup_attrs[tandstuff.virtual_result.kind].setup_limits; i>=0; i--) {
      uint32 p = tandstuff.virtual_result.people[i].id1;
      sglmask2 <<= 2;
      livemask2 <<= 2;
      orbitmask <<= 2;

      if (p) {
         int vpi = (p >> 6) & 7;
         livemask2 |= 3;

         if (tandstuff.real_saved_people[1][vpi].id1 == ~0UL) {
            sglmask2 |= 3;
         }
         else {
            uint32 orbit;

            if (fractional) {
               if (!(p & STABLE_ENAB))
                  fail("fractional twosome not supported for this call.");
            }

            orbit = p + tandstuff.virtual_result.rotation - tandstuff.saved_originals[vpi];

            if (twosome == 2) {
               orbit -= ((p & (STABLE_VBIT*3)) / STABLE_VBIT);
            }
            else if (twosome == 3) {
               orbit = ((p & (STABLE_VBIT*3)) / STABLE_VBIT);
            }
            else if (twosome == 1) {
               orbit = 0;
            }

            orbitmask |= ((orbit - tandstuff.virtual_result.rotation - tandstuff.vertical_people[vpi]) & 3);
         }

         if (fractional)
            tandstuff.virtual_result.people[i].id1 &= ~STABLE_MASK;
      }
   }

   sglmask = sglmask2 & 0x55555555UL;     /* Bits appear here in pairs!  Only low bit of each pair is used. */
   livemask = livemask2 & 0x55555555UL;   /* Bits appear here in pairs!  Only low bit of each pair is used. */
   hmask2 = ~orbitmask & livemask2 & ~sglmask2;

   /* Pick out only low bits for map search, and only bits of live paired people. */
   hmask = hmask2 & 0x55555555UL;
   if (tandstuff.no_unit_symmetry) {
      hmask = hmask2;
      livemask = livemask2;
   }

   map_search = our_map_table;
   while (map_search->outsetup != nothing) {
      if ((map_search->insetup == tandstuff.virtual_result.kind) &&
            (map_search->insinglemask == sglmask) &&
            ((map_search->ilatmask & livemask) == hmask)) {
         unpack_us(map_search, orbitmask, &tandstuff, result);
         reinstate_rotation(ss, result);

         if (dead_conc) {
            result->inner.skind = result->kind;
            result->inner.srotation = result->rotation;
            result->rotation = 0;
            result->kind = s_dead_concentric;
         }

         return;
      }
      map_search++;
   }

   fail("Don't recognize ending position from this tandem or as couples call.");
}
