/* SD -- square dance caller's helper.

    Copyright (C) 1990-1999  William B. Ackerman.

    This file is unpublished and contains trade secrets.  It is
    to be used by permission only and not to be disclosed to third
    parties without the express permission of the copyright holders.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This is for version 32. */

#include <stdio.h>
#include <setjmp.h>
#include "database.h"
#include "sdui.h"


#define MAX_PEOPLE 24
/* Actually, we don't make a resolve bigger than 3.  This is how much space
   we allocate for things.  Just being careful. */
#define MAX_RESOLVE_SIZE 5

/* Probability (out of 8) that a concept will be placed on a randomly generated call. */
#define CONCEPT_PROBABILITY 2
/* We use lots more concepts for "standardize", since it is much less likely (though
   by no means impossible) that a plain call will do the job. */
#define STANDARDIZE_CONCEPT_PROBABILITY 6

/* This defines a person in a setup.  Unfortunately, there is too much data to fit into 32 bits. */
typedef struct {
   uint32 id1;       /* Frequently used bits go here. */
   uint32 id2;       /* Bits used for evaluating predicates. */
} personrec;

/* Person bits for "id1" field are:
     0x80000000 -
     0x40000000 - not side girl    **** these 10 bits are "permanent" -- they never change in a person
     0x20000000 - not side boy
     0x10000000 - not head girl
     0x08000000 - not head boy
     0x04000000 - head corner
     0x02000000 - side corner
     0x01000000 - head
     0x00800000 - side
     0x00400000 - boy
     0x00200000 - girl             **** end of permanent bits
      4 000 000 - roll direction is CCW
      2 000 000 - roll direction is neutral
      1 000 000 - roll direction is CW
        400 000 - fractional stability enabled
        200 000 - stability "v" field -- 2 bits
        100 000 -     "
         40 000 - stability "r" field -- 3 bits
         20 000 -     "
         10 000 -     "
          4 000 - live person (just so at least one bit is always set)
          2 000 - active phantom (see below, under XPID_MASK)
          1 000 - virtual person (see below, under XPID_MASK)
            700 - these 3 bits identify actual person
             60 - these 2 bits must be clear for rotation
             10 - part of rotation (facing north/south)
              4 - bit must be clear for rotation
              2 - part of rotation
              1 - part of rotation (facing east/west)
*/

#define ID1_PERM_NSG           0x40000000UL
#define ID1_PERM_NSB           0x20000000UL
#define ID1_PERM_NHG           0x10000000UL
#define ID1_PERM_NHB           0x08000000UL
#define ID1_PERM_HCOR          0x04000000UL
#define ID1_PERM_SCOR          0x02000000UL
#define ID1_PERM_HEAD          0x01000000UL
#define ID1_PERM_SIDE          0x00800000UL
#define ID1_PERM_BOY           0x00400000UL
#define ID1_PERM_GIRL          0x00200000UL

#define ID1_PERM_ALLBITS       0x7FE00000UL

/* These are a 3 bit field -- ROLL_BIT tells where its low bit lies. */
#define ROLL_MASK   07000000UL
#define ROLL_BIT    01000000UL
#define ROLLBITL    04000000UL
#define ROLLBITM    02000000UL
#define ROLLBITR    01000000UL

/* These are a 6 bit field. */
#define STABLE_MASK  0770000UL
#define STABLE_ENAB  0400000UL
#define STABLE_VBIT  0100000UL
#define STABLE_RBIT  0010000UL

#define BIT_PERSON   0004000UL
#define BIT_ACT_PHAN 0002000UL
#define BIT_TANDVIRT 0001000UL

/* Person ID.  These bit positions are extremely hard wired into, among other
   things, the resolver and the printer. */
#define PID_MASK  0700UL

/* Extended person ID.  These 5 bits identify who the person is for the purpose
   of most manipulations.  0 to 7 are normal live people (the ones who squared up).
   8 to 15 are virtual people assembled for tandem or couples.  16 to 31 are
   active (but nevertheless identifiable) phantoms.  This means that, when
   BIT_ACT_PHAN is on, it usurps the meaning of BIT_TANDVIRT. */
#define XPID_MASK 03700UL

#define d_mask  04013UL
#define d_north 04010UL
#define d_south 04012UL
#define d_east  04001UL
#define d_west  04003UL


#define ID2_HEADLINE   0x80000000UL
#define ID2_SIDELINE   0x40000000UL
#define ID2_CTR2       0x20000000UL
#define ID2_BELLE      0x10000000UL
#define ID2_BEAU       0x08000000UL
#define ID2_CTR6       0x04000000UL
#define ID2_OUTR2      0x02000000UL
#define ID2_OUTR6      0x01000000UL
#define ID2_TRAILER    0x00800000UL
#define ID2_LEAD       0x00400000UL
#define ID2_CTRDMD     0x00200000UL
#define ID2_NCTRDMD    0x00100000UL
#define ID2_CTR1X4     0x00080000UL
#define ID2_NCTR1X4    0x00040000UL
#define ID2_CTR1X6     0x00020000UL
#define ID2_NCTR1X6    0x00010000UL
#define ID2_OUTR1X3    0x00008000UL
#define ID2_NOUTR1X3   0x00004000UL
#define ID2_FACING     0x00002000UL
#define ID2_NOTFACING  0x00001000UL
#define ID2_CENTER     0x00000800UL
#define ID2_END        0x00000400UL
#define ID2_NEARCOL    0x00000200UL
#define ID2_NEARLINE   0x00000100UL
#define ID2_NEARBOX    0x00000080UL
#define ID2_FARCOL     0x00000040UL
#define ID2_FARLINE    0x00000020UL
#define ID2_FARBOX     0x00000010UL
#define ID2_CTR4       0x00000008UL
#define ID2_OUTRPAIRS  0x00000004UL
#define ID2_FACEFRONT  0x00000002UL
#define ID2_FACEBACK   0x00000001UL

/* These are the bits that get filled in by "update_id_bits". */
#define BITS_TO_CLEAR (ID2_LEAD|ID2_TRAILER|ID2_BEAU|ID2_BELLE| \
ID2_FACING|ID2_NOTFACING|ID2_CENTER|ID2_END|ID2_CTR2|ID2_CTR6|ID2_OUTR2|ID2_OUTR6| \
ID2_CTRDMD|ID2_NCTRDMD|ID2_CTR1X4|ID2_NCTR1X4|ID2_CTR1X6|ID2_NCTR1X6| \
ID2_OUTR1X3|ID2_NOUTR1X3|ID2_CTR4|ID2_OUTRPAIRS)


/* These are the really global position bits.  They get filled in only at the top level. */
#define GLOB_BITS_TO_CLEAR (ID2_NEARCOL|ID2_NEARLINE|ID2_NEARBOX|ID2_FARCOL|ID2_FARLINE|ID2_FARBOX|ID2_FACEFRONT|ID2_FACEBACK|ID2_HEADLINE|ID2_SIDELINE)



typedef struct {
   setup_kind skind;
   int srotation;
} small_setup;

/* Flags that reside in the "cmd_misc_flags" word of a setup BEFORE a call is executed.

   BEWARE!! These flags co-exist in the cmd_misc_flags word with copies of some
   of the call invocation flags.  The mask for those flags is
   DFM1_CONCENTRICITY_FLAG_MASK.  Those flags, and that mask, are defined
   in database.h .  We must define these flags starting where the concentricity
   flags end.

   CMD_MISC__EXPLICIT_MIRROR means that the setup has been mirrored by the "mirror"
   concept, separately from anything done by "left" or "reverse".  Such a mirroring
   does NOT cause the "take right hands" stuff to compensate by going to left hands,
   so it has the effect of making the people physically take left hands.

   CMD_MISC__MATRIX_CONCEPT means that the "matrix" concept has been given, and we have
   to be very careful about what we can do.  Specifically, this will make everything illegal
   except calls to suitable "split phantom" concepts.

   CMD_MISC__VERIFY_WAVES means that, before doing the call, we have to act as though the
   "assume waves" concept had been given.  That is, we have to verify that the setup is in fact
   waves, and we have to set the assumption stuff, so that a "with active phantoms" concept
   can be handled.  This flag is used only through "divided_setup_move", to propagate
   information from a concept like "split phantom waves" through the division, and then
   cause the assumption to be acted upon in each resulting setup.  It is removed immediately
   by "divided_setup_move" after use.

   CMD_MISC__EXPLICIT_MATRIX means that the caller said "4x4 matrix" or "2x6 matrix" or whatever,
   so we got to this matrix explicitly.  This enables natural splitting of the setup, e.g. form
   a parallelogram, "2x6 matrix 1x2 checkmate" is legal -- the 2x6 gets divided naturally
   into 2x3's.

   CMD_MISC__NO_EXPAND_MATRIX means that we are at a level of recursion that no longer permits us to do the
   implicit expansion of the matrix (e.g. add outboard phantoms to turn a 2x4 into a 2x6
   if the concept "triple box" is used) that some concepts perform at the top level.

   CMD_MISC__DISTORTED means that we are at a level of recursion in which some
   distorted-setup concept has been used.  When this is set, "matrix" (a.k.a.
   "space invader") calls, such as press and truck, are not permitted.  Sorry, Clark.

   CMD_MISC__OFFSET_Z means that we are doing a 2x3 call (the only known case
   of this is "Z axle") that came from Z's picked out of a 4x4 setup.  The map
   that we are using has a "50% offset" map schema, so that, if the call goes
   into 1x4's (which Z axle does) it will put the results into a parallelogram,
   in accordance with customary usage.  The problem is that, if the call goes
   into a setup other than a 1x4 oriented in the appropriate way, using the
   50% offset schema will lead to the wrong result.  For example, if a call
   were defined that went from Z's into 2x2's, the program would attempt to
   bizarrely offset boxes.  (Yes, I know that Z axle is the only call in the
   database that can cause Z's to be picked out of a 4x4.  The program is just
   trying to be very careful.)  So, when this flag is on and a shape-changer
   occurs, the program insists on a parallelogram result.

   CMD_MISC__SAID_SPLIT means that we are at a level of recursion in which
   the "split" concept has been used.  When this is on, it is not permissible
   to do a 4-person call that has not 8-person definition, and hence would have
   been done the same way without the "split" concept.  This prevents superfluous
   things like "split pass thru".

   CMD_MISC__SAID_TRIANGLE means that we are at a level of recursion in which
   the word "triangle" has been uttered, so it is OK to do things like "triangle
   peel and trail" without giving the explicit "triangle" concept again.  This
   makes it possible to say things like "tandem-based triangles peel and trail".

   CMD_MISC__DO_AS_COUPLES means that the "couples_unless_single" invocation
   flag is on, and the call may need to be done as couples or whatever.  The
   bits in "do_couples_heritflags" control this.  If INHERITFLAG_SINGLE is on,
   do not do it as couples.  If off, do it as couples.  If various other bits
   are on (e.g. INHERITFLAG_1X3), do the appropriate thing.

   CMD_MISC__NO_CHK_ELONG means that the elongation of the incoming setup is for
   informational purposes only (to tell where people should finish) and should not
   be used for raising error messages.  It suppresses the error that would be
   raised if we have people in a facing 2x2 star thru when they are far from the
   ones they are facing.

   CMD_MISC__PHANTOMS means that we are at a level of recursion in which some phantom
   concept has been used, or when "on your own" or "so-and-so do your part" concepts
   are in use.  It indicates that we shouldn't be surprised if there are phantoms
   in the setup.  When this is set, the "tandem" or "as couples" concepts will
   forgive phantoms wherever they might appear, so that "split phantom lines tandem
   <call>" is legal.  Otherwise, we would have to say "split phantom lines phantom
   tandem <call>.  This flag also allows "basic_move" to be more creative about
   inferring the starting position for a call.  If I know that I am doing a
   "so-and-so do your part" operation, it may well be that, even though the setup
   is, say, point-to-point diamonds, the centers of the diamonds are none of my
   business (I am a point), and I want to think of the setup as a grand wave,
   so I can do my part of a grand mix.  The "CMD_MISC__PHANTOMS" flag makes it
   possible to do a grand mix from point-to-point diamonds in which the centers
   are absent.

   CMD_MISC__NO_STEP_TO_WAVE means that we are at a level of recursion that no longer permits us to do the
   implicit step to a wave or rear back from one that some calls permit at the top level.

   CMD_MISC__DOING_ENDS means that this call is directed only to the ends
   of the original setup.  If the call turns out to be an 8-person call with distinct
   centers and ends parts, we may want to just apply the ends part.  This is what
   makes "ends detour" work.
*/

