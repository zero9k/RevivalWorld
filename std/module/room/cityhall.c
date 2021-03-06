/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : cityhall.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-2-24
 * Note   : 市政大樓動作繼承物件
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit ROOM_ACTION_MOD;

#define LAND_DOMINIUM	"/obj/etc/land_dominium"

#define UNREGISTER_MONEY		"100000"
#define DEFAULT_NEWBIE_MONEY		"1000"

#define CITY_IDNAME_CHANGE_COST		"500000000"

// 登記市民指令
void do_register(object me, string arg)
{
	string city, money_unit;
	object env = environment(me);

	city = env->query_city();
	
	if( CITY_D->is_citizen(me->query_id(1), city) )
		return tell(me, pnoun(2, me)+"已經是"+CITY_D->query_city_idname(city)+"的市民了。\n");
	
	if( CITY_D->is_citizen(me->query_id(1), query("city", me))  )
		return tell(me, pnoun(2, me)+"必須先放棄原本在"+CITY_D->query_city_idname(query("city", me))+"的市籍。\n");
	
	if( wizardp(me) && city != "wizard" )
		return tell(me, "巫師不能入籍玩家城市必免造成遊戲不公。\n");

	if( CITY_D->register_citizen(me->query_id(1), city) )
	{
		set("city", city, me);
		
		msg("$ME將填好的身家資料表拿給櫃台小姐，向"+CITY_D->query_city_idname(city)+"市政府正式登記成為市民。\n",me,0,1);
		
		if( !sizeof(filter_array(all_inventory(me), (: base_name($1) == LAND_DOMINIUM :))) )
			new(LAND_DOMINIUM)->move(me);
		
		money_unit = MONEY_D->city_to_money_unit(city);
		
		me->earn_money(money_unit, DEFAULT_NEWBIE_MONEY);
		
		msg("櫃台小姐站起身來奉上了一份土地所有權狀給$ME，並發放 "HIY+money(money_unit, DEFAULT_NEWBIE_MONEY)+NOR" 的市民救助金給$ME。\n", me, 0, 1);
		
		CHANNEL_D->channel_broadcast("news", me->query_idname()+"向"+CITY_D->query_city_idname(city)+"市政府正式登記成為市民。");
		CHANNEL_D->channel_broadcast("city", me->query_idname()+"向"+CITY_D->query_city_idname(city)+"市政府正式登記成為市民。", me);
		me->save();
		set_temp("city_register_time", time(), me);
		return;
	}
	
	return tell(me, "登記失敗。\n");
}

void do_confirm_unregister(object me, string city, string arg)
{
	string money_unit = MONEY_D->city_to_money_unit(city);

	if( !arg || !arg[0] )
		return tell(me, pnoun(2, me)+"取消了註銷市籍的動作。\n");

	arg = lower_case(arg);

	if( arg != "yes" && arg[0..0] == "y" )
	{
		tell(me, HIY"若"+pnoun(2, me)+"確定要註銷市民身份的話請輸入'yes'。\n"+pnoun(2, me)+"確定要註銷"+CITY_D->query_city_idname(city)+HIY"的市籍？(Yes/No)"NOR);
		input_to((: do_confirm_unregister, me, city:));
		return;
	}
	
	if( arg != "yes" )
	{
		tell(me, pnoun(2, me)+"取消了註銷市籍的動作。\n");
		me->finish_input();
		return;
	}

	if( query("total_online_time", me) > 7*24*60*60 && !me->spend_money(money_unit, UNREGISTER_MONEY) )
	{
		tell(me, pnoun(2, me)+"註銷市籍需要繳交 $"+money_unit+" "+UNREGISTER_MONEY+"。\n");
		me->finish_input();
		return;
	}

	if( CITY_D->remove_citizen(me->query_id(1)) )
	{
		delete("city", me);
		
		msg("$ME向櫃台小姐登記註銷了在"+CITY_D->query_city_idname(city)+"的市民身份。\n", me, 0, 1);
		
		if( present("land dominium", me) )
			destruct(present("land dominium", me));

		me->spend_money(money_unit, DEFAULT_NEWBIE_MONEY);

		ESTATE_D->remove_estate(me->query_id(1));
		CHANNEL_D->channel_broadcast("news", me->query_idname()+"註銷了在"+CITY_D->query_city_idname(city)+"的市籍。");
		
		me->save();
		me->finish_input();

		return;
	}
	me->finish_input();
	return tell(me, "註銷失敗。\n");
}

// 註銷市民身份
void do_unregister(object me, string arg)
{
	string city = query("city", me);

	if( !CITY_D->city_exist(city) )
		return tell(me, pnoun(2, me)+"不屬於任何一座城市。\n");

	if( !CITY_D->is_citizen(me->query_id(1), city) )
		return tell(me, pnoun(2, me)+"本來就不是"+CITY_D->query_city_idname(city)+"的市民。\n");

	if( CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"是市長不能註銷市民身份。\n");

	if( time() - query_temp("city_register_time", me) < 600 )
		return tell(me, pnoun(2, me)+"必須等待 "+(time() - query_temp("city_register_time", me))+" 秒後才能註銷市民身份。\n");

	tell(me, HIR"\n\n註銷市民身份將會使"+pnoun(2, me)+"喪失在本城市內的所有土地與建築物並且得不到任何補償。\n"HIY+pnoun(2, me)+"確定要註銷"+CITY_D->query_city_idname(city)+HIY"的市籍？(Yes/No)"NOR);

	input_to((: do_confirm_unregister, me, city:));
}

