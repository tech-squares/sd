/* SD -- square dance caller's helper.

    Copyright (C) 1990-1997  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 31. */

/* This defines the following functions:
   get_multiple_parallel_resultflags
   initialize_conc_tables
   normalize_concentric
   concentric_move
   merge_setups
   on_your_own_move
   punt_centers_use_concept
   selective_move
   inner_selective_move
*/

#include "sd.h"


extern uint32 get_multiple_parallel_resultflags(setup outer_inners[], int number)
{
   int i;
   uint32 result_flags = 0;

   /* If a call was being done "piecewise" or "random", we demand that both
      calls run out of parts at the same time, and, when that happens, we
      report it to the higher level in the recursion. */

   for (i=0 ; i<number ; i++) {
      if (!(outer_inners[i].result_flags & RESULTFLAG__PARTS_ARE_KNOWN))
         outer_inners[i].result_flags &= ~(RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE);

      if (((outer_inners[i].result_flags & outer_inners[0].result_flags) & RESULTFLAG__PARTS_ARE_KNOWN) &&
            ((outer_inners[i].result_flags ^ outer_inners[0].result_flags) & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE)))
         fail("Two calls must use the same number of fractions.");

      result_flags |= outer_inners[i].result_flags;
   }

   return result_flags;
}



/* BEWARE!!  This is keyed to the enumeration "analyzer_kind". */
Private char *conc_error_messages[] = {
   "Can't find centers and ends in this formation.",                   /* analyzer_NORMAL */
   "Can't find checkpoint people in this formation.",                  /* analyzer_CHECKPT */
   "Can't find 2 centers and 6 ends in this formation.",               /* analyzer_2X6 */
   "Can't find 2 centers and 4 ends in this formation.",               /* analyzer_2X4 */
   "Can't find 6 centers and 2 ends in this formation.",               /* analyzer_6X2 */
   "Can't find centers and ends in this 6-person formation.",          /* analyzer_6P */
   "Can't find inside triangles in this formation.",                   /* analyzer_6X2_TGL */
   "Can't find outside 'O' spots.",                                    /* analyzer_O */
   "Can't find 12 matrix centers and ends in this formation.",         /* analyzer_CONC12 */
   "Can't find 16 matrix centers and ends in this formation.",         /* analyzer_CONC16 */
   "Can't find centers and ends in this formation.",                   /* analyzer_BAR */
   "Can't find 12 matrix centers and ends in this formation.",         /* analyzer_BAR12 */
   "Can't find 16 matrix centers and ends in this formation.",         /* analyzer_BAR16 */
   "Can't find 12 matrix centers and ends in this formation.",         /* analyzer_STAR12 */
   "Can't find 16 matrix centers and ends in this formation.",         /* analyzer_STAR16 */
   "Can't find 3x3 centers and ends in this formation.",               /* analyzer_3X3_CONC */
   "Can't find 16 matrix center and end lines in this formation.",     /* analyzer_4X4_LINES_CONC */
   "Can't find 16 matrix center and end columns in this formation.",   /* analyzer_4X4_COLS_CONC */
   "Can't find 3x1 concentric formation.",                             /* analyzer_3X1_CONC */
   "Can't find 3x1 concentric formation.",                             /* analyzer_1X3_CONC */
   "Can't do single concentric in this formation.",                    /* analyzer_SINGLE */
   "Can't do grand single concentric in this formation.",              /* analyzer_GRANDSINGLE */
   "Can't find triple lines/columns/boxes/diamonds in this formation.",/* analyzer_TRIPLE_LINE */
   "Can't find phantom lines/columns/boxes/diamonds in this formation.",/* analyzer_QUAD_LINE */
   "Wrong formation.",                                                 /* analyzer_VERTICAL6 */
   "Wrong formation.",                                                 /* analyzer_LATERAL6 */
   "Wrong formation.",                                                 /* analyzer_INTLK_VERTICAL6 */
   "Wrong formation.",                                                 /* analyzer_INTLK_LATERAL6 */
   "Wrong formation.",                                                 /* analyzer_OTHERS */
   "Can't find concentric diamonds.",                                  /* analyzer_CONC_DIAMONDS */
   "Can't find center line and outer diamond.",                        /* analyzer_DIAMOND_LINE */
   "Can't find center diamond."                                        /* analyzer_CTR_DMD */
};




/* Must be a power of 2. */
#define NUM_CONC_HASH_BUCKETS 32

static cm_thing *conc_hash_synthesize_table[NUM_CONC_HASH_BUCKETS];
static cm_thing *conc_hash_analyze_table[NUM_CONC_HASH_BUCKETS];


extern void initialize_conc_tables(void)
{
   cm_thing *tabp;
   int i;

   for (i=0 ; i<NUM_CONC_HASH_BUCKETS ; i++) conc_hash_synthesize_table[i] = (cm_thing *) 0;
   for (i=0 ; i<NUM_CONC_HASH_BUCKETS ; i++) conc_hash_analyze_table[i] = (cm_thing *) 0;

   for (tabp = conc_init_table ; tabp->bigsetup != nothing ; tabp++) {

      /* For synthesize. */

      if ((tabp->elongrotallow & 0x100) == 0) {
         uint32 hash_num = ((tabp->outsetup + (5*(tabp->insetup + 5*tabp->getout_schema))) * 25) & (NUM_CONC_HASH_BUCKETS-1);
         tabp->next_synthesize = conc_hash_synthesize_table[hash_num];
         conc_hash_synthesize_table[hash_num] = tabp;
      }

      /* For analyze. */

      if ((tabp->elongrotallow & 0x200) == 0) {
         uint32 hash_num = ((tabp->bigsetup + (5*tabp->lyzer)) * 25) & (NUM_CONC_HASH_BUCKETS-1);
         tabp->next_analyze = conc_hash_analyze_table[hash_num];
         conc_hash_analyze_table[hash_num] = tabp;
      }
   }
}





Private void fix_missing_centers(setup *inners, setup *outers, setup_kind kki, setup_kind kko, int center_arity)
{
   int i;

   /* Fix up nonexistent centers, in a rather inept way. */

   for (i=0 ; i<center_arity ; i++) {
      if (inners[i].kind == nothing) {
         inners[i].kind = kki;
         inners[i].rotation = 0;
         inners[i].result_flags = outers->result_flags;
         clear_people(&inners[i]);
      }
      else if (inners[i].kind != kki)
         fail("Don't recognize concentric ending setup.");
   }

   if (outers->kind != kko)
      fail("Don't recognize concentric ending setup.");
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

   int i, j, q, k, rot;
   Const veryshort *map_indices;
   int index;
   uint32 hash_num;
   uint32 allowmask;
   cm_thing *conc_hash_bucket;
   Const cm_thing *lmap_ptr;
   setup *inners = &outer_inners[1];
   setup *outers = &outer_inners[0];
   calldef_schema table_synthesizer = synthesizer;

   clear_people(result);
   result->result_flags = get_multiple_parallel_resultflags(outer_inners, center_arity+1);

   if (inners[0].kind == nothing && outers->kind == nothing) {
      result->kind = nothing;
      return;
   }

   compute_rotation_again:

   i = (inners[0].rotation - outers->rotation) & 3;

   if (synthesizer == schema_in_out_triple_squash) {

      /* Do special stuff to put setups back properly for squashed schema. */

      if (inners[0].kind == s2x2) {
         /* Move people to the closer parts of 2x2 setups. */
         if (outer_elongation == 2) {
            if (!(inners[1].people[2].id1 | inners[1].people[3].id1)) {
               swap_people(&inners[1], 0, 3);
               swap_people(&inners[1], 1, 2);
            }
            if (!(inners[0].people[0].id1 | inners[0].people[1].id1)) {
               swap_people(&inners[0], 0, 3);
               swap_people(&inners[0], 1, 2);
            }
         }
         else if (outer_elongation == 1) {
            if (!(inners[1].people[0].id1 | inners[1].people[3].id1)) {
               swap_people(&inners[1], 0, 1);
               swap_people(&inners[1], 3, 2);
            }
            if (!(inners[0].people[2].id1 | inners[0].people[1].id1)) {
               swap_people(&inners[0], 0, 1);
               swap_people(&inners[0], 3, 2);
            }
         }

         center_arity = 2;
         table_synthesizer = schema_in_out_triple;
      }
      else if (inners[0].kind == s1x4) {
         /* Move people to the closer parts of 1x4 setups. */
         if (inners[0].rotation == 1 && outer_elongation == 2) {
            if (!(inners[0].people[0].id1 | inners[0].people[1].id1)) {
               swap_people(&inners[0], 0, 3);
               swap_people(&inners[0], 1, 2);
            }
            if (!(inners[1].people[2].id1 | inners[1].people[3].id1)) {
               swap_people(&inners[1], 0, 3);
               swap_people(&inners[1], 1, 2);
            }
         }
         else if (inners[0].rotation == 0 && outer_elongation == 1) {
            if (!(inners[0].people[2].id1 | inners[0].people[3].id1)) {
               swap_people(&inners[0], 0, 3);
               swap_people(&inners[0], 1, 2);
            }
            if (!(inners[1].people[0].id1 | inners[1].people[1].id1)) {
               swap_people(&inners[1], 0, 3);
               swap_people(&inners[1], 1, 2);
            }
         }

         center_arity = 2;
         table_synthesizer = schema_in_out_triple;
      }
      else if (inners[0].kind == s1x2 && inners[0].rotation == 0 && outer_elongation == 2) {
         setup temp = *outers;

         (void) copy_person(outers, 0, &inners[1], 0);
         (void) copy_person(outers, 1, &inners[1], 1);
         (void) copy_person(outers, 2, &inners[0], 1);
         (void) copy_person(outers, 3, &inners[0], 0);
         outers->rotation = 0;
         outers->kind = s2x2;
         inners[0] = temp;
         i = (inners[0].rotation - outers->rotation) & 3;
         center_arity = 1;
      }
      else if (inners[0].kind == s1x2 && inners[0].rotation == 1 && outer_elongation == 1) {
         setup temp = *outers;

         (void) copy_person(outers, 0, &inners[0], 0);
         (void) copy_person(outers, 1, &inners[1], 0);
         (void) copy_person(outers, 2, &inners[1], 1);
         (void) copy_person(outers, 3, &inners[0], 1);
         outers->rotation = 0;
         outers->kind = s2x2;
         inners[0] = temp;
         i = (inners[0].rotation - outers->rotation) & 3;
         center_arity = 1;
      }
      else if (inners[0].kind == nothing) {
         inners[0] = *outers;
         outers->kind = nothing;
         i = (inners[0].rotation - outers->rotation) & 3;
         center_arity = 1;
      }
      else
         fail("Can't figure out what to do.");
   }

   if (table_synthesizer == schema_conc_o) {
      if (outers->kind != s4x4)
         fail("Outsides are not on 'O' spots.");

      if (!(outers->people[5].id1 | outers->people[6].id1 | outers->people[13].id1 | outers->people[14].id1))
         outer_elongation = 1;
      else if (!(outers->people[1].id1 | outers->people[2].id1 | outers->people[9].id1 | outers->people[10].id1))
         outer_elongation = 2;
      else
         outer_elongation = 3;
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
         outers->rotation = outer_elongation-1;
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
         case s1x4:
            switch (inners[0].kind) {
               case sdmd:
                  /* Just turn it into a star. */
                  inners[0].kind = s_star;
                  canonicalize_rotation(&inners[0]);  /*Need to do this; it has 4-way symmetry now. */
                  goto compute_rotation_again;
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
                  break;
            }
            break;
      }
   }
   else if (synthesizer == schema_conc_star12) {
      if (center_arity == 2) {
         fix_missing_centers(inners, outers, s_star, s1x4, center_arity);
      }
   }
   else if (synthesizer == schema_conc_star16) {
      if (center_arity == 3) {
         fix_missing_centers(inners, outers, s_star, s1x4, center_arity);
      }
   }
   else if (synthesizer == schema_conc_12) {
      if (center_arity == 2) {
         fix_missing_centers(inners, outers, s1x6, s2x3, center_arity);
      }
      table_synthesizer = schema_concentric;
   }
   else if (synthesizer == schema_conc_16) {
      if (center_arity == 3) {
         fix_missing_centers(inners, outers, s1x8, s2x4, center_arity);
      }
      table_synthesizer = schema_concentric;
   }
   else if (synthesizer == schema_conc_bar12) {
      if (center_arity == 2) {
         fix_missing_centers(inners, outers, s_star, s2x3, center_arity);
      }
   }
   else if (synthesizer == schema_conc_bar16) {
      if (center_arity == 3) {
         fix_missing_centers(inners, outers, s_star, s2x3, center_arity);
      }
   }
   else if (synthesizer == schema_concentric_others) {
      if (center_arity == 2) {
         fix_missing_centers(inners, outers, s1x2, s2x2, center_arity);
      }
      else if (center_arity == 3) {
         fix_missing_centers(inners, outers, s1x2, s1x2, center_arity);
      }
      else
         fail("Don't recognize concentric ending setup.");
   }
   else if (synthesizer == schema_grand_single_concentric) {
      if (center_arity == 3) {
         fix_missing_centers(inners, outers, s1x2, s1x2, center_arity);
      }
   }
   else {
      /* Fix up nonexistent centers or ends, in a rather inept way. */
      if (inners[0].kind == nothing) {
         if (table_synthesizer == schema_conc_o) {
            inners[0].kind = s2x2;
            inners[0].rotation = 0;
         }
         else {
            inners[0].kind = outers->kind;
            inners[0].rotation = outers->rotation;
         }
         inners[0].result_flags = outers->result_flags;
         clear_people(&inners[0]);
         i = 0;
      }
      else if (outers->kind == nothing) {
         if (table_synthesizer == schema_conc_o) {
            outers->kind = s4x4;
            outers->rotation = 0;
            outer_elongation = 3;
         }
         else {
            outers->kind = inners[0].kind;
            outers->rotation = inners[0].rotation;
         }
         outers->result_flags = inners[0].result_flags;
         clear_people(outers);
         i = 0;
      }

      if (     table_synthesizer != schema_in_out_triple &&
               table_synthesizer != schema_in_out_quad &&
               table_synthesizer != schema_concentric_6_2_tgl &&
               table_synthesizer != schema_intlk_vertical_6 &&
               table_synthesizer != schema_intlk_lateral_6 &&
               table_synthesizer != schema_conc_o) {
         /* Nonexistent center or ends have been taken care of.  Now figure out how to put
            the setups together. */

         switch (outers->kind) {
            case sbigdmd:
               switch (inners[0].kind) {
                  case s1x2:
                     if ((outers->people[3].id1 | outers->people[9].id1)) goto anomalize_it;
                     break;
               }
               break;
            case s4x4:
               switch (inners[0].kind) {
                  case s2x2:
                     table_synthesizer = schema_conc_o;
                     goto compute_rotation_again;
               }
               break;
         }

         table_synthesizer = schema_concentric;   /* They are all in the hash table this way. */
      }
   }

   if (inners[0].kind == s_trngl) {
      /* For triangles, we use the "2" bit of the rotation, demanding that it be even. */
      if (i&1) goto elongation_loss;
      index = (i&2) >> 1;
   }
   else {
      index = i&1;
   }

   hash_num = ((outers->kind + (5*(inners[0].kind + 5*table_synthesizer))) * 25) & (NUM_CONC_HASH_BUCKETS-1);

   if (outer_elongation == 3)
      allowmask = 1 << (index + 4);
   else if (outer_elongation <= 0 || outer_elongation > 3)
      allowmask = 5 << index;
   else
      allowmask = 1 << (index + ((((outer_elongation-1) ^ outers->rotation) & 1) << 1));

   for (conc_hash_bucket = conc_hash_synthesize_table[hash_num] ; conc_hash_bucket ; conc_hash_bucket = conc_hash_bucket->next_synthesize) {
      if (     conc_hash_bucket->outsetup == outers->kind &&
               conc_hash_bucket->insetup == inners[0].kind &&
               conc_hash_bucket->center_arity == center_arity &&
               conc_hash_bucket->getout_schema == table_synthesizer &&
               (conc_hash_bucket->elongrotallow & allowmask) == 0) {
         lmap_ptr = conc_hash_bucket;
         goto gotit;
      }
   }

   goto anomalize_it;

