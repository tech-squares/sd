// SD -- square dance caller's helper.
//
//    Copyright (C) 1990-2006  William B. Ackerman.
//
//    This file is part of "Sd".
//
//    Sd is free software; you can redistribute it and/or modify it
//    under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Sd is distributed in the hope that it will be useful, but WITHOUT
//    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//    License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Sd; if not, write to the Free Software Foundation, Inc.,
//    59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    This is for version 36.

// These are written as the first two halfwords of the binary database file.
// The format version is not related to the version of the program or database.
// It is used only to make sure that the "mkcalls" program that compiled
// the database and the "sd" program that reads it are in sync with each
// other, as indicated by the version of this file.  Whenever we change
// anything in this file that could cause an incompatibility, we bump the
// database format version.

#define DATABASE_MAGIC_NUM 21316
#define DATABASE_FORMAT_VERSION 252

// BEWARE!!  These must track the items in "tagtabinit" in mkcalls.cpp .
enum base_call_index {
   base_call_unused,
   base_call_null,
   base_call_null_second,
   base_call_basetag0,
   base_call_basetag0_noflip,
   base_call_flip,
   base_call_armturn_34,
   base_call_ends_shadow,
   base_call_chreact_1,
   base_call_makepass_1,
   base_call_nuclear_1,
   base_call_scootback,
   base_call_scoottowave,
   base_call_backemup,
   base_call_circulate,
   base_call_trade,
   base_call_any_hand_remake,
   base_call_passthru,
   base_call_check_cross_counter,
   base_call_lockit,
   base_call_disband1,
   base_call_slither,
   base_call_maybegrandslither,
   base_call_plan_ctrtoend,
   base_base_prepare_to_drop,
   base_base_hinge_and_then_trade,
   base_base_hinge_and_then_trade_for_breaker,
   base_call_two_o_circs,
   // The next "NUM_TAGGER_CLASSES" (that is, 4) must be a consecutive group.
   base_call_tagger0,
   base_call_tagger1_noref,
   base_call_tagger2_noref,
   base_call_tagger3_noref,
   base_call_circcer,
   base_call_turnstar_n,
   base_call_revert_if_needed,
   base_call_extend_n,
   num_base_call_indices    // Not an actual enumeration item.
};



// BEWARE!!  This list must track the tables "flagtabh", "defmodtabh",
// "forcetabh", and "altdeftabh" in mkcalls.cpp .  The "K" items also track
// the tables "mxntabforce", "nxntabforce", "nxntabplain", "mxntabplain",
// "reverttabplain", and "reverttabforce" in mkcalls.cpp .
//
// These are the infamous "heritable flags".  They are used in generally
// corresponding ways in the "callflagsh" word of a top level callspec_block,
// the "modifiersh" word of a "by_def_item", and the "modifier_seth" word of a
// "calldef_block", and the "cmd_final_flags.herit" of a setup with its command block.

enum heritflags {
   INHERITFLAG_DIAMOND    = 0x00000001UL,
   INHERITFLAG_REVERSE    = 0x00000002UL,
   INHERITFLAG_LEFT       = 0x00000004UL,
   INHERITFLAG_FUNNY      = 0x00000008UL,
   INHERITFLAG_INTLK      = 0x00000010UL,
   INHERITFLAG_MAGIC      = 0x00000020UL,
   INHERITFLAG_GRAND      = 0x00000040UL,
   INHERITFLAG_12_MATRIX  = 0x00000080UL,
   INHERITFLAG_16_MATRIX  = 0x00000100UL,
   INHERITFLAG_CROSS      = 0x00000200UL,
   INHERITFLAG_SINGLE     = 0x00000400UL,
   INHERITFLAG_SINGLEFILE = 0x00000800UL,
   INHERITFLAG_HALF       = 0x00001000UL,
   INHERITFLAG_YOYO       = 0x00002000UL,
   INHERITFLAG_STRAIGHT   = 0x00004000UL,
   INHERITFLAG_TWISTED    = 0x00008000UL,
   INHERITFLAG_LASTHALF   = 0x00010000UL,
   INHERITFLAG_FRACTAL    = 0x00020000UL,
   INHERITFLAG_FAST       = 0x00040000UL,
   INHERITFLAG_REWIND     = 0x00080000UL,

   // This is a 3 bit field.
   INHERITFLAG_MXNMASK    = 0x00700000UL,
   // This is its low bit.
   INHERITFLAG_MXNBIT     = 0x00100000UL,

   // These 4 things are the choices available inside.
   INHERITFLAGMXNK_1X2    = 0x00100000UL,
   INHERITFLAGMXNK_2X1    = 0x00200000UL,
   INHERITFLAGMXNK_1X3    = 0x00300000UL,
   INHERITFLAGMXNK_3X1    = 0x00400000UL,

   // This is a 3 bit field.
   INHERITFLAG_NXNMASK    = 0x03800000UL,
   // This is its low bit.
   INHERITFLAG_NXNBIT     = 0x00800000UL,

   // These 7 things are the choices available inside.
   INHERITFLAGNXNK_2X2    = 0x00800000UL,
   INHERITFLAGNXNK_3X3    = 0x01000000UL,
   INHERITFLAGNXNK_4X4    = 0x01800000UL,
   INHERITFLAGNXNK_5X5    = 0x02000000UL,
   INHERITFLAGNXNK_6X6    = 0x02800000UL,
   INHERITFLAGNXNK_7X7    = 0x03000000UL,
   INHERITFLAGNXNK_8X8    = 0x03800000UL,

   // This is a 3 bit field.
   INHERITFLAG_REVERTMASK = 0x1C000000UL,
   // This is its low bit.
   INHERITFLAG_REVERTBIT  = 0x04000000UL,

