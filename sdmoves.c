/* SD -- square dance caller's helper.

    Copyright (C) 1990-1996  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 31. */

/* This defines the following functions:
   canonicalize_rotation
   reinstate_rotation
   divide_for_magic
   do_simple_split
   do_call_in_series
   get_fraction_info
   move
*/

#include <stdio.h>
#include <string.h>
#include "sd.h"


static long_boolean debug_popup = FALSE;   /* Helps debug popups under Domain/OS. */


extern void canonicalize_rotation(setup *result)
{
   result->rotation &= 3;

   if (result->kind == s1x1) {
      (void) copy_rot(result, 0, result, 0, (result->rotation) * 011);
      result->rotation = 0;
   }
   else if (result->kind == s_normal_concentric) {
      int i;

      if (result->inner.skind == s_normal_concentric || result->outer.skind == s_normal_concentric)
         fail("Recursive concentric?????.");

      result->kind = result->inner.skind;
      result->rotation = result->inner.srotation;
      canonicalize_rotation(result);    /* Sorry! */
      result->inner.srotation = result->rotation;

      result->kind = result->outer.skind;
      result->rotation = result->outer.srotation;
      for (i=0 ; i<12 ; i++) swap_people(result, i, i+12);
      canonicalize_rotation(result);    /* Sorrier! */
      for (i=0 ; i<12 ; i++) swap_people(result, i, i+12);
      result->outer.srotation = result->rotation;

      result->kind = s_normal_concentric;
      result->rotation = 0;
   }
   else if (setup_attrs[result->kind].four_way_symmetry) {
      /* The setup has 4-way symmetry.  We can canonicalize it so the
         result rotation is zero. */
      int i, rot, rot11, delta, bigd, i0, i1, i2, i3, j0, j1, j2, j3;
      personrec x0, x1, x2, x3;

      rot = result->rotation;
      if (rot == 0) return;
      rot11 = rot * 011;
      bigd = setup_attrs[result->kind].setup_limits + 1;
      delta = bigd >> 2;

      i0 = 1;
      i1 = i0 + delta;
      i2 = i1 + delta;
      i3 = i2 + delta;
      j0 = (rot-4)*delta+1;
      j1 = j0 + delta;
      j2 = j1 + delta;
      j3 = j2 + delta;
      for (i=0; i<delta; i++) {
         if ((--i0) < 0) i0 += bigd;
         if ((--i1) < 0) i1 += bigd;
         if ((--i2) < 0) i2 += bigd;
         if ((--i3) < 0) i3 += bigd;
         if ((--j0) < 0) j0 += bigd;
         if ((--j1) < 0) j1 += bigd;
         if ((--j2) < 0) j2 += bigd;
         if ((--j3) < 0) j3 += bigd;
         x0 = result->people[i0];
         x1 = result->people[i1];
         x2 = result->people[i2];
         x3 = result->people[i3];
         result->people[j0].id1 = rotperson(x0.id1, rot11);
         result->people[j0].id2 = x0.id2;
         result->people[j1].id1 = rotperson(x1.id1, rot11);
         result->people[j1].id2 = x1.id2;
         result->people[j2].id1 = rotperson(x2.id1, rot11);
         result->people[j2].id2 = x2.id2;
         result->people[j3].id1 = rotperson(x3.id1, rot11);
         result->people[j3].id2 = x3.id2;
      }

      result->rotation = 0;
   }
   else if (result->kind == s_trngl4) {
   }
   else if (result->kind == s1x3) {
      if (result->rotation & 2) {

         /* Must turn this setup upside-down. */
   
         swap_people(result, 0, 2);
         (void) copy_rot(result, 0, result, 0, 022);
         (void) copy_rot(result, 1, result, 1, 022);
         (void) copy_rot(result, 2, result, 2, 022);
      }
      result->rotation &= 1;
   }
   else if (((setup_attrs[result->kind].setup_limits & ~07776) == 1)) {
      /* We have a setup of an even number of people.  We know how to canonicalize
         this.  The resulting rotation should be 0 or 1. */

      if (result->rotation & 2) {

         /* Must turn this setup upside-down. */
   
         int i, offs;
   
         offs = (setup_attrs[result->kind].setup_limits+1) >> 1;     /* Half the setup size. */
   
         for (i=0; i<offs; i++) {
            swap_people(result, i, i+offs);
            (void) copy_rot(result, i, result, i, 022);
            (void) copy_rot(result, i+offs, result, i+offs, 022);
         }
      }
      result->rotation &= 1;
   }
}


extern void reinstate_rotation(setup *ss, setup *result)
{
   int globalrotation;

   switch (ss->kind) {
      case s_normal_concentric:
         globalrotation = 0;
         break;
      default:
         globalrotation = ss->rotation;
   }
   
   switch (result->kind) {
      case s_normal_concentric:
         result->inner.srotation += globalrotation;
         result->outer.srotation += globalrotation;
         break;
      case nothing:
         break;
      default:
         result->rotation += globalrotation;
         break;
   }

   /* If we turned by 90 degress, and the "split axis" bits are 01 or 10,
      we have to to swap those bits. */

   if ((globalrotation & 1) &&
         ((result->result_flags + RESULTFLAG__SPLIT_AXIS_BIT) & (RESULTFLAG__SPLIT_AXIS_BIT << 1)))
      result->result_flags ^= RESULTFLAG__SPLIT_AXIS_MASK;

   canonicalize_rotation(result);
}


