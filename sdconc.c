/* SD -- square dance caller's helper.

    Copyright (C) 1990-1995  William B. Ackerman.

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

    This is for version 31. */

/* This defines the following functions:
   concentric_move
   get_multiple_parallel_resultflags
   normalize_concentric
   merge_setups
   on_your_own_move
   punt_centers_use_concept
   so_and_so_only_move
*/

#include "sd.h"

/*                                                                                                          mapelong --------|
                                                                                                           outer_rot -----|  |
                                                                                                    inner_rot ---------|  |  |
                                                              outlimit -----|                          bigsize ----|   |  |  |
                                   mapin           mapout       inlimit -|  |  bigsetup      insetup  outsetup     |   |  |  |   */

Private cm_thing map_3line =  {{0, 1, 3, 2}, {10, 11, 4, 5},             4, 4, s3x4,           s1x4,     s1x4,     12, 0, 0, 1};

Private cm_thing map1x4_2x4 =        {{10, 11, 4, 5},
                                             {0, 1, 2, 3, 6, 7, 8, 9},   4, 8, s3x4,           s1x4,     s2x4,     12, 0, 0, 0};
Private cm_thing oddmap1x4_2x4 =     {{8, 9, 2, 3},
                                             {0, 1, 4, 5, 6, 7, 10, 11}, 4, 8, sbigdmd,        s1x4,     s2x4,     12, 1, 0, 0};
Private cm_thing map1x2_1x2 =        {{1, 3},       {0, 2},              2, 2, s1x4,           s1x2,     s1x2,     4,  0, 0, 0};
Private cm_thing oddmap1x2_1x2 =     {{3, 1},       {0, 2},              2, 2, sdmd,           s1x2,     s1x2,     4,  1, 0, 0};
/* The map "oddmapdmd_dmd", with its loss of elongation information for the outer diamond, is necessary to make the call
   "with confidence" work from the setup formed by having the centers partner tag in left-hand waves.  This means that certain
   Bakerisms involving concentric diamonds, in which each diamond must remember its own elongation, are not possible.  Too bad. */
Private cm_thing oddmapdmd_dmd =     {{1, 3, 5, 7}, {6, 0, 2, 4},        4, 4, s_crosswave,    sdmd,     sdmd,     8,  0, 1, 9};
Private cm_thing mapdmd_dmd =        {{1, 3, 5, 7}, {0, 2, 4, 6},        4, 4, s_crosswave,    sdmd,     sdmd,     8,  0, 0, 9};
Private cm_thing oddmapdmd_1x4 =     {{1, 2, 5, 6}, {7, 0, 3, 4},        4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 1, 9};
Private cm_thing mapdmd_1x4 =        {{1, 2, 5, 6}, {0, 3, 4, 7},        4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 0, 9};
Private cm_thing oddmap_s_dmd_1x4 =  {{1, 2, 5, 6}, {7, 0, 3, 4},        4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 1, 9};
Private cm_thing map_s_dmd_1x4 =     {{1, 2, 5, 6}, {0, 3, 4, 7},        4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 0, 9};
Private cm_thing map_cs_1x4_dmd =    {{0, 3, 4, 7}, {1, 2, 5, 6},        4, 4, s_wingedstar,   sdmd,     s1x4,     8,  0, 0, 9};
Private cm_thing oddmap_s_star_1x4 = {{1, 2, 5, 6}, {7, 0, 3, 4},        4, 4, s_wingedstar,   s1x4,     s_star,   8,  0, 1, 9};
Private cm_thing map_s_star_1x4 =    {{1, 2, 5, 6}, {0, 3, 4, 7},        4, 4, s_wingedstar,   s1x4,     s_star,   8,  0, 0, 9};
Private cm_thing oddmap_s_short_1x6 = {{1, 2, 4, 7, 8, 10},
                                              {11, 0, 3, 5, 6, 9},       6, 6, s_wingedstar12, s1x6,     s_short6, 12, 0, 1, 9};
Private cm_thing map_s_spindle_1x8 = {{1, 2, 6, 5, 9, 10, 14, 13},
                                      {3, 7, 12, 8, 11, 15, 4, 0},       8, 8, s_wingedstar16, s1x8,    s_spindle, 16, 0, 0, 9};
Private cm_thing map_spec_star12 =   {{2, 3, 4, 11}, {0, 1, 6, 7},       4, 4, s_wingedstar12, s_star,   s1x4,     12, 0, 0, 0};
Private cm_thing map_spec_star12v =  {{11, 2, 3, 4}, {0, 1, 6, 7},       4, 4, s_wingedstar12, s_star,   s1x4,     12, 1, 0, 0};
Private cm_thing map_spec_star16 =   {{2, 3, 5, 4}, {0, 1, 8, 9},        4, 4, s_wingedstar16, s_star,   s1x4,     16, 0, 0, 0};
Private cm_thing map_spec_star16v =  {{4, 2, 3, 5}, {0, 1, 8, 9},        4, 4, s_wingedstar16, s_star,   s1x4,     16, 1, 0, 0};
Private cm_thing mapdmd_2x2h =       {{1, 3, 5, 7}, {0, 2, 4, 6},        4, 4, s_galaxy,       s2x2,     sdmd,     8,  0, 0, 9};
Private cm_thing mapdmd_2x2v =       {{7, 1, 3, 5}, {0, 2, 4, 6},        4, 4, s_galaxy,       s2x2,     sdmd,     8,  1, 0, 9};
Private cm_thing map2x3_1x2 =        {{11, 5}, {0, 1, 2, 6, 7, 8},       2, 6, s3dmd,          s1x2,     s2x3,     12, 0, 0, 9};
Private cm_thing map1x4_1x4 =        {{3, 2, 7, 6}, {0, 1, 4, 5},        4, 4, s1x8,           s1x4,     s1x4,     8,  0, 0, 0};
Private cm_thing oddmap1x4_1x4 =     {{6, 7, 2, 3}, {0, 1, 4, 5},        4, 4, s_crosswave,    s1x4,     s1x4,     8,  1, 0, 0};
Private cm_thing map2x3_2x3 = {{8, 11, 1, 2, 5, 7}, {9, 10, 0, 3, 4, 6}, 6, 6, s3x4,           s2x3,     s2x3,     12, 1, 1, 1};
Private cm_thing map2x4_2x4 = {{10, 15, 3, 1, 2, 7, 11, 9}, {12, 13, 14, 0, 4, 5, 6, 8}, 8, 8, s4x4, s2x4, s2x4,   16, 0, 0, 1};
Private cm_thing map2x4_2x4v = {{6, 11, 15, 13, 14, 3, 7, 5}, {8, 9, 10, 12, 0, 1, 2, 4}, 8, 8, s4x4, s2x4, s2x4,  16, 1, 1, 1};
Private cm_thing map1x2_bone6 = {{1, 7, 6, 5, 3, 2}, {0, 4},             6, 2, s_ptpd,         s_bone6,  s1x2,     8,  0, 0, 0};
Private cm_thing map1x6_1x2 = {{2, 6}, {0, 1, 3, 4, 5, 7},               2, 6, s1x8,           s1x2,     s1x6,     8,  0, 0, 0};
Private cm_thing mapbone6_1x2 = {{7, 3}, {0, 1, 2, 4, 5, 6},             2, 6, s_bone,         s1x2,     s_bone6,  8,  0, 0, 0};
Private cm_thing map1x4_1x4_rc = {{0, 2, 4, 6}, {1, 3, 5, 7},            4, 4, s1x8,           s1x4,     s1x4,     8,  0, 0, 0};
Private cm_thing map1x4_dmd_rc = {{0, 3, 4, 7}, {1, 2, 5, 6},            4, 4, s1x3dmd,        sdmd,     s1x4,     8,  0, 0, 0};
Private cm_thing map1x2_bone6_rc = {{0, 1, 3, 4, 5, 7}, {6, 2},          6, 2, s_bone,         s_bone6,  s1x2,     8,  0, 0, 0};
Private cm_thing map1x2_2x3_rc =   {{0, 3, 1, 4, 7, 5}, {6, 2},          6, 2, s_dhrglass,     s2x3,     s1x2,     8,  0, 0, 0};
Private cm_thing map2x2_dmd_rc = {{7, 1, 3, 5}, {0, 2, 4, 6},            4, 4, s_spindle,      sdmd,     s2x2,     8,  0, 0, 0};
Private cm_thing oddmap2x2_dmd_rc = {{7, 1, 3, 5}, {6, 0, 2, 4},         4, 4, s_spindle,      sdmd,     s2x2,     8,  0, 1, 0};
Private cm_thing map2x2_1x4_rc = {{0, 2, 4, 6}, {1, 7, 5, 3},            4, 4, s_ptpd,         s1x4,     s2x2,     8,  0, 0, 0};
Private cm_thing oddmap2x2_1x4_rc = {{0, 2, 4, 6}, {3, 1, 7, 5},         4, 4, s_ptpd,         s1x4,     s2x2,     8,  0, 1, 9};
Private cm_thing map1x2_2x3 = {{0, 1, 2, 4, 5, 6}, {7, 3},               6, 2, s_spindle,      s2x3,     s1x2,     8,  0, 0, 0};
Private cm_thing map1x2_short6 = {{1, 2, 3, 5, 6, 7}, {0, 4},            6, 2, s_galaxy,       s_short6, s1x2,     8,  0, 0, 0};
Private cm_thing map1x2_intgl = {{0, 1, 3, 4, 5, 7}, {6, 2},             6, 2, s_qtag,         s_bone6,  s1x2,     8,  0, 0, 0};
Private cm_thing mapshort6_1x2h = {{5, 1}, {6, 7, 0, 2, 3, 4},           2, 6, s_spindle,      s1x2,     s_short6, 8,  1, 1, 1};
Private cm_thing mapshort6_1x2v = {{7, 3}, {5, 6, 0, 1, 2, 4},           2, 6, s_hrglass,      s1x2,     s_short6, 8,  1, 1, 0};
Private cm_thing mapbone6_1x2v =  {{7, 3}, {0, 1, 2, 4, 5, 6},           2, 6, s_dhrglass,     s1x2,     s_bone6,  8,  1, 0, 0};
Private cm_thing mapstar_2x2 = {{1, 3, 5, 7}, {0, 2, 4, 6},              4, 4, s_galaxy,       s2x2,     s_star,   8,  0, 0, 0};
Private cm_thing mapstar_star = {{1, 3, 5, 7}, {0, 2, 4, 6},             4, 4, s_thar,         s_star,   s_star,   8,  0, 0, 0};
Private cm_thing map1x2_1x6 = {{1, 3, 2, 5, 7, 6}, {0, 4},               6, 2, s1x8,           s1x6,     s1x2,     8,  0, 0, 0};
Private cm_thing oddmap1x2_1x6 = {{0, 1, 2, 4, 5, 6}, {7, 3},            6, 2, s3x1dmd,        s1x6,     s1x2,     12, 0, 1, 0};
Private cm_thing map1x4_2x2 = {{0, 1, 4, 5}, {6, 7, 2, 3},               4, 4, s_rigger,       s2x2,     s1x4,     8,  0, 0, 0};
Private cm_thing oddmap1x4_2x2 = {{5, 0, 1, 4}, {6, 7, 2, 3},            4, 4, s_rigger,       s2x2,     s1x4,     8,  1, 0, 0};
Private cm_thing map1x4_star = {{2, 3, 6, 7}, {0, 1, 4, 5},              4, 4, s_wingedstar,   s_star,   s1x4,     8,  0, 0, 0};
Private cm_thing oddmap1x4_star = {{7, 2, 3, 6}, {0, 1, 4, 5},           4, 4, s_wingedstar,   s_star,   s1x4,     8,  1, 0, 0};
Private cm_thing map2x2_dmd = {{6, 3, 2, 7}, {0, 1, 4, 5},               4, 4, s_hrglass,      sdmd,     s2x2,     8,  0, 0, 1};
Private cm_thing map2x2_dmdv = {{6, 3, 2, 7}, {0, 1, 4, 5},              4, 4, s_dhrglass,     sdmd,     s2x2,     8,  0, 0, 0};
Private cm_thing oddmap2x2_dmd = {{6, 3, 2, 7}, {5, 0, 1, 4},            4, 4, s_hrglass,      sdmd,     s2x2,     8,  0, 1, 9};
Private cm_thing oddmap2x2_dmdv = {{6, 3, 2, 7}, {5, 0, 1, 4},           4, 4, s_dhrglass,     sdmd,     s2x2,     8,  0, 1, 9};
Private cm_thing map2x2_1x4h = {{6, 7, 2, 3}, {0, 1, 4, 5},              4, 4, s_qtag,         s1x4,     s2x2,     8,  0, 0, 1};
Private cm_thing oddmap2x2_1x4h = {{6, 7, 2, 3}, {5, 0, 1, 4},           4, 4, s_bone,         s1x4,     s2x2,     8,  0, 1, 9};
Private cm_thing mapstar_1x4 =    {{1, 2, 5, 6}, {0, 3, 4, 7},           4, 4, s3x1dmd,        s1x4,     s_star,   12, 0, 0, 9};
Private cm_thing oddmapstar_1x4 = {{1, 2, 5, 6}, {7, 0, 3, 4},           4, 4, s3x1dmd,        s1x4,     s_star,   12, 0, 1, 9};
Private cm_thing mapstar_dmd ={{1, 3, 5, 7}, {0, 2, 4, 6},               4, 4, s_crosswave,    sdmd,     s_star,   8,  0, 0, 9};
Private cm_thing oddmapstar_dmd =  {{1, 3, 5, 7}, {6, 0, 2, 4},          4, 4, s_crosswave,    sdmd,     s_star,   8,  0, 1, 9};
Private cm_thing map2x2_1x4v = {{6, 7, 2, 3}, {0, 1, 4, 5},              4, 4, s_bone,         s1x4,     s2x2,     8,  0, 0, 0};
Private cm_thing oddmap2x2_1x4v = {{6, 7, 2, 3}, {5, 0, 1, 4},           4, 4, s_qtag,         s1x4,     s2x2,     8,  0, 1, 9};
Private cm_thing map2x2_2x2v = {{1, 2, 5, 6}, {0, 3, 4, 7},              4, 4, s2x4,           s2x2,     s2x2,     8,  0, 0, 0};
Private cm_thing oddmap2x2_2x2h = {{1, 2, 5, 6}, {7, 0, 3, 4},           4, 4, s2x4,           s2x2,     s2x2,     8,  0, 1, 9};
Private cm_thing oddmap2x2_2x2v = {{6, 1, 2, 5}, {0, 3, 4, 7},           4, 4, s2x4,           s2x2,     s2x2,     8,  1, 0, 9};
Private cm_thing map2x2_2x2h = {{6, 1, 2, 5}, {7, 0, 3, 4},              4, 4, s2x4,           s2x2,     s2x2,     8,  1, 1, 9};
Private cm_thing maplatgal = {{7, 0, 1, 3, 4, 5}, {6, 2},                6, 2, s_galaxy,       s_short6, s1x2,     8,  1, 1, 0};
Private cm_thing map1x4_dmd = {{2, 3, 6, 7}, {0, 1, 4, 5},               4, 4, s1x3dmd,        sdmd,     s1x4,     8,  0, 0, 0};
Private cm_thing oddmap1x4_dmd = {{7, 2, 3, 6}, {0, 1, 4, 5},            4, 4, s3x1dmd,        sdmd,     s1x4,     8,  1, 0, 0};
Private cm_thing oddmap1x6_1x2 = {{3, 7}, {0, 1, 2, 4, 5, 6},            2, 6, s1x3dmd,        s1x2,     s1x6,     8,  1, 0, 0};
Private cm_thing map1x2_minrig = {{1, 2, 3, 5, 6, 7}, {0, 4},            6, 2, s1x3dmd,        sminirigger,s1x2,   8,  0, 0, 0};
Private cm_thing mapminrig_1x2 = {{2, 6}, {0, 1, 3, 4, 5, 7},            2, 6, s3x1dmd,        s1x2,   sminirigger,8,  0, 0, 0};
Private cm_thing oddmapminrig_1x2 = {{7, 3}, {0, 1, 2, 4, 5, 6},         2, 6, s_crosswave,    s1x2,   sminirigger,8,  1, 0, 0};
Private cm_thing oddmap1x2_bone6 = {{5, 0, 3, 1, 4, 7}, {6, 2},          6, 2, s_hrglass,      s_bone6,  s1x2,     8,  1, 0, 0};
Private cm_thing oddmap1x2_2x3 = {{5, 7, 0, 1, 3, 4}, {6, 2},            6, 2, s_qtag,         s2x3,     s1x2,     8,  1, 0, 0};
Private cm_thing oddmap1x2_short6 = {{5, 7, 0, 1, 3, 4}, {6, 2},         6, 2, s_rigger,       s_short6, s1x2,     8,  1, 0, 0};
Private cm_thing oddmap2x3_1x2 = {{11, 5}, {9, 10, 0, 3, 4, 6},          2, 6, s3x4,           s1x2,     s2x3,     12, 0, 1, 0};
Private cm_thing oddmapshort6_1x2h = {{2, 6}, {3, 0, 1, 7, 4, 5},        2, 6, s_ptpd,         s1x2,     s_short6, 8,  0, 1, 1};
Private cm_thing oddmapshort6_1x2v = {{7, 3}, {5, 6, 0, 1, 2, 4},        2, 6, s_qtag,         s1x2,     s_short6, 8,  0, 1, 0};
Private cm_thing oddmapbigdmd_1x2v = {{9, 3},
                          {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},        2,12, sbigdmd,        s1x2,     sbigdmd, 12,  1, 0, 1};
