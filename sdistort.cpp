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
   prepare_for_call_in_series
   minimize_splitting_info
   initialize_map_tables
   remove_z_distortion
   remove_tgl_distortion
   get_map_from_code
   new_divided_setup_move
   divided_setup_move
   new_overlapped_setup_move
   overlapped_setup_move
   do_phantom_2x4_concept
   do_phantom_stag_qtg_concept
   do_phantom_diag_qtg_concept
   distorted_2x2s_move
   distorted_move
   triple_twin_move
   do_concept_rigger
   common_spot_move
   triangle_move
   phantom_2x4_move
*/


#ifdef WIN32
#define SDLIB_API __declspec(dllexport)
#else
#define SDLIB_API
#endif

#include "sd.h"


extern void prepare_for_call_in_series(setup *result, setup *ss)
{
   *result = *ss;
   result->result_flags = RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}


extern void minimize_splitting_info(setup *ss, uint32 other_info)
{
   if ((ss->result_flags & RESULTFLAG__SPLIT_AXIS_XMASK) >
       (other_info & RESULTFLAG__SPLIT_AXIS_XMASK)) {
      ss->result_flags &= ~RESULTFLAG__SPLIT_AXIS_XMASK;
      ss->result_flags |= (other_info & RESULTFLAG__SPLIT_AXIS_XMASK);
   }

   if ((ss->result_flags & RESULTFLAG__SPLIT_AXIS_YMASK) >
       (other_info & RESULTFLAG__SPLIT_AXIS_YMASK)) {
      ss->result_flags &= ~(RESULTFLAG__SPLIT_AXIS_YMASK);
      ss->result_flags |= (other_info & RESULTFLAG__SPLIT_AXIS_YMASK);
   }
}


// Must be a power of 2.
static const uint32 NUM_MAP_HASH_BUCKETS = 128;

static map_thing *map_hash_table2[NUM_MAP_HASH_BUCKETS];

extern void initialize_map_tables(void)
{
   for (unsigned int i=0 ; i<NUM_MAP_HASH_BUCKETS ; i++)
      map_hash_table2[i] = (map_thing *) 0;

   for (map_thing *tab2p = map_init_table ; tab2p->inner_kind != nothing ; tab2p++) {
      uint32 code = MAPCODE(tab2p->inner_kind,tab2p->arity,tab2p->map_kind,tab2p->vert);
      tab2p->code = code;
      uint32 hash_num =
         (((code+(code>>8)) * ((unsigned int) 035761254233)) >> 13) &
         (NUM_MAP_HASH_BUCKETS-1);

      tab2p->next = map_hash_table2[hash_num];
      map_hash_table2[hash_num] = tab2p;
   }
}


extern void remove_z_distortion(setup *ss) THROW_DECL
{
   if (!(ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_MASK)) return;

   static const expand_thing fix_cw  = {{1, 2, 4, 5}, 4, s2x2, s2x3, 0};
   static const expand_thing fix_ccw = {{0, 1, 3, 4}, 4, s2x2, s2x3, 0};

   if (ss->kind != s2x3) fail("Internal error: Can't straighten 'Z'.");

   const expand_thing *fixer =
      (ss->cmd.cmd_misc2_flags & (CMD_MISC2__IN_Z_CW|CMD_MISC2__IN_AZ_CW)) ?
      &fix_cw : &fix_ccw;

   ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__IN_Z_MASK;
   ss->cmd.cmd_misc2_flags |= CMD_MISC2__DID_Z_COMPRESSBIT << (ss->rotation & 1);
   compress_setup(fixer, ss);
   update_id_bits(ss);
}


extern void remove_tgl_distortion(setup *ss) THROW_DECL
{
   if (!(ss->result_flags & RESULTFLAG__DID_TGL_EXPANSION))
      return;

   int rot;
   const expand_thing *eptr = (expand_thing *) 0;

   static const expand_thing thing1    = {{0, 1, 3},          3, s_trngl,  sdmd, 3};
   static const expand_thing thing2    = {{2, 3, 1},          3, s_trngl,  sdmd, 1};
   static const expand_thing thing1x8a = {{1, 3, 2, 5, 7, 6}, 6, s1x6,     s1x8, 0};
   static const expand_thing thing1x8b = {{0, 1, 3, 4, 5, 7}, 6, s1x6,     s1x8, 0};
   static const expand_thing thingptpa = {{1, 7, 6, 5, 3, 2}, 6, s_bone6,  s_ptpd, 0};
   static const expand_thing thingptpb = {{3, 0, 1, 7, 4, 5}, 6, s_short6, s_ptpd, 1};
   static const expand_thing thing2x4a = {{1, 2, 3, 5, 6, 7}, 6, s2x3,     s2x4, 0};
   static const expand_thing thing2x4b = {{0, 1, 2, 4, 5, 6}, 6, s2x3,     s2x4, 0};
   static const expand_thing thing2x4c = {{0, 3, 2, 4, 7, 6}, 6, s_bone6,  s2x4, 0};
   static const expand_thing thing2x4d = {{6, 0, 1, 2, 4, 5}, 6, s_short6, s2x4, 1};
   static const expand_thing thing2x4e = {{0, 3, 5, 4, 7, 1}, 6, s_bone6,  s2x4, 0};
   static const expand_thing thing2x4f = {{6, 7, 1, 2, 3, 5}, 6, s_short6, s2x4, 1};
   static const expand_thing thing1x4a = {{0, 1, 3},          3, s1x3,     s1x4, 0};
   static const expand_thing thing1x4b = {{1, 3, 2},          3, s1x3,     s1x4, 0};

   switch (ss->kind) {
   case s2x4:
      rot = ss->rotation & 1;

      if (ss->result_flags & RESULTFLAG__SPLIT_AXIS_XMASK)
         rot += 0x1000;
      if (ss->result_flags & RESULTFLAG__SPLIT_AXIS_YMASK)
         rot += 0x1001;
      if (!(rot & 0x1000)) goto losing;   // Demand that exactly one field was on.

      if (rot & 1) {
         // This is actually 1x4's.
         if (ss->people[3].id1 && ss->people[7].id1 &&
             !ss->people[0].id1 && !ss->people[4].id1)
            eptr = &thing2x4a;
         else if (ss->people[0].id1 && ss->people[4].id1 &&
                  !ss->people[3].id1 && !ss->people[7].id1)
            eptr = &thing2x4b;
      }
      else {
         // This is actually 2x2's.
         if (ss->people[0].id1 & ss->people[2].id1 &
             ss->people[4].id1 & ss->people[6].id1 & 010) {
            if (ss->people[0].id1 & (~ss->people[1].id1) &
                ss->people[2].id1 & ss->people[3].id1 &
                ss->people[4].id1 & (~ss->people[5].id1) &
                ss->people[6].id1 & ss->people[7].id1 & BIT_PERSON) {
               eptr = &thing2x4c;
            }
            if (ss->people[0].id1 & ss->people[1].id1 &
                ss->people[2].id1 & (~ss->people[3].id1) &
                ss->people[4].id1 & ss->people[5].id1 &
                ss->people[6].id1 & (~ss->people[7].id1) & BIT_PERSON) {
               eptr = &thing2x4d;
            }
         }
         else if (ss->people[1].id1 & ss->people[3].id1 &
                  ss->people[5].id1 & ss->people[7].id1 & 010) {
            if (ss->people[0].id1 & ss->people[1].id1 &
                (~ss->people[2].id1) & ss->people[3].id1 &
                ss->people[4].id1 & ss->people[5].id1 &
                (~ss->people[6].id1) & ss->people[7].id1 & BIT_PERSON) {
               eptr = &thing2x4e;
            }
            if ((~ss->people[0].id1) & ss->people[1].id1 &
                ss->people[2].id1 & ss->people[3].id1 &
                (~ss->people[4].id1) & ss->people[5].id1 &
                ss->people[6].id1 & ss->people[7].id1 & BIT_PERSON) {
               eptr = &thing2x4f;
            }
         }
      }
      goto check_and_do;
   case s1x8:
      if (ss->people[2].id1 && ss->people[6].id1 &&
          !ss->people[0].id1 && !ss->people[4].id1)
         eptr = &thing1x8a;
      else if (ss->people[0].id1 && ss->people[4].id1 &&
               !ss->people[2].id1 && !ss->people[6].id1)
         eptr = &thing1x8b;
      goto check_and_do;
   case s1x4:
      if (ss->people[0].id1 && !ss->people[2].id1)
         eptr = &thing1x4a;
      else if (ss->people[2].id1 && !ss->people[0].id1)
         eptr = &thing1x4b;
      goto check_and_do;
   case s2x2:
      ss->result_flags &= ~RESULTFLAG__DID_TGL_EXPANSION;
      if      (!ss->people[0].id1) rot = 3;
      else if (!ss->people[1].id1) rot = 2;
      else if (!ss->people[2].id1) rot = 1;
      else if (!ss->people[3].id1) rot = 0;
      else
         goto losing;

      ss->rotation += rot;
      canonicalize_rotation(ss);
      /* Now the empty spot is in the lower-left corner. */
      if ((!ss->people[0].id1) || (!ss->people[1].id1) || (!ss->people[2].id1))
         goto losing;

      if (ss->people[0].id1 & ss->people[2].id1 & 1) {
         (void) copy_person(ss, 3, ss, 2);
      }
      else if (ss->people[0].id1 & ss->people[2].id1 & 010) {
         rot--;
         ss->rotation--;
         canonicalize_rotation(ss);
      }
      else
         goto losing;

      (void) copy_person(ss, 2, ss, 1);
      (void) copy_person(ss, 1, ss, 0);
      (void) copy_person(ss, 0, ss, 3);
      ss->kind = s_trngl;
      ss->rotation -= rot;   // Put it back.
      break;
   case sdmd:
      if (ss->people[0].id1 && !ss->people[2].id1)
         eptr = &thing1;
      else if (ss->people[2].id1 && !ss->people[0].id1)
         eptr = &thing2;
      goto check_and_do;
   case s_ptpd:
      if (ss->people[2].id1 && ss->people[6].id1 &&
          !ss->people[0].id1 && !ss->people[4].id1)
         eptr = &thingptpa;
      else if (ss->people[0].id1 && ss->people[4].id1 &&
               !ss->people[2].id1 && !ss->people[6].id1)
         eptr = &thingptpb;
      goto check_and_do;
   default:
      goto losing;
   }

   return;

 check_and_do:
   if (!eptr) goto losing;
   ss->result_flags &= ~RESULTFLAG__DID_TGL_EXPANSION;
   compress_setup(eptr, ss);
   return;

 losing: fail("Bad ending setup for triangle-become-box.");
}


extern const map_thing *get_map_from_code(uint32 map_encoding)
{
   uint32 hash_num =
      (((map_encoding+(map_encoding>>8)) * ((unsigned int) 035761254233)) >> 13) &
      (NUM_MAP_HASH_BUCKETS-1);

   for (map_thing *q=map_hash_table2[hash_num] ; q ; q=q->next) {
      if (q->code == map_encoding) return q;
   }

   return (const map_thing *) 0;
}


