/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : red_envelope_2006.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-01-28
 * Note   : 丙戌狗年紅包
 * Update :
 *  o 2003-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <daemon.h>

inherit STANDARD_OBJECT;

mapping actions;

string special_awards = "/obj/pasture/treasure/fly_horse";

string *awards =
({
	"/obj/pasture/treasure/pack_animal",
	"/obj/fishfarm/treasure/integrity_stone",
	"/obj/farming/treasure/blood_orchid",
	"/obj/book/skill/leadership-advance_c",
	"/obj/book/skill/architectonic-large_c"
});

void do_open_envelope(object me, string arg)
{
	object award;
	int money = range_random(10000, 10000000);
	string msg;
	
	msg = HIY"$RW "+NUMBER_D->number_symbol(money)+NOR;

	me->earn_money("RW", money);

	if( !random(200) )
	{
		award = new(special_awards);
		
		msg += " 和最大獎"+award->query_idname();
		
		award->set_keep();
		award->move(me);
	}
	else if( !random(30) )
	{
		award = new(awards[random(sizeof(awards))]);

		msg += " 和"+award->query_idname();
		
		award->set_keep();
		award->move(me);
	}

	me->add_title(HIR"又老了一歲"NOR);
	
	award = new("/obj/festival/red_envelope_2006_no_function");
	
	award->set_keep();
	award->move(me);
	
	CHANNEL_D->channel_broadcast("news", me->query_idname()+"打開"+this_object()->query_idname()+"，得到 "+msg+"。");
	
	destruct(this_object());
	
	me->save();
}

void create()
{
	set_idname(HIR"red envelope 2006"NOR, HIY"丙戌"NOR YEL"狗年"HIR"紅"NOR RED"包"NOR);
	
	if( this_object()->set_shadow_database() ) return;
	
	set("unit", "個");
	set("long", "2006 丙戌狗年紅包");
	set("mass", 100);
	set("value", 100);
	set("flag/no_amount", 1);

	actions = ([ "open_envelope" : (: do_open_envelope :) ]);
}