void do_citizen(object me, string arg)
{
	int i;
	string msg="";
	string city = environment(me)->query_city();
	
	msg = CITY_D->query_city_idname(city)+"的市民名單如下(依照入籍順序)：\n";
	
	foreach( string id in CITY_D->query_citizens(city) )
		msg += sprintf("%-15s%s", capitalize(id), ++i%6 ? "" : "\n");
	
	return tell(me, msg+"\n\n");
}

void reset_election_roomlong(object env)
{
	int i;
	mapping election = query("election", env);
	string msg = "";
	
	if( election )
	{
		msg += HIW"\n投票主題："HIC+election["title"]+NOR WHT"\n──────────────────────────────\n"NOR;
		msg += HIG"開放登記參選："NOR+(election["candidate"] ? "是" : "否")+"\n";
	
		if( election["end"] )
			msg += HIG"投票開始時間："NOR+(election["start"] ? election["start"]/60/60/24+" 天 "+election["start"]/60/60%24+" 時 "+election["start"]/60%60+" 分鐘後" : "已經開始")+"\n";
		
		if( election["announced"] )
			msg += HIG"投票結束時間："NOR+(election["end"] ? (election["end"]/60/60/24)+" 天 "+(election["end"]/60/60%24)+" 時 "+(election["end"]/60%60)+" 分鐘後" : "已經結束")+"\n";

		msg += NOR WHT"──────────────────────────────\n"HIW"投票項目：\n\n"NOR;
		
		if( election["option"] )
		{
			foreach(string option, mapping data in election["option"] )
				msg += sprintf(HIR"%2d"NOR RED" - "NOR"%-50s %d 票\n", ++i, data["candidate"]||option, sizeof(data["vote"]));
		}
		else
			msg += "    無任何選項\n";
		
		msg += NOR WHT"──────────────────────────────\n"NOR;
		set("long", msg, env);
	}
	else
		set("long", "目前沒有任何投票活動。", env);
}

string query_pcolor(int percent)
{
	switch(percent)
	{
		case 0..30:
			return HIR+percent+"%"NOR;
		case 31..70:
			return HIY+percent+"%"NOR;
		case 71..100:
			return HIG+percent+"%"NOR;
		case 101..120:
			return HIC+percent+"%"NOR;
		default:
			return HIW+percent+"%"NOR;
	}
}

void heart_beat(object env)
{
	switch( query("function", env) )
	{
		case "election":
		{
			int time = time();
			mapping election = query("election", env) || allocate_mapping(0);
			
			if( !((time-120)%1800) )
			{
				set("city", env->query_city(), env);
				
				if( election["announced"] && !election["ended"] )
					CHANNEL_D->channel_broadcast("city", "市政府正在舉辦「"+election["title"]+"」的投票活動，請踴躍參與投票。", env);
			}
			
			if( !(time%1800) )
				env->save();

			if( election["announced"] )
			{
				if( !election["started"] && addn("election/start", -1, env) <= 0 )
				{
					set("election/started", 1, env); // 已開始
					delete("election/start", env);
					env->save();
				}
				
				if( !election["ended"] && addn("election/end", -1, env) <= 0 )
				{
					set("election/ended", 1, env); // 已結束
					delete("election/end", env);
					env->save();
				}
				reset_election_roomlong(env);
			}
			break;
		}	
	}
}