static void innards(
   setup_command *sscmd,
   const map_thing *maps,
   long_boolean recompute_id,
   assumption_thing new_assume,
   long_boolean do_second_only,
   setup *x,
   setup *result) THROW_DECL
{
   int i, j;
   uint32 main_rotation;
   uint32 rotstate, pointclip;
   const map_thing *final_map = (map_thing *) 0;
   uint32 vrot;
   const veryshort *getptr;
   setup z[8];

   uint32 rot = maps->rot;
   int vert = (maps->vert ^ rot) & 1;
   int arity = maps->arity;
   int insize = setup_attrs[maps->inner_kind].setup_limits+1;
   long_boolean nonisotropic_1x2 = FALSE;
   long_boolean direct_putback = FALSE;
   long_boolean no_reuse_map = FALSE;
   uint32 mysticflag = sscmd->cmd_misc2_flags;
   mpkind map_kind = maps->map_kind;
   if (map_kind == MPKIND__NONISOTROP1) map_kind = MPKIND__SPLIT;

   clear_people(result);
   sscmd->cmd_misc2_flags &= ~(CMD_MISC2__MYSTIFY_SPLIT | CMD_MISC2__MYSTIFY_INVERT);

   for (i=0; i<arity; i++) {
      if (x[i].kind != nothing) {
         long_boolean mirror = FALSE;

         if (mysticflag & CMD_MISC2__MYSTIFY_SPLIT) {
            mirror = i & 1;
            if (mysticflag & CMD_MISC2__MYSTIFY_INVERT)
               mirror = !mirror;
         }

         x[i].rotation = 0;
         x[i].cmd = *sscmd;

         if ((x[i].cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_MASK) && arity == 3) {
            if (x[i].cmd.cmd_misc2_flags & CMD_MISC2__IN_AZ_CW)
               x[i].cmd.cmd_misc2_flags ^= (i == 1) ?
                  (CMD_MISC2__IN_AZ_CW ^ CMD_MISC2__IN_Z_CW) :
                  (CMD_MISC2__IN_AZ_CW ^ CMD_MISC2__IN_Z_CCW);
            else if (x[i].cmd.cmd_misc2_flags & CMD_MISC2__IN_AZ_CCW)
               x[i].cmd.cmd_misc2_flags ^= (i == 1) ?
                  (CMD_MISC2__IN_AZ_CCW ^ CMD_MISC2__IN_Z_CCW) :
                  (CMD_MISC2__IN_AZ_CCW ^ CMD_MISC2__IN_Z_CW);
         }

         /* It is clearly too late to expand the matrix -- that can't be what is wanted. */
         x[i].cmd.cmd_misc_flags = (x[i].cmd.cmd_misc_flags & ~(CMD_MISC__OFFSET_Z | CMD_MISC__MATRIX_CONCEPT)) | CMD_MISC__NO_EXPAND_MATRIX;
         if (map_kind != MPKIND__SPLIT) x[i].cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
         x[i].cmd.cmd_assume = new_assume;
         if (recompute_id) update_id_bits(&x[i]);

         if (mirror) {
            mirror_this(&x[i]);
            x[i].cmd.cmd_misc_flags ^= CMD_MISC__EXPLICIT_MIRROR;
         }

         impose_assumption_and_move(&x[i], &z[i]);

         if (mirror)
            mirror_this(&z[i]);
      }
      else {
         z[i].kind = nothing;
         z[i].result_flags = 0;
      }

      if (arity >= 2 &&
          (z[i].result_flags & (RESULTFLAG__IMPRECISE_ROT | RESULTFLAG__PLUSEIGHTH_ROT)))
         fail("Rotation is imprecise or is 45 degrees.");
   }

   /* If this is a special map that flips some setup upside-down, do so. */

   if (arity <= 4) {      /* For arity >= 5, real rotation creeps into those bits. */
      if (rot & 0x200) {
         z[0].rotation += 2;
         canonicalize_rotation(&z[0]);
      }
      if (rot & 0x100) {
         z[1].rotation += 2;
         canonicalize_rotation(&z[1]);
      }
   }

   if (arity == 2 &&
       z[0].kind == s1x2 &&
       z[1].kind == s1x2 &&
       map_kind == MPKIND__SPLIT &&
       ((z[0].rotation ^ z[1].rotation) & 1) &&
       !(sscmd->cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)) {
      canonicalize_rotation(&z[0]);
      canonicalize_rotation(&z[1]);
      result->result_flags = get_multiple_parallel_resultflags(z, arity);
      nonisotropic_1x2 = TRUE;
      goto noniso1;
   }
   else if (map_kind == MPKIND__SPEC_ONCEREM) {
      canonicalize_rotation(&z[0]);
      canonicalize_rotation(&z[1]);
      result->result_flags = get_multiple_parallel_resultflags(z, arity);
      goto noniso1;
   }
   else if (map_kind == MPKIND__SPEC_TWICEREM) {
      canonicalize_rotation(&z[0]);
      canonicalize_rotation(&z[1]);
      canonicalize_rotation(&z[2]);
      result->result_flags = get_multiple_parallel_resultflags(z, arity);
      goto noniso1;
   }

   /* If the starting map is of kind MPKIND__NONE, we will be in serious trouble unless
      the final setups are of the same kind as those shown in the map.  Fix_n_results
      has a tendency to try to turn diamonds into 1x4's whenever it can, that is,
      whenever the centers are empty.  We tell it not to do that if it will cause problems. */

   if (fix_n_results(arity,
                     map_kind == MPKIND__NONE ? maps->inner_kind : nothing,
                     z, rotstate, pointclip)) {
      result->kind = nothing;
      result->result_flags = 0;
      return;
   }

   if (!(rotstate & 0xF03) && map_kind == MPKIND__SPLIT) {
      if (!(rotstate & 0x0F0))
         fail("Can't do this orientation changer.");
      if (rotstate == 0x020) z[0].rotation++;
      map_kind = MPKIND__NONISOTROPIC;
   }
   else if (map_kind == MPKIND__NONISOTROPIC) {
      if (!(rotstate & 0xF30)) {
         uint32 myrot, zzrot;

         if (!(rotstate & 0x00F))
            fail("Can't do this orientation changer.");

         for (i=0,myrot=rot; i<arity; i++,myrot>>=2) {
            z[i].rotation += myrot&3;
            canonicalize_rotation(&z[i]);
         }

         zzrot = 0;

         for (i=0; i<arity; i++) {
            zzrot <<= 2;
            zzrot |= z[i].rotation & 3;
         }

         if (zzrot == 0) {
            map_kind = MPKIND__SPLIT;
            rot = 0;
            direct_putback = TRUE;
         }
         else {
            /* Put them back. */
            for (i=0,myrot=rot; i<arity; i++,myrot>>=2) {
               z[i].rotation -= myrot&3;
               canonicalize_rotation(&z[i]);
            }
         }
      }
      else if (!(rotstate & 0xF03)) {

         // Not sure what we need to do here.....

         if (!(rotstate & 0x0F0))
            fail("Can't do this orientation changer.");

         map_kind = MPKIND__SPLIT;

         // We need to undo the misrepresentation of the rotation
         // of the middle setup(s) that took place in the incoming map.
         // But fix_n_results already did it in a brute-force way, by forcing
         // rot(1) to be rot(0).  Now, if the map is MPKIND__NONISOTROPIC,
         // the center setup was rotated CCW at the start.  So it needs to be
         // rotated CW to fix it.  If rot(0) was 0, rot(1) was 1, so forcing it
         // to be the same as rot(0) effectively rotated it CCW, which is
         // wrong.  If rot(0) was 1, rot(1) was 0, so forcing it to be the same
         // as rot(0) effectively rotated it CW, which is correct.  Therefore,
         // if rot(0) is 0, setup 1 is off by 180 degrees.
         if ((z[0].rotation&3) == 0) {
            z[1].rotation += 2;
            if (arity == 4) z[2].rotation += 2;
         }

         direct_putback = TRUE;
      }
   }

   main_rotation = z[0].rotation;
   if (arity == 2 && (z[0].kind == s_trngl || z[0].kind == s_trngl4) && (rot & 0x200))
      main_rotation += 2;
   main_rotation &= 3;

   /* Set the final result_flags to the OR of everything that happened.
      The elongation stuff doesn't matter --- if the result is a 2x2
      being done around the outside, the procedure that called us
      (basic_move) knows what is happening and will fix that bit.
      Also, check that the "did_last_part" bits are the same. */

   result->result_flags = get_multiple_parallel_resultflags(z, arity);

   if (map_kind == MPKIND__LILZCOM) {
      if (result->result_flags & RESULTFLAG__DID_Z_COMPRESSMASK) {
         /* **** Actually, should just forcibly reinstate the Z's,
            the same as is done in sdconc.c at about 1679. */
         if (sscmd->cmd_misc2_flags & CMD_MISC2__IN_Z_CW)
            map_kind = MPKIND__LILZCW;
         else if (sscmd->cmd_misc2_flags & CMD_MISC2__IN_Z_CCW)
            map_kind = MPKIND__LILZCCW;
         else if (sscmd->cmd_misc2_flags & CMD_MISC2__IN_AZ_CW)
            map_kind = MPKIND__LILAZCW;
         else if (sscmd->cmd_misc2_flags & CMD_MISC2__IN_AZ_CCW)
            map_kind = MPKIND__LILAZCCW;
         else
            fail("Internal error: Can't figure out how to unwind anisotropic Z's.");

      }
      else
         map_kind = MPKIND__SPLIT;

      no_reuse_map = TRUE;
   }

   /* Some maps (the ones used in "triangle peel and trail") do not want the result
      to be reassembled, so we get out now.  These maps are indicated by arity = 1
      and warncode = 1. */

   if (arity == 1 && maps->warncode == 1) {
      *result = z[0];
      goto getout;
   }
   else if (do_second_only) {
      /* This was a T-boned "phantom lines" in which people were
         in the center lines only. */
      *result = z[1];
      goto getout;
   }

   if (map_kind == MPKIND__OVERLAP ||
       map_kind == MPKIND__INTLK ||
       map_kind == MPKIND__CONCPHAN) {
      if (vert & 1) {
         if (result->result_flags & RESULTFLAG__SPLIT_AXIS_YMASK)
            warn(warn__did_not_interact);
      }
      else {
         if (result->result_flags & RESULTFLAG__SPLIT_AXIS_XMASK)
            warn(warn__did_not_interact);
      }
   }

   // See if we can put things back with the same map we used before.

   if (z[0].kind == maps->inner_kind &&
       main_rotation == 0 &&
       !direct_putback &&
       !no_reuse_map) {
      if (sscmd->cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
         fail("Unnecessary use of matrix concept.");

      final_map = maps;
      result->rotation = 0;
      goto finish;
   }

   noniso1:

   switch (map_kind) {
      case MPKIND__4_EDGES:
      case MPKIND__4_QUADRANTS:
         /* These particular maps misrepresent the rotation of subsetups 2 and 4, so
            we have to repair things when a shape-changer is called. */
         z[1].rotation += 2;
         z[3].rotation += 2;
         break;
      case MPKIND__ALL_8:
      case MPKIND__NONISOTROPIC:
         /* These particular maps misrepresent the rotation of subsetup 2, so
            we have to repair things when a shape-changer is called. */
         /* But we don't do it if the funny nonisotropic stuff is being done. */
         if ((z[0].rotation&3) != 0 && !(rotstate & 0x0F0)) {
            z[1].rotation += 2;
            /* These misrepresent both setups 2 and 3. */
            if (arity == 4 && map_kind == MPKIND__NONISOTROPIC) z[2].rotation += 2;
         }
         break;
      case MPKIND__DMD_STUFF:
         if (!(rotstate & 0x00F))
            fail("Can't do this orientation changer.");
         if ((z[0].rotation&3) != 0)
            z[1].rotation += 2;
         break;
   }

   for (i=0; i<arity; i++) {
      z[i].rotation += (rot & 1) + vert;
      canonicalize_rotation(&z[i]);
   }

   if (sscmd->cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT) {
      int before_distance, after_distance;
      long_boolean deadconc = FALSE;

      before_distance = setup_attrs[maps->inner_kind].bounding_box[(rot ^ vert) & 1];
      if (z[0].kind == s_dead_concentric) {
         after_distance = setup_attrs[z[0].inner.skind].bounding_box[(z[0].rotation) & 1];
         deadconc = TRUE;
      }
      else
         after_distance = setup_attrs[z[0].kind].bounding_box[(z[0].rotation) & 1];

      if (deadconc) {
         for (i=0; i<arity; i++) {
            z[i].kind = z[i].inner.skind;
            z[i].rotation += z[i].inner.srotation;
            canonicalize_rotation(&z[i]);
         }
      }

      /* Now "before_distance" is the height of the starting setup in the direction in
         which the setups are adjoined, taking setup rotation into account.  "After_distance"
         is the height of the ending setups in that direction. */

      /* Check for special case of matrix stuff going to hourglasses or galaxies.
         We are not prepared to say how wide these setups are, but, if only the center
         diamond/box is occupied, we can do it. */

      if (arity == 2 && map_kind == MPKIND__SPLIT && before_distance == 2) {
         if (z[0].kind == s_hrglass && z[0].rotation == 1) {
            map_kind = MPKIND__OVERLAP;
            final_map = &map_ov_hrg_1;
            result->rotation = z[0].rotation;
            goto got_map;
         }
         else if (z[0].kind == s_galaxy && z[0].rotation == 0) {
            map_kind = MPKIND__OVERLAP;
            final_map = &map_ov_gal_1;
            result->rotation = z[0].rotation;
            goto got_map;
         }
      }
      else if (arity == 3 && map_kind == MPKIND__OVERLAP && before_distance == 4) {
         if (z[0].kind == s_qtag && z[0].rotation == 1) {
            map_kind = MPKIND__OVERLAP;
            final_map = &map_3o_qtag_1;
            result->rotation = z[0].rotation;
            goto got_map;
         }
      }

      if (before_distance == 0 || after_distance == 0) fail("Can't use matrix with this concept or setup.");

      /* Could this happen?  Could we really be trying to get out with a different
         setup than that with which we got in, but the setups have the same bounding
         box (taking rotation into account)?  Won't the test above, that checks for
         use of the same getout map, always catch this error?  We need the test here
         for something like, from normal diamonds, "work matrix in each diamond, and
         drop in".  The 2x2 and diamond setups have the same height. */

      if (before_distance == after_distance && !deadconc) fail("Unnecessary use of matrix concept.");

      /* If the setups are 50% overlapped, make the appropriate adjustment. */

      if (map_kind == MPKIND__OVERLAP) before_distance >>= 1;
      else if (map_kind != MPKIND__SPLIT) fail("Can't use matrix with this concept.");

      /* "Before_distance" is now the distance between flagpole centers of the virtual setups.
         See what kind of fudging we have to do to get the before and after distances to match.
         This may involve 50% overlapping of the result setups, or moving them far apart and putting
         empty setups into the resulting space.  Ideally we would like to have tables of maps indexed
         by the required distance, so we don't have to do this junky stuff. */

      if (before_distance == after_distance) {
         map_kind = MPKIND__SPLIT;
      }
      else if (before_distance*2 == after_distance) {
         map_kind = MPKIND__OVERLAP;
      }
      else if (before_distance == after_distance*2 && arity == 2) {
         map_kind = MPKIND__SPLIT;
         z[2] = z[1];
         clear_people(&z[1]);
         arity = 3;
      }
      else if (before_distance == after_distance*2 && arity == 3) {
         map_kind = MPKIND__SPLIT;
         z[4] = z[2];
         z[2] = z[1];
         clear_people(&z[3]);
         clear_people(&z[1]);
         arity = 5;
      }
      else if (before_distance == after_distance*3 && arity == 2) {
         map_kind = MPKIND__SPLIT;
         z[3] = z[1];
         clear_people(&z[1]);
         clear_people(&z[2]);
         arity = 4;
      }
      else
         fail("Can't do this matrix call.");
   }

   if (nonisotropic_1x2) {
      if ((z[0].rotation) & 1) {
         final_map = &map_tgl4_2;
      }
      else {
         z[0].rotation += 2;
         canonicalize_rotation(&z[0]);
         final_map = &map_tgl4_1;
      }

      result->rotation = 1;
      goto got_map;
   }

   if (map_kind == MPKIND__SPEC_ONCEREM) {
      if (z[0].kind == s2x2 || z[1].kind == s1x4) {
         setup temp = z[1];
         z[1] = z[0];
         z[0] = temp;
      }

      result->rotation = z[0].rotation;

      if (z[0].rotation == 0 && z[1].rotation == 0) {
         if (z[0].kind == sdmd && z[1].kind == s2x2) {
            final_map = &map_spndle_once_rem;
         }
         else if (z[0].kind == s1x4 && z[1].kind == sdmd) {
            final_map = &map_1x3dmd_once_rem;
         }
      }

      warn(warn__colocated_once_rem);
      goto got_map;
   }
   else if (map_kind == MPKIND__SPEC_TWICEREM) {
      result->rotation = z[0].rotation;

      if (z[0].rotation == 0 && z[1].rotation == 0 && z[2].rotation == 0) {
         if (z[0].kind == s2x2 && z[1].kind == s1x2 && z[2].kind == s1x2) {
            final_map = get_map_from_code(MAPCODE(s2x2,3,MPKIND__SPEC_TWICEREM,0));
         }
      }
      else if (z[0].rotation == 1 && z[1].rotation == 1 && z[2].rotation == 1) {
         if (z[0].kind == s1x4 && z[1].kind == s1x2 && z[2].kind == s1x2) {
            final_map = get_map_from_code(MAPCODE(s1x4,3,MPKIND__SPEC_TWICEREM,1));
         }
      }

      goto got_map;
   }

   if (maps == &map_tgl4_1 && z[0].kind == s1x2) {
      final_map = &map_tgl4_2;
      result->rotation = z[0].rotation;
      goto got_map;
   }

   switch (map_kind) {
   case MPKIND__O_SPOTS:
      warn(warn__to_o_spots);
      break;
   case MPKIND__X_SPOTS:
      warn(warn__to_x_spots);
      break;
   case MPKIND__STAG:
      if (z[0].kind == s_qtag)
         warn(warn__bigblockqtag_feet);
      else
         warn(warn__bigblock_feet);
      break;
   case MPKIND__DIAGQTAG:
   case MPKIND__DIAGQTAG4X6:
      warn(warn__diagqtag_feet);
      break;
   case MPKIND__INTLK:
      if ((z[0].kind == s3x4 || z[0].kind == s_qtag) && z[0].rotation == 1 && arity == 2) {
         map_kind = MPKIND__REMOVED;
         warn(warn_interlocked_to_6);
      }
      break;
   case MPKIND__SPLIT:
      /* If we went from a 4-person setup to a 1x6, we are expanding due to collisions.
         If no one is present at the potential collision spots in the inboard side, assume
         that there was no collision there, and leave just one phantom instead of two. */
      if (z[0].kind == s1x6 && z[0].rotation == 0 && arity == 2 &&
          insize == 4 &&
          !(z[0].people[3].id1 | z[0].people[4].id1 | z[1].people[0].id1 | z[1].people[1].id1)) {
         final_map = &map_1x8_1x6;
         result->rotation = z[0].rotation;
         goto got_map;
      }
      else if (z[0].kind == s_1x2dmd && z[0].rotation == 0 && arity == 2 &&
               insize == 4 &&
               !(z[0].people[3].id1 | z[0].people[4].id1 |
                 z[1].people[0].id1 | z[1].people[1].id1)) {
         final_map = &map_rig_1x6;
         result->rotation = z[0].rotation;
         goto got_map;
      }
      break;
   }

   result->rotation = z[0].rotation;

   if (map_kind == MPKIND__REMOVED && arity == 2 &&
       z[0].kind == s2x3 && (z[0].rotation & 1) != 0 && rot == 0) {
      final_map = &map_2x3_rmvr;
      result->rotation = z[0].rotation-1;
      goto got_map;
   }

   if (arity == 2 &&
       z[0].kind == s_trngl4 &&
       (z[0].rotation & 1) == 0 &&
       ((z[0].rotation ^ z[1].rotation) & 2)) {
      if ((z[0].rotation + rot) & 2) {
         if (map_kind == MPKIND__OFFS_L_HALF) {
            final_map = &map_lh_zzztgl;
            result->rotation = z[0].rotation-1+(rot&2);
            goto got_map;
         }
         else if (map_kind == MPKIND__OFFS_R_HALF) {
            final_map = get_map_from_code(MAPCODE(s_trngl4,2,MPKIND__OFFS_R_HALF,0));
            result->rotation = z[0].rotation-1+(rot&2);
            goto got_map;
         }
      }
      else {
         if (map_kind == MPKIND__OFFS_L_HALF) {
            final_map = get_map_from_code(MAPCODE(s_trngl4,2,MPKIND__OFFS_L_HALF,0));
            result->rotation = z[0].rotation+1+(rot&2);
            goto got_map;
         }
         else if (map_kind == MPKIND__OFFS_R_HALF) {
            final_map = &map_rh_zzztgl;
            result->rotation = z[0].rotation+1+(rot&2);
            goto got_map;
         }
      }
   }

   if (arity == 1 && z[0].kind == s_c1phan) {
      uint32 livemask = 0;

      for (i=0 ; i<=setup_attrs[z[0].kind].setup_limits ; i++) {
         livemask <<= 1;
         if (z[0].people[i].id1) livemask |= 1;
      }

      if (map_kind == MPKIND__OFFS_L_HALF) {
         if ((livemask & ~0xAAAA) == 0) {
            final_map = &map_lh_c1phana;
            result->rotation = z[0].rotation;
            goto got_map;
         }
         else if ((livemask & ~0x5555) == 0) {
            final_map = &map_lh_c1phanb;
            result->rotation = z[0].rotation;
            goto got_map;
         }
      }
      else if (map_kind == MPKIND__OFFS_R_HALF) {
         if ((livemask & ~0x5555) == 0) {
            final_map = &map_rh_c1phana;
            result->rotation = z[0].rotation;
            goto got_map;
         }
         else if ((livemask & ~0xAAAA) == 0) {
            final_map = &map_rh_c1phanb;
            result->rotation = z[0].rotation;
            goto got_map;
         }
      }
   }

   if (map_kind == MPKIND__REMOVED && arity == 2 &&
       (z[0].rotation & 1) &&
       (z[0].kind == s_trngl4 ||z[0].kind == s_trngl) == 1) {
      if (z[0].rotation & 2) {
         goto get_coded_map;
      }
      if ((z[1].rotation & 2) && z[0].kind == s_trngl4) {
         final_map = &map_p8_tgl4;
         goto got_map;
      }
   }

   if (map_kind == MPKIND__SPLIT && (z[0].kind == s_trngl || z[0].kind == s_trngl4)) {
      if (z[0].rotation & 2)
         map_kind = MPKIND__NONISOTROP1;
      else if (z[1].rotation & 2) {
         ;
      }
      else
         map_kind = MPKIND__NONE;   /* Raise an error. */
   }
   else if (arity == 2) {
      if (z[0].rotation & 2)
         map_kind = MPKIND__NONE;   /* Raise an error. */
      else if (z[1].rotation & 2)
         map_kind = MPKIND__NONE;   /* Raise an error. */
   }

   get_coded_map:

   if (map_kind == MPKIND__NONE)
      fail("Can't do this shape-changing call here.");

   final_map = get_map_from_code(MAPCODE(z[0].kind,arity,map_kind,z[0].rotation & 1));

   got_map:

   if ((sscmd->cmd_misc_flags & CMD_MISC__OFFSET_Z) && final_map) {
      if (map_kind == MPKIND__OFFS_L_HALF || map_kind == MPKIND__OFFS_R_HALF) {
         if (final_map->outer_kind == s2x6) warn(warn__check_pgram);
         else final_map = 0;        // Raise an error.
      }
      else if (map_kind == MPKIND__OFFS_L_FULL || map_kind == MPKIND__OFFS_R_FULL) {
         if (final_map->outer_kind == s2x8) warn(warn__full_pgram);
         else final_map = 0;        /* Raise an error. */
      }
   }

   if (!final_map) {
      if (arity == 1) {
         switch (map_kind) {
            case MPKIND__OFFS_L_HALF:
            case MPKIND__OFFS_R_HALF:
            case MPKIND__OFFS_L_FULL:
            case MPKIND__OFFS_R_FULL:
               fail("Don't know how far to re-offset this.");
               break;
         }
      }

      if (arity == 2 && z[0].kind == s4x4 && map_kind == MPKIND__SPLIT) {
         /* We allow the special case of appending two 4x4's, if the real people
            (this includes active phantoms!) can fit inside a 4x4 or 2x8. */
         if (z[0].people[0].id1 | z[0].people[4].id1 |
             z[0].people[5].id1 | z[0].people[6].id1 |
             z[0].people[8].id1 | z[0].people[12].id1 |
             z[0].people[13].id1 | z[0].people[14].id1 |
             z[1].people[0].id1 | z[1].people[4].id1 |
             z[1].people[5].id1 | z[1].people[6].id1 |
             z[1].people[8].id1 | z[1].people[12].id1 |
             z[1].people[13].id1 | z[1].people[14].id1)
            final_map = &map_w4x4_4x4;
         else
            final_map = &map_f2x8_4x4;
      }
      else if (arity == 2 &&
               z[0].kind == s2x8 &&
               z[0].rotation == 1 &&
               map_kind == MPKIND__SPLIT) {
         if (z[1].people[8].id1 | z[1].people[9].id1 |
             z[1].people[10].id1 | z[1].people[11].id1 |
             z[1].people[12].id1 | z[1].people[13].id1 |
             z[1].people[14].id1 | z[1].people[15].id1 |
             z[0].people[0].id1 | z[0].people[1].id1 |
             z[0].people[2].id1 | z[0].people[3].id1 |
             z[0].people[4].id1 | z[0].people[5].id1 |
             z[0].people[6].id1 | z[0].people[7].id1)
            final_map = &map_w4x4_2x8;
         else
            final_map = &map_f2x8_2x8;
      }

      if (!final_map) fail("Can't do this shape-changing call with this concept.");
   }

   insize = setup_attrs[final_map->inner_kind].setup_limits+1;

   if (final_map == &map_tgl4_2)
      result->rotation += 2;

   if ((z[0].rotation & 1) && (final_map->rot & 1))
      result->rotation = 0;
   else if (arity == 2 &&
            z[0].rotation == 2 &&
            z[1].rotation == 0 &&
            (final_map->rot & 0xF) == 0x2)
      result->rotation = 0;

   result->rotation -= vert;

   /* A few maps require that a warning be given. */
   /* Code 1 is used elsewhere, for "triangle peel and trail". */
   if (final_map->warncode == 2) warn(warn__offset_gone);
   if (final_map->warncode == 3) warn(warn__overlap_gone);
   if (final_map->warncode == 4) warn(warn__check_hokey_4x4);
   if (final_map->warncode == 5) warn(warn_pg_in_2x6);
   if (final_map->warncode == 6) warn(warn__phantoms_thinner);
   if (final_map->warncode == 7) warn(warn_partial_solomon);

   finish:

   // If this is a special map that expects some setup to have been flipped upside-down, do so.

   if (arity == 2) {
      if (final_map->rot & 0x200) {
         z[0].rotation += 2;
         canonicalize_rotation(&z[0]);
      }
      if (final_map->rot & 0x100) {
         z[1].rotation += 2;
         canonicalize_rotation(&z[1]);
      }
   }

   if (arity != final_map->arity) fail("Confused about number of setups to divide into.");

   if (final_map->code == MAPCODE(s2x4,2,MPKIND__OFFS_L_THRQ, 1) ||
       final_map->code == MAPCODE(s2x4,2,MPKIND__OFFS_R_THRQ, 1)) {
      // We need to check for the possibility that we have to go to a 4x5 instead
      // of a 2x7.  All this is needed because we do not support a 4x7 matrix,
      // (just too big) so we assume a 2x7 and hope for the best.  The best
      // doesn't always work out.

      getptr = final_map->maps;

      for (j=0 ; j<arity ; j++) {
         for (i=0 ; i<insize ; i++) {
            if (*getptr++ < 0 && z[j].people[i].id1 != 0)
               goto emergency;
         }
      }

      goto no_emergency;

   emergency:

      // Yow!
      if (final_map->code == MAPCODE(s2x4,2,MPKIND__OFFS_L_THRQ, 1))
         final_map = &map_emergency1;
      else
         final_map = &map_emergency2;
   }
   else if (final_map->code == MAPCODE(sdmd,3,MPKIND__SPLIT, 0) &&
            pointclip == 2) {
      // If we are putting together what seem to be triple point-to-point
      // diamonds, but the center diamond was actually a 1x4, lacking ends,
      // and perpendicular to the outer diamonds, the points of that center
      // "diamond" don't exist at all.  Squeeze those spots out.  Unfortunately,
      // there is no setup for general point-to-point diamonds with a
      // perpendicular 1x2 between them.  The closest we can come is a
      // "distorted hourglass", which requires that the outermost points
      // be nonexistent.  See test bi02t (or bi01t).
      if (z[0].people[0].id1 || z[1].people[0].id1 ||
          z[1].people[2].id1 || z[2].people[2].id1)
         fail("Can't do this shape-changing call here.");
      final_map = &map_fix_triple_turnstyle;
   }


 no_emergency:

   getptr = final_map->maps;
   vrot = final_map->per_person_rot;

   for (j=0,rot=final_map->rot ; j<arity ; j++,rot>>=2) {
      if (j == 1 && final_map->map_kind == MPKIND__SPEC_TWICEREM)
         insize = setup_attrs[*getptr++].setup_limits+1;

      for (i=0 ; i<insize ; i++) {
         install_rot(result, *getptr++, &z[j], i, 011*((rot+vrot) & 3));
         vrot >>= 2;
      }
   }

   result->kind = final_map->outer_kind;

   getout:

   canonicalize_rotation(result);
}


extern void new_divided_setup_move(
   setup *ss,
   uint32 map_encoding,
   phantest_kind phancontrol,
   long_boolean recompute_id,
   setup *result) THROW_DECL
{
   divided_setup_move(
      ss,
      get_map_from_code(map_encoding),
      phancontrol,
      recompute_id,
      result);
}


extern void divided_setup_move(
   setup *ss,
   Const map_thing *maps,
   phantest_kind phancontrol,
   long_boolean recompute_id,
   setup *result) THROW_DECL
{
   int i, j;
   int vflags[8];
   setup x[8];
   setup_kind kn_twicerem;

   if (!maps || ss->kind != maps->outer_kind)
      fail("Can't do this concept in this setup.");

   assumption_thing t = ss->cmd.cmd_assume;
   setup_kind kn = maps->inner_kind;
   int insize = setup_attrs[kn].setup_limits+1;
   const veryshort *getptr = maps->maps;
   uint32 rot = maps->rot;
   uint32 vrot = maps->per_person_rot;
   int arity = maps->arity;
   uint32 frot;

   for (j=0,frot=rot; j<arity; j++,frot>>=2) {
      vflags[j] = 0;

      if (j == 1 && maps->map_kind == MPKIND__SPEC_TWICEREM) {
         kn_twicerem = (setup_kind) *getptr++;
         insize = setup_attrs[kn_twicerem].setup_limits+1;
      }

      for (i=0; i<insize; i++) {
         int mm = *getptr++;
         if (mm >= 0) {
            vflags[j] |= ss->people[mm].id1;
            (void) copy_rot(&x[j], i, ss, mm, 011*((0-frot-vrot) & 3));
         }
         else
            clear_person(&x[j], i);
         vrot >>= 2;
      }
   }

   switch (phancontrol) {
      case phantest_both:
         if (!(vflags[0] && vflags[1]))
            warn(warn__stupid_phantom_clw);   // Only one of the two setups is occupied.
         break;
      case phantest_only_one:
         if (vflags[0] && vflags[1]) fail("Can't find the setup to work in.");
         break;
      case phantest_only_one_pair:
         if ((vflags[0] || vflags[1]) && (vflags[2] || vflags[3]))
            fail("Can't find the setup to work in.");
         break;
      case phantest_only_first_one:
         if (vflags[1]) fail("Not in correct setup.");
         break;
      case phantest_only_second_one:
         if (vflags[0]) fail("Not in correct setup.");
         break;
      case phantest_first_or_both:
         if (!vflags[0])
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_2x2_both:
         // Test for "C1" blocks.
         if (!((vflags[0] | vflags[2]) && (vflags[1] | vflags[3])))
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_not_just_centers:
         if (!(vflags[0] | vflags[2]))
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_2x2_only_two:
         // Test for NOT "C1" blocks.
         if ((vflags[0] | vflags[2]) && (vflags[1] | vflags[3])) fail("Not in blocks.");
         break;
   }

   for (j=0; j<arity; j++)
      x[j].kind = (vflags[j]) ? kn : nothing;

   if (maps->map_kind == MPKIND__SPEC_ONCEREM)
      x[1].kind = (setup_kind) maps->maps[insize*2];
   else if (maps->map_kind == MPKIND__SPEC_TWICEREM) {
      x[1].kind = kn_twicerem;
      x[2].kind = kn_twicerem;
   }

   if (t.assumption == cr_couples_only || t.assumption == cr_miniwaves) {
      /* Pass it through. */
   }
   else if (recompute_id) {
      t.assumption = cr_none;
   }
   else {
      /* Fix a few things.  *** This probably needs more work. */
      /* Is this the right way to do it?  Absolutely not!  Will fix soon. */

      if (ss->kind == s2x2 && kn == s1x2 && t.assumption == cr_magic_only)
         t.assumption = cr_wave_only;    /* Only really correct if splitting into couples, not if into tandems.  Sigh... */
      else if (ss->kind == s2x2 && kn == s1x2 && t.assumption == cr_li_lo)
         t.assumption = cr_couples_only;
      else if (ss->kind == s2x2 && kn == s1x2 && t.assumption == cr_2fl_only)
         t.assumption = cr_couples_only;
      else if (ss->kind == s1x4 && kn == s1x2 && t.assumption == cr_magic_only)
         t.assumption = cr_wave_only;
      else if (ss->kind == s1x4 && kn == s1x2 && t.assumption == cr_1fl_only)
         t.assumption = cr_couples_only;
      else if (ss->kind == s1x4 && kn == s1x2 && t.assumption == cr_2fl_only)
         t.assumption = cr_couples_only;
      else if (ss->kind == s2x4 && kn == s1x4 && t.assumption == cr_li_lo)
         t.assumption = cr_1fl_only;
   }

   innards(
         &ss->cmd, maps, recompute_id, t,
         maps->map_kind == MPKIND__CONCPHAN && phancontrol == phantest_ctr_phantom_line && !vflags[0],
         x, result);

   /* "Innards" has returned with the splitting info correct for the subcalls, but
      not taking into account the incoming rotation in "ss->rotation".  We need to
      add onto that the splitting that we have just done, which also does not take
      the incoming rotation into account. */

   if (maps->map_kind == MPKIND__SPLIT ||
       maps->map_kind == MPKIND__NONISOTROP1 ||
       (arity == 2 &&
        (maps->map_kind == MPKIND__OFFS_L_HALF ||
         maps->map_kind == MPKIND__OFFS_R_HALF ||
         maps->map_kind == MPKIND__OFFS_L_FULL ||
         maps->map_kind == MPKIND__OFFS_R_FULL))) {
      if ((maps->vert ^ maps->rot) & 1) {
         if ((result->result_flags & RESULTFLAG__SPLIT_AXIS_YMASK) !=
             RESULTFLAG__SPLIT_AXIS_YMASK)
            result->result_flags += RESULTFLAG__SPLIT_AXIS_YBIT;
      }
      else {
         if ((result->result_flags & RESULTFLAG__SPLIT_AXIS_XMASK) !=
             RESULTFLAG__SPLIT_AXIS_XMASK)
            result->result_flags += RESULTFLAG__SPLIT_AXIS_XBIT;
      }
   }

   /* Now we reinstate the incoming rotation, which we have completely ignored up until
      now.  This will give the result "absolute" orientation.  (That is, absolute
      relative to the incoming "ss->rotation".  Some procedure (like a recursive call
      from ourselves) could have stripped that out.)

      This call will swap the SPLIT_AXIS fields if necessary, so that those fields
      will show the splitting in "absolute space". */

   reinstate_rotation(ss, result);
}




extern void new_overlapped_setup_move(setup *ss, uint32 map_encoding,
   uint32 *masks, setup *result) THROW_DECL
{
   overlapped_setup_move(
      ss,
      get_map_from_code(map_encoding),
      masks,
      result);
}


extern void overlapped_setup_move(setup *ss, Const map_thing *maps,
   uint32 *masks, setup *result) THROW_DECL
{
   int i, j, rot;
   uint32 k;
   setup x[8];
   assumption_thing t = ss->cmd.cmd_assume;

   if (!maps || ss->kind != maps->outer_kind)
      fail("Can't do this concept in this setup.");

   setup_kind kn = maps->inner_kind;
   int insize = setup_attrs[kn].setup_limits+1;
   int arity = maps->arity;
   const veryshort *mapbase = maps->maps;

   if (arity >= 8) fail("Can't handle this many overlapped setups.");

   for (j=0, rot=maps->rot ; j<arity ; j++, rot>>=2) {
      int rrr = 011*((-rot) & 3);
      clear_people(&x[j]);
      x[j].kind = kn;
      for (i=0, k=1; i<insize; i++, k<<=1, mapbase++) {
         if (k & masks[j])
            (void) copy_rot(&x[j], i, ss, *mapbase, rrr);
      }
   }

   t.assumption = cr_none;
   innards(&ss->cmd, maps, TRUE, t, FALSE, x, result);
   reinstate_rotation(ss, result);
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}


const expand_thing expand_big2x2_4x4 = {
   {12, 0, 4, 8}, 4, s2x2, s4x4, 0};

const expand_thing expand_2x6_4x6 = {
   {11, 10, 9, 8, 7, 6, 23, 22, 21, 20, 19, 18}, 12, s2x6, s4x6, 0};


/* This does bigblock, stagger, ladder, stairstep, "O", butterfly, and
   [split/interlocked] phantom lines/columns.  The concept block always
   provides the following:
         maps: the map to use (assuming not end-to-end)
         arg1: "phantest_kind" -- special stuff to watch for
         arg2: "linesp" -- 1 if these setups are lines; 0 if columns
         arg3: "map_kind" for the map */

/* This handles the end-to-end versions also.  We should either have a 4x4 or a 1x16. */

extern void do_phantom_2x4_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   // This concept is "standard", which means that it can look at global_tbonetest
   // and global_livemask, but may not look at anyone's facing direction other
   // than through global_tbonetest.

   int linesp = parseptr->concept->value.arg2 & 1;
   int rot = (global_tbonetest ^ linesp ^ 1) & 1;
   const map_thing *maps;

   if ((ss->cmd.cmd_misc2_flags & CMD_MISC2__MYSTIFY_SPLIT) &&
       parseptr->concept->value.arg3 != MPKIND__CONCPHAN)
      fail("Mystic not allowed with this concept.");

   if (parseptr->concept->value.arg2 == 3)
      ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

   // We allow stuff like "dodge [split phantom lines zing]" from a butterfly.
   if (ss->kind == s2x2 &&
       ss->cmd.prior_elongation_bits == 3 &&
       (ss->cmd.cmd_misc_flags & CMD_MISC__DOING_ENDS)) {
      expand_setup(&expand_big2x2_4x4, ss);
   }

   switch (ss->kind) {
   case s1x16:
      if ((global_tbonetest & 011) == 011)
         fail("Can't do this from T-bone setup, try using \"standard\".");

      if (linesp) {
         if (global_tbonetest & 1) fail("There are no lines of 4 here.");
      }
      else {
         if (global_tbonetest & 010) fail("There are no columns of 4 here.");
      }

      rot = 0;
      maps = get_map_from_code(MAPCODE(s1x8,2,parseptr->concept->value.arg3,0));
      break;
   case s4x4:

      if (parseptr->concept->value.arg3 != MPKIND__NONE)
         maps = get_map_from_code(MAPCODE(s2x4,2,parseptr->concept->value.arg3,1));
      else
         maps = parseptr->concept->value.maps;

      // Check for special case of "stagger" or "bigblock", without the word "phantom",
      // when people are not actually on block spots.

      if (parseptr->concept->value.arg3 == MPKIND__STAG &&
          parseptr->concept->value.arg1 == phantest_only_one) {
         if (global_livemask != 0x2D2D && global_livemask != 0xD2D2) {
            warn(warn__not_on_block_spots);
            distorted_move(ss, parseptr, disttest_any, result);
            return;
         }
      }

      if ((global_tbonetest & 011) == 011) {
         // People are T-boned!  This is messy.
         phantom_2x4_move(ss,
                          linesp,
                          (phantest_kind) parseptr->concept->value.arg1,
                          maps,
                          result);
         return;
      }

      break;
   case s2x6:
      // Check for special case of split phantom lines/columns in a parallelogram.

      if (parseptr->concept->value.arg3 == MPKIND__SPLIT) {
         if (rot) {
            if (global_tbonetest & 1) fail("There are no split phantom lines here.");
            else                      fail("There are no split phantom columns here.");
         }

         if      (global_livemask == 07474)
            maps = get_map_from_code(MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF,1));
         else if (global_livemask == 01717)
            maps = get_map_from_code(MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF,1));
         else fail("Must have a parallelogram for this.");

         warn(warn__split_phan_in_pgram);

         // Change the setup to a 4x6.
         expand_setup(&expand_2x6_4x6, ss);
         break;              // Note that rot is zero.
      }
      goto lose;
   default:
      goto lose;
   }

   ss->rotation += rot;   // Just flip the setup around and recanonicalize.
   canonicalize_rotation(ss);

   divided_setup_move(ss, maps, (phantest_kind) parseptr->concept->value.arg1, TRUE, result);
   result->rotation -= rot;   // Flip the setup back.
   // The split-axis bits are gone.  If someone needs them, we have work to do.
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
   return;

 lose:
   fail("Need a 4x4 setup to do this concept.");
}