gotit:

   if (!lmap_ptr) goto anomalize_it;

   if (lmap_ptr->center_arity != center_arity)
      fail("Confused about concentric arity.");

   /* Find out whether inners need to be flipped around. */
   q = i + lmap_ptr->inner_rot - lmap_ptr->outer_rot;

   if (q & 1) fail("Sorry, bug 1 in normalize_concentric.");

   result->kind = lmap_ptr->bigsetup;
   result->rotation = outers->rotation + lmap_ptr->outer_rot;

   map_indices = lmap_ptr->maps;

   if (table_synthesizer != schema_concentric_others) {   /* ****** This test is a crock!!!!! */
      if ((result->rotation+outer_elongation-1) & 2) {
         if (lmap_ptr->center_arity == 2) {
            setup tt = inners[0];
            inners[0] = inners[1];
            inners[1] = tt;
         }
         else if (lmap_ptr->center_arity == 3) {
            setup tt = inners[0];
            inners[0] = inners[2];
            inners[2] = tt;
         }
      }
   }

   for (k=0; k<lmap_ptr->center_arity; k++) {
      uint32 rr = lmap_ptr->inner_rot;

      /* Need to flip alternating triangles upside down. */
      if (lmap_ptr->insetup == s_trngl && (k&1)) rr ^= 2;

      if (q & 2) {
         inners[k].rotation += 2;
         canonicalize_rotation(&inners[k]);
      }

      if (k != 0) {
         if (((inners[k].rotation ^ inners[k-1].rotation ^ lmap_ptr->inner_rot ^ rr) & 3) != 0)
            fail("Sorry, bug 2 in normalize_concentric.");
      }


      for (j=0; j<lmap_ptr->inlimit; j++)
         (void) install_rot(result, *map_indices++, &inners[k], j, ((-rr) & 3) * 011);
   }

   rot = ((-lmap_ptr->outer_rot) & 3) * 011;

   for (j=0; j<lmap_ptr->outlimit; j++)
      (void) install_rot(result, *map_indices++, outers, j, rot);

   canonicalize_rotation(result);
   return;

   anomalize_it:            /* Failed, just leave it as it is. */

   switch (synthesizer) {
      case schema_rev_checkpoint:
         fail("Sorry, can't figure out this reverse checkpoint result.");
      case schema_single_concentric:
         fail("Can't figure out this single concentric result.");
      case schema_grand_single_concentric:
         fail("Can't figure out this grand single concentric result.");
      case schema_concentric:
         break;
      default:
         fail("Can't figure out this concentric result.");
   }

   if (outer_elongation <= 0 || outer_elongation > 2) goto elongation_loss;

   result->kind = s_normal_concentric;
   result->inner.skind = inners[0].kind;
   result->inner.srotation = inners[0].rotation;
   result->outer.skind = outers->kind;
   result->outer.srotation = outers->rotation;
   result->concsetup_outer_elongation = outer_elongation;
   if (outers->rotation & 1) result->concsetup_outer_elongation ^= 3;
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
   int i, k;
   analyzer_kind analyzer_index;
   uint32 hash_num;
   cm_thing *conc_hash_bucket;
   Const cm_thing *lmap_ptr;

   *outer_elongation = 1;   /*  **** shouldn't these be -1???? */
   *xconc_elongation = 1;
   clear_people(outers);
   clear_people(&inners[0]);

   /* First, translate the analyzer into a form that encodes only what we need to know. */

   switch (analyzer) {
      case schema_lateral_6:
         analyzer_index = analyzer_LATERAL6; break;
      case schema_vertical_6:
         analyzer_index = analyzer_VERTICAL6; break;
      case schema_intlk_lateral_6:
         analyzer_index = analyzer_INTLK_LATERAL6; break;
      case schema_intlk_vertical_6:
         analyzer_index = analyzer_INTLK_VERTICAL6; break;
      case schema_checkpoint:
      case schema_cross_checkpoint:
      case schema_ckpt_star:
         analyzer_index = analyzer_CHECKPT; break;
      case schema_single_concentric:
      case schema_single_cross_concentric:
         analyzer_index = analyzer_SINGLE; break;
      case schema_grand_single_concentric:
      case schema_grand_single_cross_concentric:
         analyzer_index = analyzer_GRANDSINGLE; break;
      case schema_conc_o:
         analyzer_index = analyzer_O;
         if (ss->kind == s_c1phan) do_matrix_expansion(ss, CONCPROP__NEEDK_4X4, FALSE);
         break;
      case schema_conc_12:
         analyzer_index = analyzer_CONC12; break;
      case schema_conc_16:
         analyzer_index = analyzer_CONC16; break;
      case schema_conc_star12:
         analyzer_index = analyzer_STAR12; break;
      case schema_conc_star16:
         analyzer_index = analyzer_STAR16; break;
      case schema_conc_bar:
         analyzer_index = analyzer_BAR; break;
      case schema_conc_bar12:
         analyzer_index = analyzer_BAR12; break;
      case schema_conc_bar16:
         analyzer_index = analyzer_BAR16; break;
      case schema_3x3_concentric:
         analyzer_index = analyzer_3X3_CONC; break;
      case schema_4x4_lines_concentric:
         analyzer_index = analyzer_4X4_LINES_CONC; break;
      case schema_4x4_cols_concentric:
         analyzer_index = analyzer_4X4_COLS_CONC; break;
      case schema_concentric_diamond_line:
         analyzer_index = analyzer_DIAMOND_LINE; break;
      case schema_concentric_6_2:
         analyzer_index = analyzer_6X2; break;
      case schema_concentric_6p:
         analyzer_index = analyzer_6P; break;
      case schema_concentric_others:
         analyzer_index = analyzer_OTHERS; break;
      case schema_concentric_6_2_tgl:
         analyzer_index = analyzer_6X2_TGL; break;
      case schema_concentric_diamonds:
      case schema_cross_concentric_diamonds:
         analyzer_index = analyzer_CONC_DIAMONDS; break;
      case schema_concentric_2_6:
         if (  (ss->kind == s3x4 &&
                     (       (ss->people[1].id1 | ss->people[2].id1 | ss->people[7].id1 | ss->people[8].id1) ||
                           (!(ss->people[0].id1 & ss->people[3].id1 & ss->people[4].id1 & ss->people[5].id1 &
                              ss->people[6].id1 & ss->people[9].id1 & ss->people[10].id1 & ss->people[11].id1)))) ||
               (ss->kind == s3dmd &&
                     (       (ss->people[9].id1 | ss->people[3].id1 | ss->people[4].id1 | ss->people[10].id1) ||
                           (!(ss->people[0].id1 & ss->people[2].id1 & ss->people[7].id1 & ss->people[5].id1 &
                              ss->people[6].id1 & ss->people[1].id1 & ss->people[8].id1 & ss->people[11].id1)))))
            fail("Can't find centers and ends in this formation.");
         analyzer_index = analyzer_2X6;
         break;
      case schema_concentric_2_6_or_2_4:
         if (setup_attrs[ss->kind].setup_limits == 5) {
            analyzer_index = analyzer_2X4;
            warn(warn__unusual);
         }
         else
            analyzer_index = analyzer_2X6;
         break;
      case schema_in_out_triple:
      case schema_in_out_triple_squash:
         analyzer_index = analyzer_TRIPLE_LINE; break;
      case schema_in_out_quad:
         analyzer_index = analyzer_QUAD_LINE; break;
      case schema_rev_checkpoint:
      case schema_concentric:
      case schema_conc_star:
      case schema_cross_concentric:
      case schema_concentric_to_outer_diamond:
         if (  (ss->kind == s4x4 &&
                     (       (ss->people[1].id1 | ss->people[2].id1 | ss->people[5].id1 | ss->people[6].id1 |
                              ss->people[9].id1 | ss->people[10].id1 | ss->people[13].id1 | ss->people[14].id1) ||
                           (!(ss->people[0].id1 & ss->people[3].id1 & ss->people[4].id1 & ss->people[7].id1 &
                              ss->people[8].id1 & ss->people[11].id1 & ss->people[12].id1 & ss->people[15].id1)))))
            fail("Can't find centers and ends in this formation.");
         analyzer_index = analyzer_NORMAL;
         break;
      case schema_single_concentric_together:
         if (ss->kind == s1x8 || ss->kind == s_ptpd || setup_attrs[ss->kind].setup_limits == 3)
            analyzer_index = analyzer_SINGLE;
         else
            analyzer_index = analyzer_NORMAL;
         break;
      case schema_concentric_or_diamond_line:
         if (ss->kind == s3x1dmd)
            analyzer_index = analyzer_DIAMOND_LINE;
         else
            analyzer_index = analyzer_NORMAL;
         break;
      case schema_concentric_6p_or_normal:
      case schema_cross_concentric_6p_or_normal:
         if (setup_attrs[ss->kind].setup_limits == 5)
            analyzer_index = analyzer_6P;
         else
            analyzer_index = analyzer_NORMAL;
         break;
      case schema_concentric_6p_or_sgltogether:
         if (setup_attrs[ss->kind].setup_limits == 5)
            analyzer_index = analyzer_6P;
         else if (ss->kind == s1x8 || ss->kind == s_ptpd || setup_attrs[ss->kind].setup_limits == 3)
            analyzer_index = analyzer_SINGLE;
         else
            analyzer_index = analyzer_NORMAL;
         break;
      case schema_1331_concentric:
         if (ss->kind == s_qtag || ss->kind == s_spindle || ss->kind == s3x1dmd)
            analyzer_index = analyzer_3X1_CONC;
         else if ((ss->kind == s3x4 && !(ss->people[0].id1 | ss->people[3].id1 | ss->people[6].id1 | ss->people[9].id1))) {
            analyzer_index = analyzer_3X1_CONC;
            /* Compress to a 1/4 tag. */
            ss->kind = s_qtag;
            swap_people(ss, 1, 0);
            swap_people(ss, 2, 1);
            swap_people(ss, 4, 2);
            swap_people(ss, 5, 3);
            swap_people(ss, 7, 4);
            swap_people(ss, 8, 5);
            swap_people(ss, 10, 6);
            swap_people(ss, 11, 7);
         }
         else
            analyzer_index = analyzer_1X3_CONC;
         break;
      case schema_maybe_matrix_single_concentric_together:
      case schema_maybe_single_concentric:
      case schema_maybe_single_cross_concentric:
      case schema_maybe_grand_single_concentric:
      case schema_maybe_grand_single_cross_concentric:
      case schema_maybe_special_single_concentric:
      case schema_maybe_nxn_lines_concentric:
      case schema_maybe_nxn_cols_concentric:
      case schema_maybe_matrix_conc_star:
      case schema_maybe_matrix_conc_bar:
      case schema_maybe_nxn_1331_lines_concentric:
      case schema_maybe_nxn_1331_cols_concentric:
         fail("Internal error: can't figure out how to specialize concentric.");
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

   if (ss->kind == s_normal_concentric || ss->kind == s_dead_concentric) {
      *center_arity = 1;

      if (ss->kind == s_dead_concentric) {
         ss->outer.skind = nothing;
         ss->outer.srotation = 0;
      }

      outers->rotation = ss->outer.srotation;
      inners[0].rotation = ss->outer.srotation;   /* Yes, this looks wrong, but it isn't. */

      switch (analyzer_index) {
#ifdef NEVERRRR
         case analyzer_DIAMOND_LINE:
            /* **** I don't think we need this any more. */
            if (ss->inner.skind == sdmd && ss->inner.srotation == ss->outer.srotation) {
               static Const veryshort map321[4] = {13, 0, 15, 2};
               static Const veryshort map123[4] = {12, 1, 14, 3};

               inners[0].kind = s1x4;
               outers->kind = sdmd;
               gather(&inners[0], ss, map321, 3, 0);
               gather(outers, ss, map123, 3, 0);

               if (ss->outer.skind == s1x4) {
                  *outer_elongation = (ss->outer.srotation & 1) + 1;
                  goto finish;
               }
            }
            break;
#endif
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

/* ***** Claim these two won't happen, because of dhrglass and 1x3dmd! */

               /* We allow a diamond inside a box with wrong elongation (if elongation were good, it would be an hourglass.) */
               if (ss->outer.skind == s2x2) {
                  *outer_elongation = ss->concsetup_outer_elongation;
                  if (ss->outer.srotation & 1) *outer_elongation ^= 3;
                  goto finish;
               }
               /* And a diamond inside a line with wrong elongation (if elongation were good, it would be a 3x1 diamond.) */
               if (ss->outer.skind == s1x4) {
                  *outer_elongation = (ss->outer.srotation & 1) + 1;
                  goto finish;
               }
            }
            else if (ss->inner.skind == s1x2 && ss->outer.skind == s1x6 && ss->inner.srotation != ss->outer.srotation) {
               static Const veryshort map44[4] = {12, 13, 15, 16};

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
               gather(outers, ss, map44, 3, 0);
               goto finish;
            }
            break;
      }
   }

   /* Next, do the 3x4 -> qtag fudging.  Don't ask permission, just do it. **** maybe that isn't right for Z calls. */

   if (analyzer_index == analyzer_NORMAL && ss->kind == s3x4) {
      *center_arity = 1;
      inners[0].kind = s1x4;
      inners[0].rotation = 0;
      outers->kind = s2x2;
      outers->rotation = 0;
      *outer_elongation = ((~outers->rotation) & 1) + 1;
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

   if ((ss->kind == s4x4 || ss->kind == s4dmd) && (analyzer_index == analyzer_CONC16 || analyzer_index == analyzer_STAR16 || analyzer_index == analyzer_BAR16))
      analyzer_index = analyzer_4X4_COLS_CONC;

   if ((ss->kind == s3x4 || ss->kind == s3dmd) && (analyzer_index == analyzer_CONC12 || analyzer_index == analyzer_STAR12 || analyzer_index == analyzer_BAR12))
      analyzer_index = analyzer_3X3_CONC;

   if (analyzer_index == analyzer_4X4_COLS_CONC || analyzer_index == analyzer_4X4_LINES_CONC) {

      /* There is a minor kludge in the tables.  For "4x4_cols" and "4x4_lines", the tables don't
         know anything about people's facing direction, so the two analyzers are used to split vertically
         or laterally.  Therefore, we translate, based on facing direction. */

      if (ss->kind == s4x4) {
         uint32 tbone =    ss->people[1].id1 | ss->people[2].id1 | ss->people[5].id1 | ss->people[6].id1 |
                           ss->people[9].id1 | ss->people[10].id1 | ss->people[13].id1 | ss->people[14].id1 |
                           ss->people[3].id1 | ss->people[7].id1 | ss->people[11].id1 | ss->people[15].id1;
         if ((tbone & 011) == 011) fail("Can't do this from T-bone setup.");
         if (tbone & 1)
            analyzer_index = (analyzer_index == analyzer_4X4_COLS_CONC) ? analyzer_4X4_LINES_CONC : analyzer_4X4_COLS_CONC;
      }
   }

/* ***** More stuff we need:  if analyzer = 3x1_cols or 3x1_lines, maybe check center 6 of qtag and spindle.
    Or maybe this is all a crock. */

#ifdef NOTANYMORE
   if (ss->kind == s3x4) {
      else if ( 
                  (ss->people[1].id1 & ss->people[2].id1 & ss->people[5].id1 & ss->people[7].id1 & ss->people[8].id1 & ss->people[11].id1)) {
         /* If a 3x4 has the center 2x3 fully occupied, we recognize "center 6 / outer 2".
            We do this by turning it into the schema that can find the center and outer 2x3's */
/* BUG!!! be sure that selective_move won't do this if the outer two are told to do anything!
The right way to do this is to have selective_move set the schema to "3x3_lines_cols_conc", so we do nothing here. */
         analyzer_index = analyzer_3X3_CONC;
      }
   }
#endif

   if (analyzer_index == analyzer_3X1_CONC)
      analyzer_index = analyzer_6X2;
   else if (analyzer_index == analyzer_1X3_CONC)
      analyzer_index = analyzer_2X6;

   hash_num = ((ss->kind + (5*analyzer_index)) * 25) & (NUM_CONC_HASH_BUCKETS-1);

   for (conc_hash_bucket = conc_hash_analyze_table[hash_num] ; conc_hash_bucket ; conc_hash_bucket = conc_hash_bucket->next_analyze) {
      if (     conc_hash_bucket->bigsetup == ss->kind &&
               conc_hash_bucket->lyzer == analyzer_index) {
         lmap_ptr = conc_hash_bucket;
         goto gotit;
      }
   }

   fail(conc_error_messages[analyzer_index]);

gotit:

   *center_arity = lmap_ptr->center_arity;

   outers->kind = lmap_ptr->outsetup;
   outers->rotation = (-lmap_ptr->outer_rot) & 3;

   gather(outers, ss, &lmap_ptr->maps[lmap_ptr->inlimit*lmap_ptr->center_arity], lmap_ptr->outlimit-1, lmap_ptr->outer_rot * 011);

   for (k=0; k<lmap_ptr->center_arity; k++) {
      uint32 rr = lmap_ptr->inner_rot;

      /* Need to flip alternating triangles upside down. */
      if (lmap_ptr->insetup == s_trngl && (k&1)) rr ^= 2;

      clear_people(&inners[k]);
      inners[k].kind = lmap_ptr->insetup;
      inners[k].rotation = (-rr) & 3;

      gather(&inners[k], ss, &lmap_ptr->maps[k*lmap_ptr->inlimit], lmap_ptr->inlimit-1, rr * 011);
   }

   /* Set the outer elongation to whatever elongation the outsides really had, as indicated
      by the map. */

   *outer_elongation = lmap_ptr->mapelong;
   if (outers->rotation & 1) *outer_elongation ^= 3;

   /* If the concept is cross-concentric, we have to set the elongation to what
         the centers (who will, of course, be going to the outside) had.
      If the original centers are in a 2x2, we set it according to the orientation
         of the entire 2x4 they were in, so that they can think about whether they were
         in lines or columns and act accordingly.  If they were not in a 2x4, that is,
         the setup was a wing or galaxy, we set the elongation to -1 to indicate an
         error.  In such a case the centers won't be able to decide whether they were
         in lines or columns. */

   /* The following additional comment used to be present, back when we obeyed a misguided
      notion of what cross concentric means:
      "But if the outsides started in a 1x4, they override the centers' own axis." */

   if (analyzer == schema_cross_concentric || analyzer == schema_cross_concentric_diamonds || analyzer == schema_cross_concentric_6p_or_normal) {
      *xconc_elongation = lmap_ptr->inner_rot+1;
/*  and this line used to implement that misguided notion:
      if (lmap_ptr->outsetup == s1x4) *xconc_elongation = lmap_ptr->outer_rot+1;
*/
      switch (ss->kind) {
         case s_galaxy:
         case s_rigger:
            *xconc_elongation = -1;    /* Can't do this! */
            break;
      }
   }
   else if (analyzer == schema_in_out_triple || analyzer == schema_in_out_triple_squash || analyzer == schema_in_out_quad || analyzer == schema_conc_o) {
      *outer_elongation = lmap_ptr->mapelong;            /* The map defines it completely. */
   }
   else if (analyzer == schema_concentric_6_2_tgl) {
      if (inners[0].kind == s_bone6)
         *outer_elongation = ((~outers->rotation) & 1) + 1;
   }
   else if (analyzer == schema_concentric_6_2) {
      if (inners[0].kind == s_bone6)
         *outer_elongation = (outers->rotation & 1) + 1;
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
   uint32 modifiersin1,
   uint32 modifiersout1,
   long_boolean recompute_id,
   setup *result)
{
   uint32 localmods1, localmodsin1, localmodsout1;
   setup begin_inner[3];
   setup begin_outer;
   int begin_outer_elongation;
   int begin_xconc_elongation;
   int final_elongation;
   int center_arity;
   long_boolean inverting = FALSE;
   setup result_inner[3];
   setup result_outer;
   setup outer_inners[4];
   int i, k, klast;
   int crossing;       /* This is an int (0 or 1) rather than a long_boolean, because we will index with it. */

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

   uint32 saved_number_fields = current_options.number_fields;

   uint32 snagflag = ss->cmd.cmd_misc2_flags;

   uint32 save_cmd_misc2_flags = ss->cmd.cmd_misc2_flags;
   parse_block *save_skippable = ss->cmd.skippable_concept;

   ss->cmd.cmd_misc2_flags &= ~(0xFFFF | CMD_MISC2__CTR_USE_INVERT | CMD_MISC2__CTR_USE);
   ss->cmd.skippable_concept = (parse_block *) 0;

   /* It is clearly too late to expand the matrix -- that can't be what is wanted. */
   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

   /* But, if we thought we weren't sure enough of where people were to allow stepping
      to a wave, we are once again sure. */
   ss->cmd.cmd_misc_flags &= ~CMD_MISC__NO_STEP_TO_WAVE;

   if (ss->kind == s_qtag &&
            (  ss->cmd.cmd_final_flags.herit &
               (INHERITFLAG_12_MATRIX|INHERITFLAG_1X3|INHERITFLAG_3X1|INHERITFLAG_3X3)))
      do_matrix_expansion(ss, CONCPROP__NEEDK_3X4, TRUE);

   /* We allow "quick so-and-so shove off"! */

   if (     analyzer != schema_in_out_triple_squash &&
            analyzer != schema_in_out_triple &&
            analyzer != schema_in_out_quad)
      ss->cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;

   for (i=0; i<32; i++) {
      orig_inners_start_directions[i] =
      orig_outers_start_directions[i] =
      final_outers_finish_directions[i] = 0;
   }

   localmodsin1 = modifiersin1;
   localmodsout1 = modifiersout1;

   /* But reverse them if doing "invert". */
   if (snagflag & CMD_MISC2__CTR_END_INVERT) {
      inverting = TRUE;
      localmodsin1 = modifiersout1;
      localmodsout1 = modifiersin1;
   }

   if (snagflag & CMD_MISC2__CTR_END_MASK) {
      switch (snagflag & CMD_MISC2__CTR_END_KMASK) {
         case CMD_MISC2__CENTRAL_SNAG:
            if (ss->cmd.cmd_frac_flags != CMD_FRAC_NULL_VALUE)
               fail("Can't do fractional \"snag\".");

            /* FALL THROUGH!!!! */
         case 0:
         case CMD_MISC2__CENTRAL_MYSTIC:
            ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__CTR_END_MASK;
            break;
      }
   }

   begin_inner[0].cmd = ss->cmd;
   begin_inner[1].cmd = ss->cmd;
   begin_inner[2].cmd = ss->cmd;
   begin_outer.cmd = ss->cmd;

   crossing =  (analyzer == schema_cross_concentric) ||
               (analyzer == schema_single_cross_concentric) ||
               (analyzer == schema_grand_single_cross_concentric) ||
               (analyzer == schema_cross_checkpoint) ||
               (analyzer == schema_cross_concentric_6p_or_normal) ||
               (analyzer == schema_cross_concentric_diamonds);

   concentrify(ss, analyzer, begin_inner, &begin_outer, &center_arity, &begin_outer_elongation, &begin_xconc_elongation);

   /* We don't need the crossing info any more. */

   if (analyzer == schema_cross_concentric)
      analyzer = schema_concentric;
   else if (analyzer == schema_single_cross_concentric)
      analyzer = schema_single_concentric;
   else if (analyzer == schema_grand_single_cross_concentric)
      analyzer = schema_grand_single_concentric;
   else if (analyzer == schema_cross_concentric_6p_or_normal)
      analyzer = schema_concentric_6p_or_normal;
   else if (analyzer == schema_cross_concentric_diamonds)
      analyzer = schema_concentric_diamonds;
   else if (analyzer == schema_cross_checkpoint)
      analyzer = schema_checkpoint;

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

      if (analyzer == schema_grand_single_concentric && center_arity == 3) {
         temptemp = begin_inner[2];
         begin_inner[2] = begin_inner[1];
         begin_inner[1] = temptemp;
      }

      final_outers_start_kind = orig_inners_start_kind;
      final_outers_start_directions = orig_inners_start_directions;
   }
   else {
      final_outers_start_kind = orig_outers_start_kind;
      final_outers_start_directions = orig_outers_start_directions;
   }

   /* If the call turns out to be "detour", this will make it do just the ends part. */

   if (     analyzer != schema_in_out_triple_squash &&
            analyzer != schema_in_out_triple &&
            analyzer != schema_in_out_quad)
      begin_outer.cmd.cmd_misc_flags |= CMD_MISC__DOING_ENDS;

   /* There are two special pieces of information we now have that will help us decide where to
      put the outsides.  "Orig_outers_start_kind" tells what setup the outsides were originally in,
      and "begin_outer_elongation" tells how the outsides were oriented (1=horiz, 2=vert).
      "begin_outer_elongation" refers to absolute orientation, that is, "our" view of the
      setups, taking all rotations into account.  "final_outers_start_directions" gives the individual
      orientations (absolute) of the people who are finishing on the outside.  Later, we will compute
      "final_outers_finish_dirs", telling how the individual people were oriented.  How we use all this
      information depends on many things that we will attend to below. */

   /* Giving one of the concept descriptor pointers as nil indicates that we don't want those people to do anything. */

   /* We will now do the parts, controlled by the counter k.  There are multiple instances of doing the centers'
      parts, indicated by k = 0, 1, ..... center_arity-1.  (Normally, center_arity=1, so we have just k=0).
      There is just one instance of doing the ends' part, indicated by k not in [0..center_arity-1].

      Now this is made complicated by the fact that we sometimes want to do things in a different order.
      Usually, we want to do the centers first and the ends later, but, when the schema is triple lines,
      we want to do the ends first and the centers later.  This has to do with the order in which we
      query the user for "slant <anything> and <anything>".  So, in the normal case, we let k run from
      0 to center_arity, inclusive, with the final value for the ends.  When the schema is triple lines,
      we let k run from zero to center_arity-1, with zero for the ends. */

   k = 0;
   klast = center_arity+1;

   if (     analyzer == schema_in_out_triple_squash ||
            analyzer == schema_in_out_triple ||
            analyzer == schema_in_out_quad) {
      k = -1;
      klast = center_arity;
   }

   for (; k<klast; k++) {
      uint32 mystictest;
      parse_block f1, f2;
      setup *begin_ptr;
      setup *result_ptr;
      uint32 modifiers1;
      uint32 ctr_use_flag;
      setup_command *cmdptr;
#ifdef PEELCHAINTHRUFAILS
      uint32 check;
#endif

      long_boolean doing_ends = (k<0) || (k==center_arity);

      begin_ptr = doing_ends ? &begin_outer : &begin_inner[k];
      result_ptr = doing_ends ? &result_outer : &result_inner[k];
      modifiers1 = doing_ends ? localmodsout1 : localmodsin1;
      cmdptr = (doing_ends ^ inverting) ? cmdout : cmdin;

      ctr_use_flag = doing_ends ? (CMD_MISC2__CTR_USE|CMD_MISC2__CTR_USE_INVERT) : CMD_MISC2__CTR_USE;

#ifdef PEELCHAINTHRUFAILS
      check = 0;    /* See if anyone is present. */
      for (i=0; i<=setup_attrs[begin_ptr->kind].setup_limits; i++) check |= begin_ptr->people[i].id1;

      if (check == 0) {
         result_ptr->kind = nothing;
         result_ptr->result_flags = 0;

         if (doing_ends)
            localmodsout1 |= DFM1_CONC_FORCE_SPOTS;      /* ????? */
      }
      else
#endif
      if (cmdptr) {
         begin_ptr->cmd.parseptr = cmdptr->parseptr;
         begin_ptr->cmd.callspec = cmdptr->callspec;
         begin_ptr->cmd.cmd_final_flags = cmdptr->cmd_final_flags;

         /* If doing something under a "3x1" (or "1x3") concentric schema,
            put the "3x3" flag into the 6-person call, whichever call that is,
            and "single" into the other one. */
         if (analyzer == schema_1331_concentric) {
            if (setup_attrs[begin_ptr->kind].setup_limits == 5)
               begin_ptr->cmd.cmd_final_flags.herit |= INHERITFLAG_3X3;
            else
               begin_ptr->cmd.cmd_final_flags.herit |= INHERITFLAG_SINGLE;
         }

         if (doing_ends) {

            /* We do this to allow unesthetic things like "invert acey deucey" from waves.
               This suppresses the error message that would otherwise occur when the ends do
               a centers part that turns out to be messy because of the elongation.  We are
               less than completely happy about this, but it seems to be acceptable usage. */

/*
            if (inverting)
               begin_ptr->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
*/



            /* If the operation isn't being done with "DFM1_CONC_CONCENTRIC_RULES" (that is, this is
               some implicit operation), we allow the user to give the explicit "concentric" concept.
               We respond to that concept simply by turning on "DFM1_CONC_CONCENTRIC_RULES".  This makes
               things like "shuttle [concentric turn to a line]" work. */

            if (!inverting && !begin_ptr->cmd.callspec && !(localmodsout1 & DFM1_CONC_CONCENTRIC_RULES)) {
               uint64 junk_concepts;
               parse_block *next_parseptr = process_final_concepts(begin_ptr->cmd.parseptr, FALSE, &junk_concepts);

               if (!junk_concepts.herit && !junk_concepts.final && next_parseptr->concept->kind == concept_concentric) {
                  localmodsout1 |= DFM1_CONC_CONCENTRIC_RULES;
                  begin_ptr->cmd.parseptr = next_parseptr->next;
               }
            }




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

            if (     analyzer != schema_in_out_triple_squash &&
                     analyzer != schema_in_out_triple &&
                     analyzer != schema_in_out_quad &&
                     analyzer != schema_conc_o &&
                     analyzer != schema_rev_checkpoint) {
               if ((begin_ptr->kind == s2x2 || begin_ptr->kind == s_short6) &&
                        begin_outer_elongation > 0) {      /* We demand elongation be 1 or more. */
                  begin_ptr->cmd.prior_elongation_bits = begin_outer_elongation;

                  /* If "demand lines" or "demand columns" has been given, we suppress elongation
                     checking.  In that case, the database author knows what elongation is required
                     and is taking responsibility for it.  This is what makes "scamper" and "divvy up" work.
                     We also do this if the concept is cross concentric.  In that case the people doing the
                     "ends" call actually did it in the center (the way they were taught in C2 class)
                     before moving to the outside. */

                  if (     ((DFM1_CONC_CONCENTRIC_RULES | DFM1_CONC_DEMAND_LINES | DFM1_CONC_DEMAND_COLUMNS) & localmodsout1) ||
                           crossing ||
                           analyzer == schema_checkpoint)
                     begin_ptr->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
               }
               else if (begin_ptr->kind == s1x4 || begin_ptr->kind == s1x6) {
                  begin_ptr->cmd.prior_elongation_bits = 0x40;     /* Indicate that these people are working around the outside. */

                  if (     (DFM1_CONC_CONCENTRIC_RULES & localmodsout1) ||
                           crossing ||
                           analyzer == schema_checkpoint)
                     begin_ptr->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
               }
            }
         }

         if (modifiers1 & DFM1_NUM_INSERT_MASK) {
            current_options.number_fields <<= 4;
            current_options.number_fields += (modifiers1 & DFM1_NUM_INSERT_MASK) / DFM1_NUM_INSERT_BIT;
            current_options.howmanynumbers++;
         }

         current_options.number_fields >>= ((DFM1_NUM_SHIFT_MASK & modifiers1) / DFM1_NUM_SHIFT_BIT) * 4;
         current_options.howmanynumbers -= ((DFM1_NUM_SHIFT_MASK & modifiers1) / DFM1_NUM_SHIFT_BIT);

         if (recompute_id && !(snagflag & CMD_MISC2__CTR_END_KMASK)) update_id_bits(begin_ptr);

         /* Inherit certain assumptions to the child setups.  This is EXTREMELY incomplete. */

         {
            if (analyzer == schema_concentric) {
               if (ss->kind == s2x4 && begin_ptr->kind == s2x2) {
                  if (begin_ptr->cmd.cmd_assume.assumption == cr_wave_only) {   /* Waves [wv/0/0] or normal columns [wv/1/0] go to normal boxes [wv/0/0]. */
                     begin_ptr->cmd.cmd_assume.assump_col = 0;
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_magic_only) {
                     if (begin_ptr->cmd.cmd_assume.assump_col == 0) {
                        begin_ptr->cmd.cmd_assume.assumption = cr_couples_only;       /* Inv lines [mag/0/0] go to couples_only [cpl/0/0]. */
                     }
                     else {
                        begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;          /* Magic cols [mag/1/0] go to normal boxes [wv/0/0]. */
                        begin_ptr->cmd.cmd_assume.assump_col = 0;
                     }
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_2fl_only) {
                     if (begin_ptr->cmd.cmd_assume.assump_col == 0) {
                        begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;          /* 2FL [2fl/0/0] go to normal boxes [wv/0/0]. */
                     }
                     else {
                        begin_ptr->cmd.cmd_assume.assumption = cr_li_lo;              /* DPT/CDPT [2fl/1/x] go to facing/btb boxes [lilo/0/x]. */
                        begin_ptr->cmd.cmd_assume.assump_col = 0;
                     }
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_li_lo) {
                     if (begin_ptr->cmd.cmd_assume.assump_col == 0) {
                                                                                      /* facing/btb lines [lilo/0/x] go to facing/btb boxes [lilo/0/x]. */
                     }
                     else {
                        begin_ptr->cmd.cmd_assume.assumption = cr_li_lo;              /* 8ch/tby [lilo/1/x] go to facing/btb boxes [lilo/0/y], */
                        begin_ptr->cmd.cmd_assume.assump_col = 0;                     /* Where calculation of whether facing or back-to-back is complicated. */
                        if (!(doing_ends ^ crossing))
                           begin_ptr->cmd.cmd_assume.assump_both ^= 3;
                     }
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_1fl_only) {
                     if (begin_ptr->cmd.cmd_assume.assump_col == 0) {
                        begin_ptr->cmd.cmd_assume.assumption = cr_couples_only;       /* 1-faced lines [1fl/0/0] go to couples_only [cpl/0/0]. */
                        goto got_new_assumption;
                     }
                  }
               }
               else if (ss->kind == s_qtag && begin_ptr->kind == s2x2 && begin_ptr->cmd.cmd_assume.assump_col == 0) {
                  if (      begin_ptr->cmd.cmd_assume.assumption == cr_jright ||
                            begin_ptr->cmd.cmd_assume.assumption == cr_jleft ||
                            begin_ptr->cmd.cmd_assume.assumption == cr_ijright ||
                            begin_ptr->cmd.cmd_assume.assumption == cr_ijleft) {
                     begin_ptr->cmd.cmd_assume.assumption = cr_li_lo;       /* 1/4 tag or line [whatever/0/2] go to facing in [lilo/0/1]. */
                     begin_ptr->cmd.cmd_assume.assump_both ^= 3;            /* 3/4 tag or line [whatever/0/1] go to facing out [lilo/0/2]. */
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_ctr_miniwaves ||
                           begin_ptr->cmd.cmd_assume.assumption == cr_ctr_couples) {
                     /* Either of those speciall assumptions means that the outsides are in a normal box. */
                     begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;
                     begin_ptr->cmd.cmd_assume.assump_col = 0;
                     begin_ptr->cmd.cmd_assume.assump_both = 0;
                     goto got_new_assumption;
                  }
               }
               else if (ss->kind == s_qtag && begin_ptr->kind == s1x4) {
                  if (begin_ptr->cmd.cmd_assume.assumption == cr_ctr_miniwaves) {
                     begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;
                     begin_ptr->cmd.cmd_assume.assump_col = 0;
                     begin_ptr->cmd.cmd_assume.assump_both = 0;
                     goto got_new_assumption;
                  }
                  else if (begin_ptr->cmd.cmd_assume.assumption == cr_ctr_couples) {
                     begin_ptr->cmd.cmd_assume.assumption = cr_2fl_only;
                     begin_ptr->cmd.cmd_assume.assump_col = 0;
                     begin_ptr->cmd.cmd_assume.assump_both = 0;
                     goto got_new_assumption;
                  }
               }
            }
            else if (analyzer == schema_concentric_2_6) {
               if (ss->kind == s_qtag && begin_ptr->kind == s_short6 && (doing_ends ^ crossing)) {
                  goto got_new_assumption;    /* We want to preserve "assume diamond" stuff to the outer 6, so 6x2 acey deucey will work. */
               }
            }
            else if (      analyzer == schema_ckpt_star &&
                           doing_ends == 1 &&
                           ss->kind == s_spindle &&
                           begin_ptr->cmd.cmd_assume.assumption == cr_ckpt_miniwaves) {
               begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;       /* The box is a real box.  This makes the hinge win on chain reaction. */
               goto got_new_assumption;
            }
            else if (analyzer == schema_single_concentric) {
               if (ss->kind == s1x4 && begin_ptr->kind == s1x2 && (begin_ptr->cmd.cmd_assume.assumption == cr_2fl_only || begin_ptr->cmd.cmd_assume.assumption == cr_wave_only)) {
                  begin_ptr->cmd.cmd_assume.assumption = cr_wave_only;
                  goto got_new_assumption;
               }
            }

            begin_ptr->cmd.cmd_assume.assumption = cr_none;

            got_new_assumption: ;
         }

         /* This call to "move" will fill in good stuff (viz. the DFM1_CONCENTRICITY_FLAG_MASK)
            into begin_ptr->cmd.cmd_misc_flags, which we will use below to do various "force_lines",
            "demand_columns", etc. things. */

         if (doing_ends) {
            /* If cross concentric, we are looking for plain "mystic" */
            mystictest = crossing ? CMD_MISC2__CENTRAL_MYSTIC : (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__CTR_END_INV_CONC);

            /* Handle "invert snag" for ends. */
            if ((snagflag & (CMD_MISC2__CTR_END_KMASK | CMD_MISC2__CTR_END_INV_CONC)) == (CMD_MISC2__CENTRAL_SNAG | CMD_MISC2__CTR_END_INV_CONC)) {
               if (mystictest == CMD_MISC2__CENTRAL_MYSTIC)
                  fail("Can't do \"central/snag/mystic\" with this call.");
               begin_ptr->cmd.cmd_frac_flags = CMD_FRAC_HALF_VALUE;
            }

            /* This makes it not normalize the setup between parts -- the 4x4 stays around. */
            if (analyzer == schema_conc_o)
               begin_ptr->cmd.cmd_misc_flags |= CMD_MISC__EXPLICIT_MATRIX;
         }
         else {
            /* If cross concentric, we are looking for "invert mystic" */

            mystictest = crossing ? (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__CTR_END_INV_CONC) : CMD_MISC2__CENTRAL_MYSTIC;

            /* Handle "snag" for centers. */
            if ((snagflag & (CMD_MISC2__CTR_END_KMASK | CMD_MISC2__CTR_END_INV_CONC)) == CMD_MISC2__CENTRAL_SNAG) {
               if (mystictest == (CMD_MISC2__CENTRAL_MYSTIC | CMD_MISC2__CTR_END_INV_CONC))
                  fail("Can't do \"central/snag/mystic\" with this call.");
               begin_ptr->cmd.cmd_frac_flags = CMD_FRAC_HALF_VALUE;
            }
         }

         /* Handle "invert mystic" for ends, or "mystic" for centers. */

         if ((snagflag & (CMD_MISC2__CTR_END_KMASK | CMD_MISC2__CTR_END_INV_CONC)) == mystictest) {
            mirror_this(begin_ptr);
            begin_ptr->cmd.cmd_misc_flags ^= CMD_MISC__EXPLICIT_MIRROR;
         }

         if ((save_cmd_misc2_flags & (CMD_MISC2__CTR_USE|CMD_MISC2__CTR_USE_INVERT)) == ctr_use_flag) {
            if (!save_skippable) fail("Internal error in centers/ends work, please report this.");

            if (begin_ptr->cmd.callspec) {
               f1 = *save_skippable;
               f1.next = &f2;

               f2.concept = &mark_end_of_list;
               f2.call = begin_ptr->cmd.callspec;
               f2.next = (parse_block *) 0;
               f2.subsidiary_root = (parse_block *) 0;
               f2.gc_ptr = (parse_block *) 0;
               f2.options = current_options;
               f2.options.tagger = -1;
               f2.options.circcer = -1;
               f2.no_check_call_level = 1;
               begin_ptr->cmd.callspec = (callspec_block *) 0;
               begin_ptr->cmd.parseptr = &f1;
            }
            else {
               fail("No callspec, centers/ends!!!!!!");
            }
         }

         impose_assumption_and_move(begin_ptr, result_ptr);

         if ((snagflag & (CMD_MISC2__CTR_END_KMASK | CMD_MISC2__CTR_END_INV_CONC)) == mystictest)
            mirror_this(result_ptr);

         current_options.number_fields = saved_number_fields;
      }
      else {
         *result_ptr = *begin_ptr;

         if (doing_ends) {
            if (begin_outer_elongation <= 0 || begin_outer_elongation > 2)
               result_ptr->result_flags = 0;   /* Outer people have unknown elongation and aren't moving.  Not good. */
            else
               result_ptr->result_flags = begin_outer_elongation;

            localmodsout1 |= DFM1_CONC_FORCE_SPOTS;      /* Make sure these people go to the same spots. */
         }
         else
            result_ptr->result_flags = 0;

         /* Strip out the roll bits -- people who didn't move can't roll. */
         if (setup_attrs[result_ptr->kind].setup_limits >= 0) {
            for (i=0; i<=setup_attrs[result_ptr->kind].setup_limits; i++) {
               if (result_ptr->people[i].id1) result_ptr->people[i].id1 = (result_ptr->people[i].id1 & (~ROLL_MASK)) | ROLLBITM;
            }
         }
      }

      if (analyzer ==  schema_concentric_to_outer_diamond && doing_ends && result_ptr->kind != sdmd) {
         if (result_ptr->kind != s1x4 || (result_ptr->people[1].id1 | result_ptr->people[3].id1))
            fail("Can't make a diamond out of this.");
         result_ptr->kind = sdmd;
      }
   }

   /* Now, if some command (centers or ends) didn't exist, we pick up the needed result flags
      from the other part. */
   /* Grab the "did_last_part" flags from the call that was actually done. */

   if (inverting) {
      if (!cmdin)
         result_outer.result_flags |= result_inner[0].result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE|RESULTFLAG__PARTS_ARE_KNOWN);

      if (!cmdout) {
         for (k=0; k<center_arity; k++)
            result_inner[k].result_flags |= result_outer.result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE|RESULTFLAG__PARTS_ARE_KNOWN);
      }
   }
   else {
      if (!cmdout)
         result_outer.result_flags |= result_inner[0].result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE|RESULTFLAG__PARTS_ARE_KNOWN);

      if (!cmdin) {
         for (k=0; k<center_arity; k++)
            result_inner[k].result_flags |= result_outer.result_flags & (RESULTFLAG__DID_LAST_PART|RESULTFLAG__SECONDARY_DONE|RESULTFLAG__PARTS_ARE_KNOWN);
      }
   }

   if (     (analyzer == schema_in_out_triple_squash || analyzer == schema_in_out_triple || analyzer == schema_in_out_quad) &&
            fix_n_results(2, nothing, result_inner))
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
      if (begin_outer_elongation <= 0 || begin_outer_elongation > 2 ||
            (orig_outers_start_dirs & (1 << 3*(begin_outer_elongation - 1))))
         fail("Outsides must be as if in lines at start of this call.");
   }

   if ((DFM1_CONC_DEMAND_COLUMNS & localmods1) && (orig_outers_start_kind == s2x2)) {
      if (begin_outer_elongation <= 0 || begin_outer_elongation > 2 ||
            (orig_outers_start_dirs & (8 >> 3*(begin_outer_elongation - 1))))
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

   /* But we don't do this if we are inverting the centers and ends. */

   if (!inverting) {
      localmods1 = crossing ? localmodsout1 : localmodsin1;

      if ((DFM1_CONC_DEMAND_LINES & localmods1) && (orig_inners_start_kind == s2x2)) {
         if (begin_outer_elongation <= 0 || begin_outer_elongation > 2 ||
               (orig_inners_start_dirs & (1 << 3*(begin_outer_elongation - 1))))
            fail("Centers must be as if in lines at start of this call.");
      }

      if ((DFM1_CONC_DEMAND_COLUMNS & localmods1) && (orig_inners_start_kind == s2x2)) {
         if (begin_outer_elongation <= 0 || begin_outer_elongation > 2 ||
               (orig_inners_start_dirs & (8 >> 3*(begin_outer_elongation - 1))))
            fail("Centers must be as if in columns at start of this call.");
      }
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
      else if (  analyzer == schema_conc_bar ||
                 analyzer == schema_conc_bar12 ||
                 analyzer == schema_conc_bar16) {

         /* This is what makes 12 matrix quarter the deucey work when everyone is
            in the stars. */

         result_outer.kind = s2x3;
         clear_people(&result_outer);
         result_outer.result_flags = 1;
         result_outer.rotation = 1;
      }
      else if (analyzer == schema_in_out_triple_squash) {
         result_outer.kind = s2x2;
         clear_people(&result_outer);
         result_outer.result_flags = 0;
         result_outer.rotation = 0;
      }
      else if (analyzer == schema_concentric_diamond_line) {
         switch (ss->kind) {
            case s_wingedstar:
            case s_wingedstar12:
            case s_wingedstar16:
            case s_barredstar:
            case s_barredstar12:
            case s_barredstar16:
            case s3x1dmd:
               result_outer.kind = s2x2;
               result_outer.rotation = 0;
               clear_people(&result_outer);
               /* Set their "natural" elongation perpendicular to their original diamond.
                  The test for this is 1P2P; touch 1/4; column circ; boys truck; split phantom
                  lines tag chain thru reaction.  They should finish in outer triple boxes,
                  not a 2x4. */
               result_outer.result_flags = (result_inner[0].result_flags & ~3) | 2;
               break;
            default:
               goto no_end_err;
         }
      }
      else {
         uint32 orig_elong_flags = result_outer.result_flags & 3;

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
         else if (result_outer.kind == s4x4 && analyzer == schema_conc_o) {
            ;        /* Take no further action. */
         }
         else if (final_outers_start_kind == s1x4 &&
                  result_outer.kind == s1x4 &&
                  (localmods1 & DFM1_CONC_FORCE_SPOTS)) {
            /* If a call starts in a 1x4 and has "force spots" indicated, it must go to a 2x2
               with same elongation. */
            result_outer.kind = s2x2;    /* Take no further action. */
         }
         else if (final_outers_start_kind == s1x4 &&
                  !crossing &&
                  (orig_elong_flags ^ begin_outer_elongation) == 3 &&
                  (localmods1 & DFM1_CONC_CONCENTRIC_RULES)) {
            /* If a call starts in a 1x4 but tries to set the result elongation to the
               opposite of the starting elongation, it must have been trying to go to a 2x2.
               In that case, the "opposite elongation" rule applies. */
            result_outer.kind = s2x2;    /* Take no further action. */
         }
         else if (final_outers_start_kind == s1x2 && result_outer.kind == s1x2 && ((orig_elong_flags+1) & 2)) {
            result_outer.rotation = orig_elong_flags & 1;    /* Note that the "desired elongation" is the opposite
                                                               of the rotation, because of the default change
                                                               if the call went to a 2x2.  (See the documentation
                                                               of the "parallel_conc_end" flag.)  So we do what
                                                               seems to be the wrong thing. */
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
               result->kind = s_dead_concentric;
               result->inner.skind = result_inner[0].kind;
               result->inner.srotation = result_inner[0].rotation;
               result->rotation = 0;
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
            analyzer == schema_in_out_triple_squash ||
            analyzer == schema_in_out_triple ||
            analyzer == schema_in_out_quad) {
                  ;        /* Take no action. */
      }
      /* If the ends are a 2x2, we just set the missing centers to a 2x2.
         The ends had better know their elongation, of course.  It shouldn't
         matter to the ends whether the phantoms in the center did something
         that leaves the whole setup as diamonds or as a 2x4.  (Some callers
         might think it matters (Hi, Clark!) but it doesn't matter to this program.)
         This is what makes split phantom diamonds diamond chain through work
         from a grand wave. */
      /* Also, if doing "O" stuff, it's easy.  Do the same thing. */
      else if (center_arity == 1 &&
            (  result_outer.kind == s2x2 ||
               (result_outer.kind == s4x4 && analyzer == schema_conc_o))) {
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
      else if (   result_outer.kind == s1x2 &&
                  (analyzer == schema_single_concentric)) {
         result_inner[0].kind = s1x2;
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
         result_inner[0].rotation = result_outer.rotation;
      }
      /* If the ends are a 1x6, we just set the missing centers to a 1x2,
         so the entire setup is a 1x8.  Maybe the phantoms went the other way,
         so the setup is really a 1x3 diamond, but we don't care.  See the comment
         just above. */
      else if (result_outer.kind == s1x6 && analyzer == schema_concentric_2_6) {
         result_inner[0].kind = s1x2;
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
         result_inner[0].rotation = result_outer.rotation;
      }
      /* If the ends are a short6, (presumably the whole setup was a qtag or hrglass),
         and the missing centers were an empty 1x2, we just restore that 1x2. */
      else if (result_outer.kind == s_short6 &&
               analyzer == schema_concentric_2_6 &&
               begin_inner[0].kind == s1x2) {
         result_inner[0] = begin_inner[0];
         clear_people(&result_inner[0]);
         result_inner[0].result_flags = 0;
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

   if (        analyzer != schema_in_out_triple &&
               analyzer != schema_in_out_triple_squash &&
               analyzer != schema_in_out_quad &&
               analyzer != schema_conc_o &&
               analyzer != schema_conc_bar12 &&
               analyzer != schema_conc_bar16) {
      if (result_outer.kind == s2x2 || result_outer.kind == s2x3 || result_outer.kind == s2x4) {
         warning_index *concwarntable;

         if (final_outers_start_kind == s1x4) {
            /* Watch for special case of cross concentric that some people may not agree with. */
            if (orig_outers_start_kind == s1x4 && ((begin_inner[0].rotation ^ begin_outer.rotation) & 1))
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

               if ((DFM1_CONC_FORCE_LINES & localmods1) && final_outers_finish_dirs) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in lines - they are T-boned.");
                  final_elongation = (final_outers_finish_dirs & 1) + 1;
               }
               else if ((DFM1_CONC_FORCE_COLUMNS & localmods1) && final_outers_finish_dirs) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in columns - they are T-boned.");
                  final_elongation = ((~final_outers_finish_dirs) & 1) + 1;
               }
               else if ((DFM1_CONC_CONCENTRIC_RULES | DFM1_CONC_FORCE_OTHERWAY) & localmods1) {
                  warn(concwarntable[crossing]);
                  final_elongation ^= 3;
               }
               else if (DFM1_CONC_FORCE_SPOTS & localmods1)
                  ;           /* It's OK the way it is. */
               else {
                  /* Get the elongation from the result setup, if possible. */
                  int newelong = result_outer.result_flags & 3;

                  if (newelong) {
                     if (final_elongation == newelong)
                        warn(concwarntable[2]);
                     else
                        warn(concwarntable[crossing]);
                  }

                  final_elongation = newelong;
               }

               break;

            case s_short6: case s_spindle: case s1x6: case s2x3: case s2x4:

               /* In these cases we honor the "concentric rules" and "force_otherway" flags.
                  The "force_spots" flag is implicitly honored -- it is the default. */

               if ((DFM1_CONC_CONCENTRIC_RULES | DFM1_CONC_FORCE_OTHERWAY) & localmods1)
                  final_elongation ^= 3;

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

               if ((DFM1_CONC_FORCE_LINES & localmods1) && final_outers_finish_dirs) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in lines - they are T-boned.");
                  final_elongation = (final_outers_finish_dirs & 1) + 1;
               }
               else if ((DFM1_CONC_FORCE_COLUMNS & localmods1) && final_outers_finish_dirs) {
                  if ((final_outers_finish_dirs & 011) == 011)
                     fail("Can't force ends to be as in columns - they are T-boned.");
                  final_elongation = ((~final_outers_finish_dirs) & 1) + 1;
               }
               else if (DFM1_CONC_FORCE_OTHERWAY & localmods1) {
                  if (((final_elongation-1) & (~1)) == 0)
                     final_elongation ^= 3;
               }
               else if (DFM1_CONC_FORCE_SPOTS & localmods1)
                  ;           /* It's OK the way it is. */
               else if (DFM1_CONC_CONCENTRIC_RULES & localmods1) {       /* do "lines-to-lines / columns-to-columns" */
                  int new_elongation = -1;

                  if (final_elongation < 0)
                     fail("People who finish on the outside can't tell whether they started in line-like or column-like orientation.");

                  if (final_elongation <= 2) {     /* If they are butterfly points, leave them there. */
                     for (i=0; i<32; i++) {     /* Search among all possible people, including virtuals and phantoms. */
                        if (final_outers_finish_directions[i]) {
                           int t = ((final_outers_start_directions[i] ^ final_outers_finish_directions[i] ^ (final_elongation-1)) & 1) + 1;
                           if (t != new_elongation) {
                              if (new_elongation >= 0)
                                 fail("Sorry, outsides would have to go to a 'pinwheel', can't handle that.");
                              new_elongation = t;
                           }
                        }
                     }

                     final_elongation = new_elongation;
                  }
               }
               else
                  final_elongation = (result_outer.result_flags & 3);

               break;
            default:
               if (     analyzer != schema_concentric_diamond_line &&
                        analyzer != schema_conc_star &&
                        analyzer != schema_ckpt_star &&
                        analyzer != schema_conc_star12 &&
                        analyzer != schema_conc_star16 &&
                        analyzer != schema_concentric_others)
                  fail("Don't recognize starting setup.");
         }
      }
      else if (result_outer.kind == s_short6 || result_outer.kind == sbigdmd) {
         /* If it both started and ended in a short6, take the info from the way the call was executed.
            Otherwise, take it from the way concentrify thought the ends were initially elongated. */
         if (final_outers_start_kind == s_short6)
            final_elongation = (result_outer.result_flags & 3);
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

   result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_FIELDMASK;
   reinstate_rotation(ss, result);
   return;

   no_end_err:
   fail("Can't figure out ending setup for concentric call -- no ends.");
}


typedef struct {
   Const setup_kind k1;
   Const setup_kind k2;
   Const uint32 m1;
   Const uint32 m2;
   /* This is the mask of things that we will reject.  The low 4 bits are rotations
      that we will reject.  It is ANDed with "1 << r".  R is the rotation of res1,
      after localizing so that res2 has rotation zero.  Hence r=0 if the two setups
      have the same orientation, and, except in the case of things like triangles,
      r=1 if they are orthogonal.  Common values of the low hex digit of "rotmask"
      are therefore:
         E demand same orientation
         D demand orthogonal
         C either way.
      Additionally, the "10" bit means that action must be merge_without_gaps,
      and the "20" bit means that action must NOT be merge_strict_matrix. */
   Const unsigned short rotmask;
   Const unsigned short swap_setups;  /* 1 bit - swap setups; 2 bit - change elongation; 4 bit - no take right hands */
   Const calldef_schema conc_type;
   Const setup_kind innerk;
   Const setup_kind outerk;
   Const warning_index warning;
   Const int irot;
   Const int orot;
   Const veryshort innermap[16];
   Const veryshort outermap[16];
} concmerge_thing;


static concmerge_thing map_tgl4l  = {nothing, nothing, 0, 0, 0, 0x0, schema_by_array,       s1x4,        nothing,  warn__none, 0, 0, {0, 1, -1, -1},             {0}}; 
static concmerge_thing map_tgl4b  = {nothing, nothing, 0, 0, 0, 0x0, schema_by_array,       s2x2,        nothing,  warn__none, 0, 0, {-1, -1, 2, 3},             {0}};
static concmerge_thing map_2234b  = {nothing, nothing, 0, 0, 0, 0x0, schema_matrix,         s4x4,        nothing,  warn__none, 0, 0, {15, 3, 7, 11},             {12, 13, 14, 0, -1, -1, 4, 5, 6, 8, -1, -1}};

static concmerge_thing merge_maps[] = {
   {s1x4,        s_qtag, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {6, 7, 2, 3},               {0}},
   {s1x4,          sdmd, 0xA,    0x5, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, -1, 2, -1},             {0}},
   {s1x4,     s_hrglass, 0xA,   0x44, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {6, -1, 2, -1},             {0}},
   {s1x4,        s_qtag, 0xA,   0x88, 0x0D, 0x1, schema_concentric,     s_short6,    s1x2,     warn__check_galaxy, 1, 0, {1, 2, 4, 5, 6, 0},      {0, 2}},
   {s1x4,     s_hrglass, 0xA,   0x88, 0x0D, 0x1, schema_concentric,     s_short6,    s1x2,     warn__check_galaxy, 1, 0, {1, 2, 4, 5, 6, 0},      {0, 2}},
   {s2x3,        s_qtag, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 3, 4, 5, 7, 0},         {0}},
   {s2x3,          s1x8, 022,   0x99, 0x1E, 0x0, schema_matrix,         s_ptpd,      nothing,  warn__none, 0, 0, {1, -1, 7, 5, -1, 3},       {-1, 0, 2, -1, -1, 4, 6, -1}},
   {s2x3,          s1x8, 022,   0xAA, 0x1D, 0x1, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 2, 4, 6},            {0, 2, 3, 5}},
   {s2x3,          s1x8, 0,     0xCC, 0x0D, 0x0, schema_matrix,         s4dmd,       nothing,  warn__none, 0, 0, {2, 7, 9, 10, 15, 1},    {12, 13, -1, -1, 4, 5, -1, -1}},
   {s2x3,     s_hrglass, 022,   0x33, 0x0C, 0x1, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {6, 3, 2, 7},            {0, 2, 3, 5}},
   {s2x3,    s_dhrglass, 022,   0x33, 0x0C, 0x1, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {6, 3, 2, 7},            {0, 2, 3, 5}},
   {s2x3,       s1x3dmd, 0,     0x66, 0x2E, 0x0, schema_matrix,         s_spindle,   nothing,  warn__none, 0, 0, {0, 1, 2, 4, 5, 6},         {7, -1, -1, 1, 3, -1, -1, 5}},
   {s1x4,          s3x4, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {10, 11, 4, 5},             {0}},
   {s2x4,      s_c1phan, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {4, 6, 11, 9, 12, 14, 3, 1},{0}},
   {s2x4,      s_c1phan, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, 2, 7, 5, 8, 10, 15, 13},{0}},
   {s2x4,          s4x4, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {14, 3, 7, 5, 6, 11, 15, 13},{0}},
   {s2x4,          s4x4, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {10, 15, 3, 1, 2, 7, 11, 9},{0}},
   {s2x2,          s2x4, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, 5, 6},               {0}},
   {s2x2,        s_bone, 0,     0xCC, 0x2C, 0x0, schema_concentric,     s2x2,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},            {0, 1, 4, 5}},
   {s2x3,          s3x4, 0,    04646, 0x0C, 0x0, schema_concentric,     s2x3,        s2x3,     warn__none, 0, 1, {0, 1, 2, 3, 4, 5},      {3, 4, 6, 9, 10, 0}},
   {s1x6,          s3x4, 0,    04646, 0x0C, 0x0, schema_concentric,     s1x6,        s2x3,     warn__none, 0, 1, {0, 1, 2, 3, 4, 5},      {3, 4, 6, 9, 10, 0}},
   {s1x4,          s3x4, 0,    04646, 0x0C, 0x0, schema_concentric,     s1x4,        s2x3,     warn__none, 0, 1, {0, 1},                  {3, 4, 6, 9, 10, 0}},
   {s1x2,          s3x4, 0,    04646, 0x0C, 0x0, schema_concentric,     s1x2,        s2x3,     warn__none, 0, 1, {0, 1},                  {3, 4, 6, 9, 10, 0}},
   {s2x3,        s_d3x4, 0,    01616, 0x0C, 0x0, schema_concentric,     s2x3,        s2x3,     warn__none, 0, 1, {0, 1, 2, 3, 4, 5},      {4, 5, 6, 10, 11, 0}},
   {s1x6,        s_d3x4, 0,    01616, 0x0C, 0x0, schema_concentric,     s1x6,        s2x3,     warn__none, 0, 1, {0, 1, 2, 3, 4, 5},      {4, 5, 6, 10, 11, 0}},
   {s2x2,         s3dmd, 0,    07272, 0x0C, 0x0, schema_matrix,         s4x4,        nothing,  warn__check_butterfly, 0, 0, {15, 3, 7, 11},             {12, -1, 0, -1, -1, -1, 4, -1, 8, -1, -1, -1}},
   {s1x8,          s2x4, 0xAA,  0x66, 0x1D, 0x0, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 2, 4, 6},            {0, 3, 4, 7}},

   {s1x6,          s2x4, 022,   0x66, 0x1E, 0x0, schema_matrix,         s_ptpd,      nothing,  warn__none, 0, 0, {0, -1, 2, 4, -1, 6},{1, -1, -1, 7, 5, -1, -1, 3}},
   {s1x8,          s2x4, 0x99,  0x66, 0x1E, 0x0, schema_matrix,         s_ptpd,      nothing,  warn__none, 0, 0, {-1, 0, 2, -1, -1, 4, 6, -1},{1, -1, -1, 7, 5, -1, -1, 3}},
   {s1x8,          s2x4, 0xAA,  0x66, 0x0E, 0x0, schema_matrix,         s_ptpd,      nothing,  warn__none, 0, 0, {0, -1, 2, -1, 4, -1, 6, -1},{1, -1, -1, 7, 5, -1, -1, 3}},

   {s2x2,          s2x6, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 3, 8, 9},               {0}},
   {s2x2,          s4x4, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {15, 3, 7, 11},             {0}},
   {s1x4,       s3x1dmd, 0xA,      0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, -1, 7, -1},             {0}},
   {s1x4,       s1x3dmd, 0,     0xAA, 0x1E, 0x0, schema_matrix,         s1x8,        nothing,  warn__none, 0, 0, {3, 2, 7, 6},               {0, -1, 1, -1, 4, -1, 5, -1}},
   {s1x4,       s1x3dmd, 0,     0xCC, 0x0E, 0x0, schema_matrix,         s1x8,        nothing,  warn__none, 0, 0, {3, 2, 7, 6},               {0, 1, -1, -1, 4, 5, -1, -1}},
   {s2x2,          s1x8, 0,     0xCC, 0x0E, 0x0, schema_concentric,     s2x2,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {s1x4,          s1x6, 0,      044, 0x0C, 0x0, schema_concentric,     s1x4,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 3, 4}},
   {s1x4,          s1x8, 0,     0xCC, 0x0C, 0x0, schema_concentric,     s1x4,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {s1x4,   s_crosswave, 0,     0xCC, 0x0E, 0x0, schema_concentric,     s1x4,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {s2x2,   s_crosswave, 0,     0xCC, 0x0C, 0x0, schema_concentric,     s2x2,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {s1x4,   s_crosswave, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 3, 6, 7},               {0}},
   {s1x4,          s1x8, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, 2, 7, 6},               {0}},
   {s1x4,         s1x10, 0,    0x318, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, 4, 8, 9},               {0}},
   {s1x6,         s1x10, 0,    0x39C, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 3, 4, 7, 8, 9},         {0}},
   {sdmd,          s1x6, 0,      044, 0x0C, 0x0, schema_concentric,     sdmd,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 3, 4}},
   {sdmd,       s1x3dmd, 0,     0xCC, 0x0C, 0x0, schema_concentric,     sdmd,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {sdmd,          s1x8, 0,     0xCC, 0x0C, 0x0, schema_concentric,     sdmd,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 4, 5}},
   {sdmd,          s1x8, 0xA,   0x88, 0x1D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 1, 2, 4, 5, 6},      {0, 2}},
