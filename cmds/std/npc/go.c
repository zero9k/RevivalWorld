/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : go.c
 * Author : Cookys@RevivalWorld
 * Date   : 2001-7-20
 * Note   : 移動指令。
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <lock.h>
#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <location.h>
#include <message.h>
#include <delay.h>
#include <buff.h>
#include <citydata.h>

inherit COMMAND;

string help = @HELP
移動指令

這個指令可以讓您在重生的世界中自由的行走，到您想去的地方
通常可以行走的方向如下:

north		北	northeast	東北
south		南	northwest	西北
east		東	southeast	東南
west		西	southwest	西南
up		上	down		下

以上這些方向的指令在系統中都有預設的別名 (請參考 alias -d)
為了方便使用，您只需要輸入方向即可，如: n, s, e, w, ne, nw, se, sw, u, d
在某些區域可能會出現例外的方向，您只要輸入該方向的全名即可。

在建築物中則可利用 go '樓層數' 直達該樓層

例如 : go 100			- 直達 100F

相關指令: look
HELP;

mapping corresponding =
([
    "north"         :"south",
    "south"         :"north",
    "east"          :"west",
    "west"          :"east",
    "northwest"     :"southeast",
    "northeast"     :"southwest",
    "southwest"     :"northeast",
    "southeast"     :"northwest",
    "up"            :"down",
    "down"          :"up",
]);

