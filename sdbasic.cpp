/* SD -- square dance caller's helper.

    Copyright (C) 1990-2002  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 34. */

/* This defines the following functions:
   collision_collector::install_with_collision
   collision_collector::fix_possible_collision
   mirror_this
   do_stability
   check_restriction
   basic_move
*/

#ifdef WIN32
#define SDLIB_API __declspec(dllexport)
#else
#define SDLIB_API
#endif

#include "sd.h"


/* This file uses a few bogus setups.  They are never allowed to escape:
   s8x8 =
     0   1   2   3  28  24  20  16
     4   5   6   7  29  25  21  17
     8   9  10  11  30  26  22  18
    12  13  14  15  31  27  23  19
    51  55  59  63  47  46  45  44
    50  54  58  62  43  42  41  40
    49  53  57  61  39  38  37  36
    48  52  56  60  35  34  33  32

   sx4dmd =
      4        8       15
   7     5     9    14    12
      6       10       13
              11
   0  1  2  3    19 18 17 16
              27
     29       26       22
  28    30    25    21    23
     31       24       20


   shypergal =

              4 5

               6
      1       3 7      8
           2      10
      0      15 11     9
               14

             13 12


   sx1x6 =
            3
            4
            5
   0  1  2     8  7  6
           11
           10
            9

   sx1x16 =
                     8
                     9
                    ...
                    14
                    15
   0  1  ....  6  7     23 22 ... 17 16
                    31
                    30
                    ...
                    25
                    24
*/





typedef struct {
   int size;                 /* how many people in the maps */
   uint32 lmask;             /* which people are facing E-W in original double-length setup */
   uint32 rmask;             /* where in original setup can people be found */
   uint32 cmask;             /* where in original setup have people collided */
   veryshort source[12];     /* where to get the people */
   veryshort map0[12];       /* where to put the north (or east)-facer */
   veryshort map1[12];       /* where to put the south (or west)-facer */
   setup_kind initial_kind;  /* what setup they are collided in */
   setup_kind final_kind;    /* what setup to change it to */
   int rot;                  /* whether to rotate final setup CW */
   warning_index warning;    /* an optional warning to give */
   int assume_key;           /* special stuff for checking assumptions; sign bit -> dangerous */
} collision_map;

static collision_map collision_map_table[] = {
   /* These items handle various types of "1/2 circulate" calls from 2x4's. */

   {4, 0x000000, 0x33, 0x33, {0, 1, 4, 5},         {0, 3, 5, 6},          {1, 2, 4, 7},
    s_crosswave, s1x8,        0, warn__none, 0},   // from lines out
   {2, 0x000000, 0x11, 0x11, {0, 4},               {0, 5},                {1, 4},
    s_crosswave, s1x8,        0, warn__none, 0},   // from lines out, only ends exist
   {2, 0x000000, 0x22, 0x22, {1, 5},               {3, 6},                {2, 7},
    s_crosswave, s1x8,        0, warn__none, 3},   // from lines out, only centers exist
   {2, 0x000000, 0x30, 0x30, {4, 5},               {5, 6},                {4, 7},
    s_crosswave, s1x8,        0, warn__none, 0},   // more of same
   {2, 0x000000, 0x03, 0x03, {0, 1},               {0, 3},                {1, 2},
    s_crosswave, s1x8,        0, warn__none, 0},

   {4, 0x0CC0CC, 0xCC, 0xCC, {2, 3, 6, 7},         {0, 3, 5, 6},          {1, 2, 4, 7},
    s_crosswave, s1x8,        1, warn__none, 0},   // from lines in
   {2, 0x044044, 0x44, 0x44, {2, 6},               {0, 5},                {1, 4},
    s_crosswave, s1x8,        1, warn__none, 0},   // from lines in, only ends exist
   {2, 0x088088, 0x88, 0x88, {3, 7},               {3, 6},                {2, 7},
    s_crosswave, s1x8,        1, warn__none, 2},   // from lines in, only centers exist
   {2, 0x0C00C0, 0xC0, 0xC0, {6, 7},               {5, 6},                {4, 7},
    s_crosswave, s1x8,        1, warn__none, 0},   // more of same
   {2, 0x00C00C, 0x0C, 0x0C, {2, 3},               {0, 3},                {1, 2},
    s_crosswave, s1x8,        1, warn__none, 0},

   {4, 0x000000, 0x0F, 0x0F, {0, 1, 2, 3},         {0, 3, 5, 6},          {1, 2, 4, 7},
    s1x4,        s1x8,        0, warn__none, 0},   // more of same

   {4, 0x022022, 0xAA, 0xAA, {1, 3, 5, 7},         {2, 5, 7, 0},          {3, 4, 6, 1},           s_spindle,   s_crosswave, 0, warn__none, 0},   // from trade by
   {2, 0x022022, 0x22, 0x22, {1, 5},               {2, 7},                {3, 6},                 s_spindle,   s_crosswave, 0, warn__none, 1},   // from trade by with no ends
   {2, 0x000000, 0x88, 0x88, {3, 7},               {5, 0},                {4, 1},                 s_spindle,   s_crosswave, 0, warn__none, 0},   // from trade by with no centers
   {3, 0x000022, 0x2A, 0x08, {1, 3, 5},            {3, 5, 7},             {3, 4, 7},              s_spindle,   s_crosswave, 0, warn__none, 0},   // from trade by with no left half
   {3, 0x000022, 0xA2, 0x80, {1, 5, 7},            {3, 7, 0},             {3, 7, 1},              s_spindle,   s_crosswave, 0, warn__none, 0},   // from trade by with no right half

   {6, 0x0880CC, 0xDD, 0x88, {0, 2, 3, 4, 6, 7},   {7, 0, 1, 3, 4, 6},    {7, 0, 2, 3, 4, 5},     s_crosswave, s3x1dmd,     1, warn__none, 0},   /* from 3&1 lines w/ centers in */
   {6, 0x000044, 0x77, 0x22, {0, 1, 2, 4, 5, 6},   {0, 1, 3, 4, 6, 7},    {0, 2, 3, 4, 5, 7},     s_crosswave, s3x1dmd,     0, warn__none, 0},   /* from 3&1 lines w/ centers out */
   {6, 0x0440CC, 0xEE, 0x44, {1, 2, 3, 5, 6, 7},   {7, 0, 2, 3, 5, 6},    {7, 1, 2, 3, 4, 6},     s_crosswave, s3x1dmd,     1, warn__none, 0},   /* from 3&1 lines w/ ends in */
   {6, 0x000088, 0xBB, 0x11, {0, 1, 3, 4, 5, 7},   {0, 2, 3, 5, 6, 7},    {1, 2, 3, 4, 6, 7},     s_crosswave, s1x3dmd,     0, warn__none, 0},   /* from 3&1 lines w/ ends out */
   {4, 0x088088, 0x99, 0x99, {0, 3, 4, 7},         {0, 2, 5, 7},          {1, 3, 4, 6},           s_crosswave, s_crosswave, 0, warn__none, 0},   // from inverted lines w/ centers in
   {4, 0x044044, 0x66, 0x66, {1, 2, 5, 6},         {6, 0, 3, 5},          {7, 1, 2, 4},           s_crosswave, s_crosswave, 1, warn__ctrs_stay_in_ctr, 0}, // from inverted lines w/ centers out

   // Collision after circulate from lines all facing same way.
   {4, 0x000000, 0x0F, 0x0F,  {0, 1, 2, 3},         {0, 2, 4, 6},          {1, 3, 5, 7},           s2x4,        s2x8,        0, warn__none, 0},
   {4, 0x000000, 0xF0, 0xF0,  {4, 5, 6, 7},         {9, 11, 13, 15},       {8, 10, 12, 14},        s2x4,        s2x8,        0, warn__none, 0},

   // This one is marked as dangerous, meaning it can't be used from
   // merge_setups, but can be used when normalizing the result of basic_move.

   {6, 0x044044, 0x77, 0x44, {0, 1, 2, 4, 5, 6},   {0, 1, 2, 4, 5, 7},   {0, 1, 3, 4, 5, 6},           s_crosswave, s_crosswave, 0, warn__ctrs_stay_in_ctr, 0x80000000}, /* from inverted lines w/ centers out */


   {4, 0x044044, 0x55, 0x55, {0, 2, 4, 6},         {0, 2, 5, 7},          {1, 3, 4, 6},           s_crosswave, s_crosswave, 0, warn__ctrs_stay_in_ctr, 0}, /* from trade-by w/ ctrs 1/4 out */
   /* This was put in so that "1/2 circulate" would work from lines in with centers counter rotated. */
   {4, 0x088088, 0xAA, 0xAA, {1, 3, 5, 7},         {0, 2, 5, 7},          {1, 3, 4, 6},           s_crosswave, s_crosswave, 0, warn__ctrs_stay_in_ctr, 0}, /* from lines in and centers face */
   {6, 0x000082, 0xAAA, 0x820, {1, 3, 5, 7, 9, 11},{3, 4, 6, 7, 0, 1},    {3, 4, 5, 7, 0, 2},     s3dmd,       s3x1dmd,     0, warn__none, 0},   /* from trade by with some outside quartered out */
   {6, 0x000088, 0x0DD, 0x044, {3, 4, 6, 7, 0, 2}, {3, 4, 6, 7, 0, 1},    {3, 4, 5, 7, 0, 2},     s3x1dmd,     s3x1dmd,     0, warn__none, 0},   /* Same. */
   {6, 0x088088, 0xBB, 0x88, {0, 1, 3, 4, 5, 7},   {0, 1, 2, 4, 5, 7},    {0, 1, 3, 4, 5, 6},     s_crosswave, s_crosswave, 0, warn__none, 0},
   {6, 0x0000CC, 0xDD, 0x11, {0, 2, 3, 4, 6, 7},   {0, 2, 3, 5, 6, 7},    {1, 2, 3, 4, 6, 7},     s_crosswave, s_crosswave, 0, warn__none, 0},
   {6, 0x0000CC, 0xEE, 0x22, {1, 2, 3, 5, 6, 7},   {0, 2, 3, 5, 6, 7},    {1, 2, 3, 4, 6, 7},     s_crosswave, s_crosswave, 0, warn__none, 0},
   {6, 0x000000,  077, 011,  {0, 1, 2, 3, 4, 5},   {0, 3, 2, 5, 7, 6},    {1, 3, 2, 4, 7, 6},     s1x6,        s1x8,        0, warn__none, 0},
   {6, 0x000000,  077, 022,  {0, 1, 2, 3, 4, 5},   {0, 1, 2, 4, 7, 6},    {0, 3, 2, 4, 5, 6},     s1x6,        s1x8,        0, warn__none, 0},
   {6, 0x000000,  077, 044,  {0, 1, 2, 3, 4, 5},   {0, 1, 3, 4, 5, 6},    {0, 1, 2, 4, 5, 7},     s1x6,        s1x8,        0, warn__none, 0},
   {4, 0x000000,  055, 055,  {0, 2, 3, 5},         {0, 3, 5, 6},          {1, 2, 4, 7},           s1x6,        s1x8,        0, warn__none, 0},

   /* These items handle parallel lines with people wedged on one end, and hence handle flip or cut the hourglass. */
   {6, 0x000000, 0x77, 0x11, {0, 1, 2, 4, 5, 6},   {0, 2, 3, 7, 8, 9},    {1, 2, 3, 6, 8, 9},     s2x4,        s2x6,        0, warn__none, 0},
   {6, 0x000000, 0xEE, 0x88, {1, 2, 3, 5, 6, 7},   {2, 3, 4, 8, 9, 11},   {2, 3, 5, 8, 9, 10},    s2x4,        s2x6,        0, warn__none, 0},
   /* These items handle single lines with people wedged on one end, and hence handle flip or cut the diamond. */
   {3, 0x000000, 0x0B, 0x01, {0, 1, 3},            {0, 2, 5},             {1, 2, 5},              s1x4,        s1x6,        0, warn__none, 0},
   {3, 0x000000, 0x0E, 0x04, {1, 2, 3},            {2, 4, 5},             {2, 3, 5},              s1x4,        s1x6,        0, warn__none, 0},
   /* These items handle single diamonds with people wedged on one end, and hence handle diamond circulate. */
   {3, 0x00000A, 0x0E, 0x04, {1, 2, 3},            {2, 4, 5},             {2, 3, 5},              sdmd,    s_1x2dmd,        0, warn__none, 0},
   {3, 0x00000A, 0x0B, 0x01, {0, 1, 3},            {0, 2, 5},             {1, 2, 5},              sdmd,    s_1x2dmd,        0, warn__none, 0},
   /* These items handle columns with people wedged everywhere, and hence handle unwraps of facing diamonds etc. */
   {4, 0x055055, 0x55, 0x55, {0, 2, 4, 6},         {12, 14, 2, 11},       {10, 3, 4, 6},          s2x4,        s4x4,        0, warn__none, 0},
   {4, 0x0AA0AA, 0xAA, 0xAA, {1, 3, 5, 7},         {13, 0, 7, 9},         {15, 1, 5, 8},          s2x4,        s4x4,        0, warn__none, 0},
   /* These items handle columns with people wedged in clumps, and hence handle gravitate from lefties etc. */
   {4, 0x033033, 0x33, 0x33, {0, 1, 4, 5},         {12, 13, 2, 7},        {10, 15, 4, 5},         s2x4,        s4x4,        0, warn__none, 0},
   {4, 0x0CC0CC, 0xCC, 0xCC, {2, 3, 6, 7},         {14, 0, 11, 9},        {3, 1, 6, 8},           s2x4,        s4x4,        0, warn__none, 0},
   /* Collision on ends of an "erase". */
   {1, 0x000000, 0x02, 0x02, {1},                  {3},                   {2},                    s1x2,        s1x4,        0, warn__none, 0},
   {1, 0x000000, 0x01, 0x01, {0},                  {0},                   {1},                    s1x2,        s1x4,        0, warn__none, 0},

   /* These items handle the situation from a 2FL/intlkd box circ/split box trade circ. */
/* Not sure what that comment meant, but we are trying to handle colliding 2FL-type circulates */
   {2, 0x000000, 0x03, 0x03, {0, 1},               {0, 2},                {1, 3},                 s2x4,        s2x8,        0, warn__none, 0},
   {2, 0x000000, 0x0C, 0x0C, {2, 3},               {4, 6},                {5, 7},                 s2x4,        s2x8,        0, warn__none, 0},
   {2, 0x000000, 0x30, 0x30, {5, 4},               {11, 9},               {10, 8},                s2x4,        s2x8,        0, warn__none, 0},
   {2, 0x000000, 0xC0, 0xC0, {7, 6},               {15, 13},              {14, 12},               s2x4,        s2x8,        0, warn__none, 0},
/* The preceding lines used to be these, which seems quite wrong:
   {2, 0x000000, 0x03, 0x03, {0, 1},               {0, 2},                {1, 3},                 s2x4,        s2x4,        0, warn__none, 0},
   {2, 0x000000, 0x0C, 0x0C, {2, 3},               {0, 2},                {1, 3},                 s2x4,        s2x4,        0, warn__none, 0},
   {2, 0x000000, 0x30, 0x30, {5, 4},               {7, 5},                {6, 4},                 s2x4,        s2x4,        0, warn__none, 0},
   {2, 0x000000, 0xC0, 0xC0, {7, 6},               {7, 5},                {6, 4},                 s2x4,        s2x4,        0, warn__none, 0},
*/

                  /* The warning "warn_bad_collision" in the warning field is special -- it means give that warning if it appears illegal.
                           If it doesn't appear illegal, don't say anything at all (other than the usual "take right hands".)
                           If anything appears illegal but does not have "warn_bad_collision" in this field, it is an ERROR. */
   {4, 0x000000, 0xAA, 0x0AA, {1, 3, 5, 7},        {2, 6, 11, 15},        {3, 7, 10, 14},         s2x4,        s2x8,        0, warn_bad_collision, 0},
   {4, 0x000000, 0x55, 0x055, {0, 2, 4, 6},        {0, 4, 9, 13},         {1, 5, 8, 12},          s2x4,        s2x8,        0, warn_bad_collision, 0},
   {4, 0x000000, 0x33, 0x033, {0, 1, 4, 5},        {0, 2, 9, 11},         {1, 3, 8, 10},          s2x4,        s2x8,        0, warn_bad_collision, 0},
   {4, 0x000000, 0xCC, 0x0CC, {2, 3, 6, 7},        {4, 6, 13, 15},        {5, 7, 12, 14},         s2x4,        s2x8,        0, warn_bad_collision, 0},

   /* These items handle various types of "1/2 circulate" calls from 2x2's. */
   {2, 0x000000, 0x05, 0x05, {0, 2},               {0, 3},                {1, 2},                 sdmd,        s1x4,        0, warn__none, 0},   /* from couples out if it went to diamond */
   {1, 0x000000, 0x01, 0x01, {0},                  {0},                   {1},                    sdmd,        s1x4,        0, warn__none, 0},   /* same, but with missing people */
   {1, 0x000000, 0x04, 0x04, {2},                  {3},                   {2},                    sdmd,        s1x4,        0, warn__none, 0},   /* same, but with missing people */
   {2, 0x000000, 0x05, 0x05, {0, 2},               {0, 3},                {1, 2},                 s1x4,        s1x4,        0, warn__none, 0},   /* from couples out if it went to line */
   {1, 0x000000, 0x01, 0x01, {0},                  {0},                   {1},                    s1x4,        s1x4,        0, warn__none, 0},   /* same, but with missing people */
   {1, 0x000000, 0x04, 0x04, {2},                  {3},                   {2},                    s1x4,        s1x4,        0, warn__none, 0},   /* same, but with missing people */
   {2, 0x00A00A, 0x0A, 0x0A, {1, 3},               {0, 3},                {1, 2},                 sdmd,        s1x4,        1, warn__none, 0},   /* from couples in if it went to diamond */
   {2, 0x000000, 0x0A, 0x0A, {1, 3},               {0, 3},                {1, 2},                 s1x4,        s1x4,        0, warn__none, 0},   /* from couples in if it went to line */
   {2, 0x000000, 0x06, 0x06, {1, 2},               {0, 3},                {1, 2},                 s1x4,        s1x4,        0, warn__none, 0},   /* from "head pass thru, all split circulate" */
   {2, 0x000000, 0x09, 0x09, {0, 3},               {0, 3},                {1, 2},                 s1x4,        s1x4,        0, warn__none, 0},   /* from "head pass thru, all split circulate" */
   {3, 0x000000, 0x07, 0x04, {0, 1, 2},            {0, 1, 3},             {0, 1, 2},              s1x4,        s1x4,        0, warn__none, 0},   /* from nasty T-bone */
   {3, 0x000000, 0x0D, 0x01, {0, 2, 3},            {0, 2, 3},             {1, 2, 3},              s1x4,        s1x4,        0, warn__none, 0},   /* from nasty T-bone */
   /* These items handle "1/2 split trade circulate" from 2x2's.
      They also do "switch to a diamond" when the ends come to the same spot in the center. */
   {3, 0x008008, 0x0D, 0x08, {0, 2, 3},            {0, 2, 1},             {0, 2, 3},              sdmd,        sdmd,        0, warn_bad_collision, 0},
   {3, 0x002002, 0x07, 0x02, {0, 2, 1},            {0, 2, 1},             {0, 2, 3},              sdmd,        sdmd,        0, warn_bad_collision, 0},

   {1, 0x008008, 0x08, 0x08, {3},            {3},             {2},                                sdmd,        s1x4,        1, warn_bad_collision, 0},
   {1, 0x002002, 0x02, 0x02, {1},            {0},             {1},                                sdmd,        s1x4,        1, warn_bad_collision, 0},

   /* These items handle various types of "circulate" calls from 2x2's. */
   {2, 0x009009, 0x09, 0x09, {0, 3},               {7, 5},                {6, 4},                 s2x2,        s2x4,        1, warn_bad_collision, 0},   /* from box facing all one way */
   {2, 0x006006, 0x06, 0x06, {1, 2},               {0, 2},                {1, 3},                 s2x2,        s2x4,        1, warn_bad_collision, 0},   /* we need all four cases */
   {2, 0x000000, 0x0C, 0x0C, {3, 2},               {7, 5},                {6, 4},                 s2x2,        s2x4,        0, warn_bad_collision, 0},   /* sigh */
   {2, 0x000000, 0x03, 0x03, {0, 1},               {0, 2},                {1, 3},                 s2x2,        s2x4,        0, warn_bad_collision, 0},   /* sigh */
   {2, 0x000000, 0x09, 0x09, {0, 3},               {0, 7},                {1, 6},                 s2x2,        s2x4,        0, warn_bad_collision, 0},   /* from "inverted" box */
   {2, 0x000000, 0x06, 0x06, {1, 2},               {2, 5},                {3, 4},                 s2x2,        s2x4,        0, warn_bad_collision, 0},   /* we need all four cases */
   {2, 0x003003, 0x03, 0x03, {1, 0},               {0, 7},                {1, 6},                 s2x2,        s2x4,        1, warn_bad_collision, 0},   /* sigh */
   {2, 0x00C00C, 0x0C, 0x0C, {2, 3},               {2, 5},                {3, 4},                 s2x2,        s2x4,        1, warn_bad_collision, 0},   /* sigh */

   // This handles circulate from a starting DPT.
   {4, 0x066066, 0x66, 0x66, {1, 2, 5, 6},         {7, 0, 2, 5},          {6, 1, 3, 4},
    s2x4,        s2x4,        1, warn_bad_collision, 0},

   // These items handle horrible lockit collisions in the middle
   // (from inverted lines, for example).
   {2, 0x000000, 0x06, 0x06, {1, 2},               {3, 5},                {2, 4},
    s1x4,        s1x8,        0, warn_bad_collision, 0},
   {2, 0x000000, 0x09, 0x09, {0, 3},               {0, 6},                {1, 7},
    s1x4,        s1x8,        0, warn_bad_collision, 0},
   {2, 0x000000, 0x03, 0x03, {0, 1},               {0, 3},                {1, 2},
    s1x4,        s1x8,        0, warn_bad_collision, 0},
   {2, 0x000000, 0x0C, 0x0C, {3, 2},               {6, 5},                {7, 4},
    s1x4,        s1x8,        0, warn_bad_collision, 0},

   {6, 0x000033, 0xBB, 0x88, {0, 1, 3, 4, 5, 7},   {0, 1, 3, 4, 5, 6},    {0, 1, 2, 4, 5, 7},
    s_qtag,      s_qtag,      0, warn_bad_collision, 0},

   // These items handle circulate in a short6, and hence handle collisions in 6X2 acey deucey.

   /* Here are the old items:
   {4, 0x12, 0x1B, 0x09, {0, 1, 3, 4},             {0, 2, 7, 8},          {1, 2, 6, 8},
    s_short6,    sbigdmd,     0, warn__none, 0},
   {4, 0x12, 0x36, 0x24, {1, 2, 4, 5},             {2, 4, 8, 11},         {2, 5, 8, 10},
    s_short6,    sbigdmd,     0, warn__none, 0},
   */
   /* And here are the new ones: */
   {4, 0x12, 0x1B, 0x09, {0, 1, 3, 4},             {0, 2, 6, 7},          {1, 2, 5, 7},
    s_short6,    sdeep2x1dmd, 0, warn__none, 0},
   {4, 0x12, 0x36, 0x24, {1, 2, 4, 5},             {2, 3, 7, 9},          {2, 4, 7, 8},
    s_short6,    sdeep2x1dmd, 0, warn__none, 0},



   {4, 0x012012, 0x36, 0x12, {1, 2, 4, 5},         {6, 0, 3, 4},          {7, 0, 2, 4},
    s_short6,    s_rigger,    1, warn__none, 0},
   {4, 0x012012, 0x1B, 0x12, {1, 0, 4, 3},         {6, 5, 3, 1},          {7, 5, 2, 1},
    s_short6,    s_rigger,    1, warn__none, 0},

   // These 4 items handle more 2x2 stuff, including the "special drop in"
   // that makes chain reaction/motivate etc. work.
   {2, 0x005005, 0x05, 0x05, {0, 2},               {7, 2},                {6, 3},                 s2x2,        s2x4,        1, warn__none, 0},
   {2, 0x00A00A, 0x0A, 0x0A, {1, 3},               {0, 5},                {1, 4},                 s2x2,        s2x4,        1, warn__none, 0},
   {2, 0x000000, 0x05, 0x05, {0, 2},               {0, 5},                {1, 4},                 s2x2,        s2x4,        0, warn__none, 0},
   {2, 0x000000, 0x0A, 0x0A, {1, 3},               {2, 7},                {3, 6},                 s2x2,        s2x4,        0, warn__none, 0},
   // Same, but with missing people.
   {1, 0x004004, 0x04, 0x04, {2},                  {2},                   {3},                    s2x2,        s2x4,        1, warn__none, 0},
   {1, 0x001001, 0x01, 0x01, {0},                  {7},                   {6},                    s2x2,        s2x4,        1, warn__none, 0},
   {1, 0x008008, 0x08, 0x08, {3},                  {5},                   {4},                    s2x2,        s2x4,        1, warn__none, 0},
   {1, 0x002002, 0x02, 0x02, {1},                  {0},                   {1},                    s2x2,        s2x4,        1, warn__none, 0},
   {1, 0x000000, 0x04, 0x01, {2},                  {5},                   {4},                    s2x2,        s2x4,        0, warn__none, 0},
   {1, 0x000000, 0x01, 0x01, {0},                  {0},                   {1},                    s2x2,        s2x4,        0, warn__none, 0},
   {1, 0x000000, 0x08, 0x08, {3},                  {7},                   {6},                    s2x2,        s2x4,        0, warn__none, 0},
   {1, 0x000000, 0x02, 0x02, {1},                  {2},                   {3},                    s2x2,        s2x4,        0, warn__none, 0},

   // Same spot at ends of bone (first choice from 2FL.)
   {6, 0x000000, 0xEE, 0x22, {1, 2, 3, 5, 6, 7},   {4, 8, 9, 11, 2, 3},   {5, 8, 9, 10, 2, 3},    s_bone,      sbigbone,     0, warn__none, 0},
   {6, 0x000000, 0xDD, 0x11, {0, 2, 3, 4, 6, 7},   {0, 8, 9, 7, 2, 3},    {1, 8, 9, 6, 2, 3},     s_bone,      sbigbone,     0, warn__none, 0},

   // Same spot as points of diamonds.
   {6, 0x022022, 0xEE, 0x22, {1, 2, 3, 5, 6, 7},   {0, 2, 3, 7, 8, 9},    {1, 2, 3, 6, 8, 9},     s_qtag,      sbigdmd,     1, warn__none, 0},
   {6, 0x011011, 0xDD, 0x11, {0, 2, 3, 4, 6, 7},   {11, 2, 3, 4, 8, 9},   {10, 2, 3, 5, 8, 9},    s_qtag,      sbigdmd,     1, warn__none, 0},
   /* Same spot as points of hourglass. */
   {6, 0x0220AA, 0xEE, 0x22, {1, 2, 3, 5, 6, 7},   {0, 2, 9, 7, 8, 3},    {1, 2, 9, 6, 8, 3},     s_hrglass,   sbighrgl,    1, warn__none, 0},
   {6, 0x011099, 0xDD, 0x11, {0, 2, 3, 4, 6, 7},   {11, 2, 9, 4, 8, 3},   {10, 2, 9, 5, 8, 3},    s_hrglass,   sbighrgl,    1, warn__none, 0},
   {-1}};