void do_refresh(object me, string arg)
{
	object env = environment(me);
	string msg, estatetax;
	string city = env->query_city();
	string moneyunit = MONEY_D->city_to_money_unit(city);
	int num = CITY_D->query_city_num(city);
	mapping fund = CITY_D->query_city_info(city, "fund") || allocate_mapping(0);
	mapping facilities = CITY_D->query_public_facility(city);
	mapping money = CITY_D->query_city_fund(city);
	string totalreceipt, totalexpense, estatecost;

	estatetax = TAX_D->query_city_tax(city);

	totalreceipt = count(estatetax, "+", totalreceipt);
	
	foreach( string id, mixed m in money )
		totalexpense = count( totalexpense, "+", m );

	foreach( string citizen in CITY_D->query_citizens(city) )
		estatecost = count( estatecost, "+", count(ESTATE_D->query_all_estate_value(citizen), "/", 15000));
	
	totalexpense = count( totalexpense, "+", estatecost);

	msg  = "┌──────────────── "HIG"更新時間 "+TIME_D->replace_ctime(time())+NOR"  ┐\n";
	msg += "｜"+sprintf("%-60s", CITY_D->query_city_idname(city)+"的財政資訊")+"｜\n";
	msg += "├─────────────"HIR"市政"NOR RED"支出"NOR"─────────────┤\n";

	msg += "｜"HIW"自然資源          數量       自然保育             所需金額"NOR"  ｜\n";
	msg += sprintf("｜  %-16s%-12s%7s"HIR"%21s"NOR"  ｜\n", NOR YEL"山脈"NOR, num+" 座城", query_pcolor(fund["山脈"]), money(moneyunit, money["山脈"]));
	msg += sprintf("｜  %-16s%-12s%7s"HIR"%21s"NOR"  ｜\n", NOR HIG"森林"NOR, num+" 座城", query_pcolor(fund["森林"]), money(moneyunit, money["森林"]));
	msg += sprintf("｜  %-16s%-12s%7s"HIR"%21s"NOR"  ｜\n", NOR HIB"河流"NOR, num+" 座城", query_pcolor(fund["河流"]), money(moneyunit, money["河流"]));
	msg += "｜                                                            ｜\n";
	msg += "｜"HIW"維護支出          數量       維護保養             所需金額"NOR"  ｜\n";
	msg += sprintf("｜  %-16s%-12d%7s"HIR"%21s"NOR"  ｜\n", NOR WHT"道路"NOR, facilities["道路"], query_pcolor(fund["道路"]), money(moneyunit, money["道路"]));
	msg += sprintf("｜  %-16s%-12d%7s"HIR"%21s"NOR"  ｜\n", NOR RED"橋樑"NOR, facilities["橋樑"], query_pcolor(fund["橋樑"]), money(moneyunit, money["橋樑"]));
	msg += sprintf("｜  %-16s%-12d%7s"HIR"%21s"NOR"  ｜\n", HIY"荒地整治"NOR, facilities["空地"], "- ", money(moneyunit, money["空地"]));
	msg += "｜                                                            ｜\n";
	msg += "｜"HIW"營運支出          數量       營運管理             所需金額"NOR"  ｜\n";
	msg += sprintf("｜  %-16s%-12d%7s"HIR"%21s"NOR"  ｜\n", HIM"政府建築物"NOR, 	facilities["公共建築物"], "- ", money(moneyunit, money["公共建築物"]));
	msg += sprintf("｜  %-16s%-12s%7s"HIR"%21s"NOR"  ｜\n", HIC"市民地產管理"NOR, 	"- ", "- ", money(moneyunit, estatecost));
	msg += "｜                                                            ｜\n";
	msg += sprintf("｜  "HIR"總支出     %45s"NOR"  ｜\n", money(moneyunit, totalexpense));
	msg += "├─────────────"HIG"市政"NOR GRN"收入"NOR"─────────────┤\n";
	msg += "｜"HIW"稅捐收入                         稅率             徵收稅額"NOR"  ｜\n";
	msg += sprintf("｜  %-28s%7d"HIG"%21s"NOR"  ｜\n", "地產稅", CITY_D->query_city_info(city, "estatetax"), money(moneyunit, estatetax));
	msg += "｜                                                            ｜\n";
	msg += sprintf("｜  "HIG"總收入     %45s"NOR"  ｜\n", money(moneyunit, totalreceipt));
	msg += "├─────────────"HIY"收支"NOR YEL"統計"NOR"─────────────┤\n";
	msg += sprintf("｜  上月財政收支 "HIY"%43s"NOR"  ｜\n", money(moneyunit, CITY_D->query_city_info(city, "last_total_gain")));
	msg += sprintf("｜  本月預計收支 "HIY"%43s"NOR"  ｜\n", money(moneyunit, count(totalreceipt, "-", totalexpense)));
	msg += "└──────────────────────────────┘\n";
	msg += NOR YEL" 請輸入 "HIY"refresh"NOR YEL" 指令來更新到最新的財政統計數字。\n"NOR;
				
	set("long", msg, env);
	tell(me, "更新財政資訊完畢。\n");

	env->save();
}

void do_election_manage(object me, string arg)
{
	mixed option;
	object env = environment(me);
	string city = env->query_city();
	mapping election = query("election", env);

	if( !arg )
		return tell(me, "請參考 help manage 指令說明。\n");
	
	if( !CITY_D->is_mayor_or_officer(city, me) )
		return tell(me, pnoun(2, me)+"不是這座城市的市長或官員，無法管理投票箱。\n");

	if( sscanf(arg, "new %s", option) )
	{
		if( noansi_strlen(option) > 30 )
			return tell(me, "主題名稱不能超過 30 個字元。\n");
		
		if( sizeof(election) )
			return tell(me, "目前已經有另一個投票主題在運作中。\n");
		
		option = kill_repeat_ansi(option+NOR);
		set("election/title", option, env);
		tell(me, pnoun(2, me)+"設定投票主題為「"+option+"」。\n");
	}
	else if( arg == "delete" )
	{
		if( !election )
			return tell(me, "目前沒有任何投票活動。\n");
		
		if( election["announced"] )		
			CHANNEL_D->channel_broadcast("city", NOR YEL+me->query_idname()+NOR YEL"取消市政府投票活動「"HIY+election["title"]+NOR YEL"」。"NOR, me);

		tell(me, "刪除所有投票資料。\n");
		delete("election", env);
	}
	else if( arg == "candidate" )
	{
		if( !election["title"] )
			return tell(me, "請先設定投票主題。\n");

		if( election["announced"] )
			return tell(me, "投票活動已經公告，無法再修改設定。\n");

		if( election["candidate"] )
		{
			tell(me, "不允許候選人登記參選。\n");
			delete("election/candidate", env);
		}
		else
		{
			tell(me, "開放候選人登記參選。\n");
			set("election/candidate", 1, env);
		}
	}
	else if( sscanf(arg, "option %s", option) )
	{
		if( !election["title"] )
			return tell(me, "請先設定投票主題。\n");

		if( election["announced"] )
			return tell(me, "投票活動已經公告，無法再修改設定。\n");

		if( query("election/option/"+option, env) )
		{
			tell(me, "刪除投票項目「"+option+"」。\n");	
			delete("election/option/"+option, env);
		}
		else
		{
			tell(me, "增加投票項目「"+option+"」。\n");
			set("election/option/"+option+"/candidate", 0, env);
		}
	}
	else if( sscanf(arg, "start %d", option) )
	{
		if( !election["title"] )
			return tell(me, "請先設定投票主題。\n");

		if( election["announced"] )
			return tell(me, "投票活動已經公告，無法再修改設定。\n");

		if( option < 0 || option > 240 )
			return tell(me, "投票開始時間並需在 0 至 240 小時之間。\n");

		if( election["end"] && option*3600 >= election["end"] )
			return tell(me, "投票開始時間必須要在投票結束時間之前。\n");
			
		set("election/start", option*3600, env);
		tell(me, pnoun(2, me)+"設定在公佈投票活動的 "+option+" 個小時後開始投票。\n");
	}
	else if( sscanf(arg, "end %d", option) )
	{
		if( !election["title"] )
			return tell(me, "請先設定投票主題。\n");

		if( election["announced"] )
			return tell(me, "投票活動已經公告，無法再修改設定。\n");

		if( option < 0 || option > 480 )
			return tell(me, "投票開始時間並需在 0 至 480 小時之間。\n");

		if( option*3600 <= election["start"] )
			return tell(me, "投票結束時間必須要在投票開始時間之後。\n");

		set("election/end", option*3600, env);
		tell(me, pnoun(2, me)+"設定在公佈投票活動的 "+option+" 個小時後結束投票。\n");
		
	}
	else if( arg == "announce" )
	{
		if( !election["title"] )
			return tell(me, "無投票主題。\n");
		if( !election["end"] )
			return tell(me, "未設投票結束時間。\n");
		if( election["announced"] )
			return tell(me, "投票活動已經公告。\n");

		if( !election["start"] )
			set("election/started", 1, env);

		set("election/announced", 1, env);
		
		heart_beat(env);
		
		CHANNEL_D->channel_broadcast("city", NOR YEL+me->query_idname()+NOR YEL"在市政府舉辦的投票活動「"HIY+election["title"]+NOR YEL"」正式開始。"NOR, me);
	}
	else tell(me, "請參考 help manage 指令說明。\n");
	
	reset_election_roomlong(env);
	env->save();
}