   // These 7 things are the choices available inside.
   INHERITFLAGRVRTK_REVERT= 0x04000000UL,
   INHERITFLAGRVRTK_REFLECT=0x08000000UL,
   INHERITFLAGRVRTK_RVF   = 0x0C000000UL,
   INHERITFLAGRVRTK_RFV   = 0x10000000UL,
   INHERITFLAGRVRTK_RVFV  = 0x14000000UL,
   INHERITFLAGRVRTK_RFVF  = 0x18000000UL,
   INHERITFLAGRVRTK_RFF   = 0x1C000000UL
};


// CFLAG1_FUDGE_TO_Q_TAG means three things, the main one being that the
// setup is to fudged from a suitably populated 3x4 into a 1/4 tag.
// The intention is that one can do "plenty" after doing a "1/2 press ahead"
// (as opposed to the more natural "extend") from waves.  It also means
// that we can fudge the other way if the schema is schema_in_out_triple
// or schema_in_out_triple_squash.  See the call "quick step part 2".
// It also means that a short6 is to be fudged to a 2x3.  See the call
// "quick step part 1".

// BEWARE!!  This list must track the table "flagtab1" in mkcalls.cpp .
// These flags go into the "callflags1" word of a callspec_block,
// and the "topcallflags1" word of the parse_state.

enum {
   CFLAG1_VISIBLE_FRACTION_MASK     = 0x00000003UL, // 2 bit field
   CFLAG1_VISIBLE_FRACTION_BIT      = 0x00000001UL, // its low bit
   CFLAG1_12_16_MATRIX_MEANS_SPLIT  = 0x00000004UL,
   CFLAG1_PRESERVE_Z_STUFF          = 0x00000008UL,
   CFLAG1_SPLIT_LIKE_DIXIE_STYLE    = 0x00000010UL,
   CFLAG1_PARALLEL_CONC_END         = 0x00000020UL,
   CFLAG1_TAKE_RIGHT_HANDS          = 0x00000040UL,
   CFLAG1_IS_STAR_CALL              = 0x00000080UL,
   CFLAG1_YOYO_FRACTAL_NUM          = 0x00000100UL,
   CFLAG1_FUDGE_TO_Q_TAG            = 0x00000200UL,
   CFLAG1_STEP_REAR_MASK            = 0x00001C00UL, // 3 bit field
   CFLAG1_STEP_TO_WAVE              = 0x00000400UL, // the encodings inside
   CFLAG1_REAR_BACK_FROM_R_WAVE     = 0x00000800UL,
   CFLAG1_STEP_TO_NONPHAN_BOX       = 0x00000C00UL,
   CFLAG1_REAR_BACK_FROM_QTAG       = 0x00001000UL,
   CFLAG1_STEP_TO_WAVE_4_PEOPLE     = 0x00001400UL,
   CFLAG1_REAR_BACK_FROM_EITHER     = 0x00001800UL,
   CFLAG1_STEP_TO_QTAG              = 0x00001C00UL,
   CFLAG1_DISTRIBUTE_REPETITIONS    = 0x00002000UL,
   CFLAG1_NUMBER_MASK               = 0x0001C000UL, // 3 bit field
   CFLAG1_NUMBER_BIT                = 0x00004000UL, // its low bit
   CFLAG1_LEFT_MEANS_TOUCH_OR_CHECK = 0x00020000UL,
   CFLAG1_LEFT_ONLY_IF_HALF         = 0x00040000UL,
   CFLAG1_SEQUENCE_STARTER          = 0x00080000UL,
   CFLAG1_SEQUENCE_STARTER_ONLY     = 0x00100000UL,
   CFLAG1_DONT_USE_IN_RESOLVE       = 0x00200000UL,
   CFLAG1_DONT_USE_IN_NICE_RESOLVE  = 0x00400000UL,
   CFLAG1_SPLIT_LARGE_SETUPS        = 0x00800000UL,
   CFLAG1_SPLIT_IF_Z                = 0x01000000UL,
   CFLAG1_BASE_TAG_CALL_MASK        = 0x0E000000UL, // 3 bit field
   CFLAG1_BASE_TAG_CALL_BIT         = 0x02000000UL, // its low bit
   CFLAG1_BASE_CIRC_CALL            = 0x10000000UL,
   CFLAG1_ENDS_TAKE_RIGHT_HANDS     = 0x20000000UL,
   CFLAG1_FUNNY_MEANS_THOSE_FACING  = 0x40000000UL,
   CFLAG1_SPLIT_LIKE_SQUARE_THRU    = 0x80000000UL
};

// These are the continuation of the "CFLAG1" bits, that have to overflow into this word.
// They must lie in the top 12 bits for now.
enum {
   CFLAG2_CAN_BE_ONE_SIDE_LATERAL   = 0x00100000UL,
   CFLAG2_NO_ELONGATION_ALLOWED     = 0x00200000UL,
   CFLAG2_IMPRECISE_ROTATION        = 0x00400000UL,
   CFLAG2_CAN_BE_FAN                = 0x00800000UL,
   CFLAG2_EQUALIZE                  = 0x01000000UL,
   CFLAG2_ONE_PERSON_CALL           = 0x02000000UL,
   CFLAG2_YIELD_IF_AMBIGUOUS        = 0x04000000UL,
   CFLAG2_DO_EXCHANGE_COMPRESS      = 0x08000000UL,
   CFLAG2_IF_MOVE_CANT_ROLL         = 0x10000000UL,
   CFLAG2_FRACTIONAL_NUMBERS        = 0x20000000UL
};

// Beware!!  This list must track the table "matrixcallflagtab" in mkcalls.cpp .
enum {
   MTX_USE_SELECTOR           = 0x01,
   MTX_STOP_AND_WARN_ON_TBONE = 0x02,
   MTX_TBONE_IS_OK            = 0x04,
   MTX_IGNORE_NONSELECTEES    = 0x08,
   MTX_MUST_FACE_SAME_WAY     = 0x10,
   MTX_FIND_JAYWALKERS        = 0x20,
   MTX_BOTH_SELECTED_OK       = 0x40,
   MTX_FIND_SQUEEZERS         = 0x80,
   MTX_FIND_SPREADERS         = 0x100,
   MTX_USE_VEER_DATA          = 0x200,
   MTX_USE_NUMBER             = 0x400,
   MTX_MIRROR_IF_RIGHT_OF_CTR = 0x800
};


