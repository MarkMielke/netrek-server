/* $Id: transwarp.c,v 1.1 2005/03/21 05:23:44 jerub Exp $
 * transwarp.c by isae@IASTATE.EDU
 */
#include "copyright.h"

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "defs.h"
#include "struct.h"
#include "data.h"
#include "packets.h"
#include "proto.h"

#define MIN_INITIAL_SPEED 2
#define TRANSWARP_SPEED 60

#ifdef SB_TRANSWARP

int handleTranswarp(void)
{
   struct player  *j;
   int		  maxspeed;

   if (!twarpMode) {
      new_warning(UNDEF, "Sorry, transwarp mode is not active during this hour.", -1);
      return (0);
   }
   if (me->p_status != PALIVE)
      return (0);
   if (!me->p_candock) {
      new_warning(UNDEF, "Starbase refuses docking from us in particular, captain!", -1);
      return (0);
   }
   if (me->p_flags & PFENG) {
      new_warning(UNDEF, "Engine temperature is too high to initiate transwarp!", -1);
      return (0);
   }
   if (me->p_ship.s_type == STARBASE) {
      new_warning(UNDEF, "Starbases are not allowed to transwarp, captain!", -1);
      return (0);
   }
   j = &players[me->p_playerl];
   if (!(me->p_flags & PFPLOCK) ||
       ((me->p_flags & PFPLOCK) && (j->p_ship.s_type != STARBASE))) {
      new_warning(UNDEF, "You're not locked on to a starbase!", -1);
      return (0);
   }
   if (!(me->p_flags & PFGREEN)) {
	new_warning(UNDEF, "OSHA safety regulations prohibit transwarp near enemy ships.", -1);
	return (0);
   }
   if (j->p_status != PALIVE) {
      new_warning(UNDEF, "The starbase has been destroyed!", -1);
      return (0);
   }
   if (!((!(j->p_war & me->p_team)) &&
	 (!(me->p_war & j->p_team)))) {
      new_warning(UNDEF, "Transwarp request rejected by battle computers, captain!", -1);
      return (0);
   }
   if (!(j->p_flags & PFDOCKOK)) {
      new_warning(UNDEF, "Starbase refusing all docking permission captain!", -1);
      return (0);
   }
   if (!(j->p_transwarp & (j->p_flags ^ PFSHIELD))) {
      char *reason = "";
      switch (j->p_transwarp) {
      case PFGREEN: 
         reason = "Starbase refusing transwarp, in red or yellow alert"; break;
      case PFYELLOW|PFGREEN:
         reason = "Starbase refusing transwarp, in red alert"; break;
      case PFSHIELD:
         reason = "Starbase refusing transwarp, her shields are up"; break;
      default: 
         reason = "Starbase refusing transwarp, captain!"; break;
      }
      new_warning(UNDEF, reason, -1);
      return (0);
   }
   if (me->p_speed > MIN_INITIAL_SPEED) {
      new_warning(UNDEF, "We cannot exceed warp 2 to initiate transwarp, captain!", -1);
      return (0);
   }
   if (me->p_flags & PFREPAIR) {
      new_warning(UNDEF, "Vessel under repair, cannot initiate transwarp!", -1);
      return (0);
   }
   if (me->p_flags & PFCLOAK) {
      new_warning(UNDEF, "We're not allowed to transwarp while cloaked, captain!", -1);
      return (0);
   }
   if (me->p_flags & PFORBIT) {
      new_warning(UNDEF, "We can't transwarp while orbiting, captain!", -1);
      return (0);
   }
   if (me->p_damage > (int) (me->p_ship.s_maxdamage / 3)) {
      new_warning(UNDEF, "Our ship's condition makes it impossible to tranwswarp, captain!", -1);
      return (0);
   }
   if (me->p_fuel < (int) (me->p_ship.s_maxfuel / 2)) {
      new_warning(UNDEF, "Too low on fuel, cannot initiate transwarp!", -1);
      return (0);
   }
   new_warning(UNDEF, "Transwarp initiated, all systems are DOWN meanwhile!", -1);

   /* This will undock the ship and turn off stuff that it might be doing. */
   set_speed(1);

   me->p_flags &= ~(PFPLOCK|PFPLLOCK|PFDOCK);
   me->p_flags |= (PFSHIELD | PFTWARP | PFPLOCK);
   /*
   me->p_desspeed = (twarpSpeed) ? twarpSpeed : TRANSWARP_SPEED;
   */

   me->p_desspeed = 5 * me->p_ship.s_maxspeed;
   maxspeed =  5 * ((me->p_ship.s_maxspeed + 2) -
	       (int)( (me->p_ship.s_maxspeed + 1) *
               ((float) me->p_damage / (float) (me->p_ship.s_maxdamage))));
   if(me->p_desspeed > maxspeed)
      me->p_desspeed = maxspeed;

   if (me->p_speed == 0)
      me->p_speed++;
   
   return (1);
}

#endif

