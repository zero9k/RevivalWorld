/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : integrity_stone.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-09
 * Note   : 正氣石
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <feature.h>
#include <equipment.h>
#include <buff.h>

inherit STANDARD_OBJECT;
inherit EQUIPMENT;

void create()
{
	set_idname(HIW"integrity "NOR WHT"stone"NOR, HIW"正氣"NOR WHT"石"NOR);
	set_temp("status", HIC"稀"NOR CYN"有");

	if( this_object()->set_shadow_ob() ) return;
	
	set("long", "一顆透明的石頭，石頭中央深處似乎有一些深邃的顏色在不停地流動著。\n");
	set("unit", "顆");
	set("mass", 0);
	set("value", 3000000);
	set("flag/no_amount", 1);
	set(BUFF_INT, 20);
	set("buff/status", HIW"正氣"NOR WHT"石"NOR);
	
	::setup_equipment(EQ_EYE, HIG"眼"NOR GRN"睛"NOR);
}
