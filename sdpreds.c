/* SD -- square dance caller's helper.

    Copyright (C) 1990-1996  William B. Ackerman.

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

/* This defines the following function:
   selectp

and the following external variables:
   selector_used
   number_used
   mandatory_call_used
   pred_table     which is filled with pointers to the predicate functions
*/

#include "sd.h"

/* These variables are external. */

long_boolean selector_used;
long_boolean number_used;
long_boolean mandatory_call_used;


/* If a real person and a person under test are XOR'ed, the result AND'ed with this constant,
   and the low bits of that result examined, it will tell whether the person under test is real
   and facing in the same direction (result = 0), the opposite direction (result == 2), or whatever. */

#define DIR_MASK (BIT_PERSON | 3)

#define ID1_PERM_ALL_ID (ID1_PERM_HEAD|ID1_PERM_SIDE|ID1_PERM_BOY|ID1_PERM_GIRL)





extern long_boolean selectp(setup *ss, int place)
{
   uint32 permpid1, pid2;

   selector_used = TRUE;

   /* Pull out the cases that do not require the person to be real. */

   switch (current_selector) {
      case selector_all:
         return TRUE;
      case selector_none:
         return FALSE;
   }

   /* Demand that the subject be real. */

   if (!(ss->people[place].id1 & BIT_PERSON))
      fail("Can't decide who are selected.");

   permpid1 = ss->people[place].id1;
   pid2 = ss->people[place].id2;

   switch (current_selector) {
      case selector_boys:
         if      ((permpid1 & (ID1_PERM_BOY|ID1_PERM_GIRL)) == ID1_PERM_BOY) return TRUE;
         else if ((permpid1 & (ID1_PERM_BOY|ID1_PERM_GIRL)) == ID1_PERM_GIRL) return FALSE;
         break;
      case selector_girls:
         if      ((permpid1 & (ID1_PERM_BOY|ID1_PERM_GIRL)) == ID1_PERM_GIRL) return TRUE;
         else if ((permpid1 & (ID1_PERM_BOY|ID1_PERM_GIRL)) == ID1_PERM_BOY) return FALSE;
         break;
      case selector_heads:
         if      ((permpid1 & (ID1_PERM_HEAD|ID1_PERM_SIDE)) == ID1_PERM_HEAD) return TRUE;
         else if ((permpid1 & (ID1_PERM_HEAD|ID1_PERM_SIDE)) == ID1_PERM_SIDE) return FALSE;
         break;
      case selector_sides:
         if      ((permpid1 & (ID1_PERM_HEAD|ID1_PERM_SIDE)) == ID1_PERM_SIDE) return TRUE;
         else if ((permpid1 & (ID1_PERM_HEAD|ID1_PERM_SIDE)) == ID1_PERM_HEAD) return FALSE;
         break;
      case selector_headcorners:
         if      ((permpid1 & (ID1_PERM_HCOR|ID1_PERM_SCOR)) == ID1_PERM_HCOR) return TRUE;
         else if ((permpid1 & (ID1_PERM_HCOR|ID1_PERM_SCOR)) == ID1_PERM_SCOR) return FALSE;
         break;
      case selector_sidecorners:
         if      ((permpid1 & (ID1_PERM_HCOR|ID1_PERM_SCOR)) == ID1_PERM_SCOR) return TRUE;
         else if ((permpid1 & (ID1_PERM_HCOR|ID1_PERM_SCOR)) == ID1_PERM_HCOR) return FALSE;
         break;
      case selector_headboys:
         if      ((permpid1 & (ID1_PERM_ALL_ID|ID1_PERM_NHB)) == (ID1_PERM_HEAD|ID1_PERM_BOY)) return TRUE;
         else if ((permpid1 & ID1_PERM_NHB) == ID1_PERM_NHB) return FALSE;
         break;
      case selector_headgirls:
         if      ((permpid1 & (ID1_PERM_ALL_ID|ID1_PERM_NHG)) == (ID1_PERM_HEAD|ID1_PERM_GIRL)) return TRUE;
         else if ((permpid1 & ID1_PERM_NHG) == ID1_PERM_NHG) return FALSE;
         break;
      case selector_sideboys:
         if      ((permpid1 & (ID1_PERM_ALL_ID|ID1_PERM_NSB)) == (ID1_PERM_SIDE|ID1_PERM_BOY)) return TRUE;
         else if ((permpid1 & ID1_PERM_NSB) == ID1_PERM_NSB) return FALSE;
         break;
      case selector_sidegirls:
         if      ((permpid1 & (ID1_PERM_ALL_ID|ID1_PERM_NSG)) == (ID1_PERM_SIDE|ID1_PERM_GIRL)) return TRUE;
         else if ((permpid1 & ID1_PERM_NSG) == ID1_PERM_NSG) return FALSE;
         break;
      case selector_centers:
         if      ((pid2 & (ID2_CENTER|ID2_END)) == ID2_CENTER) return TRUE;
         else if ((pid2 & (ID2_CENTER|ID2_END)) == ID2_END) return FALSE;
         break;
      case selector_ends:
         if      ((pid2 & (ID2_CENTER|ID2_END)) == ID2_END) return TRUE;
         else if ((pid2 & (ID2_CENTER|ID2_END)) == ID2_CENTER) return FALSE;
         break;
      case selector_leads:
         if      ((pid2 & (ID2_LEAD|ID2_TRAILER)) == ID2_LEAD) return TRUE;
         else if ((pid2 & (ID2_LEAD|ID2_TRAILER)) == ID2_TRAILER) return FALSE;
         break;
      case selector_trailers:
         if      ((pid2 & (ID2_LEAD|ID2_TRAILER)) == ID2_TRAILER) return TRUE;
         else if ((pid2 & (ID2_LEAD|ID2_TRAILER)) == ID2_LEAD) return FALSE;
         break;
      case selector_beaus:
         if      ((pid2 & (ID2_BEAU|ID2_BELLE)) == ID2_BEAU) return TRUE;
         else if ((pid2 & (ID2_BEAU|ID2_BELLE)) == ID2_BELLE) return FALSE;
         break;
      case selector_belles:
         if      ((pid2 & (ID2_BEAU|ID2_BELLE)) == ID2_BELLE) return TRUE;
         else if ((pid2 & (ID2_BEAU|ID2_BELLE)) == ID2_BEAU) return FALSE;
         break;
      case selector_center2:
         if      ((pid2 & (ID2_CTR2|ID2_OUTR6)) == ID2_CTR2) return TRUE;
         else if ((pid2 & (ID2_CTR2|ID2_OUTR6)) == ID2_OUTR6) return FALSE;
         break;
      case selector_center6:
         if      ((pid2 & (ID2_CTR6|ID2_OUTR2)) == ID2_CTR6) return TRUE;
         else if ((pid2 & (ID2_CTR6|ID2_OUTR2)) == ID2_OUTR2) return FALSE;
         break;
      case selector_outer2:
         if      ((pid2 & (ID2_CTR6|ID2_OUTR2)) == ID2_OUTR2) return TRUE;
         else if ((pid2 & (ID2_CTR6|ID2_OUTR2)) == ID2_CTR6) return FALSE;
         break;
      case selector_outer6:
         if      ((pid2 & (ID2_CTR2|ID2_OUTR6)) == ID2_OUTR6) return TRUE;
         else if ((pid2 & (ID2_CTR2|ID2_OUTR6)) == ID2_CTR2) return FALSE;
         break;
      case selector_ctrdmd:
         if      ((pid2 & (ID2_CTRDMD|ID2_NCTRDMD)) == ID2_CTRDMD) return TRUE;
         else if ((pid2 & (ID2_CTRDMD|ID2_NCTRDMD)) == ID2_NCTRDMD) return FALSE;
         break;
      case selector_ctr_1x4:
         if      ((pid2 & (ID2_CTR1X4|ID2_NCTR1X4)) == ID2_CTR1X4) return TRUE;
         else if ((pid2 & (ID2_CTR1X4|ID2_NCTR1X4)) == ID2_NCTR1X4) return FALSE;
         break;
      case selector_ctr_1x6:
         if      ((pid2 & (ID2_CTR1X6|ID2_NCTR1X6)) == ID2_CTR1X6) return TRUE;
         else if ((pid2 & (ID2_CTR1X6|ID2_NCTR1X6)) == ID2_NCTR1X6) return FALSE;
         break;
      case selector_center4:
         if      ((pid2 & (ID2_CTR4|ID2_OUTRPAIRS)) == ID2_CTR4) return TRUE;
         else if ((pid2 & (ID2_CTR4|ID2_OUTRPAIRS)) == ID2_OUTRPAIRS) return FALSE;
         break;
      case selector_outerpairs:
         if      ((pid2 & (ID2_CTR4|ID2_OUTRPAIRS)) == ID2_OUTRPAIRS) return TRUE;
         else if ((pid2 & (ID2_CTR4|ID2_OUTRPAIRS)) == ID2_CTR4) return FALSE;
         break;
      case selector_headliners:
         if      ((pid2 & (ID2_HEADLINE|ID2_SIDELINE)) == ID2_HEADLINE) return TRUE;
         else if ((pid2 & (ID2_HEADLINE|ID2_SIDELINE)) == ID2_SIDELINE) return FALSE;
         break;
      case selector_sideliners:
         if      ((pid2 & (ID2_HEADLINE|ID2_SIDELINE)) == ID2_SIDELINE) return TRUE;
         else if ((pid2 & (ID2_HEADLINE|ID2_SIDELINE)) == ID2_HEADLINE) return FALSE;
         break;
      case selector_nearline:
         if      (pid2 & ID2_NEARLINE) return TRUE;
         else if (pid2 & (ID2_FARLINE|ID2_FARCOL|ID2_FARBOX)) return FALSE;
         break;
      case selector_farline:
         if      (pid2 & ID2_FARLINE) return TRUE;
         else if (pid2 & (ID2_NEARLINE|ID2_NEARCOL|ID2_NEARBOX)) return FALSE;
         break;
      case selector_nearcolumn:
         if      (pid2 & ID2_NEARCOL) return TRUE;
         else if (pid2 & (ID2_FARLINE|ID2_FARCOL|ID2_FARBOX)) return FALSE;
         break;
      case selector_farcolumn:
         if      (pid2 & ID2_FARCOL) return TRUE;
         else if (pid2 & (ID2_NEARLINE|ID2_NEARCOL|ID2_NEARBOX)) return FALSE;
         break;
      case selector_nearbox:
         if      (pid2 & ID2_NEARBOX) return TRUE;
         else if (pid2 & (ID2_FARLINE|ID2_FARCOL|ID2_FARBOX)) return FALSE;
         break;
      case selector_farbox:
         if      (pid2 & ID2_FARBOX) return TRUE;
         else if (pid2 & (ID2_NEARLINE|ID2_NEARCOL|ID2_NEARBOX)) return FALSE;
         break;
      case selector_facingfront:
         if      (pid2 & ID2_FACEFRONT) return TRUE;
         else if (pid2 & ID2_FACEBACK) return FALSE;
         break;
      case selector_facingback:
         if      (pid2 & ID2_FACEBACK) return TRUE;
         else if (pid2 & ID2_FACEFRONT) return FALSE;
         break;
      default:
         fail("ERROR - selector failed to get initialized.");
   }
   
   fail("Can't decide who are selected.");
   /* NOTREACHED */
}