extern void do_phantom_stag_qtg_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   int rot = ss->people[0].id1 | ss->people[4].id1 | ss->people[8].id1 | ss->people[12].id1;

   if (ss->kind != s4x4)
      fail("Need a 4x4 setup to do this concept.");

   if (!(rot & 011))
      fail("Can't figure this out -- not enough live people.");

   if ((rot & 011) == 011)
      fail("Can't figure this out -- facing directions are too weird.");

   rot = (rot ^ parseptr->concept->value.arg2) & 1;

   ss->cmd.cmd_misc_flags |= parseptr->concept->value.arg3;  /* The thing to verify, like CMD_MISC__VERIFY_1_4_TAG. */

   ss->rotation += rot;   /* Just flip the setup around and recanonicalize. */
   canonicalize_rotation(ss);

   new_divided_setup_move(
      ss, 
      MAPCODE(s_qtag,2,MPKIND__STAG,1),
      (phantest_kind) parseptr->concept->value.arg1,
      TRUE,
      result);

   result->rotation -= rot;   /* Flip the setup back. */
   /* The split-axis bits are gone.  If someone needs them, we have work to do. */
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}


extern void do_phantom_diag_qtg_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   uint32 mapcode;
   int rot = 0;

   if (ss->kind == s4x4) {
      rot =
         ss->people[1].id1 | ss->people[2].id1 | ss->people[5].id1 | ss->people[6].id1 |
         ss->people[9].id1 | ss->people[10].id1 | ss->people[13].id1 | ss->people[14].id1;

      if (!(rot & 011))
         fail("Can't figure this out -- not enough live people.");
      else if ((rot & 011) == 011)
         fail("Can't figure this out -- facing directions are too weird.");

      rot = (rot ^ parseptr->concept->value.arg2) & 1;

      ss->rotation += rot;   // Just flip the setup around and recanonicalize.
      canonicalize_rotation(ss);

      mapcode = MAPCODE(s_qtag,2,MPKIND__DIAGQTAG,1);
   }
   else if (ss->kind == s4x6) {
      int dir =
         ss->people[6].id1 | ss->people[11].id1 | ss->people[18].id1 | ss->people[23].id1;

      if ((dir & 011) == 011 ||
          !((dir ^ parseptr->concept->value.arg2) & 1))
         fail("Can't figure this out -- facing directions are too weird.");

      mapcode = MAPCODE(s_qtag,2,MPKIND__DIAGQTAG4X6,0);
   }
   else
      fail("Can't do this concept in this setup.");

   // The thing to verify, like CMD_MISC__VERIFY_1_4_TAG.
   ss->cmd.cmd_misc_flags |= parseptr->concept->value.arg3;

   new_divided_setup_move(ss, mapcode,
                          (phantest_kind) parseptr->concept->value.arg1,
                          TRUE, result);

   result->rotation -= rot;   // Flip the setup back.

   // The split-axis bits are gone.  If someone needs them, we have work to do.
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}



// This returns true if each pair of live people were consecutive,
// meaning this is a true Z line/column.
static long_boolean full_search(
   int num_searches,
   int length_to_search,
   long_boolean no_err_print,
   int num_to_find,
   veryshort map_to_fill[],
   const veryshort dests[],
   const veryshort srcs[],
   const setup *ss) THROW_DECL
{
   long_boolean retval = TRUE;

   for (int j=0 ; j<num_searches ; j++) {
      const veryshort *row = &srcs[j*length_to_search];
      int fill_count = 0;
      int z = num_to_find;   // Counts downward as we see consecutive people.
      long_boolean win = FALSE;

      for (int i=0; i<length_to_search; i++) {
         if (!ss->people[row[i]].id1)    // Live person here?
            z = num_to_find;     // No, reset the consecutivity counter.
         else {
            z--;            // Yes, count -- if get all in a row, turn "win" on.
            if (z == 0) win = TRUE;
            // Then push this index onto the map, check for no more than num_to_find.
            if (fill_count >= num_to_find) goto lose;
            map_to_fill[dests[j+num_searches*fill_count]] = row[i];
            fill_count++;
         }
      }
   
      /* Now check that the map has exactly num_to_find people.
         But if no_err_print was on, we don't print an error -- we just report as our
         return value whether there were exactly two people, regardless of consecutivity. */

      if (no_err_print) {
         if (fill_count != num_to_find) retval = FALSE;
         continue;
      }
      else if (fill_count == num_to_find) {
         if (!win) retval = FALSE;
         continue;
      }

   lose:

      if (!no_err_print)
         fail("Can't identify distorted line or column.");

      retval = FALSE;
   }

   return retval;
}                        


static const veryshort list_2x8[16] = {
   0, 15, 1, 14, 3, 12, 2, 13, 7, 8, 6, 9, 4, 11, 5, 10};

static const veryshort list_2x8_in[8] = {0, 1, 2, 3, 4, 5, 6, 7};

static const veryshort list_3x4_in[8] = {0, 1, 2, 3, 7, 6, 5, 4};
static const veryshort list_3x4_out[12] = {0, 10, 9, 1, 11, 8, 2, 5, 7, 3, 4, 6};

static const veryshort list_2x4_in[8] = {7, 0, 6, 1, 5, 2, 4, 3};

static const veryshort list_2x5_out[10] = {9, 8, 7, 6, 5, 0, 1, 2, 3, 4};
static const veryshort list_2x6_out[12] = {11, 10, 9, 8, 7, 6, 0, 1, 2, 3, 4, 5};
static const veryshort list_2x7_out[14] = {13, 12, 11, 10, 9, 8, 7, 0, 1, 2, 3, 4, 5, 6};
static const veryshort list_2x8_out[16] = {
   15, 14, 13, 12, 11, 10, 9, 8, 0, 1, 2, 3, 4, 5, 6, 7};
