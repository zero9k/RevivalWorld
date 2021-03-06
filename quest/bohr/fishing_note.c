/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : Questitem.c
 * Author : 
 * Date   : 2005-01-05
 * Note   : 釣魚筆記
 * Update :
 *  o 2003-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <condition.h>

inherit STANDARD_OBJECT;

mapping actions;

void do_readnote(object me, string arg)
{	
	if( me->in_condition(FISHINGNOTE) )
		return tell(me, pnoun(2, me)+"還在同樣的加持狀態中。\n");

 	msg("$ME開始讀起"+this_object()->query_idname()+"，只見$ME非常贊同似的不斷的點頭稱讚，似乎從裡頭得到了很多的東西。\n", me, 0, 1);
	
	me->start_condition(FISHINGNOTE);

	tell(me, pnoun(2, me)+"獲得了一些釣魚技能經驗值。\n");
	me->add_skill_exp("fishing", 1000);
}

void create()
{
	set_idname(HIW"f"WHT"ishing "HIW"n"WHT"ote",HIW"釣魚"WHT"筆記"NOR);
	
	if( this_object()->set_shadow_database() ) return;
	
	set("unit", "本");
	set("mass", 150);
	set("value", 40000);
	set("badsell", 1);

	actions = ([ "readnote" : (: do_readnote :) ]);

}