void do_election_register(object me, string arg)
{
	object env = environment(me);
	string city = env->query_city();
	string myid = me->query_id(1);
	mapping election = query("election", env);

	if( !CITY_D->is_citizen(myid, city) )
		return tell(me, pnoun(2, me)+"不是"+CITY_D->query_city_idname(city)+"的市民。\n");

	if( !election )
		return tell(me, "目前沒有投票活動。\n");

	if( query("total_online_time", me) < 30*24*60*60 )
		return tell(me, pnoun(2, me)+"的總上線時間尚未超過 30 天，無法登記為候選人。\n");
	
	if( !election["candidate"] )
		return tell(me, "此次投票不開放候選人登記。\n");

	if( query("election/option/"+myid, env) )
	{
		delete("election/option/"+myid, env);
		reset_election_roomlong(env);
		CHANNEL_D->channel_broadcast("city", NOR YEL+me->query_idname()+NOR YEL"放棄參選「"HIY+election["title"]+NOR YEL"」。"NOR, me);
		return;
	}
		
	if( election["ended"] )
		return tell(me, "投票已經結束。\n");

	if( election["started"] )
		return tell(me, "已經開始投票，無法再進行登記。\n");

	set("election/option/"+myid+"/candidate", me->query_idname(), env);
	
	reset_election_roomlong(env);
	CHANNEL_D->channel_broadcast("city", NOR YEL+me->query_idname()+NOR YEL"登記參選「"HIY+election["title"]+NOR YEL"」投票活動。"NOR, me);
	
	env->save();
}

void do_election_vote(object me, string arg)
{
	int i;
	object env = environment(me);
	string city = env->query_city();
	string myid = me->query_id(1);
	mapping election = query("election", env);

	if( !arg || !to_int(arg) )
		return tell(me, "請輸入"+pnoun(2, me)+"想要投票的選項編號。\n");

	if( !CITY_D->is_citizen(myid, city) )
		return tell(me, pnoun(2, me)+"不是"+CITY_D->query_city_idname(city)+"的市民。\n");

	if( !election )
		return tell(me, "目前沒有投票活動。\n");

	if( query("total_online_time", me) < 20*24*60*60 )
		return tell(me, pnoun(2, me)+"的總上線時間尚未超過 20 天，無法投票。\n");
		
	if( election["ended"] )
		return tell(me, "投票已經結束。\n");

	if( !election["started"] )
		return tell(me, "還未開始投票。\n");
	
	if( !election["option"] )
		return tell(me, "目前沒有任何投票選項。\n");

	foreach(string option, mapping data in election["option"] )
		if( query("election/option/"+option+"/vote/"+myid, env) )
			return tell(me, pnoun(2, me)+"已經投過票了。\n");
	
	foreach(string option, mapping data in election["option"] )
		if( ++i == to_int(arg) )
			set("election/option/"+option+"/vote/"+myid, 1, env);
	
	reset_election_roomlong(env);
	CHANNEL_D->channel_broadcast("city", me->query_idname()+"投下「"+election["title"]+"」神聖的一票。", me);
	env->save();
}