/* Here are the predicates.  They will get put into the array "pred_table". */

/* ARGSUSED */
Private long_boolean selected(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return selectp(real_people, real_index);
}

/* ARGSUSED */
Private long_boolean unselect(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return !selectp(real_people, real_index);
}

/* ARGSUSED */
Private long_boolean select_near_select(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_all) return TRUE;

   return      (real_people->people[real_index ^ 1].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 1);
}

/* ARGSUSED */
Private long_boolean select_near_unselect(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_all) return FALSE;

   return      !(real_people->people[real_index ^ 1].id1 & BIT_PERSON) ||
               !selectp(real_people, real_index ^ 1);
}

/* ARGSUSED */
Private long_boolean unselect_near_select(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_none) return FALSE;

   return      (real_people->people[real_index ^ 1].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 1);
}

/* ARGSUSED */
Private long_boolean unselect_near_unselect(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_none) return TRUE;

   return      !(real_people->people[real_index ^ 1].id1 & BIT_PERSON) ||
               !selectp(real_people, real_index ^ 1);
}

/* ARGSUSED */
Private long_boolean select_once_rem_from_select(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_all) return TRUE;

   if (real_people->kind == s2x4)
      return   (real_people->people[real_index ^ 2].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 2);
   else
      return   (real_people->people[real_index ^ 3].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 3);


}

/* ARGSUSED */
Private long_boolean conc_from_select(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return selectp(real_people, real_index ^ 2);
}

/* ARGSUSED */
Private long_boolean select_once_rem_from_unselect(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_all) return FALSE;

   if (real_people->kind == s2x4)
      return   !(real_people->people[real_index ^ 2].id1 & BIT_PERSON) ||
               !selectp(real_people, real_index ^ 2);
   else
      return   !(real_people->people[real_index ^ 3].id1 & BIT_PERSON) ||
               !selectp(real_people, real_index ^ 3);
}

/* ARGSUSED */
Private long_boolean unselect_once_rem_from_select(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (selectp(real_people, real_index)) return FALSE;
   if (current_selector == selector_none) return FALSE;

   if (real_people->kind == s2x4)
      return   (real_people->people[real_index ^ 2].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 2);
   else
      return   (real_people->people[real_index ^ 3].id1 & BIT_PERSON) &&
               selectp(real_people, real_index ^ 3);
}

/* ARGSUSED */
Private long_boolean select_and_roll_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return selectp(real_people, real_index) && (real_people->people[real_index].id1 & ROLLBITR) != 0;
}

/* ARGSUSED */
Private long_boolean select_and_roll_is_ccw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return selectp(real_people, real_index) && (real_people->people[real_index].id1 & ROLLBITL) != 0;
}

/* ARGSUSED */
Private long_boolean always(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return TRUE;
}

/* ARGSUSED */
Private long_boolean x22_miniwave(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person, other_index, other_person;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: return TRUE;
      case cr_2fl_only: case cr_couples_only: return FALSE;
   }

   this_person = real_people->people[real_index].id1;
   other_index = real_index ^ (((real_direction << 1) & 2) ^ 1);
   other_person = real_people->people[other_index].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean x22_couple(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person, other_index, other_person;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: return FALSE;
      case cr_2fl_only: case cr_couples_only: return TRUE;
   }

   this_person = real_people->people[real_index].id1;
   other_index = real_index ^ (((real_direction << 1) & 2) ^ 1);
   other_person = real_people->people[other_index].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* ARGSUSED */