static const veryshort list_2x10_out[20] = {
   19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static const veryshort list_2x12_out[24] = {
   23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12,
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

static const veryshort list_foo_in[8] = {0, 4, 1, 5, 3, 7, 2, 6};
static const veryshort list_bar_in[8] = {8, 12, 9, 13, 11, 15, 10, 14};

static const veryshort list_4x4_out[16] = {
   8, 6, 5, 4, 9, 11, 7, 2, 10, 15, 3, 1, 12, 13, 14, 0};


extern void distorted_2x2s_move(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   // Maps for jays. These have the extra 24 to handle going to 1x4's.

   static const veryshort mapj1[48] = {
               7, 2, 4, 5, 0, 1, 3, 6,
               6, 3, 4, 5, 0, 1, 2, 7,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0,
               -1, -1, -1, -1, -1, -1, -1, -1};

   static const veryshort mapj2[48] = {
               6, 3, 4, 5, 0, 1, 2, 7,
               7, 2, 4, 5, 0, 1, 3, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5,
               -1, -1, -1, -1, -1, -1, -1, -1};

   static const veryshort mapj3[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               7, 2, 4, 5, 0, 1, 3, 6,
               6, 3, 4, 5, 0, 1, 2, 7,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0};

   static const veryshort mapj4[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               6, 3, 4, 5, 0, 1, 2, 7,
               7, 2, 4, 5, 0, 1, 3, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5};

   // Maps for facing/back-to-front/back-to-back parallelograms.
   // These have the extra 24 to handle going to 1x4's.

   static const veryshort mapk1[48] = {
               3, 2, 4, 5, 0, 1, 7, 6,
               6, 7, 4, 5, 0, 1, 2, 3,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0,
               -1, -1, -1, -1, -1, -1, -1, -1};
   static const veryshort mapk2[48] = {
               6, 7, 4, 5, 0, 1, 2, 3,
               3, 2, 4, 5, 0, 1, 7, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5,
               -1, -1, -1, -1, -1, -1, -1, -1};
   static const veryshort mapk3[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               3, 2, 4, 5, 0, 1, 7, 6,
               6, 7, 4, 5, 0, 1, 2, 3,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0};

   static const veryshort mapk4[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               6, 7, 4, 5, 0, 1, 2, 3,
               3, 2, 4, 5, 0, 1, 7, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5};

   int table_offset, arity, misc_indicator, i;
   setup inputs[4];
   setup results[4];
   uint32 directions, livemask, misc2_zflag;
   const veryshort *map_ptr;

   concept_descriptor *this_concept = parseptr->concept;

   // Check for special case of "interlocked parallelogram",
   // which doesn't look like the kind of concept we are expecting.

   if (this_concept->kind == concept_do_both_boxes) {
      table_offset = 8;
      misc_indicator = 3;
   }
   else {
      table_offset = this_concept->value.arg3;
      misc_indicator = this_concept->value.arg1;
   }

   // misc_indicator
   //      0       some kind of "Z"
   //      1       some kind of "Jay"
   //      2       twin parallelograms (the physical setup is like offset lines)
   //      3       interlocked boxes/Pgrams (the physical setup is like Z's)
   //      4       some kind of parallelogram (as from "heads travel thru")
   //      5       distorted blocks
   //      6       Z diamond(s)

   // Table_offset is 0, 8, or 12.  It selects the appropriate part of the maps.
   // For "Z"      :   0 == normal, 8 == interlocked.
   // For Jay/Pgram:   0 == normal, 8 == back-to-front, 16 == back-to-back.

   directions = 0;
   livemask = 0;
   arity = 2;

   if (ss->kind == s4dmd) {
      // A quadruple 1/4-tag is always construed as a matrix formation.
      // Maybe they fit into a 3x6; maybe not.  We expand to a 3x8
      // and then try to cut it back to a 3x6 or whatever.
      do_matrix_expansion(ss, CONCPROP__NEEDK_3X8, FALSE);
      normalize_setup(ss, simple_normalize);
   }

   for (i=0 ; i<=setup_attrs[ss->kind].setup_limits ; i++) {
      uint32 p = ss->people[i].id1;
      directions = ((directions & 0x3FFFFFFF)<<2) | (p&3);
      livemask <<= 1;
      if (p) livemask |= 1;
   }

   setup_kind inner_kind = s2x2;

   switch (misc_indicator) {
   case 0:
      // The concept is some variety of "Z".
      arity = this_concept->value.arg4;

      if (arity == 3) {
         switch (ss->kind) {
         case s3x6:
            // If outer Z's are ambiguous, make them look like inner ones.
            if ((livemask & 0630630) == 0)
               warn(warn_same_z_shear);

            if ((livemask & 0250250) == 0) {
               misc2_zflag = CMD_MISC2__IN_Z_CW;
               goto do_real_z_stuff;
            }
            else if ((livemask & 0520520) == 0) {
               misc2_zflag = CMD_MISC2__IN_Z_CCW;
               goto do_real_z_stuff;
            }
            else if ((livemask & 0310310) == 0) {
               misc2_zflag = CMD_MISC2__IN_AZ_CCW;
               goto do_real_z_stuff;
            }
            else if ((livemask & 0460460) == 0) {
               misc2_zflag = CMD_MISC2__IN_AZ_CW;
               goto do_real_z_stuff;
            }
            else goto lose;
         default:
            fail("Must have 3x6 for this concept.");
         }
      }
      else {
         switch (ss->kind) {
         case s4x4:
            {
               // Maps for 4x4 Z's.
               static const veryshort map1[16] =
               {12, 15, 11, 10, 3, 2, 4, 7, 12, 3, 7, 10, 15, 2, 4, 11};
               static const veryshort map2[16] =
               {12, 13, 3, 15, 11, 7, 4, 5, 12, 13, 7, 11, 15, 3, 4, 5};
               static const veryshort map3[16] =
               {14, 1, 2, 3, 10, 11, 6, 9, 11, 1, 2, 6, 10, 14, 3, 9};
               static const veryshort map4[16] =
               {13, 14, 1, 3, 9, 11, 5, 6, 13, 14, 11, 9, 3, 1, 5, 6};
               static const veryshort map5[16] =
               {10, 13, 15, 9, 7, 1, 2, 5, 10, 7, 5, 9, 13, 1, 2, 15};
               static const veryshort map6[16] =
               {13, 14, 15, 10, 7, 2, 5, 6, 13, 14, 2, 7, 10, 15, 5, 6};
               static const veryshort map7[16] =
               {3, 0, 1, 7, 9, 15, 11, 8, 15, 0, 1, 11, 9, 3, 7, 8};
               static const veryshort map8[16] =
               {14, 0, 3, 15, 11, 7, 6, 8, 14, 0, 7, 11, 15, 3, 6, 8};

               arity = 2;
               if (     (livemask & 0xC6C6) == 0) map_ptr = map1;
               else if ((livemask & 0xE2E2) == 0) map_ptr = map2;
               else if ((livemask & 0x8D8D) == 0) map_ptr = map3;
               else if ((livemask & 0xA9A9) == 0) map_ptr = map4;
               else if ((livemask & 0x9A9A) == 0) map_ptr = map5;
               else if ((livemask & 0xD8D8) == 0) map_ptr = map6;
               else if ((livemask & 0x2E2E) == 0) map_ptr = map7;
               else if ((livemask & 0x6C6C) == 0) map_ptr = map8;
               else goto lose;
            }
            break;
         case s4x6:
            arity = 2;
            {
               // Maps for 4x6 Z's.
               static const veryshort map46a[9] = {11, 1, 10, 18, 22, 6, 23, 13, -1};
               static const veryshort map46b[9] = {11, 19, 16, 18, 4, 6, 23, 7, -1};
               static const veryshort map46c[9] = {0, 10, 19, 11, 7, 23, 12, 22, -1};
               static const veryshort map46d[9] = {18, 10, 19, 17, 7, 5, 6, 22, -1};
               static const veryshort map46e[9] = {18, 19, 15, 16, 3, 4, 6, 7, -1};
               static const veryshort map46f[9] = {19, 20, 16, 17, 4, 5, 7, 8, -1};
               static const veryshort map46g[9] = {1, 2, 10, 11, 22, 23, 13, 14, -1};
               static const veryshort map46h[9] = {0, 1, 9, 10, 21, 22, 12, 13, -1};

               if (     (livemask & 0xBDCBDC) == 0) map_ptr = map46a;
               else if ((livemask & 0xF4EF4E) == 0) map_ptr = map46b;
               else if ((livemask & 0x7EC7EC) == 0) map_ptr = map46c;
               else if ((livemask & 0xF8DF8D) == 0) map_ptr = map46d;
               else if ((livemask & 0xE4FE4F) == 0) map_ptr = map46e;
               else if ((livemask & 0xF27F27) == 0) map_ptr = map46f;
               else if ((livemask & 0x9FC9FC) == 0) map_ptr = map46g;
               else if ((livemask & 0x3F93F9) == 0) map_ptr = map46h;
               else goto lose;
            }
            break;
         case s3x4:
            arity = 2;  // Might have just said "Z", which would set arity
                        // to 1.  We can nevertheless find the two Z's.
            {
               // Maps for 3x4 Z's.
               static const veryshort mapb[16] =
               {-1, -1, -1, -1, -1, -1, -1, -1, 10, 2, 5, 9, 11, 3, 4, 8};
               static const veryshort mapc[16] =
               {-1, -1, -1, -1, -1, -1, -1, -1, 0, 5, 7, 10, 1, 4, 6, 11};

               if (     (livemask & 05050) == 0) {
                  misc2_zflag = CMD_MISC2__IN_Z_CCW;
                  goto do_real_z_stuff;
               }
               else if ((livemask & 02424) == 0) {
                  misc2_zflag = CMD_MISC2__IN_Z_CW;
                  goto do_real_z_stuff;
               }
               else if ((livemask & 06060) == 0) map_ptr = mapb;
               else if ((livemask & 01414) == 0) map_ptr = mapc;
               else goto lose;
            }
            break;
         case s2x6:
            arity = 2;  // Might have just said "Z", which would set arity
                        // to 1.  We can nevertheless find the two Z's.
            {
               // Maps for 2x6 Z's.
               static const veryshort mape[16] =
               {0, 1, 9, 10, 3, 4, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};
               static const veryshort mapf[16] =
               {1, 2, 10, 11, 4, 5, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1};

               if (     (livemask & 01111) == 0) map_ptr = mape;
               else if ((livemask & 04444) == 0) map_ptr = mapf;
               else goto lose;
            }
            break;
         case s3x6:
            arity = 2;
            {
               // Maps for 3x6 Z's.
               static const veryshort mapg[16] =
               {0, 16, 13, 15, 4, 6, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1};
               static const veryshort maph[16] =
               {15, 1, 16, 14, 7, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1};
               static const veryshort mapi[16] =
               {16,17, 13, 14, 4, 5, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1};
               static const veryshort mapj[16] =
               {0, 1, 17, 16, 8, 7, 9, 10, -1, -1, -1, -1, -1, -1, -1, -1};

               if (     livemask == 0426426) map_ptr = mapg;
               else if (livemask == 0216216) map_ptr = maph;
               else if (livemask == 0033033) map_ptr = mapi;
               else if (livemask == 0603603) map_ptr = mapj;
               else goto lose;
            }
            break;
         case s2x3:
            if (arity != 1) fail("Use the 'Z' concept here.");
            {
               // Maps for single 2x3 Z.
               static const veryshort mape1[8] = {0, 1, 3, 4, -1, -1, -1, -1};
               static const veryshort mapf1[8] = {1, 2, 4, 5, -1, -1, -1, -1};

               if (     (livemask & 011) == 0) map_ptr = mape1;
               else if ((livemask & 044) == 0) map_ptr = mapf1;
               else goto lose;
            }
            break;
         default:
            fail("Must have 3x4, 2x6, 3x6, 2x3, 4x4, 4x6, or split 1/4 tags for this concept.");
         }
      }
      break;
   case 6:
      // The concept is Z diamond(s)
      arity = this_concept->value.arg4;

      switch (ss->kind) {
         /*
      case s4x4:
         {
            // Maps for 4x4 Z's.
            static const veryshort map1[16] =
            {12, 15, 11, 10, 3, 2, 4, 7, 12, 3, 7, 10, 15, 2, 4, 11};
            static const veryshort map2[16] =
            {12, 13, 3, 15, 11, 7, 4, 5, 12, 13, 7, 11, 15, 3, 4, 5};
            static const veryshort map3[16] =
            {14, 1, 2, 3, 10, 11, 6, 9, 11, 1, 2, 6, 10, 14, 3, 9};
            static const veryshort map4[16] =
            {13, 14, 1, 3, 9, 11, 5, 6, 13, 14, 11, 9, 3, 1, 5, 6};
            static const veryshort map5[16] =
            {10, 13, 15, 9, 7, 1, 2, 5, 10, 7, 5, 9, 13, 1, 2, 15};
            static const veryshort map6[16] =
            {13, 14, 15, 10, 7, 2, 5, 6, 13, 14, 2, 7, 10, 15, 5, 6};
            static const veryshort map7[16] =
            {3, 0, 1, 7, 9, 15, 11, 8, 15, 0, 1, 11, 9, 3, 7, 8};
            static const veryshort map8[16] =
            {14, 0, 3, 15, 11, 7, 6, 8, 14, 0, 7, 11, 15, 3, 6, 8};

            arity = 2;
            if (     (livemask & 0xC6C6) == 0) map_ptr = map1;
            else if ((livemask & 0xE2E2) == 0) map_ptr = map2;
            else if ((livemask & 0x8D8D) == 0) map_ptr = map3;
            else if ((livemask & 0xA9A9) == 0) map_ptr = map4;
            else if ((livemask & 0x9A9A) == 0) map_ptr = map5;
            else if ((livemask & 0xD8D8) == 0) map_ptr = map6;
            else if ((livemask & 0x2E2E) == 0) map_ptr = map7;
            else if ((livemask & 0x6C6C) == 0) map_ptr = map8;
            else goto lose;
         }
         break;
      case s4x6:
         arity = 2;
         {
            // Maps for 4x6 Z's.
            static const veryshort map46a[9] = {11, 1, 10, 18, 22, 6, 23, 13, -1};
            static const veryshort map46b[9] = {11, 19, 16, 18, 4, 6, 23, 7, -1};
            static const veryshort map46c[9] = {0, 10, 19, 11, 7, 23, 12, 22, -1};
            static const veryshort map46d[9] = {18, 10, 19, 17, 7, 5, 6, 22, -1};
            static const veryshort map46e[9] = {18, 19, 15, 16, 3, 4, 6, 7, -1};
            static const veryshort map46f[9] = {19, 20, 16, 17, 4, 5, 7, 8, -1};
            static const veryshort map46g[9] = {1, 2, 10, 11, 22, 23, 13, 14, -1};
            static const veryshort map46h[9] = {0, 1, 9, 10, 21, 22, 12, 13, -1};

            if (     (livemask & 0xBDCBDC) == 0) map_ptr = map46a;
            else if ((livemask & 0xF4EF4E) == 0) map_ptr = map46b;
            else if ((livemask & 0x7EC7EC) == 0) map_ptr = map46c;
            else if ((livemask & 0xF8DF8D) == 0) map_ptr = map46d;
            else if ((livemask & 0xE4FE4F) == 0) map_ptr = map46e;
            else if ((livemask & 0xF27F27) == 0) map_ptr = map46f;
            else if ((livemask & 0x9FC9FC) == 0) map_ptr = map46g;
            else if ((livemask & 0x3F93F9) == 0) map_ptr = map46h;
            else goto lose;
         }
         break;
         */
      case s3x4:
         {
            static const veryshort mapzda[9] = {1, 3, 4, 5, 7, 9, 10, 11, -1};
            static const veryshort mapzdb[9] = {0, 2, 4, 5, 6, 8, 10, 11, -1};
            inner_kind = s_qtag;
            arity = 1;  // Whatever the user called it, we are going to a single setup.
            if (     (livemask & 05050) == 0) map_ptr = mapzda;
            else if ((livemask & 02424) == 0) map_ptr = mapzdb;
            else goto lose;
         }
         break;
         /*
      case s2x6:
         arity = 2;  // Might have just said "Z", which would set arity
                     // to 1.  We can nevertheless find the two Z's.
         {
            // Maps for 2x6 Z's.
            static const veryshort mape[16] =
            {0, 1, 9, 10, 3, 4, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};
            static const veryshort mapf[16] =
            {1, 2, 10, 11, 4, 5, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1};

            if (     (livemask & 01111) == 0) map_ptr = mape;
            else if ((livemask & 04444) == 0) map_ptr = mapf;
            else goto lose;
         }
         break;
      case s3x6:
         arity = 2;
         {
            // Maps for 3x6 Z's.
            static const veryshort mapg[16] =
            {0, 16, 13, 15, 4, 6, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1};
            static const veryshort maph[16] =
            {15, 1, 16, 14, 7, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1};
            static const veryshort mapi[16] =
            {16,17, 13, 14, 4, 5, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1};
            static const veryshort mapj[16] =
            {0, 1, 17, 16, 8, 7, 9, 10, -1, -1, -1, -1, -1, -1, -1, -1};

            if (     livemask == 0426426) map_ptr = mapg;
            else if (livemask == 0216216) map_ptr = maph;
            else if (livemask == 0033033) map_ptr = mapi;
            else if (livemask == 0603603) map_ptr = mapj;
            else goto lose;
         }
         break;
         */
      case s2x3:
         if (arity != 1) fail("Use the 'Z diamond' concept here.");
         {
            // Maps for single 2x3 Z.
            static const veryshort mape1[8] = {0, 1, 3, 4};
            static const veryshort mapf1[8] = {5, 1, 2, 4};

            inner_kind = sdmd;
            if (     (livemask & 011) == 0) map_ptr = mape1;
            else if ((livemask & 044) == 0) map_ptr = mapf1;
            else goto lose;
         }
         break;
      default:
         fail("Must have 3x4, 2x6, 3x6, 2x3, 4x4, 4x6, or split 1/4 tags for this concept.");
      }

      break;
   case 1:
      /* The concept is some variety of jay */

      if (ss->kind != s_qtag) fail("Must have quarter-tag setup for this concept.");

      if (table_offset == 0 ||
          (livemask == 0xFF && (directions & 0xF0F0) == 0xA000)) {
         uint32 arg4 = this_concept->value.arg4 ^ directions;

         if (     ((arg4 ^ 0x0802) & 0x0F0F) == 0) map_ptr = mapj1;
         else if (((arg4 ^ 0x0208) & 0x0F0F) == 0) map_ptr = mapj2;
         else if (((arg4 ^ 0x0A00) & 0x0F0F) == 0) map_ptr = mapk1;
         else if (((arg4 ^ 0x000A) & 0x0F0F) == 0) map_ptr = mapk2;
         else goto lose;
      }
      else if (livemask == 0xFF && (directions & 0xF0F0) == 0x00A0) {
         if (     ((directions ^ 0x0802) & 0x0F0F) == 0) map_ptr = mapj3;
         else if (((directions ^ 0x0208) & 0x0F0F) == 0) map_ptr = mapj4;
         else if (((directions ^ 0x0A00) & 0x0F0F) == 0) map_ptr = mapk3;
         else if (((directions ^ 0x000A) & 0x0F0F) == 0) map_ptr = mapk4;
         else goto lose;
      }
      else goto lose;
      break;
   case 2:
      switch (ss->kind) {     // The concept is twin parallelograms.
      case s3x4:
         {
            static const veryshort map_p1[16] =
            {2, 3, 11, 10, 5, 4, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1};
            static const veryshort map_p2[16] =
            {0, 1, 4, 5, 10, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};

            if (     (livemask & 06060) == 0) map_ptr = map_p1;
            else if ((livemask & 01414) == 0) map_ptr = map_p2;
            else goto lose;
         }
         break;
      default:
         fail("Must have 3x4 setup for this concept.");
      }
      break;
   case 3:
      switch (ss->kind) {   /* The concept is interlocked boxes or interlocked parallelograms */
      case s3x4:
         {
            static const veryshort map_b1[16] =
            {1, 3, 4, 11, 10, 5, 7, 9, 1, 3, 5, 10, 11, 4, 7, 9};
            static const veryshort map_b2[16] =
            {0, 2, 5, 10, 11, 4, 6, 8, 0, 2, 4, 11, 10, 5, 6, 8};

            if (     (livemask & 05050) == 0) map_ptr = map_b1;
            else if ((livemask & 02424) == 0) map_ptr = map_b2;
            else goto lose;
         }
         break;
      default:
         fail("Must have 3x4 setup for this concept.");
      }
      break;
   case 4:
      /* The concept is facing (or back-to-back, or front-to-back) Pgram */

      if (ss->kind != s_qtag) fail("Must have quarter-line setup for this concept.");

      if (table_offset == 0 ||
          (livemask == 0xFF && (directions & 0xF0F0) == 0xA000)) {
         if (     ((directions ^ 0x0A00) & 0x0F0F) == 0) map_ptr = mapk1;
         else if (((directions ^ 0x000A) & 0x0F0F) == 0) map_ptr = mapk2;
         else goto lose;
      }
      else if (livemask == 0xFF && (directions & 0xF0F0) == 0x00A0) {
         if (     ((directions ^ 0x0A00) & 0x0F0F) == 0) map_ptr = mapk3;
         else if (((directions ^ 0x000A) & 0x0F0F) == 0) map_ptr = mapk4;
         else goto lose;
      }
      else goto lose;
      break;
   case 5:
      if (ss->kind != s4x4) fail("Must have 4x4 matrix for this concept.");

      {
         setup rotss = *ss;
         veryshort the_map[16];
         
         int rows = 1;
         int columns = 1;
         
         map_ptr = the_map;
         rotss.rotation++;
         canonicalize_rotation(&rotss);
      
         // Search for the live people, in rows first.

         if (!full_search(4, 4, TRUE, 2, the_map, list_foo_in, list_4x4_out, ss))
            rows = 0;

         // Now search in columns, and store the result starting at 8.

         if (!full_search(4, 4, TRUE, 2, the_map, list_bar_in, list_4x4_out, &rotss))
            columns = 0;

         // At this point, exactly one of "rows" and "columns" should be equal to 1.

         if (rows+columns != 1)
            fail("Can't identify distorted blocks.");

         if (columns) {
            *ss = rotss;
            map_ptr = &the_map[8];
         }
         
         for (i=0 ; i<2 ; i++) {
            gather(&inputs[i], ss, &map_ptr[i*4], 3, 011);
            inputs[i].kind = s2x2;
            inputs[i].rotation = 0;
            inputs[i].cmd = ss->cmd;
            inputs[i].cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
            update_id_bits(&inputs[i]);
            move(&inputs[i], FALSE, &results[i]);
            if (results[i].kind != s2x2 || (results[i].rotation & 1))
               fail("Can only do non-shape-changing calls in Z or distorted setups.");
            scatter(result, &results[i], &map_ptr[i*4], 3, 033);
         }

         result->kind = s4x4;
         result->rotation = results[0].rotation;
         result->result_flags = results[0].result_flags & ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
         reinstate_rotation(ss, result);
         
         if (columns) {
            result->rotation--;
            canonicalize_rotation(result);
         }

         return;
      }
   }

   map_ptr += table_offset;

   if (map_ptr[0] < 0) goto lose;

   for (i=0 ; i<arity ; i++) {
      inputs[i] = *ss;
      gather(&inputs[i], ss, map_ptr, setup_attrs[inner_kind].setup_limits, 0);
      inputs[i].rotation = 0;
      inputs[i].kind = inner_kind;
      inputs[i].cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
      inputs[i].cmd.cmd_assume.assumption = cr_none;
      update_id_bits(&inputs[i]);
      move(&inputs[i], FALSE, &results[i]);

      if (results[i].kind != inner_kind || results[i].rotation != 0) {
         if (results[i].kind != s1x4 ||
             results[i].rotation != 1 ||
             (misc_indicator != 1 && misc_indicator != 4))
            fail("Can't do shape-changer with this concept.");
         scatter(result, &results[i], &map_ptr[24], 3, 0);
      }
      else
         scatter(result, &results[i], map_ptr, setup_attrs[inner_kind].setup_limits, 0);

      map_ptr += setup_attrs[inner_kind].setup_limits+1;
   }

   result->kind = ss->kind;
   result->rotation = 0;

   if (misc_indicator == 1 || misc_indicator == 4) {
      if (results[0].kind == s1x4 && results[1].kind == s1x4) {
         result->kind = s_rigger;
         result->rotation = 1;
         if (misc_indicator == 1) warn(warn__hokey_jay_shapechanger);
      }
      else if (results[0].kind != s2x2 || results[1].kind != s2x2)
         fail("Can't do this shape-changer with this concept.");  /* Yow!  They're different! */
   }

   result->result_flags = get_multiple_parallel_resultflags(results, arity);
   reinstate_rotation(ss, result);
   return;

 lose:
   fail("Can't find the indicated formation.");

 do_real_z_stuff:

   ss->cmd.cmd_misc2_flags |= misc2_zflag;
   if (table_offset != 0) goto lose;
   new_divided_setup_move(ss, MAPCODE(s2x3,arity,MPKIND__LILZCOM,1), phantest_ok, TRUE, result);
}


extern void distorted_move(
   setup *ss,
   parse_block *parseptr,
   disttest_kind disttest,
   setup *result) THROW_DECL
{

/*
   Args from the concept are as follows:
   disttest (usually just the arg1 field from the concept,
            but may get fudged for random bigblock/stagger) =
      disttest_offset - user claims this is offset line(s)/column(s)
      disttest_z      - user claims this is Z lines/columns
      disttest_any    - user claims this is distorted lines/columns
   linesp & 7 =
      0 - user claims this is some kind of columns
      1 - user claims this is some kind of lines
      3 - user claims this is waves
   linesp & 8 != 0:
      this is a single tidal (grand) setup
   linesp & 16 != 0:
      this is distorted 1/4 tags or diamonds or whatever
   linesp & 32 != 0:
      this is C/L/W of 3 (not distorted)
   linesp & 64 != 0:
      this is staggered C/L/W of 3
   linesp & 128 != 0:
      this is offset split phantom boxes
   linesp & 256 != 0:
      this is offset 1/4 tag

   Global_tbonetest has the OR of all the people, or all the standard people if
      this is "standard", so it is what we look at to interpret the
      lines vs. columns nature of the concept.
*/


   veryshort the_map[8];
   const parse_block *next_parseptr;
   uint64 junk_concepts;
   int rot, rotz;
   setup_kind k;
   setup a1;
   setup res1;
   mpkind mk, mkbox;

   const map_thing *map_ptr;
   uint32 mapcode = ~0UL;
   int rotate_back = 0;
   uint32 livemask = global_livemask;
   uint32 linesp = parseptr->concept->value.arg2;
   long_boolean zlines = TRUE;

   if (linesp & 8) {
      if (linesp & 1) {
         if (global_tbonetest & 1) fail("There is no tidal line here.");
      }
      else {
         if (global_tbonetest & 010) fail("There is no tidal column here.");
      }

      if (disttest == disttest_offset) {
         /* Offset tidal C/L/W. */
         if (ss->kind == s2x8) {
            if (global_livemask == 0x0F0F) { map_ptr = &map_off1x81; }
            else if (global_livemask == 0xF0F0) { map_ptr = &map_off1x82; }
            else fail("Can't find offset 1x8.");

            goto do_divided_call;
         }
         else
            fail("Must have 2x8 setup for this concept.");
      }
      else {
         /* Distorted tidal C/L/W. */
         if (ss->kind == sbigbone) {
            if (global_livemask == 01717) { map_ptr = &map_dbgbn1; }
            else if (global_livemask == 07474) { map_ptr = &map_dbgbn2; }
            else fail("Can't find distorted 1x8.");

            // We know what we are doing -- shut off the error message.
            disttest = disttest_offset;
            goto do_divided_call;
         }
         else if (ss->kind == s2x8) {
            // Search for the live people.

            (void) full_search(8, 2, FALSE, 1, the_map, list_2x8_in, list_2x8, ss);
            k = s1x8;
            zlines = FALSE;
            rot = 0;
            rotz = 0;
            result->kind = s2x8;      
         }
         else
            fail("Must have 2x8 setup for this concept.");
      }
   }
   else if (linesp & 16) {
      /* The thing to verify, like CMD_MISC__VERIFY_1_4_TAG. */
      ss->cmd.cmd_misc_flags |= parseptr->concept->value.arg3;

      if (ss->kind == s3dmd) {
         if (global_livemask == 06363) { map_ptr = &map_dqtag1; }
         else if (global_livemask == 06666) { map_ptr = &map_dqtag2; }
         else fail("Can't find offset diamonds.");
      }
      else if (ss->kind == s4x4) {
         if (global_livemask == 0x6C6C) { map_ptr = &map_dqtag3; }
         else if (global_livemask == 0xE2E2) { map_ptr = &map_dqtag4; }
         else {
            rotate_back = 1;   // It must be rotated.
            ss->rotation++;
            canonicalize_rotation(ss);

            if (global_livemask == 0xC6C6) { map_ptr = &map_dqtag3; }
            else if (global_livemask == 0x2E2E) { map_ptr = &map_dqtag4; }
            else
               fail("Can't find offset diamonds.");
         }
      }
      else
         fail("Must have triple diamond setup for this concept.");

      goto do_divided_nocheck;
   }
   else if (linesp & 32) {
      // This is C/L/W of 3 (not distorted).

      int goodcount = 0;
      clw3_thing *goodmap = (clw3_thing *) 0;

      for (clw3_thing *gptr=clw3_table ; gptr->mask ; gptr++) {
         if (gptr->k == ss->kind &&
             (global_livemask & gptr->mask) == gptr->test) {
            uint32 tberrtest = global_tbonetest;
            uint32 mytbone = 0;
            for (int k=0 ; k < 3*gptr->map->arity ; k++)
               mytbone |= ss->people[gptr->map->maps[k]].id1;

            if (!((linesp ^ gptr->rot ^ gptr->map->rot) & 1)) mytbone >>= 3;

            if (mytbone & 1) {
               // These 1x3's aren't facing the right way.  But if "standard"
               // was used, and the standard people are facing the right way,
               // accept it.
               if (!((linesp ^ gptr->rot ^ gptr->map->rot) & 1)) tberrtest >>= 3;
               if (tberrtest & 1) continue;    // Too bad.
            }

            goodcount++;
            goodmap = gptr;
         }
      }

      if (goodcount != 1)
         fail((linesp & 1) ?
              "There are no lines of 3 here." :
              "There are no columns of 3 here.");

      rotate_back = goodmap->rot;
      ss->rotation += rotate_back;
      canonicalize_rotation(ss);

      setup ssave = *ss;

      if ((linesp & 7) == 3)
         ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

      divided_setup_move(ss, goodmap->map, phantest_ok, TRUE, result);
      if (result->kind != goodmap->k) fail("Can't figure out result setup.");

      // Now we have to put back the inactives.  Note also that they can't roll.

      const veryshort *inactivemap = goodmap->inactives;

      for (int i=0 ; ; i++) {
         int j = inactivemap[i];
         if (j < 0) break;
         (void) copy_person(result, j, &ssave, j);
         if (result->people[j].id1)
            result->people[j].id1 = (result->people[j].id1 & (~NROLL_MASK)) | ROLL_IS_M;
      }

      goto getoutnosplit;
   }
   else if (linesp & 64) {
      // This is staggered C/L/W of 3.  We allow only a few patterns.
      // There is no general way to scan the columns unambiguously.

      if (ss->kind == s4x4) {
         if (!((linesp ^ global_tbonetest) & 1)) {
            /* What a crock -- this is all backwards. */
            rotate_back = 1;     /* (Well, actually everything else is backwards.) */
            ss->rotation++;
            canonicalize_rotation(ss);
            livemask = ((livemask << 4) & 0xFFFF) | (livemask >> 12);
         }

         // We know what we are doing -- shut off the error message.
         disttest = disttest_offset;

         if (livemask == 0xD0D0) {
            map_ptr = &map_stw3a;
            goto do_divided_call;
         }
         else if (livemask == 0x2929) {
            map_ptr = &map_stw3b;
            goto do_divided_call;
         }
      }

      fail("Can't do this concept in this setup.");
   }
   else if (linesp & 128) {
      // Offset split phantom boxes.
      static const uint32 map_code_table[4] = {0, MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF,0),
                                               MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF,0), 0};
      int i, j;

      do_matrix_expansion(ss, CONCPROP__NEEDK_3X8, FALSE);
      if (ss->kind != s3x8) fail("Can't do this concept in this setup.");

      // Need to recompute this, darn it.
      global_livemask = 0;
      for (i=0, j=1; i<=setup_attrs[ss->kind].setup_limits; i++, j<<=1) {
         if (ss->people[i].id1) global_livemask |= j;
      }

      int key = 0;
      if ((global_livemask & 0x00F00F) == 0) key |= 1;
      if ((global_livemask & 0x0F00F0) == 0) key |= 2;

      mapcode = map_code_table[key];
      if (mapcode == 0) fail("Can't find offset 2x4's.");
      goto do_divided_call;
   }
   else if (linesp & 256) {
      if (ss->kind != spgdmdcw && ss->kind != spgdmdccw) {
         // Try to fudge a 4x4 into the setup we want.
         if (global_tbonetest & 1) {
            rotate_back = 1;
            global_tbonetest >>= 3;
            livemask = ((livemask << 4) & 0xFFFF) | (livemask >> 12);
            ss->rotation++;
            canonicalize_rotation(ss);
         }

         if (ss->kind != s4x4 || (global_tbonetest & 1)) fail("Can't find distorted 1/4 tag.");
         expand_thing *p;
         static expand_thing foo1 =
         {{-1, 2, -1, 3, -1, -1, 5, 4, -1, 6, -1, 7, -1, -1, 1, 0},
          16, s4x4, spgdmdccw, 0, 0UL, 0UL,
          warn__none, warn__none, simple_normalize, 0};
         static expand_thing foo2 =
         {{-1, -1, 2, 1, -1, 4, -1, 3, -1, -1, 6, 5, -1, 0, -1, 7},
          16, s4x4, spgdmdcw, 0, 0UL, 0UL,
          warn__none, warn__none, simple_normalize, 0};

         if (livemask == 0xCACA)
            p = &foo1;
         else if (livemask == 0xACAC)
            p = &foo2;
         else fail("Can't find distorted 1/4 tag.");

         expand_setup(p, ss);
         warn(warn__fudgy_half_offset);

         // This line taken from do_matrix_expansion.  Would like to do it right.
         for (int i=0; i<MAX_PEOPLE; i++)
            ss->people[i].id2 &= (~GLOB_BITS_TO_CLEAR | (ID2_FACEFRONT|ID2_FACEBACK|ID2_HEADLINE|ID2_SIDELINE));
      }

      // Now we have the setup we want.
      // **** We probably ought to put in a check to demand a real 1/4 tag.  Or whatever.

      if (ss->kind == spgdmdcw)
         mapcode = MAPCODE(s_qtag,1,MPKIND__OFFS_R_HALF,1);
      else if (ss->kind == spgdmdccw)
         mapcode = MAPCODE(s_qtag,1,MPKIND__OFFS_L_HALF,1);
      else
         fail("Can't find distorted 1/4 tag.");

      ss->cmd.cmd_misc_flags |= parseptr->concept->value.arg3;
      goto do_divided_nocheck;
   }
   else {
      // Look for singular "offset C/L/W" in a 2x4.

      if (disttest == disttest_offset &&
          ss->kind == s2x4 &&
          parseptr->concept->value.arg4 == 1) {
         if (linesp & 1) {
            if (global_tbonetest & 1) fail("There is no offset line here.");
         }
         else {
            if (global_tbonetest & 010) fail("There is no offset column here.");
         }

         if (livemask == 0x33)
            mapcode = MAPCODE(s1x4,1,MPKIND__OFFS_L_FULL,1);
         else if (livemask == 0xCC)
            mapcode = MAPCODE(s1x4,1,MPKIND__OFFS_R_FULL,1);
         else fail("Can't find offset C/L/W.");

         goto do_divided_call;
      }

      // Otherwise, it had better be plural.
      if (parseptr->concept->value.arg4 != 0)
         fail("Use plural offset C/L/W's.");

      k = s2x4;

      if (ss->kind == s4x4) {
         /* **** This is all sort of a crock.  Here we are picking out the "winning" method
            for doing distorted/offset/Z lines and columns.  Below, we do it by the old way,
            which allows T-bone setups. */
      
         /* If any people are T-boned, we must invoke the other method and hope for the best.
            ***** We will someday do it right. */
            
         if ((global_tbonetest & 011) == 011) {
            if (disttest != disttest_offset)
               fail("Sorry, can't apply this concept when people are T-boned.");
      
            phantom_2x4_move(ss, linesp & 1, phantest_only_one,
                             get_map_from_code(MAPCODE(s2x4,2,MPKIND__OFFS_BOTH_FULL, 1)),
                             result);
            return;
         }
   
         /* Look for butterfly or "O" spots occupied. */

         if (livemask == 0x6666 || livemask == 0x9999) {
            if (!((linesp ^ global_tbonetest) & 1)) {
               rotate_back = 1;
               ss->rotation++;
               canonicalize_rotation(ss);
            }
            mapcode = (livemask & 1) ?
               MAPCODE(s2x4,2,MPKIND__X_SPOTS,1) :
               MAPCODE(s2x4,2,MPKIND__O_SPOTS,1);
            // We know what we are doing -- shut off the error message.
            disttest = disttest_offset;
            goto do_divided_call;
         }
   
         if ((linesp ^ global_tbonetest) & 1) {
            rotate_back = 1;
            ss->rotation++;
            canonicalize_rotation(ss);
            livemask = ((livemask << 4) & 0xFFFF) | (livemask >> 12);
         }
   
         // Check for special case of offset lines/columns, and do it
         // the elegant way (handling shape-changers) if so.

         if (livemask == 0xB4B4) {
            mk = MPKIND__OFFS_L_FULL;
            mkbox = MPKIND__OFFS_L_FULL_SPECIAL;
            goto do_offset_call;
         }
         else if (livemask == 0x4B4B) {
            mk = MPKIND__OFFS_R_FULL;
            mkbox = MPKIND__OFFS_R_FULL_SPECIAL;
            goto do_offset_call;
         }

         // Search for the live people.
         // Must scan sideways for each Y value, looking for exactly 2 people
         // If any of the scans returns false, meaning that the 2 people
         // are not adjacent, set zlines to false.

         zlines = full_search(4, 4, FALSE, 2, the_map, list_3x4_in, list_4x4_out, ss);
         rot = 011;
         rotz = 033;
         result->kind = s4x4;
      }
      else {
         // All the remaining cases make the same test for lines vs. columns.

         if (linesp & 1) {
            if (global_tbonetest & 1) fail("There are no lines of 4 here.");
         }
         else {
            if (global_tbonetest & 010) fail("There are no columns of 4 here.");
         }
         
         rot = 0;
         rotz = 0;
         result->kind = ss->kind;      

         if (ss->kind == s3x4) {
            // Check for special case of offset lines/columns, and do it
            // the elegant way (handling shape-changers) if so.
            if (livemask == 07474) {
               mk = MPKIND__OFFS_L_HALF;
               mkbox = MPKIND__OFFS_L_HALF_SPECIAL;
               goto do_offset_call;
            }
            else if (livemask == 06363) {
               mk = MPKIND__OFFS_R_HALF;
               mkbox = MPKIND__OFFS_R_HALF_SPECIAL;
               goto do_offset_call;
            }

            zlines = full_search(4, 3, FALSE, 2, the_map, list_3x4_in, list_3x4_out, ss);
         }
         else if (ss->kind == s3x8 && livemask == 0x3C03C0) {
            mk = MPKIND__OFFS_L_HALF;
            goto do_offset_call;
         }
         else if (ss->kind == s3x8 && livemask == 0x303303) {
            mk = MPKIND__OFFS_R_HALF;
            goto do_offset_call;
         }
         else if (ss->kind == s2x5) {
            (void) full_search(2, 5, FALSE, 4, the_map, list_2x4_in, list_2x5_out, ss);
            warn(warn_real_people_spots);
         }
         else if (ss->kind == s2x6) {
            (void) full_search(2, 6, FALSE, 4, the_map, list_2x4_in, list_2x6_out, ss);
            warn(warn_real_people_spots);
         }
         else if (ss->kind == s2x7) {
            (void) full_search(2, 7, FALSE, 4, the_map, list_2x4_in, list_2x7_out, ss);
            warn(warn_real_people_spots);
         }
         else if (ss->kind == s2x8) {
            (void) full_search(2, 8, FALSE, 4, the_map, list_2x4_in, list_2x8_out, ss);
            warn(warn_real_people_spots);
         }
         else if (ss->kind == s2x10) {
            (void) full_search(2, 10, FALSE, 4, the_map, list_2x4_in, list_2x10_out, ss);
            warn(warn_real_people_spots);
         }
         else if (ss->kind == s2x12) {
            (void) full_search(2, 12, FALSE, 4, the_map, list_2x4_in, list_2x12_out, ss);
            warn(warn_real_people_spots);
         }
         else
            fail("Can't do this concept in this setup.");
      }
   }

   /* Now see if the concept was correctly named. */

   if (!zlines) {
      switch (disttest) {
         case disttest_z:
            fail("Can't find Z lines/columns, perhaps you mean distorted.");
         case disttest_offset:
            fail("Can't find offset lines/columns, perhaps you mean distorted.");
      }
   }
   /* We used to have this also, which would raise an error if you said "distorted lines"
      when the setup is in fact Z lines.  Unfortunately, "distorted lines" are legal at C3,
      but "Z lines" are not, so the error is not helpful.
   else {
      if (disttest != disttest_z)
         fail("You must specify Z lines/columns when in this setup.");
   }
   */

   gather(&a1, ss, the_map, 7, rot);
   a1.kind = k;
   a1.rotation = 0;
   a1.cmd = ss->cmd;
   a1.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;

   if ((linesp & 7) == 3)
      a1.cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

   update_id_bits(&a1);
   impose_assumption_and_move(&a1, &res1);

   if (res1.kind != k || (res1.rotation & 1)) fail("Can only do non-shape-changing calls in Z or distorted setups.");
   result->rotation = res1.rotation;
   scatter(result, &res1, the_map, 7, rotz);
   result->result_flags = res1.result_flags & ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
   reinstate_rotation(ss, result);
   goto getout;
   

   do_offset_call:

   /* This is known to be a plain offset C/L/W in a 3x4 or 4x4.  See if it
      is followed by "split phantom C/L/W" or "split phantom boxes", in which
      case we do something esoteric. */

   junk_concepts.her8it = 0;
   junk_concepts.final = 0;

   next_parseptr = process_final_concepts(parseptr->next, FALSE, &junk_concepts);

   if (ss->kind == s3x8) {
      if (next_parseptr->concept->kind == concept_do_phantom_boxes &&
          (junk_concepts.her8it | junk_concepts.final) == 0 &&
          next_parseptr->concept->value.arg3 == MPKIND__SPLIT) {
         ss->cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
         ss->cmd.parseptr = next_parseptr->next;
         mapcode = MAPCODE(s2x4,2,mk,0);
         warn(warn__offset_hard_to_see);
      }
      else
         fail("Can't do this concept in this setup.");
   }
   else if (next_parseptr->concept->kind == concept_do_phantom_2x4 &&
       ss->kind == s3x4 &&     /* Only allow 50% offset. */
       (junk_concepts.her8it | junk_concepts.final) == 0 &&
       linesp == (next_parseptr->concept->value.arg2 & 7) &&  /* Demand same "CLW" as original. */
            next_parseptr->concept->value.arg3 == MPKIND__SPLIT) {
      int i;

      ss->cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
      do_matrix_expansion(ss, CONCPROP__NEEDK_4X5, FALSE);
      for (i=0; i<MAX_PEOPLE; i++)
         ss->people[i].id2 &= (~GLOB_BITS_TO_CLEAR | (ID2_FACEFRONT|ID2_FACEBACK|ID2_HEADLINE|ID2_SIDELINE));

      if (ss->kind != s4x5) fail("Must have a 4x5 setup for this concept.");

      ss->cmd.parseptr = next_parseptr->next;
      mapcode = MAPCODE(s2x4,2,mkbox,1);
   }
   else if (next_parseptr->concept->kind == concept_do_phantom_boxes &&
            ss->kind == s3x4 &&     /* Only allow 50% offset. */
            (junk_concepts.her8it | junk_concepts.final) == 0 &&
            next_parseptr->concept->value.arg3 == MPKIND__SPLIT) {
      ss->cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
      do_matrix_expansion(ss, CONCPROP__NEEDK_3X8, FALSE);
      if (ss->kind != s3x8) fail("Must have a 3x4 setup for this concept.");

      ss->cmd.parseptr = next_parseptr->next;
      mapcode = MAPCODE(s2x4,2,mk,0);
   }
   else
      mapcode = MAPCODE(s2x4,1,mk,1);

   do_divided_call:

   if (disttest != disttest_offset)
      fail("You must specify offset lines/columns when in this setup.");

   if ((linesp & 7) == 3)
      ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

   do_divided_nocheck:

   if (mapcode == ~0UL)
      divided_setup_move(ss, map_ptr, phantest_ok, TRUE, result);
   else
      new_divided_setup_move(ss, mapcode, phantest_ok, TRUE, result);

 getoutnosplit:

   /* The split-axis bits are gone.  If someone needs them, we have work to do. */
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;

 getout:

   if (rotate_back) {
      result->rotation--;
      canonicalize_rotation(result);
   }
}


