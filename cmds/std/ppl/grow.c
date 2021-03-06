/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : grow.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-04
 * Note   : 戶外種植/畜牧/養殖指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <daemon.h>
#include <message.h>
#include <map.h>
#include <material.h>
#include <skill.h>

inherit COMMAND;

#define GROW_STR_COST		20
#define HARVEST_STR_COST	400
#define MAINTAIN_STR_COST	300
#define DEFAULT_GROW		10

string help = @HELP
    用來生產農田 / 牧場 / 養殖場的指令

    grow [數量] [種子或種畜或魚苗]	- 進行播種/配種/釋放魚苗的動作
    grow [數量] [水或飼料]		- 進行灌溉或餵養
    grow harvest			- 收成指令
    grow maintain			- 整地指令，用來提升土地等級，減少天然災害損害與增加產量
    grow maintain '次數'		- 一次整地多次
    grow -d				- 取消目前的生產
    
HELP;

private void maintain_levelup(object me, array loc)
{
	int level = 0;

	switch( CITY_D->add_coor_data(loc, "maintain_time", 1) )
	{
		case 1:
			level = 1; break;
		case 50:
			level = 2; break;
		case 1000:
			level = 3; break;
		case 10000:
			level = 4; break;
		case 50000:
			level = 5; break;
		case 100000:
			level = 6; break;
		case 200000:
			level = 7; break;
		case 500000:
			level = 8; break;
		case 1000000:
			level = 9; break;
		case 2000000:
			level = 10; break;
	}
	
	if( level )
	{
		CITY_D->set_coor_data(loc, "growth_level", level);
		msg("此地的土地等級隨著$ME的辛勤照料提升為第 "HIY+level+NOR" 級。\n", me, 0, 1);
		
		if( !SECURE_D->is_wizard(me->query_id(1)) )
		TOP_D->update_top("maintain", save_variable(loc), CITY_D->query_coor_data(loc, "growth_level"), me->query_idname(), CITY_D->query_coor_data(loc, TYPE));
	}
}