Private long_boolean x22_facing_someone(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_index = real_index ^ (((real_direction << 1) & 2) ^ 3);
   int other_person = real_people->people[other_index].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean x22_tandem_with_someone(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_index = real_index ^ (((real_direction << 1) & 2) ^ 3);
   int other_person = real_people->people[other_index].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* ARGSUSED */
Private long_boolean cols_someone_in_front(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->kind == s2x3) {
      if (real_index == 1 || real_index == 4) {
         if (real_people->people[real_index+((northified_index==1) ? 1 : -1)].id1)
            return TRUE;
         else return FALSE;
      }
      else return TRUE;
   }
   else {
      if (northified_index < 3 || northified_index > 4) {
         if (real_people->people[real_index+((northified_index<4) ? 1 : -1)].id1)
            return TRUE;
         else return FALSE;
      }
      else return FALSE;
   }
}

/* ARGSUSED */
Private long_boolean x14_once_rem_miniwave(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 3].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean x14_once_rem_couple(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 3].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* ARGSUSED */
Private long_boolean lines_couple(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return FALSE;
   else if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return TRUE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ 1].id1;
      if (real_people->kind == s1x6 && real_index >= 2)
         other_person = real_people->people[7 - real_index].id1;
      return ((this_person ^ other_person) & DIR_MASK) == 0;
   }
}

/* ARGSUSED */
Private long_boolean lines_miniwave(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return TRUE;
   else if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return FALSE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ 1].id1;
      if (real_people->kind == s1x6 && real_index >= 2)
         other_person = real_people->people[7 - real_index].id1;
      return ((this_person ^ other_person) & DIR_MASK) == 2;
   }
}

/* ARGSUSED */
Private long_boolean cast_normal(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only ||
         real_people->cmd.cmd_assume.assump_cast)
      return TRUE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ 1].id1;
      if (real_people->kind == s1x6 && real_index >= 2)
         other_person = real_people->people[7 - real_index].id1;
      return ((this_person ^ other_person) & DIR_MASK) == 2;
   }
}

/* ARGSUSED */
Private long_boolean cast_pushy(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only ||
         real_people->cmd.cmd_assume.assump_cast)
      return FALSE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ 1].id1;
      if (real_people->kind == s1x6 && real_index >= 2)
         other_person = real_people->people[7 - real_index].id1;
      return ((this_person ^ other_person) & DIR_MASK) == 0;
   }
}

/* ARGSUSED */
Private long_boolean cast_normal_or_warn(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only ||
         real_people->cmd.cmd_assume.assump_cast)
      return TRUE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ 1].id1;
      if (real_people->kind == s1x6 && real_index >= 2)
         other_person = real_people->people[7 - real_index].id1;

      switch ((this_person ^ other_person) & DIR_MASK) {
         case 2:
            return TRUE;
         case 0:
            return FALSE;
         default:
            /* Don't give the warning if person would have known what to do anyway. */
            if (real_index != 1 && real_index != ((real_people->kind == s1x6) ? 4 : 3))
               warn(warn__opt_for_normal_cast);
            return TRUE;
      }
   }
}

/* ARGSUSED */
Private long_boolean columns_tandem(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: case cr_2fl_only: return TRUE;
      case cr_magic_only: case cr_li_lo: return FALSE;
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 1].id1;
   if (real_people->kind == s1x6 && real_index >= 2)
      other_person = real_people->people[7 - real_index].id1;
   return ((this_person ^ other_person) & DIR_MASK) == 0;
}

/* ARGSUSED */
Private long_boolean columns_antitandem(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_magic_only: case cr_li_lo: return TRUE;
      case cr_wave_only: case cr_2fl_only: return FALSE;
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 1].id1;
   if (real_people->kind == s1x6 && real_index >= 2)
      other_person = real_people->people[7 - real_index].id1;
   return ((this_person ^ other_person) & DIR_MASK) == 2;
}

/* ARGSUSED */
Private long_boolean same_in_pair(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   if (real_people->cmd.cmd_assume.assump_col == 1) {
      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_2fl_only: case cr_li_lo: return TRUE;
         case cr_wave_only: case cr_magic_only: return FALSE;
      }
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 7].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* ARGSUSED */
Private long_boolean opp_in_pair(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   if (real_people->cmd.cmd_assume.assump_col == 1) {
      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_wave_only: case cr_magic_only: return TRUE;
         case cr_2fl_only: case cr_li_lo: return FALSE;
      }
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 7].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean opp_in_magic(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   if (real_people->cmd.cmd_assume.assump_col == 1) {
      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_wave_only: case cr_li_lo: return TRUE;
         case cr_2fl_only: case cr_magic_only: return FALSE;
      }
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 6].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean same_in_magic(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person;
   int other_person;

   if (real_people->cmd.cmd_assume.assump_col == 1) {
      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_2fl_only: case cr_magic_only: return TRUE;
         case cr_wave_only: case cr_li_lo: return FALSE;
      }
   }

   this_person = real_people->people[real_index].id1;
   other_person = real_people->people[real_index ^ 6].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* ARGSUSED */
Private long_boolean lines_once_rem_miniwave(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 2].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 2);
}

/* ARGSUSED */
Private long_boolean lines_once_rem_couple(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 2].id1;
   return(((this_person ^ other_person) & DIR_MASK) == 0);
}

/* Next test is used for figuring out how to hinge. */
/* Legal from 1x2. */
/* Returns TRUE if this person is beau, or else this belle is in a miniwave with
   the other person.  Returns FALSE if this belle is in a couple with the
   other person.  Complains if this belle can't tell. */
/* ARGSUSED */
Private long_boolean x12_beau_or_miniwave(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only || northified_index == 0)
      return TRUE;
   else if (real_people->cmd.cmd_assume.assumption == cr_couples_only)
      return FALSE;
   else {
      int other_person = real_people->people[real_index ^ 1].id1;
      int direction_diff = other_person ^ real_direction;
      if (!other_person || (direction_diff & 1))
         fail("Need a real, not T-boned, person to work with."); 
      return ((direction_diff & 2) == 2);
   }
}

/* Test for wheel and deal to be done 2FL-style, or beau side of 1FL.  Returns
   false if belle side of 1FL.  Raises an error if wheel and deal can't be done. */
/* ARGSUSED */
Private long_boolean x14_wheel_and_deal(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   /* We assume people have already been checked for coupleness. */

   if (northified_index <= 1)
      /* We are in the beau-side couple -- it's OK. */
      return TRUE;
   else {
      /* We are in the belle-side couple.  First, see if an assumption is guiding us. */

      int other_people;

      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_2fl_only: return TRUE;
         case cr_1fl_only: return FALSE;
      }

      /* Find the two people in the other couple.
         Just "or" them to be sure we get someone.  They are already known
         to be facing consistently if they are both there. */
      other_people = real_people->people[real_index ^ 2].id1 |
                     real_people->people[real_index ^ 3].id1;

      /* At least one of those people must exist. */
      if (!other_people)
         fail("Can't tell how to do this -- no live people.");

      /* See if they face the same way as myself.  Note that the "2" bit of
         real_index is the complement of my own direction bit. */
      if (((other_people ^ real_index) & 2))
         return FALSE;   /* This is a 1FL. */

      return TRUE;       /* This is a 2FL. */
   }
}