void do_officer(object me, string arg)
{
	int num;
	int maxofficers;
	string id, title, city = environment(me)->query_city();
	mapping officers;
	object officer;
	
	officers = CITY_D->query_city_info(city, "government/officer") || allocate_mapping(0);
	
	if( CITY_D->is_officer(city, me) && arg == "quit" )
	{
		title = officers[me->query_id(1)];
		map_delete(officers, me->query_id(1));
		CITY_D->set_city_info(city, "government/officer", officers);
		
		CHANNEL_D->channel_broadcast("city", me->query_idname()+"向市政府辭去了「"+title+"」的官員職務。", me);
		msg("$ME向市政府辭去了「"+title+"」的官員職務。\n", me, 0, 1);
		
		for(int i=0;CITY_D->city_exist(city, i);i++)
		{
			if( CITY_D->query_section_info(city, i, "vicemayor") == me->query_id(1) )
				CITY_D->delete_section_info(city, i, "vicemayor");
		}
			
		return;
	}
	
	if( !CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"不是市長不能設定"+pnoun(2, me)+"的官員。\n");
		
	if( !arg )
		return tell(me, "目前官員名單為："+(sizeof(officers) ? implode(keys(officers), (: capitalize($1)+", "+capitalize($2) :)) : "無")+"\n");

	if( sscanf(arg, "%s vicemayor %d", id, num) != 2 && sscanf(arg, "%s %s", id, title) != 2 )
		return tell(me, "指令格式錯誤，請 help officer。\n");

	if( num > 0 )
	{
		num--;
		
		if( !CITY_D->city_exist(city, num) )
			return tell(me, "這個城市沒有第 "+(num+1)+" 衛星都市。\n");

		if( !objectp(officer = find_player(id)) )
			return tell(me, "線上沒有 "+capitalize(id)+" 這位玩家。\n");

		if( !CITY_D->is_citizen(id, city) )
			return tell(me, officer->query_idname()+"並不是本城市的市民，無法任命為副市長。\n");
		
		maxofficers = 2+3*CITY_D->query_sections(city);
		if( undefinedp(officers[id]) && sizeof(officers) >= maxofficers )
			return tell(me, "目前最多只能同時任命 "+maxofficers+" 位官員。\n");
		
		if( !undefinedp(officers[id]) )
			return tell(me, pnoun(2, me)+"必須先解除其原本的官員職位。\n");

		if( CITY_D->query_section_info(city, num, "vicemayor") )
			return tell(me, pnoun(2, me)+"必須先解除 "+capitalize(CITY_D->query_section_info(city, num, "vicemayor"))+" 副市長的職位。\n");

		officers[id] = CITY_D->query_section_info(city, num, "name")+"副市長";

		CITY_D->set_city_info(city, "government/officer", officers);
	
		CITY_D->set_section_info(city, num, "vicemayor", id);
		
		CHANNEL_D->channel_broadcast("city", "市長"+me->query_idname()+"任命"+officer->query_idname()+"成為"+CITY_D->query_city_name(city, num)+"的副市長。", me);	
		
		return;
	}

	if( title == "-d" )
	{
		officer = find_player(id);
		
		if( undefinedp(officers[id]) )
			return tell(me, (objectp(officer) ? officer->query_idname() : capitalize(id)+" ")+"原本就不是本城市官員。\n");
			
		for(int i=0;CITY_D->city_exist(city, i);i++)
		{
			if( CITY_D->query_section_info(city, i, "vicemayor") == id )
				CITY_D->delete_section_info(city, i, "vicemayor");
		}

		map_delete(officers, id);
		CITY_D->set_city_info(city, "government/officer", officers);
		CHANNEL_D->channel_broadcast("city", "市長"+me->query_idname()+"解除了"+(objectp(officer) ? officer->query_idname() : " "+capitalize(id)+" ")+"的官員職務。", me);
		return;	
	}

	title = kill_repeat_ansi(title+NOR);

	if( !objectp(officer = find_player(id)) )
		return tell(me, "線上沒有 "+capitalize(id)+" 這位玩家。\n");
	
	if( !CITY_D->is_citizen(id, city) )
		return tell(me, officer->query_idname()+"並不是本城市的市民，無法任命為官員。\n");

	maxofficers = 2+3*CITY_D->query_sections(city);
	if( undefinedp(officers[arg]) && sizeof(officers) >= maxofficers )
		return tell(me, "目前最多只能同時任命 "+maxofficers+" 位官員。\n");
		
	if( title == officers[id] )
		return tell(me, officer->query_idname()+"已經被任命為「"+title+"」。\n");

	officers[id] = title;
	CITY_D->set_city_info(city, "government/officer", officers);
	CHANNEL_D->channel_broadcast("city", "市長"+me->query_idname()+"任命"+officer->query_idname()+"成為"+CITY_D->query_city_name(city)+"的「"+title+"」官員。", me);	
}

void do_confirm_abdicate(object me, object target, string city, string arg)
{
	if( lower_case(arg) != "yes" )
	{
		tell(me, pnoun(2, me)+"取消市長職位的交接手續。\n");
		me->finish_input();
		return;
	}
	
	CHANNEL_D->channel_broadcast("city", me->query_idname()+"正式將市長職位交接給"+target->query_idname()+"，"+target->query_idname()+"成為本城市的新市長。", me);
	CHANNEL_D->channel_broadcast("city", "由於市長交接，舊有城市官員全部卸任，新市政團隊將由新市長進行重組。", me);
	CHANNEL_D->channel_broadcast("news", CITY_D->query_city_idname(city)+"的市長"+me->query_idname()+"正式將市長職位交接給"+target->query_idname()+"，"+target->query_idname()+"成為"+CITY_D->query_city_idname(city)+"的新市長。");
	
	msg(me, "$ME將市長印璽親手交給了$YOU，並拍拍了$YOU的肩膀說道：「這座城市就交給"+pnoun(2, target)+"了，不要讓我失望」。\n", me, target, 1);
	
	CITY_D->set_city_info(city, "government/mayor", target->query_id(1));
	CITY_D->set_city_info(city, "government/officer", allocate_mapping(0));
}