/* Since DFM1_CONCENTRICITY_FLAG_MASK is FF, we start at 100. */

static Const uint32 CMD_MISC__EXPLICIT_MIRROR    = 0x00000100UL;
static Const uint32 CMD_MISC__MATRIX_CONCEPT     = 0x00000200UL;
/* This is a 4 bit field.  For codes inside same, see "CMD_MISC__VERIFY_WAVES" below. */
static Const uint32 CMD_MISC__VERIFY_MASK        = 0x00003C00UL;
static Const uint32 CMD_MISC__EXPLICIT_MATRIX    = 0x00004000UL;
static Const uint32 CMD_MISC__NO_EXPAND_MATRIX   = 0x00008000UL;
static Const uint32 CMD_MISC__DISTORTED          = 0x00010000UL;
static Const uint32 CMD_MISC__OFFSET_Z           = 0x00020000UL;
static Const uint32 CMD_MISC__SAID_SPLIT         = 0x00040000UL;
static Const uint32 CMD_MISC__SAID_TRIANGLE      = 0x00080000UL;
static Const uint32 CMD_MISC__PUT_FRAC_ON_FIRST  = 0x00100000UL;
static Const uint32 CMD_MISC__DO_AS_COUPLES      = 0x00200000UL;
static Const uint32 CMD_MISC__RESTRAIN_CRAZINESS = 0x00400000UL;
static Const uint32 CMD_MISC__RESTRAIN_MODIFIERS = 0x00800000UL;
static Const uint32 CMD_MISC__NO_CHECK_MOD_LEVEL = 0x01000000UL;
#define             CMD_MISC__MUST_SPLIT_HORIZ     0x02000000UL
#define             CMD_MISC__MUST_SPLIT_VERT      0x04000000UL
static Const uint32 CMD_MISC__NO_CHK_ELONG       = 0x08000000UL;
static Const uint32 CMD_MISC__PHANTOMS           = 0x10000000UL;
static Const uint32 CMD_MISC__NO_STEP_TO_WAVE    = 0x20000000UL;
static Const uint32 CMD_MISC__ALREADY_STEPPED    = 0x40000000UL;
static Const uint32 CMD_MISC__DOING_ENDS         = 0x80000000UL;

static Const uint32 CMD_MISC__MUST_SPLIT_MASK    = (CMD_MISC__MUST_SPLIT_HORIZ|CMD_MISC__MUST_SPLIT_VERT);

/* Here are the encodings that can go into the CMD_MISC__VERIFY_MASK field.
   Zero means no verification. */
#define             CMD_MISC__VERIFY_WAVES         0x00000400UL
#define             CMD_MISC__VERIFY_2FL           0x00000800UL
#define             CMD_MISC__VERIFY_DMD_LIKE      0x00000C00UL
#define             CMD_MISC__VERIFY_QTAG_LIKE     0x00001000UL
#define             CMD_MISC__VERIFY_1_4_TAG       0x00001400UL
#define             CMD_MISC__VERIFY_3_4_TAG       0x00001800UL
#define             CMD_MISC__VERIFY_REAL_1_4_TAG  0x00001C00UL
#define             CMD_MISC__VERIFY_REAL_3_4_TAG  0x00002000UL
#define             CMD_MISC__VERIFY_REAL_1_4_LINE 0x00002400UL
#define             CMD_MISC__VERIFY_REAL_3_4_LINE 0x00002800UL
#define             CMD_MISC__VERIFY_LINES         0x00002C00UL
#define             CMD_MISC__VERIFY_COLS          0x00003000UL



/* Flags that reside in the "cmd_misc2_flags" word of a setup BEFORE a call is executed. */

/* The following are used for the "<anyone> work <concept>" or "snag the <anyone>" mechanism:

   CMD_MISC2__ANY_WORK or CMD_MISC2__ANY_SNAG is on if there is such an
   operation in place.  We will make the centers or ends (depending on
   CMD_MISC2__ANY_WORK_INVERT) use the next concept (or do 1/2 of the call)
   while the others skip that concept.  The schema, which is one of
   schema_concentric_2_6, schema_concentric_6_2, schema_concentric, or
   schema_single_concentric, is in the low 16 bits.

   CMD_MISC2__ANY_WORK_INVERT is only meaningful if the CMD_MISC2__ANY_WORK is on.
   It says that the ends are doing the concept, instead of the centers. */

/* The following are used for Z's.
   CMD_MISC2__IN_Z_CW and CMD_MISC2__IN_Z_CCW say that the setup is actually
   a 2x3, but the "Z" (or "each Z", or "triple Z's") concept has been given,
   and the setup should probably be turned into a 2x2.  The only exception
   is if the call takes a 2x3 starting setup but not a 2x2 (that is, the call
   is "Z axle").  In that case, the call is done directly in the 2x3, and the
   "Z" distortion is presumed not to have been in place. */

/* The following are used for "mystic" and old "snag".

   CMD_MISC2__MYSTIFY_SPLIT tells "divided_setup_move" to perform selective mirroring
   of the subsidiary setups because a concept like "mystic triple boxes" is in use.
   It is removed immediately by "divided_setup_move" after use.

   CMD_MISC2__MYSTIFY_INVERT is only meaningful when CMD_MISC2__MYSTIFY_SPLIT is on.
   It says that the concept is actually "invert mystic triple boxes" or whatever. */

/*  The following are used for what we call the "center/end" mechanism.  This
    mechanism is used for the "invert" (centers and ends) concept, as well as
    "central", "snag", and "mystic" and inverts thereof.

      CMD_MISC2__CTR_END_KMASK, when nonzero, says that one of the "central", "snag",
      or "mystic" concepts is in use.  They are all closely related.

      CMD_MISC2__CTR_END_INVERT means that the call is being inverted.  It is
      orthogonal to such issues as "snag" and "invert snag".  "Invert snag invert
      strike out" means that the physical ends do only half, and everyone do
      an invert strike out.  That is, the ends hinge but do not step & fold,
      while the centers detour.

      CMD_MISC2__CTR_END_MASK embraces all of the bits of the "center/end" mechanism.
*/

/*     The low 12 bits are used for encoding the schema if
       CMD_MISC2__ANY_WORK or CMD_MISC2__ANY_SNAG is on */

static Const uint32 CMD_MISC2__IN_Z_CW           = 0x00001000UL;
static Const uint32 CMD_MISC2__IN_Z_CCW          = 0x00002000UL;
static Const uint32 CMD_MISC2__IN_AZ_CW          = 0x00004000UL;
static Const uint32 CMD_MISC2__IN_AZ_CCW         = 0x00008000UL;
static Const uint32 CMD_MISC2__IN_Z_MASK         = 0x0000F000UL;
static Const uint32 CMD_MISC2__DID_Z_COMPRESSION = 0x00010000UL;

static Const uint32 CMD_MISC2__MYSTIFY_SPLIT     = 0x00020000UL;
static Const uint32 CMD_MISC2__MYSTIFY_INVERT    = 0x00040000UL;

static Const uint32 CMD_MISC2__ANY_WORK          = 0x00080000UL;
static Const uint32 CMD_MISC2__ANY_SNAG          = 0x00100000UL;
static Const uint32 CMD_MISC2__ANY_WORK_INVERT   = 0x00200000UL;


/* Here are the inversion bits for the basic operations. */
#define             CMD_MISC2__INVERT_CENTRAL      0x00400000UL
#define             CMD_MISC2__INVERT_SNAG         0x00800000UL
#define             CMD_MISC2__INVERT_MYSTIC       0x01000000UL
/* Here are the basic operations we can do. */
#define             CMD_MISC2__DO_CENTRAL          0x02000000UL
#define             CMD_MISC2__CENTRAL_SNAG        0x04000000UL
#define             CMD_MISC2__CENTRAL_MYSTIC      0x08000000UL
/* This field embraces the above 3 bits. */
#define             CMD_MISC2__CTR_END_KMASK       0x0E000000UL

/* This says the the operator said "invert".  It might later cause
   a "central" to be turned into an "invert central". */