Private cm_thing oddmap1x2_short6_rc = {{5, 6, 0, 1, 2, 4}, {7, 3},      6, 2, s_rigger,       s_short6, s1x2,     8,  1, 0, 0};
Private cm_thing map2x4_2x2 = {{2, 3, 8, 9},
                                        {0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x6,           s2x2,     s2x4,     12, 0, 0, 9};
Private cm_thing oddmap2x4_2x2 = {{9, 2, 3, 8},
                                        {0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x6,           s2x2,     s2x4,     12, 1, 0, 9};
Private cm_thing map2x4_2x2v = {{15, 3, 7, 11},
                                        {12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s4x4,           s2x2,     s2x4,     16, 0, 0, 9};
Private cm_thing oddmap2x4_2x2v = {{11, 15, 3, 7},
                                        {12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s4x4,           s2x2,     s2x4,     16, 1, 0, 9};
Private cm_thing map1x3dmd_line = {{1, 2, 5, 6}, {0, 3, 4, 7},           4, 4, s1x3dmd,        s1x4,     sdmd,     8,  0, 0, 0};
Private cm_thing mapdmd_line = {{1, 2, 5, 6}, {0, 3, 4, 7},              4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 0, 0};
/* This one allows "finish" when the center "line" is actually a diamond whose centers are empty.
   This can happen because of the preference given for lines over diamonds at the conclusion of certain calls. */
Private cm_thing mappts_line = {{1, -1, 5, -1}, {0, 2, 4, 6},            4, 4, s_crosswave,    s1x4,     sdmd,     8,  0, 0, 0};
Private cm_thing map_s_dmd_line = {{1, 2, 5, 6}, {0, 3, 4, 7},           4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 0, 0};

Private cm_thing *concmap1x2_1x2[4]       = {&map1x2_1x2,          &oddmap1x2_1x2,       &map1x2_1x2,      &oddmap1x2_1x2};
Private cm_thing *concmapdmd_dmd[4]       = {&mapdmd_dmd,          &oddmapdmd_dmd,       &mapdmd_dmd,      &oddmapdmd_dmd};
Private cm_thing *concmapdmd_1x4[4]       = {&mapdmd_1x4,          &oddmapdmd_1x4,       &mapdmd_1x4,      &oddmapdmd_1x4};
Private cm_thing *concmap_s_dmd_1x4[4]    = {&map_s_dmd_1x4,       &oddmap_s_dmd_1x4,    &map_s_dmd_1x4,   &oddmap_s_dmd_1x4};
Private cm_thing *concmap_cs_1x4_dmd[4]   = {&map_cs_1x4_dmd,      0,                    &map_cs_1x4_dmd,  0};
Private cm_thing *concmap_s_star_1x4[4]   = {&map_s_star_1x4,      &oddmap_s_star_1x4,   &map_s_star_1x4,  &oddmap_s_star_1x4};
Private cm_thing *concmap_s_short_1x6[4]  = {0,                    &oddmap_s_short_1x6,  0,                0};
Private cm_thing *concmap_s_spindle_1x8[4] = {0,                   0,                    &map_s_spindle_1x8, 0};
Private cm_thing *concmapdmd_2x2[4]       = {&mapdmd_2x2h,         &mapdmd_2x2v,         &mapdmd_2x2h,     &mapdmd_2x2v};
Private cm_thing *concmap2x2_2x2[4]       = {&map2x2_2x2v,         &oddmap2x2_2x2v,      &map2x2_2x2h,     &oddmap2x2_2x2h};
Private cm_thing *concmap2x2_1x4[4]       = {&map2x2_1x4v,         &oddmap2x2_1x4v,      &map2x2_1x4h,     &oddmap2x2_1x4h};
Private cm_thing *concmap2x2_dmd[4]       = {&map2x2_dmdv,         &oddmap2x2_dmd,       &map2x2_dmd,      &oddmap2x2_dmdv};
Private cm_thing *concmapshort6_1x2[4]    = {&mapshort6_1x2v,      &oddmapshort6_1x2v,   &mapshort6_1x2h,  &oddmapshort6_1x2h};
Private cm_thing *concmapbigdmd_1x2[4]    = {0,                    &oddmapbigdmd_1x2v,   0,                0};
Private cm_thing *concmapminrig_1x2[4]    = {&mapminrig_1x2,       &oddmapminrig_1x2,    &mapminrig_1x2,   &oddmapminrig_1x2};
Private cm_thing *concmap1x2_1x6[4]       = {&map1x2_1x6,          &oddmap1x2_1x6,       &map1x2_1x6,      &oddmap1x2_1x6};
Private cm_thing *concmap1x2_2x3[4]       = {&map1x2_2x3,          &oddmap1x2_2x3,       &map1x2_2x3,      &oddmap1x2_2x3};
Private cm_thing *concmap1x2_short6[4]    = {&map1x2_short6,       &oddmap1x2_short6,    &map1x2_short6,   &oddmap1x2_short6};
Private cm_thing *concmap2x4_2x2[4]       = {&map2x4_2x2,          &oddmap2x4_2x2,       &map2x4_2x2v,     &oddmap2x4_2x2v};
Private cm_thing *concmap2x4_2x4[4]       = {&map2x4_2x4,          0,                    &map2x4_2x4,      0};
Private cm_thing *concmap1x4_2x2[4]       = {&map1x4_2x2,          &oddmap1x4_2x2,       &map1x4_2x2,      &oddmap1x4_2x2};
Private cm_thing *concmap1x4_star[4]      = {&map1x4_star,         &oddmap1x4_star,      &map1x4_star,     &oddmap1x4_star};
Private cm_thing *concmap1x4_dmd[4]       = {&map1x4_dmd,          &oddmap1x4_dmd,       &map1x4_dmd,      &oddmap1x4_dmd};
Private cm_thing *concmap1x4_1x4[4]       = {&map1x4_1x4,          &oddmap1x4_1x4,       &map1x4_1x4,      &oddmap1x4_1x4};
Private cm_thing *concmapstar_2x2[4]      = {&mapstar_2x2,         0,                    &mapstar_2x2,     0};
Private cm_thing *concmapstar_star[4]     = {&mapstar_star,        0,                    &mapstar_star,    0};
Private cm_thing *concmapstar_1x4[4]      = {&mapstar_1x4,         &oddmapstar_1x4,      &mapstar_1x4,     &oddmapstar_1x4};
Private cm_thing *concmapstar_dmd[4]      = {&mapstar_dmd,         &oddmapstar_dmd,      &mapstar_dmd,     &oddmapstar_dmd};
Private cm_thing *concmap2x3_1x2[4]       = {&map2x3_1x2,          &oddmap2x3_1x2,       &map2x3_1x2,      &oddmap2x3_1x2};
Private cm_thing *concmap2x3_2x3[4]       = {&map2x3_2x3,          0,                    &map2x3_2x3,      0};
Private cm_thing *concmapbone6_1x2[4]     = {&mapbone6_1x2,        &mapbone6_1x2v,       &mapbone6_1x2,    &mapbone6_1x2v};
Private cm_thing *concmap1x6_1x2[4]       = {&map1x6_1x2,          &oddmap1x6_1x2,       &map1x6_1x2,      &oddmap1x6_1x2};
Private cm_thing *concmap1x4_1x4_rc[4]    = {&map1x4_1x4_rc,       0,                    &map1x4_1x4_rc,   0};
Private cm_thing *concmap1x4_dmd_rc[4]    = {&map1x4_dmd_rc,       0,                    &map1x4_dmd_rc,   0};
Private cm_thing *concmap2x2_dmd_rc[4]    = {&map2x2_dmd_rc,       &oddmap2x2_dmd_rc,    &map2x2_dmd_rc,   &oddmap2x2_dmd_rc};
Private cm_thing *concmap2x2_1x4_rc[4]    = {&map2x2_1x4_rc,       &oddmap2x2_1x4_rc,    &map2x2_1x4_rc,   &oddmap2x2_1x4_rc};
Private cm_thing *concmap1x2_bone6[4]     = {&map1x2_bone6,        &oddmap1x2_bone6,     &map1x2_bone6,    &oddmap1x2_bone6};
/* was                                                                                    map1x2_intgl  */


Private cm_thing *concmap_1x4_2x4[4]      = {0,                    &oddmap1x4_2x4,       &map1x4_2x4,      0};



extern uint32 get_multiple_parallel_resultflags(setup outer_inners[], int number)
{
   int i;
   uint32 result_flags = 0;

   /* If a call was being done "piecewise" or "random", we demand that both
      calls run out of parts at the same time, and, when that happens, we
      report it to the higher level in the recursion. */

   for (i=0 ; i<number ; i++) {
      if (!(outer_inners[i].result_flags & RESULTFLAG__PARTS_ARE_KNOWN))
         outer_inners[i].result_flags &= ~RESULTFLAG__DID_LAST_PART;

      if (((outer_inners[i].result_flags & outer_inners[0].result_flags) & RESULTFLAG__PARTS_ARE_KNOWN) &&
            ((outer_inners[i].result_flags ^ outer_inners[0].result_flags) & RESULTFLAG__DID_LAST_PART))
         fail("Two calls must use the same number of fractions.");

      result_flags |= outer_inners[i].result_flags;
   }

   return result_flags;
}


/* This overwrites its "outer_inners" argument setups. */
extern void normalize_concentric(
   calldef_schema synthesizer,
   int center_arity,
   setup outer_inners[],   /* outers in position 0, inners follow */
   int outer_elongation,
   setup *result)
{
   /* If "outer_elongation" < 0, the outsides can't deduce their ending spots on
      the basis of the starting formation.  In this case, it is an error unless
      they go to some setup for which their elongation is obvious, like a 1x4. */

   int i, j, q, rot;
   cm_thing **map_ptr;
   cm_thing *lmap_ptr;
   setup *inners = &outer_inners[1];
   setup *outers = &outer_inners[0];

   clear_people(result);
   result->result_flags = get_multiple_parallel_resultflags(outer_inners, center_arity+1);

   if (inners[0].kind == nothing && outers->kind == nothing) {
      result->kind = nothing;
      return;
   }

   compute_rotation_again:

   i = (inners[0].rotation - outers->rotation) & 3;

   map_ptr = 0;

   if (center_arity == 2 && synthesizer != schema_conc_triple_lines) {
      /* Fix up nonexistent stars, in a rather inept way. */
      if (inners[0].kind == nothing) {
         inners[0].kind = s_star;
         inners[0].rotation = 0;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
      }

      if (inners[1].kind == nothing) {
         inners[1].kind = s_star;
         inners[1].rotation = 0;
         inners[1].result_flags = outers->result_flags;
         clear_people(&inners[1]);
      }

      if (synthesizer != schema_conc_star12 || outers->kind != s1x4 || inners[0].kind != s_star || inners[1].kind != s_star)
         fail("Can't do this with 12 matrix stars.");

      if (i&1)
         lmap_ptr = &map_spec_star12v;
      else
         lmap_ptr = &map_spec_star12;

      goto gotit;
   }
   else if (center_arity == 3) {
      /* Fix up nonexistent stars, in a rather inept way. */
      if (inners[0].kind == nothing) {
         inners[0].kind = s_star;
         inners[0].rotation = 0;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
      }

      if (inners[1].kind == nothing) {
         inners[1].kind = s_star;
         inners[1].rotation = 0;
         inners[1].result_flags = outers->result_flags;
         clear_people(&inners[1]);
      }

      if (inners[2].kind == nothing) {
         inners[2].kind = s_star;
         inners[2].rotation = 0;
         inners[2].result_flags = outers->result_flags;
         clear_people(&inners[2]);
      }

      if (synthesizer != schema_conc_star16 || outers->kind != s1x4 || inners[0].kind != s_star || inners[1].kind != s_star || inners[2].kind != s_star)
         fail("Can't do this with 16 matrix stars.");

      if (i&1)
         lmap_ptr = &map_spec_star16v;
      else
         lmap_ptr = &map_spec_star16;

      goto gotit;
   }

   /* Do special stuff to put setups back properly for triple_lines schema. */

   if (synthesizer == schema_conc_triple_lines) {
      setup temp;

      temp = *outers;

      if (inners[0].kind == s2x2) {
         /* Move people down to the closer parts of 2x2 setups. */
         if (!(inners[0].people[2].id1 | inners[0].people[3].id1)) {
            swap_people(&inners[0], 0, 3);
            swap_people(&inners[0], 1, 2);
         }
         if (!(inners[1].people[0].id1 | inners[1].people[1].id1)) {
            swap_people(&inners[1], 0, 3);
            swap_people(&inners[1], 1, 2);
         }

         (void) copy_rot(outers, 0, &inners[0], 1, 033);
         (void) copy_rot(outers, 1, &inners[0], 2, 033);
         (void) copy_rot(outers, 2, &inners[1], 1, 033);
         (void) copy_rot(outers, 3, &inners[1], 2, 033);
         (void) copy_rot(outers, 4, &inners[1], 3, 033);
         (void) copy_rot(outers, 5, &inners[1], 0, 033);
         (void) copy_rot(outers, 6, &inners[0], 3, 033);
         (void) copy_rot(outers, 7, &inners[0], 0, 033);
         outers->rotation = 1;
         outers->kind = s2x4;
      }
      else if (inners[0].kind == s1x4 && inners[0].rotation == 0) {
         (void) copy_person(outers, 0, &inners[0], 0);
         (void) copy_person(outers, 1, &inners[0], 1);
         (void) copy_person(outers, 2, &inners[0], 3);
         (void) copy_person(outers, 3, &inners[0], 2);
         (void) copy_person(outers, 4, &inners[1], 2);
         (void) copy_person(outers, 5, &inners[1], 3);
         (void) copy_person(outers, 6, &inners[1], 1);
         (void) copy_person(outers, 7, &inners[1], 0);
         outers->rotation = 0;
         outers->kind = s2x4;
      }
      else if (inners[0].kind == s1x2 && inners[0].rotation == 0) {
         (void) copy_person(outers, 0, &inners[0], 0);
         (void) copy_person(outers, 1, &inners[0], 1);
         (void) copy_person(outers, 2, &inners[1], 1);
         (void) copy_person(outers, 3, &inners[1], 0);
         outers->rotation = 0;
         outers->kind = s2x2;
      }
      else if (inners[0].kind == nothing) {
         outers->kind = nothing;
      }
      else
         fail("Can't figure out what to do.");

      inners[0] = temp;
      i = (inners[0].rotation - outers->rotation) & 3;
   }


   if (synthesizer == schema_rev_checkpoint) {
      /* Fix up nonexistent centers or ends, in a rather inept way. */
      if (inners[0].kind == nothing) {
         inners[0].kind = outers->kind;
         inners[0].rotation = outers->rotation;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
         i = 0;
      }
      else if (outers->kind == nothing) {
         outers->kind = inners[0].kind;
         outers->rotation = inners[0].rotation;
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         i = 0;
      }

      switch (outers->kind) {
         case s2x2:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap2x2_1x4_rc; break;
               case sdmd: map_ptr = concmap2x2_dmd_rc; break;
            }
            break;
         case s1x4:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap1x4_1x4_rc; break;
               case sdmd: map_ptr = concmap1x4_dmd_rc; break;
            }
            break;
      }
   }
   else if (synthesizer == schema_ckpt_star) {
      /* There are a few cases of centers or ends being phantoms, in which
         we nevertheless know what to do, since we know that the setup should
         be some kind of "winged star". */
      if (inners[0].kind == nothing && outers->kind == s1x4) {
         inners[0].kind = s_star;
         inners[0].rotation = 0;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
         goto compute_rotation_again;
      }
      else if (inners[0].kind == sdmd && outers->kind == nothing) {
         /* The test case for this is: RWV:intlkphanbox relay top;splitphanbox flip reaction. */
         outers->kind = s1x4;
         outers->rotation = inners[0].rotation;
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         goto compute_rotation_again;
      }

      switch (outers->kind) {
         case s1x4:
            switch (inners[0].kind) {
               case sdmd: map_ptr = concmap_cs_1x4_dmd; break;
            }
            break;
      }
   }
   else if (synthesizer == schema_conc_star) {
      /* There are a few cases of centers or ends being phantoms, in which
         we nevertheless know what to do, since we know that the setup should
         be some kind of "winged star". */
      if (outers->kind == nothing && inners[0].kind == s1x4) {
         outers->kind = s_star;
         outers->rotation = 0;
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         goto compute_rotation_again;
      }
      else if (outers->kind == nothing && inners[0].kind == s_star) {
         outers->kind = s1x4;
         outers->rotation = outer_elongation;
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         goto compute_rotation_again;
      }
      else if (outers->kind == s1x4 && inners[0].kind == nothing) {
         inners[0].kind = s_star;
         inners[0].rotation = 0;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
         goto compute_rotation_again;
      }

      switch (outers->kind) {
         case sdmd:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap_s_dmd_1x4; break;
            }
            break;
         case s_star:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap_s_star_1x4; break;
            }
            break;
         case s1x4:
            switch (inners[0].kind) {
               case sdmd:
                  /* Just turn it into a star. */
                  inners[0].kind = s_star;
                  canonicalize_rotation(&inners[0]);  /*Need to do this; it has 4-way symmetry now. */
                  goto compute_rotation_again;
               case s_star: map_ptr = concmap1x4_star; break;
               case s1x4:
                  /* In certain phantom cases, what should have been a diamond
                     around the outside, resulting from a 1/2 circulate, will be
                     a line with the two centers missing, since the basic_move
                     routine gives preference to a line when it is ambiguous.
                     If this happens, we have to turn it back into a diamond. */
                  if (!(outers->people[1].id1 | outers->people[3].id1)) {
                     outers->kind = sdmd;  /* That's all that it takes to fix it. */
                     goto compute_rotation_again;
                  }
                  /* Or the two ends missing. */
                  else if (!(outers->people[0].id1 | outers->people[2].id1)) {
                     outers->kind = sdmd;
                     outers->rotation = (outers->rotation-1) & 3;
                     (void) copy_rot(outers, 1, outers, 1, 011);
                     (void) copy_rot(outers, 3, outers, 3, 011);
                     canonicalize_rotation(outers);
                     goto compute_rotation_again;
                  }
            }
            break;
      }
   }
   else if (synthesizer == schema_conc_star12) {
      switch (outers->kind) {
         case s_short6:
            switch (inners[0].kind) {
               case s1x6: map_ptr = concmap_s_short_1x6; break;
            }
            break;
      }
   }
   else if (synthesizer == schema_conc_star16) {
      switch (outers->kind) {
         case s_spindle:
            switch (inners[0].kind) {
               case s1x8: map_ptr = concmap_s_spindle_1x8; break;
            }
            break;
      }
   }
   else {
      /* Fix up nonexistent centers or ends, in a rather inept way. */
      if (inners[0].kind == nothing) {
         inners[0].kind = outers->kind;
         inners[0].rotation = outers->rotation;
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
         i = 0;
      }
      else if (outers->kind == nothing) {
         outers->kind = inners[0].kind;
         outers->rotation = inners[0].rotation;
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         i = 0;
      }

      /* Nonexistent center or ends have been taken care of.  Now figure out how to put
         the setups together. */

      switch (outers->kind) {
         case s1x6:
            switch (inners[0].kind) {
               case s1x2: map_ptr = concmap1x6_1x2; break;
            }
            break;
         case s_bone6:
            switch (inners[0].kind) {
               case s1x2: map_ptr = concmapbone6_1x2; break;
            }
            break;
         case s2x3:
            switch (inners[0].kind) {
               case s1x2: map_ptr = concmap2x3_1x2; break;
               case s2x3: map_ptr = concmap2x3_2x3; break;
            }
            break;
         case s2x4:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap_1x4_2x4; break;
               case s2x2: map_ptr = concmap2x4_2x2; break;
               case s2x4: map_ptr = concmap2x4_2x4; break;
            }
            break;
         case s1x4:
            switch (inners[0].kind) {
               case s1x4: map_ptr = concmap1x4_1x4; break;
               case sdmd: map_ptr = concmap1x4_dmd; break;
               case s_star: map_ptr = concmap1x4_star; break;
               case s2x2: map_ptr = concmap1x4_2x2; break;
            }
            break;
         case s1x2:
            switch (inners[0].kind) {
               case s1x2:     map_ptr = concmap1x2_1x2; break;
               case s2x3:     map_ptr = concmap1x2_2x3; break;
               case s_bone6:  map_ptr = concmap1x2_bone6; break;
               case s_short6: map_ptr = concmap1x2_short6; break;
               case s1x6:     map_ptr = concmap1x2_1x6; break;
            }
            break;
         case s_short6:
            switch (inners[0].kind) {
               case s1x2: map_ptr = concmapshort6_1x2; break;
            }
            break;
         case sbigdmd:
            switch (inners[0].kind) {
               case s1x2:
                  if (!(outers->people[3].id1 | outers->people[9].id1)) map_ptr = concmapbigdmd_1x2; break;
            }
            break;
         case sminirigger:
            switch (inners[0].kind) {
               case s1x2: map_ptr = concmapminrig_1x2; break;
            }
            break;
         case s2x2:
            switch (inners[0].kind) {
               case s2x2: map_ptr = concmap2x2_2x2; break;
               case s1x4: map_ptr = concmap2x2_1x4; break;
               case sdmd: map_ptr = concmap2x2_dmd; break;
            }
            break;
         case sdmd:
            switch (inners[0].kind) {
               case sdmd: map_ptr = concmapdmd_dmd; break;
               case s1x4: map_ptr = concmapdmd_1x4; break;
               case s2x2: map_ptr = concmapdmd_2x2; break;
            }
            break;
         case s_star:
            switch (inners[0].kind) {
               case sdmd: map_ptr = concmapstar_dmd; break;
               case s1x4: map_ptr = concmapstar_1x4; break;
               case s_star: map_ptr = concmapstar_star; break;
               case s2x2: map_ptr = concmapstar_2x2; break;
            }
            break;
      }
   }

   if (!map_ptr) goto anomalize_it;

   if (outer_elongation < 0) {
      /* We need to find out whether it would have made a difference
         when picking out the map. */

      if (map_ptr[(i&1)] != map_ptr[(i&1) + 2]) goto elongation_loss;
   }

   lmap_ptr = map_ptr[(i&1) + (((outer_elongation ^ outers->rotation) & 1) << 1)];