private void do_command(object me, string arg)
{
	int amount, growth_level, multiplicand;
	array loc = query_temp("location", me);
	object ob;
	string msg="", file;
	mapping coor_data, growth_data;
	
	if( (MAP_D->query_map_system(loc)) != CITY_D )
		return tell(me, pnoun(2, me)+"無法在此進行生產。\n");
	
	coor_data = CITY_D->query_coor_data(loc);
	
	if( !mapp(coor_data) )
		return tell(me, pnoun(2, me)+"無法在此進行生產。\n");

	growth_data = coor_data["growth"];
	
	if( coor_data["owner"] != me->query_id(1) )
		return tell(me, "這塊土地不是"+pnoun(2, me)+"的。\n");
	
	if( !arg )
		return tell(me, pnoun(2, me)+"想要做什麼樣的生產動作？請 help grow。\n");

	if( arg == "-d" )
	{
		if( !mapp(growth_data) )
			return tell(me, "這裡沒有生產任何產品。\n");
			
		CITY_D->delete_coor_data(loc, "growth");
		GROWTH_D->set_status(loc);
		return tell(me, pnoun(2, me)+"取消了此地的產品生產。\n");
	}
		
	if( arg == "maintain" || sscanf(arg, "maintain %d", amount) == 1 )
	{
		int cost_str;

		switch(coor_data[TYPE])
		{
			case FARM:
			case PASTURE:
			case FISHFARM:
				break;
			default:
				return tell(me, pnoun(2, me)+"無法整這塊地。\n");
		}

		if( amount < 1 )
			amount = 1;

		if( mapp(growth_data) )
			return tell(me, "這裡正在生產產品，無法進行整地動作。\n");
			
		cost_str = to_int(MAINTAIN_STR_COST * pow(amount, 0.9));

		if( !me->cost_stamina(cost_str) )
			return tell(me, pnoun(2, me)+"的體力不足 "+cost_str+"，無法進行整地的動作。\n");

		msg("$ME耗費 "+cost_str+" 的體力辛勤的保養整地，使土地品質變得較為優良。("WHT"+"HIW+amount+NOR")\n", me, 0, 1);
		
		// 升級檢查
		for(int i = 0; i<amount ; ++i)
			maintain_levelup(me, loc);
		
		me->add_social_exp(amount * (10+random(30)));
		GROWTH_D->set_status(loc);

		return;
	}
	
	// 收成
	if( arg == "harvest" )
	{
		if( !mapp(growth_data) )
			return tell(me, "這裡沒有生產任何產品，無法進行收成動作。\n");
			
		if( !growth_data["wait_for_harvesting"] )
			return tell(me, "目前還無法進行收成的動作。\n");

		if( !me->cost_stamina(HARVEST_STR_COST) )
			return tell(me, pnoun(2, me)+"的體力不足夠進行收成的動作。\n");

		msg += growth_data["harvest_msg"];
		
		growth_level = CITY_D->query_coor_data(loc, "growth_level");

		foreach(file, amount in growth_data["harvest"])
		{
			ob = new(file);
			
			if( !objectp(ob) ) continue;
			
			multiplicand = growth_data["input_amount"] * growth_data["harvest_percent"] * (growth_level+DEFAULT_GROW) / 1000.;
			
			amount *= multiplicand;

			set_temp("amount", amount, ob);
		
			msg += "．"+ob->short(1)+"\n";
	
			me->add_social_exp(random(amount * 30));

			ob->move_to_environment(me);
		}
		
		msg(msg, me, 0, 1);
		
		// 升級檢查
		maintain_levelup(me, loc);

		TREASURE_D->get_treasure_growth(me, growth_level, coor_data[TYPE]);

		CITY_D->delete_coor_data(loc, "growth");
		CITY_D->delete_coor_data(loc, "status");
		GROWTH_D->set_status(loc);
		return;
	}
	
	if( sscanf(arg, "%d %s", amount, arg) != 2 )
		amount = 1;
	else if( amount < 1 )
		return tell(me, "請輸入正確的數量。\n");
	
	if( !objectp(ob = present(arg)) )
		return tell(me, pnoun(2, me)+"身上並沒有 "+arg+" 這種東西。\n");

	if( count(amount, ">", query_temp("amount", ob)||1) )
		return tell(me, pnoun(2, me)+"身上沒有那麼多的"+ob->query_idname()+"。\n");

	// 若此物品是種子/秧苗/種馬種牛/魚苗
	if( query("growth", ob) )
	{
		if( !mapp(growth_data) )
			growth_data = copy(query("growth", ob));
		else
			return tell(me, "這塊土地已經在生產"+growth_data["idname"]+"中。\n");

		if( growth_data["type"] != coor_data[TYPE] )
			return tell(me, ob->query_idname()+"無法在此進行生產。\n");

		if( growth_data["type"] == PASTURE && amount == 1 )
			return tell(me, "只有一隻"+ob->query_idname()+"要怎麼交配...？\n");

		if( amount > DEFAULT_GROW + coor_data["growth_level"] )
		{
			switch(growth_data["type"])
			{
				case FARM:
					return tell(me, "這塊農地最多一次只能耕作 "+(DEFAULT_GROW+coor_data["growth_level"])+" 個作物。\n");
					break;
				case PASTURE:
					return tell(me, "這片牧場最多一次只能飼養 "+(DEFAULT_GROW+coor_data["growth_level"])+" 隻動物。\n");
					break;
				case FISHFARM:
					return tell(me, "這處養殖場最多一次只能養殖 "+(DEFAULT_GROW+coor_data["growth_level"])+" 個水產。\n");
					break;
			}
		}
	
		foreach(string skill, int num in growth_data["skill"])
			if( me->query_skill_level(skill) < num )
				return tell(me, pnoun(2, me)+"的"+(SKILL(skill))->query_idname()+"技能不足以生產"+ob->query_idname()+"。(需求等級 Lv"+num+")\n");
		
		if( !me->cost_stamina(GROW_STR_COST * amount) )
			return tell(me, pnoun(2, me)+"的體力不夠了。\n");

		growth_data["input_amount"] = amount;
		growth_data["harvest_percent"] = 100;
		
		foreach(file, int num in growth_data["material"])
			growth_data["material"][file] *= amount;
		
		CITY_D->set_coor_data(loc, "growth", growth_data);
		GROWTH_D->set_status(loc, growth_data);

		switch(growth_data["type"])
		{
			case FARM:
				msg("$ME開始在這塊田地上耕作"+QUANTITY_D->obj(ob, amount)+"。\n"+growth_data["start_msg"], me, ob, 1);
				break;
			case PASTURE:
				msg("$ME開始在這片牧場上畜養"+QUANTITY_D->obj(ob, amount)+"。\n"+growth_data["start_msg"], me, ob, 1);
				break;
			case FISHFARM:
				msg("$ME開始在這處養殖場上養殖"+QUANTITY_D->obj(ob, amount)+"。\n"+growth_data["start_msg"], me, ob, 1);
				break;
		}
		
		
		destruct(ob, amount);
		
		return;
	}
	
	if( !mapp(growth_data) )
		return tell(me, ob->query_idname()+"無法用來進行生產。\n");

	if( mapp(growth_data["material"]) )
	switch(coor_data[TYPE])
	{
		case FARM:
		case PASTURE:
		case FISHFARM:
		{
			string basename = base_name(ob);
			
			if( !undefinedp(growth_data["material"][basename]) )
			{
				if( !me->cost_stamina(GROW_STR_COST * amount) )
					return tell(me, pnoun(2, me)+"的體力不夠了。\n");
				
				me->add_social_exp(random(amount * 50));
				
				growth_data["material"][basename] -= amount;
				
				if( growth_data["material"][basename] <= 0 )
					map_delete(growth_data["material"], basename);
					
				switch(growth_data["type"])
				{
					case FARM:
						msg("$ME利用"+QUANTITY_D->obj(ob, amount)+"對這塊農地進行灌溉。\n", me, ob, 1);
						break;
					case PASTURE:
						msg("$ME利用"+QUANTITY_D->obj(ob, amount)+"對這片牧場進行飼養。\n", me, ob, 1);
						break;
					case FISHFARM:
						msg("$ME利用"+QUANTITY_D->obj(ob, amount)+"對這處養殖場進行餵養。\n", me, ob, 1);
						break;
				}

				// 材料輸入完畢, 開始生長
				if( !sizeof(growth_data["material"]) )
				{
					map_delete(growth_data, "material");
					growth_data["current_growing"] = growth_data["start_msg"];
					msg(growth_data["start_msg"], me, 0, 1);
					growth_data["started"] = 1;
				}
			
				CITY_D->set_coor_data(loc, "growth", growth_data);
				GROWTH_D->set_status(loc, growth_data);
				destruct(ob, amount);
				
				return;
			}
			break;
		}
	}
	
	tell(me, ob->query_idname()+"無法用來進行生產。\n");
}