// These definitions help to encode things in the "qualifierstuff" field
// of a call definition.  That field is an unsigned 16 bit integer.  The low
// 7 bits are a cast of the qualifier kind itself (hence we allow 127
// nontrivial qualifiers).  The high 9 bits are as follows.

enum {
   // The next two must be consecutive for encoding in the "assump_both" field.
   QUALBIT__LEFT           = 0x8000,
   QUALBIT__RIGHT          = 0x4000,
   QUALBIT__LIVE           = 0x2000,
   QUALBIT__TBONE          = 0x1000,  // TBONE and NTBONE together mean "explicit assumption"
   QUALBIT__NTBONE         = 0x0800,
   // A 4 bit field.  If nonzero, there is a number requirement,
   // and the field is that number plus 1.
   QUALBIT__NUM_MASK       = 0x0780,
   QUALBIT__NUM_BIT        = 0x0080,
   QUALBIT__QUAL_CODE      = 0x007F
};


// BEWARE!!  This list must track the table "leveltab" in mkcalls.c .
// BEWARE!!  This list must track the table "getout_strings" in sdtables.c .
// BEWARE!!  This list must track the table "old_filename_strings" in sdtables.c .
// BEWARE!!  This list must track the table "filename_strings" in sdtables.c .
// BEWARE!!  This list must track the table "level_threshholds" in sdtables.c .
// BEWARE!!  This list must track the table "higher_acceptable_level" in sdtables.c .

enum dance_level {
   l_mainstream,
   l_plus,
   l_a1,
   l_a2,
   l_c1,
   l_c2,
   l_c3a,
   l_c3,
   l_c3x,
   l_c4a,
   l_c4,
   l_c4x,
   l_dontshow,
   l_nonexistent_concept,   // We can't have more than 16 levels.

   // Tags for some of the above.

   dixie_grand_level = l_plus,
   extend_34_level = l_plus,
   zig_zag_level = l_a2,
   beau_belle_level = l_a2,
   cross_by_level = l_c1,
   intlk_triangle_level = l_c2,
   general_magic_level = l_c3,
   phantom_tandem_level = l_c4a,
   Z_CLW_level = l_c4a
};

/* These are the states that people can be in, and the "ending setups" that can appear
   in the call data base. */
/* BEWARE!!  This list must track the array "estab" in mkcalls.cpp . */
/* BEWARE!!  This list must track the array "setup_attrs" in sdtables.cpp . */
/* BEWARE!!  The procedure "merge_setups" canonicalizes pairs of setups by their
   order in this list, and will break if it is re-ordered randomly.  See the comments
   there before changing the order of existing setups. In general, keep small setups
   first, particularly 4-person setups before 8-person setups. */

enum setup_kind {
   nothing,
   s1x1,
   s1x2,
   s1x3,
   s2x2,
   s1x4,
   sdmd,
   s_star,
   s_trngl,
   s_trngl4,
   s_bone6,
   s_short6,
   s1x6,
   s2x3,
   s_1x2dmd,
   s_2x1dmd,
   s_qtag,
   s_bone,
   s1x8,
   slittlestars,
   s_2stars,
   s1x3dmd,
   s3x1dmd,
   s_spindle,
   s_hrglass,
   s_dhrglass,
   s_hyperglass,
   s_crosswave,
   s2x4,
   s2x5,
   sd2x5,
   s_ntrgl6cw,
   s_ntrgl6ccw,
   s_nftrgl6cw,
   s_nftrgl6ccw,
   s_ntrglcw,
   s_ntrglccw,
   s_nptrglcw,
   s_nptrglccw,
   s_nxtrglcw,
   s_nxtrglccw,
   spgdmdcw,
   spgdmdccw,
   swqtag,
   sdeep2x1dmd,
   swhrglass,
   s_rigger,
   s3x3,
   s3x4,
   s2x6,
   s2x7,
   s2x9,
   s_d3x4,
   s1p5x8,   // internal use only
   s1p5x4,   // internal use only
   s2x8,
   s4x4,
   s1x10,
   s1x12,
   s1x14,
   s1x16,
   s_c1phan,
   s_hyperbone,   /* internal use only */
   s_bigblob,
   s_ptpd,
   s3dmd,
   s4dmd,
   s3ptpd,
   s4ptpd,
   s_hsqtag,
   s_dmdlndmd,
   s_hqtag,
   s_wingedstar,
   s_wingedstar12,
   s_wingedstar16,
   s_barredstar,
   s_barredstar12,
   s_barredstar16,
   s_galaxy,
   sbigh,
   sbigx,
   s3x6,
   s3x8,
   s4x5,
   s4x6,
   s2x10,
   s2x12,
   sdeepqtg,
   sdeepbigqtg,
   swiderigger,
   sdeepxwv,
   s3oqtg,
   s_thar,
   s_alamo,
   sx4dmd,    // These are too big to actually represent --
   s8x8,      // we don't let them out of their cage.
   sxequlize, // Ditto.
   sx1x16,    // Ditto.
   shypergal, // Ditto.
   shyper4x8a,// Ditto.
   shyper4x8b,// Ditto.
   shyper3x8, // Ditto.
   shyper2x16,// Ditto.
   sfat2x8,   // Ditto.  These are big setups that are the size of 4x8's,
   swide4x4,  // but only have 16 people.  The reason is to prevent loss of phantoms.
   s_323,
   s_343,
   s_525,
   s_545,
   sh545,
   s_3mdmd,
   s_3mptpd,
   s_4mdmd,
   s_4mptpd,
   sbigbigh,
   sbigbigx,
   sbigrig,
   sbighrgl,
   sbigdhrgl,
   sbigbone,
   sdblbone6,
   sbigdmd,
   sbigptpd,
   sbig3x1dmd,
   sbig1x3dmd,
   sbig3dmd,
   sbig4dmd,
   sdblxwave,
   sdblspindle,
   sdblbone,
   sdblrig,
   s_dead_concentric,
   s_normal_concentric
};