extern long_boolean divide_for_magic(
   setup *ss,
   uint32 heritflags_to_use,
   uint32 heritflags_to_check,
   setup *result)
{
   map_thing *division_maps;
   uint32 resflags = 0;

   if (ss->kind == s2x4) {
      if (heritflags_to_check == INHERITFLAG_MAGIC) {
         /* "Magic" was specified.  Split it into 1x4's in the appropriate magical way. */
         division_maps = &map_2x4_magic;
         goto divide_us;
      }
   }
   else if (ss->kind == s_qtag) {
      resflags = RESULTFLAG__NEED_DIAMOND;    /* Indicate that we have done a diamond division and the concept name needs to be changed. */
      if (heritflags_to_check == INHERITFLAG_MAGIC) {
         division_maps = &map_qtg_magic;
         goto divide_us;
      }
      else if (heritflags_to_check == INHERITFLAG_INTLK) {
         division_maps = &map_qtg_intlk;
         goto divide_us;
      }
      else if (heritflags_to_check == (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) {
         division_maps = &map_qtg_magic_intlk;
         goto divide_us;
      }
   }
   else if (ss->kind == s_ptpd) {
      resflags = RESULTFLAG__NEED_DIAMOND;
      if (heritflags_to_check == INHERITFLAG_MAGIC) {
         division_maps = &map_ptp_magic;
         goto divide_us;
      }
      else if (heritflags_to_check == INHERITFLAG_INTLK) {
         division_maps = &map_ptp_intlk;
         goto divide_us;
      }
      else if (heritflags_to_check == (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) {
         division_maps = &map_ptp_magic_intlk;
         goto divide_us;
      }
   }

   return FALSE;

divide_us:

   ss->cmd.cmd_final_flags.herit = heritflags_to_use & ~heritflags_to_check;
   divided_setup_move(ss, division_maps, phantest_ok, TRUE, result);

   /* Since more concepts follow the magic and/or interlocked stuff, we can't
      allow the concept to be just "magic" etc.  We have to change it to
      "magic diamond, ..."  Otherwise, things could come out sounding like
      "magic diamond as couples quarter right" when we should really be saying
      "magic diamond, diamond as couples quarter right".  Therefore, we are going
      to do something seriously hokey: we are going to change the concept descriptor
      to one whose name has the extra "diamond" word.  We do this by marking the
      setupflags word in the result. */

   result->result_flags |= resflags;
   return TRUE;
}


extern long_boolean do_simple_split(setup *ss, long_boolean prefer_1x4, setup *result)
{
   switch (ss->kind) {
      case s2x4:
         if (prefer_1x4)
            divided_setup_move(ss, map_lists[s1x4][1]->f[MPKIND__SPLIT][1], phantest_ok, TRUE, result);
         else
            divided_setup_move(ss, map_lists[s2x2][1]->f[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
         return FALSE;
      case s1x8:
         divided_setup_move(ss, map_lists[s1x4][1]->f[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
         return FALSE;
      case s_qtag:
         divided_setup_move(ss, map_lists[sdmd][1]->f[MPKIND__SPLIT][1], phantest_ok, TRUE, result);
         return FALSE;
      case s_ptpd:
         divided_setup_move(ss, map_lists[sdmd][1]->f[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
         return FALSE;
      default:
         return TRUE;
   }
}


/* Do a call as part of a series.  The result overwrites the incoming setup.

   This handles the task of "turning around" the setup from the result left by
   one call to "move" to the input required by the next call.

   The result_flags are carried from one part to another through the "result_flags"
   word of the setup, even though (or especially because) that word is undefined
   prior to doing a call.  The client must seed that word with RESULTFLAG__SPLIT_AXIS_MASK
   (or whatever other bits are desired) prior to the beginning of the series, and not
   clobber it between parts.  At the end of the series, it will contain all the required
   result flags for the whole operation, including the final elongation.

   The elongation bits of the setup (in the "cmd.prior_elongation_bits" field)
   must be correct at the start of the series, and must preserved from one call
   to the next.  Since the entire "cmd" field of the setup typically gets changed
   for each call in the series (in order to do different calls) this will typically
   require specific action by the client. */

extern void do_call_in_series(
   setup *sss,
   long_boolean roll_transparent,
   long_boolean normalize,
   long_boolean qtfudged)
{
   setup tempsetup;
   setup qqqq = *sss;
   uint32 current_elongation = 0;
   uint32 saved_result_flags = sss->result_flags;

   /* If we are forcing a split, and an earlier call in the series has responded to that split
      by returning an unequivocal splitting axis, we continue to split along the same axis. */

   if ((sss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT) && ((saved_result_flags + RESULTFLAG__SPLIT_AXIS_BIT) & (RESULTFLAG__SPLIT_AXIS_BIT << 1))) {
      int foo;

      if (saved_result_flags & RESULTFLAG__SPLIT_AXIS_BIT)
         foo = qqqq.rotation & 1;
      else
         foo = (~qqqq.rotation) & 1;

      if (foo && qqqq.kind != s2x4)
         fail("Can't figure out how to split multiple part call.");

      qqqq.cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;  /* Take it out. */

      if (do_simple_split(&qqqq, foo, &tempsetup))
         fail("Can't figure out how to split multiple part call.");

      qqqq.cmd.cmd_misc_flags |= CMD_MISC__MUST_SPLIT;  /* Put it back in. */

      /* This will force us to report the same axis for next time.  It throws away the split axis stuff that came
         out of divided_setup_move.  Maybe we should check same???? */
      tempsetup.result_flags |= RESULTFLAG__SPLIT_AXIS_MASK;
   }
   else
      move(&qqqq, qtfudged, &tempsetup);

   if (tempsetup.kind == s2x2) {
      switch (sss->kind) {
         case s1x4: case sdmd: case s2x2:
            current_elongation = tempsetup.result_flags & 3;

            /* If just the ends were doing this, and it had some
               "force_lines" type of directive, honor same. */

            if (     (qqqq.cmd.cmd_misc_flags & CMD_MISC__DOING_ENDS) &&
                     (qqqq.cmd.cmd_misc_flags & (DFM1_CONC_FORCE_LINES | DFM1_CONC_FORCE_COLUMNS))) {
               int i;
               uint32 tb = 0;

               for (i=0; i<4; i++) tb |= tempsetup.people[i].id1;
               if ((tb & 011) == 011) fail("Can't figure out where people finish.");

               if (qqqq.cmd.cmd_misc_flags & DFM1_CONC_FORCE_COLUMNS)
                  tb++;

               current_elongation = (tb & 1) + 1;
            }

            break;

         /* Otherwise (perhaps the setup was a star) we have no idea how to elongate the setup. */
         default:
            break;
      }
   }

   /* If this call is "roll transparent", restore roll info from before the call for those people
      that are marked as roll-neutral. */

   if (roll_transparent) {
      /* Can only do this if we understand the setups. */
      if ((setup_attrs[tempsetup.kind].setup_limits >= 0) && (setup_attrs[sss->kind].setup_limits >= 0)) {
         int u, v;

         for (u=0; u<=setup_attrs[tempsetup.kind].setup_limits; u++) {
            if (tempsetup.people[u].id1 & ROLLBITM) {
               /* This person is roll-neutral.  Reinstate his original roll info, by
                  searching for him in the starting setup. */
               tempsetup.people[u].id1 &= ~ROLL_MASK;
               for (v=0; v<=setup_attrs[sss->kind].setup_limits; v++) {
                  if (((tempsetup.people[u].id1 ^ sss->people[v].id1) & XPID_MASK) == 0)
                     tempsetup.people[u].id1 |= (sss->people[v].id1 & ROLL_MASK);
               }
            }
         }
      }
   }

   *sss = tempsetup;
   sss->cmd.cmd_misc_flags = qqqq.cmd.cmd_misc_flags;   /* But pick these up from the call. */

   /* Remove outboard phantoms. 
      It used to be that normalize_setup was not called
      here.  It was found that we couldn't do things like, from a suitable offset wave,
      [triple line 1/2 flip] back to a wave, that is, start offset and finish normally.
      So this has been added.  However, there may have been a reason for not normalizing.
      If any problems are found, it may be that a flag needs to be added to seqdef calls
      saying whether to remove outboard phantoms after each part. */

   /* But we *DON'T* remove the phantoms if "explicit matrix" is on, as of version 29.45.
      This is to make "1x12 matrix grand swing thru" work from a simple grand wave.
      The intention is to add outboard phantoms and then do the call in a gigantic way.
      We added two outboard phantoms on each end, then did the swing half, resulting
      in another 1x12 with only the center 8 spots occupied.  If outboard phantoms are
      removed, the effect of the explicit "1x12 matrix" modifier is lost.  The "grand
      1/2 by the left" is done on the 1x8.  So we leave the phantoms in.  Actually,
      making this call operate properly required a database change as well.  We had to
      add 1x12 and 1x16 starting setups for "1/2 by the left or right".  Otherwise,
      on "1x12 matrix grand swing thru", it would just do it in each 1x6.  Also, we
      don't want the warning "Do the call in each 1x6" to be given, though it is
      admittedly better to have the program warn us when it's about to do something
      wrong than to do the wrong thing silently.
      We also don't normalize if a "12 matrix" or "16 matrix" modifier is given, so that
      "12 matrix grand swing thru" will work also, along with "1x12 matrix grand swing thru".
      Only "1x12 matrix" turns on CMD_MISC__EXPLICIT_MATRIX.  Plain "12 matrix will appear
      in the "new_final_concepts" word. */

   if (normalize) normalize_setup(sss, simple_normalize);

   canonicalize_rotation(sss);

   /* To be safe, we should take away the "did last part" bit for the second call, but we are fairly sure
      it won't be on. */

   sss->cmd.prior_elongation_bits = current_elongation;
   /* Set the low bits to the current elongation, set the split bits to the AND of the before and
      after states, and set all other bits to the OR of the before and after states.
      But we clear the incoming RESULTFLAG__DID_LAST_PART bit, using only the bit from the call just executed. */
   sss->result_flags = ((
                  (saved_result_flags & ~(RESULTFLAG__DID_LAST_PART|RESULTFLAG__PARTS_ARE_KNOWN)) |
                  tempsetup.result_flags) & ~(3|RESULTFLAG__SPLIT_AXIS_MASK)) |
         current_elongation |
         (saved_result_flags & tempsetup.result_flags & RESULTFLAG__SPLIT_AXIS_MASK);
}





typedef struct gloop {
   int x;                  /* This person's coordinates, calibrated so that a matrix */
   int y;                  /*   position cooresponds to an increase by 4. */
   int nicex;              /* This person's "nice" coordinates, used for calculating jay walk legality. */
   int nicey;
   uint32 id1;             /* The actual person, for error printing. */
   long_boolean sel;       /* True if this person is selected.  (False if selectors not in use.) */
   long_boolean done;      /* Used for loop control on each pass */
   long_boolean realdone;  /* Used for loop control on each pass */
   uint32 jbits;           /* Bit mask for all possible jaywalkees. */
   int boybit;             /* 1 if boy, 0 if not (might be neither). */
   int girlbit;            /* 1 if girl, 0 if not (might be neither). */ 
   int dir;                /* This person's initial facing direction, 0 to 3. */
   int deltax;             /* How this person will move, relative to his own facing */
   int deltay;             /*   direction, when call is finally executed. */
   int nearest;            /* Forward distance to nearest jaywalkee. */
   int leftidx;            /* X-increment of leftmost valid jaywalkee. */
   int rightidx;           /* X-increment of rightmost valid jaywalkee. */
   int deltarot;           /* How this person will turn. */
   int rollinfo;           /* How this person's roll info will be set. */
   struct gloop *nextse;   /* Points to next person south (dir even) or east (dir odd.) */
   struct gloop *nextnw;   /* Points to next person north (dir even) or west (dir odd.) */
   long_boolean tbstopse;  /* True if nextse/nextnw is zero because the next spot */
   long_boolean tbstopnw;  /*   is occupied by a T-boned person (as opposed to being empty.) */
} matrix_rec;



static coordrec squeezethingglass = {s_hrglass, 3,
   { -4,   4,   8,   0,   4,  -4,  -8,   0},
   {  6,   6,   0,   2,  -6,  -6,   0,  -2}, {0}};

static coordrec squeezethinggal = {s_galaxy, 3,
   { -6,  -2,   0,   2,   6,   2,   0,  -2},
   {  0,   2,   6,   2,   0,  -2,  -6,  -2}, {0}};


Private int start_matrix_call(
   setup *ss,
   matrix_rec matrix_info[],
   uint32 flags,
   setup *people)
{
   int i;
   coordrec *thingyptr, *nicethingyptr;
   int nump = 0;

   clear_people(people);

   nicethingyptr = setup_attrs[ss->kind].nice_setup_coords;
   thingyptr = setup_attrs[ss->kind].setup_coords;

   if (flags & (MTX_FIND_SQUEEZERS|MTX_FIND_SPREADERS)) {
      thingyptr = nicethingyptr;
      /* Fix up a galaxy or hourglass so that points can squeeze.  They have funny coordinates so that they can't truck or loop. */
      if (ss->kind == s_hrglass) thingyptr = &squeezethingglass;
      else if (ss->kind == s_galaxy) thingyptr = &squeezethinggal;
   }

   if (!thingyptr || !nicethingyptr) fail("Can't do this in this setup.");
   if (setup_attrs[ss->kind].setup_limits < 0) fail("Can't do this in this setup.");        /* this is actually superfluous */

   for (i=0; i<=setup_attrs[ss->kind].setup_limits; i++) {
      if (ss->people[i].id1) {
         if (nump == 8) fail("?????too many people????");
         (void) copy_person(people, nump, ss, i);

         matrix_info[nump].nicex = nicethingyptr->xca[i];
         matrix_info[nump].nicey = nicethingyptr->yca[i];
         matrix_info[nump].x = thingyptr->xca[i];
         matrix_info[nump].y = thingyptr->yca[i];

         matrix_info[nump].done = FALSE;
         matrix_info[nump].realdone = FALSE;

         if (flags & MTX_USE_SELECTOR)
            matrix_info[nump].sel = selectp(people, nump);
         else
            matrix_info[nump].sel = FALSE;

         matrix_info[nump].id1 = people->people[nump].id1;
         matrix_info[nump].dir = people->people[nump].id1 & 3;

         matrix_info[nump].jbits = 0;
         matrix_info[nump].girlbit = (people->people[nump].id1 & ID1_PERM_GIRL) ? 1 : 0;
         matrix_info[nump].boybit = (people->people[nump].id1 & ID1_PERM_BOY) ? 1 : 0;
         matrix_info[nump].nextse = 0;
         matrix_info[nump].nextnw = 0;
         matrix_info[nump].deltax = 0;
         matrix_info[nump].deltay = 0;
         matrix_info[nump].nearest = 100000;
         matrix_info[nump].deltarot = 0;
         matrix_info[nump].rollinfo = ROLLBITM;
         matrix_info[nump].tbstopse = FALSE;
         matrix_info[nump].tbstopnw = FALSE;
         nump++;
      }
   }

   return nump;
}



static coordrec squeezefinalglass = {s_hrglass, 3,
   { -2,   2,   6,   0,   2,  -2,  -6,   0},
   {  6,   6,   0,   2,  -6,  -6,   0,  -2}, {
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1,  0,  1, -1, -1, -1,
      -1, -1,  6, -1,  3, -1,  2, -1,
      -1, -1, -1, -1,  7, -1, -1, -1,
      -1, -1, -1,  5,  4, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1}};


Private void finish_matrix_call(
   matrix_rec matrix_info[],
   int nump,
   setup *people,
   setup *result)
{
   int i, place;
   int xmax, xpar, ymax, ypar, x, y, k;
   uint32 signature;
   coordrec *checkptr;

   xmax = xpar = ymax = ypar = signature = 0;

   for (i=0; i<nump; i++) {
      people->people[i].id1 = (rotperson(people->people[i].id1, matrix_info[i].deltarot*011) & (~ROLL_MASK)) | matrix_info[i].rollinfo;

      /* If this person's position has low bit on, that means we consider his coordinates
         not sufficiently well-defined that we will allow him to do any pressing or
         trucking.  He is only allowed to turn.  That is, we will require deltax and
         deltay to be zero.  An example of this situation is the points of a galaxy. */

      /* ****** This seems to be too restrictive.  There may have been good reason for doing this
         at one time, but now it makes all press and truck calls illegal in C1 phantoms.  The
         table for C1 phantoms has been carefully chosen to make things legal only within one's
         own miniwave, but it requires odd numbers.  Perhaps we need to double the resolution
         of things in matrix_info[i].x or y, but that should wait until after version 28 is released. */

/* So this is patched out.  The same problem holds for bigdmds.
      if (((matrix_info[i].x | matrix_info[i].y) & 1) && (matrix_info[i].deltax | matrix_info[i].deltay))
         fail("Someone's ending position is not well defined.");
*/

      switch (matrix_info[i].dir) {
         case 0:
            matrix_info[i].x += matrix_info[i].deltax;
            matrix_info[i].y += matrix_info[i].deltay;
            break;
         case 1:
            matrix_info[i].x += matrix_info[i].deltay;
            matrix_info[i].y -= matrix_info[i].deltax;
            break;
         case 2:
            matrix_info[i].x -= matrix_info[i].deltax;
            matrix_info[i].y -= matrix_info[i].deltay;
            break;
         case 3:
            matrix_info[i].x -= matrix_info[i].deltay;
            matrix_info[i].y += matrix_info[i].deltax;
            break;
      }

      x = matrix_info[i].x;
      y = matrix_info[i].y;

      /* Compute new max, parity, and signature info. */

      if ((x < 0) || ((x == 0) && (y < 0))) { x = -x; y = -y; }
      signature |= 1 << ((31000 + 12*x - 11*y) % 31);
      if (y < 0) y = -y;
      /* Now x and y have both had absolute values taken. */
      if (x > xmax) xmax = x;
      if (y > ymax) ymax = y;
      k = x | 4;
      xpar |= (k & (~(k-1)));
      k = y | 4;
      ypar |= (k & (~(k-1)));
   }

   ypar |= (xmax << 20) | (xpar << 16) | (ymax << 4);

   result->rotation = 0;

   if ((ypar == 0x00A20026) && ((signature & (~0x08008404)) == 0)) {
      checkptr = setup_attrs[s_rigger].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00770077) && ((signature & (~0x00418004)) == 0)) {
      checkptr = setup_attrs[s_galaxy].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00840066) && ((signature & (~0x0C000108)) == 0)) {
      /* Fudge this to a galaxy.  The center 2 did a squeeze or spread from a spindle. */

      for (i=0; i<nump; i++) {
         if      (matrix_info[i].x ==  0 && matrix_info[i].y ==  6) {                        matrix_info[i].y =  7; }
         else if (matrix_info[i].x ==  0 && matrix_info[i].y == -6) {                        matrix_info[i].y = -7; }
         else if (matrix_info[i].x ==  8 && matrix_info[i].y ==  0) { matrix_info[i].x =  7;                        }
         else if (matrix_info[i].x == -8 && matrix_info[i].y ==  0) { matrix_info[i].x = -7;                        }
         else if (   (matrix_info[i].x == 4 || matrix_info[i].x == -4) &&
                     (matrix_info[i].y == 2 || matrix_info[i].y == -2)) { matrix_info[i].x >>= 1; }
      }

      warn(warn__check_galaxy);
      checkptr = setup_attrs[s_galaxy].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00260062 || ypar == 0x00660066) && ((signature & (~0x10100600)) == 0)) {
      /* Fudge this to an hourglass.  Six poeple did a squeeze from a galaxy. */
      checkptr = &squeezefinalglass;
      goto doit;
   }
   else if ((ypar == 0x00620026 || ypar == 0x00660066) && ((signature & (~0x09080002)) == 0)) {
      /* Same, on other orientation. */
      checkptr = &squeezefinalglass;
      goto doitrot;
   }
   else if ((ypar == 0x00660026) && ((signature & (~0x00008604)) == 0)) {
      /* Fudge this to a spindle.  Some points did a squeeze from a galaxy. */

      for (i=0; i<nump; i++) {
         if      (matrix_info[i].x ==  6 && matrix_info[i].y ==  0) { matrix_info[i].x =  8; }
         else if (matrix_info[i].x == -6 && matrix_info[i].y ==  0) { matrix_info[i].x = -8; }
         else if (   (matrix_info[i].x == 2 || matrix_info[i].x == -2) &&
                     (matrix_info[i].y == 2 || matrix_info[i].y == -2)) { matrix_info[i].x <<= 1; }
      }

      checkptr = setup_attrs[s_spindle].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00260066) && ((signature & (~0x09008004)) == 0)) {
      /* Same, on other orientation. */

      for (i=0; i<nump; i++) {
         if      (matrix_info[i].x ==  0 && matrix_info[i].y ==  6) { matrix_info[i].y =  8; }
         else if (matrix_info[i].x ==  0 && matrix_info[i].y == -6) { matrix_info[i].y = -8; }
         else if (   (matrix_info[i].x == 2 || matrix_info[i].x == -2) &&
                     (matrix_info[i].y == 2 || matrix_info[i].y == -2)) { matrix_info[i].y <<= 1; }
      }

      checkptr = setup_attrs[s_spindle].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00950062 || ypar == 0x00550062) && ((signature & (~0x091002C0)) == 0)) {
      checkptr = setup_attrs[sbigdmd].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00950095) && ((signature & (~0x22008080)) == 0)) {
      checkptr = setup_attrs[s_thar].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00950066) && ((signature & (~0x28008200)) == 0)) {
      checkptr = setup_attrs[s_crosswave].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00A20026) && ((signature & (~0x01040420)) == 0)) {
      checkptr = setup_attrs[s_bone].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00840026) && ((signature & (~0x04000308)) == 0)) {
      checkptr = setup_attrs[s_spindle].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00A200A2) && ((signature & (~0x101CC4E6)) == 0)) {
      checkptr = setup_attrs[s_bigblob].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00670055) && ((signature & (~0x01000420)) == 0)) {
      checkptr = setup_attrs[s_qtag].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00620026) && ((signature & (~0x01008404)) == 0)) {
      /* Fudge this to a 1/4 tag.  Someone did a truck-like operation from a rigger,
         for example, after a sets in motion. */

      for (i=0; i<nump; i++) {
         if      (matrix_info[i].x == -2 && matrix_info[i].y ==  2) { matrix_info[i].x = -4; matrix_info[i].y =  5; }
         else if (matrix_info[i].x ==  2 && matrix_info[i].y ==  2) { matrix_info[i].x =  5; matrix_info[i].y =  5; }
         else if (matrix_info[i].x ==  2 && matrix_info[i].y == -2) { matrix_info[i].x =  4; matrix_info[i].y = -5; }
         else if (matrix_info[i].x == -2 && matrix_info[i].y == -2) { matrix_info[i].x = -5; matrix_info[i].y = -5; }
      }

      checkptr = setup_attrs[s_qtag].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00550067) && ((signature & (~0x08410200)) == 0)) {
      checkptr = setup_attrs[s_qtag].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00D50026) && ((signature & (~0x20008202)) == 0)) {
      checkptr = setup_attrs[s1x3dmd].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00A60055) && ((signature & (~0x09000480)) == 0)) {
      checkptr = setup_attrs[s3x1dmd].setup_coords;
      goto doit;
   }
   /* Depending on how the setup is actually occupied, xmax and ymax may vary.
      ***** We need to work this out carefully.  Maybe parity should be high
      and max low so that range checking will suffice.
      For now, we just mask out max, and look at par. */
   else if (((ypar & 0x00070007) == 0x00070007) && ((signature & (~0x278198CC)) == 0)) {
      checkptr = setup_attrs[s_c1phan].setup_coords;
      goto doit;
   }
   /* If certain far out people are missing, xmax will be different, but we will
       still need to go to a 3dmd. */
   else if (((ypar == 0x00A70055) || (ypar == 0x00770055) || (ypar == 0x00730055)) && ((signature & (~0x29008480)) == 0)) {
      checkptr = setup_attrs[s3dmd].setup_coords;
      goto doit;
   }
   /* If certain far out people are missing, xmax will be different, but we will
       still need to go to a 4dmd. */
   else if (((ypar == 0x00E30055) || (ypar == 0x00B30055) || (ypar == 0x00A30055)) && ((signature & (~0x0940A422)) == 0)) {
      checkptr = setup_attrs[s4dmd].setup_coords;
      goto doit;
   }
   /* Similarly. */
   else if (((ypar == 0x00D50066) || (ypar == 0x01150066)) && ((signature & (~0x28048202)) == 0)) {
      checkptr = setup_attrs[sbigx].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00550057) && ((signature & (~0x20000620)) == 0)) {
      checkptr = setup_attrs[s_hrglass].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00840066) && ((signature & (~0x00202208)) == 0)) {
      /* Fudge this to an hourglass.  Someone had the checkpointers squeeze or spread
         from a spindle. */

      for (i=0; i<nump; i++) {
         if      (matrix_info[i].x == -4 && matrix_info[i].y ==  6) {                        matrix_info[i].y =  5; }
         else if (matrix_info[i].x ==  4 && matrix_info[i].y ==  6) { matrix_info[i].x =  5; matrix_info[i].y =  5; }
         else if (matrix_info[i].x ==  4 && matrix_info[i].y == -6) {                        matrix_info[i].y = -5; }
         else if (matrix_info[i].x == -4 && matrix_info[i].y == -6) { matrix_info[i].x = -5; matrix_info[i].y = -5; }
         else if (matrix_info[i].x ==  8 && matrix_info[i].y ==  0) { matrix_info[i].x =  5;                        }
         else if (matrix_info[i].x == -8 && matrix_info[i].y ==  0) { matrix_info[i].x = -5;                        }
      }

      checkptr = setup_attrs[s_hrglass].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00A70026) && ((signature & (~0x20040220)) == 0)) {
      checkptr = setup_attrs[s_dhrglass].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00930035) && ((signature & (~0x05200100)) == 0)) {
      checkptr = setup_attrs[s_ptpd].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00620044) && ((signature & (~0x11800C40)) == 0)) {
      checkptr = setup_attrs[s3x4].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00440062) && ((signature & (~0x0C202300)) == 0)) {
      checkptr = setup_attrs[s3x4].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00E20004) && ((signature & (~0x09002400)) == 0)) {
      checkptr = setup_attrs[s1x8].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x000400E2) && ((signature & (~0x08004202)) == 0)) {
      checkptr = setup_attrs[s1x8].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x01220004) && ((signature & (~0x49002400)) == 0)) {
      checkptr = setup_attrs[s1x10].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x01620004) && ((signature & (~0x49012400)) == 0)) {
      checkptr = setup_attrs[s1x12].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x01A20004) && ((signature & (~0x49012404)) == 0)) {
      checkptr = setup_attrs[s1x14].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x01E20004) && ((signature & (~0x49092404)) == 0)) {
      checkptr = setup_attrs[s1x16].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00620022) && ((signature & (~0x00088006)) == 0)) {
      checkptr = setup_attrs[s2x4].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00220062) && ((signature & (~0x10108004)) == 0)) {
      checkptr = setup_attrs[s2x4].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00440022) && ((signature & (~0x04000300)) == 0)) {
      checkptr = setup_attrs[s2x3].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00220044) && ((signature & (~0x01000840)) == 0)) {
      checkptr = setup_attrs[s2x3].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00A20022) && ((signature & (~0x000C8026)) == 0)) {
      checkptr = setup_attrs[s2x6].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x002200A2) && ((signature & (~0x10108484)) == 0)) {
      checkptr = setup_attrs[s2x6].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00E20022) && ((signature & (~0x004C8036)) == 0)) {
      checkptr = setup_attrs[s2x8].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x002200E2) && ((signature & (~0x12908484)) == 0)) {
      checkptr = setup_attrs[s2x8].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00A20044) && ((signature & (~0x19804E40)) == 0)) {
      checkptr = setup_attrs[s3x8].setup_coords;   /* Actually a 3x6. */
      goto doit;
   }
   else if ((ypar == 0x00E20044) && ((signature & (~0x1D806E41)) == 0)) {
      checkptr = setup_attrs[s3x8].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00A20062) && ((signature & (~0x109CC067)) == 0)) {
      checkptr = setup_attrs[s4x6].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x006200A2) && ((signature & (~0x1918C4C6)) == 0)) {
      checkptr = setup_attrs[s4x6].setup_coords;
      goto doitrot;
   }
   else if ((ypar == 0x00620062) && ((signature & (~0x1018C046)) == 0)) {
      checkptr = setup_attrs[s4x4].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00930066) && ((signature & (~0x01080C40)) == 0)) {
      checkptr = setup_attrs[sbigh].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00E20026) && ((signature & (~0x01440430)) == 0)) {
      checkptr = setup_attrs[sbigbone].setup_coords;
      goto doit;
   }
   /* **** These last ones are sort of a crock.  They are designed to make
      matrix calls work in distorted or virtual setups in some circumstances
      (i.e. if no one changes coordinates.)  However, they won't work in the
      presence of unsymmetrical phantoms.  What we really should do is, if
      the setup is virtual/distorted (or maybe the test should be if no one
      moved) just force people back to the same setup they started in. */
   else if ((ypar == 0x00220022) && ((signature & (~0x00008004)) == 0)) {
      checkptr = setup_attrs[s2x2].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00620004) && ((signature & (~0x01000400)) == 0)) {
      checkptr = setup_attrs[s1x4].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00550026) && ((signature & (~0x20020200)) == 0)) {
      checkptr = setup_attrs[sdmd].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00220004) && ((signature & (~0x01000000)) == 0)) {
      checkptr = setup_attrs[s1x2].setup_coords;
      goto doit;
   }

   fail("Can't handle this result matrix.");