gotit:

   if (!lmap_ptr) goto anomalize_it;

   result->kind = lmap_ptr->bigsetup;
   result->rotation = outers->rotation + lmap_ptr->outer_rot;

   rot = ((-lmap_ptr->outer_rot) & 3) * 011;
   for (j=0; j<lmap_ptr->outlimit; j++)
      (void) copy_rot(result, lmap_ptr->mapout[j], outers, j, rot);

   /* Find out whether inners need to be flipped around. */
   q = i + lmap_ptr->inner_rot - lmap_ptr->outer_rot;

   if (q & 1)
      fail("Sorry, there is a bug in normalize_concentric.");

   if (q & 2) {
      inners[0].rotation += 2;
      canonicalize_rotation(&inners[0]);
   }

   rot = ((-lmap_ptr->inner_rot) & 3) * 011;
   for (j=0; j<lmap_ptr->inlimit; j++)
      (void) copy_rot(result, lmap_ptr->mapin[j], &inners[0], j, rot);

   if (lmap_ptr == &map_spec_star12) {
      if (q & 2) {
         inners[1].rotation += 2;
         canonicalize_rotation(&inners[1]);
      }
      (void) copy_rot(result, 10, &inners[1], 0, rot);
      (void) copy_rot(result, 5,  &inners[1], 1, rot);
      (void) copy_rot(result, 8,  &inners[1], 2, rot);
      (void) copy_rot(result, 9,  &inners[1], 3, rot);
   }
   else if (lmap_ptr == &map_spec_star12v) {
      if (q & 2) {
         inners[1].rotation += 2;
         canonicalize_rotation(&inners[1]);
      }
      (void) copy_rot(result, 9,  &inners[1], 0, rot);
      (void) copy_rot(result, 10, &inners[1], 1, rot);
      (void) copy_rot(result, 5,  &inners[1], 2, rot);
      (void) copy_rot(result, 8,  &inners[1], 3, rot);
   }
   else if (lmap_ptr == &map_spec_star16) {
      if (q & 2) {
         inners[1].rotation += 2;
         inners[2].rotation += 2;
         canonicalize_rotation(&inners[1]);
         canonicalize_rotation(&inners[2]);
      }
      (void) copy_rot(result, 6,  &inners[1], 0, rot);
      (void) copy_rot(result, 7,  &inners[1], 1, rot);
      (void) copy_rot(result, 14, &inners[1], 2, rot);
      (void) copy_rot(result, 15, &inners[1], 3, rot);
      (void) copy_rot(result, 13, &inners[2], 0, rot);
      (void) copy_rot(result, 12, &inners[2], 1, rot);
      (void) copy_rot(result, 10, &inners[2], 2, rot);
      (void) copy_rot(result, 11, &inners[2], 3, rot);
   }
   else if (lmap_ptr == &map_spec_star16v) {
      if (q & 2) {
         inners[1].rotation += 2;
         inners[2].rotation += 2;
         canonicalize_rotation(&inners[1]);
         canonicalize_rotation(&inners[2]);
      }
      (void) copy_rot(result, 15, &inners[1], 0, rot);
      (void) copy_rot(result, 6,  &inners[1], 1, rot);
      (void) copy_rot(result, 7,  &inners[1], 2, rot);
      (void) copy_rot(result, 14, &inners[1], 3, rot);
      (void) copy_rot(result, 11, &inners[2], 0, rot);
      (void) copy_rot(result, 13, &inners[2], 1, rot);
      (void) copy_rot(result, 12, &inners[2], 2, rot);
      (void) copy_rot(result, 10, &inners[2], 3, rot);
   }

   canonicalize_rotation(result);
   return;

   anomalize_it:            /* Failed, just leave it as it is. */

   switch (synthesizer) {
      case schema_rev_checkpoint:
         fail("Sorry, can't figure out this reverse checkpoint result.");
      case schema_single_concentric:
      case schema_single_cross_concentric:
         fail("Can't figure out this single concentric result.");
      case schema_conc_star:
      case schema_conc_star12:
      case schema_conc_star16:
      case schema_conc_triple_lines:
         fail("Can't figure out this concentric result.");
   }

   if (outer_elongation < 0) goto elongation_loss;

   result->kind = s_normal_concentric;
   result->inner.skind = inners[0].kind;
   result->inner.srotation = inners[0].rotation;
   result->outer.skind = outers->kind;
   result->outer.srotation = outers->rotation;
   result->concsetup_outer_elongation = outer_elongation ^ outers->rotation;
   for (j=0; j<12; j++) {
      (void) copy_person(result, j, &inners[0], j);
      (void) copy_person(result, j+12, outers, j);
   }
   canonicalize_rotation(result);
   result->result_flags = 0;
   return;

   elongation_loss:
   fail("Ends can't figure out what spots to finish on.");
}




/* BEWARE!!  These lists are keyed to the enumeration "analyzer_kind" :

    analyzer_NORMAL
    |       analyzer_CHECKPT
    |       |       analyzer_2X6
    |       |       |       analyzer_6X2
    |       |       |       |       analyzer_6X2_TGL
    |       |       |       |       |       analyzer_STAR12
    |       |       |       |       |       |       analyzer_STAR16
    |       |       |       |       |       |       |       analyzer_SINGLE
    |       |       |       |       |       |       |       |       analyzer_TRIPLE_LINE
    |       |       |       |       |       |       |       |       |       analyzer_VERTICAL6
    |       |       |       |       |       |       |       |       |       |       analyzer_LATERAL6
    |       |       |       |       |       |       |       |       |       |       |       analyzer_CONC_DIAMONDS
    |       |       |       |       |       |       |       |       |       |       |       |       analyzer_DIAMOND_LINE */

cm_hunk concthing_1x4 = {0x5, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      &map1x2_1x2,
                                                                    0,      0,      0,      0,      0}};

cm_hunk concthing_dmd = {0x5, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      &oddmap1x2_1x2,
                                                                    0,      0,      0,      0,      0}};