void collision_collector::install_with_collision(
   setup *result, int resultplace,
   const setup *sourcepeople, int sourceplace,
   int rot) THROW_DECL
{
   if (resultplace < 0) fail("This would go into an excessively large matrix.");
   result_mask |= 1<<resultplace;
   int destination = resultplace;

   if (result->people[resultplace].id1) {
      // We have a collision.
      destination += 12;
      // Prepare the error message, in case it is needed.
      collision_person1 = result->people[resultplace].id1;
      collision_person2 = sourcepeople->people[sourceplace].id1;
      error_message1[0] = '\0';
      error_message2[0] = '\0';

      if (!allow_collisions ||
          setup_attrs[result->kind].setup_limits >= 12 ||
          result->people[destination].id1 != 0)
         throw error_flag_type(error_flag_collision);
      // Collisions are legal.  Store the person in the overflow area
      // (12 higher than the main area, which is why we only permit
      // this if the result setup size is <= 12) and record the fact
      // in the collision_mask so we can figure out what to do.
      collision_mask |= (1 << resultplace);
      collision_index = resultplace;   // In case we need to report a mundane collision.

      // Under certain circumstances (if callflags1 was manually overridden),
      // we need to keep track of who collides, and set things appropriately.
      if ((callflags1 & CFLAG1_TAKE_RIGHT_HANDS) ||
          ((callflags1 & CFLAG1_ENDS_TAKE_RIGHT_HANDS) &&
           (((result->kind == s1x4 || result->kind == sdmd) && !(resultplace&1)) ||
            (result->kind == s2x4 && !((resultplace+1)&2)) ||
            (result->kind == s_qtag && !(resultplace&2))))) {
         // Collisions are legal.
      }
      else
         collision_appears_illegal = 2;
   }

   (void) copy_rot(result, destination, sourcepeople, sourceplace, rot);
}



void collision_collector::fix_possible_collision(setup *result) THROW_DECL
{
   if (collision_mask) {
      int i;
      setup spare_setup = *result;
      bool kill_ends = false;
      bool controversial = false;
      uint32 lowbitmask = 0;
      collision_map *c_map_ptr;

      clear_people(result);

      for (i=0; i<MAX_PEOPLE; i++) lowbitmask |= ((spare_setup.people[i].id1) & 1) << i;

      for (c_map_ptr = collision_map_table ; c_map_ptr->size >= 0 ; c_map_ptr++) {
         if ((result->kind == c_map_ptr->initial_kind) &&
             ((lowbitmask == c_map_ptr->lmask)) &&
             (result_mask == c_map_ptr->rmask) &&
             (collision_mask == c_map_ptr->cmask)) {

            if (assume_ptr) {
               switch (c_map_ptr->assume_key & 0xFFFF) {
               case 1:
                  if (assume_ptr->assumption != cr_li_lo ||
                      assume_ptr->assump_col != 1 ||
                      assume_ptr->assump_both != 2)
                     kill_ends = true;
                  break;
               case 2:
                  if (assume_ptr->assumption != cr_li_lo ||
                      assume_ptr->assump_col != 0 ||
                      assume_ptr->assump_both != 1)
                     kill_ends = true;
                  break;
               case 3:
                  if (assume_ptr->assumption != cr_li_lo ||
                      assume_ptr->assump_col != 0 ||
                      assume_ptr->assump_both != 2)
                     kill_ends = true;
                  break;
               }
            }

            if (collision_appears_illegal == 2 ||
                (collision_appears_illegal == 1 && (c_map_ptr->assume_key & 0x80000000)))
               controversial = true;

            if (!controversial || c_map_ptr->warning == warn_bad_collision)
               goto win;
         }
      }

      // Don't recognize the pattern, report this as normal collision.
      throw error_flag_type(error_flag_collision);

   win:

      if (m_doing_half_override) {
         if (cmd_misc_flags & CMD_MISC__EXPLICIT_MIRROR)
            warn(warn__take_right_hands);
         else
            warn(warn__left_half_pass);
      }
      else {
         if (cmd_misc_flags & CMD_MISC__EXPLICIT_MIRROR)
            warn(warn__take_left_hands);
         else
            warn(warn__take_right_hands);
      }

      if (c_map_ptr->warning != warn_bad_collision || controversial)
         warn(c_map_ptr->warning);

      int temprot = ((-c_map_ptr->rot) & 3) * 011;
      result->kind = c_map_ptr->final_kind;
      result->rotation += c_map_ptr->rot;

      // If this is under an implicit mirror image operation,
      // make them take left hands, by swapping the maps.

      uint32 flip = m_force_mirror_warn ? 2 : 0;

      for (i=0; i<c_map_ptr->size; i++) {
         int oldperson;

         oldperson = spare_setup.people[c_map_ptr->source[i]].id1;
         install_rot(result,
                     (((oldperson ^ flip) & 2) ? c_map_ptr->map1 : c_map_ptr->map0)[i],
                     &spare_setup,
                     c_map_ptr->source[i],
                     temprot);

         oldperson = spare_setup.people[c_map_ptr->source[i]+12].id1;
         install_rot(result,
                     (((oldperson ^ flip) & 2) ? c_map_ptr->map1 : c_map_ptr->map0)[i],
                     &spare_setup,
                     c_map_ptr->source[i]+12,
                     temprot);
      }

      if (kill_ends) {
         const veryshort m3276[] = {3, 2, 7, 6};
         const veryshort m2367[] = {2, 3, 6, 7};

         // The centers are colliding, but the ends are absent, and we have
         // no assumptions to guide us about where they should go.
         if ((result->kind != s_crosswave && result->kind != s1x8) ||
             (result->people[0].id1 |
              result->people[1].id1 |
              result->people[4].id1 |
              result->people[5].id1))
            fail("Need an assumption in order to take right hands at collision.");

         spare_setup = *result;

         if (result->kind == s_crosswave) {
            gather(result, &spare_setup, m2367, 3, 033);
            result->rotation++;
         }
         else {
            gather(result, &spare_setup, m3276, 3, 0);
         }

         result->kind = s_dead_concentric;
         result->inner.srotation = result->rotation;
         result->inner.skind = s1x4;
         result->rotation = 0;
         result->concsetup_outer_elongation = 0;
      }
   }
}


extern void mirror_this(setup *s) THROW_DECL
{
   uint32 z, n, t;
   int i, x, y, place, limit, doffset;

   setup temp = *s;

   if (s->kind == nothing) return;

   const coordrec *cptr = setup_attrs[s->kind].nice_setup_coords;

   switch (s->kind) {
   case s_ntrglcw:   s->kind = s_ntrglccw; break;
   case s_ntrglccw:  s->kind = s_ntrglcw; break;
   case s_nptrglcw:  s->kind = s_nptrglccw; break;
   case s_nptrglccw: s->kind = s_nptrglcw; break;
   case s_ntrgl6cw:  s->kind = s_ntrgl6ccw; break;
   case s_ntrgl6ccw: s->kind = s_ntrgl6cw; break;
   case s_nxtrglcw:  s->kind = s_nxtrglccw; break;
   case s_nxtrglccw: s->kind = s_nxtrglcw; break;
   case spgdmdcw:    s->kind = spgdmdccw; break;
   case spgdmdccw:   s->kind = spgdmdcw; break;
   }

   const coordrec *optr = setup_attrs[s->kind].nice_setup_coords;

   if (!cptr) {
      if (s->kind == s_trngl4) {
         if (s->rotation & 1) {
            s->rotation += 2;
            for (i=0; i<4; i++) (void) copy_rot(s, i, s, i, 022);
            cptr = &tgl4_1;
         }
         else
            cptr = &tgl4_0;
      }
      else if (s->kind == s_trngl) {
         if (s->rotation & 1) {
            s->rotation += 2;
            for (i=0; i<3; i++) (void) copy_rot(s, i, s, i, 022);
            cptr = &tgl3_1;
         }
         else
            cptr = &tgl3_0;
      }
      else if (s->kind == s_normal_concentric) {
         if (     s->inner.skind == s_normal_concentric ||
                  s->outer.skind == s_normal_concentric ||
                  s->inner.skind == s_dead_concentric ||
                  s->outer.skind == s_dead_concentric)
            fail("Recursive concentric?????.");

         s->kind = s->inner.skind;
         s->rotation = s->inner.srotation;
         mirror_this(s);    /* Sorry! */
         s->inner.srotation = s->rotation;

         s->kind = s->outer.skind;
         s->rotation = s->outer.srotation;
         for (i=0 ; i<12 ; i++) swap_people(s, i, i+12);
         mirror_this(s);    /* Sorrier! */
         for (i=0 ; i<12 ; i++) swap_people(s, i, i+12);
         s->outer.srotation = s->rotation;

         s->kind = s_normal_concentric;
         s->rotation = 0;
         return;
      }
      else if (s->kind == s_dead_concentric) {
         if (s->inner.skind == s_normal_concentric || s->inner.skind == s_dead_concentric)
            fail("Recursive concentric?????.");

         s->kind = s->inner.skind;
         s->rotation += s->inner.srotation;
         mirror_this(s);    /* Sorry! */
         s->inner.srotation = s->rotation;

         s->kind = s_dead_concentric;
         s->rotation = 0;
         return;
      }
      else
         fail("Don't recognize ending setup for this call; not able to do it mirror.");

      optr = cptr;

   }

   limit = setup_attrs[s->kind].setup_limits;

   doffset = 32 - (1 << (cptr->xfactor-1));

   if (s->rotation & 1) {
      for (i=0; i<=limit; i++) {
         x = cptr->xca[i];
         y = - cptr->yca[i];
         place = optr->diagram[doffset - ((y >> 2) << cptr->xfactor) + (x >> 2)];

         if ((place < 0) || (optr->xca[place] != x) || (optr->yca[place] != y))
            fail("Don't recognize ending setup for this call; not able to do it mirror.");

         n = temp.people[i].id1;
         t = (0 - (n & (STABLE_VBIT*3))) & (STABLE_VBIT*3);
         z = (n & ~(STABLE_VBIT*3)) | t;

         // Switch the roll bits.
         z &= ~(3*NROLL_BIT);
         z |= ((n >> 1) & NROLL_BIT) | ((n & NROLL_BIT) << 1);

         s->people[place].id1 = (z & 010) ? (z ^ 2) : z;
         s->people[place].id2 = temp.people[i].id2;
      }
   }
   else {
      for (i=0; i<=limit; i++) {
         x = - cptr->xca[i];
         y = cptr->yca[i];
         place = optr->diagram[doffset - ((y >> 2) << cptr->xfactor) + (x >> 2)];

         if ((place < 0) || (optr->xca[place] != x) || (optr->yca[place] != y))
            fail("Don't recognize ending setup for this call; not able to do it mirror.");

         n = temp.people[i].id1;
         t = (0 - (n & (STABLE_VBIT*3))) & (STABLE_VBIT*3);
         z = (n & ~(STABLE_VBIT*3)) | t;

         // Switch the roll bits.
         z &= ~(3*NROLL_BIT);
         z |= ((n >> 1) & NROLL_BIT) | ((n & NROLL_BIT) << 1);

         s->people[place].id1 = (z & 1) ? (z ^ 2) : z;
         s->people[place].id2 = temp.people[i].id2;
      }
   }
}


static const veryshort identity[24] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
static const veryshort ftc2x4[8] = {10, 15, 3, 1, 2, 7, 11, 9};
static const veryshort ftc4x4[24] = {10, 15, 3, 1, 2, 7, 11, 9, 2, 7, 11, 9,
                               10, 15, 3, 1, 10, 15, 3, 1, 2, 7, 11, 9};
static const veryshort ftcphan[24] = {0, 2, 7, 5, 8, 10, 15, 13, 8, 10, 15, 13,
                                0, 2, 7, 5, 0, 2, 7, 5, 8, 10, 15, 13};
static const veryshort ftl2x4[12] = {6, 11, 15, 13, 14, 3, 7, 5, 6, 11, 15, 13};
static const veryshort ftcspn[8] = {6, 11, 13, 17, 22, 27, 29, 1};
static const veryshort ftcbone[8] = {6, 13, 18, 19, 22, 29, 2, 3};

static const veryshort foobar[8] = {1, 8, 10, -1, 9, 0, 2, -1};
static const veryshort foobletch[8] = {-1, 3, 7, -1, -1, 11, 15, -1};

static const veryshort ftequalize[8] = {6, 3, 19, 13, 22, 19, 3, 29};
static const veryshort ftlcwv[12] = {25, 26, 2, 3, 9, 10, 18, 19, 25, 26, 2, 3};
static const veryshort ftlqtg[12] = {29, 6, 10, 11, 13, 22, 26, 27, 29, 6, 10, 11};
static const veryshort qtlqtg[12] = {5, -1, -1, 0, 1, -1, -1, 4, 5, -1, -1, 0};
static const veryshort qtlbone[12] = {0, 3, -1, -1, 4, 7, -1, -1, 0, 3, -1, -1};
static const veryshort qtlxwv[12] = {0, 1, -1, -1, 4, 5, -1, -1, 0, 1, -1, -1};
static const veryshort qtl1x8[12] = {-1, -1, 5, 7, -1, -1, 1, 3, -1, -1, 5, 7};
static const veryshort qtlrig[12] = {6, 7, -1, -1, 2, 3, -1, -1, 6, 7, -1, -1};
static const veryshort ft4x4bh[16] = {9, 8, 7, -1, 6, -1, -1, -1, 3, 2, 1, -1, 0, -1, -1, -1};
static const veryshort ftqtgbh[8] = {-1, -1, 10, 11, -1, -1, 4, 5};
static const veryshort ft4x446[16] = {4, 7, 22, 8, 13, 14, 15, 21, 16, 19, 10, 20, 1, 2, 3, 9};
static const veryshort ft2646[12] = {11, 10, 9, 8, 7, 6, 23, 22, 21, 20, 19, 18};
static const veryshort galtranslateh[16]  = {-1,  3,  4,  2, -1, -1, -1,  5,
                                             -1,  7,  0,  6, -1, -1, -1,  1};
static const veryshort galtranslatev[16]  = {-1, -1, -1,  1, -1,  3,  4,  2,
                                             -1, -1, -1,  5, -1,  7,  0,  6};
static const veryshort phantranslateh[16] = { 0, -1,  1,  1, -1,  3,  2,  2,
                                              4, -1,  5,  5, -1,  7,  6,  6};
static const veryshort phantranslatev[16] = {-1,  7,  6,  6,  0, -1,  1,  1,
                                             -1,  3,  2,  2,  4, -1,  5,  5};

#ifdef BREAKS_CAST_BACK
static const veryshort phan4x4xlatea[16] = {-1, -1,  8,  6, -1, -1, 12, 10, -1, -1,  0, 14, -1, -1,  4,  2};
static const veryshort phan4x4xlateb[16] = {-1,  5, -1,  7, -1,  9, -1, 11, -1, 13, -1, 15, -1,  1, -1,  3};
#endif
static const veryshort sdmdtranslateh[8] = {0, 0, 0, 1, 2, 0, 0, 3};
static const veryshort sdmdtranslatev[8] = {0, 3, 0, 0, 0, 1, 2, 0};



static const veryshort octtranslateh[64] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,
   0,  0,  0,  7,  0,  0,  0,  6,  0,  0,  0,  5,  0,  0,  0,  4,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  9, 10, 11,
   0,  0,  0, 15,  0,  0,  0, 14,  0,  0,  0, 13,  0,  0,  0, 12};

static const veryshort octtranslatev[64] = {
   0,  0,  0, 15,  0,  0,  0, 14,  0,  0,  0, 13,  0,  0,  0, 12,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,
   0,  0,  0,  7,  0,  0,  0,  6,  0,  0,  0,  5,  0,  0,  0,  4,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  9, 10, 11};

static const veryshort octt4x6lateh[64] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  0, 11, 10,  9,
   0,  0,  0,  0,  0,  0,  5,  6,  0,  0,  4,  7,  0,  0,  3,  8,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 12, 13, 14,  0, 23, 22, 21,
   0,  0,  0,  0,  0,  0, 17, 18,  0,  0, 16, 19,  0,  0, 15, 20};

static const veryshort octt4x6latev[64] = {
   0,  0,  0,  0,  0,  0, 17, 18,  0,  0, 16, 19,  0,  0, 15, 20,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  0, 11, 10,  9,
   0,  0,  0,  0,  0,  0,  5,  6,  0,  0,  4,  7,  0,  0,  3,  8,
   0,  0,  0,  0,  0,  0,  0,  0,  0, 12, 13, 14,  0, 23, 22, 21};

static const veryshort hextranslateh[32] = {
   0,  1,  2,  3,  4,  5,  6,  7,  0,  0,  0,  0,  0,  0,  0,  0,
   8,  9, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0};

static const veryshort hextranslatev[32] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,
   0,  0,  0,  0,  0,  0,  0,  0,  8,  9, 10, 11, 12, 13, 14, 15};




static const veryshort dmdhyperh[12] = {0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 3, 0};
static const veryshort dmdhyperv[12] = {0, 3, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0};
static const veryshort linehyperh[12] = {0, 1, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0};
static const veryshort linehyperv[12] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 3, 0};
static const veryshort hyperboneh[16] = {-1, -1, -1, -1, 1, 4, 2, 3, -1, -1, -1, -1, 5, 0, 6, 7};
static const veryshort hyperbonev[16] = {5, 0, 6, 7, -1, -1, -1, -1, 1, 4, 2, 3, -1, -1, -1, -1};
static const veryshort galhyperh[12] = {6, 0, 0, 0, 3, 1, 2, 0, 4, 0, 7, 5};
static const veryshort galhyperv[12] = {0, 7, 5, 6, 0, 0, 0, 3, 1, 2, 0, 4};
static const veryshort qtghyperh[12] = {6, 7, 0, 0, 0, 1, 2, 3, 4, 0, 0, 5};
static const veryshort qtghyperv[12] = {0, 0, 5, 6, 7, 0, 0, 0, 1, 2, 3, 4};
static const veryshort starhyperh[12] =  {0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0};
static const veryshort fstarhyperh[12] = {0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0};
static const veryshort lilstar1[8] = {0, 2, 0, 0, 3, 0, 0, 1};
static const veryshort lilstar2[8] = {3, 0, 0, 1, 0, 2, 0, 0};
static const veryshort lilstar3[8] = {0, 1, 0, 0, 2, 3, 0, 0};
static const veryshort lilstar4[8] = {0, 0, 2, 3, 0, 0, 0, 1};


static const veryshort qtbd1[12] = {5, 9, 6, 7, 9, 0, 1, 9, 2, 3, 9, 4};
static const veryshort qtbd2[12] = {9, 5, 6, 7, 0, 9, 9, 1, 2, 3, 4, 9};
static const veryshort qtbd3[12] = {9, 5, 6, 7, 9, 0, 9, 1, 2, 3, 9, 4};
static const veryshort qtbd4[12] = {5, 9, 6, 7, 0, 9, 1, 9, 2, 3, 4, 9};
static const veryshort q3x4xx1[12] = {9, 5, 0, 9, 9, 1, 9, 2, 3, 9, 9, 4};
static const veryshort q3x4xx2[12] = {9, 9, 9, 9, 2, 3, 9, 9, 9, 9, 0, 1};
static const veryshort q3x4xx3[12] = {9, 9, 2, 2, 9, 9, 3, 3, 9, 9, 0, 1};
static const veryshort q3x4xx4[12] = {3, 3, 9, 9, 0, 1, 9, 9, 2, 2, 9, 9};



extern void do_stability(uint32 *personp, stability stab, int turning) THROW_DECL
{
   int t, at, st, atr;

   t = turning & 3;

   switch (stab) {
      case stb_z:
         if (t != 0)
            fail("Person turned but was marked 'Z' stability.");
         break;
      case stb_a:
         t -= 4;
         break;
      case stb_c:
         if (t == 0) t = 4;
         break;
      case stb_ac:
         /* Turn one quadrant anticlockwise. */
         do_stability(personp, stb_a, 3);
         t += 1;   /* And the rest clockwise. */
         break;
      case stb_aac:
         /* Turn two quadrants anticlockwise. */
         do_stability(personp, stb_a, 2);
         if (t == 3) t = -1;  /* And the rest clockwise. */
         t += 2;
         break;
      case stb_aaac:
         /* Turn three quadrants anticlockwise. */
         do_stability(personp, stb_a, 1);
         if (t >= 2) t -= 4;  /* And the rest clockwise. */
         t += 3;
         break;
      case stb_aaaac:
         /* Turn four quadrants anticlockwise. */
         do_stability(personp, stb_a, 0);
         if (t == 0) t = 4;   /* And the rest clockwise. */
         break;
      case stb_ca:
         /* Turn one quadrant clockwise. */
         do_stability(personp, stb_c, 1);
         if (t == 0) t = 4;   /* And the rest anticlockwise. */
         t -= 5;
         break;
      case stb_cca:
         /* Turn two quadrants clockwise. */
         do_stability(personp, stb_c, 2);
         if (t <= 1) t += 4;  /* And the rest anticlockwise. */
         t -= 6;
         break;
      case stb_ccca:
         /* Turn three quadrants clockwise. */
         do_stability(personp, stb_c, 3);
         if (t == 3) t = -1;  /* And the rest anticlockwise. */
         t -= 3;
         break;
      case stb_cccca:
         /* Turn four quadrants clockwise. */
         do_stability(personp, stb_c, 0);
         t -= 4;              /* And the rest anticlockwise. */
         break;
      case stb_aa:
         /* Turn four quadrants anticlockwise. */
         do_stability(personp, stb_a, 0);
         break;      /* And keep turning. */
      case stb_cc:
         /* Turn four quadrants clockwise. */
         do_stability(personp, stb_c, 0);
         break;      /* And keep turning. */
      default:
         *personp &= ~STABLE_MASK;
         return;
   }

   /* Now t has the number of quadrants the person turned, clockwise or anticlockwise. */

   st = (t < 0) ? -1 : 1;
   at = t * st;
   atr = at - ((*personp & (STABLE_RBIT*7)) / STABLE_RBIT);

   if (atr <= 0)
      *personp -= at*STABLE_RBIT;
   else
      *personp =
         (*personp & ~(STABLE_RBIT*7|STABLE_VBIT*3)) |
         ((*personp + (STABLE_VBIT * atr * st)) & (STABLE_VBIT*3));
}





extern long_boolean check_restriction(
   setup *ss,
   assumption_thing restr,
   long_boolean instantiate_phantoms,
   uint32 flags) THROW_DECL
{
   uint32 q0, q1, q2, q3;
   uint32 z, t;
   int idx;
   const veryshort *mp;
   long_boolean retval = TRUE;   /* TRUE means we were not able to instantiate phantoms.
                                    It is only meaningful if instantiation was requested. */

   // First, check for nice things.

   // If this successfully instantiates phantoms, it will clear retval.
   switch (verify_restriction(ss, restr, instantiate_phantoms, &retval)) {
   case restriction_passes:
      goto getout;
   case restriction_bad_level:
      if (allowing_all_concepts) {
         warn(warn__bad_call_level);
         goto getout;
      }
      else
         fail("This call is not legal from this formation at this level.");
   case restriction_fails:
      goto restr_failed;
   }

   // Now check all the other stuff.

   switch (restr.assumption) {
   case cr_real_1_4_tag: case cr_real_3_4_tag:
   case cr_real_1_4_line: case cr_real_3_4_line:
   case cr_tall6:
      // These are not legal if they weren't handled already.
      goto restr_failed;
   }

   if (!(restr.assump_col & 1)) {
      // Restriction is "line-like" or special.

      static const veryshort mapwkg8[3] = {2, 2, 6};
      static const veryshort mapwkg6[3] = {2, 2, 5};
      static const veryshort mapwkg4[3] = {2, 1, 3};
      static const veryshort mapwkg2[3] = {2, 0, 1};
      static const veryshort mapwk24[5] = {4, 1, 2, 6, 5};

      switch (restr.assumption) {
      case cr_awkward_centers:       /* check for centers not having left hands */
         switch (ss->kind) {
         case s2x4: mp = mapwk24; goto check_wk;
         case s1x8: mp = mapwkg8; goto check_wk;
         case s1x6: mp = mapwkg6; goto check_wk;
         case s1x4: mp = mapwkg4; goto check_wk;
         case s1x2: mp = mapwkg2; goto check_wk;
         }
         break;
      case cr_ends_are_peelable:
         /* check for ends in a "peelable" (everyone in genuine tandem somehow) box */
         if (ss->kind == s2x4) {
            q1 = 0; q0 = 0; q3 = 0; q2 = 0;
            if ((t = ss->people[0].id1) != 0) { q1 |= t; q0 |= (t^2); }
            if ((t = ss->people[3].id1) != 0) { q3 |= t; q2 |= (t^2); }
            if ((t = ss->people[4].id1) != 0) { q3 |= t; q2 |= (t^2); }
            if ((t = ss->people[7].id1) != 0) { q1 |= t; q0 |= (t^2); }
            if (((q1&3) && (q0&3)) || ((q3&3) && (q2&3)))
               goto restr_failed;
         }
         break;
      case cr_explodable:
         if (ss->kind == s1x4) {
            t = ss->people[1].id1;
            z = ss->people[3].id1;
            if ((z & t) && ((z ^ t) & 2)) goto restr_failed;
         }
         break;
      case cr_rev_explodable:
         if (ss->kind == s1x4) {
            t = ss->people[0].id1;
            z = ss->people[2].id1;
            if ((z & t) && ((z ^ t) & 2)) goto restr_failed;
         }
         break;
      case cr_not_tboned:
         q0 = 0;

         for (idx=0 ; idx<=setup_attrs[ss->kind].setup_limits ; idx++)
            q0 |= ss->people[idx].id1;

         if ((q0 & 011) == 011) goto restr_failed;

         break;
      }
   }

   goto getout;

   check_wk:   /* check the "awkward_centers" restriction. */

   for (idx=0 ; idx < mp[0] ; idx+=2) {
      t = ss->people[mp[idx+1]].id1;
      z = ss->people[mp[idx+2]].id1;
      if ((z&t) && !((z | (~t)) & 2)) warn(warn__awkward_centers);
   }

   goto getout;

   restr_failed:

   switch (flags) {
      case CAF__RESTR_RESOLVE_OK:
         warn(warn__dyp_resolve_ok);
         break;
      case CAF__RESTR_UNUSUAL:
         warn(warn__unusual);
         break;
      case CAF__RESTR_CONTROVERSIAL:
         warn(warn_controversial);
         break;
      case CAF__RESTR_BOGUS:
         warn(warn_serious_violation);
         break;
      case CAF__RESTR_FORBID:
         fail("This call is not legal from this formation.");
      case 99:
         if (restr.assumption == cr_qtag_like && restr.assump_both == 1)
            fail("People are not facing as in 1/4 tags.");
         else if (restr.assumption == cr_qtag_like && restr.assump_both == 2)
            fail("People are not facing as in 3/4 tags.");
         else if (restr.assumption == cr_wave_only && restr.assump_col == 0)
            fail("People are not in waves.");
         else if (restr.assumption == cr_all_ns)
            fail("People are not in lines.");
         else if (restr.assumption == cr_all_ew)
            fail("People are not in columns.");
         else
            fail("The people do not satisfy the assumption.");
      default:
         warn(warn__do_your_part);
         break;
   }

   getout:

   /* One final check.  If there is an assumption in place that is inconsistent
      with the restriction being enforced, then it is an error, even though
      no live people violate the restriction. */

   switch (ss->cmd.cmd_assume.assumption) {
   case cr_qtag_like:
      switch (restr.assumption) {
      case cr_diamond_like: case cr_pu_qtag_like:
         fail("An assumed facing direction for phantoms is illegal for this call.");
         break;
      }
      break;
   case cr_diamond_like:
      switch (restr.assumption) {
      case cr_qtag_like: case cr_pu_qtag_like:
         fail("An assumed facing direction for phantoms is illegal for this call.");
         break;
      }
      break;
   case cr_pu_qtag_like:
      switch (restr.assumption) {
      case cr_qtag_like: case cr_diamond_like:
         fail("An assumed facing direction for phantoms is illegal for this call.");
         break;
      }
      break;
   case cr_couples_only: case cr_3x3couples_only: case cr_4x4couples_only:
      switch (restr.assumption) {
      case cr_wave_only: case cr_miniwaves:
         fail("An assumed facing direction for phantoms is illegal for this call.");
         break;
      }
      break;
   case cr_wave_only: case cr_miniwaves:
      switch (restr.assumption) {
      case cr_couples_only: case cr_3x3couples_only: case cr_4x4couples_only:
         fail("An assumed facing direction for phantoms is illegal for this call.");
         break;
      }
      break;
   }

   return retval;
}