void do_abdicate(object me, string arg)
{
	object targetob;
	string city = environment(me)->query_city();
	
	if( !CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"不是本市市長。\n");

	if( !arg )
		return tell(me, pnoun(2, me)+"想將永久市長的職位讓給誰？\n");
	
	if( !objectp(targetob = find_player(arg)) )
		return tell(me, "線上並沒有 "+arg+" 這位玩家。\n");
		
	if( !CITY_D->is_citizen(arg, city) )
		return tell(me, targetob->query_idname()+"並不是本城市的市民，無法擔任此市的永久市長。\n");
		
	if( !same_environment(me, targetob) )
		return tell(me, targetob->query_idname()+"不在"+pnoun(2, me)+"的身旁，無法交接永久市長印璽。\n");

	tell(me, HIY"確定將永久市長職位交給"+targetob->query_idname()+HIY"嗎？(Yes/No)："NOR);
	
	input_to((: do_confirm_abdicate, me, targetob, city:));
}

void do_denominate(object me, string arg)
{
	int num;
	string msg;
	string city = environment(me)->query_city();
	string moneyunit = environment(me)->query_money_unit();

	if( !arg )
	{
		msg = CITY_D->query_city_idname(city)+"各衛星都市名稱如下：\n";
		
		while(CITY_D->city_exist(city, num))
		{
			msg += sprintf("%-4s%s\n", (num+1)+".", CITY_D->query_section_info(city, num, "name")||"第 "+(num+1)+" 衛星都市");
			num++;
		}
	
		return tell(me, msg+"\n");
	}
	
	if( !CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"不是市長無法使用此功能。\n");
	
	if( sscanf(arg, "id %s", arg) == 1 )
	{
		if( lower_case(remove_ansi(arg)) != city )
			return tell(me, "英文字母必須與現有的城市英文字母相同，"+pnoun(2, me)+"只能更改大小寫與色彩。\n");
			
		if( !CITY_D->change_assets(city, "-"+CITY_IDNAME_CHANGE_COST) )
			return tell(me, "市政府的資產不足 "HIY+money(moneyunit, CITY_IDNAME_CHANGE_COST)+NOR" 無法變更英文名稱。\n");
			
		arg = remove_fringe_blanks(kill_repeat_ansi(arg+NOR));

		CHANNEL_D->channel_broadcast("news", CITY_D->query_city_idname(city)+"市長"+me->query_idname()+"發佈消息：自今日起本城市之英文名稱改名為「"+arg+"」。\n");
		tell(me, pnoun(2, me)+"花費市府資產 "HIY+money(moneyunit, CITY_IDNAME_CHANGE_COST)+NOR" 變更城市的英文名稱。\n");

		CITY_D->set_city_info(city, "color_id", arg);

		return;
	}
	
	if( sscanf(arg, "name %s", arg) == 1 )
	{
		int strlen = noansi_strlen(arg);
		
		if( strlen < 2 || strlen > 10 )
			return tell(me, "城市的中文名稱字數必須介於 1 - 5 個中文字。\n");
		
		if( !is_chinese(arg) )
			return tell(me, "城市的中文名稱只能使用中文。\n");
			
		if( !CITY_D->change_assets(city, "-"+CITY_IDNAME_CHANGE_COST) )
			return tell(me, "市政府的資產不足 "HIY+money(moneyunit, CITY_IDNAME_CHANGE_COST)+NOR" 無法變更中文名稱。\n");

		arg = remove_fringe_blanks(kill_repeat_ansi(arg+NOR));

		CHANNEL_D->channel_broadcast("news", CITY_D->query_city_idname(city)+"市長"+me->query_idname()+"發佈消息：自今日起本城市之中文名稱改名為「"+arg+"」。\n");
		tell(me, pnoun(2, me)+"花費市府資產 "HIY+money(moneyunit, CITY_IDNAME_CHANGE_COST)+NOR" 變更城市的中文名稱。\n");

		CITY_D->set_city_info(city, "name", arg);

		return;
	}

	if( sscanf(arg, "-symbol %d %s", num, arg) == 2 )
	{
		int strlen;
		string *emblem = CITY_D->query_city_info(city, "emblem");

		if( num < 1 || num > 3 )
			return tell(me, "只能針對 1 至 3 行的徽記作變更。\n");
		
		arg = kill_repeat_ansi(replace_string(arg, "\n", "")+NOR);
		
		strlen = noansi_strlen(arg);

		if( strlen > 10 )
			return tell(me, "每行徽記最多 10 個字元。\n");

		if( strlen < 10 )
		{
			switch(strlen)
			{
				case 1:arg = "    "+arg+"     ";break;
				case 2:arg = "    "+arg+"    ";break;
				case 3:arg = "   "+arg+"    ";break;
				case 4:arg = "   "+arg+"   ";break;
				case 5:arg = "  "+arg+"   ";break;
				case 6:arg = "  "+arg+"  ";break;
				case 7:arg = " "+arg+"  ";break;
				case 8:arg = " "+arg+" ";break;
				case 9:arg = arg+" ";break;
			}
			
		}			
		emblem[num-1] = arg;
		
		CITY_D->set_city_info(city, "emblem", emblem);
		
		msg("$ME對城市徽記作了「"+arg+"」部份的變更。\n", me, 0, 1);
		return;
	}
		
		
		
	if( sscanf(arg, "%d %s", num, arg) != 2 )
		return tell(me, "請參考 help denominate 正確指令格式。\n");
	
	if( !CITY_D->city_exist(city, num-1) )
		return tell(me, "本城市並沒有 "+num+" 號的衛星都市。\n");

	arg = kill_repeat_ansi(replace_string(arg, "\n", "")+NOR);
	
	if( noansi_strlen(arg) > 10 )
		return tell(me, "衛星都市名稱長度最多 10 個字元。\n");
	
	CITY_D->set_section_info(city, num-1, "name", arg);
	CHANNEL_D->channel_broadcast("city", me->query_idname()+"將"+CITY_D->query_city_idname(city)+"第 "+num+" 衛星都市的名稱設定為「"+arg+"」。", me);
	return tell(me, pnoun(2, me)+"將"+CITY_D->query_city_idname(city)+"第 "+num+" 衛星都市的名稱設定為「"+arg+"」。\n");
}