/*
   {sdmd,          s1x8, 0xA,   0x11, 0x1D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {1, 3, 2, 5, 7, 6},      {0, 2}},
*/
   {sdmd,          s1x8, 0xA,   0x22, 0x1D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 3, 2, 4, 7, 6},      {0, 2}},
   {sdmd,          s1x6, 0xA,      0, 0x1D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},      {0, 2}},

   {s1x6,          s1x6, 044,    022, 0x1E, 0x4, schema_matrix,         s1x8,        nothing,  warn__none, 0, 0, {1, 3, -1, 5, 7, -1},    {0, -1, 2, 4, -1, 6}},
   {s1x6,          s1x6, 022,    044, 0x1E, 0x4, schema_matrix,         s1x8,        nothing,  warn__none, 0, 0, {0, -1, 2, 4, -1, 6},    {1, 3, -1, 5, 7, -1}},

   {s1x6,          s1x8, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 3, 2, 5, 7, 6},         {0}},

   {s1x2,         s3dmd, 0,    07070, 0x0C, 0x0, schema_concentric,     s1x2,        s2x3,     warn__none, 0, 0, {0, 1},                     {0, 1, 2, 6, 7, 8}},
   {sdmd,         s3dmd, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 5, 7, 11},              {0}},
   {s1x2,          s1x4, 0,      0xA, 0x0C, 0x0, schema_concentric,     s1x2,        s1x2,     warn__none, 0, 0, {0, 1},                     {0, 2}},
   {s1x2,          s1x6, 0,      044, 0x0C, 0x0, schema_concentric,     s1x2,        s1x4,     warn__none, 0, 0, {0, 1},                     {0, 1, 3, 4}},
   {s1x2,          s1x8, 0,     0x44, 0x0C, 0x0, schema_concentric_2_6, s1x2,        s1x6,     warn__none, 0, 0, {0, 1},                     {0, 1, 3, 4, 5, 7}},
   {s1x2,          s2x4, 0,     0x66, 0x0C, 0x0, schema_concentric,     s1x2,        s2x2,     warn__none, 0, 0, {0, 1},                     {0, 3, 4, 7}},
   {s_hrglass, s_galaxy, 0x44,  0xEE, 0x0E, 0x0, schema_concentric,     s_bone6,     s1x2,     warn__none, 1, 0, {1, 4, 7, 5, 0, 3},         {0, 4}},
   {s_hrglass, s_galaxy, 0x44,  0xBB, 0x0D, 0x0, schema_concentric,     s_bone6,     s1x2,     warn__none, 1, 1, {1, 4, 7, 5, 0, 3},         {2, 6}},
   {s_hrglass,     s2x4, 0x33,  0x66, 0x0C, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {6, 3, 2, 7},            {0, 3, 4, 7}},
   {s_dhrglass,    s2x4, 0x33,  0x66, 0x0C, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {6, 3, 2, 7},            {0, 3, 4, 7}},
   {s1x2,       s3x1dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 6},                     {0}},
   {s1x2,       s3x1dmd, 0,     0x44, 0x0D, 0x0, schema_matrix,         s_crosswave, nothing,  warn__none, 0, 0, {3, 7},                     {0, 1, -1, 2, 4, 5, -1, 6}},
   {s1x2,   s_crosswave, 0,     0x88, 0x0E, 0x0, schema_matrix,         s3x1dmd,     nothing,  warn__none, 0, 0, {2, 6},                     {0, 1, 3, -1, 4, 5, 7, -1}},
   {s1x2,       sbigdmd, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, 9},                     {0}},
   {s1x2,      sbighrgl, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {9, 3},                     {0}},
   {s1x2,       sbigdmd, 0,    01010, 0x0E, 0x0, schema_by_array,       sbighrgl,    nothing,  warn__none, 0, 0, {9, 3},                     {0}},
   {s1x2,      sbighrgl, 0,    01010, 0x0D, 0x0, schema_by_array,       sbigdmd,     nothing,  warn__none, 0, 0, {3, 9},                     {0}},