static void special_4_way_symm(
   callarray *tdef,
   setup *scopy,
   setup *destination,
   int newplacelist[],
   uint32 lilresult_mask[],
   setup *result) THROW_DECL

{
   static const veryshort table_2x4[8] = {10, 15, 3, 1, 2, 7, 11, 9};

   static const veryshort table_2x8[16] = {
      12, 13, 14, 15, 31, 27, 23, 19,
      44, 45, 46, 47, 63, 59, 55, 51};

   static const veryshort table_3x1d[8] = {
      1, 2, 3, 9, 17, 18, 19, 25};

   static const veryshort table_2x6[12] = {
      13, 14, 15, 31, 27, 23,
      45, 46, 47, 63, 59, 55};

   static const veryshort table_4x6[24] = {
       9, 10, 11, 30, 26, 22,
      23, 27, 31, 15, 14, 13,
      41, 42, 43, 62, 58, 54,
      55, 59, 63, 47, 46, 45};

   static const veryshort table_1x16[16] = {
       0,  1,  2,  3,  4,  5,  6,  7,
      16, 17, 18, 19, 20, 21, 22, 23};

   static const veryshort table_4dmd[16] = {
      7, 5, 14, 12, 16, 17, 18, 19,
      23, 21, 30, 28, 0, 1, 2, 3};

   static const veryshort table_hyperbone[8] = {
      13, 4, 6, 7, 5, 12, 14, 15};

   static const veryshort table_2x3_4dmd[6] = {
      6, 11, 13, 22, 27, 29};

   static const veryshort line_table[4] = {0, 1, 6, 7};

   static const veryshort dmd_table[4] = {0, 4, 6, 10};

   int begin_size;
   int real_index;
   int k, result_size, result_quartersize;
   const veryshort *the_table = (const veryshort *) 0;

   switch (result->kind) {
   case s2x2: case s_galaxy:
   case s_c1phan: case s4x4:
   case s_hyperglass: case s_thar:
   case s_star: case s1x1: case s_alamo:
      break;
   case s1x4:
      result->kind = s_hyperglass;
      the_table = line_table;
      break;
   case sdmd:
      result->kind = s_hyperglass;
      the_table = dmd_table;
      break;
   case s2x4:
      result->kind = s4x4;
      the_table = table_2x4;
      break;
   case s2x8:
      result->kind = s8x8;
      the_table = table_2x8;
      break;
   case s3x1dmd:
      result->kind = sx4dmd;
      the_table = table_3x1d;
      break;
   case s4x6:
      result->kind = s8x8;
      the_table = table_4x6;
      break;
   case s2x6:
      result->kind = s8x8;
      the_table = table_2x6;
      break;
   case s1x16:
      result->kind = sx1x16;
      the_table = table_1x16;
      break;
   case s4dmd:
      result->kind = sx4dmd;
      the_table = table_4dmd;
      break;
   case s_bone:
      result->kind = s_hyperbone;
      the_table = table_hyperbone;
      break;
   case s2x3:
      result->kind = sx4dmd;
      the_table = table_2x3_4dmd;
      break;
   default:
      fail("Don't recognize ending setup for this call.");
   }

   begin_size = setup_attrs[scopy->kind].setup_limits+1;
   result_size = setup_attrs[result->kind].setup_limits+1;
   result_quartersize = result_size >> 2;
   lilresult_mask[0] = 0;
   lilresult_mask[1] = 0;

   for (real_index=0; real_index<begin_size; real_index++) {
      personrec this_person = scopy->people[real_index];
      clear_person(destination, real_index);
      if (this_person.id1) {
         int real_direction = this_person.id1 & 3;
         int northified_index = (real_index + (((4-real_direction)*begin_size) >> 2)) % begin_size;
         uint32 z = find_calldef(tdef, scopy, real_index, real_direction, northified_index);
         k = (z >> 4) & 0x1F;
         if (the_table) k = the_table[k];
         k = (k + real_direction*result_quartersize) % result_size;
         uint32 rollstuff = (z * (NROLL_BIT/NDBROLL_BIT)) & NROLL_MASK;
         if ((rollstuff+NROLL_BIT) & (NROLL_BIT*2)) rollstuff |= NROLL_BIT*4;
         destination->people[real_index].id1 = (this_person.id1 & ~(NROLL_MASK | 077)) |
               ((z+real_direction*011) & 013) | rollstuff;

         if (this_person.id1 & STABLE_ENAB)
            do_stability(&destination->people[real_index].id1,
                         (stability) ((z/DBSTAB_BIT) & 0xF),
                         (z + result->rotation));

         destination->people[real_index].id2 = this_person.id2;
         newplacelist[real_index] = k;
         lilresult_mask[k>>5] |= (1 << (k&037));
      }
   }
}



static void special_triangle(
   callarray *cdef,
   callarray *ldef,
   setup *scopy,
   setup *destination,
   int newplacelist[],
   int num,
   uint32 lilresult_mask[],
   setup *result) THROW_DECL
{
   int real_index;
   int numout = setup_attrs[result->kind].setup_limits+1;
   long_boolean is_triangle = (scopy->kind != s1x3);
   long_boolean result_is_triangle = (result->kind == s_trngl || result->kind == s_trngl4);

   for (real_index=0; real_index<num; real_index++) {
      personrec this_person = scopy->people[real_index];
      destination->people[real_index].id1 = 0;
      destination->people[real_index].id2 = 0;
      newplacelist[real_index] = -1;
      if (this_person.id1) {
         int northified_index, k;
         uint32 z;
         int real_direction = this_person.id1 & 3;

         if (is_triangle) {
            int d2 = ((real_direction >> 1) & 1) ? num : 0;
            northified_index = real_index + d2;
         }
         else {
            northified_index = (real_direction & 2) ? 2-real_index : real_index;
         }

         z = find_calldef((real_direction & 1) ? cdef : ldef, scopy, real_index, real_direction, northified_index);
         k = (z >> 4) & 0x1F;

         if (real_direction & 2) {
            if (result_is_triangle) {
               k-=num;
               if (k<0) k+=(num+num);
            }
            else if (result->kind == s1x3)
               k = numout-1-k;
            else {
               k -= (numout >> 1);
               if (k<0) k+=numout;
            }
         }

         uint32 rollstuff = (z * (NROLL_BIT/NDBROLL_BIT)) & NROLL_MASK;
         if ((rollstuff+NROLL_BIT) & (NROLL_BIT*2)) rollstuff |= NROLL_BIT*4;
         destination->people[real_index].id1 = (this_person.id1 & ~(NROLL_MASK | 077)) |
               ((z + real_direction * 011) & 013) | rollstuff;

         if (this_person.id1 & STABLE_ENAB)
            do_stability(&destination->people[real_index].id1,
                         (stability) ((z/DBSTAB_BIT) & 0xF),
                         (z+result->rotation));

         destination->people[real_index].id2 = this_person.id2;
         newplacelist[real_index] = k;
         lilresult_mask[0] |= (1 << k);
      }
   }

   /* Check whether the call went into the other triangle.  If so, it
      must have done so completely. */

   if (result_is_triangle) {
      int i;

      for (i=0; i<num; i++) {
         if (newplacelist[i] >= numout) {
            result->rotation += 2;

            for (i=0; i<num; i++) {
               if (newplacelist[i] >=0) {
                  newplacelist[i] -= numout;
                  if (newplacelist[i] < 0)
                     fail("Call went into other triangle????.");
                  destination->people[i].id1 = rotperson(destination->people[i].id1, 022);
               }
            }

            break;
         }
      }
   }
}


static long_boolean handle_3x4_division(
   setup *ss, uint32 callflags1, uint32 newtb, uint32 livemask,
   uint32 *division_code_p,
   callarray *calldeflist, long_boolean matrix_aware, setup *result)

{
   static const veryshort map_3x4_fudge[8] = {1, 2, 4, 5, 7, 8, 10, 11};
   long_boolean forbid_little_stuff;
   uint32 nxnbits =
      ss->cmd.cmd_final_flags.her8it & (INHERITFLAG_NXNMASK|INHERITFLAG_MXNMASK);

   /* The call has no applicable 3x4 or 4x3 definition. */
   /* First, check whether it has 2x3/3x2 definitions, and divide the setup if so,
      and if the call permits it.  This is important for permitting "Z axle" from
      a 3x4 but forbidding "circulate" (unless we give a concept like 12 matrix
      phantom columns.)  We also enable this if the caller explicitly said
      "3x4 matrix". */

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       ((callflags1 & CFLAG1_SPLIT_IF_Z) &&
        nxnbits != INHERITFLAGNXNK_3X3 &&
        (livemask == 06565 || livemask == 07272)) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) ||
       nxnbits == INHERITFLAGMXNK_1X3 ||
       nxnbits == INHERITFLAGMXNK_3X1) {

      if ((!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_2x3, ss, calldeflist))) {
         *division_code_p = MAPCODE(s2x3,2,MPKIND__SPLIT,1);
         return TRUE;
      }
      else if ((!(newtb & 001) || assoc(b_1x3, ss, calldeflist)) &&
               (!(newtb & 010) || assoc(b_3x1, ss, calldeflist))) {
         *division_code_p = MAPCODE(s1x3,4,MPKIND__SPLIT,1);
         return TRUE;
      }
   }

   /* Search for divisions into smaller things: 2x2, 1x2, 2x1, or 1x1,
      in setups whose occupations make it clear what is meant.
      We do not allow this if the call has a larger definition.
      The reason is that we do not actually use "divided_setup_move"
      to perform the division that we want in one step.  This could require
      maps with excessive arity.  Instead, we use existing maps to break it
      down a little, and depend on recursion.  If larger definitions existed,
      the recursion might not do what we have in mind.  For example,
      we could get here on "checkmate" from offset columns, since that call
      has a 2x2 definition but no 4x3 definition.  However, all we do here
      is use the map that the "offset columns" concept uses.  We would then
      recurse on a virtual 4x2 column, and pick up the 8-person version of
      "checkmate", which would be wrong.  Similarly, this could lead to an
      "offset wave circulate" being done when we didn't say "offset wave".
      For the call "circulate", it might be considered more natural to do
      a 12 matrix circulate.  But if the user doesn't say "12 matrix",
      we want to refuse to do it.  The only legal circulate if no concept
      is given is the 2x2 circulate in each box.  So we disallow this if
      any possibly conflicting definition could be seen during the recursion. */

   forbid_little_stuff =
      !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
      (assoc(b_2x4, ss, calldeflist) ||
       assoc(b_4x2, ss, calldeflist) ||
       assoc(b_2x3, ss, calldeflist) ||
       assoc(b_3x2, ss, calldeflist));

   switch (livemask) {
   case 0xF3C: case 0xCF3:
      // We are in offset lines/columns, i.e. "clumps".
      // See if we can do the call in 2x2 or smaller setups.
      if (forbid_little_stuff ||
          (!assoc(b_2x2, ss, calldeflist) &&
           !assoc(b_1x2, ss, calldeflist) &&
           !assoc(b_2x1, ss, calldeflist) &&
           !assoc(b_1x1, ss, calldeflist)))
         fail("Don't know how to do this call in this setup.");
      if (!matrix_aware) warn(warn__each2x2);
      *division_code_p = (livemask == 0xF3C) ?
         MAPCODE(s2x2,2,MPKIND__OFFS_L_HALF,0) :
            MAPCODE(s2x2,2,MPKIND__OFFS_R_HALF,0);
      return TRUE;
   case 0xEBA: case 0xD75:
      // We are in "Z"'s.  See if we can do the call in 1x2, 2x1, or 1x1 setups.
      // We do not allow 2x2 definitions.
      // We do now!!!!  It is required to allow utb in "1/2 press ahead" Z lines.
      if (!forbid_little_stuff &&
          /* See also 32 lines below. */
          /*                        !assoc(b_2x2, ss, calldeflist) &&*/
          (((!(newtb & 001) || assoc(b_1x2, ss, calldeflist)) &&
            (!(newtb & 010) || assoc(b_2x1, ss, calldeflist))) ||
           assoc(b_1x1, ss, calldeflist))) {
         warn(warn__each1x2);
         *division_code_p = (livemask == 0xEBA) ? spcmap_lz12 : spcmap_rz12;
         return TRUE;
      }
   }

   // Check for everyone in either the outer 2 triple lines or the
   // center triple line, and able to do the call in 1x4 or smaller setups.

   if (((livemask & 0x3CF) == 0 || (livemask & 0xC30) == 0) &&
       !forbid_little_stuff &&
       (assoc(b_1x4, ss, calldeflist) ||
        assoc(b_4x1, ss, calldeflist) ||
        assoc(b_1x2, ss, calldeflist) ||
        assoc(b_2x1, ss, calldeflist) ||
        assoc(b_1x1, ss, calldeflist))) {
      if (!matrix_aware) warn(warn__each1x4);
      *division_code_p = MAPCODE(s1x4,3,MPKIND__SPLIT,1);
      return TRUE;
   }

   // Setup is randomly populated.  See if we have 1x2/1x1 definitions.
   // If so, divide the 3x4 into 3 1x4's.  We accept 1x2/2x1 definitions if the call is
   // "matrix aware" (it knows that 1x4's are what it wants) or if the facing directions
   // are such that that would win anyway.
   // We are also more lenient about "run", which we detect through
   // CAF__LATERAL_TO_SELECTEES.  See test nf35.
   //
   // This is not the right way to do this.  The right way would be,
   // in general, to try various recursive splittings and check that the call
   // could be done on the various *fully occupied* subparts.  So, for example,
   // in the case that arises in nf35, we have a 3x4 that consists of a center
   // column of 6 and two outlyers, who are in fact the runners.  We split
   // into 1x4's and then into 1x2's and 1x1's, guided by the live people.
   // For those in the centers column not adjacent to the outlyers, we have
   // to divide to 1x1's.  The call "anyone run" is arranged to be legal on
   // a 1x1 as long as that person isn't selected.  Now the outlyers and
   // their adjacent center form a fully live 1x2.  In that 1x2, the "run"
   // call is legal, when one selectee and one non-selectee.
   //
   // But that's a pretty ambitious change.

   if (!forbid_little_stuff &&
       (assoc(b_1x1, ss, calldeflist) ||
        (((ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) ||
          (calldeflist->callarray_flags & CAF__LATERAL_TO_SELECTEES)) &&
         ((!(newtb & 010) ||
           assoc(b_1x2, ss, calldeflist) ||
           assoc(b_1x4, ss, calldeflist)) &&
          (!(newtb & 001) ||
           assoc(b_2x1, ss, calldeflist) ||
           assoc(b_4x1, ss, calldeflist)))) ||
        (matrix_aware &&
         (assoc(b_1x2, ss, calldeflist) ||
          assoc(b_2x1, ss, calldeflist))))) {
      *division_code_p = MAPCODE(s1x4,3,MPKIND__SPLIT,1);
      return TRUE;
   }
   else if (!forbid_little_stuff &&
       (((!(newtb & 010) || assoc(b_3x1, ss, calldeflist)) &&
         (!(newtb & 001) || assoc(b_1x3, ss, calldeflist))))) {
      *division_code_p = MAPCODE(s1x3,4,MPKIND__SPLIT,1);
      return TRUE;
   }

   /* Now check for something that can be fudged into a "quarter tag"
      (which includes diamonds).  Note whether we fudged,
      since some calls do not tolerate it. */
   // But don't do this if the "points" are all outside and their orientation
   // is in triple lines.

   if (livemask == 0xE79 &&
       ((ss->people[0].id1 & ss->people[3].id1 & ss->people[6].id1 & ss->people[9].id1) & 1))
      fail("Can't do this call from arbitrary 3x4 setup.");

   setup sss;
   bool really_fudged = false;
   gather(&sss, ss, map_3x4_fudge, 7, 0);

   if (ss->people[0].id1) {
      if (ss->people[1].id1) fail("Can't do this call from arbitrary 3x4 setup.");
      else (void) copy_person(&sss, 0, ss, 0);
      really_fudged = true;
   }

   if (ss->people[3].id1) {
      if (ss->people[2].id1) fail("Can't do this call from arbitrary 3x4 setup.");
      else (void) copy_person(&sss, 1, ss, 3);
      really_fudged = true;
   }

   if (ss->people[6].id1) {
      if (ss->people[7].id1) fail("Can't do this call from arbitrary 3x4 setup.");
      else (void) copy_person(&sss, 4, ss, 6);
      really_fudged = true;
   }

   if (ss->people[9].id1) {
      if (ss->people[8].id1) fail("Can't do this call from arbitrary 3x4 setup.");
      else (void) copy_person(&sss, 5, ss, 9);
      really_fudged = true;
   }

   sss.kind = s_qtag;
   sss.rotation = ss->rotation;
   sss.cmd = ss->cmd;
   sss.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
   move(&sss, really_fudged, result);
   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
   return FALSE;
}


static bool handle_4x4_division(
   setup *ss, uint32 callflags1, uint32 newtb, uint32 livemask,
   uint32 & division_code,            // We write over this.
   int & finalrot,                    // We write over this.
   callarray *calldeflist, long_boolean matrix_aware)
{
   long_boolean forbid_little_stuff;
   uint32 nxnbits =
      ss->cmd.cmd_final_flags.her8it & (INHERITFLAG_NXNMASK|INHERITFLAG_MXNMASK);

   // The call has no applicable 4x4 definition.
   // First, check whether it has 2x4/4x2 definitions, and divide the setup if so,
   // and if the call permits it.

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      if ((!(newtb & 010) || assoc(b_4x2, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_2x4, ss, calldeflist))) {
         // Split to left and right halves.
         finalrot++;
         division_code = MAPCODE(s2x4,2,MPKIND__SPLIT,1);
         return true;
      }
      else if ((!(newtb & 001) || assoc(b_4x2, ss, calldeflist)) &&
               (!(newtb & 010) || assoc(b_2x4, ss, calldeflist))) {
         // Split to bottom and top halves.
         division_code = MAPCODE(s2x4,2,MPKIND__SPLIT,1);
         return true;
      }
   }

   // Look for 3x1 triangles.

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) ||
       nxnbits == INHERITFLAGMXNK_1X3 ||
       nxnbits == INHERITFLAGMXNK_3X1 ||
       nxnbits == INHERITFLAGNXNK_3X3) {
      uint32 assocstuff = 0;
      if (assoc(b_3x2, ss, calldeflist)) assocstuff |= 001;
      if (assoc(b_2x3, ss, calldeflist)) assocstuff |= 010;

      if (livemask == 0x6969 &&
          (!(newtb & 001) || (assocstuff&001)) &&
          (!(newtb & 010) || (assocstuff&010))) {
         division_code = spcmap_lh_s2x3_2;
         return true;
      }
      else if (livemask == 0x9696 &&
          (!(newtb & 010) || (assocstuff&001)) &&
          (!(newtb & 001) || (assocstuff&010))) {
         division_code = spcmap_lh_s2x3_3;
         return true;
      }
      else if (livemask == 0xF0F0 &&
          (!(newtb & 001) || (assocstuff&001)) &&
          (!(newtb & 010) || (assocstuff&010))) {
         division_code = spcmap_rh_s2x3_2;
         return true;
      }
      else if (livemask == 0x0F0F &&
          (!(newtb & 010) || (assocstuff&001)) &&
          (!(newtb & 001) || (assocstuff&010))) {
         division_code = spcmap_rh_s2x3_3;
         return true;
      }
      else if (livemask == 0xACAC &&
               (((newtb & 011) == 001 && (assocstuff==001)) ||
                ((newtb & 011) == 010 && (assocstuff==010)))) {
         division_code = spcmap_rh_s2x3_2;
         return true;
      }
      else if (livemask == 0xCACA &&
               (((newtb & 011) == 001 && (assocstuff==001)) ||
                ((newtb & 011) == 010 && (assocstuff==010)))) {
         division_code = spcmap_lh_s2x3_2;
         return true;
      }
      else if (livemask == 0xACAC &&
               (((newtb & 011) == 010 && (assocstuff==001)) ||
                ((newtb & 011) == 001 && (assocstuff==010)))) {
         division_code = spcmap_lh_s2x3_3;
         return true;
      }
      else if (livemask == 0xCACA &&
               (((newtb & 011) == 010 && (assocstuff==001)) ||
                ((newtb & 011) == 001 && (assocstuff==010)))) {
         division_code = spcmap_rh_s2x3_3;
         return true;
      }
   }

   // The only other way this can be legal is if we can identify
   // smaller setups of all real people and can do the call on them.
   // For example, if the outside phantom lines are fully occupied
   // and the inside ones empty, we could do a swing-thru.
   // We also identify Z's from which we can do "Z axle".

   // But if user said "16 matrix" or something, do don't do implicit division.

   if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      switch (livemask) {
      case 0x6666:
         division_code = MAPCODE(s1x2,4,MPKIND__4_EDGES,0);
         return true;
      case 0xAAAA:
         division_code = spcmap_4x4_spec0;
         return true;
      case 0xCCCC:
         division_code = spcmap_4x4_spec1;
         return true;
      case 0x3333:
         division_code = spcmap_4x4_spec2;
         return true;
      case 0x5555:
         division_code = spcmap_4x4_spec3;
         return true;
      case 0x8787: finalrot++;
      case 0x7878:
         division_code = spcmap_4x4_spec4;
         return true;
      case 0x1E1E: finalrot++;
      case 0xE1E1:
         division_code = spcmap_4x4_spec5;
         return true;
      case 0xA3A3: finalrot++;
      case 0x3A3A:
         division_code = spcmap_4x4_spec6;
         return true;
      case 0x5C5C: finalrot++;
      case 0xC5C5:
         division_code = spcmap_4x4_spec7;
         return true;
      case 0x7171:
         division_code = spcmap_4x4_ns;
         warn(warn__each1x4);
         return true;
      case 0x1717:
         division_code = spcmap_4x4_ew;
         warn(warn__each1x4);
         return true;
      case 0x4E4E: case 0x8B8B:
         division_code = spcmap_rh_s2x3_3;
         goto handle_z;
      case 0xA6A6: case 0x9C9C:
         division_code = spcmap_lh_s2x3_3;
         goto handle_z;
      case 0xE4E4: case 0xB8B8:
         division_code = spcmap_rh_s2x3_2;
         goto handle_other_z;
      case 0x6A6A: case 0xC9C9:
         division_code = spcmap_lh_s2x3_2;
         goto handle_other_z;
      case 0x4B4B: case 0xB4B4:
         // See comment above, for 3x4.
         forbid_little_stuff =
            !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
            (assoc(b_2x4, ss, calldeflist) ||
             assoc(b_4x2, ss, calldeflist) ||
             assoc(b_dmd, ss, calldeflist) ||
             assoc(b_pmd, ss, calldeflist) ||
             assoc(b_qtag, ss, calldeflist) ||
             assoc(b_pqtag, ss, calldeflist));

         // We are in "clumps".  See if we can do the call in 2x2 or smaller setups.

         // Special stuff:  See if we survive overriding things when call has 1x1 def'n.
         if (!assoc(b_1x1, ss, calldeflist)) {
            if (forbid_little_stuff ||
                (!assoc(b_2x2, ss, calldeflist) &&
                 !assoc(b_1x2, ss, calldeflist) &&
                 !assoc(b_2x1, ss, calldeflist) &&
                 !assoc(b_1x1, ss, calldeflist)))
               fail("Don't know how to do this call in this setup.");
         }

         if (!matrix_aware) warn(warn__each2x2);
         division_code = (livemask == 0x4B4B) ?
            MAPCODE(s2x2,2,MPKIND__OFFS_L_FULL,0) : MAPCODE(s2x2,2,MPKIND__OFFS_R_FULL,0);
         return true;
      }
   }

 dont_handle_z:

   // Setup is randomly populated.  See if we have 1x2/1x1 definitions, but no 2x2.
   // If so, divide the 4x4 into 2 2x4's.

   forbid_little_stuff =
      assoc(b_2x4, ss, calldeflist) ||
      assoc(b_4x2, ss, calldeflist) ||
      assoc(b_2x3, ss, calldeflist) ||
      assoc(b_3x2, ss, calldeflist) ||
      assoc(b_dmd, ss, calldeflist) ||
      assoc(b_pmd, ss, calldeflist) ||
      assoc(b_qtag, ss, calldeflist) ||
      assoc(b_pqtag, ss, calldeflist);

   if (!forbid_little_stuff && !assoc(b_2x2, ss, calldeflist) &&
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      if ((assoc(b_1x2, ss, calldeflist) ||
           assoc(b_2x1, ss, calldeflist) ||
           assoc(b_1x1, ss, calldeflist))) {
         // Without a lot of examination of facing directions, and whether the call
         // has 1x2 vs. 2x1 definitions, and all that, we don't know which axis
         // to use when dividing it.  But any division into 2 2x4's is safe,
         // and code elsewhere will make the tricky decisions later.
         division_code = MAPCODE(s2x4,2,MPKIND__SPLIT,1);
         return true;
      }
      else if ((callflags1 & CFLAG1_12_16_MATRIX_MEANS_SPLIT) &&
               ((!(newtb & 010) || assoc(b_1x4, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_4x1, ss, calldeflist)))) {
         division_code = MAPCODE(s1x4,4,MPKIND__SPLIT,1);
         return true;
      }
      else if ((callflags1 & CFLAG1_12_16_MATRIX_MEANS_SPLIT) &&
               ((!(newtb & 001) || assoc(b_1x4, ss, calldeflist)) &&
                (!(newtb & 010) || assoc(b_4x1, ss, calldeflist)))) {
         division_code = spcmap_4x4v;
         return true;
      }
   }

   // If the call has a 1x1 definition, and we can't do anything else,
   // divide it up into all 16 people.
   if (assoc(b_1x1, ss, calldeflist)) {
      division_code = spcmap_4x4_1x1;
      return true;
   }

   fail("You must specify a concept.");

 handle_z:

   // If this changes shape (as it will in the only known case
   // of this -- Z axle), divided_setup_move will give a warning
   // about going to a parallelogram, since we did not start
   // with 50% offset, though common practice says that a
   // parallelogram is the correct result.  If the call turns out
   // not to be a shape-changer, no warning will be given.  If
   // the call is a shape changer that tries to go into a setup
   // other than a parallelogram, divided_setup_move will raise
   // an error.
   ss->cmd.cmd_misc_flags |= CMD_MISC__OFFSET_Z;
   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) &&
       (!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
       (!(newtb & 001) || assoc(b_2x3, ss, calldeflist)))
      return true;
   goto dont_handle_z;

 handle_other_z:
   ss->cmd.cmd_misc_flags |= CMD_MISC__OFFSET_Z;
   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) &&
       (!(newtb & 010) || assoc(b_2x3, ss, calldeflist)) &&
       (!(newtb & 001) || assoc(b_3x2, ss, calldeflist)))
      return true;
   goto dont_handle_z;
}