void do_donate(object me, string arg)
{
	string myid = me->query_id(1);
	object env = environment(me);
	string city = env->query_city();
	string moneyunit = MONEY_D->city_to_money_unit(city);
	
	if( !CITY_D->is_citizen(myid, city) )
		return tell(me, pnoun(2, me)+"不是本城市市民無法捐獻。\n");

	if( !arg )
	{
		string msg;
		mapping donate = query("donate", env);
		
		if( !sizeof(donate) )
			return tell(me, "目前尚無任何捐款。\n");
			
		msg = "市政經費捐獻名單如下：\n";
		foreach(string id, mapping data in donate)
			msg += sprintf("%-40s%s\n", data["idname"], "$"+moneyunit+" "+NUMBER_D->number_symbol(data["money"]));
			
		return me->more(msg+"\n");
	}
	
	else if(!(arg = big_number_check(arg)))
		return tell(me, "請輸入正確的捐款金額。\n");
		
	if( count(arg, "<", 10000) )
		return tell(me, "最少捐款金額為 $"+moneyunit+" 10,000 元。\n");
	
	if( !me->spend_money(moneyunit, arg) )
		return tell(me, pnoun(2, me)+"身上沒有這麼多的 $"+moneyunit+" 現金。\n");
	
	set("donate/"+myid, (["idname" : me->query_idname(), "money" : count(arg, "+", query("donate/"+myid+"/money", env)) ]), env);

	env->save();
	CITY_D->change_assets(city, arg);
	CHANNEL_D->channel_broadcast("city", me->query_idname()+"捐獻了 $"+moneyunit+" "+NUMBER_D->number_symbol(arg)+" 作為市政經費之用。", me);

	return tell(me, pnoun(2, me)+"捐獻了 $"+moneyunit+" "+NUMBER_D->number_symbol(arg)+" 作為市政經費之用。\n");
}

void do_fire(object me, string arg)
{
	object env = environment(me);
	string city = env->query_city();
	
	if( !CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"不是市長無法使用此功能。\n");
		
	if( !arg )
		return tell(me, pnoun(2, me)+"要開除哪一個市民的市籍？。\n");
		
	arg = lower_case(arg);

	if( !CITY_D->is_citizen(arg, city) )
		return tell(me, capitalize(arg)+" 並不是本城市的市民。\n");
		
	if( sizeof(ESTATE_D->query_player_estate(arg)) )
		return tell(me, capitalize(arg)+" 在這城市中還有房地產，不能開除其市籍。\n");
		
	if( CITY_D->remove_citizen(arg) )
	{
		object player = find_player(arg) || load_user(arg);
		
		if( !objectp(player) )
			CHANNEL_D->channel_broadcast("news", me->query_idname()+"開除了 "+capitalize(arg)+" 在"+CITY_D->query_city_idname(city)+"的市民身份。");
		else
		{
			delete("city", player);
			CHANNEL_D->channel_broadcast("news", me->query_idname()+"開除了"+player->query_idname()+"在"+CITY_D->query_city_idname(city)+"的市民身份。");
			player->save();
			
			if( !interactive(player) )
				destruct(player);
		}
	}
	else
		error("註銷失敗，請通知巫師處理。\n");
}

void do_fund(object me, string arg)
{
	int percent;
	int taxrate;
	object env = environment(me);
	string tax, city = env->query_city();
	mapping fund = CITY_D->query_city_info(city, "fund") || allocate_mapping(0);

	if( !arg )
		return me->more(query("long", env));

	if( !CITY_D->is_mayor(city, me) )
		return tell(me, pnoun(2, me)+"不是市長無法使用此功能。\n");
	
	if( sscanf(arg, "%s稅 %d", tax, taxrate) == 2 )
	{
		if( taxrate < 0 || taxrate > 10000 )
			return tell(me, "稅率不可低於 0 或高於 10000。\n");
			
		switch(tax)
		{
			case "地產":
				CITY_D->set_city_info(city, "estatetax", taxrate);
				break;
			default:
				return tell(me, "目前並無"+tax+"稅。\n");
		}

		CHANNEL_D->channel_broadcast("city", me->query_idname()+"將"+CITY_D->query_city_idname(city)+"的"+tax+"稅調整為 "+taxrate+"。\n", me);
		return tell(me, pnoun(2, me)+"將"+CITY_D->query_city_idname(city)+"的"+tax+"稅調整為 "+taxrate+"。\n", me);
	}

	if( sscanf(arg, "%s %d", arg, percent) == 2 )
	{	
		if( percent < 0 || percent > 120 )
			return tell(me, "經費不可低於 0% 或高於 120%。\n");
			
		switch(arg)
		{
			case "山脈":
			case "森林":
			case "河流":
			case "道路":
			case "橋樑":
				fund[arg] = percent;
				break;
			default:
				return tell(me, "請輸入正確的施政選項。\n");
		}
		
		CITY_D->set_city_info(city, "fund", fund);
	
		CHANNEL_D->channel_broadcast("city", me->query_idname()+"將「"+arg+"」的相關財政經費調整為 "+query_pcolor(percent)+"。", me);

		return tell(me, pnoun(2, me)+"將「"+arg+"」的相關財政經費調整為 "+query_pcolor(percent)+"。\n", me);
	}
	return tell(me, "請參考 help fund 輸入正確格式。\n");
}

