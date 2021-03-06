/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : water.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-01-06
 * Note   : 清水
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <material.h>
#include <ansi.h>
#include <inherit.h>

inherit STANDARD_OBJECT;

void create()
{
	set_idname("water",HIC"清水"NOR);

	if( this_object()->set_shadow_ob() ) return;
	
	set("material/"+WATER, 1);
	set("unit","單位");
	set("mass", 1000);
	set("value", 10);
}