static long_boolean handle_4x6_division(
   setup *ss, uint32 callflags1, uint32 newtb, uint32 livemask,
   uint32 *division_code_p,
   callarray *calldeflist, long_boolean matrix_aware)
{
   long_boolean forbid_little_stuff;

   // The call has no applicable 4x6 definition.
   // First, check whether it has 2x6/6x2 definitions,
   // and divide the setup if so, and if the call permits it.

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      // If the call wants a 2x6 or 3x4, do it.
      if ((!(newtb & 010) || assoc(b_2x6, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_6x2, ss, calldeflist))) {
         *division_code_p = MAPCODE(s2x6,2,MPKIND__SPLIT,1);
         return TRUE;
      }
      else if ((!(newtb & 010) || assoc(b_4x3, ss, calldeflist)) &&
               (!(newtb & 001) || assoc(b_3x4, ss, calldeflist))) {
         *division_code_p = MAPCODE(s3x4,2,MPKIND__SPLIT,1);
         return TRUE;
      }

      // Look for special Z's.
      if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
         switch (livemask) {
         case 043204320: case 023402340:
            *division_code_p = spcmap_rh_s2x3_7;
            if ((!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_2x3, ss, calldeflist))) {
               ss->cmd.cmd_misc_flags |= CMD_MISC__OFFSET_Z;
               return TRUE;
            }
            break;
         case 061026102: case 062016201:
            *division_code_p = spcmap_lh_s2x3_7;
            if ((!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_2x3, ss, calldeflist))) {
               ss->cmd.cmd_misc_flags |= CMD_MISC__OFFSET_Z;
               return TRUE;
            }
            break;
         }
      }

      // If the call wants a 2x3 and we didn't find one of the special Z's do it.

      if ((!(newtb & 010) || assoc(b_2x3, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_3x2, ss, calldeflist))) {
         *division_code_p = MAPCODE(s3x4,2,MPKIND__SPLIT,1);
         return TRUE;
      }
   }

   // The only other way this can be legal is if we can identify
   // smaller setups of all real people and can do the call on them.

   // But if user said "16 matrix" or something, do don't do implicit division.

   if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      switch (livemask) {
      case 0xC03C03: case 0x0F00F0:
         forbid_little_stuff =
            !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
            (assoc(b_2x4, ss, calldeflist) ||
             assoc(b_4x2, ss, calldeflist) ||
             assoc(b_dmd, ss, calldeflist) ||
             assoc(b_pmd, ss, calldeflist) ||
             assoc(b_qtag, ss, calldeflist) ||
             assoc(b_pqtag, ss, calldeflist));

         // We are in "clumps".  See if we can do the call in 2x2 or smaller setups.
         if (forbid_little_stuff ||
             (!assoc(b_2x2, ss, calldeflist) &&
              !assoc(b_1x2, ss, calldeflist) &&
              !assoc(b_2x1, ss, calldeflist) &&
              !assoc(b_1x1, ss, calldeflist)))
            fail("Don't know how to do this call in this setup.");
         if (!matrix_aware) warn(warn__each2x2);
         // This will do.
         *division_code_p = MAPCODE(s2x4,3,MPKIND__SPLIT,1);
         return TRUE;
      case 0x1D01D0: case 0xE02E02:
         forbid_little_stuff =
            !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
            (assoc(b_2x4, ss, calldeflist) || assoc(b_4x2, ss, calldeflist));

         // We are in "diamond clumps".  See if we can do the call in diamonds.
         if (forbid_little_stuff ||
             (!assoc(b_dmd, ss, calldeflist) &&
              !assoc(b_pmd, ss, calldeflist)))
            fail("Don't know how to do this call in this setup.");
         if (!matrix_aware) warn(warn__eachdmd);

         *division_code_p = (livemask == 0x1D01D0) ?
            MAPCODE(sdmd,2,MPKIND__OFFS_R_FULL,1) : MAPCODE(sdmd,2,MPKIND__OFFS_L_FULL,1);
         return TRUE;
      }
   }

   return FALSE;
}


static long_boolean handle_3x8_division(
   setup *ss, uint32 callflags1, uint32 newtb, uint32 livemask,
   uint32 *division_code_p,
   callarray *calldeflist, long_boolean matrix_aware)
{
   long_boolean forbid_little_stuff;

   // The call has no applicable 3x8 definition.
   // First, check whether it has 3x4/4x3/2x3/3x2 definitions,
   // and divide the setup if so, and if the call permits it.

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      if ((!(newtb & 010) ||
           assoc(b_3x4, ss, calldeflist) ||
           assoc(b_3x2, ss, calldeflist)) &&
          (!(newtb & 001) ||
           assoc(b_4x3, ss, calldeflist) ||
           assoc(b_2x3, ss, calldeflist))) {
         *division_code_p = MAPCODE(s3x4,2,MPKIND__SPLIT,0);
         return TRUE;
      }
   }

   // The only other way this can be legal is if we can identify
   // smaller setups of all real people and can do the call on them.

   // But if user said "16 matrix" or something, do don't do implicit division.

   if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      switch (livemask) {
      case 0x00F00F: case 0x0F00F0:
         forbid_little_stuff =
            !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
            (assoc(b_2x4, ss, calldeflist) ||
             assoc(b_4x2, ss, calldeflist) ||
             assoc(b_dmd, ss, calldeflist) ||
             assoc(b_pmd, ss, calldeflist) ||
             assoc(b_qtag, ss, calldeflist) ||
             assoc(b_pqtag, ss, calldeflist));

         // We are in 1x4's in the corners.  See if we can do the call in 1x4
         // or smaller setups.
         if (forbid_little_stuff ||
             (!assoc(b_1x4, ss, calldeflist) &&
              !assoc(b_4x1, ss, calldeflist) &&
              !assoc(b_1x2, ss, calldeflist) &&
              !assoc(b_2x1, ss, calldeflist) &&
              !assoc(b_1x1, ss, calldeflist)))
            fail("Don't know how to do this call in this setup.");
         if (!matrix_aware) warn(warn__each1x4);
         // This will do.
         *division_code_p = MAPCODE(s1x8,3,MPKIND__SPLIT,1);
         return TRUE;
      }
   }

   return FALSE; 
}


static long_boolean handle_2x12_division(
   setup *ss, uint32 callflags1, uint32 newtb, uint32 livemask,
   uint32 *division_code_p,
   callarray *calldeflist, long_boolean matrix_aware)
{
   long_boolean forbid_little_stuff;

   // The call has no applicable 2x12 definition.
   // First, check whether it has 2x6/6x2 definitions,
   // and divide the setup if so, and if the call permits it.

   if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
       (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      // If the call wants a 2x6, do it.
      if ((!(newtb & 010) || assoc(b_6x2, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_2x6, ss, calldeflist))) {
         *division_code_p = MAPCODE(s2x6,2,MPKIND__SPLIT,0);
         return TRUE;
      }
   }

   // The only other way this can be legal is if we can identify
   // smaller setups of all real people and can do the call on them.

   // But if user said "16 matrix" or something, do don't do implicit division.

   if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
      switch (livemask) {
      case 0x00F00F: case 0xF00F00:
         forbid_little_stuff =
            !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
            (assoc(b_2x4, ss, calldeflist) ||
             assoc(b_4x2, ss, calldeflist) ||
             assoc(b_dmd, ss, calldeflist) ||
             assoc(b_pmd, ss, calldeflist) ||
             assoc(b_qtag, ss, calldeflist) ||
             assoc(b_pqtag, ss, calldeflist));

         // We are in 1x4's in the corners.  See if we can do the call in 1x4
         // or smaller setups.
         if (forbid_little_stuff ||
             (!assoc(b_1x4, ss, calldeflist) &&
              !assoc(b_4x1, ss, calldeflist) &&
              !assoc(b_1x2, ss, calldeflist) &&
              !assoc(b_2x1, ss, calldeflist) &&
              !assoc(b_1x1, ss, calldeflist)))
            fail("Don't know how to do this call in this setup.");
         if (!matrix_aware) warn(warn__each1x4);
         // This will do.
         *division_code_p = MAPCODE(s2x4,3,MPKIND__SPLIT,0);
         return TRUE;
      }
   }

   return FALSE; 
}