cm_hunk concthing_qtag = {0x33, 0xDD, 0x11,
   {&map2x2_1x4h,
            0,      &oddmapshort6_1x2v,
                            &oddmap1x2_2x3,
                                     &map1x2_intgl,
                                            0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_bone = {0x33, 0, 0x11,
   {&map2x2_1x4v,
            &map1x2_bone6_rc,
                    &mapbone6_1x2,
                            0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_rigger = {0xCC, 0xDD, 0,
   {&map1x4_2x2,
            &oddmap1x2_short6_rc,
                    0,      &oddmap1x2_short6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_spindle = {0, 0xEE, 0x44,
   {0,      &map2x2_dmd_rc,
                    &mapshort6_1x2h,
                            &map1x2_2x3,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_hrglass = {0x33, 0xDD, 0x11,
   {&map2x2_dmd,
            0,      &mapshort6_1x2v,
                            &oddmap1x2_bone6,
                                    0,      0,      0,      0,      0,      &oddmap1x2_bone6,
                                                                                    0,      0,      0}};

cm_hunk concthing_dhrglass = {0x33, 0, 0x11,
   {&map2x2_dmdv,
            &map1x2_2x3_rc,
                    &mapbone6_1x2v,
                            0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_xwave = {0x33, 0x77, 0x11,
   {&oddmap1x4_1x4,
            0,      &oddmapminrig_1x2,
                            0,      0,      0,      0,      0,      0,      0,      0,      &mapdmd_dmd,
                                                                                                    &mappts_line}};

cm_hunk concthing_1x8 = {0x33, 0x77, 0x22,
   {&map1x4_1x4,
            &map1x4_1x4_rc,
                    &map1x6_1x2,
                            &map1x2_1x6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_2x4 = {0x66, 0, 0,
   {&map2x2_2x2v,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_3x4 = {0, 0, 0x041,
   {0,      0,      &oddmap2x3_1x2,
                            0,      0,      &map2x3_2x3,
                                                    0,      0,      &map_3line,
                                                                            0,      0,      0,      0}};

cm_hunk concthing_4x4 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      &map2x4_2x4v,
                                                            0,      0,      0,      0,      0,      0}};

cm_hunk concthing_ptpd = {0, 0x77, 0x22,
   {0,      &map2x2_1x4_rc,
                    &oddmapshort6_1x2h,
                            &map1x2_bone6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0}};

cm_hunk concthing_1x3dmd = {0x33, 0x77, 0x11,
   {&map1x4_dmd,
            &map1x4_dmd_rc,
                    &oddmap1x6_1x2,
                            &map1x2_minrig,
                                    0,      0,      0,      0,      0,      0,      0,      0,      &map1x3dmd_line}};

cm_hunk concthing_3x1dmd = {0x33, 0, 0,
   {&oddmap1x4_dmd,
            0,      &mapminrig_1x2,
                            &oddmap1x2_1x6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      &mapdmd_line}};

cm_hunk concthing_wstar = {0x33, 0, 0,
   {&map1x4_star,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map_s_dmd_line}};

cm_hunk concthing_wstar12 = {0, 0, 0,
   {0,      0,      0,      0,      0,      &map_spec_star12,
                                                    0,      0,      0,      0,      0,      0,      &oddmap_s_short_1x6}};

cm_hunk concthing_wstar16 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      &map_spec_star16,
                                                            0,      0,      0,      0,      0,      &map_s_spindle_1x8}};

cm_hunk concthing_gal = {0x55, 0, 0,
   {&mapstar_2x2,
            0,      0,      0,      0,      0,      0,      0,      0,      &map1x2_short6,
                                                                                    &maplatgal,
                                                                                            0,      0}};

cm_hunk concthing_thar = {0x55, 0, 0,
   {&mapstar_star,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

/* BEWARE!!  This is keyed to the enumeration "analyzer_kind". */
Private char *conc_error_messages[] = {
   "Can't find centers and ends in this formation.",                   /* analyzer_NORMAL */
   "Can't find checkpoint people in this formation.",                  /* analyzer_CHECKPT */
   "Can't find 2 centers and 6 ends in this formation.",               /* analyzer_2X6 */
   "Can't find 6 centers and 2 ends in this formation.",               /* analyzer_6X2 */
   "Can't find inside triangles in this formation.",                   /* analyzer_6X2_TGL */
   "Can't find 12 matrix centers and ends in this formation.",         /* analyzer_STAR12 */
   "Can't find 16 matrix centers and ends in this formation.",         /* analyzer_STAR16 */
   "Can't find single concentric centers and ends in this formation.", /* analyzer_SINGLE */
   "Can't find triple lines in this formation.",                       /* analyzer_TRIPLE_LINE */
   "Wrong formation.",                                                 /* analyzer_VERTICAL6 */
   "Wrong formation.",                                                 /* analyzer_LATERAL6 */
   "Can't find concentric diamonds.",                                  /* analyzer_CONC_DIAMONDS */
   "Can't find center line and outer diamond."                         /* analyzer_DIAMOND_LINE */
};




/* This sets "outer_elongation" to the absolute elongation of the
   outsides.  If the outsides are in a 2x2, this, along with individual
   facing directions, will permit enforcement of the "lines-to-lines/
   columns-to-columns" rule.  Otherwise, this will permit enforcement
   of the Hodson checkpoint rule.

   There are a few cases in which this result may seem wrong:
      (1) If we have triple diamonds with points in only the center
         diamond (that is, a line of 6 with some points hanging off
         the center 2), and we ask for the center 6/outer 2, the
         lonesome points become the ends, and "outer_elongation"
         reflects their elongation, even though the line of 6 is longer.
         This doesn't seem to affect any checkpoint or concentric cases.
         Perhaps the phrases "center 6" and "outer 2" aren't really
         correct here.
      (2) If we have a quarter tag, and we ask for the center 6/outer 2,
         the center 6 are, of course, a 2x3, and the ends of the line
         are the outer 2.  We set "outer_elongation" to reflect
         the elongation of the outer 2, which may not be what people
         would think.  Once again, this does not arise in any actual
         checkpoint or concentric case.
      (3) If we have an "H", and we ask for the center 2/outer 6, the
         outer 6 are the 2x3, and "outer_elongation" will show
         their elongation, even though that is not the elongation of
         the 3x4 matrix.  Once again, this does not arise in any actual
         checkpoint or concentric case. */

Private void concentrify(
   setup *ss,
   calldef_schema analyzer,
   setup inners[],
   setup *outers,
   int *center_arity,
   int *outer_elongation,    /* Set to elongation of original outers, except if center 6 and outer 2, in which case, if centers are a bone6, it shows their elongation. */
   int *xconc_elongation)    /* If cross concentric, set to elongation of original ends. */

{
   int i, rot, analyzer_index;
   cm_hunk *chunk;
   cm_thing *lmap_ptr;

   clear_people(outers);
   clear_people(&inners[0]);

   *center_arity = 1;

   /* First, translate the analyzer into a form that encodes only what we need to know. */

   switch (analyzer) {
      case schema_lateral_6:
         analyzer_index = analyzer_LATERAL6; break;
      case schema_vertical_6:
         analyzer_index = analyzer_VERTICAL6; break;
      case schema_checkpoint:
      case schema_ckpt_star:
         analyzer_index = analyzer_CHECKPT; break;
      case schema_single_concentric:
      case schema_single_cross_concentric:
         analyzer_index = analyzer_SINGLE; break;
      case schema_conc_star12:
         analyzer_index = analyzer_STAR12; break;
      case schema_conc_star16:
         analyzer_index = analyzer_STAR16; break;
      case schema_concentric_diamond_line:
         analyzer_index = analyzer_DIAMOND_LINE; break;
      case schema_concentric_6_2:
         analyzer_index = analyzer_6X2; break;
      case schema_concentric_6_2_tgl:
         analyzer_index = analyzer_6X2_TGL; break;
      case schema_concentric_diamonds:
      case schema_cross_concentric_diamonds:
         analyzer_index = analyzer_CONC_DIAMONDS; break;
      case schema_concentric_2_6:
         if (ss->kind == s3x4 && ((ss->people[1].id1 | ss->people[2].id1 | ss->people[7].id1 | ss->people[8].id1) ||
                     (!(ss->people[0].id1 & ss->people[3].id1 & ss->people[4].id1 & ss->people[5].id1 &
                     ss->people[6].id1 & ss->people[9].id1 & ss->people[10].id1 & ss->people[11].id1))))
            fail("Can't find centers and ends in this formation.");
         analyzer_index = analyzer_2X6;
         break;
      case schema_conc_triple_lines:
         if (     ss->kind != s3x4 ||
                  (((ss->people[0].id1 | ss->people[1].id1) != 0) && ((ss->people[2].id1 | ss->people[3].id1) != 0)) ||
                  (((ss->people[6].id1 | ss->people[7].id1) != 0) && ((ss->people[8].id1 | ss->people[9].id1) != 0)))
            fail("Illegal setup for this call.");
         analyzer_index = analyzer_TRIPLE_LINE; break;
      case schema_rev_checkpoint:
      case schema_concentric:
      case schema_conc_star:
      case schema_cross_concentric:
         analyzer_index = analyzer_NORMAL; break;
      case schema_single_concentric_together:
         if (ss->kind == s1x8 || ss->kind == s_ptpd || setup_attrs[ss->kind].setup_limits == 3)
            analyzer_index = analyzer_SINGLE;
         else
            analyzer_index = analyzer_NORMAL;
         break;
      case schema_maybe_single_concentric:
      case schema_maybe_single_cross_concentric:
         fail("Can't figure out whether concentric is single -- this shouldn't happen.");
      case schema_maybe_matrix_conc_star:
         fail("Can't figure out whether concentric is 12 or 16 matrix -- this shouldn't happen.");
      default:
         fail("Don't understand this concentricity type???.");
   }

   /* Next, deal with the "normal_concentric" special case.
      We need to be careful here.  The setup was not able to be normalized, but
      we are being asked to pick out centers and ends.  There are very few
      non-normal concentric setups for which we can do that correctly.  For example,
      if we have concentric diamonds whose points are along different axes from each
      other, who are the 4 centers of the total setup?  (If the axes of the diamonds
      had been consistent, the setup would have been normalized to crossed lines,
      and we wouldn't be here.)
      If we don't take action here and the setup is normal_concentric, an error will
      be raised, since the "concthing" entries are zero. */

   if (ss->kind == s_normal_concentric) {
      outers->rotation = ss->outer.srotation;
      inners[0].rotation = ss->outer.srotation;   /* Yes, this looks wrong, but it isn't. */

      switch (analyzer_index) {
         case analyzer_DIAMOND_LINE:
            if (ss->inner.skind == sdmd && ss->inner.srotation == ss->outer.srotation) {
               inners[0].kind = s1x4;
               outers->kind = sdmd;
               (void) copy_person(&inners[0], 0, ss, 13);
               (void) copy_person(&inners[0], 1, ss, 0);
               (void) copy_person(&inners[0], 2, ss, 15);
               (void) copy_person(&inners[0], 3, ss, 2);
               (void) copy_person(outers, 0, ss, 12);
               (void) copy_person(outers, 1, ss, 1);
               (void) copy_person(outers, 2, ss, 14);
               (void) copy_person(outers, 3, ss, 3);
               if (ss->outer.skind == s1x4) {
                  *outer_elongation = ss->outer.srotation & 1;
                  goto finish;
               }
            }
            break;
         case analyzer_NORMAL:
            if (ss->outer.skind == nothing) {
               inners[0].kind = ss->inner.skind;
               outers->kind = nothing;
               for (i=0; i<4; i++) (void) copy_person(&inners[0], i, ss, i);
               *outer_elongation = ss->concsetup_outer_elongation;
               goto finish;
            }
            else if (ss->inner.skind == sdmd && ss->inner.srotation == ss->outer.srotation) {
               inners[0].kind = sdmd;
               outers->kind = ss->outer.skind;
               for (i=0; i<4; i++) {
                  (void) copy_person(&inners[0], i, ss, i);
                  (void) copy_person(outers, i, ss, i+12);
               }

               /* We allow a diamond inside a box with wrong elongation (if elongation were good, it would be an hourglass.) */
               if (ss->outer.skind == s2x2) {
                  *outer_elongation = (ss->outer.srotation ^ ss->concsetup_outer_elongation) & 1;
                  goto finish;
               }
               /* And a diamond inside a line with wrong elongation (if elongation were good, it would be a 3x1 diamond.) */
               if (ss->outer.skind == s1x4) {
                  *outer_elongation = ss->outer.srotation & 1;
                  goto finish;
               }
            }
            else if (ss->inner.skind == s1x2 && ss->outer.skind == s1x6 && ss->inner.srotation != ss->outer.srotation) {
               inners[0].kind = sdmd;
               outers->kind = s1x4;

               if ((ss->inner.srotation - ss->outer.srotation) & 2) {
                  (void) copy_rot(&inners[0], 1, ss, 1, 033);
                  (void) copy_rot(&inners[0], 3, ss, 0, 033);
               }
               else {
                  (void) copy_rot(&inners[0], 1, ss, 0, 011);
                  (void) copy_rot(&inners[0], 3, ss, 1, 011);
               }

               (void) copy_person(&inners[0], 0, ss, 14);
               (void) copy_person(&inners[0], 2, ss, 17);
               (void) copy_person(outers, 0, ss, 12);
               (void) copy_person(outers, 1, ss, 13);
               (void) copy_person(outers, 2, ss, 15);
               (void) copy_person(outers, 3, ss, 16);

               goto finish;
            }
            break;
      }
   }

   /* Next, do the 3x4 -> qtag fudging.  Don't ask permission, just do it. **** maybe that isn't right for Z calls. */

   if (analyzer_index == analyzer_NORMAL && ss->kind == s3x4) {
      inners[0].kind = s1x4;
      inners[0].rotation = 0;
      outers->kind = s2x2;
      outers->rotation = 0;
      *outer_elongation = (outers->rotation^1) & 1;
      (void) copy_person(&inners[0], 0, ss, 10);
      (void) copy_person(&inners[0], 1, ss, 11);
      (void) copy_person(&inners[0], 2, ss, 4);
      (void) copy_person(&inners[0], 3, ss, 5);

      if (!ss->people[0].id1 && ss->people[1].id1)
         (void) copy_person(outers, 0, ss, 1);
      else if (!ss->people[1].id1 && !ss->people[0].id1)
         (void) copy_person(outers, 0, ss, 0);
      else fail("Can't find centers and ends in this formation.");

      if (!ss->people[2].id1 && ss->people[3].id1)
         (void) copy_person(outers, 1, ss, 3);
      else if (!ss->people[3].id1 && ss->people[2].id1)
         (void) copy_person(outers, 1, ss, 2);
      else fail("Can't find centers and ends in this formation.");

      if (!ss->people[6].id1 && ss->people[7].id1)
         (void) copy_person(outers, 2, ss, 7);
      else if (!ss->people[7].id1 && ss->people[6].id1)
         (void) copy_person(outers, 2, ss, 6);
      else fail("Can't find centers and ends in this formation.");

      if (!ss->people[8].id1 && ss->people[9].id1)
         (void) copy_person(outers, 3, ss, 9);
      else if (!ss->people[9].id1 && ss->people[8].id1)
         (void) copy_person(outers, 3, ss, 8);
      else fail("Can't find centers and ends in this formation.");
      goto finish;
   }

   chunk = setup_attrs[ss->kind].conctab;
   if (!chunk) fail(conc_error_messages[analyzer_index]);
   lmap_ptr = chunk->hunk[analyzer_index];
   if (!lmap_ptr) fail(conc_error_messages[analyzer_index]);

   if (lmap_ptr == &map2x4_2x4v) {
      /* See if people were facing laterally, and use the other map if so. */
      for (i=0; i<16; i++) {
         if (ss->people[i].id1 & 1) { lmap_ptr = &map2x4_2x4; break; }
      }
   }

   inners[0].kind = lmap_ptr->insetup;
   inners[0].rotation = 0;
   outers->kind = lmap_ptr->outsetup;
   outers->rotation = 0;

   rot = 0;

   if (lmap_ptr->outer_rot) {
      outers->rotation--;
      rot = 011;
   }

   for (i=0; i<lmap_ptr->outlimit; i++) (void) copy_rot(outers, i, ss, lmap_ptr->mapout[i], rot);

   rot = 0;

   if (lmap_ptr->inner_rot) {
      inners[0].rotation--;
      rot = 011;
   }

   for (i=0; i<lmap_ptr->inlimit; i++) (void) copy_rot(&inners[0], i, ss, lmap_ptr->mapin[i], rot);

   if (lmap_ptr == &map_spec_star12) {
      *center_arity = 2;
      clear_people(&inners[1]);
      inners[1].kind = lmap_ptr->insetup;
      inners[1].rotation = 0;
      (void) copy_person(&inners[1], 0, ss, 10);
      (void) copy_person(&inners[1], 1, ss, 5);
      (void) copy_person(&inners[1], 2, ss, 8);
      (void) copy_person(&inners[1], 3, ss, 9);
   }
   else if (lmap_ptr == &map_spec_star16) {
      *center_arity = 3;
      clear_people(&inners[1]);
      clear_people(&inners[2]);
      inners[1].kind = lmap_ptr->insetup;
      inners[2].kind = lmap_ptr->insetup;
      inners[1].rotation = 0;
      inners[2].rotation = 0;
      (void) copy_person(&inners[1], 0, ss, 6);
      (void) copy_person(&inners[1], 1, ss, 7);
      (void) copy_person(&inners[1], 2, ss, 14);
      (void) copy_person(&inners[1], 3, ss, 15);
      (void) copy_person(&inners[2], 0, ss, 13);
      (void) copy_person(&inners[2], 1, ss, 12);
      (void) copy_person(&inners[2], 2, ss, 10);
      (void) copy_person(&inners[2], 3, ss, 11);
   }
   if (lmap_ptr == &map_3line) {
      *center_arity = 2;
      clear_people(&inners[1]);
      inners[1].kind = lmap_ptr->insetup;
      inners[1].rotation = 0;
      (void) copy_person(&inners[1], 0, ss, 9);
      (void) copy_person(&inners[1], 1, ss, 8);
      (void) copy_person(&inners[1], 2, ss, 6);
      (void) copy_person(&inners[1], 3, ss, 7);
   }

   /* Set the outer elongation to whatever elongation the outsides really had, as indicated
      by the map. */

   *outer_elongation = (lmap_ptr->mapelong + outers->rotation) & 1;

   /* If the concept is cross-concentric, we have to set the elongation to what
         the centers (who will, of course, be going to the outside) had.
      If the original centers are in a 2x2, we set it according to the orientation
         of the entire 2x4 they were in, so that they can think about whether they were
         in lines or columns and act accordingly.  If they were not in a 2x4, that is,
         the setup was a wing or galaxy, we set the elongation to -1 to indicate an
         error.  In such a case the centers won't be able to decide whether they were
         in lines or columns.
      But if the outsides started in a 1x4, they override the centers' own axis. */

   if (analyzer == schema_cross_concentric || analyzer == schema_cross_concentric_diamonds) {
      *xconc_elongation = lmap_ptr->inner_rot;
      if (lmap_ptr->outsetup == s1x4) *xconc_elongation = lmap_ptr->outer_rot;
      switch (ss->kind) {
         case s_galaxy:
         case s_rigger:
            *xconc_elongation = -1;    /* Can't do this! */
            break;
      }
   }
   else if (analyzer == schema_concentric_6_2_tgl) {
      if (inners[0].kind == s_bone6)
         *outer_elongation = (outers->rotation ^ 1) & 1;
   }
   else if (analyzer == schema_concentric_6_2) {
      if (inners[0].kind == s_bone6)
         *outer_elongation = outers->rotation & 1;
   }

   finish:

   canonicalize_rotation(outers);
   canonicalize_rotation(&inners[0]);
   if (*center_arity >= 2)
      canonicalize_rotation(&inners[1]);
   if (*center_arity == 3)
      canonicalize_rotation(&inners[2]);
}




warning_index concwarneeetable[] = {warn__lineconc_perp, warn__xclineconc_perpe, warn__lineconc_par};
warning_index concwarn1x4table[] = {warn__lineconc_perp, warn__xclineconc_perpc, warn__lineconc_par};
warning_index concwarndmdtable[] = {warn__dmdconc_perp, warn__xcdmdconc_perpc, warn__dmdconc_par};



extern void concentric_move(
   setup *ss,
   setup_command *cmdin,
   setup_command *cmdout,
   calldef_schema analyzer,
   defmodset modifiersin1,
   defmodset modifiersout1,
   setup *result)
{
   defmodset localmods1, localmodsin1, localmodsout1;
   setup begin_inner[3];
   setup begin_outer;
   int begin_outer_elongation;
   int begin_xconc_elongation;
   int final_elongation;
   int center_arity;
   setup result_inner[3];
   setup result_outer;
   setup outer_inners[4];
   int i, k;
   int crossing =      /* This is an int (0 or 1) rather than a long_boolean, because we will index with it. */
               (analyzer == schema_cross_concentric) ||
               (analyzer == schema_single_cross_concentric) ||
               (analyzer == schema_cross_concentric_diamonds);

   setup_kind orig_inners_start_kind;    /* The original info about the people who STARTED on the inside. */
   uint32 orig_inners_start_dirs;        /* We don't need rotation, since we will only use this if 2x2. */
   uint32 orig_inners_start_directions[32];

   setup_kind orig_outers_start_kind;    /* The original info about the people who STARTED on the outside. */
   uint32 orig_outers_start_dirs;        /* We don't need rotation, since we will only use this if 2x2. */
   uint32 orig_outers_start_directions[32];

   setup_kind final_outers_start_kind;   /* The original info about the people who will FINISH on the outside. */
   uint32 *final_outers_start_directions;

   int final_outers_finish_dirs;         /* The final info about the people who FINISHED on the outside. */
   uint32 final_outers_finish_directions[32];

   uint32 saved_number_fields = current_number_fields;

   uint32 snagflag = ss->cmd.cmd_misc2_flags;

   uint32 save_cmd_misc2_flags = ss->cmd.cmd_misc2_flags;
   parse_block *save_skippable = ss->cmd.skippable_concept;

   ss->cmd.cmd_misc2_flags &= ~(0xFFFF | CMD_MISC2__CTR_USE_INVERT | CMD_MISC2__CTR_USE);
   ss->cmd.skippable_concept = (parse_block *) 0;

   /* It is clearly too late to expand the matrix -- that can't be what is wanted. */
   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
   if (analyzer != schema_conc_triple_lines)    /* Yes!  We allow "quick so-and-so shove off"! */
      ss->cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;

   for (i=0; i<32; i++) {
      orig_inners_start_directions[i] =
      orig_outers_start_directions[i] =
      final_outers_finish_directions[i] = 0;
   }

   localmodsin1 = modifiersin1;
   localmodsout1 = modifiersout1;

   if ((snagflag & CMD_MISC2__CENTRAL_MASK) == CMD_MISC2__CENTRAL_SNAG) {
      if (ss->cmd.cmd_frac_flags)
         fail("Can't do fractional \"snag\".");

      ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__CENTRAL_MASK;
   }
   else if ((snagflag & CMD_MISC2__CENTRAL_MASK) == CMD_MISC2__CENTRAL_MYSTIC)
      ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__CENTRAL_MASK;

   begin_inner[0].cmd = ss->cmd;
   begin_inner[1].cmd = ss->cmd;
   begin_inner[2].cmd = ss->cmd;
   begin_outer.cmd = ss->cmd;

   concentrify(ss, analyzer, begin_inner, &begin_outer, &center_arity, &begin_outer_elongation, &begin_xconc_elongation);

   /* Get initial info for the original ends. */
   orig_outers_start_dirs = 0;
   for (i=0; i<=setup_attrs[begin_outer.kind].setup_limits; i++) {
      uint32 q = begin_outer.people[i].id1;
      orig_outers_start_dirs |= q;
      if (q) orig_outers_start_directions[(q >> 6) & 037] = q;
   }
   orig_outers_start_kind = begin_outer.kind;

   /* Get initial info for the original centers. */
   orig_inners_start_dirs = 0;
   for (i=0; i<=setup_attrs[begin_inner[0].kind].setup_limits; i++) {
      uint32 q = begin_inner[0].people[i].id1;
      orig_inners_start_dirs |= q;
      if (q) orig_inners_start_directions[(q >> 6) & 037] = q;
   }
   orig_inners_start_kind = begin_inner[0].kind;

   if (crossing) {
      setup temptemp = begin_inner[0];
      begin_inner[0] = begin_outer;
      begin_outer = temptemp;

      final_outers_start_kind = orig_inners_start_kind;
      final_outers_start_directions = orig_inners_start_directions;
   }
   else {
      final_outers_start_kind = orig_outers_start_kind;
      final_outers_start_directions = orig_outers_start_directions;
   }

   /* If the call turns out to be "detour", this will make it do just the ends part. */
   begin_outer.cmd.cmd_misc_flags |= CMD_MISC__DOING_ENDS;

   /* There are two special pieces of information we now have that will help us decide where to
      put the outsides.  "Orig_outers_start_kind" tells what setup the outsides were originally in,
      and "begin_outer_elongation" is odd if the outsides were oriented vertically.
      "begin_outer_elongation" refers to absolute orientation, that is, "our" view of the
      setups, taking all rotations into account.  "final_outers_start_directions" gives the individual
      orientations (absolute) of the people who are finishing on the outside.  Later, we will compute
      "final_outers_finish_dirs", telling how the individual people were oriented.  How we use all this
      information depends on many things that we will attend to below. */

   /* Giving one of the concept descriptor pointers as nil indicates that we don't want those people to do anything. */

   if (cmdin) {
      for (k=0; k<center_arity; k++) {
         uint32 mystictest;
         parse_block f1, f2;

         update_id_bits(&begin_inner[k]);

         /* Inherit certain assumptions to the child setups.  This is EXTREMELY incomplete. */

         if ((analyzer == schema_single_concentric) || (analyzer == schema_single_cross_concentric)) {
            if (ss->kind == s1x4 && begin_inner[k].kind == s1x2 && (begin_inner[k].cmd.cmd_assume.assumption == cr_2fl_only || begin_inner[k].cmd.cmd_assume.assumption == cr_wave_only))
               begin_inner[k].cmd.cmd_assume.assumption = cr_wave_only;
            else
               begin_inner[k].cmd.cmd_assume.assumption = cr_none;
         }
         else
            begin_inner[k].cmd.cmd_assume.assumption = cr_none;

         current_number_fields >>= ((DFM1_NUM_SHIFT_MASK & modifiersin1) / DFM1_NUM_SHIFT_BIT) * 4;
         begin_inner[k].cmd.parseptr = cmdin->parseptr;
         begin_inner[k].cmd.callspec = cmdin->callspec;
         begin_inner[k].cmd.cmd_final_flags = cmdin->cmd_final_flags;

         /* If cross concentric, we are looking for "invert mystic" */

         mystictest = crossing ? (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__INVERT_CENTRAL) : CMD_MISC2__CENTRAL_MYSTIC;

         /* Handle "snag" for centers. */
         if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == CMD_MISC2__CENTRAL_SNAG) {
            if (mystictest == (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__INVERT_CENTRAL))
               fail("Can't do \"central/snag/mystic\" with this call.");
            begin_inner[k].cmd.cmd_frac_flags = 0x000112;
         }

         /* Handle "mystic" for centers. */

         if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == mystictest) {
            mirror_this(&begin_inner[k]);
            begin_inner[k].cmd.cmd_misc_flags ^= CMD_MISC__EXPLICIT_MIRROR;
         }



   if ((save_cmd_misc2_flags & (CMD_MISC2__CTR_USE|CMD_MISC2__CTR_USE_INVERT)) == CMD_MISC2__CTR_USE) {
      if (!save_skippable) fail("Internal error in centers work, please report this.");

      if (begin_inner[k].cmd.callspec) {
         f1 = *save_skippable;
         f1.next = &f2;

         f2.concept = &mark_end_of_list;
         f2.call = begin_inner[k].cmd.callspec;
         f2.next = (parse_block *) 0;
         f2.subsidiary_root = (parse_block *) 0;
         f2.gc_ptr = (parse_block *) 0;
         f2.selector = current_selector;
         f2.direction = current_direction;
         f2.number = current_number_fields;
         f2.tagger = -1;
         f2.circcer = -1;
         begin_inner[k].cmd.callspec = (callspec_block *) 0;
         begin_inner[k].cmd.parseptr = &f1;
      }
      else {
         fail("No callspec, centers!!!!!!");
      }
   }




         move(&begin_inner[k], FALSE, &result_inner[k]);

         if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == mystictest)
            mirror_this(&result_inner[k]);

         current_number_fields = saved_number_fields;
      }
   }
   else {
      for (k=0; k<center_arity; k++) {
         result_inner[k] = begin_inner[k];
         result_inner[k].result_flags = 0;
         /* Strip out the roll bits -- people who didn't move can't roll. */
         if (setup_attrs[result_inner[k].kind].setup_limits >= 0) {
            for (i=0; i<=setup_attrs[result_inner[k].kind].setup_limits; i++) {
               if (result_inner[k].people[i].id1) result_inner[k].people[i].id1 = (result_inner[k].people[i].id1 & (~ROLL_MASK)) | ROLLBITM;
            }
         }
      }
   }

   if (cmdout) {
      /* If the ends' starting setup is a 2x2, and we did not say "concentric" (indicated by
         the DFM1_CONC_CONCENTRIC_RULES flag being off), we mark the setup as elongated.  If the call
         turns out to be a 2-person call, the elongation will be checked against the pairings
         of people, and an error will be given if it isn't right.  This is what makes "cy-kick"
         illegal from diamonds, and "ends hinge" illegal from waves.  The reason this is turned
         off when the "concentric" concept is given is so that "concentric hinge" from waves,
         obnoxious as it may be, will be legal.
      We also turn it off if this is reverse checkpoint.  In that case, the ends know exactly
         where they should go.  This is what makes "reverse checkpoint recycle by star thru"
         work from a DPT setup. */

      uint32 mystictest;
      parse_block f1, f2;

      if ((begin_outer.kind == s2x2 || begin_outer.kind == s_short6) &&
            analyzer != schema_rev_checkpoint &&
            !(begin_outer_elongation & ~1)) {      /* We demand elongation be 0 or 1. */
         begin_outer.cmd.prior_elongation_bits = begin_outer_elongation+1;

         /* If "demand lines" or "demand columns" has been given, we suppress elongation
            checking.  In that case, the database author knows what elongation is required
            and is taking responsibility for it.  This is what makes "scamper" and "divvy up" work.
            We also do this if the concept is cross concentric.  In that case the people doing the
            "ends" call actually did it in the center (the way they were taught in C2 class)
            before moving to the outside. */

         if (     ((DFM1_CONC_CONCENTRIC_RULES | DFM1_CONC_DEMAND_LINES | DFM1_CONC_DEMAND_COLUMNS) & modifiersout1) ||
                  crossing ||
                  analyzer == schema_checkpoint)
            begin_outer.cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
      }
      else if (begin_outer.kind == s1x4 && analyzer != schema_rev_checkpoint) {
         begin_outer.cmd.prior_elongation_bits = 0x40;     /* Indicate that these people are working around the outside. */

         if (     (DFM1_CONC_CONCENTRIC_RULES & modifiersout1) ||
                  crossing ||
                  analyzer == schema_checkpoint)
            begin_outer.cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
      }

      current_number_fields >>= ((DFM1_NUM_SHIFT_MASK & modifiersout1) / DFM1_NUM_SHIFT_BIT) * 4;
      update_id_bits(&begin_outer);

      /* Inherit certain assumptions to the child setups.  This is EXTREMELY incomplete. */

      if ((analyzer == schema_single_concentric) || (analyzer == schema_single_cross_concentric)) {
         if (ss->kind == s1x4 && begin_outer.kind == s1x2 && (begin_outer.cmd.cmd_assume.assumption == cr_2fl_only || begin_outer.cmd.cmd_assume.assumption == cr_wave_only))
            begin_outer.cmd.cmd_assume.assumption = cr_wave_only;
         else
            begin_outer.cmd.cmd_assume.assumption = cr_none;
      }
      else
         begin_outer.cmd.cmd_assume.assumption = cr_none;

      /* This call to "move" will fill in good stuff (viz. the DFM1_CONCENTRICITY_FLAG_MASK)
         into begin_outer.cmd.cmd_misc_flags, which we will use below to do various "force_lines",
         "demand_columns", etc. things. */
      begin_outer.cmd.parseptr = cmdout->parseptr;
      begin_outer.cmd.callspec = cmdout->callspec;
      begin_outer.cmd.cmd_final_flags = cmdout->cmd_final_flags;

      /* If cross concentric, we are looking for plain "mystic" */
      mystictest = crossing ? CMD_MISC2__CENTRAL_MYSTIC : (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__INVERT_CENTRAL);

      /* Handle "invert snag" for ends. */
      if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == (CMD_MISC2__CENTRAL_SNAG | CMD_MISC2__INVERT_CENTRAL)) {
         if (mystictest == CMD_MISC2__CENTRAL_MYSTIC)
            fail("Can't do \"central/snag/mystic\" with this call.");
         begin_outer.cmd.cmd_frac_flags = 0x000112;
      }

      /* Handle "invert mystic" for ends. */

      if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == mystictest) {
         mirror_this(&begin_outer);
         begin_outer.cmd.cmd_misc_flags ^= CMD_MISC__EXPLICIT_MIRROR;
      }




      if ((save_cmd_misc2_flags & (CMD_MISC2__CTR_USE|CMD_MISC2__CTR_USE_INVERT)) == (CMD_MISC2__CTR_USE|CMD_MISC2__CTR_USE_INVERT)) {
         if (!save_skippable) fail("Internal error in ends work, please report this.");

         if (begin_outer.cmd.callspec) {
            f1 = *save_skippable;
            f1.next = &f2;

            f2.concept = &mark_end_of_list;
            f2.call = begin_outer.cmd.callspec;
            f2.next = (parse_block *) 0;
            f2.subsidiary_root = (parse_block *) 0;
            f2.gc_ptr = (parse_block *) 0;
            f2.selector = current_selector;
            f2.direction = current_direction;
            f2.number = current_number_fields;
            f2.tagger = -1;
            f2.circcer = -1;
            begin_outer.cmd.callspec = (callspec_block *) 0;
            begin_outer.cmd.parseptr = &f1;
         }
         else {
            fail("No callspec, ends!!!!!!");
         }
      }





      move(&begin_outer, FALSE, &result_outer);

      if ((snagflag & (CMD_MISC2__CENTRAL_MASK | CMD_MISC2__INVERT_CENTRAL)) == mystictest)
         mirror_this(&result_outer);

      current_number_fields = saved_number_fields;
   }
   else {
      result_outer = begin_outer;
      if (!(begin_outer_elongation & ~1))
         result_outer.result_flags = begin_outer_elongation+1;
      else
         result_outer.result_flags = 0;   /* Outer people have unknown elongation and aren't moving.  Not good. */

      /* Grab the "did_last_part" flag from the call that was actually done. */
      result_outer.result_flags |= result_inner[0].result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__PARTS_ARE_KNOWN);

      localmodsout1 |= DFM1_CONC_FORCE_SPOTS;      /* Make sure these people go to the same spots. */
      /* Strip out the roll bits -- people who didn't move can't roll. */
      if (setup_attrs[result_outer.kind].setup_limits >= 0) {
         for (i=0; i<=setup_attrs[result_outer.kind].setup_limits; i++) {
            if (result_outer.people[i].id1) result_outer.people[i].id1 = (result_outer.people[i].id1 & (~ROLL_MASK)) | ROLLBITM;
         }
      }
   }

   if (!cmdin) {
      /* Grab the "did_last_part" flags from the call that was actually done. */
      for (k=0; k<center_arity; k++)
         result_inner[k].result_flags |= result_outer.result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__PARTS_ARE_KNOWN);
   }

   if (analyzer == schema_conc_triple_lines && fix_n_results(2, result_inner))
      result_inner[0].kind = nothing;

   /* If the call was something like "ends detour", the concentricity info was left in the
      cmd_misc_flags during the execution of the call, so we have to pick it up to make sure
      that the necessary "demand" and "force" bits are honored. */
   localmodsout1 |= (begin_outer.cmd.cmd_misc_flags & DFM1_CONCENTRICITY_FLAG_MASK);

   /* Check whether the necessary "demand" conditions are met.  First, set "localmods1"
      to the demand info for the call that the original ends did.  Where this comes from
      depends on whether the schema is cross concentric. */

   localmods1 = crossing ? localmodsin1 : localmodsout1;

   if ((DFM1_CONC_DEMAND_LINES & localmods1) && (orig_outers_start_kind == s2x2)) {
      /* We make use of the fact that the setup, being a 2x2, is canonicalized. */
      if ((begin_outer_elongation < 0) ||
            (orig_outers_start_dirs & (1 << 3*(begin_outer_elongation & 1))))
         fail("Outsides must be as if in lines at start of this call.");
   }

   if ((DFM1_CONC_DEMAND_COLUMNS & localmods1) && (orig_outers_start_kind == s2x2)) {
      if ((begin_outer_elongation < 0) ||
            (orig_outers_start_dirs & (8 >> 3*(begin_outer_elongation & 1))))
         fail("Outsides must be as if in columns at start of this call.");
   }

   /* Now check whether there are any demands on the original centers.  The interpretation
      of "lines" and "columns" is slightly different in this case.  We apply the test only if
      the centers are in a 2x2, but we don't care about the outsides' setup, as long as it
      has a measurable elongation.  If the outsides are also in a 2x2, so that the whole setup
      is a 2x4, these tests will do just what they say -- they will check whether the centers
      believe they are in lines or columns.  However, if the outsides are in a 1x4, so the
      overall setup is a "rigger", we simply test the outsides' elongation.  In such a case
      "demand lines" means "demand outsides lateral to me". */

   localmods1 = crossing ? localmodsout1 : localmodsin1;

   if ((DFM1_CONC_DEMAND_LINES & localmods1) && (orig_inners_start_kind == s2x2)) {
      if ((begin_outer_elongation < 0) ||
            (orig_inners_start_dirs & (1 << 3*(begin_outer_elongation & 1))))
         fail("Centers must be as if in lines at start of this call.");
   }

   if ((DFM1_CONC_DEMAND_COLUMNS & localmods1) && (orig_inners_start_kind == s2x2)) {
      if ((begin_outer_elongation < 0) ||
            (orig_inners_start_dirs & (8 >> 3*(begin_outer_elongation & 1))))
         fail("Centers must be as if in columns at start of this call.");
   }

   localmods1 = localmodsout1;

   final_outers_finish_dirs = 0;
   for (i=0; i<=setup_attrs[result_outer.kind].setup_limits; i++) {
      int q = result_outer.people[i].id1;
      final_outers_finish_dirs |= q;
      if (q) final_outers_finish_directions[(q >> 6) & 037] = q;
   }

   /* Now final_outers_finish_dirs tells whether outer peoples' orientations changed.
      This is only meaningful if outer setup is 2x2.  Note that, if the setups
      are 2x2's, canonicalization sets their rotation to zero, so the
      tbonetest quantities refer to absolute orientation. */

   /* Deal with empty setups. */

   if (result_outer.kind == nothing) {
      if (result_inner[0].kind == nothing) {
         result->kind = nothing;    /* If everyone is a phantom, it's simple. */
         result->result_flags = 0;
         return;
      }

      /* If the schema is one of the special ones, we will know what to do. */
      if (  analyzer == schema_conc_star ||
            analyzer == schema_ckpt_star ||
            analyzer == schema_conc_star12 ||
            analyzer == schema_conc_star16) {

         /* This is what makes 12 matrix relay the top work when everyone is
            in the stars. */

         result_outer.kind = s1x4;
         clear_people(&result_outer);
         result_outer.result_flags = 0;
         result_outer.rotation = 0;
      }
      else if (analyzer == schema_conc_triple_lines) {
         result_outer.kind = s2x2;
         clear_people(&result_outer);
         result_outer.result_flags = 0;
         result_outer.rotation = 0;
      }
      else if (analyzer == schema_concentric_diamond_line) {
         if (ss->kind == s_wingedstar || ss->kind == s_wingedstar12 || ss->kind == s_wingedstar16) {
            result_outer.kind = s2x2;
            result_outer.rotation = 0;
            clear_people(&result_outer);
            /* Set their "natural" elongation perpendicular to their original diamond.
               The test for this is 1P2P; touch 1/4; column circ; boys truck; split phantom
               lines tag chain thru reaction.  They should finish in outer triple boxes,
               not a 2x4. */
            result_outer.result_flags = (result_inner[0].result_flags & ~3) | 2;
         }
         else
            goto no_end_err;
      }
      else {
         /* We may be in serious trouble -- we have to figure out what setup the ends
            finish in, and they are all phantoms. */

         result_outer = begin_outer;               /* Restore the original bunch of phantoms. */
         result_outer.result_flags = 0;

         /* If setup is 2x2 and a command "force spots" or "force otherway" was given, we can
            honor it, even though there are no people present in the outer setup. */

         if (     final_outers_start_kind == s2x2 &&
                  result_outer.kind == s2x2 &&
                  (localmods1 & (DFM1_CONC_FORCE_SPOTS | DFM1_CONC_FORCE_OTHERWAY))) {
            ;        /* Take no further action. */
         }
         else {
            /* Otherwise, we can save the day only if we
               can convince ourselves that they did the call "nothing".  We make use
               of the fact that "concentrify" did NOT flush them, so we still know
               what their starting setup was.
            This is what makes split phantom diamonds diamond chain through work
               from columns far apart. */

            /* Make sure these people go to the same spots, and remove possibly misleading info. */
            localmods1 |= DFM1_CONC_FORCE_SPOTS;
            localmods1 &= ~(DFM1_CONC_FORCE_LINES | DFM1_CONC_FORCE_COLUMNS | DFM1_CONC_FORCE_OTHERWAY);

            if (cmdout && cmdout->callspec && (cmdout->callspec->schema == schema_nothing))
               ;        /* It's OK. */
            else if (center_arity > 1)
               ;        /* It's OK. */
            else {
               /* We simply have no idea where the outsides should be.  We
                  simply contract the setup to a 4-person setup (or whatever),
                  throwing away the outsides completely.  If this was an
                  "on your own", it may be possible to put things back together.
                  This is what makes "1P2P; pass thru; ENDS leads latch on;
                  ON YOUR OWN disband & snap the lock" work.  But if we try to glue
                  these setups together, "fix_n_results" will raise an error, since
                  it won't know whether to leave room for the phantoms. */

               *result = result_inner[0];   /* This gets all the inner people, and the result_flags. */
               result->kind = s_normal_concentric;
               result->inner.skind = result_inner[0].kind;
               result->inner.srotation = result_inner[0].rotation;
               result->outer.skind = nothing;
               result->outer.srotation = 0;
               /* We remember a vague awareness of where the outside would have been. */
               result->concsetup_outer_elongation = begin_outer_elongation;
               goto getout;
            }
         }
      }
   }
   else if (result_inner[0].kind == nothing) {
      /* If the schema is one of the special ones, we will know what to do. */
      if (  analyzer == schema_conc_star ||
            analyzer == schema_ckpt_star ||
            analyzer == schema_conc_star12 ||
            analyzer == schema_conc_star16 ||
            analyzer == schema_conc_triple_lines) {
                  ;        /* Take no action. */
      }
      /* If the ends are a 2x2, we just set the missing centers to a 2x2.
         The ends had better know their elongation, of course.  It shouldn't
         matter to the ends whether the phantoms in the center did something
         that leaves the whole setup as diamonds or as a 2x4.  (Some callers
         might think it matters (Hi, Clark!) but it doesn't matter to this program.)
         This is what makes split phantom diamonds diamond chain through work
         from a grand wave. */
      else if (result_outer.kind == s2x2 && center_arity == 1) {
         result_inner[0].kind = s2x2;
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
         result_inner[0].rotation = 0;
      }
      /* If the ends are a 1x4, we just set the missing centers to a 1x4,
         so the entire setup is a 1x8.  Maybe the phantoms went to a 2x2,
         so the setup is really a rigger, but we don't care.  See the comment
         just above.  This is what makes "1P2P; pass thru; ENDS leads latch on;
         ON YOUR OWN disband & snap the lock" work. */
      else if (result_outer.kind == s1x4 && center_arity == 1) {
         result_inner[0].kind = s1x4;
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
         result_inner[0].rotation = result_outer.rotation;
      }
      /* A similar thing, for single concentric. */
      else if (result_outer.kind == s1x2 && analyzer == schema_single_concentric) {
         result_inner[0].kind = s1x2;
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
         result_inner[0].rotation = result_outer.rotation;
      }
      else {
         /* The centers are just gone!  It is quite possible that "fix_n_results"
            may be able to repair this damage by copying some info from another setup.
            Missing centers are not as serious as missing ends, because they won't
            lead to indecision about whether to leave space for the phantoms. */

         int j;
         *result = result_outer;   /* This gets the result_flags. */
         result->kind = s_normal_concentric;
         result->outer.skind = result_outer.kind;
         result->outer.srotation = result_outer.rotation;
         result->inner.skind = nothing;
         result->inner.srotation = 0;
         result->concsetup_outer_elongation = 0;

         for (j=0; j<12; j++) (void) copy_person(result, j+12, &result_outer, j);
         result->result_flags = 0;
         goto getout;
      }
   }

   /* The time has come to compute the elongation of the outsides in the final setup.
      This gets complicated if the outsides' final setup is a 2x2.  Among the
      procedures we could use are:
         (1) if the call is "checkpoint", go to spots with opposite elongation
            from the original outsides' elongation.  This is the "Hodson checkpoint
            rule", named after the caller who first used a consistent, methodical,
            and universal rule for the checkpoint concept.
         (2) if the call is "concentric", use the Hodson rule if the original setup
            was a 1x4 or diamond, or the "lines-to-lines, columns-to-columns" rule
            if the original setup was a 2x2.
         (3) if we have various definition flags, such as "force_lines" or
            "force_otherway", obey them.
      We will use information from several sources in carrying out these rules.
      The concentric concept will signify itself by turning on the "lines_lines"
      flag.  The checkpoint concept will signify itself by turning on the
      "force_otherway" flag.  The "parallel_conc_end" flag in the outsides' setup
      indicates that, if "concentric" or "checkpoint" are NOT being used, the call
      wants the outsides to maintain the same elongation as they had at the beginning.
      This is what makes "ends hinge" and "ends recycle" do their respective
      right things when called from a grand wave. */

   /* Default: the ends just keep their original elongation.  This will often
      mean that they stay on their spots. */

   final_elongation = crossing ? begin_xconc_elongation : begin_outer_elongation;

   /* Note: final_elongation might be -1 now, meaning that the people on the outside
      cannot determine their elongation from the original setup.  Unless their
      final setup is one that does not require knowing the value of final_elongation,
      it is an error. */

   /* At this point, "final_elongation" actually has the INITIAL elongation of the
      people who finished on the outside.  That is, if they went from a wave or diamond
      to a 2x2, it has the elongation of their initial wave or diamond points.

      Exception: if the schema was conc_6_2 or conc_6_2_tri, and the centers are in a bone6,
      "final_elongation" has the elongation of that bone6.

      The elongation bits in their setup tells how they "naturally" wanted to end,
      based on the call they did, how it got divided up, whether it had the "parallel_conc_end"
      flag on, etc.

      We will use both pieces of information to figure out how to elongate the outsides at
      the conclusion of the call.  For example, if the word "concentric" was not spoken,
      we will just use their "natural" elongation from the setup.  This is what makes
      "ends hinge" work from a grand wave.  If the word "concentric" was spoken, their
      natural elongation is discarded, and we will set them perpendicular to their
      original 1x4 or diamond, using the value in "final_elongation"  If invocation
      flags like "force lines" or "force columns" are present, we will use those.

      When we are done, our final judgement will be put back into the variable
      "final_elongation". */

   if (analyzer == schema_conc_triple_lines)
      /* This is ALWAYS vertically oriented.  In any case, we can't do any of the other calculations,
         because the centers and ends have been reversed. */
      final_elongation = 1;
   else {
      if (result_outer.kind == s2x2 || result_outer.kind == s2x4) {
         warning_index *concwarntable;

         if (final_outers_start_kind == s1x4) {
            if (orig_outers_start_kind == s1x4)
               concwarntable = concwarneeetable;
            else
               concwarntable = concwarn1x4table;
         }
         else
            concwarntable = concwarndmdtable;

         switch (final_outers_start_kind) {
            case s1x4: case sdmd:

               /* Outers' call has gone from a 1x4 or diamond to a 2x2.  The rules are:
                  (1) The "force_columns" or "force_lines" flag in the invocation takes precedence
                     over anything else.
                  (2) If the "concentric rules" flag is on (that flag is a euphemism for "the
                     concentric or checkpoint concept is explicitly in use here"), we set the
                     elongation perpendicular to the original 1x4 or diamond.
                  (3) If the "force_otherway" invocation flag is on, meaning the database
                     really wants us to, we set the elongation perpendicular to the original
                     1x4 or diamond.
                  (4) If the "force_spots" invocation flag is on, meaning the database
                     really wants us to, we set the elongation parallel to the original
                     1x4 or diamond.
                  (5) Otherwise, we set the elongation to the natural elongation that the people
                     went to.  This uses the result of the "par_conc_end" flag for 1x4/dmd -> 2x2
                     calls, or the manner in which the setup was divided for calls that were put
                     together from 2-person calls, or whatever.  (For 1x4->2x2 calls, the "par_conc_end"
                     flag means the call prefers the SAME elongation in the resulting 2x2.)  The default,
                     absent this flag, is to change the elongation.  In any case, the result of all that
                     has been encoded into the elongation of the 2x2 setup that the people went to;
                     we just have to obey. */

               if (DFM1_CONC_FORCE_LINES & localmods1) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in lines - they are T-boned.");
                  final_elongation = final_outers_finish_dirs & 1;
               }
               else if (DFM1_CONC_FORCE_COLUMNS & localmods1) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in columns - they are T-boned.");
                  final_elongation = (final_outers_finish_dirs+1) & 1;
               }
               else if ((DFM1_CONC_CONCENTRIC_RULES | DFM1_CONC_FORCE_OTHERWAY) & localmods1) {
                  warn(concwarntable[crossing]);
                  final_elongation ^= 1;
               }
               else if (DFM1_CONC_FORCE_SPOTS & localmods1)
                  ;           /* It's OK the way it is. */
               else {
                  /* Get the elongation from the result setup, if possible. */
                  int newelong = (result_outer.result_flags & 3) - 1;

                  if (result_outer.result_flags & 3) {
                     if (final_elongation == newelong)
                        warn(concwarntable[2]);
                     else
                        warn(concwarntable[crossing]);
                  }

                  final_elongation = newelong;
               }

               break;
            case s2x2:
               /* If call went from 2x2 to 2x2, the rules are:
                  First, check for "force_columns" or "force_lines" in the invocation.  This is not
                     a property of the call that we did, but of the way its parent (or the concept) invoked it.
                  Second, check for "force_spots" or "force_otherway" in the invocation.  This is not
                     a property of the call that we did, but of the way its parent (or the concept) invoked it.
                  Third, check for "lines_lines" in the invocation.  This is not
                     a property of the call that we did, but of the way its parent (or the concept) invoked it.
                     If the concept was "concentric", it will be on, of course.
                  Finally, check the elongation bits in the result flags left over from the call.  These tell
                     whether to work to spots, or antispots, or whatever, based what the call was, and whether
                     it, or various sequential parts of it, had the "parallel_conc_end" flag on.
                  If there are no elongation bits, we simply don't know what to do.

                  Note that the "ends do thus-and-so" concept does NOT set the lines_lines flag in the
                     invocation, so we work to spots unless the call says "parallel_conc_end".  Counter-rotate,
                     for example, says "parallel_conc_end", so it works to antispots. */

               if (DFM1_CONC_FORCE_LINES & localmods1) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in lines - they are T-boned.");
                  final_elongation = final_outers_finish_dirs & 1;
               }
               else if (DFM1_CONC_FORCE_COLUMNS & localmods1) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in columns - they are T-boned.");
                  final_elongation = (final_outers_finish_dirs+1) & 1;
               }
               else if (DFM1_CONC_FORCE_OTHERWAY & localmods1)
                  final_elongation ^= 1;
               else if (DFM1_CONC_FORCE_SPOTS & localmods1)
                  ;           /* It's OK the way it is. */
               else if (DFM1_CONC_CONCENTRIC_RULES & localmods1) {       /* do "lines-to-lines / columns-to-columns" */
                  int new_elongation = -1;

                  if (final_elongation < 0)
                     fail("People who finish on the outside can't tell whether they started in line-like or column-like orientation.");

                  for (i=0; i<32; i++) {     /* Search among all possible people, including virtuals and phantoms. */
                     if (final_outers_finish_directions[i]) {
                        int t = (final_outers_start_directions[i] ^ final_outers_finish_directions[i] ^ final_elongation) & 1;
                        if (t != new_elongation) {
                           if (new_elongation >= 0)
                              fail("Sorry, outsides would have to go to a 'pinwheel', can't handle that.");
                           new_elongation = t;
                        }
                     }
                  }

                  final_elongation = new_elongation;
               }
               else
                  final_elongation = (result_outer.result_flags & 3) - 1;

               break;
            default:
               fail("Don't recognize starting setup.");
         }
      }
      else if (result_outer.kind == s_short6 || result_outer.kind == sbigdmd) {
         /* If it both started and ended in a short6, take the info from the way the call was executed.
            Otherwise, take it from the way concentrify thought the ends were initially elongated. */
         if (final_outers_start_kind == s_short6)
            final_elongation = (result_outer.result_flags & 3) - 1;
      }
   }

   /* Now lossage in "final_elongation" may have been repaired.  If it is still
      negative, there may be trouble ahead. */

   outer_inners[0] = result_outer;
   outer_inners[1] = result_inner[0];
   outer_inners[2] = result_inner[1];
   outer_inners[3] = result_inner[2];

   normalize_concentric(analyzer, center_arity, outer_inners, final_elongation, result);

   getout:

   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_MASK;
   reinstate_rotation(ss, result);
   return;

   no_end_err:
   fail("Can't figure out ending setup for concentric call -- no ends.");
}


/* This overwrites its first argument setup. */
extern void merge_setups(setup *ss, merge_action action, setup *result)
{
   int i, r, rot, offs, limit, lim1, limhalf;
   setup res2copy;
   setup outer_inners[2];
   setup *res1, *res2;

   res2copy = *result;
   res1 = ss;
   res2 = &res2copy;

   /* If either incoming setup is big, opt for a 4x4 rather than C1 phantoms.
      The test for this is, from a squared set, everyone phantom column wheel thru.
      We want a 4x4. */

   canonicalize_rotation(res1);    /* Do we really need to do this before normalize_setup? */
   normalize_setup(res1, normalize_before_merge);
   canonicalize_rotation(res1);    /* We definitely need to do it now -- a 2x2 might have been created. */

   tryagain:

   canonicalize_rotation(res2);
   normalize_setup(res2, normalize_before_merge);
   canonicalize_rotation(res2);

   /* Canonicalize the setups according to their kind.  This is a bit sleazy, since
      the enumeration order of setup kinds is supposed to be insignificant.  We depend in
      general on small setups being before larger ones.  In particular, we seem to require:
         s2x2 < s2x4
         s2x2 < s2x6
         s2x2 < s1x8
         s1x4 < s1x8
         s1x4 < s2x4
         s2x4 < s_c1phan
         s2x4 < s2x6
      You get the idea. */

   if (res2->kind < res1->kind) {
      setup *temp = res2;
      res2 = res1;
      res1 = temp;
   }

   /* If one of the setups was a "concentric" setup in which there are no ends, we can still handle it. */

   if (res2->kind == s_normal_concentric && res2->outer.skind == nothing) {
      res2->kind = res2->inner.skind;
      res2->rotation = res2->inner.srotation;
      goto tryagain;    /* Need to recanonicalize setup order. */
   }

   result->rotation = res2->rotation;

   r = (res1->rotation - res2->rotation) & 3;
   rot = r * 011;

   if (res1->kind == nothing) {
      *result = *res2;
      return;
   }
   else if ((res1->kind == s2x4) && (res2->kind == s2x4) && (r&1)) {
      offs = r * 2;

      /* It used to be that we used an algorithm, shown below, to decide whether to opt for C1 pahntoms
         or a 4x4 matrix.  That algorithm said that one opted for C1 phantoms if each incoming 2x4
         had, in each quadrant, either both dancers or neither.  That is, we would opt for C1 phantoms
         if the result would have, in each quadrant, either a star, no one, or two people in a miniwave
         or equivalent sort of thing.  That is, the result would be either stars or "classic" C1
         phantoms.  It would never produce star-like things populated in a peculiar arrangement.
         This was presumably a consequence of our aversion to such peculiarly populated stars.
         It has since come to light that "perk up", done from classic C1 phantom miniwaves of
         consistent handedness, really ought to go to C1 phantoms, even though the stars that
         result are peculiarly populated.  It has further come to light that having "phantom columns
         wheel thru" from a squared set go to C1 phantoms is not right -- they should go to a 4x4
         matrix even though the C1 phantoms would be "classically" populated.  So we now ignore
         the issue of how the stars would be populated, and make the determination based on the
         sizes, prior to stripping, of the incoming setups.  If either was larger than 8 people,
         they are presumed to be doing some kind of phantom call in a matrix, and want to maintain
         spots.  The old code was:

         if ((((res1->people[0].id1 & res1->people[1].id1 & res1->people[4].id1 & res1->people[5].id1) |
                  (res1->people[2].id1 & res1->people[3].id1 & res1->people[6].id1 & res1->people[7].id1)) & BIT_PERSON) &&
            (((res2->people[0].id1 & res2->people[1].id1 & res2->people[4].id1 & res2->people[5].id1) |
                  (res2->people[2].id1 & res2->people[3].id1 & res2->people[6].id1 & res2->people[7].id1)) & BIT_PERSON)) {
            result->kind = s_c1phan;
            .....

         The test cases for this stuff are the aforesaid phantom perk up and the aforesaid phantom
         columns wheel thru. */

      /* Late-breaking news:  We now do this even more carefully.  The argument "action" tells us what to do. */

      /* Even later-breaking news:  We now go to a 16 matrix anyway, if the actual spots that people
         occupy are "O" spots. */

      if (action == merge_strict_matrix ||
               (res1->people[1].id1 | res1->people[2].id1 | res1->people[5].id1 | res1->people[6].id1 |
               res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1) == 0) {
         result->kind = s4x4;
         clear_person(result, 12);
         clear_person(result, 13);
         clear_person(result, 14);
         clear_person(result, 0);
         clear_person(result, 4);
         clear_person(result, 5);
         clear_person(result, 6);
         clear_person(result, 8);
         (void) copy_person(result, 10, res2, 0);
         (void) copy_person(result, 15, res2, 1);
         (void) copy_person(result, 3, res2, 2);
         (void) copy_person(result, 1, res2, 3);
         (void) copy_person(result, 2, res2, 4);
         (void) copy_person(result, 7, res2, 5);
         (void) copy_person(result, 11, res2, 6);
         (void) copy_person(result, 9, res2, 7);

         install_rot(result, 7, res1, 0^offs, rot);
         install_rot(result, 5, res1, 1^offs, rot);
         install_rot(result, 14, res1, 2^offs, rot);
         install_rot(result, 3, res1, 3^offs, rot);
         install_rot(result, 15, res1, 4^offs, rot);
         install_rot(result, 13, res1, 5^offs, rot);
         install_rot(result, 6, res1, 6^offs, rot);
         install_rot(result, 11, res1, 7^offs, rot);
      }
      else {
         uint32 t1, t2;

         result->kind = s_c1phan;
         t1  = copy_person(result, 0,  res2, 0);
         t1 |= copy_person(result, 2,  res2, 1);
         t2  = copy_person(result, 7,  res2, 2);
         t2 |= copy_person(result, 5,  res2, 3);
         t1 |= copy_person(result, 8,  res2, 4);
         t1 |= copy_person(result, 10, res2, 5);
         t2 |= copy_person(result, 15, res2, 6);
         t2 |= copy_person(result, 13, res2, 7);
         t2 |= copy_rot(result, 11, res1, 0^offs, rot);
         t2 |= copy_rot(result, 9,  res1, 1^offs, rot);
         t1 |= copy_rot(result, 4,  res1, 2^offs, rot);
         t1 |= copy_rot(result, 6,  res1, 3^offs, rot);
         t2 |= copy_rot(result, 3,  res1, 4^offs, rot);
         t2 |= copy_rot(result, 1,  res1, 5^offs, rot);
         t1 |= copy_rot(result, 12, res1, 6^offs, rot);
         t1 |= copy_rot(result, 14, res1, 7^offs, rot);

         /* See if we have a "classical" C1 phantom setup, and give the appropriate warning. */
         if (action != merge_c1_phantom_nowarn) {
            if (t1 == 0 || t2 == 0)
               warn(warn__check_c1_phan);
            else
               warn(warn__check_c1_stars);
         }
      }
      return;
   }
   else if (res1->kind == s_crosswave && res2->kind == s_crosswave && (r&1)) {
      result->kind = s_crosswave;

      if ((res2->people[0].id1 | res2->people[4].id1) == 0) {
         /* Exchange the setups and try again. */
         setup *temp = res2;
         res2 = res1;
         res1 = temp;
         result->rotation = res2->rotation;
         r = (res1->rotation - res2->rotation) & 3;
         rot = r * 011;
      }

      offs = r * 2;

      if ((res1->people[0].id1 | res1->people[3].id1 | res1->people[4].id1 | res1->people[7].id1) == 0) {
         *result = *res2;
         install_rot(result, 5, res1, 0^offs, rot);
         install_rot(result, 2, res1, 3^offs, rot);
         install_rot(result, 1, res1, 4^offs, rot);
         install_rot(result, 6, res1, 7^offs, rot);
         return;
      }
   }
   else if (res1->kind == s1x4 && res2->kind == s_crosswave && (r&1)) {
     *result = *res2;
      install_rot(result, 2, res1, 1^r, rot);
      install_rot(result, 3, res1, 0^r, rot);
      install_rot(result, 6, res1, 3^r, rot);
      install_rot(result, 7, res1, 2^r, rot);
      return;
   }
   else if (res2->kind == s_c1phan && res1->kind == s2x4) {
      result->kind = s_c1phan;
      for (i=0; i<16; i++)
         (void) copy_person(result, i, res2, i);

      result->rotation -= r;
      canonicalize_rotation(result);

      install_person(result, 0, res1, 0);
      install_person(result, 2, res1, 1);
      install_person(result, 7, res1, 2);
      install_person(result, 5, res1, 3);
      install_person(result, 8, res1, 4);
      install_person(result, 10, res1, 5);
      install_person(result, 15, res1, 6);
      install_person(result, 13, res1, 7);

      result->rotation += r;
      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s_qtag && res1->kind == s1x2 &&
            (!(res2->people[3].id1 | res2->people[7].id1))) {
      res2->kind = s_short6;
      res2->rotation++;
      (void) copy_rot(res2, 3, res2, 5, 033);         /* careful -- order is important */
      (void) copy_rot(res2, 5, res2, 0, 033);
      (void) copy_rot(res2, 0, res2, 1, 033);
      (void) copy_rot(res2, 1, res2, 2, 033);
      (void) copy_rot(res2, 2, res2, 4, 033);
      (void) copy_rot(res2, 4, res2, 6, 033);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric_2_6, 1, outer_inners, res2->rotation, result);
      return;
   }
   else if (res2->kind == s2x4 && res1->kind == s2x2) {
      result->kind = s2x4;
      for (i=0; i<8; i++)
         (void) copy_person(result, i, res2, i);

      res1->rotation += r;
      canonicalize_rotation(res1);

      install_person(result, 1, res1, 0);
      install_person(result, 2, res1, 1);
      install_person(result, 5, res1, 2);
      install_person(result, 6, res1, 3);

      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s4x4 && res1->kind == s2x4) {
      *result = *res2;
      result->rotation -= r;
      canonicalize_rotation(result);

      install_person(result, 10, res1, 0);
      install_person(result, 15, res1, 1);
      install_person(result, 3,  res1, 2);
      install_person(result, 1,  res1, 3);
      install_person(result, 2,  res1, 4);
      install_person(result, 7,  res1, 5);
      install_person(result, 11, res1, 6);
      install_person(result, 9,  res1, 7);

      result->rotation += r;
      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s2x6 && res1->kind == s2x2) {
      result->kind = s2x6;
      for (i=0; i<12; i++)
         (void) copy_person(result, i, res2, i);

      res1->rotation += r;
      canonicalize_rotation(res1);

      install_person(result, 2, res1, 0);
      install_person(result, 3, res1, 1);
      install_person(result, 8, res1, 2);
      install_person(result, 9, res1, 3);

      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s2x2 &&
            (!(res2->people[2].id1 | res2->people[3].id1 | res2->people[6].id1 | res2->people[7].id1))) {
      res2->kind = s1x4;
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 5);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s3x1dmd && res1->kind == s2x2 &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      res2->kind = sdmd;
      (void) copy_person(res2, 1, res2, 3);
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 7);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s2x2 &&
            (!(res2->people[0].id1 | res2->people[2].id1 | res2->people[4].id1 | res2->people[6].id1))) {
      res2->kind = s1x4;
      (void) copy_person(res2, 0, res2, 1);
      (void) copy_person(res2, 1, res2, 3);
      (void) copy_person(res2, 2, res2, 5);
      (void) copy_person(res2, 3, res2, 7);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s2x2 && action == merge_without_gaps &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      res2->kind = s1x4;
      (void) copy_person(res2, 1, res2, 3);
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 7);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res1->kind == s_rigger &&
            (!(res1->people[0].id1 | res1->people[1].id1 | res1->people[4].id1 | res1->people[5].id1))) {
      res1->kind = s1x4;
      (void) copy_person(res1, 0, res1, 6);
      (void) copy_person(res1, 1, res1, 7);
      outer_inners[1] = *res2;
      outer_inners[0] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s1x4 &&
            (!(res2->people[2].id1 | res2->people[3].id1 | res2->people[6].id1 | res2->people[7].id1))) {
      res2->kind = s1x4;
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 5);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s1x2 &&
            (!(res2->people[2].id1 | res2->people[6].id1))) {
      res2->kind = s1x6;
      (void) copy_person(res2, 2, res2, 3);         /* careful -- order is important */
      (void) copy_person(res2, 3, res2, 4);
      (void) copy_person(res2, 4, res2, 5);
      (void) copy_person(res2, 5, res2, 7);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric_2_6, 1, outer_inners, 0, result);
      return;
   }
   else if (res2->kind == s2x4 && res1->kind == s1x4 &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      int outer_elongation = res2->rotation & 1;
      res2->kind = s2x2;
      (void) copy_person(res2, 1, res2, 3);
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 7);
      canonicalize_rotation(res2);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, outer_elongation, result);
      return;
   }
   else if (res2->kind == s2x4 && res1->kind == s_qtag &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1 |
               res1->people[0].id1 | res1->people[1].id1 | res1->people[4].id1 | res1->people[5].id1))) {
      int outer_elongation = res2->rotation & 1;
      res2->kind = s2x2;
      (void) copy_person(res2, 1, res2, 3);
      (void) copy_person(res2, 2, res2, 4);
      (void) copy_person(res2, 3, res2, 7);
      res1->kind = s1x4;
      (void) copy_person(res1, 0, res1, 6);
      (void) copy_person(res1, 1, res1, 7);
      canonicalize_rotation(res2);
      canonicalize_rotation(res1);
      outer_inners[0] = *res2;
      outer_inners[1] = *res1;
      normalize_concentric(schema_concentric, 1, outer_inners, outer_elongation, result);
      return;
   }
   else if (res1->kind == s_qtag && res2->kind == s2x4 && (r&1) &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      *result = *res1;
      r = (res2->rotation - res1->rotation) & 3;
      rot = r * 011;

      offs = r * 2;

      install_rot(result, 0, res2, 5^offs, rot);
      install_rot(result, 1, res2, 2^offs, rot);
      install_rot(result, 4, res2, 1^offs, rot);
      install_rot(result, 5, res2, 6^offs, rot);
      return;
   }
   else if (res2->kind == s_ptpd && res1->kind == s1x8 && r == 0 &&
            (!(res1->people[1].id1 | res1->people[3].id1 | res1->people[5].id1 | res1->people[7].id1))) {
      *result = *res2;

      install_person(result, 0, res1, 0);
      install_person(result, 2, res1, 2);
      install_person(result, 4, res1, 4);
      install_person(result, 6, res1, 6);
      return;
   }
   else if (res2->kind == s_crosswave && res1->kind == s1x2 && r == 0) {
      *result = *res2;
      result->kind = s3x1dmd;
      install_person(result, 3, res2, 2);
      install_person(result, 7, res2, 6);
      (void) copy_person(result, 2, res1, 0);
      (void) copy_person(result, 6, res1, 1);
      return;
   }
   else if (res2->kind == s2x4 && res1->kind == s1x8 && r == 0 && action == merge_without_gaps &&
            (!(res1->people[0].id1 | res1->people[3].id1 | res1->people[4].id1 | res1->people[7].id1 |
            res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      *result = *res2;
      result->kind = s_ptpd;
      (void) copy_person(result, 1, res2, 0);
      (void) copy_person(result, 5, res2, 4);
      (void) copy_person(result, 3, res2, 7);
      (void) copy_person(result, 7, res2, 3);
      (void) copy_person(result, 0, res1, 1);
      (void) copy_person(result, 2, res1, 2);
      (void) copy_person(result, 4, res1, 5);
      (void) copy_person(result, 6, res1, 6);
      return;
   }
   else if (res2->kind == s_crosswave && res1->kind == s1x2 && (r&1)) {
      *result = *res2;
      offs = r >> 1;
      install_rot(result, 3, res1, 0^offs, rot);
      install_rot(result, 7, res1, 1^offs, rot);
      return;
   }
   else if (res2->kind == s3x1dmd && res1->kind == s1x2 && (r&1)) {
      *result = *res2;
      result->kind = s_crosswave;
      install_person(result, 2, res2, 3);
      install_person(result, 6, res2, 7);
      offs = r >> 1;
      (void) copy_rot(result, 3, res1, 0^offs, rot);
      (void) copy_rot(result, 7, res1, 1^offs, rot);
      return;
   }
   else if (res2->kind == s3x1dmd && res1->kind == s1x2 && r == 0) {
      *result = *res2;
      install_person(result, 2, res1, 0);
      install_person(result, 6, res1, 1);
      return;
   }
   else if (res2->kind == s_bone && res1->kind == s1x4 && r == 0) {
      *result = *res2;

      install_person(result, 6, res1, 0);
      install_person(result, 7, res1, 1);
      install_person(result, 2, res1, 2);
      install_person(result, 3, res1, 3);
      return;
   }
   else if (res2->kind == s3x4 && res1->kind == s_qtag && r == 0) {
      *result = *res2;

      install_person(result, 1, res1, 0);
      install_person(result, 2, res1, 1);
      install_person(result, 4, res1, 2);
      install_person(result, 5, res1, 3);
      install_person(result, 7, res1, 4);
      install_person(result, 8, res1, 5);
      install_person(result, 10, res1, 6);
      install_person(result, 11, res1, 7);
      return;
   }
   else if (res2->kind == s1x8 && res1->kind == s_bone && r == 0 &&
            (!(res1->people[0].id1 | res1->people[1].id1 | res1->people[4].id1 | res1->people[5].id1))) {
      *result = *res2;

      install_person(result, 3, res1, 6);
      install_person(result, 2, res1, 7);
      install_person(result, 7, res1, 2);
      install_person(result, 6, res1, 3);
      return;
   }
   else if (res2->kind == s_ptpd && res1->kind == s2x2 &&
            (!(res2->people[0].id1 | res2->people[2].id1 | res2->people[4].id1 | res2->people[6].id1))) {
      result->kind = s2x4;
      res1->rotation += r;
      canonicalize_rotation(res1);
      (void) copy_person(result, 0, res2, 1);
      (void) copy_person(result, 3, res2, 7);
      (void) copy_person(result, 4, res2, 5);
      (void) copy_person(result, 7, res2, 3);
      (void) copy_person(result, 1, res1, 0);
      (void) copy_person(result, 2, res1, 1);
      (void) copy_person(result, 5, res1, 2);
      (void) copy_person(result, 6, res1, 3);
      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s_galaxy && res1->kind == s2x2) {
      *result = *res2;
      (void) install_person(result, 1, res1, 0);
      (void) install_person(result, 3, res1, 1);
      (void) install_person(result, 5, res1, 2);
      (void) install_person(result, 7, res1, 3);
      return;
   }
   else if (res2->kind == s_bone && res1->kind == s2x2 && action != merge_strict_matrix &&
            (!(res2->people[3].id1 | res2->people[2].id1 | res2->people[7].id1 | res2->people[6].id1))) {
      result->kind = s2x4;
      res1->rotation += r;
      canonicalize_rotation(res1);
      (void) copy_person(result, 0, res2, 0);
      (void) copy_person(result, 3, res2, 1);
      (void) copy_person(result, 4, res2, 4);
      (void) copy_person(result, 7, res2, 5);
      (void) copy_person(result, 1, res1, 0);
      (void) copy_person(result, 2, res1, 1);
      (void) copy_person(result, 5, res1, 2);
      (void) copy_person(result, 6, res1, 3);
      canonicalize_rotation(result);
      return;
   }
   else if (res2->kind == s_ptpd && res1->kind == s1x8 && r == 0 &&
            (!(res2->people[1].id1 | res2->people[3].id1 | res2->people[5].id1 | res2->people[7].id1))) {
      *result = *res1;

      install_person(result, 0, res2, 0);
      install_person(result, 2, res2, 2);
      install_person(result, 4, res2, 4);
      install_person(result, 6, res2, 6);
      return;
   }
   else if (res2->kind == s2x4 && res1->kind == s_bone && r == 0 &&
            (!(res2->people[1].id1 | res2->people[2].id1 | res2->people[5].id1 | res2->people[6].id1))) {
      *result = *res1;

      install_person(result, 0, res2, 0);
      install_person(result, 1, res2, 3);
      install_person(result, 4, res2, 4);
      install_person(result, 5, res2, 7);
      return;
   }
   else if (res2->kind == s2x6 && res1->kind == s_qtag && (r&1) &&
            (!(res2->people[2].id1 | res2->people[3].id1 | res2->people[8].id1 | res2->people[9].id1 |
               res1->people[0].id1 | res1->people[1].id1 | res1->people[4].id1 | res1->people[5].id1))) {
      *result = *res2;
      result->kind = sbigdmd;

      if (r&2) {
         (void) copy_rot(result, 2,  res1, 2,  rot);
         (void) copy_rot(result, 3,  res1, 3,  rot);
         (void) copy_rot(result, 8,  res1, 6,  rot);
         (void) copy_rot(result, 9,  res1, 7,  rot);
      }
      else {
         (void) copy_rot(result, 8,  res1, 2,  rot);
         (void) copy_rot(result, 9,  res1, 3,  rot);
         (void) copy_rot(result, 2,  res1, 6,  rot);
         (void) copy_rot(result, 3,  res1, 7,  rot);
      }
      return;
   }
   else if (res2->kind == s2x6 && res1->kind == s1x4 && (r&1) &&
            (!(res2->people[2].id1 | res2->people[3].id1 | res2->people[8].id1 | res2->people[9].id1))) {
      *result = *res2;
      result->kind = sbigdmd;

      if (r&2) {
         (void) copy_rot(result, 2,  res1, 2,  rot);
         (void) copy_rot(result, 3,  res1, 3,  rot);
         (void) copy_rot(result, 8,  res1, 0,  rot);
         (void) copy_rot(result, 9,  res1, 1,  rot);
      }
      else {
         (void) copy_rot(result, 8,  res1, 2,  rot);
         (void) copy_rot(result, 9,  res1, 3,  rot);
         (void) copy_rot(result, 2,  res1, 0,  rot);
         (void) copy_rot(result, 3,  res1, 1,  rot);
      }
      return;
   }
   else if (res2->kind == s2x6 && res1->kind == s2x4 && r == 0) {
      /* Because of canonicalization, we know that r = 0 unless
         they are 90 degrees from each other. */
      *result = *res2;

      install_person(result, 1, res1, 0);
      install_person(result, 2, res1, 1);
      install_person(result, 3, res1, 2);
      install_person(result, 4, res1, 3);
      install_person(result, 7, res1, 4);
      install_person(result, 8, res1, 5);
      install_person(result, 9, res1, 6);
      install_person(result, 10, res1, 7);
      return;
   }

   /* This is sleazy. */

   limit = setup_attrs[res1->kind].setup_limits;
   lim1 = limit+1;
   limhalf = lim1 >> 1;

   /* The only remaining hope is that the setups match and we can blindly combine them.  We require
      lim1 even because our 180 degree rotation wouldn't work for triangles. */

   if (res1->kind != res2->kind || r & 1 || limit < 0 || lim1 & 1)
      fail("Can't figure out result setup.");

   *result = *res2;

   if (r) {
      for (i=0; i<limit+1; i++)
         install_rot(result, i, res1, (i+limhalf) % lim1, rot);
   }
   else {
      for (i=0; i<limit+1; i++)
         install_person(result, i, res1, i);
   }
}


extern void on_your_own_move(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   setup setup1, setup2, res1;
   setup outer_inners[2];

   if (ss->kind != s2x4) fail("Must have 2x4 setup for 'on your own'.");

   setup1 = *ss;              /* Get outers only. */
   clear_person(&setup1, 1);
   clear_person(&setup1, 2);
   clear_person(&setup1, 5);
   clear_person(&setup1, 6);
   setup1.cmd = ss->cmd;
   setup1.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED | CMD_MISC__PHANTOMS;
   move(&setup1, FALSE, &res1);

   setup2 = *ss;              /* Get inners only. */
   clear_person(&setup2, 0);
   clear_person(&setup2, 3);
   clear_person(&setup2, 4);
   clear_person(&setup2, 7);
   setup1.cmd = ss->cmd;
   setup2.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED | CMD_MISC__PHANTOMS;
   setup2.cmd.parseptr = parseptr->subsidiary_root;
   move(&setup2, FALSE, result);

   outer_inners[0] = res1;
   outer_inners[1] = *result;

   result->result_flags = get_multiple_parallel_resultflags(outer_inners, 2);
   merge_setups(&res1, merge_strict_matrix, result);
}



/* We know that the setup has well-defined size, and that the conctable masks are good. */
extern void punt_centers_use_concept(setup *ss, setup *result)
{
   int i, setupcount;
   uint32 ssmask;
   warning_info saved_warnings;
   setup the_setups[2], the_results[2];
   int sizem1 = setup_attrs[ss->kind].setup_limits;
   cm_hunk *chunk = setup_attrs[ss->kind].conctab;
   calldef_schema schema = ((calldef_schema) ss->cmd.cmd_misc2_flags & 0xFFFFUL);
   int crossconc = (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE_INVERT) ? 1 : 0;

   /* Clear the stuff out of the cmd_misc2_flags word. */

   ss->cmd.cmd_misc2_flags &= ~(0xFFFF | CMD_MISC2__CTR_USE_INVERT | CMD_MISC2__CTR_USE);

   the_setups[0] = *ss;              /* designees */
   the_setups[1] = *ss;              /* non-designees */

   switch (schema) {
      case schema_concentric_2_6:
         ssmask = chunk->mask_2_6;
         break;
      case schema_concentric_6_2:
         ssmask = chunk->mask_6_2;
         break;
      default:
         ssmask = chunk->mask_normal;
         break;
   }

   for (i=sizem1; i>=0; i--) {
      clear_person(&the_setups[(ssmask & 1) ^ crossconc], i);
      ssmask >>= 1;
   }

   normalize_setup(&the_setups[0], normalize_before_isolated_call);
   normalize_setup(&the_setups[1], normalize_before_isolated_call);
   saved_warnings = history[history_ptr+1].warnings;

   for (setupcount=0; setupcount<2; setupcount++) {
      the_setups[setupcount].cmd = ss->cmd;
      the_setups[setupcount].cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
      if (setupcount == 1) {
         parse_block *parseptrcopy = skip_one_concept(ss->cmd.parseptr);
         the_setups[setupcount].cmd.parseptr = parseptrcopy->next;
      }

      move(&the_setups[setupcount], FALSE, &the_results[setupcount]);
   }

   /* Shut off "each 1x4" types of warnings -- they will arise spuriously while
      the people do the calls in isolation. */
   history[history_ptr+1].warnings.bits[0] &= ~dyp_each_warnings.bits[0];
   history[history_ptr+1].warnings.bits[1] &= ~dyp_each_warnings.bits[1];
   history[history_ptr+1].warnings.bits[0] |= saved_warnings.bits[0];
   history[history_ptr+1].warnings.bits[1] |= saved_warnings.bits[1];

   *result = the_results[1];
   result->result_flags = get_multiple_parallel_resultflags(the_results, 2);
   merge_setups(&the_results[0], merge_c1_phantom, result);
}


typedef struct fixerjunk {
   setup_kind ink;
   setup_kind outk;
   int rot;
   int numsetups;
   Const struct fixerjunk *next1x2;
   Const struct fixerjunk *next1x2rot;
   Const struct fixerjunk *next1x4;
   Const struct fixerjunk *next1x4rot;
   Const struct fixerjunk *nextdmd;
   Const struct fixerjunk *nextdmdrot;
   Const struct fixerjunk *next2x2;
   int nonrot[4][4];
   int yesrot[4][4];
} fixer;


static Const fixer foocc;
static Const fixer f1x8aa;
static Const fixer foozz;
static Const fixer foozzd;
static Const fixer f1x8aad;
static Const fixer foo55d;
static Const fixer foo99d;
static Const fixer foo66d;
static Const fixer f2x4endd;
static Const fixer bar55d;
static Const fixer f3x4rzz;
static Const fixer f3x4lzz;
static Const fixer fppaad;
static Const fixer fpp55d;
static Const fixer f1x3zzd;
static Const fixer f1x3yyd;
static Const fixer fboneendo;


static Const fixer foo33 = {
   s1x2,
   s2x4,
   0,
   2,
   &foo33,                   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 1}, {5, 4}},
   {{7, 6}, {2, 3}}};

