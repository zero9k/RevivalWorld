/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : score.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-6-29
 * Note   : score 
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <buff.h>

inherit COMMAND;

string help = @HELP
琩高à︹篈
    
score	- 琩高à︹膀セ篈
score s	- е硉琩高(箇砞 hp)
score m - 琩高à︹瞷籔蹿

score ㄏノ╰参罽糶 sc

HELP;

private string stat_symbol(int cur, int max)
{
	if( cur >= max)
		return NOR RED+cur+" / "HIR+max+NOR;
	else if( cur >= max*3/4 )
		return NOR YEL+cur+" / "HIY+max+NOR;
	else
		return NOR GRN+cur+" / "HIG+max+NOR;
}

private varargs string bonus(int bonus, string unit)
{
	if( bonus > 0 )
		return NOR"("CYN"+"HIC+bonus+(unit||"")+NOR")";
	else if( bonus == 0 )
		return "";
	else
		return NOR"("RED"-"HIR+(-bonus)+(unit||"")+NOR")";
}

private varargs string regen(int r)
{
	if( r > 0 )
		return NOR CYN"+"HIC+r+NOR;
	else if( r == 0 )
		return "";
	else
		return NOR RED"-"HIR+(-r)+NOR;
}

string score_standard(object ob)
{
	string msg;
	mapping dbase = ob->query_database();	

	string mycity = dbase["city"];
	string myenterprise = dbase["enterprise"];
	string mytitle = dbase["title"];
	string mynickname = dbase["nickname"];
	string myidname = ob->query_idname();
	int mytalent = dbase["exp"]["talent"];
	int myage = dbase["age"];
	
	int mystr = ob->query_str();
	int myphy = ob->query_phy();
	int myint = ob->query_int();
	int myagi = ob->query_agi();
	int mycha = ob->query_cha();
	
	int mystrbuff = ob->query_all_buff(BUFF_STR);
	int myphybuff = ob->query_all_buff(BUFF_PHY);
	int myintbuff = ob->query_all_buff(BUFF_INT);
	int myagibuff = ob->query_all_buff(BUFF_AGI);
	int mychabuff = ob->query_all_buff(BUFF_CHA);
	
	int mysocialcur = ob->query_social_exp_cur();
	int mysocialtot = ob->query_social_exp_tot();
	int mysocialexpbuff = myint*2-20+ob->query_all_buff(BUFF_SOCIAL_EXP_BONUS);
	
	int mystaminacur = ob->query_stamina_cur();
	int mystaminamax = ob->query_stamina_max();
	int mystaminamaxbuff = ob->query_all_buff(BUFF_STAMINA_MAX);
	int myhealthcur = ob->query_health_cur();
	int myhealthmax = ob->query_health_max();
	int myhealthmaxbuff = ob->query_all_buff(BUFF_HEALTH_MAX);
	int myenergycur = ob->query_energy_cur();
	int myenergymax = ob->query_energy_max();
	int myenergymaxbuff = ob->query_all_buff(BUFF_ENERGY_MAX);
	
	int myfoodcur = ob->query_food_cur();
	int myfoodmax = ob->query_food_max();
	int myfoodmaxbuff = ob->query_all_buff(BUFF_FOOD_MAX);
	int mydrinkcur = ob->query_drink_cur();
	int mydrinkmax = ob->query_drink_max();
	int mydrinkmaxbuff = ob->query_all_buff(BUFF_DRINK_MAX);

	msg = sprintf(@SCORE_STANDARD
[1;37mⅰ[0;1;30m
[m%-70s[1;30m
 [1;36m[0;36m篈 [1;30m
[m 秖 [1;30mStr[0m %-15s[1;30m[m 闹 %-4d   %-21s     [1;30m
[m 砰 [1;30mPhy[0m %-15s[1;30m[m ぱ结 %-4d  都 %-21s     [1;30m
[m 醇坝 [1;30mInt[0m %-15s[1;30m                                           [1;30m
[m 庇倍 [1;30mAgi[0m %-15s[1;30m[m                                           [1;30m
[m 緔 [1;30mCha[0m %-15s[1;30m[m 穦竒喷%-7s  %-25s[1;30m
                                                                      
[m 砰 %5d / %-12s%-40s%-4s[1;30m
[m ネ㏑ %5d / %-12s%-40s%-4s[1;30m
[m 弘 %5d / %-12s%-40s%-4s[1;30m
SCORE_STANDARD

        ,""+(CITY_D->query_city_name(mycity)||"礚瓣虑")+""+(ENTERPRISE_D->query_enterprise_color_id(myenterprise)||"")+(mytitle?" "+mytitle:"")+(mynickname?" "+mynickname:"")+" "+myidname
        ,mystr+bonus(mystrbuff), myage, stat_symbol(myfoodcur, myfoodmax)+bonus(myfoodmaxbuff)
        ,myphy+bonus(myphybuff), mytalent, stat_symbol(mydrinkcur, mydrinkmax)+bonus(mydrinkmaxbuff)
        ,myint+bonus(myintbuff)
        ,myagi+bonus(myagibuff)
        ,mycha+bonus(mychabuff), bonus(mysocialexpbuff, "%"), mysocialcur+" / "+mysocialtot
        ,mystaminacur, mystaminamax+bonus(mystaminamaxbuff), GRAPH_D->graph(mystaminacur,mystaminamax,2,0,20), regen(ob->query_all_buff(BUFF_STAMINA_REGEN))  
	,myhealthcur , myhealthmax+bonus(myhealthmaxbuff), GRAPH_D->graph(myhealthcur,myhealthmax,1,0,20), regen(ob->query_all_buff(BUFF_HEALTH_REGEN))
        ,myenergycur , myenergymax+bonus(myenergymaxbuff), GRAPH_D->graph(myenergycur,myenergymax,4,0,20), regen(ob->query_all_buff(BUFF_ENERGY_REGEN))
        );
        
        if( sizeof(dbase["condition"]) )
        {
        	msg += NOR WHT"                                                                      \n"NOR;
        	foreach( string condition, mapping data in dbase["condition"] )
        		msg += sprintf(NOR WHT" "NOR"%-23s %-45s"NOR WHT"\n"NOR, replace_string(condition,"#", "/")->query_condition_name(), HIW+(data["time"]/60)+NOR WHT" min "HIW+(data["time"]%60)+NOR WHT" sec"NOR );
        }
        
        if( sizeof(dbase["stock"]) )
        {
        	float ownstockvalue, curstockvalue;
        	string stockcolor, stockdesc;
        	msg += "                                                                      \n";
        	foreach( string enterprise, mapping data in dbase["stock"] )
        	{
        		if( wizardp(ob) && !ENTERPRISE_D->enterprise_exists(enterprise) )
        		{
        			delete("stock/"+enterprise, ob);
        			continue;
        		}
        			
        		ownstockvalue = data["value"];
        		curstockvalue = ENTERPRISE_D->query_enterprise_info(enterprise, "stockvalue");
        		
        		if( ownstockvalue > curstockvalue )
        		{
        			stockcolor = HIG;
        			stockdesc = "□";
        		}
        		else if( ownstockvalue < curstockvalue )
        		{
        			stockcolor = HIR;
        			stockdesc = "◆";
        		}
        		else
        		{
        			stockcolor = HIW;
        			stockdesc = "";
        		}

        		msg += sprintf(NOR WHT" "NOR"%-19s%8s  %-20s%-20s"NOR WHT"\n"NOR, ENTERPRISE_D->query_enterprise_color_id(enterprise), WHT +data["amount"] +NOR" 眎", "基 "HIW+sprintf("%.2f", ownstockvalue||0.0)+NOR, "ヘ玡基 "+stockcolor+sprintf("%.2f", curstockvalue||0.0)+stockdesc+NOR );
        	}
        }
        msg += "[1;37mⅱ"+CITY_D->query_age_name(query("city", ob))+"ⅲ[m\n";
        
        return msg;
}

#define MAX_MONEY	"999999999999999999999999999999999999999999999"
private string score_money(object ob)
{
	string bodymsg = "", bankmsg = "";
	mapping dbase = ob->query_database();
	mapping moneydata = MONEY_D->query_moneydata(ob->query_id(1));
	mapping cash_info;
	mapping bank_info;
	object room;
	
	if( mapp(moneydata) )
	{
		cash_info = moneydata["cash"];
		bank_info = moneydata["bank"];
	}
	
	if( mapp(cash_info) && sizeof(cash_info) )
		foreach( string unit in sort_array(keys(cash_info), 1) )
		{
			if( count(cash_info[unit], ">", MAX_MONEY) )
				set("money/"+unit, MAX_MONEY, ob);
			bodymsg += sprintf("[1;30m   [m$%-2s %-63s[0;1;30m\n", unit, NUMBER_D->number_symbol(cash_info[unit]));
		}
	else
		bodymsg += "[1;30m   [mō⊿Τヴ瞷                                                  [0;1;30m \n";

	if( mapp(bank_info) && sizeof(bank_info) )
	foreach( string bankfile, mixed data in bank_info )
	{
		if( !file_exist(bankfile+".o") || !objectp(room = load_object(bankfile)) )
		{
			//delete("banks/"+bankfile, ob);
			continue;
		}

		bankmsg += sprintf("[1;30m [m%-69s[0;1;30m\n", query("short", room) + (MONEY_D->is_bank_autotransfer(ob->query_id(1), bankfile)?" (笆锣眀"HIG"秨币"NOR")":" (笆锣眀"HIR"闽超"NOR")") );
		
		data = query("savings/"+ob->query_id(1), room);
		
		if( mapp(data) && mapp(data = data["money"]) )
		{
			foreach( string unit, string money in data )
				bankmsg += sprintf("[1;30m   [m$%-2s %-62s[0;1;30m \n", unit, NUMBER_D->number_symbol(money));
		}
	}
	else
		bankmsg += "[1;30m   [m蝗︽⊿Τヴ蹿                                                  [0;1;30m \n";

	return sprintf(@SCORE_STANDARD

[1;37mⅰ[0;1;30m
[m%-70s[1;30m
 [1;33m戈[0;33m [1;30m
[m [1;33m瞷[0;33m                                                               [0;1;30m  
%s
%s
[1;37mⅱ%sⅲ[m

SCORE_STANDARD
,""+(CITY_D->query_city_name(dbase["city"])||"礚瓣虑")+""+(ENTERPRISE_D->query_enterprise_color_id(dbase["enterprise"])||"")+(dbase["title"]?" "+dbase["title"]:"")+(dbase["nickname"]?" "+dbase["nickname"]:"")+" "+ob->query_idname()
        ,bodymsg[0..<2], bankmsg[0..<2], CITY_D->query_age_name(dbase["city"]));
}

string score_simple(object ob)
{
	return sprintf("ō砰篈"HIG"砰[%d/%d]"NOR""HIR"ネ㏑[%d/%d]"NOR""HIB"弘[%d/%d]"NOR"\n"
		,ob->query_stamina_cur(), ob->query_stamina_max()
		,ob->query_health_cur(), ob->query_health_max()
		,ob->query_energy_cur(), ob->query_energy_max()); 
}

private void do_command(object me, string arg)
{
        object ob;
        string msg, arg2;

	if( arg && arg != "s" && arg != "m" )
	{
		if( wizardp(me) )
		{
			if( !objectp(ob = find_player(arg) || present(arg)) || !ob->is_living()  )
			{
				if( sscanf(arg, "%s %s", arg, arg2) == 2 )
				{
					if( objectp(ob = find_player(arg) || present(arg)) && ob->is_living() )
						arg = arg2;
					else
						ob = me;
				}
				else
					ob = me;
			}
		}
		else if( objectp(ob = present(arg)) && ob->is_living() )
		{
			if( query("boss", ob) != me->query_id(1) )
				return tell(me, ob->query_idname()+"ぃ琌"+pnoun(2, me)+"侗ノ骋\n");
		}
		else
			ob = me;
	}
	else
		ob = me;	

	switch(arg)
	{
		case "m":
		case "money":
			if( ob->is_npc() )
				return tell(me, "礚猭琩高"+ob->query_idname()+"窥戈\n");

			msg = score_money(ob);
			break;
		
		case "s":
		case "simple":
			msg = score_simple(ob);
			break;
		default:
			msg = score_standard(ob);
			break;
	}
	
	tell(me, msg);
}                             