#define             CMD_MISC2__SAID_INVERT         0x10000000UL
/* This mask embraces this whole mechanism, including the "invert" bit. */
#define             CMD_MISC2__CTR_END_MASK        0x1FC00000UL
static Const uint32 CMD_MISC2_RESTRAINED_SUPER   = 0x20000000UL;


/* Flags that reside in the "result_flags" word of a setup AFTER a call is executed.

   The low two bits tell, for a 2x2 setup after
   having a call executed, how that 2x2 is elongated in the east-west or
   north-south direction.  Since 2x2's are always totally canonical, the
   interpretation of the elongation direction is always absolute.  A 1 in this
   field means the elongation is east-west.  A 2 means the elongation is
   north-south.  A zero means the elongation is unknown.  These bits will be set
   whenever we know which elongation is preferred.  This will be the case if the
   setup was an elongated 2x2 prior to the call (because they are the ends), or
   the setup was a 1x4 prior to the call.  So, for example, after any recycle
   from a wave, these bits will be set to indicate that the dancers want to
   elongate the 2x2 perpendicular to their original wave.  These bits will be
   looked at only if the call was directed to the ends (starting from a grand
   wave, for example.)  Furthermore, the bits will be overridden if the
   "concentric" or "checkpoint" concepts were used, or if the ends did this
   as part of a concentrically defined call for which some forcing modifier
   such as "force_columns" was used.  Note that these two bits are in the same
   format as the "cmd.prior_elongation" field of a setup before the call is
   executed.  In many case, that field can be simply copied into the "result_flags"
   field.

   RESULTFLAG__DID_LAST_PART means that, when a sequentially defined call was executed
   with the "cmd_frac_flags" word nonzero, so that just one part was done,
   that part was the last part.  Hence, if we are doing a call with some "piecewise"
   or "random" concept, we do the parts of the call one at a time, with appropriate
   concepts on each part, until it comes back with this flag set.  This is used with
   RESULTFLAG__PARTS_ARE_KNOWN.

   RESULTFLAG__PARTS_ARE_KNOWN means that the bit in RESULTFLAG__DID_LAST_PART is
   valid.  Sometimes we are unable to obtain information about parts, because
   no one is doing the call.  This can legally happen, for example, on "interlocked
   parallelogram leads shakedown while the trailers star thru" from waves.  In each
   virtual 2x2 setup one or the other of the calls is being done by no one.

   RESULTFLAG__EXPAND_TO_2X3 means that a call was executed that takes a four person
   starting setup (typically a line) and yields a 2x3 result setup.  Two of those
   result spots will be phantoms, of course.  When recombining a divided setup in
   which such a call was executed, if the recombination wuold yield a 2x6, and if
   the center 4 spots would be empty, this flag directs divided_setup_move to
   collapse the setup to a 2x4.  This is what makes calls like "pair the line"
   and "step and slide" behave correctly from parallel lines and from a grand line.

   RESULTFLAG__NEED_DIAMOND means that a call has been executed with the "magic" or
   "interlocked" modifier, and it really should be changed to the "magic diamond"
   concept.  Otherwise, we might end up saying "magic diamond single wheel" when
   we should have said "magic diamond, diamond single wheel".

   RESULTFLAG__SPLIT_AXIS_MASK has info saying whether the call was split
   vertically (2) or horizontally (1) in "absolute space".  We can't say for
   sure whether the orientation is absolute, since a client may have stripped
   out rotation info.  What we mean is that it is relative to the incoming setup
   including the effect of its rotation field.  So, if the incoming setup was
   a 2x4 with rotation=0, that is, horizontally oriented in "absolute space",
   1 means it was done in 2x2's (split horizontally) and 2 means it was done
   in 1x4's (split vertically).  If the incoming setup was a 2x4 with
   rotation=1, that is, vertically oriented in "absolute space", 1 means it
   was done in 1x4's (split horizontally) and 2 means it was done in 2x2's
   (split vertically).  3 means that the call was a 1 or 2 person call, that
   could be split either way, so we have no information.

   RESULTFLAG__ACTIVE_PHANTOMS_ON and _OFF tell whether the state of the "active
   phantoms" mode flag was used in this sequence.  If it was read and found to
   be on, RESULTFLAG__ACTIVE_PHANTOMS_ON is set.  If it was read and found to
   be off, RESULTFLAG__ACTIVE_PHANTOMS_OFF is set.  It is, of course, read any
   time an "assume <whatever> concept is invoked.  This information is used to
   tell what kind of annotation to place in the transcript file to tell what
   assumptions were made.  The reason for having both bits is that the state
   of the active phantoms mode can be toggled at arbitrary times, so a single
   card might have both usages.  In that unusual case, we want to so indicate
   on the card.
*/

/* The two low bits are used for result elongation, so we start with 0x00000004. */
#define RESULTFLAG__DID_LAST_PART        0x00000004UL
#define RESULTFLAG__PARTS_ARE_KNOWN      0x00000008UL
#define RESULTFLAG__EXPAND_TO_2X3        0x00000010UL
#define RESULTFLAG__NEED_DIAMOND         0x00000020UL
#define RESULTFLAG__IMPRECISE_ROT        0x00000040UL
/* This is a six bit field. */
#define RESULTFLAG__SPLIT_AXIS_FIELDMASK 0x00001F80UL
#define RESULTFLAG__SPLIT_AXIS_XMASK     0x00000380UL
#define RESULTFLAG__SPLIT_AXIS_XBIT      0x00000080UL
#define RESULTFLAG__SPLIT_AXIS_YMASK     0x00001C00UL
#define RESULTFLAG__SPLIT_AXIS_YBIT      0x00000400UL
#define RESULTFLAG__SPLIT_AXIS_SEPARATION  3

static Const uint32 RESULTFLAG__ACTIVE_PHANTOMS_ON  = 0x00002000UL;
static Const uint32 RESULTFLAG__ACTIVE_PHANTOMS_OFF = 0x00004000UL;
static Const uint32 RESULTFLAG__SECONDARY_DONE      = 0x00008000UL;
static Const uint32 RESULTFLAG__YOYO_FINISHED       = 0x00010000UL;
static Const uint32 RESULTFLAG__TWISTED_FINISHED    = 0x00020000UL;
static Const uint32 RESULTFLAG__SPLIT_FINISHED      = 0x00040000UL;
static Const uint32 RESULTFLAG__NO_REEVALUATE       = 0x00080000UL;
static Const uint32 RESULTFLAG__DID_Z_COMPRESSION   = 0x00100000UL;
static Const uint32 RESULTFLAG__VERY_ENDS_ODD       = 0x00200000UL;
static Const uint32 RESULTFLAG__VERY_CTRS_ODD       = 0x00400000UL;

#define EXPIRATION_STATE_BITS (RESULTFLAG__YOYO_FINISHED|RESULTFLAG__TWISTED_FINISHED|RESULTFLAG__SPLIT_FINISHED)


/* It should be noted that the CMD_MISC__??? and RESULTFLAG__XXX bits have
   nothing to do with each other.  It is not intended that
   the flags resulting from one call be passed to the next.  In fact, that
   would be incorrect.  The CMD_MISC__??? bits should start at zero at the
   beginning of each call, and accumulate stuff as the call goes deeper into
   recursion.  The RESULTFLAG__??? bits should, in general, be the OR of the
   bits of the components of a compound call, though this may not be so for
   the elongation bits at the bottom of the word. */



#define TEST_HERITBITS(x,y) ((x).her8it & (y))

typedef struct {
   uint32 her8it;
   uint32 final;
} uint64;

#define MAPCODE(setupkind,num,mapkind,rot) ((((int)(setupkind)) << 10) | (((int)(mapkind)) << 4) | (((num)-1) << 1) | (rot))


typedef struct gfwzqg {
   Const setup_kind bigsetup;
   Const calldef_schema lyzer;
   Const veryshort maps[24];
   Const short inlimit;
   Const short outlimit;
   Const setup_kind insetup;
   Const setup_kind outsetup;
   Const int bigsize;
   Const int inner_rot;    /* 1 if inner setup is rotated CCW relative to big setup */
   Const int outer_rot;    /* 1 if outer setup is rotated CCW relative to big setup */
   Const int mapelong;
   Const int center_arity;
   Const int elongrotallow;
   Const calldef_schema getout_schema;
   uint32 used_mask_analyze;
   uint32 used_mask_synthesize;
   struct gfwzqg *next_analyze;
   struct gfwzqg *next_synthesize;
} cm_thing;


typedef enum {
   meta_key_random,
   meta_key_rev_random,   /* Must follow meta_key_random. */
   meta_key_piecewise,
   meta_key_initially,
   meta_key_finish,
   meta_key_revorder,
   meta_key_like_a,
   meta_key_finally,
   meta_key_nth_part_work,
   meta_key_skip_nth_part,
   meta_key_shift_n,
   meta_key_shifty,
   meta_key_echo,
   meta_key_rev_echo,   /* Must follow meta_key_echo. */
   meta_key_shift_half,
   meta_key_shift_n_half
} meta_key_kind;


/* These bits appear in the "concparseflags" word. */
/* This is a duplicate, and exists only to make menus nicer.
   Ignore it when scanning in parser. */
#define CONCPARSE_MENU_DUP       0x00000001UL
/* If the parse turns out to be ambiguous, don't use this one --
   yield to the other one. */
#define CONCPARSE_YIELD_IF_AMB   0x00000002UL
/* Parse directly.  It directs the parser to allow this concept
   (and similar concepts) and the following call to be typed
   on one line.  One needs to be very careful about avoiding
   ambiguity when setting this flag. */
#define CONCPARSE_PARSE_DIRECT   0x00000004UL
/* These are used by "print_recurse" in sdutil.c to control the printing.
   They govern the placement of commas. */
#define CONCPARSE_PARSE_L_TYPE 0x8
#define CONCPARSE_PARSE_F_TYPE 0x10
#define CONCPARSE_PARSE_G_TYPE 0x20


/* The following items are not actually part of the setup description,
   but are placed here for the convenience of "move" and similar procedures.
   They contain information about the call to be executed in this setup.
   Once the call is complete, that is, when printing the setup or storing it
   in a history array, this stuff is meaningless. */

typedef struct {
   unsigned int assump_col:  16;  /* Stuff to go with assumption -- col vs. line. */
   unsigned int assump_both:  8;  /* Stuff to go with assumption -- "handedness" enforcement --
                                                0/1/2 = either/1st/2nd. */
   unsigned int assump_cast:  6;  /* Nonzero means there is an "assume normal casts" assumption. */
   unsigned int assump_live:  1;  /* One means to accept only if everyone is live. */
   unsigned int assump_negate:1;  /* One means to invert the sense of everything. */
   call_restriction assumption;   /* Any "assume waves" type command. */
} assumption_thing;