/* Test for 3X3 wheel_and_deal to be done 2FL-style, or beau side of 1FL. */
/* ARGSUSED */
Private long_boolean x16_wheel_and_deal(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   /* We assume people have already been checked for coupleness. */

   if (northified_index <= 2 || northified_index >= 9)
      /* We are in the beau-side triad -- it's OK. */
      return(TRUE);
   else {
      /* We are in the belle-side triad -- find the three people in the other triad.
         Just "or" them to be sure we get someone.  They are already known
         to be facing consistently if they are all there. */
      int other_side = ((real_index / 3) ^ 1) * 3;
      int other_people = real_people->people[other_side].id1 |
                           real_people->people[other_side+1].id1 |
                           real_people->people[other_side+2].id1;

      /* At least one of those people must exist. */
      if (!other_people)
         fail("Can't tell how to do this -- no live people.");

      /* See if they face the same way as myself. */
      if (((other_people ^ real_people->people[real_index].id1) & 2))
         return(TRUE);    /* This is a 2FL. */

      return(FALSE);      /* This is a 1FL. */
   }
}

/* Test for 4X4 wheel_and_deal to be done 2FL-style, or beau side of 1FL. */
/* ARGSUSED */
Private long_boolean x18_wheel_and_deal(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   /* We assume people have already been checked for coupleness. */

   if (northified_index <= 3 || northified_index >= 12)
      /* We are in the beau-side quad -- it's OK. */
      return(TRUE);
   else {
      /* We are in the belle-side quad -- find the four people in the other quad.
         Just "or" them to be sure we get someone.  They are already known
         to be facing consistently if they are all there. */
      int other_side = (real_index & 014) ^ 4;
      int other_people = real_people->people[other_side].id1 |
                           real_people->people[other_side+1].id1 |
                           real_people->people[other_side+2].id1 |
                           real_people->people[other_side+3].id1;

      /* At least one of those people must exist. */
      if (!other_people)
         fail("Can't tell how to do this -- no live people.");

      /* See if they face the same way as myself. */
      if (((other_people ^ real_people->people[real_index].id1) & 2))
         return(TRUE);    /* This is a 2FL. */

      return(FALSE);      /* This is a 1FL. */
   }
}

/* First test for how to do cycle and wheel.  This always passes the extreme beau,
   and always fails the extreme belle (causing the belle to go on to the next test.)
   For centers, it demands an adjacent end (otherwise we wouldn't know whether
   we were cycling or wheeling) and then returns true if that end is an extreme beau. */
/* ARGSUSED */
Private long_boolean cycle_and_wheel_1(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (northified_index == 0)
      return TRUE;
   else if (northified_index == 2)
      return FALSE;
   else {     /* We are a center.  Find our adjacent end. */
      int other_person;

      /* We think it is rather silly to use an "assume" concept to specify
         a symmetric kind of line, and then give a call that doesn't do
         anything interesting with any of the lines that the assumption can
         make, but we have to do this. */

      switch (real_people->cmd.cmd_assume.assumption) {
         case cr_1fl_only:  case cr_2fl_only:   return (northified_index == 1);
         case cr_wave_only: case cr_magic_only: return (northified_index == 3);
      }

      other_person = real_people->people[real_index ^ 1].id1;

      if (!other_person)
         fail("Can't tell how to do this -- no live people.");

      /* See if he is an extreme beau. */
      if (((other_person ^ real_index) & 2))
         return FALSE;

      return TRUE;
   }
}

/* Second test for how to do cycle and wheel.  This finds the far end, and checks
   whether he faces the same way as myself. */
/* ARGSUSED */
Private long_boolean cycle_and_wheel_2(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int other_person;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_1fl_only: case cr_2fl_only: return TRUE;
      case cr_wave_only:  return (northified_index != 2);
      case cr_magic_only: return (northified_index == 2);
   }

   other_person = real_people->people[(real_index ^ 2) & (~1)].id1;

   /* Here we default to the "non-colliding" version of the call if the
      opposite end of our line doesn't exist. */
   if (!other_person)
      return (northified_index == 1);

   /* See if he faces the same way as myself. */
   if (((other_person ^ real_people->people[real_index].id1) & 2))
      return FALSE;

   return TRUE;
}

Private long_boolean vert1(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!(northified_index & 1))
      return TRUE;
   else if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return FALSE;
   else if (real_people->cmd.cmd_assume.assumption == cr_couples_only ||
            real_people->cmd.cmd_assume.assumption == cr_li_lo)
      return TRUE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ (((real_direction << 1) & 2) | 1)].id1;

      return(((this_person ^ other_person) & DIR_MASK) == 0);
   }
}

Private long_boolean vert2(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (!(northified_index & 1))
      return FALSE;
   else if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return TRUE;
   else if (real_people->cmd.cmd_assume.assumption == cr_couples_only ||
            real_people->cmd.cmd_assume.assumption == cr_li_lo)
      return FALSE;
   else {
      int this_person = real_people->people[real_index].id1;
      int other_person = real_people->people[real_index ^ (((real_direction << 1) & 2) | 1)].id1;

      return(((this_person ^ other_person) & DIR_MASK) == 2);
   }
}

/* ARGSUSED */
Private long_boolean inner_active_lines(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if ((real_index+3) & 2)
      return(northified_index >= 4);     /* I am an end */
   else if (real_people->people[real_index ^ 1].id1)
      return(                            /* I am a center, with a live partner */
         (012 - ((real_index & 4) >> 1)) == (real_people->people[real_index ^ 1].id1 & 017));
   else if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return(northified_index < 4);
   else if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return(northified_index >= 4);
   else
      return FALSE;
}

/* ARGSUSED */
Private long_boolean outer_active_lines(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if ((real_index+3) & 2)
      return(northified_index < 4);      /* I am an end */
   else if (real_people->people[real_index ^ 1].id1)
      return(                            /* I am a center, with a live partner */
         (010 + ((real_index & 4) >> 1)) == (real_people->people[real_index ^ 1].id1 & 017));
   else if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return(northified_index >= 4);
   else if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return(northified_index < 4);
   else
      return FALSE;
}

/* ARGSUSED */
Private long_boolean judge_is_right(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: case cr_2fl_only: return FALSE;  /* This is an error -- socker/judge can't be unambiguous. */
      case cr_all_facing_same: case cr_1fl_only: case cr_li_lo: return TRUE;
      case cr_magic_only: return (real_index & 1) == 0;
   }

   return(
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) == 0)       /* judge exists to my right */
         &&
      (((real_people->people[f].id1 ^ this_person) & 013) != 2));         /* we do not have another judge to my left */
}

/* ARGSUSED */
Private long_boolean judge_is_left(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: case cr_2fl_only: return FALSE;  /* This is an error -- socker/judge can't be unambiguous. */
      case cr_all_facing_same: case cr_1fl_only: case cr_li_lo: return FALSE;
      case cr_magic_only: return (real_index & 1) != 0;
   }

   return(
      (((real_people->people[f].id1 ^ this_person) & 013) == 2)           /* judge exists to my left */
         &&
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) != 0));     /* we do not have another judge to my right */
}

/* ARGSUSED */
Private long_boolean socker_is_right(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: case cr_2fl_only: return FALSE;  /* This is an error -- socker/judge can't be unambiguous. */
      case cr_all_facing_same: case cr_1fl_only: case cr_li_lo: return FALSE;
      case cr_magic_only: return (real_index & 1) != 0;
   }

   return(
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) == 2)       /* socker exists to my right */
         &&
      (((real_people->people[f].id1 ^ this_person) & 013) != 0));         /* we do not have another socker to my left */
}

/* ARGSUSED */
Private long_boolean socker_is_left(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_wave_only: case cr_2fl_only: return FALSE;  /* This is an error -- socker/judge can't be unambiguous. */
      case cr_all_facing_same: case cr_1fl_only: case cr_li_lo: return TRUE;
      case cr_magic_only: return (real_index & 1) == 0;
   }

   return(
      (((real_people->people[f].id1 ^ this_person) & 013) == 0)           /* socker exists to my left */
         &&
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) != 2));     /* we do not have another socker to my right */
}