doit:
      result->kind = checkptr->result_kind;
      for (i=0; i<nump; i++) {
         place = checkptr->diagram[28 - ((matrix_info[i].y >> 2) << checkptr->xfactor) + (matrix_info[i].x >> 2)];
         if (place < 0) fail("Person has moved into a grossly ill-defined location.");
         if ((checkptr->xca[place] != matrix_info[i].x) || (checkptr->yca[place] != matrix_info[i].y))
            fail("Person has moved into a slightly ill-defined location.");
         install_person(result, place, people, i);
         result->people[place].id1 &= ~STABLE_MASK;   /* For now, can't do fractional stable on this kind of call. */
      }

      return;

doitrot:
      result->kind = checkptr->result_kind;
      result->rotation = 1;   
      for (i=0; i<nump; i++) {
         place = checkptr->diagram[28 - ((matrix_info[i].x >> 2) << checkptr->xfactor) + ((-matrix_info[i].y) >> 2)];
         if (place < 0) fail("Person has moved into a grossly ill-defined location.");
         if ((checkptr->xca[place] != -matrix_info[i].y) || (checkptr->yca[place] != matrix_info[i].x))
            fail("Person has moved into a slightly ill-defined location.");
         install_rot(result, place, people, i, 033);
         result->people[place].id1 &= ~STABLE_MASK;   /* For now, can't do fractional stable on this kind of call. */
      }

      return;
}



Private void matrixmove(
   setup *ss,
   callspec_block *callspec,
   setup *result)
{
   uint32 datum;
   setup people;
   matrix_rec matrix_info[9];
   int i, nump, alldelta;
    uint32 flags = callspec->stuff.matrix.flags;

   if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT)
      fail("Can't split the setup.");

   alldelta = 0;

   nump = start_matrix_call(ss, matrix_info, flags, &people);

   for (i=0; i<nump; i++) {
      matrix_rec *this = &matrix_info[i];

      if (!(flags & MTX_USE_SELECTOR) || this->sel) {
         /* This is legal if girlbit or boybit is on (in which case we use the appropriate datum)
            or if the two data are identical so the sex doesn't matter. */
         if ((this->girlbit | this->boybit) == 0 &&
                  (callspec->stuff.matrix.stuff[0] != callspec->stuff.matrix.stuff[1]))
            fail("Can't determine sex of this person.");

         datum = callspec->stuff.matrix.stuff[this->girlbit];
         alldelta |= (  this->deltax = ( ((datum >> 7) & 0x1F) - 16) << 1  );
         alldelta |= (  this->deltay = ( ((datum >> 2) & 0x1F) - 16) << 1  );
         this->deltarot = datum & 3;
         this->rollinfo = (datum >> 12) * ROLLBITR;
      }
   }

   if ((alldelta != 0) && (ss->cmd.cmd_misc_flags & CMD_MISC__DISTORTED))
      fail("This call not allowed in distorted or virtual setup.");
   
   finish_matrix_call(matrix_info, nump, &people, result);
}



Private void do_pair(
   matrix_rec *ppp,        /* Selected person */
   matrix_rec *qqq,        /* Unselected person */
   callspec_block *callspec,
   int flip,
   int filter)             /* 1 to do N/S facers, 0 for E/W facers. */
{
   int base;
   int datum;
   uint32 flags;

   flags = callspec->stuff.matrix.flags;

   if ((!(flags & (MTX_IGNORE_NONSELECTEES | MTX_BOTH_SELECTED_OK))) && qqq->sel)
      fail("Two adjacent selected people.");

   /* We know that either ppp is actually selected, or we are not using selectors. */

   if ((filter ^ ppp->dir) & 1) {
      base = (ppp->dir & 2) ? 6 : 4;
      if (!(flags & MTX_USE_SELECTOR)) base &= 3;
      base ^= flip;

      /* This is legal if girlbit or boybit is on (in which case we use the appropriate datum)
         or if the two data are identical so the sex doesn't matter. */
      if ((ppp->girlbit | ppp->boybit) == 0 &&
               (callspec->stuff.matrix.stuff[base] != callspec->stuff.matrix.stuff[base+1]))
         fail("Can't determine sex of this person.");

      datum = callspec->stuff.matrix.stuff[base+ppp->girlbit];
      if (datum == 0) failp(ppp->id1, "can't do this call.");
      ppp->deltax = (((datum >> 7) & 0x1F) - 16) << 1;
      ppp->deltay = (((datum >> 2) & 0x1F) - 16) << 1;
      ppp->deltarot = datum & 3;
      ppp->rollinfo = (datum >> 12) * ROLLBITR;
      ppp->realdone = TRUE;
   }
   ppp->done = TRUE;

   if ((filter ^ qqq->dir) & 1) {
      base = (qqq->dir & 2) ? 0 : 2;
      if ((flags & MTX_IGNORE_NONSELECTEES) || qqq->sel) base |= 4;
      base ^= flip;

      /* This is legal if girlbit or boybit is on (in which case we use the appropriate datum)
         or if the two data are identical so the sex doesn't matter. */
      if ((qqq->girlbit | qqq->boybit) == 0 &&
               (callspec->stuff.matrix.stuff[base] != callspec->stuff.matrix.stuff[base+1]))
         fail("Can't determine sex of this person.");

      datum = callspec->stuff.matrix.stuff[base+qqq->girlbit];
      if (datum == 0) failp(qqq->id1, "can't do this call.");
      qqq->deltax = (((datum >> 7) & 0x1F) - 16) << 1;
      qqq->deltay = (((datum >> 2) & 0x1F) - 16) << 1;
      qqq->deltarot = datum & 3;
      qqq->rollinfo = (datum >> 12) * ROLLBITR;
      qqq->realdone = TRUE;
   }
   qqq->done = TRUE;
}



Private void make_matrix_chains(
   matrix_rec matrix_info[],
   int nump,
   long_boolean finding_far_squeezers,
   uint32 flags,
   int filter)                        /* 1 for E/W chains, 0 for N/S chains. */
{
   int i, j;

   /* Find adjacency relationships, and fill in the "se"/"nw" pointers. */

   for (i=0; i<nump; i++) {
      matrix_rec *mi = &matrix_info[i];

      if ((flags & MTX_IGNORE_NONSELECTEES) && (!mi->sel)) continue;

      for (j=0; j<nump; j++) {
         matrix_rec *mj = &matrix_info[j];
         int delx, dely;
         uint32 dirxor;
         int ix, jx, iy, jy;

         if ((flags & MTX_IGNORE_NONSELECTEES) && (!mj->sel)) continue;

         /* Find out if these people are adjacent in the right way. */

         if (flags & MTX_FIND_JAYWALKERS) {
            jx = mj->nicex;
            ix = mi->nicex;
            jy = mj->nicey;
            iy = mi->nicey;
         }
         else if (filter) {
            jx = - mj->y;
            ix = - mi->y;
            jy = - mj->x;
            iy = - mi->x;
         }
         else {
            jx = mj->x;
            ix = mi->x;
            jy = mj->y;
            iy = mi->y;
         }

         delx = ix - jx;
         dely = iy - jy;

         dirxor = mi->dir ^ mj->dir;

         if (flags & MTX_FIND_JAYWALKERS) {
            int jdist = (mi->dir & 1) ? delx : dely;
            if (!(mi->dir & 2)) jdist = -jdist;

            /* Jdist is the forward distance to the jaywalkee.  We demand that it be
               strictly positive, and minimal with respect to all other jaywalkee
               candidates.  We also consider only jaywalkees that are either facing
               back toward us or are facing lateral but are in such a position that
               they can see us. */

            if (jdist > 0 && jdist <= mi->nearest && dirxor != 0) {
/**** that dirxor test isn't really good enough */
               if (jdist < mi->nearest) {
                  /* This jaywalkee is closer than any others that we have seen; drop all others. */
                  mi->jbits = 0;
                  mi->nearest = jdist;  /* This is the new nearest distance. */
               }
               mi->jbits |= (1 << j);
            }
         }
         else {
            if (dely != (finding_far_squeezers ? 12 : 4) || delx != 0) continue;

            /* Now, if filter = 1, person j is just east of person i.
               If filter = 0, person j is just south of person i. */
   
            if (flags & MTX_TBONE_IS_OK) {
               /* Make the chain independently of facing direction. */
            }
            else {
               if ((mi->dir ^ filter) & 1) {
                  if (dirxor & 1) {
                     if (!(flags & MTX_STOP_AND_WARN_ON_TBONE)) fail("People are T-boned.");
                     mi->tbstopse = TRUE;
                     mj->tbstopnw = TRUE;
                     break;
                  }
                  else {
                     if ((flags & MTX_MUST_FACE_SAME_WAY) && dirxor)
                        fail("Paired people must face the same way.");
                  }
               }
               else
                  break;
            }

            if (mj->nextnw) fail("Internal error: adjacent to too many people.");

            mi->nextse = mj;
            mj->nextnw = mi;
            break;
         }
      }
   }
}

Private void process_matrix_chains(
   matrix_rec matrix_info[],
   int nump,
   callspec_block *callspec,
   uint32 flags,
   int filter)                        /* 1 for E/W chains, 0 for N/S chains. */
{
   long_boolean another_round = TRUE;
   long_boolean picking_end_jaywalkers = FALSE;
   int i, j;

   /* Pick out pairs of people and move them. */

   while (another_round) {
      another_round = FALSE;

      if (flags & MTX_FIND_JAYWALKERS) {
         /* Clear out any bits that aren't bidirectional. */
   
         for (i=0; i<nump; i++) {
            matrix_rec *mi = &matrix_info[i];
            if ((flags & MTX_IGNORE_NONSELECTEES) && (!mi->sel)) continue;
   
            for (j=0; j<nump; j++) {
               if (      ((1<<j) & mi->jbits) &&
                        !((1<<i) & matrix_info[j].jbits))
                  mi->jbits &= ~(1<<j);
            }
   
            /* If selected person has no jaywalkee, it is an error. */
            if (!mi->jbits) failp(mi->id1, "can't find person to jaywalk with.");
         }
      }

      for (i=0; i<nump; i++) {
         matrix_rec *mi = &matrix_info[i];

         if (!mi->done) {
            if (flags & MTX_FIND_JAYWALKERS) {

               if (!picking_end_jaywalkers) {
                  mi->leftidx = -1;
                  mi->rightidx = -1;
               }

               if (mi->jbits) {   /* If no jbits, person wasn't selected. */
                  for (j=0; j<nump; j++) {
                     matrix_rec *mj = &matrix_info[j];

                     if (picking_end_jaywalkers) {
                        /* Look for people who have been identified as extreme jaywalkers with each other. */
                        if (j == mi->leftidx && i == mj->rightidx) {
                           mi->jbits = (1<<j);    /* Separate them from the rest. */
                           mj->jbits = (1<<i);
                           another_round = TRUE;   /* We will move them on a future iteration. */
                        }
                     }
                     else {
                        int jdist;

                        /* Fill in jaywalkee indices with record-holding lateral distance. */
   
                        if ((1<<j) & mi->jbits) {
                           jdist = (mi->dir & 1) ? mj->nicey : mj->nicex;
                           if ((mi->dir+1) & 2) jdist = -jdist;
      
                           if (mi->leftidx >= 0) {
                              int temp = (mi->dir & 1) ? matrix_info[mi->leftidx].nicey : matrix_info[mi->leftidx].nicex;
                              if ((mi->dir+1) & 2) temp = -temp;
                              if (jdist < temp) mi->leftidx = j;
                           }
                           else
                              mi->leftidx = j;
      
                           if (mi->rightidx >= 0) {
                              int temp = (mi->dir & 1) ? matrix_info[mi->rightidx].nicey : matrix_info[mi->rightidx].nicex;
                              if ((mi->dir+1) & 2) temp = -temp;
                              if (jdist > temp) mi->rightidx = j;
                           }
                           else
                              mi->rightidx = j;
                        }
   
                        /* Look for special case of two people uniquely selecting each other. */
                        if (      (1<<j) == mi->jbits &&
                                  (1<<i) == mj->jbits) {
                           int delx = mj->x - mi->x;
                           int dely = mj->y - mi->y;

                           uint32 datum = callspec->stuff.matrix.stuff[mi->girlbit];
                           if (datum == 0) failp(mi->id1, "can't do this call.");

                           another_round = TRUE;
   
                           if (mi->dir & 2) delx = -delx;
                           if ((mi->dir+1) & 2) dely = -dely;

                           mi->deltax = (((datum >> 7) & 0x1F) - 16) << 1;
                           mi->deltay = (((datum >> 2) & 0x1F) - 16) << 1;
                           mi->deltarot = datum & 3;
                           mi->rollinfo = (datum >> 12) * ROLLBITR;
                           mi->realdone = TRUE;

                           mi->deltarot += (mj->dir - mi->dir + 2);
                           mi->deltarot &= 3;

                           if (mi->dir & 1) { mi->deltax += dely; mi->deltay = +delx; }
                           else             { mi->deltax += delx; mi->deltay = +dely; }

                           mi->done = TRUE;
                        }
                        /* Take care of anyone who has unambiguous jaywalkee.  Get that
                           jaywalkee's attention by stripping off all his other bits. */
                        else if ((1<<j) == mi->jbits) {
                           mj->jbits = (1<<i);
                           another_round = TRUE;   /* We will move them on a future iteration. */
                        }
                     }
                  }
               }
            }
            else if (mi->nextse) {
               /* This person might be ready to be paired up with someone. */
               if (mi->nextnw)
                  /* This person has neighbors on both sides.  Can't do anything yet. */
                  ;
               else {
                  /* This person has a neighbor on south/east side only. */

                  if (mi->tbstopnw) warn(warn__not_tbone_person);

                  if (   (!(flags & MTX_USE_SELECTOR))  ||  mi->sel   ) {
                     /* Do this pair.  First, chop the pair off from anyone else. */

                     if (mi->nextse->nextse) mi->nextse->nextse->nextnw = 0;
                     mi->nextse->nextse = 0;
                     another_round = TRUE;
                     do_pair(mi, mi->nextse, callspec, 0, filter);
                  }
               }
            }
            else if (mi->nextnw) {
               /* This person has a neighbor on north/west side only. */

               if (mi->tbstopse) warn(warn__not_tbone_person);

               if (   (!(flags & MTX_USE_SELECTOR))  ||  mi->sel   ) {
                  /* Do this pair.  First, chop the pair off from anyone else. */

                  if (mi->nextnw->nextnw) mi->nextnw->nextnw->nextse = 0;
                  mi->nextnw->nextnw = 0;
                  another_round = TRUE;
                  do_pair(mi, mi->nextnw, callspec, 2, filter);
               }
            }
            else {
               /* Person is alone.  If this is his lateral axis, mark him done and don't move him. */

               if ((mi->dir ^ filter) & 1) {
                  if (mi->tbstopse || mi->tbstopnw) warn(warn__not_tbone_person);
                  if (   (!(flags & MTX_USE_SELECTOR))  ||  mi->sel   )
                     failp(mi->id1, "has no one to work with.");
                  mi->done = TRUE;
               }
            }
         }
      }

      /* If anything happened, don't pick jaywalkers next time around. */

      if (another_round) picking_end_jaywalkers = FALSE;

      /* If doing a jaywalk, and nothing was processed, the leftest/rightest stuff
         is filled in, and we should check same. */

      if ((flags & MTX_FIND_JAYWALKERS) && !another_round && !picking_end_jaywalkers) {
         picking_end_jaywalkers = TRUE;
         another_round = TRUE;     /* Do another round this way. */
      }
   }
}