/*    not yet
   {s1x2,     sbigdhrgl, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 8},                     {0}},
   {s1x2,      sbigbone, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, 9},                     {0}},
*/

   {s1x4,       s3x1dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, 5, 6},               {0}},
   {s1x4,       s3x1dmd, 0,     0x66, 0x2D, 0x1, schema_matrix,         s3x1dmd,     nothing,  warn__none, 0, 0, {7, -1, -1, 0, 3, -1, -1, 4}, {1, 2, 5, 6}},
   {s1x4,      s_1x2dmd, 0,      044, 0x0E, 0x0, schema_matrix,         s1x8,        nothing,  warn__none, 0, 0, {3, 2, 7, 6},               {0, 1, -1, 4, 5, -1}},
   {s1x6,       s3x1dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, 1, 2, 4, 5, 6},         {0}},
   {s1x6,      s_2x1dmd, 022,    022, 0x0E, 0x0, schema_matrix,         s3x1dmd,     nothing,  warn__none, 0, 0, {0, -1, 2, 4, -1, 6},       {1, -1, 3, 5, -1, 7}},
   {s1x2,         s3dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {11, 5},                    {0}},
   {s1x4,         s3dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {10, 11, 4, 5},             {0}},
   {s1x6,         s3dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {9, 10, 11, 3, 4, 5},       {0}},
   {s2x3,         s3dmd, 0,    07070, 0x0E, 0x0, schema_concentric,     s2x3,        s2x3,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},         {0, 1, 2, 6, 7, 8}},
   {s1x4,        s_bone, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {6, 7, 2, 3},               {0}},
   {s_qtag,   s_hrglass, 0x88,  0xBB, 0x0D, 0x0, schema_concentric,     s_short6,    s1x2,     warn__check_galaxy, 1, 0, {1, 2, 4, 5, 6, 0}, {6, 2}},
   {s_qtag,        s3x4, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, 4, 5, 7, 8, 10, 11}, {0}},
   {s_bone,        s1x8, 0x33,     0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {-1, -1, 7, 6, -1, -1, 3, 2},{0}},
   {s2x2,        s_ptpd, 0,     0x55, 0x0E, 0x0, schema_concentric,     s2x2,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {1, 7, 5, 3}},
   {s2x2,      s_galaxy, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 3, 5, 7},               {0}},
   {s2x2,       s3x1dmd, 0,     0x66, 0x0E, 0x0, schema_concentric,     s2x2,        sdmd,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 3, 4, 7}},

   {s2x2,          s1x8, 0,     0x55, 0x0E, 0x0, schema_concentric,     s2x2,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {1, 3, 5, 7}},
   {s2x2,          s1x6, 0,      044, 0x0E, 0x0, schema_concentric,     s2x2,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 1, 3, 4}},

   {s2x2,          s1x8, 0,     0x66, 0x1E, 0x0, schema_concentric,     s2x2,        s1x4,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 3, 4, 7}},
   {s_bone6,       s1x8, 0,     0xAA, 0x0E, 0x0, schema_matrix,          s_ptpd,     nothing,  warn__none, 0, 0, {1, 7, 6, 5, 3, 2},         {0, -1, 2, -1, 4, -1, 6, -1}},
   {s1x8,        s_ptpd, 0xAA,     0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, -1, 2, -1, 4, -1, 6, -1},{0}},
   {s_1x2dmd,   s1x3dmd, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, 3, 5, 6, 7},         {0}},
   {s_1x2dmd, s_spindle, 0,     0x55, 0x2E, 0x0, schema_matrix,         s1x3dmd,     nothing,  warn__none, 0, 0, {1, 2, 3, 5, 6, 7}, {-1, 3, -1, 4, -1, 7, -1, 0}},
   {s1x3dmd,     s_ptpd, 0x66,  0x55, 0x0E, 0x0, schema_rev_checkpoint,    sdmd,     s2x2,     warn__none, 0, 0, {0, 3, 4, 7},               {1, 7, 5, 3}},
   {s1x3dmd,  s_spindle, 0x66,  0xAA, 0x0E, 0x0, schema_rev_checkpoint,    sdmd,     s2x2,     warn__none, 0, 0, {0, 3, 4, 7},               {0, 2, 4, 6}},

   {s1x8,       s1x3dmd, 0x55,  0x66, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {-1, 1, -1, 2, -1, 5, -1, 6},{0}},
   {s1x6,       s1x3dmd, 044,   0x66, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, -1, 5, 6, -1},{0}},

   {s1x8,       s3x1dmd, 0xAA,  0x66, 0x1E, 0x0, schema_matrix,         s3x1dmd,     nothing,  warn__none, 0, 0, {0, -1, 2, -1, 4, -1, 6, -1},{1, -1, -1, 3, 5, -1, -1, 7}},
   {s1x8,     s_spindle, 0xAA,  0xAA, 0x0E, 0x0, schema_rev_checkpoint, s1x4,        s2x2,     warn__none, 0, 0, {0, 2, 4, 6},               {0, 2, 4, 6}},
   {s1x8,      s_rigger, 0xCC,     0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {6, 7, -1, -1, 2, 3, -1, -1},{0}},
   {s1x2,   s_crosswave, 0,        0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {3, 7},                     {0}},
   {s_crosswave,s_crosswave, 0, 0x99, 0x0D, 0x1, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {-1, 6, 1, -1, -1, 2, 5, -1}, {0}},
   {s_crosswave,s_crosswave, 0x99, 0, 0x0D, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {-1, 2, 5, -1, -1, 6, 1, -1}, {0}},
   {s2x2,         s4dmd, 0,   0xF0F0, 0x0E, 0x0, schema_matrix,         s4x4,        nothing,  warn__none, 0, 0, {15, 3, 7, 11},             {12, 13, 14, 0, -1, -1, -1, -1, 4, 5, 6, 8, -1, -1, -1, -1}},
   {s_qtag,        s2x6, 0x33, 0x30C, 0x0D, 0x0, schema_matrix,         sbigdmd,     nothing,  warn__none, 0, 0, {-1, -1, 8, 9, -1, -1, 2, 3}, {0, 1, -1, -1, 4, 5, 6, 7, -1, -1, 10, 11}},
   {s1x4,          s2x6, 0,    0x30C, 0x0D, 0x0, schema_matrix,         sbigdmd,     nothing,  warn__none, 0, 0, {2, 3, 8, 9},               {0, 1, -1, -1, 4, 5, 6, 7, -1, -1, 10, 11}},
   {s1x4,          s2x6, 0,    0x30C, 0x0E, 0x0, schema_matrix,         sbigbone,    nothing,  warn__none, 0, 0, {2, 3, 8, 9},               {0, 1, -1, -1, 4, 5, 6, 7, -1, -1, 10, 11}},
   {sdmd,          s2x6, 0,    0x30C, 0x0D, 0x0, schema_matrix,         sbighrgl,    nothing,  warn__none, 0, 0, {2, 3, 8, 9},               {0, 1, -1, -1, 4, 5, 6, 7, -1, -1, 10, 11}},
   {sdmd,          s2x6, 0,    0x30C, 0x0E, 0x0, schema_matrix,         sbigdhrgl,   nothing,  warn__none, 0, 0, {9, 2, 3, 8},               {0, 1, -1, -1, 4, 5, 6, 7, -1, -1, 10, 11}},
   {s2x4,          s2x6, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {1, 2, 3, 4, 7, 8, 9, 10},  {0}},
   {s2x4,          s2x8, 0,        0, 0x0E, 0x0, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {2, 3, 4, 5, 10, 11, 12, 13},{0}},
   {s1x4,          s1x6, 0xA,      0, 0x0D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},         {0, 2}},
   {s1x6,          s1x6, 066,      0, 0x2D, 0x1, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},         {0, 3}},
   {s1x6,          s1x6, 0,      066, 0x2D, 0x0, schema_concentric,     s1x6,        s1x2,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},         {0, 3}},
   {s1x4,          s2x3, 0xA,      0, 0x0D, 0x1, schema_concentric,     s2x3,        s1x2,     warn__none, 0, 0, {0, 1, 2, 3, 4, 5},         {0, 2}},
   {s_qtag,        s2x3, 0,      022, 0x0D, 0x1, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {5, -1, 0, 1, -1, 4},       {0}},
   {s_qtag,        s2x4, 0,     0x66, 0x0D, 0x1, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {5, -1, -1, 0, 1, -1, -1, 4}, {0}},
   {s_bone,        s2x4, 0,     0x66, 0x0E, 0x1, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, -1, -1, 1, 4, -1, -1, 5}, {0}},
   {s1x8,        s_ptpd, 0,     0xAA, 0x0E, 0x1, schema_nothing,        nothing,     nothing,  warn__none, 0, 0, {0, -1, 2, -1, 4, -1, 6, -1}, {0}},
   {s1x4,          s2x4, 0,     0x66, 0x0C, 0x0, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 3, 4, 7}},
   {s2x2,          s2x3, 0,      022, 0x0C, 0x0, schema_concentric,     s2x2,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 2, 3, 5}},
   {s1x4,          s2x3, 0,      022, 0x0C, 0x0, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 2, 3, 5}},
   {s_qtag,        s2x4, 0x33,  0x66, 0x0C, 0x0, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {6, 7, 2, 3},               {0, 3, 4, 7}},
   {s1x2,        s_qtag, 0,     0x88, 0x0C, 0x2, schema_concentric_2_6, s1x2,        s_short6, warn__none, 0, 1, {0, 1},                     {1, 2, 4, 5, 6, 0}},
   {s2x3,        s_thar, 022,   0xCC, 0x0D, 0x1, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 1, 4, 5},               {0, 2, 3, 5}},
   {s2x3,        s_thar, 022,   0x33, 0x0E, 0x1, schema_concentric,     s1x4,        s2x2,     warn__none, 1, 0, {2, 3, 6, 7},               {0, 2, 3, 5}},
   {s2x4,        s_thar, 0x66,  0xCC, 0x0D, 0x1, schema_concentric,     s1x4,        s2x2,     warn__none, 0, 0, {0, 1, 4, 5},               {0, 3, 4, 7}},
   {s2x4,        s_thar, 0x66,  0x33, 0x0E, 0x1, schema_concentric,     s1x4,        s2x2,     warn__none, 1, 0, {2, 3, 6, 7},               {0, 3, 4, 7}},
   {s1x4,      s_galaxy, 0,     0xAA, 0x0C, 0x0, schema_concentric,     s1x4,        sdmd,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 2, 4, 6}},
   {sdmd,          s2x3, 0,      022, 0x0C, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 2, 3, 5}},
   {sdmd,          s2x4, 0,     0x66, 0x0C, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {0, 1, 2, 3},               {0, 3, 4, 7}},
   {s_spindle,     s2x4, 0x55,  0x66, 0x0D, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {7, 1, 3, 5},               {0, 3, 4, 7}},
   {s_spindle, s_spindle, 0x55, 0xAA, 0x0D, 0x0, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {7, 1, 3, 5},               {0, 2, 4, 6}},
   {s_spindle, s_spindle, 0xAA, 0x55, 0x0D, 0x1, schema_concentric,     sdmd,        s2x2,     warn__none, 0, 0, {7, 1, 3, 5},               {0, 2, 4, 6}},
   {nothing, nothing}};