Private long_boolean judge_is_right_1x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   return(
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) == 0)       /* judge exists to my right */
         &&
      (((real_people->people[f].id1 ^ this_person) & 013) != 2));         /* we do not have another judge to my left */
}

Private long_boolean judge_is_left_1x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   return(
      (((real_people->people[f].id1 ^ this_person) & 013) == 2)           /* judge exists to my left */
         &&
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) != 0));     /* we do not have another judge to my right */
}

Private long_boolean socker_is_right_1x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   return(
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) == 2)       /* socker exists to my right */
         &&
      (((real_people->people[f].id1 ^ this_person) & 013) != 0));         /* we do not have another socker to my left */
}

Private long_boolean socker_is_left_1x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int f = this_person & 2;

   return(
      (((real_people->people[f].id1 ^ this_person) & 013) == 0)           /* socker exists to my left */
         &&
      (((real_people->people[f ^ 2].id1 ^ this_person) & 013) != 2));     /* we do not have another socker to my right */
}

/* Helper function for in/out roll circulate stuff.  "Yes_roll_direction" is the facing direction
   that constitutes what we are looking for (inroller or outroller as the case may be). */
Private long_boolean in_out_roll_select(int yes_roll_direction, int cw_end, int ccw_end, char *errmsg)
{
   int no_roll_direction = 022 - yes_roll_direction;

   if (  /* cw_end exists and is proper, and we do not have ccw_end proper also */
         (cw_end == yes_roll_direction && ccw_end != yes_roll_direction) ||
         /* or if ccw_end exists and is improper, and cw_end is a phantom */
         (ccw_end == no_roll_direction && cw_end == 0))
      return(TRUE);
   else if (
         /* ccw_end exists and is proper, and we do not have cw_end proper also */
         (ccw_end == yes_roll_direction && cw_end != yes_roll_direction) ||
         /* or if cw_end exists and is improper, and ccw_end is a phantom */
         (cw_end == no_roll_direction && ccw_end == 0))
      return(FALSE);
   else {
      fail(errmsg);
      /* NOTREACHED */
   }
}

static int magic_cw_idx[8] =      {3,  7,  7,  3,  7,  3,  3,  7};
static int magic_ccw_idx[8] =     {0,  4,  4,  0,  4,  0,  0,  4};
static int magic_cw_idx_2x3[8] =  {2,  5,  2,  5,  2,  5};
static int magic_ccw_idx_2x3[8] = {0,  3,  0,  3,  0,  3};
static int cw_idx_2x3[6] =        {2,  2,  2,  5,  5,  5};
static int ccw_idx_2x3[6] =       {0,  0,  0,  3,  3,  3};
static int cw_idx_2x6[12] =       {5,  5,  5,  5,  5,  5, 11, 11, 11, 11, 11, 11};
static int ccw_idx_2x6[12] =      {0,  0,  0,  0,  0,  0,  6,  6,  6,  6,  6,  6};
static int cw_idx_2x8[16] =       {7,  7,  7,  7,  7,  7,  7,  7, 15, 15, 15, 15, 15, 15, 15, 15};
static int ccw_idx_2x8[16] =      {0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8};

static int magic_inroll_directions[8] =     {012, 010, 010, 012, 010, 012, 012, 010};
static int inroll_directions_2x3[6] =       {012, 012, 012, 010, 010, 010};
static int inroll_directions_2x6[12] =      {012, 012, 012, 012, 012, 012, 010, 010, 010, 010, 010, 010};
static int inroll_directions_2x8[16] =      {012, 012, 012, 012, 012, 012, 012, 012, 010, 010, 010, 010, 010, 010, 010, 010};
static int magic_inroll_directions_2x3[6] = {012, 010, 012, 010, 012, 010};


/* ARGSUSED */
Private long_boolean inroller_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) == 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) == 0;

   return in_out_roll_select(
      012 - ((real_index & 4) >> 1),
      real_people->people[real_index | 3].id1 & 017,
      real_people->people[real_index & 4].id1 & 017,
      "Can't find end looking in.");
}

/* ARGSUSED */
Private long_boolean magic_inroller_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) == 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) == 0;

   return in_out_roll_select(
      magic_inroll_directions[real_index],
      real_people->people[magic_cw_idx[real_index]].id1 & 017,
      real_people->people[magic_ccw_idx[real_index]].id1 & 017,
      "Can't find magic end looking in.");
}

/* ARGSUSED */
Private long_boolean outroller_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) != 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) != 0;

   return in_out_roll_select(
      010 + ((real_index & 4) >> 1),
      real_people->people[real_index | 3].id1 & 017,
      real_people->people[real_index & 4].id1 & 017,
      "Can't find end looking out.");
}

/* ARGSUSED */
Private long_boolean magic_outroller_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) != 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) != 0;

   return in_out_roll_select(
      022 - magic_inroll_directions[real_index],
      real_people->people[magic_cw_idx[real_index]].id1 & 017,
      real_people->people[magic_ccw_idx[real_index]].id1 & 017,
      "Can't find magic end looking out.");
}

/* ARGSUSED */
Private long_boolean inroller_is_cw_2x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      fail("Not legal.");

   return in_out_roll_select(
      inroll_directions_2x3[real_index],
      real_people->people[cw_idx_2x3[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x3[real_index]].id1 & 017,
      "Can't find end looking in.");
}

/* ARGSUSED */
Private long_boolean magic_inroller_is_cw_2x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      fail("Not legal.");

   return in_out_roll_select(
      magic_inroll_directions_2x3[real_index],
      real_people->people[magic_cw_idx_2x3[real_index]].id1 & 017,
      real_people->people[magic_ccw_idx_2x3[real_index]].id1 & 017,
      "Can't find magic end looking in.");
}

/* ARGSUSED */
Private long_boolean outroller_is_cw_2x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      fail("Not legal.");

   return in_out_roll_select(
      022 - inroll_directions_2x3[real_index],
      real_people->people[cw_idx_2x3[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x3[real_index]].id1 & 017,
      "Can't find end looking out.");
}

/* ARGSUSED */
Private long_boolean magic_outroller_is_cw_2x3(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      fail("Not legal.");

   return in_out_roll_select(
      022 - magic_inroll_directions_2x3[real_index],
      real_people->people[magic_cw_idx_2x3[real_index]].id1 & 017,
      real_people->people[magic_ccw_idx_2x3[real_index]].id1 & 017,
      "Can't find magic end looking out.");
}

/* ARGSUSED */
Private long_boolean inroller_is_cw_2x6(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index / 6)) & 1) == 0;

   return in_out_roll_select(
      inroll_directions_2x6[real_index],
      real_people->people[cw_idx_2x6[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x6[real_index]].id1 & 017,
      "Can't find end looking in.");
}

/* ARGSUSED */
Private long_boolean outroller_is_cw_2x6(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index / 6)) & 1) != 0;

   return in_out_roll_select(
      022 - inroll_directions_2x6[real_index],
      real_people->people[cw_idx_2x6[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x6[real_index]].id1 & 017,
      "Can't find end looking out.");
}

/* ARGSUSED */
Private long_boolean inroller_is_cw_2x8(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 3)) & 1) == 0;

   return in_out_roll_select(
      inroll_directions_2x8[real_index],
      real_people->people[cw_idx_2x8[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x8[real_index]].id1 & 017,
      "Can't find end looking in.");
}