Private void partner_matrixmove(
   setup *ss,
   callspec_block *callspec,
   setup *result)
{
   uint32 flags;
   setup people;
   matrix_rec matrix_info[9];
   int i, nump;

   if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT)
      fail("Can't split the setup.");

   if (ss->cmd.cmd_misc_flags & CMD_MISC__DISTORTED)
      fail("This call not allowed in distorted or virtual setup.");

   flags = callspec->stuff.matrix.flags;

   nump = start_matrix_call(ss, matrix_info, flags, &people);

   /* Make the lateral chains first. */

   make_matrix_chains(matrix_info, nump, FALSE, flags, 1);
   if (flags & MTX_FIND_SQUEEZERS)
      make_matrix_chains(matrix_info, nump, TRUE, flags, 1);
   process_matrix_chains(matrix_info, nump, callspec, flags, 1);

   /* If jaywalking, don't do it again. */

   if (!(flags & MTX_FIND_JAYWALKERS)){
      /* Now clean off the pointers in preparation for the second pass. */

      for (i=0; i<nump; i++) {
         matrix_info[i].done = FALSE;
         matrix_info[i].nextse = 0;
         matrix_info[i].nextnw = 0;
         matrix_info[i].tbstopse = FALSE;
         matrix_info[i].tbstopnw = FALSE;
      }

      /* Vertical chains next. */

      make_matrix_chains(matrix_info, nump, FALSE, flags, 0);
      if (flags & MTX_FIND_SQUEEZERS)
         make_matrix_chains(matrix_info, nump, TRUE, flags, 0);
      process_matrix_chains(matrix_info, nump, callspec, flags, 0);
   }

   /* Scan for people who ought to have done something but didn't. */

   for (i=0; i<nump; i++) {
      if (!matrix_info[i].realdone) {
         if ((!(flags & MTX_USE_SELECTOR)) || matrix_info[i].sel)
            failp(matrix_info[i].id1, "could not identify other person to work with.");
      }
   }

   finish_matrix_call(matrix_info, nump, &people, result);
}


Private void rollmove(
   setup *ss,
   callspec_block *callspec,
   setup *result)
{
   int i;
   int rot;
   uint32 st;

   if (setup_attrs[ss->kind].setup_limits < 0) fail("Can't roll in this setup.");
   
   result->kind = ss->kind;
   result->rotation = ss->rotation;
   
   for (i=0; i<=setup_attrs[ss->kind].setup_limits; i++) {
      if (ss->people[i].id1) {
         rot = 0;
         st = ((uint32) stb_z)*DBSTAB_BIT; 
         if (!(callspec->callflagsf & CFLAGH__REQUIRES_SELECTOR) || selectp(ss, i)) {
            switch (ss->people[i].id1 & ROLL_MASK) {
               case ROLLBITL: rot = 033, st = ((uint32) stb_a)*DBSTAB_BIT; break;
               case ROLLBITM: break;
               case ROLLBITR: rot = 011; st = ((uint32) stb_c)*DBSTAB_BIT; break;
               default: fail("Roll not supported after previous call.");
            }
         }
         install_rot(result, i, ss, i, rot);

         if (result->people[i].id1 & STABLE_ENAB) {
            do_stability(&result->people[i].id1, (stability) ((st/DBSTAB_BIT) & 0xF), rot);
         }
      }
      else
         clear_person(result, i);
   }
}


Private long_boolean get_real_subcall(
   parse_block *parseptr,
   by_def_item *item,
   uint64 new_final_concepts,      /* The flags, heritable and otherwise, with which the parent call was invoked.
                                       Some of these may be inherited to the subcall. */
   callspec_block *parent_call,
   setup_command *cmd_out)         /* We fill in just the parseptr, callspec, cmd_final_flags fields. */

/* ****** Comment from long ago:  "Need to send out alternate_concept!!!"  I wonder what that meant? */

{
   char tempstring_text[MAX_TEXT_LINE_LENGTH];
   parse_block *search;
   parse_block **newsearch;
   int snumber;
   int item_id = item->call_id;
   uint32 mods1 = item->modifiers1;
   callspec_block *orig_call = base_calls[item_id];
   long_boolean this_is_tagger = item_id >= BASE_CALL_TAGGER0 && item_id <= BASE_CALL_TAGGER3;
   long_boolean this_is_tagger_circcer = this_is_tagger || item_id == BASE_CALL_CIRCCER;
   uint32 callflagsh = parent_call->callflagsh;  /* This has the "XXX_is_inherited" stuff from the parent. */
   uint32 this_modh = item->modifiersh;   /* The "inherit_XXX" and "force_XXX" flags for this invocation.
                                             Which it is (inherit or force) depends on whether the corresponding
                                             "XXX_is_inherited" top-level call flag is on in the parent, that is,
                                             whether the corresponding bit is on in "callflagsh". */

   /* Fill in defaults in case we choose not to get a replacement call. */

   cmd_out->parseptr = parseptr;
   cmd_out->callspec = orig_call;
   cmd_out->cmd_final_flags = new_final_concepts;

   /* If this context requires a tagging or scoot call, pass that fact on. */
   if (this_is_tagger) cmd_out->cmd_final_flags.final |= FINAL__MUST_BE_TAG;

   /* Strip out those concepts that do not have the "dfm__xxx" flag set saying that they are to be
      inherited to this part of the call.  BUT: the "INHERITFLAG_LEFT" flag controls
      both "INHERITFLAG_REVERSE" and "INHERITFLAG_LEFT", turning the former into the latter.  This makes reverse
      circle by, touch by, and clean sweep work. */

   /* If this subcall has "inherit_reverse" or "inherit_left" given, but the top-level call
      doesn't permit the corresponding flag to be given, we should turn any "reverse" or
      "left" modifier that was given into the other one, and cause that to be inherited.
      This is what turns, for example, part 3 of "*REVERSE* clean sweep" into a "*LEFT* 1/2 tag". */

   if (this_modh & ~callflagsh & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT)) {
      if (new_final_concepts.herit & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT))
         cmd_out->cmd_final_flags.herit |= (INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
   }

   /* Pass any "inherit" flags.  That is, turn off any that are NOT to be inherited.  Flags to be inherited
      are indicated by "this_modh" and "callflagsh" both on.  But we don't check "callflagsh", since, if it
      is off, we will force the bit immediately below. */

   cmd_out->cmd_final_flags.herit &= (~new_final_concepts.herit | this_modh);
   
   /* Now turn on any "force" flags.  These are indicated by "this_modh" on and "callflagsh" off. */

   if (((INHERITFLAG_REVERSE | INHERITFLAG_LEFT) & callflagsh) == 0)
      /* If neither of the "reverse_means_mirror" or "left_means_mirror" bits is on,
         we allow forcing of left or reverse. */
      cmd_out->cmd_final_flags.herit |= this_modh & ~callflagsh;
   else
      /* Otherwise, we only allow the other bits. */
      cmd_out->cmd_final_flags.herit |= this_modh & ~callflagsh & ~(INHERITFLAG_REVERSE | INHERITFLAG_LEFT);

   /* If this subcall invocation does not permit modification under any value of the
      "allowing_modifications" switch, we do nothing.  Just return the default.
      We do not search the list.  Hence, such subcalls are always protected
      from being substituted, and, if the same call appears multiple times
      in the derivation tree of a compound call, it will never be replaced.
      What do we mean by that?  Suppose we did a
         "[tally ho but [2/3 recycle]] the difference".
      After we create the table entry saying to change cast off 3/4 into 2/3
      recycle, we wouldn't want the cast off's in the difference getting
      modified.  Actually, that is not the real reason.  The casts are in
      different sublists.  The real reason is that the final part of mixed-up
      square thru is defined as 
         conc nullcall [mandatory_anycall] nullcall []
      and we don't want the unmodifiable nullcall that the ends are supposed to
      do getting modified, do we? */

   /* But if this is a tagging call substitution, we most definitely do proceed with the search. */

   if (!(mods1 & DFM1_CALL_MOD_MASK) && !this_is_tagger_circcer)
      return FALSE;

   /* See if we had something from before.  This avoids embarassment if a call is actually
      done multiple times.  We want to query the user just once and use that result everywhere.
      We accomplish this by keeping a subcall list showing what modifications the user
      supplied when we queried. */

   /* We ALWAYS search this list, if such exists.  Even if modifications are disabled.
      Why?  Because the reconciler re-executes calls after the point of insertion to test
      their fidelity with the new setup that results from the inserted calls.  If those
      calls have modified subcalls, we will find ourselves here, looking through the list.
      Modifications may have been enabled at the time the call was initially entered, but
      might not be now that we are being called from the reconciler. */

   if (parseptr->concept->kind == concept_another_call_next_mod) {
      newsearch = &parseptr->next;

      while ((search = *newsearch) != (parse_block *) 0) {
         if (  orig_call == search->call ||
               (this_is_tagger && search->call == base_calls[BASE_CALL_TAGGER0])) {
            /* Found a reference to this call. */
            parse_block *subsidiary_ptr = search->subsidiary_root;

            /* If the pointer is nil, we already asked about this call,
               and the reply was no. */
            if (!subsidiary_ptr) return FALSE;

            cmd_out->parseptr = subsidiary_ptr;

            if (this_is_tagger_circcer)
               cmd_out->callspec = subsidiary_ptr->call;
            else {
               cmd_out->callspec = (callspec_block *) 0;  /* ****** not right????. */
               cmd_out->cmd_final_flags.herit = 0;        /* ****** not right????. */
               cmd_out->cmd_final_flags.final = 0;        /* ****** not right????. */
            }

            return TRUE;
         }

         newsearch = &search->next;
      }
   }

   snumber = (mods1 & DFM1_CALL_MOD_MASK) / DFM1_CALL_MOD_BIT;

   /* Note whether we are using any mandatory substitutions, so that the menu
      initialization will always accept this call. */

   if (snumber == 2 || snumber == 6) mandatory_call_used = TRUE;

   /* Now we know that the list doesn't say anything about this call.  Perhaps we should
      query the user for a replacement and add something to the list.  First, decide whether
      we should consider doing so.  If we are initializing the
      database, the answer is always "no", even for calls that require a replacement call, such as
      "clover and anything".  This means that, for the purposes of database initialization,
      "clover and anything" is tested as "clover and nothing", since "nothing" is the subcall
      that appears in the database. */
      
   /* Of course, if we are testing the fidelity of later calls during a reconcile
      operation, we DO NOT EVER add any modifiers to the list, even if the user
      clicked on "allow modification" before clicking on "reconcile".  It is perfectly
      legal to click on "allow modification" before clicking on "reconcile".  It means
      we want modifications (chosen by random number generator, since we won't be
      interactive) for the calls that we randomly choose, but not for the later calls
      that we test for fidelity. */

   if (interactivity == interactivity_database_init || interactivity == interactivity_verify || testing_fidelity) return FALSE;

   /* When we are searching for resolves and the like, the situation is different.  In this case,
      the interactivity state is set for a search.  We do perform mandatory
      modifications, so we will generate things like "clover and shakedown".  Of course, no
      querying actually takes place.  Instead, get_subcall just uses the random number generator.
      Therefore, whether resolving or in normal interactive mode, we are guided by the
      call modifier flags and the "allowing_modifications" global variable. */

   /* Depending on what type of substitution is involved and what the "allowing modifications"
      level is, we may choose not to query about this subcall, but just return the default. */

   switch (snumber) {
      case 1:   /* or_anycall */
      case 3:   /* allow_plain_mod */
      case 5:   /* or_secondary_call */
         if (!allowing_modifications) return FALSE;
         break;
      case 4:   /* allow_forced_mod */
         if (allowing_modifications <= 1) return FALSE;
         break;
   }

   /* At this point, we know we should query the user about this call. */
      
   /* Set ourselves up for modification by making the null modification list
      if necessary.  ***** Someday this null list will always be present. */

   if (parseptr->concept->kind == marker_end_of_list) {
      parseptr->concept = &marker_concept_mod;
      newsearch = &parseptr->next;
   }
   else if (parseptr->concept->kind != concept_another_call_next_mod)
      fail("wrong marker in get_real_subcall???");

   /* Create a reference on the list.  "search" points to the null item at the end. */

   tempstring_text[0] = '\0';           /* Null string, just to be safe. */

   /* If doing a tagger, just get the call. */

   if (snumber == 0 && this_is_tagger_circcer)
      ;

   /* If the replacement is mandatory, or we are not interactive,
      don't present the popup.  Just get the replacement call. */

   else if (interactivity != interactivity_normal)
      ;
   else if (snumber == 2 || snumber == 6) {
      (void) sprintf (tempstring_text, "SUBSIDIARY CALL");
   }
   else {

      /* Need to present the popup to the operator and find out whether modification is desired. */

      modify_popup_kind kind;

      if (this_is_tagger) kind = modify_popup_only_tag;
      else if (this_is_tagger_circcer) kind = modify_popup_only_circ;
      else kind = modify_popup_any;

      if (debug_popup || uims_do_modifier_popup(orig_call->menu_name, kind)) {
         /* User accepted the modification.
            Set up the prompt and get the concepts and call. */
      
         (void) sprintf (tempstring_text, "REPLACEMENT FOR THE %s", orig_call->menu_name);
      }
      else {
         /* User declined the modification.  Create a null entry so that we don't query again. */
         *newsearch = get_parse_block();
         (*newsearch)->concept = &marker_concept_mod;
         (*newsearch)->options.number_fields = mods1;
         (*newsearch)->call = orig_call;
         return FALSE;
      }
   }

   *newsearch = get_parse_block();
   (*newsearch)->concept = &marker_concept_mod;
   (*newsearch)->options.number_fields = mods1;
   (*newsearch)->call = orig_call;

   /* Set stuff up for reading subcall and its concepts. */

   /* Create a new parse block, point concept_write_ptr at its contents. */
   /* Create the new root at the start of the subsidiary list. */

   parse_state.concept_write_base = &(*newsearch)->subsidiary_root;
   parse_state.concept_write_ptr = parse_state.concept_write_base;

   parse_state.parse_stack_index = 0;
   parse_state.call_list_to_use = call_list_any;
   (void) strncpy(parse_state.specialprompt, tempstring_text, MAX_TEXT_LINE_LENGTH);

   /* Search for special case of "must_be_tag_call" with no other modification bits.
      That means it is a new-style tagging call. */

   if (snumber == 0 && this_is_tagger_circcer) {
      longjmp(longjmp_ptr->the_buf, 5);
   }
   else {
      if (query_for_call())
         longjmp(longjmp_ptr->the_buf, 5);     /* User clicked on something unusual like "exit" or "undo". */
   }

   cmd_out->parseptr = (*newsearch)->subsidiary_root;
   cmd_out->callspec = (callspec_block *) 0;    /* We THROW AWAY the alternate call, because we want our user to get it from the concept list. */
   cmd_out->cmd_final_flags.herit = 0;
   cmd_out->cmd_final_flags.final = 0;
   return TRUE;
}


