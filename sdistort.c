/* -*- mode:C; c-basic-offset:3; indent-tabs-mode:nil; -*- */

/* SD -- square dance caller's helper.

    Copyright (C) 1990-1998  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 32. */

/* This defines the following functions:
   prepare_for_call_in_series
   minimize_splitting_info
   initialize_map_tables
   remove_z_distortion
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


/* Must be a power of 2. */
#define NUM_MAP_HASH_BUCKETS 64

static mapcoder *map_hash_table[NUM_MAP_HASH_BUCKETS];
static map_thing *map_hash_table2[NUM_MAP_HASH_BUCKETS];

extern void initialize_map_tables(void)
{
   map_thing *tab2p;
   int i;

   for (i=0 ; i<NUM_MAP_HASH_BUCKETS ; i++) {
      map_hash_table[i] = (mapcoder *) 0;
      map_hash_table2[i] = (map_thing *) 0;
   }

   for (tab2p = map_init_table2 ; tab2p->outer_kind != nothing ; tab2p++) {
      uint32 hash_num = ((tab2p->code+(tab2p->code>>8) * ((unsigned int) 035761254233))) & (NUM_MAP_HASH_BUCKETS-1);
      tab2p->next = map_hash_table2[hash_num];
      map_hash_table2[hash_num] = tab2p;
   }
}


extern void remove_z_distortion(setup *ss)
{
   setup stemp;
   static Const veryshort fix_cw[]  = {1, 2, 4, 5};
   static Const veryshort fix_ccw[] = {0, 1, 3, 4};

   if (ss->kind != s2x3 ||
       (!(ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_CW) &&
        !(ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_CCW)))
      fail("Internal error: Can't straighten 'Z'.");

   stemp = *ss;
   ss->kind = s2x2;
   gather(ss, &stemp,
          (ss->cmd.cmd_misc2_flags & CMD_MISC2__IN_Z_CW) ? fix_cw : fix_ccw,
          3, 0);

   ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__IN_Z_MASK;
   ss->cmd.cmd_misc2_flags |= CMD_MISC2__DID_Z_COMPRESSION;
   canonicalize_rotation(ss);
   update_id_bits(ss);
}



Private Const map_thing *get_map_from_code(uint32 map_encoding)
{
   int mk = (map_encoding & 0x3F0) >> 4;
   uint32 hash_num;
   mapcoder *p;
   map_thing *q;

   if (mk == MPKIND__SPLIT && !(map_encoding & 8)) {
      int sk = (map_encoding & 0xFFFFFC00) >> 10;
      if (sk <= s2x6)
         return split_lists[sk][(map_encoding & 0xF)-2];
   }

   hash_num = ((map_encoding+(map_encoding>>8) * ((unsigned int) 035761254233))) & (NUM_MAP_HASH_BUCKETS-1);

   for (p=map_hash_table[hash_num] ; p ; p=p->next) {
      if (p->code == map_encoding) return p->the_map;
   }

   for (q=map_hash_table2[hash_num] ; q ; q=q->next) {
      if (q->code == map_encoding) return q;
   }

   return (Const map_thing *) 0;
}