static Const veryshort matrixmap1[8] = {14, 3, 7, 5, 6, 11, 15, 13};
static Const veryshort matrixmap2[8] = {10, 15, 3, 1, 2, 7, 11, 9};
static Const veryshort phanmap2[8]  = {0, 2, 7, 5, 8, 10, 15, 13};
static Const veryshort phanmap1[8]  = {4, 6, 11, 9, 12, 14, 3, 1};


/* This overwrites its first argument setup. */
extern void merge_setups(setup *ss, merge_action action, setup *result)
{
   int i, j, r, rot, lim1, limhalf;
   setup res2copy;
   setup outer_inners[2];
   setup *res1, *res2;
   uint32 collision_mask;
   int collision_index;
   uint32 rotmaskreject;
   uint32 mask1, mask2, result_mask;
   concmerge_thing *the_map;
   int outer_elongation;
   int reinstatement_rotation;

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

   if (     (res2->kind == s_normal_concentric && res2->outer.skind == nothing) ||
            res2->kind == s_dead_concentric) {
      res2->kind = res2->inner.skind;
      res2->rotation += res2->inner.srotation;
      goto tryagain;    /* Need to recanonicalize setup order. */
   }

   reinstatement_rotation = res2->rotation;
   res1->rotation -= res2->rotation;
   res2->rotation = 0;
   result->rotation = 0;
   canonicalize_rotation(res1);
   canonicalize_rotation(res2);

   for (i=0, j=1, mask1 = 0; i<=setup_attrs[res1->kind].setup_limits; i++, j<<=1) {
      if (res1->people[i].id1) mask1 |= j;
   }

   for (i=0, j=1, mask2 = 0; i<=setup_attrs[res2->kind].setup_limits; i++, j<<=1) {
      if (res2->people[i].id1) mask2 |= j;
   }

   r = res1->rotation & 3;
   rot = r * 011;

   if (res1->kind == nothing) {
      *result = *res2;
      goto final_getout;
   }

   rotmaskreject = (1<<r);
   if (action != merge_without_gaps) rotmaskreject |= 0x10;
   if (action == merge_strict_matrix) rotmaskreject |= 0x20;

   for (the_map=merge_maps ; the_map->k1 != nothing ; the_map++) {
      if (  res1->kind == the_map->k1 &&
            res2->kind == the_map->k2 &&
            (!(rotmaskreject & the_map->rotmask)) &&
            (mask1 & the_map->m1) == 0 &&
            (mask2 & the_map->m2) == 0)
         goto merge_concentric;
   }

   if (res1->kind == s2x4 && res2->kind == s2x4 && (r&1)) {
      long_boolean going_to_stars;
      long_boolean going_to_o;
      long_boolean conflict_at_4x4;
      uint32 t1, t2, t3, t4;

      t1 = res2->people[0].id1 | res2->people[1].id1 | res2->people[4].id1 | res2->people[5].id1;
      t2 = res2->people[2].id1 | res2->people[3].id1 | res2->people[6].id1 | res2->people[7].id1;
      t3 = res1->people[0].id1 | res1->people[1].id1 | res1->people[4].id1 | res1->people[5].id1;
      t4 = res1->people[2].id1 | res1->people[3].id1 | res1->people[6].id1 | res1->people[7].id1;

      conflict_at_4x4 = (
         (res2->people[1].id1 & res1->people[6].id1) |
         (res2->people[2].id1 & res1->people[1].id1) |
         (res2->people[5].id1 & res1->people[2].id1) |
         (res2->people[6].id1 & res1->people[5].id1)) != 0;

      going_to_stars = ((mask1 == 0x33) && (mask2 == 0xCC)) || ((mask1 == 0xCC) && (mask2 == 0x33));
      going_to_o = ((mask1 | mask2) & 0x66) == 0;

      if (   (action == merge_strict_matrix && !going_to_stars && !conflict_at_4x4) || going_to_o) {
         result->kind = s4x4;
         clear_people(result);
         scatter(result, res1, matrixmap1, 7, 011);
         /* We need to use "install" for these, lest we overwrite a live person with zero. */
         for (i=0 ; i<8 ; i++) install_person(result, matrixmap2[i], res2, i);
      }
      else {
         result->kind = s_c1phan;
         scatter(result, res1, phanmap1, 7, 011);
         scatter(result, res2, phanmap2, 7, 0);

         /* See if we have a "classical" C1 phantom setup, and give the appropriate warning. */
         if (action != merge_c1_phantom_nowarn) {
            if ((t1 | t3) == 0 || (t2 | t4) == 0)
               warn(warn__check_c1_phan);
            else if ((t1 | t4) == 0 || (t2 | t3) == 0)
               warn(warn__check_c1_stars);
            else
               warn(warn__check_gen_c1_stars);
         }
      }
      goto final_getout;
   }
   else if (res2->kind == s_trngl4 && res1->kind == s_trngl4 && r == 2 && (mask1 & 0xC) == 0 && (mask2 & 0xC) == 0) {
      (void) copy_rot(res2, 0, res2, 0, 011);
      (void) copy_rot(res2, 1, res2, 1, 011);
      res2->rotation = 3;
      the_map = &map_tgl4l;
      rot = 011;
      goto merge_concentric;
   }
   else if (res2->kind == s_trngl4 && res1->kind == s_trngl4 && r == 2 && (mask1 & 0x3) == 0 && (mask2 & 0x3) == 0) {
      (void) swap_people(res1, 0, 2);
      (void) swap_people(res1, 1, 3);
      res1->kind = s2x2;
      canonicalize_rotation(res1);
      (void) swap_people(res2, 0, 2);
      (void) swap_people(res2, 1, 3);
      the_map = &map_tgl4b;
      rot = 0;
      goto merge_concentric;
   }
   else if (res2->kind == s3x4 && res1->kind == s2x2 && ((mask2 & 06060) == 0)) {
      the_map = &map_2234b;
      warn((mask2 & 06666) ? warn__check_4x4 : warn__check_butterfly);
      goto merge_concentric;
   }

   /* This is sleazy. */

   /* The only remaining hope is that the setups match and we can blindly combine them.  We require
      lim1 even because our 180 degree rotation wouldn't work for triangles. */

   *result = *res2;
   lim1 = setup_attrs[result->kind].setup_limits+1;
   limhalf = (r) ? lim1 >> 1 : 0;

   if (res1->kind != res2->kind || res1->kind == s_trngl4 || (r & 1) || lim1 <= 0 || (lim1 & 1))
      fail("Can't figure out result setup.");

   collision_mask = 0;
   result_mask = 0;

   for (i=0; i<lim1; i++)
      if (result->people[i].id1) result_mask |= (1 << i);

   for (i=0; i<lim1; i++) {
      unsigned int newperson = rotperson(res1->people[(i+limhalf) % lim1].id1, rot);

      if (newperson) {
         if (result->people[i].id1 == 0) {
            result->people[i].id1 = newperson;
            result->people[i].id2 = res1->people[(i+limhalf) % lim1].id2;
            result_mask |= (1 << i);
         }
         else {
            /* We have a collision. */
            collision_person1 = result->people[i].id1;   /* Prepare the error message. */
            collision_person2 = newperson;
            error_message1[0] = '\0';
            error_message2[0] = '\0';

            if (action >= merge_c1_phantom && lim1 <= 12 && result->people[i+12].id1 == 0) {
               /* Collisions are legal.  Store the person in the overflow area
                  (12 higher than the main area, which is why we only permit
                  this if the result setup size is <= 12) and record the fact
                  in the collision_mask so we can figure out what to do. */
               result->people[i+12].id1 = newperson;
               result->people[i+12].id2 = res1->people[(i+limhalf) % lim1].id2;
               collision_mask |= (1 << i);
               collision_index = i;        /* In case we need to report a mundane collision. */
            }
            else {
               longjmp(longjmp_ptr->the_buf, 3);
            }
         }
      }
   }

   if (collision_mask) fix_collision(0, collision_mask, collision_index, result_mask, FALSE, FALSE, result);

   goto final_getout;

   merge_concentric:

   if (the_map->swap_setups & 1) {
      setup *temp = res2;
      res2 = res1;
      res1 = temp;
      rot = ((-r) & 3) * 011;
   }

   outer_elongation = (res2->rotation ^ (the_map->swap_setups >> 1)) & 1;
   warn(the_map->warning);

   if (the_map->conc_type == schema_nothing) {
      *result = *res2;
      goto merge_merge;
   }
   else if (the_map->conc_type == schema_by_array) {
      *result = *res2;
      result->kind = the_map->innerk;
      canonicalize_rotation(result);
      goto merge_merge;
   }
   else if (the_map->conc_type == schema_matrix) {
      *result = *res2;
      result->kind = the_map->innerk;
      clear_people(result);
      scatter(result, res2, the_map->outermap, setup_attrs[res2->kind].setup_limits, 0);
      goto merge_merge;
   }

   rot = 0;
   res2->kind = the_map->outerk;
   if (the_map->orot) {
      res2->rotation++;
      rot = 033;
   }
   outer_inners[0] = *res2;
   gather(&outer_inners[0], res2, the_map->outermap, setup_attrs[res2->kind].setup_limits, rot);
   canonicalize_rotation(&outer_inners[0]);

   rot = 0;
   res1->kind = the_map->innerk;
   if (the_map->irot) {
      res1->rotation++;
      rot = 033;
   }
   outer_inners[1] = *res1;
   gather(&outer_inners[1], res1, the_map->innermap, setup_attrs[res1->kind].setup_limits, rot);
   canonicalize_rotation(&outer_inners[1]);
   normalize_concentric(the_map->conc_type, 1, outer_inners, outer_elongation + 1, result);
   goto final_getout;

   merge_merge:

   lim1 = setup_attrs[result->kind].setup_limits+1;
   collision_mask = 0;
   result_mask = 0;

   for (i=0; i<lim1; i++)
      if (result->people[i].id1) result_mask |= (1 << i);

   for (i=0; i<=setup_attrs[res1->kind].setup_limits; i++) {
      int resultplace = the_map->innermap[i];
      uint32 newperson = rotperson(res1->people[i].id1, rot);
   
      if (newperson) {
         if (resultplace < 0) fail("This would go into an excessively large matrix.");
   
         if (result->people[resultplace].id1 == 0) {
            result->people[resultplace].id1 = newperson;
            result->people[resultplace].id2 = res1->people[i].id2;
            result_mask |= (1 << resultplace);
         }
         else {
            collision_person1 = result->people[resultplace].id1;
            collision_person2 = newperson;
            error_message1[0] = '\0';
            error_message2[0] = '\0';
            if (     action >= merge_c1_phantom &&
                     lim1 <= 12 &&
                     !(the_map->swap_setups & 4) &&
                     result->people[resultplace+12].id1 == 0) {
               /* Collisions are legal.  Store the person in the overflow area
                  (12 higher than the main area, which is why we only permit
                  this if the result setup size is <= 12) and record the fact
                  in the collision_mask so we can figure out what to do. */

               result->people[resultplace+12].id1 = newperson;
               result->people[resultplace+12].id2 = res1->people[i].id2;
               collision_mask |= (1 << resultplace);
               collision_index = resultplace;        /* In case we need to report a mundane collision. */
            }
            else {
               longjmp(longjmp_ptr->the_buf, 3);
            }
         }
      }
   }

   if (collision_mask) fix_collision(0, collision_mask, collision_index, result_mask, FALSE, FALSE, result);

   final_getout:

   result->rotation += reinstatement_rotation;
   canonicalize_rotation(result);
}