Private void divide_diamonds(setup *ss, setup *result)
{
   int ii;

   if (ss->kind == s_qtag) ii = 1;
   else if (ss->kind == s_ptpd) ii = 0;
   else fail("Must have diamonds for this concept.");

   divided_setup_move(ss, map_lists[sdmd][1]->f[MPKIND__SPLIT][ii], phantest_ok, FALSE, result);
}



/* The fraction stuff is encoded into 6 hexadecimal digits, which we will call
   digits 3 through 8:

      Late-breaking news:  digit 2 is now used.  The bit 0x01000000 means
      "this is 'initially' or 'finally', and I promise to invoke all parts
      of the call, even though I appear at present to be invoking just a
      subset."  It is used to allow 'initially' and 'finally' to be stacked
      with themselves.

      digit 3 - the code (3 bits) and the reversal bit (1 bit)
                  Note that the code*2 is what is visible in this digit.
                  If this digit is odd, the reversal bit is on.  The code
                  indicates what kind of special job we are doing.
      digit 4 - the selected part, called "N".  Think of this as being 1-based,
                  even though the part numbering stuff that comes out of this
                  procedure will be zero-based.  This will always be nonzero
                  when some nontrivial job is being done.  the meaning of the
                  codes has been defined so that no job ever requires N to be zero.
                  The only time N is zero is when we are not doing anything, in
                  which case the code is zero.  Code = N = 0 means no special
                  job is being done.  (Though reversal and nontrivial fractions
                  may still be present.)
      digits 5 and 6 - the numerator and denominator, respectively, of the
                  start point of the call, always in reduced-fraction form.
                  The default is [0,1] meaning to start at 0.0000, that is, the
                  beginning.
      digits 7 and 8 - the numerator and denominator, respectively, of the
                  end point of the call, always in reduced-fraction form.
                  The default is [1,1] meaning to end at 1.0000, that is, the
                  end.

   The default configuration is 000111 hex, meaning do not reverse, go from
   0.0000 to 1.0000, and don't do any special job.  A word of zero means the same
   thing, and the fraction flag word is initialized to 0 if no fractions are being
   used.  This makes it easier to tell whether any fractions are being used.
   The zero is changed to 000111 hex at the start of any fraction-manipulation
   operation.

   The meaning of all this stuff is as follows:

   First, if the reversal bit is on, the target call (or target random crazy
   call, or whatever) has its parts reversed.

   Second, the fraction stuff, determined by the low 4 hexadecimal digits, is
   applied to the possibly reversed call.  This creates the "region".

   Third, the region is manipulated according to the key and the value of N.

   Before describing the key actions, here is how the region is computed.

      Suppose the call has 10 parts: A, B, C, D, E, F, G, H, I, and J, and that
      the low 4 digits of the fraction word are 1535.  This means that the start
      point is 1/5 and the end point is 3/5.  So the first 1/5 of the call (2 parts,
      A and B) are chopped off, and the last 2/5 of the call (4 parts, G, H, I, and J)
      are also chopped off.  The region is C, D, E, and F.  This effect could be
      obtained by saying "do the last 2/3, 3/5, <call>" or "1/2, do the last 4/5, <call>".

      Now suppose that the "reverse" bit is also on.  This reverses the parts BEFORE
      the fractions are computed.  The call is considered to consist of J, I, H, G, F,
      E, D, C, B, and A.  If the low 4 digits are 1535, this chops off J and I at
      the beginning, and D, C, B, and A at the end.  The region is thus H, G, F, and E.
      This could be obtained by saying "do the last 2/3, 3/5, reverse order, <call>",
      or "reverse order, 2/3, do the last 3/5, <call>", or "1/2, do the last 4/5,
      reverse order, <call>", etc.

   What happens next depends on the key and the value of "N", but always applies just
   to the region as though the region were the only thing under consideration.

   If "N" is zero (and the key is zero also, of course), the region is simply
   executed from beginning to end.

   Otherwise, the parts of the region are considered to be numbered in a 1-based
   fashion.  In the case above of the "reverse" bit being on, the low 4 digits being
   1535 hex, and the call having 10 parts, the region was H, G, F, and E.  These
   are numbered:
      H = 1
      G = 2
      F = 3
      E = 4

   Let T be the number of parts in the region.  In this example, T is 4.

   The meanings of the key values are as follows:
      (What you see in the digit is twice this,
      because the reversal bit is at the bottom.)

      key = 0 (but N != 0) - do part N only.

      key = 1 - obsolete -- use 4 instead.

      key = 2 - do parts from the beginning of the region up through N, inclusive,
                  that is, 1, 2, 3, .... N.

      key = 3 - [PLANNED, NOT IMPLEMENTED] - do parts from N down to 1, inclusive,
                  that is, N, N-1, ..... 2, 1.

      key = 4 - do parts from N+1 up to the end of the region,
                  that is, N, N+1, ..... T-1, T.

      key = 5 - do parts from the top of the region down to N+1,
                  that is, T, T-1, ..... N+2, N+1.  This executes the region
                  in reverse order.  Of course, if the "reverse" bit is on,
                  the region represents the call in reverse order, so the parts
                  of the call will be executed in forward order.


   The output of this procedure, after digesting the above, is a "fraction_info"
   structure, whose important items are:

      subcall_index - the first part of the target call that we are to execute.
         This is now in ZERO-BASED numbering, as befits the sequential-definition
         array.
      highlimit - if going forward, this is the (zero-based) part JUST AFTER the
         last part that we are to execute.  We will execute (highlimit-subcall_index)
         parts.  If going backward, this is the (zero-based) LAST PART that we will
         execute.  We will execute (subcall_index-highlimit+1) parts.  Note the
         assymetry.  Sorry.

   So, to execute all of a 10 part call in forward order, we will have
      subcall_index = 0    and    highlimit = 10
   To execute it in reverse order, we will have
      subcall_index = 9    and    highlimit = 0

   The "instant_stop" and "do_half_of_last_part" flags do various things.
*/


extern fraction_info get_fraction_info(uint32 frac_flags, uint32 callflags1, int total)
{
   fraction_info retval;
   int e_numer, e_denom, s_numer, s_denom, this_part, test_size;
   int subcall_index, highlimit;

   int available_fractions = (callflags1 & CFLAG1_VISIBLE_FRACTION_MASK) / CFLAG1_VISIBLE_FRACTION_BIT;
   if (available_fractions == 3) available_fractions = 1000;     /* 3 means all parts. */

   retval.reverse_order = 0;
   retval.instant_stop = 0;
   retval.do_half_of_last_part = 0;

   this_part = (frac_flags & 0xF0000) >> 16;
   s_numer = (frac_flags & 0xF000) >> 12;      /* Start point. */
   s_denom = (frac_flags & 0xF00) >> 8;
   e_numer = (frac_flags & 0xF0) >> 4;         /* End point. */
   e_denom = (frac_flags & 0xF);

   if (s_numer >= s_denom) fail("Fraction must be proper.");
   subcall_index = total * s_numer;
   if ((subcall_index % s_denom) != 0) fail("This call can't be fractionalized with this fraction.");
   subcall_index = subcall_index / s_denom;

   if (e_numer <= 0) fail("Fraction must be proper.");
   highlimit = total * e_numer;
   test_size = highlimit / e_denom;

   if (test_size*e_denom == highlimit)
      highlimit = test_size;
   else {
      if (2*test_size*e_denom + e_denom == 2*highlimit) {
         highlimit = test_size+1;
         retval.do_half_of_last_part = 1;
      }
      else
         fail("This call can't be fractionalized with this fraction.");
   }

   /* Now subcall_index is the start point, and highlimit is the end point. */

   if (subcall_index >= highlimit || highlimit > total)
      fail("Fraction must be proper.");

   /* Check for "reverse order" */
   if (frac_flags & CMD_FRAC_REVERSE) {
      retval.reverse_order = 1;
      subcall_index = total-1-subcall_index;
      highlimit = total-highlimit;
      if (retval.do_half_of_last_part) fail("This call can't be fractionalized with this fraction.");
   }

   if (this_part != 0) {
      /* In addition to everything else, we are picking out a specific part
         of whatever series we have decided upon. */

      if (retval.do_half_of_last_part) fail("This call can't be fractionalized with this fraction.");

      switch ((frac_flags & CMD_FRAC_CODE_MASK) / CMD_FRAC_CODE_BIT) {
         case 0:
            retval.instant_stop = 1;
                     subcall_index += retval.reverse_order ? (1-this_part) : (this_part-1);
                     /* Be sure that enough parts are visible. */
                     if (subcall_index+1 > available_fractions)
                        fail("This call can't be fractionalized.");
                     if (subcall_index >= total) fail("The indicated part number doesn't exist.");
            break;
         case 2:
            /* We are not just doing part N, we are doing parts up through N. */

            if (retval.reverse_order) {
               if (highlimit > subcall_index-this_part+1) fail("This call can't be fractionalized this way.");
               highlimit = subcall_index-this_part+1;
               if (subcall_index > available_fractions) fail("This call can't be fractionalized.");
               if (subcall_index > total) fail("The indicated part number doesn't exist.");
            }
            else {
               if (highlimit < subcall_index+this_part) fail("This call can't be fractionalized this way.");
               highlimit = subcall_index+this_part;
               if (highlimit > available_fractions) fail("This call can't be fractionalized.");
               if (highlimit > total) fail("The indicated part number doesn't exist.");
            }

            break;
         case 1: case 4:
            /* We are not just doing part N, we are doing parts strictly after N. */
                     subcall_index += retval.reverse_order ? (-this_part) : (this_part);
                     /* Be sure that enough parts are visible. */
                     if (subcall_index > available_fractions)
                        fail("This call can't be fractionalized.");
                     if (subcall_index > total) fail("The indicated part number doesn't exist.");
            break;
         case 5:
            if (retval.reverse_order) {
               int t = subcall_index;
               subcall_index = highlimit;
               highlimit = t+1-this_part;
/* ***** need error checks */
               retval.reverse_order = 0;
            }
            else {
               int t = subcall_index;
               subcall_index = highlimit-1;
               highlimit = t+this_part;
/* ***** need error checks */
               retval.reverse_order = 1;
            }

            break;
         default:
            fail("Internal error: bad fraction code.");
      }
   }
   else {
      /* Unless all parts are visible, this is illegal. */
      if (available_fractions != 1000)
         fail("This call can't be fractionalized.");
   }

   retval.subcall_index = subcall_index;
   retval.highlimit = highlimit;
   return retval;
}