// These are the "beginning setups" that can appear in the call data base.
// BEWARE!!  This list must track the array "sstab" in mkcalls.cpp .
// BEWARE!!  This list must track the array "begin_sizes" in mkcalls.cpp .
// BEWARE!!  This list must track the array "begin_sizes" in sdtables.cpp .

enum begin_kind {
   b_nothing,
   b_1x1,
   b_1x2,
   b_2x1,
   b_1x3,
   b_3x1,
   b_2x2,
   b_dmd,
   b_pmd,
   b_star,
   b_trngl,
   b_ptrngl,
   b_trngl4,
   b_ptrngl4,
   b_bone6,
   b_pbone6,
   b_short6,
   b_pshort6,
   b_1x2dmd,
   b_p1x2dmd,
   b_2x1dmd,
   b_p2x1dmd,
   b_qtag,
   b_pqtag,
   b_bone,
   b_pbone,
   b_rigger,
   b_prigger,
   b_3x3,
   b_2stars,
   b_p2stars,
   b_spindle,
   b_pspindle,
   b_hrglass,
   b_phrglass,
   b_dhrglass,
   b_pdhrglass,
   b_crosswave,
   b_pcrosswave,
   b_1x4,
   b_4x1,
   b_1x8,
   b_8x1,
   b_2x4,
   b_4x2,
   b_2x3,
   b_3x2,
   b_2x5,
   b_5x2,
   b_d2x5,
   b_d5x2,
   b_wqtag,
   b_pwqtag,
   b_deep2x1dmd,
   b_pdeep2x1dmd,
   b_whrglass,
   b_pwhrglass,
   b_1x6,
   b_6x1,
   b_3x4,
   b_4x3,
   b_2x6,
   b_6x2,
   b_2x7,
   b_7x2,
   b_2x9,
   b_9x2,
   b_d3x4,
   b_d4x3,
   b_2x8,
   b_8x2,
   b_4x4,
   b_1x10,
   b_10x1,
   b_1x12,
   b_12x1,
   b_1x14,
   b_14x1,
   b_1x16,
   b_16x1,
   b_c1phan,
   b_galaxy,
   b_3x6,
   b_6x3,
   b_3x8,
   b_8x3,
   b_4x5,
   b_5x4,
   b_4x6,
   b_6x4,
   b_2x10,
   b_10x2,
   b_2x12,
   b_12x2,
   b_deepqtg,
   b_pdeepqtg,
   b_deepbigqtg,
   b_pdeepbigqtg,
   b_widerigger,
   b_pwiderigger,
   b_deepxwv,
   b_pdeepxwv,
   b_3oqtg,
   b_p3oqtg,
   b_thar,
   b_alamo,
   b_ptpd,
   b_pptpd,
   b_1x3dmd,
   b_p1x3dmd,
   b_3x1dmd,
   b_p3x1dmd,
   b_3dmd,
   b_p3dmd,
   b_4dmd,
   b_p4dmd,
   b_3ptpd,
   b_p3ptpd,
   b_4ptpd,
   b_p4ptpd,
   b_hqtag,
   b_phqtag,
   b_hsqtag,
   b_phsqtag,
   b_wingedstar,
   b_pwingedstar,
   b_323,
   b_p323,
   b_343,
   b_p343,
   b_525,
   b_p525,
   b_545,
   b_p545,
   bh545,
   bhp545,
   b_3mdmd,
   b_p3mdmd,
   b_3mptpd,
   b_p3mptpd,
   b_4mdmd,
   b_p4mdmd,
   b_4mptpd,
   b_p4mptpd,
   b_bigh,
   b_pbigh,
   b_bigx,
   b_pbigx,
   b_bigbigh,
   b_pbigbigh,
   b_bigbigx,
   b_pbigbigx,
   b_bigrig,
   b_pbigrig,
   b_bighrgl,
   b_pbighrgl,
   b_bigdhrgl,
   b_pbigdhrgl,
   b_bigbone,
   b_pbigbone,
   b_dblbone6,
   b_pdblbone6,
   b_bigdmd,
   b_pbigdmd,
   b_bigptpd,
   b_pbigptpd,
   b_big3x1dmd,
   b_pbig3x1dmd,
   b_big1x3dmd,
   b_pbig1x3dmd,
   b_big3dmd,
   b_pbig3dmd,
   b_big4dmd,
   b_pbig4dmd,
   b_dblxwave,
   b_pdblxwave,
   b_dblspindle,
   b_pdblspindle,
   b_dblbone,
   b_pdblbone,
   b_dblrig,
   b_pdblrig
};

// These bits are used in the "callarray_flags" field of a "callarray".
// There is room for 21 of them.

