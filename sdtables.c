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

/* This defines the following external variables:
   getout_strings
   filename_strings
   level_threshholds
   menu_names
   map_spec_star12
   map_spec_star12v
   map_spec_star16
   map_spec_star16v
   map2x4_2x4
   map2x4_2x4v
   mapgnd1x2_1x2
   mapgnd1x2_1x2r
   conc_init_table
   setup_attrs
   begin_sizes
   startinfolist
   map_b6_trngl
   map_s6_trngl
   map_bone_trngl4
   map_rig_trngl4
   map_s8_tgl4
   map_p8_tgl4
   map_2x2v
   map_2x4_magic
   map_qtg_magic
   map_qtg_intlk
   map_qtg_magic_intlk
   map_ptp_magic
   map_ptp_intlk
   map_ptp_magic_intlk
   map_2x4_diagonal
   map_2x4_int_pgram
   map_2x4_trapezoid
   map_3x4_2x3_intlk
   map_3x4_2x3_conc
   map_4x4_ns
   map_4x4_ew
   map_phantom_box
   map_intlk_phantom_box
   map_phantom_dmd
   map_intlk_phantom_dmd
   map_split_f
   map_intlk_f
   map_full_f
   map_stagger
   map_stairst
   map_ladder
   map_but_o
   map_o_s2x4_3
   map_x_s2x4_3
   map_offset
   map_4x4v
   map_blocks
   map_trglbox
   map_hv_2x4_2
   map_3x4_2x3
   map_4x6_2x4
   map_hv_qtg_2
   map_vv_qtg_2
   map_tgl4_1
   map_tgl4_2
   map_2x6_2x3
   map_dbloff1
   map_dbloff2
   map_trngl_box1
   map_trngl_box2
   map_inner_box
   map_lh_s2x3_3
   map_lh_s2x3_2
   map_rh_s2x3_3
   map_rh_s2x3_2
   map_lz12
   map_rz12
   map_dmd_1x1
   map_star_1x1
   map_qtag_f0
   map_qtag_f1
   map_qtag_f2
   map_diag2a
   map_diag2b
   map_f2x8_4x4
   map_w4x4_4x4
   map_f2x8_2x8
   map_w4x4_2x8
   maps_3diag
   maps_3diagwk
   map_lists
*/

#include "sd.h"



/* These are used in setup_coords. */