static Const fixer foocc = {
   s1x2,
   s2x4,
   0,
   2,
   &foocc,                   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{2, 3}, {7, 6}},
   {{0, 1}, {5, 4}}};

static Const fixer f1x8aa = {
   s1x2,
   s1x8,
   0,
   2,
   &f1x8aa,                  /* next1x2    */
   &foozz,                   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 3}, {7, 5}},
   {{-1}}};

static Const fixer foozz = {
   s1x2,
   s_ptpd,
   1,
   2,
   &foozz,                   /* next1x2    */
   &f1x8aa,                  /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 3}, {7, 5}},
   {{-1}}};

static Const fixer foozzd = {
   s2x2,
   s_ptpd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8aad,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &foozzd,                  /* next2x2    */
   {{1, 7, 5, 3}},
   {{-1}}};






static Const fixer f3x4left = {
   s1x2,
   s3x4,
   0,
   2,
   &f3x4left,                /* next1x2    */
   &f3x4rzz,                 /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 1}, {7, 6}},
   {{-1}}};

static Const fixer f3x4right = {
   s1x2,
   s3x4,
   0,
   0x100+2,
   &f3x4right,               /* next1x2    */
   &f3x4lzz,                 /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{2, 3}, {9, 8}},
   {{-1}}};

static Const fixer f3x4lzz = {
   s1x2,
   s2x6,
   0,
   2,
   &f3x4lzz,                 /* next1x2    */
   &f3x4right,               /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 1}, {7, 6}},
   {{-1}}};