extern void on_your_own_move(
   setup *ss,
   parse_block *parseptr,
   setup *result)
{
   int i;
   warning_info saved_warnings;
   setup setup1, setup2, res1;
   setup outer_inners[2];

   if (ss->kind != s2x4) fail("Must have 2x4 setup for 'on your own'.");

   saved_warnings = history[history_ptr+1].warnings;

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

   /* Shut off "superfluous phantom setups" warnings. */

   for (i=0 ; i<WARNING_WORDS ; i++) {
      history[history_ptr+1].warnings.bits[i] &= ~useless_phan_clw_warnings.bits[i];
      history[history_ptr+1].warnings.bits[i] |= saved_warnings.bits[i];
   }



}



/* We know that the setup has well-defined size, and that the conctable masks are good. */
extern void punt_centers_use_concept(setup *ss, setup *result)
{
   int i, setupcount;
   uint32 ssmask;
   warning_info saved_warnings;
   setup the_setups[2], the_results[2];
   int sizem1 = setup_attrs[ss->kind].setup_limits;
   calldef_schema schema = ((calldef_schema) ss->cmd.cmd_misc2_flags & 0xFFFFUL);
   int crossconc = (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE_INVERT) ? 1 : 0;
   long_boolean doing_yoyo = FALSE;
   parse_block *parseptrcopy;

   /* Clear the stuff out of the cmd_misc2_flags word. */

   ss->cmd.cmd_misc2_flags &= ~(0xFFFF | CMD_MISC2__CTR_USE_INVERT | CMD_MISC2__CTR_USE);

   the_setups[0] = *ss;              /* designees */
   the_setups[1] = *ss;              /* non-designees */

   switch (schema) {
      case schema_concentric_2_6:
         ssmask = setup_attrs[ss->kind].mask_2_6;
         break;
      case schema_concentric_6_2:
         ssmask = setup_attrs[ss->kind].mask_6_2;
         break;
      default:
         ssmask = setup_attrs[ss->kind].mask_normal;
         break;
   }

   for (i=sizem1; i>=0; i--) {
      clear_person(&the_setups[(ssmask & 1) ^ crossconc], i);
      ssmask >>= 1;
   }

   normalize_setup(&the_setups[0], normalize_before_isolated_call);
   normalize_setup(&the_setups[1], normalize_before_isolated_call);
   saved_warnings = history[history_ptr+1].warnings;

   /* Check for "someone work yoyo".  If call is sequential and yoyo is consumed by
      first part, then just do this stuff on the first part.  After that, merge
      the setups and finish the call normally. */

   if (     ss->cmd.parseptr->concept->kind == concept_yoyo &&
            ss->cmd.parseptr->next->concept->kind == marker_end_of_list &&
            ss->cmd.parseptr->next->call->schema == schema_sequential &&
            (ss->cmd.parseptr->next->call->callflagsh & INHERITFLAG_YOYO) &&
            (ss->cmd.parseptr->next->call->stuff.def.defarray[0].modifiersh & INHERITFLAG_YOYO) &&
            ss->cmd.cmd_frac_flags == CMD_FRAC_NULL_VALUE) {
      doing_yoyo = TRUE;
      ss->cmd.cmd_frac_flags = CMD_FRAC_BREAKING_UP | CMD_FRAC_FORCE_VIS | CMD_FRAC_PART_BIT | CMD_FRAC_NULL_VALUE;
   }

   for (setupcount=0; setupcount<2; setupcount++) {
      the_setups[setupcount].cmd = ss->cmd;
      the_setups[setupcount].cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;
      if (setupcount == 1) {
         parseptrcopy = skip_one_concept(ss->cmd.parseptr);
         the_setups[setupcount].cmd.parseptr = parseptrcopy->next;
      }

      move(&the_setups[setupcount], FALSE, &the_results[setupcount]);
   }

   /* Shut off "each 1x4" types of warnings -- they will arise spuriously while
      the people do the calls in isolation. */
   for (i=0 ; i<WARNING_WORDS ; i++) {
      history[history_ptr+1].warnings.bits[i] &= ~dyp_each_warnings.bits[i];
      history[history_ptr+1].warnings.bits[i] |= saved_warnings.bits[i];
   }

   *result = the_results[1];
   result->result_flags = get_multiple_parallel_resultflags(the_results, 2);
   merge_setups(&the_results[0], merge_c1_phantom, result);

   if (doing_yoyo) {
      uint32 finalresultflags = result->result_flags;

      the_setups[0] = *result;
      the_setups[0].cmd = ss->cmd;    /* Restore original command stuff (though we clobbered fractionalization info). */
      the_setups[0].cmd.cmd_assume.assumption = cr_none;  /* Assumptions don't carry through. */
      the_setups[0].cmd.cmd_frac_flags = CMD_FRAC_BREAKING_UP | CMD_FRAC_FORCE_VIS | CMD_FRAC_CODE_BEYOND | CMD_FRAC_PART_BIT | CMD_FRAC_NULL_VALUE;
      the_setups[0].cmd.parseptr = parseptrcopy->next;      /* Skip over the concept. */
      move(&the_setups[0], FALSE, result);
      finalresultflags |= result->result_flags;
      normalize_setup(result, simple_normalize);
      result->result_flags = finalresultflags & ~3;
   }
}