/* ARGSUSED */
Private long_boolean outroller_is_cw_2x8(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 3)) & 1) != 0;

   return in_out_roll_select(
      022 - inroll_directions_2x8[real_index],
      real_people->people[cw_idx_2x8[real_index]].id1 & 017,
      real_people->people[ccw_idx_2x8[real_index]].id1 & 017,
      "Can't find end looking out.");
}

/* ARGSUSED */
Private long_boolean outposter_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int outroll_direction;
   uint32 cw_dir;

   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) != 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) != 0;

   outroll_direction = 010 + ((real_index & 4) >> 1);
   cw_dir = real_people->people[real_index | 3].id1 & 017;

   /* cw_end exists and is looking out */
   if (cw_dir == outroll_direction) return TRUE;
   /* or cw_end is phantom but ccw_end is looking in, so it has to be the phantom */
   else if (cw_dir == 0 && (real_people->people[real_index & 4].id1 & 017) == (022 - outroll_direction)) return TRUE;
   /* we don't know */
   else return FALSE;
}

/* ARGSUSED */
Private long_boolean outposter_is_ccw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int inroll_direction;

   if (real_people->cmd.cmd_assume.assumption == cr_wave_only)
      return ((northified_index ^ (northified_index >> 2)) & 1) == 0;
   if (real_people->cmd.cmd_assume.assumption == cr_2fl_only)
      return ((northified_index ^ (northified_index >> 1)) & 2) == 0;

   inroll_direction = 012 - ((real_index & 4) >> 1);

   /* Demand that cw_end be looking in -- otherwise "outposter_is_cw"
      would have accepted it if it were legal. */
   if ((real_people->people[real_index | 3].id1 & 017) != inroll_direction) return FALSE;
   else {
      /* Now if ccw_end is looking out or is a phantom, it's OK. */
      uint32 ccw_dir = real_people->people[real_index & 4].id1 & 017;
      if (ccw_dir == 0 || ccw_dir == 022-inroll_direction) return TRUE;
      return FALSE;
   }
}


/* -3 means error, -2 means return FALSE, -1 does not occur, and >= 0 means test that person. */

/* ARGSUSED */
Private long_boolean check_tbone(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   short z = extra_stuff[(real_index<<2) + real_direction];

   if (z == -2)
      return FALSE;
   else if (z >= 0) {
      uint32 zz = real_people->people[z].id1;
      if (zz & BIT_PERSON)
         return (zz ^ real_people->people[real_index].id1) & 1;
   }
   fail("Can't determine where to go or which way to face.");
   /* NOTREACHED */
}

static short trnglspot_tboned_tab[12] = {-3, 2, -3, 1,     -2, -2, 0, 0,    -2, 0, 0, -2};
static short six2spot_tboned_tab[24]  = {-2, -2, -2, -2,    -3, 2, -3, 0,    -2, -2, -2, -2,    -2, -2, -2, -2,    -3, 3, -3, 5,    -2, -2, -2, -2};
static short mag62spot_tboned_tab[24] = {-3, -2, -3, -2,    -3, 3, -3, 5,     -3, -2, -3, -2,     -3, -2, -3, -2,     -3, 2, -3, 0,     -3, -2, -3, -2};

/* ARGSUSED */
Private long_boolean nextinttrnglspot_is_tboned(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   static short bb[24] = {-2, -2, 4, 4,     -3, 3, -3, 5,     -2, 4, 4, -2,     1, 1, -2, -2,     -3, 2, -3, 0,     1, -2, -2, 1};
   static short cc[24] = {2, 2, -2, -2,      5, -2, -2, 5,    0, -3, 4, -3,     -2, -2, 5, 5,     -2, 2, 2, -2,     1, -3, 3, -3};
   return check_tbone(real_people, real_index, real_direction, northified_index,
         (real_people->kind == s_short6) ? bb : cc);
}

/* ARGSUSED */
Private long_boolean next_galaxyspot_is_tboned(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   static short aa[32] = {1, -3, 7, -3,     2, 2, 0, 0,     -3, 3, -3, 1,     2, 4, 4, 2,    3, -3, 5, -3,     4, 4, 6, 6,    -3, 5, -3, 7,      0, 6, 6, 0};

   /* We always return true for centers.  That way
      the centers can reverse flip a galaxy even if the
      next point does not exist.  Maybe this isn't a good way
      to do it, and we need another predicate.  Sigh. */

   if (real_index & 1) return TRUE;

   return check_tbone(real_people, real_index, real_direction, northified_index, aa);
}

/* ARGSUSED */
Private long_boolean column_double_down(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return(
      (northified_index < 3)              /* unless #1 in column, it's easy */
         ||
      (northified_index > 4)
         ||
      /* if #1, my adjacent end must exist and face in */
      (((((real_index + 2) & 4) >> 1) + 1) == (real_people->people[real_index ^ 7].id1 & 017)));
}

/* ARGSUSED */
Private long_boolean boyp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return((real_people->people[real_index].id1 & ID1_PERM_BOY) != 0);
}

/* ARGSUSED */
Private long_boolean girlp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return((real_people->people[real_index].id1 & ID1_PERM_GIRL) != 0);
}

/* ARGSUSED */
Private long_boolean roll_is_cw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return((real_people->people[real_index].id1 & ROLLBITR) != 0);
}

/* ARGSUSED */
Private long_boolean roll_is_ccw(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return((real_people->people[real_index].id1 & ROLLBITL) != 0);
}

/* ARGSUSED */
Private long_boolean x12_boy_facing_girl(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 1].id1;
   return((this_person & ID1_PERM_BOY) && (other_person & ID1_PERM_GIRL));
}

/* ARGSUSED */
Private long_boolean x12_girl_facing_boy(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ 1].id1;
   return((this_person & ID1_PERM_GIRL) && (other_person & ID1_PERM_BOY));
}

/* ARGSUSED */
Private long_boolean x22_boy_facing_girl(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int this_person = real_people->people[real_index].id1;
   int other_person = real_people->people[real_index ^ (((real_direction << 1) & 2) ^ 3)].id1;
   return((this_person & ID1_PERM_BOY) && (other_person & ID1_PERM_GIRL));
}

/* ARGSUSED */
Private long_boolean x22_girl_facing_boy(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   int other_person = real_people->people[real_index ^ (((real_direction << 1) & 2) ^ 3)].id1;
   int this_person = real_people->people[real_index].id1;
   return((this_person & ID1_PERM_GIRL) && (other_person & ID1_PERM_BOY));
}

/* ARGSUSED */
Private long_boolean leftp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_left;
}

/* ARGSUSED */
Private long_boolean rightp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_right;
}

/* ARGSUSED */
Private long_boolean inp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_in;
}

/* ARGSUSED */
Private long_boolean outp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_out;
}


/* ARGSUSED */
Private long_boolean zigzagp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_zigzag;
}

/* ARGSUSED */
Private long_boolean zagzigp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_zagzig;
}

/* ARGSUSED */
Private long_boolean zigzigp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_zigzig;
}

/* ARGSUSED */
Private long_boolean zagzagp(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_zagzag;
}

/* ARGSUSED */
Private long_boolean no_dir_p(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   return current_direction == direction_no_direction;
}





Private long_boolean check_handedness(short *p1, short *p2, personrec ppp[])
{
   uint32 z = 0;
   long_boolean b1 = TRUE;
   long_boolean b2 = TRUE;
   short d1 = *(p1++);
   short d2 = *(p2++);

   while (*p1>=0) {
      uint32 t1 = ppp[*(p1++)].id1;
      uint32 t2 = ppp[*(p2++)].id1;
      z |= t1 | t2;
      if (t1 && (t1 & d_mask)!=d1) b1 = FALSE;
      if (t2 && (t2 & d_mask)!=d2) b1 = FALSE;
      if (t1 && (t1 & d_mask)!=d2) b2 = FALSE;
      if (t2 && (t2 & d_mask)!=d1) b2 = FALSE;
   }

   if (z) {
      if (b1) return TRUE;
      else if (b2) return FALSE;
   }

   fail("Can't determine handedness.");
   /* NOTREACHED */
}