enum {
   // This one must be 1!!!!
   CAF__ROT                     = 0x1,
   CAF__FACING_FUNNY            = 0x2,
   // Next one says this is concentrically defined --- the "end_setup" slot
   // has the centers' end setup, and there is an extra slot with the ends' end setup.
   CAF__CONCEND                 = 0x4,
   // Next one meaningful only if previous one is set.
   CAF__ROT_OUT                 = 0x8,
   // This is a 3 bit field.
   CAF__RESTR_MASK             = 0x70,
   // These next 6 are the nonzero values it can have.
   CAF__RESTR_UNUSUAL          = 0x10,
   CAF__RESTR_FORBID           = 0x20,
   CAF__RESTR_RESOLVE_OK       = 0x30,
   CAF__RESTR_CONTROVERSIAL    = 0x40,
   CAF__RESTR_BOGUS            = 0x50,
   CAF__RESTR_ASSUME_DPT       = 0x60,
   CAF__PREDS                  = 0x80,
   CAF__NO_CUTTING_THROUGH    = 0x100,
   CAF__NO_FACING_ENDS        = 0x200,
   CAF__LATERAL_TO_SELECTEES  = 0x400,
   CAF__VACATE_CENTER         = 0x800,
   CAF__OTHER_ELONGATE       = 0x1000,
   CAF__SPLIT_TO_BOX         = 0x2000,
   CAF__REALLY_WANT_DIAMOND  = 0x4000,
   CAF__NO_COMPRESS          = 0x8000,
   CAF__PLUSEIGHTH_ROTATION = 0x10000
};

// BEWARE!!  This list must track the array "qualtab" in mkcalls.cpp
enum call_restriction {
   cr_none,                // Qualifier only.
   cr_alwaysfail,          // Restriction only.
   cr_give_fudgy_warn,
   cr_wave_only,
   cr_wave_unless_say_2faced, // Not implemented.
   cr_all_facing_same,
   cr_1fl_only,
   cr_2fl_only,
   cr_2fl_per_1x4,
   cr_ctr_2fl_only,
   cr_3x3_2fl_only,
   cr_4x4_2fl_only,
   cr_leads_only,          // Restriction only.
   cr_trailers_only,       // Restriction only.
   cr_couples_only,
   cr_3x3couples_only,
   cr_4x4couples_only,
   cr_ckpt_miniwaves,      // Restriction only.
   cr_ctr_miniwaves,       // Restriction only.
   cr_ctr_couples,         // Restriction only.
   cr_awkward_centers,     // Restriction only.
   cr_dmd_same_pt,         // Qualifier only.
   cr_dmd_facing,
   cr_diamond_like,
   cr_qtag_like,
   cr_pu_qtag_like,
   cr_conc_iosame,
   cr_conc_iodiff,
   cr_reg_tbone,
   cr_gen_qbox,            // Qualifier only.
   cr_nice_diamonds,
   cr_nice_wv_triangles,
   cr_nice_tnd_triangles,
   cr_magic_only,
   cr_li_lo,               // Qualifier only.
   cr_ctrs_in_out,         // Qualifier only.
   cr_indep_in_out,        // Qualifier only.
   cr_miniwaves,
   cr_not_miniwaves,       // Qualifier only.
   cr_tgl_tandbase,        // Qualifier only.
   cr_true_Z_cw,           // Qualifier only.
   cr_true_Z_ccw,          // Qualifier only.
   cr_true_PG_cw,          // Qualifier only.
   cr_true_PG_ccw,         // Qualifier only.
   cr_lateral_cols_empty,  // Qualifier only.
   cr_ctrwv_end2fl,        // Qualifier only.
   cr_ctr2fl_endwv,        // Qualifier only.
   cr_split_dixie,         // Qualifier only.
   cr_not_split_dixie,     // Qualifier only.
   cr_dmd_ctrs_mwv,        // Qualifier only.
   cr_spd_base_mwv,        // Qualifier only.
   cr_qtag_mwv,            // Qualifier only.
   cr_qtag_mag_mwv,        // Qualifier only.
   cr_dmd_ctrs_1f,         // Qualifier only.
   cr_dmd_pts_mwv,         // Qualifier only.
   cr_dmd_pts_1f,          // Qualifier only.
   cr_dmd_intlk,
   cr_dmd_not_intlk,
   cr_tall6,               // Actually not checked as qualifier or restriction.
   cr_ctr_pts_rh,          // Qualifier only.
   cr_ctr_pts_lh,          // Qualifier only.
   cr_extend_inroutl,      // Qualifier only.
   cr_extend_inloutr,      // Qualifier only.
   cr_said_tgl,            // Qualifier only.
   cr_didnt_say_tgl,       // Qualifier only.
   cr_occupied_as_stars,   // Qualifier only.
   cr_occupied_as_clumps,  // Qualifier only.
   cr_occupied_as_blocks,  // Qualifier only.
   cr_occupied_as_h,       // Qualifier only.
   cr_occupied_as_qtag,    // Qualifier only.
   cr_occupied_as_3x1tgl,  // Qualifier only.
   cr_line_ends_looking_out, // Qualifier only.
   cr_col_ends_lookin_in,  // Qualifier only.
   cr_ripple_one_end,      // Qualifier only.
   cr_ripple_both_ends,    // Qualifier only.
   cr_ripple_both_ends_1x4_only, // Qualifier only.
   cr_ripple_both_centers, // Qualifier only.
   cr_ripple_any_centers,  // Qualifier only.
   cr_people_1_and_5_real, // Qualifier only.
   cr_ctrs_sel,
   cr_ends_sel,
   cr_all_sel,
   cr_not_all_sel,
   cr_some_sel,
   cr_none_sel,
   cr_nor_unwrap_sel,
   cr_ptp_unwrap_sel,
   cr_explodable,          // Restriction only.
   cr_rev_explodable,      // Restriction only.
   cr_peelable_box,        // Restriction only.
   cr_ends_are_peelable,   // Restriction only.
   cr_siamese_in_quad,
   cr_not_tboned_in_quad,
   cr_inroller_is_cw,
   cr_inroller_is_ccw,
   cr_outroller_is_cw,
   cr_outroller_is_ccw,
   cr_judge_is_cw,
   cr_judge_is_ccw,
   cr_socker_is_cw,
   cr_socker_is_ccw,
   cr_levelplus,
   cr_levela1,
   cr_levela2,
   cr_levelc1,
   cr_levelc2,
   cr_levelc3,
   cr_levelc4,
   cr_not_tboned,          // Restriction only.
   cr_opposite_sex,        // Restriction only.
   cr_quarterbox_or_col,   // Restriction only.
   cr_quarterbox_or_magic_col, // Restriction only.
   cr_all_ns,              // Restriction only.
   cr_all_ew,              // Restriction only.
   cr_real_1_4_tag,        // Restriction only.
   cr_real_3_4_tag,        // Restriction only.
   cr_real_1_4_line,       // Restriction only.
   cr_real_3_4_line,       // Restriction only.
   cr_jleft,               // Restriction only.
   cr_jright,              // Restriction only.
   cr_ijleft,              // Restriction only.
   cr_ijright,             // Restriction only.
   NUM_QUALIFIERS          // Not really in the enumeration.
};