typedef struct fixerjunk {
   Const setup_kind ink;
   Const setup_kind outk;
   Const int rot;
   Const short prior_elong;
   Const short numsetups;
   Const struct fixerjunk *next1x2;
   Const struct fixerjunk *next1x2rot;
   Const struct fixerjunk *next1x4;
   Const struct fixerjunk *next1x4rot;
   Const struct fixerjunk *nextdmd;
   Const struct fixerjunk *nextdmdrot;
   Const struct fixerjunk *next2x2;
   Const struct fixerjunk *next2x2v;
   Const veryshort nonrot[24];
} fixer;


static Const fixer f1x8aa;
static Const fixer f1x8ctr;
static Const fixer foozz;
static Const fixer fo6zz;
static Const fixer fqtgend;
static Const fixer f1x6aad;
static Const fixer f1x8aad;
static Const fixer foo66d;
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



/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer foo33a    = {s1x2, s2x4,        0, 0, 2,       0,          0,          0,          0, 0,          0,    0,          0,          {5, 4, 0, 1}};
static Const fixer foocc     = {s1x2, s2x4,        0, 0, 2,       &foocc,     &foo33a,    0,          0, 0,          0,    0,          0,          {2, 3, 7, 6}};
static Const fixer foo33     = {s1x2, s2x4,        0, 0, 2,       &foo33,     &foocc,     0,          0, 0,          0,    0,          0,          {0, 1, 5, 4}};
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
static Const fixer f3x4outer = {s1x3, s3x4,        1, 0, 2,       &f3x4outer, &f1x12outer,0,          0, 0,          0,    0,          0,          {0, 10, 9, 3, 4, 6}};
static Const fixer f1x12outer= {s1x3, s1x12,       0, 0, 2,       &f1x12outer,&f3x4outer, 0,          0, 0,          0,    0,          0,          {0, 1, 2, 8, 7, 6}};
static Const fixer f3x1zzd   = {sdmd, s3x1dmd,     0, 0, 1,       0,          0,          0,          0, &f3x1zzd,   0,    0,          0,          {0, 3, 4, 7}};
static Const fixer f1x3zzd   = {sdmd, s1x3dmd,     0, 0, 1,       0,          0,          &f1x8ctr,   0, &f1x3zzd,   0,    0,          0,          {0, 3, 4, 7}};
static Const fixer f3x1yyd   = {sdmd, s3x1dmd,     0, 0, 1,       0,          0,          &f1x8aad,   0, &f3x1yyd,   0,    0,          0,          {1, 3, 5, 7}};
static Const fixer f2x1yyd   = {sdmd, s_2x1dmd,    0, 0, 1,       0,          0,          &f1x8aad,   0, &f2x1yyd,   0,    0,          0,          {0, 2, 3, 5}};
static Const fixer f1x3yyd   = {sdmd, s1x3dmd,     0, 0, 1,       0,          0,          &f1x8ctr,   0, &f1x3yyd,   0,    0,          0,          {1, 3, 5, 7}};
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

static Const fixer box3c     = {s1x2, s2x2,        0, 0, 1,       &box3c,     0,          0,          0, 0,          0,    0,          0,          {0, 1}};
static Const fixer box6c     = {s1x2, s2x2,        1, 0, 1,       &box6c,     0,          0,          0, 0,          0,    0,          0,          {1, 2}};
static Const fixer box9c     = {s1x2, s2x2,        1, 0, 1,       &box9c,     0,          0,          0, 0,          0,    0,          0,          {0, 3}};
static Const fixer boxcc     = {s1x2, s2x2,        0, 0, 1,       &boxcc,     0,          0,          0, 0,          0,    0,          0,          {3, 2}};

static Const fixer f1x2aad   = {s1x2, s_1x2dmd,    0, 0, 2,       &f1x2aad,   &f2x3c,     0,          0, 0,          0,    0,          0,          {0, 1, 4, 3}};
static Const fixer f1x3bbd   = {s1x4, s1x3dmd,     0, 0, 1,       0,          0,          &f1x3bbd,   0, 0,          0,    &fspindld,  &fspindld,  {1, 2, 5, 6}};
static Const fixer fhrglassd = {s2x2, s_hrglass,   0, 2, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 4, 5}};
static Const fixer fspindld  = {s2x2, s_spindle,   0, 1, 1,       0,          0,          &f1x3bbd,   0, 0,          0,    &fspindld,  &fhrglassd, {0, 2, 4, 6}};
static Const fixer fptpzzd   = {s1x4, s_ptpd,      0, 0, 1,       0,          0,          &fptpzzd,   0, &fspindlbd, 0,    0,          0,          {0, 2, 4, 6}};
static Const fixer fspindlbd = {sdmd, s_spindle,   0, 0, 1,       0,          0,          &fptpzzd,   0, &fspindlbd, 0,    0,          0,          {7, 1, 3, 5}};
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
static Const fixer f1x8_77   = {s1x6, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 4, 5, 6}};
static Const fixer f1x8_bb   = {s1x6, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 3, 4, 5, 7}};
static Const fixer f1x8_dd   = {s1x6, s1x8,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 3, 2, 4, 7, 6}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer foo99d    = {s1x4, s1x8,        0, 0, 1,       0,          0,          &foo99d,    0, 0,          0,    &f2x4endd,  &f2x4endd,  {0, 3, 4, 7}};
static Const fixer foo66d    = {s1x4, s1x8,        0, 0, 1,       0,          0,          &foo66d,    0, 0,          0,    &bar55d,    &bar55d,    {1, 2, 5, 6}};
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
/* These 6 are unsymmetrical. */
static Const fixer f2x4far   = {s1x4, s2x4,        0, 0, 1,       0,          0,          &f2x4far,   0, 0,          0,    0,          0,          {0, 1, 3, 2}};
static Const fixer f2x4near  = {s1x4, s2x4,        0, 0, 1,       0,          0,          &f2x4near,  0, 0,          0,    0,          0,          {7, 6, 4, 5}};
static Const fixer f2x4left  = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4left,  &f2x4left,  {0, 1, 6, 7}};
static Const fixer f2x4right = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4right, &f2x4right, {2, 3, 4, 5}};
static Const fixer f2x4dleft = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4dleft, &f2x4dleft, {0, 2, 5, 7}};
static Const fixer f2x4dright= {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    &f2x4dright,&f2x4dright,{1, 3, 4, 6}};

/*                              ink   outk       rot  el numsetup 1x2         1x2rot      1x4    1x4rot dmd         dmdrot 2x2      2x2v             nonrot  */

static Const fixer f2x4endd  = {s2x2, s2x4,        0, 1, 1,       0,          0,          &frigendd,  &frigendd, 0,  0,    &f2x4endd,  &fqtgend,   {0, 3, 4, 7}};
static Const fixer f2x477    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 2, 4, 5, 6}};
static Const fixer f2x4ee    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 2, 3, 5, 6, 7}};
static Const fixer f2x4bb    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 1, 3, 4, 5, 7}};
static Const fixer f2x4dd    = {s2x3, s2x4,        0, 1, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {0, 2, 3, 4, 6, 7}};
static Const fixer fdhrgld1  = {s1x2, s_dhrglass,  0, 1, 2,       &fdhrgld1,  0,          0,          0, 0,          0,    0,          0,          {0, 3, 7, 4}};
static Const fixer fdhrgld2  = {s1x2, s_dhrglass,  0, 1, 2,       &fdhrgld2,  0,          0,          0, 0,          0,    0,          0,          {3, 1, 5, 7}};
static Const fixer f2x4endo  = {s1x2, s2x4,        1, 0, 2,       &f2x4endo,  &f1x8endo,  0,          0, 0,          0,    0,          0,          {0, 7, 3, 4}};
static Const fixer bar55d    = {s2x2, s2x4,        0, 0, 1,       0,          0,          0,          0, 0,          0,    0,          0,          {1, 2, 5, 6}};
static Const fixer fppaad    = {s1x2, s2x4,        0, 0, 2,       &fppaad,    0,          0,          0, 0,          0,    0,          0,          {1, 3, 7, 5}};
static Const fixer fpp55d    = {s1x2, s2x4,        0, 0, 2,       &fpp55d,    0,          0,          0, 0,          0,    0,          0,          {0, 2, 6, 4}};



#define LOOKUP_NONE     0x1
#define LOOKUP_DIST_DMD 0x2
#define LOOKUP_DIST_BOX 0x4
#define LOOKUP_DIST_CLW 0x8
#define LOOKUP_DIAG_CLW 0x10
#define LOOKUP_DISC     0x20
#define LOOKUP_IGNORE   0x40


typedef struct {
   Const uint32 key;
   Const setup_kind kk;
   Const uint32 thislivemask;
   Const fixer *fixp;
   Const fixer *fixp2;
   Const int use_fixp2;
} sel_item;