static short spindle1[] = {d_east, 0, 1, 2, -1};
static short spindle2[] = {d_west, 6, 5, 4, -1};

static short short1[] = {d_north, 0, 5, -1};
static short short2[] = {d_south, 2, 3, -1};

static short dmd1[] = {d_east, 1, -1};
static short dmd2[] = {d_west, 3, -1};



/* ARGSUSED */
Private long_boolean dmd_ctrs_rh(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->kind == s_spindle)
      return check_handedness(spindle1, spindle2, real_people->people);
   else if (real_people->kind == s_short6)
      return check_handedness(short1, short2, real_people->people);
   else    /* Required to be diamond or single general 1/4 tag. */
      return check_handedness(dmd1, dmd2, real_people->people);
}

/* ARGSUSED */
Private long_boolean dmd_ctrs_lh(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if (real_people->kind == s_spindle)
      return !check_handedness(spindle1, spindle2, real_people->people);
   else if (real_people->kind == s_short6)
      return !check_handedness(short1, short2, real_people->people);
   else    /* Required to be diamond or single general 1/4 tag. */
      return !check_handedness(dmd1, dmd2, real_people->people);
}

/* ARGSUSED */
Private long_boolean trngl_pt_rh(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   if ((real_people->people[0].id1 & d_mask)==d_west)
      return TRUE;
   else if ((real_people->people[0].id1 & d_mask)==d_east)
      return FALSE;

   fail("Can't determine handedness of triangle point.");
   /* NOTREACHED */
}

typedef struct {
   short ctr_action;    /* -1 for TRUE, -2 for FALSE, else required direction xor. */
   short end_action;
   short bbbbb;
} simple_qtag_action;

typedef struct {
   simple_qtag_action if_jleft;
   simple_qtag_action if_jright;
   simple_qtag_action if_ijleft;
   simple_qtag_action if_ijright;
   simple_qtag_action none;
} full_qtag_action;

static full_qtag_action q_tag_front_action = {
   {-1,   1,  0},            /* if_jleft    */
   {-1,   0,  1},            /* if_jright   */
   {-2,  -1, 99},            /* if_ijleft   */
   {-2,  -1, 99},            /* if_ijright  */
   { 2, 010,  2}};           /* none        */

static full_qtag_action q_tag_back_action = {
   {-1,   0,  1},            /* if_jleft    */
   {-1,   1,  0},            /* if_jright   */
   {-2,  -1, 99},            /* if_ijleft   */
   {-2,  -1, 99},            /* if_ijright  */
   { 2, 012,  2}};           /* none        */

static full_qtag_action q_line_front_action = {
   {-2,  -1, 99},            /* if_jleft    */
   {-2,  -1, 99},            /* if_jright   */
   {-1,   1,  0},            /* if_ijleft   */
   {-1,   0,  1},            /* if_ijright  */
   { 0, 010,  0}};           /* none        */

static full_qtag_action q_line_back_action = {
   {-2,  -1, 99},            /* if_jleft    */
   {-2,  -1, 99},            /* if_jright   */
   {-1,   0,  1},            /* if_ijleft   */
   {-1,   1,  0},            /* if_ijright  */
   { 0, 012,  0}};           /* none        */

/* ARGSUSED */
Private long_boolean q_tag_check(setup *real_people, int real_index,
   int real_direction, int northified_index, short *extra_stuff)
{
   full_qtag_action *bigactionp = (full_qtag_action *) extra_stuff;
   simple_qtag_action *actionp;

   switch (real_people->cmd.cmd_assume.assumption) {
      case cr_jleft:     actionp = (simple_qtag_action *) &(bigactionp->if_jleft);   break;
      case cr_jright:    actionp = (simple_qtag_action *) &(bigactionp->if_jright);  break;
      case cr_ijleft:    actionp = (simple_qtag_action *) &(bigactionp->if_ijleft);  break;
      case cr_ijright:   actionp = (simple_qtag_action *) &(bigactionp->if_ijright); break;
      default:           actionp = (simple_qtag_action *) &(bigactionp->none);       break;
   }

   if (real_index & 2) {
      /* I am in the center line. */
      if (actionp->ctr_action == -1) return TRUE;
      else if (actionp->ctr_action == -2) return FALSE;
      else return ((real_people->people[real_index].id1 ^ real_people->people[real_index ^ 1].id1) & DIR_MASK) == actionp->ctr_action;
   }
   else {
      /* I am on the outside; find the end of the center line nearest me. */

      if (actionp->end_action == -1) return FALSE;
      else if (actionp->end_action <= 1) {
         if (real_people->cmd.cmd_assume.assump_col == 4) {
            if (actionp->end_action == 0)
               return ((((real_index+3) >> 1) ^ real_people->people[real_index].id1) & 2) == 0;
            else if (actionp->end_action == 1)
               return ((((real_index+3) >> 1) ^ real_people->people[real_index].id1) & 2) != 0;
         }
         else {
            if (actionp->bbbbb == 0)
               return ((real_index ^ real_people->cmd.cmd_assume.assump_both) & 1) == 0;
            else
               return ((real_index ^ real_people->cmd.cmd_assume.assump_both) & 1) != 0;
         }
      }
      else {
         int z;
         if (real_index & 1) z = real_index ^ 3; else z = real_index ^ 6;
         return(((real_people->people[z].id1 & 017) == (actionp->end_action ^ (real_index >> 1))) &&
               (((real_people->people[z].id1 ^ real_people->people[z ^ 1].id1) & DIR_MASK) == actionp->bbbbb));
      }
   }
}


/* BEWARE!!  This list must track the array "predtab" in dbcomp.c . */

/* The first 12 of these (the constant to use is SELECTOR_PREDS) take a predicate.
   Any call that uses one of these predicates in its definition will cause a
   popup to appear asking "who?". */

