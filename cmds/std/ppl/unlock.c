/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : unlock.c
 * Author : 
 * Date   : 2003-05-03
 * Note   : 解鎖指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <map.h>
#include <lock.h>
#include <feature.h>
#include <message.h>
#include <daemon.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
這指令可讓你解除門鎖。

unlock '方向'		- 解除某個方向的門鎖
(north, south, east, west, northeast, northwest, southeast, southwest, up, down)
HELP;

private void do_command(object me, string arg)
{
	int unlocktype;
	string option;
        mapping exits;
        mapping a = ([
        "west"  : "east",
        "east"  : "west",
        "north" : "south",
        "south" : "north",
        "northeast" : "southwest",
        "northwest" : "southeast",
        "southeast" : "northwest",
        "southwest" : "northeast",
        "up"    : "down",
        "down"  : "up"
        ]);
        object env, to;
        
        env = environment(me);
	
	if( !arg )
		return tell(me, pnoun(2, me)+"想要解哪一個方向的鎖？\n");

	switch(option)
	{
		case "wiz":
			if( wizardp(me) )
				unlocktype = WIZLOCKED;
			else
				unlocktype = LOCKED;
			break;
		default:
			unlocktype = LOCKED;
	}
	
	if( !env->is_maproom() )
	{
	        if( !wizardp(me) && query("owner", env) != me->query_id(1) )
	                return tell(me, "這裡並不是你的建築物，無法解鎖。\n");
	
	        if( !mapp(exits = query("exits", env)) )
	                return tell(me, "這裡並沒有任何鎖住的門，要怎麼解鎖呢？\n");
	
	        if( !exits[arg] )
	                return tell(me, "那個方法並沒有門鎖，無法解鎖。\n");

	        if( arrayp(exits[arg]) )
	        {
	        	delete("lock/"+arg, env);
	        	CITY_D->delete_coor_data(exits[arg], "lock");
	        		
	        	env->save();
	        }
	        else
	        {
	                to = load_object(exits[arg]);
	                
	        	delete("lock/"+arg, env);
	                delete("lock/"+a[arg], to);

	                broadcast(to, "似乎有人把 "+a[arg] +"方向的門鎖打開了。\n");
	                
	                env->save();
	                to->save();
	        }
	        
	        tell(me, pnoun(2, me)+"把往 "+capitalize(arg)+" 的方向的門鎖打開。\n");
	}
	else
	{
		int x, y, num;
		string city;
		array loc = query_temp("location", me);
		
		x = loc[X];
		y = loc[Y];
		city = loc[CITY];
		num = loc[NUM];

		if( !CITY_D->is_city_location(loc) )
			return tell(me, "這附近沒有門可以讓"+pnoun(2, me)+"上鎖。\n");

		switch(arg)
		{
			case "north":
				loc = arrange_city_location(x, --y, city, num);
				break;
			case "south":
				loc = arrange_city_location(x, ++y, city, num);
				break;
			case "east":
				loc = arrange_city_location(++x, y, city, num);
				break;
			case "west":
				loc = arrange_city_location(--x, y, city, num);
				break;
			case "northeast":
				loc = arrange_city_location(++x, --y, city, num);
				break;
			case "northwest":
				loc = arrange_city_location(--x, --y, city, num);
				break;
			case "southeast":
				loc = arrange_city_location(++x, ++y, city, num);
				break;
			case "southwest":
				loc = arrange_city_location(--x, ++y, city, num);
				break;
			default:
				return tell(me, pnoun(2, me)+"想要解哪一個方向的鎖？\n");
		}

		if( !CITY_D->valid_coordinate(x, y, city, num) )
			return tell(me, "那個方向是不合理的座標。\n");

		if( !wizardp(me) && CITY_D->query_coor_data(loc, "owner") != me->query_id(1) )
			return tell(me, "這裡並不是你的建築物，無法解鎖。\n");
		
		if( CITY_D->query_coor_data(loc, TYPE) != DOOR )
			return tell(me, "那裡並沒有門，無法解鎖。\n");

        	CITY_D->delete_coor_data(loc, "lock");	        		
		tell(me, pnoun(2, me)+"把往 "+capitalize(arg)+" 的方向的門鎖打開。\n");
	}
}