static int divide_the_setup(
   setup *ss,
   uint32 *newtb_p,
   callarray *calldeflist,
   int *desired_elongation_p,
   setup *result) THROW_DECL
{
   int i, j;
   uint32 livemask;
   long_boolean recompute_anyway;
   long_boolean temp_for_2x2;
   long_boolean temp;
   callarray *have_1x2, *have_2x1;
   uint32 division_code = ~0UL;
   mpkind map_kind;
   uint32 newtb = *newtb_p;
   uint32 callflags1 = ss->cmd.callspec->the_defn.callflags1;
   uint64 final_concepts = ss->cmd.cmd_final_flags;
   setup_command conc_cmd;
   uint32 must_do_mystic = ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_KMASK;
   calldef_schema conc_schema = schema_concentric;
   long_boolean matrix_aware =
         (callflags1 & CFLAG1_12_16_MATRIX_MEANS_SPLIT) &&
         (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX);
   int finalrot = 0;

   uint32 nxnbits =
      ss->cmd.cmd_final_flags.her8it & (INHERITFLAG_NXNMASK|INHERITFLAG_MXNMASK);

   // It will be helpful to have a mask of where the live people are.

   for (i=0, j=1, livemask = 0; i<=setup_attrs[ss->kind].setup_limits; i++, j<<=1) {
      if (ss->people[i].id1) livemask |= j;
   }

   // Take care of "snag" and "mystic".  "Central" is illegal, and was already caught.
   // We first limit it to just the few setups for which it can possibly be legal, to make
   // it easier to test later.

   if (must_do_mystic) {
      switch (ss->kind) {
      case s_rigger:
      case s_qtag:
      case s_bone:
      case s1x8:
      case s2x4:
      case s_crosswave:
         break;
      default:
         fail("Can't do \"snag/mystic\" with this call.");
      }
   }

   switch (ss->kind) {
      int tbi, tbo;

   case s_thar:
      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");
      division_code = MAPCODE(s1x2,4,MPKIND__4_EDGES,1);
      goto divide_us_no_recompute;
   case s2x8:
      // The call has no applicable 2x8 or 8x2 definition.

      // Check whether it has 2x4/4x2/1x8/8x1 definitions, and divide the setup if so,
      // or if the caller explicitly said "2x8 matrix".

      temp =
         (callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) &&
         (ss->cmd.cmd_final_flags.her8it & INHERITFLAG_NXNMASK) != INHERITFLAGNXNK_4X4;

      if (temp ||
          (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_16_MATRIX)) ||
          (ss->cmd.cmd_misc_flags & (CMD_MISC__EXPLICIT_MATRIX|CMD_MISC__PHANTOMS))) {
         if ((!(newtb & 010) || assoc(b_2x4, ss, calldeflist)) &&
             (!(newtb & 001) || assoc(b_4x2, ss, calldeflist))) {
            division_code = MAPCODE(s2x4,2,MPKIND__SPLIT,0);
            /* I consider it an abomination to call such a thing as "2x8 matrix
               swing-o-late" from 2x8 lines, expecting people to do the call
               in split phantom boxes, or "2x8 matrix grand mix", expecting
               twin tidal lines.  However, we allow it for consistency with such
               things as "2x6 matrix double play" from 2x6 columns, expecting
               12 matrix divided columns.  The correct usage should involve the
               explicit concepts "split phantom boxes", "phantom tidal lines",
               or "12 matrix divided columns", as appropriate. */
            /* If database said to split, don't give warning, unless said "4x4". */
            if (!temp) warn(warn__split_to_2x4s);
            goto divide_us_no_recompute;
         }
         else if ((!(newtb & 010) ||
                   assoc(b_1x4, ss, calldeflist) ||
                   assoc(b_1x8, ss, calldeflist)) &&
                  (!(newtb & 001) ||
                   assoc(b_4x1, ss, calldeflist) ||
                   assoc(b_8x1, ss, calldeflist))) {
            division_code = MAPCODE(s1x8,2,MPKIND__SPLIT,1);
            /* See comment above about abomination. */
            /* If database said to split, don't give warning, unless said "4x4". */
            if (!temp) warn(warn__split_to_1x8s);
            goto divide_us_no_recompute;
         }
      }

      /* Otherwise, the only way this can be legal is if we can identify
         smaller setups of all real people and can do the call on them.  For
         example, we will look for 1x4 setups, so we could do things like
         swing thru from a totally offset parallelogram. */

      switch (livemask) {
      case 0xF0F0:    /* a parallelogram */
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_R_FULL,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      case 0x0F0F:    /* a parallelogram */
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_L_FULL,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      case 0xC3C3:    /* the outer quadruple boxes */
         division_code = MAPCODE(s2x2,4,MPKIND__SPLIT,0);
         warn(warn__each2x2);
         goto divide_us_no_recompute;
      }

      {
         // Setup is randomly populated.  See if we have 1x2/1x1 definitions, but no 2x2.
         // If so, divide the 2x8 into 2 2x4's.

         long_boolean forbid_little_stuff =
            assoc(b_2x4, ss, calldeflist) ||
            assoc(b_4x2, ss, calldeflist) ||
            assoc(b_2x3, ss, calldeflist) ||
            assoc(b_3x2, ss, calldeflist) ||
            assoc(b_dmd, ss, calldeflist) ||
            assoc(b_pmd, ss, calldeflist) ||
            assoc(b_qtag, ss, calldeflist) ||
            assoc(b_pqtag, ss, calldeflist);

         if (!forbid_little_stuff &&
             !assoc(b_2x2, ss, calldeflist) &&
             (assoc(b_1x2, ss, calldeflist) ||
              assoc(b_2x1, ss, calldeflist) ||
              assoc(b_1x1, ss, calldeflist))) {
            // Without a lot of examination of facing directions, and whether the call
            // has 1x2 vs. 2x1 definitions, and all that, we don't know which axis
            // to use when dividing it.  But any division into 2 2x4's is safe,
            // and code elsewhere will make the tricky decisions later.
            division_code = MAPCODE(s2x4,2,MPKIND__SPLIT,0);
            goto divide_us_no_recompute;
         }
      }

      fail("You must specify a concept.");
   case s2x6:
      // The call has no applicable 2x6 or 6x2 definition.

      // See if this call has applicable 2x8 definitions and matrix expansion is permitted.
      // If so, that is what we must do.

      if (!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
          (!(newtb & 010) || assoc(b_2x8, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_8x2, ss, calldeflist))) {
         do_matrix_expansion(ss, CONCPROP__NEEDK_2X8, TRUE);
         // Should never fail, but we don't want a loop.
         if (ss->kind != s2x8) fail("Failed to expand to 2X8.");
         return 2;        // And try again.
      }

      // Next, check whether it has 1x3/3x1/2x3/3x2/1x6/6x1 definitions,
      // and divide the setup if so, and if the call permits it.  This is important
      // for permitting "Z axle" from a 2x6 but forbidding "circulate".
      // We also enable this if the caller explicitly said "2x6 matrix".

      temp =
         (callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
         ((callflags1 & CFLAG1_SPLIT_IF_Z) &&
          nxnbits != INHERITFLAGNXNK_3X3 &&
          (livemask == 03333 || livemask == 06666));

      if (temp ||
          (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_12_MATRIX)) ||
          (ss->cmd.cmd_misc_flags & (CMD_MISC__EXPLICIT_MATRIX|CMD_MISC__PHANTOMS))) {
         if ((!(newtb & 010) || assoc(b_2x3, ss, calldeflist)) &&
             (!(newtb & 001) || assoc(b_3x2, ss, calldeflist))) {
            division_code = MAPCODE(s2x3,2,MPKIND__SPLIT,0);
            // See comment above about abomination.
            // If database said to split, don't give warning, unless said "3x3".
            if (!temp) warn(warn__split_to_2x3s);
            goto divide_us_no_recompute;
         }
         else if ((!(newtb & 010) ||
                   assoc(b_1x3, ss, calldeflist) ||
                   assoc(b_1x6, ss, calldeflist)) &&
                  (!(newtb & 001) ||
                   assoc(b_3x1, ss, calldeflist) ||
                   assoc(b_6x1, ss, calldeflist))) {
            division_code = MAPCODE(s1x6,2,MPKIND__SPLIT,1);
            // See comment above about abomination.
            // If database said to split, don't give warning, unless said "3x3".
            if (!temp) warn(warn__split_to_1x6s);
            goto divide_us_no_recompute;
         }
      }

      // Next, check whether it has 1x2/2x1/2x2/1x1 definitions,
      // and we are doing some phantom concept.
      // Divide the setup into 3 boxes if so.

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) ||
          (callflags1 & CFLAG1_SPLIT_LARGE_SETUPS)) {
         if (assoc(b_2x2, ss, calldeflist) ||
             assoc(b_1x2, ss, calldeflist) ||
             assoc(b_2x1, ss, calldeflist) ||
             assoc(b_1x1, ss, calldeflist)) {
            division_code = MAPCODE(s2x2,3,MPKIND__SPLIT,0);
            goto divide_us_no_recompute;
         }
      }

      // Otherwise, the only way this can be legal is if we can identify
      // smaller setups of all real people and can do the call on them.  For
      // example, we will look for 1x4 setups, so we could do things like
      // swing thru from a parallelogram.

      switch (livemask) {
      case 07474:    // a parallelogram
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_R_HALF,1);
         warn(warn__each1x4);
         break;
      case 01717:    // a parallelogram
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_L_HALF,1);
         warn(warn__each1x4);
         break;
      case 06060: case 00303:
      case 06363:    // the outer triple boxes
         division_code = MAPCODE(s2x2,3,MPKIND__SPLIT,0);
         warn(warn__each2x2);
         break;
      case 05555: case 04141: case 02222:
         // Split into 6 stacked 1x2's.
         division_code = MAPCODE(s1x2,6,MPKIND__SPLIT,1);
         warn(warn__each1x2);
         break;
      default:
         fail("You must specify a concept.");
      }

      goto divide_us_no_recompute;
   case spgdmdcw:
      division_code = MAPCODE(sdmd,2,MPKIND__OFFS_R_HALF,1);
      warn(warn__eachdmd);
      goto divide_us_no_recompute;
   case spgdmdccw:
      division_code = MAPCODE(sdmd,2,MPKIND__OFFS_L_HALF,1);
      warn(warn__eachdmd);
      goto divide_us_no_recompute;
   case s2x7:
      // The call has no applicable 2x7 or 7x2 definition.
      // Check for a 75% offset parallelogram,

      switch (livemask) {
      case 0x3C78:
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_R_THRQ,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      case 0x078F:
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_L_THRQ,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      }

      break;
   case sdeepqtg:
         /* Check whether it has short6/pshort6 definitions, and divide the setup if so,
            and if the call permits it. */

         if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
             (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
            if ((!(newtb & 010) || assoc(b_short6, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_pshort6, ss, calldeflist))) {
               division_code = MAPCODE(s_short6,2,MPKIND__SPLIT,0);
               goto divide_us_no_recompute;
            }
         }
         break;
      case s4x5:
         // **** actually want to test that call says "occupied_as_3x1tgl".

         if ((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
             (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
            if ((!(newtb & 010) || assoc(b_2x3, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_3x2, ss, calldeflist))) {
               if (livemask == 0x3A0E8 || livemask == 0x1705C) {
                  division_code = spcmap_tgl451;
                  goto divide_us_no_recompute;
               }
               else if (livemask == 0x41D07 || livemask == 0xE0B82) {
                  division_code = spcmap_tgl452;
                  goto divide_us_no_recompute;
               }
            }
         }

         break;
      case s1p5x8:
         /* This is a phony setup, allowed only so that we can have people temporarily
            in 50% offset 1x4's that are offset the impossible way. */

         switch (livemask) {
            case 0xF0F0:
               division_code = MAPCODE(s1x4,2,MPKIND__OFFS_L_HALF,0);
               break;
            case 0x0F0F:
               division_code = MAPCODE(s1x4,2,MPKIND__OFFS_R_HALF,0);
               break;
            default:
               fail("You must specify a concept.");
         }

         goto divide_us_no_recompute;
      case s1x12:
         /* The call has no applicable 1x12 or 12x1 definition. */

         /* See if this call has applicable 1x16 definitions and matrix expansion is permitted.
            If so, that is what we must do. */

         if (  !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
               (!(newtb & 010) || assoc(b_1x16, ss, calldeflist)) &&
               (!(newtb & 001) || assoc(b_16x1, ss, calldeflist))) {
            do_matrix_expansion(ss, CONCPROP__NEEDK_1X16, TRUE);
            if (ss->kind != s1x16) fail("Failed to expand to 1X16.");  /* Should never fail, but we don't want a loop. */
            return 2;        /* And try again. */
         }

         /* Check whether it has 1x6/6x1 definitions, and divide the setup if so,
            and if the caller explicitly said "1x12 matrix". */

         temp = (callflags1 & CFLAG1_SPLIT_LARGE_SETUPS);

         if (temp ||
             (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_12_MATRIX)) ||
             (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
            if ((!(newtb & 010) || assoc(b_1x6, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_6x1, ss, calldeflist))) {
               division_code = MAPCODE(s1x6,2,MPKIND__SPLIT,0);
               /* See comment above about abomination. */
               /* If database said to split, don't give warning. */
               if (!temp) warn(warn__split_to_1x6s);
               goto divide_us_no_recompute;
            }
            else if ((!(newtb & 010) || assoc(b_1x3, ss, calldeflist)) &&
                     (!(newtb & 001) || assoc(b_3x1, ss, calldeflist))) {
               division_code = MAPCODE(s1x3,4,MPKIND__SPLIT,0);
               /* See comment above about abomination. */
               warn(warn__split_1x6);
               goto divide_us_no_recompute;
            }
         }

         /* Otherwise, the only way this can be legal is if we can identify
            smaller setups of all real people and can do the call on them. */

         switch (livemask) {
            case 01717:    /* outer 1x4's */
               division_code = MAPCODE(s1x4,3,MPKIND__SPLIT,0);
               warn(warn__each1x4);
               break;
            case 06363:    /* center 1x4 and outer 1x2's */
               division_code = MAPCODE(s1x2,6,MPKIND__SPLIT,0);
               warn(warn__each1x2);
               break;
            default:
               fail("You must specify a concept.");
         }

         goto divide_us_no_recompute;
      case s1x16:
         // The call has no applicable 1x16 or 16x1 definition.

         // Check whether it has 1x8/8x1 definitions, and divide the setup if so,
         // and if the caller explicitly said "1x16 matrix".

         temp = (callflags1 & CFLAG1_SPLIT_LARGE_SETUPS);

         if (temp ||
             (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_16_MATRIX)) ||
             (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) {
            if ((!(newtb & 010) || assoc(b_1x8, ss, calldeflist)) &&
                (!(newtb & 001) || assoc(b_8x1, ss, calldeflist))) {
               division_code = MAPCODE(s1x8,2,MPKIND__SPLIT,0);
               // See comment above about abomination.
               // If database said to split, don't give warning.
               if (!temp) warn(warn__split_to_1x8s);
               goto divide_us_no_recompute;
            }
         }
         break;
   case s1x10:
      // See if this call has applicable 1x12 or 1x16 definitions and
      // matrix expansion is permitted.  If so, that is what we must do.
      // These two cases are required to make things like 12 matrix
      // grand swing thru work from a 1x10.

      if (!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
          (!(newtb & 010) || assoc(b_1x12, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_12x1, ss, calldeflist))) {
         do_matrix_expansion(ss, CONCPROP__NEEDK_1X12, TRUE);
         if (ss->kind != s1x12) fail("Failed to expand to 1X12.");  // Should never fail, but we don't want a loop.
         return 2;        // And try again.
      }
      // FALL THROUGH!!!!!
   case s1x14:      // WARNING!!  WE FELL THROUGH!!
      /* See if this call has applicable 1x16 definitions and matrix expansion is permitted.
            If so, that is what we must do. */

      if (  !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
            (!(newtb & 010) || assoc(b_1x16, ss, calldeflist)) &&
            (!(newtb & 001) || assoc(b_16x1, ss, calldeflist))) {
         do_matrix_expansion(ss, CONCPROP__NEEDK_1X16, TRUE);
         if (ss->kind != s1x16) fail("Failed to expand to 1X16.");  // Should never fail, but we don't want a loop.
         return 2;        // And try again.
      }

      if (ss->kind == s1x10) {   /* Can only do this in 1x10, for now. */
         division_code = MAPCODE(s1x2,5,MPKIND__SPLIT,0);

         if (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) {
            if (((newtb & 001) == 0 && assoc(b_1x2, ss, calldeflist)) ||
                ((newtb & 010) == 0 && assoc(b_2x1, ss, calldeflist)))
               goto divide_us_no_recompute;
         }
         else if (assoc(b_1x1, ss, calldeflist))
            goto divide_us_no_recompute;
      }

      break;
   case s3x6:
      /* Check whether it has 2x3/3x2/1x6/6x1 definitions, and divide the setup if so,
            or if the caller explicitly said "3x6 matrix" (not that "3x6 matrix"
            exists at present.) */

         /* We do *NOT* use the "CFLAG1_SPLIT_LARGE_SETUPS" flag.
            We are willing to split to a 12 matrix, but not an 18 matrix. */

      if (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) {
         if ((!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
             (!(newtb & 001) || assoc(b_2x3, ss, calldeflist))) {
            division_code = MAPCODE(s2x3,3,MPKIND__SPLIT,1);
            warn(warn__split_to_2x3s);
            goto divide_us_no_recompute;
         }
         else if ((!(newtb & 010) || assoc(b_1x6, ss, calldeflist)) &&
                  (!(newtb & 001) || assoc(b_6x1, ss, calldeflist))) {
            division_code = MAPCODE(s1x6,3,MPKIND__SPLIT,1);
            warn(warn__split_to_1x6s);
            goto divide_us_no_recompute;
            /* YOW!!  1x3's are hard!  We need a 3x3 formation. */
         }
      }

      /* Otherwise, the only way this can be legal is if we can identify
         smaller setups of all real people and can do the call on them.  For
         example, we will look for 1x2 setups, so we could trade in
         individual couples scattered around. */

      switch (livemask) {
      case 0505505:
      case 0702702:
      case 0207207:
         warn(warn__each1x2);
      case 0603603:
      case 0306306:
         division_code = MAPCODE(s2x3,2,MPKIND__OFFS_R_HALF,0);
         break;
      case 0550550:
      case 0720720:
      case 0270270:
         warn(warn__each1x2);
      case 0660660:
      case 0330330:
         division_code = MAPCODE(s2x3,2,MPKIND__OFFS_L_HALF,0);
         break;
      default:
         fail("You must specify a concept.");
      }

      goto divide_us_no_recompute;
   case s_c1phan:

      /* Check for "twisted split" stuff. */

      if ((TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_TWISTED)) &&
          (ss->cmd.cmd_final_flags.final &
           (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED)) &&
          (livemask == 0xAAAA || livemask == 0x5555)) {
         finalrot = newtb & 1;
         map_kind = (livemask & 1) ? MPKIND__SPLIT : MPKIND__NONISOTROP1;
         division_code = MAPCODE(s_trngl4,2,map_kind,0);
         goto divide_us_no_recompute;
      }

      /* The only way this can be legal is if people are in genuine
         C1 phantom spots and the call can be done from 1x2's or 2x1's.
         *** Actually, that isn't so.  We ought to be able to do 1x1 calls
         from any population at all. */

      if ((livemask & 0xAAAA) == 0)
         division_code = MAPCODE(s1x2,4,MPKIND__4_QUADRANTS,0);
      else if ((livemask & 0x5555) == 0)
         division_code = MAPCODE(s1x2,4,MPKIND__4_QUADRANTS,1);
      else if (livemask == 0x0F0F || livemask == 0xF0F0)
         division_code = MAPCODE(s_star,4,MPKIND__4_QUADRANTS,0);
      else if (   (livemask & 0x55AA) == 0 ||
                  (livemask & 0xAA55) == 0 ||
                  (livemask & 0x5AA5) == 0 ||
                  (livemask & 0xA55A) == 0) {
         setup scopy;
         setup the_results[2];

         /* This is an unsymmetrical thing.  Do each quadrant (a 1x2) separately by
               using both maps, and then merge the result and hope for the best. */

         ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT_MASK;
         scopy = *ss;    /* "Move" can write over its input. */
         divided_setup_move(ss, MAPCODE(s1x2,4,MPKIND__4_QUADRANTS,0),
                            phantest_ok, FALSE, &the_results[0]);
         divided_setup_move(&scopy, MAPCODE(s1x2,4,MPKIND__4_QUADRANTS,1),
                            phantest_ok, FALSE, &the_results[1]);
         *result = the_results[1];
         result->result_flags = get_multiple_parallel_resultflags(the_results, 2);
         merge_setups(&the_results[0], merge_c1_phantom, result);
         return 1;
      }
      else
         fail("You must specify a concept.");

      goto divide_us_no_recompute;
   case sbigdmd:

      /* The only way this can be legal is if people are in genuine "T" spots. */

      switch (livemask) {
      case 01717:
         division_code = MAPCODE(s_trngl4,2,MPKIND__OFFS_L_HALF,0);
         break;
      case 07474:
         division_code = MAPCODE(s_trngl4,2,MPKIND__OFFS_R_HALF,0);
         break;
      default:
         fail("You must specify a concept.");
      }

      goto divide_us_no_recompute;
   case s2x2:
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

      /* Any 2x2 -> 2x2 call that acts by dividing itself into 1x2's
         is presumed to want the people in each 1x2 to stay near each other.
         We signify that by reverting to the original elongation,
         overriding anything that may have been in the call definition. */

      /* Tentatively choose a map.  We may change it later to "map_2x2v". */
      division_code = MAPCODE(s1x2,2,MPKIND__SPLIT,1);

      if ((newtb & 011) == 011) {
         /* The situation is serious.  If the call has both 1x2 and 2x1 definitions,
            we can do it, by guessing correctly which way to divide the setup.
            Otherwise, if it has either a 1x2 or a 2x1 definition, but not both,
            we lose, because the call presumably wants to use same.
            But if the call has neither 1x2 nor 2x1 definitions, but does have
            a 1x1 definition, we can do it.  Just divide the setup arbitrarily. */

         long_boolean losing = FALSE;

         if (assoc(b_2x1, ss, calldeflist)) {
            if (assoc(b_1x2, ss, calldeflist)) {
               /* The call has both definitions.
                  If the "lateral_to_selectees" flag is on (that is, the call is "run"),
                  We decide what to do according to the direction of the selectees.
                  There must be at least one, they must be collectively consistent.
                  Otherwise, we look at the manner in which the setup is T-boned
                  in order to figure out how to divide the setup. */

               if (calldeflist->callarray_flags & CAF__LATERAL_TO_SELECTEES) {
                  uint32 selmask = 0;

                  for (i=0 ; i<4 ; i++) if (selectp(ss, i)) selmask |= ss->people[i].id1;

                  if (selmask == 0 || (selmask & 011) == 011)
                     fail("People are not working with each other in a consistent way.");
                  else if (selmask & 1)
                     { division_code = spcmap_2x2v; }
               }
               else {
                  if ((((ss->people[0].id1 | ss->people[3].id1) & 011) != 011) &&
                      (((ss->people[1].id1 | ss->people[2].id1) & 011) != 011))
                     { division_code = spcmap_2x2v; }
                  else if ((((ss->people[0].id1 | ss->people[1].id1) & 011) == 011) ||
                           (((ss->people[2].id1 | ss->people[3].id1) & 011) == 011))
                     fail("Can't figure out who should be working with whom.");
               }

               goto divide_us_no_recompute;
            }
            else
               losing = TRUE;
         }
         else if (assoc(b_1x2, ss, calldeflist))
            losing = TRUE;
         else if (assoc(b_1x1, ss, calldeflist))
            goto divide_us_no_recompute;

         if (losing) {
            if (ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS)
               fail("Sorry, should have people do their own part, but don't know how.");
            else
               fail("People are not working with each other in a consistent way.");
         }
      }
      else {
         /* People are not T-boned.  Check for a 2x1 or 1x2 definition.
            If found, use it as a guide.  If both are present, we use
            the setup elongation flag to tell us what to do.  In any
            case, the setup elongation flag, if present, must not be
            inconsistent with our decision. */

         unsigned long int elong = 0;

            // If this is "run" and people aren't T-boned, just ignore the 2x1 definition.

         if (!(calldeflist->callarray_flags & CAF__LATERAL_TO_SELECTEES) &&
             assoc(b_2x1, ss, calldeflist))
            elong |= (2 - (newtb & 1));

         if (assoc(b_1x2, ss, calldeflist)) {
            // If "split_to_box" is on, prefer the 2x1 definition.
            if (!(calldeflist->callarray_flags & CAF__SPLIT_TO_BOX) || elong == 0)
               elong |= (1 + (newtb & 1));
         }

         if (elong == 0) {
            // Neither 2x1 or 1x2 definition existed.  Check for 1x1.
            // If found, any division axis will work.
            if (assoc(b_1x1, ss, calldeflist))
               goto divide_us_no_recompute;
         }
         else {
            unsigned long int foo = (ss->cmd.prior_elongation_bits | ~elong) & 3;

            if (foo == 0) {
               fail("Can't figure out who should be working with whom.");
            }
            else if (foo == 3) {
               // We are in trouble if CMD_MISC__NO_CHK_ELONG is off.
               // But, if there was a 1x1 definition, we allow it anyway.
               // This is what makes "you all" and "the K" legal from lines.
               // The "U-turn-back" is done in a 2x2 that is elongated laterally.
               // "U-turn-back" has a 1x2 definition (so that you can roll) but
               // no 2x1 definition (because, from a 2x2, it might overtake the 1x2
               // definition, in view of the fact that there is no definite priority
               // for searching for definitions, which could make people unable to
               // roll during certain phases of the moon.)  So, when we are having the
               // ends U-turn-back while in parallel lines, their 1x2's appear to be
               // illegally separated.  Since they could have done it in 1x1's,
               // we allow it.  And, incidentally, we allow a roll afterwards.

               if (!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_CHK_ELONG) &&
                   !assoc(b_1x1, ss, calldeflist))
                  fail("People are too far apart to work with each other on this call.");

               foo ^= elong;
            }

            if (foo == 1)
               division_code = spcmap_2x2v;

            goto divide_us_no_recompute;
         }
      }

      break;
   case s_rigger:
      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");

      if ((final_concepts.final &
           (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED)) ||
          assoc(b_trngl4, ss, calldeflist) ||
          assoc(b_ptrngl4, ss, calldeflist)) {
         division_code = MAPCODE(s_trngl4,2,MPKIND__SPLIT, 1);
         goto divide_us_no_recompute;
      }

      if (must_do_mystic)
         goto do_mystically;

      {
         uint32 tinytb =
            ss->people[2].id1 | ss->people[3].id1 |
            ss->people[6].id1 | ss->people[7].id1;

         // See if this call has applicable 1x2 or 2x1 definitions,
         // and the people in the wings are facing appropriately.
         // Then do it concentrically, which will break it into 4-person triangles
         // first and 1x2/2x1's later.  If it has a 1x1 definition,
         // do it no matter how people are facing.

         if ((!(tinytb & 010) || assoc(b_1x2, ss, calldeflist)) &&
             (!(tinytb & 1) || assoc(b_2x1, ss, calldeflist)))
            goto do_concentrically;

         if (assoc(b_1x1, ss, calldeflist))
            goto do_concentrically;
      }
      break;
   case s3x4:
      if (handle_3x4_division(ss, callflags1, newtb, livemask,
                              &division_code,
                              calldeflist, matrix_aware, result))
         goto divide_us_no_recompute;
      return 1;
   case s4x4:
      if (handle_4x4_division(ss, callflags1, newtb, livemask,
                              division_code,
                              finalrot,
                              calldeflist, matrix_aware))
         goto divide_us_no_recompute;
      break;
   case s4x6:
      if (handle_4x6_division(ss, callflags1, newtb, livemask,
                              &division_code,
                              calldeflist, matrix_aware))
         goto divide_us_no_recompute;
      break;
   case s3x8:
      if (handle_3x8_division(ss, callflags1, newtb, livemask,
                              &division_code,
                              calldeflist, matrix_aware))
         goto divide_us_no_recompute;
      break;
   case s2x12:
      if (handle_2x12_division(ss, callflags1, newtb, livemask,
                              &division_code,
                              calldeflist, matrix_aware))
         goto divide_us_no_recompute;
      break;
   case s_qtag:
      if (assoc(b_dmd, ss, calldeflist) ||
          assoc(b_pmd, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist) ||
          assoc(b_1x4, ss, calldeflist)) {
         division_code = MAPCODE(sdmd,2,MPKIND__SPLIT,1);
         goto divide_us_no_recompute;
      }

      /* Check whether it has 2x3/3x2 definitions, and divide the setup if so,
         and if the call permits it.  This is important for permitting "Z axle" from
         a 3x4 but forbidding "circulate" (unless we give a concept like 12 matrix
         phantom columns.)  We also enable this if the caller explicitly said
         "3x4 matrix". */

      if (((callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
           (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) ||
           nxnbits == INHERITFLAGMXNK_1X3 ||
           nxnbits == INHERITFLAGMXNK_3X1 ||
           nxnbits == INHERITFLAGNXNK_3X3) &&
          (!(newtb & 010) || assoc(b_3x2, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_2x3, ss, calldeflist))) {
         division_code = spcmap_qtag_2x3;
         goto divide_us_no_recompute;
      }

      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");

      if (must_do_mystic)
         goto do_mystically;

      if ((!(newtb & 010) || assoc(b_1x2, ss, calldeflist)) &&
          (!(newtb & 1) || assoc(b_2x1, ss, calldeflist))) {
         goto do_concentrically;
      }
      else if ((livemask & 0x55) == 0) {
         /* Check for stuff like "heads pass the ocean; side corners only slide thru". */
         division_code = spcmap_qtag_f1;
         goto divide_us_no_recompute;
      }
      else if ((livemask & 0x66) == 0) {
         division_code = spcmap_qtag_f2;
         goto divide_us_no_recompute;
      }

      /* ******** Regression test OK to here.  The next thing doesn't work, because the
         "do your part" junk doesn't work right!  This has been a known problem for some
         time, of course.  It's about time it got fixed.  It appears to have been broken
         as far back as 27.8. */

      else if ((livemask & 0x77) == 0) {
         /* Check for stuff like "center two slide thru". */
         division_code = spcmap_qtag_f0;
         goto divide_us_no_recompute;
      }

      break;
   case s_2stars:
      if (assoc(b_star, ss, calldeflist)) {
         division_code = MAPCODE(s_star,2,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }

      break;
   case s_bone:
      division_code = MAPCODE(s_trngl4,2,MPKIND__NONISOTROP1,1);
      temp_for_2x2 = assoc(b_trngl4, ss, calldeflist) ||
         assoc(b_ptrngl4, ss, calldeflist);

      if (((ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_HORIZ) && !(ss->rotation & 1)) ||
          ((ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_VERT) && (ss->rotation & 1))) {

         if (temp_for_2x2)
            goto divide_us_no_recompute;
      }

      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");

      if (temp_for_2x2) goto divide_us_no_recompute;

      /* See if this call is being done "split" as in "split square thru" or
         "split dixie style", in which case split into triangles.
         (Presumably there is a "twisted" somewhere.) */

      if ((final_concepts.final &
           (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))) {
         goto divide_us_no_recompute;
      }

      if (must_do_mystic)
         goto do_mystically;

      {
         uint32 tbi = ss->people[2].id1 | ss->people[3].id1 |
            ss->people[6].id1 | ss->people[7].id1;
         uint32 tbo = ss->people[0].id1 | ss->people[1].id1 |
            ss->people[4].id1 | ss->people[5].id1;

         /* See if this call has applicable 1x2 or 2x1 definitions,
            and the people in the center 1x4 are facing appropriately.
            Then do it concentrically, which will break it into 4-person
            triangles first and 1x2/2x1's later.  If it has a 1x1 definition,
            do it no matter how people are facing. */

         if ((!((tbi & 010) | (tbo & 001)) || assoc(b_1x2, ss, calldeflist)) &&
             (!((tbi & 001) | (tbo & 010)) || assoc(b_2x1, ss, calldeflist)))
            goto do_concentrically;

         if (assoc(b_1x1, ss, calldeflist))
            goto do_concentrically;
      }

      /* Turn a bone with only the center line occupied into a 1x8. */
      if (livemask == 0xCC &&
          (!(newtb & 010) || assoc(b_1x8, ss, calldeflist)) &&
          (!(newtb & 1) || assoc(b_8x1, ss, calldeflist))) {
         ss->kind = s1x8;
         swap_people(ss, 2, 7);
         swap_people(ss, 3, 6);
         return 2;                        /* And try again. */
      }
      break;
   case s_ptpd:
      if (assoc(b_dmd, ss, calldeflist) ||
          assoc(b_pmd, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist)) {
         division_code = MAPCODE(sdmd,2,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }
      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");
      break;
   case s2x3:
      /* See if this call has applicable 1x2 or 2x1 definitions,
         in which case split it 3 ways. */
      if (((!(newtb & 010) || assoc(b_2x1, ss, calldeflist)) &&
           (!(newtb & 001) || assoc(b_1x2, ss, calldeflist))) ||
          assoc(b_1x1, ss, calldeflist)) {
         division_code = MAPCODE(s1x2,3,MPKIND__SPLIT,1);
         goto divide_us_no_recompute;
      }

      // See if this call has applicable 1x3 or 3x1 definitions,
      // in which case split it 2 ways.
      if ((!(newtb & 010) || assoc(b_1x3, ss, calldeflist)) &&
          (!(newtb & 001) || assoc(b_3x1, ss, calldeflist))) {
         division_code = MAPCODE(s1x3,2,MPKIND__SPLIT,1);
         goto divide_us_no_recompute;
      }

      // See if people only occupy Z-like spots.  Maybe this isn't good enough.

      if (livemask == 033 &&
          (!(newtb & 010) || assoc(b_1x2, ss, calldeflist)) &&
          (!(newtb & 1) || assoc(b_2x1, ss, calldeflist))) {
         division_code = MAPCODE(s1x2,2,MPKIND__OFFS_L_HALF,1);
         goto divide_us_no_recompute;
      }
      else if (livemask == 066 &&
               (!(newtb & 010) || assoc(b_1x2, ss, calldeflist)) &&
               (!(newtb & 1) || assoc(b_2x1, ss, calldeflist))) {
         division_code = MAPCODE(s1x2,2,MPKIND__OFFS_R_HALF,1);
         goto divide_us_no_recompute;
      }
      else if (livemask == 036 &&
               (!(newtb & 010) || assoc(b_1x2, ss, calldeflist)) &&
               (!(newtb & 1) || assoc(b_2x1, ss, calldeflist))) {
         division_code = spcmap_2x3_1234;
         goto divide_us_no_recompute;
      }
      else if (livemask == 063 &&
               (!(newtb & 010) || assoc(b_1x2, ss, calldeflist)) &&
               (!(newtb & 1) || assoc(b_2x1, ss, calldeflist))) {
         division_code = spcmap_2x3_0145;
         goto divide_us_no_recompute;
      }

      break;
   case s_short6:
      if (assoc(b_trngl, ss, calldeflist) || assoc(b_ptrngl, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist) || assoc(b_2x2, ss, calldeflist)) {
         division_code = MAPCODE(s_trngl,2,MPKIND__SPLIT,1);
         goto divide_us_no_recompute;
      }
      break;
   case s_ntrgl6cw:
   case s_ntrgl6ccw:
      if (assoc(b_trngl, ss, calldeflist) || assoc(b_ptrngl, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist) || assoc(b_2x2, ss, calldeflist)) {
         division_code = (ss->kind == s_ntrgl6cw) ?
            MAPCODE(s_trngl,2,MPKIND__NONISOTROP1,0):
            MAPCODE(s_trngl,2,MPKIND__SPLIT,0);
         //         ss->cmd.cmd_final_flags.final |= FINAL__TRIANGLE;
         goto divide_us_no_recompute;
      }
      break;
   case s_bone6:
      if (assoc(b_trngl, ss, calldeflist) || assoc(b_ptrngl, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist) || assoc(b_2x2, ss, calldeflist)) {
         division_code = MAPCODE(s_trngl,2,MPKIND__NONISOTROP1,1);
         goto divide_us_no_recompute;
      }

      {
         uint32 tbi = ss->people[2].id1 | ss->people[5].id1;
         uint32 tbo =
            ss->people[0].id1 | ss->people[1].id1 |
            ss->people[3].id1 | ss->people[4].id1;

         if (assoc(b_1x1, ss, calldeflist) ||
             ((!((tbi & 010) | (tbo & 001)) || assoc(b_1x2, ss, calldeflist)) &&
              (!((tbi & 001) | (tbo & 010)) || assoc(b_2x1, ss, calldeflist)))) {
            conc_schema = schema_concentric_6p;
            goto do_concentrically;
         }
      }

      break;
   case s_1x2dmd:
      {
         uint32 tbi = ss->people[2].id1 | ss->people[5].id1;
         uint32 tbo = ss->people[0].id1 | ss->people[1].id1 |
            ss->people[3].id1 | ss->people[4].id1;

         if (assoc(b_1x1, ss, calldeflist) ||
             ((!((tbi & 010) | (tbo & 001)) || assoc(b_2x1, ss, calldeflist)) &&
              (!((tbi & 001) | (tbo & 010)) || assoc(b_1x2, ss, calldeflist)))) {
            conc_schema = schema_concentric_6p;
            goto do_concentrically;
         }
      }
      break;
   case sdeepxwv:
      if (assoc(b_1x1, ss, calldeflist) ||
          assoc(b_2x1, ss, calldeflist) ||
          assoc(b_1x2, ss, calldeflist)) {
         conc_schema = schema_concentric_8_4;
         goto do_concentrically;
      }
      break;
   case s_trngl:
      if (assoc(b_2x2, ss, calldeflist)) {
         uint32 leading = final_concepts.final;

         if (ss->cmd.cmd_misc_flags & CMD_MISC__SAID_TRIANGLE) {
            if (final_concepts.final & FINAL__TRIANGLE)
               fail("'Triangle' concept is redundant.");
         }
         else {
            if (!(final_concepts.final & (FINAL__TRIANGLE|FINAL__LEADTRIANGLE)))
               fail("You must give the 'triangle' concept.");
         }

         if ((ss->people[0].id1 & d_mask) == d_east)
            leading = ~leading;
         else if ((ss->people[0].id1 & d_mask) != d_west)
            fail("Can't figure out which way triangle point is facing.");

         division_code = (leading & FINAL__LEADTRIANGLE) ? spcmap_trngl_box1 : spcmap_trngl_box2;

         final_concepts.final &= ~(FINAL__TRIANGLE|FINAL__LEADTRIANGLE);
         ss->cmd.cmd_final_flags = final_concepts;
         divided_setup_move(ss, division_code, phantest_ok, FALSE, result);
         result->result_flags |= RESULTFLAG__DID_TGL_EXPANSION;
         result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
         return 1;
      }
      break;
   case s1x6:
      // See if this call has a 1x2, 2x1, or 1x1 definition, in which case split it 3 ways.
      if (assoc(b_1x2, ss, calldeflist) ||
          assoc(b_2x1, ss, calldeflist) ||
          assoc(b_1x1, ss, calldeflist)) {
         division_code = MAPCODE(s1x2,3,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }
      // If it has 1x3 or 3x1 definitions, split it 2 ways.
      if (assoc(b_1x3, ss, calldeflist) || assoc(b_3x1, ss, calldeflist)) {
         division_code = MAPCODE(s1x3,2,MPKIND__SPLIT,0);
         // We want to be sure that the operator knows what we are doing, and why,
         // if we divide a 1x6 into 1x3's.  We allow "swing thru" in a wave of
         // 3 or 4 people.  If the operator wants to do a swing thru with
         // all 6 people, use "grand swing thru".
         warn(warn__split_1x6);
         goto divide_us_no_recompute;
      }
      break;
   case s1x2:
      // See if the call has a 1x1 definition, in which case split it and do each part.
      if (assoc(b_1x1, ss, calldeflist)) {
         division_code = MAPCODE(s1x1,2,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }
      break;
   case s1x3:
      /* See if the call has a 1x1 definition, in which case split it and do each part. */
      if (assoc(b_1x1, ss, calldeflist)) {
         division_code = MAPCODE(s1x1,3,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }
      break;
   case sdmd:
      /* See if the call has a 1x1 definition, in which case split it and do each part. */
      if (assoc(b_1x1, ss, calldeflist)) {
         division_code = spcmap_dmd_1x1;
         goto divide_us_no_recompute;
      }
      break;
   case s_star:
      /* See if the call has a 1x1 definition, in which case split it and do each part. */
      if (assoc(b_1x1, ss, calldeflist)) {
         division_code = spcmap_star_1x1;
         goto divide_us_no_recompute;
      }
      break;
   case s1x8:
      division_code = MAPCODE(s1x4,2,MPKIND__SPLIT,0);

      /* See if the call has a 1x4, 4x1, 1x2, 2x1, or 1x1 definition, in which case split it and do each part. */
      if (     assoc(b_1x4, ss, calldeflist) || assoc(b_4x1, ss, calldeflist)) {
         goto divide_us_no_recompute;
      }

      if (must_do_mystic)
         goto do_mystically;

      if (     assoc(b_1x2, ss, calldeflist) || assoc(b_2x1, ss, calldeflist) ||
               assoc(b_1x1, ss, calldeflist)) {
         goto divide_us_no_recompute;
      }

      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");

      /* See if this call has applicable 1x12 or 1x16 definitions and matrix expansion is permitted.
         If so, that is what we must do. */

      if (  !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
            (!(newtb & 010) || assoc(b_1x12, ss, calldeflist) || assoc(b_1x16, ss, calldeflist)) &&
            (!(newtb & 1) || assoc(b_12x1, ss, calldeflist) || assoc(b_16x1, ss, calldeflist))) {
         do_matrix_expansion(ss, CONCPROP__NEEDK_1X12, TRUE);
         if (ss->kind != s1x12) fail("Failed to expand to 1X12.");  /* Should never fail, but we don't want a loop. */
         return 2;                        /* And try again. */
      }

      /* We might be doing some kind of "own the so-and-so" operation in which people who are ends of
         each wave in a 1x8 want to think they are points of diamonds instead.  This could happen,
         for example, with point-to-point diamonds if we say "own the <points>, flip the diamond by
         flip the diamond".  Yes, it's stupid.  Now normalize_setup turned the centerless diamonds
         into a 1x8 (it needs to do that in order for "own the <points>, trade by flip the diamond"
         to work.  We must turn that 1x8 back into diamonds.  The "own the so-and-so" concept turns
         on CMD_MISC__PHANTOMS.  If this flag weren't on, we would have no business saying "I see
         phantoms in the center 2 spots of my wave, I'm allowed to think of this as a diamond."
         The same thing is done below for 2x4's and 1x4's. */

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) &&
          (ss->people[1].id1 | ss->people[3].id1 |
           ss->people[5].id1 | ss->people[7].id1) == 0) {
         setup sstest = *ss;
         sstest.kind = s_ptpd;

         if ((!(newtb & 010) ||
              assoc(b_ptpd, &sstest, calldeflist) ||
              assoc(b_dmd, &sstest, calldeflist)) &&
             (!(newtb & 001) ||
              assoc(b_pptpd, &sstest, calldeflist) ||
              assoc(b_pmd, &sstest, calldeflist))) {
            *ss = sstest;
            return 2;
         }
      }
      else if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) &&
               (ss->people[0].id1 | ss->people[1].id1 |
                ss->people[4].id1 | ss->people[5].id1) == 0) {
         setup sstest = *ss;
         swap_people(&sstest, 2, 7);
         swap_people(&sstest, 3, 6);
         sstest.kind = s_qtag;

         if ((!(newtb & 010) ||
              assoc(b_qtag, &sstest, calldeflist) ||
              assoc(b_pmd, &sstest, calldeflist)) &&
             (!(newtb & 001) ||
              assoc(b_pqtag, &sstest, calldeflist) ||
              assoc(b_dmd, &sstest, calldeflist))) {
            *ss = sstest;
            return 2;
         }
      }

      break;
   case s_crosswave:
      if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
         fail("Can't split the setup.");

      /* If we do not have a 1x4 or 4x1 definition, but we have 1x2, 2x1, or 1x1 definitions,
         do the call concentrically.  This will have the effect of having each miniwave do it.
         If we did this when a 1x4 or 4x1 definition existed, it would have the effect of having
         the people in the outer, disconnected, 1x4 work with each other across the set, which
         we do not want. */

      if (must_do_mystic)
         goto do_mystically;

      if (!assoc(b_4x1, ss, calldeflist) && !assoc(b_1x4, ss, calldeflist) &&
          (assoc(b_2x1, ss, calldeflist) || assoc(b_1x2, ss, calldeflist) || assoc(b_1x1, ss, calldeflist)))
         goto do_concentrically;

      break;
   case s2x4:
      division_code = MAPCODE(s2x2,2,MPKIND__SPLIT,0);    /* The map we will probably use. */

      /* See if this call is being done "split" as in "split square thru" or
         "split dixie style", in which case split into boxes. */

      if (final_concepts.final & (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))
         goto divide_us_no_recompute;

      /* If this is "run", always split it into boxes.  If they are T-boned,
         they will figure it out, we hope. */

      if (calldeflist->callarray_flags & CAF__LATERAL_TO_SELECTEES)
         goto divide_us_no_recompute;

      /* See if this call has applicable 2x6 or 2x8 definitions and matrix expansion
         is permitted.  If so, that is what we must do.  But if it has a 4x4 definition
         also, it is ambiguous, so we can't do it. */

      if (!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX) &&
          !assoc(b_4x4, ss, calldeflist) &&
          (!(newtb & 010) ||
           assoc(b_2x6, ss, calldeflist) ||
           assoc(b_2x8, ss, calldeflist)) &&
          (!(newtb & 1) ||
           assoc(b_6x2, ss, calldeflist) ||
           assoc(b_8x2, ss, calldeflist))) {

         if (must_do_mystic)
            fail("Can't do \"snag/mystic\" with this call.");

         do_matrix_expansion(ss, CONCPROP__NEEDK_2X6, TRUE);

         /* Should never fail, but we don't want a loop. */
         if (ss->kind != s2x6) fail("Failed to expand to 2X6.");

         return 2;                        /* And try again. */
      }

      /* If we are splitting for "central", "crazy", or "splitseq",
         give preference to 2x2 splitting.  Also give preference
         if the "split_to_box" flag was given. */

      temp_for_2x2 = TRUE;

      if (((ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_HORIZ) && !(ss->rotation & 1)) ||
          ((ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_VERT) && (ss->rotation & 1)) ||
          (calldeflist->callarray_flags & CAF__SPLIT_TO_BOX)) {
         if (assoc(b_2x2, ss, calldeflist))
            goto divide_us_no_recompute;
         temp_for_2x2 = FALSE;    /* So we don't waste time computing it again. */
      }

      /* See if this call has applicable 1x4 or 4x1 definitions,
         in which case split it that way. */

      if ((!(newtb & 010) || assoc(b_1x4, ss, calldeflist)) &&
          (!(newtb & 1) || assoc(b_4x1, ss, calldeflist))) {
         division_code = MAPCODE(s1x4,2,MPKIND__SPLIT,1);
         goto divide_us_no_recompute;
      }

      /* See if this call has applicable 2x2 definition, in which case split into boxes. */

      if (temp_for_2x2 && assoc(b_2x2, ss, calldeflist)) goto divide_us_no_recompute;

      if (must_do_mystic)
         goto do_mystically;

      /* See long comment above for s1x8.  The test cases for this are
         "own the <points>, trade by flip the diamond", and
         "own the <points>, flip the diamond by flip the diamond". */

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) &&
          (ss->people[1].id1 | ss->people[2].id1 |
           ss->people[5].id1 | ss->people[6].id1) == 0) {
         uint32 tbtest;
         setup sstest = *ss;

         expand_setup(&comp_qtag_2x4_stuff, &sstest);

         tbtest =
            sstest.people[0].id1 | sstest.people[1].id1 |
            sstest.people[4].id1 | sstest.people[5].id1;

         if ((!(tbtest & 010) ||
              assoc(b_qtag, &sstest, calldeflist) ||
              assoc(b_pmd, &sstest, calldeflist)) &&
             (!(tbtest & 001) ||
              assoc(b_pqtag, &sstest, calldeflist) ||
              assoc(b_dmd, &sstest, calldeflist))) {
            *ss = sstest;
            *newtb_p = tbtest;
            return 2;                        /* And try again. */
         }
      }

      /* Look very carefully at how we split this, so we get the RESULTFLAG__SPLIT_AXIS_MASK stuff right. */

      have_1x2 = assoc(b_1x2, ss, calldeflist);
      have_2x1 = assoc(b_2x1, ss, calldeflist);

      /* See if this call has applicable 1x2 or 2x1 definitions, (but not 2x2), in a non-T-boned setup.
         If so, split into boxes.  Furthermore, if the split could have been along either axis, we set
         both RESULTFLAG__SPLIT_AXIS_MASK bits. */

      if (((newtb & 1) == 0 && have_1x2 != 0) || ((newtb & 010) == 0 && have_2x1 != 0))
         goto divide_us_no_recompute;

      // If the splitting is into 4 side-by-side 1x2 setups, just split into 2x2's --
      // that will get the correct RESULTFLAG__SPLIT_AXIS_MASK bits.
      if (((newtb & 1) == 0 && have_2x1 != 0) || ((newtb & 010) == 0 && have_1x2 != 0))
         goto divide_us_no_recompute;

      // If we are T-boned and have 1x2 or 2x1 definitions, we need to be careful.
      tbi = ss->people[1].id1 | ss->people[2].id1 | ss->people[5].id1 | ss->people[6].id1;
      tbo = ss->people[0].id1 | ss->people[3].id1 | ss->people[4].id1 | ss->people[7].id1;

      if ((newtb & 011) == 011) {
         // If the centers and ends are separately consistent, we can do the call
         // concentrically *IF* the appropriate type of definition exists for the ends
         //  to work with the near person rather than the far one.  This is what makes
         // "heads into the middle and everbody partner trade" work,
         // and forbids "heads into the middle and everbody star thru".
         //
         // Or if we have a 1x1 definition, we can divide it.  Otherwise, we lose.

         if (((tbi & 011) != 011) && ((tbo & 011) != 011)) {
            if ((!(tbo & 010) || have_2x1 != 0) && (!(tbo & 1) || have_1x2 != 0)) {
               if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
                  fail("Can't split the setup.");

               goto do_concentrically;
            }
            else if (assoc(b_1x1, ss, calldeflist))
               goto divide_us_no_recompute;
         }

         // If the centers and ends are not separately consistent, we should just
         // split it into 2x2's.  Perhaps the call has both 1x2 and 2x1 definitions,
         // and will be done sort of siamese in each quadrant.  Another possibility
         // is that the call has just (say) 1x2 definitions, but everyone can do their
         // part and miraculously not hit each other.

         else goto divide_us_no_recompute;
      }

      // If some phantom concept has been used and there are 1x2 or 2x1
      // definitions, we also split it into boxes even if people are T-boned.
      // This is what makes everyone do their part if we say "heads into the middle
      // and heads are standard in split phantom lines, partner trade".
      // But we don't turn on both splitting bits in this case.  Note that,
      // since the previous test failed, the setup must be T-boned if this test passes.

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) && (have_1x2 != 0 || have_2x1 != 0))
         goto divide_us_no_recompute;

      // We are not T-boned, and there is no 1x2 or 2x1 definition.
      // The only possibility is that there is a 1x1 definition,
      // in which case splitting into boxes will work.

      else if (assoc(b_1x1, ss, calldeflist))
         goto divide_us_no_recompute;

      break;
   case s2x5:
      division_code = MAPCODE(s1x2,5,MPKIND__SPLIT,1);

      if (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) {
         if (     ((newtb & 001) == 0 && assoc(b_2x1, ss, calldeflist)) ||
                  ((newtb & 010) == 0 && assoc(b_1x2, ss, calldeflist)))
            goto divide_us_no_recompute;
      }
      else if (assoc(b_1x1, ss, calldeflist))
         goto divide_us_no_recompute;

      switch (livemask) {
      case 0x3DE:
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_R_ONEQ,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      case 0x1EF:
         division_code = MAPCODE(s1x4,2,MPKIND__OFFS_L_ONEQ,1);
         warn(warn__each1x4);
         goto divide_us_no_recompute;
      }

      break;
   case s1x4:
      /* See if the call has a 1x2, 2x1, or 1x1 definition,
         in which case split it and do each part. */
      if ((assoc(b_1x2, ss, calldeflist) ||
           assoc(b_2x1, ss, calldeflist) ||
           assoc(b_1x1, ss, calldeflist))) {

         /* The following makes "ends hinge" work from a grand wave.  Any 1x4 -> 2x2 call
            that acts by dividing itself into 1x2's is presumed to want the people in each 1x2
            to stay near each other.  We signify that by flipping the elongation, which we
            had previously set perpendicular to the 1x4 axis, overriding anything that may
            have been in the call definition. */

         *desired_elongation_p ^= 3;
         /* If the flags were zero and we complemented them so that
            both are set, that's not good. */
         if (*desired_elongation_p == 3)
            *desired_elongation_p = 0;

         division_code = MAPCODE(s1x2,2,MPKIND__SPLIT,0);
         goto divide_us_no_recompute;
      }

      /* See long comment above for s1x8.  The test cases for this are "tandem own the <points>, trade
         by flip the diamond", and "tandem own the <points>, flip the diamond by flip the diamond",
         both from a tandem diamond (the point being that there will be only one of them.) */

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__PHANTOMS) &&
          (ss->people[1].id1 | ss->people[3].id1) == 0) {
         setup sstest = *ss;

         sstest.kind = sdmd;   /* It makes assoc happier if we do this now. */

         if (
             (!(newtb & 010) || assoc(b_dmd, &sstest, calldeflist)) &&
             (!(newtb & 001) || assoc(b_pmd, &sstest, calldeflist))) {
            *ss = sstest;
            return 2;                        /* And try again. */
         }
      }

      break;
   case s_trngl4:
      // See if the call has a 1x2, 2x1, or 1x1 definition, in which case
      // split it and do each part.
      if ((assoc(b_1x2, ss, calldeflist) ||
           assoc(b_2x1, ss, calldeflist) ||
           assoc(b_1x1, ss, calldeflist))) {
         division_code = spcmap_tgl4_1;
         goto divide_us_no_recompute;
      }
   }

   if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK)
      fail("Can't split the setup.");

   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
   return 0;    /* We did nothing.  An error will presumably result. */

   divide_us_no_recompute:

   if (must_do_mystic)
      fail("Can't do \"snag/mystic\" with this call.");

   recompute_anyway = (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) != 0;
   ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT_MASK;
   ss->cmd.prior_elongation_bits = 0;
   ss->cmd.prior_expire_bits = 0;
   ss->rotation += finalrot;   // Flip the setup around and recanonicalize.
   canonicalize_rotation(ss);
   divided_setup_move(ss, division_code, phantest_ok, recompute_anyway, result);

   // Flip the setup back if necessary.  It will get canonicalized.
   result->rotation -= finalrot;

   /* If expansion to a 2x3 occurred (because the call was, for example, a "pair the line"),
      and the two 2x3's are end-to-end in a 2x6, see if we can squash phantoms.  We squash both
      internal (the center triple box) and external ones.  The external ones would probably have
      been squashed anyway due to the top level normalization, but we want this to occur
      immediately, not just at the top level, though we can't think of a concrete example
      in which it makes a difference. */

   if (result->result_flags & RESULTFLAG__EXPAND_TO_2X3) {
      if (result->kind == s2x6) {
         if (!(result->people[2].id1 | result->people[3].id1 |
               result->people[8].id1 | result->people[9].id1)) {
            // Inner spots are empty.

            // If the outer ones are empty also, we don't know what to do. 
            // This is presumably a "snag pair the line", or something like that.
            // Clear the inner spots away, and turn off the flag, so that
            // "punt_centers_use_concept" will know what to do.
            if (!(result->people[0].id1 | result->people[5].id1 |
                  result->people[6].id1 | result->people[11].id1))
               result->result_flags &= ~RESULTFLAG__EXPAND_TO_2X3;

            static const expand_thing inner_2x6 = {
               {0, 1, 4, 5, 6, 7, 10, 11}, 8, s2x4, s2x6, 0};
            compress_setup(&inner_2x6, result);
         }
      }
      else if (result->kind == s2x10) {
         if (!(result->people[4].id1 | result->people[5].id1 |
               result->people[14].id1 | result->people[15].id1)) {
            // Innermost 4 spots are empty.
            if (!(result->people[3].id1 | result->people[6].id1 |
                  result->people[13].id1 | result->people[16].id1)) {
               // Actually, inner 8 spots are empty.
               static const expand_thing inner_2x10 = {
                  {0, 1, 2, 7, 8, 9, 10, 11, 12, 17, 18, 19}, 12, s2x6, s2x10, 0};
               compress_setup(&inner_2x10, result);
            }
            else {
               static const expand_thing inner_2x10 = {
                  {0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19}, 16, s2x8, s2x10, 0};
               compress_setup(&inner_2x10, result);
            }
         }
         else if (!(result->people[0].id1 | result->people[9].id1 |
                    result->people[10].id1 | result->people[19].id1)) {
            // Outermost 4 spots are empty.

            if (!(result->people[1].id1 | result->people[8].id1 |
                  result->people[11].id1 | result->people[18].id1)) {
               // Actually, outer 8 spots are empty.
               static const expand_thing outer_2x10 = {
                  {2, 3, 4, 5, 6, 7, 12, 13, 14, 15, 16, 17}, 12, s2x6, s2x10, 0};
               compress_setup(&outer_2x10, result);
            }
            else {
               static const expand_thing outer_2x10 = {
                  {1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15, 16, 17, 18}, 16, s2x8, s2x10, 0};
               compress_setup(&outer_2x10, result);
            }
         }
      }
      else if (result->kind == s_rigger) {
         /* The outer spots are already known to be empty and have been cleaned up.
            So we just have to deal with the inner spots.  This means that both
            inner and outer spots are empty, so we have to do the same thing
            that we do above in the 2x6. */
         if (!(result->people[0].id1 | result->people[1].id1 |
               result->people[4].id1 | result->people[5].id1)) {
            static const expand_thing inner_rig = {
               {6, 7, -1, 2, 3, -1}, 6, s1x6, s_rigger, 0};
            compress_setup(&inner_rig, result);
            result->result_flags &= ~RESULTFLAG__EXPAND_TO_2X3;
         }
      }
      else if (result->kind == s4x6) {
         // We do the same for two concatenated 3x4's.
         // This could happen if the people folding were not the ends.
         if (!(result->people[2].id1 | result->people[3].id1 |
               result->people[8].id1 | result->people[9].id1 |
               result->people[20].id1 | result->people[21].id1 |
               result->people[14].id1 | result->people[15].id1)) {
            // Inner spots are empty.
            setup temp = *result;
            static const veryshort outer_4x6[16] = {
               5, 6, 23, 7, 12, 13, 16, 22, 17, 18, 11, 19, 0, 1, 4, 10};
   
            gather(result, &temp, outer_4x6, 15, 0);
            result->kind = s4x4;   // It will get canonicalized.
         }
         else if (!( result->people[0].id1 | result->people[5].id1 |
                     result->people[6].id1 | result->people[11].id1 |
                     result->people[18].id1 | result->people[23].id1 |
                     result->people[12].id1 | result->people[17].id1)) {
            // Outer spots are empty.
            setup temp = *result;
            static const veryshort inner_4x6[16] = {
               4, 7, 22, 8, 13, 14, 15, 21, 16, 19, 10, 20, 1, 2, 3, 9};
            gather(result, &temp, inner_4x6, 15, 0);
            result->kind = s4x4;   // It will get canonicalized.
         }
      }
      else if (result->kind == s3x6 &&
                  (result->result_flags & RESULTFLAG__SPLIT_AXIS_FIELDMASK) == 
                     ((result->rotation & 1) ?
                     RESULTFLAG__SPLIT_AXIS_YBIT :
                     RESULTFLAG__SPLIT_AXIS_XBIT)) {
         /* These were offset 2x3's. */
         if (!(result->people[2].id1 | result->people[3].id1 | result->people[8].id1 | result->people[11].id1 | result->people[12].id1 | result->people[17].id1)) {
            /* Inner spots are empty. */
            setup temp = *result;
            static const veryshort inner_3x6[12] = {0, 1, 4, 5, 6, 7, 9, 10, 13, 14, 15, 16};
   
            gather(result, &temp, inner_3x6, 11, 0);
            result->kind = s3x4;
         }
         else if (!(result->people[0].id1 | result->people[5].id1 | result->people[6].id1 | result->people[9].id1 | result->people[14].id1 | result->people[15].id1)) {
            /* Outer spots are empty. */
            setup temp = *result;
            static const veryshort outer_3x6[12] = {1, 2, 3, 4, 7, 8, 10, 11, 12, 13, 16, 17};
   
            gather(result, &temp, outer_3x6, 11, 0);
            result->kind = s3x4;
         }
      }
   }

   return 1;

   do_concentrically:

   conc_cmd = ss->cmd;
   concentric_move(ss, &conc_cmd, &conc_cmd, conc_schema,
                   0, DFM1_SUPPRESS_ELONGATION_WARNINGS, FALSE, ~0UL, result);
   return 1;

   do_mystically:

   conc_cmd = ss->cmd;