static Const fixer f3x4rzz = {
   s1x2,
   s2x6,
   0,
   0x100+2,
   &f3x4rzz,                 /* next1x2    */
   &f3x4left,                /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{4, 5}, {11, 10}},
   {{-1}}};



static Const fixer f1x8aad = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8aad,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &foozzd,                  /* next2x2    */
   {{1, 3, 5, 7}},
   {{-1}}};

static Const fixer f3x1zzd = {
   sdmd,
   s3x1dmd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &f3x1zzd,                 /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */   /* f1x3zzd */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 3, 4, 7}},
   {{-1}}};

static Const fixer f1x3zzd = {
   sdmd,
   s1x3dmd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &f1x3zzd,                 /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */   /* f3x1zzd */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 3, 4, 7}},
   {{-1}}};

static Const fixer f3x1yyd = {
   sdmd,
   s3x1dmd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &f3x1yyd,                 /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */   /* f1x3yyd */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 3, 5, 7}},
   {{-1}}};

static Const fixer f1x3yyd = {
   sdmd,
   s1x3dmd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &f1x3yyd,                 /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */   /* f3x1yyd */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 3, 5, 7}},
   {{-1}}};

static Const fixer fxwv1d = {
   sdmd,
   s_crosswave,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &fxwv1d,                  /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 2, 4, 6}},
   {{-1}}};