extern void triple_twin_move(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   uint32 tbonetest = global_tbonetest;
   uint32 mapcode;
   phantest_kind phan = (phantest_kind) parseptr->concept->value.arg4;

   // Arg1 = 0/1/3 for C/L/W, usual coding.
   // Arg2 = matrix to expand to.
   // Arg4 = phantest kind.
   // Arg3 =
   // 0 : triple twin C/L/W
   // 1 : triple C/L/W of 6
   // 2 : quadruple C/L/W of 6
   // 3 : [split/interlocked] phantom C/L/W of 6, arg5 has map kind
   // 4 : triple twin C/L/W of 3
   // 5 : triple tidal C/L/W
   // 6 : twin phantom C/L/W of 6
   // 7 : twin phantom tidal C/L/W
   // 8 : 12 matrix [split/interlocked] phantom C/L/W, arg5 has map kind
   // 9 : 12 matrix divided C/L/W
   // 10 : divided C/L/W

   // The setup has not necessarily been expanded to a 4x6.  It has only been
   // expanded to a 4x4.  Why?  Because the stuff in toplevelmove that does
   // the expansion didn't know which way to orient the 4x6.  (It might have
   // depended on "standard" information.)  So why is the expansion supposed
   // to be done there rather than at the point where the concept is executed?
   // It is to prevent monstrosities like "in your split phantom line you
   // have a split phantom column".  Expansion takes place only at the top
   // level.  When the concept "in your split phantom line" is executed,
   // toplevelmove expands the setup to a 4x4, the concept routine splits
   // it into 2x4's, and then the second concept is applied without further
   // expansion.  Since we now have 2x4's and the "you have a split phantom
   // column" concept requires a 4x4, it will raise an error.  If the
   // expansion were done wherever a concept is performed, this monstrosity
   // would by permitted to occur.  So the remaining question is "What
   // safety are we sacrificing (or what monstrosities are we permitting)
   // by doing the expansion here?"  The answer is that, if there were a
   // concept that divided the setup into 4x4's, we could legally do
   // something like "in your split phantom 4x4's you have triple twin
   // columns".  It would expand each 4x4 into a 4x6 and go the call.
   // Horrors.  Fortunately, there are no such concepts.  Of course
   // the really right way to do this is to have a setupflag called
   // NOEXPAND, and do the expansion when the concept is acted upon.
   // Anyway, here goes.

   if ((tbonetest & 011) == 011) fail("Can't do this from T-bone setup.");

   tbonetest ^= parseptr->concept->value.arg1;

   switch (parseptr->concept->value.arg3) {
   case 10:
      if (ss->kind != s2x8) fail("Must have a 2x8 setup for this concept.");
      mapcode = MAPCODE(s2x4,2,MPKIND__SPLIT,0);
      tbonetest ^= 1;
      break;
   case 9:
      if (ss->kind != s2x6) fail("Must have a 2x6 setup for this concept.");
      mapcode = MAPCODE(s2x3,2,MPKIND__SPLIT,0);
      tbonetest ^= 1;
      break;
   case 8:
      if (ss->kind != s3x4) fail("Must have a 3x4 setup for this concept.");
      mapcode = MAPCODE(s2x3,2,parseptr->concept->value.arg5,1);
      break;
   case 7:
      if (ss->kind == s2x8)
         mapcode = MAPCODE(s1x8,2,MPKIND__SPLIT,1);
      else if (ss->kind == s1x16)
         mapcode = MAPCODE(s1x8,2,MPKIND__SPLIT,0);
      else
         fail("Must have a 2x8 or 1x16 setup for this concept.");

      tbonetest ^= 1;
      ss->cmd.cmd_misc_flags |= CMD_MISC__EXPLICIT_MATRIX;
      break;
   case 6:
      if (ss->kind == s2x6)
         mapcode = MAPCODE(s1x6,2,MPKIND__SPLIT,1);
      else if (ss->kind == s1x12)
         mapcode = MAPCODE(s1x6,2,MPKIND__SPLIT,0);
      else
         fail("Must have a 2x6 or 1x12 setup for this concept.");

      tbonetest ^= 1;
      ss->cmd.cmd_misc_flags |= CMD_MISC__EXPLICIT_MATRIX;
      break;
   case 1:
      if (ss->kind != s3x6) fail("Must have a 3x6 setup for this concept.");
      tbonetest ^= 1;
      mapcode = MAPCODE(s1x6,3,MPKIND__SPLIT,1);
      break;
   case 4:
      if (ss->kind != s3x6) fail("Must have a 3x6 setup for this concept.");
      mapcode = MAPCODE(s2x3,3,MPKIND__SPLIT,1);
      break;
   case 5:
      if (ss->kind != s3x8) fail("Must have a 3x8 setup for this concept.");
      tbonetest ^= 1;
      mapcode = MAPCODE(s1x8,3,MPKIND__SPLIT,1);
      break;
   default:
      if (parseptr->concept->value.arg3 != 0) tbonetest ^= 1;

      if (ss->kind == s4x4) {
         expand_setup(((tbonetest & 1) ? &exp_4x4_4x6_stuff_b : &exp_4x4_4x6_stuff_a), ss);
         tbonetest = 0;
      }

      if (ss->kind != s4x6) fail("Must have a 4x6 setup for this concept.");

      switch (parseptr->concept->value.arg3) {
      case 0:
         mapcode = MAPCODE(s2x4,3,MPKIND__SPLIT,1);
         break;
      case 2:
         mapcode = MAPCODE(s1x6,4,MPKIND__SPLIT,1);
         break;
      case 3:
         mapcode = MAPCODE(s2x6,2,(mpkind)parseptr->concept->value.arg5,1);
         break;
      }

      if ((ss->cmd.cmd_misc2_flags & CMD_MISC2__MYSTIFY_SPLIT) &&
          parseptr->concept->value.arg3 != 0)
         fail("Mystic not allowed with this concept.");
      break;
   }

   if (tbonetest & 1) {
      if (parseptr->concept->value.arg1 == 0) fail("Can't find the required columns.");
      else fail("Can't find the required lines.");
   }

   if (parseptr->concept->value.arg3 != 8 && parseptr->concept->value.arg1 == 3)
      ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

   new_divided_setup_move(ss, mapcode, phan, TRUE, result);
}