Private void do_sequential_call(
   setup *ss,
   callspec_block *callspec,
   long_boolean qtfudged,
   long_boolean *mirror_p,
   setup *result)
{
   long_boolean qtf;
   int fetch_index;
   int dist_index;
   uint64 new_final_concepts = ss->cmd.cmd_final_flags;
   int *test_index = &fetch_index;
   parse_block *parseptr = ss->cmd.parseptr;
   uint32 callflags1 = callspec->callflags1;
   int instant_stop = 99;  /* If not 99, says to stop instantly after doing one part, and to report
                              (in RESULTFLAG__DID_LAST_PART bit) if that part was the last part. */
   long_boolean reverse_order = FALSE;
   int subcall_incr = 1;   /* Will be -1 if doing call in reverse order. */
   long_boolean do_half_of_last_part = FALSE;
   long_boolean first_call = TRUE;
   int realtotal = callspec->stuff.def.howmanyparts;
   int total = realtotal;
   int start_point = 0;    /* Where we start, in the absence of special stuff. */
   int end_point;  /* Where we end, in the absence of special stuff. */
   int highlimit;
   long_boolean distribute = FALSE;
   /* This tells whether the setup was genuinely elongated when it came in.  We keep track of pseudo-elongation
      during the call even when it wasn't, but sometimes we really need to know. */
   long_boolean setup_is_elongated = (ss->kind == s2x2 || ss->kind == s_short6) && ss->cmd.prior_elongation_bits != 0;
   int remembered_2x2_elongation = 0;
   int subpart_count = 0;
   uint32 restrained_fraction = 0;

   if (callflags1 & CFLAG1_DISTRIBUTE_REPETITIONS) distribute = TRUE;

   if (distribute) {
      int ii;
      total = 0;
      for (ii=0 ; ii<callspec->stuff.def.howmanyparts ; ii++) {
         by_def_item *this_item = &callspec->stuff.def.defarray[ii];
         uint32 this_mod1 = this_item->modifiers1;

         if ((DFM1_REPEAT_N | DFM1_REPEAT_NM1 | DFM1_REPEAT_N_ALTERNATE) & this_mod1) {
            total += (current_options.number_fields >> (((DFM1_NUM_SHIFT_MASK & this_mod1) / DFM1_NUM_SHIFT_BIT) * 4)) & 0xF;

            if (DFM1_REPEAT_N_ALTERNATE & this_mod1) ii++;
            if (DFM1_REPEAT_NM1 & this_mod1) total--;
         }
         else
            total++;
      }
      test_index = &dist_index;
   }

   highlimit = total;

   /* If the "cmd_frac_flags" word is nonzero, we are being asked to do something special. */

   if (ss->cmd.cmd_frac_flags) {
      fraction_info zzz;

      /* If we are doing something under a craziness restraint, take out the low 16 bits of the
         fraction stuff -- they aren't meant for us -- and pass them to the subject call later. */

      if (ss->cmd.cmd_misc_flags & CMD_MISC__RESTRAIN_CRAZINESS) {
         restrained_fraction = ss->cmd.cmd_frac_flags;
         ss->cmd.cmd_frac_flags = (ss->cmd.cmd_frac_flags & ~0xFFFF) | 0x0111;
      }

      zzz = get_fraction_info(ss->cmd.cmd_frac_flags, callflags1, total);
      reverse_order = zzz.reverse_order;
      do_half_of_last_part = zzz.do_half_of_last_part;
      highlimit = zzz.highlimit;
      if (reverse_order) {
         highlimit = 1-highlimit;
         subcall_incr = -1;
      }
      start_point = zzz.subcall_index;
      if (zzz.instant_stop) instant_stop = start_point*subcall_incr+1;
      if (reverse_order && !zzz.instant_stop) first_call = FALSE;
   }

   ss->cmd.cmd_misc_flags &= ~CMD_MISC__RESTRAIN_CRAZINESS;

   if (new_final_concepts.final & FINAL__SPLIT) {
      if (callflags1 & CFLAG1_SPLIT_LIKE_SQUARE_THRU)
         new_final_concepts.final |= FINAL__SPLIT_SQUARE_APPROVED;
      else if (callflags1 & CFLAG1_SPLIT_LIKE_DIXIE_STYLE)
         new_final_concepts.final |= FINAL__SPLIT_DIXIE_APPROVED;
   }

   qtf = qtfudged;

   if (ss->kind != s2x2 && ss->kind != s_short6) ss->cmd.prior_elongation_bits = 0;

   /* Did we neglect to do the touch/rear back stuff because fractionalization was enabled?
      If so, now is the time to correct that.  We only do it for the first part, and only if
      doing parts in forward order. */

   /* Test for all this is "random left, swing thru".
      The test cases for this stuff are such things as "left swing thru". */

   if ((!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_STEP_TO_WAVE)) && (start_point == 0) && !reverse_order &&
         (callflags1 & (CFLAG1_REAR_BACK_FROM_R_WAVE | CFLAG1_REAR_BACK_FROM_QTAG | CFLAG1_STEP_TO_WAVE))) {

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;  /* Can only do it once. */

      if (new_final_concepts.herit & INHERITFLAG_LEFT) {
         if (!*mirror_p) mirror_this(ss);
         *mirror_p = TRUE;
      }

      touch_or_rear_back(ss, *mirror_p, callflags1);
   }

   /* See comment earlier about mirroring.  For sequentially or concentrically defined
      calls, the "left" flag does not mean mirror; it is simply passed to subcalls.
      So we must put things into their normal state.  If we did any mirroring, it was
      only to facilitate the action of "touch_or_rear_back". */

   if (*mirror_p) mirror_this(ss);
   *mirror_p = FALSE;
   *result = *ss;

   result->result_flags = RESULTFLAG__SPLIT_AXIS_MASK;   /* Seed the result for the calls to "do_call_in_series". */

   /* Iterate over the parts of the call.
      We will let "fetch_index" scan the actual call definition:
         forward - from 0 to realtotal-1 inclusive
         reverse - from realtotal-1 down to 0 inclusive.
      While doing this, we will let "test_index" scan the parts of the
      call as seen by the fracionalization stuff.  If we are not distributing
      parts, "test_index" will be the same as "fetch_index".  Otherwise,
      it will show the distributed subparts. */

   if (reverse_order) {
      fetch_index = distribute ? realtotal-1 : start_point;
      dist_index = total-1;
      end_point = -highlimit+1;
   }
   else {
      fetch_index = distribute ? 0 : start_point;
      dist_index = 0;
      end_point = highlimit-1;
   }

   for (;;) {
      by_def_item *this_item;
      uint32 this_mod1;
      uint32 remember_elongation;
      setup_command foo1, foo2;
      by_def_item *alt_item;
      int use_alternate;
      setup_kind oldk;
      long_boolean recompute_id;
      uint32 saved_number_fields = current_options.number_fields;

      /* Now the "index" values (fetch_index and dist_index) contain the
         number of parts we have completed.  That is, they point (in 0-based
         numbering) to what we are about to do.  Also, if "subpart_count" is
         nonzero, it has the number of extra repetitions of what we just did
         that we must perform before we can go on to the next thing. */

      if (subpart_count) {
         subpart_count--;    /* This is now the number of EXTRA repetitions
                                 of this that we will still have to do after
                                 we do the repetition that we are about to do. */

         use_alternate ^= 1;
         dist_index += subcall_incr;
         goto do_plain_call;
      }


      if (reverse_order) {
         if (fetch_index < 0) break;
      }
      else {
         if (fetch_index >= realtotal) break;
      }

      if (fetch_index >= realtotal || fetch_index < 0) fail("The indicated part number doesn't exist.");
      this_item = &callspec->stuff.def.defarray[fetch_index];
      this_mod1 = this_item->modifiers1;

      fetch_index += subcall_incr;
      dist_index += subcall_incr;

      if (reverse_order) {
         if (fetch_index >= 0 && (DFM1_REPEAT_N_ALTERNATE & callspec->stuff.def.defarray[fetch_index].modifiers1)) {
            alt_item = this_item;
            this_item = &callspec->stuff.def.defarray[fetch_index];
            this_mod1 = this_item->modifiers1;
            fetch_index--;
         }
      }
      else {
         if (DFM1_REPEAT_N_ALTERNATE & this_mod1) {
            alt_item = &callspec->stuff.def.defarray[fetch_index];
            fetch_index++;
         }
      }

      /* If we are not distributing, perform the range test now, so we don't
         query the user needlessly about parts of calls that we won't do. */

      if (!distribute) {
         if (reverse_order) {
            if (*test_index+1 > start_point)
               continue;
            if (*test_index+1 < end_point)
               break;
         }
         else {
            if (*test_index-1 < start_point)
               continue;
            if (*test_index-1 > end_point)
               break;
         }
      }

      /* If an explicit substitution was made, we will recompute the ID bits for the setup.  Normally, we don't,
         which is why "patch the <anyone>" works.  The original evaluation of the designees is retained after
         the first part of the call.  But if the user does something like "circle by 1/4 x [leads run]", we
         want to re-evaluate who the leads are. */

      recompute_id = get_real_subcall(parseptr, this_item, new_final_concepts, callspec, &foo1);

      if (DFM1_REPEAT_N_ALTERNATE & this_mod1)
         (void) get_real_subcall(parseptr, alt_item, new_final_concepts, callspec, &foo2);

      if (recompute_id) update_id_bits(result);

      current_options.number_fields >>= ((DFM1_NUM_SHIFT_MASK & this_mod1) / DFM1_NUM_SHIFT_BIT) * 4;

      /* Check for special repetition stuff. */
      if ((DFM1_REPEAT_N | DFM1_REPEAT_NM1 | DFM1_REPEAT_N_ALTERNATE) & this_mod1) {
         int count_to_use = current_options.number_fields & 0xF;

         number_used = TRUE;
         if (DFM1_REPEAT_NM1 & this_mod1) count_to_use--;

         if (do_half_of_last_part && !distribute && fetch_index == highlimit) {
            if (count_to_use & 1) fail("Can't fractionalize this call this way.");
            count_to_use >>= 1;
         }

         use_alternate = reverse_order && !(count_to_use & 1);
         subpart_count = count_to_use;
         if (subpart_count == 0) goto done_with_big_cycle;
         subpart_count--;
      }

      remember_elongation = result->cmd.prior_elongation_bits;

do_plain_call:

      /* The index point AFTER what we are about to do (0-based numbering, of course),
         so index-1 (or index+1) point to what we are about to do.  Subpart_count has the
         number of ADDITION repetitions of what we are about to do, after we finish the
         upcoming one. */

      if (reverse_order) {
         if (*test_index+1 > start_point)
            continue;
         if (*test_index+1 < end_point)
            break;
      }
      else {
         if (*test_index-1 < start_point)
            continue;
         if (*test_index-1 > end_point)
            break;
      }

      result->cmd = ss->cmd;
      result->cmd.prior_elongation_bits = remember_elongation;

      /* We don't supply these; they get filled in by the call. */
      result->cmd.cmd_misc_flags &= ~DFM1_CONCENTRICITY_FLAG_MASK;

      if (do_half_of_last_part && *test_index == highlimit)
         result->cmd.cmd_frac_flags = 0x000112UL;
      else
         result->cmd.cmd_frac_flags = 0;

      if (restrained_fraction) {
         if (result->cmd.cmd_frac_flags) fail("Random/piecewise is too complex.");
         result->cmd.cmd_frac_flags = restrained_fraction & 0xFFFF;
         if (result->cmd.cmd_frac_flags == 0x000111UL)
            result->cmd.cmd_frac_flags = 0;
      }

      if (!first_call) {
         if (!setup_is_elongated)
            result->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;  /* Stop checking unless we are really serious. */
         result->cmd.cmd_assume.assumption = cr_none;
      }

      if ((DFM1_REPEAT_N_ALTERNATE & this_mod1) && use_alternate) {
         result->cmd.parseptr = foo2.parseptr;
         result->cmd.callspec = foo2.callspec;
         result->cmd.cmd_final_flags = foo2.cmd_final_flags;

      }
      else {
         result->cmd.parseptr = foo1.parseptr;
         result->cmd.callspec = foo1.callspec;
         result->cmd.cmd_final_flags = foo1.cmd_final_flags;
      }

      if ((DFM1_CPLS_UNLESS_SINGLE & this_mod1) && !(new_final_concepts.herit & INHERITFLAG_SINGLE))
         result->cmd.cmd_misc_flags |= CMD_MISC__DO_AS_COUPLES;

      oldk = result->kind;
      if (oldk == s2x2 && result->cmd.prior_elongation_bits != 0) remembered_2x2_elongation = result->cmd.prior_elongation_bits & 3;

      do_call_in_series(
         result,
         DFM1_ROLL_TRANSPARENT & this_mod1,
         !(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) &&
            !(new_final_concepts.herit & (INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX)),
         qtf);

      if (oldk != s2x2 && result->kind == s2x2 && remembered_2x2_elongation != 0) {
         result->result_flags = (result->result_flags & ~3) | remembered_2x2_elongation;
         result->cmd.prior_elongation_bits = (result->cmd.prior_elongation_bits & ~3) | remembered_2x2_elongation;
      }

      if (subpart_count && !distribute) continue;

done_with_big_cycle:

      /* We allow expansion on the first part, and then shut it off for later parts.
         This is required for things like 12 matrix grand swing thru from a 1x8 or 1x10. */
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

      /* This really isn't right.  It is done in order to make "ENDS FINISH set back" work.
         The flaw is that, if the sequentially defined call "FINISH set back" had more than one
         part, we would be OR'ing the bits from multiple parts.  What would it mean?  The bits
         we are interested in are the "demand_lines" and "force_lines" stuff.  I guess we should
         take the "demand" bits from the first part and the "force" bits from the last part.  Yuck! */

      /* The claim is that the following code removes the above problem.  The test is "ends 2/3 chisel thru".
         Below, we will pick up the concentricity flags from the last subcall. */

      ss->cmd.cmd_misc_flags |= result->cmd.cmd_misc_flags & ~DFM1_CONCENTRICITY_FLAG_MASK;

      current_options.number_fields = saved_number_fields;

      qtf = FALSE;

      new_final_concepts.final &= ~(FINAL__SPLIT | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED);

      first_call = FALSE;

      /* If we are being asked to do just one part of a call (from cmd_frac_flags),
         exit now.  Also, see if we just did the last part. */

      if (ss->cmd.cmd_frac_flags && instant_stop != 99) {
         /* Check whether we honored the last possible request.  That is,
            whether we did the last part of the call in forward order, or
            the first part in reverse order. */
         result->result_flags |= RESULTFLAG__PARTS_ARE_KNOWN;
         if (instant_stop >= highlimit)
            result->result_flags |= RESULTFLAG__DID_LAST_PART;
         break;
      }
   }

   /* Pick up the concentricity command stuff from the last thing we did, but take out the effect of "splitseq". */

   ss->cmd.cmd_misc_flags |= result->cmd.cmd_misc_flags;
   ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;
}


/* Check for a schema that we weren't sure about, and fix it up. */

Private calldef_schema fixup_conc_schema(callspec_block *callspec, setup *ss)
{
   calldef_schema the_schema = callspec->schema;
   uint32 herit_concepts = ss->cmd.cmd_final_flags.herit;

   if (the_schema == schema_maybe_single_concentric) {
      if (herit_concepts & INHERITFLAG_SINGLE)
         the_schema = schema_single_concentric;
      else
         the_schema = schema_concentric;
   }
   else if (the_schema == schema_maybe_grand_single_concentric) {
      if (herit_concepts & INHERITFLAG_GRAND) {
         if (herit_concepts & INHERITFLAG_SINGLE)
            the_schema = schema_grand_single_concentric;
         else
            fail("You must not use \"grand\" without \"single\".");
      }
      else {
         if (herit_concepts & INHERITFLAG_SINGLE)
            the_schema = schema_single_concentric;
         else
            the_schema = schema_concentric;
      }
   }
   else if (the_schema == schema_maybe_single_cross_concentric)
      the_schema = (herit_concepts & INHERITFLAG_SINGLE) ? schema_single_cross_concentric : schema_cross_concentric;
   else if (the_schema == schema_maybe_4x2_concentric)
      the_schema = (setup_attrs[ss->kind].setup_limits == 5) ? schema_concentric_4_2 : schema_concentric;
   else if (the_schema == schema_maybe_matrix_conc_star) {
      if (herit_concepts & INHERITFLAG_12_MATRIX)
         the_schema = schema_conc_star12;
      else if (herit_concepts & INHERITFLAG_16_MATRIX)
         the_schema = schema_conc_star16;
      else
         the_schema = schema_conc_star;
   }

   return the_schema;
}


/* This leaves the split axis result bits in absolute orientation. */