static Const fixer fxwv2d = {
   sdmd,
   s_crosswave,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &fxwv2d,                  /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 3, 4, 7}},
   {{-1}}};

static Const fixer fxwv3d = {
   sdmd,
   s_crosswave,
   1,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &fxwv3d,                  /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{2, 5, 6, 1}},
   {{-1}}};

static Const fixer fspindlc = {
   s1x2,
   s_spindle,
   1,
   2,
   &fspindlc,                /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 6}, {2, 4}},
   {{-1}}};

static Const fixer fspindld = {
   s2x2,
   s_spindle,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &fspindld,                /* next2x2    */
   {{0, 2, 4, 6}},
   {{-1}}};

static Const fixer fspindlbd = {
   sdmd,
   s_spindle,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   &fspindlbd,               /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{7, 1, 3, 5}},
   {{-1}}};

static Const fixer foo55d = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &foo55d,                  /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &bar55d,                  /* next2x2    */
   {{0, 2, 4, 6}},
   {{-1}}};

static Const fixer fgalctb = {
   s2x2,
   s_galaxy,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &fgalctb,                 /* next2x2    */
   {{1, 3, 5, 7}},
   {{-1}}};

static Const fixer f3x1ctl = {
   s1x4,
   s3x1dmd,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f3x1ctl,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &fgalctb,                 /* next2x2    */
   {{1, 2, 5, 6}},
   {{-1}}};

static Const fixer foo99d = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &foo99d,                  /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4endd,                /* next2x2    */
   {{0, 3, 4, 7}},
   {{-1}}};

static Const fixer foo66d = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &foo66d,                  /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &bar55d,                  /* next2x2    */
   {{1, 2, 5, 6}},
   {{-1}}};

static Const fixer f1x8ctr = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8ctr,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &bar55d,                  /* next2x2    */
   {{3, 2, 7, 6}},
   {{-1}}};

static Const fixer fqtgctr = {
   s1x4,
   s_qtag,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &fqtgctr,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &bar55d,                  /* next2x2    */
   {{6, 7, 2, 3}},
   {{-1}}};

static Const fixer f1x8endd = {
   s1x4,
   s1x8,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8endd,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4endd,                /* next2x2    */
   {{0, 1, 4, 5}},
   {{-1}}};

static Const fixer f1x8endo = {
   s1x2,
   s1x8,
   0,
   2,
   &f1x8endo,                /* next1x2    */
   &fboneendo,               /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 1}, {5, 4}},
   {{-1}}};

static Const fixer fbonectr = {
   s1x4,
   s_bone,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &fbonectr,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &bar55d,                  /* next2x2    */
   {{6, 7, 2, 3}},
   {{-1}}};

static Const fixer fboneendd = {
   s2x2,
   s_bone,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8endd,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &fboneendd,               /* next2x2    */
   {{0, 1, 4, 5}},
   {{-1}}};

static Const fixer fboneendo = {
   s1x2,
   s_bone,
   1,
   2,
   &fboneendo,               /* next1x2    */
   &f1x8endo,                /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 5}, {1, 4}},
   {{-1}}};

static Const fixer frigendd = {
   s1x4,
   s_rigger,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &frigendd,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4endd,                /* next2x2    */
   {{6, 7, 2, 3}},
   {{-1}}};

static Const fixer frigctr = {
   s2x2,
   s_rigger,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f1x8ctr,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &frigctr,                 /* next2x2    */
   {{0, 1, 4, 5}},
   {{-1}}};


static Const fixer f2x4ctr = {
   s2x2,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &fbonectr,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4ctr,                 /* next2x2    */
   {{1, 2, 5, 6}},
   {{-1}}};

static Const fixer f2x4far = {    /* unsymmetrical */
   s1x4,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f2x4far,                 /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 1, 3, 2}},
   {{-1}}};

static Const fixer f2x4near = {   /* unsymmetrical */
   s1x4,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &f2x4near,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{7, 6, 4, 5}},
   {{-1}}};

static Const fixer f2x4left = {   /* unsymmetrical */
   s2x2,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4left,                /* next2x2    */
   {{0, 1, 6, 7}},
   {{-1}}};

static Const fixer f2x4right = {  /* unsymmetrical */
   s2x2,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4right,               /* next2x2    */
   {{2, 3, 4, 5}},
   {{-1}}};