typedef struct {
   long_boolean reverse_order;
   long_boolean first_call;
   int instant_stop;
   uint32 do_half_of_last_part;
   uint32 do_last_half_of_first_part;
   int highlimit;
   int subcall_index;
   int subcall_incr;
} fraction_info;


/* Meanings of the items in a "setup_command" structure:

   parseptr
      The full parse tree for the concept(s)/call(s) we are trying to do.
      While we traverse the big concepts in this tree, the "callspec" and
      "cmd_final_flags" are typically zero.  They only come into use when
      we reach the end of a subtree of big concepts, at which point we read
      the remaining "small" (or "final") concepts and the call name out of
      the end of the concept parse tree.

   callspec
      The call, after we reach the end of the parse tree.

   cmd_final_flags
      The various "final concept" flags with names INHERITFLAG_??? and FINAL__???.
      The INHERITFLAG_??? bits contain the final concepts like "single" that
      can be inherited from one part of a call definition to another.  The
      FINAL__??? bits contain other miscellaneous final concepts.

   cmd_misc_flags
      Other miscellaneous info controlling the execution of the call,
      with names like CMD_MISC__???.

   cmd_misc2_flags
      Even more miscellaneous info controlling the execution of the call,
      with names like CMD_MISC2__???.

   do_couples_heritflags
      These are "heritable" flags that tell what single/3x3/etc modifiers
      are to be used in determining what to do when CMD_MISC__DO_AS_COUPLES
      is specified.

   cmd_frac_flags
      If nonzero, fractionalization info controlling the execution of the call.
      See the comments in front of "get_fraction_info" in sdmoves.c for details.

   cmd_assume
      Any "assume waves" type command.

   skippable_concept
      For "<so-and-so> work <concept>", this is the concept to be skipped by some people.

   prior_elongation_bits;
      This tells, for a 2x2 setup prior to having a call executed, how that
      2x2 is elongated (due to these people being the outsides) in the east-west
      or north-south direction.  Since 2x2's are always totally canonical, the
      interpretation of the elongation direction is always absolute.  A 1 means
      the elongation is east-west.  A 2 means the elongation is north-south.
      A zero means there was no elongation. */


#define CMD_FRAC_NULL_VALUE      0x00000111
#define CMD_FRAC_HALF_VALUE      0x00000112
#define CMD_FRAC_LASTHALF_VALUE  0x00001211

#define CMD_FRAC_PART_BIT        0x00010000
#define CMD_FRAC_PART_MASK       0x000F0000
#define CMD_FRAC_REVERSE         0x00100000
/* This is a 3 bit field. */
#define CMD_FRAC_CODE_MASK       0x00E00000

/* Here are the codes that can be inside.  We require that CMD_FRAC_CODE_ONLY be zero.
   We require that the PART_MASK field be nonzero (we use 1-based part numbering)
   when these are in use.  If the PART_MASK field is zero, the code must be zero
   (that is, CMD_FRAC_CODE_ONLY), and this stuff is not in use. */

#define CMD_FRAC_CODE_ONLY           0x00000000
#define CMD_FRAC_CODE_ONLYREV        0x00200000
#define CMD_FRAC_CODE_FROMTO         0x00400000
#define CMD_FRAC_CODE_FROMTOREV      0x00600000
#define CMD_FRAC_CODE_FROMTOREVREV   0x00800000
#define CMD_FRAC_CODE_FROMTOMOST     0x00A00000
#define CMD_FRAC_CODE_LATEFROMTOREV  0x00C00000

#define CMD_FRAC_PART2_BIT       0x01000000
#define CMD_FRAC_PART2_MASK      0x07000000
#define CMD_FRAC_IMPROPER_BIT    0x08000000
#define CMD_FRAC_BREAKING_UP     0x10000000
#define CMD_FRAC_FORCE_VIS       0x20000000
#define CMD_FRAC_LASTHALF_ALL    0x40000000
#define CMD_FRAC_FIRSTHALF_ALL   0x80000000


typedef struct {
   parse_block *parseptr;
   callspec_block *callspec;
   uint64 cmd_final_flags;
   uint32 cmd_frac_flags;
   uint32 cmd_misc_flags;
   uint32 cmd_misc2_flags;
   uint32 do_couples_her8itflags;
   assumption_thing cmd_assume;
   uint32 prior_elongation_bits;
   uint32 prior_expire_bits;
   parse_block *restrained_concept;
   uint32 restrained_fraction;
   uint32 restrained_super8flags;
   parse_block *skippable_concept;
} setup_command;


/* Warning!  Do not rearrange these fields without good reason.  There are data
   initializers instantiating these in sdinit.c (test_setup_*) and in sdtables.c
   (startinfolist) that will need to be rewritten. */
typedef struct {
   setup_kind kind;
   int rotation;
   setup_command cmd;
   personrec people[MAX_PEOPLE];

   /* The following item is not actually part of the setup description, but contains
      miscellaneous information left by "move" and similar procedures, for the
      convenience of whatever called same. */
   uint32 result_flags;           /* Miscellaneous info, with names like RESULTFLAG__???. */

   /* The following three items are only used if the setup kind is "s_normal_concentric".  Note in particular that
      "outer_elongation" is thus underutilized, and that a lot of complexity goes into storing similar information
      (in two different places!) in the "prior_elongation_bits" and "result_flags" words. */
   small_setup inner;
   small_setup outer;
   int concsetup_outer_elongation;
} setup;

typedef struct {
   long_boolean (*predfunc) (setup *, int, int, int, Const long int *);
   Const long int *extra_stuff;
} predicate_descriptor;

typedef struct predptr_pair_struct {
   predicate_descriptor *pred;
   struct predptr_pair_struct *next;
   /* Dynamically allocated to whatever size is required. */
   uint16 arr[4];
} predptr_pair;

/* BEWARE!!  This list must track the array "warning_strings" in sdutil.c . */
typedef enum {
   warn__none,
   warn__do_your_part,
   warn__tbonephantom,
   warn__awkward_centers,
   warn__bad_concept_level,
   warn__not_funny,
   warn__hard_funny,
   warn__rear_back,
   warn__awful_rear_back,
   warn__excess_split,
   warn__lineconc_perp,
   warn__dmdconc_perp,
   warn__lineconc_par,
   warn__dmdconc_par,
   warn__xclineconc_perpc,
   warn__xcdmdconc_perpc,
   warn__xclineconc_perpe,
   warn__ctrstand_endscpls,
   warn__ctrscpls_endstand,
   warn__each2x2,
   warn__each1x4,
   warn__each1x2,
   warn__take_right_hands,
   warn__ctrs_are_dmd,
   warn__full_pgram,
   warn__offset_gone,
   warn__overlap_gone,
   warn__to_o_spots,
   warn__to_x_spots,
   warn__check_butterfly,
   warn__check_galaxy,
   warn__some_rear_back,
   warn__not_tbone_person,
   warn__check_c1_phan,
   warn__check_dmd_qtag,
   warn__check_quad_dmds,
   warn__check_3x4,
   warn__check_2x4,
   warn__check_4x4,
   warn__check_hokey_4x4,
   warn__check_4x4_start,
   warn__check_pgram,
   warn__ctrs_stay_in_ctr,
   warn__check_c1_stars,
   warn__check_gen_c1_stars,
   warn__bigblock_feet,
   warn__bigblockqtag_feet,
   warn__diagqtag_feet,
   warn__adjust_to_feet,
   warn__some_touch,
   warn__split_to_2x4s,
   warn__split_to_2x3s,
   warn__split_to_1x8s,
   warn__split_to_1x6s,
   warn__take_left_hands,
   warn__evil_interlocked,
   warn__split_phan_in_pgram,
   warn__bad_interlace_match,
   warn__not_on_block_spots,
   warn__stupid_phantom_clw,
   warn__bad_modifier_level,
   warn__bad_call_level,
   warn__did_not_interact,
   warn__opt_for_normal_cast,
   warn__opt_for_normal_hinge,
   warn__opt_for_2fl,
   warn_partial_solomon,
   warn_same_z_shear,
   warn__like_linear_action,
   warn__no_z_action,
   warn__phantoms_thinner,
   warn__hokey_jay_shapechanger,
   warn__split_1x6,
   warn_interlocked_to_6,
   warn__colocated_once_rem,
   warn_big_outer_triangles,
   warn_hairy_fraction,
   warn_bad_collision,
   warn__dyp_resolve_ok,
   warn__unusual,
   warn_controversial,
   warn_serious_violation,
   warn_bogus_yoyo_rims_hubs,
   warn_pg_in_2x6,
   warn__tasteless_com_spot,
   warn__tasteless_slide_thru  /* If this ceases to be last, look 2 lines below! */
} warning_index;
#define NUM_WARNINGS (((int) warn__tasteless_slide_thru)+1)

/* BEWARE!!  This list must track the array "resolve_table" in sdgetout.c . */
typedef enum {
   resolve_none,
   resolve_rlg,
   resolve_la,
   resolve_ext_rlg,
   resolve_ext_la,
   resolve_slipclutch_rlg,
   resolve_slipclutch_la,
   resolve_circ_rlg,
   resolve_circ_la,
   resolve_pth_rlg,
   resolve_pth_la,
   resolve_tby_rlg,
   resolve_tby_la,
   resolve_xby_rlg,
   resolve_xby_la,
   resolve_dixie_grand,
   resolve_prom,
   resolve_revprom,
   resolve_sglfileprom,
   resolve_revsglfileprom,
   resolve_circle
} resolve_kind;

typedef struct {
   resolve_kind kind;
   int distance;
} resolve_indicator;


/* These bits are used to allocate flag bits
   that appear in the "callflagsf" word of a top level callspec_block
   and the "cmd_final_flags.final" of a setup with its command block. */

