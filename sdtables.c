/* -*- mode:C; c-basic-offset:3; indent-tabs-mode:nil; -*- */

/* SD -- square dance caller's helper.

    Copyright (C) 1990-1998  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 31. */

/* This defines the following external variables:
   getout_strings
   filename_strings
   level_threshholds
   higher_acceptable_level
   menu_names
   id_bit_table_2x6_pg
   id_bit_table_bigdmd_wings
   id_bit_table_bigbone_wings
   id_bit_table_bighrgl_wings
   id_bit_table_bigdhrgl_wings
   id_bit_table_3x4_offset
   id_bit_table_3x4_h
   id_bit_table_3x4_ctr6
   id_bit_table_525_nw
   id_bit_table_525_ne
   id_bit_table_343_outr
   id_bit_table_343_innr
   id_bit_table_545_outr
   id_bit_table_545_innr
   id_bit_table_3dmd_in_out
   id_bit_table_3dmd_ctr1x6
   id_bit_table_3dmd_ctr1x4
   id_bit_table_3ptpd
   conc_init_table
   f2x4far
   f2x4near
   fdhrgl
   f323
   sel_init_table
   setup_attrs
   begin_sizes
   startinfolist
   map_p8_tgl4
   map_spndle_once_rem
   map_1x3dmd_once_rem
   map_lh_zzztgl
   map_rh_zzztgl
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
   map_4x4_ns
   map_4x4_ew
   map_vsplit_f
   map_stairst
   map_ladder
   map_but_o
   map_offset
   map_4x4v
   map_blocks
   map_trglbox
   map_2x3_0134
   map_2x3_1245
   map_1x8_1x6
   map_rig_1x6
   map_4x6_2x4
   map_ov_hrg_1
   map_ov_gal_1
   map_3o_qtag_1
   map_tgl4_1
   map_tgl4_2
   map_qtag_2x3
   map_2x3_rmvr
   map_dbloff1
   map_dbloff2
   map_dhrgl1
   map_dhrgl2
   map_dbgbn1
   map_dbgbn2
   map_off1x81
   map_off1x82
   map_trngl_box1
   map_trngl_box2
   map_inner_box
   map_lh_s2x3_3
   map_lh_c1phana
   map_lh_c1phanb
   map_lh_s2x3_2
   map_rh_c1phana
   map_rh_c1phanb
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
   map_diag23a
   map_diag23b
   map_diag23c
   map_diag23d
   map_f2x8_4x4
   map_w4x4_4x4
   map_f2x8_2x8
   map_w4x4_2x8
   maps_3diag
   maps_3diagwk
   map_init_table2
   split_lists
*/

#include "sd.h"

/* These are used in setup_coords. */