predicate_descriptor pred_table[] = {
      {selected,                        (short *) 0},  /* "select" */
      {unselect,                        (short *) 0},  /* "unselect" */
      {select_near_select,              (short *) 0},  /* "select_near_select" */
      {select_near_unselect,            (short *) 0},  /* "select_near_unselect" */
      {unselect_near_select,            (short *) 0},  /* "unselect_near_select" */
      {unselect_near_unselect,          (short *) 0},  /* "unselect_near_unselect" */
      {select_once_rem_from_select,     (short *) 0},  /* "select_once_rem_from_select" */
      {conc_from_select,                (short *) 0},  /* "conc_from_select" */
      {select_once_rem_from_unselect,   (short *) 0},  /* "select_once_rem_from_unselect" */
      {unselect_once_rem_from_select,   (short *) 0},  /* "unselect_once_rem_from_select" */
      {select_and_roll_is_cw,           (short *) 0},  /* "select_and_roll_is_cw" */
      {select_and_roll_is_ccw,          (short *) 0},  /* "select_and_roll_is_ccw" */
      {always,                          (short *) 0},  /* "always" */
      {x22_miniwave,                    (short *) 0},  /* "x22_miniwave" */
      {x22_couple,                      (short *) 0},  /* "x22_couple" */
      {x22_facing_someone,              (short *) 0},  /* "x22_facing_someone" */
      {x22_tandem_with_someone,         (short *) 0},  /* "x22_tandem_with_someone" */
      {cols_someone_in_front,           (short *) 0},  /* "columns_someone_in_front" */
      {x14_once_rem_miniwave,           (short *) 0},  /* "x14_once_rem_miniwave" */
      {x14_once_rem_couple,             (short *) 0},  /* "x14_once_rem_couple" */
      {lines_miniwave,                  (short *) 0},  /* "lines_miniwave" */
      {lines_couple,                    (short *) 0},  /* "lines_couple" */
      {cast_normal,                     (short *) 0},  /* "cast_normal" */
      {cast_pushy,                      (short *) 0},  /* "cast_pushy" */
      {cast_normal_or_warn,             (short *) 0},  /* "cast_normal_or_warn" */
      {opp_in_magic,                    (short *) 0},  /* "lines_magic_miniwave" */
      {same_in_magic,                   (short *) 0},  /* "lines_magic_couple" */
      {lines_once_rem_miniwave,         (short *) 0},  /* "lines_once_rem_miniwave" */
      {lines_once_rem_couple,           (short *) 0},  /* "lines_once_rem_couple" */
      {same_in_pair,                    (short *) 0},  /* "lines_tandem" */
      {opp_in_pair,                     (short *) 0},  /* "lines_antitandem" */
      {columns_tandem,                  (short *) 0},  /* "columns_tandem" */
      {columns_antitandem,              (short *) 0},  /* "columns_antitandem" */
      {same_in_magic,                   (short *) 0},  /* "columns_magic_tandem" */
      {opp_in_magic,                    (short *) 0},  /* "columns_magic_antitandem" */
      {lines_once_rem_couple,           (short *) 0},  /* "columns_once_rem_tandem" */
      {lines_once_rem_miniwave,         (short *) 0},  /* "columns_once_rem_antitandem" */
      {same_in_pair,                    (short *) 0},  /* "columns_couple" */
      {opp_in_pair,                     (short *) 0},  /* "columns_miniwave" */
      {x12_beau_or_miniwave,            (short *) 0},  /* "1x2_beau_or_miniwave" */
      {x14_wheel_and_deal,              (short *) 0},  /* "1x4_wheel_and_deal" */
      {x16_wheel_and_deal,              (short *) 0},  /* "1x6_wheel_and_deal" */
      {x18_wheel_and_deal,              (short *) 0},  /* "1x8_wheel_and_deal" */
      {cycle_and_wheel_1,               (short *) 0},  /* "cycle_and_wheel_1" */
      {cycle_and_wheel_2,               (short *) 0},  /* "cycle_and_wheel_2" */
      {vert1,                           (short *) 0},  /* "vert1" */
      {vert2,                           (short *) 0},  /* "vert2" */
      {inner_active_lines,              (short *) 0},  /* "inner_active_lines" */
      {outer_active_lines,              (short *) 0},  /* "outer_active_lines" */
      {judge_is_right,                  (short *) 0},  /* "judge_is_right" */
      {judge_is_left,                   (short *) 0},  /* "judge_is_left" */
      {socker_is_right,                 (short *) 0},  /* "socker_is_right" */
      {socker_is_left,                  (short *) 0},  /* "socker_is_left" */
      {judge_is_right_1x3,              (short *) 0},  /* "judge_is_right_1x3" */
      {judge_is_left_1x3,               (short *) 0},  /* "judge_is_left_1x3" */
      {socker_is_right_1x3,             (short *) 0},  /* "socker_is_right_1x3" */
      {socker_is_left_1x3,              (short *) 0},  /* "socker_is_left_1x3" */
      {inroller_is_cw,                  (short *) 0},  /* "inroller_is_cw" */
      {magic_inroller_is_cw,            (short *) 0},  /* "magic_inroller_is_cw" */
      {outroller_is_cw,                 (short *) 0},  /* "outroller_is_cw" */
      {magic_outroller_is_cw,           (short *) 0},  /* "magic_outroller_is_cw" */
      {inroller_is_cw_2x3,              (short *) 0},  /* "inroller_is_cw_2x3" */
      {magic_inroller_is_cw_2x3,        (short *) 0},  /* "magic_inroller_is_cw_2x3" */
      {outroller_is_cw_2x3,             (short *) 0},  /* "outroller_is_cw_2x3" */
      {magic_outroller_is_cw_2x3,       (short *) 0},  /* "magic_outroller_is_cw_2x3" */
      {inroller_is_cw_2x6,              (short *) 0},  /* "inroller_is_cw_2x6" */
      {outroller_is_cw_2x6,             (short *) 0},  /* "outroller_is_cw_2x6" */
      {inroller_is_cw_2x8,              (short *) 0},  /* "inroller_is_cw_2x8" */
      {outroller_is_cw_2x8,             (short *) 0},  /* "outroller_is_cw_2x8" */
      {outposter_is_cw,                 (short *) 0},  /* "outposter_is_cw" */
      {outposter_is_ccw,                (short *) 0},  /* "outposter_is_ccw" */
      {check_tbone,            trnglspot_tboned_tab},  /* "nexttrnglspot_is_tboned" */
      {nextinttrnglspot_is_tboned,      (short *) 0},  /* "nextinttrnglspot_is_tboned" */
      {check_tbone,             six2spot_tboned_tab},  /* "next62spot_is_tboned" */
      {check_tbone,            mag62spot_tboned_tab},  /* "next_magic62spot_is_tboned" */
      {next_galaxyspot_is_tboned,       (short *) 0},  /* "next_galaxyspot_is_tboned" */
      {column_double_down,              (short *) 0},  /* "column_double_down" */
      {boyp,                            (short *) 0},  /* "boyp" */
      {girlp,                           (short *) 0},  /* "girlp" */
      {roll_is_cw,                      (short *) 0},  /* "roll_is_cw" */
      {roll_is_ccw,                     (short *) 0},  /* "roll_is_ccw" */
      {x12_boy_facing_girl,             (short *) 0},  /* "x12_boy_facing_girl" */
      {x12_girl_facing_boy,             (short *) 0},  /* "x12_girl_facing_boy" */
      {x22_boy_facing_girl,             (short *) 0},  /* "x22_boy_facing_girl" */
      {x22_girl_facing_boy,             (short *) 0},  /* "x22_girl_facing_boy" */
      {leftp,                           (short *) 0},  /* "leftp" */
      {rightp,                          (short *) 0},  /* "rightp" */
      {inp,                             (short *) 0},  /* "inp" */
      {outp,                            (short *) 0},  /* "outp" */
      {zigzagp,                         (short *) 0},  /* "zigzagp" */
      {zagzigp,                         (short *) 0},  /* "zagzigp" */
      {zigzigp,                         (short *) 0},  /* "zigzigp" */
      {zagzagp,                         (short *) 0},  /* "zagzagp" */
      {no_dir_p,                        (short *) 0},  /* "no_dir_p" */
      {dmd_ctrs_rh,                     (short *) 0},  /* "dmd_ctrs_rh" */
      {dmd_ctrs_lh,                     (short *) 0},  /* "dmd_ctrs_lh" */
      {trngl_pt_rh,                     (short *) 0},  /* "trngl_pt_rh" */
      {q_tag_check,   (short *) &q_tag_front_action},  /* "q_tag_front" */
      {q_tag_check,    (short *) &q_tag_back_action},  /* "q_tag_back" */
      {q_tag_check,  (short *) &q_line_front_action},  /* "q_line_front" */
      {q_tag_check,   (short *) &q_line_back_action}}; /* "q_line_back" */