extern void do_concept_rigger(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   /* First 8 are for rigger; second 8 are for 1/4-tag, next 16 are for crosswave. */
   /* A huge coincidence is at work here -- the first two parts of the maps are the same. */
   static const veryshort map1[38] = {
         0, 1, 3, 2, 4, 5, 7, 6, 0, 1, 3, 2, 4, 5, 7, 6,
         13, 15, 1, 3, 5, 7, 9, 11, 14, 0, 2, 4, 6, 8, 10, 12,
         4, 5, 0, 1, 2, 3};        // For 2x3.
   static const veryshort map2[38] = {
         2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1,
         0, 2, 4, 6, 8, 10, 12, 14, 1, 7, 5, 11, 9, 15, 13, 3,
         5, 0, 1, 2, 3, 4};        // For 2x3.

   int rstuff, indicator, base;
   setup a1;
   setup res1;
   const veryshort *map_ptr;
   setup_kind startkind;
   int rot = 0;

   rstuff = parseptr->concept->value.arg1;

   /* rstuff =
      outrigger   : 0
      leftrigger  : 1
      inrigger    : 2
      rightrigger : 3
      backrigger  : 16
      frontrigger : 18
      Note that outrigger and frontrigger are nearly the same, as are inrigger and backrigger.
      They differ only in allowable starting setup.  So, after checking the setup, we look
      only at the low 2 bits. */

   if (ss->kind == s_rigger) {
      if (rstuff >= 16) fail("This variety of 'rigger' not permitted in this setup.");

      if (!(ss->people[2].id1 & BIT_PERSON) ||
            ((ss->people[2].id1 ^ ss->people[6].id1) & d_mask) != 2)
         fail("'Rigger' people are not facing consistently!");

      indicator = (ss->people[6].id1 ^ rstuff) & 3;
      base = 0;
      startkind = s2x4;
   }
   else if (ss->kind == s_qtag) {
      if (rstuff < 16 && !(rstuff & 1)) fail("This variety of 'rigger' not permitted in this setup.");

      if (!(ss->people[0].id1 & BIT_PERSON) ||
            ((ss->people[0].id1 ^ ss->people[1].id1) & d_mask) != 0 ||
            ((ss->people[4].id1 ^ ss->people[5].id1) & d_mask) != 0 ||
            ((ss->people[0].id1 ^ ss->people[5].id1) & d_mask) != 2)
         fail("'Rigger' people are not facing consistently!");

      indicator = (ss->people[0].id1 ^ rstuff ^ 3) & 3;
      base = 8;
      startkind = s2x4;
   }
   else if (ss->kind == s_crosswave) {
      if (rstuff >= 16) fail("This variety of 'rigger' not permitted in this setup.");

      if (!(ss->people[4].id1 & BIT_PERSON) ||
            ((ss->people[4].id1 ^ ss->people[0].id1) & d_mask) != 2)
         fail("'Rigger' people are not facing consistently!");

      indicator = (ss->people[0].id1 ^ rstuff) & 3;
      base = 16;
      startkind = s_c1phan;
   }
   else if (ss->kind == s_short6) {
      if (rstuff >= 16) fail("This variety of 'rigger' not permitted in this setup.");

      if (!(ss->people[1].id1 & BIT_PERSON) ||
            ((ss->people[1].id1 ^ ss->people[4].id1) & d_mask) != 2)
         fail("'Rigger' people are not facing consistently!");

      indicator = ((ss->people[4].id1 + 1) ^ rstuff) & 3;
      base = 32;
      rot = 1;
      startkind = s2x3;
   }
   else
      fail("Can't do this concept in this setup.");

   if (indicator & 1)
      fail("'Rigger' direction is inappropriate.");

   map_ptr = indicator ? map1 : map2;

   clear_people(&a1);
   scatter(&a1, ss, &map_ptr[base], setup_attrs[ss->kind].setup_limits, rot*033);
   a1.kind = startkind;
   a1.rotation = 0;
   a1.cmd = ss->cmd;
   a1.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
   a1.cmd.cmd_assume.assumption = cr_none;
   move(&a1, FALSE, &res1);

   if ((res1.rotation) & 1) base ^= 8;    /* Won't happen in C1 phantom. */

   if (startkind == s_c1phan) {
      int i;
      uint32 evens = 0;
      uint32 odds = 0;

      for (i=0; i<16; i+=2) {
         evens |= res1.people[i].id1;
         odds |= res1.people[i+1].id1;
      }

      if (res1.kind == s_c1phan) {
         if (indicator) {
            if (evens) {
               if (odds) fail("Can't do this.");
               base = 24;
               result->kind = s2x4;
            }
            else
               result->kind = s_crosswave;
         }
         else {
            if (odds) {
               if (evens) fail("Can't do this.");
               base = 24;
               result->kind = s2x4;
            }
            else
               result->kind = s_crosswave;
         }
      }
      else {
         base ^= 16;
         if (res1.kind != s2x4) fail("Can't do this.");
         result->kind = base ? s_qtag : s_rigger;
      }
   }
   else if (startkind == s2x3) {
      if (res1.kind != s2x3 || ((res1.rotation) & 1)) fail("Can't do this.");
      result->kind = s_short6;
   }
   else {
      if (res1.kind != s2x4) fail("Can't do this.");
      result->kind = base ? s_qtag : s_rigger;
   }

   gather(result, &res1, &map_ptr[base], setup_attrs[res1.kind].setup_limits, rot*011);
   result->rotation = res1.rotation;
   result->result_flags = res1.result_flags;
   reinstate_rotation(ss, result);
}


typedef struct {
   int indicator;
   setup_kind orig_kind;
   setup_kind partial_kind;  /* what setup they are virtually in */
   uint32 forbidden_people;
   int rot;                  /* whether to rotate partial setup CW */
   int uncommon[12];
   int common0[12];
   uint32 dir0[12];
   int common1[12];
   uint32 dir1[12];
} common_spot_map;