Private void move_with_real_call(
   setup *ss,
   long_boolean qtfudged,
   setup *result)
{
   warning_info saved_warnings;
   uint32 tbonetest;
   uint32 imprecise_rotation_result_flag = 0;
   uint32 unaccepted_flags;
   calldef_schema the_schema;
   long_boolean mirror;
   callspec_block *callspec = ss->cmd.callspec;
   uint32 callflags1 = callspec->callflags1;

   clear_people(result);
   result->result_flags = 0;   /* In case we bail out. */

   if (ss->kind == nothing) {
      if (ss->cmd.cmd_frac_flags)
         fail("Can't fractionalize a call if no one is doing it.");
      result->kind = nothing;
      return;
   }

   /* We have a genuine call.  Presumably all serious concepts have been disposed of
      (that is, nothing interesting will be found in parseptr -- it might be
      useful to check that someday) and we just have the callspec and the final
      concepts. */



/* ***** Beware!!!!  There are still some checks for
   if (ss->cmd.cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
that probably need to be put in. */


   the_schema = fixup_conc_schema(callspec, ss);

   /* Check for "central" concept and its ilk, and pick up correct definition. */

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK) {
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX | CMD_MISC__DISTORTED;

      /* If we invert centers and ends parts, we don't raise errors
         for bad elongation if "suppress_elongation_warnings" was set for the centers part.
         This allows horrible "ends trade" on "invert acey deucey", for example,
         since "acey deucey" has that flag set for the trade that the centers do. */

      if ((ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_INVERT) && the_schema == schema_concentric &&
            (DFM1_SUPPRESS_ELONGATION_WARNINGS & callspec->stuff.conc.innerdef.modifiers1))
         ss->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;

      /* We shut off the "doing ends" stuff.  If we say "ends detour" we mean "ends do the ends part of
         detour".  But if we say "ends central detour" we mean "ends do the *centers* part of detour". */
      ss->cmd.cmd_misc_flags &= ~CMD_MISC__DOING_ENDS;
   
      /* Now we demand that, if a concept was given, the call had the appropriate flag set saying
         that the concept is legal and will be inherited to the children. */
   
      if (ss->cmd.cmd_final_flags.herit & (~callspec->callflagsh)) fail("Can't do this call with this concept.");

      switch (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_KMASK) {
         case CMD_MISC2__CENTRAL_PLAIN:
            /* If it is sequential, we just pass it through.  Otherwise, we handle it here. */
            if (the_schema != schema_sequential) {
               uint32 temp_concepts, forcing_concepts;
               by_def_item *defptr;
               uint32 inv_bits = ss->cmd.cmd_misc2_flags & (CMD_MISC2__CTR_END_INV_CONC | CMD_MISC2__CTR_END_INVERT);

               /* Normally, we get the centers' part of the definition.  But if the user said either
                  "invert central" (the concept that means to get the ends' part) or "central invert"
                  (the concept that says to get the centers' part of the inverted call) we get the ends'
                  part.  If BOTH inversion bits are on, the user said "invert central invert", meaning to
                  get the ends' part of the inverted call, so we just get the centers' part as usual. */

               if (inv_bits == CMD_MISC2__CTR_END_INV_CONC || inv_bits == CMD_MISC2__CTR_END_INVERT)
                  defptr = &callspec->stuff.conc.outerdef;
               else
                  defptr = &callspec->stuff.conc.innerdef;

               if (the_schema != schema_concentric)
                  fail("Can't do \"central\" with this call.");

               if (ss->cmd.cmd_final_flags.final & ~(FINAL__SPLIT | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))
                  fail("This concept not allowed here.");

               temp_concepts = ss->cmd.cmd_final_flags.herit;

               forcing_concepts = defptr->modifiersh & ~callspec->callflagsh;

               if (forcing_concepts & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT)) {
                  if (ss->cmd.cmd_final_flags.herit & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT))
                     temp_concepts |= (INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
               }

               temp_concepts &= ~(ss->cmd.cmd_final_flags.herit & ~defptr->modifiersh);
               temp_concepts |= forcing_concepts & ~(INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
               callspec = base_calls[defptr->call_id];
               callflags1 = callspec->callflags1;
               ss->cmd.cmd_final_flags.herit = temp_concepts;
               ss->cmd.callspec = callspec;
               ss->cmd.cmd_misc2_flags &= ~CMD_MISC2__CTR_END_MASK;   /* We are done. */
               the_schema = fixup_conc_schema(callspec, ss);
            }

            break;
         case 0: case CMD_MISC2__CENTRAL_SNAG: case CMD_MISC2__CENTRAL_MYSTIC:
            if (ss->cmd.cmd_final_flags.final) fail("This concept not allowed here.");
            break;
      }
   }

   if (callflags1 & CFLAG1_IMPRECISE_ROTATION)
      imprecise_rotation_result_flag = RESULTFLAG__IMPRECISE_ROT;

   /* We of course don't allow "mystic" or "snag" for things that are
      *CROSS* concentrically defined. */

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK) {
      switch (the_schema) {
         case schema_sequential:
         case schema_concentric:
         case schema_single_concentric:
         case schema_single_concentric_together:
         case schema_cross_concentric:
         case schema_single_cross_concentric:
         case schema_concentric_4_2:
         case schema_by_array:
            break;
         default:
            fail("Can't do \"central/snag/mystic\" with this call.");
      }
   }

   /* Do some quick error checking for visible fractions.  For now, any flag is acceptable.  Later, we will
      distinguish among the various flags. */

   if (ss->cmd.cmd_frac_flags) {
      switch (the_schema) {
         case schema_by_array:
            /* We allow the fraction "1/2" to be given.  Basic_move will handle it. */
            if (ss->cmd.cmd_frac_flags != 0x000112)
               fail("This call can't be fractionalized this way.");
            ss->cmd.cmd_frac_flags = 0;
            ss->cmd.cmd_final_flags.herit |= INHERITFLAG_HALF;
            break;
         case schema_nothing: case schema_matrix: case schema_partner_matrix: case schema_roll:
            fail("This call can't be fractionalized.");
            break;
         case schema_sequential: case schema_split_sequential:
            if (!(callflags1 & CFLAG1_VISIBLE_FRACTION_MASK))
               fail("This call can't be fractionalized.");
            break;
         default:

            /* Must be some form of concentric.  We allow visible fractions, and take no action in that case.
               This means that any fractions will be sent to constituent calls. */

            if (!(callflags1 & CFLAG1_VISIBLE_FRACTION_MASK)) {

               /* Otherwise, we allow the fraction "1/2" to be given, if the top-level heritablilty
                  flag allows it.  We turn the fraction into a "final concept". */

               if (!(callspec->callflagsh & INHERITFLAG_HALF) ||
                     (ss->cmd.cmd_frac_flags != 0x000112))
                  fail("This call can't be fractionalized this way.");
               ss->cmd.cmd_frac_flags = 0;
               ss->cmd.cmd_final_flags.herit |= INHERITFLAG_HALF;
            }

            break;
      }
   }

   /* If the "diamond" concept has been given and the call doesn't want it, we do
      the "diamond single wheel" variety. */

   if (INHERITFLAG_DIAMOND & ss->cmd.cmd_final_flags.herit & (~callspec->callflagsh))  {
      /* If the call is sequentially or concentrically defined, the top level flag is required
         before the diamond concept can be inherited.  Since that flag is off, it is an error. */
      if (the_schema != schema_by_array)
         fail("Can't do this call with the \"diamond\" concept.");

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
         fail("Can't do \"invert/central/snag/mystic\" with the \"diamond\" concept.");

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

      if (ss->kind == sdmd) {
         ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_DIAMOND;
         divided_setup_move(ss, map_lists[s1x2][1]->f[MPKIND__DMD_STUFF][0], phantest_ok, TRUE, result);
         return;
      }
      else {
         /* If in a qtag or point-to-points, perhaps we ought to divide into single diamonds and try again.
            BUT: if "magic" or "interlocked" is also present, we don't.  We let basic_move deal with
            it.  It will come back here after it has done what it needs to. */

         if ((ss->cmd.cmd_final_flags.herit & (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) == 0) {
            /* Divide into diamonds and try again.  Note that we do not clear the concept. */
            divide_diamonds(ss, result);
            return;
         }
      }
   }

   mirror = FALSE;

   /* It may be appropriate to step to a wave or rear back from one.
      This is only legal if the flag forbidding same is off.
      Furthermore, if certain modifiers have been given, we don't allow it. */

   if (ss->cmd.cmd_final_flags.herit & (INHERITFLAG_MAGIC | INHERITFLAG_INTLK | INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX | INHERITFLAG_FUNNY))
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;

   /* But, alas, if fractionalization is on, we can't do it yet, because we don't
      know whether we are starting at the beginning.  In the case of fractionalization,
      we will do it later.  We also can't do it yet if we are going
      to split the setup for "central" or "crazy", or if we are doing the call "mystic". */

   if (  !ss->cmd.cmd_frac_flags &&
         (ss->cmd.cmd_misc_flags & (CMD_MISC__NO_STEP_TO_WAVE | CMD_MISC__MUST_SPLIT)) == 0 &&
         ((ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_KMASK) != CMD_MISC2__CENTRAL_MYSTIC || the_schema != schema_by_array) &&
         (callflags1 & (CFLAG1_REAR_BACK_FROM_R_WAVE | CFLAG1_REAR_BACK_FROM_QTAG | CFLAG1_STEP_TO_WAVE))) {

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;  /* Can only do it once. */

      if (ss->cmd.cmd_final_flags.herit & INHERITFLAG_LEFT) {
         mirror_this(ss);
         mirror = TRUE;
      }

      touch_or_rear_back(ss, mirror, callflags1);

      /* But, if the "left_means_touch_or_check" flag is set, we only wanted the "left" flag for the
         purpose of what "touch_or_rear_back" just did.  So, in that case, we turn off the "left"
         flag and set things back to normal. */

      if (callflags1 & CFLAG1_LEFT_MEANS_TOUCH_OR_CHECK) {
         ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_LEFT;
         if (mirror) mirror_this(ss);
         mirror = FALSE;
      }
   }

   /* Check for a call whose schema is single (cross) concentric.
      If so, be sure the setup is divided into 1x4's or diamonds. */

   switch (the_schema) {
      case schema_single_concentric:
      case schema_single_cross_concentric:
         ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;
         if (!do_simple_split(ss, TRUE, result)) return;
         break;
      case schema_single_concentric_together:
         switch (ss->kind) {
            case s1x8: case s_ptpd:
               ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;
               if (!do_simple_split(ss, TRUE, result)) return;
               break;
         }
         break;
   }

   /* At this point, we may have mirrored the setup and, of course, left the switch "mirror"
      on.  We did it only as needed for the [touch / rear back / check] stuff.  What we
      did doesn't actually count.  In particular, if the call is defined concentrically
      or sequentially, mirroring the setup in response to "left" is *NOT* the right thing
      to do.  The right thing is to pass the "left" flag to all subparts that have the
      "inherit_left" invocation flag, and letting events take their course.  So we allow
      the "INHERITFLAG_LEFT" bit to remain in "cmd_final_flags", because it is still important
      to know whether we have been invoked with the "left" modifier. */

   /* Check for special case of ends doing a call like "detour" which specifically
      allows just the ends part to be done.  If the call was "central", this flag will be turned off. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__DOING_ENDS) {
      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
         fail("Can't do \"invert/central/snag/mystic\" with a call for the ends only.");

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
      if ((the_schema == schema_concentric || the_schema == schema_rev_checkpoint) &&
            (DFM1_ENDSCANDO & callspec->stuff.conc.outerdef.modifiers1)) {

         /* Copy the concentricity flags from the call definition into the setup.  All the fuss
            in database.h about concentricity flags co-existing with setupflags refers to this moment. */
         ss->cmd.cmd_misc_flags |= (callspec->stuff.conc.outerdef.modifiers1 & DFM1_CONCENTRICITY_FLAG_MASK);

         callspec = base_calls[callspec->stuff.conc.outerdef.call_id];
         callflags1 = callspec->callflags1;
         the_schema = fixup_conc_schema(callspec, ss);
      }
   }

   ss->cmd.callspec = callspec;

   /* Enforce the restriction that only tagging calls are allowed in certain contexts. */

   if (ss->cmd.cmd_final_flags.final & FINAL__MUST_BE_TAG) {
      if (!(callflags1 & CFLAG1_BASE_TAG_CALL_MASK))
         fail("Only a tagging call is allowed here.");
   }

   ss->cmd.cmd_final_flags.final &= ~FINAL__MUST_BE_TAG;

   /* If the "split" concept has been given and this call uses that concept for a special
      meaning (split square thru, split dixie style), set the special flag to determine that
      action, and remove the split concept.  Why remove it?  So that "heads split catch grand
      mix 3" will work.  If we are doing a "split catch", we don't really want to split the
      setup into 2x2's that are isolated from each other, or else the "grand mix" won't work. */

   if (ss->cmd.cmd_final_flags.final & FINAL__SPLIT) {
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

      if (callflags1 & CFLAG1_SPLIT_LIKE_SQUARE_THRU) {
         ss->cmd.cmd_final_flags.final = (ss->cmd.cmd_final_flags.final | FINAL__SPLIT_SQUARE_APPROVED) & (~FINAL__SPLIT);
         if ((current_options.number_fields & 0xF) == 1) fail("Can't split square thru 1.");
      }
      else if (callflags1 & CFLAG1_SPLIT_LIKE_DIXIE_STYLE)
         ss->cmd.cmd_final_flags.final = (ss->cmd.cmd_final_flags.final | FINAL__SPLIT_DIXIE_APPROVED) & (~FINAL__SPLIT);
   }

   /* NOTE: We may have mirror-reflected the setup.  "Mirror" is true if so.  We may need to undo this. */

   /* If this is the "split sequential" schema and we have not already done so,
      cause splitting to take place. */

   if (the_schema == schema_split_sequential) {
      if (      setup_attrs[ss->kind].setup_limits == 7 ||
               (setup_attrs[ss->kind].setup_limits == 11 && (ss->cmd.cmd_final_flags.herit & INHERITFLAG_3X3)) ||
               (setup_attrs[ss->kind].setup_limits == 15 && (ss->cmd.cmd_final_flags.herit & INHERITFLAG_4X4)))
         ss->cmd.cmd_misc_flags |= CMD_MISC__MUST_SPLIT;
      else if (setup_attrs[ss->kind].setup_limits != 3)
         fail("Need a 4 or 8 person setup for this.");
   }

   /* If the split concept is still present, do it. */

   if (ss->cmd.cmd_final_flags.final & FINAL__SPLIT) {
      map_thing *split_map;

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
         fail("Can't do \"invert/central/snag/mystic\" with the \"split\" concept.");

      ss->cmd.cmd_final_flags.final &= ~FINAL__SPLIT;
      ss->cmd.cmd_misc_flags |= (CMD_MISC__SAID_SPLIT | CMD_MISC__NO_EXPAND_MATRIX);

      /* We can't handle the mirroring, so undo it. */
      if (mirror) { mirror_this(ss); mirror = FALSE; }

      if (ss->kind == s2x4) split_map = map_lists[s2x2][1]->f[MPKIND__SPLIT][0];
      else if (ss->kind == s1x8) split_map = map_lists[s1x4][1]->f[MPKIND__SPLIT][0];
      else if (ss->kind == s_ptpd) split_map = map_lists[sdmd][1]->f[MPKIND__SPLIT][0];
      else if (ss->kind == s_qtag) split_map = map_lists[sdmd][1]->f[MPKIND__SPLIT][1];
      else if (ss->kind == s2x2) {
         /* "Split" was given while we are already in a 2x2?  The only way that
            can be legal is if the word "split" was meant as a modifier for "split square thru"
            etc., rather than as a virtual-setup concept, or if the "split sequential" schema
            is in use.  In those cases, some "split approved" flag will still be on. */

         if (!(ss->cmd.cmd_final_flags.final & (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED)))
            fail("Split concept is meaningless in a 2x2.");

         if (ss->cmd.cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
            fail("\"Matrix\" concept must be followed by applicable concept.");

         move(ss, qtfudged, result);
         result->result_flags &= ~RESULTFLAG__SPLIT_AXIS_MASK;
         return;
      }
      else
         fail("Can't do split concept in this setup.");

      /* If the user said "matrix split", the "matrix" flag will be on at this point,
         and the right thing will happen. */

      divided_setup_move(ss, split_map, phantest_ok, TRUE, result);
      return;
   }

   /* If the "matrix" concept is on and we get here, that is, we haven't acted on a "split" command,
      it is illegal. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
      fail("\"Matrix\" concept must be followed by applicable concept.");

   tbonetest = 0;
   if (setup_attrs[ss->kind].setup_limits >= 0) {
      int j;

      for (j=0; j<=setup_attrs[ss->kind].setup_limits; j++) tbonetest |= ss->people[j].id1;
      if (!(tbonetest & 011)) {
/*      this make "ends start, bits and pieces" fail in diamonds.
         if (ss->cmd.cmd_frac_flags)
            fail("Can't fractionalize a call if no one is doing it.");
*/
         if (the_schema != schema_by_array) {    /* If it's by array, we go ahead anyway. */
            result->kind = nothing;
            return;
         }
      }
   }

   /* We can't handle the mirroring unless the schema is by_array, so undo it. */

   if (the_schema != schema_by_array) {
      if (mirror) { mirror_this(ss); mirror = FALSE; }
   }

   switch (the_schema) {
      case schema_nothing:
         if (ss->cmd.cmd_final_flags.herit | ss->cmd.cmd_final_flags.final) fail("Illegal concept for this call.");
         *result = *ss;
         /* This call is a 1-person call, so it can be presumed to have split both ways. */
         result->result_flags = (ss->cmd.prior_elongation_bits & 3) | RESULTFLAG__SPLIT_AXIS_BIT*3;
         break;
      case schema_matrix:
         /* The "reverse" concept might mean mirror, as in "reverse truck". */

         if ((ss->cmd.cmd_final_flags.herit & INHERITFLAG_REVERSE) && (callspec->callflagsh & INHERITFLAG_REVERSE)) {
            mirror_this(ss);
            mirror = TRUE;
            ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_REVERSE;
         }

         if (ss->cmd.cmd_final_flags.herit | ss->cmd.cmd_final_flags.final) fail("Illegal concept for this call.");
         matrixmove(ss, callspec, result);
         reinstate_rotation(ss, result);
         result->result_flags = 0;
         break;
      case schema_partner_matrix:
         if (ss->cmd.cmd_final_flags.herit | ss->cmd.cmd_final_flags.final) fail("Illegal concept for this call.");
         partner_matrixmove(ss, callspec, result);
         reinstate_rotation(ss, result);
         result->result_flags = 0;
         break;
      case schema_roll:
         if (ss->cmd.cmd_final_flags.herit | ss->cmd.cmd_final_flags.final) fail("Illegal concept for this call.");
         rollmove(ss, callspec, result);
         /* This call is a 1-person call, so it can be presumed to have split both ways. */
         result->result_flags = (ss->cmd.prior_elongation_bits & 3) | RESULTFLAG__SPLIT_AXIS_BIT*3;
         break;
      case schema_by_array:

         /* Dispose of the "left" concept first -- it can only mean mirror.  If it is on,
            mirroring may already have taken place. */

         if (ss->cmd.cmd_final_flags.herit & INHERITFLAG_LEFT) {
/* ***** why isn't this particular error test taken care of more generally elsewhere? */
            if (!(callspec->callflagsh & INHERITFLAG_LEFT)) fail("Can't do this call 'left'.");
            if (!mirror) mirror_this(ss);
            mirror = TRUE;
            ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_LEFT;
         }

         /* The "reverse" concept might mean mirror, or it might be genuine. */

         if ((ss->cmd.cmd_final_flags.herit & INHERITFLAG_REVERSE) && (callspec->callflagsh & INHERITFLAG_REVERSE)) {
            /* This "reverse" just means mirror. */
            if (mirror) fail("Can't do this call 'left' and 'reverse'.");
            mirror_this(ss);
            mirror = TRUE;
            ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_REVERSE;
         }

         /* If the "reverse" flag is still set in cmd_final_flags, it means a genuine
            reverse as in reverse cut/flip the diamond or reverse change-O. */

         ss->cmd.callspec = callspec;
         basic_move(ss, tbonetest, qtfudged, mirror, result);
         break;
      default:
         /* Must be sequential or some form of concentric. */

         /* We demand that the final concepts that remain be only those in the following list,
            which includes all of the "heritable" concepts. */

         if (ss->cmd.cmd_final_flags.final & ~(FINAL__SPLIT | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))
            fail("This concept not allowed here.");

         /* Now we figure out how to dispose of "heritable" concepts.  In general, we will selectively inherit them to
            the call's children, once we verify that the call accepts them for inheritance.  If it doesn't accept them,
            it is an error unless the concepts are the special ones "magic" and/or "interlocked", which we can dispose
            of by doing the call in the appropriate magic/interlocked setup. */

         unaccepted_flags = ss->cmd.cmd_final_flags.herit & (~callspec->callflagsh);    /* The unaccepted flags. */

         if (unaccepted_flags != 0) {
            if (divide_for_magic(ss, ss->cmd.cmd_final_flags.herit, unaccepted_flags, result))
               return;
            else
               fail("Can't do this call with this concept.");
         }

         if (the_schema == schema_sequential || the_schema == schema_split_sequential) {
            do_sequential_call(ss, callspec, qtfudged, &mirror, result);
         }
         else {
            setup_command foo1, foo2;
            by_def_item *innerdef = &callspec->stuff.conc.innerdef;
            by_def_item *outerdef = &callspec->stuff.conc.outerdef;
            parse_block *parseptr = ss->cmd.parseptr;

            /* Must be some form of concentric. */

            if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT)
               fail("Can't split this call.");

            ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;     /* We think this is the right thing to do. */
            saved_warnings = history[history_ptr+1].warnings;

            (void) get_real_subcall(
               parseptr, innerdef,
               ss->cmd.cmd_final_flags, callspec,
               &foo1);

            (void) get_real_subcall(
               parseptr, outerdef,
               ss->cmd.cmd_final_flags, callspec,
               &foo2);

            /* Fudge a 3x4 into a 1/4-tag if appropriate. */

            if (ss->kind == s3x4 && (callflags1 & CFLAG1_FUDGE_TO_Q_TAG) &&
                  (the_schema == schema_concentric || the_schema == schema_cross_concentric)) {

               if (ss->people[0].id1) {
                  if (ss->people[1].id1) fail("Can't do this call from arbitrary 3x4 setup.");
               }
               else (void) copy_person(ss, 0, ss, 1);

               if (ss->people[3].id1) {
                  if (ss->people[2].id1) fail("Can't do this call from arbitrary 3x4 setup.");
                  else (void) copy_person(ss, 1, ss, 3);
               }
               else (void) copy_person(ss, 1, ss, 2);

               (void) copy_person(ss, 2, ss, 4);
               (void) copy_person(ss, 3, ss, 5);

               if (ss->people[6].id1) {
                  if (ss->people[7].id1) fail("Can't do this call from arbitrary 3x4 setup.");
                  else (void) copy_person(ss, 4, ss, 6);
               }
               else (void) copy_person(ss, 4, ss, 7);

               if (ss->people[9].id1) {
                  if (ss->people[8].id1) fail("Can't do this call from arbitrary 3x4 setup.");
                  else (void) copy_person(ss, 5, ss, 9);
               }
               else (void) copy_person(ss, 5, ss, 8);

               (void) copy_person(ss, 6, ss, 10);
               (void) copy_person(ss, 7, ss, 11);

               ss->kind = s_qtag;
               ss->cmd.cmd_misc_flags |= CMD_MISC__DISTORTED;
            }
            else if (ss->kind == s_qtag &&
                     (callflags1 & CFLAG1_FUDGE_TO_Q_TAG) &&
                     (the_schema == schema_in_out_triple || the_schema == schema_in_out_triple_squash)) {
               /* Or change from qtag to 3x4 if schema requires same. */
               (void) copy_person(ss, 11, ss, 7);
               (void) copy_person(ss, 10, ss, 6);
               (void) copy_person(ss, 8, ss, 5);
               (void) copy_person(ss, 7, ss, 4);
               (void) copy_person(ss, 5, ss, 3);
               (void) copy_person(ss, 4, ss, 2);
               (void) copy_person(ss, 2, ss, 1);
               (void) copy_person(ss, 1, ss, 0);
               clear_person(ss, 0);
               clear_person(ss, 3);
               clear_person(ss, 6);
               clear_person(ss, 9);

               ss->kind = s3x4;
            }

            concentric_move(
               ss, &foo1, &foo2,
               the_schema,
               innerdef->modifiers1,
               outerdef->modifiers1,
               TRUE,
               result);

            if (DFM1_SUPPRESS_ELONGATION_WARNINGS & outerdef->modifiers1) {
               history[history_ptr+1].warnings.bits[0] &= ~conc_elong_warnings.bits[0];
               history[history_ptr+1].warnings.bits[1] &= ~conc_elong_warnings.bits[1];
            }
            history[history_ptr+1].warnings.bits[0] |= saved_warnings.bits[0];
            history[history_ptr+1].warnings.bits[1] |= saved_warnings.bits[1];
         }
         break;
   }

   result->result_flags |= imprecise_rotation_result_flag;
   /* Reflect back if necessary. */
   if (mirror) mirror_this(result);
   canonicalize_rotation(result);
}