private void do_command(object me, string arg)
{
	mixed exit;
	mapping exits;
	string my_idname, from;
	object env, newenv;
	int moving_stamina_cost;

	if( !arg )
		return tell(me, "你想要往那裡走？\n");
	
	// 是犯人
	if( query("prisoner", me) )
	{
		msg("$ME的手腳被鐵鍊銬住，無法自由行動。\n", me, 0, 1);
		return;
	}
		
	//忙碌中不能下指令
	if( me->is_delaying() )
	{
		tell(me, me->query_delay_msg());
		return me->show_prompt();
	}

	env = environment(me);

	if( !objectp(env) ) 
		return tell(me, pnoun(2, me)+"目前無法向任何位置移動，請通知巫師處理。\n");

	my_idname = me->query_idname();

	if( env->is_maproom() )
	{
		switch(MAP_D->valid_move(me, arg))
		{
			case 1:
			{
				moving_stamina_cost = -me->query_all_buff(BUFF_MOVING_STAMINA);
				
				if( moving_stamina_cost > 0 )
				{
					if( !me->cost_stamina(moving_stamina_cost) )
						return tell(me, pnoun(2, me)+"已經沒有體力繼續前進了。\n");
					else
						msg("$ME耗費了 "+moving_stamina_cost+" 點的體力奮力往前行進。\n", me, 0, 1);
				}
				
				if( me->query_loading() > me->query_max_loading() )
				{
					msg("$ME拖著沉重的身體前進，但終究是力不從心。\n", me, 0, 1);
					me->faint();
					return;
				}
				MAP_D->move(me, arg);
				
				return;
				break;
			}
			case -1:
			{
				if( wizardp(me))
					return MAP_D->move(me, arg);
				else
					return tell(me, "往"+CHINESE_D->to_chinese(arg)+"方向的門被鎖住了，"+pnoun(2, me)+"沒辦法往那走。\n");
					
				break;
			}
			case -2:
			{
				if( query_temp("go_resist", me) == "WALL_"+arg )
					msg("$ME明明知道"+CHINESE_D->to_chinese(arg)+"方是牆，卻依然從正面撞了下去，把嘴唇撞的跟香腸一樣。\n", me, 0, 1);
				else
				{
					set_temp("go_resist", "WALL_"+arg, me);
					tell(me, "往"+CHINESE_D->to_chinese(arg)+"方向是一塊牆壁，"+pnoun(2, me)+"沒辦法往那走。\n");
				}
				return;
				break;
			}
			default:
			{
				return tell(me, "往 "+arg+" 的方向沒有出路。\n");
				break;
			}
		}
	}

	// 一般移動
	if( !mapp(exits = query("exits", env)) || !sizeof(exits) )
		return tell(me, "這裡四周都沒有出口，"+pnoun(2, me)+"想走到哪裡去？\n");

	if( exit = exits[arg] )
	{
		if( me->is_fighting() )
		{
			if( !COMBAT_D->do_flee(me) )
				return;
		}

		from = CHINESE_D->to_chinese(corresponding[arg]);

		if( functionp(exit) )
			exit = evaluate(exit, me);

		// 若是座標出口
		if( arrayp(exit) )
		{
			if( (MAP_D->query_map_system(exit))->query_coor_data(exit, "lock") & LOCKED && !wizardp(me))
				return tell(me, "往 "+capitalize(arg)+" 方向的門被鎖住了，"+pnoun(2, me)+"沒辦法往那走。\n");

			if( !wizardp(me) && (MAP_D->query_map_system(exit))->query_coor_data(exit, "lock") & WIZLOCKED )
				return tell(me, "往 "+capitalize(arg)+" 方向有一道由天而降的光牆擋住，"+pnoun(2, me)+"沒辦法往那走。\n");

			broadcast(env, my_idname+"往"+(CHINESE_D->to_chinese(arg)||" "+capitalize(arg)+NOR" 方向")+"的門口走了出去。\n", 0, me);

			me->move(exit);

			if( from )
				broadcast(exit, my_idname+"從"+from+"的門裡走了出來。\n", 0, me);
			else
				broadcast(exit, my_idname+"走了過來。\n", 0, me);

			me->follower_move(env, exit);
		}
		// 若是物件名稱出口
		else if( stringp(exit) )
		{
			if( query("lock/"+arg, env) & LOCKED && !wizardp(me))                       
				return tell(me, "往 "+capitalize(arg)+" 方向的門被鎖住了，"+pnoun(2, me)+"沒辦法往那走。\n");

			if( !wizardp(me) && query("lock/"+arg, env) & WIZLOCKED )
				return tell(me, "往 "+capitalize(arg)+" 方向有一道由天而降的光牆擋住，"+pnoun(2, me)+"沒辦法往那走。\n");

			broadcast(env, my_idname+"往"+(CHINESE_D->to_chinese(arg)||" "+capitalize(arg)+NOR" 方向")+"離開。\n", 0, me);

			me->move(exit);

			if( from )
				broadcast(exit, my_idname+"從"+from+"走了過來。\n", 0, me);
			else
				broadcast(exit, my_idname+"走了過來。\n", 0, me);

			me->follower_move(env, exit);
		}
		else
			tell(me, "這個方向的出口被封鎖住了，無法通過。\n");
	}
	else if( to_int(arg) > 0 && big_number_check(arg) )
	{
		int current_floor = env->query_floor()-1;
		int floor = to_int(arg)-1;
		string msg = "";
		array loc = env->query_location();

		if( current_floor == floor )
			return tell(me, pnoun(2, me)+"已經是在 "+(floor+1)+" 樓了。\n");

		if( !arrayp(loc) ) return tell(me, "這裡沒有 "+arg+" 樓。\n");

		// 快速上樓
		if( current_floor < floor )
		{
			newenv = environment(me);

			for(current_floor = env->query_floor();current_floor<=floor;current_floor++)
			{
				exit = CITY_NUM_ROOM(loc[CITY], loc[NUM]) + "/"+loc[X]+"_"+loc[Y]+"_"+(current_floor?current_floor+"_":"")+env->query_building_type()+".o";

				if( !file_exists(exit) )
					return tell(me, "這裡沒有 "+arg+" 樓。\n");

				if( query("lock/up", newenv) & LOCKED )
					return tell(me, "往第 "+(current_floor+1)+" 樓的通道被鎖住了。\n");

				newenv = load_object(exit[0..<3]);
			}
			
			//for(current_floor = env->query_floor();current_floor<=floor;current_floor++)		
			msg += pnoun(2, me)+"搭乘電梯往第 "WHT+(floor+1)+NOR" 樓上去。\n";

			tell(me, msg);
			me->move(newenv);
			me->follower_move(env, newenv);
		}
		// 快速下樓
		else
		{
			newenv = environment(me);

			for(current_floor = env->query_floor()-2;current_floor>=floor;current_floor--)
			{	
				exit = CITY_NUM_ROOM(loc[CITY], loc[NUM]) + "/"+loc[X]+"_"+loc[Y]+"_"+(current_floor?current_floor+"_":"")+env->query_building_type()+".o";
					
				if( !file_exists(exit) )
					return tell(me, "這裡沒有 "+arg+" 樓。\n");

				if( query("lock/down", newenv) & LOCKED )
					return tell(me, "往第 "+(current_floor+1)+" 樓的通道被鎖住了。\n");
					
				newenv = load_object(exit[0..<3]);
			}

			//for(current_floor = env->query_floor()-2;current_floor>=floor;current_floor--)
				msg += pnoun(2, me)+"搭乘電梯往第 "WHT+(floor+1)+NOR" 樓下去。\n";
			
			tell(me, msg);
			me->move(newenv);
			me->follower_move(env, newenv);
		}

		msg("$ME從電梯裡走了出來。\n", me, 0, 1);
	}
	else
		tell(me, "往 "+arg+" 的方向沒有出路。\n");
}