// 設定建築物內房間型態種類
nosave mapping action_info =
([
	"lobby"	:
	([
		"short"	: HIC"大廳"NOR,
		"help"	:
			([
"topics":
@HELP
    市政大廳包括各種市民交流功能。
HELP,

"donate":
@HELP
捐獻金錢作為市政府資金使用，用法如下：
  donate '金錢'		- 捐獻金錢作為市政府資金使用
  donate		- 捐獻名單
HELP,
			]),
		"action":
			([
				"donate"	: (: do_donate :),
			]),
	]),
	
	"register" :
	([
		"short"	: HIY"戶政事務所"NOR,
		"help"	:
			([
"topics":
@HELP
    戶政事務所負責各項戶籍與企業管理業務。
HELP,

"register":
@HELP
登記註冊為本城市市民，用法如下：
  register		- 登記註冊為本城市市民
HELP,

"unregister":
@HELP
註銷市籍，用法如下：
  register		- 註銷市籍
HELP,

			]),
		"action":
			([
				"register"	: (: do_register :),
				"unregister"	: (: do_unregister :),
				"citizen"	: (: do_citizen :),
			]),
	]),

	"science"	:
	([
		"short"	: HIR"科技發展部"NOR,
		"help"	: "無說明文件。",
		"action":
			([

			]),
	]),
	
	"election"	:
	([
		"short"	: HIG"選舉委員會"NOR,
		"help"	:
			([
"topics":
@HELP
    選舉委員會負責各項投票相關業務。
HELP,

"register":
@HELP
登記或取消參選，用法如下：
  register		- 登記參選本次選舉(再按一次取消)
HELP,

"vote":
@HELP
投下神聖的一票，用法如下：
  vote '項目'		- 投下神聖的一票
HELP,

"manage":
@HELP
選舉管理指令，用法如下：
  manage new '主題'		- 新增投票主題
  manage delete 		- 刪除此次投票資料
  manage candidate		- 設定此選舉是否開放候選人登記(不自設投票項目)
  manage option '選項'		- 自設投票項目(不開放候選人登記)，再設定一次則刪除
  manage start '小時'		- 設定幾小時後投票開始，若設定為 0 即立刻開始投票(開放候選人登記時不可設為 0)
  manage end '小時'		- 設定幾小時後投票結束
  manage announce		- 發佈投票訊息並開始啟動投票機制
HELP,
			]),
		"heartbeat":1,
		"action":
			([
				"manage"	: (: do_election_manage :),
				"register"	: (: do_election_register :),
				"vote"		: (: do_election_vote :),
			]),
	]),

	"finance"	:
	([
		"short"	: HIM"財政部門"NOR,
		"help"	:
			([
"topics":
@HELP
    財政部門負責城市各項財政業務。
HELP,

"fund":
@HELP
設定財政經費，用法如下：
  fund			- 查證財政資料
  fund 河流 80		- 設定河流環境保育經費為 80%
  fund 地產稅 20	- 將地產稅調整為 20
HELP,
			]),
		"heartbeat":0,
		"action":
			([
				"fund"		: (: do_fund :),
				"refresh"	: (: do_refresh :),
			]),
	]),

	"mayor"	:
	([
		"short"	: HIW"市長辦公室"NOR,
		"help"	:
			([
"topics":
@HELP
    市長辦公室。
HELP,

"officer":
@HELP
任命或解除官員職位，用法如下：
  officer			- 檢視目前官員資料
  officer 'ID' vicemayor 1	- 任命某市民成為第一位衛都市副市長
  officer 'ID' '職稱'		- 任命某市民成為官員，職稱由市長設計
  officer 'ID' -d		- 解除某位官員的職務
  officer quit			- 官員請辭職務
HELP,

"abdicate":
@HELP
市長職位交接指令，用法如下：
  abdicate 'ID'		- 將市長職位交接給另一位市民
HELP,

"denominate":
@HELP
都市命名指令，用法如下：
  denominate -symbol 1 '徽記'	- 變更城市徽記第 1 行
  denominate -symbol 2 '徽記'	- 變更城市徽記第 2 行
  denominate -symbol 3 '徽記'	- 變更城市徽記第 3 行
  denominate '編號' '名稱'	- 設定某編號衛星都市的各別名稱
  denominate id '英文'		- 設定新的城市英文名稱 (只能變更色彩與大小寫)(需花費 5 億的市府資產)
  denominate name '中文'	- 設定新的城市中文名稱 (需花費 5 億的市府資產)
HELP,

"fire":
@HELP
開除某位市民的指令，用法如下：
fire '市民ID'			- 將某位沒有房地產的市民開除
HELP,
			]),
		"action":
			([
				"officer"	: (: do_officer :),
				"abdicate"	: (: do_abdicate :),
				"denominate"	: (: do_denominate :),
				"fire"		: (: do_fire :),
			]),
	]),
]);


// 設定建築物資料
nosave array building_info = ({
	
	// 建築物之中文名稱
	HIW"市政"NOR WHT"大樓"NOR

	// 開張此建築物之最少房間限制
	,6

	// 城市中最大相同建築數量限制(0 代表不限制)
	,1

	// 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
	,GOVERNMENT

	// 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
	,AGRICULTURE_REGION | INDUSTRY_REGION | COMMERCE_REGION

	// 開張儀式費用
	,"0"
	
	// 建築物關閉測試標記
	,0

	// 繁榮貢獻度
	,50
	
	// 最高可加蓋樓層
	,1
	
	// 最大相同建築物數量(0 代表不限制)
	,0
	
	// 建築物時代
	,1
});