// Schemata with "maybe" in their names will be translated to other
// schemata based on the modifiers with which they are used.  This is
// done by "get_real_callspec_and_schema", which happens early in
// "move_with_real_call".  For example, the "single" modifier on a
// call will turn "schema_maybe_single_concentric" into
// "schema_single_concentric".

// After the above, schemata with "or" in their names will be
// translated to other schemata based on the setup.  For example,
// "schema_concentric_2_4_or_normal" will be turned into
// "schema_concentric_2_4" (2 centers and 4 outsides) if the setup has
// 6 spots, and into "schema_single_concentric" if the setup has 4
// spots.

// BEWARE!!  This list must track the array "schematab" in mkcalls.cpp .
// BEWARE!!  This list must track the array "schema_attrs" in sdtables.cpp .
// Also, "schema_sequential" must be the start of all the sequential ones.
enum calldef_schema {
   schema_concentric,
   schema_cross_concentric,
   schema_3x3k_concentric,
   schema_3x3k_cross_concentric,
   schema_4x4k_concentric,
   schema_4x4k_cross_concentric,
   schema_single_concentric,
   schema_single_cross_concentric,
   schema_grand_single_concentric,
   schema_grand_single_cross_concentric,
   schema_single_concentric_together,
   schema_single_cross_concentric_together,
   schema_maybe_matrix_single_concentric_together,
   schema_maybe_single_concentric,
   schema_maybe_single_cross_concentric,
   schema_maybe_grand_single_concentric,
   schema_maybe_grand_single_cross_concentric,
   schema_maybe_special_single_concentric,
   schema_maybe_special_single_concentric_or_2_4,
   schema_grand_single_or_matrix_concentric,
   schema_3x3_concentric,
   schema_4x4_lines_concentric,
   schema_4x4_cols_concentric,
   schema_maybe_nxn_lines_concentric,
   schema_maybe_nxn_cols_concentric,
   schema_maybe_nxn_1331_lines_concentric,
   schema_maybe_nxn_1331_cols_concentric,
   schema_1331_concentric,
   schema_1313_concentric,       // Not for public use!
   schema_1221_concentric,
   schema_concentric_diamond_line,
   schema_concentric_lines_z,
   schema_concentric_diamonds,
   schema_cross_concentric_diamonds,
   schema_concentric_zs,
   schema_cross_concentric_zs,
   schema_concentric_or_diamond_line,
   schema_concentric_or_6_2_line,
   schema_concentric_6_2,
   schema_cross_concentric_6_2,
   schema_concentric_6_2_line,
   schema_concentric_2_6,
   schema_cross_concentric_2_6,
   schema_concentric_2_4,
   schema_cross_concentric_2_4,
   schema_concentric_2_4_or_normal,
   schema_concentric_4_2,
   schema_concentric_4_2_prefer_1x4,
   schema_cross_concentric_4_2,
   schema_concentric_4_2_or_normal,
   schema_concentric_8_4,        // Not for public use!
   schema_concentric_big2_6,     // Not for public use!
   schema_concentric_2_6_or_2_4,
   schema_cross_concentric_2_6_or_2_4,
   schema_concentric_innermost,
   schema_cross_concentric_innermost,
   schema_concentric_double_innermost,
   schema_cross_concentric_double_innermost,
   schema_concentric_6p,
   schema_concentric_6p_or_normal,
   schema_concentric_6p_or_sgltogether,
   schema_cross_concentric_6p_or_normal,
   schema_concentric_others,
   schema_concentric_6_2_tgl,
   schema_concentric_to_outer_diamond,
   schema_concentric_no31dwarn,
   schema_conc_12,
   schema_conc_16,
   schema_conc_star,
   schema_conc_star12,
   schema_conc_star16,
   schema_conc_bar,
   schema_conc_bar12,
   schema_conc_bar16,
   schema_conc_o,
   schema_maybe_matrix_conc,
   schema_maybe_matrix_conc_star,
   schema_maybe_matrix_conc_bar,
   schema_checkpoint,
   schema_checkpoint_mystic_ok,
   schema_cross_checkpoint,
   schema_rev_checkpoint,
   schema_ckpt_star,
   schema_maybe_in_out_triple_squash,
   schema_in_out_triple_squash,
   schema_sgl_in_out_triple_squash,
   schema_3x3_in_out_triple_squash,
   schema_4x4_in_out_triple_squash,
   schema_in_out_triple,
   schema_sgl_in_out_triple,
   schema_3x3_in_out_triple,
   schema_4x4_in_out_triple,
   schema_in_out_quad,
   schema_in_out_12mquad,
   schema_in_out_triple_zcom,
   schema_select_leads,
   schema_select_headliners,
   schema_select_sideliners,
   schema_select_original_rims,
   schema_select_original_hubs,
   schema_select_those_facing_both_live,
   schema_select_ctr2,
   schema_select_ctr4,
   schema_select_ctr6,
   schema_select_who_can,
   schema_select_who_did,
   schema_select_who_didnt,
   schema_select_who_did_and_didnt,
   schema_lateral_6,             // Not for public use!
   schema_vertical_6,            // Not for public use!
   schema_intlk_lateral_6,       // Not for public use!
   schema_intlk_vertical_6,      // Not for public use!
   schema_by_array,
   schema_nothing,
   schema_nothing_noroll,
   schema_matrix,
   schema_partner_matrix,
   schema_roll,
   schema_recenter,
   schema_sequential,            // All after this point are sequential.
   schema_split_sequential,
   schema_sequential_with_fraction,
   schema_sequential_with_split_1x8_id,
   schema_sequential_alternate,
   schema_sequential_remainder,
   schema_alias                  // Not a schema once the program is running.
};


