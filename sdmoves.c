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
   canonicalize_rotation
   reinstate_rotation
   divide_for_magic
   do_simple_split
   do_call_in_series
   get_fraction_info
   move
*/

#include "sd.h"


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
   uint32 flags_to_use,
   uint32 flags_to_check,
   setup *result)
{
   map_thing *division_maps;
   uint32 resflags = 0;

   if (ss->kind == s2x4) {
      if (flags_to_check == INHERITFLAG_MAGIC) {
         /* "Magic" was specified.  Split it into 1x4's in the appropriate magical way. */
         division_maps = &map_2x4_magic;
         goto divide_us;
      }
   }
   else if (ss->kind == s_qtag) {
      resflags = RESULTFLAG__NEED_DIAMOND;    /* Indicate that we have done a diamond division and the concept name needs to be changed. */
      if (flags_to_check == INHERITFLAG_MAGIC) {
         division_maps = &map_qtg_magic;
         goto divide_us;
      }
      else if ((flags_to_check) == INHERITFLAG_INTLK) {
         division_maps = &map_qtg_intlk;
         goto divide_us;
      }
      else if ((flags_to_check) == (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) {
         division_maps = &map_qtg_magic_intlk;
         goto divide_us;
      }
   }
   else if (ss->kind == s_ptpd) {
      resflags = RESULTFLAG__NEED_DIAMOND;
      if (flags_to_check == INHERITFLAG_MAGIC) {
         division_maps = &map_ptp_magic;
         goto divide_us;
      }
      else if ((flags_to_check) == INHERITFLAG_INTLK) {
         division_maps = &map_ptp_intlk;
         goto divide_us;
      }
      else if ((flags_to_check) == (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) {
         division_maps = &map_ptp_magic_intlk;
         goto divide_us;
      }
   }

   return FALSE;

divide_us:

   ss->cmd.cmd_final_flags = flags_to_use & ~flags_to_check;
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
            divided_setup_move(ss, (*map_lists[s1x4][1])[MPKIND__SPLIT][1], phantest_ok, TRUE, result);
         else
            divided_setup_move(ss, (*map_lists[s2x2][1])[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
         return FALSE;
      case s1x8:
         divided_setup_move(ss, (*map_lists[s1x4][1])[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
         return FALSE;
      case s_qtag:
         divided_setup_move(ss, (*map_lists[sdmd][1])[MPKIND__SPLIT][1], phantest_ok, TRUE, result);
         return FALSE;
      case s_ptpd:
         divided_setup_move(ss, (*map_lists[sdmd][1])[MPKIND__SPLIT][0], phantest_ok, TRUE, result);
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
            break;

         /* Otherwise (perhaps the setup was a star) we have no idea how to elongate the setup. */
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
    long_boolean sel;       /* True if this person is selected.  (False if selectors not in use.) */
    long_boolean done;      /* Used for loop control on each pass */
    long_boolean realdone;  /* Used for loop control on each pass */
    int boybit;             /* 1 if boy, 0 if not (might be neither). */
    int girlbit;            /* 1 if girl, 0 if not (might be neither). */ 
    int dir;                /* This person's initial facing direction, 0 to 3. */
    int deltax;             /* How this person will move, relative to his own facing */
    int deltay;             /*   direction, when call is finally executed. */
    int deltarot;           /* How this person will turn. */
    int rollinfo;           /* How this person's roll info will be set. */
    struct gloop *nextse;   /* Points to next person south (dir even) or east (dir odd.) */
    struct gloop *nextnw;   /* Points to next person north (dir even) or west (dir odd.) */
    long_boolean tbstopse;  /* True if nextse/nextnw is zero because the next spot */
    long_boolean tbstopnw;  /*   is occupied by a T-boned person (as opposed to being empty.) */
    } matrix_rec;




/* This function is internal. */

Private void start_matrix_call(
   setup *ss,
   int *nump,
   matrix_rec matrix_info[],
   long_boolean use_selector,
   setup *people)
{
   int i;
   coordrec *thingyptr;

   clear_people(people);
   
   thingyptr = setup_attrs[ss->kind].setup_coords;
   if (!thingyptr) fail("Can't do this in this setup.");
   
   if (setup_attrs[ss->kind].setup_limits < 0) fail("Can't do this in this setup.");        /* this is actually superfluous */
   
   *nump = 0;
   for (i=0; i<=setup_attrs[ss->kind].setup_limits; i++) {
      if (ss->people[i].id1) {
         if (*nump == 8) fail("?????too many people????");
         (void) copy_person(people, *nump, ss, i);
         matrix_info[*nump].x = thingyptr->xca[i];
         matrix_info[*nump].y = thingyptr->yca[i];

         matrix_info[*nump].done = FALSE;
         matrix_info[*nump].realdone = FALSE;

         if (use_selector)
            matrix_info[*nump].sel = selectp(people, *nump);
         else
            matrix_info[*nump].sel = FALSE;

         matrix_info[*nump].dir = people->people[*nump].id1 & 3;
   
         matrix_info[*nump].girlbit = (people->people[*nump].id2 & ID2_GIRL) ? 1 : 0;
         matrix_info[*nump].boybit = (people->people[*nump].id2 & ID2_BOY) ? 1 : 0;
         matrix_info[*nump].nextse = 0;
         matrix_info[*nump].nextnw = 0;
         matrix_info[*nump].deltax = 0;
         matrix_info[*nump].deltay = 0;
         matrix_info[*nump].deltarot = 0;
         matrix_info[*nump].rollinfo = ROLLBITM;
         matrix_info[*nump].tbstopse = FALSE;
         matrix_info[*nump].tbstopnw = FALSE;

         (*nump)++;
      }
   }
}


/* This function is internal. */

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
   else if ((ypar == 0x00950063) && ((signature & (~0x08400320)) == 0)) {
      checkptr = setup_attrs[sbigdmd].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00950095) && ((signature & (~0x22008080)) == 0)) {
      checkptr = setup_attrs[s_thar].setup_coords;
      goto doit;
   }
   else if ((ypar == 0x00950075) && ((signature & (~0x20018000)) == 0)) {
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
   else if ((ypar == 0x00550067) && ((signature & (~0x08410200)) == 0)) {
      checkptr = setup_attrs[s_qtag].setup_coords;
      goto doitrot;
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
   else if ((ypar == 0x00550057) && ((signature & (~0x20000620)) == 0)) {
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
         place = checkptr->diagram[27 - ((matrix_info[i].x >> 2) << checkptr->xfactor) - (matrix_info[i].y >> 2)];
         if (place < 0) fail("Person has moved into a grossly ill-defined location.");
         if ((checkptr->xca[place] != -matrix_info[i].y) || (checkptr->yca[place] != matrix_info[i].x))
            fail("Person has moved into a slightly ill-defined location.");
         install_rot(result, place, people, i, 033);
         result->people[place].id1 &= ~STABLE_MASK;   /* For now, can't do fractional stable on this kind of call. */
      }
      
      return;
}



/* This function is internal. */

Private void matrixmove(
   setup *ss,
   callspec_block *callspec,
   setup *result)
{
   int datum;
   setup people;
   matrix_rec matrix_info[9];
   int i, nump, alldelta;

   if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT)
      fail("Can't split the setup.");

   alldelta = 0;

   start_matrix_call(ss, &nump, matrix_info, TRUE, &people);

   for (i=0; i<nump; i++) {
      if (matrix_info[i].sel) {
         /* This is legal if girlbit or boybit is on (in which case we use the appropriate datum)
            or if the two data are identical so the sex doesn't matter. */
         if ((matrix_info[i].girlbit | matrix_info[i].boybit) == 0 &&
                  (callspec->stuff.matrix.stuff[0] != callspec->stuff.matrix.stuff[1]))
            fail("Can't determine sex of this person.");

         datum = callspec->stuff.matrix.stuff[matrix_info[i].girlbit];
         alldelta |= (  matrix_info[i].deltax = ( ((datum >> 7) & 0x1F) - 16) << 1  );
         alldelta |= (  matrix_info[i].deltay = ( ((datum >> 2) & 0x1F) - 16) << 1  );
         matrix_info[i].deltarot = datum & 03;
         matrix_info[i].rollinfo = (datum >> 12) * ROLLBITR;
      }
   }

   if ((alldelta != 0) && (ss->cmd.cmd_misc_flags & CMD_MISC__DISTORTED))
      fail("This call not allowed in distorted or virtual setup.");
   
   finish_matrix_call(matrix_info, nump, &people, result);
}


/* This function is internal. */

Private void do_pair(
   matrix_rec *ppp,        /* Selected person */
   matrix_rec *qqq,        /* Unselected person */
   callspec_block *callspec,
   int flip,
   int filter)             /* 1 to do N/S facers, 0 for E/W facers. */
{
   int base;
   int datum;
   int flags;

   flags = callspec->stuff.matrix.flags;

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
      if (datum == 0) fail("Can't do this call.");
   
      ppp->deltax = (((datum >> 7) & 0x1F) - 16) << 1;
      ppp->deltay = (((datum >> 2) & 0x1F) - 16) << 1;
      ppp->deltarot = datum & 3;
      ppp->rollinfo = (datum >> 12) * ROLLBITR;
      ppp->realdone = TRUE;
   }
   ppp->done = TRUE;

   if ((filter ^ qqq->dir) & 1) {
      base = (qqq->dir & 2) ? 0 : 2;
      if (flags & MTX_IGNORE_NONSELECTEES) base |= 4;
      base ^= flip;

      /* This is legal if girlbit or boybit is on (in which case we use the appropriate datum)
         or if the two data are identical so the sex doesn't matter. */
      if ((qqq->girlbit | qqq->boybit) == 0 &&
               (callspec->stuff.matrix.stuff[base] != callspec->stuff.matrix.stuff[base+1]))
         fail("Can't determine sex of this person.");

      datum = callspec->stuff.matrix.stuff[base+qqq->girlbit];
      if (datum == 0) fail("Can't do this call.");

      qqq->deltax = (((datum >> 7) & 0x1F) - 16) << 1;
      qqq->deltay = (((datum >> 2) & 0x1F) - 16) << 1;
      qqq->deltarot = datum & 3;
      qqq->rollinfo = (datum >> 12) * ROLLBITR;
      qqq->realdone = TRUE;
   }
   qqq->done = TRUE;
}


/* This function is internal. */

Private void do_matrix_chains(
   matrix_rec matrix_info[],
   int nump,
   callspec_block *callspec,
   int filter)                        /* 1 for E/W chains, 0 for N/S chains. */
{
   long_boolean another_round;
   int i, j;
   uint32 flags;

   flags = callspec->stuff.matrix.flags;

   /* Find adjacency relationships, and fill in the "se"/"nw" pointers. */

   for (i=0; i<nump; i++) {
      if ((flags & MTX_IGNORE_NONSELECTEES) && (!matrix_info[i].sel)) continue;
      for (j=0; j<nump; j++) {
         if ((flags & MTX_IGNORE_NONSELECTEES) && (!matrix_info[j].sel)) continue;
         /* Find out if these people are adjacent in the right way. */

         if (    ( filter && matrix_info[j].x == matrix_info[i].x + 4 && matrix_info[j].y == matrix_info[i].y)
            ||   (!filter && matrix_info[j].y == matrix_info[i].y - 4 && matrix_info[j].x == matrix_info[i].x)   ) {

            /* Now, if filter = 1, person j is just east of person i.
               If filter = 0, person j is just south of person i. */

            if (flags & MTX_TBONE_IS_OK) {
               matrix_info[i].nextse = &matrix_info[j];     /* Make the chain independently of facing direction. */
               matrix_info[j].nextnw = &matrix_info[i];
            }
            else {
               if ((matrix_info[i].dir ^ filter) & 1) {
                  if ((matrix_info[i].dir ^ matrix_info[j].dir) & 1) {
                     if (!(flags & MTX_STOP_AND_WARN_ON_TBONE)) fail("People are T-boned.");
                     matrix_info[i].tbstopse = TRUE;
                     matrix_info[j].tbstopnw = TRUE;
                  }
                  else {
                     if ((flags & MTX_MUST_FACE_SAME_WAY) && (matrix_info[i].dir ^ matrix_info[j].dir))
                        fail("Paired people must face the same way.");
                     matrix_info[i].nextse = &matrix_info[j];
                     matrix_info[j].nextnw = &matrix_info[i];
                  }
               }
            }
            break;
         }
      }
   }

   /* Pick out pairs of people and move them. */

   another_round = TRUE;

   while (another_round) {
      another_round = FALSE;

      for (i=0; i<nump; i++) {
         if (!matrix_info[i].done) {

            /* This person might be ready to be paired up with someone. */

            if (matrix_info[i].nextse) {
               if (matrix_info[i].nextnw)
                  /* This person has neighbors on both sides.  Can't do anything yet. */
                  ;
               else {
                  /* This person has a neighbor on south/east side only. */

                  if (matrix_info[i].tbstopnw) warn(warn__not_tbone_person);

                  if (   (!(flags & MTX_USE_SELECTOR))  ||  matrix_info[i].sel   ) {
                     if ((!(flags & MTX_IGNORE_NONSELECTEES)) && matrix_info[i].nextse->sel) {
                        fail("Two adjacent selected people.");
                     }
                     else {

                        /* Do this pair.  First, chop the pair off from anyone else. */

                        if (matrix_info[i].nextse->nextse) matrix_info[i].nextse->nextse->nextnw = 0;
                        matrix_info[i].nextse->nextse = 0;
                        another_round = TRUE;
                        do_pair(&matrix_info[i], matrix_info[i].nextse, callspec, 0, filter);
                     }
                  }
               }
            }
            else {
               if (matrix_info[i].nextnw) {
                  /* This person has a neighbor on north/west side only. */

                  if (matrix_info[i].tbstopse) warn(warn__not_tbone_person);

                  if (   (!(flags & MTX_USE_SELECTOR))  ||  matrix_info[i].sel   ) {
                     if ((!(flags & MTX_IGNORE_NONSELECTEES)) && matrix_info[i].nextnw->sel) {
                        fail("Two adjacent selected people.");
                     }
                     else {
                        /* Do this pair.  First, chop the pair off from anyone else. */

                        if (matrix_info[i].nextnw->nextnw) matrix_info[i].nextnw->nextnw->nextse = 0;
                        matrix_info[i].nextnw->nextnw = 0;
                        another_round = TRUE;
                        do_pair(&matrix_info[i], matrix_info[i].nextnw, callspec, 2, filter);
                     }
                  }
               }
               else {
                  /* Person is alone.  If this is his lateral axis, mark him done and don't move him. */

                  if ((matrix_info[i].dir ^ filter) & 1) {
                     if (matrix_info[i].tbstopse || matrix_info[i].tbstopnw) warn(warn__not_tbone_person);
                     if (   (!(flags & MTX_USE_SELECTOR))  ||  matrix_info[i].sel   )
                        fail("Person has no one to work with.");
                     matrix_info[i].done = TRUE;
                  }
               }
            }
         }
      }
   }
}




/* This function is internal. */

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

   start_matrix_call(ss, &nump, matrix_info, flags & MTX_USE_SELECTOR, &people);

   /* Make the lateral chains first. */

   do_matrix_chains(matrix_info, nump, callspec, 1);

   /* Now clean off the pointers in preparation for the second pass. */

   for (i=0; i<nump; i++) {
      matrix_info[i].done = FALSE;
      matrix_info[i].nextse = 0;
      matrix_info[i].nextnw = 0;
      matrix_info[i].tbstopse = FALSE;
      matrix_info[i].tbstopnw = FALSE;
   }

   /* Vertical chains next. */

   do_matrix_chains(matrix_info, nump, callspec, 0);

   /* Scan for people who ought to have done something but didn't. */

   for (i=0; i<nump; i++) {
      if (!matrix_info[i].realdone) {
         if (   (!(flags & MTX_USE_SELECTOR))  ||  matrix_info[i].sel   ) {
            fail("Person could not identify other person to work with.");
         }
      }
   }

   finish_matrix_call(matrix_info, nump, &people, result);
}


/* This function is internal. */

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
         if (!(callspec->callflagsh & CFLAGH__REQUIRES_SELECTOR) || selectp(ss, i)) {
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


/* Strip out those concepts that do not have the "dfm__xxx" flag set saying that they are to be
   inherited to this part of the call.  BUT: the "INHERITFLAG_LEFT" flag controls
   both "INHERITFLAG_REVERSE" and "INHERITFLAG_LEFT", turning the former into the latter.  This makes reverse
   circle by, touch by, and clean sweep work. */

Private uint32 get_mods_for_subcall(uint32 new_final_concepts, uint32 this_modh, uint32 callflagsh)
{
   uint32 retval;

   retval = new_final_concepts;

   /* If this subcall has "inherit_reverse" or "inherit_left" given, but the top-level call
      doesn't permit the corresponding flag to be given, we should turn any "reverse" or
      "left" modifier that was given into the other one, and cause that to be inherited.
      This is what turns, for example, part 3 of "*REVERSE* clean sweep" into a "*LEFT* 1/2 tag". */

   if (this_modh & ~callflagsh & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT)) {
      if (new_final_concepts & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT))
         retval |= (INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
   }

   retval &= ~(new_final_concepts & HERITABLE_FLAG_MASK & ~this_modh);
   
   /* Now turn on any "force" flags.  These are indicated by "this_modh" on and "callflagsh" off.
      We only do this for heritable flags other than left/reverse. */

   retval |= this_modh & (HERITABLE_FLAG_MASK & ~(INHERITFLAG_REVERSE | INHERITFLAG_LEFT)) & ~callflagsh;

   return retval;
}


Private void divide_diamonds(setup *ss, setup *result)
{
   if (ss->kind == s_qtag) {
      divided_setup_move(ss, (*map_lists[sdmd][1])[MPKIND__SPLIT][1], phantest_ok, FALSE, result);
   }
   else if (ss->kind == s_ptpd) {
      divided_setup_move(ss, (*map_lists[sdmd][1])[MPKIND__SPLIT][0], phantest_ok, FALSE, result);
   }
   else
      fail("Must have diamonds for this concept.");
}


extern fraction_info get_fraction_info(uint32 frac_flags, uint32 callflags1, int total)
{
   fraction_info retval;
   int numer, denom, s_numer, s_denom, this_part, test_size;
   int subcall_index, highlimit;

   retval.reverse_order = 0;
   retval.instant_stop = 0;
   retval.do_half_of_last_part = 0;

   this_part = (frac_flags & 0xF0000) >> 16;
   s_numer = (frac_flags & 0xF000) >> 12;      /* Start point. */
   s_denom = (frac_flags & 0xF00) >> 8;
   numer = (frac_flags & 0xF0) >> 4;           /* Stop point. */
   denom = (frac_flags & 0xF);

   if (s_numer >= s_denom) fail("Fraction must be proper.");
   subcall_index = total * s_numer;
   if ((subcall_index % s_denom) != 0) fail("This call can't be fractionalized with this fraction.");
   subcall_index = subcall_index / s_denom;

   if (numer <= 0) fail("Fraction must be proper.");
   highlimit = total * numer;
   test_size = highlimit / denom;

   if (test_size*denom == highlimit)
      highlimit = test_size;
   else {
      if (2*test_size*denom + denom == 2*highlimit) {
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
   if (frac_flags & 0x100000) {
      subcall_index = total-1-subcall_index;
      retval.reverse_order = TRUE;
      if (retval.do_half_of_last_part) fail("This call can't be fractionalized with this fraction.");
   }

   if (this_part != 0) {
      /* In addition to everything else, we are picking out a specific part
         of whatever series we have decided upon. */

      if (retval.do_half_of_last_part) fail("This call can't be fractionalized with this fraction.");
      subcall_index += (retval.reverse_order) ? (1-this_part) : (this_part-1);

      retval.instant_stop = 1;

      /* Be sure that enough parts are visible. */
      if (     (callflags1 & CFLAG1_VISIBLE_FRACTION_MASK) != 3*CFLAG1_VISIBLE_FRACTION_BIT &&
               (subcall_index+1 > (callflags1 & CFLAG1_VISIBLE_FRACTION_MASK) / CFLAG1_VISIBLE_FRACTION_BIT))
         fail("This call can't be fractionalized.");

      if (subcall_index >= total) fail("The indicated part number doesn't exist.");

      if ((frac_flags & 0xE00000) == 0x400000) {
         /* We are not just doing part N, we are doing parts up through N. */
         if (retval.reverse_order)
            fail("Sorry, can't do this with reverse order.");
         subcall_index = 0;     /* Start at the beginning. */
         highlimit = this_part;
         retval.instant_stop = 0;
      }
      else if ((frac_flags & 0xE00000) == 0x200000) {
         /* We are not just doing part N, we are doing parts strictly after N. */
         if (retval.reverse_order)
            fail("Sorry, can't do this with reverse order.");
         subcall_index++;  /* Strictly after. */
         retval.instant_stop = 0;
      }
   }
   else {
      /* Unless all parts are visible, this is illegal. */
      if ((callflags1 & CFLAG1_VISIBLE_FRACTION_MASK) != 3*CFLAG1_VISIBLE_FRACTION_BIT)
         fail("This call can't be fractionalized.");
   }

   retval.subcall_index = subcall_index;
   retval.highlimit = highlimit;

   return retval;
}



/* This leaves the split axis result bits in absolute orientation. */

Private void move_with_real_call(
   setup *ss,
   long_boolean qtfudged,
   setup *result)
{
   long_boolean qtf;
   parse_block *cp1;
   parse_block *cp2;
   warning_info saved_warnings;
   uint32 tbonetest;
   uint32 imprecise_rotation_result_flag = 0;
   uint32 unaccepted_flags;
   uint32 new_final_concepts;
   callspec_block *call1, *call2;
   calldef_schema the_schema;
   long_boolean mirror;
   parse_block *parseptr = ss->cmd.parseptr;
   callspec_block *callspec = ss->cmd.callspec;
   uint32 final_concepts = ss->cmd.cmd_final_flags;

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



   /* Check for "central" concept and its ilk, and pick up correct definition. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__CENTRAL_MASK) {
      uint32 temp_concepts, forcing_concepts;

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX | CMD_MISC__DISTORTED;
      /* We shut off the "doing ends" stuff.  If we say "ends detour" we mean "ends do the ends part of
         detour".  But if we say "ends central detour" we mean "ends do the *centers* part of detour". */
      ss->cmd.cmd_misc_flags &= ~CMD_MISC__DOING_ENDS;
   
      /* Now we demand that, if a concept was given, the call had the appropriate flag set saying
         that the concept is legal and will be inherited to the children. */
   
      if (HERITABLE_FLAG_MASK & final_concepts & (~callspec->callflagsh)) fail("Can't do this call with this concept.");

      switch (ss->cmd.cmd_misc_flags & CMD_MISC__CENTRAL_MASK) {
         case CMD_MISC__CENTRAL_PLAIN:
            if (callspec->schema != schema_concentric)
               fail("Can't do \"central\" with this call.");

            if (final_concepts &
                  ~(FINAL__SPLIT | HERITABLE_FLAG_MASK | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))
               fail("This concept not allowed here.");

            temp_concepts = final_concepts;

            forcing_concepts = callspec->stuff.conc.innerdef.modifiersh & ~callspec->callflagsh;

            if (forcing_concepts & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT)) {
               if (final_concepts & (INHERITFLAG_REVERSE | INHERITFLAG_LEFT))
                  temp_concepts |= (INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
            }

            temp_concepts &= ~(final_concepts & HERITABLE_FLAG_MASK & ~callspec->stuff.conc.innerdef.modifiersh);
            temp_concepts |= forcing_concepts & ~(INHERITFLAG_REVERSE | INHERITFLAG_LEFT);
            callspec = base_calls[callspec->stuff.conc.innerdef.call_id];
            final_concepts = temp_concepts;
            ss->cmd.cmd_final_flags = final_concepts;
            ss->cmd.callspec = callspec;
            ss->cmd.cmd_misc_flags &= ~CMD_MISC__CENTRAL_MASK;   /* We are done. */
            break;
         case CMD_MISC__CENTRAL_SNAG: case CMD_MISC__CENTRAL_MYSTIC:
            if (final_concepts & ~HERITABLE_FLAG_MASK) fail("This concept not allowed here.");
            break;
      }
   }

   if (callspec->callflags1 & CFLAG1_IMPRECISE_ROTATION)
      imprecise_rotation_result_flag = RESULTFLAG__IMPRECISE_ROT;

   /* Check for a call whose schema is single (cross) concentric.
      If so, be sure the setup is divided into 1x4's or diamonds. */

   the_schema = callspec->schema;
   if (the_schema == schema_maybe_single_concentric)
      the_schema = (final_concepts & INHERITFLAG_SINGLE) ? schema_single_concentric : schema_concentric;
   else if (the_schema == schema_maybe_single_cross_concentric)
      the_schema = (final_concepts & INHERITFLAG_SINGLE) ? schema_single_cross_concentric : schema_cross_concentric;
   else if (the_schema == schema_maybe_matrix_conc_star) {
      if (final_concepts & INHERITFLAG_12_MATRIX)
         the_schema = schema_conc_star12;
      else if (final_concepts & INHERITFLAG_16_MATRIX)
         the_schema = schema_conc_star16;
      else
         the_schema = schema_conc_star;
   }

   /* We of course don't allow "mystic" or "snag" for things that are
      *CROSS* concentrically defined. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__CENTRAL_MASK) {
      switch (the_schema) {
         case schema_concentric:
         case schema_single_concentric:
         case schema_single_concentric_together:
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
            final_concepts |= INHERITFLAG_HALF;
            break;
         case schema_nothing: case schema_matrix: case schema_partner_matrix: case schema_roll:
            fail("This call can't be fractionalized.");
            break;
         case schema_sequential: case schema_split_sequential:
            if (!(callspec->callflags1 & CFLAG1_VISIBLE_FRACTION_MASK))
               fail("This call can't be fractionalized.");
            break;
         default:

            /* Must be some form of concentric.  We allow visible fractions, and take no action in that case.
               This means that any fractions will be sent to constituent calls. */

            if (!(callspec->callflags1 & CFLAG1_VISIBLE_FRACTION_MASK)) {

               /* Otherwise, we allow the fraction "1/2" to be given, if the top-level heritablilty
                  flag allows it.  We turn the fraction into a "final concept". */

               if (!(callspec->callflagsh & INHERITFLAG_HALF) ||
                     (ss->cmd.cmd_frac_flags != 0x000112))
                  fail("This call can't be fractionalized this way.");
               ss->cmd.cmd_frac_flags = 0;
               final_concepts |= INHERITFLAG_HALF;
            }

            break;
      }
   }

   switch (the_schema) {
      case schema_single_concentric:
      case schema_single_cross_concentric:
         ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;
         if (!do_simple_split(ss, TRUE, result))
            return;

         break;
      case schema_single_concentric_together:
         switch (ss->kind) {
            case s1x8: case s_ptpd:
               ss->cmd.cmd_misc_flags &= ~CMD_MISC__MUST_SPLIT;
               (void) do_simple_split(ss, TRUE, result);
               return;
         }
         break;
   }

   /* If the "diamond" concept has been given and the call doesn't want it, we do
      the "diamond single wheel" variety. */

   if (INHERITFLAG_DIAMOND & final_concepts & (~callspec->callflagsh))  {
      /* If the call is sequentially or concentrically defined, the top level flag is required
         before the diamond concept can be inherited.  Since that flag is off, it is an error. */
      if (the_schema != schema_by_array)
         fail("Can't do this call with the 'diamond' concept.");

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;

      if (ss->kind == sdmd) {
         ss->cmd.cmd_final_flags &= ~INHERITFLAG_DIAMOND;
         divided_setup_move(ss, (*map_lists[s1x2][1])[MPKIND__DMD_STUFF][0], phantest_ok, TRUE, result);
         return;
      }
      else {
         /* If in a qtag or point-to-points, perhaps we ought to divide into single diamonds and try again.
            BUT: if "magic" or "interlocked" is also present, we don't.  We let basic_move deal with
            it.  It will come back here after it has done what it needs to. */

         if ((final_concepts & (INHERITFLAG_MAGIC | INHERITFLAG_INTLK)) == 0) {
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

   if (final_concepts & (INHERITFLAG_MAGIC | INHERITFLAG_INTLK | INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX | INHERITFLAG_FUNNY))
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;

   /* But, alas, if fractionalization is on, we can't do it yet, because we don't
      know whether we are starting at the beginning.  In the case of fractionalization,
      we will do it later.  We also can't do it yet if we are going
      to split the setup for "central" or "crazy". */

   if (  !ss->cmd.cmd_frac_flags &&
         (ss->cmd.cmd_misc_flags & (CMD_MISC__NO_STEP_TO_WAVE | CMD_MISC__MUST_SPLIT)) == 0 &&
         (callspec->callflags1 & (CFLAG1_REAR_BACK_FROM_R_WAVE | CFLAG1_REAR_BACK_FROM_QTAG | CFLAG1_STEP_TO_WAVE))) {

      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;  /* Can only do it once. */

      if (final_concepts & INHERITFLAG_LEFT) {
         mirror_this(ss);
         mirror = TRUE;
      }

      touch_or_rear_back(ss, mirror, callspec->callflags1);

      /* But, if the "left_means_touch_or_check" flag is set, we only wanted the "left" flag for the
         purpose of what "touch_or_rear_back" just did.  So, in that case, we turn off the "left"
         flag and set things back to normal. */

      if (callspec->callflags1 & CFLAG1_LEFT_MEANS_TOUCH_OR_CHECK) {
         final_concepts &= ~INHERITFLAG_LEFT;
         if (mirror) mirror_this(ss);
         mirror = FALSE;
      }
   }

   ss->cmd.cmd_final_flags = final_concepts;

   /* At this point, we may have mirrored the setup and, of course, left the switch "mirror"
      on.  We did it only as needed for the [touch / rear back / check] stuff.  What we
      did doesn't actually count.  In particular, if the call is defined concentrically
      or sequentially, mirroring the setup in response to "left" is *NOT* the right thing
      to do.  The right thing is to pass the "left" flag to all subparts that have the
      "inherit_left" invocation flag, and letting events take their course.  So we allow
      the "INHERITFLAG_LEFT" bit to remain in "final_concepts", because it is still important
      to know whether we have been invoked with the "left" modifier. */

   /* Check for special case of ends doing a call like "detour" which specifically
      allows just the ends part to be done.  If the call was "central", this flag will be turned off. */

   if (ss->cmd.cmd_misc_flags & CMD_MISC__DOING_ENDS) {
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
      if ((the_schema == schema_concentric || the_schema == schema_rev_checkpoint) &&
            (DFM1_ENDSCANDO & callspec->stuff.conc.outerdef.modifiers1)) {

         /* Copy the concentricity flags from the call definition into the setup.  All the fuss
            in database.h about concentricity flags co-existing with setupflags refers to this moment. */
         ss->cmd.cmd_misc_flags |= (callspec->stuff.conc.outerdef.modifiers1 & DFM1_CONCENTRICITY_FLAG_MASK);

         callspec = base_calls[callspec->stuff.conc.outerdef.call_id];
         the_schema = callspec->schema;
         if (the_schema == schema_maybe_single_concentric)
            the_schema = (final_concepts & INHERITFLAG_SINGLE) ? schema_single_concentric : schema_concentric;
         else if (the_schema == schema_maybe_single_cross_concentric)
            the_schema = (final_concepts & INHERITFLAG_SINGLE) ? schema_single_cross_concentric : schema_cross_concentric;
         else if (the_schema == schema_maybe_matrix_conc_star)
            if (final_concepts & INHERITFLAG_12_MATRIX)
               the_schema = schema_conc_star12;
            else if (final_concepts & INHERITFLAG_16_MATRIX)
               the_schema = schema_conc_star16;
            else
               the_schema = schema_conc_star;
      }
   }

   ss->cmd.callspec = callspec;

   /* Enforce the restriction that only tagging calls are allowed in certain contexts. */

   if (final_concepts & FINAL__MUST_BE_TAG) {
      if (!(callspec->callflags1 & CFLAG1_BASE_TAG_CALL_MASK))
         fail("Only a tagging call is allowed here.");
   }

   final_concepts &= ~FINAL__MUST_BE_TAG;
   ss->cmd.cmd_final_flags = final_concepts;

   /* If the "split" concept has been given and this call uses that concept for a special
      meaning (split square thru, split dixie style), set the special flag to determine that
      action, and remove the split concept.  Why remove it?  So that "heads split catch grand
      mix 3" will work.  If we are doing a "split catch", we don't really want to split the
      setup into 2x2's that are isolated from each other, or else the "grand mix" won't work. */

   if (final_concepts & FINAL__SPLIT) {
      ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;
      if (callspec->callflags1 & CFLAG1_SPLIT_LIKE_SQUARE_THRU) {
         final_concepts = (final_concepts | FINAL__SPLIT_SQUARE_APPROVED) & (~FINAL__SPLIT);
         if ((current_number_fields & 0xF) == 1) fail("Can't split square thru 1.");
      }
      else if (callspec->callflags1 & CFLAG1_SPLIT_LIKE_DIXIE_STYLE)
         final_concepts = (final_concepts | FINAL__SPLIT_DIXIE_APPROVED) & (~FINAL__SPLIT);
   }

   ss->cmd.cmd_final_flags = final_concepts;

   /* NOTE: We may have mirror-reflected the setup.  "Mirror" is true if so.  We may need to undo this. */

   /* If this is the "split sequential" schema and we have not already done so,
      cause splitting to take place. */

   if (the_schema == schema_split_sequential) {
      if (      setup_attrs[ss->kind].setup_limits == 7 ||
               (setup_attrs[ss->kind].setup_limits == 11 && (final_concepts & INHERITFLAG_3X3)) ||
               (setup_attrs[ss->kind].setup_limits == 15 && (final_concepts & INHERITFLAG_4X4)))
         ss->cmd.cmd_misc_flags |= CMD_MISC__MUST_SPLIT;
      else if (setup_attrs[ss->kind].setup_limits != 3)
         fail("Need a 4 or 8 person setup for this.");
   }

   ss->cmd.cmd_final_flags = final_concepts;

   /* If the split concept is still present, do it. */

   if (final_concepts & FINAL__SPLIT) {
      map_thing *split_map;

      final_concepts &= ~FINAL__SPLIT;
      ss->cmd.cmd_final_flags = final_concepts;
      ss->cmd.cmd_misc_flags |= (CMD_MISC__SAID_SPLIT | CMD_MISC__NO_EXPAND_MATRIX);

      /* We can't handle the mirroring, so undo it. */
      if (mirror) { mirror_this(ss); mirror = FALSE; }

      if (ss->kind == s2x4) split_map = (*map_lists[s2x2][1])[MPKIND__SPLIT][0];
      else if (ss->kind == s1x8) split_map = (*map_lists[s1x4][1])[MPKIND__SPLIT][0];
      else if (ss->kind == s_ptpd) split_map = (*map_lists[sdmd][1])[MPKIND__SPLIT][0];
      else if (ss->kind == s_qtag) split_map = (*map_lists[sdmd][1])[MPKIND__SPLIT][1];
      else if (ss->kind == s2x2) {
         /* "Split" was given while we are already in a 2x2?  The only way that
            can be legal is if the word "split" was meant as a modifier for "split square thru"
            etc., rather than as a virtual-setup concept, or if the "split sequential" schema
            is in use.  In those cases, some "split approved" flag will still be on. */

         if (!(final_concepts & (FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED)))
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
         if (ss->cmd.cmd_frac_flags)
            fail("Can't fractionalize a call if no one is doing it.");
         result->kind = nothing;
         return;
      }
   }

   /* We can't handle the mirroring unless the schema is by_array, so undo it. */

   if (the_schema != schema_by_array) {
      if (mirror) { mirror_this(ss); mirror = FALSE; }
   }

   switch (the_schema) {
      case schema_nothing:
         if (final_concepts) fail("Illegal concept for this call.");
         *result = *ss;
         /* This call is a 1-person call, so it can be presumed to have split both ways. */
         result->result_flags = (ss->cmd.prior_elongation_bits & 3) | RESULTFLAG__SPLIT_AXIS_BIT*3;
         break;
      case schema_matrix:
         /* The "reverse" concept might mean mirror, as in "reverse truck". */

         if ((final_concepts & INHERITFLAG_REVERSE) && (callspec->callflagsh & INHERITFLAG_REVERSE)) {
            mirror_this(ss);
            mirror = TRUE;
            final_concepts &= ~INHERITFLAG_REVERSE;
         }

         if (final_concepts) fail("Illegal concept for this call.");
         matrixmove(ss, callspec, result);
         reinstate_rotation(ss, result);
         result->result_flags = 0;
         break;
      case schema_partner_matrix:
         if (final_concepts) fail("Illegal concept for this call.");
         partner_matrixmove(ss, callspec, result);
         reinstate_rotation(ss, result);
         result->result_flags = 0;
         break;
      case schema_roll:
         if (final_concepts) fail("Illegal concept for this call.");
         rollmove(ss, callspec, result);
         /* This call is a 1-person call, so it can be presumed to have split both ways. */
         result->result_flags = (ss->cmd.prior_elongation_bits & 3) | RESULTFLAG__SPLIT_AXIS_BIT*3;
         break;
      case schema_by_array:

         /* Dispose of the "left" concept first -- it can only mean mirror.  If it is on,
            mirroring may already have taken place. */

         if (final_concepts & INHERITFLAG_LEFT) {
/* ***** why isn't this particular error test taken care of more generally elsewhere? */
            if (!(callspec->callflagsh & INHERITFLAG_LEFT)) fail("Can't do this call 'left'.");
            if (!mirror) mirror_this(ss);
            mirror = TRUE;
            final_concepts &= ~INHERITFLAG_LEFT;
         }

         /* The "reverse" concept might mean mirror, or it might be genuine. */

         if ((final_concepts & INHERITFLAG_REVERSE) && (callspec->callflagsh & INHERITFLAG_REVERSE)) {
            /* This "reverse" just means mirror. */
            if (mirror) fail("Can't do this call 'left' and 'reverse'.");
            mirror_this(ss);
            mirror = TRUE;
            final_concepts &= ~INHERITFLAG_REVERSE;
         }

         /* If the "reverse" flag is still set in final_concepts, it means a genuine
            reverse as in reverse cut/flip the diamond or reverse change-O. */

         ss->cmd.callspec = callspec;
         ss->cmd.cmd_final_flags = final_concepts;
         basic_move(ss, tbonetest, qtfudged, mirror, result);
         break;
      default:
   
         /* Must be sequential or some form of concentric. */

         new_final_concepts = final_concepts;
   
         /* We demand that the final concepts that remain be only those in the following list,
            which includes all of the "heritable" concepts. */

         if (new_final_concepts &
               ~(FINAL__SPLIT | HERITABLE_FLAG_MASK | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED))
            fail("This concept not allowed here.");

         /* Now we figure out how to dispose of "heritable" concepts.  In general, we will selectively inherit them to
            the call's children, once we verify that the call accepts them for inheritance.  If it doesn't accept them,
            it is an error unless the concepts are the special ones "magic" and/or "interlocked", which we can dispose
            of by doing the call in the appropriate magic/interlocked setup. */

         unaccepted_flags = HERITABLE_FLAG_MASK & new_final_concepts & (~callspec->callflagsh);    /* The unaccepted flags. */

         if (unaccepted_flags != 0) {
            if (divide_for_magic(ss, new_final_concepts, unaccepted_flags, result))
               return;
            else
               fail("Can't do this call with this concept.");
         }

         if (the_schema == schema_sequential || the_schema == schema_split_sequential) {
            int highlimit;
            int subcall_incr;       /* Will be -1 if doing call in reverse order. */
            int subcall_index;      /* Where we start, in the absence of special stuff. */
            int instant_stop;       /* If >= 0, says to stop instantly after doing one part, and to report
                                       (in RESULTFLAG__DID_LAST_PART bit) if that part was the last part. */
            long_boolean reverse_order;
            long_boolean do_half_of_last_part;
            long_boolean first_call;
            int total = callspec->stuff.def.howmanyparts;

            qtf = qtfudged;

            if (new_final_concepts & FINAL__SPLIT) {
               if (callspec->callflags1 & CFLAG1_SPLIT_LIKE_SQUARE_THRU)
                  new_final_concepts |= FINAL__SPLIT_SQUARE_APPROVED;
               else if (callspec->callflags1 & CFLAG1_SPLIT_LIKE_DIXIE_STYLE)
                  new_final_concepts |= FINAL__SPLIT_DIXIE_APPROVED;

            }

            /* If the "cmd_frac_flags" word is nonzero, we are being asked to do something special.
               See comments in sd.h for "cmd_frac_flags". */

            if (ss->cmd.cmd_frac_flags) {
               fraction_info zzz = get_fraction_info(ss->cmd.cmd_frac_flags, callspec->callflags1, total);
               reverse_order = zzz.reverse_order;
               do_half_of_last_part = zzz.do_half_of_last_part;
               highlimit = zzz.highlimit;
               if (reverse_order) highlimit = 1-total+highlimit;
               subcall_index = zzz.subcall_index;
               subcall_incr = (reverse_order) ? -1 : 1;
               instant_stop = zzz.instant_stop ? subcall_index*subcall_incr+1 : 99;
            }
            else {     /* No fractions. */
               reverse_order = FALSE;
               instant_stop = 99;
               do_half_of_last_part = FALSE;
               highlimit = total;
               subcall_index = 0;
               subcall_incr = 1;
            }

            first_call = reverse_order ? FALSE : TRUE;

            if (ss->kind != s2x2 && ss->kind != s_short6) ss->cmd.prior_elongation_bits = 0;

            /* Did we neglect to do the touch/rear back stuff because fractionalization was enabled?
               If so, now is the time to correct that.  We only do it for the first part, and only if
               doing parts in forward order. */

            /* Test for all this is "random left, swing thru".
               The test cases for this stuff are such things as "left swing thru". */

            if ((!(ss->cmd.cmd_misc_flags & CMD_MISC__NO_STEP_TO_WAVE)) && (subcall_index == 0) && !reverse_order &&
                  (callspec->callflags1 & (CFLAG1_REAR_BACK_FROM_R_WAVE | CFLAG1_REAR_BACK_FROM_QTAG | CFLAG1_STEP_TO_WAVE))) {

               ss->cmd.cmd_misc_flags |= CMD_MISC__NO_STEP_TO_WAVE;  /* Can only do it once. */

               if (new_final_concepts & INHERITFLAG_LEFT) {
                  if (!mirror) mirror_this(ss);
                  mirror = TRUE;
               }

               touch_or_rear_back(ss, mirror, callspec->callflags1);
            }

            /* See comments above relating to "left_means_touch_or_check".  We are about to
               un-mirror this in any case. */

            if (callspec->callflags1 & CFLAG1_LEFT_MEANS_TOUCH_OR_CHECK)
               final_concepts &= ~INHERITFLAG_LEFT;

            /* See comment earlier about mirroring.  For sequentially or concentrically defined
               calls, the "left" flag does not mean mirror; it is simply passed to subcalls.
               So we must put things into their normal state.  If we did any mirroring, it was
               only to facilitate the action of "touch_or_rear_back". */

            if (mirror) mirror_this(ss);
            mirror = FALSE;
            *result = *ss;

            result->result_flags = RESULTFLAG__SPLIT_AXIS_MASK;   /* Seed the result for the calls to "do_call_in_series". */

            /* Iterate over the parts of the call. */

            for (;;) {
               int j;
               uint32 temp_concepts, conc1, conc2;
               by_def_item *this_item;
               uint32 this_mod1, this_modh;
               uint32 saved_number_fields = current_number_fields;
               int count_to_use;

               if (subcall_index*subcall_incr >= highlimit) break;

               if (subcall_index >= total) fail("The indicated part number doesn't exist.");

               this_item = &callspec->stuff.def.defarray[subcall_index];

               this_mod1 = this_item->modifiers1;
               this_modh = this_item->modifiersh;

               temp_concepts = get_mods_for_subcall(new_final_concepts, this_modh, callspec->callflagsh);
               get_real_subcall(parseptr, this_item, temp_concepts, &cp1, &call1, &conc1);

               /* If this context requires a tagging or scoot call, pass that fact on. */
               if (this_item->call_id >= BASE_CALL_TAGGER0 && this_item->call_id <= BASE_CALL_TAGGER3) conc1 |= FINAL__MUST_BE_TAG;

               current_number_fields >>= ((DFM1_NUM_SHIFT_MASK & this_mod1) / DFM1_NUM_SHIFT_BIT) * 4;
               count_to_use = current_number_fields & 0xF;

               if ((DFM1_REPEAT_N | DFM1_REPEAT_NM1) & this_mod1) {
                  uint32 remember_elongation = result->cmd.prior_elongation_bits;

                  number_used = TRUE;
                  if (DFM1_REPEAT_NM1 & this_mod1) count_to_use--;

                  for (j = 1; j <= count_to_use; j++) {
                     result->cmd = ss->cmd;
                     result->cmd.cmd_frac_flags = 0;
                     if (!first_call) {
                        result->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
                        result->cmd.cmd_assume.assumption = cr_none;
                     }
                     result->cmd.prior_elongation_bits = remember_elongation;
                     result->cmd.parseptr = cp1;
                     result->cmd.callspec = call1;
                     result->cmd.cmd_final_flags = conc1;

                     if ((DFM1_CPLS_UNLESS_SINGLE & this_mod1) && !(new_final_concepts & INHERITFLAG_SINGLE))
                        result->cmd.cmd_misc_flags |= CMD_MISC__DO_AS_COUPLES;

                     do_call_in_series(
                        result,
                        DFM1_ROLL_TRANSPARENT & this_mod1,
                        !(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) &&
                           !(new_final_concepts & (INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX)),
                        qtf);
                  }

               }
               else if (DFM1_REPEAT_N_ALTERNATE & this_mod1) {
                  uint32 remember_elongation = result->cmd.prior_elongation_bits;
                  /* Read the call after this one -- we will alternate between the two. */
                  by_def_item *alt_item = &callspec->stuff.def.defarray[subcall_index+subcall_incr];
                  uint32 alt_concepts = get_mods_for_subcall(new_final_concepts, alt_item->modifiersh, callspec->callflagsh);
                  get_real_subcall(parseptr, alt_item, alt_concepts, &cp2, &call2, &conc2);

                  number_used = TRUE;

                  for (j = 1; j <= count_to_use; j++) {
                     result->cmd = ss->cmd;
                     result->cmd.cmd_frac_flags = 0;
                     if (!first_call) {
                        result->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
                        result->cmd.cmd_assume.assumption = cr_none;
                     }
                     result->cmd.prior_elongation_bits = remember_elongation;

                     if (j&1) {
                        result->cmd.parseptr = cp1;
                        result->cmd.callspec = call1;
                        result->cmd.cmd_final_flags = conc1;
                     }
                     else {
                        result->cmd.parseptr = cp2;
                        result->cmd.callspec = call2;
                        result->cmd.cmd_final_flags = conc2;
                     }

                     if ((DFM1_CPLS_UNLESS_SINGLE & this_mod1) && !(new_final_concepts & INHERITFLAG_SINGLE))
                        result->cmd.cmd_misc_flags |= CMD_MISC__DO_AS_COUPLES;

                     do_call_in_series(
                        result,
                        DFM1_ROLL_TRANSPARENT & this_mod1,
                        !(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) &&
                           !(new_final_concepts & (INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX)),
                        qtf);
                  }
                  subcall_index += subcall_incr;     /* Skip over the second call. */
               }
               else {
                  uint32 remember_elongation = result->cmd.prior_elongation_bits;

                  result->cmd = ss->cmd;

                  if (do_half_of_last_part && subcall_index+1 == highlimit)
                     result->cmd.cmd_frac_flags = 0x000112;
                  else
                     result->cmd.cmd_frac_flags = 0;

                  /* We don't supply these; they get filled in by the call. */
                  result->cmd.cmd_misc_flags &= ~DFM1_CONCENTRICITY_FLAG_MASK;
                  if (!first_call) {
                     result->cmd.cmd_misc_flags |= CMD_MISC__NO_CHK_ELONG;
                     result->cmd.cmd_assume.assumption = cr_none;
                  }
                  result->cmd.parseptr = cp1;
                  result->cmd.callspec = call1;
                  result->cmd.cmd_final_flags = conc1;
                  result->cmd.prior_elongation_bits = remember_elongation;

                  if ((DFM1_CPLS_UNLESS_SINGLE & this_mod1) && !(new_final_concepts & INHERITFLAG_SINGLE))
                     result->cmd.cmd_misc_flags |= CMD_MISC__DO_AS_COUPLES;

                  do_call_in_series(
                     result,
                     DFM1_ROLL_TRANSPARENT & this_mod1,
                     !(ss->cmd.cmd_misc_flags & CMD_MISC__EXPLICIT_MATRIX) &&
                        !(new_final_concepts & (INHERITFLAG_12_MATRIX | INHERITFLAG_16_MATRIX)),
                     qtf);
               }

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

               current_number_fields = saved_number_fields;

               qtf = FALSE;

               new_final_concepts &= ~(FINAL__SPLIT | FINAL__SPLIT_SQUARE_APPROVED | FINAL__SPLIT_DIXIE_APPROVED);

               subcall_index += subcall_incr;
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
         else {
            setup_command foo1, foo2;
            uint32 temp_concepts, conc1, conc2;

            /* Must be some form of concentric. */

            if (ss->cmd.cmd_misc_flags & CMD_MISC__MUST_SPLIT)
               fail("Can't split this call.");

            ss->cmd.cmd_misc_flags |= CMD_MISC__NO_EXPAND_MATRIX;     /* We think this is the right thing to do. */
            saved_warnings = history[history_ptr+1].warnings;

            temp_concepts = get_mods_for_subcall(new_final_concepts, callspec->stuff.conc.innerdef.modifiersh, callspec->callflagsh);
            get_real_subcall(parseptr, &callspec->stuff.conc.innerdef, temp_concepts, &cp1, &call1, &conc1);

            /* Do it again. */

            temp_concepts = get_mods_for_subcall(new_final_concepts, callspec->stuff.conc.outerdef.modifiersh, callspec->callflagsh);
            get_real_subcall(parseptr, &callspec->stuff.conc.outerdef, temp_concepts, &cp2, &call2, &conc2);

            /* Fudge a 3x4 into a 1/4-tag if appropriate. */

            if (ss->kind == s3x4 && (callspec->callflags1 & CFLAG1_FUDGE_TO_Q_TAG) &&
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
            else if (ss->kind == s_qtag && (callspec->callflags1 & CFLAG1_FUDGE_TO_Q_TAG) &&
                  the_schema == schema_conc_triple_lines) {
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

            foo1.parseptr = cp1;
            foo1.callspec = call1;
            foo1.cmd_final_flags = conc1;
            foo2.parseptr = cp2;
            foo2.callspec = call2;
            foo2.cmd_final_flags = conc2;

            concentric_move(
               ss, &foo1, &foo2,
               the_schema,
               callspec->stuff.conc.innerdef.modifiers1,
               callspec->stuff.conc.outerdef.modifiers1,
               result);

            if (DFM1_SUPPRESS_ELONGATION_WARNINGS & callspec->stuff.conc.outerdef.modifiers1) {
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
   uint32 new_final_concepts;
   uint32 check_concepts;
   parse_block *parseptrcopy;
   parse_block *parseptr = ss->cmd.parseptr;

   /* This shouldn't be necessary, but there have been occasional reports of the
      bigblock and stagger concepts getting confused with each other.  This would happen
      if the incoming 4x4 did not have its rotation field equal to zero, as is required
      when in canonical form.  So we check this. */

   if (setup_attrs[ss->kind].four_way_symmetry && ss->rotation != 0)
      fail("There is a bug in 4 way canonicalization -- please report this sequence.");

   if (ss->cmd.cmd_misc_flags & CMD_MISC__DO_AS_COUPLES) {
      ss->cmd.cmd_misc_flags &= ~CMD_MISC__DO_AS_COUPLES;
      tandem_couples_move(ss, selector_uninitialized, 0, 0, 0, 1, result);
      return;
   }

   if (ss->cmd.callspec) {
      move_with_real_call(ss, qtfudged, result);
      return;
   }

   /* Scan the "final" concepts, remembering them and their end point. */
   last_magic_diamond = 0;
   parseptrcopy = process_final_concepts(parseptr, TRUE, &new_final_concepts);

   saved_magic_diamond = last_magic_diamond;

   /* See if there were any "non-final" ones present also. */

   new_final_concepts |= ss->cmd.cmd_final_flags;         /* Include any old ones we had. */

   if (parseptrcopy->concept->kind <= marker_end_of_list) {
      uint32 saved_number_fields = current_number_fields;
      selector_kind saved_selector = current_selector;
      direction_kind saved_direction = current_direction;

      /* There are no "non-final" concepts.  The only concepts are the final ones that
         have been encoded into new_final_concepts. */

      /* We must read the selector out of the concept list and use it for this call to "move".
         We are effectively using it as an argument to "move", with all the care that must go
         into invocations of recursive procedures.  However, at its point of actual use, it
         must be in a global variable.  Therefore, we explicitly save and restore that
         global variable (in a dynamic variable local to this instance) rather than passing
         it as an explicit argument.  By saving it and restoring it in this way, we make
         things like "checkpoint bounce the beaus by bounce the belles" work. */

      current_selector = parseptrcopy->selector;
      current_direction = parseptrcopy->direction;
      current_number_fields = parseptrcopy->number;
      ss->cmd.parseptr = parseptrcopy;
      ss->cmd.callspec = parseptrcopy->call;
      ss->cmd.cmd_final_flags = new_final_concepts;
      move_with_real_call(ss, qtfudged, result);
      current_selector = saved_selector;
      current_direction = saved_direction;
      current_number_fields = saved_number_fields;
   }
   else {
      /* We now know that there are "non-final" (virtual setup) concepts present. */

      /* These are the concepts that we are interested in. */

      check_concepts = new_final_concepts & ~FINAL__MUST_BE_TAG;

      result->result_flags = 0;

      if (check_concepts == 0 || check_concepts == INHERITFLAG_REVERSE) {
         /* Look for virtual setup concept that can be done by dispatch from table, with no
            intervening final concepts. */
   
         ss->cmd.parseptr = parseptrcopy;
         ss->cmd.cmd_final_flags = new_final_concepts;

         /* We know that ss->callspec is null. */
         /* We do not know that ss->cmd.cmd_final_flags is null.  It may contain
            FINAL__MUST_BE_TAG and/or INHERITFLAG_REVERSE.  The code for doing hairy
            concepts used to just ignore those, passing zero for the final commands.
            This may be a bug.  In any case, we have now preserved even those two flags
            in the cmd_final_flags, so things can possibly get better. */

         if (do_big_concept(ss, result)) {
            canonicalize_rotation(result);
            return;
         }
      }

      clear_people(result);

      if (ss->cmd.cmd_misc_flags & CMD_MISC__CENTRAL_MASK)
         fail("Can't do \"central/snag/mystic\" followed by another concept or modifier.");

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
      ss->cmd.callspec = NULLCALLSPEC;
      ss->cmd.cmd_final_flags = new_final_concepts;

      /* We can tolerate the "matrix" flag if we are going to do "split".  For anything else,
         "matrix" is illegal. */

      if (check_concepts == FINAL__SPLIT) {
         map_thing *split_map;
   
         ss->cmd.cmd_misc_flags |= CMD_MISC__SAID_SPLIT;

         if (ss->kind == s2x4) split_map = (*map_lists[s2x2][1])[MPKIND__SPLIT][0];
         else if (ss->kind == s1x8) split_map = (*map_lists[s1x4][1])[MPKIND__SPLIT][0];
         else if (ss->kind == s_ptpd) split_map = (*map_lists[sdmd][1])[MPKIND__SPLIT][0];
         else if (ss->kind == s_qtag) split_map = (*map_lists[sdmd][1])[MPKIND__SPLIT][1];
         else fail("Can't do split concept in this setup.");

         ss->cmd.cmd_final_flags &= ~FINAL__SPLIT;
         divided_setup_move(ss, split_map, phantest_ok, TRUE, result);
      }
      else {
         if (ss->cmd.cmd_misc_flags & CMD_MISC__MATRIX_CONCEPT)
            fail("\"Matrix\" concept must be followed by applicable concept.");

         if (divide_for_magic(
               ss,
               ss->cmd.cmd_final_flags,
               check_concepts & ~INHERITFLAG_DIAMOND,
               result)) {
         }
         else if (check_concepts == INHERITFLAG_DIAMOND) {
            ss->cmd.cmd_final_flags &= ~INHERITFLAG_DIAMOND;

            if (ss->kind == sdmd)
               divided_setup_move(ss, (*map_lists[s1x2][1])[MPKIND__DMD_STUFF][0], phantest_ok, TRUE, result);
            else {
               /* Divide into diamonds and try again.  (Note that we back up the concept pointer.) */
               ss->cmd.parseptr = parseptr;
               ss->cmd.cmd_final_flags = 0;
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