common_spot_map cmaps[] = {

   /* Common point galaxy */

   {1, s_rigger, s_galaxy, 0, 0,
         {      -1,       0,      -1,       1,      -1,       4,      -1,       5},
         {       6,      -1,      -1,      -1,       2,      -1,      -1,      -1},
         { d_north,       0,       0,       0, d_south,       0,       0,       0},
         {       7,      -1,      -1,      -1,       3,      -1,      -1,      -1},
         { d_south,       0,       0,       0, d_north,       0,       0,       0}},

   /* Common point diamonds */
   /* We currently have no defense against unchecked spots being occupied! */

   {4, sbigdmd, s_qtag, 0, 1,
         {      -1,      -1,       8,       9,      -1,      -1,       2,       3},
         {       5,      -1,      -1,      -1,      11,      -1,      -1,      -1},
         { d_south,       0,       0,       0, d_north,       0,       0,       0},
         {       4,      -1,      -1,      -1,      10,      -1,      -1,      -1},
         { d_north,       0,       0,       0, d_south,       0,       0,       0}},
   {4, sbigdmd, s_qtag, 0, 1,
         {      -1,      -1,       8,       9,      -1,      -1,       2,       3},
         {      -1,       6,      -1,      -1,      -1,       0,      -1,      -1},
         {       0, d_south,       0,       0,       0, d_north,       0,       0},
         {      -1,       7,      -1,      -1,      -1,       1,      -1,      -1},
         {       0, d_north,       0,       0,       0, d_south,       0,       0}},


   // Common spot point-to-point diamonds.
   {0x400, sbigptpd, s_ptpd, 0, 0,
         {       2,      -1,       3,      -1,       8,      -1,       9,      -1},
         {      -1,       5,      -1,      -1,      -1,      11,      -1,      -1},
         {       0,  d_east,       0,       0,       0,  d_west,       0,       0},
         {      -1,       4,      -1,      -1,      -1,      10,      -1,      -1},
         {       0,  d_west,       0,       0,       0,  d_east,       0,       0}},
   {0x400, sbigptpd, s_ptpd, 0, 0,
         {       2,      -1,       3,      -1,       8,      -1,       9,      -1},
         {      -1,      -1,      -1,       1,      -1,      -1,      -1,       7},
         {       0,       0,       0,  d_east,       0,       0,       0,  d_west},
         {      -1,      -1,      -1,       0,      -1,      -1,      -1,       6},
         {       0,       0,       0,  d_west,       0,       0,       0,  d_east}},

   /* Common point hourglass */

   {0x80, sbighrgl, s_hrglass, 0, 1,
         {      -1,      -1,       8,       3,      -1,      -1,       2,       9},
         {       5,      -1,      -1,      -1,      11,      -1,      -1,      -1},
         { d_south,       0,       0,       0, d_north,       0,       0,       0},
         {       4,      -1,      -1,      -1,      10,      -1,      -1,      -1},
         { d_north,       0,       0,       0, d_south,       0,       0,       0}},
   {0x80, sbighrgl, s_hrglass, 0, 1,
         {      -1,      -1,       8,       3,      -1,      -1,       2,       9},
         {      -1,       6,      -1,      -1,      -1,       0,      -1,      -1},
         {       0, d_south,       0,       0,       0, d_north,       0,       0},
         {      -1,       7,      -1,      -1,      -1,       1,      -1,      -1},
         {       0, d_north,       0,       0,       0, d_south,       0,       0}},

   /* Common spot lines from a parallelogram -- the centers are themselves and the wings become ends. */
   /* We currently have no defense against unchecked spots being occupied! */

   {0x10, s2x6, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,      -1,      -1,       5,       6,      -1,      -1,      11},
         { d_north,       0,       0, d_south, d_south,       0,       0, d_north},
         {       1,      -1,      -1,       4,       7,      -1,      -1,      10},
         { d_south,       0,       0, d_north, d_north,       0,       0, d_south}},
   {0x10, s2x6, s2x4, 0, 0,
         {      -1,       2,       3,      -1,      -1,       8,       9,      -1},
         {      -1,      -1,      -1,       5,      -1,      -1,      -1,      11},
         {       0,       0,       0, d_south,       0,       0,       0, d_north},
         {      -1,      -1,      -1,       4,      -1,      -1,      -1,      10},
         {       0,       0,       0, d_north,       0,       0,       0, d_south}},
   {0x10, s2x6, s2x4, 0, 0,
         {      -1,       2,       3,      -1,      -1,       8,       9,      -1},
         {       0,      -1,      -1,      -1,       6,      -1,      -1,      -1},
         { d_north,       0,       0,       0, d_south,       0,       0,       0},
         {       1,      -1,      -1,      -1,       7,      -1,      -1,      -1},
         { d_south,       0,       0,       0, d_north,       0,       0,       0}},

   /* Common spot lines from waves -- they become 2-faced lines; everyone is a center */

   {0x20, s2x4, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,       0,       3,      -1,      -1,       4,       7,      -1},
         {       0, d_north, d_south,       0,       0, d_south, d_north,       0},
         {      -1,       1,       2,      -1,      -1,       5,       6,      -1},
         {       0, d_south, d_north,       0,       0, d_north, d_south,       0}},

   /* Common spot lines from a 2x8        Occupied as     ^V^V....
                                                          ....^V^V   (or other way)
      they become 2-faced lines */

   {8, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,       2,      -1,      -1,       8,      10,      -1,      -1},
         { d_north, d_north,       0,       0, d_south, d_south,       0,       0},
         {       1,       3,      -1,      -1,       9,      11,      -1,      -1},
         { d_south, d_south,       0,       0, d_north, d_north,       0,       0}},
   {8, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,      -1,       4,       6,      -1,      -1,      12,      14},
         {       0,       0, d_north, d_north,       0,       0, d_south, d_south},
         {      -1,      -1,       5,       7,      -1,      -1,      13,      15},
         {       0,       0, d_south, d_south,       0,       0, d_north, d_north}},

   /* Common spot lines from a 2x8        Occupied as     ^V..^V..
                                                          ..^V..^V   (or other way)
      they become 2-faced lines */

   {8, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,      -1,       5,      -1,       8,      -1,      13,      -1},
         { d_north,       0, d_south,       0, d_south,       0, d_north,       0},
         {       1,      -1,       4,      -1,       9,      -1,      12,      -1},
         { d_south,       0, d_north,       0, d_north,       0, d_south,       0}},

   {8, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,       2,      -1,       7,      -1,      10,      -1,      15},
         {       0, d_north,       0, d_south,       0, d_south,       0, d_north},
         {      -1,       3,      -1,       6,      -1,      11,      -1,      14},
         {       0, d_south,       0, d_north,       0, d_north,       0, d_south}},

   /* Common spot lines from a 2x8        Occupied as     ^V^V....
                                                          ....^V^V   (or other way)
      they become waves */

   {0x40, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,       3,      -1,      -1,       8,      11,      -1,      -1},
         { d_north, d_south,       0,       0, d_south, d_north,       0,       0},
         {       1,       2,      -1,      -1,       9,      10,      -1,      -1},
         { d_south, d_north,       0,       0, d_north, d_south,       0,       0}},
   {0x40, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,      -1,       4,       7,      -1,      -1,      12,      15},
         {       0,       0, d_north, d_south,       0,       0, d_south, d_north},
         {      -1,      -1,       5,       6,      -1,      -1,      13,      14},
         {       0,       0, d_south, d_north,       0,       0, d_north, d_south}},

   /* Common spot lines from a 2x8        Occupied as     ^V..^V..
                                                          ..^V..^V   (or other way)
      they become waves */

   {0x40, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,      -1,       4,      -1,       8,      -1,      12,      -1},
         { d_north,       0, d_north,       0, d_south,       0, d_south,       0},
         {       1,      -1,       5,      -1,       9,      -1,      13,      -1},
         { d_south,       0, d_south,       0, d_north,       0, d_north,       0}},
   {0x40, s2x8, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,       2,      -1,       6,      -1,      10,      -1,      14},
         {       0, d_north,       0, d_north,       0, d_south,       0, d_south},
         {      -1,       3,      -1,       7,      -1,      11,      -1,      15},
         {       0, d_south,       0, d_south,       0, d_north,       0, d_north}},

   /* Common spot columns, facing E-W */

   /* Clumps */
   {2, s4x4, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      12,      13,      -1,      -1,       4,       5,      -1,      -1},
         {  d_east,  d_east,       0,       0,  d_west,  d_west,       0,       0},
         {      10,      15,      -1,      -1,       2,       7,      -1,      -1},
         {  d_west,  d_west,       0,       0,  d_east,  d_east,       0,       0}},
   {2, s4x4, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,      -1,      14,       0,      -1,      -1,       6,       8},
         {       0,       0,  d_east,  d_east,       0,       0,  d_west,  d_west},
         {      -1,      -1,       3,       1,      -1,      -1,      11,       9},
         {       0,       0,  d_west,  d_west,       0,       0,  d_east,  d_east}},
   /* Stairsteps */
   {2, s4x4, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      12,      -1,      14,      -1,       4,      -1,       6,      -1},
         {  d_east,       0,  d_east,       0,  d_west,       0,  d_west,       0},
         {      10,      -1,       3,      -1,       2,      -1,      11,      -1},
         {  d_west,       0,  d_west,       0,  d_east,       0,  d_east,       0}},
   {2, s4x4, s2x4, 0, 0,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,      13,      -1,       0,      -1,       5,      -1,       8},
         {       0,  d_east,       0,  d_east,       0,  d_west,       0,  d_west},
         {      -1,      15,      -1,       1,      -1,       7,      -1,       9},
         {       0,  d_west,       0,  d_west,       0,  d_east,       0,  d_east}},
   /* Stairsteps in middle, ends are normal */
   {2, s4x4, s2x4, 0x5151, 0,
         {      10,      -1,       3,       1,       2,      -1,      11,       9},
         {      -1,      13,      -1,      -1,      -1,       5,      -1,      -1},
         {       0,  d_east,       0,       0,       0,  d_west,       0,       0},
         {      -1,      15,      -1,      -1,      -1,       7,      -1,      -1},
         {       0,  d_west,       0,       0,       0,  d_east,       0,       0}},
   {2, s4x4, s2x4, 0x3131, 0,
         {      10,      15,      -1,       1,       2,       7,      -1,       9},
         {      -1,      -1,      14,      -1,      -1,      -1,       6,      -1},
         {       0,       0,  d_east,       0,       0,       0,  d_west,       0},
         {      -1,      -1,       3,      -1,      -1,      -1,      11,      -1},
         {       0,       0,  d_west,       0,       0,       0,  d_east,       0}},
   /* 'Z' columns, centers are normal */
   {2, s4x4, s2x4, 0, 0,
         {      -1,      15,       3,      -1,      -1,       7,      11,      -1},
         {      12,      -1,      -1,      -1,       4,      -1,      -1,      -1},
         {  d_east,       0,       0,       0,  d_west,       0,       0,       0},
         {      10,      -1,      -1,      -1,       2,      -1,      -1,      -1},
         {  d_west,       0,       0,       0,  d_east,       0,       0,       0}},
   {2, s4x4, s2x4, 0, 0,
         {      -1,      15,       3,      -1,      -1,       7,      11,      -1},
         {      -1,      -1,      -1,       0,      -1,      -1,      -1,       8},
         {       0,       0,       0,  d_east,       0,       0,       0,  d_west},
         {      -1,      -1,      -1,       1,      -1,      -1,      -1,       9},
         {       0,       0,       0,  d_west,       0,       0,       0,  d_east}},

   /* Common spot columns, facing N-S */

   /* Clumps */
   {2, s4x4, s2x4, 0, 1,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,       1,      -1,      -1,       8,       9,      -1,      -1},
         { d_south, d_south,       0,       0, d_north, d_north,       0,       0},
         {      14,       3,      -1,      -1,       6,      11,      -1,      -1},
         { d_north, d_north,       0,       0, d_south, d_south,       0,       0}},
   {2, s4x4, s2x4, 0, 1,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,      -1,       2,       4,      -1,      -1,      10,      12},
         {       0,       0, d_south, d_south,       0,       0, d_north, d_north},
         {      -1,      -1,       7,       5,      -1,      -1,      15,      13},
         {       0,       0, d_north, d_north,       0,       0, d_south, d_south}},
   /* Stairsteps */
   {2, s4x4, s2x4, 0, 1,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {       0,      -1,       2,      -1,       8,      -1,      10,      -1},
         { d_south,       0, d_south,       0, d_north,       0, d_north,       0},
         {      14,      -1,       7,      -1,       6,      -1,      15,      -1},
         { d_north,       0, d_north,       0, d_south,       0, d_south,       0}},
   {2, s4x4, s2x4, 0, 1,
         {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
         {      -1,       1,      -1,       4,      -1,       9,      -1,      12},
         {       0, d_south,       0, d_south,       0, d_north,       0, d_north},
         {      -1,       3,      -1,       5,      -1,      11,      -1,      13},
         {       0, d_north,       0, d_north,       0, d_south,       0, d_south}},
   /* Stairsteps in middle, ends are normal */
   {2, s4x4, s2x4, 0x1515, 1,
         {      14,      -1,       7,       5,       6,      -1,      15,      13},
         {      -1,       1,      -1,      -1,      -1,       9,      -1,      -1},
         {       0, d_south,       0,       0,       0, d_north,       0,       0},
         {      -1,       3,      -1,      -1,      -1,      11,      -1,      -1},
         {       0, d_north,       0,       0,       0, d_south,       0,       0}},
   {2, s4x4, s2x4, 0x1313, 1,
         {      14,       3,      -1,       5,       6,      11,      -1,      13},
         {      -1,      -1,       2,      -1,      -1,      -1,      10,      -1},
         {       0,       0, d_south,       0,       0,       0, d_north,       0},
         {      -1,      -1,       7,      -1,      -1,      -1,      15,      -1},
         {       0,       0, d_north,       0,       0,       0, d_south,       0}},
   /* 'Z' columns, centers are normal */
   {2, s4x4, s2x4, 0, 1,
    {      -1,       3,       7,      -1,      -1,      11,      15,      -1},
    {       0,      -1,      -1,      -1,       8,      -1,      -1,      -1},
    { d_south,       0,       0,       0, d_north,       0,       0,       0},
    {      14,      -1,      -1,      -1,       6,      -1,      -1,      -1},
    { d_north,       0,       0,       0, d_south,       0,       0,       0}},
   {2, s4x4, s2x4, 0, 1,
    {      -1,       3,       7,      -1,      -1,      11,      15,      -1},
    {      -1,      -1,      -1,       4,      -1,      -1,      -1,      12},
    {       0,       0,       0, d_south,       0,       0,       0, d_north},
    {      -1,      -1,      -1,       5,      -1,      -1,      -1,      13},
    {       0,       0,       0, d_north,       0,       0,       0, d_south}},

   /* Common spot columns out of waves, just centers of virtual columns will be occupied.  */
   {2, s2x4, s2x4, 0, 1,
    {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
    {      -1,       3,       4,      -1,      -1,       7,       0,      -1},
    {       0, d_south, d_south,       0,       0, d_north, d_north,       0},
    {      -1,       2,       5,      -1,      -1,       6,       1,      -1},
    {       0, d_north, d_north,       0,       0, d_south, d_south,       0}},

   /* Common spot 1/4 tags from a tidal wave (just center line) */

   {0x200, s1x8, s_qtag, 0, 0,
    {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
    {      -1,      -1,       4,       6,      -1,      -1,       0,       2},
    {       0,       0, d_south, d_north,       0,       0, d_north, d_south},
    {      -1,      -1,       5,       7,      -1,      -1,       1,       3},
    {       0,       0, d_north, d_south,       0,       0, d_south, d_north}},

   /* Common spot 1/4 lines from a tidal wave (just center line) */

   {0x100, s1x8, s_qtag, 0, 0,
    {      -1,      -1,      -1,      -1,      -1,      -1,      -1,      -1},
    {      -1,      -1,       4,       7,      -1,      -1,       0,       3},
    {       0,       0, d_south, d_south,       0,       0, d_north, d_north},
    {      -1,      -1,       5,       6,      -1,      -1,       1,       2},
    {       0,       0, d_north, d_north,       0,       0, d_south, d_south}},

   {0, nothing, nothing, 0, 0, {0}, {0}, {0}, {0}, {0}},
};



extern void common_spot_move(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   int rstuff, i, k, r;
   uint32 livemask, jbit;
   long_boolean uncommon = FALSE;
   setup a0, a1;
   setup the_results[2];
   long_boolean not_rh;
   common_spot_map *map_ptr;
   warning_info saved_warnings = history[history_ptr+1].warnings;

   rstuff = parseptr->concept->value.arg1;
   /* rstuff =
      common point galaxy from rigger          : 0x1
      common spot columns (from 4x4)           : 0x2
      common point diamonds                    : 0x4
      common point hourglass                   : 0x80
      common end lines/waves (from 2x6)        : 0x10
      common center lines/waves (from 2x4)     : 0x20
      common spot 2-faced lines (from 2x8)     : 0x8
      common spot lines                        : 0x78
      common spot waves                        : 0x70
      common spot 1/4 lines                    : 0x100
      common spot 1/4 tags                     : 0x200
      common spot point-to-point diamonds      : 0x400 */

   if (ss->kind == s_c1phan) {
      do_matrix_expansion(ss, CONCPROP__NEEDK_4X4, FALSE);
      // Shut off any "check a 4x4 matrix" warning that this raised.
      history[history_ptr+1].warnings.clearbit(warn__check_4x4_start);
      // Unless, of course, we already had that warning.
      history[history_ptr+1].warnings.setmultiple(saved_warnings);
   }

   for (i=0, jbit=1, livemask = 0; i<=setup_attrs[ss->kind].setup_limits; i++, jbit<<=1) {
      if (ss->people[i].id1) livemask |= jbit;
   }

   for (map_ptr = cmaps ; map_ptr->orig_kind != nothing ; map_ptr++) {
      if (  ss->kind != map_ptr->orig_kind ||
            !(rstuff & map_ptr->indicator) ||
            (livemask & map_ptr->forbidden_people)) goto not_this_map;

      not_rh = FALSE;

      /* See if this map works with right hands. */

      for (i=0; i<=setup_attrs[map_ptr->partial_kind].setup_limits; i++) {
         int t = map_ptr->common0[i];
         int u = map_ptr->common1[i];

         if (t >= 0) {
            if ((ss->people[t].id1 & d_mask) != map_ptr->dir0[i]) goto not_this_rh_map;
         }

         if (u >= 0) {
            if ((ss->people[u].id1 & d_mask) != map_ptr->dir1[i]) goto not_this_rh_map;
         }

         continue;

         not_this_rh_map: ;

         not_rh = TRUE;

         if (t >= 0) {
            if ((ss->people[t].id1 & d_mask) != (map_ptr->dir0[i] ^ 2)) goto not_this_map;
         }

         if (u >= 0) {
            if ((ss->people[u].id1 & d_mask) != (map_ptr->dir1[i] ^ 2)) goto not_this_map;
         }
      }

      goto found;

      not_this_map: ;
   }

   fail("Not in legal setup for common-spot call.");

   found:

   if (not_rh) warn(warn__tasteless_com_spot);

   a0 = *ss;
   a1 = *ss;

   clear_people(&a0);
   clear_people(&a1);

   a0.kind = map_ptr->partial_kind;
   a1.kind = map_ptr->partial_kind;
   a0.rotation = map_ptr->rot;
   a1.rotation = map_ptr->rot;

   r = 011*((-map_ptr->rot) & 3);

   for (i=0; i<=setup_attrs[map_ptr->partial_kind].setup_limits; i++) {
      int t = map_ptr->uncommon[i];
      if (t >= 0) {
         uncommon = TRUE;
         /* The common folks go into each setup! */
         (void) copy_rot(&a0, i, ss, t, r);
         (void) copy_rot(&a1, i, ss, t, r);
      }
      t = map_ptr->common0[i];
      if (t >= 0) (void) copy_rot(&a0, i, ss, t, r);
      t = map_ptr->common1[i];
      if (t >= 0) (void) copy_rot(&a1, i, ss, t, r);
   }

   a0.cmd.cmd_misc_flags |= parseptr->concept->value.arg2;
   a1.cmd.cmd_misc_flags |= parseptr->concept->value.arg2;

   update_id_bits(&a0);
   impose_assumption_and_move(&a0, &the_results[0]);
   update_id_bits(&a1);
   impose_assumption_and_move(&a1, &the_results[1]);

   if (uncommon) {
      if (the_results[0].kind == s_qtag && the_results[1].kind == s2x3 &&
          the_results[0].rotation != the_results[1].rotation)
         expand_setup(&exp_2x3_qtg_stuff, &the_results[1]);
      else if (the_results[1].kind == s_qtag && the_results[0].kind == s2x3 &&
               the_results[0].rotation != the_results[1].rotation)
         expand_setup(&exp_2x3_qtg_stuff, &the_results[0]);
      else if (the_results[0].kind == s2x4 && the_results[1].kind == s4x4)
         do_matrix_expansion(&the_results[0], CONCPROP__NEEDK_4X4, FALSE);
      else if (the_results[1].kind == s2x4 && the_results[0].kind == s4x4)
         do_matrix_expansion(&the_results[1], CONCPROP__NEEDK_4X4, FALSE);

      if (the_results[0].kind != the_results[1].kind ||
          the_results[0].rotation != the_results[1].rotation)
         fail("This common spot call is very problematical.");

      // Remove the uncommon people from the common results, while checking that
      // they wound up in the same position in all 3 results.

      for (i=0; i<=setup_attrs[map_ptr->partial_kind].setup_limits; i++) {
         int t = map_ptr->uncommon[i];
         if (t >= 0 && ss->people[t].id1) {

            /* These bits are not a property just of the person and his position
               in the formation -- they depend on other people's facing direction. */
#define ID2_BITS_NOT_INTRINSIC (ID2_FACING | ID2_NOTFACING)

            for (k=0; k<=setup_attrs[the_results[0].kind].setup_limits; k++) {
               if (the_results[0].people[k].id1 &&
                   ((the_results[0].people[k].id1 ^ ss->people[t].id1) & PID_MASK) == 0) {
                  if (((the_results[0].people[k].id1 ^ the_results[1].people[k].id1) |
                       ((the_results[0].people[k].id2 ^ the_results[1].people[k].id2) &
                        !ID2_BITS_NOT_INTRINSIC)))
                     fail("People moved inconsistently during common-spot call.");

                  clear_person(&the_results[0], k);
                  goto did_it;
               }
            }
            fail("Lost someone during common-spot call.");
         did_it: ;
         }
      }

      the_results[1].result_flags = get_multiple_parallel_resultflags(the_results, 2);
      merge_setups(&the_results[0], merge_c1_phantom, &the_results[1]);
      reinstate_rotation(ss, &the_results[1]);
      *result = the_results[1];
   }
   else {
      /* There were no "uncommon" people.  We simply have two setups that worked
         independently.  They do not have to be similar.  Just merge them. */

      the_results[0].result_flags = get_multiple_parallel_resultflags(the_results, 2);
      merge_setups(&the_results[1], merge_c1_phantom, &the_results[0]);
      reinstate_rotation(ss, &the_results[0]);
      *result = the_results[0];
   }

   // Turn off any "do your part" warnings that arose during execution
   // of the subject call.  The dancers already know.
   history[history_ptr+1].warnings.clearbit(warn__do_your_part);
   // Restore any warnings from before.
   history[history_ptr+1].warnings.setmultiple(saved_warnings);
}



static void do_glorious_triangles(
   setup *ss,
   const tgl_map **map_ptr_table,
   int indicator,
   setup *result) THROW_DECL
{
   int i, r, startingrot;
   uint32 rotstate, pointclip;
   setup a1, a2;
   setup idle;
   setup res[2];
   const veryshort *mapnums;
   const tgl_map *map_ptr = map_ptr_table[(indicator >> 6) & 3];

   if (ss->kind == s_c1phan || ss->kind == sdeepbigqtg) {
      mapnums = map_ptr->mapcp1;
      startingrot = 2;
   }
   else if (ss->kind == sbigdmd || ss->kind == s_rigger) {
      mapnums = map_ptr->mapbd1;
      startingrot = 1;
   }
   else {   /* s_qtag */
      mapnums = map_ptr->mapqt1;
      startingrot = 0;
   }

   r = ((-startingrot) & 3) * 011;
   gather(&a1, ss, mapnums, 2, r);
   gather(&a2, ss, &mapnums[3], 2, r^022);

   /* Save the two people who don't move. */
   (void) copy_person(&idle, 0, ss, mapnums[6]);
   (void) copy_person(&idle, 1, ss, mapnums[7]);

   a1.cmd = ss->cmd;
   a2.cmd = ss->cmd;
   a1.kind = s_trngl;
   a2.kind = s_trngl;
   a1.rotation = 0;
   a2.rotation = 2;
   a1.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
   a2.cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
   move(&a1, FALSE, &res[0]);
   move(&a2, FALSE, &res[1]);

   if (fix_n_results(2, nothing, res, rotstate, pointclip)) {
      result->kind = nothing;
      result->result_flags = 0;
      return;
   }

   /* The low digit of rotstate (should have just low 2 bits on) tells
      what possible rotation states could exist with all setups in
      same parity.  The next digit tells what possible states  of z0
      exist with alternating parity.  The highest bit deals with triangles
      rotating by 90 degrees from one setup to the next, or something
      like that. */

   if (!(rotstate & 0xF03)) fail("Sorry, can't do this orientation changer.");

   result->result_flags = get_multiple_parallel_resultflags(res, 2);
   res[1].rotation += 2;
   canonicalize_rotation(&res[1]);

   /* Check for non-shape-or-orientation-changing result. */

   if (res[0].kind == s_trngl && res[0].rotation == 0) {
      result->kind = ss->kind;
      result->rotation = 0;
      /* Restore the two people who don't move. */
      (void) copy_person(result, mapnums[6], &idle, 0);   
      (void) copy_person(result, mapnums[7], &idle, 1);
      r = startingrot * 011;
      scatter(result, &res[0], mapnums, 2, r);
      scatter(result, &res[1], &mapnums[3], 2, r^022);
      return;
   }

   res[0].rotation += startingrot;
   res[1].rotation += startingrot;
   canonicalize_rotation(&res[0]);
   canonicalize_rotation(&res[1]);

   result->rotation = res[0].rotation;

   r = ((-res[0].rotation) & 3) * 011;

   if (res[0].rotation & 2)
      result->kind = map_ptr->other->kind;
   else
      result->kind = map_ptr->kind;

   if (res[0].kind == s_trngl) {

      if (ss->kind == sdeepbigqtg)
         fail("Sorry, can't do this.");

      /* We know that res[0].rotation != startingrot */
      if (startingrot == 1) fail("Sorry, can't do this.");

      if (res[0].rotation == 0) {
         if (result->kind == nothing) goto noshapechange;
         result->kind = s_qtag;
         /* Restore the two people who don't move. */
         (void) copy_person(result, map_ptr->mapqt1[6], &idle, 0);
         (void) copy_person(result, map_ptr->mapqt1[7], &idle, 1);
         scatter(result, &res[0], map_ptr->mapqt1, 2, 0);
         scatter(result, &res[1], &map_ptr->mapqt1[3], 2, 022);
      }
      else if (res[0].rotation == 2) {
         if (map_ptr->nointlkshapechange)
            goto noshapechange;

         result->kind = s_c1phan;
         /* Restore the two people who don't move. */
         (void) copy_rot(result, map_ptr->mapcp1[7], &idle, 0, r);
         (void) copy_rot(result, map_ptr->mapcp1[6], &idle, 1, r);
         scatter(result, &res[0], &map_ptr->mapcp1[3], 2, 0);
         scatter(result, &res[1], map_ptr->mapcp1, 2, 022);
      }
      else {
         if (result->kind == nothing || map_ptr->nointlkshapechange)
            goto noshapechange;

         map_ptr = map_ptr->other;

         if (result->kind == s_c1phan) {
            /* Restore the two people who don't move. */
            (void) copy_rot(result, map_ptr->mapcp1[6], &idle, 0, r);
            (void) copy_rot(result, map_ptr->mapcp1[7], &idle, 1, r);
   
            /* Copy the triangles. */
            scatter(result, &res[1], map_ptr->mapcp1, 2, 022);
            scatter(result, &res[0], &map_ptr->mapcp1[3], 2, 0);
         }
         else {
            /* Restore the two people who don't move. */
            (void) copy_rot(result, map_ptr->mapqt1[7], &idle, 0, r);
            (void) copy_rot(result, map_ptr->mapqt1[6], &idle, 1, r);

            /* Copy the triangles. */
            scatter(result, &res[0], map_ptr->mapqt1, 2, 0);
            scatter(result, &res[1], &map_ptr->mapqt1[3], 2, 022);
         }
      }
   }
   else if (res[0].kind == s1x3) {

      if (result->kind == nothing || map_ptr->nointlkshapechange)
         goto noshapechange;

      if (res[0].rotation == 0) {

         if (ss->kind == sdeepbigqtg)
            fail("Sorry, can't do this.");

         if (startingrot == 1) {
            mapnums = map_ptr->map261;
            result->kind = s2x6;
         }
         else {
            mapnums = map_ptr->map241;
            result->kind = map_ptr->kind1x3;
         }

         (void) copy_person(result, mapnums[6], &idle, 0);
         (void) copy_person(result, mapnums[7], &idle, 1);
         scatter(result, &res[0], mapnums, 2, 0);
         scatter(result, &res[1], &mapnums[3], 2, 022);
      }
      else {
         if (startingrot == 1)
            fail("Can't do this shape-changer.");

         map_ptr = map_ptr->other;

         result->kind = map_ptr->kind1x3;

         if (map_ptr->switchtgls) {    /* What a crock! */
            (void) copy_rot(result, map_ptr->map241[6], &idle, 0, r);
            (void) copy_rot(result, map_ptr->map241[7], &idle, 1, r);

            for (i=0; i<3; i++) {
               (void) copy_person(result, map_ptr->map241[i+3], &res[0], 2-i);
               (void) copy_rot(result, map_ptr->map241[i], &res[1], 2-i, 022);
            }
         }
         else {
            (void) copy_rot(result, map_ptr->map241[7], &idle, 0, r);
            (void) copy_rot(result, map_ptr->map241[6], &idle, 1, r);
            scatter(result, &res[0], map_ptr->map241, 2, 0);
            scatter(result, &res[1], &map_ptr->map241[3], 2, 022);
         }
      }
   }
   else
      fail("Improper result from triangle call.");

   if (result->kind == s2x4)
       warn(warn__check_2x4);
   else if (result->kind == s2x6)
       warn(warn__check_pgram);
   else if (result->kind == s2x8)
       warn(warn__full_pgram);
   else if (res[0].rotation != startingrot) {
      if (result->kind == s_c1phan)
          warn(warn__check_c1_phan);
      else if (result->kind == s_qtag)
          warn(warn__check_dmd_qtag);
   }

   return;

 noshapechange:
   fail("Can't do shape-changer in interlocked or magic triangles.");
}




// This procedure does wave-base, tandem-base, and so-and-so-base.
static void wv_tand_base_move(
   setup *s,
   int indicator,
   setup *result) THROW_DECL
{
   uint32 tbonetest;
   int t;
   calldef_schema schema;
   const tgl_map **map_ptr_table;

   switch (s->kind) {
   case s_bone:
   case s_rigger:
      tbonetest = s->people[0].id1 | s->people[1].id1 | s->people[4].id1 | s->people[5].id1;

      if ((indicator & 076) != 6 || (tbonetest & 011) == 011 || !((indicator ^ tbonetest) & 1))
         goto losing;

      if (s->kind == s_bone) {
         if (indicator & 0100) fail("Can't do this concept in this setup.");
         concentric_move(s, (setup_command *) 0, &s->cmd, schema_concentric_2_6,
                         0, 0, TRUE, ~0UL, result);
         return;
      }
      else {
         // We now know that the desired triangles are the "inside" ones.

         if (indicator & 0100) {
            do_glorious_triangles(s, rgtglmap1, indicator, result);
            reinstate_rotation(s, result);
            return;
         }
         else
            schema = schema_concentric_6_2;
      }

      break;
   case s_galaxy:
      if ((indicator & 076) != 6)   // Only "tandem-base" and "wave-base" are allowed here.
         goto losing;

      tbonetest = s->people[1].id1 | s->people[3].id1 | s->people[5].id1 | s->people[7].id1;

      if ((tbonetest & 011) == 011)
         goto losing;
      else if ((indicator ^ tbonetest) & 1)
         schema = (indicator & 0100) ? schema_intlk_lateral_6 : schema_lateral_6;
      else
         schema = (indicator & 0100) ? schema_intlk_vertical_6 : schema_vertical_6;
   
      // For galaxies, the schema is now in terms of the absolute orientation.
      // We know that the original setup rotation was canonicalized.
      break;
   case s_hrglass:
      if ((indicator & 076) != 6)   // Only "tandem-base" and "wave-base" are allowed here.
         goto losing;

      tbonetest = s->people[0].id1 | s->people[1].id1 | s->people[4].id1 | s->people[5].id1;

      if ((tbonetest & 011) == 011 || ((indicator ^ tbonetest) & 1))
         goto losing;
   
      schema = (indicator & 0100) ? schema_intlk_vertical_6 : schema_vertical_6;
      break;
   case s_c1phan:
      if ((indicator & 077) == 20) {
         t = 0;
         if (global_selectmask == (global_livemask & 0x5A5A))
            t = 1;
         else if (global_selectmask != (global_livemask & 0xA5A5))
            goto losing;
      }
      else {
         t = indicator & 1;
         if ((global_tbonetest & 010) == 0) t ^= 1;
         else if ((global_tbonetest & 1) != 0)
            goto losing;
      }

      // Now t is 0 to select the triangles whose bases are horizontally
      // aligned, and 1 for the vertically aligned bases.

      s->rotation += t;   // Just flip the setup around and recanonicalize.
      canonicalize_rotation(s);

      if ((global_livemask & 0xAAAA) == 0)
         map_ptr_table = c1tglmap1;
      else if ((global_livemask & 0x5555) == 0)
         map_ptr_table = c1tglmap2;
      else
         goto losing;

      do_glorious_triangles(s, map_ptr_table, indicator, result);
      result->rotation -= t;   // Flip the setup back.
      reinstate_rotation(s, result);
      return;
   case sdeepbigqtg:
      if ((indicator & 077) == 20) {
         if (global_selectmask != (global_livemask & 0xF0F0))
            goto losing;
      }
      else {
         if (global_tbonetest & ((indicator & 1) ? 010 : 1))
            goto losing;
      }

      if ((global_livemask & 0x3A3A) == 0)
         map_ptr_table = dbqtglmap1;
      else if ((global_livemask & 0xC5C5) == 0)
         map_ptr_table = dbqtglmap2;
      else
         goto losing;

      do_glorious_triangles(s, map_ptr_table, indicator, result);
      reinstate_rotation(s, result);
      return;
   default:
      fail("Can't do this concept in this setup.");
   }

   concentric_move(s, &s->cmd, (setup_command *) 0, schema, 0, 0, TRUE, ~0UL, result);
   return;

 losing:
   fail("Can't find the indicated triangles.");
}


extern void triangle_move(
   setup *ss,
   parse_block *parseptr,
   setup *result) THROW_DECL
{
   uint32 tbonetest;
   calldef_schema schema;
   int indicator = parseptr->concept->value.arg1;
   int indicator_base;

/* indicator = 0 - tall 6
               1 - short 6
               2 - inside
               3 - outside
               4 - out point
               5 - in point
               6 - wave-base
               7 - tandem-base
               20 - <anyone>-base
   Add 100 octal if interlocked triangles.
   Add 200 octal if magic triangles. */

   indicator_base = indicator & 077;

   if (indicator_base >= 2 && indicator_base <= 21 &&
       (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_INTLK))) {
      indicator |= 0100;     /* Interlocked triangles. */
      ss->cmd.cmd_final_flags.her8it &= ~INHERITFLAG_INTLK;
   }

   if (indicator_base >= 4 && indicator_base <= 5 &&
       (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_MAGIC))) {
      indicator |= 0200;     /* Magic triangles. */
      ss->cmd.cmd_final_flags.her8it &= ~INHERITFLAG_MAGIC;
   }

   /* Now demand that no flags remain. */
   if (ss->cmd.cmd_final_flags.her8it | ss->cmd.cmd_final_flags.final)
      fail("Illegal modifier for this concept.");

   // Same level (C2) for interlocked or magic.  OK?
   if ((indicator & 0300) && calling_level < intlk_triangle_level) {
      if (allowing_all_concepts)
         warn(warn__bad_concept_level);
      else
         fail("This concept is not allowed at this level.");
   }

   if (indicator <= 1) {
      /* Indicator = 0 for tall 6, 1 for short 6 */

      if (ss->kind == s_galaxy) {
         // We know the setup rotation is canonicalized.
         tbonetest = ss->people[1].id1 | ss->people[3].id1 |
            ss->people[5].id1 | ss->people[7].id1;

         if ((tbonetest & 011) == 011) fail("Can't find tall/short 6.");
         else if ((indicator ^ tbonetest) & 1)
            schema = schema_lateral_6;
         else
            schema = schema_vertical_6;
      }
      else
         fail("Must have galaxy for this concept.");

      /* For galaxies, the schema is now in terms of the absolute orientation. */

      concentric_move(ss, &ss->cmd, (setup_command *) 0, schema, 0, 0, TRUE, ~0UL, result);
   }
   else {
      /* Set this so we can do "peel and trail" without saying "triangle" again. */
      ss->cmd.cmd_misc_flags |= CMD_MISC__SAID_TRIANGLE;

      if (indicator_base >= 6) {
         // Indicator = 6 for wave-base, 7 for tandem-base, 20 for <anyone>-base.
         wv_tand_base_move(ss, indicator, result);
      }
      else if (indicator_base >= 4) {
         /* Indicator = 5 for in point, 4 for out point */

         int t = 0;
   
         if (ss->kind != s_qtag) fail("Must have diamonds.");
   
         if (indicator_base == 5) {
            if ((ss->people[0].id1 & d_mask) == d_east) t |= 1;
            if ((ss->people[4].id1 & d_mask) == d_west) t |= 1;
            if ((ss->people[1].id1 & d_mask) == d_west) t |= 2;
            if ((ss->people[5].id1 & d_mask) == d_east) t |= 2;
         }
         else {
            if ((ss->people[0].id1 & d_mask) == d_west) t |= 1;
            if ((ss->people[4].id1 & d_mask) == d_east) t |= 1;
            if ((ss->people[1].id1 & d_mask) == d_east) t |= 2;
            if ((ss->people[5].id1 & d_mask) == d_west) t |= 2;
         }
   
         const tgl_map **map_ptr_table;

         if (t == 1)
            map_ptr_table = qttglmap1;
         else if (t == 2)
            map_ptr_table = qttglmap2;
         else
            fail("Can't find designated point.");

         do_glorious_triangles(ss, map_ptr_table, indicator, result);
         reinstate_rotation(ss, result);
      }
      else {

         /* Indicator = 2 for inside, 3 for outside */
   
         /* Only a few cases allow interlocked. */

         if (indicator_base == 2 && ss->kind == sbigdmd) {
            const tgl_map **map_ptr_table;

            if (global_livemask == 07474)
               map_ptr_table = bdtglmap1;
            else if (global_livemask == 01717)
               map_ptr_table = bdtglmap2;
            else
               fail("Can't find the indicated triangles.");

            do_glorious_triangles(ss, map_ptr_table, indicator, result);
            reinstate_rotation(ss, result);

            return;
         }
         else if (indicator == 0102 && ss->kind == s_rigger) {
            do_glorious_triangles(ss, rgtglmap1, indicator, result);
            reinstate_rotation(ss, result);

            return;
         }
         else if (indicator & 0100)
            fail("Illegal modifier for this concept.");

         if (indicator_base == 2) {
            switch (ss->kind) {
               case s_hrglass:
                  // This is the schema for picking out the triangles in an hourglass.
                  schema = schema_vertical_6;
                  break;
               case s_rigger:
               case s_ptpd:
                  schema = schema_concentric_6_2;
                  break;
               case s_qtag:
                  schema = schema_concentric_6_2_tgl;
                  break;
               case sbigdmd:
               default:
                  fail("There are no 'inside' triangles.");
            }
   
            concentric_move(ss, &ss->cmd, (setup_command *) 0, schema, 0, 0, TRUE, ~0UL, result);
         }
         else {
            switch (ss->kind) {
            case s_ptpd:
            case s_qtag:
            case s_spindle:
            case s_bone:
            case s_hrglass:
            case s_dhrglass:
            case s_ntrglcw:
            case s_ntrglccw:
            case s_nptrglcw:
            case s_nptrglccw:
               schema = schema_concentric_2_6;
               break;
            default:
               fail("There are no 'outside' triangles.");
            }
   
            concentric_move(ss, (setup_command *) 0, &ss->cmd, schema, 0, 0, TRUE, ~0UL, result);
         }
      }
   }
}