/* BEWARE!!  Some of these flags co-exist with other flags defined elsewhere.
   The early ones are "concentricity" flags.  They must co-exist with the
   setupflags defined in sd.h because they share the same word.  For that reason,
   the latter flags are defined at the high end of the word, and the concentricity
   flags shown here are at the low end.
   The last bunch of flags are pushed up against the high end of the word, so that
   they can exactly match some other flags.  The constant HERITABLE_FLAG_MASK
   embraces them.  The flags that must stay in step are in the "FINAL__XXX" group
   in sd.h, the "cflag__xxx" group in database.h, and the "dfm_xxx" group in
   database.h . There is compile-time code in sdinit.c to check that these
   constants are all in step.

   dfm_conc_demand_lines             --  concdefine outers: must be ends of lines at start
   dfm_conc_demand_columns           --  concdefine outers: must be ends of columns at start
   dfm_conc_force_lines              --  concdefine outers: force them to line spots when done
   dfm_conc_force_columns            --  concdefine outers: force them to column spots when done
   dfm_conc_force_otherway           --  concdefine outers: force them to other spots when done
   dfm_conc_force_spots              --  concdefine outers: force them to same spots when done
   dfm1_conc_concentric_rules        --  concdefine outers: apply actual concentric ("lines-to-lines/columns-to-columns") rule
   dfm1_suppress_elongation_warnings --  concdefine outers: suppress warn_lineconc_perp etc.
           NOTE: the above 8 flags are specified only in the second spec, even if the concept is
              cross-concentric, in which case the "demand" flags might be considered to belong
              with the first spec.
   DFM1_CALL_MOD_MASK                 --  concdefine/seqdefine: can substitute something under certain circumstances
                  specific encodings within this mask are:
                     1   "or_anycall"
                     2   "mandatory_anycall"
                     3   "allow_plain_mod"
                     4   "allow_forced_mod"
                     5   "or_secondary_call"
                     6   "mandatory_secondary_call"
   dfm1_repeat_n                      --  seqdefine: take a numeric argument and replicate this part N times
   dfm1_repeat_n_alternate            --  seqdefine: take a numeric argument and replicate this part and the next one N times alternately
   dfm1_endscando                     --  concdefine outers: can tell ends only to do this
   dfm1_repeat_nm1                    --  seqdefine: take a numeric argument and replicate this part N-1 times
   dfm1_roll_transparent              --  seqdefine: any person who is marked roll-neutral after this call has his previous roll status restored
   dfm1_cpls_unless_single            --  seqdefine: the do this part as couples, unless this call is being done "single"
                                                               and "single_is_inherited" was set

   DFM1_NUM_INSERT_MASK              --  if nonzero, shift that 3-bit number into the number fields
   DFM1_FRACTAL_INSERT               --  if on, any number insertion is to have 1<->3 switched.
   DFM1_NO_CHECK_MOD_LEVEL           --  don't check the level of a modifier like "interlocked" -- if the subcall
                                             says it is only legal at C3, accept it anyway.

   INHERITFLAG_DIAMOND               --  concdefine/seqdefine: if original call said "diamond" apply it to this part
   INHERITFLAG_LEFT                  --  concdefine/seqdefine: if original call said "left" apply it to this part
   INHERITFLAG_FUNNY                 --  concdefine/seqdefine: if original call said "funny" apply it to this part
   INHERITFLAG_INTLK                 --  concdefine/seqdefine: if original call said "interlocked" apply it to this part
   INHERITFLAG_MAGIC                 --  concdefine/seqdefine: if original call said "magic" apply it to this part
   INHERITFLAG_GRAND                 --  concdefine/seqdefine: if original call said "grand" apply it to this part
   INHERITFLAG_12_MATRIX             --  concdefine/seqdefine: if original call said "12matrix" apply it to this part
   INHERITFLAG_16_MATRIX             --  concdefine/seqdefine: if original call said "16matrix" apply it to this part
   INHERITFLAG_CROSS                 --  concdefine/seqdefine: if original call said "cross" apply it to this part
   INHERITFLAG_SINGLE                --  concdefine/seqdefine: if original call said "single" apply it to this part
*/


// These are the call modifiers bits.  They go in the "modifiers1" word of a by_def_item.
// BEWARE!!  The "CONC" stuff, and all the later stuff, must track the table "defmodtab1" in mkcalls.cpp .
// BEWARE!!  The "SEQ" stuff must track the table "seqmodtab1" in mkcalls.cpp .
// BEWARE!!  The union of all of these flags, which is encoded in DFM1_CONCENTRICITY_FLAG_MASK,
// must coexist with the CMD_MISC__ flags defined in sd.h .  Note that the bit definitions
// of those flags start where these end.  Keep it that way.  If any flags are added here,
// they must be taken away from the CMD_MISC__ flags.

enum {
   // These are the "conc" flags.  They overlay the "seq" flags.

   DFM1_CONC_DEMAND_LINES            = 0x00000001,
   DFM1_CONC_DEMAND_COLUMNS          = 0x00000002,
   DFM1_CONC_FORCE_LINES             = 0x00000004,
   DFM1_CONC_FORCE_COLUMNS           = 0x00000008,
   DFM1_CONC_FORCE_OTHERWAY          = 0x00000010,
   DFM1_CONC_FORCE_SPOTS             = 0x00000020,
   DFM1_CONC_CONCENTRIC_RULES        = 0x00000040,
   DFM1_SUPPRESS_ELONGATION_WARNINGS = 0x00000080,
   // Beware!!  The above "conc" flags must all lie within DFM1_CONCENTRICITY_FLAG_MASK.