static coordrec thing1x1 = {s1x1, 3,
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

static coordrec thing1x2 = {s1x2, 3,
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

static coordrec thing1x3 = {s1x3, 3,
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

static coordrec thing1x4 = {s1x4, 3,
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

static coordrec thing1x6 = {s1x6, 3,
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

static coordrec thing1x8 = {s1x8, 3,
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

static coordrec thing1x10 = {s1x10, 4,
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

static coordrec thing1x12 = {s1x12, 4,
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

static coordrec thing1x14 = {s1x14, 4,
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

static coordrec thing1x16 = {s1x16, 4,
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

static coordrec thingdmd = {sdmd, 3,
   { -5,   0,   4,   0},
   {  0,   2,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1,  1,  2, -1, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec nicethingdmd = {sdmd, 3,
   { -5,   0,   5,   0},
   {  0,   2,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1,  1,  2, -1, -1,
      -1, -1, -1, -1,  3, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing2x2 = {s2x2, 3,
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

static coordrec thing2x3 = {s2x3, 3,
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

static coordrec thing2x4 = {s2x4, 3,
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

static coordrec thing2x5 = {s2x5, 3,
   { -8,  -4,   0,   4,   8,   8,   4,   0,  -4,  -8},
   {  2,   2,   2,   2,   2,  -2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3,  4, -1,
      -1, -1,  9,  8,  7,  6,  5, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing2x6 = {s2x6, 3,
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

static coordrec thing2x8 = {s2x8, 3,
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

static coordrec thing3x6 = {s3x6, 3,
   {-10,  -6,  -2,   2,   6,  10,  10,   6,   2,  10,   6,   2,  -2,  -6, -10, -10,  -6,  -2},
   {  4,   4,   4,   4,   4,   4,   0,   0,   0,  -4,  -4,  -4,  -4,  -4,  -4,   0,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  3,  4,  5, -1,
      -1, 15, 16, 17,  8,  7,  6, -1,
      -1, 14, 13, 12, 11, 10,  9, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing3x8 = {s3x8, 3,
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

static coordrec thing3x4 = {s3x4, 3,
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

static coordrec thing_d3x4 = {s_d3x4, 3,
   { -8,  -4,   0,   4,   8,   8,   8,   4,   0,  -4,  -8,  -8},
   {  4,   2,   2,   2,   4,   0,  -4,  -2,  -2,  -2,  -4,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1, -1, -1,  4, -1,
      -1, -1, 11,  1,  2,  3,  5, -1,
      -1, -1, 10,  9,  8,  7,  6, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing_323 = {s_323, 3,
   { -4,   0,   4,   2,   4,   0,  -4,  -2},
   {  4,   4,   4,   0,  -4,  -4,  -4,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1,  2, -1, -1,
      -1, -1, -1,  7,  3, -1, -1, -1,
      -1, -1, -1,  6,  5,  4, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing_343 = {s_343, 3,
   { -4,   0,   4,   6,   2,   4,   0,  -4,  -6,  -2},
   {  4,   4,   4,   0,   0,  -4,  -4,  -4,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1,  2, -1, -1,
      -1, -1,  8,  9,  4,  3, -1, -1,
      -1, -1, -1,  7,  6,  5, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing_525 = {s_525, 3,
   { -8,  -4,   0,   4,   8,   2,   8,   4,   0,  -4,  -8,  -2},
   {  4,   4,   4,   4,   4,   0,  -4,  -4,  -4,  -4,  -4,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3,  4, -1,
      -1, -1, -1, 11,  5, -1, -1, -1,
      -1, -1, 10,  9,  8,  7,  6, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing_545 = {s_545, 3,
   { -8,  -4,   0,   4,   8,   6,   2,   8,   4,   0,  -4,  -8,  -6,  -2},
   {  4,   4,   4,   4,   4,   0,   0,  -4,  -4,  -4,  -4,  -4,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3,  4, -1,
      -1, -1, 12, 13,  6,  5, -1, -1,
      -1, -1, 11, 10,  9,  8,  7, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing_h545 = {sh545, 3,
   { -8,  -4,   0,   4,   8,   8,   2,   8,   4,   0,  -4,  -8,  -8,  -2},
   {  4,   4,   4,   4,   4,   0,   0,  -4,  -4,  -4,  -4,  -4,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3,  4, -1,
      -1, -1, 12, 13,  6, -1,  5, -1,
      -1, -1, 11, 10,  9,  8,  7, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thinggal = {s_galaxy, 3,
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

static coordrec thingthar = {s_thar, 3,
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

static coordrec thingxwv = {s_crosswave, 3,
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

static coordrec thingrigger = {s_rigger, 3,
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

static coordrec thingspindle = {s_spindle, 3,
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

static coordrec thingbone = {s_bone, 3,
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

static coordrec thingbone6 = {s_bone6, 3,
   { -6,   6,   2,   6,  -6,  -2},
   {  2,   2,   0,  -2,  -2,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  5,  2,  1, -1, -1,
      -1, -1,  4, -1, -1,  3, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thingqtag = {s_qtag, 3,       /* "fudged" coordinates */
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

static coordrec nicethingqtag = {s_qtag, 3,   /* "true" coordinates */
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

static coordrec thingptpd = {s_ptpd, 3,       /* "fudged" coordinates */
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

static coordrec nicethingptpd = {s_ptpd, 3,       /* "true" coordinates */
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

static coordrec thingbigdmd = {sbigdmd, 3,   /* used for both -- symmetric and safe for press/truck */
   { -9,  -5,   0,   0,   5,   9,   9,   5,   0,   0,  -5,  -9},
   {  2,   2,   6,   2,   2,   2,  -2,  -2,  -6,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  2, -1, -1, -1,
      -1,  0,  1, -1,  3,  4,  5, -1,
      -1, 11, 10, -1,  9,  7,  6, -1,
      -1, -1, -1, -1,  8, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing1x3dmd = {s1x3dmd, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec thing3x1dmd = {s3x1dmd, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec thing3dmd = {s3dmd, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec thing4dmd = {s4dmd, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec thingshort6 = {s_short6, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec nicethingphan = {s_c1phan, 3,   /* "true" coordinates for mirroring --
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

static coordrec thingphan = {s_c1phan, 3,   /* "fudged" coordinates -- only truck within each 1x2 is allowed */
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

static coordrec nicethingglass = {s_hrglass, 3,   /* "true" coordinates for mirroring --
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

static coordrec thingglass = {s_hrglass, 3,   /* "fudged" coordinates -- only truck by center 2 is allowed */
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

static coordrec thingdglass = {s_dhrglass, 3,   /* used for both -- symmetric and safe for press/truck */
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

static coordrec thing4x4 = {s4x4, 3,
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

static coordrec thing4x5 = {s4x5, 3,
   { -8,  -4,   0,   4,   8,   8,   4,   0,  -4,  -8,   8,   4,   0,  -4,  -8,  -8,  -4,   0,   4,   8},
   {  6,   6,   6,   6,   6,   2,   2,   2,   2,   2,  -6,  -6,  -6,  -6,  -6,  -2,  -2,  -2,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0,  1,  2,  3,  4, -1,
      -1, -1,  9,  8,  7,  6,  5, -1,
      -1, -1, 15, 16, 17, 18, 19, -1,
      -1, -1, 14, 13, 12, 11, 10, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thing4x6 = {s4x6, 3,
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

static coordrec thing3oqtg = {s3oqtg, 3,
   { -8,   0,   8,  12,   8,   4,   0,  12,   8,   4,   8,   0,  -8, -12,  -8,  -4,   0, -12,  -8,  -4},
   {  6,   6,   6,   2,   2,   2,   2,  -2,  -2,  -2,  -6,  -6,  -6,  -2,  -2,  -2,  -2,   2,   2,   2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1,  0, -1,  1, -1,  2, -1,
      -1, 17, 18, 19,  6,  5,  4,  3,
      -1, 13, 14, 15, 16,  9,  8,  7,
      -1, -1, 12, -1, 11, -1, 10, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thingbigh = {sbigh, 3,
   { -9,  -9,  -9,  -9,  -6,  -2,   9,   9,   9,   9,   6,   2},
   {  6,   2,  -2,  -6,   0,   0,  -6,  -2,   2,   6,   0,   0}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1,  0, -1, -1, -1, -1,  9, -1,
      -1,  1,  4,  5, 11, 10,  8, -1,
      -1,  2, -1, -1, -1, -1,  7, -1,
      -1,  3, -1, -1, -1, -1,  6, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thingbigbone = {sbigbone, 3,
   {-14, -10,  -6,  -2,  10,  14,  14,  10,   6,   2, -10, -14},
   {  2,   2,   0,   0,   2,   2,  -2,  -2,   0,   0,  -2,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
       0,  1,  2,  3,  9,  8,  4,  5,
      11, 10, -1, -1, -1, -1,  7,  6,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};

static coordrec thingbigx = {sbigx, 4,
   {-17, -13,  -9,  -5,   0,   0,  17,  13,   9,   5,   0,   0},
   {  0,   0,   0,   0,   6,   2,   0,   0,   0,   0,  -6,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1,  4, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1,  0,
       1,  2,  3, -1,  5,  9,  8,  7,
       6, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, 11, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, 10, -1, -1, -1}};

static coordrec thingblob = {s_bigblob, 3,
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
   "C4X",
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
   ".C4X",
   ".all",
   ""};

/* This list tells what level calls will be accepted for the "pick level call"
   operation.  When doing a "pick level call, we don't actually require calls
   to be exactly on the indicated level, as long as it's plausibly close. */
/* BEWARE!!  This list is keyed to the definition of "dance_level" in database.h . */
dance_level level_threshholds[] = {
   l_mainstream,
   l_plus,
   l_a1,
   l_a1,      /* If a2 is given, an a1 call is OK. */
   l_c1,
   l_c2,
   l_c3a,
   l_c3a,     /* If c3 is given, a c3a call is OK. */
   l_c3a,     /* If c3x is given, a c3a call is OK. */
   l_c3x,     /* If c4a is given, a c3x call is OK. */
   l_c3x,     /* If c4 is given, a c3x call is OK. */
   l_c3x,     /* If c4x is given, a c3x call is OK. */
   l_dontshow,
   l_nonexistent_concept};

/* This list tells what level calls will be put in the menu and hence made available.
   In some cases, we make calls available that are higher than the requested level.
   When we use such a call, a warning is printed. */

/* BEWARE!!  This list is keyed to the definition of "dance_level" in database.h . */
dance_level higher_acceptable_level[] = {
   l_mainstream,
   l_plus,
   l_a1,
   l_a2,
   l_c1,
   l_c2,
   l_c3a,
   l_c3x,     /* If c3 is given, we allow c3x. */
   l_c3x,
   l_c4a,
   l_c4x,     /* If c4 is given, we allow c4x. */
   l_c4x,
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


#define WESTBIT(otherbits) { ID2_BEAU | otherbits, ID2_TRAILER | otherbits, ID2_BELLE | otherbits, ID2_LEAD | otherbits }
#define EASTBIT(otherbits) { ID2_BELLE | otherbits, ID2_LEAD | otherbits, ID2_BEAU | otherbits, ID2_TRAILER | otherbits }
#define NORTHBIT(otherbits) { ID2_LEAD | otherbits, ID2_BEAU | otherbits, ID2_TRAILER | otherbits, ID2_BELLE | otherbits }
#define SOUTHBIT(otherbits) { ID2_TRAILER | otherbits, ID2_BELLE | otherbits, ID2_LEAD | otherbits, ID2_BEAU | otherbits }
#define NOBIT(otherbits) { otherbits, otherbits, otherbits, otherbits }
#define NWBITS(otherbits) { ID2_LEAD   |ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BELLE | otherbits, ID2_LEAD   |ID2_BELLE | otherbits }
#define SWBITS(otherbits) { ID2_TRAILER|ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BELLE | otherbits, ID2_LEAD   |ID2_BELLE | otherbits, ID2_LEAD   |ID2_BEAU  | otherbits }
#define SEBITS(otherbits) { ID2_TRAILER|ID2_BELLE | otherbits, ID2_LEAD   |ID2_BELLE | otherbits, ID2_LEAD   |ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BEAU  | otherbits }
#define NEBITS(otherbits) { ID2_LEAD   |ID2_BELLE | otherbits, ID2_LEAD   |ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BEAU  | otherbits, ID2_TRAILER|ID2_BELLE | otherbits }


static id_bit_table id_bit_table_1x2[] = {
   WESTBIT(0),
   EASTBIT(0)};

static id_bit_table id_bit_table_2x2[] = {
   NWBITS(0),
   NEBITS(0),
   SEBITS(0),
   SWBITS(0)};

static id_bit_table id_bit_table_2x4[] = {
   NWBITS(ID2_END),
   NEBITS(ID2_CENTER|ID2_CTR4),
   NWBITS(ID2_CENTER|ID2_CTR4),
   NEBITS(ID2_END),
   SEBITS(ID2_END),
   SWBITS(ID2_CENTER|ID2_CTR4),
   SEBITS(ID2_CENTER|ID2_CTR4),
   SWBITS(ID2_END)};

static id_bit_table id_bit_table_2x5[] = {
   NORTHBIT(0),
   NORTHBIT(0),
   NORTHBIT(0),
   NORTHBIT(0),
   NORTHBIT(0),
   SOUTHBIT(0),
   SOUTHBIT(0),
   SOUTHBIT(0),
   SOUTHBIT(0),
   SOUTHBIT(0)};

/* This table is accepted in any circumstances. */
static id_bit_table id_bit_table_2x6[] = {  
   NWBITS(ID2_END),
   NEBITS(ID2_END),
   NWBITS(ID2_CTR4),
   NEBITS(ID2_CTR4),
   NWBITS(ID2_END),
   NEBITS(ID2_END),
   SEBITS(ID2_END),
   SWBITS(ID2_END),
   SEBITS(ID2_CTR4),
   SWBITS(ID2_CTR4),
   SEBITS(ID2_END),
   SWBITS(ID2_END)};

/* This table is only accepted if the population is a parallelogram. */
id_bit_table id_bit_table_2x6_pg[] = {
   NWBITS(ID2_OUTRPAIRS),
   NEBITS(ID2_OUTRPAIRS),
   NWBITS(ID2_CTR4),
   NEBITS(ID2_CTR4),
   NWBITS(ID2_OUTRPAIRS),
   NEBITS(ID2_OUTRPAIRS),
   SEBITS(ID2_OUTRPAIRS),
   SWBITS(ID2_OUTRPAIRS),
   SEBITS(ID2_CTR4),
   SWBITS(ID2_CTR4),
   SEBITS(ID2_OUTRPAIRS),
   SWBITS(ID2_OUTRPAIRS)};

static id_bit_table id_bit_table_1x4[] = {
   WESTBIT(ID2_END),
   EASTBIT(ID2_CENTER),
   EASTBIT(ID2_END),
   WESTBIT(ID2_CENTER)};

static id_bit_table id_bit_table_dmd[] = {
   NOBIT(ID2_END),
   NORTHBIT(ID2_CENTER),
   NOBIT(ID2_END),
   SOUTHBIT(ID2_CENTER)};

static id_bit_table id_bit_table_bone6[] = {
   NORTHBIT(0),
   NORTHBIT(0),
   /* We mark triangle points as trailers, in case we need to identify them in a 2x2 box,
       as in "leads start wheel the ocean". */
   {ID2_BEAU|ID2_TRAILER,     ID2_TRAILER,             ID2_BELLE|ID2_TRAILER,  ID2_LEAD},
   SOUTHBIT(0),
   SOUTHBIT(0),
   {ID2_BELLE|ID2_TRAILER,    ID2_LEAD,                ID2_BEAU|ID2_TRAILER,   ID2_TRAILER}};

static id_bit_table id_bit_table_short6[] = {
   NWBITS(ID2_CENTER),
   NOBIT(ID2_END),
   NEBITS(ID2_CENTER),
   SEBITS(ID2_CENTER),
   NOBIT(ID2_END),
   SWBITS(ID2_CENTER)};

static id_bit_table id_bit_table_1x8[] = {
   WESTBIT(ID2_OUTR6| ID2_OUTR2| ID2_NCTR1X6| ID2_NCTR1X4| ID2_OUTRPAIRS),
   EASTBIT(ID2_OUTR6| ID2_CTR6 | ID2_CTR1X6 | ID2_NCTR1X4| ID2_OUTRPAIRS),
   EASTBIT(ID2_CTR2 | ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4 | ID2_CTR4),
   WESTBIT(ID2_OUTR6| ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4 | ID2_CTR4),
   EASTBIT(ID2_OUTR6| ID2_OUTR2| ID2_NCTR1X6| ID2_NCTR1X4| ID2_OUTRPAIRS),
   WESTBIT(ID2_OUTR6| ID2_CTR6 | ID2_CTR1X6 | ID2_NCTR1X4| ID2_OUTRPAIRS),
   WESTBIT(ID2_CTR2 | ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4 | ID2_CTR4),
   EASTBIT(ID2_OUTR6| ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4 | ID2_CTR4)};

/* Used only if center 6 are contiguous, then a gap, then isolated people. */
static id_bit_table id_bit_table_1x10[] = {
   NOBIT(ID2_OUTR2| ID2_NCTR1X6| ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4  | ID2_CTR4),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4  | ID2_CTR4),
   NOBIT(ID2_OUTR2| ID2_NCTR1X6| ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4  | ID2_CTR4),
   NOBIT(ID2_CTR6 | ID2_CTR1X6 | ID2_CTR1X4  | ID2_CTR4)};

static id_bit_table id_bit_table_qtag[] = {
   NOBIT(ID2_END    | ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_CTR6  | ID2_OUTR6),
   NOBIT(ID2_END    | ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_CTR6  | ID2_OUTR6),
   NOBIT(ID2_CENTER | ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR2 | ID2_OUTR6),
   NOBIT(ID2_CENTER | ID2_CTR4      | ID2_CTR1X4  | ID2_CTR6  | ID2_CTR2),
   NOBIT(ID2_END    | ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_CTR6  | ID2_OUTR6),
   NOBIT(ID2_END    | ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_CTR6  | ID2_OUTR6),
   NOBIT(ID2_CENTER | ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR2 | ID2_OUTR6),
   NOBIT(ID2_CENTER | ID2_CTR4      | ID2_CTR1X4  | ID2_CTR6  | ID2_CTR2)};

static id_bit_table id_bit_table_2stars[] = {
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_CTR2),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_CTR2)};

static id_bit_table id_bit_table_ptpd[] = {
   {ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2,           ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2},
   {ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU,   ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE},
   {ID2_CTR2|ID2_CTR6|ID2_BEAU,     ID2_CTR2|ID2_CTR6|ID2_TRAILER, ID2_CTR2|ID2_CTR6|ID2_BELLE,    ID2_CTR2|ID2_CTR6|ID2_LEAD},
   {ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE,  ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU},
   {ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2,           ID2_OUTR6|ID2_OUTR2,            ID2_OUTR6|ID2_OUTR2},
   {ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE,  ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU},
   {ID2_CTR2|ID2_CTR6|ID2_BELLE,    ID2_CTR2|ID2_CTR6|ID2_LEAD,    ID2_CTR2|ID2_CTR6|ID2_BEAU,     ID2_CTR2|ID2_CTR6|ID2_TRAILER},
   {ID2_OUTR6|ID2_CTR6|ID2_LEAD,    ID2_OUTR6|ID2_CTR6|ID2_BEAU,   ID2_OUTR6|ID2_CTR6|ID2_TRAILER, ID2_OUTR6|ID2_CTR6|ID2_BELLE}};

static id_bit_table id_bit_table_crosswave[] = {
   WESTBIT( ID2_END   |ID2_OUTRPAIRS|ID2_OUTR6|ID2_NCTRDMD| ID2_NCTR1X4),
   EASTBIT( ID2_END   |ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTRDMD | ID2_NCTR1X4),
   NORTHBIT(ID2_CENTER|ID2_CTR4     |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR1X4),
   SOUTHBIT(ID2_CENTER|ID2_CTR4     |ID2_CTR2 |ID2_CTRDMD | ID2_CTR1X4),
   EASTBIT( ID2_END   |ID2_OUTRPAIRS|ID2_OUTR6|ID2_NCTRDMD| ID2_NCTR1X4),
   WESTBIT( ID2_END   |ID2_OUTRPAIRS|ID2_OUTR6|ID2_CTRDMD | ID2_NCTR1X4),
   SOUTHBIT(ID2_CENTER|ID2_CTR4     |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR1X4),
   NORTHBIT(ID2_CENTER|ID2_CTR4     |ID2_CTR2 |ID2_CTRDMD | ID2_CTR1X4)};

static id_bit_table id_bit_table_gal[] = {
   NOBIT(ID2_END),
   NWBITS(ID2_CENTER|ID2_CTR4),
   NOBIT(ID2_END),
   NEBITS(ID2_CENTER|ID2_CTR4),
   NOBIT(ID2_END),
   SEBITS(ID2_CENTER|ID2_CTR4),
   NOBIT(ID2_END),
   SWBITS(ID2_CENTER|ID2_CTR4)};

static id_bit_table id_bit_table_hrglass[] = {
   NOBIT(ID2_END            |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR6),
   NOBIT(ID2_END            |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR6),
   NOBIT(ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTRDMD | ID2_OUTR2),
   NOBIT(ID2_CENTER|ID2_CTR4|ID2_CTR2 |ID2_CTRDMD | ID2_CTR6),
   NOBIT(ID2_END            |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR6),
   NOBIT(ID2_END            |ID2_OUTR6|ID2_NCTRDMD| ID2_CTR6),
   NOBIT(ID2_CENTER|ID2_CTR4|ID2_OUTR6|ID2_CTRDMD | ID2_OUTR2),
   NOBIT(ID2_CENTER|ID2_CTR4|ID2_CTR2 |ID2_CTRDMD | ID2_CTR6)};

static id_bit_table id_bit_table_dhrglass[] = {
   NORTHBIT(ID2_END   |ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   NORTHBIT(ID2_END   |ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(   ID2_CENTER|ID2_CTRDMD | ID2_OUTR6|ID2_NOUTR1X3),
   NORTHBIT(ID2_CENTER|ID2_CTRDMD | ID2_CTR2 |ID2_OUTR1X3),
   SOUTHBIT(ID2_END   |ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   SOUTHBIT(ID2_END   |ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(   ID2_CENTER|ID2_CTRDMD | ID2_OUTR6|ID2_NOUTR1X3),
   SOUTHBIT(ID2_CENTER|ID2_CTRDMD | ID2_CTR2 |ID2_OUTR1X3)};

static id_bit_table id_bit_table_323[] = {
   NOBIT(  ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(  ID2_CTRDMD | ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(  ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   EASTBIT(ID2_CTRDMD | ID2_CTR2 |ID2_NOUTR1X3),
   NOBIT(  ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(  ID2_CTRDMD | ID2_OUTR6|ID2_OUTR1X3),
   NOBIT(  ID2_NCTRDMD| ID2_OUTR6|ID2_OUTR1X3),
   WESTBIT(ID2_CTRDMD | ID2_CTR2 |ID2_NOUTR1X3)};

/* If the population is "common spot diamonds", the richer table below is used instead. */
static id_bit_table id_bit_table_bigdmd[] = {  
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6)};

/* This table is only accepted if the population is "common spot diamonds". */
id_bit_table id_bit_table_bigdmd_wings[] = {  
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6)};

static id_bit_table id_bit_table_bigptpd[] = {  
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_CTR2),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_CTR2),
   NOBIT(ID2_OUTR6),
   NOBIT(ID2_OUTR6)};

/* If the population is a distorted tidal line (2-4-2), the richer table below is used instead. */
static id_bit_table id_bit_table_bigbone[] = {  
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_NCTR1X4 | ID2_OUTR6)};

/* This table is only accepted if the population is a distorted tidal line (2-4-2). */
id_bit_table id_bit_table_bigbone_wings[] = {  
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTR1X4  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4 | ID2_OUTR6)};

/* If the population is a "common spot hourglass", the richer table below is used instead. */
static id_bit_table id_bit_table_bighrgl[] = {  
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6)};

/* This table is only accepted if the population is a "common spot hourglass". */
id_bit_table id_bit_table_bighrgl_wings[] = {  
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6)};

/* If the population is a "common spot distorted hourglass", the richer table below is used instead. */
static id_bit_table id_bit_table_bigdhrgl[] = {  
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_NCTRDMD | ID2_OUTR6)};

/* This table is only accepted if the population is a "common spot distorted hourglass". */
id_bit_table id_bit_table_bigdhrgl_wings[] = {  
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_CTR2),
   NOBIT(ID2_CTR4      | ID2_CTRDMD  | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTRDMD | ID2_OUTR6)};

/* If the population is an "H", the richer table "id_bit_table_3x4_h" below is used instead. */
/* If the population is offset C/L/W, this table is used. */
/* If the center 2x3 is full, the richer table "id_bit_table_3x4_ctr6" below is used instead. */
/* Otherwise the standard table is used, identifying only the center 1x4. */
id_bit_table id_bit_table_3x4_offset[] = {
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_OUTRPAIRS | ID2_NCTR1X4),
   NOBIT(ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_CTR4 | ID2_CTR1X4)};

id_bit_table id_bit_table_3x4_h[] = {
   NOBIT(ID2_OUTR6 | ID2_NCTR1X4|ID2_OUTR1X3),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_OUTR6 | ID2_NCTR1X4|ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_CTR1X4 |ID2_OUTR1X3),
   NOBIT(ID2_CTR2  | ID2_CTR1X4 |ID2_NOUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTR1X4|ID2_OUTR1X3),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_OUTR6 | ID2_NCTR1X4|ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_CTR1X4 |ID2_OUTR1X3),
   NOBIT(ID2_CTR2  | ID2_CTR1X4 |ID2_NOUTR1X3)};

id_bit_table id_bit_table_3x4_ctr6[] = {
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6)};

static id_bit_table id_bit_table_3x4[] = {
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_CTR1X4)};

/* This is only used if the center 2x3 is full. */
static id_bit_table id_bit_table_d3x4[] = {
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_CTR6),
   NOBIT(ID2_OUTR2),
   NOBIT(ID2_CTR6)};

/* If the population is a butterfly, this table is used. */
/* Otherwise, no table is used at all -- there are no ID bits. */
static id_bit_table id_bit_table_4x4[] = {
   NOBIT(ID2_END),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4),
   NOBIT(ID2_END),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4),
   NOBIT(ID2_END),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4),
   NOBIT(ID2_END),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4)};

id_bit_table id_bit_table_525_nw[] = {
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_CTRDMD  | ID2_OUTR1X3),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CTR2  | ID2_CTRDMD  | ID2_NOUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_CTRDMD  | ID2_OUTR1X3),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CTR2  | ID2_CTRDMD  | ID2_NOUTR1X3)};

id_bit_table id_bit_table_525_ne[] = {
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_OUTR6 | ID2_CTRDMD  | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_CTR2  | ID2_CTRDMD  | ID2_NOUTR1X3),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_OUTR6 | ID2_CTRDMD  | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_OUTR6 | ID2_NCTRDMD | ID2_OUTR1X3),
   NOBIT(ID2_CTR2  | ID2_CTRDMD  | ID2_NOUTR1X3)};

id_bit_table id_bit_table_343_outr[] = {
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_NOUTR1X3),
   NOBIT(ID2_NOUTR1X3),
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_OUTR1X3),
   NOBIT(ID2_NOUTR1X3),
   NOBIT(ID2_NOUTR1X3)};

id_bit_table id_bit_table_343_innr[] = {
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4)};

id_bit_table id_bit_table_545_outr[] = {
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_NOUTR1X3|ID2_CTR1X4),
   NOBIT(ID2_NOUTR1X3|ID2_CTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_OUTR1X3 |ID2_NCTR1X4),
   NOBIT(ID2_NOUTR1X3|ID2_CTR1X4),
   NOBIT(ID2_NOUTR1X3|ID2_CTR1X4)};

id_bit_table id_bit_table_545_innr[] = {
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4),
   NOBIT(ID2_CTR1X4 | ID2_CTR4)};

id_bit_table id_bit_table_3dmd_in_out[] = {
   NOBIT(ID2_END | ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_END | ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_CENTER | ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_END | ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_END | ID2_NCTR1X4),
   NOBIT(0),
   NOBIT(ID2_CENTER | ID2_CTR4 | ID2_CTR1X4),
   NOBIT(ID2_CENTER | ID2_CTR4 | ID2_CTR1X4)};

static id_bit_table id_bit_table_spindle[] = {
   NORTHBIT(ID2_CTR6 |ID2_OUTR6),
   NORTHBIT(ID2_CTR6 |ID2_CTR2),
   NORTHBIT(ID2_CTR6 |ID2_OUTR6),
   NOBIT   (ID2_OUTR2|ID2_OUTR6),
   SOUTHBIT(ID2_CTR6 |ID2_OUTR6),
   SOUTHBIT(ID2_CTR6 |ID2_CTR2),
   SOUTHBIT(ID2_CTR6 |ID2_OUTR6),
   NOBIT   (ID2_OUTR2|ID2_OUTR6)};

static id_bit_table id_bit_table_rigger[] = {
   NWBITS( ID2_CTR6 |ID2_CENTER|ID2_CTR4),
   NEBITS( ID2_CTR6 |ID2_CENTER|ID2_CTR4),
   EASTBIT(ID2_OUTR2|ID2_END   |ID2_OUTRPAIRS),
   WESTBIT(ID2_CTR6 |ID2_END   |ID2_OUTRPAIRS),
   SEBITS( ID2_CTR6 |ID2_CENTER|ID2_CTR4),
   SWBITS( ID2_CTR6 |ID2_CENTER|ID2_CTR4),
   WESTBIT(ID2_OUTR2|ID2_END   |ID2_OUTRPAIRS),
   EASTBIT(ID2_CTR6 |ID2_END   |ID2_OUTRPAIRS)};

static id_bit_table id_bit_table_1x3dmd[] = {
   NOBIT(   ID2_END|    ID2_OUTRPAIRS| ID2_OUTR6| ID2_OUTR2| ID2_NCTRDMD),
   NOBIT(   ID2_END|    ID2_OUTRPAIRS| ID2_OUTR6| ID2_CTR6|  ID2_NCTRDMD),
   NOBIT(   ID2_CENTER| ID2_CTR4|      ID2_OUTR6| ID2_CTR6|  ID2_CTRDMD),
   NORTHBIT(ID2_CENTER| ID2_CTR4|      ID2_CTR2|  ID2_CTR6|  ID2_CTRDMD),
   NOBIT(   ID2_END|    ID2_OUTRPAIRS| ID2_OUTR6| ID2_OUTR2| ID2_NCTRDMD),
   NOBIT(   ID2_END|    ID2_OUTRPAIRS| ID2_OUTR6| ID2_CTR6|  ID2_NCTRDMD),
   NOBIT(   ID2_CENTER| ID2_CTR4|      ID2_OUTR6| ID2_CTR6|  ID2_CTRDMD),
   SOUTHBIT(ID2_CENTER| ID2_CTR4|      ID2_CTR2|  ID2_CTR6|  ID2_CTRDMD)};

static id_bit_table id_bit_table_3x1dmd[] = {
   WESTBIT(ID2_OUTR2| ID2_OUTR6| ID2_OUTRPAIRS| ID2_END|    ID2_CTR1X6|  ID2_NCTRDMD | ID2_NCTR1X4),
   EASTBIT(ID2_CTR6|  ID2_OUTR6| ID2_OUTRPAIRS| ID2_END|    ID2_CTR1X6|  ID2_NCTRDMD | ID2_CTR1X4),
   WESTBIT(ID2_CTR6|  ID2_CTR2|  ID2_CTR4|      ID2_CENTER| ID2_CTR1X6|  ID2_CTRDMD  | ID2_CTR1X4),
   NOBIT(  ID2_CTR6|  ID2_OUTR6| ID2_CTR4|      ID2_CENTER| ID2_NCTR1X6| ID2_CTRDMD  | ID2_NCTR1X4),
   EASTBIT(ID2_OUTR2| ID2_OUTR6| ID2_OUTRPAIRS| ID2_END|    ID2_CTR1X6|  ID2_NCTRDMD | ID2_NCTR1X4),
   WESTBIT(ID2_CTR6|  ID2_OUTR6| ID2_OUTRPAIRS| ID2_END|    ID2_CTR1X6|  ID2_NCTRDMD | ID2_CTR1X4),
   EASTBIT(ID2_CTR6|  ID2_CTR2|  ID2_CTR4|      ID2_CENTER| ID2_CTR1X6|  ID2_CTRDMD  | ID2_CTR1X4),
   NOBIT(  ID2_CTR6|  ID2_OUTR6| ID2_CTR4|      ID2_CENTER| ID2_NCTR1X6| ID2_CTRDMD  | ID2_NCTR1X4)};

/* If center diamond is fully occupied and outer diamonds have only points, use this. */
static id_bit_table id_bit_table_3dmd[] = {
   NOBIT(ID2_OUTR6| ID2_NCTRDMD|ID2_OUTR1X3),
   NOBIT(ID2_OUTR6| ID2_CTRDMD |ID2_OUTR1X3),
   NOBIT(ID2_OUTR6| ID2_NCTRDMD|ID2_OUTR1X3),
   NOBIT(           ID2_NCTRDMD),
   NOBIT(           ID2_NCTRDMD),
   NOBIT(ID2_CTR2|  ID2_CTRDMD |ID2_NOUTR1X3),
   NOBIT(ID2_OUTR6| ID2_NCTRDMD|ID2_OUTR1X3),
   NOBIT(ID2_OUTR6| ID2_CTRDMD |ID2_OUTR1X3),
   NOBIT(ID2_OUTR6| ID2_NCTRDMD|ID2_OUTR1X3),
   NOBIT(           ID2_NCTRDMD),
   NOBIT(           ID2_NCTRDMD),
   NOBIT(ID2_CTR2|  ID2_CTRDMD |ID2_NOUTR1X3)};

/* If center 1x6 is fully occupied, use this.  It is external. */
id_bit_table id_bit_table_3dmd_ctr1x6[] = {
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_NCTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_CTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_CTR1X4),
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X6 | ID2_NCTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_NCTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_CTR1X4),
   NOBIT(ID2_CTR1X6  | ID2_CTR1X4)};

/* If only center 1x4 is fully occupied, use this.  It is external. */
id_bit_table id_bit_table_3dmd_ctr1x4[] = {
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_NCTR1X4),
   NOBIT(ID2_CTR1X4),
   NOBIT(ID2_CTR1X4)};

/* If center diamond has only centers and outer diamonds have only points, use this. */
static id_bit_table id_bit_table_4dmd[] = {
   NOBIT(ID2_END|ID2_NCTR1X4|ID2_OUTRPAIRS),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_END|ID2_NCTR1X4|ID2_OUTRPAIRS),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER|ID2_CTR1X4|ID2_CTR4),
   NOBIT(ID2_CENTER|ID2_CTR1X4|ID2_CTR4),
   NOBIT(ID2_END|ID2_NCTR1X4|ID2_OUTRPAIRS),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_END|ID2_NCTR1X4|ID2_OUTRPAIRS),
   NOBIT(0),
   NOBIT(0),
   NOBIT(ID2_CENTER|ID2_CTR1X4|ID2_CTR4),
   NOBIT(ID2_CENTER|ID2_CTR1X4|ID2_CTR4)};

/* If center diamond of triple point-to-point diamonds is fully occupied,
   along with inboard points of other diamonds, this lets us do "triple trade".
   It is external. */
id_bit_table id_bit_table_3ptpd[] = {
   NORTHBIT(ID2_OUTR2| ID2_NCTRDMD),
   NORTHBIT(ID2_CTR6|  ID2_CTRDMD),
   NORTHBIT(ID2_OUTR2| ID2_NCTRDMD),
   NOBIT   (ID2_OUTR2| ID2_NCTRDMD),
   EASTBIT (ID2_CTR6|  ID2_NCTRDMD),
   WESTBIT (ID2_CTR6|  ID2_CTRDMD),
   SOUTHBIT(ID2_OUTR2| ID2_NCTRDMD),
   SOUTHBIT(ID2_CTR6|  ID2_CTRDMD),
   SOUTHBIT(ID2_OUTR2| ID2_NCTRDMD),
   NOBIT   (ID2_OUTR2| ID2_NCTRDMD),
   WESTBIT (ID2_CTR6|  ID2_NCTRDMD),
   EASTBIT (ID2_CTR6|  ID2_CTRDMD)};

static id_bit_table id_bit_table_bone[] = {
   NORTHBIT(ID2_END  |ID2_OUTRPAIRS|ID2_OUTR6),
   NORTHBIT(ID2_END  |ID2_OUTRPAIRS|ID2_OUTR6),
   EASTBIT(ID2_CENTER|ID2_CTR4     |ID2_OUTR6),
   WESTBIT(ID2_CENTER|ID2_CTR4     |ID2_CTR2),
   SOUTHBIT(ID2_END  |ID2_OUTRPAIRS|ID2_OUTR6),
   SOUTHBIT(ID2_END  |ID2_OUTRPAIRS|ID2_OUTR6),
   WESTBIT(ID2_CENTER|ID2_CTR4     |ID2_OUTR6),
   EASTBIT(ID2_CENTER|ID2_CTR4     |ID2_CTR2)};






#define XXX schema_nothing
#define Z schema_concentric
#define DL schema_concentric_diamond_line
#define CD schema_concentric_diamonds
#define OT schema_concentric_others
#define IL schema_intlk_lateral_6
#define IV schema_intlk_vertical_6
#define L6 schema_lateral_6
#define V6 schema_vertical_6
#define QL schema_in_out_quad
#define TL schema_in_out_triple
#define GS schema_grand_single_concentric
#define SI schema_single_concentric
#define S6 schema_conc_star16
#define S2 schema_conc_star12
#define B6 schema_conc_bar16
#define B2 schema_conc_bar12
#define G3 schema_3x3_concentric
#define C4 schema_4x4_cols_concentric
#define L4 schema_4x4_lines_concentric
#define BA schema_conc_bar
#define OO schema_conc_o
#define T6 schema_concentric_6_2_tgl
#define X6P schema_concentric_6p
#define X62 schema_concentric_6_2
#define X26 schema_concentric_2_6
#define B26 schema_concentric_big2_6
#define X24 schema_concentric_2_4
#define CK schema_checkpoint





/*                                                                            center_arity -------|
                                                                              mapelong --------|  |
                                                                             outer_rot -----|  |  |
                                                                      inner_rot ---------|  |  |  |
                                                outlimit -----|          bigsize ----|   |  |  |  |
     bigsetup    lyzer     maps                   inlimit -|  |  insetup  outsetup   |   |  |  |  |   */

cm_thing conc_init_table[] = {
   {s_ptpd,         Z, {0, 2, 4, 6,    3, 1, 7, 5},        4, 4, s1x4,     s2x2,     8,  0, 1, 9, 1,  0x2F5, schema_rev_checkpoint},
   {s_spindle,      Z, {7, 1, 3, 5,    6, 0, 2, 4},        4, 4, sdmd,     s2x2,     8,  0, 1, 1, 1,  0x2F5, schema_rev_checkpoint},
   {s_wingedstar,   Z, {0, 3, 4, 7,    1, 2, 5, 6},        4, 4, sdmd,     s1x4,     8,  0, 0, 9, 1,  0x2FA, schema_ckpt_star},
   {s_wingedstar,   Z, {1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s1x4,     sdmd,     8,  0, 1, 9, 1,  0x2F5, schema_conc_star},
   {s_wingedstar,   Z, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     sdmd,     8,  0, 0, 9, 1,  0x2FA, schema_conc_star},
   {s_wingedstar,   Z, {1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s1x4,     s_star,   8,  0, 1, 9, 1,  0x2F5, schema_conc_star},
   {s_wingedstar,   Z, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     s_star,   8,  0, 0, 9, 1,  0x2FA, schema_conc_star},
   {s_wingedstar12, Z, {11, 2, 3, 4, 9, 10, 5, 8,
                           0, 1, 6, 7},                    4, 4, s_star,   s1x4,     12, 1, 0, 1, 2,  0x2F5, schema_conc_star12},
   {s_wingedstar16, Z, {4, 2, 3, 5, 15, 6, 7, 14, 11, 13, 12, 10,
                           0, 1, 8, 9},                    4, 4, s_star,   s1x4,     16, 1, 0, 1, 3,  0x2F5, schema_conc_star16},
   {s_barredstar,   Z, {6, 9, 1, 4,    7, 8, 0, 2, 3, 5},  4, 6, s_star,   s2x3,     10, 1, 1, 2, 1,  0x2FB, schema_concentric},
   {s2x4,           Z, {6, 5, 1, 2,      7, 0, 3, 4},      2, 4, s1x2,     s2x2,     8,  0, 1, 1, 2,  0x2F7, schema_concentric_others},
   {s_qtag,         Z, {6, 7, 3, 2,      5, 0, 1, 4},      2, 4, s1x2,     s2x2,     8,  0, 1, 1, 2,  0x2FD, schema_concentric_others},
   {s2x6,           Z, {11, 0, 1, 10, 7, 4, 5, 6,
                                       9, 2, 3, 8},        4, 4, s2x2,     s2x2,     12, 1, 1, 2, 2,  0x2FB, TL},
   {sbigbone,       Z, {11, 0, 1, 10, 7, 4, 5, 6,
                                       2, 3, 8, 9},        4, 4, s2x2,     s1x4,     12, 1, 0, 1, 2,  0x2FD, TL},


   {sbigdmd,        Z, {0, 1, 10, 11, 4, 5, 6, 7,       8, 9, 2, 3},
             4, 4, s2x2,     s1x4,     12, 0, 1, 2, 2,  0x2F7, TL},



   {sbigdhrgl,      Z, {11, 0, 1, 10, 7, 4, 5, 6,       9, 2, 3, 8},
             4, 4, s2x2,     sdmd,     12, 1, 0, 1, 2,  0x2FD, TL},
   {sbighrgl,       Z, {11, 0, 1, 10, 7, 4, 5, 6,       8, 9, 2, 3},
             4, 4, s2x2,     sdmd,     12, 1, 1, 1, 2,  0x2FB, TL},
   {sbigrig,        Z, {0, 1, 3, 2, 9, 8, 6, 7,         11, 4, 5, 10},
             4, 4, s1x4,     s2x2,     12, 0, 1, 1, 2,  0x2F7, TL},
   {s4x4,           Z, {8, 6, 4, 5, 12, 13, 0, 14,      11, 15, 3, 7},
             4, 4, s1x4,     s2x2,     12, 0, 1, 2, 2,  0x2FD, TL},
   {s3x4,           Z, {9, 8, 6, 7, 0, 1, 3, 2,         10, -1, 4, -1},
             4, 4, s1x4,     sdmd,     12, 0, 0, 2, 2,  0x2FB, TL},

   {s1x3dmd,       TL, {-1, 0, -1, 1, -1, 5, -1, 4,     2, 3, 6, 7},
             4, 4, sdmd,     sdmd,     12, 1, 0, 1, 2,  0x1FD, XXX},
   {s_3mdmd,        Z, {8, -1, 0, -1, 6, -1, 2, -1,     11, 1, 5, 7},
             4, 4, s1x4,     sdmd,     12, 1, 0, 1, 2,  0x2FD, TL},
   {s_3mdmd,        TL, {8, 9, 0, 10, 6, 4, 2, 3,       11, 1, 5, 7},
             4, 4, sdmd,     sdmd,     12, 1, 0, 1, 2,  0x0FD, TL},
   {s_4mdmd,        QL, {11, 12, 0, 13, 8, 5, 3, 4,     14, 1, 15, 10, 6, 9, 7, 2},
             4, 8, sdmd,     s_ptpd,   16, 1, 0, 1, 2,  0x0FD, QL},

   {s_3mptpd,       Z, {9, -1, 10, -1, 4, -1, 3, -1,    7, 11, 1, 5},
             4, 4, s1x4,     sdmd,     12, 0, 1, 1, 2,  0x2F7, TL},
   {s_3mptpd,       TL, {9, 0, 10, 8, 4, 2, 3, 6,       7, 11, 1, 5},
             4, 4, sdmd,     sdmd,     12, 0, 1, 1, 2,  0x0F7, TL},
   {s_4mptpd,       QL, {12, 0, 13, 11, 5, 3, 4, 8,     1, 2, 6, 7, 9, 10, 14, 15},
             4, 8, sdmd,     s_qtag,   16, 0, 0, 1, 2,  0x0FE, QL},

   {s3dmd,          Z, {8, -1, 0, -1, 6, -1, 2, -1,     7, 11, 1, 5},
             4, 4, s1x4,     sdmd,     12, 1, 1, 1, 2,  0x2FB, TL},
   {s3dmd,          TL, {8, 9, 0, 10, 6, 4, 2, 3,       7, 11, 1, 5},
             4, 4, sdmd,     sdmd,     12, 1, 1, 1, 2,  0x0FB, TL},
   {s4dmd,          QL, {11, 12, 0, 13, 8, 5, 3, 4,     1, 2, 6, 7, 9, 10, 14, 15},
             4, 8, sdmd,     s_qtag,   16, 1, 0, 1, 2,  0x0FD, QL},
   /* This one must not be used for analysis ("going in").  The reason is that we go in
      with stuff separated left-to-right.  But this is top-to-bottom.  Wasn't the
      elongation supposed to take care of all this???? */
   {s3x8,            Z, {17, 16, 14, 15, 2, 3, 5, 4,    20, 21, 23, 22, 8, 9, 11, 10},
             4, 8, s1x4,     s1x8,     24, 0, 0, 2, 2,  0x2FB, QL},


   {s3x8,           Z, {19, 20, 0, 1, 21, 18, 13, 9, 6, 7, 8, 12,     22, 23, 10, 11},
             6, 4, s2x3,     s1x4,     24, 1, 0, 1, 2,  0x2FD, TL},
   /* No!  This conflicts with the next one.  Replace the missing center z peel the top
      in t46t when this gets fixed.
   {s_343,          Z, {7, 6, 5, -1, -1, -1, -1, -1, -1, 2, 1, 0,    8, 9, 3, 4},
             6, 4, s2x3,     s1x4,     10, 0, 0, 2, 2,  0x2FB, TL},
   */
   {s_crosswave,    Z, {-1, 0, -1, -1, 1, -1, -1, 5, -1, -1, 4, -1,       6, 7, 2, 3},
             6, 4, s2x3,     s1x4,      8, 1, 1, 1, 2,  0x2FB, TL},

   {s3ptpd,         Z, {9, -1, 10, -1, 4, -1, 3, -1,    11, 1, 5, 7},
             4, 4, s1x4,     sdmd,     12, 0, 0, 1, 2,  0x2FE, TL},

   {s3ptpd,         TL, {9, 0, 10, 8, 4, 2, 3, 6,       11, 1, 5, 7},
             4, 4, sdmd,     sdmd,     12, 0, 0, 1, 2,  0x0FE, TL},
   {s4ptpd,         QL, {12, 0, 13, 11, 5, 3, 4, 8,     14, 1, 15, 10, 6, 9, 7, 2},
             4, 8, sdmd,     s_ptpd,   16, 0, 0, 1, 2,  0x0FE, QL},

   {s_crosswave,    Z, {-1, 0, -1, 1, -1, 5, -1, 4,     6, 7, 2, 3},
             4, 4, sdmd,     s1x4,     12, 1, 1, 1, 2,  0x2FB, TL},
   {s1x8,           Z, {-1, 0, -1, 1, -1, 5, -1, 4,     3, 2, 7, 6},
             4, 4, sdmd,     s1x4,     12, 1, 0, 1, 2,  0x2F5, TL},
   {s_rigger,       Z, {-1, 6, -1, 7, -1, 3, -1, 2,     0, 1, 4, 5},
             4, 4, sdmd,     s2x2,     12, 1, 0, 1, 2,  0x2F5, TL},
   {s_rigger,       Z, {-1, 6, -1, -1, 7, -1, -1, 3, -1, -1, 2, -1,       0, 1, 4, 5},
             6, 4, s2x3,     s2x2,     12, 1, 0, 1, 2,  0x2FD, TL},
   {s4x6,           Z, {17, 18, 0, 11, 12, 23, 5, 6,    10, 9, 8, 7, 22, 21, 20, 19},
             4, 8, s1x4,     s2x4,     24, 1, 0, 1, 2,  0x2FD, QL},
   {sbigrig,        Z, {0, 1, 3, 2, 9, 8, 6, 7,         -1, 11, 4, -1, -1, 5, 10, -1},
             4, 8, s1x4,     s2x4,     12, 0, 1, 1, 2,  0x2F7, QL},
   {s4x6,           Z, {18, 11, 10, 19, 22, 7, 6, 23,
                            15, 20, 9, 2, 3, 8, 21, 24},   4, 8, s2x2,     s2x4,     16, 1, 1, 1, 2,  0x2FB, QL},
   {s2x4,           Z, {6, 1, 2, 5,    -1, -1, -1, -1, -1, 3, 4, -1,
                           -1, -1, -1, -1, -1, 7, 0, -1},  4,16, s2x2,     s4x4,     8,  1, 1, 1, 1,  0x2FB, schema_conc_o},
   {s_qtag,         Z, {6, 7, 2, 3,    -1, 0, 1, -1, -1, -1, -1, -1,
                           -1, 4, 5, -1, -1, -1, -1, -1},  4,16, s1x4,     s4x4,     8,  0, 1, 1, 1,  0x2FD, schema_conc_o},
   {s_bone,         Z, {6, 7, 2, 3,    -1, -1, -1, -1, -1, 1, 4, -1,
                           -1, -1, -1, -1, -1, 5, 0, -1},  4,16, s1x4,     s4x4,     8,  0, 1, 1, 1,  0x2F7, schema_conc_o},
   {s_hrglass,      Z, {6, 3, 2, 7,    -1, 0, 1, -1, -1, -1, -1, -1,
                           -1, 4, 5, -1, -1, -1, -1, -1},  4,16, sdmd,     s4x4,     8,  0, 1, 1, 1,  0x2FD, schema_conc_o},
   {s_dhrglass,     Z, {6, 3, 2, 7,    -1, -1, -1, -1, -1, 1, 4, -1,
                           -1, -1, -1, -1, -1, 5, 0, -1},  4,16, sdmd,     s4x4,     8,  0, 1, 1, 1,  0x2F7, schema_conc_o},
   {s_bone,         Z, {6, 7, 2, 3,
                              -1, 0, 1, -1, -1, 4, 5, -1}, 4, 8, s1x4,     s2x4,      8, 0, 0, 1, 1,  0x2FE, schema_concentric},
   {sbigdmd,        Z, {8, 9, 2, 3,
                                0, 1, 4, 5, 6, 7, 10, 11}, 4, 8, s1x4,     s2x4,     12, 1, 0, 1, 1,  0x2FD, schema_concentric},


   {s3x4,           Z, {10, 11, 4, 5,
                                0, 1, 2, 3, 6, 7, 8, 9},   4, 8, s1x4,     s2x4,     12, 0, 0, 1, 1,  0x2FB, schema_concentric},

   {s3x6,           Z, {15, 16, 17, 6, 7, 8,     0, 1, 2, 3, 4, 5, 9, 10, 11, 12, 13, 14},
             6, 12, s1x6,    s2x6,     18, 0, 0, 1, 1,  0x2FB, schema_concentric},


   {s4x4,           Z, {11, 15, 3, 7,
                           12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s2x2,     s2x4,     16, 1, 0, 2, 1,  0x2F7, schema_concentric},
   {s4x4,           Z, {15, 3, 7, 11,
                           12, 13, 14, 0, 4, 5, 6, 8},     4, 8, s2x2,     s2x4,     16, 0, 0, 2, 1,  0x2FB, schema_concentric},
   {s2x6,           Z, {9, 2, 3, 8,
                           0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x2,     s2x4,     12, 1, 0, 9, 1,  0x2FD, schema_concentric},
   {s2x6,           Z, {2, 3, 8, 9,
                           0, 1, 4, 5, 6, 7, 10, 11},      4, 8, s2x2,     s2x4,     12, 0, 0, 9, 1,  0x2FE, schema_concentric},
   {s3dmd,         G3, {9, 10, 11, 3, 4, 5,                                             /* This was 2FE VVV */
                                  0, 1, 2, 6, 7, 8},       6, 6, s1x6,     s2x3,     12, 0, 0, 2, 1,  0x0FB, schema_concentric},
   {s3x8,          G3, {21, 22, 23, 9, 10, 11,
                                  19, 20, 0, 7, 8, 12},    6, 6, s1x6,     s2x3,     24, 0, 1, 1, 1,  0x0F7, schema_concentric},

   {s4dmd,         C4, {12, 13, 15, 14, 4, 5, 7, 6,
                          0, 1, 2, 3, 8, 9, 10, 11},       8, 8, s1x8,     s2x4,     16, 0, 0, 2, 1,  0x0FB, schema_concentric},

   {sbigrig,        Z, {11, 4, 5, 10,
                           0, 1, 3, 2, 6, 7, 9, 8},        4, 8, s2x2,     s1x8,     12, 1, 0, 9, 1,  0x2F5, schema_concentric},
   {sbigrig,        Z, {4, 5, 10, 11,
                           0, 1, 3, 2, 6, 7, 9, 8},        4, 8, s2x2,     s1x8,     12, 0, 0, 9, 1,  0x2FA, schema_concentric},
   /*  These were used in a misguided attempt to make "triple cast" work.
   {s2x6,           Z, {1, 2, 3, 4, 7, 8, 9, 10,
                                        11, 0, 5, 6},      8, 4, s2x4,     s2x2,     12, 0, 1, 9, 1,  0x2F7, schema_concentric},
   {s2x6,           Z, {1, 2, 3, 4, 7, 8, 9, 10,
                                        0, 5, 6, 11},      8, 4, s2x4,     s2x2,     12, 0, 0, 9, 1,  0x2FD, schema_concentric},
   */

   {s_bone6,        Z, {5, 2,    4, 0, 1, 3},              2, 4, s1x2,     s2x2,     6,  0, 1, 2, 1,  0x2F7, schema_concentric},
   {s_bone6,      X6P, {5, 2,    0, 1, 3, 4},              2, 4, s1x2,     s2x2,     6,  0, 0, 1, 1,  0x0FE, schema_concentric},
   {s_bone6,      X24, {5, 2,    0, 1, 3, 4},              2, 4, s1x2,     s2x2,     6,  0, 0, 1, 1,  0x0FE, schema_concentric},

   {s2x3,           Z, {4, 1,    5, 0, 2, 3},              2, 4, s1x2,     s2x2,     6,  1, 1, 2, 1,  0x2FB, schema_concentric},
   {s_short6,       Z, {5, 0, 2, 3,    4, 1},              4, 2, s2x2,     s1x2,     6,  1, 1, 2, 1,  0x2FA, schema_concentric},
   {s_1x2dmd,     X6P, {5, 2,    0, 1, 3, 4},              2, 4, s1x2,     s1x4,     6,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_1x2dmd,     X24, {5, 2,    0, 1, 3, 4},              2, 4, s1x2,     s1x4,     6,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s1x6,           Z, {2, 5,    0, 1, 3, 4},              2, 4, s1x2,     s1x4,     6,  0, 0, 1, 1,  0x2FA, schema_concentric},
   /* these two are duplicate. */
   {s_wingedstar,   Z, {7, 2, 3, 6,    0, 1, 4, 5},        4, 4, s_star,   s1x4,     8,  1, 0, 1, 1,  0x2F5, schema_conc_star},
   {s_wingedstar,   Z, {7, 2, 3, 6,    0, 1, 4, 5},        4, 4, s_star,   s1x4,     8,  1, 0, 1, 1,  0x2F5, schema_concentric},
   {s_rigger,       Z, {5, 0, 1, 4,    6, 7, 2, 3},        4, 4, s2x2,     s1x4,     8,  1, 0, 1, 1,  0x2F5, schema_concentric},
   {s_qtag,         Z, {0, 1, 3, 4, 5, 7,    6, 2},        6, 2, s_bone6,  s1x2,     8,  0, 0, 1, 1,  0x2FA, schema_concentric_6_2_tgl},
   {sbigdmd,        Z, {9, 3,
             0, 1, 2, -1, 4, 5, 6, 7, 8, -1, 10, 11},      2,12, s1x2,     sbigdmd, 12,  1, 0, 2, 1,  0x2FD, schema_concentric},
   {sbighrgl,       Z, {9, 3,
             0, 1, 2, -1, 4, 5, 6, 7, 8, -1, 10, 11},      2,12, s1x2,     sbigdmd, 12,  0, 0, 2, 1,  0x2FE, schema_concentric},
   {s3x1dmd,        Z, {1, 2, 3, 5, 6, 7,    0, 4},        6, 2, s_2x1dmd, s1x2,     8,  0, 0, 1, 1,  0x2FA, schema_concentric},
   {s_crosswave,    Z, {0, 1, 3, 4, 5, 7,    6, 2},        6, 2, s_1x2dmd, s1x2,     8,  0, 1, 1, 1,  0x2F5, schema_concentric},
   {s2x4,           Z, {1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s2x2,     s2x2,     8,  0, 1, 9, 1,  0x2F7, schema_concentric},
   {s2x4,           Z, {6, 1, 2, 5,    0, 3, 4, 7},        4, 4, s2x2,     s2x2,     8,  1, 0, 9, 1,  0x2FD, schema_concentric},
   {s2x4,           Z, {6, 1, 2, 5,    7, 0, 3, 4},        4, 4, s2x2,     s2x2,     8,  1, 1, 9, 1,  0x2FB, schema_concentric},
   {s_bone,         Z, {6, 7, 2, 3,    5, 0, 1, 4},        4, 4, s1x4,     s2x2,     8,  0, 1, 9, 1,  0x2F7, schema_concentric},
   {s_qtag,         Z, {6, 7, 2, 3,    5, 0, 1, 4},        4, 4, s1x4,     s2x2,     8,  0, 1, 9, 1,  0x2FD, schema_concentric},
   {s_dhrglass,     Z, {6, 3, 2, 7,    5, 0, 1, 4},        4, 4, sdmd,     s2x2,     8,  0, 1, 9, 1,  0x2F7, schema_concentric},
   {s_hrglass,      Z, {6, 3, 2, 7,    5, 0, 1, 4},        4, 4, sdmd,     s2x2,     8,  0, 1, 9, 1,  0x2FD, schema_concentric},

   {s_crosswave,   CD, {1, 3, 5, 7,    0, 2, 4, 6},        4, 4, sdmd,     sdmd,     8,  0, 0, 9, 1,  0x0FA, schema_concentric},
   /* This next map, with its loss of elongation information for the outer diamond, is necessary to make the call
      "with confidence" work from the setup formed by having the centers partner tag in left-hand waves.  This means that certain
      Bakerisms involving concentric diamonds, in which each diamond must remember its own elongation, are not possible.  Too bad. */
   {s_crosswave,    Z, {1, 3, 5, 7,    6, 0, 2, 4},        4, 4, sdmd,     sdmd,     8,  0, 1, 9, 1,  0x2F5, schema_concentric},
   {s3x1dmd,        Z, {1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s1x4,     sdmd,     8,  0, 1, 9, 1,  0x2F5, schema_concentric},
   {s3x1dmd,       DL, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     sdmd,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_galaxy,       Z, {7, 1, 3, 5,    0, 2, 4, 6},        4, 4, s2x2,     sdmd,     8,  1, 0, 9, 1,  0x2F5, schema_concentric},
   {s_galaxy,       Z, {1, 3, 5, 7,    0, 2, 4, 6},        4, 4, s2x2,     sdmd,     8,  0, 0, 9, 1,  0x2FA, schema_concentric},
   {s_crosswave,    Z, {1, 3, 5, 7,    6, 0, 2, 4},        4, 4, sdmd,     s_star,   8,  0, 1, 9, 1,  0x2F5, schema_concentric},
   {s_crosswave,    Z, {1, 3, 5, 7,    0, 2, 4, 6},        4, 4, sdmd,     s_star,   8,  0, 0, 9, 1,  0x2FA, schema_concentric},
   {s3x1dmd,        Z, {1, 2, 5, 6,    7, 0, 3, 4},        4, 4, s1x4,     s_star,   12, 0, 1, 9, 1,  0x2F5, schema_concentric},
   {s3x1dmd,        Z, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     s_star,   12, 0, 0, 1, 1,  0x2FA, schema_concentric},

   {s3dmd,          Z, {10, 11, 4, 5,    0, 2, 6, 8},      4, 4, s1x4,     s2x2,    12,  0, 0, 3, 1,  0x0EF, schema_concentric},
   {s3dmd,          Z, {10, 11, 4, 5,    8, 0, 2, 6},      4, 4, s1x4,     s2x2,    12,  0, 1, 3, 1,  0x2DF, schema_concentric},
   {s4x4,           Z, {15, 3, 7, 11,    12, 0, 4, 8},     4, 4, s2x2,     s2x2,    16,  0, 0, 3, 1,  0x0CF, schema_concentric},

   {s1x3dmd,       DL, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     sdmd,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s_crosswave,   DL, {1, -1, 5, -1,    0, 2, 4, 6},      4, 4, s1x4,     sdmd,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s1x8,          DL, {1, -1, 5, -1,    0, -1, 4, -1},    4, 4, s1x4,     sdmd,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   /* This one allows "finish" when the center "line" is actually a diamond whose centers are empty.
      This can happen because of the preference given for lines over diamonds at the conclusion of certain calls. */
                                                                                 /* used to be 1, and still is, 'cuz it has to be! */
   {s_wingedstar,  DL, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s1x4,     sdmd,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s_wingedstar12,DL, {1, 2, 4, 7, 8, 10,
                           11, 0, 3, 5, 6, 9},             6, 6, s1x6,     s_short6, 12, 0, 1, 1, 1,  0x0F5, schema_conc_star12},
   {s_wingedstar16,DL, {1, 2, 6, 5, 9, 10, 14, 13,
                           3, 7, 12, 8, 11, 15, 4, 0},     8, 8, s1x8,    s_spindle, 16, 0, 0, 1, 1,  0x0FA, schema_conc_star16},


   {s_barredstar12,DL, {11, 12, 13, 4, 5, 6,
                           0, 1, 2, 3, 7, 8, 9, 10},       6, 8, s1x6,     s2x4,    14,  0, 0, 2, 1,  0x0FA, schema_conc_bar12},
   {s_barredstar16,DL, {14, 15, 17, 16, 5, 6, 8, 7,
                      0, 1, 2, 3, 4, 9, 10, 11, 12, 13},   8,10, s1x8,     s2x5,    18,  0, 0, 2, 1,  0x0FA, schema_conc_bar16},
   {s_barredstar,  DL, {8, 9, 3, 4,
                           0, 1, 2, 5, 6, 7},              4, 6, s1x4,     s2x3,    10,  0, 0, 2, 1,  0x0FB, schema_concentric},
   {s_qtag,        OT, {6, 7, 3, 2,    0, 1, 4, 5},        2, 4, s1x2,     s2x2,     8,  0, 0, 2, 2,  0x100, schema_nothing},
   {s_spindle,     OT, {6, 0, 5, 1, 4, 2,    7, 3},        2, 2, s1x2,     s1x2,     8,  1, 0, 1, 3,  0x0F5, schema_concentric_others},
   {s1x8,          OT, {1, 3, 2, 6, 7, 5,    0, 4},        2, 2, s1x2,     s1x2,     8,  0, 0, 1, 3,  0x0FA, schema_concentric_others},
   {s2x4,          OT, {1, 2, 6, 5,    0, 3, 4, 7},        2, 4, s1x2,     s2x2,     8,  0, 0, 1, 2,  0x100, schema_nothing},
   {s_galaxy,      IL, {0, 3, 5, 4, 7, 1,    6, 2},        3, 2, s_trngl,  s1x2,     8,  3, 1, 1, 2,  0x0FE, schema_intlk_lateral_6},
   {s_hrglass,     IV, {7, 0, 1, 3, 4, 5,    6, 2},        3, 2, s_trngl,  s1x2,     8,  0, 0, 1, 2,  0x0FE, schema_intlk_vertical_6},
   {s_galaxy,      IV, {2, 5, 7, 6, 1, 3,    0, 4},        3, 2, s_trngl,  s1x2,     8,  2, 0, 1, 2,  0x0FD, schema_intlk_vertical_6},
   {s_galaxy,      L6, {7, 0, 1, 3, 4, 5,    6, 2},        6, 2, s_short6, s1x2,     8,  1, 1, 1, 1,  0x100, schema_nothing},
   {s_galaxy,      V6, {1, 2, 3, 5, 6, 7,    0, 4},        6, 2, s_short6, s1x2,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   /* This next one is duplicate!  It has to be! */
   {s_hrglass,     V6, {5, 0, 3, 1, 4, 7,    6, 2},        6, 2, s_bone6,  s1x2,     8,  1, 0, 1, 1,  0x100, schema_nothing},


   {s2x8,          QL, {0, 1, 14, 15, 6, 7, 8, 9,       2, 3, 4, 5, 10, 11, 12, 13},
             4, 8, s2x2,     s2x4,     16, 0, 0, 1, 2,  0x0FE, QL},
   {s4x4,          QL, {8, 6, 4, 5, 12, 13, 0, 14,      10, 15, 3, 1, 2, 7, 11, 9},
             4, 8, s1x4,     s2x4,     16, 0, 0, 2, 2,  0x0FB, QL},
   {s_hrglass,     TL, {5, 4, -1, -1, -1, -1, 1, 0,     6, 3, 2, 7},
             4, 4, s2x2,     sdmd,     12, 0, 0, 2, 2,  0x100, XXX},
   {s_dhrglass,    TL, {-1, -1, 0, 5, 4, 1, -1, -1,     6, 3, 2, 7},
             4, 4, s2x2,     sdmd,     12, 1, 0, 1, 2,  0x100, XXX},
   {s3x4,          TL, {9, 8, 6, 7, 0, 1, 3, 2,         10, 11, 4, 5},
             4, 4, s1x4,     s1x4,     12, 0, 0, 2, 2,  0x0FB, TL},
   {s_qtag,         Z, {5, 4, 0, 1,                     6, 7, 2, 3},
             2, 4, s1x2,     s1x4,      8, 0, 0, 2, 2,  0x2FB, TL},

   {s4dmd,          Z, {11, 12, 0, 13, 8, 5, 3, 4,      10, -1, -1, 1, 2, -1, -1, 9},
             4, 8, sdmd,     s2x4,     16, 1, 1, 1, 2,  0x2FB, QL},
   {s4dmd,          Z, {11, -1, 0, -1, 8, -1, 3, -1,    1, 2, 6, 7, 9, 10, 14, 15},
             4, 8, s1x4,     s_qtag,   16, 1, 0, 1, 2,  0x2FD, QL},


   {s1x12,         TL, {0, 1, 3, 2, 9, 8, 6, 7,         4, 5, 10, 11},
             4, 4, s1x4,     s1x4,     12, 0, 0, 1, 2,  0x0FE, TL},
   {sbigx,         TL, {0, 1, 3, 2, 9, 8, 6, 7,         10, 11, 4, 5},
             4, 4, s1x4,     s1x4,     12, 0, 1, 1, 2,  0x0F7, TL},
   {sbigh,         TL, {3, 2, 0, 1, 6, 7, 9, 8,         4, 5, 10, 11},
             4, 4, s1x4,     s1x4,     12, 1, 0, 1, 2,  0x0FD, TL},
   {sbigbone,      TL, {0, 1, 10, 11, 4, 5, 6, 7,       2, 3, 8, 9},
             4, 4, s2x2,     s1x4,     12, 0, 0, 1, 2,  0x0FE, TL},
   {sbigrig,       TL, {0, 1, 3, 2, 9, 8, 6, 7,         4, 5, 10, 11},
             4, 4, s1x4,     s2x2,     12, 0, 0, 1, 2,  0x0FE, TL},
   {sbigdmd,       TL, {11, 0, 1, 10, 7, 4, 5, 6,       8, 9, 2, 3},
             4, 4, s2x2,     s1x4,     12, 1, 1, 1, 2,  0x0FB, TL},
   {sbighrgl,      TL, {0, 1, 10, 11, 4, 5, 6, 7,       8, 9, 2, 3},
             4, 4, s2x2,     sdmd,     12, 0, 1, 1, 2,  0x0F7, TL},
   {sbigdhrgl,     TL, {0, 1, 10, 11, 4, 5, 6, 7,       9, 2, 3, 8},
             4, 4, s2x2,     sdmd,     12, 0, 0, 1, 2,  0x0FE, TL},
   {s2x6,          TL, {0, 1, 10, 11, 4, 5, 6, 7,       2, 3, 8, 9},
             4, 4, s2x2,     s2x2,     12, 0, 0, 1, 2,  0x0FE, TL},



   {s4x6,          TL, {17, 18, 11, 0, 1, 10, 19, 16, 13, 22, 7, 4, 5, 6, 23, 12,
                           15, 20, 9, 2, 3, 8, 21, 14},
             8, 8, s2x4,     s2x4,     24, 1, 1, 1, 2,  0x0FB, TL},
   {s3x6,          TL, {14, 15, 0, 1, 16, 13, 10, 7, 4, 5, 6, 9,     12, 17, 2, 3, 8, 11},
             6, 6, s2x3,     s2x3,     18, 1, 1, 1, 2,  0x0FB, TL},



   {s4x4,          TL, {8, 6, 4, 5, 12, 13, 0, 14,
                                       15, 3, 7, 11},      4, 4, s1x4,     s2x2,     12, 0, 0, 2, 2,  0x0FB, TL},
   {s1x8,          GS, {2, 6, 3, 7, 1, 5,    0, 4},        2, 2, s1x2,     s1x2,     8,  0, 0, 1, 3,  0x0FA, schema_grand_single_concentric},
   {s_spindle,     GS, {5, 1, 6, 0, 4, 2,    7, 3},        2, 2, s1x2,     s1x2,     8,  1, 0, 1, 3,  0x0F5, schema_grand_single_concentric},
   {s1x6,          GS, {2, 5, 1, 4,    0, 3},              2, 2, s1x2,     s1x2,     6,  0, 0, 1, 2,  0x0FA, schema_grand_single_concentric},
   {s1x4,          SI, {1, 3,          0, 2},              2, 2, s1x2,     s1x2,     4,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {sdmd,          SI, {3, 1,          0, 2},              2, 2, s1x2,     s1x2,     4,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_wingedstar16,S6, {2, 3, 5, 4, 6, 7, 14, 15, 13, 12, 10, 11,
                           0, 1, 8, 9},                    4, 4, s_star,   s1x4,     16, 0, 0, 1, 3,  0x0FA, schema_conc_star16},

   {s4x4,          L4, {10, 15, 3, 1, 2, 7, 11, 9,
                           12, 13, 14, 0, 4, 5, 6, 8},     8, 8, s2x4,     s2x4,     16, 0, 0, 2, 1,  0x0FB, schema_concentric},
   {s4x4,          C4, {6, 11, 15, 13, 14, 3, 7, 5,
                           8, 9, 10, 12, 0, 1, 2, 4},      8, 8, s2x4,     s2x4,     16, 1, 1, 2, 1,  0x100, XXX},
   {s4x6,          L4, {10, 9, 8, 7, 22, 21, 20, 19,                                 /* This used to be 2FD, need this for 4x4 countershake from blocks */
                           17, 18, 11, 0, 5, 6, 23, 12},   8, 8, s2x4,     s2x4,     16, 0, 1, 2, 1,  0x0F7, schema_concentric},
   {s4x6,          C4, {10, 9, 8, 7, 22, 21, 20, 19,
                           17, 18, 11, 0, 5, 6, 23, 12},   8, 8, s2x4,     s2x4,     16, 0, 1, 2, 1,  0x100, XXX},

   {s_wingedstar12,S2, {2, 3, 4, 11, 10, 5, 8, 9,
                           0, 1, 6, 7},                    4, 4, s_star,   s1x4,     12, 0, 0, 1, 2,  0x0FA, schema_conc_star12},

                /* elong used to be 1, changed to 2 to make 12 matrix relay the diamond work.  V  */
   {s3x4,          G3, {8, 11, 1, 2, 5, 7,
                           9, 10, 0, 3, 4, 6},             6, 6, s2x3,     s2x3,     12, 1, 1, 2, 1,  0x0FA, schema_concentric},
   {s_d3x4,        G3, {1, 2, 3, 7, 8, 9,
                           10, 11, 0, 4, 5, 6},            6, 6, s2x3,     s2x3,     12, 0, 1, 2, 1,  0x0F5, schema_concentric},

   {s_barredstar16,B6, {15, 1, 16, 12, 17, 2, 8, 11, 7, 3, 6, 10,
                           13, 14, 0, 4, 5, 9},            4, 6, s_star,   s2x3,    18,  0, 1, 2, 3,  0x0F5, schema_conc_bar16},
   {s2x8,          B6, {1, 2, 13, 14, 3, 4, 11, 12, 5, 6, 9, 10,
                           0, 7, 8, 15},                   4, 4, s2x2,     s2x2,    16,  0, 0, 1, 3,  0x100, schema_nothing},
   {s_barredstar12,B2, {12, 1, 13, 9, 6, 2, 5, 8,
                           10, 11, 0, 3, 4, 7},            4, 6, s_star,   s2x3,    14,  0, 1, 2, 2,  0x0F5, schema_conc_bar12},
   {s2x6,          B2, {1, 2, 9, 10, 3, 4, 7, 8,
                           0, 5, 6, 11},                   4, 4, s2x2,     s2x2,    12,  0, 0, 1, 2,  0x100, schema_nothing},
   {s_barredstar,  BA, {9, 1, 4, 6,    7, 8, 0, 2, 3, 5},  4, 6, s_star,   s2x3,    10,  0, 1, 2, 1,  0x0F7, schema_concentric},


   /* these 2 are sort of duplicates */
   {s2x4,          BA, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s2x2,     s2x2,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s2x4,           Z, {1, 2, 5, 6,    0, 3, 4, 7},        4, 4, s2x2,     s2x2,     8,  0, 0, 1, 1,  0x0FE, schema_concentric},

   {s_qtag,        OO, {6, 7, 2, 3,    -1, -1, -1, -1, -1, 4, 5, -1,
                           -1, -1, -1, -1, -1, 0, 1, -1},  4,16, s1x4,     s4x4,     8,  0, 0, 1, 1,  0x0FB, schema_conc_o},
   {s4x4,          OO, {15, 3, 7, 11,    -1, 1, 2, -1, -1, 5, 6, -1,
                           -1, 9, 10, -1, -1, 13, 14, -1}, 4,16, s2x2,     s4x4,     16, 0, 0, 1, 1,  0x0CF, schema_conc_o},
   {s2x4,          OO, {1, 2, 5, 6,    -1, 3, 4, -1, -1, -1, -1, -1,
                           -1, 7, 0, -1, -1, -1, -1, -1},  4,16, s2x2,     s4x4,     8,  0, 0, 1, 1,  0x0FE, schema_conc_o},
   {s_hrglass,     OO, {6, 3, 2, 7,    -1, -1, -1, -1, -1, 4, 5, -1,
                           -1, -1, -1, -1, -1, 0, 1, -1},  4,16, sdmd,     s4x4,     8,  0, 0, 1, 1,  0x0FB, schema_conc_o},
   {s_dhrglass,    OO, {6, 3, 2, 7,    -1, 1, 4, -1, -1, -1, -1, -1,
                           -1, 5, 0, -1, -1, -1, -1, -1},  4,16, sdmd,     s4x4,     8,  0, 0, 1, 1,  0x0FE, schema_conc_o},
   {s_bone,        OO, {6, 7, 2, 3,    -1, 1, 4, -1, -1, -1, -1, -1,
                           -1, 5, 0, -1, -1, -1, -1, -1},  4,16, s1x4,     s4x4,     8,  0, 0, 1, 1,  0x0FE, schema_conc_o},
   {s_qtag,        T6, {0, 1, 3, 4, 5, 7,    6, 2},        6, 2, s_bone6,  s1x2,     8,  0, 0, 1, 1,  0x100, schema_nothing},

   /* these 2 are sort of duplicates */
   {s_short6,     X6P, {0, 2, 3, 5,    4, 1},              4, 2, s2x2,     s1x2,     6,  0, 1, 2, 1,  0x100, schema_nothing},
   {s_short6,       Z, {0, 2, 3, 5,    4, 1},              4, 2, s2x2,     s1x2,     6,  0, 1, 2, 1,  0x0F5, schema_concentric},

   {s2x3,         X6P, {4, 1,    0, 2, 3, 5},              2, 4, s1x2,     s2x2,     6,  1, 0, 1, 1,  0x0FD, schema_concentric},
   {s2x3,         X24, {4, 1,    0, 2, 3, 5},              2, 4, s1x2,     s2x2,     6,  1, 0, 1, 1,  0x0FD, schema_concentric},
   {s_2x1dmd,     X6P, {0, 1, 3, 4,    5, 2},              4, 2, s1x4,     s1x2,     6,  0, 1, 1, 1,  0x0F5, schema_concentric},
   {s3x1dmd,      X62, {0, 1, 2, 4, 5, 6,    7, 3},        6, 2, s1x6,     s1x2,     8,  0, 1, 1, 1,  0x0F5, schema_concentric},
   {s_crosswave,  X62, {6, 7, 1, 2, 3, 5,    0, 4},        6, 2, s_2x1dmd, s1x2,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s1x3dmd,      X62, {1, 2, 3, 5, 6, 7,    0, 4},        6, 2, s_1x2dmd, s1x2,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   /* This next one is duplicated!!!!  It has to be!!!  I wish I knew where. */
   {s_hrglass,    X62, {5, 0, 3, 1, 4, 7,    6, 2},        6, 2, s_bone6,  s1x2,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_ptpd,       X62, {1, 7, 6, 5, 3, 2,    0, 4},        6, 2, s_bone6,  s1x2,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s1x8,         X62, {1, 3, 2, 5, 7, 6,    0, 4},        6, 2, s1x6,     s1x2,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_rigger,     X62, {5, 7, 0, 1, 3, 4,    6, 2},        6, 2, s_short6, s1x2,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_qtag,       X62, {5, 7, 0, 1, 3, 4,    6, 2},        6, 2, s2x3,     s1x2,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_spindle,    X62, {0, 1, 2, 4, 5, 6,    7, 3},        6, 2, s2x3,     s1x2,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_crosswave,  X26, {7, 3,    0, 1, 2, 4, 5, 6},        2, 6, s1x2,     s_2x1dmd, 8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s3x1dmd,      X26, {2, 6,    0, 1, 3, 4, 5, 7},        2, 6, s1x2,     s_2x1dmd, 8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s1x3dmd,      X26, {7, 3,    0, 1, 2, 4, 5, 6},        2, 6, s1x2,     s1x6,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_ptpd,       X26, {2, 6,    3, 0, 1, 7, 4, 5},        2, 6, s1x2,     s_short6, 8,  0, 1, 2, 1,  0x0F7, schema_concentric},
   {s3x4,         X26, {11, 5,    9, 10, 0, 3, 4, 6},      2, 6, s1x2,     s2x3,     12, 0, 1, 2, 1,  0x0F5, schema_concentric},
   {s1x8,         X26, {2, 6,    0, 1, 3, 4, 5, 7},        2, 6, s1x2,     s1x6,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_dhrglass,   X26, {7, 3,    0, 1, 2, 4, 5, 6},        2, 6, s1x2,     s_bone6,  8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s_3mdmd,      B26, {7, 1,    0, 2, 5, 6, 8, 11},       2, 6, s1x2,     s_bone6, 12,  1, 0, 1, 1,  0x0FD, B26},
   {s4dmd,        B26, {15, 7,   0, 3, 6, 8, 11, 14},      2, 6, s1x2,     s_bone6, 16,  0, 0, 1, 1,  0x0FE, B26},
   {s_bone,       X26, {7, 3,    0, 1, 2, 4, 5, 6},        2, 6, s1x2,     s_bone6,  8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_qtag,       X26, {7, 3,    5, 6, 0, 1, 2, 4},        2, 6, s1x2,     s_short6, 8,  0, 1, 1, 1,  0x0FD, schema_concentric},
   {s_hrglass,    X26, {7, 3,    5, 6, 0, 1, 2, 4},        2, 6, s1x2,     s_short6, 8,  1, 1, 1, 1,  0x0FE, schema_concentric},
   {s_spindle,    X26, {5, 1,    6, 7, 0, 2, 3, 4},        2, 6, s1x2,     s_short6, 8,  1, 1, 2, 1,  0x0FB, schema_concentric},
   {s3dmd,        X26, {11, 5,    0, 1, 2, 6, 7, 8},       2, 6, s1x2,     s2x3,     12, 0, 0, 9, 1,  0x100, schema_nothing},
   {s_323,        X26, {7, 3,     0, 1, 2, 4, 5, 6},       2, 6, s1x2,     s2x3,      8, 0, 0, 2, 1,  0x0FA, schema_concentric},
   {s1x3dmd,       CK, {0, 3, 4, 7,    1, 2, 5, 6},        4, 4, sdmd,     s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_rev_checkpoint},
   {s_ptpd,        CK, {0, 2, 4, 6,    1, 7, 5, 3},        4, 4, s1x4,     s2x2,     8,  0, 0, 1, 1,  0x0FA, schema_rev_checkpoint},
   {s1x8,          CK, {0, 2, 4, 6,    1, 3, 5, 7},        4, 4, s1x4,     s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_rev_checkpoint},
   {s_dhrglass,    CK, {0, 3, 1, 4, 7, 5,    6, 2},        6, 2, s2x3,     s1x2,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s_spindle,     CK, {7, 1, 3, 5,    0, 2, 4, 6},        4, 4, sdmd,     s2x2,     8,  0, 0, 1, 1,  0x0FA, schema_rev_checkpoint},
   {s_rigger,      CK, {5, 6, 0, 1, 2, 4,    7, 3},        6, 2, s_short6, s1x2,     8,  1, 0, 1, 1,  0x100, schema_nothing},
   {s_bone,        CK, {0, 1, 3, 4, 5, 7,    6, 2},        6, 2, s_bone6,  s1x2,     8,  0, 0, 1, 1,  0x100, schema_nothing},
   {s_thar,         Z, {1, 3, 5, 7,    0, 2, 4, 6},        4, 4, s_star,   s_star,   8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_galaxy,       Z, {1, 3, 5, 7,    0, 2, 4, 6},        4, 4, s2x2,     s_star,   8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   /* Next two are duplicate */
   {s_wingedstar,   Z, {2, 3, 6, 7,    0, 1, 4, 5},        4, 4, s_star,   s1x4,     8,  0, 0, 1, 1,  0x2FA, schema_conc_star},
   {s_wingedstar,   Z, {2, 3, 6, 7,    0, 1, 4, 5},        4, 4, s_star,   s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s3x1dmd,        Z, {7, 2, 3, 6,    0, 1, 4, 5},        4, 4, sdmd,     s1x4,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s1x3dmd,        Z, {2, 3, 6, 7,    0, 1, 4, 5},        4, 4, sdmd,     s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_crosswave,    Z, {6, 7, 2, 3,    0, 1, 4, 5},        4, 4, s1x4,     s1x4,     8,  1, 0, 1, 1,  0x0F5, schema_concentric},
   {s1x8,           Z, {3, 2, 7, 6,    0, 1, 4, 5},        4, 4, s1x4,     s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_dhrglass,     Z, {6, 3, 2, 7,    0, 1, 4, 5},        4, 4, sdmd,     s2x2,     8,  0, 0, 1, 1,  0x0FE, schema_concentric},
   {s_hrglass,      Z, {6, 3, 2, 7,    0, 1, 4, 5},        4, 4, sdmd,     s2x2,     8,  0, 0, 2, 1,  0x0FB, schema_concentric},
   {s_rigger,       Z, {0, 1, 4, 5,    6, 7, 2, 3},        4, 4, s2x2,     s1x4,     8,  0, 0, 1, 1,  0x0FA, schema_concentric},
   {s_bone,         Z, {6, 7, 2, 3,    0, 1, 4, 5},        4, 4, s1x4,     s2x2,     8,  0, 0, 1, 1,  0x0FE, schema_concentric},
   {s_qtag,         Z, {6, 7, 2, 3,    0, 1, 4, 5},        4, 4, s1x4,     s2x2,     8,  0, 0, 2, 1,  0x0FB, schema_concentric},


   {nothing,        Z, {0},                                0, 0, nothing,  nothing},
};



static Const fixer f1x8aa;
static Const fixer f1x8ctr;
static Const fixer foozz;
static Const fixer fo6zz;
static Const fixer fqtgend;
static Const fixer f1x6aad;
static Const fixer f1x8aad;
static Const fixer foo66d;
static Const fixer foo55d;
static Const fixer f2x4endd;
static Const fixer fgalch;
static Const fixer fspindld;
static Const fixer fspindlbd;
static Const fixer f1x8endd;
static Const fixer bar55d;
static Const fixer f1x12outer;
static Const fixer f3x4rzz;
static Const fixer f3x4lzz;
static Const fixer f4x4rzz;
static Const fixer f4x4lzz;
static Const fixer f3dmoutrd;
static Const fixer fspindlod;
static Const fixer f323d;
static Const fixer fppaad;
static Const fixer fpp55d;
static Const fixer f1x3aad;
static Const fixer f1x2aad;
static Const fixer fboneendo;
static Const fixer distrig1;
static Const fixer distrig2;
static Const fixer distrig5;
static Const fixer distrig6;
static Const fixer dgald1;
static Const fixer dgald2;
static Const fixer dgald3;
static Const fixer dgald4;
static Const fixer d2x4x1;
static Const fixer d2x4c1;
static Const fixer d2x4x2;
static Const fixer d2x4c2;
static Const fixer d4x4l1;
static Const fixer d4x4l2;
static Const fixer d4x4l3;
static Const fixer d4x4l4;
static Const fixer d4x4d1;
static Const fixer d4x4d2;
static Const fixer d4x4d3;
static Const fixer d4x4d4;
static Const fixer box6c;
static Const fixer box9c;
static Const fixer fdrhgl_bb;
static Const fixer frigctr;


/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer foo33a    = {s1x2, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {5, 4, 0, 1}};
static Const fixer foocc     = {s1x2, s2x4,        0, 0, 2,       &foocc,     &foo33a,    0,          0, 0,          0,    0,          0,          {2, 3, 7, 6}};
static Const fixer foo33     = {s1x2, s2x4,        0, 0, 2,       &foo33,     &foocc,     0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};
static Const fixer foo5a     = {s1x2, s2x4,        1, 0, 2,       &foo5a,     0,          0,          0, 0,          0,    0,          0,          {1, 6, 3, 4}};
static Const fixer fooa5     = {s1x2, s2x4,        1, 0, 2,       &fooa5,     0,          0,          0, 0,          0,    0,          0,          {0, 7, 2, 5}};
static Const fixer foo55     = {s1x1, s2x4,        0, 0, 4,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 2, 4, 6}};
static Const fixer fooaa     = {s1x1, s2x4,        0, 0, 4,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 3, 5, 7}};

static Const fixer foo11     = {s1x1, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 4}};
static Const fixer foo22     = {s1x1, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 5}};
static Const fixer foo44     = {s1x1, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {2, 6}};
static Const fixer foo88     = {s1x1, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {3, 7}};

static Const fixer n1x43     = {s1x2, s1x4,        0, 0, 1,       &n1x43,     &box9c,     0,          0, 0,          0,    0,          0,          {0, 1}};
static Const fixer n1x4c     = {s1x2, s1x4,        0, 0, 1,       &n1x4c,     &box6c,     0,          0, 0,          0,    0,          0,          {3, 2}};
static Const fixer n1x45     = {s1x1, s1x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 2}};
static Const fixer n1x4a     = {s1x2, s1x4,        0, 0, 1,       &n1x4a,     0,          0,          0, 0,          0,    0,          0,          {1, 3}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f1x6aa    = {s1x2, s1x6,        0, 0, 2,       &f1x6aa,    &fo6zz,     0,          0, 0,          0,    0,          0,          {0, 1, 4, 3}};
static Const fixer f1x8aa    = {s1x2, s1x8,        0, 0, 2,       &f1x8aa,    &foozz,     0,          0, 0,          0,    0,          0,          {1, 3, 7, 5}};
static Const fixer fo6zz     = {s1x2, s_bone6,     1, 0, 2,       &fo6zz,     &f1x6aa,    0,          0, 0,          0,    0,          0,          {0, 4, 1, 3}};
static Const fixer foozz     = {s1x2, s_ptpd,      1, 0, 2,       &foozz,     &f1x8aa,    0,          0, 0,          0,    0,          0,          {1, 3, 7, 5}};
static Const fixer fo6zzd    = {s2x2, s_bone6,     0, 1, 1,       0,          0,          &f1x6aad,   0, 0,          0,    &fo6zzd,    0,          {0, 1, 3, 4}};
static Const fixer foozzd    = {s2x2, s_ptpd,      0, 1, 1,       0,          0,          &f1x8aad,   0, 0,          0,    &foozzd,    &fqtgend,   {1, 7, 5, 3}};
static Const fixer f3x4left  = {s1x2, s3x4,        0, 0, 2,       &f3x4left,  &f3x4rzz,   0,          0, 0,          0,    0,          0,          {0, 1, 7, 6}};
static Const fixer f3x4right = {s1x2, s3x4,        0, 0, 0x100+2, &f3x4right, &f3x4lzz,   0,          0, 0,          0,    0,          0,          {2, 3, 9, 8}};
static Const fixer f3x4lzz   = {s1x2, s2x6,        0, 0, 2,       &f3x4lzz,   &f3x4right, 0,          0, 0,          0,    0,          0,          {0, 1, 7, 6}};
static Const fixer f3x4rzz   = {s1x2, s2x6,        0, 0, 0x100+2, &f3x4rzz,   &f3x4left,  0,          0, 0,          0,    0,          0,          {4, 5, 11, 10}};
static Const fixer f4x4left  = {s1x2, s4x4,        0, 0, 2,       &f4x4left,  &f4x4rzz,   0,          0, 0,          0,    0,          0,          {12, 13, 5, 4}};
static Const fixer f4x4right = {s1x2, s4x4,        0, 0, 0x100+2, &f4x4right, &f4x4lzz,   0,          0, 0,          0,    0,          0,          {14, 0, 8, 6}};
static Const fixer f4x4lzz   = {s1x2, s2x8,        0, 0, 2,       &f4x4lzz,   &f4x4right, 0,          0, 0,          0,    0,          0,          {0, 1, 9, 8}};
static Const fixer f4x4rzz   = {s1x2, s2x8,        0, 0, 0x100+2, &f4x4rzz,   &f4x4left,  0,          0, 0,          0,    0,          0,          {6, 7, 15, 14}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f3x4outer = {s1x3, s3x4,        1, 0, 2,       &f3x4outer, 0,          0,          0, 0,          0,    0,          0,          {0, 10, 9, 3, 4, 6}};
static Const fixer f3dmouter = {s1x3, s3dmd,       0, 0, 2,       &f3dmouter, 0,          0,          0, 0,          0,    0,          0,          {8, 7, 6, 0, 1, 2}};
       Const fixer fdhrgl    = {s_trngl,s_dhrglass,3, 0, 2,       &f323,      0,          0,          0, 0,          0,    0,          0,          {6, 5, 0, 2, 1, 4}};
static Const fixer f525nw    = {s1x3, s_525,       0, 0, 2,       &f525nw,    0,          0,          0, 0,          0,    0,          0,          {8, 7, 6, 0, 1, 2}};
static Const fixer f525ne    = {s1x3, s_525,       0, 0, 2,       &f525ne,    0,          0,          0, 0,          0,    0,          0,          {10, 9, 8, 2, 3, 4}};
static Const fixer f1x12outer= {s1x3, s1x12,       0, 0, 2,       &f1x12outer,&f3x4outer, 0,          0, 0,          0,    0,          0,          {0, 1, 2, 8, 7, 6}};

static Const fixer f3x4outrd = {s2x3, s3x4,        1, 0, 1,       0,          0,          0,          0, 0,          0,    &f3x4outrd, &f3dmoutrd, {3, 4, 6, 9, 10, 0}};
static Const fixer f3dmoutrd = {s2x3, s3dmd,       0, 0, 1,       0,          0,          0,          0, 0,          0,    &f3dmoutrd, &f3x4outrd, {0, 1, 2, 6, 7, 8}};
static Const fixer fdhrgld   = {s_bone6,s_dhrglass,0, 0, 1,       0,          &fdhrgld,   0,          0, 0,          &fspindlod,0,     &f323d,     {0, 1, 2, 4, 5, 6}};
static Const fixer f1x12outrd= {s1x6, s1x12,       0, 0, 1,       0,          0,          &f1x12outrd,0, 0,          0,    0,          0,          {0, 1, 2, 6, 7, 8}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f1x3d6    =  {s1x3, s1x3dmd,    0, 0, 2,       &f1x3d6,    &f323,      0,          0, 0,          0,    0,          0,          {0, 1, 2, 6, 5, 4}};
       Const fixer f323    =   {s1x3, s_323,       0, 0, 2,       &f323,      &f1x3d6,    0,          0, 0,          0,    0,          0,          {6, 5, 4, 0, 1, 2}};

static Const fixer f323d   =   {s2x3, s_323,       0, 0, 1,       0,          0,          0,          0, 0,          0,    &f323d,     &f3x4outrd, {0, 1, 2, 4, 5, 6}};
static Const fixer f3x1zzd   = {sdmd, s3x1dmd,     0, 0, 1,       0,          0,          0,          0, &f3x1zzd,   0,    0,          0,          {0, 3, 4, 7}};
static Const fixer f1x3zzd   = {sdmd, s1x3dmd,     0, 0, 1,       0,          0,          &foo55d,    0, &f1x3zzd,   0,    0,          0,          {0, 3, 4, 7}};
static Const fixer f3x1yyd   = {sdmd, s3x1dmd,     0, 0, 1,       0,          0,          &f1x8aad,   0, &f3x1yyd,   0,    0,          0,          {1, 3, 5, 7}};
static Const fixer f2x1yyd   = {sdmd, s_2x1dmd,    0, 0, 1,       0,          0,          &f1x8aad,   0, &f2x1yyd,   0,    0,          0,          {0, 2, 3, 5}};
static Const fixer f1x4xv    = {s1x4, s_crosswave, 1, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,    {2, 3, 6, 7}};
static Const fixer f1x3yyd   = {sdmd, s1x3dmd,     0, 0, 1,       0,          0,          &foo66d,    &f1x4xv, &f1x3yyd, 0, &frigctr,  &frigctr,   {1, 3, 5, 7}};
static Const fixer f1x6aad   = {s1x4, s1x6,        0, 0, 1,       0,          0,          &f1x6aad,   0, &f3x1zzd,   0,    &fo6zzd,    &fo6zzd,    {0, 1, 3, 4}};
static Const fixer f1x8aad   = {s1x4, s1x8,        0, 0, 1,       0,          0,          &f1x8aad,   0, &f3x1zzd,   0,    &foozzd,    &foozzd,    {1, 3, 5, 7}};
static Const fixer fxwv1d    = {sdmd, s_crosswave, 0, 0, 1,       0,          0,          0,          0, &fxwv1d,    0,    0,          0,          {0, 2, 4, 6}};
static Const fixer fxwv2d    = {sdmd, s_crosswave, 0, 0, 1,       0,          0,          0,          0, &fxwv2d,    0,    0,          0,          {0, 3, 4, 7}};
static Const fixer fxwv3d    = {sdmd, s_crosswave, 1, 0, 1,       0,          0,          0,          0, &fxwv3d,    0,    0,          0,          {2, 5, 6, 1}};

static Const fixer fqtgns    = {s1x2, s_qtag,      0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};
static Const fixer ftharns   = {s1x2, s_thar,      1, 0, 2,       &ftharns,   &fqtgns,    0,          0, 0,          0,    0,          0,          {2, 3, 7, 6}};
static Const fixer ftharew   = {s1x2, s_thar,      0, 0, 2,       &ftharew,   &fqtgns,    0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};

static Const fixer fqtgj1    = {s1x2, s_qtag,      1, 0, 2,       &fqtgj1,    0,          0,          0, 0,          0,    0,          0,          {1, 3, 7, 5}};
static Const fixer fqtgj2    = {s1x2, s_qtag,      1, 0, 2,       &fqtgj2,    0,          0,          0, 0,          0,    0,          0,          {0, 7, 3, 4}};
static Const fixer f2x3j1    = {s1x2, s2x3,        0, 0, 2,       &f2x3j1,    0,          0,          0, 0,          0,    0,          0,          {0, 1, 4, 3}};
static Const fixer f2x3j2    = {s1x2, s2x3,        0, 0, 2,       &f2x3j2,    0,          0,          0, 0,          0,    0,          0,          {1, 2, 5, 4}};
static Const fixer fqtgjj1   = {s2x2, s_qtag,      0, 0, 1,       0,          0,          0,          0, 0,          0,    &fqtgjj1,   &fqtgjj1,   {7, 1, 3, 5}};
static Const fixer fqtgjj2   = {s2x2, s_qtag,      0, 0, 1,       0,          0,          0,          0, 0,          0,    &fqtgjj2,   &fqtgjj2,   {0, 3, 4, 7}};
static Const fixer fgalcv    = {s1x2, s_galaxy,    1, 0, 1,       &fgalcv,    &fgalch,    0,          0, 0,          0,    0,          0,          {2, 6}};
static Const fixer fgalch    = {s1x2, s_galaxy,    0, 0, 1,       &fgalch,    &fgalcv,    0,          0, 0,          0,    0,          0,          {0, 4}};
static Const fixer fspindlc  = {s1x2, s_spindle,   1, 0, 2,       &fspindlc,  &f1x3aad,   0,          0, 0,          0,    0,          0,          {0, 6, 2, 4}};
static Const fixer fspindlf  = {s1x2, s_spindle,   0, 0, 2,       &fspindlf,  0,          0,          0, 0,          0,    0,          0,          {1, 2, 6, 5}};
static Const fixer fspindlg  = {s1x2, s_spindle,   0, 0, 2,       &fspindlg,  0,          0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};
static Const fixer fspindlfd = {s2x2, s_spindle,   0, 0, 1,       0,          0,          0,          0, 0,          0,    &fspindlfd, 0,          {1, 2, 5, 6}};
static Const fixer fspindlgd = {s2x2, s_spindle,   0, 0, 1,       0,          0,          0,          0, 0,          0,    &fspindlgd, 0,          {0, 1, 4, 5}};
static Const fixer f1x3aad   = {s1x2, s1x3dmd,     0, 0, 2,       &f1x3aad,   &fspindlc,  0,          0, 0,          0,    0,          0,          {1, 2, 6, 5}};
static Const fixer f2x3c     = {s1x2, s2x3,        1, 0, 2,       &f2x3c,     &f1x2aad,   0,          0, 0,          0,    0,          0,          {0, 5, 2, 3}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer box3c     = {s1x2, s2x2,        0, 0, 1,       &box3c,     0,          0,          0, 0,          0,    0,          0,          {0, 1}};
static Const fixer box6c     = {s1x2, s2x2,        1, 0, 1,       &box6c,     0,          0,          0, 0,          0,    0,          0,          {1, 2}};
static Const fixer box9c     = {s1x2, s2x2,        1, 0, 1,       &box9c,     0,          0,          0, 0,          0,    0,          0,          {0, 3}};
static Const fixer boxcc     = {s1x2, s2x2,        0, 0, 1,       &boxcc,     0,          0,          0, 0,          0,    0,          0,          {3, 2}};
static Const fixer box55     = {s1x1, s2x2,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 2}};
static Const fixer boxaa     = {s1x1, s2x2,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 3}};

static Const fixer f1x2aad   = {s1x2, s_1x2dmd,    0, 0, 2,       &f1x2aad,   &f2x3c,     0,          0, 0,          0,    0,          0,          {0, 1, 4, 3}};
static Const fixer f1x3bbd   = {s1x4, s1x3dmd,     0, 0, 1,       0,          0,          &f1x3bbd,   0, 0,          0,    &fspindld,  &fspindld,  {1, 2, 5, 6}};
static Const fixer fhrglassd = {s2x2, s_hrglass,   0, 2, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 4, 5}};
static Const fixer fspindld  = {s2x2, s_spindle,   0, 1, 1,       0,          0,          &f1x3bbd,   0, 0,          0,    &fspindld,  &fhrglassd, {0, 2, 4, 6}};
static Const fixer fptpzzd   = {s1x4, s_ptpd,      0, 0, 1,       0,          0,          &fptpzzd,   0, &fspindlbd, 0,    0,          0,          {0, 2, 4, 6}};
static Const fixer f3ptpo6   = {s_1x2dmd, s3ptpd,  0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {10, 11, 1, 4, 5, 7}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer fspindlbd = {sdmd, s_spindle,   0, 0, 1,       0,          0,          &fptpzzd,   0, &fspindlbd, 0,    0,          0,          {7, 1, 3, 5}};


/* This map is broken!!!!!! */
static Const fixer fspindlo  = {s_trngl,s_spindle, 1, 0, 2,       &f323,      0,          0,          0, &fspindlo,  &fspindlo, 0,     0,          {7, 0, 6, 3, 4, 2}};


static Const fixer fspindlod = {s_short6,s_spindle,1, 0, 1,       0,          &fdhrgld,   0,          0, 0,          &fspindlod, &f323d, 0,        {2, 3, 4, 6, 7, 0}};
static Const fixer d2x4b1    = {s2x2, s4x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {13, 7, 5, 15}};
static Const fixer d2x4b2    = {s2x2, s4x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {11, 14, 3, 6}};
static Const fixer d2x4w1    = {s1x4, s2x4,        0, 0, 1,       0,          0,          &d2x4w1,    0, 0,          0,    &d2x4b1,    &d2x4b1,    {0, 1, 4, 5}};
static Const fixer d2x4w2    = {s1x4, s2x4,        0, 0, 1,       0,          0,          &d2x4w2,    0, 0,          0,    &d2x4b2,    &d2x4b2,    {7, 6, 3, 2}};
static Const fixer d2x4d1    = {sdmd, s2x4,        0, 0, 1,       0,          0,          &d2x4x1,    0, &d2x4d1,    0,    &d2x4c1,    &d2x4c1,    {7, 1, 3, 5}};
static Const fixer d2x4d2    = {sdmd, s2x4,        0, 0, 1,       0,          0,          &d2x4x2,    0, &d2x4d2,    0,    &d2x4c2,    &d2x4c2,    {0, 2, 4, 6}};
static Const fixer d2x4c1    = {s2x2, s2x4,        0, 0, 1,       0,          0,          &d2x4x1,    0, &d2x4d1,    0,    &d2x4c1,    &d2x4c1,    {1, 3, 5, 7}};
static Const fixer d2x4c2    = {s2x2, s2x4,        0, 0, 1,       0,          0,          &d2x4x2,    0, &d2x4d2,    0,    &d2x4c2,    &d2x4c2,    {0, 2, 4, 6}};
static Const fixer d2x4z1    = {s2x2, s4x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {9, 11, 1, 3}};
static Const fixer d2x4z2    = {s2x2, s4x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {10, 15, 2, 7}};
static Const fixer d2x4y1    = {s2x2, s2x4,        0, 0, 1,       0,          0,          &d2x4w1,    &d2x4z1, 0,    0,    &d2x4y1,    &d2x4y1,    {0, 1, 4, 5}};
static Const fixer d2x4y2    = {s2x2, s2x4,        0, 0, 1,       0,          0,          &d2x4w2,    &d2x4z2, 0,    0,    &d2x4y2,    &d2x4y2,    {2, 3, 6, 7}};
static Const fixer d2x4x1    = {s1x4, s2x4,        0, 0, 1,       0,          0,          &d2x4x1,    0, &d2x4d1,    0,    &d2x4c1,    &d2x4c1,    {7, 1, 3, 5}};
static Const fixer d2x4x2    = {s1x4, s2x4,        0, 0, 1,       0,          0,          &d2x4x2,    0, &d2x4d2,    0,    &d2x4c2,    &d2x4c2,    {0, 6, 4, 2}};
static Const fixer dgalw1    = {s1x4, s_galaxy,    1, 0, 1,       0,          0,          &dgalw1,    0, &dgald1,    0,    0,          0,          {2, 1, 6, 5}};
static Const fixer dgalw2    = {s1x4, s_galaxy,    1, 0, 1,       0,          0,          &dgalw2,    0, &dgald2,    0,    0,          0,          {2, 3, 6, 7}};
static Const fixer dgalw3    = {s1x4, s_galaxy,    0, 0, 1,       0,          0,          &dgalw3,    0, &dgald3,    0,    0,          0,          {0, 1, 4, 5}};
static Const fixer dgalw4    = {s1x4, s_galaxy,    0, 0, 1,       0,          0,          &dgalw4,    0, &dgald4,    0,    0,          0,          {0, 7, 4, 3}};
static Const fixer dgald1    = {sdmd, s_galaxy,    1, 0, 1,       0,          0,          &dgalw1,    0, &dgald1,    0,    0,          0,          {2, 5, 6, 1}};
static Const fixer dgald2    = {sdmd, s_galaxy,    1, 0, 1,       0,          0,          &dgalw2,    0, &dgald2,    0,    0,          0,          {2, 3, 6, 7}};
static Const fixer dgald3    = {sdmd, s_galaxy,    0, 0, 1,       0,          0,          &dgalw3,    0, &dgald3,    0,    0,          0,          {0, 1, 4, 5}};
static Const fixer dgald4    = {sdmd, s_galaxy,    0, 0, 1,       0,          0,          &dgalw4,    0, &dgald4,    0,    0,          0,          {0, 3, 4, 7}};
static Const fixer ddmd1     = {sdmd, s_qtag,      0, 0, 1,       0,          0,          0,          0, &ddmd1,     0,    0,          0,          {6, 1, 2, 5}};
static Const fixer ddmd2     = {sdmd, s_qtag,      0, 0, 1,       0,          0,          0,          0, &ddmd2,     0,    0,          0,          {6, 0, 2, 4}};
static Const fixer distbone1 = {s1x4, s_bone,      0, 0, 1,       0,          0,          &distbone1, 0, 0,          0,    0,          0,          {0, 6, 4, 2}};
static Const fixer distbone2 = {s1x4, s_bone,      0, 0, 1,       0,          0,          &distbone2, 0, 0,          0,    0,          0,          {0, 7, 4, 3}};
static Const fixer distbone5 = {s1x4, s_bone,      0, 0, 1,       0,          0,          &distbone5, 0, 0,          0,    0,          0,          {5, 6, 1, 2}};
static Const fixer distbone6 = {s1x4, s_bone,      0, 0, 1,       0,          0,          &distbone6, 0, 0,          0,    0,          0,          {5, 7, 1, 3}};
static Const fixer distrig3  = {sdmd, s_rigger,    0, 0, 1,       0,          0,          &distrig1,  0, &distrig3,  0,    0,          0,          {7, 0, 3, 4}};
static Const fixer distrig1  = {s1x4, s_rigger,    0, 0, 1,       0,          0,          &distrig1,  0, &distrig3,  0,    0,          0,          {7, 0, 3, 4}};
static Const fixer distrig4  = {sdmd, s_rigger,    0, 0, 1,       0,          0,          &distrig2,  0, &distrig4,  0,    0,          0,          {6, 1, 2, 5}};
static Const fixer distrig2  = {s1x4, s_rigger,    0, 0, 1,       0,          0,          &distrig2,  0, &distrig4,  0,    0,          0,          {6, 5, 2, 1}};
static Const fixer distrig7  = {sdmd, s_rigger,    0, 0, 1,       0,          0,          &distrig5,  0, &distrig7,  0,    0,          0,          {6, 0, 2, 4}};
static Const fixer distrig5  = {s1x4, s_rigger,    0, 0, 1,       0,          0,          &distrig5,  0, &distrig7,  0,    0,          0,          {6, 0, 2, 4}};
static Const fixer distrig8  = {sdmd, s_rigger,    0, 0, 1,       0,          0,          &distrig6,  0, &distrig8,  0,    0,          0,          {7, 1, 3, 5}};
static Const fixer distrig6  = {s1x4, s_rigger,    0, 0, 1,       0,          0,          &distrig6,  0, &distrig8,  0,    0,          0,          {7, 5, 3, 1}};
static Const fixer disthrg1  = {s1x4, s_hrglass,   1, 0, 1,       0,          0,          &disthrg1,  0, 0,          0,    0,          0,          {1, 3, 5, 7}};
static Const fixer disthrg2  = {s1x4, s_hrglass,   1, 0, 1,       0,          0,          &disthrg2,  0, 0,          0,    0,          0,          {0, 3, 4, 7}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer d4x4l1    = {s1x4, s4x4,        1, 0, 1,       0,          0,          &d4x4l1, &d4x4l4, &d4x4d1, &d4x4d4, 0,       0,          {0, 3, 8, 11}};
static Const fixer d4x4l2    = {s1x4, s4x4,        0, 0, 1,       0,          0,          &d4x4l2, &d4x4l3, &d4x4d2, &d4x4d3, 0,       0,          {8, 11, 0, 3}};
static Const fixer d4x4l3    = {s1x4, s4x4,        1, 0, 1,       0,          0,          &d4x4l3, &d4x4l2, &d4x4d3, &d4x4d2, 0,       0,          {12, 15, 4, 7}};
static Const fixer d4x4l4    = {s1x4, s4x4,        0, 0, 1,       0,          0,          &d4x4l4, &d4x4l1, &d4x4d4, &d4x4d1, 0,       0,          {12, 15, 4, 7}};

static Const fixer d4x4d1    = {sdmd, s4x4,        1, 0, 1,       0,          0,          &d4x4l1, &d4x4l4, &d4x4d1, &d4x4d4, 0,       0,          {0, 7, 8, 15}};
static Const fixer d4x4d2    = {sdmd, s4x4,        0, 0, 1,       0,          0,          &d4x4l2, &d4x4l3, &d4x4d2, &d4x4d3, 0,       0,          {8, 15, 0, 7}};
static Const fixer d4x4d3    = {sdmd, s4x4,        1, 0, 1,       0,          0,          &d4x4l3, &d4x4l2, &d4x4d3, &d4x4d2, 0,       0,          {12, 3, 4, 11}};
static Const fixer d4x4d4    = {sdmd, s4x4,        0, 0, 1,       0,          0,          &d4x4l4, &d4x4l1, &d4x4d4, &d4x4d1, 0,       0,          {12, 3, 4, 11}};

static Const fixer fcpl12    = {s2x2, s4x4,     0x14, 0, 1,       0,          0,          0,          0, 0,          0,    &fcpl12,    0,          {1, 2, 5, 6}};
static Const fixer fcpl23    = {s2x2, s4x4,    0x3C0, 0, 1,       0,          0,          0,          0, 0,          0,    &fcpl23,    0,          {13, 14, 1, 2}};
static Const fixer fcpl34    = {s2x2, s4x4,    0x140, 0, 1,       0,          0,          0,          0, 0,          0,    &fcpl34,    0,          {13, 14, 9, 10}};
static Const fixer fcpl41    = {s2x2, s4x4,     0x3C, 0, 1,       0,          0,          0,          0, 0,          0,    &fcpl41,    0,          {9, 10, 5, 6}};

static Const fixer foo55d    = {s1x4, s1x8,        0, 0, 1,       0,          0,          &foo55d,    0, &f1x3zzd,   0,    &bar55d,    &bar55d,    {0, 2, 4, 6}};
static Const fixer fgalctb   = {s2x2, s_galaxy,    0, 0, 1,       0,          0,          0,          0, 0,          0,    &fgalctb,   &fgalctb,   {1, 3, 5, 7}};
static Const fixer f3x1ctl   = {s1x4, s3x1dmd,     0, 0, 1,       0,          0,          &f3x1ctl,   0, 0,          0,    &fgalctb,   &fgalctb,   {1, 2, 5, 6}};
static Const fixer f2x2pl    = {s2x2, s2x2,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x2pl,    &f2x2pl,    {0, 1, 2, 3}};
static Const fixer f1x4pl    = {s1x4, s1x4,        0, 0, 1,       0,          0,          &f1x4pl,    &f1x4pl, 0,    0,    &f2x2pl,    &f2x2pl,    {0, 1, 2, 3}};

static Const fixer f3x1d_2   = {s1x2, s3x1dmd,     1, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {3, 7}};
static Const fixer f1x8_88   = {s1x2, s1x8,        0, 0, 1,       &f1x8_88,   &f3x1d_2,   0,          0, 0,          0,    0,          0,          {3, 7}};
static Const fixer f1x8_22   = {s1x2, s1x8,        0, 0, 1,       &f1x8_22,   &f3x1d_2,   0,          0, 0,          0,    0,          0,          {1, 5}};
static Const fixer f1x8_11   = {s1x2, s1x8,        0, 0, 1,       &f1x8_11,   &f3x1d_2,   0,          0, 0,          0,    0,          0,          {0, 4}};


/* Used for get-out only. */
static Const fixer fdqtagzzz = {s2x3, s_qtag,      1, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0, {1, 3, 4, 5, 7, 0}};


static Const fixer f1x8_77_3 = {s1x2, s1x8,        0, 0, 3,       &f1x8_77_3, 0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 6, 5, 4}};
static Const fixer f1x8_77   = {s1x6, s1x8,        0, 0, 1,       0,          0,          &f1x8_77,   0, 0,          0,    &fdrhgl_bb, 0,          {0, 1, 2, 4, 5, 6}};
static Const fixer fdrhgl_bb = {s2x3, s_dhrglass,  0, 1, 1,       0,          0,          &f1x8_77,   0, 0,          0,    &fdrhgl_bb, &fdqtagzzz, {0, 3, 1, 4, 7, 5}};
static Const fixer f1x8_bb   = {s1x6, s1x8,        0, 0, 1,       0,          0,          &f1x8_bb,   0, 0,          0,    0,          0,          {0, 1, 3, 4, 5, 7}};
static Const fixer f1x8_dd   = {s1x6, s1x8,        0, 0, 1,       0,          0,          &f1x8_dd,   0, 0,          0,    0,          0,          {0, 3, 2, 4, 7, 6}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer foo99d    = {s1x4, s1x8,        0, 0, 1,       0,          0,          &foo99d,    0, 0,          0,    &f2x4endd,  &f2x4endd,  {0, 3, 4, 7}};
static Const fixer foo66d    = {s1x4, s1x8,        0, 0, 1,       0,          0,          &foo66d,    0, &f1x3yyd,   0,    &bar55d,    &bar55d,    {1, 2, 5, 6}};
static Const fixer f1x8ctr   = {s1x4, s1x8,        0, 0, 1,       0,          0,          &f1x8ctr,   0, 0,          0,    &bar55d,    &bar55d,    {3, 2, 7, 6}};
static Const fixer fqtgctr   = {s1x4, s_qtag,      0, 0, 1,       0,          0,          &fqtgctr,   0, 0,          0,    &bar55d,    &bar55d,    {6, 7, 2, 3}};
static Const fixer fxwve     = {s1x4, s_crosswave, 0, 0, 1,       0,          0,          &fxwve,     &f1x8endd, 0,  0,    &f2x4endd,  &f2x4endd,  {0, 1, 4, 5}};
static Const fixer fboneendd = {s2x2, s_bone,      0, 1, 1,       0,          0,          &f1x8endd,  0, 0,          0,    &fboneendd, &fqtgend,   {0, 1, 4, 5}};
static Const fixer fqtgend   = {s2x2, s_qtag,      0, 2, 1,       0,          0,          &f1x8endd,  0, 0,          0,    &fqtgend,   &fboneendd, {0, 1, 4, 5}};
static Const fixer fdrhgl1   = {s2x2, s_dhrglass,  0, 1, 1,       0,          0,          0,          0, 0,          0,    &fdrhgl1,   &fqtgend,   {0, 1, 4, 5}};
static Const fixer f1x8endd  = {s1x4, s1x8,        0, 0, 1,       0,          0,          &f1x8endd,  &fxwve, 0,     0,    &f2x4endd,  &f2x4endd,  {0, 1, 4, 5}};
static Const fixer f1x8endo  = {s1x2, s1x8,        0, 0, 2,       &f1x8endo,  &fboneendo, 0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};

static Const fixer f1x8lowf  = {s1x4, s1x8,        0, 0, 1,       0,          0,          &f1x8lowf,  0, 0,          0,    0,          0,          {0, 1, 2, 3}};
static Const fixer f1x8hif   = {s1x4, s1x8,        0, 0, 1,       0,          0,          &f1x8hif,   0, 0,          0,    0,          0,          {6, 7, 4, 5}};

static Const fixer fbonectr  = {s1x4, s_bone,      0, 0, 1,       0,          0,          &fbonectr,  0, 0,          0,    &bar55d,    &bar55d,    {6, 7, 2, 3}};
static Const fixer fbonetgl  = {s_bone6, s_bone,   0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 3, 4, 5, 7}};
static Const fixer frigtgl   = {s_short6, s_rigger,1, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 2, 4, 5, 6, 0}};
static Const fixer fboneendo = {s1x2, s_bone,      1, 0, 2,       &fboneendo, &f1x8endo,  0,          0, 0,          0,    0,          0,          {0, 5, 1, 4}};
static Const fixer frigendd  = {s1x4, s_rigger,    0, 0, 1,       0,          0,          &frigendd,  0, 0,          0,    &f2x4endd,  &f2x4endd,  {6, 7, 2, 3}};
static Const fixer frigctr   = {s2x2, s_rigger,    0, 0, 1,       0,          0,          &f1x8ctr,   0, 0,          0,    &frigctr,   &frigctr,   {0, 1, 4, 5}};
static Const fixer f2x4ctr   = {s2x2, s2x4,        0, 0, 1,       0,          0,          &fbonectr,  0, 0,          0,    &f2x4ctr,   &f2x4ctr,   {1, 2, 5, 6}};
/* These 26 are unsymmetrical. */
       Const fixer f2x4far   = {s1x4, s2x4,        0, 0, 1,       0,          0,          &f2x4far,   0, 0,          0,    0,          0,          {0, 1, 3, 2}};
       Const fixer f2x4near  = {s1x4, s2x4,        0, 0, 1,       0,          0,          &f2x4near,  0, 0,          0,    0,          0,          {7, 6, 4, 5}};

       Const fixer f4dmdiden = {s4dmd, s4dmd,      0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};


static Const fixer f2x4pos1  = {s1x2, s2x4,        1, 0, 1,       &f2x4pos1,  0,         0,           0, 0,          0,    0,          0,          {0, 7}};
static Const fixer f2x4pos2  = {s1x2, s2x4,        1, 0, 1,       &f2x4pos2,  0,         0,           0, 0,          0,    0,          0,          {1, 6}};
static Const fixer f2x4pos3  = {s1x2, s2x4,        1, 0, 1,       &f2x4pos3,  0,         0,           0, 0,          0,    0,          0,          {2, 5}};
static Const fixer f2x4pos4  = {s1x2, s2x4,        1, 0, 1,       &f2x4pos4,  0,         0,           0, 0,          0,    0,          0,          {3, 4}};
static Const fixer f2x4pos5  = {s1x2, s2x4,        1, 0, 3,       &f2x4pos5,  0,         0,           0, 0,          0,    0,          0,          {1, 6, 2, 5, 3, 4}};
static Const fixer f2x4pos6  = {s1x2, s2x4,        1, 0, 3,       &f2x4pos6,  0,         0,           0, 0,          0,    0,          0,          {0, 7, 2, 5, 3, 4}};
static Const fixer f2x4pos7  = {s1x2, s2x4,        1, 0, 3,       &f2x4pos7,  0,         0,           0, 0,          0,    0,          0,          {0, 7, 1, 6, 3, 4}};
static Const fixer f2x4pos8  = {s1x2, s2x4,        1, 0, 3,       &f2x4pos8,  0,         0,           0, 0,          0,    0,          0,          {0, 7, 1, 6, 2, 5}};
static Const fixer f2x4posa  = {s1x2, s2x4,        0, 0, 1,       &f2x4posa,  0,         0,           0, 0,          0,    0,          0,          {0, 1}};
static Const fixer f2x4posb  = {s1x2, s2x4,        0, 0, 1,       &f2x4posb,  0,         0,           0, 0,          0,    0,          0,          {1, 2}};
static Const fixer f2x4posc  = {s1x2, s2x4,        0, 0, 1,       &f2x4posc,  0,         0,           0, 0,          0,    0,          0,          {2, 3}};
static Const fixer f2x4posd  = {s1x2, s2x4,        0, 0, 1,       &f2x4posd,  0,         0,           0, 0,          0,    0,          0,          {5, 4}};
static Const fixer f2x4pose  = {s1x2, s2x4,        0, 0, 1,       &f2x4pose,  0,         0,           0, 0,          0,    0,          0,          {6, 5}};
static Const fixer f2x4posf  = {s1x2, s2x4,        0, 0, 1,       &f2x4posf,  0,         0,           0, 0,          0,    0,          0,          {7, 6}};
static Const fixer f2x4posp  = {s1x2, s2x4,        0, 0, 2,       &f2x4posp,  0,         0,           0, 0,          0,    0,          0,          {0, 1, 6, 5}};
static Const fixer f2x4posq  = {s1x2, s2x4,        0, 0, 2,       &f2x4posq,  0,         0,           0, 0,          0,    0,          0,          {2, 3, 6, 5}};
static Const fixer f2x4posr  = {s1x2, s2x4,        0, 0, 2,       &f2x4posr,  0,         0,           0, 0,          0,    0,          0,          {1, 2, 7, 6}};
static Const fixer f2x4poss  = {s1x2, s2x4,        0, 0, 2,       &f2x4poss,  0,         0,           0, 0,          0,    0,          0,          {1, 2, 5, 4}};
static Const fixer f2x4posy  = {s1x2, s2x4,        0, 0, 3,       &f2x4posy,  0,         0,           0, 0,          0,    0,          0,          {0, 1, 2, 3, 6, 5}};
static Const fixer f2x4posz  = {s1x2, s2x4,        0, 0, 3,       &f2x4posz,  0,         0,           0, 0,          0,    0,          0,          {1, 2, 7, 6, 5, 4}};

static Const fixer f2x4left  = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4left,  &f2x4left,  {0, 1, 6, 7}};
static Const fixer f2x4right = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4right, &f2x4right, {2, 3, 4, 5}};
static Const fixer f2x4dleft = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4dleft, &f2x4dleft, {0, 2, 5, 7}};
static Const fixer f2x4dright= {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4dright,&f2x4dright,{1, 3, 4, 6}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f2zzrdsc =  {s2x4, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 3, 4, 5, 6, 7}};
static Const fixer f2yyrdsc =  {s1x8, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 3, 4, 5, 6, 7}};


static Const fixer f2x6ndsc =  {s2x4, s2x6,        0, 0, 1,       0,          0,          0,          &f2yyrdsc, 0,  0,    0,          &f2zzrdsc,  {0, 2, 3, 5, 6, 8, 9, 11}};
static Const fixer f1x8nd96 =  {s1x4, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 2, 4, 7}};
static Const fixer f1x8nd69 =  {s1x4, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 3, 5, 6}};
static Const fixer f1x8nd41 =  {s1x2, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 6}};
static Const fixer f1x8nd82 =  {s1x2, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 7}};
static Const fixer f1x8nd28 =  {s1x2, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {3, 5}};
static Const fixer f1x8nd14 =  {s1x2, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {2, 4}};
static Const fixer f1x10ndsc =  {s1x8, s1x10,      0, 0, 1,       0,          0,          0,          &f2yyrdsc, 0,  0,    &f2zzrdsc,  &f2zzrdsc,  {0, 1, 4, 3, 5, 6, 9, 8}};
static Const fixer f1x10ndsd =  {s1x8, s1x10,      0, 0, 1,       0,          0,          0,          &f2yyrdsc, 0,  0,    &f2zzrdsc,  &f2zzrdsc,  {0, 2, 4, 3, 5, 7, 9, 8}};
static Const fixer f1x10ndse =  {s1x8, s1x10,      0, 0, 1,       0,          0,          0,          &f2yyrdsc, 0,  0,    &f2x6ndsc,  &f2zzrdsc,  {0, 1, 4, 2, 5, 6, 9, 7}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f2x4endd  = {s2x2, s2x4,        0, 1, 1,       0,          0,          &frigendd,  &frigendd, 0,  0,    &f2x4endd,  &fqtgend,   {0, 3, 4, 7}};
static Const fixer f2x477    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    &f2x477,    0,          {0, 1, 2, 4, 5, 6}};
static Const fixer f2x4ee    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    &f2x4ee,    0,          {1, 2, 3, 5, 6, 7}};
static Const fixer f2x4bb    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    &f2x4bb,    0,          {0, 1, 3, 4, 5, 7}};
static Const fixer f2x4dd    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    &f2x4dd,    0,          {0, 2, 3, 4, 6, 7}};
static Const fixer fdhrgld1  = {s1x2, s_dhrglass,  0, 1, 2,       &fdhrgld1,  0,          0,          0, 0,          0,    0,          0,          {0, 3, 7, 4}};
static Const fixer fdhrgld2  = {s1x2, s_dhrglass,  0, 1, 2,       &fdhrgld2,  0,          0,          0, 0,          0,    0,          0,          {3, 1, 5, 7}};
static Const fixer f2x4endo  = {s1x2, s2x4,        1, 0, 2,       &f2x4endo,  &f1x8endo,  0,          0, 0,          0,    0,          0,          {0, 7, 3, 4}};
static Const fixer bar55d    = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 2, 5, 6}};
static Const fixer fppaad    = {s1x2, s2x4,        0, 0, 2,       &fppaad,    0,          0,          0, 0,          0,    0,          0,          {1, 3, 7, 5}};
static Const fixer fpp55d    = {s1x2, s2x4,        0, 0, 2,       &fpp55d,    0,          0,          0, 0,          0,    0,          0,          {0, 2, 6, 4}};


sel_item sel_init_table[] = {
   {LOOKUP_DIST_DMD,           s_rigger,    0x99,   &distrig3,   (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_rigger,    0x66,   &distrig4,   (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_rigger,    0x55,   &distrig7,   (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_rigger,    0xAA,   &distrig8,   (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_galaxy,    0x66,   &dgald1,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_galaxy,    0xCC,   &dgald2,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_galaxy,    0x33,   &dgald3,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_galaxy,    0x99,   &dgald4,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s2x4,        0xAA,   &d2x4d1,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s2x4,        0x55,   &d2x4d2,     (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s4x4,        0x8181, &d4x4d1,     &d4x4d2,     0},
   {LOOKUP_DIST_DMD,           s4x4,        0x1818, &d4x4d3,     &d4x4d4,     4},
   {LOOKUP_DIST_DMD,           s_qtag,      0x66,   &ddmd1,      (fixer *) 0, -1},
   {LOOKUP_DIST_DMD,           s_qtag,      0x55,   &ddmd2,      (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s2x4,        0xAA,   &d2x4c1,     (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s2x4,        0x55,   &d2x4c2,     (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s2x4,        0x33,   &d2x4y1,     (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s2x4,        0xCC,   &d2x4y2,     (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s_qtag,      0xAA,   &fqtgjj1,    (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s_qtag,      0x99,   &fqtgjj2,    (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s_spindle,   0x66,   &fspindlfd,  (fixer *) 0, -1},
   {LOOKUP_DIST_BOX,           s_spindle,   0x33,   &fspindlgd,  (fixer *) 0, -1},
   {LOOKUP_DIAG_CLW|LOOKUP_DIST_CLW, s4x4,  0x0909, &d4x4l1,     &d4x4l2,     0},
   {LOOKUP_DIAG_CLW|LOOKUP_DIST_CLW, s4x4,  0x9090, &d4x4l3,     &d4x4l4,     4},
   {LOOKUP_DIST_CLW,           s_rigger,    0x99,   &distrig1,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_rigger,    0x66,   &distrig2,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_rigger,    0x55,   &distrig5,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_rigger,    0xAA,   &distrig6,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_bone,      0x55,   &distbone1,  (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_bone,      0x99,   &distbone2,  (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_bone,      0x66,   &distbone5,  (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_bone,      0xAA,   &distbone6,  (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_hrglass,   0xAA,   &disthrg1,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_hrglass,   0x99,   &disthrg2,   (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_galaxy,    0x66,   &dgalw1,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_galaxy,    0xCC,   &dgalw2,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_galaxy,    0x33,   &dgalw3,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s_galaxy,    0x99,   &dgalw4,     (fixer *) 0, -1},
   {LOOKUP_OFFS_CLW|LOOKUP_DIST_CLW, s2x4,  0x33,   &d2x4w1,     (fixer *) 0, -1},
   {LOOKUP_OFFS_CLW|LOOKUP_DIST_CLW, s2x4,  0xCC,   &d2x4w2,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s2x4,        0xAA,   &d2x4x1,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s2x4,        0x55,   &d2x4x2,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x6,         033,   &f1x6aad,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0xAA,   &f1x8aad,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x55,   &foo55d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x99,   &foo99d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x66,   &foo66d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x33,   &f1x8endd,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x88,   &f1x8_88,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x22,   &f1x8_22,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x11,   &f1x8_11,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0x77,   &f1x8_77,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0xBB,   &f1x8_bb,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x8,        0xDD,   &f1x8_dd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s3x4,       03131,   &f3x4outrd,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s3dmd,      00707,   &f3dmoutrd,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_dhrglass,  0x77,   &fdhrgld,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x12,      00707,   &f1x12outrd, (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_323,       0x77,   &f323d,      (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_bone,      0x33,   &fboneendd,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_bone,      0xBB,   &fbonetgl,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_rigger,    0x77,   &frigtgl,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_qtag,      0x33,   &fqtgend,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_bone6,      033,   &fo6zzd,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_ptpd,      0xAA,   &foozzd,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_ptpd,      0x55,   &fptpzzd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_dhrglass,  0x33,   &fdrhgl1,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s3x1dmd,     0x99,   &f3x1zzd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s3x1dmd,     0xAA,   &f3x1yyd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_2x1dmd,     055,   &f2x1yyd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x3dmd,     0x99,   &f1x3zzd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x3dmd,     0xAA,   &f1x3yyd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s1x3dmd,     0x66,   &f1x3bbd,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_crosswave, 0x55,   &fxwv1d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_crosswave, 0x99,   &fxwv2d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_crosswave, 0x66,   &fxwv3d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_crosswave, 0x33,   &fxwve,      (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_spindle,   0x55,   &fspindld,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_spindle,   0xAA,   &fspindlbd,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_spindle,   0xDD,   &fspindlod,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_dhrglass,  0xBB,   &fdrhgl_bb,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0xAA,   &fppaad,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0x55,   &fpp55d,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0xA5,   &f2x4dleft,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0x5A,   &f2x4dright, (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0x99,   &f2x4endd,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_dhrglass,  0x99,   &fdhrgld1,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s_dhrglass,  0xAA,   &fdhrgld2,   (fixer *) 0, -1},
   {LOOKUP_IGNORE,             s2x4,        0x77,   &f2x477,     (fixer *) 0, -1},  /* 2x4 with 2 corners ignored.  These 2 are *NOT* disconnected. */
   {LOOKUP_IGNORE,             s2x4,        0xEE,   &f2x4ee,     (fixer *) 0, -1},  /* 2x4 with 2 centers ignored.  These 2 *ARE* disconnected (or ignored). */
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0xBB,   &f2x4bb,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE, s2x4,        0xDD,   &f2x4dd,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s1x8,     0xCC, &f1x8ctr,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s_qtag,   0xCC, &fqtgctr,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s_bone,   0xCC, &fbonectr,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s_rigger, 0x33, &frigctr,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s3x1dmd,  0x66, &f3x1ctl,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s1x4,     0x0F, &f1x4pl,    (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x66, &f2x4ctr,   (fixer *) 0, -1},

   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x0F, &f2x4far,   (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xF0, &f2x4near,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x81, &f2x4pos1,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x42, &f2x4pos2,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x24, &f2x4pos3,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x18, &f2x4pos4,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x7E, &f2x4pos5,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xBD, &f2x4pos6,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xDB, &f2x4pos7,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xE7, &f2x4pos8,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x03, &f2x4posa,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x06, &f2x4posb,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x0C, &f2x4posc,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x30, &f2x4posd,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x60, &f2x4pose,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xC0, &f2x4posf,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x63, &f2x4posp,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x6C, &f2x4posq,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xC6, &f2x4posr,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x36, &f2x4poss,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x6F, &f2x4posy,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xF6, &f2x4posz,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xC3, &f2x4left,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x3C, &f2x4right, (fixer *) 0, -1},

   {LOOKUP_DISC,                           s2x6,    05555, &f2x6ndsc,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x96, &f1x8nd96,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x69, &f1x8nd69,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x41, &f1x8nd41,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x82, &f1x8nd82,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x28, &f1x8nd28,  (fixer *) 0, -1},
   {LOOKUP_DISC,                           s1x8,     0x14, &f1x8nd14,  (fixer *) 0, -1},
   {LOOKUP_DISC,                          s1x10,    0x37B, &f1x10ndsc, (fixer *) 0, -1},
   {LOOKUP_DISC,                          s1x10,    0x3BD, &f1x10ndsd, (fixer *) 0, -1},
   {LOOKUP_DISC,                          s1x10,    0x2F7, &f1x10ndse, (fixer *) 0, -1},

   {LOOKUP_NONE,               s4x4,        0x0066, &fcpl12,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x6006, &fcpl23,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x6600, &fcpl34,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x0660, &fcpl41,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_thar,      0x00CC, &ftharns,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s_thar,      0x0033, &ftharew,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x33,   &foo33,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x5A,   &foo5a,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0xA5,   &fooa5,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x55,   &foo55,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0xAA,   &fooaa,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x88,   &foo88,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x44,   &foo44,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x22,   &foo22,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x11,   &foo11,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x4,        0x3,    &n1x43,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x4,        0xC,    &n1x4c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x4,        0x5,    &n1x45,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x4,        0xA,    &n1x4a,      (fixer *) 0, -1},

   {LOOKUP_NONE,               s2x4,        0xCC,   &foocc,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x99,   &f2x4endo,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x6,         033,   &f1x6aa,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x8,        0xAA,   &f1x8aa,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s3x4,        03131,  &f3x4outer,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s3dmd,       00707,  &f3dmouter,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s_dhrglass,   0x77,  &fdhrgl,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_525,       00707,  &f525nw,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_525,       03434,  &f525ne,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_323,        0x77,  &f323,       (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x3dmd,      0x77,  &f1x3d6,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x12,       00707,  &f1x12outer, (fixer *) 0, -1},
   {LOOKUP_NONE,               s3x4,        0x0C3,  &f3x4left,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s3x4,        0x30C,  &f3x4right,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,       0x3030,  &f4x4left,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,       0x4141,  &f4x4right,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x6,        0x0C3,  &f3x4lzz,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x6,        0xC30,  &f3x4rzz,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x8,       0x0303,  &f4x4lzz,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x8,       0xC0C0,  &f4x4rzz,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x8,        0x33,   &f1x8endo,   (fixer *) 0, -1},

   {LOOKUP_NONE,               s1x8,        0x77,   &f1x8_77_3,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x8,        0x0F,   &f1x8lowf,   (fixer *) 0, -1},   /* Unsymmetrical */
   {LOOKUP_NONE,               s1x8,        0xF0,   &f1x8hif,    (fixer *) 0, -1},   /* Unsymmetrical */

   {LOOKUP_NONE,               s_bone,      0x33,   &fboneendo,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s_ptpd,      0xAA,   &foozz,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s_spindle,   0x55,   &fspindlc,   (fixer *) 0, -1},

   /*
   {LOOKUP_NONE,               s_spindle,   0xDD,   &fspindlo,   (fixer *) 0, -1},
   */

   {LOOKUP_NONE,               s_spindle,   0x66,   &fspindlf,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s_spindle,   0x33,   &fspindlg,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s_galaxy,    0x44,   &fgalcv,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_galaxy,    0x11,   &fgalch,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x3dmd,     0x66,   &f1x3aad,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s_1x2dmd,    033,    &f1x2aad,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s3ptpd,      06262,  &f3ptpo6,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,        055,    &f2x3c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x3,    &box3c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x6,    &box6c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x9,    &box9c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0xC,    &boxcc,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x5,    &box55,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0xA,    &boxaa,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s_qtag,      0xAA,   &fqtgj1,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_qtag,      0x99,   &fqtgj2,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,         033,   &f2x3j1,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,         066,   &f2x3j2,     (fixer *) 0, -1},
   {LOOKUP_NONE,               nothing}};


/* BEWARE!!  This list is keyed to the definition of "setup_kind" in database.h . */
setup_attr setup_attrs[] = {
   /* nothing */
   {-1,                           /* setup_limits */
    (coordrec *) 0,               /* setup_coords */
    (coordrec *) 0,               /* nice_setup_coords */
    0, 0, 0, 0,                   /* concentric masks:
                                     mask_normal,
                                     mask_6_2,
                                     mask_2_6, and
                                     mask_ctr_dmd */
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
     0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0x5, 0, 0, 0,
      {b_1x4,       b_4x1},
      { 4, 1},
      FALSE,
      id_bit_table_1x4,
      {  "a  b  d  c@",
         "a@b@d@c@"}},
   /* sdmd */
      { 3,
      &thingdmd,
      &nicethingdmd,
      0x5, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
      {b_trngl4,    b_ptrngl4},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_bone6 */
      { 5,
      &thingbone6,
      &thingbone6,
      0, 0, 0, 0,
      {b_bone6,     b_pbone6},
      { 0, 0},
      FALSE,
      id_bit_table_bone6,
      {  "a        b@    fc@e        d@",
         "ea@  f@  c@db@"}},
   /* s_short6 */
      { 5,
      &thingshort6,
      &thingshort6,
      0, 0, 0, 0,
      {b_short6,    b_pshort6},
      { 0, 0},
      FALSE,
      id_bit_table_short6,
      {  "   b@a  c@f  d@   e@",
         "   fa@e      b@   dc@"}},
   /* s1x6 */
      { 5,
      &thing1x6,
      &thing1x6,
      0, 0, 0, 0,
      {b_1x6,       b_6x1},
      { 6, 1},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  f  e  d@",
         "a@b@c@f@e@d@"}},
   /* s2x3 */
      { 5,
      &thing2x3,
      &thing2x3,
      0, 0, 0, 0,
      {b_2x3,       b_3x2},
      { 3, 2},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c@f  e  d@",
         "f  a@e  b@d  c@"}},
   /* s_1x2dmd */
      { 5,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0x33, 0xDD, 0x11, 0,
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
      0x33, 0, 0x11, 0,
      {b_bone,      b_pbone},
      { 0, 0},
      FALSE,
      id_bit_table_bone,
      {  "a                   b@    g h d c@f                   e",
         "fa@  g@  h@  d@  c@eb"}},
   /* s1x8 */
      { 7,
      &thing1x8,
      &thing1x8,
      0x33, 0x77, 0x22, 0,
      {b_1x8,       b_8x1},
      { 8, 1},
      FALSE,
      id_bit_table_1x8,
      {  "a b d c g h f e",
         "a@b@d@c@g@h@f@e"}},
   /* slittlestars */
      {7,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_2stars */
      {7,
      (coordrec *) 0,
      (coordrec *) 0,
       /*
         0x33, 0xDD, 0x11, 0,
       */
      0, 0, 0, 0,
      {b_2stars,   b_p2stars},
      { 0, 0},
      FALSE,
      id_bit_table_2stars,
      {  "   a     b@g hd c@   f     e",
         "   g@f a@   h@@   d@e b@   c"}},
   /* s1x3dmd */
      { 7,
      &thing1x3dmd,
      &thing1x3dmd,
      0x33, 0x77, 0x11, 0,
      {b_1x3dmd,    b_p1x3dmd},
      { 0, 0},
      FALSE,
      id_bit_table_1x3dmd,
      {  "               d@a b c      g f e@               h",
         "   a@@   b@@   c@@ hd@@   g@@   f@@   e@"}},
   /* s3x1dmd */
      { 7,
      &thing3x1dmd,
      &thing3x1dmd,
      0x33, 0xEE, 0, 0,
      {b_3x1dmd,    b_p3x1dmd},
      { 0, 0},
      FALSE,
      id_bit_table_3x1dmd,
      {  "             d@@a b c g f e@@             h",
         "      a@@      b@@      c@h        d@      g@@      f@@      e"}},
   /* s_spindle */
      { 7,
      &thingspindle,
      &thingspindle,
      0, 0xEE, 0x44, 0,
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
      0x33, 0xDD, 0x11, 0,
      {b_hrglass,   b_phrglass},
      { 0, 0},
      FALSE,
      id_bit_table_hrglass,
      {  "   a  b@      d@g        c@      h@   f  e",
         "     g@f      a@   hd@e      b@     c"}},
   /* s_dhrglass */
      { 7,
      &thingdglass,
      &thingdglass,
      0x33, 0, 0x11, 0,
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
      0, 0, 0, 0,
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
      0x33, 0x77, 0x11, 0x55,
      {b_crosswave, b_pcrosswave},
      { 0, 0},
      FALSE,
      id_bit_table_crosswave,
      {  "          c@          d@ab        fe@          h@          g",
         "      a@      b@@ghdc@@      f@      e"}},
   /* s2x4 */
      { 7,
      &thing2x4,
      &thing2x4,
      0x66, 0, 0, 0,
      {b_2x4,       b_4x2},
      { 4, 2},
      FALSE,
      id_bit_table_2x4,
      {  "a  b  c  d@@h  g  f  e",
         "h  a@@g  b@@f  c@@e  d"}},
   /* s2x5 */
      {9,
      &thing2x5,
      &thing2x5,
      0, 0, 0, 0,
      {b_2x5,       b_5x2},
      { 5, 2},
      FALSE,
      id_bit_table_2x5,
      {  "a  b  c  d  e@@j  i  h  g  f",
         "j  a@@i  b@@h  c@@g  d@@f  e"}},
   /* s_rigger */
      { 7,
      &thingrigger,
      &thingrigger,
      0xCC, 0xDD, 0, 0,
      {b_rigger,    b_prigger},
      { 0, 0},
      FALSE,
      id_bit_table_rigger,
      {  "        a b@gh         dc@        f e",
         "  g@  h@fa@eb@  d@  c"}},
   /* s3x4 */
      {11,
      &thing3x4,
      &thing3x4,
      0, 0, 0x041, 0,         /* Only used if occupied as "H" */
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
      0, 0, 0, 0,
      {b_2x6,       b_6x2},
      { 6, 2},
      FALSE,
      id_bit_table_2x6,
      {  "a  b  c  d  e  f@@l  k  j  i  h  g",
         "l  a@@k  b@@j  c@@i  d@@h  e@@g  f"}},
   /* d3x4 */
      {11,
      &thing_d3x4,
      &thing_d3x4,
      0, 0, 0, 0,
      {b_d3x4,   b_d4x3},
      { 5, 3},
      FALSE,
      id_bit_table_d3x4,
      {  "a              e@   b  c  d@l              f@   j  i  h@k              g",
         "k  l  a@@   j  b@@   i  c@@   h  d@@g  f  e"}},
   /* s1p5x8 */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0x1111, 0, 0, 0,        /* Only used if occupied as butterfly */
      {b_4x4,       b_4x4},
      { 4, 4},
      TRUE,
      id_bit_table_4x4,
      {  "m  n  o  a@@k  p  d  b@@j  l  h  c@@i  g  f  e",
         (Cstring) 0}},
   /* s1x10 */
      { 9,
      &thing1x10,
      &thing1x10,
      0, 0, 0, 0,
      {b_1x10,      b_10x1},
      {10, 1},
      FALSE,
      id_bit_table_1x10,
      {  "a b c d e j i h g f",
         "a@b@c@d@e@j@i@h@g@f"}},
   /* s1x12 */
      {11,
      &thing1x12,
      &thing1x12,
      0, 0, 0x041, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
      {b_c1phan,    b_c1phan},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "   b        e@a  c  h  f@   d        g@@   o        l@n  p  k  i@   m        j",
         (Cstring) 0}},
   /* s_hyperbone */
      {15,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_bigblob */
      {23,
      &thingblob,
      &thingblob,
      0, 0, 0, 0,
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
      0, 0x77, 0x22, 0,
      {b_ptpd,      b_pptpd},
      { 0, 0},
      FALSE,
      id_bit_table_ptpd,
      {  "    b           h@a    c   g    e@    d           f",
         "  a@@db@@  c@@  g@@fh@@  e"}},
   /* s3dmd */
      {11,
      &thing3dmd,
      &thing3dmd,
      0, 0, 0x041, 0,        /* Only used if occupied as center 1x2 and outer 1x3's */
      {b_3dmd,      b_p3dmd},
      { 0, 0},
      FALSE,
      id_bit_table_3dmd,
      {  "   a      b      c@@j k l f e d@@   i      h      g",
         "      j@i        a@      k@@      l@h        b@      f@@      e@g        c@      d"}},
   /* s4dmd */
      {15,
      &thing4dmd,
      &thing4dmd,
      0, 0, 0, 0,
      {b_4dmd,      b_p4dmd},
      { 0, 0},
      FALSE,
      id_bit_table_4dmd,
      {  "   a      b      c      d@@m n o p h g f e@@   l      k      j      i",
         "      m@l        a@      n@@      o@k        b@      p@@      h@j        c@      g@@      f@i        d@      e"}},
   /* s3ptpd */
      { 11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_3ptpd,      b_p3ptpd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "    a           b           c@j    k   l    f   e    d@    i           h           g",
         "  j@@ia@@  k@@  l@@hb@@  f@@  e@@gc@@  d"}},
   /* s4ptpd */
      { 15,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_4ptpd,      b_p4ptpd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "    a           b           c           d@m    n   o    p   h    g   f    e@    l           k           j           i",
         "  m@@la@@  n@@  o@@kb@@  p@@  h@@jc@@  g@@  f@@id@@  e"}},
   /* s_wingedstar */
      { 7,
      (coordrec *) 0,
      (coordrec *) 0,
      0x33, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "             d       h       m@a b c  f g  o n  k j i@             e       p       l",
         "   a@@   b@@   c@e  d@   f@   g@p  h@   o@   n@l  m@   k@@   j@@   i"}},
   /* s_barredstar */
      { 9,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  " a          c@        b@i j  e d@        g@ h          f",
         "h  i  a@@      j@   g  b@      e@@f  d  c"}},
   /* s_barredstar12 */
      {13,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  " a                     d@        b       c@l m  n g  f e@        j       i@ k                     h",
         "k  l  a@      m@   j  b@      n@@      g@   i  c@      f@@h  e  d"}},
   /* s_barredstar16 */
      {17,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  " a                                e@        b       c       d@o p  q r  i h  g f@        m       l       k@ n                                j",
         "n  o  a@      p@   m  b@      q@      r@   l  c@      i@@      h@   k  d@      g@@j  f  e"}},
   /* s_galaxy */
      { 7,
      &thinggal,
      &thinggal,
      0x55, 0, 0, 0,
      {b_galaxy,    b_galaxy},
      { 0, 0},
      TRUE,
      id_bit_table_gal,
      {  "     c@   bd@a      e@   hf@     g",
         (Cstring) 0}},
   /* s3x6 */
      {17,
      &thing3x6,
      &thing3x6,
      0, 0, 0, 0,
      {b_3x6,       b_6x3},
      { 6, 3},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f@@p  q  r  i  h  g@@o  n  m  l  k  j",
         "o  p  a@@n  q  b@@m  r  c@@l  i  d@@k  h  e@@j  g  f"}},
   /* s3x8 */
      {23,
      &thing3x8,
      &thing3x8,
      0, 0, 0, 0,
      {b_3x8,       b_8x3},
      { 8, 3},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h@@u  v  w  x  l  k  j  i@@t  s  r  q  p  o  n  m",
         "t  u  a@@s  v  b@@r  w  c@@q  x  d@@p  l  e@@o  k  f@@n  j  g@@m  i  h"}},
   /* s4x5 */
      {19,
      &thing4x5,
      &thing4x5,
      0, 0, 0, 0,
      {b_4x5,       b_5x4},
      { 5, 4},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e@@j  i  h  g  f@@p  q  r  s  t@@o  n  m  l  k",
         "o  p  j  a@@n  q  i  b@@m  r  h  c@@l  s  g  d@@k  t  f  e"}},
   /* s4x6 */
      {23,
      &thing4x6,
      &thing4x6,
      0, 0, 0, 0,
      {b_4x6,       b_6x4},
      { 6, 4},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f@@l  k  j  i  h  g@@s  t  u  v  w  x@@r  q  p  o  n  m",
         "r  s  l  a@@q  t  k  b@@p  u  j  c@@o  v  i  d@@n  w  h  e@@m  x  g  f"}},
   /* s2x10 */
      {19,
/*
      &thing2x10,
      &thing2x10,
*/
      (coordrec *) 0,
      (coordrec *) 0,


      0, 0, 0, 0,
      {b_2x10,      b_10x2},
      { 10, 2},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h  i  j@@t  s  r  q  p  o  n  m  l  k",
         "t  a@@s  b@@r  c@@q  d@@p  e@@o  f@@n  g@@m  h@@l  i@@k  j"}},
   /* s2x12 */
      {23,
/*
      &thing2x12,
      &thing2x12,
*/
      (coordrec *) 0,
      (coordrec *) 0,


      0, 0, 0, 0,
      {b_2x12,      b_12x2},
      { 12, 2},
      FALSE,
      (id_bit_table *) 0,
      {  "a  b  c  d  e  f  g  h  i  j  k  l@@x  w  v  u  t  s  r  q  p  o  n  m",
         "x  a@@w  b@@v  c@@u  d@@t  e@@s  f@@r  g@@q  h@@p  i@@o  j@@n  k@@m  l"}},
   /* sdeepqtg */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_deepqtg,      b_pdeepqtg},
      { 4, 4},
      FALSE,
      (id_bit_table *) 0,
      {  "   a        b@f  e  d  c@@i  j  k  l@   h        g",
         "    i  f@h          a@    j  e@@    k  d@g          b@    l  c"}},
   /* s3oqtg */
      {19,
      &thing3oqtg,
      &thing3oqtg,
      0, 0, 0, 0,
      {b_3oqtg,     b_p3oqtg},
      { 7, 4},
      FALSE,
      (id_bit_table *) 0,
      {  "      a        b        c@@r  s  t  g  f  e  d@@n  o  p  q  j  i  h@@      m        l        k",
         "      n  r@@m  o  s  a@@      p  t@@l  q  b  g@@      j  f@@k  i  e  c@@      h  d"}},
   /* s_thar */
      { 7,
      &thingthar,
      &thingthar,
      0x55, 0, 0, 0,
      {b_thar,      b_thar},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "      c@      d@abfe@      h@      g",
         (Cstring) 0}},
   /* s_alamo */
      { 7,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_alamo,     b_alamo},
      { 0, 0},
      TRUE,
      (id_bit_table *) 0,
      {  "   ab@h   c@g   d@   fe",
         (Cstring) 0}},
   /* sx4dmd */
      {31,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
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
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* sx1x16 */
      {31,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
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
      0, 0, 0, 0,
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
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 8, 4},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_323 */
      { 7,
      &thing_323,
      &thing_323,
      0, 0, 0x11, 0,
      {b_323,      b_p323},
      { 0, 0},
      FALSE,
      id_bit_table_323,
      {  "   a  b  c@@      h  d@@   g  f  e",
         "g    a@    h@f    b@    d@e    c"}},
   /* s_343 */
      { 9,
      &thing_343,
      &thing_343,
      0, 0, 0, 0,
      {b_343,      b_p343},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "      a  b  c@@   i  j  e  d@@      h  g  f",
         "    i@h    a@    j@g    b@    e@f    c@    d"}},
   /* s_525 */
      { 11,
      &thing_525,
      &thing_525,
      0, 0, 0, 0,
      {b_525,      b_p525},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a  b  c  d  e@@            l  f@@   k  j  i  h  g",
         "k    a@@j    b@    l@i    c@    f@h    d@@g    e"}},
   /* s_545 */
      { 13,
      &thing_545,
      &thing_545,
      0, 0, 0, 0,
      {b_545,      b_p545},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a  b  c  d  e@@      m  n  g  f@@   l  k  j  i  h",
         "l    a@    m@k    b@    n@j    c@    g@i    d@    f@h    e"}},
   /* sh545 */
      { 13,
      &thing_h545,
      &thing_h545,
      0, 0, 0, 0,
      {bh545,      bhp545},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a  b  c  d  e@@   m     n  g     f@@   l  k  j  i  h",
         "lma@@k    b@    n@j    c@    g@i    d@@hfe"}},
   /* s_3mdmd */
      { 11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_3mdmd,      b_p3mdmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a                     c@               b@j k l      f e d@               h@   i                     g",
         "      j@i        a@      k@@      l@@    hb@@      f@@      e@g        c@      d"}},
   /* s_3mptpd */
      { 11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_3mptpd,      b_p3mptpd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "                  b@     a                     c@j      k l f e      d@     i                     g@                  h",
         "      j@@    ia@@      k@@      l@h        b@      f@@      e@@    gc@@      d"}},
   /* s_4mdmd */
      { 15,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_4mdmd,      b_p4mdmd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "   a                                   d@               b           c@m n o      p h      g f e@               k           j@   l                                   i",
         "      m@l        a@      n@@      o@@    kb@@      p@@      h@@    jc@@      g@@      f@i        d@      e"}},
   /* s_4mptpd */
      { 15,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_4mptpd,      b_p4mptpd},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "                  b      c@     a                               d@m      n o p h g f      e@     l                               i@                  k      j",
         "      m@@    la@@      n@@      o@k        b@      p@@      h@j        c@      g@@      f@@    id@@      e"}},
   /* sbigh */
      {11,
      &thingbigh,
      &thingbigh,
      0, 0, 0x041, 0,
      {b_bigh,      b_pbigh},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "a                   j@b                   i@    e f l k@c                   h@d                   g",
         "dcba@      e@      f@      l@      k@ghij"}},
   /* sbigx */
      {11,
      &thingbigx,
      &thingbigx,
      0, 0, 0x041, 0,
      {b_bigx,      b_pbigx},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "                e@                f@abcd    jihg@                l@                k",
         "      a@      b@      c@      d@klfe@      j@      i@      h@      g"}},
   /* sbigrig */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_bigrig,    b_pbigrig},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  "                e f@abcd         jihg@                l k",
         "  a@  b@  c@  d@le@kf@  j@  i@  h@  g"}},
   /* sbighrgl */
      {11,
      (coordrec *) 0,   /* FIX */
      (coordrec *) 0,   /* FIX */
      0, 0, 0x104, 0,
      {b_bighrgl,   b_pbighrgl},
      { 0, 0},
      FALSE,
      id_bit_table_bighrgl,
      {  "          c@a b      e f@        jd@l k      h g@          i",
         "   l  a@@   k  b@      j@i        c@      d@   h  e@@   g  f"}},
   /* sbigdhrgl */
      {11,
      (coordrec *) 0,   /* FIX */
      (coordrec *) 0,   /* FIX */
      0, 0, 0x104, 0,
      {b_bigdhrgl,   b_pbigdhrgl},
      { 0, 0},
      FALSE,
      id_bit_table_bigdhrgl,
      {  "a  b      c      e  f@           j      d@l  k      i      h  g",
         "l  a@@k  b@@   j@@i  c@@   d@@h  e@@g  f"}},
   /* sbigbone */
      {11,
      &thingbigbone,
      &thingbigbone,
      0, 0, 0x104, 0,
      {b_bigbone,   b_pbigbone},
      { 0, 0},
      FALSE,
      id_bit_table_bigbone,
      {  "a  b                   e  f@          c d j i@l  k                   h  g",
         "la@kb@  c@  d@  j@  i@he@gf"}},
   /* sbigdmd */
      {11,
      &thingbigdmd,
      &thingbigdmd,
      0, 0, 0x104, 0,
      {b_bigdmd, b_pbigdmd},
      {0, 0},
      FALSE,
      id_bit_table_bigdmd,
      {  "           c@a b        e f@           d@@           j@l k        h g@           i",
         "   l      a@   k      b@i j d c@   h      e@   g      f"}},
   /* sbigptpd */
      {11,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0x104, 0,
      {b_bigptpd, b_pbigptpd},
      {0, 0},
      FALSE,
      id_bit_table_bigptpd,
      {  "   f       g@   e       h@c  d j  i@   b       k@   a       l",
         "      c@@abef@@      d@@      j@@lkhg@@      i"}},
   /* s_dead_concentric */
      {-1,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}},
   /* s_normal_concentric */
      {-1,
      (coordrec *) 0,
      (coordrec *) 0,
      0, 0, 0, 0,
      {b_nothing,   b_nothing},
      { 0, 0},
      FALSE,
      (id_bit_table *) 0,
      {  (Cstring) 0,
         (Cstring) 0}}};


/* BEWARE!!  This list is keyed to the definition of "begin_kind" in database.h . */
/*   It must also match the similar table in the mkcalls.c . */
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
   8,          /* b_2stars */
   8,          /* b_p2stars */
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
  10,          /* b_2x5 */
  10,          /* b_5x2 */
   6,          /* b_1x6 */
   6,          /* b_6x1 */
   12,         /* b_3x4 */
   12,         /* b_4x3 */
   12,         /* b_2x6 */
   12,         /* b_6x2 */
   12,         /* b_d3x4 */
   12,         /* b_d4x3 */
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
   18,         /* b_3x6 */
   18,         /* b_6x3 */
   24,         /* b_3x8 */
   24,         /* b_8x3 */
   20,         /* b_4x5 */
   20,         /* b_5x4 */
   24,         /* b_4x6 */
   24,         /* b_6x4 */
   20,         /* b_2x10 */
   20,         /* b_10x2 */
   24,         /* b_2x12 */
   24,         /* b_12x2 */
   12,         /* b_deepqtg */
   12,         /* b_pdeepqtg */
   20,         /* b_3oqtg */
   20,         /* b_p3oqtg */
   8,          /* b_thar */
   8,          /* b_alamo */
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
   12,         /* b_3ptpd */
   12,         /* b_p3ptpd */
   16,         /* b_4ptpd */
   16,         /* b_p4ptpd */
   8,          /* b_323 */
   8,          /* b_p323 */
   10,         /* b_343 */
   10,         /* b_p343 */
   12,         /* b_525 */
   12,         /* b_p525 */
   14,         /* b_545 */
   14,         /* b_p545 */
   14,         /* bh545 */
   14,         /* bhp545 */
   12,         /* b_3mdmd */
   12,         /* b_p3mdmd */
   12,         /* b_3mptpd */
   12,         /* b_p3mptpd */
   16,         /* b_4mdmd */
   16,         /* b_p4mdmd */
   16,         /* b_4mptpd */
   16,         /* b_p4mptpd */
   12,         /* b_bigh */
   12,         /* b_pbigh */
   12,         /* b_bigx */
   12,         /* b_pbigx */
   12,         /* b_bigrig */
   12,         /* b_pbigrig */
   12,         /* b_bighrgl */
   12,         /* b_pbighrgl */
   12,         /* b_bigdhrgl */
   12,         /* b_pbigdhrgl */
   12,         /* b_bigbone */
   12,         /* b_pbigbone */
   12,         /* b_bigdmd */
   12,         /* b_pbigdmd */
   12,         /* b_bigptpd */
   12};        /* b_pbigptpd */


/* The following 8 definitions are taken verbatim from sdinit.c . */
#define B1A (0000 | ID1_PERM_NSG|ID1_PERM_NSB|ID1_PERM_NHG|ID1_PERM_HCOR|ID1_PERM_HEAD|ID1_PERM_BOY)
#define G1A (0100 | ID1_PERM_NSG|ID1_PERM_NSB|ID1_PERM_NHB|ID1_PERM_SCOR|ID1_PERM_HEAD|ID1_PERM_GIRL)
#define B2A (0200 | ID1_PERM_NSG|ID1_PERM_NHG|ID1_PERM_NHB|ID1_PERM_SCOR|ID1_PERM_SIDE|ID1_PERM_BOY)
#define G2A (0300 | ID1_PERM_NSB|ID1_PERM_NHG|ID1_PERM_NHB|ID1_PERM_HCOR|ID1_PERM_SIDE|ID1_PERM_GIRL)
#define B3A (0400 | ID1_PERM_NSG|ID1_PERM_NSB|ID1_PERM_NHG|ID1_PERM_HCOR|ID1_PERM_HEAD|ID1_PERM_BOY)
#define G3A (0500 | ID1_PERM_NSG|ID1_PERM_NSB|ID1_PERM_NHB|ID1_PERM_SCOR|ID1_PERM_HEAD|ID1_PERM_GIRL)
#define B4A (0600 | ID1_PERM_NSG|ID1_PERM_NHG|ID1_PERM_NHB|ID1_PERM_SCOR|ID1_PERM_SIDE|ID1_PERM_BOY)
#define G4A (0700 | ID1_PERM_NSB|ID1_PERM_NHG|ID1_PERM_NHB|ID1_PERM_HCOR|ID1_PERM_SIDE|ID1_PERM_GIRL)

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
         {{G2A|d_south,0}, {B2A|d_south,0}, {G1A|d_south,0}, {B1A|d_south,0},
            {G4A|d_north,0}, {B4A|d_north,0}, {G3A|d_north,0}, {B3A|d_north,0}},
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
         {{G3A|d_south,0}, {B3A|d_south,0}, {G2A|d_south,0}, {B2A|d_south,0},
            {G1A|d_north,0}, {B1A|d_north,0}, {G4A|d_north,0}, {B4A|d_north,0}},
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
         {{B4A|d_east,0}, {G3A|d_south,0}, {B3A|d_south,0}, {G2A|d_west,0},
            {B2A|d_west,0}, {G1A|d_north,0}, {B1A|d_north,0}, {G4A|d_east,0}},
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
         {{B3A|d_east,0}, {G2A|d_south,0}, {B2A|d_south,0}, {G1A|d_west,0},
            {B1A|d_west,0}, {G4A|d_north,0}, {B4A|d_north,0}, {G3A|d_east,0}},
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
         {{0,0}, {G2A|d_west,0}, {B2A|d_west,0}, {0,0}, {0,0}, {G1A|d_north,0},
            {B1A|d_north,0}, {0,0}, {0,0}, {G4A|d_east,0}, {B4A|d_east,0}, {0,0},
            {0,0}, {G3A|d_south,0}, {B3A|d_south,0}, {0,0}},
         0                           /* result_flags (imprecise_rotation is off) */
      }
   }
};



/* Nonzero item in map3[1] for arity 1 map means do not reassemble. */
/* For maps with arity 1, a nonzero item in map3[0] means that the "offset goes away" warning should be given in certain cases. */
/* For maps with arity 3, a value of 3 after the last map means that the "overlap goes away" warning should be given in certain cases. */

/*                                                                                                          setups are stacked on top of each other ----------------------|
                                                                                        each setup is rotated clockwise before being concatenated -------------------|    |
                                                                                                                                                                     V    V
                                             map1                             map2                        map3 map4   map_kind     warncode  arity outer   inner    rot  vert */

Private map_thing map_rig_trngl4        = {{6, 7, 0, 5,                       2, 3, 4, 1},                           MPKIND__SPLIT,       0, 2,  s_rigger,s_trngl4, 0x10D, 0};

Private map_thing map_s6_trngl          = {{4, 5, 3,                          1, 2, 0},                              MPKIND__SPLIT,       0, 2,  s_short6, s_trngl, 0x108, 1, MAPCODE(s_trngl,2,MPKIND__SPLIT,1)};


Private map_thing map_phan_trngl4b      = {{12, 14, 0, 2,                     4, 6, 8, 10},                          MPKIND__SPLIT,       0, 2,  s_c1phan,s_trngl4, 0x108, 0, MAPCODE(s_trngl4,2,MPKIND__SPLIT,0)};




        map_thing map_p8_tgl4           = {{0, 2, 7, 5,                       4, 6, 3, 1},                           MPKIND__REMOVED,     0, 2,  s_ptpd, s_trngl4,  0x10D, 0};
        map_thing map_spndle_once_rem   = {{7, 1, 3, 5,                       0, 2, 4, 6, (int) s2x2},               MPKIND__SPEC_ONCEREM,0, 2,  s_spindle, sdmd,   0x000, 0};
        map_thing map_1x3dmd_once_rem   = {{0, 2, 4, 6,                       1, 3, 5, 7, (int) sdmd},               MPKIND__SPEC_ONCEREM,0, 2,  s1x3dmd, s1x4,     0x000, 0};
        map_thing map_lh_zzztgl         = {{11, 7, 2, 4,                      3, 15, 10, 12},                        MPKIND__OFFS_L_HALF, 4, 2,  s4x4,   s_trngl4,  0x00D, 1};
        map_thing map_rh_zzztgl         = {{7, 11, 8, 9,                      15, 3, 0, 1},                          MPKIND__OFFS_R_HALF, 4, 2,  s4x4,   s_trngl4,  0x007, 1};
        map_thing map_2x2v              = {{0, 3,                             1, 2},                                 MPKIND__SPLIT,       0, 2,  s2x2,   s1x2,      0x005, 0};
        map_thing map_2x4_magic         = {{0, 6, 3, 5,                       7, 1, 4, 2},                           MPKIND__NONE,        0, 2,  s2x4,   s1x4,      0x000, 0};
        map_thing map_qtg_magic         = {{0, 2, 5, 3,                       1, 7, 4, 6},                           MPKIND__NONE,        0, 2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_qtg_intlk         = {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__NONE,        0, 2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_qtg_magic_intlk   = {{0, 2, 5, 7,                       1, 3, 4, 6},                           MPKIND__NONE,        0, 2,  s_qtag, sdmd,      0x005, 0};
        map_thing map_ptp_magic         = {{6, 1, 4, 3,                       0, 7, 2, 5},                           MPKIND__NONE,        0, 2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_ptp_intlk         = {{0, 1, 6, 3,                       2, 7, 4, 5},                           MPKIND__NONE,        0, 2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_ptp_magic_intlk   = {{2, 1, 4, 3,                       0, 7, 6, 5},                           MPKIND__NONE,        0, 2,  s_ptpd, sdmd,      0x000, 0};
        map_thing map_2x4_diagonal      = {{2, 3, 6, 7,                       0, 1, 4, 5},                           MPKIND__NONE,        0, 2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_2x4_int_pgram     = {{1, 3, 5, 7,                       0, 2, 4, 6},                           MPKIND__NONE,        0, 2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_2x4_trapezoid     = {{1, 2, 4, 7,                       0, 3, 5, 6},                           MPKIND__NONE,        0, 2,  s2x4,   s2x2,      0x000, 0};
        map_thing map_4x4_ns            = {{12, 13, 0, 14,                    8, 6, 4, 5},                           MPKIND__NONE,        0, 2,  s4x4,   s1x4,      0x000, 1};
        map_thing map_4x4_ew            = {{0, 1, 4, 2,                       12, 10, 8, 9},                         MPKIND__NONE,        0, 2,  s4x4,   s1x4,      0x005, 1};
        map_thing map_stairst           = {{9, 13, 7, 0, 1, 5, 15, 8,         12, 11, 14, 2, 4, 3, 6, 10},           MPKIND__NONE,        0, 2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_ladder            = {{10, 15, 14, 0, 2, 7, 6, 8,        12, 13, 3, 1, 4, 5, 11, 9},            MPKIND__NONE,        0, 2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_offset            = {{9, 11, 14, 0, 1, 3, 6, 8,         12, 13, 7, 2, 4, 5, 15, 10},           MPKIND__NONE,        0, 2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_but_o             = {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__NONE,        0, 2,  s4x4,   s2x4,      0x000, 1};
        map_thing map_4x4v     = {{12, 10, 8, 9,         13, 15, 6, 11,       14, 3, 5, 7,      0, 1, 4, 2},         MPKIND__SPLIT,       0, 4,  s4x4,   s1x4,      0x055, 0};
        map_thing map_blocks   = {{12, 14, 7, 9,         13, 0, 2, 11,        15, 1, 4, 6,      10, 3, 5, 8},        MPKIND__NONE,        0, 4,  s4x4,   s2x2,      0x000, 0};
        map_thing map_trglbox  = {{12, 14, 15, 9,        13, 0, 2, 3,         7, 1, 4, 6,       10, 11, 5, 8},       MPKIND__NONE,        0, 4,  s4x4,   s2x2,      0x000, 0};

        map_thing map_2x3_0134  = {{4, 3,                0, 1},                                                      MPKIND__NONE,        0, 2,  s2x3,   s1x2,      0x000, 1};
        map_thing map_2x3_1245  = {{5, 4,                1, 2},                                                      MPKIND__NONE,        0, 2,  s2x3,   s1x2,      0x000, 1};

Private map_thing map_4x4_1x4  = {{8, 6, 4, 5,           9, 11, 2, 7,         10, 15, 1, 3,     12, 13, 0, 14},      MPKIND__SPLIT,       0, 4,  s4x4,   s1x4,      0x000, 1};
Private map_thing map_1x16_1x4 = {{0, 1, 3, 2,           4, 5, 7, 6,          15, 14, 12, 13,   11, 10, 8, 9},       MPKIND__SPLIT,       0, 4,  s1x16,  s1x4,      0x000, 0};
Private map_thing map_1x12_1x3 = {{0, 1, 2,              3, 4, 5,             11, 10, 9,        8, 7, 6},            MPKIND__SPLIT,       0, 4,  s1x12,  s1x3,      0x000, 0};
Private map_thing map_3x4_1x3  = {{0, 10, 9,             1, 11, 8,            2, 5, 7,          3, 4, 6},            MPKIND__SPLIT,       0, 4,  s3x4,   s1x3,      0x055, 0};
Private map_thing map_2x8_2x2  = {{0, 1, 14, 15,         2, 3, 12, 13,        4, 5, 10, 11,     6, 7, 8, 9},         MPKIND__SPLIT,       0, 4,  s2x8,   s2x2,      0x000, 0};
Private map_thing map_4dmd_dmd          = {{0, 13, 11, 12,     1, 15, 10, 14,     2, 6, 9, 7,     3, 4, 8, 5},       MPKIND__SPLIT,       0, 4,  s4dmd,  sdmd,      0x055, 0};
Private map_thing map_4ptp_dmd          = {{12, 0, 13, 11,     14, 1, 15, 10,     7, 2, 6, 9,     5, 3, 4, 8},       MPKIND__SPLIT,       0, 4,  s4ptpd, sdmd,      0x000, 0};
Private map_thing map_2x6_1x6           = {{11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       0, 2,  s2x6,   s1x6,      0x000, 1};
Private map_thing map_3x6_1x6           = {{14, 13, 12, 9, 10, 11,            15, 16, 17, 6, 7, 8,
                                                                              0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       0, 3,  s3x6,   s1x6,      0x000, 1};
Private map_thing map_4x6_1x6           = {{17, 16, 15, 12, 13, 14,           18, 19, 20, 23, 22, 21,
                                            11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       0, 4,  s4x6,   s1x6,      0x000, 1};
Private map_thing map_2x8_1x8           = {{15, 14, 12, 13, 8, 9, 11, 10,     0, 1, 3, 2, 7, 6, 4, 5},               MPKIND__SPLIT,       0, 2,  s2x8,   s1x8,      0x000, 1};
Private map_thing map_3x8_1x8           = {{19, 18, 16, 17, 12, 13, 15, 14,   20, 21, 23, 22, 8, 9, 11, 10,
        0, 1, 3, 2, 7, 6, 4, 5},               MPKIND__SPLIT,       0, 3,  s3x8,   s1x8,      0x000, 1};
Private map_thing map_1x12_1x6          = {{0, 1, 2, 5, 4, 3,                 11, 10, 9, 6, 7, 8},                   MPKIND__SPLIT,       0, 2,  s1x12,  s1x6,      0x000, 0};
/* Special maps for putting back end-to-end 1x6's, or 1x2 diamonds, resulting from collisions at one end. */
        map_thing map_1x8_1x6           = {{0, 1, 3, -1, -1, 2,               -1, -1, 6, 4, 5, 7},                   MPKIND__NONE,        0, 2,  s1x8,   s1x6,      0x000, 0};
        map_thing map_rig_1x6           = {{6, 7, 0, -1, -1, 5,               -1, -1, 1, 2, 3, 4},                   MPKIND__NONE,        0, 2,  s_rigger,s_1x2dmd, 0x000, 0};
Private map_thing map_1x16_1x8          = {{0, 1, 3, 2, 7, 6, 4, 5,           15, 14, 12, 13, 8, 9, 11, 10},         MPKIND__SPLIT,       0, 2,  s1x16,  s1x8,      0x000, 0};
Private map_thing map_hv_2x4_2          = {{0, 1, 2, 3, 12, 13, 14, 15,       4, 5, 6, 7, 8, 9, 10, 11},             MPKIND__SPLIT,       0, 2,  s2x8,   s2x4,      0x000, 0};
Private map_thing map_3x4_2x3           = {{1, 11, 8, 9, 10, 0,               3, 4, 6, 7, 5, 2},                     MPKIND__SPLIT,       0, 2,  s3x4,   s2x3,      0x005, 0};
        map_thing map_vsplit_f          = {{13, 15, 11, 6, 8, 9, 10, 12,      0, 1, 2, 4, 5, 7, 3, 14},              MPKIND__SPLIT,       0, 2,  s4x4,   s2x4,      0x005, 0};
Private map_thing map_split_f           = {{9, 11, 7, 2, 4, 5, 6, 8,          12, 13, 14, 0, 1, 3, 15, 10},          MPKIND__SPLIT,       0, 2,  s4x4,   s2x4,      0x000, 1};
Private map_thing map_2x4_2x2           = {{0, 1, 6, 7,                       2, 3, 4, 5},                           MPKIND__SPLIT,       0, 2,  s2x4,   s2x2,      0x000, 0};
Private map_thing map_2x3_1x3           = {{5, 4, 3,                          0, 1, 2},                              MPKIND__SPLIT,       0, 2,  s2x3,   s1x3,      0x000, 1};
Private map_thing map_bigd_12d          = {{11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__SPLIT,       0, 2,  sbigdmd,s_1x2dmd,  0x000, 1};
Private map_thing map_bone_12d          = {{-1, -1, 0, 7, 6, 5,               3, 2, 1, -1, -1, 4},                   MPKIND__SPLIT,       0, 2,  s_bone, s_1x2dmd,  0x000, 0};
Private map_thing map_bone_rig          = {{-1, 0, 7, 5, -1, 6,               1, -1, 2, -1, 4, 3},                   MPKIND__SPLIT,       0, 2,  s_rigger,s_bone6,  0x000, 0};
Private map_thing map_2x4_1x4           = {{7, 6, 4, 5,                       0, 1, 3, 2},                           MPKIND__SPLIT,       0, 2,  s2x4,   s1x4,      0x000, 1};
Private map_thing map_qtg_dmd           = {{0, 7, 5, 6,                       1, 2, 4, 3},                           MPKIND__SPLIT,       0, 2,  s_qtag, sdmd,      0x005, 0};
Private map_thing map_ptp_dmd           = {{0, 1, 2, 3,                       6, 7, 4, 5},                           MPKIND__SPLIT,       0, 2,  s_ptpd, sdmd,      0x000, 0};
Private map_thing map_2stars            = {{6, 0, 7, 5,                       3, 1, 2, 4},                           MPKIND__SPLIT,       0, 2,  s_2stars, s_star,  0x000, 0};

Private map_thing map_3x8_3x4           = {{0, 1, 2, 3, 23, 22, 16, 17, 18, 19, 20, 21,
                                                                   4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 11, 10},        MPKIND__SPLIT,       0, 2,  s3x8,   s3x4,      0x000, 0};
Private map_thing map_4x6_3x4           = {{2, 9, 20, 15, 16, 19, 17, 18, 11, 0, 1, 10,
                                                                   5, 6, 23, 12, 13, 22, 14, 21, 8, 3, 4, 7},        MPKIND__SPLIT,       0, 2,  s4x6,   s3x4,      0x005, 0};
Private map_thing map_par_rig           = {{-1, -1, 6, 7, -1, -1, 11, 10,
                                                                   -1, -1, 5, 4, -1, -1, 0, 1},                      MPKIND__SPLIT,       6, 2,  s2x6,   s_rigger,      0x000, 1};
Private map_thing map_4x6_2x6           = {{18, 19, 20, 21, 22, 23, 12, 13, 14, 15, 16, 17,
                                                                   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},            MPKIND__SPLIT,       0, 2,  s4x6,   s2x6,      0x000, 1};
Private map_thing map_3dmd_dmd          = {{0, 10, 8, 9,           1, 5, 7, 11,           2, 3, 6, 4},               MPKIND__SPLIT,       0, 3,  s3dmd,  sdmd,      0x015, 0};
Private map_thing map_3ptp_dmd          = {{9, 0, 10, 8,           11, 1, 5, 7,           4, 2, 3, 6},               MPKIND__SPLIT,       0, 3,  s3ptpd, sdmd,      0x000, 0};

        map_thing map_4x6_2x4  = {{1, 10, 19, 16, 17, 18, 11, 0,   3, 8, 21, 14, 15, 20, 9, 2,
                                                                   5, 6, 23, 12, 13, 22, 7, 4},                      MPKIND__SPLIT,       0, 3,  s4x6,   s2x4,      0x015, 0};

Private map_thing map_2x12_2x4  = {{0, 1, 2, 3, 20, 21, 22, 23,    4, 5, 6, 7, 16, 17, 18, 19,
                                                                   8, 9, 10, 11, 12, 13, 14, 15},                    MPKIND__SPLIT,       0, 3,  s2x12,  s2x4,      0x000, 0};

Private map_thing map_hv_qtg_2          = {{0, 1, 15, 14, 10, 11, 12, 13,     2, 3, 4, 5, 8, 9, 7, 6},               MPKIND__SPLIT,       0, 2,  s4dmd,  s_qtag,    0x000, 0};
Private map_thing map_vv_qtg_2          = {{9, 20, 16, 19, 18, 11, 1, 10,     6, 23, 13, 22, 21, 8, 4, 7},           MPKIND__SPLIT,       0, 2,  s4x6,   s_qtag,    0x005, 0};
Private map_thing map_spin_3x4          = {{1, 11, 8, -1, 9, 10, 0, -1,       3, 4, 6, -1, 7, 5, 2, -1},             MPKIND__SPLIT,       0, 2,  s3x4,   s_spindle, 0x005, 0};
Private map_thing map_hrgl_ptp          = {{-1, -1, 2, 1, -1, -1, 0, 3,       -1, -1, 4, 7, -1, -1, 6, 5},           MPKIND__SPLIT,       0, 2,  s_ptpd, s_hrglass, 0x000, 0};
        map_thing map_ov_hrg_1          = {{-1, -1, 5, 7, -1, -1, 0, 6,       -1, -1, 4, 2, -1, -1, 1, 3},           MPKIND__OVERLAP,     0, 2,  s_qtag, s_hrglass, 0x005, 0};
        map_thing map_ov_gal_1          = {{-1, 0, -1, 1, -1, 6, -1, 7,       -1, 2, -1, 3, -1, 4, -1, 5},           MPKIND__OVERLAP,     0, 2,  s2x4,   s_galaxy,  0x000, 0};


        map_thing map_3o_qtag_1         = {{19, 15, 12, 14, 13, 17, 0, 18,    5, 9, 11, 16, 15, 19, 1, 6,
                                                                              3, 7, 10, 8, 9, 5, 2, 4},              MPKIND__OVERLAP,     0, 3,  s3oqtg, s_qtag,    0x015, 0};


Private map_thing map_1x6_1x3           = {{0, 1, 2,                          5, 4, 3},                              MPKIND__SPLIT,       0, 2,  s1x6,   s1x3,      0x000, 0};
Private map_thing map_1x8_1x4           = {{0, 1, 2, 3,                       6, 7, 4, 5},                           MPKIND__SPLIT,       0, 2,  s1x8,   s1x4,      0x000, 0};
Private map_thing map_1x12_1x4          = {{0, 1, 3, 2,            4, 5, 10, 11,          9, 8, 6, 7},               MPKIND__SPLIT,       0, 3,  s1x12,  s1x4,      0x000, 0};
Private map_thing map_1x2_1x1           = {{0,                                1},                                    MPKIND__SPLIT,       0, 2,  s1x2,   s1x1,      0x000, 0};
Private map_thing map_1x3_1x1           = {{0,                     1,                     2},                        MPKIND__SPLIT,       0, 3,  s1x3,   s1x1,      0x000, 0};
Private map_thing map_1x4_1x2           = {{0, 1,                             3, 2},                                 MPKIND__SPLIT,       0, 2,  s1x4,   s1x2,      0x000, 0};
        map_thing map_tgl4_1            = {{1, 0,                             2, 3},                                 MPKIND__NONE,        0, 2,  s_trngl4,s1x2,     0x001, 1};
        map_thing map_tgl4_2            = {{3, 2,                             1, 0},                                 MPKIND__NONE,        0, 2,  s_trngl4,s1x2,     0x006, 1};
Private map_thing map_3x6_2x3           = {{1, 16, 13, 14, 15, 0,  3, 8, 11, 12, 17, 2,   5, 6, 9, 10, 7, 4},        MPKIND__SPLIT,       0, 3,  s3x6,   s2x3,      0x015, 0};
Private map_thing map_2x6_2x2           = {{0, 1, 10, 11,          2, 3, 8, 9,            4, 5, 6, 7},               MPKIND__SPLIT,       0, 3,  s2x6,   s2x2,      0x000, 0};
Private map_thing map_2x6_2x2r          = {{1, 10, 11, 0,          3, 8, 9, 2,            5, 6, 7, 4},               MPKIND__SPLIT,       0, 3,  s2x6,   s2x2,      0x015, 0};
Private map_thing map_3x4_1x4           = {{9, 8, 6, 7,            10, 11, 4, 5,          0, 1, 3, 2},               MPKIND__SPLIT,       0, 3,  s3x4,   s1x4,      0x000, 1};
Private map_thing map_2x3_1x2           = {{0, 5,                  1, 4,                  2, 3},                     MPKIND__SPLIT,       0, 3,  s2x3,   s1x2,      0x015, 0};
Private map_thing map_1x6_1x2           = {{0, 1,                  2, 5,                  4, 3},                     MPKIND__SPLIT,       0, 3,  s1x6,   s1x2,      0x000, 0};
Private map_thing map_2x2h              = {{3, 2,                             0, 1},                                 MPKIND__SPLIT,       0, 2,  s2x2,   s1x2,      0x000, 1};
Private map_thing map_2x4_2x2r          = {{1, 6, 7, 0,                       3, 4, 5, 2},                           MPKIND__SPLIT,       0, 2,  s2x4,   s2x2,      0x005, 0};
Private map_thing map_2x6_2x3           = {{0, 1, 2, 9, 10, 11,               3, 4, 5, 6, 7, 8},                     MPKIND__SPLIT,       0, 2,  s2x6,   s2x3,      0x000, 0};



Private map_thing map_short6_2 =
   {{5, 0, 4, 9, 7, 8,                 3, 1, 2, 11, 6, 10},                   MPKIND__SPLIT,       0, 2, sdeepqtg,s_short6,  0x000, 0, MAPCODE(s_short6,2,MPKIND__SPLIT,   0)};


        map_thing map_qtag_2x3          = {{0, 7, 5, -1, 6, -1,               -1, 2, -1, 4, 3, 1},                   MPKIND__SPLIT,       0, 2,  s_qtag, s2x3,      0x005, 0};
        map_thing map_2x3_rmvr          = {{2, 5, 7, 9, 10, 0,                3, 4, 6, 8, 11, 1},                    MPKIND__REMOVED,     0, 2,  s3x4,   s2x3,      0x005, 0};
        map_thing map_dbloff1           = {{0, 1, 3, 2, 4, 5, 7, 6},                                                 MPKIND__NONE,        0, 1,  s2x4,   s_qtag,    0x000, 0};
        map_thing map_dbloff2           = {{2, 3, 4, 5, 6, 7, 0, 1},                                                 MPKIND__NONE,        0, 1,  s2x4,   s_qtag,    0x000, 0};
        map_thing map_dhrgl1            = {{0, 1, 9, 6, 7, 3},                                                       MPKIND__NONE,        0, 1,  sbighrgl,s1x6,     0x000, 0};
        map_thing map_dhrgl2            = {{11, 10, 9, 5, 4, 3},                                                     MPKIND__NONE,        0, 1,  sbighrgl,s1x6,     0x000, 0};


        map_thing map_dbgbn1            = {{0, 1, 3, 2, 6, 7, 9, 8},                                                 MPKIND__NONE,        0, 1,  sbigbone,s1x8,     0x000, 0};
        map_thing map_dbgbn2            = {{11, 10, 3, 2, 5, 4, 9, 8},                                               MPKIND__NONE,        0, 1,  sbigbone,s1x8,     0x000, 0};
        map_thing map_off1x81           = {{0, 1, 3, 2, 8, 9, 11, 10},                                               MPKIND__NONE,        0, 1,  s2x8,    s1x8,     0x000, 0};
        map_thing map_off1x82           = {{15, 14, 12, 13, 7, 6, 4, 5},                                             MPKIND__NONE,        0, 1,  s2x8,    s1x8,     0x000, 0};



/* Maps for turning triangles into boxes for the "triangle" concept. */

        map_thing map_trngl_box1        = {{1, 2, -1, 0},                                                            MPKIND__NONE,        1, 1,  s_trngl, s2x2,     0x000, 0};
        map_thing map_trngl_box2        = {{1, 2, 0, -1},                                                            MPKIND__NONE,        1, 1,  s_trngl, s2x2,     0x000, 0};

        map_thing map_inner_box         = {{2, 3, 4, 5, 10, 11, 12, 13},                                             MPKIND__NONE,        0, 1,  s2x8,   s2x4,      0x000, 0};


        map_thing map_lh_c1phana        = {{0, -1, 1, -1, 2, -1, 3, -1, 6, -1, 7, -1, 8, -1, 9, -1},                 MPKIND__OFFS_L_HALF, 0, 1,  sbigdmd, s_c1phan, 0x000, 0};
        map_thing map_lh_c1phanb        = {{-1, 12, -1, 10, -1, 3, -1, 15, -1, 4, -1, 2, -1, 11, -1, 7},             MPKIND__OFFS_L_HALF, 4, 1,  s4x4,   s_c1phan,  0x000, 0};

        map_thing map_lh_s2x3_3         = {{1, 2, 4, 5, 7, 3,                 13, 15, 11, 9, 10, 12},                MPKIND__OFFS_L_HALF, 0, 2,  s4x4,   s2x3,      0x005, 1};
        map_thing map_lh_s2x3_2         = {{9, 11, 7, 5, 6, 8,                13, 14, 0, 1, 3, 15},                  MPKIND__OFFS_L_HALF, 0, 2,  s4x4,   s2x3,      0x000, 0};

        map_thing map_rh_c1phana        = {{-1, 2, -1, 3, -1, 5, -1, 4, -1, 8, -1, 9, -1, 11, -1, 10},               MPKIND__OFFS_R_HALF, 0, 1,  sbigdmd, s_c1phan, 0x000, 0};
        map_thing map_rh_c1phanb        = {{15, -1, 3, -1, 0, -1, 1, -1, 7, -1, 11, -1, 8, -1, 9, -1},               MPKIND__OFFS_R_HALF, 4, 1,  s4x4,   s_c1phan,  0x000, 0};

        map_thing map_rh_s2x3_3         = {{15, 11, 6, 8, 9, 10,              0, 1, 2, 7, 3, 14},                    MPKIND__OFFS_R_HALF, 0, 2,  s4x4,   s2x3,      0x005, 1};
        map_thing map_rh_s2x3_2         = {{12, 13, 14, 3, 15, 10,            11, 7, 2, 4, 5, 6},                    MPKIND__OFFS_R_HALF, 0, 2,  s4x4,   s2x3,      0x000, 0};



Private map_thing map_blob_1x4a         = {{13, 10, 6, 8,          15, 17, 3, 5,          18, 20, 1, 22},            MPKIND__NONE,        0, 3,  s_bigblob,   s1x4, 0x000, 1};
Private map_thing map_blob_1x4b         = {{19, 16, 12, 14,        21, 23, 9, 11,         0, 2, 7, 4},               MPKIND__NONE,        0, 3,  s_bigblob,   s1x4, 0x000, 1};
Private map_thing map_blob_1x4c         = {{1, 22, 18, 20,         3, 5, 15, 17,          6, 8, 13, 10},             MPKIND__NONE,        0, 3,  s_bigblob,   s1x4, 0x015, 0};
Private map_thing map_blob_1x4d         = {{19, 16, 12, 14,        21, 23, 9, 11,         0, 2, 7, 4},               MPKIND__NONE,        0, 3,  s_bigblob,   s1x4, 0x015, 0};
Private map_thing map_wblob_1x4a        = {{15, 17, 5, 3, 6, 8, 10, 13,       18, 20, 22, 1, 3, 5, 17, 15},          MPKIND__NONE,        0, 2,  s_bigblob,   s2x4, 0x000, 1};
Private map_thing map_wblob_1x4b        = {{21, 23, 11, 9, 12, 14, 16, 19,    0, 2, 4, 7, 9, 11, 23, 21},            MPKIND__NONE,        0, 2,  s_bigblob,   s2x4, 0x000, 1};
Private map_thing map_wblob_1x4c        = {{3, 5, 17, 15, 18, 20, 22, 1,      6, 8, 10, 13, 15, 17, 5, 3},           MPKIND__NONE,        0, 2,  s_bigblob,   s2x4, 0x005, 0};
Private map_thing map_wblob_1x4d        = {{21, 23, 11, 9, 12, 14, 16, 19,    0, 2, 4, 7, 9, 11, 23, 21},            MPKIND__NONE,        0, 2,  s_bigblob,   s2x4, 0x005, 0};



        map_thing map_lz12     = {{10, 9,                1, 11,                5, 7,                 3, 4},          MPKIND__NONE,        0, 4,  s3x4,   s1x2,      0x055, 0};
        map_thing map_rz12     = {{0, 10,                11, 8,                2, 5,                 4, 6},          MPKIND__NONE,        0, 4,  s3x4,   s1x2,      0x055, 0};

        map_thing map_dmd_1x1  = {{0,                    1,                    2,                  3},               MPKIND__NONE,        0, 4,  sdmd,        s1x1, 0x000, 0};
        map_thing map_star_1x1 = {{0,                    1,                    2,                  3},               MPKIND__NONE,        0, 4,  s_star,      s1x1, 0x000, 0};

        map_thing map_qtag_f0           = {{7, 3,                           0, 0},                                   MPKIND__NONE,        0, 1,  s_qtag,      s1x2, 0x000, 0};
        map_thing map_qtag_f1           = {{1, 3,                           7, 5},                                   MPKIND__NONE,        0, 2,  s_qtag,      s1x2, 0x005, 0};
        map_thing map_qtag_f2           = {{3, 4,                           0, 7},                                   MPKIND__NONE,        0, 2,  s_qtag,      s1x2, 0x005, 0};

        map_thing map_diag2a            = {{5, 7, 21, 15, 17, 19, 9, 3},                                             MPKIND__NONE,        0, 1,  s4x6,        s2x4,      1, 0};
        map_thing map_diag2b            = {{2, 8, 22, 12, 14, 20, 10, 0},                                            MPKIND__NONE,        0, 1,  s4x6,        s2x4,      1, 0};
        map_thing map_diag23a           = {{13, 3, 2, 5, 11, 10},                                                    MPKIND__NONE,        0, 1,  s4x4,        s2x3,      0, 0};
        map_thing map_diag23b           = {{13, 3, 2, 5, 11, 10},                                                    MPKIND__NONE,        0, 1,  s4x4,        s2x3,      1, 0};
        map_thing map_diag23c           = {{9, 15, 14, 1, 7, 6},                                                     MPKIND__NONE,        0, 1,  s4x4,        s2x3,      0, 0};
        map_thing map_diag23d           = {{1, 7, 6, 9, 15, 14},                                                     MPKIND__NONE,        0, 1,  s4x4,        s2x3,      1, 0};


        map_thing map_f2x8_4x4          = {{-1, 3, 12, 2, -1, -1, -1, 13, -1, 15, 0, 14, -1, -1, -1, 1,
                                            -1, 7,  8, 6, -1, -1, -1,  9, -1, 11, 4, 10, -1, -1, -1, 5},             MPKIND__SPLIT,       0, 2,  sfat2x8,  s4x4,    0x000, 0};
        map_thing map_w4x4_4x4          = {{13, 15, 11, 10, 6, 8, -1, 9, -1, -1, -1, -1, -1, -1, 12, -1,
                                            -1, -1, -1, -1, -1, -1, 4, -1, 5, 7, 3, 2, 14, 0, -1, 1},                MPKIND__SPLIT,       0, 2,  swide4x4, s4x4,    0x000, 0};
        map_thing map_f2x8_2x8          = {{15, 14, 13, 12, 11, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1, -1
                                            -1, -1, -1, -1, -1, -1, -1, -1, 7, 6, 5, 4, 3, 2, 1, 0},                 MPKIND__SPLIT,       0, 2,  sfat2x8,  s2x8,    0x000, 1};
        map_thing map_w4x4_2x8          = {{-1, -1, 9, 11, 7, 2, -1, -1, -1, -1,  4, 5, 6, 8, -1, -1,
                                            -1, -1, 12, 13, 14, 0, -1, -1, -1, -1, 1, 3, 15, 10, -1, -1},            MPKIND__SPLIT,       0, 2,  swide4x4, s2x8,    0x000, 1};



        map_thing *maps_3diag[4] = {&map_blob_1x4c, &map_blob_1x4a, &map_blob_1x4d, &map_blob_1x4b};
        map_thing *maps_3diagwk[4] = {&map_wblob_1x4a, &map_wblob_1x4c, &map_wblob_1x4b, &map_wblob_1x4d};


map_thing map_init_table2[] = {
   {{0, 1, 5, 4, 6, 7, 11, 10},                                               MPKIND_DBLBENTCW,    0, 1,  sbigh,       s1x8, 0x000, 0, MAPCODE(s1x8,1,MPKIND_DBLBENTCW,    0)},
   {{3, 2, 5, 4, 9, 8, 11, 10},                                               MPKIND_DBLBENTCCW,   0, 1,  sbigh,       s1x8, 0x000, 0, MAPCODE(s1x8,1,MPKIND_DBLBENTCCW,   0)},
   {{1, 9, 8, 12, 13, 21, 20, 0},                                             MPKIND_DBLBENTCW,    0, 1,  s4x6,        s2x4, 0x000, 0, MAPCODE(s2x4,1,MPKIND_DBLBENTCW,    0)},
   {{17, 9, 8, 4, 5, 21, 20, 16},                                             MPKIND_DBLBENTCCW,   0, 1,  s4x6,        s2x4, 0x000, 0, MAPCODE(s2x4,1,MPKIND_DBLBENTCCW,   0)},

   {{0, 1, 3, 2,            4, 5, 10, 11,          9, 8, 6, 7},               MPKIND__NONISOTROPIC,0, 3,  sbigh,       s1x4, 0x011, 0, MAPCODE(s1x4,3,MPKIND__NONISOTROPIC,1)},
   {{0, 1, 3, 2,            4, 5, 10, 11,          9, 8, 6, 7},               MPKIND__NONISOTROPIC,0, 3,  sbigx,       s1x4, 0x004, 0, MAPCODE(s1x4,3,MPKIND__NONISOTROPIC,0)},
   {{0, 10, 8, 9,           11, 1, 5, 7,           2, 3, 6, 4},               MPKIND__NONISOTROPIC,0, 3,  s_3mdmd,     sdmd, 0x011, 0, MAPCODE(sdmd,3,MPKIND__NONISOTROPIC,1)},
   {{9, 0, 10, 8,           1, 5, 7, 11,           4, 2, 3, 6},               MPKIND__NONISOTROPIC,0, 3,  s_3mptpd,    sdmd, 0x004, 0, MAPCODE(sdmd,3,MPKIND__NONISOTROPIC,0)},
   /*  This was a misguided attempt to do triple waves step and fold.
   {{0, 1, 10, 11,          3, 8, 9, 2,            4, 5, 6, 7},               MPKIND__NONISOTROPIC,0, 3,  s2x6,        s2x2, 0x004, 0, MAPCODE(s2x2,3,MPKIND__NONISOTROPIC,0)},
   */

   {{0, 13, 11, 12,     14, 1, 15, 10,     7, 2, 6, 9,      3, 4, 8, 5},      MPKIND__NONISOTROPIC,0, 4,  s_4mdmd,     sdmd, 0x041, 0, MAPCODE(sdmd,4,MPKIND__NONISOTROPIC,1)},
   {{12, 0, 13, 11,     1, 15, 10, 14,     2, 6, 9, 7,      5, 3, 4, 8},      MPKIND__NONISOTROPIC,0, 4,  s_4mptpd,    sdmd, 0x014, 0, MAPCODE(sdmd,4,MPKIND__NONISOTROPIC,0)},


   {{5, 4, 0,                          2, 1, 3},                              MPKIND__SPLIT,       0, 2,  s_bone6, s_trngl,  0x207, 0, MAPCODE(s_trngl,2,MPKIND__NONISOTROP1,1)},
   {{7, 6, 5, 0,                       3, 2, 1, 4},                           MPKIND__SPLIT,       0, 2,  s_bone, s_trngl4,  0x207, 0, MAPCODE(s_trngl4,2,MPKIND__NONISOTROP1,1)},

   {{1, 3, 15, 13,                     9, 11, 7, 5},                          MPKIND__SPLIT,       0, 2,  s_c1phan,s_trngl4, 0x202, 0, MAPCODE(s_trngl4,2,MPKIND__NONISOTROP1,0)},





   {{12, 15, 14, 1, 4, 7, 6, 9,        10, 13, 3, 0, 2, 5, 11, 8},            MPKIND__STAG,        0, 2,  s4x4,        s2x4, 0x000, 1, MAPCODE(s2x4,2,MPKIND__STAG,        0)},
   {{10, 13, 3, 0, 2, 5, 11, 8,        12, 15, 14, 1, 4, 7, 6, 9},            MPKIND__STAG,        0, 2,  s4x4,        s2x4, 0x000, 1, MAPCODE(s2x4,2,MPKIND__STAG,        1)},
   {{12, 14, 1, 7, 4, 6, 9, 15,        13, 0, 2, 3, 5, 8, 10, 11},            MPKIND__STAG,        0, 2,  s4x4,      s_qtag, 0x000, 1, MAPCODE(s_qtag,2,MPKIND__STAG,      0)},
   {{13, 0, 2, 3, 5, 8, 10, 11,        12, 14, 1, 7, 4, 6, 9, 15},            MPKIND__STAG,        0, 2,  s4x4,      s_qtag, 0x000, 1, MAPCODE(s_qtag,2,MPKIND__STAG,      1)},

   {{14, 1, 4, 7, 6, 9, 12, 15,        2, 5, 8, 11, 10, 13, 0, 3},            MPKIND__DIAGQTAG,    0, 2,  s4x4,      s_qtag, 0x000, 1, MAPCODE(s_qtag,2,MPKIND__DIAGQTAG,  0)},
   {{14, 1, 4, 7, 6, 9, 12, 15,        10, 13, 0, 3, 2, 5, 8, 11},            MPKIND__DIAGQTAG,    0, 2,  s4x4,      s_qtag, 0x000, 1, MAPCODE(s_qtag,2,MPKIND__DIAGQTAG,  1)},

   {{0, 3, 4, 7, 2, 5, 6, 1},                                                 MPKIND__DMD_STUFF,   0, 2,  s_thar,      sdmd, 0x104, 0, MAPCODE(sdmd,2,MPKIND__DMD_STUFF,   0)},
   {{0, 3, 4, 7, 2, 5, 6, 1},                                                 MPKIND__DMD_STUFF,   0, 2,  s_thar,      sdmd, 0x004, 0, MAPCODE(sdmd,2,MPKIND__DMD_STUFF,   1)},
   {{0, 2,                 1, 3},                                             MPKIND__DMD_STUFF,   0, 2,  sdmd,        s1x2, 0x004, 0, MAPCODE(s1x2,2,MPKIND__DMD_STUFF,   0)},
   {{1, 3,                 0, 2},                                             MPKIND__DMD_STUFF,   0, 2,  sdmd,        s1x2, 0x001, 0, MAPCODE(s1x2,2,MPKIND__DMD_STUFF,   1)},
   {{0, 1, 2, 3, 1, 2, 3, 0},                                                 MPKIND__DMD_STUFF,   0, 2,  s2x2,        s2x2, 0x104, 0, MAPCODE(s2x2,2,MPKIND__DMD_STUFF,   0)},
   {{0, 1, 4, 5, 2, 3, 6, 7},                                                 MPKIND__DMD_STUFF,   0, 2,  s_thar,      s1x4, 0x104, 0, MAPCODE(s1x4,2,MPKIND__DMD_STUFF,   0)},
   {{0, 1, 4, 5, 2, 3, 6, 7},                                                 MPKIND__DMD_STUFF,   0, 2,  s_thar,      s1x4, 0x004, 0, MAPCODE(s1x4,2,MPKIND__DMD_STUFF,   1)},
   {{13, 14, 5, 6,         1, 2, 9, 10},                                      MPKIND__ALL_8,       0, 2,  s4x4,        s2x2, 0x004, 0, MAPCODE(s2x2,2,MPKIND__ALL_8,       0)},
   {{1, 2, 9, 10,          13, 14, 5, 6},                                     MPKIND__ALL_8,       0, 2,  s4x4,        s2x2, 0x001, 0, MAPCODE(s2x2,2,MPKIND__ALL_8,       1)},
   {{0, 3, 4, 7,           2, 5, 6, 1},                                       MPKIND__ALL_8,       0, 2,  s_thar,      sdmd, 0x004, 0, MAPCODE(sdmd,2,MPKIND__ALL_8,       0)},
   {{2, 5, 6, 1,           0, 3, 4, 7},                                       MPKIND__ALL_8,       0, 2,  s_thar,      sdmd, 0x001, 0, MAPCODE(sdmd,2,MPKIND__ALL_8,       1)},
   {{0, 1, 4, 5,           2, 3, 6, 7},                                       MPKIND__ALL_8,       0, 2,  s_thar,      s1x4, 0x004, 0, MAPCODE(s1x4,2,MPKIND__ALL_8,       0)},
   {{2, 3, 6, 7,           0, 1, 4, 5},                                       MPKIND__ALL_8,       0, 2,  s_thar,      s1x4, 0x001, 0, MAPCODE(s1x4,2,MPKIND__ALL_8,       1)},

   {{13, 14,               1, 2,                 6, 5,               10, 9},  MPKIND__4_EDGES,     0, 4,  s4x4,        s1x2, 0x044, 0, MAPCODE(s1x2,4,MPKIND__4_EDGES,     0)},
   {{2, 3,                 5, 4,                 7, 6,               0, 1},   MPKIND__4_EDGES,     0, 4,  s_thar,      s1x2, 0x011, 0, MAPCODE(s1x2,4,MPKIND__4_EDGES,     1)},
   {{0, 2,                 4, 6,                 10, 8,              14, 12}, MPKIND__4_QUADRANTS, 0, 4,  s_c1phan,    s1x2, 0x044, 0, MAPCODE(s1x2,4,MPKIND__4_QUADRANTS, 0)},
   {{1, 3,                 7, 5,                 11, 9,              13, 15}, MPKIND__4_QUADRANTS, 0, 4,  s_c1phan,    s1x2, 0x011, 0, MAPCODE(s1x2,4,MPKIND__4_QUADRANTS, 1)},
   {{0, 1, 2, 3,        7, 4, 5, 6,        10, 11, 8, 9,    13, 14, 15, 12},  MPKIND__4_QUADRANTS, 0, 4,  s_c1phan,  s_star, 0x000, 0, MAPCODE(s_star,4,MPKIND__4_QUADRANTS, 0)},

   {{-1, 0, 1, 2, 11, 12, 13, 14,      3, 4, 5, 6, -1, 8, 9, 10},             MPKIND__OFFS_L_HALF_SPECIAL, 0, 2,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF_SPECIAL, 0)},
   {{1, 2, 3, 4, 13, 14, 15, -1,       5, 6, 7, -1, 9, 10, 11, 12},           MPKIND__OFFS_R_HALF_SPECIAL, 0, 2,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF_SPECIAL, 0)},

   {{1, 8, 17, 12, 13, 16, 9, 0,       3, 6, 19, 10, 11, 18, 7, 2},           MPKIND__OFFS_L_HALF_SPECIAL, 0, 2,  s4x5,   s2x4,      0x005, 0, MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF_SPECIAL, 1)},
   {{2, 7, 16, 13, 14, 15, 8, 1,       4, 5, 18, 11, 12, 17, 6, 3},           MPKIND__OFFS_R_HALF_SPECIAL, 0, 2,  s4x5,   s2x4,      0x005, 0, MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF_SPECIAL, 1)},

   {{0, 1, 2, 3, 6, 7, 8, 9},                                                 MPKIND__OFFS_L_HALF, 0, 1,  s2x6,   s2x4,      0x000, 0, MAPCODE(s2x4,1,MPKIND__OFFS_L_HALF, 0)},
   {{2, 3, 4, 5, 8, 9, 10, 11},                                               MPKIND__OFFS_R_HALF, 0, 1,  s2x6,   s2x4,      0x000, 0, MAPCODE(s2x4,1,MPKIND__OFFS_R_HALF, 0)},
   {{0, 1, 2, 3, 8, 9, 10, 11},                                               MPKIND__OFFS_L_FULL, 0, 1,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,1,MPKIND__OFFS_L_FULL, 0)},
   {{4, 5, 6, 7, 12, 13, 14, 15},                                             MPKIND__OFFS_R_FULL, 0, 1,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,1,MPKIND__OFFS_R_FULL, 0)},

   {{0, 1, 2, 3, 7, 8, 9, 10, 11, 12, 16, 17},                                MPKIND__OFFS_L_HALF, 7, 1,  s3x6,   s3x4,      0x000, 0, MAPCODE(s3x4,1,MPKIND__OFFS_L_HALF, 0)},
  {{2, 3, 4, 5, 7, 8, 11, 12, 13, 14, 16, 17},                                MPKIND__OFFS_R_HALF, 7, 1,  s3x6,   s3x4,      0x000, 0, MAPCODE(s3x4,1,MPKIND__OFFS_R_HALF, 0)},
   {{1, 2, 7, 8, 10, 11, 16, 17},                                             MPKIND__OFFS_L_HALF, 7, 1,  s3x6,   s_qtag,    0x000, 0, MAPCODE(s_qtag,1,MPKIND__OFFS_L_HALF, 0)},
   {{3, 4, 7, 8, 12, 13, 16, 17},                                             MPKIND__OFFS_R_HALF, 7, 1,  s3x6,   s_qtag,    0x000, 0, MAPCODE(s_qtag,1,MPKIND__OFFS_R_HALF, 0)},

   {{-1, -1, 0, 1, 2, 3, -1, -1, 5, 6, 7, 8},                                 MPKIND__OFFS_L_HALF, 5, 1,  s2x5,   s2x6,      0x000, 0, MAPCODE(s2x6,1,MPKIND__OFFS_L_HALF, 0)},
   {{1, 2, 3, 4, -1, -1, 6, 7, 8, 9, -1, -1},                                 MPKIND__OFFS_R_HALF, 5, 1,  s2x5,   s2x6,      0x000, 0, MAPCODE(s2x6,1,MPKIND__OFFS_R_HALF, 0)},

   {{0, 1, 2, 3, 4, 5, 6, 7},                                                 MPKIND__OFFS_L_HALF, 2, 1,  s1x8,   s1x8,      0x000, 0, MAPCODE(s1x8,1,MPKIND__OFFS_L_HALF, 0)},
   {{0, 1, 2, 3, 4, 5, 6, 7},                                                 MPKIND__OFFS_R_HALF, 2, 1,  s1x8,   s1x8,      0x000, 0, MAPCODE(s1x8,1,MPKIND__OFFS_R_HALF, 0)},
   {{0, 1, 2, 3, 4, 5, 6, 7},                                                 MPKIND__OFFS_L_FULL, 2, 1,  s1x8,   s1x8,      0x000, 0, MAPCODE(s1x8,1,MPKIND__OFFS_L_FULL, 0)},
   {{0, 1, 2, 3, 4, 5, 6, 7},                                                 MPKIND__OFFS_R_FULL, 2, 1,  s1x8,   s1x8,      0x000, 0, MAPCODE(s1x8,1,MPKIND__OFFS_R_FULL, 0)},

   {{10, 11, 2, 3, 4, 5, 8, 9},                                               MPKIND__OFFS_L_HALF, 0, 1,  s3x4,   s2x4,      0x000, 1, MAPCODE(s2x4,1,MPKIND__OFFS_L_HALF, 1)},
   {{15, 16, 17, 3, 4, 5, 6, 7, 8, 12, 13, 14},                               MPKIND__OFFS_L_HALF, 0, 1,  s3x6,   s2x6,      0x000, 1, MAPCODE(s2x6,1,MPKIND__OFFS_L_HALF, 1)},
   {{0, 1, 5, 4, 6, 7, 11, 10},                                               MPKIND__OFFS_R_HALF, 0, 1,  s3x4,   s2x4,      0x000, 1, MAPCODE(s2x4,1,MPKIND__OFFS_R_HALF, 1)},
   {{0, 1, 2, 8, 7, 6, 9, 10, 11, 17, 16, 15},                                MPKIND__OFFS_R_HALF, 0, 1,  s3x6,   s2x6,      0x000, 1, MAPCODE(s2x6,1,MPKIND__OFFS_R_HALF, 1)},
   {{13, 15, 2, 4, 5, 7, 10, 12},                                             MPKIND__OFFS_L_FULL, 0, 1,  s4x4,   s2x4,      0x001, 0, MAPCODE(s2x4,1,MPKIND__OFFS_L_FULL, 1)},
   {{0, 1, 11, 6, 8, 9, 3, 14},                                               MPKIND__OFFS_R_FULL, 0, 1,  s4x4,   s2x4,      0x001, 0, MAPCODE(s2x4,1,MPKIND__OFFS_R_FULL, 1)},

   {{10, 11, 8, 9,                     2, 3, 4, 5},                           MPKIND__OFFS_L_HALF, 0, 2,  s3x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__OFFS_L_HALF, 0)},
   {{0, 1, 11, 10,                     5, 4, 6, 7},                           MPKIND__OFFS_R_HALF, 0, 2,  s3x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__OFFS_R_HALF, 0)},
   {{9, 11, 6, 8,                      14, 0, 1, 3},                          MPKIND__OFFS_L_FULL, 0, 2,  s4x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__OFFS_L_FULL, 0)},
   {{12, 13, 15, 10,                   7, 2, 4, 5},                           MPKIND__OFFS_R_FULL, 0, 2,  s4x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__OFFS_R_FULL, 0)},

   {{11, 8, 9, 10,                     3, 4, 5, 2},                           MPKIND__OFFS_L_HALF, 0, 2,  s3x4,   s2x2,      0x005, 0, MAPCODE(s2x2,2,MPKIND__OFFS_L_HALF, 1)},
   {{1, 11, 10, 0,                     4, 6, 7, 5},                           MPKIND__OFFS_R_HALF, 0, 2,  s3x4,   s2x2,      0x005, 0, MAPCODE(s2x2,2,MPKIND__OFFS_R_HALF, 1)},
   {{11, 6, 8, 9,                      0, 1, 3, 14},                          MPKIND__OFFS_L_FULL, 0, 2,  s4x4,   s2x2,      0x005, 0, MAPCODE(s2x2,2,MPKIND__OFFS_L_FULL, 1)},
   {{13, 15, 10, 12,                   2, 4, 5, 7},                           MPKIND__OFFS_R_FULL, 0, 2,  s4x4,   s2x2,      0x005, 0, MAPCODE(s2x2,2,MPKIND__OFFS_R_FULL, 1)},

   {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_L_HALF, 0, 2,  s1p5x8, s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__OFFS_L_HALF, 0)},
   {{0, 1, 3, 2,                       11, 10, 8, 9},                         MPKIND__OFFS_R_HALF, 0, 2,  s1p5x8, s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__OFFS_R_HALF, 0)},
   {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_L_FULL, 0, 2,  s2x8,   s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__OFFS_L_FULL, 0)},
   {{0, 1, 3, 2,                       11, 10, 8, 9},                         MPKIND__OFFS_R_FULL, 0, 2,  s2x8,   s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__OFFS_R_FULL, 0)},

   {{9, 8, 6, 7,                       0, 1, 3, 2},                           MPKIND__OFFS_L_HALF, 0, 2,  s2x6,   s1x4,      0x000, 1, MAPCODE(s1x4,2,MPKIND__OFFS_L_HALF, 1)},
   {{11, 10, 8, 9,                     2, 3, 5, 4},                           MPKIND__OFFS_R_HALF, 0, 2,  s2x6,   s1x4,      0x000, 1, MAPCODE(s1x4,2,MPKIND__OFFS_R_HALF, 1)},
   {{11, 10, 8, 9,                     0, 1, 3, 2},                           MPKIND__OFFS_L_FULL, 0, 2,  s2x8,   s1x4,      0x000, 1, MAPCODE(s1x4,2,MPKIND__OFFS_L_FULL, 1)},
   {{15, 14, 12, 13,                   4, 5, 7, 6},                           MPKIND__OFFS_R_FULL, 0, 2,  s2x8,   s1x4,      0x000, 1, MAPCODE(s1x4,2,MPKIND__OFFS_R_FULL, 1)},

   {{6, 7, 8, 9,                       0, 1, 2, 3},                           MPKIND__OFFS_L_HALF, 0, 2,  sbigdmd,s_trngl4,  0x107, 1, MAPCODE(s_trngl4,2,MPKIND__OFFS_L_HALF, 1)},
   {{11, 10, 9, 8,                     5, 4, 3, 2},                           MPKIND__OFFS_R_HALF, 0, 2,  sbigdmd,s_trngl4,  0x10D, 1, MAPCODE(s_trngl4,2,MPKIND__OFFS_R_HALF, 1)},

   {{15, 16, 17, 12, 13, 14,           3, 4, 5, 6, 7, 8},                     MPKIND__OFFS_L_HALF, 0, 2,  s3x6,   s2x3,      0x000, 0, MAPCODE(s2x3,2,MPKIND__OFFS_L_HALF, 0)},
   {{0, 1, 2, 17, 16, 15,              8, 7, 6, 9, 10, 11},                   MPKIND__OFFS_R_HALF, 0, 2,  s3x6,   s2x3,      0x000, 0, MAPCODE(s2x3,2,MPKIND__OFFS_R_HALF, 0)},
   {{20, 21, 22, 23, 16, 17, 18, 19,   4, 5, 6, 7, 8, 9, 10, 11},             MPKIND__OFFS_L_HALF, 0, 2,  s3x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF, 0)},
   {{0, 1, 2, 3, 23, 22, 21, 20,       11, 10, 9, 8, 12, 13, 14, 15},         MPKIND__OFFS_R_HALF, 0, 2,  s3x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF, 0)},
   {{20, 21, 22, 23, 12, 13, 14, 15,   0, 1, 2, 3, 8, 9, 10, 11},             MPKIND__OFFS_L_HALF, 0, 2,  s4x6,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF, 1)},
   {{18, 19, 20, 21, 14, 15, 16, 17,   2, 3, 4, 5, 6, 7, 8, 9},               MPKIND__OFFS_R_HALF, 0, 2,  s4x6,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF, 1)},
   {{13, 12, 10, 11, -1, -1, 9, 8,     -1, -1, 1, 0, 5, 4, 2, 3},             MPKIND__OFFS_L_HALF, 0, 2,  s2x8,   s1x8,      0x000, 1, MAPCODE(s1x8,2,MPKIND__OFFS_L_HALF, 1)},
   {{-1, -1, 14, 15, 10, 11, 13, 12,   2, 3, 5, 4, -1, -1, 6, 7},             MPKIND__OFFS_R_HALF, 0, 2,  s2x8,   s1x8,      0x000, 1, MAPCODE(s1x8,2,MPKIND__OFFS_R_HALF, 1)},

   {{10, 1, 11, 9,                     5, 3, 4, 7},                           MPKIND__LILZCCW,     0, 2,  s3x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__LILZCCW,0)},
   {{0, 11, 8, 10,                     2, 4, 6, 5},                           MPKIND__LILZCW,      0, 2,  s3x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__LILZCW,0)},
   {{15, 1, 16, 14,     17, 3, 8, 12,     7, 5, 6, 10},                       MPKIND__LILZCCW,     0, 3,  s3x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__LILZCCW,0)},
   {{0, 16, 13, 15,     2, 8, 11, 17,     4, 6, 9, 7},                        MPKIND__LILZCW,      0, 3,  s3x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__LILZCW,0)},
   {{0, 16, 13, 15,     17, 3, 8, 12,     4, 6, 9, 7},                        MPKIND__LILAZCCW,    0, 3,  s3x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__LILAZCCW,0)},
   {{15, 1, 16, 14,     2, 8, 11, 17,     7, 5, 6, 10},                       MPKIND__LILAZCW,     0, 3,  s3x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__LILAZCW,0)},
   {{1, 11, 8, 9, 10, 0,               3, 4, 6, 7, 5, 2},                     MPKIND__LILZCOM,     0, 2,  s3x4,   s2x3,      0x005, 0, MAPCODE(s2x3,2,MPKIND__LILZCOM,1)},
   {{1, 16, 13, 14, 15, 0,  3, 8, 11, 12, 17, 2,   5, 6, 9, 10, 7, 4},        MPKIND__LILZCOM,     0, 3,  s3x6,   s2x3,      0x015, 0, MAPCODE(s2x3,3,MPKIND__LILZCOM,1)},

   {{0, 1, 6, 7, 8, 9, 14, 15,         2, 3, 4, 5, 10, 11, 12, 13},           MPKIND__CONCPHAN,    0, 2,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__CONCPHAN,    0)},
   {{0, 1, 4, 5, 10, 11, 14, 15,       2, 3, 6, 7, 8, 9, 12, 13},             MPKIND__INTLK,       0, 2,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__INTLK,       0)},
   {{12, 13, 14, 0, 4, 5, 6, 8,        10, 15, 3, 1, 2, 7, 11, 9},            MPKIND__CONCPHAN,    0, 2,  s4x4,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__CONCPHAN,    1)},
   {{10, 15, 3, 1, 4, 5, 6, 8,         12, 13, 14, 0, 2, 7, 11, 9},           MPKIND__INTLK,       0, 2,  s4x4,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__INTLK,       1)},
   {{0, 1, 3, 2, 8, 9, 11, 10,         4, 5, 7, 6, 12, 13, 15, 14},           MPKIND__CONCPHAN,    0, 2,  s1x16,  s1x8,      0x000, 0, MAPCODE(s1x8,2,MPKIND__CONCPHAN,    0)},
   {{0, 1, 3, 2, 12, 13, 15, 14,       4, 5, 7, 6, 8, 9, 11, 10},             MPKIND__INTLK,       0, 2,  s1x16,  s1x8,      0x000, 0, MAPCODE(s1x8,2,MPKIND__INTLK,       0)},
   {{0, 3, 4, 5, 8, 11, 12, 13,        1, 2, 6, 7, 9, 10, 14, 15},            MPKIND__CONCPHAN,    0, 2,  s4dmd,  s_qtag,    0x000, 0, MAPCODE(s_qtag,2,MPKIND__CONCPHAN,  0)},
   {{0, 2, 6, 7, 9, 11, 12, 13,        1, 3, 4, 5, 8, 10, 14, 15},            MPKIND__INTLK,       0, 2,  s4dmd,  s_qtag,    0x000, 0, MAPCODE(s_qtag,2,MPKIND__INTLK,     0)},
   {{12, 0, 13, 11, 4, 8, 5, 3,        14, 1, 15, 10, 6, 9, 7, 2},            MPKIND__CONCPHAN,    0, 2,  s4ptpd, s_ptpd,    0x000, 0, MAPCODE(s_ptpd,2,MPKIND__CONCPHAN,  0)},
   {{12, 0, 13, 11, 6, 9, 7, 2,        14, 1, 15, 10, 4, 8, 5, 3},            MPKIND__INTLK,       0, 2,  s4ptpd, s_ptpd,    0x000, 0, MAPCODE(s_ptpd,2,MPKIND__INTLK,     0)},
   {{12, 0, 13, 11, 15, 10, 14, 1,     7, 2, 6, 9, 4, 8, 5, 3},               MPKIND__SPLIT,       0, 2,  s4ptpd, s_ptpd,    0x000, 0, MAPCODE(s_ptpd,2,MPKIND__SPLIT,     0)},
   {{0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16, 17,
                            11, 10, 9, 8, 7, 6, 23, 22, 21, 20, 19, 18},      MPKIND__CONCPHAN,    0, 2,  s4x6,   s2x6,      0x000, 1, MAPCODE(s2x6,2,MPKIND__CONCPHAN,    1)},
   {{11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17,
                            0, 1, 2, 3, 4, 5, 23, 22, 21, 20, 19, 18},        MPKIND__INTLK,       0, 2,  s4x6,   s2x6,      0x000, 1, MAPCODE(s2x6,2,MPKIND__INTLK,       1)},
   {{3, 4, 6, 9, 10, 0,                2, 5, 7, 8, 11, 1},                    MPKIND__CONCPHAN,    0, 2,  s3x4,   s2x3,      0x005, 0, MAPCODE(s2x3,2,MPKIND__CONCPHAN,    1)},
   {{2, 5, 7, 9, 10, 0,                3, 4, 6, 8, 11, 1},                    MPKIND__INTLK,       0, 2,  s3x4,   s2x3,      0x005, 0, MAPCODE(s2x3,2,MPKIND__INTLK,       1)},

   {{0, 5, 6, 11,           1, 4, 7, 10,           2, 3, 8, 9},               MPKIND__CONCPHAN,    0, 3,  s2x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__CONCPHAN,    0)},
   {{5, 6, 11, 0,           4, 7, 10, 1,           3, 8, 9, 2},               MPKIND__CONCPHAN,    0, 3,  s2x6,   s2x2,      0x015, 0, MAPCODE(s2x2,3,MPKIND__CONCPHAN,    1)},
   {{0, 7, 8, 15,     1, 6, 9, 14,     2, 5, 10, 13,     3, 4, 11, 12},       MPKIND__CONCPHAN,    0, 4,  s2x8,   s2x2,      0x000, 0, MAPCODE(s2x2,4,MPKIND__CONCPHAN,    0)},

   {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__O_SPOTS,     0, 2,  s4x4,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__O_SPOTS,     1)},
   {{10, 13, 14, 1, 2, 5, 6, 9,        12, 15, 3, 0, 4, 7, 11, 8},            MPKIND__X_SPOTS,     0, 2,  s4x4,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__X_SPOTS,     1)},

   {{0, 1, 4, 5,                       1, 2, 3, 4},                           MPKIND__OVERLAP,     0, 2,  s2x3,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__OVERLAP,     0)},
   {{1, 4, 5, 0,                       2, 3, 4, 1},                           MPKIND__OVERLAP,     0, 2,  s2x3,   s2x2,      0x005, 0, MAPCODE(s2x2,2,MPKIND__OVERLAP,     1)},
   {{1, 2, 3, 4, 11, 23, 15, 16, 17, 18, 21, 22,
                            3, 4, 5, 6, 9, 10, 13, 14, 15, 16, 23, 11},       MPKIND__OVERLAP,     0, 2,  s3x8,   s3x4,      0x000, 0, MAPCODE(s3x4,2,MPKIND__OVERLAP,     0)},
   {{0, 1, 5, 11, 7, 8, 9, 10,         1, 2, 3, 4, 6, 7, 11, 5},              MPKIND__OVERLAP,     0, 2,  s3dmd,  s_qtag,    0x000, 0, MAPCODE(s_qtag,2,MPKIND__OVERLAP,   0)},
   {{9, 0, 10, 8, 5, 7, 11, 1,         11, 1, 5, 7, 3, 6, 4, 2},              MPKIND__OVERLAP,     0, 2,  s3ptpd, s_ptpd,    0x000, 0, MAPCODE(s_ptpd,2,MPKIND__OVERLAP,   0)},
   {{20, 21, 22, 23, 11, 10, 9, 8, 12, 13, 14, 15, 16, 17, 18, 19,
           0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 23, 22, 21, 20},             MPKIND__OVERLAP,     0, 2,  s3x8,   s2x8,      0x000, 1, MAPCODE(s2x8,2,MPKIND__OVERLAP,     1)},
   {{0, 1, 3, 2, 10, 11, 4, 5,         4, 5, 10, 11, 6, 7, 9, 8},             MPKIND__OVERLAP,     0, 2,  s1x12,  s1x8,      0x000, 0, MAPCODE(s1x8,2,MPKIND__OVERLAP,     0)},
   {{0, 1, 3, 2, 7, 6, 4, 5,           4, 5, 7, 6, 12, 13, 15, 14,
                            15, 14, 12, 13, 8, 9, 11, 10},                    MPKIND__OVERLAP,     0, 3,  s1x16,  s1x8,      0x000, 0, MAPCODE(s1x8,3,MPKIND__OVERLAP,     0)},
   {{0, 1, 2, 3, 8, 9, 10, 11,         2, 3, 4, 5, 6, 7, 8, 9},               MPKIND__OVERLAP,     0, 2,  s2x6,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__OVERLAP,     0)},
   {{10, 11, 5, 4, 6, 7, 8, 9,         0, 1, 2, 3, 4, 5, 11, 10},             MPKIND__OVERLAP,     0, 2,  s3x4,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__OVERLAP,     1)},
   {{1, 11, 8, 9, 10, 0,    2, 5, 7, 8, 11, 1,     3, 4, 6, 7, 5, 2},         MPKIND__OVERLAP,     0, 3,  s3x4,   s2x3,      0x015, 0, MAPCODE(s2x3,3,MPKIND__OVERLAP,     1)},
   {{0, 1, 6, 7,            1, 2, 5, 6,            2, 3, 4, 5},               MPKIND__OVERLAP,     0, 3,  s2x4,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__OVERLAP,     0)},
   {{0, 1, 15, 14, 10, 11, 12, 13,     1, 2, 6, 7, 9, 10, 14, 15,
                            2, 3, 4, 5, 8, 9, 7, 6},                          MPKIND__OVERLAP,     0, 3,  s4dmd,  s_qtag,    0x000, 0, MAPCODE(s_qtag,3,MPKIND__OVERLAP,   0)},
   {{12, 0, 13, 11, 15, 10, 14, 1,     14, 1, 15, 10, 6, 9, 7, 2,
                            7, 2, 6, 9, 4, 8, 5, 3},                          MPKIND__OVERLAP,     0, 3,  s4ptpd, s_ptpd,    0x000, 0, MAPCODE(s_ptpd,3,MPKIND__OVERLAP,   0)},
   {{0, 1, 5, 2,                     2, 5, 3, 4},                             MPKIND__OVERLAP,     0, 2,  s1x6,   s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__OVERLAP,     0)},
   {{0, 1, 2, 3,            3, 2, 7, 6,            6, 7, 4, 5},               MPKIND__OVERLAP,     0, 3,  s1x8,   s1x4,      0x000, 0, MAPCODE(s1x4,3,MPKIND__OVERLAP,     0)},
   {{9, 8, 6, 7,            10, 11, 4, 5,          0, 1, 3, 2},               MPKIND__OVERLAP,     3, 3,  s3x4,   s1x4,      0x000, 0, MAPCODE(s1x4,3,MPKIND__OVERLAP,     1)},
   {{7, 0, -1, 6,           -1, 1, -1, 5,          -1, 2, 3, 4},              MPKIND__OVERLAP,     0, 3,s_spindle,sdmd,      0x000, 0, MAPCODE(sdmd,3,MPKIND__OVERLAP,     0)},
   {{0, 1, 2, 3, 12, 13, 14, 15,       2, 3, 4, 5, 10, 11, 12, 13,
                             4, 5, 6, 7, 8, 9, 10, 11},                       MPKIND__OVERLAP,     0, 3,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,3,MPKIND__OVERLAP,     0)},
   {{9, 11, 7, 2, 4, 5, 6, 8,          10, 15, 3, 1, 2, 7, 11, 9,
                               12, 13, 14, 0, 1, 3, 15, 10},                  MPKIND__OVERLAP,     0, 3,  s4x4,   s2x4,      0x000, 1, MAPCODE(s2x4,3,MPKIND__OVERLAP,     1)},
   {{1, 8, 16, 13, 14, 15, 9, 0,       2, 7, 17, 12, 13, 16, 8, 1,
        3, 6, 18, 11, 12, 17, 7, 2,      4, 5, 19, 10, 11, 18, 6, 3},         MPKIND__OVERLAP,     0, 4,  s4x5,   s2x4,      0x055, 0, MAPCODE(s2x4,4,MPKIND__OVERLAP,     1)},
   {{1, 10, 19, 16, 17, 18, 11, 0,       2, 9, 20, 15, 16, 19, 10, 1,   3, 8, 21, 14, 15, 20, 9, 2,
        4, 7, 22, 13, 14, 21, 8, 3,      5, 6, 23, 12, 13, 22, 7, 4},         MPKIND__OVERLAP,     0, 5,  s4x6,   s2x4,      0x155, 0, MAPCODE(s2x4,5,MPKIND__OVERLAP,     1)},
   {{0, 1, 8, 9,       1, 2, 7, 8,       2, 3, 6, 7,      3, 4, 5, 6},        MPKIND__OVERLAP,     0, 4,  s2x5,   s2x2,      0x000, 0, MAPCODE(s2x2,4,MPKIND__OVERLAP,     0)},

   {{0, 3,                             1, 2},                                 MPKIND__REMOVED,     0, 2,  s1x4,   s1x2,      0x000, 0, MAPCODE(s1x2,2,MPKIND__REMOVED,     0)},
   {{0, 5,                  1, 4,                  2, 3},                     MPKIND__TWICE_REMOVED,0,3,  s1x6,   s1x2,      0x000, 0, MAPCODE(s1x2,3,MPKIND__TWICE_REMOVED,0)},
   {{0, 6,              1, 7,              3, 5,            2, 4},            MPKIND__THRICE_REMOVED,0,4, s1x8,   s1x2,      0x000, 0, MAPCODE(s1x2,4,MPKIND__THRICE_REMOVED,0)},

   {{0, 3,                             1, 2},                                 MPKIND__REMOVED,     0, 2,  s2x2,   s1x2,      0x005, 0, MAPCODE(s1x2,2,MPKIND__REMOVED,     1)},
   {{0, 5,                  1, 4,                  2, 3},                     MPKIND__TWICE_REMOVED,0,3,  s2x3,   s1x2,      0x015, 0, MAPCODE(s1x2,3,MPKIND__TWICE_REMOVED,1)},
   {{0, 7,              1, 6,              2, 5,            3, 4},            MPKIND__THRICE_REMOVED,0,4, s2x4,   s1x2,      0x055, 0, MAPCODE(s1x2,4,MPKIND__THRICE_REMOVED,1)},

   {{0, 2, 5, 7,                       1, 3, 4, 6},                           MPKIND__REMOVED,     0, 2,  s2x4,   s2x2,      0x000, 0, MAPCODE(s2x2,2,MPKIND__REMOVED,     0)},
   {{0, 3, 8, 11,           1, 4, 7, 10,           2, 5, 6, 9},               MPKIND__TWICE_REMOVED,0,3,  s2x6,   s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__TWICE_REMOVED,0)},
   {{0, 4, 11, 15,      1, 5, 10, 14,      2, 6, 9, 13,     3, 7, 8, 12},     MPKIND__THRICE_REMOVED,0,4, s2x8,   s2x2,      0x000, 0, MAPCODE(s2x2,4,MPKIND__THRICE_REMOVED,0)},

   {{0, 2, 4,                          1, 5, 3},                              MPKIND__REMOVED,     0, 2,  s1x6,   s1x3,      0x000, 0, MAPCODE(s1x3,2,MPKIND__REMOVED,     0)},
   {{5, 4, 3,                          0, 1, 2},                              MPKIND__REMOVED,     0, 2,  s2x3,   s1x3,      0x000, 1, MAPCODE(s1x3,2,MPKIND__REMOVED,1)},
   {{0, 2, 4, 7, 9, 11,                1, 3, 5, 6, 8, 10},                    MPKIND__REMOVED,     0, 2,  s2x6,   s2x3,      0x000, 0, MAPCODE(s2x3,2,MPKIND__REMOVED,0)},

   {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__REMOVED,     0, 2,  s1x8,   s1x4,      0x000, 0, MAPCODE(s1x4,2,MPKIND__REMOVED,0)},
   {{0, 3, 8, 11,           1, 4, 7, 10,           2, 5, 6, 9},               MPKIND__TWICE_REMOVED,0,3,  s1x12,  s1x4,      0x000, 0, MAPCODE(s1x4,3,MPKIND__TWICE_REMOVED,0)},
   {{0, 4, 11, 15,      1, 5, 10, 14,      2, 6, 9, 13,     3, 7, 8, 12},     MPKIND__THRICE_REMOVED,0,4, s1x16,  s1x4,      0x000, 0, MAPCODE(s1x4,4,MPKIND__THRICE_REMOVED,0)},

   {{7, 6, 4, 5,                       0, 1, 3, 2},                           MPKIND__REMOVED,     0, 2,  s2x4,   s1x4,      0x000, 1, MAPCODE(s1x4,2,MPKIND__REMOVED,1)},

   {{0, 2, 6, 4, 9, 11, 15, 13,        1, 3, 7, 5, 8, 10, 14, 12},            MPKIND__REMOVED,     0, 2,  s1x16,  s1x8,      0x000, 0, MAPCODE(s1x8,2,MPKIND__REMOVED,0)},

   {{15, 14, 12, 13, 8, 9, 11, 10,     0, 1, 3, 2, 7, 6, 4, 5},               MPKIND__REMOVED,     0, 2,  s2x8,   s1x8,      0x000, 1, MAPCODE(s1x8,2,MPKIND__REMOVED,1)},
   {{19, 18, 16, 17, 12, 13, 15, 14,   20, 21, 23, 22, 8, 9, 11, 10,
              0, 1, 3, 2, 7, 6, 4, 5},                                        MPKIND__TWICE_REMOVED,0,3,  s3x8,   s1x8,      0x000, 1, MAPCODE(s1x8,3,MPKIND__TWICE_REMOVED,1)},


   {{6, 0, 3, 5,                       7, 1, 2, 4},                           MPKIND__REMOVED,     0, 2,  s_rigger, sdmd,    0x000, 0, MAPCODE(sdmd,2,MPKIND__REMOVED,     0)},
   {{0, 3, 5, 6,                       1, 2, 4, 7},                           MPKIND__REMOVED,     0, 2,  s_qtag, sdmd,      0x005, 0, MAPCODE(sdmd,2,MPKIND__REMOVED,     1)},
   {{0, 2, 4, 7, 9, 11,                1, 3, 5, 6, 8, 10},                    MPKIND__REMOVED,     0, 2,  s1x12,  s1x6,      0x000, 0, MAPCODE(s1x6,2,MPKIND__REMOVED,     0)},
   {{11, 10, 9, 6, 7, 8,               0, 1, 2, 5, 4, 3},                     MPKIND__REMOVED,     0, 2,  s2x6,   s1x6,      0x000, 1, MAPCODE(s1x6,2,MPKIND__REMOVED,     1)},

   {{0, 2, 4, 6, 9, 11, 13, 15,        1, 3, 5, 7, 8, 10, 12, 14},            MPKIND__REMOVED,     0, 2,  s2x8,   s2x4,      0x000, 0, MAPCODE(s2x4,2,MPKIND__REMOVED,0)},
   {{0, 3, 6, 9, 14, 17, 20, 23,      1, 4, 7, 10, 13, 16, 19, 22,
              2, 5, 8, 11, 12, 15, 18, 21},                                   MPKIND__TWICE_REMOVED,0,3,  s2x12,  s2x4,      0x000, 0, MAPCODE(s2x4,3,MPKIND__TWICE_REMOVED,0)},
   {{10, 15, 3, 1, 4, 5, 6, 8,         12, 13, 14, 0, 2, 7, 11, 9},           MPKIND__REMOVED,     0, 2,  s4x4,   s2x4,      0x000, 1, MAPCODE(s2x4,2,MPKIND__REMOVED,1)},
   {{3, 8, 21, 14, 17, 18, 11, 0,     4, 7, 22, 13, 16, 19, 10, 1,
              5, 6, 23, 12, 15, 20, 9,  2},                                   MPKIND__TWICE_REMOVED,0,3,  s4x6,   s2x4,      0x015, 1, MAPCODE(s2x4,3,MPKIND__TWICE_REMOVED,1)},

   {{4, 7, 22, 13, 15, 20, 17, 18, 11, 0, 2, 9,
     5, 6, 23, 12, 14, 21, 16, 19, 10, 1, 3, 8},                              MPKIND__REMOVED,     0, 2,  s4x6,   s3x4,      0x005, 0, MAPCODE(s3x4,2,MPKIND__REMOVED,1)},
   {{7, 22, 15, 20, 18, 11, 2, 9,
     6, 23, 14, 21, 19, 10, 3, 8},                                            MPKIND__REMOVED,     0, 2,  s4x6, s_qtag,      0x005, 0, MAPCODE(s_qtag,2,MPKIND__REMOVED,1)},

   {{1, 7, 5, 3,                 (veryshort) s1x2, 0, 6, 2, 4},               MPKIND__SPEC_TWICEREM,0,3,  s_ptpd, s2x2,      0x000, 0, MAPCODE(s2x2,3,MPKIND__SPEC_TWICEREM,0)},
   {{6, 7, 2, 3,                 (veryshort) s1x2, 5, 4, 0, 1},               MPKIND__SPEC_TWICEREM,0,3,  s_qtag, s1x4,      0x000, 1, MAPCODE(s1x4,3,MPKIND__SPEC_TWICEREM,1)},

   {{2, 7, 5, 0,                       6, 3, 1, 4},                           MPKIND__REMOVED,     0, 2,  s_bone, s_trngl4,  0x207, 0, MAPCODE(s_trngl4,2,MPKIND__REMOVED, 1)},
   {{0, 1,     2, 3,        4, 9,      8, 7,       6, 5},                     MPKIND__SPLIT,       0, 5,  s1x10,  s1x2,      0x000, 0, MAPCODE(s1x2,5,MPKIND__SPLIT,       0)},
   {{0, 1,     2, 3,        4, 5,      11, 10,     9, 8,     7, 6},           MPKIND__SPLIT,       0, 6,  s1x12,  s1x2,      0x000, 0, MAPCODE(s1x2,6,MPKIND__SPLIT,       0)},
   {{0, 9,     1, 8,        2, 7,      3, 6,       4, 5},                     MPKIND__SPLIT,       0, 5,  s2x5,   s1x2,      0x155, 0, MAPCODE(s1x2,5,MPKIND__SPLIT,       1)},
   {{0, 11,    1, 10,       2, 9,      3, 8,       4, 7,     5, 6},           MPKIND__SPLIT,       0, 6,  s2x6,   s1x2,      0x555, 0, MAPCODE(s1x2,6,MPKIND__SPLIT,       1)},
   {{0, 9, 14, 15,          1, 8, 13, 16,          2, 7, 12, 17,
                            3, 6, 11, 18,          4, 5, 10, 19},             MPKIND__SPLIT,       0, 5,  s4x5,   s1x4,      0x155, 0, MAPCODE(s1x4,5,MPKIND__SPLIT,       1)},

   {{0}, MPKIND__NONE, 0, 0,  nothing}};