Private coordrec thing1x1 = {s1x1, 3,
   {  0},
   {  0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  0, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x2 = {s1x2, 3,
   { -2,   2},
   {  0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x3 = {s1x3, 3,
   { -4,   0,   4},
   {  0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1,  2, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x4 = {s1x4, 3,
   { -6,  -2,   6,   2},
   {  0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  3,  2, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x6 = {s1x6, 3,
   {-10,  -6,  -2,  10,   6,   2},
   {  0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  5,  4,  3, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x8 = {s1x8, 3,
   {-14, -10,  -2,  -6,  14,  10,   2,   6},
   {  0,   0,   0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  3,  2,  6,  7,  5,  4,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x10 = {s1x10, 4,
   {-18, -14, -10,  -6,  -2,  18,  14,  10,   6,   2},
   {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1,  0,
       1,  2,  3,  4,  9,  8,  7,  6,
       5, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x12 = {s1x12, 4,
   {-22, -18, -14, -10,  -6,  -2,  22,  18,  14,  10,   6,   2},
   {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1,  0,  1,
       2,  3,  4,  5, 11, 10,  9,  8,
       7,  6, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x14 = {s1x14, 4,
   {-26, -22, -18, -14, -10,  -6,  -2,  26,  22,  18,  14,  10,   6,   2},
   {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1,  0,  1,  2,
       3,  4,  5,  6, 13, 12, 11, 10,
       9,  8,  7, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x16 = {s1x16, 4,
   {-30, -26, -22, -18, -14, -10,  -6,  -2,  30,  26,  22,  18,  14,  10,   6,   2},
   {  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  0,  1,  2,  3,
       4,  5,  6,  7, 15, 14, 13, 12,
      11, 10,  9,  8, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing2x2 = {s2x2, 3,
   { -2,   2,   2,  -2},
   {  2,   2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1, -1, -1, -1,
      -1, -1, -1,  3,  2, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing2x3 = {s2x3, 3,
   { -4,   0,   4,   4,   0,  -4},
   {  2,   2,   2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1,  2, -1, -1,
      -1, -1, -1,  5,  4,  3, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing2x4 = {s2x4, 3,
   { -6,  -2,   2,   6,   6,   2,  -2,  -6},
   {  2,   2,   2,   2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3, -1, -1,
      -1, -1,  7,  6,  5,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing2x6 = {s2x6, 3,
   {-10,  -6,  -2,   2,   6,  10,  10,   6,   2,  -2,  -6,  -10},
   {  2,   2,   2,   2,   2,   2,  -2,  -2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  3,  4,  5, -1,
      -1, 11, 10,  9,  8,  7,  6, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing2x8 = {s2x8, 3,
   {-14, -10,  -6,  -2,   2,   6,  10,  14,  14,  10,   6,   2,  -2,  -6, -10, -14},
   {  2,   2,   2,   2,   2,   2,   2,   2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  2,  3,  4,  5,  6,  7,
      15, 14, 13, 12, 11, 10,  9,  8,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing3x8 = {s3x8, 3,
   {-14, -10,  -6,  -2,   2,   6,  10,  14,  14,  10,   6,   2,  14,  10,   6,   2,  -2,  -6, -10, -14, -14, -10,  -6,  -2},
   {  4,   4,   4,   4,   4,   4,   4,   4,   0,   0,   0,   0,  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  2,  3,  4,  5,  6,  7,
      20, 21, 22, 23, 11, 10,  9,  8,
      19, 18, 17, 16, 15, 14, 13, 12,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing3x4 = {s3x4, 3,
   { -6,  -2,   2,   6,   6,   2,   6,   2,  -2,  -6,  -6,  -2},
   {  4,   4,   4,   4,   0,   0,  -4,  -4,  -4,  -4,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3, -1, -1,
      -1, -1, 10, 11,  5,  4, -1, -1,
      -1, -1,  9,  8,  7,  6, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thinggal = {s_galaxy, 3,
   { -7,  -2,   0,   2,   7,   2,   0,  -2},
   {  0,   2,   7,   2,   0,  -2,  -7,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  2, -1, -1, -1,
      -1, -1,  0,  1,  3,  4, -1, -1,
      -1, -1, -1,  7,  5, -1, -1, -1,
      -1, -1, -1, -1,  6, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingthar = {s_thar, 3,
   { -9,  -5,   0,   0,   9,   5,   0,   0},
   {  0,   0,   9,   5,   0,   0,  -9,  -5}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  2, -1, -1, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1,  0,  1, -1, -1,  5,  4, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1, -1, -1,  6, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingxwv = {s_crosswave, 3,
   { -9,  -5,   0,   0,   9,   5,   0,   0},
   {  0,   0,   6,   2,   0,   0,  -6,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  2, -1, -1, -1,
      -1,  0,  1, -1,  3,  5,  4, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1, -1, -1,  6, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingrigger = {s_rigger, 3,
   { -2,   2,  10,   6,   2,  -2, -10,  -6},
   {  2,   2,   0,   0,  -2,  -2,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  6,  7,  0,  1,  3,  2, -1,
      -1, -1, -1,  5,  4, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingspindle = {s_spindle, 3,
   { -4,   0,   4,   8,   4,   0,  -4,  -8},
   {  2,   2,   2,   0,  -2,  -2,  -2,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  7,  0,  1,  2,  3, -1,
      -1, -1, -1,  6,  5,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingbone = {s_bone, 3,
   {-10,  10,   6,   2,  10, -10,  -6,  -2},
   {  2,   2,   0,   0,  -2,  -2,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  6,  7,  3,  2,  1, -1,
      -1,  5, -1, -1, -1, -1,  4, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingqtag = {s_qtag, 3,       /* "fudged" coordinates */
   { -4,   5,   6,   2,   4,  -5,  -6,  -2},
   {  5,   5,   0,   0,  -5,  -5,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0, -1,  1, -1, -1,
      -1, -1,  6,  7,  3,  2, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  5, -1, -1,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec nicethingqtag = {s_qtag, 3,   /* "true" coordinates */
   { -5,   5,   6,   2,   5,  -5,  -6,  -2},
   {  5,   5,   0,   0,  -5,  -5,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1, -1,  1, -1, -1,
      -1, -1,  6,  7,  3,  2, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  5, -1, -1,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingptpd = {s_ptpd, 3,       /* "fudged" coordinates */
   { -9,  -6,  -2,  -6,   9,   6,   2,   6},
   {  1,   1,   0,  -3,  -1,  -1,   0,   3}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  6,  7, -1, -1,
      -1, -1,  3, -1, -1,  5,  4, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec nicethingptpd = {s_ptpd, 3,       /* "true" coordinates */
   {-10,  -6,  -2,  -6,  10,   6,   2,   6},
   {  0,   2,   0,  -2,   0,  -2,   0,   2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  6,  7,  4, -1,
      -1, -1,  3, -1, -1,  5, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingbigdmd = {sbigdmd, 3,   /* used for both -- symmetric and safe for press/truck */
   { -9,  -5,   0,   0,   5,   9,   9,   5,   0,   0,  -5,  -9},
   {  5,   5,   6,   2,   5,   5,  -5,  -5,  -6,  -2,  -5,  -5}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1, -1,  2,  4,  5, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1, -1, -1, -1,  9, -1, -1, -1,
      -1, 11, 10, -1,  8,  7,  6, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing1x3dmd = {s1x3dmd, 3,   /* used for both -- symmetric and safe for press/truck */
   {-13,  -9,  -5,   0,  13,   9,   5,   0},
   {  0,   0,   0,   2,   0,   0,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  2, -1,  3,  6,  5,  4,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing3x1dmd = {s3x1dmd, 3,   /* used for both -- symmetric and safe for press/truck */
   {-10,  -6,  -2,   0,  10,   6,   2,   0},
   {  0,   0,   0,   5,   0,   0,   0,  -5}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1,  0,  1,  2,  6,  5,  4, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing3dmd = {s3dmd, 3,   /* used for both -- symmetric and safe for press/truck */
   { -7,   0,   7,  10,   6,   2,   7,   0,  -7, -10,  -6,  -2},
   {  5,   5,   5,   0,   0,   0,  -5,  -5,  -5,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1,  1,  2, -1, -1,
      -1,  9, 10, 11,  5,  4,  3, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  8, -1,  7,  6, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing4dmd = {s4dmd, 3,   /* used for both -- symmetric and safe for press/truck */
   {-11,  -5,   5,  11,  14,  10,   6,   2,  11,   5,  -5, -11, -14, -10,  -6,  -2},
   {  5,   5,   5,   5,   0,   0,   0,   0,  -5,  -5,  -5,  -5,   0,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1, -1, -1,  2,  3, -1,
      12, 13, 14, 15,  7,  6,  5,  4,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, 11, 10, -1, -1,  9,  8, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec nicethingdmd = {sdmd, 3,          /* "true" coordinates -- there is no "fudged" version */
   { -4,   0,   4,   0},
   {  0,   4,   0,  -4}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  1, -1, -1, -1,
      -1, -1, -1,  0, -1,  2, -1, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec nicethingshort6 = {s_short6, 3,   /* "true" coordinates -- there is no "fudged" version */
   { -2,   0,   2,   2,   0,  -2},
   {  2,   6,   2,  -2,  -6,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  1, -1, -1, -1,
      -1, -1, -1,  0,  2, -1, -1, -1,
      -1, -1, -1,  5,  3, -1, -1, -1,
      -1, -1, -1, -1,  4, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec nicethingphan = {s_c1phan, 3,   /* "true" coordinates for mirroring --
                                   actually, the truck distances are greatly exaggerated. */
   {-10,  -6,  -2,  -6,   6,  10,   6,   2,  10,   6,   2,   6,  -6, -10,  -6,  -2},
   {  6,  10,   6,   2,  10,   6,   2,   6,  -6, -10,  -6,  -2, -10,  -6,  -2,  -6}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  1, -1, -1,  4, -1, -1,
      -1,  0, -1,  2,  7, -1,  5, -1,
      -1, -1,  3, -1, -1,  6, -1, -1,
      -1, -1, 14, -1, -1, 11, -1, -1,
      -1, 13, -1, 15, 10, -1,  8, -1,
      -1, -1, 12, -1, -1,  9, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingphan = {s_c1phan, 3,   /* "fudged" coordinates -- only truck within each 1x2 is allowed */
   { -6,  -3,  -2,  -3,   4,  15,   4,  11,   7,   4,   3,   4,  -5, -14,  -5, -10},
   {  4,  13,   4,   9,   7,   4,   3,   4,  -5, -14,  -5, -10,  -6,  -7,  -2,  -7}, {
      -1, -1, -1,  1, -1, -1, -1, -1,
      -1, -1, -1,  3, -1, -1, -1, -1,
      -1, -1,  0,  2, -1,  4,  7,  5,
      -1, -1, -1, -1, -1,  6, -1, -1,
      -1, -1, 14, -1, -1, -1, -1, -1,
      13, 15, 12, -1, 10,  8, -1, -1,
      -1, -1, -1, -1, -1, 11, -1, -1,
      -1, -1, -1, -1, -1,  9, -1, -1}};

Private coordrec nicethingglass = {s_hrglass, 3,   /* "true" coordinates for mirroring --
                                   actually, the truck distances are greatly exaggerated. */
   { -5,   5,   5,   0,   5,  -5,  -5,   0},
   {  5,   5,   0,   2,  -5,  -5,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1, -1,  1, -1, -1,
      -1, -1,  6, -1,  3,  2, -1, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1,  5, -1, -1,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingglass = {s_hrglass, 3,   /* "fudged" coordinates -- only truck by center 2 is allowed */
   { -4,   5,   5,   0,   4,  -5,  -5,   0},
   {  5,   5,   0,   2,  -5,  -5,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0, -1,  1, -1, -1,
      -1, -1,  6, -1,  3,  2, -1, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1,  5, -1, -1,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingdglass = {s_dhrglass, 3,   /* used for both -- symmetric and safe for press/truck */
   {-10,  10,   5,   0,  10, -10,  -5,   0},
   {  2,   2,   0,   2,  -2,  -2,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  6, -1,  3,  2,  1, -1,
      -1,  5, -1, -1,  7, -1,  4, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing4x4 = {s4x4, 3,
   {  6,   6,   6,   2,   6,   2,  -2,   2,  -6,  -6,  -6,  -2,  -6,  -2,   2,  -2},
   {  6,   2,  -2,   2,  -6,  -6,  -6,  -2,  -6,  -2,   2,  -2,   6,   6,   6,   2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, 12, 13, 14,  0, -1, -1,
      -1, -1, 10, 15,  3,  1, -1, -1,
      -1, -1,  9, 11,  7,  2, -1, -1,
      -1, -1,  8,  6,  5,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thing4x6 = {s4x6, 3,
   {-10,  -6,  -2,   2,   6,  10,  10,   6,   2,  -2,  -6, -10,  10,   6,   2,  -2,  -6, -10, -10,  -6,  -2,   2,   6,  10},
   {  6,   6,   6,   6,   6,   6,   2,   2,   2,   2,   2,   2,  -6,  -6,  -6,  -6,  -6,  -6,  -2,  -2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  3,  4,  5, -1,
      -1, 11, 10,  9,  8,  7,  6, -1,
      -1, 18, 19, 20, 21, 22, 23, -1,
      -1, 17, 16, 15, 14, 13, 12, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

Private coordrec thingblob = {s_bigblob, 3,
   { -2,   2,   2,   6,   6,   2,  10,  10,   6,   6,   2,   2,   2,  -2,  -2,  -6,  -6,  -2, -10, -10,  -6,  -6,  -2,  -2},
   { 10,  10,   6,   6,   2,   2,   2,  -2,  -2,  -6,  -6,  -2, -10, -10,  -6,  -6,  -2,  -2,  -2,   2,   2,   6,   6,   2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1, -1, -1, -1,
      -1, -1, 21, 22,  2,  3, -1, -1,
      -1, 19, 20, 23,  5,  4,  6, -1,
      -1, 18, 16, 17, 11,  8,  7, -1,
      -1, -1, 15, 14, 10,  9, -1, -1,
      -1, -1, -1, 13, 12, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};



/* BEWARE!!  This list is keyed to the definition of "dance_level" in database.h . */
Cstring getout_strings[] = {
   "Mainstream",
   "Plus",
   "A1",
   "A2",
   "C1",
   "C2",
   "C3A",
   "C3",
   "C3X",
   "C4A",
   "C4",
   "all",
   ""};

/* BEWARE!!  This list is keyed to the definition of "dance_level" in database.h . */
Cstring filename_strings[] = {
   ".MS",
   ".Plus",
   ".A1",
   ".A2",
   ".C1",
   ".C2",
   ".C3A",
   ".C3",
   ".C3X",
   ".C4A",
   ".C4",
   ".all",
   ""};

/* BEWARE!!  This list is keyed to the definition of "dance_level" in database.h . */
dance_level level_threshholds[] = {
   l_mainstream,
   l_plus,
   l_a1,
   l_a1,
   l_c1,
   l_c2,
   l_c3a,
   l_c3a,
   l_c3a,
   l_c3x,
   l_c3x,
   l_dontshow,
   l_nonexistent_concept};

/* BEWARE!!  This list is keyed to the definition of "call_list_kind" in sd.h . */
Cstring menu_names[] = {
   "???",
   "???",
   "(any setup)",
   "grand wave",
   "left grand wave",
   "dpt",
   "cdpt",
   "column",
   "left column",
   "8 chain",
   "trade by",
   "facing lines",
   "lines-out",
   "waves",
   "left waves",
   "2-faced lines",
   "left 2-faced lines",
   "grand column",
   "diamond/qtag"};


#define ID2_BOX0 (ID2_LEAD|ID2_BEAU)
#define ID2_BOX1 (ID2_LEAD|ID2_BELLE)
#define ID2_BOX2 (ID2_TRAILER|ID2_BELLE)
#define ID2_BOX3 (ID2_TRAILER|ID2_BEAU)



Private id_bit_table id_bit_table_1x2[] = {
   {ID2_BEAU,              ID2_TRAILER,           ID2_BELLE,             ID2_LEAD},
   {ID2_BELLE,             ID2_LEAD,              ID2_BEAU,              ID2_TRAILER}};

Private id_bit_table id_bit_table_2x2[] = {
   {ID2_BOX0, ID2_BOX3, ID2_BOX2, ID2_BOX1},
   {ID2_BOX1, ID2_BOX0, ID2_BOX3, ID2_BOX2},
   {ID2_BOX2, ID2_BOX1, ID2_BOX0, ID2_BOX3},
   {ID2_BOX3, ID2_BOX2, ID2_BOX1, ID2_BOX0}};

Private id_bit_table id_bit_table_2x4[] = {
   {ID2_BOX0|ID2_END,        ID2_BOX3|ID2_END,     ID2_BOX2|ID2_END,    ID2_BOX1|ID2_END},
   {ID2_BOX1|ID2_CENTER,    ID2_BOX0|ID2_CENTER,     ID2_BOX3|ID2_CENTER,  ID2_BOX2|ID2_CENTER},
   {ID2_BOX0|ID2_CENTER,     ID2_BOX3|ID2_CENTER,  ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER},
   {ID2_BOX1|ID2_END,       ID2_BOX0|ID2_END,        ID2_BOX3|ID2_END,     ID2_BOX2|ID2_END},
   {ID2_BOX2|ID2_END,    ID2_BOX1|ID2_END,       ID2_BOX0|ID2_END,        ID2_BOX3|ID2_END},
   {ID2_BOX3|ID2_CENTER,  ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER,    ID2_BOX0|ID2_CENTER},
   {ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER,    ID2_BOX0|ID2_CENTER,     ID2_BOX3|ID2_CENTER},
   {ID2_BOX3|ID2_END,     ID2_BOX2|ID2_END,    ID2_BOX1|ID2_END,       ID2_BOX0|ID2_END}};

Private id_bit_table id_bit_table_2x6p[] = {
   {ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS},
   {ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS},
   {ID2_BOX0|ID2_CTR4,      ID2_BOX3|ID2_CTR4,      ID2_BOX2|ID2_CTR4,      ID2_BOX1|ID2_CTR4},
   {ID2_BOX1|ID2_CTR4,      ID2_BOX0|ID2_CTR4,      ID2_BOX3|ID2_CTR4,      ID2_BOX2|ID2_CTR4},
   {ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS},
   {ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS},
   {ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS},
   {ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS},
   {ID2_BOX2|ID2_CTR4,      ID2_BOX1|ID2_CTR4,      ID2_BOX0|ID2_CTR4,      ID2_BOX3|ID2_CTR4},
   {ID2_BOX3|ID2_CTR4,      ID2_BOX2|ID2_CTR4,      ID2_BOX1|ID2_CTR4,      ID2_BOX0|ID2_CTR4},
   {ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS, ID2_BOX3|ID2_OUTRPAIRS},
   {ID2_BOX3|ID2_OUTRPAIRS, ID2_BOX2|ID2_OUTRPAIRS, ID2_BOX1|ID2_OUTRPAIRS, ID2_BOX0|ID2_OUTRPAIRS}};

Private id_bit_table id_bit_table_1x4[] = {
   {ID2_BEAU|ID2_END,        ID2_TRAILER|ID2_END,      ID2_BELLE|ID2_END,       ID2_LEAD|ID2_END},
   {ID2_BELLE|ID2_CENTER,    ID2_LEAD|ID2_CENTER,      ID2_BEAU|ID2_CENTER,     ID2_TRAILER|ID2_CENTER},
   {ID2_BELLE|ID2_END,       ID2_LEAD|ID2_END,         ID2_BEAU|ID2_END,        ID2_TRAILER|ID2_END},
   {ID2_BEAU|ID2_CENTER,     ID2_TRAILER|ID2_CENTER,   ID2_BELLE|ID2_CENTER,    ID2_LEAD|ID2_CENTER}};

Private id_bit_table id_bit_table_dmd[] = {
   {ID2_END,                  ID2_END,                 ID2_END,                ID2_END},
   {ID2_LEAD|ID2_CENTER,      ID2_BEAU|ID2_CENTER,     ID2_TRAILER|ID2_CENTER, ID2_BELLE|ID2_CENTER},
   {ID2_END,                  ID2_END,                 ID2_END,                ID2_END},
   {ID2_TRAILER|ID2_CENTER,   ID2_BELLE|ID2_CENTER,    ID2_LEAD|ID2_CENTER,    ID2_BEAU|ID2_CENTER}};

Private id_bit_table id_bit_table_1x8[] = {
   {ID2_BEAU|ID2_OUTR6|ID2_OUTR2,   ID2_TRAILER|ID2_OUTR6|ID2_OUTR2, ID2_BELLE|ID2_OUTR6|ID2_OUTR2,  ID2_LEAD|ID2_OUTR6|ID2_OUTR2},
   {ID2_BELLE|ID2_OUTR6|ID2_CTR6,   ID2_LEAD|ID2_OUTR6|ID2_CTR6,     ID2_BEAU|ID2_OUTR6|ID2_CTR6,    ID2_TRAILER|ID2_OUTR6|ID2_CTR6},
   {ID2_BELLE|ID2_CTR2|ID2_CTR6,    ID2_LEAD|ID2_CTR2|ID2_CTR6,      ID2_BEAU|ID2_CTR2|ID2_CTR6,     ID2_TRAILER|ID2_CTR2|ID2_CTR6},
   {ID2_BEAU|ID2_OUTR6|ID2_CTR6,    ID2_TRAILER|ID2_OUTR6|ID2_CTR6,  ID2_BELLE|ID2_OUTR6|ID2_CTR6,   ID2_LEAD|ID2_OUTR6|ID2_CTR6},
   {ID2_BELLE|ID2_OUTR6|ID2_OUTR2,  ID2_LEAD|ID2_OUTR6|ID2_OUTR2,    ID2_BEAU|ID2_OUTR6|ID2_OUTR2,   ID2_TRAILER|ID2_OUTR6|ID2_OUTR2},
   {ID2_BEAU|ID2_OUTR6|ID2_CTR6,    ID2_TRAILER|ID2_OUTR6|ID2_CTR6,  ID2_BELLE|ID2_OUTR6|ID2_CTR6,   ID2_LEAD|ID2_OUTR6|ID2_CTR6},
   {ID2_BEAU|ID2_CTR2|ID2_CTR6,     ID2_TRAILER|ID2_CTR2|ID2_CTR6,   ID2_BELLE|ID2_CTR2|ID2_CTR6,    ID2_LEAD|ID2_CTR2|ID2_CTR6},
   {ID2_BELLE|ID2_OUTR6|ID2_CTR6,   ID2_LEAD|ID2_OUTR6|ID2_CTR6,     ID2_BEAU|ID2_OUTR6|ID2_CTR6,    ID2_TRAILER|ID2_OUTR6|ID2_CTR6}};

Private id_bit_table id_bit_table_qtag[] = {
   {ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6},
   {ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2},
   {ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6},
   {ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6,    ID2_END|ID2_OUTRPAIRS|ID2_CTR6|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6,     ID2_CENTER|ID2_CTR4|ID2_OUTR2|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2,       ID2_CENTER|ID2_CTR4|ID2_CTR6|ID2_CTR2}};

Private id_bit_table id_bit_table_ptpd[] = {
   {ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2,           ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2},
   {ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU,   ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE},
   {ID2_CTR2|ID2_CTR6|ID2_BEAU,     ID2_CTR2|ID2_CTR6|ID2_TRAILER, ID2_CTR2|ID2_CTR6|ID2_BELLE,    ID2_CTR2|ID2_CTR6|ID2_LEAD},
   {ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE,  ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU},
   {ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2,           ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2},
   {ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE,  ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU},
   {ID2_CTR2|ID2_CTR6|ID2_BELLE,    ID2_CTR2|ID2_CTR6|ID2_LEAD,    ID2_CTR2|ID2_CTR6|ID2_BEAU,     ID2_CTR2|ID2_CTR6|ID2_TRAILER},
   {ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU,   ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE}};

Private id_bit_table id_bit_table_crosswave[] = {
   {ID2_END|ID2_OUTRPAIRS|ID2_BEAU|ID2_OUTR6,       ID2_END|ID2_OUTRPAIRS|ID2_TRAILER|ID2_OUTR6,  ID2_END|ID2_OUTRPAIRS|ID2_BELLE|ID2_OUTR6,      ID2_END|ID2_OUTRPAIRS|ID2_LEAD|ID2_OUTR6},
   {ID2_END|ID2_OUTRPAIRS|ID2_BELLE|ID2_OUTR6,      ID2_END|ID2_OUTRPAIRS|ID2_LEAD|ID2_OUTR6,     ID2_END|ID2_OUTRPAIRS|ID2_BEAU|ID2_OUTR6,       ID2_END|ID2_OUTRPAIRS|ID2_TRAILER|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_OUTR6,    ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_OUTR6,  ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_OUTR6, ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_CTR2,  ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_CTR2,  ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_CTR2,     ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_CTR2},
   {ID2_END|ID2_OUTRPAIRS|ID2_BELLE|ID2_OUTR6,      ID2_END|ID2_OUTRPAIRS|ID2_LEAD|ID2_OUTR6,     ID2_END|ID2_OUTRPAIRS|ID2_BEAU|ID2_OUTR6,       ID2_END|ID2_OUTRPAIRS|ID2_TRAILER|ID2_OUTR6},
   {ID2_END|ID2_OUTRPAIRS|ID2_BEAU|ID2_OUTR6,       ID2_END|ID2_OUTRPAIRS|ID2_TRAILER|ID2_OUTR6,  ID2_END|ID2_OUTRPAIRS|ID2_BELLE|ID2_OUTR6,      ID2_END|ID2_OUTRPAIRS|ID2_LEAD|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_OUTR6, ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_OUTR6, ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_OUTR6,    ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_CTR2,     ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_CTR2,   ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_CTR2,  ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_CTR2}};

Private id_bit_table id_bit_table_gal[] = {
   {ID2_END,             ID2_END,             ID2_END,             ID2_END},
   {ID2_BOX0|ID2_CENTER, ID2_BOX3|ID2_CENTER, ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER},
   {ID2_END,             ID2_END,             ID2_END,             ID2_END},
   {ID2_BOX1|ID2_CENTER, ID2_BOX0|ID2_CENTER, ID2_BOX3|ID2_CENTER, ID2_BOX2|ID2_CENTER},
   {ID2_END,             ID2_END,             ID2_END,             ID2_END},
   {ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER, ID2_BOX0|ID2_CENTER, ID2_BOX3|ID2_CENTER},
   {ID2_END,             ID2_END,             ID2_END,             ID2_END},
   {ID2_BOX3|ID2_CENTER, ID2_BOX2|ID2_CENTER, ID2_BOX1|ID2_CENTER, ID2_BOX0|ID2_CENTER}};

Private id_bit_table id_bit_table_hrglass[] = {
   {ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6},
   {ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6},
   {ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2},
   {ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6},
   {ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6},
   {ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6,     ID2_END|ID2_OUTR6|ID2_CTR6},
   {ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2, ID2_CENTER|ID2_OUTR6|ID2_OUTR2},
   {ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6,   ID2_CENTER|ID2_CTR2|ID2_CTR6}};

Private id_bit_table id_bit_table_dhrglass[] = {
   {ID2_END|ID2_OUTR6|ID2_LEAD,    ID2_END|ID2_OUTR6|ID2_BEAU,    ID2_END|ID2_OUTR6|ID2_TRAILER,    ID2_END|ID2_OUTR6|ID2_BELLE},
   {ID2_END|ID2_OUTR6|ID2_LEAD,    ID2_END|ID2_OUTR6|ID2_BEAU,    ID2_END|ID2_OUTR6|ID2_TRAILER,    ID2_END|ID2_OUTR6|ID2_BELLE},
   {ID2_CENTER|ID2_OUTR6,          ID2_CENTER|ID2_OUTR6,          ID2_CENTER|ID2_OUTR6,             ID2_CENTER|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR2|ID2_LEAD,  ID2_CENTER|ID2_CTR2|ID2_BEAU,  ID2_CENTER|ID2_CTR2|ID2_TRAILER,  ID2_CENTER|ID2_CTR2|ID2_BELLE},
   {ID2_END|ID2_OUTR6|ID2_TRAILER, ID2_END|ID2_OUTR6|ID2_BELLE,   ID2_END|ID2_OUTR6|ID2_LEAD,       ID2_END|ID2_OUTR6|ID2_BEAU},
   {ID2_END|ID2_OUTR6|ID2_TRAILER, ID2_END|ID2_OUTR6|ID2_BELLE,   ID2_END|ID2_OUTR6|ID2_LEAD,       ID2_END|ID2_OUTR6|ID2_BEAU},
   {ID2_CENTER|ID2_OUTR6,          ID2_CENTER|ID2_OUTR6,          ID2_CENTER|ID2_OUTR6,             ID2_CENTER|ID2_OUTR6},
   {ID2_CENTER|ID2_CTR2|ID2_TRAILER,ID2_CENTER|ID2_CTR2|ID2_BELLE,ID2_CENTER|ID2_CTR2|ID2_LEAD,     ID2_CENTER|ID2_CTR2|ID2_BEAU}};

Private id_bit_table id_bit_table_bigdmd[] = {
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS}};

Private id_bit_table id_bit_table_3x4[] = {
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS, ID2_OUTRPAIRS},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4},
   {ID2_CTR4,      ID2_CTR4,      ID2_CTR4,      ID2_CTR4}};

Private id_bit_table id_bit_table_spindle[] = {
   {ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6},
   {ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2},
   {ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6},
   {ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6},
   {ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6},
   {ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2,     ID2_CTR6|ID2_CTR2},
   {ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6,    ID2_CTR6|ID2_OUTR6},
   {ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6,   ID2_OUTR2|ID2_OUTR6}};

Private id_bit_table id_bit_table_rigger[] = {
   {ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX0,     ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX3,  ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX2, ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX1},
   {ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX1,    ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX0,     ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX3,  ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX2},
   {ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_BELLE,          ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_LEAD,           ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_BEAU,           ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_TRAILER},
   {ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_BEAU,            ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_TRAILER,         ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_BELLE,           ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_LEAD},
   {ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX2, ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX1,    ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX0,     ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX3},
   {ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX3,  ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX2, ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX1,    ID2_CTR6|ID2_CENTER|ID2_CTR4|ID2_BOX0},
   {ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_BEAU,           ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_TRAILER,        ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_BELLE,          ID2_OUTR2|ID2_END|ID2_OUTRPAIRS|ID2_LEAD},
   {ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_BELLE,           ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_LEAD,            ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_BEAU,            ID2_CTR6|ID2_END|ID2_OUTRPAIRS|ID2_TRAILER}};

Private id_bit_table id_bit_table_1x3dmd[] = {
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,         ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,       ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,         ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,          ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,        ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,          ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,            ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,          ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,            ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6},
   {ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_CTR2|ID2_CTR6,    ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_CTR2|ID2_CTR6,  ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_CTR2|ID2_CTR6, ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_CTR2|ID2_CTR6},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,         ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,       ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2,         ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_OUTR2},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,          ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,        ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6,          ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTR6},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,            ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,          ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6,            ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTR6},     
   {ID2_CENTER|ID2_CTR4|ID2_TRAILER|ID2_CTR2|ID2_CTR6, ID2_CENTER|ID2_CTR4|ID2_BELLE|ID2_CTR2|ID2_CTR6, ID2_CENTER|ID2_CTR4|ID2_LEAD|ID2_CTR2|ID2_CTR6,    ID2_CENTER|ID2_CTR4|ID2_BEAU|ID2_CTR2|ID2_CTR6}};

Private id_bit_table id_bit_table_3x1dmd[] = {
   {ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BEAU,    ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_TRAILER, ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BELLE, ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_LEAD},
   {ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BELLE,    ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_LEAD,     ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BEAU,   ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_TRAILER},
   {ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_BEAU,        ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_TRAILER,     ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_BELLE,     ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_LEAD},
   {ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,                ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,                ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,              ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER},
   {ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BELLE,   ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_LEAD,    ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BEAU,  ID2_OUTR2|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_TRAILER},
   {ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BEAU,     ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_TRAILER,  ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_BELLE,  ID2_CTR6|ID2_OUTR6|ID2_OUTRPAIRS|ID2_END|ID2_LEAD},
   {ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_BELLE,       ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_LEAD,        ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_BEAU,      ID2_CTR2|ID2_CTR6|ID2_CTR4|ID2_CENTER|ID2_TRAILER},
   {ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,                ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,                ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER,              ID2_CTR6|ID2_OUTR6|ID2_CTR4|ID2_CENTER}};

Private id_bit_table id_bit_table_bone[] = {
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_LEAD,    ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BEAU,  ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_TRAILER,  ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BELLE},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_LEAD,    ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BEAU,  ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_TRAILER,  ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BELLE},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_BELLE,     ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_LEAD,    ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_BEAU,   ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_TRAILER},
   {ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_BEAU,       ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_TRAILER,  ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_BELLE,   ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_LEAD},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_TRAILER, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BELLE, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_LEAD, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BEAU},
   {ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_TRAILER, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BELLE, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_LEAD, ID2_END|ID2_OUTRPAIRS|ID2_OUTR6|ID2_BEAU},
   {ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_BEAU,      ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_TRAILER, ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_BELLE,  ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_LEAD},
   {ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_BELLE,      ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_LEAD,     ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_BEAU,    ID2_CENTER|ID2_CTR4|ID2_CTR2|ID2_TRAILER}};



/*                                                                                                           center_arity -------|
                                                                                                             mapelong --------|  |
                                                                                                            outer_rot -----|  |  |
                                                                                                     inner_rot ---------|  |  |  |
                                                               outlimit -----|                          bigsize ----|   |  |  |  |
                                          maps                   inlimit -|  |  bigsetup      insetup  outsetup     |   |  |  |  |   */

Private cm_thing map_3line    =      {{0, 1, 3, 2, 9, 8, 6, 7,
                                                      10, 11, 4, 5},      4, 4, s3x4,           s1x4,     s1x4,     12, 0, 0, 1, 2};
Private cm_thing map_3lb      =      {{11, 0, 1, 10, 7, 4, 5, 6,
                                                      8, 9, 2, 3},        4, 4, sbigdmd,        s2x2,     s1x4,     12, 1, 1, 1, 2};
Private cm_thing map_4line    =      {{12, 13, 0, 14, 8, 6, 4, 5,
                                           10, 15, 3, 1, 2, 7, 11, 9},    4, 8, s4x4,           s1x4,     s2x4,     16, 0, 0, 1, 2};
Private cm_thing map_3box    =       {{0, 1, 10, 11, 4, 5, 6, 7,
                                                      2, 3, 8, 9},        4, 4, s2x6,           s2x2,     s2x2,     12, 0, 0, 0, 2};
Private cm_thing map_3boxv   =       {{11, 0, 1, 10, 7, 4, 5, 6,
                                                      9, 2, 3, 8},        4, 4, s2x6,           s2x2,     s2x2,     12, 1, 1, 0, 2};
Private cm_thing map_4box    =       {{0, 1, 14, 15, 6, 7, 8, 9,
                                           2, 3, 4, 5, 10, 11, 12, 13},   4, 8, s2x8,           s2x2,     s2x4,     16, 0, 0, 0, 2};
Private cm_thing map1x4_boxes =      {{6, 7, 2, 3,
                                             -1, 0, 1, -1, -1, 4, 5, -1}, 4, 8, s_bone,         s1x4,     s2x4,      8, 0, 0, 0, 1};
Private cm_thing map1x4_2x4 =        {{10, 11, 4, 5,
                                               0, 1, 2, 3, 6, 7, 8, 9},   4, 8, s3x4,           s1x4,     s2x4,     12, 0, 0, 0, 1};
Private cm_thing oddmap1x4_2x4 =     {{8, 9, 2, 3,
                                               0, 1, 4, 5, 6, 7, 10, 11}, 4, 8, sbigdmd,        s1x4,     s2x4,     12, 1, 0, 0, 1};
Private cm_thing map1x2_1x2 =        {{1, 3,          0, 2},              2, 2, s1x4,           s1x2,     s1x2,     4,  0, 0, 0, 1};
Private cm_thing oddmap1x2_1x2 =     {{3, 1,          0, 2},              2, 2, sdmd,           s1x2,     s1x2,     4,  1, 0, 0, 1};
/* The map "oddmapdmd_dmd", with its loss of elongation information for the outer diamond, is necessary to make the call
   "with confidence" work from the setup formed by having the centers partner tag in left-hand waves.  This means that certain
   Bakerisms involving concentric diamonds, in which each diamond must remember its own elongation, are not possible.  Too bad. */
Private cm_thing oddmapdmd_dmd =     {{1, 3, 5, 7,    6, 0, 2, 4},        4, 4, s_crosswave,    sdmd,     sdmd,     8,  0, 1, 9, 1};
Private cm_thing mapdmd_dmd =        {{1, 3, 5, 7,    0, 2, 4, 6},        4, 4, s_crosswave,    sdmd,     sdmd,     8,  0, 0, 9, 1};
Private cm_thing oddmapdmd_1x4 =     {{1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 1, 9, 1};
Private cm_thing mapdmd_1x4 =        {{1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 0, 9, 1};
Private cm_thing oddmap_s_dmd_1x4 =  {{1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 1, 9, 1};
Private cm_thing map_s_dmd_1x4 =     {{1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 0, 9, 1};
Private cm_thing map_cs_1x4_dmd =    {{0, 3, 4, 7,    1, 2, 5, 6},        4, 4, s_wingedstar,   sdmd,     s1x4,     8,  0, 0, 9, 1};
Private cm_thing oddmap_s_star_1x4 = {{1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s_wingedstar,   s1x4,     s_star,   8,  0, 1, 9, 1};
Private cm_thing map_s_star_1x4 =    {{1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s_wingedstar,   s1x4,     s_star,   8,  0, 0, 9, 1};
Private cm_thing oddmap_s_short_1x6 = {{1, 2, 4, 7, 8, 10,
                                                11, 0, 3, 5, 6, 9},       6, 6, s_wingedstar12, s1x6,     s_short6, 12, 0, 1, 9, 1};
Private cm_thing map_s_spindle_1x8 = {{1, 2, 6, 5, 9, 10, 14, 13,
                                        3, 7, 12, 8, 11, 15, 4, 0},       8, 8, s_wingedstar16, s1x8,    s_spindle, 16, 0, 0, 9, 1};
        cm_thing map_spec_star12 =   {{2, 3, 4, 11, 10, 5, 8, 9,
                                                       0, 1, 6, 7},       4, 4, s_wingedstar12, s_star,   s1x4,     12, 0, 0, 0, 2};
        cm_thing map_spec_star12v =  {{11, 2, 3, 4, 9, 10, 5, 8,
                                                       0, 1, 6, 7},       4, 4, s_wingedstar12, s_star,   s1x4,     12, 1, 0, 0, 2};
        cm_thing map_spec_star16 =   {{2, 3, 5, 4, 6, 7, 14, 15, 13, 12, 10, 11,
                                                      0, 1, 8, 9},        4, 4, s_wingedstar16, s_star,   s1x4,     16, 0, 0, 0, 3};
        cm_thing map_spec_star16v =  {{4, 2, 3, 5, 15, 6, 7, 14, 11, 13, 12, 10,
                                                      0, 1, 8, 9},        4, 4, s_wingedstar16, s_star,   s1x4,     16, 1, 0, 0, 3};
Private cm_thing mapdmd_2x2h =       {{1, 3, 5, 7,    0, 2, 4, 6},        4, 4, s_galaxy,       s2x2,     sdmd,     8,  0, 0, 9, 1};
Private cm_thing mapdmd_2x2v =       {{7, 1, 3, 5,    0, 2, 4, 6},        4, 4, s_galaxy,       s2x2,     sdmd,     8,  1, 0, 9, 1};
Private cm_thing map2x3_1x2 =        {{11, 5,    0, 1, 2, 6, 7, 8},       2, 6, s3dmd,          s1x2,     s2x3,     12, 0, 0, 9, 1};
Private cm_thing map1x4_1x4 =        {{3, 2, 7, 6,    0, 1, 4, 5},        4, 4, s1x8,           s1x4,     s1x4,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x4_1x4 =     {{6, 7, 2, 3,    0, 1, 4, 5},        4, 4, s_crosswave,    s1x4,     s1x4,     8,  1, 0, 0, 1};
Private cm_thing oddmap1x4_1x2 =     {{0, 1, 3, 4,    5, 2},              4, 2, s_2x1dmd,       s1x4,     s1x2,     6,  0, 1, 0, 1};
Private cm_thing map2x3_2x3 = {{8, 11, 1, 2, 5, 7,    9, 10, 0, 3, 4, 6}, 6, 6, s3x4,           s2x3,     s2x3,     12, 1, 1, 1, 1};
        cm_thing map2x4_2x4 = {{10, 15, 3, 1, 2, 7, 11, 9,    12, 13, 14, 0, 4, 5, 6, 8}, 8, 8, s4x4, s2x4, s2x4,   16, 0, 0, 1, 1};
        cm_thing map2x4_2x4v = {{6, 11, 15, 13, 14, 3, 7, 5,    8, 9, 10, 12, 0, 1, 2, 4}, 8, 8, s4x4, s2x4, s2x4,  16, 1, 1, 1, 1};
Private cm_thing map1x2_bone6 = {{1, 7, 6, 5, 3, 2,    0, 4},             6, 2, s_ptpd,         s_bone6,  s1x2,     8,  0, 0, 0, 1};
Private cm_thing map1x6_1x2 = {{2, 6,    0, 1, 3, 4, 5, 7},               2, 6, s1x8,           s1x2,     s1x6,     8,  0, 0, 0, 1};
Private cm_thing mapbone6_1x2 = {{7, 3,    0, 1, 2, 4, 5, 6},             2, 6, s_bone,         s1x2,     s_bone6,  8,  0, 0, 0, 1};
Private cm_thing map1x4_1x4_rc = {{0, 2, 4, 6,    1, 3, 5, 7},            4, 4, s1x8,           s1x4,     s1x4,     8,  0, 0, 0, 1};
Private cm_thing map1x4_dmd_rc = {{0, 3, 4, 7,    1, 2, 5, 6},            4, 4, s1x3dmd,        sdmd,     s1x4,     8,  0, 0, 0, 1};
Private cm_thing map1x2_bone6_rc = {{0, 1, 3, 4, 5, 7,    6, 2},          6, 2, s_bone,         s_bone6,  s1x2,     8,  0, 0, 0, 1};
Private cm_thing map1x2_2x3_rc =   {{0, 3, 1, 4, 7, 5,    6, 2},          6, 2, s_dhrglass,     s2x3,     s1x2,     8,  0, 0, 0, 1};
Private cm_thing map2x2_dmd_rc = {{7, 1, 3, 5,    0, 2, 4, 6},            4, 4, s_spindle,      sdmd,     s2x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap2x2_dmd_rc = {{7, 1, 3, 5,    6, 0, 2, 4},         4, 4, s_spindle,      sdmd,     s2x2,     8,  0, 1, 0, 1};
Private cm_thing map2x2_1x4_rc = {{0, 2, 4, 6,    1, 7, 5, 3},            4, 4, s_ptpd,         s1x4,     s2x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap2x2_1x4_rc = {{0, 2, 4, 6,    3, 1, 7, 5},         4, 4, s_ptpd,         s1x4,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing map1x2_2x3 = {{0, 1, 2, 4, 5, 6,    7, 3},               6, 2, s_spindle,      s2x3,     s1x2,     8,  0, 0, 0, 1};
Private cm_thing map1x2_short6 = {{1, 2, 3, 5, 6, 7,    0, 4},            6, 2, s_galaxy,       s_short6, s1x2,     8,  0, 0, 0, 1};
Private cm_thing map1x2_intgl = {{0, 1, 3, 4, 5, 7,    6, 2},             6, 2, s_qtag,         s_bone6,  s1x2,     8,  0, 0, 0, 1};
Private cm_thing mapshort6_1x2h = {{5, 1,    6, 7, 0, 2, 3, 4},           2, 6, s_spindle,      s1x2,     s_short6, 8,  1, 1, 1, 1};
Private cm_thing mapshort6_1x2v = {{7, 3,    5, 6, 0, 1, 2, 4},           2, 6, s_hrglass,      s1x2,     s_short6, 8,  1, 1, 0, 1};
Private cm_thing mapbone6_1x2v =  {{7, 3,    0, 1, 2, 4, 5, 6},           2, 6, s_dhrglass,     s1x2,     s_bone6,  8,  1, 0, 0, 1};
Private cm_thing mapstar_2x2 = {{1, 3, 5, 7,    0, 2, 4, 6},              4, 4, s_galaxy,       s2x2,     s_star,   8,  0, 0, 0, 1};
Private cm_thing mapstar_star = {{1, 3, 5, 7,    0, 2, 4, 6},             4, 4, s_thar,         s_star,   s_star,   8,  0, 0, 0, 1};
Private cm_thing map1x2_1x6 = {{1, 3, 2, 5, 7, 6,    0, 4},               6, 2, s1x8,           s1x6,     s1x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x2_1x6 = {{0, 1, 2, 4, 5, 6,    7, 3},            6, 2, s3x1dmd,        s1x6,     s1x2,     8,  0, 1, 0, 1};

        cm_thing mapgnd1x2_1x2 = {{1, 3, 2, 6, 7, 5,    0, 4},            2, 2, s1x8,           s1x2,     s1x2,     8,  0, 0, 0, 3};
        cm_thing mapgnd1x2_1x2r = {{6, 0, 5, 1, 4, 2,    7, 3},           2, 2, s_spindle,      s1x2,     s1x2,     8,  1, 0, 0, 3};
Private cm_thing mapqtag_other = {{6, 7, 3, 2,    0, 1, 4, 5},            2, 4, s_qtag,         s1x2,     s2x2,     8,  0, 0, 1, 2};
        cm_thing map1x8_other  = {{1, 3, 2, 6, 7, 5,    0, 4},            2, 2, s1x8,           s1x2,     s1x2,     8,  0, 0, 0, 3};
        cm_thing mapspin_other = {{6, 0, 5, 1, 4, 2,    7, 3},            2, 2, s_spindle,      s1x2,     s1x2,     8,  1, 0, 0, 3};
Private cm_thing map2x4_other  = {{1, 2, 6, 5,    0, 3, 4, 7},            2, 4, s2x4,           s1x2,     s2x2,     8,  0, 0, 0, 2};
        cm_thing specialmapqtag_other = {{6, 7, 3, 2,      5, 0, 1, 4},   2, 4, s_qtag,         s1x2,     s2x2,     8,  0, 1, 0, 2};
        cm_thing specialmap2x4_other  = {{6, 5, 1, 2,      7, 0, 3, 4},   2, 4, s2x4,           s1x2,     s2x2,     8,  0, 1, 0, 2};

Private cm_thing map1x4_2x2 = {{0, 1, 4, 5,    6, 7, 2, 3},               4, 4, s_rigger,       s2x2,     s1x4,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x4_2x2 = {{5, 0, 1, 4,    6, 7, 2, 3},            4, 4, s_rigger,       s2x2,     s1x4,     8,  1, 0, 0, 1};
Private cm_thing map1x4_star = {{2, 3, 6, 7,    0, 1, 4, 5},              4, 4, s_wingedstar,   s_star,   s1x4,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x4_star = {{7, 2, 3, 6,    0, 1, 4, 5},           4, 4, s_wingedstar,   s_star,   s1x4,     8,  1, 0, 0, 1};
Private cm_thing map2x2_dmd = {{6, 3, 2, 7,    0, 1, 4, 5},               4, 4, s_hrglass,      sdmd,     s2x2,     8,  0, 0, 1, 1};
Private cm_thing map2x2_dmdv = {{6, 3, 2, 7,    0, 1, 4, 5},              4, 4, s_dhrglass,     sdmd,     s2x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap2x2_dmd = {{6, 3, 2, 7,    5, 0, 1, 4},            4, 4, s_hrglass,      sdmd,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing oddmap2x2_dmdv = {{6, 3, 2, 7,    5, 0, 1, 4},           4, 4, s_dhrglass,     sdmd,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing map2x2_1x4h = {{6, 7, 2, 3,    0, 1, 4, 5},              4, 4, s_qtag,         s1x4,     s2x2,     8,  0, 0, 1, 1};
Private cm_thing oddmap2x2_1x4h = {{6, 7, 2, 3,    5, 0, 1, 4},           4, 4, s_bone,         s1x4,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing mapstar_1x4 =    {{1, 2, 5, 6,    0, 3, 4, 7},           4, 4, s3x1dmd,        s1x4,     s_star,   12, 0, 0, 9, 1};
Private cm_thing oddmapstar_1x4 = {{1, 2, 5, 6,    7, 0, 3, 4},           4, 4, s3x1dmd,        s1x4,     s_star,   12, 0, 1, 9, 1};
Private cm_thing mapstar_dmd ={{1, 3, 5, 7,    0, 2, 4, 6},               4, 4, s_crosswave,    sdmd,     s_star,   8,  0, 0, 9, 1};
Private cm_thing oddmapstar_dmd =  {{1, 3, 5, 7,    6, 0, 2, 4},          4, 4, s_crosswave,    sdmd,     s_star,   8,  0, 1, 9, 1};
Private cm_thing map2x2_1x4v = {{6, 7, 2, 3,    0, 1, 4, 5},              4, 4, s_bone,         s1x4,     s2x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap2x2_1x4v = {{6, 7, 2, 3,    5, 0, 1, 4},           4, 4, s_qtag,         s1x4,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing map2x2_2x2v = {{1, 2, 5, 6,    0, 3, 4, 7},              4, 4, s2x4,           s2x2,     s2x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap2x2_2x2h = {{1, 2, 5, 6,    7, 0, 3, 4},           4, 4, s2x4,           s2x2,     s2x2,     8,  0, 1, 9, 1};
Private cm_thing oddmap2x2_2x2v = {{6, 1, 2, 5,    0, 3, 4, 7},           4, 4, s2x4,           s2x2,     s2x2,     8,  1, 0, 9, 1};
Private cm_thing map2x2_2x2h = {{6, 1, 2, 5,    7, 0, 3, 4},              4, 4, s2x4,           s2x2,     s2x2,     8,  1, 1, 9, 1};
Private cm_thing maplatgal = {{7, 0, 1, 3, 4, 5,    6, 2},                6, 2, s_galaxy,       s_short6, s1x2,     8,  1, 1, 0, 1};
Private cm_thing map1x4_dmd = {{2, 3, 6, 7,    0, 1, 4, 5},               4, 4, s1x3dmd,        sdmd,     s1x4,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x4_dmd = {{7, 2, 3, 6,    0, 1, 4, 5},            4, 4, s3x1dmd,        sdmd,     s1x4,     8,  1, 0, 0, 1};
Private cm_thing oddmap1x6_1x2 = {{3, 7,    0, 1, 2, 4, 5, 6},            2, 6, s1x3dmd,        s1x2,     s1x6,     8,  1, 0, 0, 1};

Private cm_thing map1x2_2x1dmd = {{1, 2, 3, 5, 6, 7,    0, 4},            6, 2, s3x1dmd,        s_2x1dmd, s1x2,     8,  0, 0, 0, 1};
Private cm_thing oddmap1x2_2x1dmd = {{6, 7, 1, 2, 3, 5,    0, 4},         6, 2, s_crosswave,    s_2x1dmd, s1x2,     8,  1, 0, 0, 1};
Private cm_thing map1x2_1x2dmd =    {{1, 2, 3, 5, 6, 7,    0, 4},         6, 2, s1x3dmd,        s_1x2dmd, s1x2,     8,  0, 0, 0, 1};

Private cm_thing oddmap1x2_1x2dmd = {{0, 1, 3, 4, 5, 7,    6, 2},         6, 2, s_crosswave,    s_1x2dmd, s1x2,     8,  0, 1, 0, 1};

Private cm_thing map2x1dmd_1x2 = {{2, 6,    0, 1, 3, 4, 5, 7},            2, 6, s3x1dmd,        s1x2,     s_2x1dmd, 8,  0, 0, 0, 1};
Private cm_thing oddmap2x1dmd_1x2 = {{7, 3,    0, 1, 2, 4, 5, 6},         2, 6, s_crosswave,    s1x2,     s_2x1dmd, 8,  1, 0, 0, 1};


Private cm_thing oddmap1x2_bone6 = {{5, 0, 3, 1, 4, 7,    6, 2},          6, 2, s_hrglass,      s_bone6,  s1x2,     8,  1, 0, 0, 1};
Private cm_thing oddmap1x2_2x3 = {{5, 7, 0, 1, 3, 4,    6, 2},            6, 2, s_qtag,         s2x3,     s1x2,     8,  1, 0, 0, 1};
Private cm_thing oddmap1x2_short6 = {{5, 7, 0, 1, 3, 4,    6, 2},         6, 2, s_rigger,       s_short6, s1x2,     8,  1, 0, 0, 1};
Private cm_thing oddmap2x3_1x2 = {{11, 5,    9, 10, 0, 3, 4, 6},          2, 6, s3x4,           s1x2,     s2x3,     12, 0, 1, 0, 1};
Private cm_thing oddmapshort6_1x2h = {{2, 6,    3, 0, 1, 7, 4, 5},        2, 6, s_ptpd,         s1x2,     s_short6, 8,  0, 1, 1, 1};
Private cm_thing oddmapshort6_1x2v = {{7, 3,    5, 6, 0, 1, 2, 4},        2, 6, s_qtag,         s1x2,     s_short6, 8,  0, 1, 0, 1};
Private cm_thing oddmapbigdmd_1x2v = {{9, 3,
                            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},        2,12, sbigdmd,        s1x2,     sbigdmd, 12,  1, 0, 1, 1};
Private cm_thing oddmap1x2_short6_rc = {{5, 6, 0, 1, 2, 4,    7, 3},      6, 2, s_rigger,       s_short6, s1x2,     8,  1, 0, 0, 1};
Private cm_thing map2x4_2x2 = {{2, 3, 8, 9,
                                          0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x6,           s2x2,     s2x4,     12, 0, 0, 9, 1};
Private cm_thing oddmap2x4_2x2 = {{9, 2, 3, 8,
                                          0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x6,           s2x2,     s2x4,     12, 1, 0, 9, 1};
Private cm_thing map2x4_2x2v = {{15, 3, 7, 11,
                                          12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s4x4,           s2x2,     s2x4,     16, 0, 0, 9, 1};
Private cm_thing oddmap2x4_2x2v = {{11, 15, 3, 7,
                                          12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s4x4,           s2x2,     s2x4,     16, 1, 0, 9, 1};
Private cm_thing map1x8_2x2 = {{0, 1, 4, 5,
                                          -1, -1, 7, 6, -1, -1, 3, 2},    4, 8, s_rigger,       s2x2,     s1x8,     12, 0, 0, 9, 1};
Private cm_thing oddmap1x8_2x2 = {{5, 0, 1, 4,
                                          -1, -1, 7, 6, -1, -1, 3, 2},    4, 8, s_rigger,       s2x2,     s1x8,     12, 1, 0, 9, 1};
Private cm_thing map1x3dmd_line = {{1, 2, 5, 6,    0, 3, 4, 7},           4, 4, s1x3dmd,        s1x4,     sdmd,     8,  0, 0, 0, 1};
Private cm_thing mapdmd_line = {{1, 2, 5, 6,    0, 3, 4, 7},              4, 4, s3x1dmd,        s1x4,     sdmd,     8,  0, 0, 0, 1};
/* This one allows "finish" when the center "line" is actually a diamond whose centers are empty.
   This can happen because of the preference given for lines over diamonds at the conclusion of certain calls. */
Private cm_thing mappts_line = {{1, -1, 5, -1,    0, 2, 4, 6},            4, 4, s_crosswave,    s1x4,     sdmd,     8,  0, 0, 0, 1};
Private cm_thing map_s_dmd_line = {{1, 2, 5, 6,    0, 3, 4, 7},           4, 4, s_wingedstar,   s1x4,     sdmd,     8,  0, 0, 0, 1};


conc_initializer conc_init_table[] = {
/* outerk        innerk          conc_type     center_arity    maps */
   {s2x2,        s1x4,     schema_rev_checkpoint,    1, {&map2x2_1x4_rc,  &oddmap2x2_1x4_rc,   &map2x2_1x4_rc,   &oddmap2x2_1x4_rc}},
   {s2x2,        sdmd,     schema_rev_checkpoint,    1, {&map2x2_dmd_rc,  &oddmap2x2_dmd_rc,   &map2x2_dmd_rc,   &oddmap2x2_dmd_rc}},
   {s1x4,        s1x4,     schema_rev_checkpoint,    1, {&map1x4_1x4_rc,  0,                   &map1x4_1x4_rc,   0}},
   {s1x4,        sdmd,     schema_rev_checkpoint,    1, {&map1x4_dmd_rc,  0,                   &map1x4_dmd_rc,   0}},
   {s1x4,        sdmd,     schema_ckpt_star,         1, {&map_cs_1x4_dmd, 0,                   &map_cs_1x4_dmd,  0}},
   {sdmd,        s1x4,     schema_conc_star,         1, {&map_s_dmd_1x4,  &oddmap_s_dmd_1x4,   &map_s_dmd_1x4,   &oddmap_s_dmd_1x4}},
   {s_star,      s1x4,     schema_conc_star,         1, {&map_s_star_1x4, &oddmap_s_star_1x4,  &map_s_star_1x4,  &oddmap_s_star_1x4}},
   {s1x4,        s_star,   schema_conc_star,         1, {&map1x4_star,    &oddmap1x4_star,     &map1x4_star,     &oddmap1x4_star}},
   {s_short6,    s1x6,     schema_conc_star12,       1, {0,               &oddmap_s_short_1x6, 0,                0}},
   {s_spindle,   s1x8,     schema_conc_star16,       1, {0,               0,                   &map_s_spindle_1x8, 0}},
   {s1x4,        s_star,   schema_conc_star12,       2, {&map_spec_star12,&map_spec_star12v,   &map_spec_star12, &map_spec_star12v}},
   {s1x4,        s_star,   schema_conc_star16,       3, {&map_spec_star16,&map_spec_star16v,   &map_spec_star16, &map_spec_star16v}},
   {s1x2,      s1x2, schema_grand_single_concentric, 3, {&mapgnd1x2_1x2,  &mapgnd1x2_1x2r,     &mapgnd1x2_1x2,   &mapgnd1x2_1x2r}},
   {s1x2,        s1x2,     schema_concentric_others, 3, {&map1x8_other,   &mapspin_other,      &map1x8_other,    &mapspin_other}},
   {s2x2,        s1x2,     schema_concentric_others, 2, {0,               &specialmapqtag_other, 0,              &specialmap2x4_other}},
   {s1x4,        s1x4,     schema_in_out_triple,     2, {0,               0,                   &map_3line,       0}},
   {s2x2,        s2x2,     schema_in_out_triple,     2, {&map_3box,       0,                   &map_3boxv,       0}},
   {s1x4,        s2x2,     schema_in_out_triple,     2, {0,               0,                   &map_3lb,         0}},
   {s2x4,        s1x4,     schema_in_out_quad,       2, {0,               0,                   &map_4line,       0}},
   {s2x4,        s2x2,     schema_in_out_quad,       2, {&map_4box,       0,                   0,                0}},
   {s1x6,        s1x2,     schema_concentric,        1, {&map1x6_1x2,     &oddmap1x6_1x2,      &map1x6_1x2,      &oddmap1x6_1x2}},
   {s_bone6,     s1x2,     schema_concentric,        1, {&mapbone6_1x2,   &mapbone6_1x2v,      &mapbone6_1x2,    &mapbone6_1x2v}},
   {s2x3,        s1x2,     schema_concentric,        1, {&map2x3_1x2,     &oddmap2x3_1x2,      &map2x3_1x2,      &oddmap2x3_1x2}},
   {s2x3,        s2x3,     schema_concentric,        1, {&map2x3_2x3,     0,                   &map2x3_2x3,      0}},
   {s2x4,        s1x4,     schema_concentric,        1, {&map1x4_boxes,   &oddmap1x4_2x4,      &map1x4_2x4,      0}},
   {s2x4,        s2x2,     schema_concentric,        1, {&map2x4_2x2,     &oddmap2x4_2x2,      &map2x4_2x2v,     &oddmap2x4_2x2v}},
   {s1x8,        s2x2,     schema_concentric,        1, {&map1x8_2x2,     &oddmap1x8_2x2,      &map1x8_2x2,      &oddmap1x8_2x2}},
   {s2x4,        s2x4,     schema_concentric,        1, {&map2x4_2x4,     0,                   &map2x4_2x4,      0}},
   {s1x4,        s1x4,     schema_concentric,        1, {&map1x4_1x4,     &oddmap1x4_1x4,      &map1x4_1x4,      &oddmap1x4_1x4}},
   {s1x2,        s1x4,     schema_concentric,        1, {0,               &oddmap1x4_1x2,      0,                &oddmap1x4_1x2}},
   {s1x4,        sdmd,     schema_concentric,        1, {&map1x4_dmd,     &oddmap1x4_dmd,      &map1x4_dmd,      &oddmap1x4_dmd}},
   {s1x4,        s_star,   schema_concentric,        1, {&map1x4_star,    &oddmap1x4_star,     &map1x4_star,     &oddmap1x4_star}},
   {s1x4,        s2x2,     schema_concentric,        1, {&map1x4_2x2,     &oddmap1x4_2x2,      &map1x4_2x2,      &oddmap1x4_2x2}},
   {s1x2,        s1x2,     schema_concentric,        1, {&map1x2_1x2,     &oddmap1x2_1x2,      &map1x2_1x2,      &oddmap1x2_1x2}},
   {s1x2,        s2x3,     schema_concentric,        1, {&map1x2_2x3,     &oddmap1x2_2x3,      &map1x2_2x3,      &oddmap1x2_2x3}},
   {s1x2,        s_bone6,  schema_concentric,        1, {&map1x2_bone6,   &oddmap1x2_bone6,    &map1x2_bone6,    &oddmap1x2_bone6}},
   {s1x2,        s_short6, schema_concentric,        1, {&map1x2_short6,  &oddmap1x2_short6,   &map1x2_short6,   &oddmap1x2_short6}},
   {s1x2,        s1x6,     schema_concentric,        1, {&map1x2_1x6,     &oddmap1x2_1x6,      &map1x2_1x6,      &oddmap1x2_1x6}},
   {s_short6,    s1x2,     schema_concentric,        1, {&mapshort6_1x2v, &oddmapshort6_1x2v,  &mapshort6_1x2h,  &oddmapshort6_1x2h}},
   {sbigdmd,     s1x2,     schema_concentric,        1, {0,               &oddmapbigdmd_1x2v,  0,                0}},
   {s1x2,        s_2x1dmd, schema_concentric,        1, {&map1x2_2x1dmd,  &oddmap1x2_2x1dmd,   &map1x2_2x1dmd,   &oddmap1x2_2x1dmd}},
   {s1x2,        s_1x2dmd, schema_concentric,        1, {&map1x2_1x2dmd,  &oddmap1x2_1x2dmd,   &map1x2_1x2dmd,   &oddmap1x2_1x2dmd}},
   {s_2x1dmd,    s1x2,     schema_concentric,        1, {&map2x1dmd_1x2,  &oddmap2x1dmd_1x2,   &map2x1dmd_1x2,   &oddmap2x1dmd_1x2}},
   {s2x2,        s2x2,     schema_concentric,        1, {&map2x2_2x2v,    &oddmap2x2_2x2v,     &map2x2_2x2h,     &oddmap2x2_2x2h}},
   {s2x2,        s1x4,     schema_concentric,        1, {&map2x2_1x4v,    &oddmap2x2_1x4v,     &map2x2_1x4h,     &oddmap2x2_1x4h}},
   {s2x2,        sdmd,     schema_concentric,        1, {&map2x2_dmdv,    &oddmap2x2_dmd,      &map2x2_dmd,      &oddmap2x2_dmdv}},
   {sdmd,        sdmd,     schema_concentric,        1, {&mapdmd_dmd,     &oddmapdmd_dmd,      &mapdmd_dmd,      &oddmapdmd_dmd}},
   {sdmd,        s1x4,     schema_concentric,        1, {&mapdmd_1x4,     &oddmapdmd_1x4,      &mapdmd_1x4,      &oddmapdmd_1x4}},
   {sdmd,        s2x2,     schema_concentric,        1, {&mapdmd_2x2h,    &mapdmd_2x2v,        &mapdmd_2x2h,     &mapdmd_2x2v}},
   {s_star,      sdmd,     schema_concentric,        1, {&mapstar_dmd,    &oddmapstar_dmd,     &mapstar_dmd,     &oddmapstar_dmd}},
   {s_star,      s1x4,     schema_concentric,        1, {&mapstar_1x4,    &oddmapstar_1x4,     &mapstar_1x4,     &oddmapstar_1x4}},
   {s_star,      s_star,   schema_concentric,        1, {&mapstar_star,   0,                   &mapstar_star,    0}},
   {s_star,      s2x2,     schema_concentric,        1, {&mapstar_2x2,    0,                   &mapstar_2x2,     0}},
   {nothing}};


/* BEWARE!!  These lists are keyed to the enumeration "analyzer_kind" :

    analyzer_NORMAL
    |       analyzer_CHECKPT
    |       |       analyzer_2X6
    |       |       |       analyzer_6X2
    |       |       |       |       analyzer_4X2
    |       |       |       |       |       analyzer_6X2_TGL
    |       |       |       |       |       |       analyzer_STAR12
    |       |       |       |       |       |       |       analyzer_STAR16
    |       |       |       |       |       |       |       |       analyzer_SINGLE
    |       |       |       |       |       |       |       |       |       analyzer_GRANDSINGLE
    |       |       |       |       |       |       |       |       |       |       analyzer_TRIPLE_LINE
    |       |       |       |       |       |       |       |       |       |       |       analyzer_QUAD_LINE
    |       |       |       |       |       |       |       |       |       |       |       |       analyzer_VERTICAL6
    |       |       |       |       |       |       |       |       |       |       |       |       |       analyzer_LATERAL6
    |       |       |       |       |       |       |       |       |       |       |       |       |       |       analyzer_OTHERS
    |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       analyzer_CONC_DIAMONDS
    |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       analyzer_DIAMOND_LINE */

Private cm_hunk concthing_1x4 = {0x5, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      0,      &map1x2_1x2,
                                                                            0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_dmd = {0x5, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      0,      &oddmap1x2_1x2,
                                                                            0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_2x1dmd = {0, 0, 0,
   {0,      0,      0,      0,      &oddmap1x4_1x2,
                                            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_qtag = {0x33, 0xDD, 0x11,
   {&map2x2_1x4h,
            0,      &oddmapshort6_1x2v,
                            &oddmap1x2_2x3,
                                    0,      &map1x2_intgl,
                                                    0,      0,      0,      0,      0,      0,      0,      0,      &mapqtag_other,
                                                                                                                            0,      0}};

Private cm_hunk concthing_bone = {0x33, 0, 0x11,
   {&map2x2_1x4v,
            &map1x2_bone6_rc,
                    &mapbone6_1x2,
                            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_rigger = {0xCC, 0xDD, 0,
   {&map1x4_2x2,
            &oddmap1x2_short6_rc,
                    0,      &oddmap1x2_short6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_spindle = {0, 0xEE, 0x44,
   {0,      &map2x2_dmd_rc,
                    &mapshort6_1x2h,
                            &map1x2_2x3,
                                    0,      0,      0,      0,      0,      &mapgnd1x2_1x2r,
                                                                                    0,      0,      0,      0,      &mapspin_other,
                                                                                                                            0,      0}};

Private cm_hunk concthing_hrglass = {0x33, 0xDD, 0x11,
   {&map2x2_dmd,
            0,      &mapshort6_1x2v,
                            &oddmap1x2_bone6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      &oddmap1x2_bone6,
                                                                                                            0,      0,      0,      0}};

Private cm_hunk concthing_dhrglass = {0x33, 0, 0x11,
   {&map2x2_dmdv,
            &map1x2_2x3_rc,
                    &mapbone6_1x2v,
                            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_xwave = {0x33, 0x77, 0x11,
   {&oddmap1x4_1x4,
            0,      &oddmap2x1dmd_1x2,
                            &oddmap1x2_2x1dmd,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &mapdmd_dmd,
                                                                                                                                    &mappts_line}};

Private cm_hunk concthing_1x8 = {0x33, 0x77, 0x22,
   {&map1x4_1x4,
            &map1x4_1x4_rc,
                    &map1x6_1x2,
                            &map1x2_1x6,
                                    0,      0,      0,      0,      0,      &mapgnd1x2_1x2,    
                                                                                    0,      0,      0,      0,      &map1x8_other,
                                                                                                                            0,      0}};

Private cm_hunk concthing_2x4 = {0x66, 0, 0,
   {&map2x2_2x2v,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map2x4_other,
                                                                                                                            0,      0}};

Private cm_hunk concthing_3x4 = {0, 0, 0x041,
   {0,      0,      &oddmap2x3_1x2,
                            0,      0,      0,      &map2x3_2x3,
                                                            0,      0,      0,      &map_3line,
                                                                                            0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_2x6 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map_3box,
                                                                                            0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_2x8 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map_4box,
                                                                                                    0,      0,      0,      0,      0}};

Private cm_hunk concthing_4x4 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      &map2x4_2x4v,
                                                                    0,      0,      0,      &map_4line,
                                                                                                    0,      0,      0,      0,      0}};

Private cm_hunk concthing_ptpd = {0, 0x77, 0x22,
   {0,      &map2x2_1x4_rc,
                    &oddmapshort6_1x2h,
                            &map1x2_bone6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};

Private cm_hunk concthing_1x3dmd = {0x33, 0x77, 0x11,
   {&map1x4_dmd,
            &map1x4_dmd_rc,
                    &oddmap1x6_1x2,
                            &map1x2_1x2dmd,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map1x3dmd_line}};

Private cm_hunk concthing_3x1dmd = {0x33, 0xEE, 0,
   {&oddmap1x4_dmd,
            0,      &map2x1dmd_1x2,
                            &oddmap1x2_1x6,
                                    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &mapdmd_line}};

Private cm_hunk concthing_wstar = {0x33, 0, 0,
   {&map1x4_star,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map_s_dmd_line}};

Private cm_hunk concthing_wstar12 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      &map_spec_star12,
                                                            0,      0,      0,      0,      0,      0,      0,      0,      0,      &oddmap_s_short_1x6}};

Private cm_hunk concthing_wstar16 = {0, 0, 0,
   {0,      0,      0,      0,      0,      0,      0,      &map_spec_star16,
                                                                    0,      0,      0,      0,      0,      0,      0,      0,      &map_s_spindle_1x8}};

Private cm_hunk concthing_gal = {0x55, 0, 0,
   {&mapstar_2x2,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      &map1x2_short6,
                                                                                                            &maplatgal,
                                                                                                                    0,      0,      0}};

Private cm_hunk concthing_thar = {0x55, 0, 0,
   {&mapstar_star,
            0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0}};


/* BEWARE!!  This list is keyed to the definition of "setup_kind" in database.h . */
setup_attr setup_attrs[] = {
   /* nothing */
      {-1,                          /* setup_limits */
      (coordrec *) 0,               /* setup_coords */
      (coordrec *) 0,               /* nice_setup_coords */
      (cm_hunk *) 0,                /* conctab */
      {b_nothing,   b_nothing},     /* keytab */
      { 0, 0},                      /* bounding_box */
      FALSE,                        /* four_way_symmetry */
      (id_bit_table *) 0,           /* id_bit_table_ptr */
      {  (Cstring) 0,               /* print_strings */
         (Cstring) 0}},
   /* s1x1 */
      { 0,
      &thing1x1,
      &thing1x1,
      (cm_hunk *) 0,
      {b_1x1,       b_1x1},
      { 1, 1},
      TRUE,
      (id_bit_table *) 0,
      {  "a@",
         (Cstring) 0}},
   /* s1x2 */
      { 1,
      &thing1x2,
      &thing1x2,
      (cm_hunk *) 0,
      {b_1x2,       b_2x1},
      { 2, 1},
      FALSE,
      id_bit_table_1x2,
      {  "a  b@",
         "a@b@"}},
   /* s1x3 */
      { 2,
      &thing1x3,
      &thing1x3,
      (cm_hunk *) 0,
      {b_1x3,       b_3x1},
      { 3, 1},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s2x2 */
      { 3,
      &thing2x2,
      &thing2x2,
      (cm_hunk *) 0,
      {b_2x2,       b_2x2},
      { 2, 2},
      TRUE,
      id_bit_table_2x2,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s1x4 */
      { 3,
      &thing1x4,
      &thing1x4,
      &concthing_1x4,
      {b_1x4,       b_4x1},
      { 4, 1},
      FALSE,
      id_bit_table_1x4,
      {  "a  b  d  c@",
         "a@b@d@c@"}},
   /* sdmd */
      { 3,
      (coordrec *) 0,
      &nicethingdmd,
      &concthing_dmd,
      {b_dmd,       b_pmd},
      { 0, 2},
      FALSE,
      id_bit_table_dmd,
      {  "     b@a      c@     d@",
         "   a@@d  b@@   c@"}},
   /* s_star */
      { 3,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_star,      b_star},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "   b@a  c@   d@",
         (Cstring) 0}},
   /* s_trngl */
      { 2,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_trngl,     b_ptrngl},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_trngl4 */
      { 3,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_trngl4,    b_ptrngl4},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_bone6 */
      { 5,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_bone6,     b_pbone6},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "a        b@    fc@e        d@",
         "ea@  f@  c@db@"}},
   /* s_short6 */
      { 5,
      (coordrec *) 0,
      &nicethingshort6,
      (cm_hunk *) 0,
      {b_short6,    b_pshort6},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   b@a  c@f  d@   e@",
         "   fa@e      b@   dc@"}},
   /* s_1x2dmd */
      { 5,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_1x2dmd,    b_p1x2dmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "           c@a  b      e  d@           f@",
         "   a@@   b@@f  c@@   e@@   d@"}},
   /* s_2x1dmd */
      { 5,
      (coordrec *) 0,
      (coordrec *) 0,
      &concthing_2x1dmd,
      {b_2x1dmd,    b_p2x1dmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "         c@@a  b  e  d@@         f",
         "      a@@      b@f        c@      e@@      d"}},
   /* s_qtag */
      { 7,
      &thingqtag,
      &nicethingqtag,
      &concthing_qtag,
      {b_qtag,      b_pqtag},
      { 4, 0},
      FALSE,
      id_bit_table_qtag,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_bone */
      { 7,
      &thingbone,
      &thingbone,
      &concthing_bone,
      {b_bone,      b_pbone},
      { 0, 0},
      FALSE,
      id_bit_table_bone,
      {  "a                   b@    g h d c@f                   e",
         "fa@  g@  h@  d@  c@eb"}},
   /* s_rigger */
      { 7,
      &thingrigger,
      &thingrigger,
      &concthing_rigger,
      {b_rigger,    b_prigger},
      { 0, 0},
      FALSE,
      id_bit_table_rigger,
      {  "        a b@gh         dc@        f e",
         "  g@  h@fa@eb@  d@  c"}},
   /* s_spindle */
      { 7,
      &thingspindle,
      &thingspindle,
      &concthing_spindle,
      {b_spindle,   b_pspindle},
      { 0, 0},
      FALSE,
      id_bit_table_spindle,
      {  "    a b c@h              d@    g f e",
         "  h@ga@fb@ec@  d"}},
   /* s_hrglass */
      { 7,
      &thingglass,
      &nicethingglass,
      &concthing_hrglass,
      {b_hrglass,   b_phrglass},
      { 0, 4},
      FALSE,
      id_bit_table_hrglass,
      {  "   a  b@      d@g        c@      h@   f  e",
         "     g@f      a@   hd@e      b@     c"}},
   /* s_dhrglass */
      { 7,
      &thingdglass,
      &thingdglass,
      &concthing_dhrglass,
      {b_dhrglass,  b_pdhrglass},
      { 0, 0},
      FALSE,
      id_bit_table_dhrglass,
      {  "a      d      b@     g      c@f      h      e",
         "f  a@@   g@@h  d@@   c@@e  b"}},
   /* s_hyperglass */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_crosswave */
      { 7,
      &thingxwv,
      &thingxwv,
      &concthing_xwave,
      {b_crosswave, b_pcrosswave},
      { 0, 0},
      FALSE,
      id_bit_table_crosswave,
      {  "          c@          d@ab        fe@          h@          g",
         "      a@      b@@ghdc@@      f@      e"}},
   /* s1x8 */
      { 7,
      &thing1x8,
      &thing1x8,
      &concthing_1x8,
      {b_1x8,       b_8x1},
      { 8, 1},
      FALSE,
      id_bit_table_1x8,
      {  "a b d c g h f e",
         "a@b@d@c@g@h@f@e"}},
   /* s2x4 */
      { 7,
      &thing2x4,
      &thing2x4,
      &concthing_2x4,
      {b_2x4,       b_4x2},
      { 4, 2},
      FALSE,
      id_bit_table_2x4,
      {  "a  b  c  d@@h  g  f  e",
         "h  a@@g  b@@f  c@@e  d"}},
   /* s2x3 */
      { 5,
      &thing2x3,
      &thing2x3,
      (cm_hunk *) 0,
      {b_2x3,       b_3x2},
      { 3, 2},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c@f  e  d@",
         "f  a@e  b@d  c@"}},
   /* s1x6 */
      { 5,
      &thing1x6,
      &thing1x6,
      (cm_hunk *) 0,
      {b_1x6,       b_6x1},
      { 6, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  f  e  d@",
         "a@b@c@f@e@d@"}},
   /* s3x4 */
      {11,
      &thing3x4,
      &thing3x4,
      &concthing_3x4,
      {b_3x4,       b_4x3},
      { 4, 3},
      FALSE,
      id_bit_table_3x4,
      {  "a  b  c  d@@k  l  f  e@@j  i  h  g",
         "j  k  a@@i  l  b@@h  f  c@@g  e  d"}},
   /* s2x6 */
      {11,
      &thing2x6,
      &thing2x6,
      &concthing_2x6,
      {b_2x6,       b_6x2},
      { 6, 2},
      FALSE,
      id_bit_table_2x6p,
      {  "a  b  c  d  e  f@@l  k  j  i  h  g",
         "l  a@@k  b@@j  c@@i  d@@h  e@@g  f"}},
   /* s1p5x8 */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h@p  o  n  m  l  k  j  i",
         "pa@@ob@@nc@@md@@le@@kf@@jg@@ih"}},
   /* s2x8 */
      {15,
      &thing2x8,
      &thing2x8,
      &concthing_2x8,
      {b_2x8,       b_8x2},
      { 8, 2},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h@@p  o  n  m  l  k  j  i",
         "p  a@@o  b@@n  c@@m  d@@l  e@@k  f@@j  g@@i  h"}},
   /* s4x4 */
      {15,
      &thing4x4,
      &thing4x4,
      &concthing_4x4,
      {b_4x4,       b_4x4},
      { 4, 4},
      TRUE,
      (id_bit_table *) 0,
      {  "m  n  o  a@@k  p  d  b@@j  l  h  c@@i  g  f  e",
         (Cstring) 0}},
   /* sx1x6 */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s1x10 */
      { 9,
      &thing1x10,
      &thing1x10,
      (cm_hunk *) 0,
      {b_1x10,      b_10x1},
      {10, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "a b c d e j i h g f",
         "a@b@c@d@e@j@i@h@g@f"}},
   /* s1x12 */
      {11,
      &thing1x12,
      &thing1x12,
      (cm_hunk *) 0,
      {b_1x12,      b_12x1},
      {12, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "a b c d e f l k j i h g",
         "a@b@c@d@e@f@l@k@j@i@h@g"}},
   /* s1x14 */
      {13,
      &thing1x14,
      &thing1x14,
      (cm_hunk *) 0,
      {b_1x14,      b_14x1},
      {14, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "abcdefgnmlkjih",
         "a@b@c@d@e@f@g@n@m@l@k@j@i@h"}},
   /* s1x16 */
      {15,
      &thing1x16,
      &thing1x16,
      (cm_hunk *) 0,
      {b_1x16,      b_16x1},
      {16, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "abcdefghponmlkji",
         "a@b@c@d@e@f@g@h@p@o@n@m@l@k@j@i"}},
   /* s_c1phan */
      {15,
      &thingphan,
      &nicethingphan,
      (cm_hunk *) 0,
      {b_c1phan,    b_c1phan},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "   b        e@a  c  h  f@   d        g@@   o        l@n  p  k  i@   m        j",
         (Cstring) 0}},
   /* s_bigblob */
      {23,
      &thingblob,
      &thingblob,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "            a  b@@      v  w  c  d@@t  u  x  f  e  g@@s  q  r  l  i  h@@      p  o  k  j@@            n  m",
         (Cstring) 0}},
   /* s_ptpd */
      { 7,
      &thingptpd,
      &nicethingptpd,
      &concthing_ptpd,
      {b_ptpd,      b_pptpd},
      { 0, 0},
      FALSE,
      id_bit_table_ptpd,
      {  "    b           h@a    c   g    e@    d           f",
         "  a@@db@@  c@@  g@@fh@@  e"}},
   /* s1x3dmd */
      { 7,
      &thing1x3dmd,
      &thing1x3dmd,
      &concthing_1x3dmd,
      {b_1x3dmd,    b_p1x3dmd},
      { 0, 0},
      FALSE,
      id_bit_table_1x3dmd,
      {  "               d@a b c      g f e@               h",
         "   a@@   b@@   c@@h  d@@   g@@   f@@   e@"}},
   /* s3x1dmd */
      { 7,
      &thing3x1dmd,
      &thing3x1dmd,
      &concthing_3x1dmd,
      {b_3x1dmd,    b_p3x1dmd},
      { 0, 0},
      FALSE,
      id_bit_table_3x1dmd,
      {  "             d@@a b c g f e@@             h",
         "      a@@      b@@      c@h        d@      g@@      f@@      e"}},
   /* s3dmd */
      {11,
      &thing3dmd,
      &thing3dmd,
      (cm_hunk *) 0,
      {b_3dmd,      b_p3dmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a      b      c@@j k l f e d@@   i      h      g",
         "      j@i        a@      k@@      l@h        b@      f@@      e@g        c@      d"}},
   /* s4dmd */
      {15,
      &thing4dmd,
      &thing4dmd,
      (cm_hunk *) 0,
      {b_4dmd,      b_p4dmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a      b      c      d@@m n o p h g f e@@   l      k      j      i",
         "      m@l        a@      n@@      o@k        b@      p@@      h@j        c@      g@@      f@i        d@      e"}},
   /* s_wingedstar */
      { 7,
      (coordrec *) 0,
      (coordrec *) 0,
      &concthing_wstar,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "             d@a b c  g f e@             h",
         "   a@@   b@@   c@h  d@   g@@   f@@   e"}},
   /* s_wingedstar12 */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      &concthing_wstar12,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "             d       f@a b c  e k  i h g@             l       j",
         "   a@@   b@@   c@l  d@   e@   k@j  f@   i@@   h@@   g"}},
   /* s_wingedstar16 */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      &concthing_wstar16,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "             d       h       m@a b c  f g  o n  k j i@             e       p       l",
         "   a@@   b@@   c@e  d@   f@   g@p  h@   o@   n@l  m@   k@@   j@@   i"}},
   /* s_galaxy */
      { 7,
      &thinggal,
      &thinggal,
      &concthing_gal,
      {b_galaxy,    b_galaxy},
      { 0, 0},
      TRUE,
      id_bit_table_gal,
      {  "     c@   bd@a      e@   hf@     g",
         (Cstring) 0}},
   /* s3x8 */
      {23,
      &thing3x8,
      &thing3x8,
      (cm_hunk *) 0,
      {b_3x8,       b_8x3},
      { 8, 3},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h@@u  v  w  x  l  k  j  i@@t  s  r  q  p  o  n  m",
         "t  u  a@@s  v  b@@r  w  c@@q  x  d@@p  l  e@@o  k  f@@n  j  g@@m  i  h"}},
   /* s4x6 */
      {23,
      &thing4x6,
      &thing4x6,
      (cm_hunk *) 0,
      {b_4x6,       b_6x4},
      { 6, 4},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f@@l  k  j  i  h  g@@s  t  u  v  w  x@@r  q  p  o  n  m",
         "r  s  l  a@@q  t  k  b@@p  u  j  c@@o  v  i  d@@n  w  h  e@@m  x  g  f"}},
   /* s_thar */
      { 7,
      &thingthar,
      &thingthar,
      &concthing_thar,
      {b_thar,      b_thar},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "      c@      d@abfe@      h@      g",
         (Cstring) 0}},
   /* sx4dmd */
      {31,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s8x8 */
      {63,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* sfat2x8 */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 8, 4},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* swide4x4 */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 8, 4},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* sbigdmd */
      {11,
      &thingbigdmd,
      &thingbigdmd,
      (cm_hunk *) 0,
      {b_bigdmd,    b_pbigdmd},
      { 0, 0},
      FALSE,
      id_bit_table_bigdmd,
      {  "          c@a b        e f@          d@@          j@l k        h g@          i",
         "   l      a@   k      b@i j d c@   h      e@   g      f"}},
   /* s_normal_concentric */
      {-1,
      (coordrec *) 0,
      (coordrec *) 0,
      (cm_hunk *) 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}}};


/* BEWARE!!  This list is keyed to the definition of "begin_kind" in database.h . */
/*   It must also match the similar table in the database-building program. */
int begin_sizes[] = {
   0,          /* b_nothing */
   1,          /* b_1x1 */
   2,          /* b_1x2 */
   2,          /* b_2x1 */
   3,          /* b_1x3 */
   3,          /* b_3x1 */
   4,          /* b_2x2 */
   4,          /* b_dmd */
   4,          /* b_pmd */
   4,          /* b_star */
   6,          /* b_trngl */
   6,          /* b_ptrngl */
   8,          /* b_trngl4 */
   8,          /* b_ptrngl4 */
   6,          /* b_bone6 */
   6,          /* b_pbone6 */
   6,          /* b_short6 */
   6,          /* b_pshort6 */
   6,          /* b_1x2dmd */
   6,          /* b_p1x2dmd */
   6,          /* b_2x1dmd */
   6,          /* b_p2x1dmd */
   8,          /* b_qtag */
   8,          /* b_pqtag */
   8,          /* b_bone */
   8,          /* b_pbone */
   8,          /* b_rigger */
   8,          /* b_prigger */
   8,          /* b_spindle */
   8,          /* b_pspindle */
   8,          /* b_hrglass */
   8,          /* b_phrglass */
   8,          /* b_dhrglass */
   8,          /* b_pdhrglass */
   8,          /* b_crosswave */
   8,          /* b_pcrosswave */
   4,          /* b_1x4 */
   4,          /* b_4x1 */
   8,          /* b_1x8 */
   8,          /* b_8x1 */
   8,          /* b_2x4 */
   8,          /* b_4x2 */
   6,          /* b_2x3 */
   6,          /* b_3x2 */
   6,          /* b_1x6 */
   6,          /* b_6x1 */
   12,         /* b_3x4 */
   12,         /* b_4x3 */
   12,         /* b_2x6 */
   12,         /* b_6x2 */
   16,         /* b_2x8 */
   16,         /* b_8x2 */
   16,         /* b_4x4 */
   10,         /* b_1x10 */
   10,         /* b_10x1 */
   12,         /* b_1x12 */
   12,         /* b_12x1 */
   14,         /* b_1x14 */
   14,         /* b_14x1 */
   16,         /* b_1x16 */
   16,         /* b_16x1 */
   16,         /* b_c1phan */
   8,          /* b_galaxy */
   24,         /* b_3x8 */
   24,         /* b_8x3 */
   24,         /* b_4x6 */
   24,         /* b_6x4 */
   8,          /* b_thar */
   8,          /* b_ptpd */
   8,          /* b_pptpd */
   8,          /* b_1x3dmd */
   8,          /* b_p1x3dmd */
   8,          /* b_3x1dmd */
   8,          /* b_p3x1dmd */
   12,         /* b_3dmd */
   12,         /* b_p3dmd */
   16,         /* b_4dmd */
   16,         /* b_p4dmd */
   12,         /* b_bigdmd */
   12};        /* b_pbigdmd */


#define B1A 0000
#define G1A 0100
#define B2A 0200
#define G2A 0300
#define B3A 0400
#define G3A 0500
#define B4A 0600
#define G4A 0700

#define B1B (ID2_NSG|ID2_NSB|ID2_NHG|ID2_HCOR|ID2_HEAD|ID2_BOY)
#define G1B (ID2_NSG|ID2_NSB|ID2_NHB|ID2_SCOR|ID2_HEAD|ID2_GIRL)
#define B2B (ID2_NSG|ID2_NHG|ID2_NHB|ID2_SCOR|ID2_SIDE|ID2_BOY)
#define G2B (ID2_NSB|ID2_NHG|ID2_NHB|ID2_HCOR|ID2_SIDE|ID2_GIRL)
#define B3B B1B
#define G3B G1B
#define B4B B2B
#define G4B G2B

/* BEWARE!!  This list is keyed to the definition of "start_select_kind" in sd.h. */
startinfo startinfolist[] = {
   {
      "???",      /* A non-existent setup. */
      FALSE,                         /* into_the_middle */
      {
         nothing,                    /* kind */
         1,                          /* rotation */
         {0},                        /* cmd */
         {{0,0}},                    /* people */
         0                           /* result_flags (imprecise_rotation is off) */
      }
   },
   {
      "Heads 1P2P",
      FALSE,                         /* into_the_middle */
      {
         s2x4,                       /* kind */
         1,                          /* rotation */
         {0},                        /* cmd */
         {{G2A|d_south,G2B}, {B2A|d_south,B2B}, {G1A|d_south,G1B}, {B1A|d_south,B1B},
            {G4A|d_north,G4B}, {B4A|d_north,B4B}, {G3A|d_north,G3B}, {B3A|d_north,B3B}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   },
   {
      "Sides 1P2P",
      FALSE,                         /* into_the_middle */
      {
         s2x4,                       /* kind */
         0,                          /* rotation */
         {0},                        /* cmd */
         {{G3A|d_south,G3B}, {B3A|d_south,B3B}, {G2A|d_south,G2B}, {B2A|d_south,B2B},
            {G1A|d_north,G1B}, {B1A|d_north,B1B}, {G4A|d_north,G4B}, {B4A|d_north,B4B}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   },
   {
      "HEADS",
      TRUE,                          /* into_the_middle */
      {
         s2x4,                       /* kind */
         0,                          /* rotation */
         {0},                        /* cmd */
         {{B4A|d_east,B4B}, {G3A|d_south,G3B}, {B3A|d_south,B3B}, {G2A|d_west,G2B},
            {B2A|d_west,B2B}, {G1A|d_north,G1B}, {B1A|d_north,B1B}, {G4A|d_east,G4B}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   },
   {
      "SIDES",
      TRUE,                          /* into_the_middle */
      {
         s2x4,                       /* kind */
         1,                          /* rotation */
         {0},                        /* cmd */
         {{B3A|d_east,B3B}, {G2A|d_south,G2B}, {B2A|d_south,B2B}, {G1A|d_west,G1B},
            {B1A|d_west,B1B}, {G4A|d_north,G4B}, {B4A|d_north,B4B}, {G3A|d_east,G3B}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   },
   {
      "From squared set",
      FALSE,                         /* into_the_middle */
      {
         s4x4,                       /* kind */
         0,                          /* rotation */
         {0},                        /* cmd */
         {{0,0}, {G2A|d_west,G2B}, {B2A|d_west,B2B}, {0,0}, {0,0}, {G1A|d_north,G1B},
            {B1A|d_north,B1B}, {0,0}, {0,0}, {G4A|d_east,G4B}, {B4A|d_east,B4B}, {0,0},
            {0,0}, {G3A|d_south,G3B}, {B3A|d_south,B3B}, {0,0}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   }
#ifdef RIPNSNORT
   {
      "1/4 stable, couple #3 rip 'n' snort, then HEADS",
      TRUE,                          /* into_the_middle */
      {
         s2x4,                       /* kind */
         0,                          /* rotation */
         {0},                        /* cmd */
         {{B4A|d_south,B4B}, {G3A|d_west,G3B}, {B3A|d_east,B3B}, {G2A|d_south,G2B},
            {B2A|d_south,B2B}, {G1A|d_west,G1B}, {B1A|d_east,B1B}, {G4A|d_south,G4B}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   }
#endif
};



/* Nonzero item in map3[1] for arity 1 map means do not reassemble. */
/* For maps with arity 1, a nonzero item in map3[0] means that the "offset goes away" warning should be given in certain cases. */
/* For maps with arity 3, a value of 3 after the last map means that the "overlap goes away" warning should be given in certain cases. */

/*                                                                                                          setups are stacked on top of each other --------------------|
                                                                                        each setup is rotated clockwise before being concatenated -----------------|    |
                                                                                              (rot = 2 for second setup upside-down, 1-fold symmetry)              |    |
                                                                                                 (rot = 3 for 1st setup ccw, 2nd cw, 1-fold symmetry)              |    |
                                                                                                                                                                   V    V
                                             map1                             map2                        map3 map4   map_kind          arity  outer   inner      rot  vert */

        map_thing map_b6_trngl          = {{5, 4, 0,                          2, 1, 3},                              MPKIND__SPLIT,       2,  s_bone6, s_trngl,  0x207, 0};
        map_thing map_s6_trngl          = {{4, 5, 3,                          1, 2, 0},                              MPKIND__SPLIT,       2,  s_short6, s_trngl, 0x108, 1};
        map_thing map_bone_trngl4       = {{7, 6, 5, 0,                       3, 2, 1, 4},                           MPKIND__SPLIT,       2,  s_bone, s_trngl4,  0x207, 0};
        map_thing map_rig_trngl4        = {{6, 7, 0, 5,                       2, 3, 4, 1},                           MPKIND__SPLIT,       2,  s_rigger,s_trngl4, 0x10D, 0};
        map_thing map_s8_tgl4           = {{2, 7, 5, 0,                       6, 3, 1, 4},                           MPKIND__REMOVED,     2,  s_bone, s_trngl4,  0x207, 0};
        map_thing map_p8_tgl4           = {{0, 2, 7, 5,                       4, 6, 3, 1},                           MPKIND__REMOVED,     2,  s_ptpd, s_trngl4,  0x10D, 0};
        map_thing map_2x2v              = {{0, 3,                             1, 2},                                 MPKIND__SPLIT,       2,  s2x2,   s1x2,      0x005, 0};
        map_thing map_2x4_magic         = {{0, 6, 3, 5,                       7, 1, 4, 2},                           MPKIND__NONE,        2,  s2x4,   s1x4,      0x000, 0};
        map_thing map_qtg_magic         = {{0, 2, 5, 3,                       1, 7, 4, 6},                           MPKIND__NONE,        2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_qtg_intlk         = {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__NONE,        2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_qtg_magic_intlk   = {{0, 2, 5, 7,                       1, 3, 4, 6},                           MPKIND__NONE,        2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_ptp_magic         = {{6, 1, 4, 3,                       0, 7, 2, 5},                           MPKIND__NONE,        2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_ptp_intlk         = {{0, 1, 6, 3,                       2, 7, 4, 5},                           MPKIND__NONE,        2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_ptp_magic_intlk   = {{2, 1, 4, 3,                       0, 7, 6, 5},                           MPKIND__NONE,        2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_2x4_diagonal      = {{2, 3, 6, 7,                       0, 1, 4, 5},                           MPKIND__NONE,        2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_2x4_int_pgram     = {{1, 3, 5, 7,                       0, 2, 4, 6},                           MPKIND__NONE,        2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_2x4_trapezoid     = {{1, 2, 4, 7,                       0, 3, 5, 6},                           MPKIND__NONE,        2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_3x4_2x3_intlk     = {{2, 5, 7, 9, 10, 0,                3, 4, 6, 8, 11, 1},                    MPKIND__NONE,        2,  s3x4,   s2x3,      0x005, 0};
        map_thing map_3x4_2x3_conc      = {{3, 4, 6, 9, 10, 0,                2, 5, 7, 8, 11, 1},                    MPKIND__NONE,        2,  s3x4,   s2x3,      0x005, 0};
        map_thing map_4x4_ns            = {{12, 13, 0, 14,                    8, 6, 4, 5},                           MPKIND__NONE,        2,  s4x4,   s1x4,      0x000, 1};
        map_thing map_4x4_ew            = {{0, 1, 4, 2,                       12, 10, 8, 9},                         MPKIND__NONE,        2,  s4x4,   s1x4,      0x005, 1};
        map_thing map_phantom_box       = {{0, 1, 6, 7, 8, 9, 14, 15,         2, 3, 4, 5, 10, 11, 12, 13},           MPKIND__CONCPHAN,    2,  s2x8,   s2x4,      0x000, 0};
        map_thing map_intlk_phantom_box = {{0, 1, 4, 5, 10, 11, 14, 15,       2, 3, 6, 7, 8, 9, 12, 13},             MPKIND__INTLK,       2,  s2x8,   s2x4,      0x000, 0};
        map_thing map_phantom_dmd       = {{0, 3, 4, 5, 8, 11, 12, 13,        1, 2, 6, 7, 9, 10, 14, 15},            MPKIND__CONCPHAN,    2,  s4dmd,  s_qtag,    0x000, 0};
        map_thing map_intlk_phantom_dmd = {{0, 2, 6, 7, 9, 11, 12, 13,        1, 3, 4, 5, 8, 10, 14, 15},            MPKIND__INTLK,       2,  s4dmd,  s_qtag,    0x000, 0};
        map_thing map_stagger           = {{10, 13, 3, 0, 2, 5, 11, 8,        12, 15, 14, 1, 4, 7, 6, 9},            MPKIND__STAG,        2,  s4x4,   s2x4,      0x000, 1};
Private map_thing map_staggerv          = {{12, 15, 14, 1, 4, 7, 6, 9,        10, 13, 3, 0, 2, 5, 11, 8},            MPKIND__STAG,        2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_stairst           = {{9, 13, 7, 0, 1, 5, 15, 8,         12, 11, 14, 2, 4, 3, 6, 10},           MPKIND__NONE,        2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_ladder            = {{10, 15, 14, 0, 2, 7, 6, 8,        12, 13, 3, 1, 4, 5, 11, 9},            MPKIND__NONE,        2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_offset            = {{9, 11, 14, 0, 1, 3, 6, 8,         12, 13, 7, 2, 4, 5, 15, 10},           MPKIND__NONE,        2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_but_o             = {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__NONE,        2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_o_s2x4_3          = {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__O_SPOTS,     2,  s4x4,   s2x4,      0x000, 0};
        map_thing map_x_s2x4_3          = {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__X_SPOTS,     2,  s4x4,   s2x4,      0x000, 0};
Private map_thing map_o_s2x4_2          = {{14, 1, 2, 5, 6, 9, 10, 13,        0, 3, 7, 4, 8, 11, 15, 12},            MPKIND__O_SPOTS,     2,  s4x4,   s2x4,      0x005, 0};
Private map_thing map_x_s2x4_2          = {{14, 1, 2, 5, 6, 9, 10, 13,        0, 3, 7, 4, 8, 11, 15, 12},            MPKIND__X_SPOTS,     2,  s4x4,   s2x4,      0x005, 0};
        map_thing map_4x4v     = {{12, 10, 8, 9,         13, 15, 6, 11,        14, 3, 5, 7,      0, 1, 4, 2},        MPKIND__SPLIT,       4,  s4x4,   s1x4,      0x055, 0};
        map_thing map_blocks   = {{12, 14, 7, 9,         13, 0, 2, 11,         15, 1, 4, 6,      10, 3, 5, 8},       MPKIND__NONE,        4,  s4x4,   s2x2,      0x000, 0};
        map_thing map_trglbox  = {{12, 14, 15, 9,        13, 0, 2, 3,          7, 1, 4, 6,       10, 11, 5, 8},      MPKIND__NONE,        4,  s4x4,   s2x2,      0x000, 0};
Private map_thing map_4x4_1x4  = {{8, 6, 4, 5,           9, 11, 2, 7,          10, 15, 1, 3,     12, 13, 0, 14},     MPKIND__SPLIT,       4,  s4x4,   s1x4,      0x000, 1};
Private map_thing map_1x16_1x4 = {{0, 1, 3, 2,           4, 5, 7, 6,           15, 14, 12, 13,   11, 10, 8, 9},      MPKIND__SPLIT,       4,  s1x16,  s1x4,      0x000, 0};
Private map_thing map_2x8_2x2  = {{0, 1, 14, 15,         2, 3, 12, 13,         4, 5, 10, 11,     6, 7, 8, 9},        MPKIND__SPLIT,       4,  s2x8,   s2x2,      0x000, 0};
Private map_thing map_conc_qb  = {{0, 7, 8, 15,          1, 6, 9, 14,          2, 5, 10, 13,     3, 4, 11, 12},      MPKIND__CONCPHAN,    4,  s2x8,   s2x2,      0x000, 0};
Private map_thing map_4dmd_dmd = {{0, 13, 11, 12,        1, 15, 10, 14,        2, 6, 9, 7,       3, 4, 8, 5},        MPKIND__SPLIT,       4,  s4dmd,  sdmd,      0x055, 0};
Private map_thing map_2x6_1x6           = {{11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       2,  s2x6,   s1x6,      0x000, 1};
Private map_thing map_2x8_1x8           = {{15, 14, 12, 13, 8, 9, 11, 10,     0, 1, 3, 2, 7, 6, 4, 5},               MPKIND__SPLIT,       2,  s2x8,   s1x8,      0x000, 1};
Private map_thing map_1x12_1x6          = {{0, 1, 2, 5, 4, 3,                 11, 10, 9, 6, 7, 8},                   MPKIND__SPLIT,       2,  s1x12,  s1x6,      0x000, 0};
Private map_thing map_1x16_1x8          = {{0, 1, 3, 2, 7, 6, 4, 5,           15, 14, 12, 13, 8, 9, 11, 10},         MPKIND__SPLIT,       2,  s1x16,  s1x8,      0x000, 0};
Private map_thing map_intlk_phan_grand  = {{0, 1, 3, 2, 12, 13, 15, 14,       4, 5, 7, 6, 8, 9, 11, 10},             MPKIND__INTLK,       2,  s1x16,  s1x8,      0x000, 0};
Private map_thing map_conc_phan_grand   = {{0, 1, 3, 2, 8, 9, 11, 10,         4, 5, 7, 6, 12, 13, 15, 14},           MPKIND__CONCPHAN,    2,  s1x16,  s1x8,      0x000, 0};
        map_thing map_hv_2x4_2          = {{0, 1, 2, 3, 12, 13, 14, 15,       4, 5, 6, 7, 8, 9, 10, 11},             MPKIND__SPLIT,       2,  s2x8,   s2x4,      0x000, 0};
        map_thing map_3x4_2x3           = {{1, 11, 8, 9, 10, 0,               3, 4, 6, 7, 5, 2},                     MPKIND__SPLIT,       2,  s3x4,   s2x3,      0x005, 0};
        map_thing map_split_f           = {{9, 11, 7, 2, 4, 5, 6, 8,          12, 13, 14, 0, 1, 3, 15, 10},          MPKIND__SPLIT,       2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_intlk_f           = {{10, 15, 3, 1, 4, 5, 6, 8,         12, 13, 14, 0, 2, 7, 11, 9},           MPKIND__INTLK,       2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_full_f            = {{12, 13, 14, 0, 4, 5, 6, 8,        10, 15, 3, 1, 2, 7, 11, 9},            MPKIND__CONCPHAN,    2,  s4x4,   s2x4,      0x000, 1};
Private map_thing map_2x4_2x2           = {{0, 1, 6, 7,                       2, 3, 4, 5},                           MPKIND__SPLIT,       2,  s2x4,   s2x2,      0x000, 0};
Private map_thing map_2x3_1x3           = {{5, 4, 3,                          0, 1, 2},                              MPKIND__SPLIT,       2,  s2x3,   s1x3,      0x000, 1};
Private map_thing map_bigd_12d          = {{11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       2,  sbigdmd,s_1x2dmd,  0x000, 1};
Private map_thing map_bone_12d          = {{-1, -1, 0, 7, 6, 5,               3, 2, 1, -1, -1, 4},                   MPKIND__SPLIT,       2,  s_bone, s_1x2dmd,  0x000, 0};
Private map_thing map_2x4_1x4           = {{7, 6, 4, 5,                       0, 1, 3, 2},                           MPKIND__SPLIT,       2,  s2x4,   s1x4,      0x000, 1};
Private map_thing map_qtg_dmd           = {{0, 7, 5, 6,                       1, 2, 4, 3},                           MPKIND__SPLIT,       2,  s_qtag, sdmd,      0x005, 0};
Private map_thing map_ptp_dmd           = {{0, 1, 2, 3,                       6, 7, 4, 5},                           MPKIND__SPLIT,       2,  s_ptpd, sdmd,      0x000, 0};
Private map_thing map_3x8_3x4           = {{0, 1, 2, 3, 23, 22, 16, 17, 18, 19, 20, 21,
                                                                   4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 11, 10},        MPKIND__SPLIT,       2,  s3x8,   s3x4,      0x000, 0};
Private map_thing map_4x6_3x4           = {{2, 9, 20, 15, 16, 19, 17, 18, 11, 0, 1, 10,
                                                                   5, 6, 23, 12, 13, 22, 14, 21, 8, 3, 4, 7},        MPKIND__SPLIT,       2,  s4x6,   s3x4,      0x005, 0};
Private map_thing map_4x6_2x6           = {{18, 19, 20, 21, 22, 23, 12, 13, 14, 15, 16, 17,
                                                                   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},            MPKIND__SPLIT,       2,  s4x6,   s2x6,      0x000, 1};
Private map_thing map_3dmd_dmd = {{0, 10, 8, 9,                    1, 5, 7, 11,                   2, 3, 6, 4,    0}, MPKIND__SPLIT,       3,  s3dmd,  sdmd,      0x015, 0};
        map_thing map_4x6_2x4  = {{1, 10, 19, 16, 17, 18, 11, 0,   3, 8, 21, 14, 15, 20, 9, 2,
                                                                   5, 6, 23, 12, 13, 22, 7, 4,                   0}, MPKIND__SPLIT,       3,  s4x6,   s2x4,      0x015, 0};
        map_thing map_hv_qtg_2          = {{0, 1, 15, 14, 10, 11, 12, 13,     2, 3, 4, 5, 8, 9, 7, 6},               MPKIND__SPLIT,       2,  s4dmd,  s_qtag,    0x000, 0};
        map_thing map_vv_qtg_2          = {{9, 20, 16, 19, 18, 11, 1, 10,     6, 23, 13, 22, 21, 8, 4, 7},           MPKIND__SPLIT,       2,  s4x6,   s_qtag,    0x005, 0};
Private map_thing map_spin_3x4          = {{1, 11, 8, -1, 9, 10, 0, -1,       3, 4, 6, -1, 7, 5, 2, -1},             MPKIND__SPLIT,       2,  s3x4,   s_spindle, 0x005, 0};
Private map_thing map_hrgl_ptp          = {{-1, -1, 2, 1, -1, -1, 0, 3,       -1, -1, 4, 7, -1, -1, 6, 5},           MPKIND__SPLIT,       2,  s_ptpd, s_hrglass, 0x000, 0};
Private map_thing map_ov_hrg_1          = {{-1, -1, 5, 7, -1, -1, 0, 6,       -1, -1, 4, 2, -1, -1, 1, 3},           MPKIND__OVERLAP,     2,  s_qtag, s_hrglass, 0x005, 0};
Private map_thing map_1x6_1x3           = {{0, 1, 2,                          5, 4, 3},                              MPKIND__SPLIT,       2,  s1x6,   s1x3,      0x000, 0};
Private map_thing map_1x8_1x4           = {{0, 1, 2, 3,                       6, 7, 4, 5},                           MPKIND__SPLIT,       2,  s1x8,   s1x4,      0x000, 0};
Private map_thing map_1x12_1x4          = {{0, 1, 3, 2,            4, 5, 10, 11,          9, 8, 6, 7,            0}, MPKIND__SPLIT,       3,  s1x12,  s1x4,      0x000, 0};
Private map_thing map_1x2_1x1           = {{0,                                1},                                    MPKIND__SPLIT,       2,  s1x2,   s1x1,      0x000, 0};
Private map_thing map_1x4_1x2           = {{0, 1,                             3, 2},                                 MPKIND__SPLIT,       2,  s1x4,   s1x2,      0x000, 0};
        map_thing map_tgl4_1            = {{1, 0,                             2, 3},                                 MPKIND__NONE,        2,  s_trngl4,s1x2,     0x001, 1};
        map_thing map_tgl4_2            = {{3, 2,                             1, 0},                                 MPKIND__NONE,        2,  s_trngl4,s1x2,     0x006, 1};
Private map_thing map_2x6_2x2           = {{0, 1, 10, 11,          2, 3, 8, 9,            4, 5, 6, 7,            0}, MPKIND__SPLIT,       3,  s2x6,   s2x2,      0x000, 0};
Private map_thing map_2x6_2x2r          = {{1, 10, 11, 0,          3, 8, 9, 2,            5, 6, 7, 4,            0}, MPKIND__SPLIT,       3,  s2x6,   s2x2,      0x015, 0};
Private map_thing map_conc_tb           = {{0, 5, 6, 11,           1, 4, 7, 10,           2, 3, 8, 9,            0}, MPKIND__CONCPHAN,    3,  s2x6,   s2x2,      0x000, 0};
Private map_thing map_conc_tbr          = {{5, 6, 11, 0,           4, 7, 10, 1,           3, 8, 9, 2,            0}, MPKIND__CONCPHAN,    3,  s2x6,   s2x2,      0x015, 0};
Private map_thing map_3x4_1x4           = {{9, 8, 6, 7,            10, 11, 4, 5,          0, 1, 3, 2,            0}, MPKIND__SPLIT,       3,  s3x4,   s1x4,      0x000, 1};
Private map_thing map_2x3_1x2           = {{0, 5,                  1, 4,                  2, 3,                  0}, MPKIND__SPLIT,       3,  s2x3,   s1x2,      0x015, 0};
Private map_thing map_1x6_1x2           = {{0, 1,                  2, 5,                  4, 3,                  0}, MPKIND__SPLIT,       3,  s1x6,   s1x2,      0x000, 0};
Private map_thing map_2x2h              = {{3, 2,                             0, 1},                                 MPKIND__SPLIT,       2,  s2x2,   s1x2,      0x000, 1};
Private map_thing map_2x4_2x2r          = {{1, 6, 7, 0,                       3, 4, 5, 2},                           MPKIND__SPLIT,       2,  s2x4,   s2x2,      0x005, 0};
        map_thing map_2x6_2x3           = {{0, 1, 2, 9, 10, 11,               3, 4, 5, 6, 7, 8},                     MPKIND__SPLIT,       2,  s2x6,   s2x3,      0x000, 0};
Private map_thing map_1x2_rmv           = {{0, 3,                             1, 2},                                 MPKIND__REMOVED,     2,  s1x4,   s1x2,      0x000, 0};
Private map_thing map_1x2_rmvr          = {{0, 3,                             1, 2},                                 MPKIND__REMOVED,     2,  s2x2,   s1x2,      0x005, 0};
Private map_thing map_2x2_rmv           = {{0, 2, 5, 7,                       1, 3, 4, 6},                           MPKIND__REMOVED,     2,  s2x4,   s2x2,      0x000, 0};
Private map_thing map_1x4_rmv           = {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__REMOVED,     2,  s1x8,   s1x4,      0x000, 0};
Private map_thing map_1x4_rmvr          = {{7, 6, 4, 5,                       0, 1, 3, 2},                           MPKIND__REMOVED,     2,  s2x4,   s1x4,      0x000, 1};
Private map_thing map_2x4_rmv           = {{0, 2, 4, 6, 9, 11, 13, 15,        1, 3, 5, 7, 8, 10, 12, 14},            MPKIND__REMOVED,     2,  s2x8,   s2x4,      0x000, 0};
Private map_thing map_2x4_rmvr          = {{10, 15, 3, 1, 4, 5, 6, 8,         12, 13, 14, 0, 2, 7, 11, 9},           MPKIND__REMOVED,     2,  s4x4,   s2x4,      0x000, 1};
Private map_thing map_dmd_rmv           = {{6, 0, 3, 5,                       7, 1, 2, 4},                           MPKIND__REMOVED,     2,  s_rigger, sdmd,    0x000, 0};
Private map_thing map_dmd_rmvr          = {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__REMOVED,     2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_dbloff1           = {{0, 1, 3, 2, 4, 5, 7, 6,                                              0}, MPKIND__NONE,        1,  s2x4,   s_qtag,    0x000, 0};
        map_thing map_dbloff2           = {{2, 3, 4, 5, 6, 7, 0, 1,                                              0}, MPKIND__NONE,        1,  s2x4,   s_qtag,    0x000, 0};
Private map_thing map_ov_1x4_2          = {{0, 1, 2, 3,            3, 2, 7, 6,            6, 7, 4, 5,            0}, MPKIND__OVERLAP,     3,  s1x8,   s1x4,      0x000, 0};
Private map_thing map_ov_1x4_3          = {{9, 8, 6, 7,            10, 11, 4, 5,          0, 1, 3, 2,            3}, MPKIND__OVERLAP,     3,  s3x4,   s1x4,      0x000, 0};
Private map_thing map_ov_s2x2_1         = {{0, 1, 4, 5,                     1, 2, 3, 4},                             MPKIND__OVERLAP,     2,  s2x3,   s2x2,      0x000, 0};
Private map_thing map_ov_s2x2r_1        = {{1, 4, 5, 0,                     2, 3, 4, 1},                             MPKIND__OVERLAP,     2,  s2x3,   s2x2,      0x005, 0};
Private map_thing map_ov_s2x2_2         = {{0, 1, 6, 7,            1, 2, 5, 6,            2, 3, 4, 5,            0}, MPKIND__OVERLAP,     3,  s2x4,   s2x2,      0x000, 0};
Private map_thing map_ov_2x3_3          = {{1, 11, 8, 9, 10, 0,    2, 5, 7, 8, 11, 1,     3, 4, 6, 7, 5, 2,      0}, MPKIND__OVERLAP,     3,  s3x4,   s2x3,      0x015, 0};
Private map_thing map_ov_2x4_0          = {{0, 1, 2, 3, 8, 9, 10, 11,         2, 3, 4, 5, 6, 7, 8, 9},               MPKIND__OVERLAP,     2,  s2x6,   s2x4,      0x000, 0};
Private map_thing map_ov_2x4_1          = {{10, 11, 5, 4, 6, 7, 8, 9,         0, 1, 2, 3, 4, 5, 11, 10},             MPKIND__OVERLAP,     2,  s3x4,   s2x4,      0x000, 1};
Private map_thing map_ov_2x4_2          = {{0, 1, 2, 3, 12, 13, 14, 15,       2, 3, 4, 5, 10, 11, 12, 13,     4, 5, 6, 7, 8, 9, 10, 11,       0},
                                                                                                                     MPKIND__OVERLAP,     3,  s2x8,   s2x4,      0x000, 0};
Private map_thing map_ov_2x4_3          = {{9, 11, 7, 2, 4, 5, 6, 8,          10, 15, 3, 1, 2, 7, 11, 9,      12, 13, 14, 0, 1, 3, 15, 10,    0},
                                                                                                                     MPKIND__OVERLAP,     3,  s4x4,   s2x4,      0x000, 1};
Private map_thing map_ov_s1x8_0         = {{0, 1, 3, 2, 10, 11, 4, 5,         4, 5, 10, 11, 6, 7, 9, 8},             MPKIND__OVERLAP,     2,  s1x12,  s1x8,      0x000, 0};
Private map_thing map_ov_s1x8_1         = {{0, 1, 3, 2, 7, 6, 4, 5,           4, 5, 7, 6, 12, 13, 15, 14,     15, 14, 12, 13, 8, 9, 11, 10,   0},
                                                                                                                     MPKIND__OVERLAP,     3,  s1x16,  s1x8,      0x000, 0};
Private map_thing map_ov_qtag_0         = {{0, 1, 5, 11, 7, 8, 9, 10,         1, 2, 3, 4, 6, 7, 11, 5},              MPKIND__OVERLAP,     2,  s3dmd,  s_qtag,    0x000, 0};
Private map_thing map_ov_qtag_2         = {{0, 1, 15, 14, 10, 11, 12, 13,     1, 2, 6, 7, 9, 10, 14, 15,      2, 3, 4, 5, 8, 9, 7, 6,         0},
                                                                                                                     MPKIND__OVERLAP,     3,  s4dmd,  s_qtag,    0x000, 0};

/* Maps for turning triangles into boxes for the "triangle" concept. */

        map_thing map_trngl_box1        = {{1, 2, -1, 0,                                                         1}, MPKIND__NONE,        1,  s_trngl, s2x2,     0x000, 0};
        map_thing map_trngl_box2        = {{1, 2, 0, -1,                                                         1}, MPKIND__NONE,        1,  s_trngl, s2x2,     0x000, 0};

        map_thing map_inner_box         = {{2, 3, 4, 5, 10, 11, 12, 13,                                          0}, MPKIND__NONE,        1,  s2x8,   s2x4,      0x000, 0};

Private map_thing map_lh_s2x4_2         = {{20, 21, 22, 23, 16, 17, 18, 19,   4, 5, 6, 7, 8, 9, 10, 11},             MPKIND__OFFS_L_HALF, 2,  s3x8,   s2x4,      0x000, 0};
Private map_thing map_lh_s2x4_3         = {{20, 21, 22, 23, 12, 13, 14, 15,   0, 1, 2, 3, 8, 9, 10, 11},             MPKIND__OFFS_L_HALF, 2,  s4x6,   s2x4,      0x000, 1};
Private map_thing map_lh_s1x4_2         = {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_L_HALF, 2,  s1p5x8, s1x4,      0x000, 0};
Private map_thing map_lh_s1x4_3         = {{9, 8, 6, 7,                       0, 1, 3, 2},                           MPKIND__OFFS_L_HALF, 2,  s2x6,   s1x4,      0x000, 1};
Private map_thing map_lh_s2x2_2         = {{10, 11, 8, 9,                     2, 3, 4, 5},                           MPKIND__OFFS_L_HALF, 2,  s3x4,   s2x2,      0x000, 0};
Private map_thing map_lh_s2x2_3         = {{11, 8, 9, 10,                     3, 4, 5, 2},                           MPKIND__OFFS_L_HALF, 2,  s3x4,   s2x2,      0x005, 0};
Private map_thing map_lh_bigd           = {{6, 7, 8, 9,                       0, 1, 2, 3},                           MPKIND__OFFS_L_HALF, 2,  sbigdmd,s_trngl4,  0x007, 0};

Private map_thing map_lh_s2x4_0         = {{0, 1, 2, 3, 6, 7, 8, 9,                                              0}, MPKIND__OFFS_L_HALF, 1,  s2x6,   s2x4,      0x000, 0};
Private map_thing map_lh_s2x4_1         = {{10, 11, 2, 3, 4, 5, 8, 9,                                            0}, MPKIND__OFFS_L_HALF, 1,  s3x4,   s2x4,      0x000, 1};
Private map_thing map_lh_s1x8_0         = {{0, 1, 2, 3, 4, 5, 6, 7,                                              2}, MPKIND__OFFS_L_HALF, 1,  s1x8,   s1x8,      0x000, 0};
        map_thing map_lh_s2x3_3         = {{1, 2, 4, 5, 7, 3,                 13, 15, 11, 9, 10, 12},                MPKIND__OFFS_L_HALF, 2,  s4x4,   s2x3,      0x005, 1};
        map_thing map_lh_s2x3_2         = {{9, 11, 7, 5, 6, 8,                13, 14, 0, 1, 3, 15},                  MPKIND__OFFS_L_HALF, 2,  s4x4,   s2x3,      0x000, 0};

Private map_thing map_rh_s2x4_2         = {{0, 1, 2, 3, 23, 22, 21, 20,       11, 10, 9, 8, 12, 13, 14, 15},         MPKIND__OFFS_R_HALF, 2,  s3x8,   s2x4,      0x000, 0};
Private map_thing map_rh_s2x4_3         = {{18, 19, 20, 21, 14, 15, 16, 17,   2, 3, 4, 5, 6, 7, 8, 9},               MPKIND__OFFS_R_HALF, 2,  s4x6,   s2x4,      0x000, 1};
Private map_thing map_rh_s1x4_2         = {{0, 1, 3, 2,                       11, 10, 8, 9},                         MPKIND__OFFS_R_HALF, 2,  s1p5x8, s1x4,      0x000, 0};
Private map_thing map_rh_s1x4_3         = {{11, 10, 8, 9,                     2, 3, 5, 4},                           MPKIND__OFFS_R_HALF, 2,  s2x6,   s1x4,      0x000, 1};
Private map_thing map_rh_s2x2_2         = {{0, 1, 11, 10,                     5, 4, 6, 7},                           MPKIND__OFFS_R_HALF, 2,  s3x4,   s2x2,      0x000, 0};
Private map_thing map_rh_s2x2_3         = {{1, 11, 10, 0,                     4, 6, 7, 5},                           MPKIND__OFFS_R_HALF, 2,  s3x4,   s2x2,      0x005, 0};
Private map_thing map_rh_bigd           = {{11, 10, 9, 8,                     5, 4, 3, 2},                           MPKIND__OFFS_R_HALF, 2,  sbigdmd,s_trngl4,  0x00D, 1};

Private map_thing map_rh_s2x4_0         = {{2, 3, 4, 5, 8, 9, 10, 11,                                            0}, MPKIND__OFFS_R_HALF, 1,  s2x6,   s2x4,      0x000, 0};
Private map_thing map_rh_s2x4_1         = {{0, 1, 5, 4, 6, 7, 11, 10,                                            0}, MPKIND__OFFS_R_HALF, 1,  s3x4,   s2x4,      0x000, 1};
Private map_thing map_rh_s1x8_0         = {{0, 1, 2, 3, 4, 5, 6, 7,                                              2}, MPKIND__OFFS_R_HALF, 1,  s1x8,   s1x8,      0x000, 0};
        map_thing map_rh_s2x3_3         = {{15, 11, 6, 8, 9, 10,              0, 1, 2, 7, 3, 14},                    MPKIND__OFFS_R_HALF, 2,  s4x4,   s2x3,      0x005, 1};
        map_thing map_rh_s2x3_2         = {{12, 13, 14, 3, 15, 10,            11, 7, 2, 4, 5, 6},                    MPKIND__OFFS_R_HALF, 2,  s4x4,   s2x3,      0x000, 0};

Private map_thing map_lf_s1x4_2         = {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_L_FULL, 2,  s2x8,   s1x4,      0x000, 0};
Private map_thing map_lf_s1x4_3         = {{11, 10, 8, 9,                     0, 1, 3, 2},                           MPKIND__OFFS_L_FULL, 2,  s2x8,   s1x4,      0x000, 1};
Private map_thing map_lf_s2x2_2         = {{9, 11, 6, 8,                      14, 0, 1, 3},                          MPKIND__OFFS_L_FULL, 2,  s4x4,   s2x2,      0x000, 0};
Private map_thing map_lf_s2x2_3         = {{11, 6, 8, 9,                      0, 1, 3, 14},                          MPKIND__OFFS_L_FULL, 2,  s4x4,   s2x2,      0x005, 0};
Private map_thing map_lf_s2x4_0         = {{0, 1, 2, 3, 8, 9, 10, 11,                                            0}, MPKIND__OFFS_L_FULL, 1,  s2x8,   s2x4,      0x000, 0};
Private map_thing map_lf_s2x4_1         = {{13, 15, 2, 4, 5, 7, 10, 12,                                          0}, MPKIND__OFFS_L_FULL, 1,  s4x4,   s2x4,      0x001, 0};
Private map_thing map_lf_s1x8_0         = {{0, 1, 2, 3, 4, 5, 6, 7,                                              2}, MPKIND__OFFS_L_FULL, 1,  s1x8,   s1x8,      0x000, 0};

Private map_thing map_rf_s1x4_2         = {{0, 1, 3, 2,                       11, 10, 8, 9},                         MPKIND__OFFS_R_FULL, 2,  s2x8,   s1x4,      0x000, 0};
Private map_thing map_rf_s1x4_3         = {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_R_FULL, 2,  s2x8,   s1x4,      0x000, 1};
Private map_thing map_rf_s2x2_2         = {{12, 13, 15, 10,                   7, 2, 4, 5},                           MPKIND__OFFS_R_FULL, 2,  s4x4,   s2x2,      0x000, 0};
Private map_thing map_rf_s2x2_3         = {{13, 15, 10, 12,                   2, 4, 5, 7},                           MPKIND__OFFS_R_FULL, 2,  s4x4,   s2x2,      0x005, 0};
Private map_thing map_rf_s2x4_0         = {{4, 5, 6, 7, 12, 13, 14, 15,                                          0}, MPKIND__OFFS_R_FULL, 1,  s2x8,   s2x4,      0x000, 0};
Private map_thing map_rf_s2x4_1         = {{0, 1, 11, 6, 8, 9, 3, 14,                                            0}, MPKIND__OFFS_R_FULL, 1,  s4x4,   s2x4,      0x001, 0};
Private map_thing map_rf_s1x8_0         = {{0, 1, 2, 3, 4, 5, 6, 7,                                              2}, MPKIND__OFFS_R_FULL, 1,  s1x8,   s1x8,      0x000, 0};

Private map_thing map_blob_1x4a         = {{13, 10, 6, 8,          15, 17, 3, 5,          18, 20, 1, 22,         0}, MPKIND__NONE,        3,  s_bigblob,   s1x4, 0x000, 1};
Private map_thing map_blob_1x4b         = {{19, 16, 12, 14,        21, 23, 9, 11,         0, 2, 7, 4,            0}, MPKIND__NONE,        3,  s_bigblob,   s1x4, 0x000, 1};
Private map_thing map_blob_1x4c         = {{1, 22, 18, 20,         3, 5, 15, 17,          6, 8, 13, 10,          0}, MPKIND__NONE,        3,  s_bigblob,   s1x4, 0x015, 0};
Private map_thing map_blob_1x4d         = {{19, 16, 12, 14,        21, 23, 9, 11,         0, 2, 7, 4,            0}, MPKIND__NONE,        3,  s_bigblob,   s1x4, 0x015, 0};
Private map_thing map_wblob_1x4a        = {{15, 17, 5, 3, 6, 8, 10, 13,       18, 20, 22, 1, 3, 5, 17, 15},          MPKIND__NONE,        2,  s_bigblob,   s2x4, 0x000, 1};
Private map_thing map_wblob_1x4b        = {{21, 23, 11, 9, 12, 14, 16, 19,    0, 2, 4, 7, 9, 11, 23, 21},            MPKIND__NONE,        2,  s_bigblob,   s2x4, 0x000, 1};
Private map_thing map_wblob_1x4c        = {{3, 5, 17, 15, 18, 20, 22, 1,      6, 8, 10, 13, 15, 17, 5, 3},           MPKIND__NONE,        2,  s_bigblob,   s2x4, 0x005, 0};
Private map_thing map_wblob_1x4d        = {{21, 23, 11, 9, 12, 14, 16, 19,    0, 2, 4, 7, 9, 11, 23, 21},            MPKIND__NONE,        2,  s_bigblob,   s2x4, 0x005, 0};



        map_thing map_lz12     = {{10, 9,                1, 11,                5, 7,                 3, 4},          MPKIND__NONE,        4,  s3x4,   s1x2,      0x055, 0};
        map_thing map_rz12     = {{0, 10,                11, 8,                2, 5,                 4, 6},          MPKIND__NONE,        4,  s3x4,   s1x2,      0x055, 0};

        map_thing map_dmd_1x1  = {{0,                    1,                    2,                  3},               MPKIND__NONE,        4,  sdmd,        s1x1, 0x000, 0};
        map_thing map_star_1x1 = {{0,                    1,                    2,                  3},               MPKIND__NONE,        4,  s_star,      s1x1, 0x000, 0};

        map_thing map_qtag_f0           = {{7, 3,                           0, 0,                                0}, MPKIND__NONE,        1,  s_qtag,      s1x2, 0x000, 0};
        map_thing map_qtag_f1           = {{1, 3,                           7, 5},                                   MPKIND__NONE,        2,  s_qtag,      s1x2, 0x005, 0};
        map_thing map_qtag_f2           = {{3, 4,                           0, 7},                                   MPKIND__NONE,        2,  s_qtag,      s1x2, 0x005, 0};

        map_thing map_diag2a            = {{5, 7, 21, 15, 17, 19, 9, 3,                                          0}, MPKIND__NONE,        1,  s4x6,        s2x4,      1, 0};
        map_thing map_diag2b            = {{2, 8, 22, 12, 14, 20, 10, 0,                                         0}, MPKIND__NONE,        1,  s4x6,        s2x4,      1, 0};

        map_thing map_f2x8_4x4          = {{-1, 3, 12, 2, -1, -1, -1, 13, -1, 15, 0, 14, -1, -1, -1, 1,
                                            -1, 7,  8, 6, -1, -1, -1,  9, -1, 11, 4, 10, -1, -1, -1, 5},             MPKIND__SPLIT,       2,  sfat2x8,  s4x4,    0x000, 0};
        map_thing map_w4x4_4x4          = {{13, 15, 11, 10, 6, 8, -1, 9, -1, -1, -1, -1, -1, -1, 12, -1,
                                            -1, -1, -1, -1, -1, -1, 4, -1, 5, 7, 3, 2, 14, 0, -1, 1},                MPKIND__SPLIT,       2,  swide4x4, s4x4,    0x000, 0};
        map_thing map_f2x8_2x8          = {{15, 14, 13, 12, 11, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1, -1
                                            -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, 4, 3, 2, 1, 0},                 MPKIND__SPLIT,       2,  sfat2x8,  s2x8,    0x000, 1};
        map_thing map_w4x4_2x8          = {{-1, -1, 9, 11, 7, 2, -1, -1, -1, -1,  4, 5, 6, 8, -1, -1,
                                            -1, -1, 12, 13, 14, 0, -1, -1, -1, -1, 1, 3, 15, 10, -1, -1},            MPKIND__SPLIT,       2,  swide4x4, s2x8,    0x000, 1};

Private map_thing map_4quads1  = {{0, 2,                 4, 6,                 10, 8,              14, 12},          MPKIND__4_QUADRANTS, 4,  s_c1phan,    s1x2, 0x044, 0};
Private map_thing map_4quads2  = {{1, 3,                 7, 5,                 11, 9,              13, 15},          MPKIND__4_QUADRANTS, 4,  s_c1phan,    s1x2, 0x011, 0};
Private map_thing map_4edges1  = {{13, 14,               1, 2,                 6, 5,               10, 9},           MPKIND__4_EDGES,     4,  s4x4,        s1x2, 0x044, 0};
Private map_thing map_4edges2  = {{2, 3,                 5, 4,                 7, 6,               0, 1},            MPKIND__4_EDGES,     4,  s_thar,      s1x2, 0x011, 0};

Private map_thing map_all_8_1  = {{0, 1, 4, 5,           2, 3, 6, 7},                                                MPKIND__ALL_8,       2,  s_thar,      s1x4, 0x004, 0};
Private map_thing map_all_8_2  = {{2, 3, 6, 7,           0, 1, 4, 5},                                                MPKIND__ALL_8,       2,  s_thar,      s1x4, 0x001, 0};
Private map_thing map_all_8_d1 = {{0, 3, 4, 7,           2, 5, 6, 1},                                                MPKIND__ALL_8,       2,  s_thar,      sdmd, 0x004, 0};
Private map_thing map_all_8_d2 = {{2, 5, 6, 1,           0, 3, 4, 7},                                                MPKIND__ALL_8,       2,  s_thar,      sdmd, 0x001, 0};

Private map_thing map_all_8_b1 = {{13, 14, 5, 6,         1, 2, 9, 10},                                               MPKIND__ALL_8,       2,  s4x4,       s2x2, 0x004, 0};
Private map_thing map_all_8_b2 = {{1, 2, 9, 10,          13, 14, 5, 6},                                              MPKIND__ALL_8,       2,  s4x4,       s2x2, 0x001, 0};

Private map_thing map_dmd1     = {{0, 2,                 1, 3},                                                      MPKIND__DMD_STUFF,   2,  sdmd,        s1x2, 0x004, 0};
Private map_thing map_dmd2     = {{1, 3,                 0, 2},                                                      MPKIND__DMD_STUFF,   2,  sdmd,        s1x2, 0x001, 0};

Private map_thing mapovdmd1    = {{0, 3, 4, 7, 2, 5, 6, 1},                                                          MPKIND__DMD_STUFF,   2,  s_thar,      sdmd, 0x104, 0};
Private map_thing mapovdmd2    = {{0, 3, 4, 7, 2, 5, 6, 1},                                                          MPKIND__DMD_STUFF,   2,  s_thar,      sdmd, 0x004, 0};
Private map_thing mapovline1   = {{0, 1, 4, 5, 2, 3, 6, 7},                                                          MPKIND__DMD_STUFF,   2,  s_thar,      s1x4, 0x104, 0};
Private map_thing mapovline2   = {{0, 1, 4, 5, 2, 3, 6, 7},                                                          MPKIND__DMD_STUFF,   2,  s_thar,      s1x4, 0x004, 0};
Private map_thing mapovbox1    = {{0, 1, 2, 3, 1, 2, 3, 0},                                                          MPKIND__DMD_STUFF,   2,  s2x2,        s2x2, 0x104, 0};

        map_thing *maps_3diag[4] = {&map_blob_1x4c, &map_blob_1x4a, &map_blob_1x4d, &map_blob_1x4b};
        map_thing *maps_3diagwk[4] = {&map_wblob_1x4a, &map_wblob_1x4c, &map_wblob_1x4b, &map_wblob_1x4d};

/* Beware!!  These items must be keyed to definition of "mpkind" in sd.h as follows:
                    split
                    |        removed
                    |        |        overlap
                    |        |        |        intlk
                    |        |        |        |        concphan
                    |        |        |        |        |        l_half
                    |        |        |        |        |        |        r_half
                    |        |        |        |        |        |        |        l_full
                    |        |        |        |        |        |        |        |        r_full
                    |        |        |        |        |        |        |        |        |        o_spots
                    |        |        |        |        |        |        |        |        |        |        x_spots
                    |        |        |        |        |        |        |        |        |        |        |        4quads
                    |        |        |        |        |        |        |        |        |        |        |        |        4edges
                    |        |        |        |        |        |        |        |        |        |        |        |        |        all_8
                    |        |        |        |        |        |        |        |        |        |        |        |        |        |        diamond  
                    |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        stag
                    |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        |
                    |        |        |        |        |        |        |        |        |        |        |        |        |        |        |        | */
Private map_hunk mm_1x8_1 = {{0, 0},
                    {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_lh_s1x8_0, 0},
                                                                          {&map_rh_s1x8_0, 0},
                                                                                   {&map_lf_s1x8_0, 0},
                                                                                            {&map_rf_s1x8_0, 0},
                                                                                                     {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x4_1 = {{0, 0},
                    {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_lh_s2x4_0, &map_lh_s2x4_1},
                                                                          {&map_rh_s2x4_0, &map_rh_s2x4_1},
                                                                                   {&map_lf_s2x4_0, &map_lf_s2x4_1},
                                                                                            {&map_rf_s2x4_0, &map_rf_s2x4_1},
                                                                                                     {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x1_2 = {{0, 0},
                    {&map_1x2_1x1, &map_1x2_1x1},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x2_2 = {{0, 0},
                    {&map_1x4_1x2, &map_2x2h},
                             {&map_1x2_rmv, &map_1x2_rmvr},
                                      {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_dmd1, &map_dmd2},
                                                                                                                                                           {0, 0}};

Private map_hunk mm_2x2_2 = {{0, 0},
                    {&map_2x4_2x2, &map_2x4_2x2r},
                             {&map_2x2_rmv, 0},
                                      {&map_ov_s2x2_1, &map_ov_s2x2r_1},
                                               {0, 0},  {0, 0},  {&map_lh_s2x2_2, &map_lh_s2x2_3},
                                                                          {&map_rh_s2x2_2, &map_rh_s2x2_3},
                                                                                   {&map_lf_s2x2_2, &map_lf_s2x2_3},
                                                                                            {&map_rf_s2x2_2, &map_rf_s2x2_3},
                                                                                                     {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_all_8_b1, &map_all_8_b2},
                                                                                                                                                  {&mapovbox1, 0},
                                                                                                                                                           {0, 0}};

Private map_hunk mm_dmd_2 = {{0, 0},
                    {&map_ptp_dmd, &map_qtg_dmd},
                             {&map_dmd_rmv, &map_dmd_rmvr},
                                      {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_all_8_d1, &map_all_8_d2},
                                                                                                                                                  {&mapovdmd1, &mapovdmd2},
                                                                                                                                                           {0, 0}};

Private map_hunk mm_3x4_2 = {{0, 0},
                    {&map_3x8_3x4, &map_4x6_3x4},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_tgl_2 = {{0, 0},
                    {0, &map_s6_trngl},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_tg4_2 = {{0, 0},
                    {0, &map_rig_trngl4},
                             {0, &map_s8_tgl4},
                                      {0, 0},  {0, 0},  {0, 0},  {&map_lh_bigd, 0},
                                                                          {&map_rh_bigd, 0},
                                                                                   {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_qtg_2 = {{0, 0},
                    {&map_hv_qtg_2, &map_vv_qtg_2},
                             {0, 0},  {&map_ov_qtag_0, 0},
                                               {&map_intlk_phantom_dmd, 0},
                                                        {&map_phantom_dmd, 0},
                                                                 {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_spn_2 = {{0, 0},
                    {0, &map_spin_3x4},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_hrg_2 = {{0, 0},
                    {&map_hrgl_ptp, 0},
                             {0, 0},  {0, &map_ov_hrg_1},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_12d_2 = {{0, 0},
                    {&map_bone_12d, &map_bigd_12d},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x3_2 = {{0, 0},
                    {&map_1x6_1x3, &map_2x3_1x3},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x4_2 = {{0, 0},
                    {&map_1x8_1x4, &map_2x4_1x4},
                             {&map_1x4_rmv, &map_1x4_rmvr},
                                      {0, 0},  {0, 0},  {0, 0},  {&map_lh_s1x4_2, &map_lh_s1x4_3},
                                                                          {&map_rh_s1x4_2, &map_rh_s1x4_3},
                                                                                   {&map_lf_s1x4_2, &map_lf_s1x4_3},
                                                                                            {&map_rf_s1x4_2, &map_rf_s1x4_3},
                                                                                                     {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_all_8_1, &map_all_8_2},
                                                                                                                                                  {&mapovline1, &mapovline2},
                                                                                                                                                           {0, 0}};

Private map_hunk mm_1x8_2 = {{0, 0},
                    {&map_1x16_1x8, &map_2x8_1x8},
                             {0, 0},  {&map_ov_s1x8_0, 0},
                                               {&map_intlk_phan_grand, 0},
                                                        {&map_conc_phan_grand, 0},
                                                                 {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x8_3 = {{0, 0},
                    {0, 0},  {0, 0},  {&map_ov_s1x8_1, 0},
                                               {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x4_2 = {{0, 0},
                    {&map_hv_2x4_2, &map_split_f},
                             {&map_2x4_rmv, &map_2x4_rmvr},
                                      {&map_ov_2x4_0, &map_ov_2x4_1},
                                               {&map_intlk_phantom_box, &map_intlk_f},
                                                        {&map_phantom_box, &map_full_f},
                                                                 {&map_lh_s2x4_2, &map_lh_s2x4_3},
                                                                          {&map_rh_s2x4_2, &map_rh_s2x4_3},
                                                                                   {0, 0},  {0, 0},  {&map_o_s2x4_3, &map_o_s2x4_2},
                                                                                                              {&map_x_s2x4_3, &map_x_s2x4_2},
                                                                                                                       {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_staggerv, 0}};

Private map_hunk mm_2x6_2 = {{0, 0},
                    {0, &map_4x6_2x6},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x3_2 = {{0, 0},
                    {&map_2x6_2x3, &map_3x4_2x3},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x3_3 = {{0, 0},
                    {0, 0},  {0, 0},  {0, &map_ov_2x3_3},
                                               {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x6_2 = {{0, 0},
                    {&map_1x12_1x6, &map_2x6_1x6},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x2_3 = {{0, 0},
                    {&map_1x6_1x2, &map_2x3_1x2},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x2_3 = {{0, 0},
                    {&map_2x6_2x2, &map_2x6_2x2r},
                             {0, 0},  {&map_ov_s2x2_2, 0},
                                               {0, 0},  {&map_conc_tb, &map_conc_tbr},
                                                                 {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_dmd_3 = {{0, 0},
                    {0, &map_3dmd_dmd},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_qtg_3 = {{0, 0},
                    {0, 0},  {0, 0},  {&map_ov_qtag_2, 0},
                                               {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x4_3 = {{0, 0},
                    {&map_1x12_1x4, &map_3x4_1x4},
                             {0, 0},  {&map_ov_1x4_2, &map_ov_1x4_3},
                                               {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x4_3 = {{0, 0},
                    {0, &map_4x6_2x4},
                             {0, 0},  {&map_ov_2x4_2, &map_ov_2x4_3},
                                      {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x2_4 = {{0, 0},
                    {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {&map_4quads1, &map_4quads2},
                                                                                                                                {&map_4edges1, &map_4edges2},
                                                                                                                                         {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_2x2_4 = {{0, 0},
                    {&map_2x8_2x2, 0},
                             {0, 0},  {0, 0},  {0, 0},  {&map_conc_qb, 0},
                                                                 {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_dmd_4 = {{0, 0},
                    {0, &map_4dmd_dmd},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

Private map_hunk mm_1x4_4 = {{0, 0},
                    {&map_1x16_1x4, &map_4x4_1x4},
                             {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0},  {0, 0}};

/*
 * The following conditional code is used to turn off the following
 * error message in the MicroSoft Visual C++ compiler since there
 * doesn't appear to be any way to cause the compiler to accept the
 * following table without error.
 *
 * Warning # 4048 is:
 *
 *	"Compiler Warning (level 1) C4048
 *	 
 *	 different declared array subscripts: 'identifier1' and 'identifier2'
 *
 *	 An expression involved pointers to arrays of different size.
 *	 The pointers were used without conversion.
 *
 *	 This warning can be avoided by explicitly casting the arrays to the
 *	 same or equivalent type."
 *
 * Unfortunately there doesn't appear to be any way to perform the cast such that
 * the compiler is happy.  Sigh.
*/

#ifdef _MSC_VER
#pragma warning (disable : 4048)
#endif

/* BEWARE!!  This list is keyed to the definition of "setup_kind" in database.h. */
map_hunk *map_lists[][4] = {
   {0,         0,         0,         0},          /* nothing */
   {0,         &mm_1x1_2, 0,         0},          /* s1x1 */
   {0,         &mm_1x2_2, &mm_1x2_3, &mm_1x2_4},  /* s1x2 */
   {0,         &mm_1x3_2, 0,         0},          /* s1x3 */
   {0,         &mm_2x2_2, &mm_2x2_3, &mm_2x2_4},  /* s2x2 */
   {0,         &mm_1x4_2, &mm_1x4_3, &mm_1x4_4},  /* s1x4 */
   {0,         &mm_dmd_2, &mm_dmd_3, &mm_dmd_4},  /* sdmd */
   {0,         0,         0,         0},          /* s_star */
   {0,         &mm_tgl_2, 0,         0},          /* s_trngl */
   {0,         &mm_tg4_2, 0,         0},          /* s_trngl4 */
   {0,         0,         0,         0},          /* s_bone6 */
   {0,         0,         0,         0},          /* s_short6 */
   {0,         &mm_12d_2, 0,         0},          /* s_1x2dmd */
   {0,         0,         0,         0},          /* s_2x1dmd */
   {0,         &mm_qtg_2, &mm_qtg_3, 0},          /* s_qtag */
   {0,         0,         0,         0},          /* s_bone */
   {0,         0,         0,         0},          /* s_rigger */
   {0,         &mm_spn_2, 0,         0},          /* s_spindle */
   {0,         &mm_hrg_2, 0,         0},          /* s_hrglass */
   {0,         0,         0,         0},          /* s_dhrglass */
   {0,         0,         0,         0},          /* s_hyperglass */
   {0,         0,         0,         0},          /* s_crosswave */
   {&mm_1x8_1, &mm_1x8_2, &mm_1x8_3, 0},          /* s1x8 */
   {&mm_2x4_1, &mm_2x4_2, &mm_2x4_3, 0},          /* s2x4 */
   {0,         &mm_2x3_2, &mm_2x3_3, 0},          /* s2x3 */
   {0,         &mm_1x6_2, 0,         0},          /* s1x6 */
   {0,         &mm_3x4_2, 0,         0},          /* s3x4 */
   {0,         &mm_2x6_2, 0,         0},          /* s2x6 */
   {0,         0,         0,         0},          /* s1p5x8 */
   {0,         0,         0,         0},          /* s2x8 */
   {0,         0,         0,         0},          /* s4x4 */
   {0,         0,         0,         0},          /* sx1x6 */
   {0,         0,         0,         0},          /* s1x10 */
   {0,         0,         0,         0},          /* s1x12 */
   {0,         0,         0,         0},          /* s1x14 */
   {0,         0,         0,         0},          /* s1x16 */
   {0,         0,         0,         0},          /* s_c1phan */
   {0,         0,         0,         0},          /* s_bigblob */
   {0,         0,         0,         0},          /* s_ptpd */
   {0,         0,         0,         0},          /* s1x3dmd */
   {0,         0,         0,         0},          /* s3x1dmd */
   {0,         0,         0,         0},          /* s3dmd */
   {0,         0,         0,         0},          /* s4dmd */
   {0,         0,         0,         0},          /* s_wingedstar */
   {0,         0,         0,         0},          /* s_wingedstar12 */
   {0,         0,         0,         0},          /* s_wingedstar16 */
   {0,         0,         0,         0},          /* s_galaxy */
   {0,         0,         0,         0},          /* s3x8 */
   {0,         0,         0,         0},          /* s4x6 */
   {0,         0,         0,         0},          /* s_thar */
   {0,         0,         0,         0},          /* sx4dmd */
   {0,         0,         0,         0},          /* s8x8 */
   {0,         0,         0,         0},          /* sfat2x8 */
   {0,         0,         0,         0},          /* swide4x4 */
   {0,         0,         0,         0},          /* sbigdmd */
   {0,         0,         0,         0}};         /* s_normal_concentric */