/* A 3-bit field. */
#define CFLAGH__TAG_CALL_RQ_MASK          0x00000007UL
#define CFLAGH__TAG_CALL_RQ_BIT           0x00000001UL
#define CFLAGH__REQUIRES_SELECTOR         0x00000008UL
#define CFLAGH__REQUIRES_DIRECTION        0x00000010UL
#define CFLAGH__CIRC_CALL_RQ_BIT          0x00000020UL
#define CFLAGH__ODD_NUMBER_ONLY           0x00000040UL
#define CFLAGHSPARE_1                     0x00000080UL
#define CFLAGHSPARE_2                     0x00000100UL
#define CFLAGHSPARE_3                     0x00000200UL
#define CFLAGHSPARE_4                     0x00000400UL
#define CFLAGHSPARE_5                     0x00000800UL
#define CFLAGHSPARE_6                     0x00001000UL
/* We need to leave the top 8 bits free in order to accomodate the "CFLAG2" bits. */


/* These flags go along for the ride, in some parts of the code (BUT NOT
   THE CALLFLAGSF WORD OF A CALLSPEC!), in the same word as the heritable flags,
   but are not part of the inheritance mechanism.  We use symbols that have been
   graciously provided for us from database.h to tell us what bits may be safely
   used next to the heritable flags. */

#define FINAL__SPLIT                      CFLAGHSPARE_1
#define FINAL__SPLIT_SQUARE_APPROVED      CFLAGHSPARE_2
#define FINAL__SPLIT_DIXIE_APPROVED       CFLAGHSPARE_3
#define FINAL__MUST_BE_TAG                CFLAGHSPARE_4
#define FINAL__TRIANGLE                   CFLAGHSPARE_5
#define FINAL__LEADTRIANGLE               CFLAGHSPARE_6

/* These flags, and "CFLAGH__???" flags, go along for the ride, in the callflagsf
   word of a callspec.  We use symbols that have been graciously
   provided for us from database.h to tell us what bits may be safely used next
   to the heritable flags.  Note that these bits overlap the FINAL__?? bits above.
   These are used in the callflagsf word of a callspec.  The FINAL__?? bits are
   used elsewhere.  They don't mix. */

#define ESCAPE_WORD__LEFT                 CFLAGHSPARE_1
#define ESCAPE_WORD__CROSS                CFLAGHSPARE_2
#define ESCAPE_WORD__MAGIC                CFLAGHSPARE_3
#define ESCAPE_WORD__INTLK                CFLAGHSPARE_4

typedef struct glonk {
   char txt[MAX_TEXT_LINE_LENGTH];
   struct glonk *nxt;
} comment_block;

typedef struct flonk {
   char txt[80];
   struct flonk *nxt;
} outfile_block;

typedef uint32 defmodset;


/* These flags go into the "concept_prop" field of a "concept_table_item".

   CONCPROP__SECOND_CALL means that the concept takes a second call, so a sublist must
      be created, with a pointer to same just after the concept pointer.

   CONCPROP__USE_SELECTOR means that the concept requires a selector, which must be
      inserted into the concept list just after the concept pointer.

   CONCPROP__NEED_4X4   mean that the concept requires the indicated setup, and, at
   CONCPROP__NEED_2X8   the top level, the existing setup should be expanded as needed.
      etc....

   CONCPROP__SET_PHANTOMS means that phantoms are in use under this concept, so that,
      when looking for tandems or couples, we shouldn't be disturbed if we
      pair someone with a phantom.  It is what makes "split phantom lines tandem"
      work, so that "split phantom lines phantom tandem" is unnecessary.

   CONCPROP__NO_STEP means that stepping to a wave or rearing back from one is not
      allowed under this concept.

   CONCPROP__GET_MASK means that tbonetest & livemask need to be computed before executing the concept.

   CONCPROP__STANDARD means that the concept can be "standard".

   CONCPROP__USE_NUMBER         If a concept takes one number, only CONCPROP__USE_NUMBER is set.
   CONCPROP__USE_TWO_NUMBERS    If it takes two numbers both bits are set.

   CONCPROP__SHOW_SPLIT means that the concept prepares the "split_axis" bits properly
      for transmission back to the client.  Normally this is off, and the split axis bits
      will be cleared after execution of the concept.

   CONCPROP__NEED_ARG2_MATRIX means that the "arg2" word of the concept_descriptor
      block contains additional bits of the "CONCPROP__NEED_3X8" kind to be sent to
      "do_matrix_expansion".  This is done so that concepts with different matrix
      expansion bits can share the same concept type.
*/



#define CONCPROP__SECOND_CALL      0x00000001UL
#define CONCPROP__USE_SELECTOR     0x00000002UL
#define CONCPROP__SET_PHANTOMS     0x00000004UL
#define CONCPROP__NO_STEP          0x00000008UL

/* A bit mask for the "NEEDK" bits. */
#define CONCPROP__NEED_MASK        0x000001F0UL

#define CONCPROP__NEEDK_4X4        0x00000010UL
#define CONCPROP__NEEDK_2X8        0x00000020UL
#define CONCPROP__NEEDK_2X6        0x00000030UL
#define CONCPROP__NEEDK_4DMD       0x00000040UL
#define CONCPROP__NEEDK_BLOB       0x00000050UL
#define CONCPROP__NEEDK_4X6        0x00000060UL
#define CONCPROP__NEEDK_3X8        0x00000070UL
#define CONCPROP__NEEDK_3DMD       0x00000080UL
#define CONCPROP__NEEDK_1X10       0x00000090UL
#define CONCPROP__NEEDK_1X12       0x000000A0UL
#define CONCPROP__NEEDK_3X4        0x000000B0UL
#define CONCPROP__NEEDK_1X16       0x000000C0UL
#define CONCPROP__NEEDK_QUAD_1X4   0x000000D0UL
#define CONCPROP__NEEDK_TWINDMD    0x000000E0UL
#define CONCPROP__NEEDK_TWINQTAG   0x000000F0UL
#define CONCPROP__NEEDK_CTR_DMD    0x00000100UL
#define CONCPROP__NEEDK_END_DMD    0x00000110UL
#define CONCPROP__NEEDK_TRIPLE_1X4 0x00000120UL
#define CONCPROP__NEEDK_CTR_1X4    0x00000130UL
#define CONCPROP__NEEDK_END_1X4    0x00000140UL
#define CONCPROP__NEEDK_CTR_2X2    0x00000150UL
#define CONCPROP__NEEDK_END_2X2    0x00000160UL
#define CONCPROP__NEEDK_3X4_D3X4   0x00000170UL
#define CONCPROP__NEEDK_3X6        0x00000180UL
#define CONCPROP__NEEDK_4D_4PTPD   0x00000190UL
#define CONCPROP__NEEDK_4X5        0x000001A0UL
#define CONCPROP__NEEDK_2X12       0x000001B0UL

#define CONCPROP__NEED_ARG2_MATRIX 0x00000200UL                                   
/* spare:                          0x00000400UL */
/* spare:                          0x00000800UL */
/* spare:                          0x00010000UL */
/* spare:                          0x00020000UL */
/* spare:                          0x00040000UL */
/* spare:                          0x00080000UL */
/* spare:                          0x00100000UL */
#define CONCPROP__GET_MASK         0x00200000UL
#define CONCPROP__STANDARD         0x00400000UL
#define CONCPROP__USE_NUMBER       0x00800000UL
#define CONCPROP__USE_TWO_NUMBERS  0x01000000UL
#define CONCPROP__MATRIX_OBLIVIOUS 0x02000000UL
#define CONCPROP__PERMIT_MATRIX    0x04000000UL
#define CONCPROP__SHOW_SPLIT       0x08000000UL
#define CONCPROP__PERMIT_MYSTIC    0x10000000UL
#define CONCPROP__PERMIT_REVERSE   0x20000000UL

typedef struct {
   Const veryshort source_indices[24];
   Const int size;
   Const setup_kind inner_kind;
   Const setup_kind outer_kind;
   Const int rot;
} expand_thing;

typedef enum {    /* These control error messages that arise when we divide a setup
                     into subsetups (e.g. phantom lines) and find that one of
                     the setups is empty.  For example, if we are in normal lines
                     and select "phantom lines", it will notice that one of the setups
                     (the outer one) isn't occupied and will give the error message
                     "Don't use phantom concept if you don't mean it." */

   phantest_ok,               /* Anything goes */
   phantest_impossible,       /* Can't happen in symmetric stuff?? */
   phantest_both,             /* Require both setups (partially) occupied */
   phantest_only_one,         /* Require only one setup occupied, don't care which */
   phantest_only_first_one,   /* Require only first setup occupied, second empty */
   phantest_only_second_one,  /* Require only second setup occupied, first empty */
   phantest_first_or_both,    /* Require first setup only, or a mixture */
   phantest_ctr_phantom_line, /* Special, created when outside phantom setup is empty. */
   phantest_2x2_both,
   phantest_2x2_only_two,
   phantest_not_just_centers
} phantest_kind;

typedef enum {
   /* Warning!!!!  Order is important!  See all the stupid ways these are used
      in sdconc.c . */
   selective_key_dyp,
   selective_key_own,
   selective_key_plain,
   selective_key_disc_dist,
   selective_key_ignore,
   selective_key_work_concept,
   selective_key_lead_for_a,
   selective_key_work_no_concentric,
   selective_key_snag_anyone,
   selective_key_plain_from_id_bits
} selective_key;

typedef enum {
   disttest_t, disttest_nil, disttest_only_two,
   disttest_any, disttest_offset, disttest_z} disttest_kind;

typedef enum {
   simple_normalize,
   normalize_before_isolated_call,
   normalize_to_6,
   normalize_to_4,
   normalize_to_2,
   normalize_after_triple_squash,
   normalize_before_merge
} normalize_action;

typedef enum {
   merge_strict_matrix,
   merge_c1_phantom,
   merge_c1_phantom_nowarn,
   merge_without_gaps
} merge_action;

typedef enum {
   chk_none,
   chk_wave,
   chk_groups,
   chk_anti_groups,
   chk_box,
   chk_box_dbl,
   chk_indep_box,
   chk_dmd_qtag,
   chk_qtag,
   chk_qbox,
   chk_peelable,
   chk_spec_directions
} chk_type;

typedef struct {
   int size;
   veryshort map1[17];
   veryshort map2[17];
   veryshort map3[8];
   veryshort map4[8];
   long_boolean ok_for_assume;
   chk_type check;
} restriction_thing;


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

#define LOOKUP_NONE     0x1
#define LOOKUP_DIST_DMD 0x2
#define LOOKUP_DIST_BOX 0x4
#define LOOKUP_DIST_CLW 0x8
#define LOOKUP_DIAG_CLW 0x10
#define LOOKUP_OFFS_CLW 0x20
#define LOOKUP_DISC     0x40
#define LOOKUP_IGNORE   0x80
#define LOOKUP_Z        0x100


