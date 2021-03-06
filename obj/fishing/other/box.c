/* This	program	is a part of RW	mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File	  : box.c
 * Author :
 * Date	  : 2003-5-31
 * Note	  : 寶箱
 * Update :
 *  o 2000-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <message.h>
#include <inherit.h>
#include <daemon.h>

inherit	STANDARD_OBJECT;

mapping	actions;

void do_open(object me,	string arg, object box)
{
	int i;
	object ob, env;
	array loc = query_temp("location", box);

	env = environment(box);
	
	if( !box->id(arg) )
		return tell(me,	pnoun(2, me)+"要打開什麼東西？\n", CMDMSG);

	if( arrayp(loc) && env && env->is_maproom() )
	{
		string owner = CITY_D->query_coor_data(loc, "owner");
		
		if( owner && owner[0..9] != "GOVERNMENT" && owner != me->query_id(1) && member_array(find_player(owner), present_objects(me)) != -1 )
			return msg("$YOU在一旁冷冷地看著$ME，$ME不敢隨意地開啟地上的寶箱。\n", me, find_player(owner), 1);
	}
		
	switch(	random(21) ) {
		case 0..8:
			i = range_random(20, 50);
			ob = new("/obj/fishing/fish/fish"+random(4));
			set_temp("amount", i, ob);
			msg("$ME打開寶箱一看，發現裡面裝著"+ob->short(1)+"。\n",	me, 0, 1, CMDMSG);
			break;
		case 9..15:
			i = range_random(20, 50);
			ob = new("/obj/fishing/adv_fish/fish"+random(4));
			set_temp("amount", i, ob);
			msg("$ME打開寶箱一看，發現裡面裝著"+ob->short(1)+"。\n",	me, 0, 1, CMDMSG);
			break;
		case 16..19:
			i = range_random(20, 50);
			ob = new("/obj/fishing/sp_fish/fish"+random(3));
			set_temp("amount", i, ob);
			msg("$ME打開寶箱一看，發現裡面裝著"+ob->short(1)+"。\n",	me, 0, 1, CMDMSG);
			break;
		case 20:
			i = range_random(10, 30);
			ob = new("/obj/gem/gem"+random(7));
			set_temp("amount", i, ob);
			msg("$ME打開寶箱一看，發現裡面裝著"+ob->short(1)+"。\n",	me, 0, 1, CMDMSG);
			break;
	}

	if( objectp(ob)	&& !me->get_object(ob)	)
	{
		msg("$ME拿不動那麼多東西，只好先把"+ob->short(1)+"放在地上了。\n", me, 0, 1, CMDMSG);
		
		if( query_temp("location", me) )
			ob->move(copy(query_temp("location", me)));
		else
			ob->move(environment(me));
	}
	destruct(this_object());
}
void create()
{
	set_idname("treasure chest", HIY"寶箱"NOR);

	actions	= ([ "open" : (: do_open :) ]);

	if( this_object()->set_shadow_ob() ) return;
	
	set("unit", "個");
	set("value", 5000);
	set("mass", 500);
	set("flag/no_amount", 1);
}