   // These are the "seq" flags.  They overlay the "conc" flags.

   // Under normal conditions, we do *not* re-evaluate between parts.  This
   // flag overrides that and makes us re-evaluate.
   DFM1_SEQ_RE_EVALUATE              = 0x00000001UL,
   DFM1_SEQ_DO_HALF_MORE             = 0x00000002UL,
   // But, if we break up a call with something like "random", the convention
   // is to re-evaluate at the break point.  This flag, used for calls like
   // "patch the <anyone>" or "rims trade back", says that we *never* re-evaluate,
   // even if the call is broken up.
   DFM1_SEQ_NO_RE_EVALUATE           = 0x00000004UL,
   DFM1_SEQ_REENABLE_ELONG_CHK       = 0x00000008UL,
   DFM1_SEQ_REPEAT_N                 = 0x00000010UL,
   DFM1_SEQ_REPEAT_NM1               = 0x00000020UL,
   DFM1_SEQ_NORMALIZE                = 0x00000040UL,
   // Beware!!  The above "seq" flags must all lie within DFM1_CONCENTRICITY_FLAG_MASK.

   // End of the separate conc/seq flags.  This constant embraces them.
   // Beware!!  The above "conc" and "seq" flags must lie within this.
   // Beware also!!  The CMD_MISC__ flags must be disjoint from this.
   // If this mask is made bigger, be sure the CMD_MISC__ flags (in sd.h)
   // are moved out of the way.  If it is made smaller (to accommodate CMD_MISC__)
   // be sure the conc/seq flags stay inside it.
   DFM1_CONCENTRICITY_FLAG_MASK      = 0x000000FF,

   // BEWARE!!  The following ones must track the table "defmodtab1" in mkcalls.cpp
   // Start of miscellaneous flags.

   // This is a 3 bit field -- CALL_MOD_BIT tells where its low bit lies.
   DFM1_CALL_MOD_MASK                = 0x00000700UL,
   DFM1_CALL_MOD_BIT                 = 0x00000100UL,
   // Here are the codes that can be inside.
   DFM1_CALL_MOD_ANYCALL             = 0x00000100UL,
   DFM1_CALL_MOD_MAND_ANYCALL        = 0x00000200UL,
   DFM1_CALL_MOD_ALLOW_PLAIN_MOD     = 0x00000300UL,
   DFM1_CALL_MOD_ALLOW_FORCED_MOD    = 0x00000400UL,
   DFM1_CALL_MOD_OR_SECONDARY        = 0x00000500UL,
   DFM1_CALL_MOD_MAND_SECONDARY      = 0x00000600UL,

   DFM1_ONLY_FORCE_ELONG_IF_EMPTY    = 0x00000800UL,
   // spare:                         = 0x00001000UL,
   DFM1_ENDSCANDO                    = 0x00002000UL,
   DFM1_FINISH_THIS                  = 0x00004000UL,
   DFM1_ROLL_TRANSPARENT             = 0x00008000UL,
   DFM1_PERMIT_TOUCH_OR_REAR_BACK    = 0x00010000UL,
   DFM1_CPLS_UNLESS_SINGLE           = 0x00020000UL,
   // This is a 2 bit field -- NUM_SHIFT_BIT tells where its low bit lies.
   DFM1_NUM_SHIFT_MASK               = 0x000C0000UL,
   DFM1_NUM_SHIFT_BIT                = 0x00040000UL,
   // This is a 3 bit field -- NUM_INSERT_BIT tells where its low bit lies.
   DFM1_NUM_INSERT_MASK              = 0x00700000UL,
   DFM1_NUM_INSERT_BIT               = 0x00100000UL,
   DFM1_NO_CHECK_MOD_LEVEL           = 0x00800000UL,
   DFM1_FRACTAL_INSERT               = 0x01000000UL
};

enum  {
   // These are the individual codes.  They must fit in 3 bits.
   STB_NONE,      // Unknown if REVERSE off, "Z" if REVERSE on.
   STB_A,         // "A" - person turns anticlockwise from 1 to 4 quadrants
   STB_AC,        // "AC" - person turns anticlockwise once,
                  //     then clockwise 1 to 4 quadrants
   STB_AAC,       // "AAC" - person turns anticlockwise twice,
                  //     then clockwise 1 to 4 quadrants
   STB_AAAC,      // "AAAC" - person turns anticlockwise 3 times,
                  //     then clockwise 1 to 4 quadrants
   STB_AAAAC,     // "AAAAC" - person turns anticlockwise 4 times,
                  //     then clockwise 1 to 4 quadrants
   STB_AA,        // "AA" - person turns anticlockwise from 5 to 8 quadrants

   // This is the entire field in which it fits.
   STB_MASK = 15,

   // This bit reverses everything (or changes "none" to "Z".)
   STB_REVERSE = 8
};

/* These define the format of the short int (16 bits, presumably) items emitted
   for each person in a by-array call definition.  These will get read into the
   "arr" array of a predptr_pair or the "stuff.def" array of a callarray.

   The format of this item is:
       stability info     roll info     where to go     direction to face
           4 bits           3 bits        5 bits            4 bits

   The direction is in the special format
            north   10 octal
            south   12 octal
            east    01 octal
            west    03 octal
   that makes procedures "rotcw" etc. work correctly.  The constants
   d_north, d_south, d_east, d_west in sd.h are just these numbers
   with the 1000 (octal) bit, which is BIT_PERSON, added. */

enum {
   NDBROLL_BIT    = 0x0200,
   DBSTAB_BIT     = 0x1000
};