typedef struct dirbtek {
   Const uint32 key;
   Const setup_kind kk;
   Const uint32 thislivemask;
   Const fixer *fixp;
   Const fixer *fixp2;
   Const int use_fixp2;
   struct dirbtek *next;
} sel_item;


/* This allows 96 warnings. */
/* BEWARE!!  If this is changed, this initializers for things like "no_search_warnings"
   in sdamin.c will need to be updated. */
#define WARNING_WORDS 3

typedef struct {
   uint32 bits[WARNING_WORDS];
} warning_info;

typedef struct {           /* This record is one state in the evolving sequence. */
   parse_block *command_root;
   setup state;
   resolve_indicator resolve_flag;
   long_boolean draw_pic;
   warning_info warnings;
   int centersp;           /* only nonzero for history[1] */
   int text_line;          /* how many lines of text existed after this item was written,
                              only meaningful if "written_history_items" is >= this index */
} configuration;

typedef struct {
   uint32 concept_prop;      /* Takes bits of the form CONCPROP__??? */
   void (*concept_action)(setup *, parse_block *, setup *);
} concept_table_item;

typedef struct {
   char *name;
   long_boolean into_the_middle;
   setup the_setup;
} startinfo;

typedef enum {
   error_flag_none = 0,          /* Must be zero because setjmp returns this. */
   error_flag_1_line,            /* 1-line error message, text is in error_message1. */
   error_flag_2_line,            /* 2-line error message, text is in error_message1 and
                                    error_message2. */
   error_flag_collision,         /* collision error, message is that people collided, they are in
                                    collision_person1 and collision_person2. */
   error_flag_wrong_resolve_command, /* "resolve" or similar command was called
                                         in inappropriate context, text is in error_message1. */
   error_flag_wrong_command,     /* clicked on something inappropriate in subcall reader. */
   error_flag_cant_execute,      /* unable-to-execute error, person is in collision_person1,
                                    text is in error_message1. */
   error_flag_show_stats,        /* wants to display stale call statistics. */
   error_flag_selector_changed,  /* warn that selector was changed during clipboard paste. */
   error_flag_formation_changed  /* warn that formation changed during clipboard paste. */
} error_flag_type;

typedef struct {
   int *full_list;
   int *on_level_list;
   int full_list_size;
} nice_setup_thing;

typedef Const struct {
   Const setup_kind result_kind;
   Const int xfactor;
   Const veryshort xca[24];
   Const veryshort yca[24];
   Const veryshort diagram[64];
} coordrec;

typedef uint32 id_bit_table[4];

typedef Const struct {
   /* This is the size of the setup MINUS ONE. */
   Const int setup_limits;

   /* These "coordrec" items have the fudged coordinates that are used for doing
      press/truck calls.  For some setups, the coordinates of some people are
      deliberately moved away from the obvious precise matrix spots so that
      those people can't press or truck.  For example, the lateral spacing of
      diamond points is not an integer.  If a diamond point does any truck or loop
      call, he/she will not end up on the other diamond point spot (or any other
      spot in the formation), so the call will not be legal.  This enforces our
      view, not shared by all callers (Hi, Clark!) that the diamond points are NOT
      as if the ends of lines of 3, and hence can NOT trade with each other by
      doing a right loop 1. */
   coordrec *setup_coords;

   /* The above table is not suitable for performing mirror inversion because,
      for example, the points of diamonds do not reflect onto each other.  This
      table has unfudged coordinates, in which all the symmetries are observed.
      This is the table that is used for mirror reversal.  Most of the items in
      it are the same as those in the table above. */
   coordrec *nice_setup_coords;

   /* These determine how designators like "side boys" get turned into
      "center 2", so that so-and-so moves can be done with the much
      more powerful concentric mechanism. */

   Const uint32 mask_normal;
   Const uint32 mask_6_2;
   Const uint32 mask_2_6;
   Const uint32 mask_ctr_dmd;

   /* These show the beginning setups that we look for in a by-array call
      definition in order to do a call in this setup. */
   Const begin_kind keytab[2];

   /* In the bounding boxes, we do not fill in the "length" of a diamond, nor
      the "height" of a qtag.  Everyone knows that the number must be 3, but it
      is not really accepted that one can use that in instances where precision
      is required.  That is, one should not make "matrix" calls depend on this
      number.  Witness all the "diamond to quarter tag adjustment" stuff that
      callers worry about, and the ongoing controversy about which way a quarter
      tag setup is elongated, even though everyone knows that it is 4 wide and 3
      deep, and that it is generally recognized, by the mathematically erudite,
      that 4 is greater than 3. */
   Const short int bounding_box[2];

   /* This is true if the setup has 4-way symmetry.  Such setups will always be
      canonicalized so that their rotation field will be zero. */
   Const long_boolean four_way_symmetry;

   /* This is the bit table for filling in the "ID2" bits. */
   id_bit_table *id_bit_table_ptr;

   /* These are the tables that show how to print out the setup. */
   Cstring print_strings[2];
} setup_attr;

typedef struct milch {
   uint32 code;
   Const map_thing *the_map;
   struct milch *next;
} mapcoder;

typedef struct {
   uint32 newheritmods;
   uint32 newfinalmods;
   int before;    /* These are indices into concept_descriptor_table. */
   int after;
} concept_fixer_thing;

typedef enum {
   resolve_goodness_only_nice,
   resolve_goodness_always,
   resolve_goodness_maybe
} resolve_goodness_test;


#define zig_zag_level l_a2
#define cross_by_level l_c1
#define dixie_grand_level l_plus
#define extend_34_level l_plus
#define phantom_tandem_level l_c4a
#define intlk_triangle_level l_c2
#define beau_belle_level l_a2

typedef struct {           /* This is done to preserve the encapsulation of type "jmp_buf".                  */
   jmp_buf the_buf;        /*   We are going to use pointers to these things.  If we simply used             */
} real_jmp_buf;            /*   pointers to jmp_buf, the semantics would not be transparent if jmp_buf       */
                           /*   were defined as an array.  The semantics of pointers to arrays are different */
                           /*   from those of pointers to other types, and are not transparent.              */
                           /*   In particular, we would need to look inside the include file to see what     */
                           /*   underlying type jmp_buf is an array of, which would violate the principle    */
                           /*   of type encapsulation.                                                       */


/* VARIABLES */

extern real_jmp_buf longjmp_buffer;                                 /* in SDUTIL */
extern real_jmp_buf *longjmp_ptr;                                   /* in SDUTIL */
extern configuration *history;                                      /* in SDUTIL */
extern int history_allocation;                                      /* in SDUTIL */
extern int history_ptr;                                             /* in SDUTIL */
extern int written_history_items;                                   /* in SDUTIL */
extern int written_history_nopic;                                   /* in SDUTIL */
extern parse_block *last_magic_diamond;                             /* in SDUTIL */
extern char error_message1[MAX_ERR_LENGTH];                         /* in SDUTIL */
extern char error_message2[MAX_ERR_LENGTH];                         /* in SDUTIL */
extern uint32 collision_person1;                                    /* in SDUTIL */
extern uint32 collision_person2;                                    /* in SDUTIL */
extern Cstring warning_strings[];                                   /* in SDUTIL */

extern uint32 global_tbonetest;                                     /* in SDCONCPT */
extern uint32 global_livemask;                                      /* in SDCONCPT */
extern uint32 global_selectmask;                                    /* in SDCONCPT */
extern uint32 global_tboneselect;                                   /* in SDCONCPT */
extern concept_table_item concept_table[];                          /* in SDCONCPT */

extern concept_fixer_thing concept_fixer_table[];                   /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_4x4;                       /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_3x4;                       /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_2x8;                       /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_2x6;                       /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_1x12;                      /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_1x16;                      /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_3dmd;                      /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_4dmd;                      /* in SDCTABLE */
extern nice_setup_thing nice_setup_thing_4x6;                       /* in SDCTABLE */
extern int phantom_concept_index;                                   /* in SDCTABLE */
extern int matrix_2x8_concept_index;                                /* in SDCTABLE */
extern int cross_concept_index;                                     /* in SDCTABLE */
extern int magic_concept_index;                                     /* in SDCTABLE */
extern int intlk_concept_index;                                     /* in SDCTABLE */
extern int left_concept_index;                                      /* in SDCTABLE */
extern int grand_concept_index;                                     /* in SDCTABLE */
extern int general_concept_offset;                                  /* in SDCTABLE */
extern int general_concept_size;                                    /* in SDCTABLE */
extern int *concept_offset_tables[];                                /* in SDCTABLE */
extern int *concept_size_tables[];                                  /* in SDCTABLE */
extern Cstring concept_menu_strings[];                              /* in SDCTABLE */

