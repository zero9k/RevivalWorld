/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : top.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-8-15
 * Note   : top 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <skill.h>
#include <map.h>

inherit COMMAND;

string help = @HELP
列出各種排行榜：

top 選項		- 預設列出世界前 30 名列表
top 選項 100 		- 列出世界前 100 名列表
top 選項 all		- 列出全部排名(最多 1000 名)

項目如下
top rich		- 世界富豪排行榜(現金＋房地產總值)
top cash		- 世界現金排行榜
top estate		- 世界房地產排行榜
top product		- 世界暢銷產品排行榜
top store		- 世界最佳商店排行榜
top skill		- 世界技能宗師排行榜
top skill 技能名	- 世界單獨技能等級排行榜
top building		- 世界高樓建築排行榜
top enterprise		- 世界企業排行榜
top stock		- 世界股王排行榜
top socialexp		- 世界社會經驗排行榜
top maintain		- 世界土地等級排行榜
top quest		- 世界任務完成數量排行榜
HELP;

private void do_command(object me, string arg)
{
	int i, top;
	string msg, arg1, arg2, arg3;

	if( !arg ) 
		return tell(me, help);

	if( sscanf(arg, "%s %s %s", arg1, arg2, arg3) == 3 )
	{
		if( arg3 == "all" ) 
			i = 1000;
		else 
			i = to_int(arg3);
	}
	else if( sscanf(arg, "%s %s", arg1, arg2) == 2 )
	{
		if( arg2 == "all" )
			i = 1000;
		else if( to_int(arg2) )
		{
			i = to_int(arg2);
			arg2= 0;
		}
		else
			i = 30;
	}
	else
	{
		i = 30;
		arg1 = arg;
	}

	if( i < 1 || i > 1000 )
		return tell(me, "顯示排名數量不得小於 1 或大於 1000。\n");

	switch( arg1 )
	{
		case "rich":
			{

				msg = NOR YEL"世界"HIY"前 "+i+" 富豪"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-28s %-12s %20s %12s\n"NOR, "排名", "玩家", "城市", "總資產($RW)", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach(array data in TOP_D->query_top("rich"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."NOR" %-28s %-12s "HIY"%20s"NOR YEL" %12s\n"NOR,
						top,
						data[3],
						(CITY_D->query_city_name(data[4]) || ""),
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "cash":
			{

				msg = NOR YEL"世界"HIY"前 "+i+" 現金"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-28s %-12s %20s %12s\n"NOR, "排名", "玩家", "城市", "現金($RW)", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach(array data in TOP_D->query_top("cash"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."NOR" %-28s %-12s "HIY"%20s"NOR YEL" %12s\n"NOR,
						top,
						data[3],
						(CITY_D->query_city_name(data[4]) || ""),
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "estate":
			{

				msg = NOR YEL"世界"HIY"前 "+i+" 房地產"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-28s %-12s %20s %12s\n"NOR, "排名", "玩家", "城市", "房地產($RW)", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach(array data in TOP_D->query_top("estate"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."NOR" %-28s %-12s "HIY"%20s"NOR YEL" %12s\n"NOR,
						top,
						data[3],
						(CITY_D->query_city_name(data[4]) || ""),
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "product":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 暢銷產品"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-33s %8s %11s  %-12s %6s\n"NOR, "排名", "產品", "銷售總數", "價值", "開發者", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("product"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."NOR" %-33s"HIW" %8s"HIY" %11s"HIG"  %-12s"NOR YEL" %6s\n"NOR,
						top,
						data[3],
						NUMBER_D->number_symbol(data[1]),
						data[4],
						ansi_capitalize(data[5]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "store":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 最佳商店排行榜"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-31s %14s %10s %-10s %6s\n"NOR, "排名", "商店", "銷售總數", "城市", "店長", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("storesell"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."NOR" %-31s"HIW" %14s"HIY" %10s"HIG" %-10s"NOR YEL" %6s\n"NOR,
						top,
						data[4],
						NUMBER_D->number_symbol(data[1]),
						(CITY_D->query_city_name(data[5]) || ""),
						ansi_capitalize(data[3]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "skill":
			{
				if( !arg2 )
				{
					msg = NOR YEL"世界"HIY"前 "+i+" 技能宗師"NOR YEL"排行榜：\n"NOR;
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
					msg += sprintf(HIW"%4s %-28s %-12s %20s %12s\n"NOR, "排名", "玩家", "城市", "神技數", "紀錄");
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
	
					foreach(array data in TOP_D->query_top("skill"))
					{
						if( ++top > i ) break;
						msg += sprintf(HIW"%3d."NOR" %-28s %-12s "HIY"%20s"NOR YEL" %12s\n"NOR,
							top,
							data[3],
							(CITY_D->query_city_name(data[4]) || ""),
							NUMBER_D->number_symbol(data[1]),
							TIME_D->replace_ctime(data[2])[0..9],
							);
					}
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
				}
				else
				{
					arg2 = lower_case(arg2);

					if( !SKILL_D->skill_exists(arg2) )
						return tell(me, "並沒有這種技能。\n");
					
					msg = NOR YEL"世界"HIY"前 "+i+" "+(SKILL(arg2))->query_idname()+"技能等級"NOR YEL"排行榜：\n"NOR;
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
					msg += sprintf(HIW"%4s %-28s %-12s %20s %12s\n"NOR, "排名", "玩家", "城市", "技能等級", "紀錄");
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
	
					foreach(array data in TOP_D->query_top("skill/"+arg2))
					{
						if( ++top > i ) break;
						msg += sprintf(HIW"%3d."NOR" %-28s %-12s "HIY"%20s"NOR YEL" %12s\n"NOR,
							top,
							data[3],
							sizeof(data) > 4 ? (CITY_D->query_city_name(data[4]) || "") : "",
							NUMBER_D->number_symbol(data[1]),
							TIME_D->replace_ctime(data[2])[0..9],
							);
					}
					msg += NOR WHT"────────────────────────────────────────\n"NOR;
					
					
				}
				return me->more(msg+"\n");
			}
			break;
		case "building":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 高樓建築"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				msg += sprintf(HIW"%4s %-14s %-32s %-10s %4s %10s\n"NOR, "排名", "玩家", "商店", "城市", "樓層", "紀錄");
				msg += NOR WHT"────────────────────────────────────────\n"NOR;

				foreach(array data in TOP_D->query_top("building"))
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%3d."HIY" %-14s "NOR"%-32s %-10s "HIG"%4s"NOR YEL" %10s\n"NOR,
						top,
						data[3],
						data[4],
						(CITY_D->query_city_name(data[5]) || ""),
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9],
						);
				}
				msg += NOR WHT"────────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;

		case "stock":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 股王"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				msg +=     HIW"排名 玩家                     城市          持有股票現值        紀錄時間\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("stockvalue") )
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%-3d."NOR" %-25s%-14s"HIG"%-18s"NOR YEL"%s\n"NOR,
						top,
						data[3],
						CITY_D->query_city_name(data[4])||"",
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9]);
				}
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "quest":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 任務完成"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				msg +=     HIW"排名 玩家                     城市          任務完成數量        紀錄時間\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("quest") )
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%-3d."NOR" %-25s%-14s"HIG"%-18s"NOR YEL"%s\n"NOR,
						top,
						data[3],
						CITY_D->query_city_name(data[4])||"",
						data[1],
						TIME_D->replace_ctime(data[2])[0..9]);
				}
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "enterprise":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 企業"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				msg +=     HIW"排名 企業                     資產總值		               紀錄時間\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("stock") )
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%-3d."NOR" %-25s"HIY"%-22s"NOR YEL"%s\n"NOR,
						top,
						data[2],
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9]);
				}
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "socialexp":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 社會經驗總值"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				msg +=     HIW"排名 玩家                     城市          社會經驗總值        紀錄時間\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("social_exp") )
				{
					if( ++top > i ) break;
					msg += sprintf(HIW"%-3d."NOR" %-25s%-14s"HIG"%-18s"NOR YEL"%s\n"NOR,
						top,
						data[3],
						CITY_D->query_city_name(data[4])||"",
						NUMBER_D->number_symbol(data[1]),
						TIME_D->replace_ctime(data[2])[0..9]);
				}
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		case "maintain":
			{
				msg = NOR YEL"世界"HIY"前 "+i+" 土地等級"NOR YEL"排行榜：\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				msg +=     HIW"排名 玩家                      座標                 種類 等級   紀錄時間\n"NOR;
				msg += NOR WHT"────────────────────────────────────\n"NOR;

				foreach( array data in TOP_D->query_top("maintain") )
				{
					if( ++top > i ) break;
						
					switch(data[4])
					{
						case FARM: arg1 = HIG"農"NOR GRN"田"NOR; break;
						case PASTURE: arg1 = HIY"牧"NOR YEL"場"NOR; break;
						case FISHFARM: arg1 = HIB"養"NOR BLU"殖"NOR; break;
					}
					msg += sprintf(HIW"%-3d."NOR" %-25s%-22s%4s"HIR"%5s"NOR YEL" %s\n"NOR,
						top,
						data[3],
						loc_short(restore_variable(data[0])),
						arg1,
						data[1],
						TIME_D->replace_ctime(data[2])[0..9]);
				}
				msg += NOR WHT"────────────────────────────────────\n"NOR;
				return me->more(msg+"\n");
			}
			break;
		default:
			tell(me, help);
			break;
	}
}		
