/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : info.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-01-31
 * Note   : 資訊查詢指令。
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <map.h>
#include <ansi.h>
#include <feature.h>
#include <daemon.h>
#include <message.h>
#include <location.h>
#include <material.h>

inherit COMMAND;

string help = @HELP
        標準 info 指令。
HELP;

private void do_command(object me, string arg)
{
	object env = environment(me);
	mapping data;
	string msg;
	array loc = query_temp("location", me) || env->query_location();

	if( !arrayp(loc) ) 
		return tell(me, "無法查詢此地資料。\n");

	if( !arg || arg == "here" )
	{
		msg = pnoun(2, me)+"現在所在位置的相關資訊如下：\n";
	
		if( CITY_D->is_city_location(loc) )
		{
			data = CITY_D->query_coor_data(loc);
		
			if( !data["owner"] )
				msg += "擁有者  ：無\n";
			else if( belong_to_government(data["owner"]) )
				msg += "擁有者  ：市政府 "+CITY_D->query_city_idname(loc[CITY])+"\n";
			else if( belong_to_enterprise(data["owner"]) )
				msg += "擁有者  ：企業集團 "+ENTERPRISE_D->query_enterprise_color_id(data["owner"][11..])+"\n";
			else
				msg += "擁有者  ："+(find_player(data["owner"]) ? find_player(data["owner"])->query_idname() +HIC" 線上"NOR : capitalize(data["owner"])+HIR" 離線"NOR)+"\n";
	
			switch( data["region"] )
			{
				case AGRICULTURE_REGION:
					msg += "土地規劃："HIG"農業區\n"NOR;
					break;
				case INDUSTRY_REGION:
					msg += "土地規劃："HIY"工業區\n"NOR;
					break;
				case COMMERCE_REGION:
					msg += "土地規劃："HIC"商業區\n"NOR;
					break;
				default:
					msg += "土地規劃：無\n"NOR;
					break;
			}
				
			msg += "土地價值：$"+MONEY_D->city_to_money_unit(loc[CITY])+" "+NUMBER_D->number_symbol(data[VALUE])+"\n";
			msg += "地產價值：$"+MONEY_D->city_to_money_unit(loc[CITY])+" "+NUMBER_D->number_symbol(ESTATE_D->query_estate_value(loc))+"\n";
	

		}
		else
			msg += "一望無際的郊區區域...。\n";
	}
	else
		return tell(me, "目前只能查詢土地資料。\n");

	return tell(me, msg);
}
	