/* This handles only the T-boned case.  Other cases are handled elsewhere. */

extern void phantom_2x4_move(
   setup *ss,
   int lineflag,
   phantest_kind phantest,
   Const map_thing *maps,
   setup *result) THROW_DECL
{
   setup hpeople, vpeople;
   setup the_setups[2];
   int i;
   int vflag, hflag;
   phantest_kind newphantest = phantest;

   warn(warn__tbonephantom);

   vflag = 0;
   hflag = 0;
   hpeople = *ss;
   vpeople = *ss;
   clear_people(&hpeople);
   clear_people(&vpeople);
   
   for (i=0; i<16; i++) {
      if ((ss->people[i].id1 ^ lineflag) & 1)
         hflag |= copy_person(&hpeople, i, ss, i);
      else
         vflag |= copy_person(&vpeople, i, ss, i);
   }

   /* For certain types of phantom tests, we turn off the testing that we pass to
      divided_setup_move.  It could give bogus errors, because it only sees the
      headliners or sideliners at any one time. */
   switch (phantest) {
      case phantest_both:           /* This occurs on "phantom bigblock lines", for example,
                                       and is intended to give error "Don't use phantom concept
                                       if you don't mean it." if everyone is in one variety of
                                       bigblock setup.  But, in this procedure, we could have
                                       headliners entirely in one and sideliners in another, so
                                       that divided_setup_move could be misled. */
         newphantest = phantest_ok;
         break;
      case phantest_first_or_both:  /* This occurs on "phantom lines", for example, and is
                                       intended to give the smae error as above if only the center
                                       phantom lines are occupied.  But the headliners might
                                       occupy just the center phantom lines while the sideliners
                                       make full use of the concept, so, once again, we have to
                                       disable it.  In fact, we do better -- we tell
                                       divided_setup_move to do the right thing if the outer
                                       phantom setup is empty. */
         newphantest = phantest_ctr_phantom_line;
         break;
   }

   /* Do the E-W facing people. */

   if (vflag) {
      vpeople.rotation--;
      canonicalize_rotation(&vpeople);
      divided_setup_move(&vpeople, maps, newphantest, TRUE, &the_setups[1]);
      the_setups[1].rotation++;
      canonicalize_rotation(&the_setups[1]);
   }
   else {
      the_setups[1].kind = nothing;
      the_setups[1].result_flags = 0;
   }

   /* Do the N-S facing people. */

   if (hflag) {
      divided_setup_move(&hpeople, maps, newphantest, TRUE, &the_setups[0]);
   }
   else {
      the_setups[0].kind = nothing;
      the_setups[0].result_flags = 0;
   }

   the_setups[0].result_flags = get_multiple_parallel_resultflags(the_setups, 2);
   merge_setups(&the_setups[1], merge_strict_matrix, &the_setups[0]);
   reinstate_rotation(ss, &the_setups[0]);
   *result = the_setups[0];
}