static Const sel_item sel_table[] = {
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
   {LOOKUP_DIAG_CLW,           s4x4,        0x0909, &d4x4l1,     &d4x4l2,     0},
   {LOOKUP_DIAG_CLW,           s4x4,        0x9090, &d4x4l3,     &d4x4l4,     4},
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
   {LOOKUP_DIST_CLW,           s2x4,        0x33,   &d2x4w1,     (fixer *) 0, -1},
   {LOOKUP_DIST_CLW,           s2x4,        0xCC,   &d2x4w2,     (fixer *) 0, -1},
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
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0xC3, &f2x4left,  (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s2x4,     0x3C, &f2x4right, (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x0066, &fcpl12,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x6006, &fcpl23,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x6600, &fcpl34,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s4x4,        0x0660, &fcpl41,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_thar,      0x00CC, &ftharns,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s_thar,      0x0033, &ftharew,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x33,   &foo33,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0xCC,   &foocc,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x4,        0x99,   &f2x4endo,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x6,         033,   &f1x6aa,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x8,        0xAA,   &f1x8aa,     (fixer *) 0, -1},
   {LOOKUP_DISC|LOOKUP_IGNORE|LOOKUP_NONE, s3x4,        03131,  &f3x4outer,  (fixer *) 0, -1},
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

   {LOOKUP_NONE,               s1x8,        0x0F,   &f1x8lowf,   (fixer *) 0, -1},   /* Unsymmetrical */
   {LOOKUP_NONE,               s1x8,        0xF0,   &f1x8hif,    (fixer *) 0, -1},   /* Unsymmetrical */

   {LOOKUP_NONE,               s_bone,      0x33,   &fboneendo,  (fixer *) 0, -1},
   {LOOKUP_NONE,               s_ptpd,      0xAA,   &foozz,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s_spindle,   0x55,   &fspindlc,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s_spindle,   0x66,   &fspindlf,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s_spindle,   0x33,   &fspindlg,   (fixer *) 0, -1},
   {LOOKUP_NONE,               s_galaxy,    0x44,   &fgalcv,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_galaxy,    0x11,   &fgalch,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s1x3dmd,     0x66,   &f1x3aad,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s_1x2dmd,    033,    &f1x2aad,    (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,        055,    &f2x3c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x3,    &box3c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x6,    &box6c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0x9,    &box9c,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x2,        0xC,    &boxcc,      (fixer *) 0, -1},
   {LOOKUP_NONE,               s_qtag,      0xAA,   &fqtgj1,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s_qtag,      0x99,   &fqtgj2,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,         033,   &f2x3j1,     (fixer *) 0, -1},
   {LOOKUP_NONE,               s2x3,         066,   &f2x3j2,     (fixer *) 0, -1},
   {LOOKUP_NONE,               nothing}};



/* This does the various types of "so-and-so do this, perhaps while the others do that" concepts. */

/* indicator = 0  - <> do your part
               1  - <> do your part while the others ....
               2  - own the <>, with the others not doing any call, which doesn't exist
               3  - own the <>, .... by ....
               4  - <>
               5  - <> while the others ....
               6  - <> disconnected   or   <> distorted
               7  - <> disconnected .... while the others ....
               8  - <> ignored
               9  - <> ignored .... while the others ...., which doesn't exist
               10 - <> work <concept>, with the others not doing any call, which doesn't exist
               11 - <> work <concept> (the others do the call, but without the concept)
               12 - <> lead for a .... (as in cast a shadow from a promenade)

        arg2 = 0 - not doing distorted setup, this is the usual case, people work in the
                       actual setup that they have
               1 or 17 - distorted line -- used only with indicator = 6 (16 bit means user said "diagonal")
               2 or 18 - distorted column -- used only with indicator = 6 (same)
               3 or 19 - distorted wave -- used only with indicator = 6   (same)
               4       - distorted box -- used only with indicator = 6
               5       - distorted diamond -- used only with indicator = 6 */


extern void selective_move(
   setup *ss,
   parse_block *parseptr,
   int indicator,
   int arg2,
   uint32 override_selector,
   selector_kind selector_to_use,
   long_boolean concentric_rules,
   setup *result)
{
   setup_command cmd1thing, cmd2thing;
   setup_command *cmd2ptr;
   long_boolean others = indicator & 1;

   cmd1thing = ss->cmd;
   cmd2thing = ss->cmd;
   indicator &= ~1;

   if (indicator == 10) {
      parse_block *parseptrcopy = skip_one_concept(parseptr->next);
      cmd2thing.parseptr = parseptrcopy->next;
   }
   else
      cmd2thing.parseptr = parseptr->subsidiary_root;

   if (others) {
      cmd2ptr = &cmd2thing;
   }
   else
      cmd2ptr = (setup_command *) 0;

   inner_selective_move(
      ss,
      &cmd1thing,
      cmd2ptr,
      indicator,
      others,
      arg2,
      override_selector,
      selector_to_use,
      0,
      (concentric_rules ? DFM1_CONC_CONCENTRIC_RULES : 0),
      result);
}


extern void inner_selective_move(
   setup *ss,
   setup_command *cmd1,
   setup_command *cmd2,
   int indicator,
   long_boolean others,
   int arg2,
   uint32 override_selector,
   selector_kind selector_to_use,
   uint32 modsa1,
   uint32 modsb1,
   setup *result)
{
   selector_kind saved_selector;
   int i, k;
volatile   int setupcount;    /* ******FUCKING DEBUGGER BUG!!!!!! */
   int crossconc;
   uint32 livemask[2];
   uint32 j;
   warning_info saved_warnings;
   calldef_schema schema;
   setup the_setups[2], the_results[2];
   uint32 ssmask, llmask;
   int sizem1 = setup_attrs[ss->kind].setup_limits;
   int orig_indicator = indicator;
   normalize_action action = normalize_before_isolated_call;

   if (indicator == 4) {
      action = normalize_before_merge;
      if (!others && sizem1 == 3) {
         if (selector_to_use == selector_center2)
            selector_to_use = selector_centers;
         else if (selector_to_use == selector_center4)
            selector_to_use = selector_all;
      }
   }

   saved_selector = current_options.who;
   current_options.who = selector_to_use;

   the_setups[0] = *ss;              /* designees */
   the_setups[1] = *ss;              /* non-designees */

   if (sizem1 < 0) fail("Can't identify people in this setup.");

   for (i=0, ssmask=0, llmask=0, j=1; i<=sizem1; i++, j<<=1) {
      ssmask <<= 1;
      llmask <<= 1;
      if (ss->people[i].id1) {
         int q = 0;

         /* We allow the designators "centers" and "ends" while in a 1x8, which
            would otherwise not be allowed.  The reason we don't allow it in
            general is that people would carelessly say "centers kickoff" in a 1x8
            when they should realy say "each 1x4, centers kickoff".  But we assume
            that they will not misuse the term here. */

         if (ss->kind == s1x8 && current_options.who == selector_centers) {
            if (i&2) q = 1;
         }
         else if (ss->kind == s1x8 && current_options.who == selector_ends) {
            if (!(i&2)) q = 1;
         }
         else if (override_selector) {
            if (override_selector & 0x80000000UL) {
               /* It is a headliner/sideliner mask. */
               if (override_selector & 011 & ss->people[i].id1) q = 1;
            }
            else {
               /* It is a little-endian mask of the people to select. */
               if (override_selector & j) q = 1;
            }
         }
         else if (selectp(ss, i))
            q = 1;

         /* Indicator 8 is "<> ignored" -- like "<> disconnected", but inverted. */
         if (orig_indicator == 8) q ^= 1;

         ssmask |= q;
         llmask |= 1;
         clear_person(&the_setups[q], i);
      }
   }

   current_options.who = saved_selector;

   /* If this is "ignored", we have to invert the selector. */
   if (orig_indicator == 8) {
      selector_to_use = selector_list[selector_to_use].opposite;
      indicator = 6;
   }

   if (orig_indicator == 12) {
      /* This is "so-and-so lead for a cast a shadow". */

      uint32 dirmask;
      Const veryshort *map_prom;
      static Const veryshort map_prom_1[16] = {6, 7, 1, 0, 2, 3, 5, 4, 011, 011, 022, 022, 011, 011, 022, 022};
      static Const veryshort map_prom_2[16] = {4, 5, 7, 6, 0, 1, 3, 2, 022, 022, 033, 033, 022, 022, 033, 033};
      static Const veryshort map_prom_3[16] = {0, 1, 3, 2, 4, 5, 7, 6, 000, 000, 011, 011, 000, 000, 011, 011};
      static Const veryshort map_prom_4[16] = {6, 7, 1, 0, 2, 3, 5, 4, 011, 011, 022, 022, 011, 011, 022, 022};

      the_setups[0] = *ss;        /* Use this all over again. */
      clear_people(&the_setups[0]);
      the_setups[0].kind = s2x4;

      for (i=0, dirmask=0; i<8; i++) {
         dirmask <<= 2;
         dirmask |= ss->people[i].id1 & 3;
      }

      if (ss->kind == s_crosswave || ss->kind == s_thar) {
         if (ssmask == 0xCC && dirmask == 0xAF05) {
            map_prom = map_prom_1;
            goto got_map;
         }
         else if (ssmask == 0xCC && dirmask == 0x05AF) {
            map_prom = map_prom_2;
            goto got_map;
         }
         else if (ssmask == 0x33 && dirmask == 0xAF05) {
            map_prom = map_prom_3;
            the_setups[0].rotation++;
            goto got_map;
         }
         else if (ssmask == 0x33 && dirmask == 0x05AF) {
            map_prom = map_prom_4;
            the_setups[0].rotation++;
            goto got_map;
         }
      }

      fail("Can't do this in this setup or with these people designated.");

      got_map:

      for (i=0 ; i<8 ; i++)
         (void) copy_rot(&the_setups[0], i, ss, map_prom[i], map_prom[i+8]);

      update_id_bits(&the_setups[0]);
      move(&the_setups[0], FALSE, result);
      result->result_flags |= RESULTFLAG__IMPRECISE_ROT;
      return;
   }

   /* See if the user requested "centers" (or the equivalent people under some other
      designation), and just do it with the concentric_move stuff if so.
      The concentric_move stuff is much more sophisticated about a lot of things than
      what we would otherwise do. */

   if (orig_indicator == 4 || orig_indicator == 8 || orig_indicator == 10) {
      uint32 mask = ~(~0 << (sizem1+1));

      if (sizem1 == 3) {
         schema = schema_single_concentric;
      }
      else {
         if (ss->kind == s3x4) {             /* **** BUG  what a crock -- fix this right. */

            /* Look for "qtag" occupation. */
            if (llmask == 03333) {
               schema = schema_concentric_6_2;
               if (selector_to_use == selector_center6) goto do_concentric_ctrs;
               if (selector_to_use == selector_outer2) goto do_concentric_ends;
            }

            /* Look for "H" occupation. */
            if (llmask == 04747) {
               schema = schema_concentric_2_6;
               if (selector_to_use == selector_center2) goto do_concentric_ctrs;
               if (selector_to_use == selector_outer6) goto do_concentric_ends;
            }
         }
         else if (ss->kind == s_d3x4) {           /* **** BUG  what a crock -- fix this right. */

            /* Look for "spindle" occupation. */
            if (llmask == 03535) {
               schema = schema_concentric_6_2;
               if (selector_to_use == selector_center6) goto do_concentric_ctrs;
               if (selector_to_use == selector_outer2) goto do_concentric_ends;
            }
         }
         else {
            schema = schema_concentric_6_2;
            if (selector_to_use == selector_center6) goto do_concentric_ctrs;
            if (selector_to_use == selector_outer2) goto do_concentric_ends;
            schema = schema_concentric_2_6;
            if (selector_to_use == selector_center2) goto do_concentric_ctrs;
            if (selector_to_use == selector_outer6) goto do_concentric_ends;
         }

         if (selector_to_use == selector_center2 && !others) {
            schema = schema_select_ctr2;
            action = normalize_to_2;
            goto back_here;
         }
         else if (selector_to_use == selector_center4 && !others) {
            schema = schema_select_ctr4;
            action = normalize_to_4;
            goto back_here;
         }
         else if (selector_to_use == selector_center6 && !others) {
            schema = schema_select_ctr6;
            action = normalize_to_6;
            goto back_here;
         }

         schema = schema_concentric;
      }

      if (selector_to_use == selector_centers) goto do_concentric_ctrs;
      if (selector_to_use == selector_ends) goto do_concentric_ends;

      if (      (ss->kind != s3x4 || llmask == 04747) &&
                (ss->kind != s3dmd || llmask == 07171) &&
                (ss->kind != s4x4 || llmask == 0x9999)) {
         if (setup_attrs[ss->kind].mask_normal) {
            if (ssmask == setup_attrs[ss->kind].mask_normal) goto do_concentric_ctrs;
            else if (ssmask == mask-setup_attrs[ss->kind].mask_normal) goto do_concentric_ends;
         }

         if (setup_attrs[ss->kind].mask_6_2) {
            schema = schema_concentric_6_2;
            if (ssmask == setup_attrs[ss->kind].mask_6_2) goto do_concentric_ctrs;
            else if (ssmask == mask-setup_attrs[ss->kind].mask_6_2) goto do_concentric_ends;
         }

         if (setup_attrs[ss->kind].mask_2_6) {
            schema = schema_concentric_2_6;
            if (ssmask == setup_attrs[ss->kind].mask_2_6) goto do_concentric_ctrs;
            else if (ssmask == mask-setup_attrs[ss->kind].mask_2_6) goto do_concentric_ends;
         }

         if (setup_attrs[ss->kind].mask_ctr_dmd) {
            schema = schema_concentric_diamonds;
            if (ssmask == setup_attrs[ss->kind].mask_ctr_dmd) goto do_concentric_ctrs;
         }

         schema = schema_lateral_6;
         if (ss->kind == s_galaxy && ssmask == 0xDD) goto do_concentric_ctrs;
         schema = schema_vertical_6;
         if (ss->kind == s_galaxy && ssmask == 0x77) goto do_concentric_ctrs;
      }
   }
   else if (orig_indicator == 6) {
      uint32 mask = ~(~0 << (sizem1+1));

      if (setup_attrs[ss->kind].mask_ctr_dmd) {
         schema = schema_concentric_diamonds;
         if (ssmask == mask-setup_attrs[ss->kind].mask_ctr_dmd) goto do_concentric_ends;
      }
      else if (ss->kind == s_galaxy && setup_attrs[ss->kind].mask_normal) {
         schema = schema_concentric;
         if (ssmask == mask-setup_attrs[ss->kind].mask_normal) goto do_concentric_ends;
      }
   }

back_here:

/*    taken out!!!!!!
   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
*/

   normalize_setup(&the_setups[0], action);
   if (others)
      normalize_setup(&the_setups[1], action);

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
      setup *this_one = &the_setups[setupcount];
      setup_kind kk = this_one->kind;
      setup_command *cmdp = (setupcount == 1) ? cmd2 : cmd1;

      this_one->cmd = ss->cmd;

      this_one->cmd.parseptr = cmdp->parseptr;
      this_one->cmd.callspec = cmdp->callspec;
      this_one->cmd.cmd_final_flags = cmdp->cmd_final_flags;

      this_one->cmd.cmd_misc_flags |= CMD_MISC__PHANTOMS;

      if (indicator >= 4 && indicator != 10) {
         Const fixer *fixp = (Const fixer *) 0;
         int lilcount;
         int numsetups;
         uint32 key;
         setup lilsetup[4], lilresult[4];
         int map_scanner;
         Const sel_item *p;
         long_boolean feet_warning = FALSE;

         /* Check for special cases of no one or everyone. */

         if (indicator < 6 || orig_indicator == 8) {
            if (thislivemask == 0) {               /* No one. */
               the_results[setupcount].kind = nothing;
               the_results[setupcount].result_flags = 0;
               continue;
            }
            else if (thislivemask == ((1 << (setup_attrs[kk].setup_limits+1)) - 1) ||
                     otherlivemask == 0 ||
                     schema == schema_select_ctr2 ||
                     schema == schema_select_ctr4 ||
                     schema == schema_select_ctr6) {   /* Everyone. */
               update_id_bits(this_one);
               move(this_one, FALSE, &the_results[setupcount]);
               continue;
            }
         }

         if (arg2 == 5)
            key = LOOKUP_DIST_DMD;
         else if (arg2 == 4)
            key = LOOKUP_DIST_BOX;
         else if (arg2 & 16)
            key = LOOKUP_DIAG_CLW;
         else if (arg2 != 0)
            key = LOOKUP_DIST_CLW;
         else if (orig_indicator == 8)
            key = LOOKUP_IGNORE;
         else if (indicator >= 6)
            key = LOOKUP_DISC;
         else
            key = LOOKUP_NONE;

         for (p = sel_table ; p->kk != nothing ; p++) {
            if ((p->key & key) && p->kk == kk && p->thislivemask == thislivemask) {
               fixp = p->fixp;

               /* We make an extremely trivial test here to see which way the distortion goes.
                  It will be checked thoroughly later. */

               if (p->use_fixp2 >= 0 && ((this_one->people[p->use_fixp2].id1 ^ arg2) & 1))
                  fixp = p->fixp2;

               goto blah;
            }
         }

         /* These two have a looser livemask criterion. */

         if (key & (LOOKUP_IGNORE|LOOKUP_DISC|LOOKUP_NONE)) {
            if (kk == s2x4 && the_setups[setupcount^1].kind == s2x4 && (thislivemask & ~0x0F) == 0 && (otherlivemask & 0x0F) == 0)
               fixp = &f2x4far;
            else if (kk == s2x4 && the_setups[setupcount^1].kind == s2x4 && (thislivemask & ~0xF0) == 0  && (otherlivemask & 0xF0) == 0)
               fixp = &f2x4near;
         }

         if (!fixp)
            fail("Can't do this with these people designated.");

         blah:

         numsetups = fixp->numsetups & 0xFF;
         map_scanner = 0;

         for (lilcount=0; lilcount<numsetups; lilcount++) {
            uint32 tbone = 0;
            uint32 frot = fixp->rot;  /* This stays fixed. */
            uint32 vrot;              /* This shifts down. */
            setup *lilss = &lilsetup[lilcount];
            setup *lilres = &lilresult[lilcount];

            lilss->cmd = this_one->cmd;
            lilss->cmd.prior_elongation_bits = fixp->prior_elong;
            lilss->cmd.cmd_assume.assumption = cr_none;
            if (indicator == 6)
               lilss->cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
            lilss->kind = fixp->ink;
            lilss->rotation = 0;

            for (k=0,vrot=frot>>2; k<=setup_attrs[fixp->ink].setup_limits; k++,map_scanner++,vrot>>=2)
               tbone |= copy_rot(lilss, k, this_one, fixp->nonrot[map_scanner], 011*((-frot-vrot) & 3));

            /* If we are picking out a distorted diamond from a 4x4, we can't tell unambiguously how
               to do it unless all 4 people are facing in a sensible way, that is, as if in real diamonds.
               We did an extremely cursory test to see which map to use, now we have to test whether
               everyone agrees that the distortion direction was correct, by checking whether they are
               in fact in real diamonds. */

            if (kk == s4x4 && arg2 == 5) {
               if ((    (lilss->people[0].id1) |
                        (~lilss->people[1].id1) |
                        (lilss->people[2].id1) |
                        (~lilss->people[3].id1)) & 1)
                  fail("Can't determine direction of diamond distortion.");
            }

            if ((arg2&7) == 2 && (tbone & 010) != 0) fail("There is no column of 4 here.");
            if ((arg2&5) == 1 && (tbone & 001) != 0) fail("There is no line of 4 here.");

            if ((arg2&7) == 3)
               lilss->cmd.cmd_misc_flags |= CMD_MISC__VERIFY_WAVES;

            if (indicator == 6)
               lilss->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;

            update_id_bits(lilss);
            impose_assumption_and_move(lilss, lilres);

            /* There are a few cases in which we handle shape-changers in a distorted setup.
               Print a warning if so.  Of course, it may not be allowed, in which case an
               error will occur later.
               But we won't give the warning if we went to a 4x4, so we just set a flag.  See below. */

            if (arg2 != 0) {
               if (     lilss->kind != lilres->kind ||
                        lilss->rotation != lilres->rotation)
                  feet_warning = TRUE;
            }
         }

         if (fix_n_results(numsetups, nothing, lilresult)) goto lose;

         clear_people(&the_results[setupcount]);
         the_results[setupcount].result_flags = get_multiple_parallel_resultflags(lilresult, numsetups);

         if (lilresult[0].rotation != 0) {
            Const fixer *nextfixp;

            the_results[setupcount].rotation = 0;

            if (lilresult[0].kind == s1x2)
               nextfixp = fixp->next1x2rot;
            else if (lilresult[0].kind == s1x4)
               nextfixp = fixp->next1x4rot;
            else if (lilresult[0].kind == sdmd)
               nextfixp = fixp->nextdmdrot;
            else
               nextfixp = 0;    /* Raise an error. */

            if (nextfixp) {
               if (((fixp->rot ^ nextfixp->rot) & 3) == 0) {
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
               if ((fixp->rot & 3) == 0) {
                  for (lilcount=0; lilcount<numsetups; lilcount++) {
                     lilresult[lilcount].rotation += 2;
                     canonicalize_rotation(&lilresult[lilcount]);
                  }
               }
            }
            else
               goto lose;
         }
         else {
            if (lilresult[0].kind == s1x2)
               fixp = fixp->next1x2;
            else if (lilresult[0].kind == s1x4)
               fixp = fixp->next1x4;
            else if (lilresult[0].kind == sdmd)
               fixp = fixp->nextdmd;
            else if (lilresult[0].kind == s2x2) {
               if ((lilresult[0].result_flags & 3) == 2)
                  fixp = fixp->next2x2v;
               else
                  fixp = fixp->next2x2;
            }
            else if (lilresult[0].kind != fixp->ink)
               fixp = 0;    /* Raise an error. */

            if (!fixp) goto lose;

            the_results[setupcount].kind = fixp->outk;
            the_results[setupcount].rotation = 0;
         }

         map_scanner = 0;

         for (lilcount=0; lilcount<numsetups; lilcount++) {
            uint32 frot = fixp->rot;  /* This stays fixed. */
            uint32 vrot;              /* This shifts down. */
            for (k=0,vrot=frot>>2; k<=setup_attrs[lilresult[0].kind].setup_limits; k++,map_scanner++,vrot>>=2)
               (void) copy_rot(&the_results[setupcount], fixp->nonrot[map_scanner], &lilresult[lilcount], k, 011*((frot+vrot) & 3));
         }

         /* We only give the warning if they in fact went to spots.  Some of the maps
            create a result setup of 4x4.  For these maps, the dancers are not actually
            going to spots, but are going back to the quadrants the belong in.
            This is a "put the offset back" type of adjustment.  There don't seem to be
            any generally recognized words that one says to cause this to happen.
            We hope the dancers will know what to do. */

         if (feet_warning && fixp->outk != s4x4) warn(warn__adjust_to_feet);

         reinstate_rotation(this_one, &the_results[setupcount]);
      }
      else {
         uint32 doing_mystic = this_one->cmd.cmd_misc2_flags & (CMD_MISC2__CTR_END_KMASK | CMD_MISC2__CTR_END_INV_CONC);
         long_boolean mirror = FALSE;
         this_one->cmd.cmd_misc2_flags &= ~CMD_MISC2__CTR_END_MASK;

         if (setupcount == 1)
            doing_mystic ^= CMD_MISC2__CTR_END_INV_CONC;

         if (doing_mystic == CMD_MISC2__CENTRAL_MYSTIC)
            mirror = TRUE;
         else if (doing_mystic == CMD_MISC2__CENTRAL_SNAG) {
            if (this_one->cmd.cmd_frac_flags != CMD_FRAC_NULL_VALUE)
               fail("Can't do fractional \"snag\".");
            this_one->cmd.cmd_frac_flags = CMD_FRAC_HALF_VALUE;
         }

         if (mirror) {
            mirror_this(this_one);
            this_one->cmd.cmd_misc_flags ^= CMD_MISC__EXPLICIT_MIRROR;
         }

         move(this_one, FALSE, &the_results[setupcount]);
         if (mirror) mirror_this(&the_results[setupcount]);
      }
   }

   if (!others) {      /* The non-designees did nothing. */
      the_results[1] = the_setups[1];
       /* Give the people who didn't move the same result flags as those who did.
         This is important for the "did last part" check. */
      the_results[1].result_flags = the_results[0].result_flags;
      if (livemask[1] == 0) the_results[1].kind = nothing;

      /* Strip out the roll bits -- people who didn't move can't roll. */
      if (setup_attrs[the_results[1].kind].setup_limits >= 0) {
         for (k=0; k<=setup_attrs[the_results[1].kind].setup_limits; k++) {
            if (the_results[1].people[k].id1) the_results[1].people[k].id1 = (the_results[1].people[k].id1 & (~ROLL_MASK)) | ROLLBITM;
         }
      }
   }

   /* Shut off "each 1x4" types of warnings -- they will arise spuriously while
      the people do the calls in isolation.
      Also, shut off "superfluous phantom setups" warnings if this was "own the
      <anyone> or <anyone> do your part". */
   for (i=0 ; i<WARNING_WORDS ; i++) {
      history[history_ptr+1].warnings.bits[i] &= ~dyp_each_warnings.bits[i];
      if (indicator < 4)
         history[history_ptr+1].warnings.bits[i] &= ~useless_phan_clw_warnings.bits[i];
      history[history_ptr+1].warnings.bits[i] |= saved_warnings.bits[i];
   }

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

   /* If the setup is bigger than 8 people, concentric_move won't be able to handle it.
      The only hope is that we are just having the center 2 or center 4 do something. */

   if (     setup_attrs[ss->kind].setup_limits > 7 &&
            indicator == 4 && !others)
      goto back_here;

   if (indicator == 10) goto forward_here;

   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

   concentric_move(ss, cmd1, cmd2, schema, modsa1, modsb1, TRUE, result);
   return;

   do_concentric_ends:

   crossconc = 1;

   if (indicator == 10) goto forward_here;

   ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

   concentric_move(ss, cmd2, cmd1, schema, modsa1, modsb1, TRUE, result);
   return;

   forward_here:

   switch (schema) {
      case schema_concentric_2_6:
         ssmask = setup_attrs[ss->kind].mask_2_6;
         break;
      case schema_concentric_6_2:
         ssmask = setup_attrs[ss->kind].mask_6_2;
         break;
      default:
         ssmask = setup_attrs[ss->kind].mask_normal;
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