extern id_bit_table id_bit_table_2x6_pg[];                          /* in SDTABLES */
extern id_bit_table id_bit_table_bigdmd_wings[];                    /* in SDTABLES */
extern id_bit_table id_bit_table_bigbone_wings[];                   /* in SDTABLES */
extern id_bit_table id_bit_table_bighrgl_wings[];                   /* in SDTABLES */
extern id_bit_table id_bit_table_bigdhrgl_wings[];                  /* in SDTABLES */
extern id_bit_table id_bit_table_3x4_offset[];                      /* in SDTABLES */
extern id_bit_table id_bit_table_3x4_h[];                           /* in SDTABLES */
extern id_bit_table id_bit_table_3x4_ctr6[];                        /* in SDTABLES */
extern id_bit_table id_bit_table_525_nw[];                          /* in SDTABLES */
extern id_bit_table id_bit_table_525_ne[];                          /* in SDTABLES */
extern id_bit_table id_bit_table_343_outr[];                        /* in SDTABLES */
extern id_bit_table id_bit_table_343_innr[];                        /* in SDTABLES */
extern id_bit_table id_bit_table_545_outr[];                        /* in SDTABLES */
extern id_bit_table id_bit_table_545_innr[];                        /* in SDTABLES */
extern id_bit_table id_bit_table_3dmd_in_out[];                     /* in SDTABLES */
extern id_bit_table id_bit_table_3dmd_ctr1x6[];                     /* in SDTABLES */
extern id_bit_table id_bit_table_3dmd_ctr1x4[];                     /* in SDTABLES */
extern id_bit_table id_bit_table_3ptpd[];                           /* in SDTABLES */
extern cm_thing conc_init_table[];                                  /* in SDTABLES */
extern Const fixer f2x4far;                                         /* in SDTABLES */
extern Const fixer f2x4near;                                        /* in SDTABLES */
extern Const fixer fdhrgl;                                          /* in SDTABLES */
extern Const fixer f323;                                            /* in SDTABLES */
extern sel_item sel_init_table[];                                   /* in SDTABLES */
extern setup_attr setup_attrs[];                                    /* in SDTABLES */
extern int begin_sizes[];                                           /* in SDTABLES */
extern startinfo startinfolist[];                                   /* in SDTABLES */
extern map_thing map_p8_tgl4;                                       /* in SDTABLES */
extern map_thing map_spndle_once_rem;                               /* in SDTABLES */
extern map_thing map_1x3dmd_once_rem;                               /* in SDTABLES */
extern map_thing map_lh_zzztgl;                                     /* in SDTABLES */
extern map_thing map_rh_zzztgl;                                     /* in SDTABLES */
extern map_thing map_2x2v;                                          /* in SDTABLES */
extern map_thing map_2x4_magic;                                     /* in SDTABLES */
extern map_thing map_qtg_magic;                                     /* in SDTABLES */
extern map_thing map_qtg_intlk;                                     /* in SDTABLES */
extern map_thing map_qtg_magic_intlk;                               /* in SDTABLES */
extern map_thing map_ptp_magic;                                     /* in SDTABLES */
extern map_thing map_ptp_intlk;                                     /* in SDTABLES */
extern map_thing map_ptp_magic_intlk;                               /* in SDTABLES */
extern map_thing map_2x4_diagonal;                                  /* in SDTABLES */
extern map_thing map_2x4_int_pgram;                                 /* in SDTABLES */
extern map_thing map_2x4_trapezoid;                                 /* in SDTABLES */
extern map_thing map_4x4_ns;                                        /* in SDTABLES */
extern map_thing map_4x4_ew;                                        /* in SDTABLES */
extern map_thing map_vsplit_f;                                      /* in SDTABLES */
extern map_thing map_stairst;                                       /* in SDTABLES */
extern map_thing map_ladder;                                        /* in SDTABLES */
extern map_thing map_but_o;                                         /* in SDTABLES */
extern map_thing map_offset;                                        /* in SDTABLES */
extern map_thing map_4x4v;                                          /* in SDTABLES */
extern map_thing map_blocks;                                        /* in SDTABLES */
extern map_thing map_trglbox;                                       /* in SDTABLES */
extern map_thing map_2x3_0134;                                      /* in SDTABLES */
extern map_thing map_2x3_1245;                                      /* in SDTABLES */
extern map_thing map_1x8_1x6;                                       /* in SDTABLES */
extern map_thing map_rig_1x6;                                       /* in SDTABLES */
extern map_thing map_3mdmd_3d;                                      /* in SDTABLES */
extern map_thing map_4x6_2x4;                                       /* in SDTABLES */
extern map_thing map_ov_hrg_1;                                      /* in SDTABLES */
extern map_thing map_ov_gal_1;                                      /* in SDTABLES */
extern map_thing map_3o_qtag_1;                                     /* in SDTABLES */
extern map_thing map_tgl4_1;                                        /* in SDTABLES */
extern map_thing map_tgl4_2;                                        /* in SDTABLES */
extern map_thing map_qtag_2x3;                                      /* in SDTABLES */
extern map_thing map_2x3_rmvr;                                      /* in SDTABLES */
extern map_thing map_dbloff1;                                       /* in SDTABLES */
extern map_thing map_dbloff2;                                       /* in SDTABLES */
extern map_thing map_dhrgl1;                                        /* in SDTABLES */
extern map_thing map_dhrgl2;                                        /* in SDTABLES */
extern map_thing map_dbgbn1;                                        /* in SDTABLES */
extern map_thing map_dbgbn2;                                        /* in SDTABLES */
extern map_thing map_off1x81;                                       /* in SDTABLES */
extern map_thing map_off1x82;                                       /* in SDTABLES */
extern map_thing map_dqtag1;                                        /* in SDTABLES */
extern map_thing map_dqtag2;                                        /* in SDTABLES */
extern map_thing map_trngl_box1;                                    /* in SDTABLES */
extern map_thing map_trngl_box2;                                    /* in SDTABLES */
extern map_thing map_inner_box;                                     /* in SDTABLES */
extern map_thing map_lh_c1phana;                                    /* in SDTABLES */
extern map_thing map_lh_c1phanb;                                    /* in SDTABLES */
extern map_thing map_lh_s2x3_3;                                     /* in SDTABLES */
extern map_thing map_lh_s2x3_2;                                     /* in SDTABLES */
extern map_thing map_rh_c1phana;                                    /* in SDTABLES */
extern map_thing map_rh_c1phanb;                                    /* in SDTABLES */
extern map_thing map_rh_s2x3_3;                                     /* in SDTABLES */
extern map_thing map_rh_s2x3_2;                                     /* in SDTABLES */
extern map_thing map_d1x10;                                         /* in SDTABLES */
extern map_thing map_lz12;                                          /* in SDTABLES */
extern map_thing map_rz12;                                          /* in SDTABLES */
extern map_thing map_dmd_1x1;                                       /* in SDTABLES */
extern map_thing map_star_1x1;                                      /* in SDTABLES */
extern map_thing map_qtag_f0;                                       /* in SDTABLES */
extern map_thing map_qtag_f1;                                       /* in SDTABLES */
extern map_thing map_qtag_f2;                                       /* in SDTABLES */
extern map_thing map_diag2a;                                        /* in SDTABLES */
extern map_thing map_diag2b;                                        /* in SDTABLES */
extern map_thing map_diag23a;                                       /* in SDTABLES */
extern map_thing map_diag23b;                                       /* in SDTABLES */
extern map_thing map_diag23c;                                       /* in SDTABLES */
extern map_thing map_diag23d;                                       /* in SDTABLES */
extern map_thing map_f2x8_4x4;                                      /* in SDTABLES */
extern map_thing map_w4x4_4x4;                                      /* in SDTABLES */
extern map_thing map_f2x8_2x8;                                      /* in SDTABLES */
extern map_thing map_w4x4_2x8;                                      /* in SDTABLES */
extern map_thing *maps_3diag[4];                                    /* in SDTABLES */
extern map_thing *maps_3diagwk[4];                                  /* in SDTABLES */
extern mapcoder map_init_table[];                                   /* in SDTABLES */
extern map_thing map_init_table2[];                                 /* in SDTABLES */
extern map_thing *split_lists[][6];                                 /* in SDTABLES */


/*
extern comment_block *comment_root;
extern comment_block *comment_last;
*/

extern warning_info no_search_warnings;                             /* in SDMAIN */
extern warning_info conc_elong_warnings;                            /* in SDMAIN */
extern warning_info dyp_each_warnings;                              /* in SDMAIN */
extern warning_info useless_phan_clw_warnings;                      /* in SDMAIN */

extern command_kind search_goal;                                    /* in SDPICK */

extern long_boolean selector_used;                                  /* in SDPREDS */
extern long_boolean number_used;                                    /* in SDPREDS */
extern long_boolean mandatory_call_used;                            /* in SDPREDS */
extern predicate_descriptor pred_table[];                           /* in SDPREDS */
extern int selector_preds;                                          /* in SDPREDS */

extern  expand_thing comp_qtag_2x4_stuff;                           /* in SDTOP */
extern  expand_thing exp_2x3_qtg_stuff;                             /* in SDTOP */
extern  expand_thing exp_4x4_4x6_stuff_a;                           /* in SDTOP */
extern  expand_thing exp_4x4_4x6_stuff_b;                           /* in SDTOP */
extern  expand_thing exp_4x4_4dm_stuff_a;                           /* in SDTOP */
extern  expand_thing exp_4x4_4dm_stuff_b;                           /* in SDTOP */
extern  expand_thing exp_c1phan_4x4_stuff1;                         /* in SDTOP */
extern  expand_thing exp_c1phan_4x4_stuff2;                         /* in SDTOP */


/* In SDMAIN */

extern char *sd_version_string(void);
extern parse_block *mark_parse_blocks(void);
extern void release_parse_blocks_to_mark(parse_block *mark_point);
extern void initialize_parse(void);
extern parse_block *copy_parse_tree(parse_block *original_tree);
extern void save_parse_state(void);
extern long_boolean restore_parse_state(void);
extern long_boolean query_for_call(void);
extern void write_header_stuff(long_boolean with_ui_version, uint32 act_phan_flags);
extern long_boolean sequence_is_resolved(void);

/* In SDPICK */

extern void reset_internal_iterators(void);
extern selector_kind do_selector_iteration(long_boolean allow_iteration);
extern direction_kind do_direction_iteration(void);
extern void do_number_iteration(int howmanynumbers,
                                uint32 odd_number_only,
                                long_boolean allow_iteration,
                                uint32 *number_list);
extern void do_circcer_iteration(uint32 *circcp);
extern long_boolean do_tagger_iteration(uint32 tagclass,
                                        uint32 *tagg,
                                        uint32 numtaggers,
                                        callspec_block **tagtable);
extern callspec_block *do_pick(void);
extern concept_descriptor *pick_concept(long_boolean already_have_concept_in_place);
extern resolve_goodness_test get_resolve_goodness_info(void);
extern long_boolean pick_allow_multiple_items(void);
extern void start_pick(void);
extern void end_pick(void);
extern long_boolean forbid_call_with_mandatory_subcall(void);
extern long_boolean allow_random_subcall_pick(void);

/* In SDPREDS */

extern long_boolean selectp(setup *ss, int place);

/* In SDSI */

extern void general_initialize(void);
extern int generate_random_number(int modulus);
extern void hash_nonrandom_number(int number);
extern void get_date(char dest[]);
extern void unparse_number(int arg, char dest[]);
extern void open_file(void);
extern void write_file(char line[]);
extern void close_file(void);
extern void print_line(Cstring s);
extern void print_id_error(int n);
extern void init_error(char s[]) nonreturning;
extern void add_resolve_indices(char junk[], int cur, int max);
extern void fill_in_neglect_percentage(char junk[], int n);
extern int parse_number(char junk[]);

/* In SDUTIL */