#ifdef TRY_NEW_MYSTIC
   update_id_bits(ss);    /* It would be nice if we didn't have to do this. */
#endif
   inner_selective_move(ss, &conc_cmd, &conc_cmd, selective_key_dyp,
                        TRUE, 0, 0, selector_centers, 0, 0, result);
   return 1;
}



static veryshort exp_conc_1x8[] = {3, 2, 7, 6};
static veryshort exp_conc_qtg[] = {6, 7, 2, 3};
static veryshort exp_conc_2x2[] = {6, 1, 2, 5, 6};




static veryshort s1x6translateh[32] = {
   -1, 0, 1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static veryshort s1x6translatev[32] = {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, 4, 5, -1, -1, -1, -1};

static veryshort sxwvtranslatev[40] = {
   -1, -1, 6, 7, -1, -1, -1, -1, -1, 0, 1, -1, -1, -1, -1, -1,
   -1, -1, 2, 3, -1, -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1,
   -1, -1, 6, 7, -1, -1, -1, -1};

static veryshort shrgltranslatev[40] = {
   -1, -1, -1, 7, -1, -1, 5, -1, -1, -1, 6, -1, -1, 0, -1, -1,
   -1, -1, -1, 3, -1, -1, 1, -1, -1, -1, 2, -1, -1, 4, -1, -1,
   -1, -1, -1, 7, -1, -1, 5, -1};

static veryshort shypergalv[20] = {
   -1, -1, -1, 6, 7, 0, -1, 1, -1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1, 6};

static veryshort shypergaldhrglv[20] = {
   -1, -1, -1, 7, 5, 0, 6, 3, -1, -1, -1, 3, 1, 4, 2, 7, -1, -1, -1, 7};

static veryshort s3dmftranslateh[32] = {
   -1, 0, 1, 2, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1,
   -1, 4, 5, 6, -1, -1, -1, -1, -1, 7, -1, -1, -1, -1, -1, -1};

static veryshort s3dmftranslatev[32] = {
   -1, 7, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, -1, -1, -1, -1,
   -1, 3, -1, -1, -1, -1, -1, -1, -1, 4, 5, 6, -1, -1, -1, -1};

static veryshort s3dmntranslateh[32] = {
   -1, 0, 1, 2, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1,
   -1, 4, 5, 6, -1, -1, -1, -1, -1, -1, 7, -1, -1, -1, -1, -1};

static veryshort s3dmntranslatev[32] = {
   -1, -1, 7, -1, -1, -1, -1, -1, -1, 0, 1, 2, -1, -1, -1, -1,
   -1, -1, 3, -1, -1, -1, -1, -1, -1, 4, 5, 6, -1, -1, -1, -1};

static veryshort s_vacate_star[32] = {
   -1, 0, 1, 2, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1,
   -1, 4, 5, 6, -1, -1, -1, -1, -1, -1, -1, 7, -1, -1, -1, -1};

static veryshort jqttranslateh[32] = {
   -1, 6, -1, 7, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1,
   -1, 2, -1, 3, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, 5, -1};

static veryshort jqttranslatev[32] = {
   -1, -1, -1, -1, -1, -1, 5, -1, -1, 6, -1, 7, -1, 0, -1, -1,
   -1, -1, -1, -1, -1, -1, 1, -1, -1, 2, -1, 3, -1, 4, -1, -1};

static veryshort j23translateh[32] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  0,
   0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  4,  0,  5,  0,  0};

static veryshort j23translatev[32] = {
   0,  0,  0,  4,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  1,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0};

static veryshort qdmtranslateh[32] = {
   12, 13, 14, 15,  0,  1,  0,  0,  0,   0,  0,  0,  3,  0,  2,  0,
   4,   5,  6,  7,  0,  9,  0,  8,  0,   0,  0,  0, 11,  0, 10,  0};

static veryshort qdmtranslatev[32] = {
   0,  0,  0,  0,  11, 0,  10, 0,  12, 13, 14, 15, 0,  1,  0,  0,
   0,  0,  0,  0,  3,  0,  2,  0,  4,  5,  6,  7,  0,  9,  0,  8};

static veryshort qtgtranslateh[32] = {
   -1, -1, 6, 7, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, 1, -1,
   -1, -1, 2, 3, -1, 4, -1, -1, -1, -1, -1, -1, -1, -1, 5, -1};

static veryshort qtgtranslatev[32] = {
   -1, -1, -1, -1, -1, -1, 5, -1, -1, -1, 6, 7, -1, 0, -1, -1,
   -1, -1, -1, -1, -1, -1, 1, -1, -1, -1, 2, 3, -1, 4, -1, -1};

static veryshort starstranslateh[32] = {
   -1, -1, 6, 7, -1, -1, 0, -1, -1, -1, -1, -1, -1, 1, -1, -1,
   -1, -1, 2, 3, -1, -1, 4, -1, -1, -1, -1, -1, -1, 5, -1, -1};

static veryshort starstranslatev[32] = {
   -1, -1, -1, -1, -1, 5, -1, -1, -1, -1, 6, 7, -1, -1, 0, -1,
   -1, -1, -1, -1, -1, 1, -1, -1, -1, -1, 2, 3, -1, -1, 4, -1};




/* For this routine, we know that callspec is a real call, with an array definition schema.
   Also, result->people have been cleared. */

extern void basic_move(
   setup *ss,
   calldefn *the_calldefn,
   int tbonetest,
   bool fudged,
   bool mirror,
   setup *result) THROW_DECL
{
   int j, k;
   callarray *calldeflist;
   long_boolean funny;
   uint32 division_code;
   callarray *linedefinition;
   callarray *coldefinition;
   uint32 matrix_check_flag = 0;
   callarray *goodies;
   uint32 search_concepts_without_funny,
      search_temp_without_funny, search_temp_with_funny;
   int real_index, northified_index;
   int num, halfnum, final_numout;
   setup newpersonlist;
   int newplacelist[MAX_PEOPLE];
   int orig_elongation = 0;
   int inconsistent_rotation, inconsistent_setup;
   long_boolean four_way_startsetup;
   uint32 newtb = tbonetest;
   uint32 resultflags = 0;
   int desired_elongation = 0;
   long_boolean funny_ok1 = FALSE;
   long_boolean funny_ok2 = FALSE;
   long_boolean other_elongate = FALSE;
   calldef_block *qq;
   const calldefn *callspec = the_calldefn;

   long_boolean check_peeloff_migration = FALSE;

   // We don't allow "central" or "invert" with array-defined calls.
   // We might allow "snag" or "mystic".

   if (ss->cmd.cmd_misc2_flags & (CMD_MISC2__SAID_INVERT | CMD_MISC2__DO_CENTRAL))
      fail("Can't do \"central\" or \"invert\" with this call.");

   // Do this now, for 2 reasons:
   // (1) We want it to be zero in case we bail out.
   // (2) we want the RESULTFLAG__SPLIT_AXIS_MASK stuff to be clear
   //     for the normal case, and have bits only if splitting actually occurs.
   result->result_flags = 0;

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__DO_NOT_EXECUTE) {
      result->kind = nothing;
      return;
   }

   /* We demand that the final concepts that remain be only the following ones. */

   if (ss->cmd.cmd_final_flags.final &
         ~(FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED |
           FINAL__TRIANGLE | FINAL__LEADTRIANGLE))
      fail("This concept not allowed here.");

   /* Set up the result elongation that we will need if the result is
      a 2x2 or short6.  This comes from the original 2x2 elongation, or is set
      perpendicular to the original 1x4/diamond elongation.  If the call has the
      "parallel_conc_end" flag set, invert that elongation.

      We will override all this if we divide a 1x4 or 2x2 into 1x2's.

      What this means is that the default for a 2x2->2x2 or short6->short6 call
      is to work to "same absolute elongation".  For a 2x2 that is of course the same
      as working to footprints.  For a short6 it works to footprints if the call
      doesn't rotate, and does something a little bit weird if it does.  In either
      case the "parallel_conc_end" flag inverts that.

      The reason for putting up with this rather weird behavior is to make counter
      rotate work for 2x2 and short6 setups.  The call has the "parallel_conc_end" flag
      set, so that "ends counter rotate" will do the correct (anti-footprint) thing.
      The "parallel_conc_end" flag applies only to an entire call, not to a specific
      by-array definition, so it is set for counter rotate from a short6 also.
      Counter rotate for a short6 rotates the setup, of course, and we want it to do
      the correct shape-preserving thing, which requires that the absolute elongation
      change when "parallel_conc_end" is set.  So the default is that, in the absence
      of "parallel_conc_end", the absolute elongation stays the same, even though that
      changes the shape for a rotating short6 call. */

   switch (ss->kind) {
      case s2x2: case s_short6:
         orig_elongation = ss->cmd.prior_elongation_bits;
         break;
      case s1x2: case s1x4: case sdmd:
         orig_elongation = 2 - (ss->rotation & 1);
         break;
   }

   desired_elongation = orig_elongation & 3;

   /* Attend to a few details, but only if there are real people present. */

   if (newtb) {
      switch (ss->kind) {
         case s2x2:
            /* Now we do a special check for split-square-thru or split-dixie-style types of things. */

            if (ss->cmd.cmd_final_flags.final & (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED)) {
               uint32 i1, i2, p1, p2;

               ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

               /* Find out who are facing each other directly and will therefore start. */

               if (((ss->people[2].id1 & d_mask) == d_west) &&
                   ((ss->people[3].id1 & d_mask) == d_east))
                  i1 = 0;
               else if (((ss->people[3].id1 & d_mask) == d_north) &&
                        ((ss->people[0].id1 & d_mask) == d_south))
                  i1 = 1;
               else if (((ss->people[0].id1 & d_mask) == d_east) &&
                        ((ss->people[1].id1 & d_mask) == d_west))
                  i1 = 2;
               else if (((ss->people[1].id1 & d_mask) == d_south) &&
                        ((ss->people[2].id1 & d_mask) == d_north))
                  i1 = 3;
               else
                  fail("People are not in correct position for split call.");

               i2 = (i1 + 1) & 3;
               p1 = ss->people[i1].id1;
               p2 = ss->people[i2].id1;

               /* Demand that the other people are facing the shoulders of the people who start. */
               if (((p1 & (BIT_PERSON | 3)) != (i1 ^ (BIT_PERSON | 2))) || ((p2 ^ p1) & d_mask))
                  fail("People are not in correct position for split call.");

               /* Now do the required transformation, if it is a "split square thru" type.
                  For "split dixie style" stuff, do nothing -- the database knows what to do. */

               if (!(ss->cmd.cmd_final_flags.final & FINAL__SPLIT_DIXIE_APPROVED)) {
                  swap_people(ss, i1, i2);
                  copy_rot(ss, i1, ss, i1, 033);
                  copy_rot(ss, i2, ss, i2, 011);

                  /* Repair the damage. */
                  newtb = ss->people[0].id1 | ss->people[1].id1 | ss->people[2].id1 | ss->people[3].id1;
               }
            }
            break;
         case s_trngl4:
            /* The same, with twisted. */

            if ((TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_TWISTED)) &&
                (ss->cmd.cmd_final_flags.final &
                 (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))) {
               ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

               if (  ((ss->people[0].id1 & d_mask) != d_north) ||
                     ((ss->people[1].id1 & d_mask) != d_south) ||
                     ((ss->people[2].id1 & d_mask) != d_south) ||
                     ((ss->people[3].id1 & d_mask) != d_south))
                  fail("People are not in correct position for split call.");

               /* Now do the required transformation, if it is a "split square thru" type.
                  For "split dixie style" stuff, do nothing -- the database knows what to do. */

               if (!(ss->cmd.cmd_final_flags.final & FINAL__SPLIT_DIXIE_APPROVED)) {
                  copy_rot(ss, 0, ss, 0, 011);
                  copy_rot(ss, 1, ss, 1, 011);
                  copy_rot(ss, 2, ss, 2, 011);
                  copy_rot(ss, 3, ss, 3, 033);
                  ss->rotation--;
                  ss->kind = s1x4;
                  canonicalize_rotation(ss);

                  /* Repair the damage. */
                  newtb = ss->people[0].id1 | ss->people[1].id1 | ss->people[2].id1 | ss->people[3].id1;
               }
            }
            break;
         case s_qtag:
            if (fudged && !(callspec->callflags1 & CFLAG1_FUDGE_TO_Q_TAG))
               fail("Can't do this call from arbitrary 3x4 setup.");
            break;
      }
   }

   // Many of the remaining final concepts (all of the heritable ones
   // except "funny" and "left", but "left" has been taken care of)
   // determine what call definition we will get.

   uint32 given_funny_flag = ss->cmd.cmd_final_flags.her8it & INHERITFLAG_FUNNY;

   search_concepts_without_funny = ss->cmd.cmd_final_flags.her8it & ~INHERITFLAG_FUNNY;
   search_temp_without_funny = 0;