/* The current interpretation of the elongation flags, on input and output, is now
   as follows:
   
   Note first that, on input and output, the elongation bits are only meaningful in
      a 2x2 or short6 setup.
   On input, nonzero bits in "prior_elongation_bits" field with a 2x2 setup mean that
      the setup _was_ _actually_ _elongated_, and that the elongation is actually felt
      by the dancers.  In this case, the "move" routine is entitled to raise an error
      if the 2x2 call is awkward.  For example, a star thru from facing couples is
      illegal if the elongate bit is on that makes the people far away from the one
      they are facing.  It follows from this that, if we call concentric star thru,
      these bits will be cleared before calling "move", since the concentric concept
      forgives awkward elongation.  Of course, the "concentric_move" routine will
      remember the actual elongation in order to move people to the correct ending
      formation.
   On output, nonzero bits in the low two bits with a 2x2 setup mean that,
      _if_ _result_ _elongation_ _is_ _required_, this is what it should be.  It does
      _not_ mean that such elongation actually exists.  Whoever called "move" must
      make that judgement.  These bits are set when, for example, a 1x4 -> 2x2 call
      is executed, to indicate how to elongate the result, if it turns out that those
      people were working around the outside.  This determination is made not just on
      the "checkpoint rule" that says they go perpendicular to their original axis,
      but also on the basis of the "parallel_conc_end" flag in the call.  That is, these
      bits tell which way to go if the call had been "ends do <whatever>".  Of course,
      if the concentric concept was in use, this information is not used.  Instead,
      "concentric_move" overrides the bits we return with an absolute "checkpoint rule"
      application.

   It may well be that the goal described above is not actually implemented correctly.
*/


extern void move(
   setup *ss,
   long_boolean qtfudged,
   setup *result)
{
   parse_block *saved_magic_diamond;
   uint64 new_final_concepts;
   uint64 check_concepts;
   parse_block *parseptrcopy;
   parse_block *parseptr = ss->cmd.parseptr;

   /* This shouldn't be necessary, but there have been occasional reports of the
      bigblock and stagger concepts getting confused with each other.  This would happen
      if the incoming 4x4 did not have its rotation field equal to zero, as is required
      when in canonical form.  So we check this. */

   if (setup_attrs[ss->kind].four_way_symmetry && ss->rotation != 0)
      fail("There is a bug in 4 way canonicalization -- please report this sequence.");

   if (ss->cmd.cmd_misc_flags & CMD_MISC__DO_AS_COUPLES) {
      /* If we have a pending "centers/ends work <concept>" concept,
         we must dispose of it the crude way. */

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE) {
         punt_centers_use_concept(ss, result);
         return;
      }

      ss->cmd.cmd_misc_flags &= ~CMD_MISC__DO_AS_COUPLES;
      tandem_couples_move(ss, selector_uninitialized, 0, 0, 0, 1, result);
      return;
   }

   if (ss->cmd.callspec) {
      /* This next thing shouldn't happen -- we shouldn't have a call in place when
         there is a pending "centers/ends work <concept>" concept, since that concept should be next. */
      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE) {
         punt_centers_use_concept(ss, result);
         return;
      }
      move_with_real_call(ss, qtfudged, result);
      return;
   }

   /* Scan the "final" concepts, remembering them and their end point. */
   last_magic_diamond = 0;

   /* But if we have a pending "centers/ends work <concept>" concept, don't. */

   if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE) {
      parseptrcopy = skip_one_concept(ss->cmd.parseptr);
      parseptrcopy = parseptrcopy->next;    /* It is now after the subject concept. */
   }
   else
      parseptrcopy = parseptr;

   parseptrcopy = process_final_concepts(parseptrcopy, TRUE, &new_final_concepts);
   saved_magic_diamond = last_magic_diamond;

   /* See if there were any old ones present, and include them. */

   ss->cmd.cmd_final_flags.herit |= new_final_concepts.herit;
   ss->cmd.cmd_final_flags.final |= new_final_concepts.final;

   if (parseptrcopy->concept->kind <= marker_end_of_list) {
      uint32 saved_number_fields = current_options.number_fields;
      selector_kind saved_selector = current_options.who;
      direction_kind saved_direction = current_options.where;

      /* There are no "non-final" concepts.  The only concepts are the final ones that
         have been encoded into cmd_final_flags. */

      /* We must read the selector, direction, and number out of the concept list and use them
         for this call to "move".  We are effectively using them as arguments to "move",
         with all the care that must go into invocations of recursive procedures.  However,
         at their point of actual use, they must be in global variables.  Therefore, we
         explicitly save and restore those global variables (in dynamic variables local to this
         instance) rather than passing them as explicit arguments.  By saving them and restoring
         them in this way, we make things like "checkpoint bounce the beaus by bounce the belles" work. */

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE) {
         if (parseptrcopy->call->schema != schema_concentric) {
            punt_centers_use_concept(ss, result);
            return;
         }

         ss->cmd.skippable_concept = ss->cmd.parseptr;
      }

      current_options.who = parseptrcopy->options.who;
      current_options.where = parseptrcopy->options.where;
      current_options.number_fields = parseptrcopy->options.number_fields;
      ss->cmd.parseptr = parseptrcopy;
      ss->cmd.callspec = parseptrcopy->call;
      move_with_real_call(ss, qtfudged, result);
      current_options.who = saved_selector;
      current_options.where = saved_direction;
      current_options.number_fields = saved_number_fields;
   }
   else {
      /* We now know that there are "non-final" (virtual setup) concepts present. */

      /* If we have a pending "centers/ends work <concept>" concept,
         we must dispose of it the crude way. */

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_USE) {
         punt_centers_use_concept(ss, result);
         return;
      }

      /* These are the concepts that we are interested in. */

      check_concepts = ss->cmd.cmd_final_flags;
      check_concepts.final &= ~FINAL__MUST_BE_TAG;

      result->result_flags = 0;

      /* ***** We used to have FINAL__SPLIT in the list below, but it caused "matrix split, tandem step thru" to fail.
         This needs to be reworked. */

      if (             (check_concepts.herit & ~(INHERITFLAG_REVERSE|INHERITFLAG_LEFT|INHERITFLAG_GRAND|INHERITFLAG_CROSS|INHERITFLAG_SINGLE|INHERITFLAG_INTLK)) == 0 &&
                        check_concepts.final == 0) {
         /* Look for virtual setup concept that can be done by dispatch from table, with no
            intervening final concepts. */
   
         ss->cmd.parseptr = parseptrcopy;

         /* We know that ss->callspec is null. */
         /* ss->cmd.cmd_final_flags may contain
            FINAL__MUST_BE_TAG and/or one of the modifiers (reverse/left/grand/cross/single)
            listed above.  do_big_concept will take care of the latter. */

         if (do_big_concept(ss, result)) {
            canonicalize_rotation(result);
            return;
         }
      }

      clear_people(result);

      if (ss->cmd.cmd_misc2_flags & CMD_MISC2__CTR_END_MASK)
         fail("Can't do \"invert/central/snag/mystic\" followed by another concept or modifier.");

      /* Some final concept (e.g. "magic") is present in front of our virtual setup concept.
         We have to dispose of it.  This means that expanding the matrix (e.g. 2x4->2x6)
         and stepping to a wave or rearing back from one are no longer legel. */

      ss->cmd.cmd_misc_flags |= (CMD_MISC__NO_EXPAND_MATRIX | CMD_MISC__NO_STEP_TO_WAVE);

      /* There are a few "final" concepts that
         will not be treated as such if there are non-final concepts occurring
         after them.  Instead, they will be treated as virtual setup concepts.
         This is what makes "magic once removed trade" work, for
         example.  On the other hand, if there are no non-final concepts following, treat these as final.
         This is what makes "magic transfer" or "split square thru" work. */

      ss->cmd.parseptr = parseptrcopy;
      ss->cmd.callspec = (callspec_block *) 0;

      /* We can tolerate the "matrix" flag if we are going to do "split".  For anything else,
         "matrix" is illegal. */

      if (check_concepts.final == FINAL__SPLIT && check_concepts.herit == 0) {
         map_thing *split_map;
   
         ss->cmd.cmd_misc_flags |= CMD_MISC__SAID_SPLIT;

         if (ss->kind == s2x4) split_map = map_lists[s2x2][1]->f[MPKIND__SPLIT][0];
         else if (ss->kind == s1x8) split_map = map_lists[s1x4][1]->f[MPKIND__SPLIT][0];
         else if (ss->kind == s_ptpd) split_map = map_lists[sdmd][1]->f[MPKIND__SPLIT][0];
         else if (ss->kind == s_qtag) split_map = map_lists[sdmd][1]->f[MPKIND__SPLIT][1];
         else fail("Can't do split concept in this setup.");

         ss->cmd.cmd_final_flags.final &= ~FINAL__SPLIT;
         divided_setup_move(ss, split_map, phantest_ok, TRUE, result);
      }
      else {
         if (ss->cmd.cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
            fail("\"Matrix\" concept must be followed by applicable concept.");

         if (divide_for_magic(
               ss,
               ss->cmd.cmd_final_flags.herit,
               check_concepts.herit & ~INHERITFLAG_DIAMOND,
               result)) {
         }
         else if (check_concepts.herit == INHERITFLAG_DIAMOND && check_concepts.final == 0) {
            ss->cmd.cmd_final_flags.herit &= ~INHERITFLAG_DIAMOND;

            if (ss->kind == sdmd)
               divided_setup_move(ss, map_lists[s1x2][1]->f[MPKIND__DMD_STUFF][0], phantest_ok, TRUE, result);
            else {
               /* Divide into diamonds and try again.  (Note that we back up the concept pointer.) */
               ss->cmd.parseptr = parseptr;
               ss->cmd.cmd_final_flags.final = 0;
               ss->cmd.cmd_final_flags.herit = 0;
               divide_diamonds(ss, result);
            }
         }
         else
            fail2("Can't do this concept with other concepts preceding it:", parseptrcopy->concept->menu_name);
      }
   }

   /* If execution of the call raised a request that we change a concept name from "magic" to
      "magic diamond,", for example, do so. */

   if ((result->result_flags & RESULTFLAG__NEED_DIAMOND) && saved_magic_diamond && saved_magic_diamond->concept->value.arg1 == 0) {
      if (saved_magic_diamond->concept->kind == concept_magic) saved_magic_diamond->concept = &special_magic;
      else if (saved_magic_diamond->concept->kind == concept_interlocked) saved_magic_diamond->concept = &special_interlocked;
   }
}