Private void innards(
   setup_command *sscmd,
   Const map_thing *maps,
   long_boolean recompute_id,
   assumption_thing new_assume,
   long_boolean do_second_only,
   setup *x,
   setup *result)
{
   int i, j;
   uint32 main_rotation;
   uint32 rotstate;
   Const map_thing *final_map = (map_thing *) 0;
   Const veryshort *getptr;
   setup z[8];

   uint32 rot = maps->rot;
   int vert = maps->vert;
   int arity = maps->arity;
   int insize = setup_attrs[maps->inner_kind].setup_limits+1;
   long_boolean nonisotropic_1x2 = FALSE;
   long_boolean direct_putback = FALSE;
   long_boolean no_reuse_map = FALSE;
   uint32 mysticflag = sscmd->cmd_misc2_flags;
   mpkind map_kind = maps->map_kind;

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

   if (     arity == 2 &&
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
                     z, &rotstate)) {
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
   else if (!(rotstate & 0xF30) && map_kind == MPKIND__NONISOTROPIC) {
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

   main_rotation = z[0].rotation & 3;
   if (arity == 2 && (z[0].kind == s_trngl || z[0].kind == s_trngl4) && (rot & 0x200))
      main_rotation += 2;

   /* Set the final result_flags to the OR of everything that happened.
      The elongation stuff doesn't matter --- if the result is a 2x2
      being done around the outside, the procedure that called us
      (basic_move) knows what is happening and will fix that bit.
      Also, check that the "did_last_part" bits are the same. */

   result->result_flags = get_multiple_parallel_resultflags(z, arity);

   if (map_kind == MPKIND__LILZCOM) {
      if (result->result_flags & RESULTFLAG__DID_Z_COMPRESSION) {
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
      else {
         /* warn(warn__no_z_action);   This warning is stupid. */
         map_kind = MPKIND__SPLIT;
      }
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

   /* See if we can put things back with the same map we used before. */

   if (z[0].kind == maps->inner_kind && main_rotation == 0 && !direct_putback && !no_reuse_map) {
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
      case MPKIND__DMD_STUFF:
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
               !(z[0].people[3].id1 | z[0].people[4].id1 | z[1].people[0].id1 | z[1].people[1].id1)) {
            final_map = &map_rig_1x6;
            result->rotation = z[0].rotation;
            goto got_map;
         }
         break;
   }

   result->rotation = z[0].rotation;

   if (arity == 2 && z[0].kind == s2x3 && (z[0].rotation & 1) != 0 && rot == 0 && map_kind == MPKIND__REMOVED) {
      final_map = &map_2x3_rmvr;
      result->rotation = z[0].rotation-1;
      goto got_map;
   }

   if (arity == 2 && z[0].kind == s_trngl4 && (z[0].rotation & 1) == 0 && ((z[0].rotation ^ z[1].rotation) & 2)) {
      if ((z[0].rotation + rot) & 2) {
         if (map_kind == MPKIND__OFFS_L_HALF) {
            final_map = &map_lh_zzztgl;
            result->rotation = z[0].rotation-1+(rot&2);
            goto got_map;
         }
         else if (map_kind == MPKIND__OFFS_R_HALF) {
            final_map = get_map_from_code(MAPCODE(s_trngl4,2,MPKIND__OFFS_R_HALF, 1));
            result->rotation = z[0].rotation-1+(rot&2);
            goto got_map;
         }
      }
      else {
         if (map_kind == MPKIND__OFFS_L_HALF) {
            final_map = get_map_from_code(MAPCODE(s_trngl4,2,MPKIND__OFFS_L_HALF, 1));
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

   if (arity == 2 && map_kind == MPKIND__REMOVED && z[0].kind == s_trngl4 && (z[0].rotation & 1) == 1) {
      if (z[0].rotation & 2) {
         goto get_coded_map;
      }
      if (z[1].rotation & 2) {
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

   if ((sscmd->cmd_misc_flags & CMD_MISC__OFFSET_Z) && final_map && (map_kind == MPKIND__OFFS_L_HALF || map_kind == MPKIND__OFFS_R_HALF)) {
      if (final_map->outer_kind == s2x6) warn(warn__check_pgram);
      else final_map = 0;        /* Raise an error. */
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
         if (  z[0].people[0].id1 | z[0].people[4].id1 | z[0].people[5].id1 | z[0].people[6].id1 |
               z[0].people[8].id1 | z[0].people[12].id1 | z[0].people[13].id1 | z[0].people[14].id1 |
               z[1].people[0].id1 | z[1].people[4].id1 | z[1].people[5].id1 | z[1].people[6].id1 |
               z[1].people[8].id1 | z[1].people[12].id1 | z[1].people[13].id1 | z[1].people[14].id1)
            final_map = &map_w4x4_4x4;
         else
            final_map = &map_f2x8_4x4;
      }
      else if (arity == 2 && z[0].kind == s2x8 && z[0].rotation == 1 && map_kind == MPKIND__SPLIT) {
         if (  z[1].people[8].id1 | z[1].people[9].id1 | z[1].people[10].id1 | z[1].people[11].id1 |
               z[1].people[12].id1 | z[1].people[13].id1 | z[1].people[14].id1 | z[1].people[15].id1 |
               z[0].people[0].id1 | z[0].people[1].id1 | z[0].people[2].id1 | z[0].people[3].id1 |
               z[0].people[4].id1 | z[0].people[5].id1 | z[0].people[6].id1 | z[0].people[7].id1)
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
   else if (arity == 2 && z[0].rotation == 2 && z[1].rotation == 0 && (final_map->rot & 0xF) == 0x2)
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

   /* If this is a special map that expects some setup to have been flipped upside-down, do so. */

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

   getptr = final_map->maps;

   for (j=0,rot=final_map->rot ; j<arity ; j++,rot>>=2) {
      int rrr = 011*(rot & 3);

      if (j == 1 && final_map->map_kind == MPKIND__SPEC_TWICEREM)
         insize = setup_attrs[*getptr++].setup_limits+1;

      for (i=0 ; i<insize ; i++)
         install_rot(result, *getptr++, &z[j], i, rrr);
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
   setup *result)
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
   setup *result)
{
   int i, j;
   int vflags[8];
   setup x[8];
   setup_kind kn, kn2;
   int insize;
   int rot;
   int arity;

   assumption_thing t = ss->cmd.cmd_assume;
   Const veryshort *getptr = maps->maps;

   if (!maps) fail("Can't do this concept in this setup.");

   kn = maps->inner_kind;
   insize = setup_attrs[kn].setup_limits+1;
   rot = maps->rot;
   arity = maps->arity;

   for (j=0; j<arity; j++) {
      vflags[j] = 0;

      if (j == 1 && maps->map_kind == MPKIND__SPEC_TWICEREM) {
         kn2 = (setup_kind) *getptr++;
         insize = setup_attrs[kn2].setup_limits+1;
      }

      for (i=0; i<insize; i++) {
         int mm = *getptr++;
         if (mm >= 0) {
            vflags[j] |= ss->people[mm].id1;
            (void) copy_rot(&x[j], i, ss, mm, 011*((-(rot>>(j*2))) & 3));
         }
         else
            clear_person(&x[j], i);
      }
   }

   switch (phancontrol) {
#ifdef NOCANDO
/* So "phantest_impossible" is obsolete and should be replaced with "phantest_ok". */
      case phantest_impossible:
         if (!(vflags[0] && vflags[1]))
            fail("This is impossible in a symmetric setup!!!!");
         break;
#endif
      case phantest_both:
         if (!(vflags[0] && vflags[1]))
            /* Only one of the two setups is occupied. */
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_only_one:
         if (vflags[0] && vflags[1]) fail("Can't find the setup to work in.");
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
         /* Test for "C1" blocks. */
         if (!((vflags[0] | vflags[2]) && (vflags[1] | vflags[3])))
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_not_just_centers:
         if (!(vflags[0] | vflags[2]))
            warn(warn__stupid_phantom_clw);
         break;
      case phantest_2x2_only_two:
         /* Test for NOT "C1" blocks. */
         if ((vflags[0] | vflags[2]) && (vflags[1] | vflags[3])) fail("Not in blocks.");
         break;
   }

   for (j=0; j<arity; j++)
      x[j].kind = (vflags[j]) ? kn : nothing;

   if (maps->map_kind == MPKIND__SPEC_ONCEREM)
      x[1].kind = (setup_kind) maps->maps[insize*2];
   else if (maps->map_kind == MPKIND__SPEC_TWICEREM) {
      x[1].kind = kn2;
      x[2].kind = kn2;
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
            (arity == 2 &&
              (maps->map_kind == MPKIND__OFFS_L_HALF ||
               maps->map_kind == MPKIND__OFFS_R_HALF ||
               maps->map_kind == MPKIND__OFFS_L_FULL ||
               maps->map_kind == MPKIND__OFFS_R_FULL))) {
      if (maps->vert & 1) {
         if ((result->result_flags & RESULTFLAG__SPLIT_AXIS_YMASK) != RESULTFLAG__SPLIT_AXIS_YMASK)
            result->result_flags += RESULTFLAG__SPLIT_AXIS_YBIT;
      }
      else {
         if ((result->result_flags & RESULTFLAG__SPLIT_AXIS_XMASK) != RESULTFLAG__SPLIT_AXIS_XMASK)
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
   uint32 *masks, setup *result)
{
   overlapped_setup_move(
      ss,
      get_map_from_code(map_encoding),
      masks,
      result);
}


extern void overlapped_setup_move(setup *ss, Const map_thing *maps,
   uint32 *masks, setup *result)
{
   int i, j, rot;
   uint32 k;
   setup x[8];
   assumption_thing t = ss->cmd.cmd_assume;

   setup_kind kn = maps->inner_kind;
   int insize = setup_attrs[kn].setup_limits+1;
   int arity = maps->arity;
   Const veryshort *mapbase = maps->maps;

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

static Const veryshort list_10_6_5_4[4] = {8, 6, 5, 4};
static Const veryshort list_11_13_7_2[4] = {9, 11, 7, 2};
static Const veryshort list_12_17_3_1[4] = {10, 15, 3, 1};
static Const veryshort list_14_15_16_0[4] = {12, 13, 14, 0};

static Const veryshort indices_for_2x6_4x6[12]  = {11, 10, 9, 8, 7, 6, 23, 22, 21, 20, 19, 18};



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
   setup *result)
{
   /* This concept is "standard", which means that it can look at global_tbonetest
      and global_livemask, but may not look at anyone's facing direction other
      than through global_tbonetest. */

   int linesp = parseptr->concept->value.arg2 & 1;
   int rot = (global_tbonetest ^ linesp ^ 1) & 1;
   Const map_thing *maps;

   /* If this was phantom columns, we allow stepping to a wave.  This makes it
      possible to do interesting cases of turn and weave, when one column
      is a single 8 chain and another is a single DPT.  But if it was phantom
      lines, we forbid it.  We also always forbid it if it was one of the special
      things like "stairstep lines", which is indicated by the map_kind field being MPKIND__NONE. */

   /* No!  We want to allow [LO] split phantom lines, assume facing lines, relay the shadow. */
#ifdef NOT_ANY_MORE
   if (linesp || parseptr->concept->value.arg3 == MPKIND__NONE)
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;
#endif

   if ((ss->cmd.cmd_misc2_flags & CMD_MISC2__MYSTIFY_SPLIT) &&
       parseptr->concept->value.arg3 != MPKIND__CONCPHAN)
      fail("Mystic not allowed with this concept.");

   if (parseptr->concept->value.arg2 == 3)
      ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

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

         /* Check for special case of "stagger" or "bigblock", without the word "phantom",
            when people are not actually on block spots. */

      if (parseptr->concept->value.arg3 == MPKIND__STAG &&
          parseptr->concept->value.arg1 == phantest_only_one) {
         if (global_livemask != 0x2D2D && global_livemask != 0xD2D2) {
            warn(warn__not_on_block_spots);
            distorted_move(ss, parseptr, disttest_any, result);
            return;
         }
      }

      if ((global_tbonetest & 011) == 011) {
         /* People are T-boned!  This is messy. */
         phantom_2x4_move(
                          ss,
                          linesp,
                          (phantest_kind) parseptr->concept->value.arg1,
                          maps,
                          result);
         return;
      }

      break;
   case s2x6:
      /* Check for special case of split phantom lines/columns in a parallelogram. */

      if (parseptr->concept->value.arg3 == MPKIND__SPLIT) {
         setup stemp;

         if (rot) {
            if (global_tbonetest & 1) fail("There are no split phantom lines here.");
            else                      fail("There are no split phantom columns here.");
         }

         if      (global_livemask == 07474) maps = get_map_from_code(MAPCODE(s2x4,2,MPKIND__OFFS_R_HALF,1));
         else if (global_livemask == 01717) maps = get_map_from_code(MAPCODE(s2x4,2,MPKIND__OFFS_L_HALF,1));
         else fail("Must have a parallelogram for this.");

         warn(warn__split_phan_in_pgram);

         /* Change the setup to a 4x6. */

         stemp = *ss;
         ss->kind = s4x6;
         clear_people(ss);
         scatter(ss, &stemp, indices_for_2x6_4x6, 11, 0);
         break;              /* Note that rot is zero. */
      }
      /* Otherwise fall through to error message!!! */
      /* FALL THROUGH!!! */
   default:
      /* FELL THROUGH!!! */
      fail("Need a 4x4 setup to do this concept.");
   }

   ss->rotation += rot;   /* Just flip the setup around and recanonicalize. */
   canonicalize_rotation(ss);

   divided_setup_move(ss, maps, (phantest_kind) parseptr->concept->value.arg1, TRUE, result);
   result->rotation -= rot;   /* Flip the setup back. */
   /* The split-axis bits are gone.  If someone needs them, we have work to do. */
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}


extern void do_phantom_stag_qtg_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result)
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

   divided_setup_move(
      ss, 
      get_map_from_code(MAPCODE(s_qtag,2,MPKIND__STAG,1)),
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
   setup *result)
{
   int rot =   ss->people[1].id1 | ss->people[2].id1 | ss->people[5].id1 | ss->people[6].id1 |
               ss->people[9].id1 | ss->people[10].id1 | ss->people[13].id1 | ss->people[14].id1;

   if (ss->kind != s4x4)
      fail("Need a 4x4 setup to do this concept.");

   if (!(rot & 011))
      fail("Can't figure this out -- not enough live people.");

   if ((rot & 011) == 011)
      fail("Can't figure this out -- facing directions are too weird.");

   rot = (rot ^ parseptr->concept->value.arg2) & 1;

   /* The thing to verify, like CMD_MISC__VERIFY_1_4_TAG. */
   ss->cmd.cmd_misc_flags |= parseptr->concept->value.arg3;

   ss->rotation += rot;   /* Just flip the setup around and recanonicalize. */
   canonicalize_rotation(ss);

   divided_setup_move(
      ss, 
      get_map_from_code(MAPCODE(s_qtag,2,MPKIND__DIAGQTAG,1)),
      (phantest_kind) parseptr->concept->value.arg1,
      TRUE,
      result);

   result->rotation -= rot;   /* Flip the setup back. */
   /* The split-axis bits are gone.  If someone needs them, we have work to do. */
   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
}



/* This returns true if the two live people were consecutive, meaning this is a true Z line/column. */
Private long_boolean search_row(
   int n,
   veryshort *x1,
   veryshort *x2,
   Const veryshort row[8],
   Const setup *s)
{
   int i, z, fill_count;
   long_boolean win;
   int nn = n;
   if (n < 0) nn = -n;     /* Negative means we handle errors differently. */

   fill_count = 0;
   z = 2;                       /* counts downward as we see consecutive people */
   win = FALSE;
   for (i=0; i<nn; i++)
      if (!s->people[row[i]].id1)           /* live person here? */
         z = 2;                 /* no, reset the consecutivity counter */
      else {
         z--;                   /* yes, count -- if get two in a row, turn "win" on */
         if (z == 0) win = TRUE;
         /* then push this index onto the map, check for no more than 2 */
         switch (++fill_count) {
            case 1: *x1 = row[i]; break;
            case 2: *x2 = row[i]; break;
            default: goto lose;
         }
      }
   
   /* Now check that the map has exactly 2 (or 1 if nn==2) people.
      But if n was negative, we don't print an error -- we just report as our
      return value whether there were exactly two people, regardless of consecutivity. */

   if (n < 0) return (fill_count == 2);
   else if (fill_count == 2 || (nn == 2 && fill_count == 1)) return win;

   lose:

   if (n < 0) return FALSE;
   fail("Can't identify distorted line or column.");
}


extern void distorted_2x2s_move(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   /* maps for 4x4 Z's */
   static Const veryshort map1[16] = {12, 15, 11, 10, 3, 2, 4, 7, 12, 3, 7, 10, 15, 2, 4, 11};
   static Const veryshort map2[16] = {12, 13, 3, 15, 11, 7, 4, 5, 12, 13, 7, 11, 15, 3, 4, 5};
   static Const veryshort map3[16] = {14, 1, 2, 3, 10, 11, 6, 9, 11, 1, 2, 6, 10, 14, 3, 9};
   static Const veryshort map4[16] = {13, 14, 1, 3, 9, 11, 5, 6, 13, 14, 11, 9, 3, 1, 5, 6};
   static Const veryshort map5[16] = {10, 13, 15, 9, 7, 1, 2, 5, 10, 7, 5, 9, 13, 1, 2, 15};
   static Const veryshort map6[16] = {13, 14, 15, 10, 7, 2, 5, 6, 13, 14, 2, 7, 10, 15, 5, 6};
   static Const veryshort map7[16] = {3, 0, 1, 7, 9, 15, 11, 8, 15, 0, 1, 11, 9, 3, 7, 8};
   static Const veryshort map8[16] = {14, 0, 3, 15, 11, 7, 6, 8, 14, 0, 7, 11, 15, 3, 6, 8};

   /* maps for 3x4 Z's */
   static Const veryshort mapb[16] = {-1, -1, -1, -1, -1, -1, -1, -1, 10, 2, 5, 9, 11, 3, 4, 8};
   static Const veryshort mapc[16] = {-1, -1, -1, -1, -1, -1, -1, -1, 0, 5, 7, 10, 1, 4, 6, 11};

   /* maps for 2x6 Z's */
   static Const veryshort mape[16] = {0, 1, 9, 10, 3, 4, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};
   static Const veryshort mapf[16] = {1, 2, 10, 11, 4, 5, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1};

   /* maps for split phantom 1/4 tag Z's */
   static Const veryshort mapg[16] = {0, 14, 10, 13, 2, 5, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1};
   static Const veryshort maph[16] = {13, 1, 14, 11, 6, 3, 5, 9, -1, -1, -1, -1, -1, -1, -1, -1};

   /* maps for 2x3 Z */
   static Const veryshort mape1[8] = {0, 1, 3, 4, -1, -1, -1, -1};
   static Const veryshort mapf1[8] = {1, 2, 4, 5, -1, -1, -1, -1};

   /* maps for twin parallelograms */
   static Const veryshort map_p1[16] = {2, 3, 11, 10, 5, 4, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1};
   static Const veryshort map_p2[16] = {0, 1, 4, 5, 10, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};

   /* maps for interlocked boxes/interlocked parallelograms */
   static Const veryshort map_b1[16] = {1, 3, 4, 11, 10, 5, 7, 9, 1, 3, 5, 10, 11, 4, 7, 9};
   static Const veryshort map_b2[16] = {0, 2, 5, 10, 11, 4, 6, 8, 0, 2, 4, 11, 10, 5, 6, 8};

   /* Maps for jays.
      These have the extra 24 to handle going to 1x4's. */

   static Const veryshort mapj1[48] = {
               7, 2, 4, 5, 0, 1, 3, 6,
               6, 3, 4, 5, 0, 1, 2, 7,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0,
               -1, -1, -1, -1, -1, -1, -1, -1};

   static Const veryshort mapj2[48] = {
               6, 3, 4, 5, 0, 1, 2, 7,
               7, 2, 4, 5, 0, 1, 3, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5,
               -1, -1, -1, -1, -1, -1, -1, -1};

   static Const veryshort mapj3[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               7, 2, 4, 5, 0, 1, 3, 6,
               6, 3, 4, 5, 0, 1, 2, 7,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0};

   static Const veryshort mapj4[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               6, 3, 4, 5, 0, 1, 2, 7,
               7, 2, 4, 5, 0, 1, 3, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5};

   /* Maps for facing/back-to-front/back-to-back parallelograms.
      These have the extra 24 to handle going to 1x4's. */

   static Const veryshort mapk1[48] = {
               3, 2, 4, 5, 0, 1, 7, 6,
               6, 7, 4, 5, 0, 1, 2, 3,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0,
               -1, -1, -1, -1, -1, -1, -1, -1};
   static Const veryshort mapk2[48] = {
               6, 7, 4, 5, 0, 1, 2, 3,
               3, 2, 4, 5, 0, 1, 7, 6,
               -1, -1, -1, -1, -1, -1, -1, -1,
               5, 4, 2, 3, 6, 7, 1, 0,
               0, 1, 2, 3, 6, 7, 4, 5,
               -1, -1, -1, -1, -1, -1, -1, -1};
   static Const veryshort mapk3[48] = {
               -1, -1, -1, -1, -1, -1, -1, -1,
               3, 2, 4, 5, 0, 1, 7, 6,
               6, 7, 4, 5, 0, 1, 2, 3,
               -1, -1, -1, -1, -1, -1, -1, -1,
               0, 1, 2, 3, 6, 7, 4, 5,
               5, 4, 2, 3, 6, 7, 1, 0};

   static Const veryshort mapk4[48] = {
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
   Const veryshort *map_ptr;

   concept_descriptor *this_concept = parseptr->concept;

   /* Check for special case of "interlocked parallelogram", which doesn't look like the
      kind of concept we are expecting. */

   if (this_concept->kind == concept_do_both_boxes) {
      table_offset = 8;
      misc_indicator = 3;
   }
   else {
      table_offset = this_concept->value.arg3;
      misc_indicator = this_concept->value.arg1;
   }

   /* misc_indicator    concept
           0         some kind of "Z"
           1         some kind of "Jay"
           2       twin parallelograms (the physical setup is like offset lines)
           3       interlocked boxes/Pgrams (the physical setup is like Z's)
           4         some kind of parallelogram (as from "heads travel thru")
           5       distorted blocks */

   /* table_offset is 0, 8, or 12.  It selects the appropriate part of the maps. */
   /* For "Z"      :   0 == normal, 8 == interlocked. */
   /* For Jay/Pgram:   0 == normal, 8 == back-to-front, 16 == back-to-back. */

   directions = 0;
   livemask = 0;
   arity = 2;

   for (i=0 ; i<=setup_attrs[ss->kind].setup_limits ; i++) {
      uint32 p = ss->people[i].id1;
      directions = (directions<<2) | (p&3);
      livemask <<= 1;
      if (p) livemask |= 1;
   }

   switch (misc_indicator) {
   case 0:
      /* The concept is some variety of "Z" */
      arity = this_concept->value.arg4;

      if (arity == 3) {
         switch (ss->kind) {
         case s3x6:
            if ((livemask & 0630630) == 0)
               warn(warn_same_z_shear);  /* Outer Z's are ambiguous --
                                            make them look like inner ones. */

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
            break;
         case s3x4:
            arity = 2;  /* Might have just said "Z", which would set arity
                           to 1.  We can nevertheless find the two Z's. */
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
         case s2x6:
            arity = 2;  /* Might have just said "Z", which would set arity
                           to 1.  We can nevertheless find the two Z's. */
            if (     (livemask & 01111) == 0) map_ptr = mape;
            else if ((livemask & 04444) == 0) map_ptr = mapf;
            else goto lose;
            break;
         case s4dmd:
            arity = 2;
            if (     (livemask & 0x5959) == 0) map_ptr = mapg;
            else if ((livemask & 0xA9A9) == 0) map_ptr = maph;
            else goto lose;
            break;
         case s2x3:
            if (arity != 1) fail("Use the 'Z' concept here.");
            if (     (livemask & 011) == 0) map_ptr = mape1;
            else if ((livemask & 044) == 0) map_ptr = mapf1;
            else goto lose;
            break;
         default:
            fail("Must have 3x4, 2x6, 2x3, 4x4, or split 1/4 tags for this concept.");
         }
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
      switch (ss->kind) {     /* The concept is twin parallelograms */
      case s3x4:
         if (     (livemask & 06060) == 0) map_ptr = map_p1;
         else if ((livemask & 01414) == 0) map_ptr = map_p2;
         else goto lose;
         break;
      default:
         fail("Must have 3x4 setup for this concept.");
      }
      break;
   case 3:
      switch (ss->kind) {   /* The concept is interlocked boxes or interlocked parallelograms */
      case s3x4:
         if (     (livemask & 05050) == 0) map_ptr = map_b1;
         else if ((livemask & 02424) == 0) map_ptr = map_b2;
         else goto lose;
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
      
         /* Search for the live people, in rows first. */

         if (!search_row(-4, &the_map[0], &the_map[3], list_10_6_5_4,   ss)) rows = 0;
         if (!search_row(-4, &the_map[4], &the_map[7], list_11_13_7_2,  ss)) rows = 0;
         if (!search_row(-4, &the_map[1], &the_map[2], list_12_17_3_1,  ss)) rows = 0;
         if (!search_row(-4, &the_map[5], &the_map[6], list_14_15_16_0, ss)) rows = 0;

            /* Now search in columns, and store the result starting at 8. */

         if (!search_row(-4, &the_map[8],  &the_map[11], list_10_6_5_4,   &rotss)) columns = 0;
         if (!search_row(-4, &the_map[12], &the_map[15], list_11_13_7_2,  &rotss)) columns = 0;
         if (!search_row(-4, &the_map[9],  &the_map[10], list_12_17_3_1,  &rotss)) columns = 0;
         if (!search_row(-4, &the_map[13], &the_map[14], list_14_15_16_0, &rotss)) columns = 0;

         /* At this point, exactly one of "rows" and "columns" should be equal to 1. */

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
      gather(&inputs[i], ss, map_ptr, 3, 0);
      inputs[i].rotation = 0;
      inputs[i].kind = s2x2;
      inputs[i].cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
      inputs[i].cmd.cmd_assume.assumption = cr_none;
      update_id_bits(&inputs[i]);
      move(&inputs[i], FALSE, &results[i]);

      if (results[i].kind != s2x2) {
         if (results[i].kind != s1x4 ||
             results[i].rotation != 1 ||
             (misc_indicator != 1 && misc_indicator != 4))
            fail("Can't do shape-changer with this concept.");
         scatter(result, &results[i], &map_ptr[24], 3, 0);
      }
      else
         scatter(result, &results[i], map_ptr, 3, 0);

      map_ptr += 4;
   }

   result->kind = ss->kind;
   result->rotation = 0;

   if (misc_indicator == 1 || misc_indicator == 4) {
      if (results[0].kind == s1x4 && results[0].kind == s1x4) {
         result->kind = s_rigger;
         result->rotation = 1;
         if (misc_indicator == 1) warn(warn__hokey_jay_shapechanger);
      }
      else if (results[0].kind != s2x2 || results[0].kind != s2x2)
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
   setup *result)
{

/*
   Args from the concept are as follows:
   disttest (usually just the arg1 field from the concept,
            but may get fudged for random bigblock/stagger) =
      disttest_offset - user claims this is offset lines/columns
      disttest_z      - user claims this is Z lines/columns
      disttest_any    - user claims this is distorted lines/columns
   linesp & 7 =
      0 - user claims this is some kind of columns
      1 - user claims this is some kind of lines
      3 - user claims this is waves
   linesp & 8 != 0:
      user claims this is a single tidal (grand) setup
   linesp & 16 != 0:
      user claims this is distorted 1/4 tags or diamonds or whatever

   Global_tbonetest has the OR of all the people, or all the standard people if
      this is "standard", so it is what we look at to interpret the
      lines vs. columns nature of the concept.
*/

   static Const veryshort list_0_12_11[3] = {0, 10, 9};
   static Const veryshort list_1_13_10[3] = {1, 11, 8};
   static Const veryshort list_2_5_7[3] = {2, 5, 7};
   static Const veryshort list_3_4_6[3] = {3, 4, 6};
   static Const veryshort list_2x8[16] = {0, 15, 1, 14, 3, 12, 2, 13, 7, 8, 6, 9, 4, 11, 5, 10};

   veryshort the_map[8];
   Const parse_block *next_parseptr;
   uint64 junk_concepts;
   int rot, rotz;
   setup_kind k;
   setup a1;
   setup res1;
   mpkind mk, mkbox;

   map_thing *map_ptr;
   uint32 mapcode = ~0UL;
   int rotate_back = 0;
   int livemask = global_livemask;
   uint32 linesp = parseptr->concept->value.arg2;
   long_boolean zlines = TRUE;

   if (linesp & 8) {
      int i;

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

            disttest = disttest_offset;  /* We know what we are doing -- shut off the error message. */
            goto do_divided_call;
         }
         else if (ss->kind == s2x8) {
            /* Search for the live people. */

            for (i=0; i<8; i++) (void) search_row(2, &the_map[i], &the_map[i], &list_2x8[i<<1], ss);

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

         goto do_divided_nocheck;
      }
      else
         fail("Must have triple diamond setup for this concept.");
   }
   else {
      k = s2x4;

      if (ss->kind == s4x4) {
         /* **** This is all sort of a crock.  Here we are picking out the "winning" method for
           doing distorted/offset/Z lines and columns.  Below, we do it by the old way, which allows
           T-bone setups. */
      
         /* If any people are T-boned, we must invoke the other method and hope for the best.
            ***** We will someday do it right. */
            
         if ((global_tbonetest & 011) == 011) {
            if (disttest != disttest_offset)
               fail("Sorry, can't apply this concept when people are T-boned.");
      
            phantom_2x4_move(ss, linesp & 1, phantest_only_one, &map_offset, result);
            return;
         }
   
         /* Look for butterfly or "O" spots occupied. */

         if (livemask == 0x6666 || livemask == 0x9999) {
            if (!((linesp ^ global_tbonetest) & 1)) {    /* What a crock -- this is all backwards. */
               rotate_back = 1;                          /* (Well, actually everything else is backwards.) */
               ss->rotation++;
               canonicalize_rotation(ss);
            }
            mapcode = (livemask & 1) ? MAPCODE(s2x4,2,MPKIND__X_SPOTS,1) : MAPCODE(s2x4,2,MPKIND__O_SPOTS,1);
            disttest = disttest_offset;  /* We know what we are doing -- shut off the error message. */
            goto do_divided_call;
         }
   
         if ((linesp ^ global_tbonetest) & 1) {
            rotate_back = 1;
            ss->rotation++;
            canonicalize_rotation(ss);
            livemask = ((livemask << 4) & 0xFFFF) | (livemask >> 12);
         }
   
         /* Check for special case of offset lines/columns, and do it the elegant way (handling shape-changers) if so. */

         if (livemask == 0xB4B4) { mk = MPKIND__OFFS_L_FULL; mkbox = MPKIND__OFFS_L_FULL_SPECIAL; goto do_offset_call; }
         if (livemask == 0x4B4B) { mk = MPKIND__OFFS_R_FULL; mkbox = MPKIND__OFFS_R_FULL_SPECIAL; goto do_offset_call; }

         /* Search for the live people.
            Must scan sideways for each Y value, looking for exactly 2 people
            If any of the scans returns false, meaning that the 2 people are not adjacent,
            set zlines to false. */

         if (!search_row(4, &the_map[0], &the_map[7], list_10_6_5_4, ss)) zlines = FALSE;
         if (!search_row(4, &the_map[1], &the_map[6], list_11_13_7_2, ss)) zlines = FALSE;
         if (!search_row(4, &the_map[2], &the_map[5], list_12_17_3_1, ss)) zlines = FALSE;
         if (!search_row(4, &the_map[3], &the_map[4], list_14_15_16_0, ss)) zlines = FALSE;
      
         rot = 011;
         rotz = 033;
         result->kind = s4x4;
      }
      else if (ss->kind == s3x4) {
         if (linesp & 1) {
            if (global_tbonetest & 1) fail("There are no lines of 4 here.");
         }
         else {
            if (global_tbonetest & 010) fail("There are no columns of 4 here.");
         }
   
         /* Check for special case of offset lines/columns, and do it the elegant way (handling shape-changers) if so. */
         
         if (livemask == 07474) { mk = MPKIND__OFFS_L_HALF; mkbox = MPKIND__OFFS_L_HALF_SPECIAL; goto do_offset_call; }
         if (livemask == 06363) { mk = MPKIND__OFFS_R_HALF; mkbox = MPKIND__OFFS_R_HALF_SPECIAL; goto do_offset_call; }

         /* Search for the live people. */
         
         if (!search_row(3, &the_map[0], &the_map[7], list_0_12_11, ss)) zlines = FALSE;
         if (!search_row(3, &the_map[1], &the_map[6], list_1_13_10, ss)) zlines = FALSE;
         if (!search_row(3, &the_map[2], &the_map[5], list_2_5_7, ss)) zlines = FALSE;
         if (!search_row(3, &the_map[3], &the_map[4], list_3_4_6, ss)) zlines = FALSE;
         
         rot = 0;
         rotz = 0;
         result->kind = s3x4;      
      }
      else
         fail("Must have 3x4 or 4x4 setup for this concept.");
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

   if (next_parseptr->concept->kind == concept_do_phantom_2x4 &&
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
   setup *result)
{
   uint32 tbonetest = global_tbonetest;
   uint32 mapcode;
   phantest_kind phan = phantest_ok;

   /* Arg1 = 0/1/3 for C/L/W, usual coding. */
   /* Arg3 =
      0 : triple twin C/L/W
     (not used!)      1 : triple C/L/W of 6
      2 : quadruple C/L/W of 6
      3 : [split/interlocked] phantom C/L/W of 6, arg4 has map kind
      4 : triple twin C/L/W of 3 */

   /* This needs a 3x6 setup -- need to make a NEEDK indicator and use the "ARG2_MATRIX" stuff. */
   if (parseptr->concept->value.arg3 == 1) fail("Sorry, can't do this.");

   /* The setup has not necessarily been expanded to a 4x6.  It has only been
      expanded to a 4x4.  Why?  Because the stuff in toplevelmove that does
      the expansion didn't know which way to orient the 4x6.  (It might have
      depended on "standard" information.)  So why is the expansion supposed
      to be done there rather than at the point where the concept is executed?
      It is to prevent monstrosities like "in your split phantom line you
      have a split phantom column".  Expansion takes place only at the top
      level.  When the concept "in your split phantom line" is executed,
      toplevelmove expands the setup to a 4x4, the concept routine splits
      it into 2x4's, and then the second concept is applied without further
      expansion.  Since we now have 2x4's and the "you have a split phantom
      column" concept requires a 4x4, it will raise an error.  If the
      expansion were done wherever a concept is performed, this monstrosity
      would by permitted to occur.  So the remaining question is "What
      safety are we sacrificing (or what monstrosities are we permitting)
      by doing the expansion here?"  The answer is that, if there were a
      concept that divided the setup into 4x4's, we could legally do
      something like "in your split phantom 4x4's you have triple twin
      columns".  It would expand each 4x4 into a 4x6 and go the call.
      Horrors.  Fortunately, there are no such concepts.  Of course
      the really right way to do this is to have a setupflag called
      NOEXPAND, and do the expansion when the concept is acted upon.
      Anyway, here goes. */

   if ((tbonetest & 011) == 011) fail("Can't do this from T-bone setup.");

   tbonetest ^= parseptr->concept->value.arg1;

   if (parseptr->concept->value.arg3 == 4) {
      if (ss->kind != s3x6) fail("Must have a 3x6 setup for this concept.");
      mapcode = MAPCODE(s2x3,3,MPKIND__SPLIT,1);
      phan = phantest_not_just_centers;
   }
   else {
      if (parseptr->concept->value.arg3 != 0) tbonetest ^= 1;

      if (ss->kind == s4x4) {
         expand_setup(((tbonetest & 1) ? &exp_4x4_4x6_stuff_b : &exp_4x4_4x6_stuff_a), ss);
         tbonetest = 0;
      }

      if (ss->kind != s4x6) fail("Must have a 4x6 setup for this concept.");
   
      switch (parseptr->concept->value.arg3) {
      case 0:
         mapcode = MAPCODE(s2x4,3,MPKIND__SPLIT,1);
         phan = phantest_not_just_centers;
         break;
      case 2:
         mapcode = MAPCODE(s1x6,4,MPKIND__SPLIT,1);
         break;
      case 3:
         mapcode = MAPCODE(s2x6,2,(mpkind)parseptr->concept->value.arg4,1);
         break;
      }

      if ((ss->cmd.cmd_misc2_flags & CMD_MISC2__MYSTIFY_SPLIT) &&
          parseptr->concept->value.arg3 != 0)
         fail("Mystic not allowed with this concept.");
   }

   if (parseptr->concept->value.arg1 == 3)
      ss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;
   
   if (tbonetest & 1) {
      if (parseptr->concept->value.arg1 == 0) fail("Can't find the required columns.");
      else fail("Can't find the required lines.");
   }

   new_divided_setup_move(ss, mapcode, phan, TRUE, result);
}



extern void do_concept_rigger(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   /* First 8 are for rigger; second 8 are for 1/4-tag, final 16 are for crosswave. */
   /* A huge coincidence is at work here -- the first two parts of the maps are the same. */
   static Const veryshort map1[32] = {
         0, 1, 3, 2, 4, 5, 7, 6, 0, 1, 3, 2, 4, 5, 7, 6,
         13, 15, 1, 3, 5, 7, 9, 11, 14, 0, 2, 4, 6, 8, 10, 12};
   static Const veryshort map2[32] = {
         2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1,
         0, 2, 4, 6, 8, 10, 12, 14, 1, 7, 5, 11, 9, 15, 13, 3};

   int rstuff, indicator, base;
   setup a1;
   setup res1;
   Const veryshort *map_ptr;
   setup_kind startkind;

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

   clear_people(&a1);

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
   else
      fail("Must have a 'rigger' or quarter-tag setup to do this concept.");

   if (indicator & 1)
      fail("'Rigger' direction is inappropriate.");

   map_ptr = indicator ? map1 : map2;
   scatter(&a1, ss, &map_ptr[base], 7, 0);
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
   else {
      if (res1.kind != s2x4) fail("Can't do this.");
      result->kind = base ? s_qtag : s_rigger;
   }

   gather(result, &res1, &map_ptr[base], 7, 0);
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
   setup *result)
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
      common spot 1/4 tags                     : 0x200 */

   if (ss->kind == s_c1phan) {
      do_matrix_expansion(ss, CONCPROP__NEEDK_4X4, FALSE);
      /* Shut off the "check a 4x4 matrix" warning that this will raise. */
      history[history_ptr+1].warnings.bits[warn__check_4x4_start>>5] &=
         ~(1 << (warn__check_4x4_start & 0x1F));
      /* Unless, of course, we already had that warning. */
      history[history_ptr+1].warnings.bits[warn__check_4x4_start>>5] |=
         saved_warnings.bits[warn__check_4x4_start>>5];
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

      if (the_results[0].kind == s_qtag && the_results[1].kind == s2x3 && the_results[0].rotation != the_results[1].rotation)
         expand_setup(&exp_2x3_qtg_stuff, &the_results[1]);
      else if (the_results[1].kind == s_qtag && the_results[0].kind == s2x3 && the_results[0].rotation != the_results[1].rotation)
         expand_setup(&exp_2x3_qtg_stuff, &the_results[0]);

      if (the_results[0].kind != the_results[1].kind || the_results[0].rotation != the_results[1].rotation)
         fail("This common spot call is very problematical.");

      /* Remove the uncommon people from the common results, while checking that
         they wound up in the same position in all 3 results. */

      for (i=0; i<=setup_attrs[map_ptr->partial_kind].setup_limits; i++) {
         int t = map_ptr->uncommon[i];
         if (t >= 0 && ss->people[t].id1) {

            /* These bits are not a property just of the person and his position
               in the formation -- they depend on other people's facing directin. */
#define ID2_BITS_NOT_INTRINSIC (ID2_FACING | ID2_NOTFACING)

            for (k=0; k<=setup_attrs[the_results[0].kind].setup_limits; k++) {
               if (     the_results[0].people[k].id1 &&
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

   /* Turn of any "do your part" warnings that arose during execution
      of the subject call.  The dancers already know. */

   history[history_ptr+1].warnings.bits[warn__do_your_part>>5] &= ~(1 << (warn__do_your_part & 0x1F));

   for (i=0 ; i<WARNING_WORDS ; i++)
      history[history_ptr+1].warnings.bits[i] |= saved_warnings.bits[i];
}


typedef struct qwerty {
   Const setup_kind kind;
   Const struct qwerty *other;
   Const veryshort mapqt1[8];   /* In quarter-tag: first triangle (upright), then second triangle (inverted), then idle. */
   Const veryshort mapcp1[8];   /* In C1 phantom: first triangle (inverted), then second triangle (upright), then idle. */
   Const veryshort mapbd1[8];   /* In bigdmd. */
   Const veryshort map241[8];   /* In 2x4. */
   Const veryshort map261[8];   /* In 2x6. */
} tgl_map;

/* These need to be predeclared so that they can refer to each other. */
static Const tgl_map map1b;
static Const tgl_map map2b;
static Const tgl_map map1i;
static Const tgl_map map2i;
static Const tgl_map map1j;
static Const tgl_map map2j;
static Const tgl_map map1k;
static Const tgl_map map2k;

/*                             kind     other
               mapqt1                         mapcp1                         mapbd1                            map241                    map261 */

static Const tgl_map map1b = {s_c1phan, &map2b,
   {4, 3, 2,   0, 7, 6,   1, 5}, {6,  8, 10,   14,  0,  2,   4, 12}, {10, 9, 8,   4, 3, 2,   5, 11}, {6, 5, 4, 2, 1, 0, 3, 7},  {10, 9, 8, 4, 3, 2, 5, 11}};

static Const tgl_map map2b = {s_qtag,   &map1b,
   {5, 6, 7,   1, 2, 3,   0, 4}, {3, 15, 13,   11,  7,  5,   1,  9}, {1, 2, 3,    7, 8, 9,   0, 6},  {7, 6, 5, 3, 2, 1, 0, 4},  {1, 2, 3, 7, 8, 9, 0, 6}};

/* Interlocked triangles: */                                                                                                                            
static Const tgl_map map1i = {nothing,  &map2i,
   {0, 0, 0,   0, 0, 0,   0, 0}, {4,  8, 10,   12,  0,  2,   6, 14}, {0, 0, 0,    0, 0, 0,   0, 0},  {0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 0, 0}};

static Const tgl_map map2i = {nothing,  &map1i,
   {0, 0, 0,   0, 0, 0,   0, 0}, {1, 15, 13,   9,   7,  5,   3, 11}, {0, 0, 0,    0, 0, 0,   0, 0},  {0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 0, 0}};

/* Interlocked triangles in quarter-tag: */                                                                                                             
static Const tgl_map map1j = {s_qtag,   &map2j,
   {4, 7, 2,   0, 3, 6,   1, 5}, {0,  0,  0,    0,  0,  0,   0, -1}, {0, 0, 0,    0, 0, 0,   0, 0},  {0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 0, 0}};

static Const tgl_map map2j = {s_qtag,   &map1j,
   {5, 6, 3,   1, 2, 7,   0, 4}, {0,  0,  0,    0,  0,  0,   0, -1}, {0, 0, 0,    0, 0, 0,   0, 0},  {0, 0, 0, 0, 0, 0, 0, 0},  {0, 0, 0, 0, 0, 0, 0, 0}};

/* Interlocked triangles in bigdmd: */                                                                                                                  
static Const tgl_map map1k = {nothing,  &map2k,
   {0, 0, 0,   0, 0, 0,   0, 0},
   {0, 0, 0,   0, 0, 0,   0, -1},
   {10, 3, 8,  4, 9, 2,   5, 11},
   {0},
   {0}};

static Const tgl_map map2k = {nothing,  &map1k,
   {0, 0, 0,   0, 0, 0,   0, 0},
   {0, 0, 0,   0, 0, 0,   0, -1},
   {1, 2, 9,   7, 8, 3,   0, 6},
   {0},
   {0}};

static Const tgl_map map2r = {nothing,  &map2r,
   {0, 0, 0,   0, 0, 0,   0, 0},
   {0, 0, 0,   0, 0, 0,   0, -1},
   {7, 1, 4,   3, 5, 0,   2, 6},
   {0},
   {0}};


Private void do_glorious_triangles(
   setup *ss,
   Const tgl_map *map_ptr,
   setup *result)
{
   int i, r, startingrot;
   uint32 rotstate;
   setup a1, a2;
   setup idle;
   setup res[2];
   Const veryshort *mapnums;

   if (ss->kind == s_c1phan) {
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

   if (fix_n_results(2, nothing, res, &rotstate)) {
      result->kind = nothing;
      result->result_flags = 0;
      return;
   }

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
      /* We know that res[0].rotation != startingrot */
      if (startingrot == 1) fail("Sorry, can't do this.");

      if (res[0].rotation == 0) {
         if (result->kind == nothing) fail("Can't do shape-changer in interlocked triangles.");
         result->kind = s_qtag;
         /* Restore the two people who don't move. */
         (void) copy_person(result, map_ptr->mapqt1[6], &idle, 0);
         (void) copy_person(result, map_ptr->mapqt1[7], &idle, 1);
         scatter(result, &res[0], map_ptr->mapqt1, 2, 0);
         scatter(result, &res[1], &map_ptr->mapqt1[3], 2, 022);
      }
      else if (res[0].rotation == 2) {
         if (map_ptr->mapcp1[7] < 0)
            fail("Can't do shape-changer in interlocked triangles.");

         result->kind = s_c1phan;
         /* Restore the two people who don't move. */
         (void) copy_rot(result, map_ptr->mapcp1[7], &idle, 0, r);
         (void) copy_rot(result, map_ptr->mapcp1[6], &idle, 1, r);
         scatter(result, &res[0], &map_ptr->mapcp1[3], 2, 0);
         scatter(result, &res[1], map_ptr->mapcp1, 2, 022);
      }
      else {
         if (result->kind == nothing || map_ptr->mapcp1[7] < 0)
            fail("Can't do shape-changer in interlocked triangles.");

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
      if (result->kind == nothing || map_ptr->mapcp1[7] < 0)
         fail("Can't do shape-changer in interlocked triangles.");

      result->kind = s2x4;

      if (res[0].rotation == 0) {
         if (startingrot == 1) {
            mapnums = map_ptr->map261;
            result->kind = s2x6;
         }
         else {
            mapnums = map_ptr->map241;
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

         if (map_ptr->kind == s_qtag) {    /* What a crock! */
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
   else if (res[0].rotation != startingrot) {
      if (result->kind == s_c1phan)
          warn(warn__check_c1_phan);
      else
          warn(warn__check_dmd_qtag);
   }
}




/* This procedure does wave-base, tandem-base, and so-and-so-base. */
Private void wv_tand_base_move(
   setup *s,
   int indicator,
   setup *result)
{
   uint32 tbonetest;
   int t;
   calldef_schema schema;
   Const tgl_map *map_ptr;

   switch (s->kind) {
      case s_galaxy:
         if ((indicator & 076) != 6)   /* Only "tandem-base" and "wave-base" are allowed here. */
            fail("Can't find the indicated triangles.");

         tbonetest = s->people[1].id1 | s->people[3].id1 | s->people[5].id1 | s->people[7].id1;

         if ((tbonetest & 011) == 011)
            fail("Can't find the indicated triangles.");
         else if ((indicator ^ tbonetest) & 1)
            schema = (indicator & 0100) ? schema_intlk_lateral_6 : schema_lateral_6;
         else
            schema = (indicator & 0100) ? schema_intlk_vertical_6 : schema_vertical_6;
   
         /* For galaxies, the schema is now in terms of the absolute orientation. */
         /* We know that the original setup rotation was canonicalized. */
         break;
      case s_hrglass:
         if ((indicator & 076) != 6)   /* Only "tandem-base" and "wave-base" are allowed here. */
            fail("Can't find the indicated triangles.");

         tbonetest = s->people[0].id1 | s->people[1].id1 | s->people[4].id1 | s->people[5].id1;

         if ((tbonetest & 011) == 011 || ((indicator ^ tbonetest) & 1))
            fail("Can't find the indicated triangles.");
   
         schema = (indicator & 0100) ? schema_intlk_vertical_6 : schema_vertical_6;
         break;
      case s_c1phan:
         if ((indicator&63) == 20) {
            t = 0;
            if (global_selectmask == (global_livemask & 0x5A5A))
               t = 1;
            else if (global_selectmask != (global_livemask & 0xA5A5))
               fail("Can't find the indicated triangles.");
         }
         else {
            t = indicator & 1;
            if ((global_tbonetest & 010) == 0) t ^= 1;
            else if ((global_tbonetest & 1) != 0)
               fail("Can't find the indicated triangles.");
         }

         /* Now t is 0 to select the triangles whose bases are horizontally
            aligned, and 1 for the vertically aligned bases. */

         s->rotation += t;   /* Just flip the setup around and recanonicalize. */
         canonicalize_rotation(s);

         if ((global_livemask & 0xAAAA) == 0)
            map_ptr = (indicator & 0100) ? &map1i : &map1b;
         else if ((global_livemask & 0x5555) == 0)
            map_ptr = (indicator & 0100) ? &map2i : &map2b;
         else
            fail("Can't find the indicated triangles.");

         do_glorious_triangles(s, map_ptr, result);
         result->rotation -= t;   /* Flip the setup back. */
         reinstate_rotation(s, result);
         return;
      default:
         fail("Can't do this concept in this setup.");
   }

   concentric_move(s, &s->cmd, (setup_command *) 0, schema, 0, 0, TRUE, result);
}


extern void triangle_move(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   uint32 tbonetest;
   Const tgl_map *map_ptr;
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
   Add 100 octal if interlocked triangles. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__RESTRAIN_MODIFIERS) {
      ss->cmd.cmd_misc_flags &= ~CMD_MISC__RESTRAIN_MODIFIERS;
   }
   else {
      if (indicator >= 2 && indicator <= 21 && (TEST_HERITBITS(ss->cmd.cmd_final_flags,INHERITFLAG_INTLK))) {
         indicator |= 0100;     /* Interlocked triangles. */
         ss->cmd.cmd_final_flags.her8it &= ~INHERITFLAG_INTLK;
      }

      if (ss->cmd.cmd_final_flags.her8it | ss->cmd.cmd_final_flags.final)   /* Now demand that no flags remain. */
         fail("Illegal modifier for this concept.");
   }

   indicator_base = indicator & 077;

   if ((indicator & 0100) && calling_level < intlk_triangle_level) {
      if (allowing_all_concepts)
         warn(warn__bad_concept_level);
      else
         fail("This concept modifier is not allowed at this level.");
   }

   if (indicator <= 1) {
      /* Indicator = 0 for tall 6, 1 for short 6 */

      if (ss->kind == s_galaxy) {
         /* We know the setup rotation is canonicalized. */
         tbonetest = ss->people[1].id1 | ss->people[3].id1 | ss->people[5].id1 | ss->people[7].id1;

         if ((tbonetest & 011) == 011) fail("Can't find tall/short 6.");
         else if ((indicator ^ tbonetest) & 1)
            schema = schema_lateral_6;
         else
            schema = schema_vertical_6;
      }
      else
         fail("Must have galaxy for this concept.");

      /* For galaxies, the schema is now in terms of the absolute orientation. */

      concentric_move(ss, &ss->cmd, (setup_command *) 0, schema, 0, 0, TRUE, result);
   }
   else {
      /* Set this so we can do "peel and trail" without saying "triangle" again. */
      ss->cmd.cmd_misc_flags |= CMD_MISC__SAID_TRIANGLE;

      if (indicator_base >= 6) {
         /* Indicator = 6 for wave-base, 7 for tandem-base, 20 for <anyone>-base. */
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
   
         if (t == 1)
            map_ptr = (indicator & 0100) ? &map1j : &map1b;
         else if (t == 2)
            map_ptr = (indicator & 0100) ? &map2j : &map2b;
         else
            fail("Can't find designated point.");

         do_glorious_triangles(ss, map_ptr, result);
         reinstate_rotation(ss, result);
      }
      else {
         /* Indicator = 2 for inside, 3 for outside */
   
         /* Only a few cases allow interlocked. */

         if (indicator_base == 2 && ss->kind == sbigdmd) {
            if (global_livemask == 07474)
               map_ptr = (indicator & 0100) ? &map1k : &map1b;
            else if (global_livemask == 01717)
               map_ptr = (indicator & 0100) ? &map2k : &map2b;
            else
               fail("Can't find the indicated triangles.");

            do_glorious_triangles(ss, map_ptr, result);
            reinstate_rotation(ss, result);

            return;
         }
         else if (indicator == 0102 && ss->kind == s_rigger) {
            do_glorious_triangles(ss, &map2r, result);
            reinstate_rotation(ss, result);

            return;
         }
         else if (indicator & 0100)
            fail("Illegal modifier for this concept.");

         if (indicator_base == 2) {
            switch (ss->kind) {
               case s_hrglass:
                  schema = schema_vertical_6;   /* This is the schema for picking out
                                                   the triangles in an hourglass. */
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
   
            concentric_move(ss, &ss->cmd, (setup_command *) 0, schema, 0, 0, TRUE, result);
         }
         else {
            switch (ss->kind) {
               case s_ptpd:
               case s_qtag:
               case s_spindle:
               case s_bone:
               case s_hrglass:
               case s_dhrglass:
                  schema = schema_concentric_2_6;
                  break;
               default:
                  fail("There are no 'outside' triangles.");
            }
   
            concentric_move(ss, (setup_command *) 0, &ss->cmd, schema, 0, 0, TRUE, result);
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
   setup *result)
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