extern void initialize_restr_tables(void);
extern restriction_thing *get_restriction_thing(setup_kind k, assumption_thing t);
extern void clear_screen(void);
extern void newline(void);
extern void writestuff(Const char s[]);
extern void unparse_call_name(Cstring name, char *s, call_conc_option_state *options);
extern void doublespace_file(void);
extern void fail(Const char s[]) nonreturning;
extern void fail2(Const char s1[], Const char s2[]) nonreturning;
extern void failp(uint32 id1, Const char s[]) nonreturning;
extern void specialfail(Const char s[]) nonreturning;
extern Const char *get_escape_string(char c);
extern void string_copy(char **dest, Cstring src);
extern void print_recurse(parse_block *thing, int print_recurse_arg);
extern void display_initial_history(int upper_limit, int num_pics);
extern void write_history_line(int history_index, Const char *header, long_boolean picture, file_write_flag write_to_file);
extern void warn(warning_index w);
extern call_list_kind find_proper_call_list(setup *s);
extern Const restriction_thing *get_rh_test(setup_kind kind);
extern long_boolean verify_restriction(
   setup *ss,
   restriction_thing *rr,
   assumption_thing tt,
   long_boolean instantiate_phantoms,
   long_boolean *failed_to_instantiate);
extern callarray *assoc(begin_kind key, setup *ss, callarray *spec);
extern uint32 find_calldef(
   callarray *tdef,
   setup *scopy,
   int real_index,
   int real_direction,
   int northified_index);
extern void clear_people(setup *z);
extern uint32 rotperson(uint32 n, int amount);
extern uint32 rotcw(uint32 n);
extern uint32 rotccw(uint32 n);
extern void clear_person(setup *resultpeople, int resultplace);
extern uint32 copy_person(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace);
extern uint32 copy_rot(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace, int rotamount);
extern void swap_people(setup *ss, int oneplace, int otherplace);
extern void install_person(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace);
extern void install_rot(setup *resultpeople, int resultplace, setup *sourcepeople, int sourceplace, int rotamount);
extern void scatter(setup *resultpeople, setup *sourcepeople, Const veryshort *resultplace, int countminus1, int rotamount);
extern void gather(setup *resultpeople, setup *sourcepeople, Const veryshort *resultplace, int countminus1, int rotamount);
extern parse_block *process_final_concepts(
   parse_block *cptr,
   long_boolean check_errors,
   uint64 *final_concepts);
extern parse_block *really_skip_one_concept(
   parse_block *incoming,
   concept_kind *k_p,
   uint32 *need_to_restrain_p,   /* 1=(if not doing echo), 2=(yes, always) */
   parse_block **parseptr_skip_p);
extern long_boolean fix_n_results(int arity, setup_kind goal, setup z[], uint32 *rotstatep);

/* In SDGETOUT */

extern resolve_indicator resolve_p(setup *s);
extern void write_resolve_text(long_boolean doing_file);
extern uims_reply full_resolve(void);
extern int concepts_in_place(void);
extern int reconcile_command_ok(void);
extern int resolve_command_ok(void);
extern int nice_setup_command_ok(void);
extern void initialize_getout_tables(void);

/* In SDBASIC */

extern void mirror_this(setup *s);

extern void fix_collision(
   uint32 explicit_mirror_flag,
   uint32 collision_mask,
   int collision_index,
   uint32 result_mask,
   int appears_illegal,
   long_boolean mirror,
   assumption_thing *assumption,
   setup *result);

extern void do_stability(uint32 *personp, stability stab, int turning);

extern long_boolean check_restriction(
   setup *ss,
   assumption_thing restr,
   long_boolean instantiate_phantoms,
   uint32 flags);

extern void basic_move(
   setup *ss,
   int tbonetest,
   long_boolean fudged,
   long_boolean mirror,
   setup *result);

/* In SDMOVES */

extern void canonicalize_rotation(setup *result);
extern void reinstate_rotation(setup *ss, setup *result);

extern long_boolean divide_for_magic(
   setup *ss,
   uint32 heritflags_to_check,
   setup *result);

extern long_boolean do_simple_split(
   setup *ss,
   uint32 prefer_1x4,   /* 1 means prefer 1x4, 2 means this is 1x8 and do not recompute id. */
   setup *result);

extern void do_call_in_series(
   setup *sss,
   long_boolean dont_enforce_consistent_split,
   long_boolean roll_transparent,
   long_boolean normalize,
   long_boolean qtfudged);

extern void anchor_someone_and_move(setup *ss, parse_block *parseptr, setup *result);

extern void process_number_insertion(uint32 mod_word);

extern int gcd(int a, int b);

extern uint32 process_new_fractions(
   int numer,
   int denom,
   uint32 incoming_fracs,
   uint32 reverse_orderbit,   /* Low bit on mean treat as if we mean "do the last M/N". */
   long_boolean allow_improper,
   long_boolean *improper_p);

extern void get_fraction_info(
   uint32 frac_flags,
   uint32 callflags1,
   int total,
   fraction_info *zzz);

extern long_boolean fill_active_phantoms_and_move(setup *ss, setup *result);

extern void move_perhaps_with_active_phantoms(setup *ss, setup *result);

extern void impose_assumption_and_move(setup *ss, setup *result);

extern void move(
   setup *ss,
   long_boolean qtfudged,
   setup *result);

/* In SDISTORT */

extern void prepare_for_call_in_series(setup *result, setup *ss);

extern void minimize_splitting_info(setup *ss, uint32 other_info);

extern void initialize_map_tables(void);

extern void remove_z_distortion(setup *ss);

extern void new_divided_setup_move(
   setup *ss,
   uint32 map_encoding,
   phantest_kind phancontrol,
   long_boolean recompute_id,
   setup *result);

extern void divided_setup_move(
   setup *ss,
   Const map_thing *maps,
   phantest_kind phancontrol,
   long_boolean recompute_id,
   setup *result);

extern void new_overlapped_setup_move(setup *ss, uint32 map_encoding,
   uint32 *masks, setup *result);

extern void overlapped_setup_move(setup *ss, Const map_thing *maps,
   uint32 *masks, setup *result);

extern void do_phantom_2x4_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void do_phantom_stag_qtg_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void do_phantom_diag_qtg_concept(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void phantom_2x4_move(
   setup *ss,
   int lineflag,
   phantest_kind phantest,
   Const map_thing *maps,
   setup *result);

extern void distorted_2x2s_move(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void distorted_move(
   setup *ss,
   parse_block *parseptr,
   disttest_kind disttest,
   setup *result);

extern void triple_twin_move(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void do_concept_rigger(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void common_spot_move(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void triangle_move(
   setup *ss,
   parse_block *parseptr,
   setup *result);

/* In SDCONCPT */

extern long_boolean do_big_concept(
   setup *ss,
   setup *result);

/* In SDTAND */

#define tandem_key_tand 0
#define tandem_key_cpls 1
#define tandem_key_siam 2
#define tandem_key_tand3 4
#define tandem_key_cpls3 5
#define tandem_key_siam3 6
#define tandem_key_tand4 8
#define tandem_key_cpls4 9
#define tandem_key_siam4 10
#define tandem_key_box 16
#define tandem_key_diamond 17
#define tandem_key_skew 18
#define tandem_key_outpoint_tgls 20
#define tandem_key_inpoint_tgls 21
#define tandem_key_inside_tgls 22
#define tandem_key_outside_tgls 23
#define tandem_key_wave_tgls 26
#define tandem_key_tand_tgls 27
#define tandem_key_anyone_tgls 30
#define tandem_key_3x1tgls 31
#define tandem_key_ys 32

extern void tandem_couples_move(
   setup *ss,
   selector_kind selector,
   int twosome,           /* solid=0 / twosome=1 / solid-to-twosome=2 / twosome-to-solid=3 */
   int fraction,          /* number, if doing fractional twosome/solid */
   int phantom,           /* normal=0 phantom=1 general-gruesome=2 gruesome-with-wave-check=3 */
   int key,               /* tandem of 2 = 0 / couples of 2 = 1 / siamese of 2 = 2
                             tandem of 3 = 4 / couples of 3 = 5 / siamese of 3 = 6
                             tandem of 4 = 8 / couples of 4 = 9 / siamese of 4 = 10
                             10: box
                             11: diamond
                          */
   uint32 mxn_bits,
   long_boolean phantom_pairing_ok,
   setup *result);

extern void initialize_tandem_tables(void);

/* In SDCONC */

extern void concentric_move(
   setup *ss,
   setup_command *cmdin,
   setup_command *cmdout,
   calldef_schema analyzer,
   uint32 modifiersin1,
   uint32 modifiersout1,
   long_boolean recompute_id,
   setup *result);

extern uint32 get_multiple_parallel_resultflags(setup outer_inners[], int number);

extern void initialize_conc_tables(void);

extern void normalize_concentric(
   calldef_schema synthesizer,
   int center_arity,
   setup outer_inners[],   /* outers in position 0, inners follow */
   int outer_elongation,
   setup *result);

extern void merge_setups(setup *ss, merge_action action, setup *result);

extern void on_your_own_move(
   setup *ss,
   parse_block *parseptr,
   setup *result);

extern void punt_centers_use_concept(setup *ss, setup *result);

extern void selective_move(
   setup *ss,
   parse_block *parseptr,
   selective_key indicator,
   long_boolean others,
   int arg2,
   uint32 override_selector,
   selector_kind selector_to_use,
   long_boolean concentric_rules,
   setup *result);

extern void inner_selective_move(
   setup *ss,
   setup_command *cmd1,
   setup_command *cmd2,
   selective_key indicator,
   long_boolean others,
   int arg2,
   uint32 override_selector,
   selector_kind selector_to_use,
   uint32 modsa1,
   uint32 modsb1,
   setup *result);

/* In SDTOP */

extern void compress_setup(expand_thing *thing, setup *stuff);

extern void expand_setup(expand_thing *thing, setup *stuff);

extern void update_id_bits(setup *ss);

extern void initialize_touch_tables(void);

extern void touch_or_rear_back(
   setup *scopy,
   long_boolean did_mirror,
   int callflags1);

extern void do_matrix_expansion(
   setup *ss,
   uint32 concprops,
   long_boolean recompute_id);

extern void normalize_setup(setup *ss, normalize_action action);

extern void toplevelmove(void);

extern void finish_toplevelmove(void);