static Const fixer f2x4endd = {
   s2x2,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   &frigendd,                /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   &f2x4endd,                /* next2x2    */
   {{0, 3, 4, 7}},
   {{-1}}};

static Const fixer f2x4endo = {
   s1x2,
   s2x4,
   1,
   2,
   &f2x4endo,                /* next1x2    */
   &f1x8endo,                /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 7}, {3, 4}},
   {{-1}}};



/* This should actually be some special thing that causes
   the setup not to be translated at all. */
static Const fixer bar55d = {
   s2x2,
   s2x4,
   0,
   1,
   (struct fixerjunk *) 0,   /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 2, 5, 6}},
   {{-1}}};

static Const fixer fppaad = {
   s1x2,
   s2x4,
   0,
   2,
   &fppaad,                  /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{1, 3}, {7, 5}},
   {{-1}}};

static Const fixer fpp55d = {
   s1x2,
   s2x4,
   0,
   2,
   &fpp55d,                  /* next1x2    */
   (struct fixerjunk *) 0,   /* next1x2rot */
   (struct fixerjunk *) 0,   /* next1x4    */
   (struct fixerjunk *) 0,   /* next1x4rot */
   (struct fixerjunk *) 0,   /* nextdmd    */
   (struct fixerjunk *) 0,   /* nextdmdrot */
   (struct fixerjunk *) 0,   /* next2x2    */
   {{0, 2}, {6, 4}},
   {{-1}}};



/* This does the various types of "so-and-so do this while the others do that" concepts. */

extern void so_and_so_only_move(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   setup_command subsid_cmd;
   setup_command *subsid_cmd_p;
   selector_kind saved_selector;
   int i, k;
volatile   int setupcount;    /* ******FUCKING DEBUGGER BUG!!!!!! */
   int crossconc;
   uint32 livemask[2];
   uint32 j;
   warning_info saved_warnings;
   calldef_schema schema;
   setup the_setups[2], the_results[2];

   uint32 ssmask;
   int sizem1 = setup_attrs[ss->kind].setup_limits;
   int indicator = parseptr->concept->value.arg1;
   long_boolean others = indicator & 1;
   indicator &= ~1;

/* arg1 = 0 - <> do your part
          1 - <> do your part while the others ....
          2 - own the <>, with the others not doing any call, which doesn't exist
          3 - own the <>, .... by ....
          4 - <>
          5 - <> while the others ....
          6 - <> disconnected
          7 - <> disconnected .... while the others ....
          8 - <> work <concept>, with the others not doing any call, which doesn't exist
          9 - <> work <concept> (the others do the call, but without the concept) */

   saved_selector = current_selector;
   current_selector = parseptr->selector;

   the_setups[0] = *ss;              /* designees */
   the_setups[1] = *ss;              /* non-designees */

   if (sizem1 < 0) fail("Can't identify people in this setup.");

   for (i=0, ssmask=0; i<=sizem1; i++) {
      ssmask <<= 1;
      if (ss->people[i].id1) {
         int q = 0;

         /* We allow the designators "centers" and "ends" while in a 1x8, which
            would otherwise not be allowed.  The reason we don't allow it in
            general is that people would carelessly say "centers kickoff" in a 1x8
            when they should realy say "each 1x4, centers kickoff".  But we assume
            that they will not misuse the term here. */

         if (ss->kind == s1x8 && current_selector == selector_centers) {
            if (i&2) q = 1;
         }
         else if (ss->kind == s1x8 && current_selector == selector_ends) {
            if (!(i&2)) q = 1;
         }
         else if (selectp(ss, i))
            q = 1;

         ssmask |= q;
         clear_person(&the_setups[q], i);
      }
   }

   current_selector = saved_selector;

   /* See if the user requested "centers" (or the equivalent people under some other
      designation), and just do it with the concentric_move stuff if so.
      The concentric_move stuff is much more sophisticated about a lot of things than
      what we would otherwise do. */

   if (indicator == 4 || indicator == 8) {
      cm_hunk *chunk = setup_attrs[ss->kind].conctab;
      uint32 mask = ~(~0 << (sizem1+1));

      if (others) {
         subsid_cmd = ss->cmd;
         subsid_cmd.parseptr = parseptr->subsidiary_root;
         subsid_cmd_p = &subsid_cmd;
      }
      else
         subsid_cmd_p = (setup_command *) 0;

      if (sizem1 == 3) {
         schema = schema_single_concentric;
      }
      else {
         schema = schema_concentric_6_2;
         if (parseptr->selector == selector_center6) goto do_concentric_ctrs;
         if (parseptr->selector == selector_outer2) goto do_concentric_ends;
         schema = schema_concentric_2_6;
         if (parseptr->selector == selector_center2) goto do_concentric_ctrs;
         if (parseptr->selector == selector_outer6) goto do_concentric_ends;
         schema = schema_concentric;
      }

      if (parseptr->selector == selector_centers) goto do_concentric_ctrs;
      if (parseptr->selector == selector_ends) goto do_concentric_ends;

      if (chunk &&
                (ss->kind != s3x4 ||
                      !(ss->people[1].id1 | ss->people[2].id1 | ss->people[7].id1 | ss->people[8].id1) &&
                        (ss->people[0].id1 & ss->people[3].id1 & ss->people[4].id1 & ss->people[5].id1 &
                        ss->people[6].id1 & ss->people[9].id1 & ss->people[10].id1 & ss->people[11].id1))) {
         if (chunk->mask_normal) {
            if (ssmask == chunk->mask_normal) goto do_concentric_ctrs;
            else if (ssmask == mask-chunk->mask_normal) goto do_concentric_ends;
         }

         if (chunk->mask_6_2) {
            schema = schema_concentric_6_2;
            if (ssmask == chunk->mask_6_2) goto do_concentric_ctrs;
            else if (ssmask == mask-chunk->mask_6_2) goto do_concentric_ends;
         }

         if (chunk->mask_2_6) {
            schema = schema_concentric_2_6;
            if (ssmask == chunk->mask_2_6) goto do_concentric_ctrs;
            else if (ssmask == mask-chunk->mask_2_6) goto do_concentric_ends;
         }
      }
   }

back_here:

   normalize_setup(&the_setups[0], (indicator == 4) ? normalize_before_merge : normalize_before_isolated_call);
   normalize_setup(&the_setups[1], (indicator == 4) ? normalize_before_merge : normalize_before_isolated_call);
   saved_warnings = history[history_ptr+1].warnings;

   /* It will be helpful to have a mask of where the live people are. */

   for (setupcount=0; setupcount<2; setupcount++) {
      for (i=0, j=1, livemask[setupcount] = 0; i<=setup_attrs[the_setups[setupcount].kind].setup_limits; i++, j<<=1) {
         if (the_setups[setupcount].people[i].id1) livemask[setupcount] |= j;
      }
   }

   /* Iterate 1 or 2 times, depending on whether the "other" people do a call. */

   for (setupcount=0; setupcount<=others; setupcount++) {
      uint32 thislivemask = livemask[setupcount];
      uint32 otherlivemask = livemask[setupcount^1];

      the_setups[setupcount].cmd = ss->cmd;
      the_setups[setupcount].cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
      if (setupcount == 1) {
         if (indicator == 8) {
            parse_block *parseptrcopy = skip_one_concept(parseptr->next);
            the_setups[setupcount].cmd.parseptr = parseptrcopy->next;
         }
         else
            the_setups[setupcount].cmd.parseptr = parseptr->subsidiary_root;
      }

      if (indicator >= 4 && indicator != 8) {
         Const fixer *fixp;
         int lilcount;
         int numsetups;
         setup lilsetup[4], lilresult[4];

         /* A few operations are independent of whether we said "disconnected",
            because the people are connected anyway. */

         if (the_setups[setupcount].kind == s1x8 && thislivemask == 0xCC)
            fixp = &f1x8ctr;
         else if (the_setups[setupcount].kind == s_qtag && thislivemask == 0xCC)
            fixp = &fqtgctr;
         else if (the_setups[setupcount].kind == s_bone && thislivemask == 0xCC)
            fixp = &fbonectr;
         else if (the_setups[setupcount].kind == s_rigger && thislivemask == 0x33)
            fixp = &frigctr;
         else if (the_setups[setupcount].kind == s3x1dmd && thislivemask == 0x66)
            fixp = &f3x1ctl;
         else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x66)
            fixp = &f2x4ctr;
         else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x0F)  /* unsymmetrical */
            fixp = &f2x4far;
         else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0xF0)  /* unsymmetrical */
            fixp = &f2x4near;
         else if (the_setups[setupcount].kind == s2x4 && the_setups[setupcount^1].kind == s2x4 && (thislivemask & ~0x0F) == 0 && (otherlivemask & 0x0F) == 0)
            fixp = &f2x4far;
         else if (the_setups[setupcount].kind == s2x4 && the_setups[setupcount^1].kind == s2x4 && (thislivemask & ~0xF0) == 0  && (otherlivemask & 0xF0) == 0)
            fixp = &f2x4near;
         else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0xC3)  /* unsymmetrical */
            fixp = &f2x4left;
         else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x3C)  /* unsymmetrical */
            fixp = &f2x4right;
         else if (indicator >= 6) {
            /* Search for "disconnected" stuff. */
            if (the_setups[setupcount].kind == s1x8 && thislivemask == 0xAA)
               fixp = &f1x8aad;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0x55)
               fixp = &foo55d;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0x99)
               fixp = &foo99d;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0x66)
               fixp = &foo66d;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0x33)
               fixp = &f1x8endd;
            else if (the_setups[setupcount].kind == s_bone && thislivemask == 0x33)
               fixp = &fboneendd;
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0xAA)
               fixp = &fppaad;
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x55)
               fixp = &fpp55d;
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x99)
               fixp = &f2x4endd;
            else if (the_setups[setupcount].kind == s_ptpd && thislivemask == 0xAA)
               fixp = &foozzd;
            else if (the_setups[setupcount].kind == s3x1dmd && thislivemask == 0x99)
               fixp = &f3x1zzd;
            else if (the_setups[setupcount].kind == s3x1dmd && thislivemask == 0xAA)
               fixp = &f3x1yyd;
            else if (the_setups[setupcount].kind == s1x3dmd && thislivemask == 0x99)
               fixp = &f1x3zzd;
            else if (the_setups[setupcount].kind == s1x3dmd && thislivemask == 0xAA)
               fixp = &f1x3yyd;
            else if (the_setups[setupcount].kind == s_crosswave && thislivemask == 0x55)
               fixp = &fxwv1d;
            else if (the_setups[setupcount].kind == s_crosswave && thislivemask == 0x99)
               fixp = &fxwv2d;
            else if (the_setups[setupcount].kind == s_crosswave && thislivemask == 0x66)
               fixp = &fxwv3d;
            else if (the_setups[setupcount].kind == s_spindle && thislivemask == 0x55)
               fixp = &fspindld;
            else if (the_setups[setupcount].kind == s_spindle && thislivemask == 0xAA)
               fixp = &fspindlbd;
            else
               fail("Can't do this with these people designated.");
         }
         else {
            /* Search for "so-and-so only" stuff. */
            if (thislivemask == 0) {
               /* Check for special case of no one. */
               the_results[setupcount].kind = nothing;
               the_results[setupcount].result_flags = 0;
               continue;
            }
            else if (thislivemask == ((1 << (setup_attrs[the_setups[setupcount].kind].setup_limits+1)) - 1)) {
               /* And special case of everyone. */
               move(&the_setups[setupcount], FALSE, &the_results[setupcount]);
               continue;
            }
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x33)
               fixp = &foo33;
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0xCC)
               fixp = &foocc;
            else if (the_setups[setupcount].kind == s2x4 && thislivemask == 0x99)
               fixp = &f2x4endo;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0xAA)
               fixp = &f1x8aa;
            else if (the_setups[setupcount].kind == s3x4 && thislivemask == 0x0C3)
               fixp = &f3x4left;
            else if (the_setups[setupcount].kind == s3x4 && thislivemask == 0x30C)
               fixp = &f3x4right;
            else if (the_setups[setupcount].kind == s2x6 && thislivemask == 0x0C3)
               fixp = &f3x4lzz;
            else if (the_setups[setupcount].kind == s2x6 && thislivemask == 0xC30)
               fixp = &f3x4rzz;
            else if (the_setups[setupcount].kind == s1x8 && thislivemask == 0x33)
               fixp = &f1x8endo;
            else if (the_setups[setupcount].kind == s_bone && thislivemask == 0x33)
               fixp = &fboneendo;
            else if (the_setups[setupcount].kind == s_ptpd && thislivemask == 0xAA)
               fixp = &foozz;
            else if (the_setups[setupcount].kind == s_spindle && thislivemask == 0x55)
               fixp = &fspindlc;
            else
               fail("Can't do this with these people designated.");
         }

         numsetups = fixp->numsetups & 0xFF;

         for (lilcount=0; lilcount<numsetups; lilcount++) {
            lilsetup[lilcount].cmd = the_setups[setupcount].cmd;
            lilsetup[lilcount].kind = fixp->ink;
            lilsetup[lilcount].rotation = 0;
            for (k=0; k<=setup_attrs[fixp->ink].setup_limits; k++)
               (void) copy_rot(&lilsetup[lilcount], k, &the_setups[setupcount], fixp->nonrot[lilcount][k], 011*((-fixp->rot) & 3));
            lilsetup[lilcount].cmd.cmd_assume.assumption = cr_none;
            move(&lilsetup[lilcount], FALSE, &lilresult[lilcount]);
         }

         if (fix_n_results(numsetups, lilresult)) goto lose;

         clear_people(&the_results[setupcount]);
         the_results[setupcount].result_flags = get_multiple_parallel_resultflags(lilresult, numsetups);

         if (lilresult[0].rotation != 0) {
            Const fixer *nextfixp;

            the_results[setupcount].kind = fixp->outk;
            the_results[setupcount].rotation = 0;

            if (lilresult[0].kind != fixp->ink) goto lose;

            if (lilresult[0].kind == s1x2)
               nextfixp = fixp->next1x2rot;
            else if (lilresult[0].kind == s1x4)
               nextfixp = fixp->next1x4rot;
            else if (lilresult[0].kind == sdmd)
               nextfixp = fixp->nextdmdrot;
            else
               nextfixp = 0;    /* Raise an error. */

            if (nextfixp) {
               if (fixp->rot == nextfixp->rot) {
                  the_results[setupcount].rotation--;

                  if (fixp->numsetups & 0x100) {
                     the_results[setupcount].rotation += 2;
                     lilresult[0].rotation += 2;
                     lilresult[1].rotation += 2;
                     canonicalize_rotation(&lilresult[0]);
                     canonicalize_rotation(&lilresult[1]);
                  }
               }

               fixp = nextfixp;
               the_results[setupcount].kind = fixp->outk;

               if (fixp->rot == 0) {
                  lilresult[0].rotation += 2;
                  lilresult[1].rotation += 2;
                  canonicalize_rotation(&lilresult[0]);
                  canonicalize_rotation(&lilresult[1]);
               }

               for (lilcount=0; lilcount<numsetups; lilcount++) {
                  for (k=0; k<=setup_attrs[lilresult[0].kind].setup_limits; k++)
                     (void) copy_rot(&the_results[setupcount], fixp->nonrot[lilcount][k], &lilresult[lilcount], k, 011*fixp->rot);
               }
            }
            else {
               if (fixp->yesrot[0][0] < 0) fail("Can't do this call with these people.");
               the_results[setupcount].rotation++;
               for (lilcount=0; lilcount<numsetups; lilcount++) {
                  for (k=0; k<=setup_attrs[lilresult[0].kind].setup_limits; k++)
                     (void) copy_rot(&the_results[setupcount], fixp->yesrot[lilcount][k], &lilresult[lilcount], k, 011*fixp->rot);
               }
            }
         }
         else {
            if (lilresult[0].kind == s1x2)
               fixp = fixp->next1x2;
            else if (lilresult[0].kind == s1x4)
               fixp = fixp->next1x4;
            else if (lilresult[0].kind == sdmd)
               fixp = fixp->nextdmd;
            else if (lilresult[0].kind == s2x2)
               fixp = fixp->next2x2;
            else
               fixp = 0;    /* Raise an error. */

            if (!fixp) goto lose;

            the_results[setupcount].kind = fixp->outk;
            the_results[setupcount].rotation = 0;

            for (lilcount=0; lilcount<numsetups; lilcount++) {
               for (k=0; k<=setup_attrs[lilresult[0].kind].setup_limits; k++)
                  (void) copy_rot(&the_results[setupcount], fixp->nonrot[lilcount][k], &lilresult[lilcount], k, 011*fixp->rot);
            }
         }

         reinstate_rotation(&the_setups[setupcount], &the_results[setupcount]);
      }
      else
         move(&the_setups[setupcount], FALSE, &the_results[setupcount]);
   }

   if (!others) {      /* The non-designees did nothing. */
      the_results[1] = the_setups[1];
       /* Give the people who didn't move the same result flags as those who did.
         This is important for the "did last part" check. */
      the_results[1].result_flags = the_results[0].result_flags;
   }

   /* Shut off "each 1x4" types of warnings -- they will arise spuriously while
      the people do the calls in isolation. */
   history[history_ptr+1].warnings.bits[0] &= ~dyp_each_warnings.bits[0];
   history[history_ptr+1].warnings.bits[1] &= ~dyp_each_warnings.bits[1];
   history[history_ptr+1].warnings.bits[0] |= saved_warnings.bits[0];
   history[history_ptr+1].warnings.bits[1] |= saved_warnings.bits[1];

   *result = the_results[1];
   result->result_flags = get_multiple_parallel_resultflags(the_results, 2);

   /* For "own the <anyone>", we use strict matrix spots for the merge.
      Otherwise, we allow a little breathing. */

   merge_setups(
      &the_results[0],
      indicator == 2 ? merge_strict_matrix :
         (indicator == 6 ? merge_without_gaps : merge_c1_phantom),
      result);

   return;

   lose: fail("Can't do this call with these people.");


   do_concentric_ctrs:

   crossconc = 0;

   if (indicator == 8) goto forward_here;

   concentric_move(ss, &ss->cmd, subsid_cmd_p,
            schema, 0, 0, result);
   return;

   do_concentric_ends:

   crossconc = 1;

   if (indicator == 8) goto forward_here;

   concentric_move(ss, subsid_cmd_p, &ss->cmd,
            schema, 0, 0, result);
   return;

   forward_here:

   {
      cm_hunk *chunk = setup_attrs[ss->kind].conctab;

      switch (schema) {
         case schema_concentric_2_6:
            ssmask = chunk->mask_2_6;
            break;
         case schema_concentric_6_2:
            ssmask = chunk->mask_6_2;
            break;
         default:
            ssmask = chunk->mask_normal;
            break;
      }

      if (!ssmask) goto back_here;    /* We don't know how to find centers and ends. */

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE)
         fail("Can't stack \"centers/ends work <concept>\" concepts.");

      ss->cmd.cmd_misc2_flags |= CMD_MISC2__CTR_USE;
      ss->cmd.cmd_misc2_flags &= ~(0xFFFF | CMD_MISC2__CTR_USE_INVERT);
      ss->cmd.cmd_misc2_flags |= (0xFFFFUL & ((int) schema));
      if (crossconc) ss->cmd.cmd_misc2_flags |= CMD_MISC2__CTR_USE_INVERT;

      move(ss, FALSE, result);
   }
}