foobar:

   search_temp_without_funny |= search_concepts_without_funny;
   search_temp_with_funny = search_temp_without_funny | given_funny_flag;

   funny = 0;   // Will have the bit if we are supposed to use the "CAF__FACING_FUNNY" stuff.

   for (qq = callspec->stuff.arr.def_list; qq; qq = qq->next) {
      // First, see if we have a match including any incoming "funny" flag.
      if (qq->modifier_seth == search_temp_with_funny) {
         goto use_this_calldeflist;
      }
      // Search again, for a plain definition that has the "CAF__FACING_FUNNY" flag.
      else if (given_funny_flag &&
               qq->modifier_seth == search_temp_without_funny &&
               (qq->callarray_list->callarray_flags & CAF__FACING_FUNNY)) {
         funny = given_funny_flag;
         goto use_this_calldeflist;
      }
   }

   /* We didn't find anything. */

   if (matrix_check_flag != 0) goto need_to_divide;

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
      fail("Can't do \"snag/mystic\" with this call.");

   // Perhaps the concept "magic" or "interlocked" was given, and the call
   // has no special definition for same, but wants us to divide the setup
   // magically or interlockedly.  Or a similar thing with 12 matrix.

   // First, check for "magic" and "interlocked" stuff, and do those divisions if so.
   if (divide_for_magic(ss,
                        search_concepts_without_funny & ~(INHERITFLAG_DIAMOND | INHERITFLAG_SINGLE |
                                                          INHERITFLAG_SINGLEFILE | INHERITFLAG_CROSS |
                                                          INHERITFLAG_GRAND | INHERITFLAG_REVERTMASK),
                        result))
      goto un_mirror;

   // Now check for 12 matrix or 16 matrix things.  If the call has the
   // "12_16_matrix_means_split" flag, and that (plus possible 3x3/4x4 stuff)
   // was what we were looking for, remove those flags and split the setup.

   if (callspec->callflags1 & CFLAG1_12_16_MATRIX_MEANS_SPLIT) {
      uint32 z = search_concepts_without_funny & ~INHERITFLAG_NXNMASK;

      switch (ss->kind) {
      case s3x4:
         if (z == INHERITFLAG_12_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "12 matrix" was specified.  Split it into 1x4's in the appropriate way.
            division_code = MAPCODE(s1x4,3,MPKIND__SPLIT,1);
            goto divide_us;
         }
         break;
      case s1x12:
         if (z == INHERITFLAG_12_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "12 matrix" was specified.  Split it into 1x4's in the appropriate way.
            division_code = MAPCODE(s1x4,3,MPKIND__SPLIT,0);
            goto divide_us;
         }
         break;
      case s3dmd:
         if (z == INHERITFLAG_12_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "12 matrix" was specified.  Split it into diamonds in the appropriate way.
            division_code = MAPCODE(sdmd,3,MPKIND__SPLIT,1);
            goto divide_us;
         }
         break;
      case s4dmd:
         if (z == INHERITFLAG_16_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "16 matrix" was specified.  Split it into diamonds in the appropriate way.
            division_code = MAPCODE(sdmd,4,MPKIND__SPLIT,1);
            goto divide_us;
         }
         break;
      case s2x6:
         if (z == INHERITFLAG_12_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "12 matrix" was specified.  Split it into 2x2's in the appropriate way.
            division_code = MAPCODE(s2x2,3,MPKIND__SPLIT,0);
            goto divide_us;
         }
         break;
      case s2x8:
         if (z == INHERITFLAG_16_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            // "16 matrix" was specified.  Split it into 2x2's in the appropriate way.
            division_code = MAPCODE(s2x2,4,MPKIND__SPLIT,0);
            goto divide_us;
         }
         break;
      case s4x4:
         if (z == INHERITFLAG_16_MATRIX ||
             (z == 0 && (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX))) {
            /* "16 matrix" was specified.  Split it into 1x4's in the appropriate way. */
            /* But which way is appropriate?  A 4x4 is ambiguous. */
            /* Being rather lazy, we just look to see whether the call is "pass thru",
               which is the only one that wants tandems rather than couples.
               Really ought to try splitting to 2x2's and see what happens. */

            int zxy = (callspec == &base_calls[base_call_passthru]->the_defn) ? 1 : 0;

            if ((newtb ^ zxy) & 1) {
               // If the setup is empty and newtb is zero, it doesn't matter what we do.
               division_code = spcmap_4x4v;
            }
            else
               division_code = MAPCODE(s1x4,4,MPKIND__SPLIT,1);
   
            goto divide_us;
         }
         break;
      }
   }

   calldeflist = 0;

   goto try_to_find_deflist;

 divide_us:

   ss->cmd.cmd_final_flags.her8it &= ~search_concepts_without_funny;
   divided_setup_move(ss, division_code, phantest_ok, TRUE, result);

   goto un_mirror;

   use_this_calldeflist:

   calldeflist = qq->callarray_list;
   if (qq->modifier_level > calling_level &&
       !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_CHECK_MOD_LEVEL)) {
      if (allowing_all_concepts)
         warn(warn__bad_modifier_level);
      else
         fail("Use of this modifier on this call is not allowed at this level.");
   }

   /* We now have an association list (setups ==> definition arrays) in calldeflist.
      Search it for an entry matching the setup we have, or else divide the setup
         until we find something.
      If we can't handle the starting setup, perhaps we need to look for "12 matrix" or
         "16 matrix" call definitions. */

   search_for_call_def:

   linedefinition = (callarray *) 0;
   coldefinition = (callarray *) 0;

   /* If we have to split the setup for "crazy", "central", or "splitseq", we
      specifically refrain from finding a call definition.  The same is true if
      we have "snag" or "mystic". */

   if (calldeflist &&
       !(ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT_MASK) &&
       !(ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)) {

      /* If we came in with a "dead concentric" or its equivalent, try to turn it
         into a real setup, depending on what the call is looking for.  If we fail
         to do so, setup_limits will be negative and an error will arise shortly. */

      if ((ss->kind == s_dead_concentric) ||
          (ss->kind == s_normal_concentric && ss->outer.skind == nothing)) {
         setup stemp;
         newtb = 0;
         for (j=0; j<=setup_attrs[ss->inner.skind].setup_limits; j++)
            newtb |= ss->people[j].id1;
         stemp = *ss;
         stemp.rotation += ss->inner.srotation;
   
         if (ss->inner.skind == s1x4) {
            stemp.kind = s1x8;
            clear_people(&stemp);
            scatter(&stemp, ss, exp_conc_1x8, 3, 0);

            if (  (!(newtb & 010) || assoc(b_1x8, &stemp, calldeflist))
                                    &&
                  (!(newtb & 1) || assoc(b_8x1, &stemp, calldeflist))) {
               *ss = stemp;
            }
            else {
               stemp.kind = s_qtag;
               clear_people(&stemp);
               scatter(&stemp, ss, exp_conc_qtg, 3, 0);

               if (  (!(newtb & 010) || assoc(b_qtag, &stemp, calldeflist))
                                       &&
                     (!(newtb & 1) || assoc(b_pqtag, &stemp, calldeflist))) {
                  *ss = stemp;
               }
            }
         }
         else if (ss->inner.skind == s2x2) {
            stemp.kind = s2x4;
            clear_people(&stemp);

            if (ss->concsetup_outer_elongation == 1) {
               scatter(&stemp, ss, &exp_conc_2x2[1], 3, 0);
            }
            else if (ss->concsetup_outer_elongation == 2) {
               stemp.rotation++;
               scatter(&stemp, ss, exp_conc_2x2, 3, 033);
            }
            else fail("Setup is bizarre.");

            *ss = stemp;
            newtb = 0;

            for (j=0; j<=setup_attrs[ss->inner.skind].setup_limits; j++)
               newtb |= ss->people[j].id1;
         }
      }

      begin_kind key1 = setup_attrs[ss->kind].keytab[0];
      begin_kind key2 = setup_attrs[ss->kind].keytab[1];

      four_way_startsetup = FALSE;

      if (key1 != b_nothing && key2 != b_nothing) {
         if (key1 == key2) {     /* This is for things like 2x2 or 1x1. */
            linedefinition = assoc(key1, ss, calldeflist);
            coldefinition = linedefinition;
            four_way_startsetup = TRUE;
         }
         else {
            /* If the setup is empty, get whatever definitions we can get, so that
               we can find the "CFLAG1_PARALLEL_CONC_END" bit,
               also known as the "other_elongate" bit. */

            if (ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_MASK) {
               /* See if the call has a 2x3 definition (we know the setup is a 2x3)
                  that goes to a setup of size 4.  That is, see if this is "Z axle".
                  If so, turn off the special "Z" flags and forget about it.
                  Otherwise, change to a 2x2 and try again. */
               if (!newtb || (newtb & 010)) linedefinition = assoc(key1, ss, calldeflist);
               if (!newtb || (newtb & 1)) coldefinition = assoc(key2, ss, calldeflist);

               if ((linedefinition &&
                    (setup_attrs[linedefinition->end_setup].setup_limits == 3 ||
                     (callspec->callflags1 & CFLAG1_PRESERVE_Z_STUFF))) ||
                   (coldefinition &&
                    (setup_attrs[coldefinition->end_setup].setup_limits == 3 ||
                     (callspec->callflags1 & CFLAG1_PRESERVE_Z_STUFF)))) {
                  ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__IN_Z_MASK;
               }
               else {
                  remove_z_distortion(ss);
                  newtb = 0;

                  for (j=0; j<=setup_attrs[ss->kind].setup_limits; j++)
                     newtb |= ss->people[j].id1;

                  linedefinition = assoc(b_2x2, ss, calldeflist);
                  coldefinition = linedefinition;
                  four_way_startsetup = TRUE;
               }
            }
            else {
               if (!newtb || (newtb & 010)) linedefinition = assoc(key1, ss, calldeflist);
               if (!newtb || (newtb & 1)) coldefinition = assoc(key2, ss, calldeflist);
            }
         }
      }
   }

   if (setup_attrs[ss->kind].setup_limits < 0) fail("Setup is extremely bizarre.");

   switch (ss->kind) {
   case s_short6:
   case s_bone6:
   case s_trngl:
   case s_ntrgl6cw:
   case s_ntrgl6ccw:
      break;
   default:
      if (ss->cmd.cmd_final_flags.final & (FINAL__TRIANGLE|FINAL__LEADTRIANGLE))
         fail("Triangle concept not allowed here.");
   }

   /* If we found a definition for the setup we are in, we win.
      This is true even if we only found a definition for the lines version
      of the setup and not the columns version, or vice-versa.
      If we need both and don't have both, we will lose. */

   if (linedefinition || coldefinition) {
      /* Raise a warning if the "split" concept was explicitly used but the
         call would have naturally split that way anyway. */

      /* ******* we have patched this out, because we aren't convinced that it really
         works.  How do we make it not complain on "split sidetrack" even though some
         parts of the call, like the "zig-zag", would complain?  And how do we account
         for the fact that it is observed not to raise the warning on split sidetrack
         even though we don't understand why?  By the way, uncertainty about this is what
         led us to make this a warning.  It was originally an error.  Which is correct?
         It is probably best to leave it as a warning of the "don't use in resolve" type.

      if (ss->setupflags & CMD_MISC__SAID_SPLIT) {
         switch (ss->kind) {
            case s2x2:
               if (!assoc(b_2x4, ss, calldeflist) && !assoc(b_4x2, ss, calldeflist))
                  warn(warn__excess_split);
               break;
            case s1x4:
               if (!assoc(b_1x8, ss, calldeflist) && !assoc(b_8x1, ss, calldeflist))
                  warn(warn__excess_split);
               break;
            case sdmd:
               if (!assoc(b_qtag, ss, calldeflist) && !assoc(b_pqtag, ss, calldeflist) &&
                        !assoc(b_ptpd, ss, calldeflist) && !assoc(b_pptpd, ss, calldeflist))
                  warn(warn__excess_split);
               break;
         }
      }
      */

      if (ss->cmd.cmd_final_flags.final & (FINAL__TRIANGLE|FINAL__LEADTRIANGLE))
         fail("Triangle concept not allowed here.");

      /* We got here if either linedefinition or coldefinition had something.
         If only one of them had something, but both needed to (because the setup
         was T-boned) the normal intention is that we proceed anyway, which will
         raise an error.  However, we check here for the case of a 1x2 setup
         that has one definition but not the other, and has a 1x1 definition as well.
         In that case, we split the setup.  This allows T-boned "quarter <direction>".
         The problem is that "quarter <direction>" has a 1x2 definition (for
         "quarter in") and a 1x1 definition, but no 2x1 definition.  (If it had
         a 2x1 definition, then the splitting from a 2x2 would be ambiguous.)
         So we have to fix that. */

      if (ss->kind == s1x2 && (newtb & 011) == 011 && (!linedefinition || !coldefinition))
         goto need_to_divide;

      goto do_the_call;
   }

 try_to_find_deflist:

   /* We may have something in "calldeflist" corresponding to the modifiers on this call,
      but there was nothing listed under that definition matching the starting setup. */

   /* First, see if adding a "12 matrix" or "16 matrix" modifier to the call will help.
      We need to be very careful about this.  The code below will divide, for example,
      a 2x6 into 2x3's if CMD_MISC__EXPLICIT_MATRIX is on (that is, if the caller
      said "2x6 matrix") and the call has a 2x3 definition.  This is what makes
      "2x6 matrix double play" work correctly from parallelogram columns, while
      just "double play" is not legal.  We take the position that the division of the
      2x6 into 2x3's only occurs if the caller said "2x6 matrix".  But the call
      "circulate" has a 2x3 column definition for the case with no 12matrix modifier
      (so that "center 6, circulate" will work), and a 2x6 definition if the 12matrix
      modifier is given.  We want the 12-person version of the circulate to happen
      if the caller said either "12 matrix" or "2x6 matrix".  If "2x6 matrix" was
      used, we will get here with nothing in coldefinition.  We must notice that
      doing the call search again with the "12 matrix" modifier set will get us the
      12-person call definition.

      If we didn't do this check, then a 2x6 parallelogram column, for example,
      would be split into 2x3's in the code below, if a 2x3 definition existed
      for the call.  This would mean that, if we said "2x6 matrix circulate",
      we would split the setup and do the circulate in each 2x3, which is not
      what people want.

      What actually goes on here is even more complicated now.  We want to allow the
      concept "12 matrix" to cause division of the set.  This means that we not only
      allow an explicit matrix ("2x6 matrix") to be treated as a "12 matrix", the way
      we always have, but we allow it to go the other way.  That is, under certain
      conditions we will turn a "12 matrix" to get turned into an explicit matrix
      concept.  We do this special action only when all else has failed.

      We do this only once.  Because of all the goto's that we execute as we
      try various things, there is danger of looping.  To make sure that this happens
      only once, we set "matrix_check_flag" nonzero when we do it, and only do it if
      that flag is zero.

      We don't do this if "snag" or "mystic" was given.  In those cases, we know exactly
      why there is no definition, and we need to call "divide_the_setup" to fix it. */

   if (matrix_check_flag == 0 && !(ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)) {
      if (!(search_concepts_without_funny & INHERITFLAG_16_MATRIX) &&
          ((ss->kind == s2x6 || ss->kind == s3x4 || ss->kind == s_d3x4 ||
            ss->kind == s1x12 || ss->kind == sdeepqtg) ||
           (((callspec->callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
             (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) &&
            !(search_concepts_without_funny & INHERITFLAG_16_MATRIX) &&
            (ss->kind == s2x3 || ss->kind == s1x6))))
         matrix_check_flag = INHERITFLAG_12_MATRIX;
      else if (!(search_concepts_without_funny & INHERITFLAG_12_MATRIX) &&
               ((ss->kind == s2x8 || ss->kind == s4x4 || ss->kind == s1x16) ||
                (((callspec->callflags1 & CFLAG1_SPLIT_LARGE_SETUPS) ||
                  (ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX)) &&
                 !(search_concepts_without_funny & INHERITFLAG_16_MATRIX) &&
                 (ss->kind == s2x4 || ss->kind == s1x8))))
         matrix_check_flag = INHERITFLAG_16_MATRIX;

      /* But we might not have set "matrix_check_flag" nonzero!  How are we going to
         prevent looping?  The answer is that we won't execute the goto unless we did
         set set it nonzero. */

      if (!(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) &&
          calldeflist == 0 &&
          (matrix_check_flag & ~search_concepts_without_funny) == 0) {

         /* Here is where we do the very special stuff.  We turn a "12 matrix" concept
            into an explicit matrix.  Note that we only do it if we would have lost
            anyway about 25 lines below (note that "calldeflist" is zero, and
            the search again stuff won't be executed unless CMD_MISC__EXPLICIT_MATRIX is on,
            which it isn't.)  So we turn on the CMD_MISC__EXPLICIT_MATRIX bit,
            and we turn off the INHERITFLAG_12_MATRIX or INHERITFLAG_16_MATRIX bit. */

         if (matrix_check_flag == 0) {
            /* We couldn't figure out from the setup what the matrix is,
               so we have to expand it. */

            if (!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_EXPAND_MATRIX)) {
               if (search_concepts_without_funny & INHERITFLAG_12_MATRIX) {
                  do_matrix_expansion(
                     ss,
                     (ss->kind == s2x4) ? CONCPROP__NEEDK_2X6 : CONCPROP__NEEDK_TRIPLE_1X4,
                     TRUE);

                  if (ss->kind != s2x6 && ss->kind != s3x4 && ss->kind != s1x12)
                     fail("Can't expand to a 12 matrix.");
                  matrix_check_flag = INHERITFLAG_12_MATRIX;
               }
               else if (search_concepts_without_funny & INHERITFLAG_16_MATRIX) {
                  if (ss->kind == s2x6) do_matrix_expansion(ss, CONCPROP__NEEDK_2X8, TRUE);
                  else if (ss->kind != s2x4) do_matrix_expansion(ss, CONCPROP__NEEDK_1X16, TRUE);
                  /* Take no action (and hence cause an error) if the setup was a 2x4.
                     If someone wants to say "16 matrix 4x4 peel off" from normal columns,
                     that person needs more help than we can give. */

                  if (ss->kind != s2x8 && ss->kind != s4x4 && ss->kind != s1x16)
                     fail("Can't expand to a 16 matrix."); 
                  matrix_check_flag = INHERITFLAG_16_MATRIX;
               }
            }
         }

         search_concepts_without_funny &= ~matrix_check_flag;
         ss->cmd.cmd_final_flags.her8it &= ~matrix_check_flag;
         search_temp_without_funny = 0;
         ss->cmd.cmd_misc_flags |= CMD_MISC__EXPLICIT_MATRIX;
      }
      else {
         uint32 sc = search_concepts_without_funny & INHERITFLAG_NXNMASK;

         if (((matrix_check_flag & INHERITFLAG_12_MATRIX) &&
              (search_concepts_without_funny & INHERITFLAG_12_MATRIX) &&
              (sc == INHERITFLAGNXNK_3X3 || sc == INHERITFLAGNXNK_6X6)) ||
             ((matrix_check_flag & INHERITFLAG_16_MATRIX) &&
              (search_concepts_without_funny & INHERITFLAG_16_MATRIX) &&
              (sc == INHERITFLAGNXNK_4X4 || sc == INHERITFLAGNXNK_8X8))) {
            search_concepts_without_funny &= ~matrix_check_flag;
            ss->cmd.cmd_final_flags.her8it &= ~matrix_check_flag;
            search_temp_without_funny = 0;
         }
         else
            search_temp_without_funny = matrix_check_flag;
      }

      if ((ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) && matrix_check_flag != 0)
         goto foobar;
   }

   /* We need to divide the setup. */

 need_to_divide:

   if (!newtb) {
      result->kind = nothing;
      goto un_mirror;
   }

   if (!calldeflist)
      fail("Can't do this call with this modifier.");

   j = divide_the_setup(ss, &newtb, calldeflist, &desired_elongation, result);
   if (j == 1)
      goto un_mirror;     // It divided and did the call.  We are done.
   else if (j == 2) goto search_for_call_def;      /* It did some necessary expansion
                                                      or transformation, but did not
                                                      do the call.  Try again. */

   if (ss->kind == s_ntrgl6cw || ss->kind == s_ntrgl6ccw) {
      // If the problem was that we had a "nonisotropic triangle" setup
      // and neither the call definition nor the splitting can handle it,
      // we give a warning and fudge to a 2x3.
      // Note that the "begin_kind" fields for these setups are "nothing".
      // It is simply not possible for a call definition to specify a
      // starting setup of nonisotropic triangles.
      ss->kind = s2x3;
      warn(warn__may_be_fudgy);
      goto search_for_call_def;
   }

   /* If we get here, linedefinition and coldefinition are both zero, and we will fail. */

   /* We are about to do the call by array! */

 do_the_call:

   /* This shouldn't happen, but we are being very careful here. */
   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
      fail("Can't do \"snag/mystic\" with this call.");

   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
   inconsistent_rotation = 0;
   inconsistent_setup = 0;

   if ((coldefinition && (coldefinition->callarray_flags & CAF__PLUSEIGHTH_ROTATION)) ||
       (linedefinition && (linedefinition->callarray_flags & CAF__PLUSEIGHTH_ROTATION)))
      result->result_flags |= RESULTFLAG__PLUSEIGHTH_ROT;

   if ((callspec->callflags1 & CFLAG1_PARALLEL_CONC_END) ||
       (coldefinition && (coldefinition->callarray_flags & CAF__OTHER_ELONGATE)) ||
       (linedefinition && (linedefinition->callarray_flags & CAF__OTHER_ELONGATE)))
      other_elongate = TRUE;

   if (other_elongate && ((desired_elongation+1) & 2))
      desired_elongation ^= 3;

   if (!newtb) {
      result->kind = nothing;   /* Note that we get the benefit of the
                                   "CFLAG1_PARALLEL_CONC_END" stuff here.  */
      goto un_mirror;           /* This means that a counter rotate in
                                   an empty 1x2 will still change shape. */
   }

   /* Check that "linedefinition" has been set up if we will need it. */

   goodies = (callarray *) 0;

   if ((newtb & 010) || four_way_startsetup) {
      assumption_thing t;

      if (!linedefinition) {
         switch (ss->kind) {
         case sdmd:
            fail("Can't handle people in diamond or PU quarter-tag for this call.");
         case s_trngl:
            fail("Can't handle people in triangle for this call.");
         case s_qtag:
            fail("Can't handle people in quarter tag for this call.");
         case s_ptpd:
            fail("Can't handle people in point-to-point diamonds for this call.");
         case s3x4:
            fail("Can't handle people in triple lines for this call.");
         case s2x3: case s2x4:
            fail("Can't handle people in lines for this call.");
         case s_short6:
            fail("Can't handle people in tall/short 6 for this call.");
         case s2x2:
            fail("Can't handle people in box of 4 for this call.");
         case s1x2:
            fail("Can't handle people in line of 2 for this call.");
         case s1x3:
            fail("Can't handle people in line of 3 for this call.");
         case s1x4:
            fail("Can't handle people in line of 4 for this call.");
         case s1x6:
            fail("Can't handle people in line of 6 for this call.");
         case s1x8:
            fail("Can't handle people in line of 8 for this call.");
         case s_galaxy:
            fail("Can't handle people in galaxy for this call.");
         default:
            fail("Can't do this call.");
         }
      }

      t.assumption = linedefinition->restriction;
      t.assump_col = 0;
      t.assump_both = 0;
      t.assump_cast = 0;
      t.assump_live = 0;
      t.assump_negate = 0;

      if (t.assumption != cr_none)
         (void) check_restriction(ss, t, FALSE,
                                  linedefinition->callarray_flags & CAF__RESTR_MASK);
      goodies = linedefinition;
   }

   /* Check that "coldefinition" has been set up if we will need it. */

   if ((newtb & 1) && (!four_way_startsetup)) {
      assumption_thing t;

      if (!coldefinition) {
         switch (ss->kind) {
         case sdmd:
            fail("Can't handle people in diamond or normal quarter-tag for this call.");
         case s_trngl:
            fail("Can't handle people in triangle for this call.");
         case s_qtag: case s3x4:
            fail("Can't handle people in triple columns for this call.");
         case s2x3: case s2x4:
            fail("Can't handle people in columns for this call.");
         case s_short6:
            fail("Can't handle people in tall/short 6 for this call.");
         case s1x2:
            fail("Can't handle people in column of 2 for this call.");
         case s1x4:
            fail("Can't handle people in column of 4 for this call.");
         case s1x6:
            fail("Can't handle people in column of 6 for this call.");
         case s1x8:
            fail("Can't handle people in column of 8 for this call.");
         case s_galaxy:
            fail("Can't handle people in galaxy for this call.");
         default:
            fail("Can't do this call.");
         }
      }

      t.assumption = coldefinition->restriction;
      t.assump_col = 1;
      t.assump_both = 0;
      t.assump_cast = 0;
      t.assump_live = 0;
      t.assump_negate = 0;

      if (t.assumption != cr_none)
         (void) check_restriction(ss, t, FALSE,
                                  coldefinition->callarray_flags & CAF__RESTR_MASK);

      /* If we have linedefinition also, check for consistency. */

      if (goodies) {
         /* ***** should also check the other stupid fields! */
         inconsistent_rotation =
            (goodies->callarray_flags ^ coldefinition->callarray_flags) & CAF__ROT;
         if ((setup_kind) goodies->end_setup != (setup_kind) coldefinition->end_setup)
            inconsistent_setup = 1;
      }

      goodies = coldefinition;
   }

   result->kind = (setup_kind) goodies->end_setup;

   if (result->kind == s_normal_concentric) {
      /* ***** this requires an 8-person call definition */
      setup outer_inners[2];
      int outer_elongation;
      setup p1;

      if (inconsistent_rotation | inconsistent_setup)
         fail("This call is an inconsistent shape-changer.");

      if (funny) fail("Sorry, can't do this call 'funny'");

      clear_people(&p1);

      for (real_index=0; real_index<8; real_index++) {
         personrec this_person = ss->people[real_index];
         if (this_person.id1) {
            uint32 z;
            int real_direction = this_person.id1 & 3;
            int d2 = (this_person.id1 << 1) & 4;
            northified_index = (real_index ^ d2);
            z = find_calldef((real_direction & 1) ? coldefinition : linedefinition,
                             ss, real_index, real_direction, northified_index);
            k = ((z >> 4) & 0x1F) ^ (d2 >> 1);
            install_person(&p1, k, ss, real_index);
            uint32 rollstuff = (z * (NROLL_BIT/NDBROLL_BIT)) & NROLL_MASK;
            if ((rollstuff+NROLL_BIT) & (NROLL_BIT*2)) rollstuff |= NROLL_BIT*4;
            p1.people[k].id1 = (p1.people[k].id1 & ~(NROLL_MASK | 077)) |
               ((z + real_direction * 011) & 013) | rollstuff;
            /* For now, can't do fractional stable on this kind of call. */
            p1.people[k].id1 &= ~STABLE_MASK;
         }
      }

      for (k=0; k<4; k++) {
         (void) copy_person(&outer_inners[1], k, &p1, k);
         (void) copy_person(&outer_inners[0], k, &p1, k+4);
      }

      outer_inners[0].result_flags = 0;
      outer_inners[1].result_flags = 0;
      outer_inners[0].kind = (setup_kind) goodies->end_setup_out;
      outer_inners[1].kind = (setup_kind) goodies->end_setup_in;
      outer_inners[0].rotation = (goodies->callarray_flags & CAF__ROT_OUT) ? 1 : 0;
      outer_inners[1].rotation = goodies->callarray_flags & CAF__ROT;

      /* For calls defined by array with concentric end setup, the "other_elongate" flag,
         which comes from "CFLAG1_PARALLEL_CONC_END" or "CAF__OTHER_ELONGATE",
         turns on the outer elongation. */
      outer_elongation = outer_inners[0].rotation;
      if (other_elongate) outer_elongation ^= 1;

      normalize_concentric(schema_concentric, 1, outer_inners,
                           (outer_elongation & 1) + 1, 0, result);

      goto fixup;
   }
   else {
      uint32 lilresult_mask[2];
      setup_kind tempkind;
      uint32 vacate = 0;

      result->rotation = goodies->callarray_flags & CAF__ROT;
      num = setup_attrs[ss->kind].setup_limits+1;
      halfnum = num >> 1;
      tempkind = result->kind;
      lilresult_mask[0] = 0;
      lilresult_mask[1] = 0;

      if (funny) {
         if ((ss->kind != result->kind) || result->rotation ||
             inconsistent_rotation || inconsistent_setup)
            fail("Can't do 'funny' shape-changer.");
      }

      // Check for a 1x4 call around the outside that
      // sends people far away without permission.
      if ((ss->kind == s1x4 || ss->kind == s1x6) &&
          ss->cmd.prior_elongation_bits & 0x40 &&
          !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_CHK_ELONG)) {
         if (goodies->callarray_flags & CAF__NO_CUTTING_THROUGH) {
            fail_no_retry("Call has outsides going too far around the set.");
         }
      }

      /* Check for people cutting through or working around an elongated 2x2 setup. */
      if (ss->kind == s2x2) {
         uint32 groovy_elongation = orig_elongation >> 8;

         if ((groovy_elongation & 0x3F) != 0 &&
             (goodies->callarray_flags & CAF__NO_FACING_ENDS)) {
            for (int i=0; i<4; i++) {
               uint32 p = ss->people[i].id1;
               if (p != 0 && ((p-i-1) & 2) != 0 && ((p ^ groovy_elongation) & 1) == 0)
                  fail("Centers aren't staying in the center.");
            }
         }

         if ((orig_elongation & 0x3F) != 0 &&
             !(ss->cmd.cmd_misc_flags & CMD_MISC__NO_CHK_ELONG)) {
            if (callspec->callflags1 & CFLAG1_NO_ELONGATION_ALLOWED)
               fail_no_retry("Call can't be done around the outside of the set.");

            if (goodies->callarray_flags & CAF__NO_CUTTING_THROUGH) {
               if (result->kind == s1x4)
                  check_peeloff_migration = TRUE;
               else {
                  for (int i=0; i<4; i++) {
                     uint32 p = ss->people[i].id1;
                     if (p != 0 && ((p-i-1) & 2) == 0 && ((p ^ orig_elongation) & 1) == 0)
                        fail("Call has outsides cutting through the middle of the set.");
                  }
               }
            }
         }
      }

      if (four_way_startsetup) {
         special_4_way_symm(linedefinition, ss, &newpersonlist, newplacelist,
                            lilresult_mask, result);
      }
      else if (ss->kind == s_trngl || ss->kind == s_trngl4 || ss->kind == s1x3) {
         if (inconsistent_rotation | inconsistent_setup)
            fail("This call is an inconsistent shape-changer.");
         special_triangle(coldefinition, linedefinition, ss, &newpersonlist, newplacelist,
                          num, lilresult_mask, result);
      }
      else {
         int halfnumoutl, halfnumoutc, numoutl, numoutc;
         const veryshort *final_translatec = identity;
         const veryshort *final_translatel = identity;
         int rotfudge_line = 0;
         int rotfudge_col = 0;

         numoutl = setup_attrs[result->kind].setup_limits+1;
         numoutc = numoutl;

         if (inconsistent_setup) {
            setup_kind other_kind = (setup_kind) linedefinition->end_setup;

            if (inconsistent_rotation) {
               if (result->kind == s_spindle && other_kind == s_crosswave) {
                  result->kind = sx4dmd;
                  tempkind = sx4dmd;
                  final_translatec = ftcspn;
                  vacate = linedefinition->callarray_flags & CAF__VACATE_CENTER;

                  if (goodies->callarray_flags & CAF__ROT) {
                     final_translatel = &ftlcwv[0];
                     rotfudge_line = 3;
                  }
                  else {
                     final_translatel = &ftlcwv[4];
                     rotfudge_line = 1;
                  }
               }
               else if (result->kind == s2x4 && other_kind == s_qtag) {
                  // We seem to be doing a complicated T-boned "transfer and []".
                  // Check whether we have been requested to "equalize",
                  // in which case we can do glorious things like going into
                  // a center diamond.
                  if ((callspec->callflagsf & CFLAG2_EQUALIZE)) {
                     result->kind = sx4dmd;
                     tempkind = sx4dmd;
                     final_translatec = ftequalize;

                     if (goodies->callarray_flags & CAF__ROT) {
                        final_translatel = &ftlqtg[0];
                        rotfudge_line = 3;
                     }
                     else {
                        final_translatel = &ftlqtg[4];
                        rotfudge_line = 1;
                     }
                  }
                  else {
                     // In this case, line people are right, column people are wrong.
                     result->rotation = linedefinition->callarray_flags & CAF__ROT;
                     result->kind = s_qtag;
                     tempkind = s_qtag;

                     if (goodies->callarray_flags & CAF__ROT) {
                        final_translatec = &qtlqtg[4];
                        rotfudge_col = 1;
                     }
                     else {
                        final_translatec = &qtlqtg[0];
                        rotfudge_col = 3;
                     }
                  }
               }
               else if (result->kind == s_bone && other_kind == s_qtag) {
                  result->kind = sx4dmd;
                  tempkind = sx4dmd;
                  final_translatec = ftcbone;

                  if (goodies->callarray_flags & CAF__ROT) {
                     final_translatel = &ftlqtg[0];
                     rotfudge_line = 3;
                  }
                  else {
                     final_translatel = &ftlqtg[4];
                     rotfudge_line = 1;
                  }
               }
               else if (result->kind == s_qtag && other_kind == s_bone) {
                  result->rotation = linedefinition->callarray_flags & CAF__ROT;
                  result->kind = sx4dmd;
                  tempkind = sx4dmd;
                  rotfudge_col = 1;
                  final_translatel = ftcbone;

                  if (goodies->callarray_flags & CAF__ROT) {
                     final_translatec = &ftlqtg[4];
                  }
                  else {
                     final_translatec = &ftlqtg[0];
                     rotfudge_line = 2;
                  }
               }
               else
                  fail("T-bone call went to a weird setup.");
            }
            else {
               if (result->kind == s4x4 && other_kind == s2x4) {
                  final_translatel = &ftc4x4[0];
                  numoutl = setup_attrs[other_kind].setup_limits+1;
               }
               else if (result->kind == s4x4 && other_kind == s_qtag) {
                  numoutl = setup_attrs[other_kind].setup_limits+1;
                  result->kind = sbigh;
                  tempkind = sbigh;
                  final_translatec = ft4x4bh;
                  final_translatel = ftqtgbh;
               }
               else if (result->kind == s4x4 && other_kind == s2x6) {
                  numoutl = setup_attrs[other_kind].setup_limits+1;
                  result->kind = s4x6;
                  tempkind = s4x6;
                  final_translatec = ft4x446;
                  final_translatel = ft2646;
               }
               else if (result->kind == s_c1phan && other_kind == s2x4) {
                  numoutl = setup_attrs[other_kind].setup_limits+1;
                  final_translatel = ftcphan;
               }
               else if (result->kind == s2x4 && other_kind == s_bone) {
                  // We seem to be doing a complicated T-boned "busy []".
                  // Check whether we have been requested to "equalize",
                  // in which case we can do glorious things like going into
                  // a center diamond.
                  if ((callspec->callflagsf & CFLAG2_EQUALIZE)) {
                     result->kind = shypergal;
                     tempkind = shypergal;
                     final_translatec = foobletch;
                     final_translatel = foobar;
                  }
                  else {
                     final_translatel = qtlbone;
                  }
               }
               else if (result->kind == s1x8 && other_kind == s_crosswave) {
                  final_translatel = qtlxwv;
               }
               else if (result->kind == s1x8 && other_kind == s_qtag) {
                  final_translatel = qtl1x8;
               }
               else if (result->kind == s_rigger && other_kind == s1x8) {
                  final_translatel = qtlrig;
               }
               else
                  fail("T-bone call went to a weird setup.");
            }
         }
         else if (inconsistent_rotation) {
            if (result->kind == s2x4) {
               result->kind = s4x4;
               tempkind = s4x4;
               final_translatec = ftc2x4;

               if (goodies->callarray_flags & CAF__ROT) {
                  final_translatel = &ftl2x4[0];
                  rotfudge_line = 3;
               }
               else {
                  final_translatel = &ftl2x4[4];
                  rotfudge_line = 1;
               }
            }
            else
               fail("This call is an inconsistent shape-changer.");
         }

         halfnumoutl = numoutl >> 1;
         halfnumoutc = numoutc >> 1;

         for (real_index=0; real_index<num; real_index++) {
            const veryshort *final_translate;
            int kt;
            callarray *the_definition;
            uint32 z;
            personrec this_person = ss->people[real_index];
            clear_person(&newpersonlist, real_index);
            if (this_person.id1) {
               int final_direction, d2out, thisnumout;
               int real_direction = this_person.id1 & 3;
               int d2 = ((this_person.id1 >> 1) & 1) * halfnum;
               northified_index = (real_index + d2) % num;

               if (real_direction & 1) {
                  d2out = ((this_person.id1 >> 1) & 1) * halfnumoutc;
                  thisnumout = numoutc;
                  final_translate = final_translatec;
                  final_direction = rotfudge_col;
                  the_definition = coldefinition;
               }
               else {
                  d2out = ((this_person.id1 >> 1) & 1) * halfnumoutl;
                  thisnumout = numoutl;
                  final_translate = final_translatel;
                  final_direction = rotfudge_line;
                  the_definition = linedefinition;
               }

               final_direction = (final_direction+real_direction) & 3;

               z = find_calldef(the_definition, ss, real_index,
                                real_direction, northified_index);
               k = (((z >> 4) & 0x1F) + d2out) % thisnumout;
               uint32 rollstuff = (z * (NROLL_BIT/NDBROLL_BIT)) & NROLL_MASK;
               if ((rollstuff+NROLL_BIT) & (NROLL_BIT*2)) rollstuff |= NROLL_BIT*4;
               newpersonlist.people[real_index].id1 = (this_person.id1 & ~(NROLL_MASK | 077)) |
                     ((z + final_direction * 011) & 013) | rollstuff;

               if (this_person.id1 & STABLE_ENAB)
                  do_stability(&newpersonlist.people[real_index].id1,
                               (stability) ((z/DBSTAB_BIT) & 0xF),
                               (z + final_direction - real_direction + result->rotation));

               newpersonlist.people[real_index].id2 = this_person.id2;
               kt = final_translate[k];
               if (kt < 0) fail("T-bone call went to a weird and confused setup.");
               newplacelist[real_index] = kt;
               lilresult_mask[0] |= (1 << kt);
            }
         }
      }

      /* If the result setup size is larger than the starting setup size, we assume that this call
         has a concocted result setup (e.g. squeeze the galaxy, unwrap the galaxy, change your
         image), and we try to compress it.  We claim that, if the result size given
         in the calls database is bigger than the starting size, as it is for those calls, we
         don't really want that big setup, but want to compress it immediately if possible.
         Q: Why don't we just let the natural setup normalization that will occur later do this
         for us?  A: That only happens at the top level.  In this case, we consider the compression
         to be part of doing the call.  If we someday were able to do a reverse flip of split
         phantom galaxies, we would want each galaxy to compress itself to a 2x4 before
         reassembling them.
      When we do a 16-matrix circulate in a 4x4 start setup, we do NOT want to compress
         the 4x4 to a 2x4!!!!!  This is why we compare the beginning and ending setup sizes. */

      if (!(goodies->callarray_flags & CAF__NO_COMPRESS) &&
          (result->kind == s_hyperglass ||
           result->kind == s_hyperbone ||
           result->kind == shypergal ||
           setup_attrs[ss->kind].setup_limits < setup_attrs[result->kind].setup_limits)) {
         const veryshort *permuter = (const veryshort *) 0;
         int rotator = 0;

         switch (result->kind) {
         case s4x4:
            /* See if people landed on 2x4 spots. */
            if ((lilresult_mask[0] & 0x7171) == 0) {
               result->kind = s2x4;
               permuter = galtranslateh;
            }
            else if ((lilresult_mask[0] & 0x1717) == 0) {
               result->kind = s2x4;
               permuter = galtranslatev;
               rotator = 1;
            }
            else {
               // If fakery occurred and we were not able to compress it, that is an error.
               // It means the points got confused on a reverse flip the galaxy.
               // If we were not able to compress a 4x4 but no fakery occurred,
               // we let it pass.  That simply means that the unwrappers were T-boned
               // in an unwrap the galaxy, so they led out in strange directions.
               // Is this the right thing to do?  Do we want to allow
               // T-boned reverse flip?  Probably not.

               if (tempkind != s4x4) fail("Galaxy call went to improperly-formed setup.");
#ifdef BREAKS_CAST_BACK
               /* See if people landed classical C1 phantom spots. */
               if ((lilresult_mask[0] & 0x3333) == 0) {
                  result->kind = s_c1phan;
                  permuter = phan4x4xlatea;
               }
               else if ((lilresult_mask[0] & 0x5555) == 0) {
                  result->kind = s_c1phan;
                  permuter = phan4x4xlateb;
               }

#endif
            }
            break;
         case s_c1phan:
            /* See if people landed on 2x4 spots. */
            if ((lilresult_mask[0] & 0x7B7B) == 0) {
               result->kind = s2x4;
               permuter = phantranslateh;
            }
            else if ((lilresult_mask[0] & 0xB7B7) == 0) {
               result->kind = s2x4;
               permuter = phantranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0x9696) == 0) {
               result->kind = s2x4;
               permuter = phantranslateh;
            }
            else if ((lilresult_mask[0] & 0x6969) == 0) {
               result->kind = s2x4;
               permuter = phantranslatev;
               rotator = 1;
            }
            break;
         case s_thar:
            if ((lilresult_mask[0] & 0x66) == 0) {    /* Check horiz dmd spots. */
               result->kind = sdmd;
               permuter = sdmdtranslateh;
            }
            else if ((lilresult_mask[0] & 0x99) == 0) {    /* Check vert dmd spots. */
               result->kind = sdmd;
               permuter = sdmdtranslatev;
               rotator = 1;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case s8x8:
            /* See if people landed on 2x8 or 4x6 spots. */
            result->kind = s2x8;
            permuter = octtranslateh;

            if ((lilresult_mask[0] & 0x333F11FFUL) == 0 &&
                (lilresult_mask[1] & 0x333F11FFUL) == 0) {
               result->kind = s4x6;
               permuter = octt4x6lateh;
            }
            else if ((lilresult_mask[0] & 0x11FF333FUL) == 0 &&
                     (lilresult_mask[1] & 0x11FF333FUL) == 0) {
               result->kind = s4x6;
               permuter = octt4x6latev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0x0FFF7777UL) == 0 &&
                     (lilresult_mask[1] & 0x0FFF7777UL) == 0) {
               permuter = octtranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0x77770FFFUL) != 0 ||
                     (lilresult_mask[1] & 0x77770FFFUL) != 0)
               fail("Call went to improperly-formed setup.");
            break;
         case sx1x16:
            /* See if people landed on 1x16 spots. */
            result->kind = s1x16;
            permuter = hextranslateh;

            if ((lilresult_mask[0] & 0x00FF00FFUL) == 0) {
               permuter = hextranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFF00FF00UL) != 0)
               fail("Call went to improperly-formed setup.");
            break;
         case sx4dmd:
            if ((lilresult_mask[0] & 0xD7BFD7BFUL) == 0) {
               result->kind = s2x3;
               permuter = j23translateh;
            }
            else if ((lilresult_mask[0] & 0xBFD7BFD7UL) == 0) {
               result->kind = s2x3;
               permuter = j23translatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFFF1FFF1) == 0) {
               // Check horiz 1x6 spots.
               result->kind = s1x6;
               permuter = s1x6translateh;
            }
            else if ((lilresult_mask[0] & 0xF1FFF1FF) == 0) {
               // Check vert 1x6 spots.
               result->kind = s1x6;
               permuter = s1x6translatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xBFD5BFD5UL) == 0) {
               result->kind = s_qtag;
               permuter = jqttranslateh;
            }
            else if ((lilresult_mask[0] & 0xD5BFD5BFUL) == 0) {
               result->kind = s_qtag;
               permuter = jqttranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xBFD3BFD3UL) == 0) {
               result->kind = s_qtag;
               permuter = qtgtranslateh;
            }
            else if ((lilresult_mask[0] & 0xD3BFD3BFUL) == 0) {
               result->kind = s_qtag;
               permuter = qtgtranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xF3F9F3F9) == 0) {
               // Check horiz xwv spots.
               result->kind = s_crosswave;
               permuter = &sxwvtranslatev[8];
            }
            else if ((lilresult_mask[0] & 0xF9F3F9F3) == 0) {
               // Check vert xwv spots.
               result->kind = s_crosswave;
               permuter = sxwvtranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFDF1FDF1) == 0) {
               // Check horiz 3x1dmd spots w/points out far.
               result->kind = s3x1dmd;
               permuter = s3dmftranslateh;
            }
            else if ((lilresult_mask[0] & 0xF1FDF1FD) == 0) {
               // Check vert 3x1dmd spots w/points out far.
               result->kind = s3x1dmd;
               permuter = s3dmftranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFBF1FBF1) == 0) {
               // Check horiz 3x1dmd spots w/points in close.
               result->kind = s3x1dmd;
               permuter = s3dmntranslateh;
            }
            else if ((lilresult_mask[0] & 0xF1FBF1FB) == 0) {
               // Check vert 3x1dmd spots w/points in close.
               result->kind = s3x1dmd;
               permuter = s3dmntranslatev;
               rotator = 1;
            }
            else if (vacate &&
                     (lilresult_mask[0] & 0xF7F1F7F1) == 0 &&
                     (lilresult_mask[0] & 0x00080008) == 0x00080008) {
               // Check for star in the middle that can be disambiguated
               // by having someone vacate it.  We have to mark this
               // as controversial -- the center star is actually isotropic,
               // and we are fudging it so that T-boned coordinate will work.
               warn(warn_controversial);
               result->kind = s1x3dmd;
               permuter = s_vacate_star;
            }
            else if ((lilresult_mask[0] & 0xAF50AF50UL) == 0) {
               result->kind = s4dmd;
               permuter = qdmtranslateh;
            }
            else if ((lilresult_mask[0] & 0x50AF50AFUL) == 0) {
               result->kind = s4dmd;
               permuter = qdmtranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xDFB3DFB3UL) == 0) {
               //               result->kind = s_2stars;
               result->kind = s_bone;
               permuter = starstranslateh;
            }
            else if ((lilresult_mask[0] & 0xB3DFB3DFUL) == 0) {
               //               result->kind = s_2stars;
               result->kind = s_bone;
               permuter = starstranslatev;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xB7DBB7DB) == 0) {
               // Check horiz hourglass spots.
               result->kind = s_hrglass;
               permuter = &shrgltranslatev[8];
            }
            else if ((lilresult_mask[0] & 0xDBB7DBB7) == 0) {
               // Check vert hourglass spots.
               result->kind = s_hrglass;
               permuter = shrgltranslatev;
               rotator = 1;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case shypergal:
            if ((lilresult_mask[0] & 0xFFFF7474) == 0) {
               result->kind = s2x4;
               permuter = &shypergalv[4];
            }
            else if ((lilresult_mask[0] & 0xFFFF4747) == 0) {
               result->kind = s2x4;
               permuter = shypergalv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFFFF7878) == 0) {
               result->kind = s_dhrglass;
               permuter = &shypergaldhrglv[4];
            }
            else if ((lilresult_mask[0] & 0xFFFF8787) == 0) {
               result->kind = s_dhrglass;
               permuter = shypergaldhrglv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xFFFFF0F0) == 0) {
               result->kind = s_dhrglass;
               permuter = &shypergaldhrglv[4];
            }
            else if ((lilresult_mask[0] & 0xFFFF0F0F) == 0) {
               result->kind = s_dhrglass;
               permuter = shypergaldhrglv;
               rotator = 1;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case s_hyperglass:

            /* Investigate possible diamonds and 1x4's.  If enough centers
               and ends (points) are present to determine unambiguously what
               result setup we must create, we of course do so.  Otherwise, if
               the centers are missing but points are present, we give preference
               to making a 1x4, no matter what the call definition said the ending
               setup was.  But if centers are present and the points are missing,
               we go do diamonds if the original call definition wanted diamonds. */

            if ((lilresult_mask[0] & 05757) == 0 && tempkind == sdmd) {
               result->kind = sdmd;     /* Only centers present, and call wanted diamonds. */
               permuter = dmdhyperh;
            }
            else if ((lilresult_mask[0] & 07575) == 0 && tempkind == sdmd) {
               result->kind = sdmd;     /* Only centers present, and call wanted diamonds. */
               permuter = dmdhyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 05656) == 0 &&
                     (goodies->callarray_flags & CAF__REALLY_WANT_DIAMOND)) {
               result->kind = sdmd;     /* Setup is consistent with diamonds,
                                           though maybe centers are absent,
                                           but user specifically requested diamonds. */
               permuter = dmdhyperh;
            }
            else if ((lilresult_mask[0] & 06565) == 0 &&
                     (goodies->callarray_flags & CAF__REALLY_WANT_DIAMOND)) {
               result->kind = sdmd;     /* Setup is consistent with diamonds,
                                           though maybe centers are absent,
                                           but user specifically requested diamonds. */
               permuter = dmdhyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 07474) == 0) {
               result->kind = s1x4;     /* Setup is consistent with lines,
                                           though maybe centers are absent. */
               permuter = linehyperh;
            }
            else if ((lilresult_mask[0] & 04747) == 0) {
               result->kind = s1x4;     /* Setup is consistent with lines,
                                           though maybe centers are absent. */
               permuter = linehyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 05656) == 0) {
               result->kind = sdmd;     /* Setup is consistent with diamonds,
                                           though maybe centers are absent. */
               permuter = dmdhyperh;
            }
            else if ((lilresult_mask[0] & 06565) == 0) {
               result->kind = sdmd;     /* Setup is consistent with diamonds,
                                           though maybe centers are absent. */
               permuter = dmdhyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 01212) == 0) {
               result->kind = s_hrglass;    /* Setup is an hourglass. */
               permuter = galhyperh;
            }
            else if ((lilresult_mask[0] & 02121) == 0) {
               result->kind = s_hrglass;    /* Setup is an hourglass. */
               permuter = galhyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 03030) == 0) {
               result->kind = s_qtag;    /* Setup is qtag/diamonds. */
               permuter = qtghyperh;
            }
            else if ((lilresult_mask[0] & 00303) == 0) {
               result->kind = s_qtag;    /* Setup is qtag/diamonds. */
               permuter = qtghyperv;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 05555) == 0) {
               result->kind = s_star;
               permuter = starhyperh;
            }
            else if ((lilresult_mask[0] & 06666) == 0) {
               result->kind = s_star;        /* Actually, this is a star with all people
                                                sort of far away from the center.  We may
                                                need to invent a new setup, "farstar". */
               permuter = fstarhyperh;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case s_hyperbone:
            if ((lilresult_mask[0] & 0x0F0F) == 0) {
               result->kind = s_bone;
               permuter = hyperboneh;
            }
            else if ((lilresult_mask[0] & 0xF0F0) == 0) {
               result->kind = s_bone;
               permuter = hyperbonev;
               rotator = 1;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case slittlestars:
            if ((lilresult_mask[0] & 0xCC) == 0) {
               result->kind = s2x2;
               permuter = lilstar3;
            }
            else if ((lilresult_mask[0] & 0x33) == 0) {
               result->kind = s1x4;
               permuter = lilstar4;
            }
            else if ((lilresult_mask[0] & 0x2D) == 0) {
               result->kind = s_trngl4;
               permuter = lilstar1;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 0xD2) == 0) {
               result->kind = s_trngl4;
               permuter = lilstar2;
               rotator = 3;
            }
            else
               fail("Call went to improperly-formed setup.");
            break;
         case sbigdmd:
            if ((lilresult_mask[0] & 02222) == 0) {        /* All outside */
               result->kind = s_qtag;
               permuter = qtbd1;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 04141) == 0) {   /* All inside */
               result->kind = s_qtag;
               permuter = qtbd2;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 02121) == 0) {   /* Some inside, some outside */
               result->kind = s_qtag;
               permuter = qtbd3;
               rotator = 1;
            }
            else if ((lilresult_mask[0] & 04242) == 0) {   /* Some inside, some outside */
               result->kind = s_qtag;
               permuter = qtbd4;
               rotator = 1;
            }
            break;
         case s2x3:
         case s2x5:
         case s_bone6:
            // This call turned a smaller setup (like a 1x4) into a 2x3.
            // It is presumably a call like "pair the line" or "step and slide".
            // Flag the result setup so that the appropriate phantom-squashing
            // will take place if two of these results are placed end-to-end.
            // We also do this if a 3x3 pair the line went to a 2x5.
            resultflags |= RESULTFLAG__EXPAND_TO_2X3;
            break;
         case s3x4:
            if (ss->kind == s1x4) {

               /* This could be the even more glorious form of the above, for "fold".
                  If we can strip it down to a 2x3 (because the ends were the ones doing
                  the fold), do so now.  In any case, set the flag so that the 3-to-2
                  squashing can take place later. */

               if ((lilresult_mask[0] & 03131) == 0) {
                  result->kind = s2x3;
                  permuter = q3x4xx1;
                  rotator = 1;
               }
               else if ((lilresult_mask[0] & 01717) == 0) {
                  result->kind = s1x4;
                  permuter = q3x4xx2;
                  rotator = 0;
               }
               else if ((lilresult_mask[0] & 01567) == 0 || (lilresult_mask[0] & 01673) == 0) {
                  result->kind = s_trngl4;
                  permuter = q3x4xx3;
                  rotator = 1;
               }
               else if ((lilresult_mask[0] & 06715) == 0 || (lilresult_mask[0] & 07316) == 0) {
                  result->kind = s_trngl4;
                  permuter = q3x4xx4;
                  rotator = 3;
               }

               resultflags |= RESULTFLAG__EXPAND_TO_2X3;
            }
            break;
         }

         if (permuter) {
            uint32 r = 011*((-rotator) & 3);

            for (real_index=0; real_index<num; real_index++) {
               if (ss->people[real_index].id1) {
                  newplacelist[real_index] = permuter[newplacelist[real_index]];
                  if (rotator)
                     newpersonlist.people[real_index].id1 =
                        rotperson(newpersonlist.people[real_index].id1, r);
               }
            }

            result->rotation += rotator;
         }
      }
   }

   /* Install all the people. */

   {
      // We create an especially glorious collision collector here, with all
      // the stuff for handling the nuances of the call definition and the
      // assumption.

      collision_collector CC(mirror, &ss->cmd, callspec);

      final_numout = setup_attrs[result->kind].setup_limits+1;

      for (real_index=0; real_index<num; real_index++) {
         personrec newperson = newpersonlist.people[real_index];
         if (newperson.id1) {
            if (funny) {
               if (newperson.id1 != ~0UL) {       /* We only handle people who haven't been erased. */
                  k = real_index;
                  j = real_index;               /* j will move twice as fast as k, looking for a loop not containing starting point. */
                  do {
                     j = newplacelist[j];
                     /* If hit a phantom, we can't proceed. */
                     if (!newpersonlist.people[j].id1) fail("Can't do 'funny' call with phantoms.");
                     /* If hit an erased person, we have clearly hit a loop not containing starting point. */
                     else if (newpersonlist.people[j].id1 == ~0UL) break;
                     j = newplacelist[j];
                     if (!newpersonlist.people[j].id1) fail("Can't do 'funny' call with phantoms.");
                     else if (newpersonlist.people[j].id1 == ~0UL) break;
                     k = newplacelist[k];
                     if (k == real_index) goto funny_win;
                  } while (k != j);

                  // This person can't move, because he moves into a loop
                  // not containing his starting point.
                  k = real_index;
                  newperson.id1 = (ss->people[real_index].id1 & ~NROLL_MASK) | (3*NROLL_BIT);
                  newperson.id2 = ss->people[real_index].id2;
                  result->people[k] = newperson;
                  newpersonlist.people[k].id1 = ~0UL;
                  funny_ok1 = TRUE;    // Someone decided not to move.  Hilarious.
                  goto funny_end;

               funny_win:
                  /* Move the entire loop, replacing people with -1. */
                  k = real_index;
                  j = 0;      /* See how long the loop is. */
                  do {
                     newperson = newpersonlist.people[k];
                     newpersonlist.people[k].id1 = ~0UL;
                     k = newplacelist[k];
                     result->people[k] = newperson;
                     j++;
                  } while (k != real_index);

                  if (j > 2) warn(warn__hard_funny);
                  funny_ok2 = TRUE;    // Someone was able to move.  Hysterical.
                  // Actually, I don't see how this test can fail.
                  // Someone can always move!

               funny_end: ;
               }
            }
            else {              // Un-funny move.
               CC.install_with_collision(result, newplacelist[real_index],
                                         &newpersonlist, real_index, 0);
            }
         }
      }

      if (funny && (!funny_ok1 || !funny_ok2))
         warn(warn__not_funny);

      CC.fix_possible_collision(result);
   }

   fixup:

   reinstate_rotation(ss, result);

   un_mirror:

   canonicalize_rotation(result);

   if (check_peeloff_migration) {
      /* Check that the resultant 1x4 is parallel to the original 2x2 elongation,
         and that everyone stayed on their own side of the split. */
      if (((orig_elongation ^ result->rotation) & 1) == 0)
         fail("People are too far apart to work with each other on this call.");

      for (int i=0; i<4; i++) {
         int z = (i-result->rotation+1) & 2;
         uint32 p = ss->people[i].id1;
         if ((((p ^ result->people[z  ].id1) & PID_MASK) != 0) &&
             (((p ^ result->people[z+1].id1) & PID_MASK) != 0))
            fail_no_retry("Call can't be done around the outside of the set.");
      }
   }

   /* We take out any elongation info that divided_setup_move may have put in
      and override it with the correct info. */

   result->result_flags =
      (result->result_flags & (~3)) |
      resultflags |
      (desired_elongation & 3);
}
