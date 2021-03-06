/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _export_action.c
 * Author : Clode@RevivalWorld
 * Date   : 2004-06-17
 * Note   : 
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <message.h>

void output_object(object env, string database, string basename, string amount);

// 物品輸出指令
void do_export(object me, string arg, string database)
{
	int i, j, which = 1;
	object env, master, ob, newob;
	mapping objects;
	string amount = "1", oamount, env_city, objectname, basename;
	string *shelfsort, shelf;
	
	env = environment(me);
	master = env->query_master();

	if( query("owner", env) != me->query_id(1) )
		return tell(me, pnoun(2, me)+"無法在別人的建築中輸出物品。\n");

	if( !arg )
		return tell(me, pnoun(2, me)+"想要輸出什麼物品？\n");
	
	env_city = env->query_city();
	
	sscanf(arg, "%s %s", amount, objectname);
	
	if( !objectname || (amount != "all" && !(amount = big_number_check(amount))) )
	{
		objectname = arg;
		amount = "1";
	}
	
	if( sscanf( objectname, "%s %d", arg, which ) == 2 )
	{
		if( which < 1 ) which = 1;
		objectname = arg;
	}

	objects = query(database, master);
	
	if( !sizeof(objects) )
		return tell(me, env->query_room_name()+"裡並沒有任何物品。\n");

	shelfsort = keys(objects) | (query("shelfsort", master) || allocate(0));

	foreach( shelf in shelfsort )
	{
		for(i=0;i<sizeof(objects[shelf]);i+=2)
		{
			basename = objects[shelf][i];
			oamount = objects[shelf][i+1];
			
			if( catch(ob = load_object(basename)) )
				continue;

			// 若物品已經消失
			if( !objectp(ob) )
			{
				error(basename+" 商品資料錯誤。");
				continue;
			}

			j++;
	
			if( j == to_int(objectname) || objectname == "all" || (ob->query_id(1) == lower_case(objectname) && !(--which)) )
			{
				if( amount == "all" || objectname == "all" )
					amount = oamount;
				else if( count(amount,">",oamount) )
					return tell(me, "這裡並沒有這麼多"+(query("unit", ob)||"個")+ob->query_idname()+"。\n");
				else if( count(amount, "<", 1) )
					return tell(me, "請輸入正確的輸出數量。\n");
			
				newob = new(basename);
				
				if( !query("flag/no_amount", newob) )
					set_temp("amount", amount, newob);
	
				msg("$ME將"+QUANTITY_D->obj(ob, amount)+"自"+env->query_room_name()+"輸出。\n",me,0,1);

				if( !me->get_object(newob, amount) )
				{
					if( count(amount, "==", 1) )
					{
						tell(me, pnoun(2, me)+"身上的物品太多或太重無法再拿更多東西了，只好先放在地上。\n");
						newob->move_to_environment(me);
					}
					else
					{
						tell(me, pnoun(2, me)+"身上的物品太多或太重無法再拿更多東西了。\n");
						destruct(newob);
						return;
					}
				}
	
				output_object(master, database, basename, amount);
				
				if( objectname != "all" )
					return;
			}
		}
	}
	
	if( objectname != "all" )
		return tell(me, "這裡並沒有 "+objectname+" 這種商品。\n");
	else if( master->query_building_type() == "warehouse" )
		master->save();
}
