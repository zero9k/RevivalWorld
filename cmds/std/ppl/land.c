/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : land.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-03-08
 * Note   : land 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <daemon.h>
#include <map.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
    土地精華吸納。

land			- 查詢目前已經吸納的土地精華
land absorb		- 吸納土地精華(需要特殊物品)
land release '數字'	- 將土地精華列表中第 N 個吸收的土地精華釋放到目前所在的土地

HELP;

private void do_command(object me, string arg)
{
	int i;

	if( !arg )
	{
		string list = "";
		array landdata = query("landdata", me);
		
		if( arrayp(landdata) && sizeof(landdata) )
		{
			list += HIY"土地"NOR YEL"精華"NOR"\n";
			foreach(array elem in landdata)
			{
				i++;
				switch(elem[0])
				{
					case FARM: 
						list += sprintf("%-3d%-20s "NOR GRN"土地等級 "HIG"%s"NOR GRN"/%d\n"NOR, i, HIG"農田"NOR, repeat_string("*", elem[1]), elem[2]);
						break;
					case PASTURE: 
						list += sprintf("%-3d%-20s "NOR YEL"土地等級 "HIY"%s"NOR YEL"/%d\n"NOR, i, NOR YEL"牧場"NOR, repeat_string("*", elem[1]), elem[2]);
						break;
					case FISHFARM: 
						list += sprintf("%-3d%-20s "NOR BLU"土地等級 "HIB"%s"NOR BLU"/%d\n"NOR, i, HIB"養殖場"NOR, repeat_string("*", elem[1]), elem[2]);
						break;
				}
			}
			return tell(me, list+"\n");
		}
		else
			return tell(me, "目前沒有任何土地吸納精華。\n");
	}
	else if( arg == "absorb" )
	{
		int type;
		int growth_level;
		int maintain_time;
		
		object media;
		object env = environment(me);
		array loc = query_temp("location", me);
		array landdata = query("landdata", me) || allocate(0);
		
		if( !env->is_maproom() || !arrayp(loc) || !CITY_D->city_exist(loc[CITY], loc[NUM]) )
			return tell(me, "無法吸納這塊土地的精華。\n");
		
		if( CITY_D->query_coor_data(loc, "owner") != me->query_id(1) )
			return tell(me, "這塊土地並不是"+pnoun(2, me)+"的。\n");
			
		foreach(object ob in all_inventory(me))
		{
			if( query("land_absorb", ob) )	
			{
				media = ob;
				break;
			}
		}
		
		if( !objectp(media) )
			return tell(me, pnoun(2, me)+"身上缺乏吸納土地精華的媒介物品。\n");
		
		type = CITY_D->query_coor_data(loc, TYPE);
		growth_level = CITY_D->query_coor_data(loc, "growth_level");
		maintain_time = CITY_D->query_coor_data(loc, "maintain_time");
	
		if( type != FARM && type != FISHFARM && type != PASTURE )
			return tell(me, "無法吸納這種土地的精華。\n");
	
		if( maintain_time < 1 )
			return tell(me, "這塊土地沒有任何精華可以吸收。\n");

		msg("$ME將$YOU放置在土地正中央，只見$YOU發出刺眼的"HIW"白光"NOR"將這塊土地的精華完全吸納起來。\n", me, media, 1);
		msg("隨著白光逐漸散去，$YOU也跟著消失了。\n", me, media, 1);
		destruct(media, 1);

		CITY_D->delete_coor_data(loc, "growth_level");
		CITY_D->delete_coor_data(loc, "maintain_time");
		CITY_D->delete_coor_data(loc, "growth");
		CITY_D->delete_coor_data(loc, "status");
		TOP_D->delete_top("maintain", save_variable(loc));

		landdata += ({ ({ type, growth_level, maintain_time }) });
		set("landdata", landdata, me);
		me->save();
	}
	else if( sscanf(arg, "release %d", i) == 1 )
	{
		object env = environment(me);
		array landdata = query("landdata", me);
		array loc = query_temp("location", me);
		array elem;
		
		if( !env->is_maproom() || !arrayp(loc) || !CITY_D->city_exist(loc[CITY], loc[NUM]) )
			return tell(me, "無法將土地精華釋放到這塊土地上。\n");

		if( CITY_D->query_coor_data(loc, "owner") != me->query_id(1) )
			return tell(me, "這塊土地並不是"+pnoun(2, me)+"的。\n");
			
		if( !arrayp(landdata) )
			return tell(me, "目前沒有任何的土地精華。\n");

		if( i < 1 || i > sizeof(landdata) )
			return tell(me, "請輸入正確的數字。\n");

		elem = landdata[i-1];
		
		if( CITY_D->query_coor_data(loc, TYPE) != elem[0] )
			return tell(me, "無法將這種類型的土地精華釋放到這塊土地上。\n");
			
		if( CITY_D->query_coor_data(loc, "maintain_time") )
			return tell(me, "這塊土地已經有現存的土地精華，無法再將土地精華釋放到這塊土地上。\n");

		CITY_D->set_coor_data(loc, "growth_level", elem[1]);
		CITY_D->set_coor_data(loc, "maintain_time", elem[2]);
		GROWTH_D->set_status(loc);
		
		msg("$ME將大量的土地精華釋放到這塊土地上，只見土地上泛起一陣"HIW"白晢光芒"NOR"。\n", me, 0, 1);
		TOP_D->update_top("maintain", save_variable(loc), elem[1], me->query_idname(), elem[0]);

		landdata = landdata[0..i-2] + landdata[i..<1];
		
		if( !sizeof(landdata) )
			delete("landdata", me);
		else
			set("landdata", landdata, me);
		
		me->save();
	}
	else return tell(me, "請輸入正確的指令參數(help land)。\n");
}
