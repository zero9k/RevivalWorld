/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : beauty_hibiscus.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-12-19
 * Note   : 美芙蓉
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
	set_idname(HIR"beauty "NOR RED"hibiscus"NOR, HIR"美"NOR RED"芙蓉"NOR);
	set_temp("status", HIC"稀"NOR CYN"有");

	if( this_object()->set_shadow_ob() ) return;

	set("long", "只有在絕對純淨的水中才能生長的美芙蓉，幾近絕種因此成為珍寶。」\n");
	set("unit", "株");
	set("mass", 0);
	set("value", 8000000);
	set("flag/no_amount", 1);
	set(BUFF_CHA, 36);
	set(BUFF_SOCIAL_EXP_BONUS, 15);
	set("buff/status", HIR"美"NOR RED"芙蓉"NOR);
	
	::setup_equipment(EQ_BROOCH, HIG"胸"NOR GRN"針"NOR);
}