/* BEWARE!!  This list is keyed to the definition of "setup_kind" in database.h. */
map_thing *split_lists[][6] = {
   {0,             0,             0,             0,             0,             0},          /* nothing */
   {&map_1x2_1x1,  &map_1x2_1x1,  &map_1x3_1x1,  &map_1x3_1x1,  0,             0},          /* s1x1 */
   {&map_1x4_1x2,  &map_2x2h,     &map_1x6_1x2,  &map_2x3_1x2,  0,             0},          /* s1x2 */
   {&map_1x6_1x3,  &map_2x3_1x3,  0,             0,             &map_1x12_1x3, &map_3x4_1x3}, /* s1x3 */
   {&map_2x4_2x2,  &map_2x4_2x2r, &map_2x6_2x2,  &map_2x6_2x2r, &map_2x8_2x2,  0},          /* s2x2 */
   {&map_1x8_1x4,  &map_2x4_1x4,  &map_1x12_1x4, &map_3x4_1x4,  &map_1x16_1x4, &map_4x4_1x4},  /* s1x4 */
   {&map_ptp_dmd,  &map_qtg_dmd,  &map_3ptp_dmd, &map_3dmd_dmd, &map_4ptp_dmd, &map_4dmd_dmd},  /* sdmd */
   {&map_2stars,   0,             0,             0,             0,             0},          /* s_star */
   {0,             &map_s6_trngl, 0,             0,             0,             0},          /* s_trngl */
   {&map_phan_trngl4b, &map_rig_trngl4, 0,       0,             0,             0},          /* s_trngl4 */
   {&map_bone_rig, 0,             0,             0,             0,             0},          /* s_bone6 */
   {&map_short6_2, 0,             0,             0,             0,             0},          /* s_short6 */
   {&map_1x12_1x6, &map_2x6_1x6,  0,             &map_3x6_1x6,  0,             &map_4x6_1x6}, /* s1x6 */
   {&map_2x6_2x3,  &map_3x4_2x3,  0,             &map_3x6_2x3,  0,             0},          /* s2x3 */
   {&map_bone_12d, &map_bigd_12d, 0,             0,             0,             0},          /* s_1x2dmd */
   {0,             0,             0,             0,             0,             0},          /* s_2x1dmd */
   {&map_hv_qtg_2, &map_vv_qtg_2, 0,             0,             0,             0},          /* s_qtag */
   {0,             0,             0,             0,             0,             0},          /* s_bone */
   {&map_1x16_1x8, &map_2x8_1x8,  0,             &map_3x8_1x8,  0,             0},          /* s1x8 */
   {0,             0,             0,             0,             0,             0},          /* slittlestars */
   {0,             0,             0,             0,             0,             0},          /* s_2stars */
   {0,             0,             0,             0,             0,             0},          /* s1x3dmd */
   {0,             0,             0,             0,             0,             0},          /* s3x1dmd */
   {0,             &map_spin_3x4, 0,             0,             0,             0},          /* s_spindle */
   {&map_hrgl_ptp, 0,             0,             0,             0,             0},          /* s_hrglass */
   {0,             0,             0,             0,             0,             0},          /* s_dhrglass */
   {0,             0,             0,             0,             0,             0},          /* s_hyperglass */
   {0,             0,             0,             0,             0,             0},          /* s_crosswave */
   {&map_hv_2x4_2, &map_split_f,  &map_2x12_2x4, &map_4x6_2x4,  0,             0},          /* s2x4 */
   {0,             0,             0,             0,             0,             0},          /* s2x5 */
   {0,             &map_par_rig,  0,             0,             0,             0},          /* s_rigger */
   {&map_3x8_3x4,  &map_4x6_3x4,  0,             0,             0,             0},          /* s3x4 */
   {0,             &map_4x6_2x6,  0,             0,             0,             0}};         /* s2x6 */
