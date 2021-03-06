/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : labor.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-03
 * Note   : 人力介紹所物件
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <feature.h>
#include <message.h>
#include <daemon.h>
#include <citydata.h>
#include <gender.h>
#include <object.h>

inherit ROOM_ACTION_MOD;

#define LABOR_RENT	"100000"

#define LABOR_ID	"ID"
#define LABOR_IDNAME	"IDNAME"
#define LABOR_AGE	"AGE"
#define LABOR_SKILLS	"SKILLS"

// 雇用人力
void do_hire(object me, string arg)
{
	int num, i;
	object newnpc;
	object env = environment(me)->query_master();
	int leadership_level;
	int leadership_adv_level;
	string money_unit = env->query_money_unit();

	if( !arg )
		return tell(me, "每個人力介紹費需要 "HIY+money(money_unit, LABOR_RENT)+NOR"。\n"+pnoun(2, me)+"想要雇用全新人力(Boy/Girl)？或是雇用有經驗的人力(list 列表)？\n");

	arg = lower_case(arg);

	leadership_level = me->query_skill_level("leadership");
	leadership_adv_level = me->query_skill_level("leadership-adv");
	
	if( sizeof(query("hirelabors", me)) >= (leadership_level + leadership_adv_level)/5 )
		return tell(me, pnoun(2, me)+"目前的領導能力最多只能雇用 "+ ((leadership_level + leadership_adv_level)/5)+" 人。\n");

	if( !me->spend_money(money_unit, LABOR_RENT) )
		return tell(me, pnoun(2, me)+"沒有錢雇用人力了。\n");

	switch(arg)
	{
	case "b":
	case "boy":
		newnpc = load_object("/npc/"+SERIAL_NUMBER_D->query_serial_number("labor"));
		set("gender", MALE_GENDER, newnpc);
		set("birthday", TIME_D->query_game_time(), newnpc);
		newnpc->set_idname("poor boy", "無家可歸的小男孩");
		break;
	case "g":
	case "girl":
		newnpc = load_object("/npc/"+SERIAL_NUMBER_D->query_serial_number("labor"));
		set("gender", FEMALE_GENDER, newnpc);
		set("birthday", TIME_D->query_game_time(), newnpc);
		newnpc->set_idname("poor girl", "無家可歸的小女孩");
		break;
	default:
		if( (num = to_int(arg)) > 0 )
		{
			foreach( string file, array info in LABOR_D->query_labors() )
			{
				if( ++i == num )
				{
					newnpc = load_object(file);
					break;
				}	
			}

			if( !objectp(newnpc) )
				return tell(me, "請輸入正確的編號。\n");
			else
				LABOR_D->remove_labor(base_name(newnpc));
		}
		else
			return tell(me, pnoun(2, me)+"想要雇用男的還是女的(Boy/Girl)或是輸入 list 的編號？\n");
	}


	set("boss", me->query_id(1), newnpc);

	newnpc->save();
	newnpc->move_to_environment(me);

	msg("$ME花了 "HIY+money(money_unit, LABOR_RENT)+NOR" 元雇用了一個人力，只見$YOU緩緩從門後走了出來。\n", me, newnpc, 1);
	newnpc->do_command("follow "+me->query_id(1));
	newnpc->do_command("say 謝謝"+me->query_name()+HIG"願意雇用我，我一定會好好做事的...\n"NOR);

	set("hirelabors", (query("hirelabors", me)||({})) | ({base_name(newnpc)}), me);
	set("hired", count(query("hired", env), "+", 1), env);

	env->save();
	log_file("command/hire", sprintf("%s hired %O\n", me->query_id(1), newnpc));
}

private void confirm_fire(object me, object npc, string arg)
{
	if( lower_case(arg) == "n" || lower_case(arg) == "no" )
	{
		int suicide;
		string basename;
		string idname;

		msg("$ME哼了一聲說道：叫"+pnoun(2, npc)+"滾有沒有聽到！？我叫"+pnoun(2, npc)+"滾吶！！\n", me, 0, 1);
		npc->do_command("cry");
		msg("$YOU傷心欲絕地走進"+query("short", environment(me))+"的後門，消失在$ME眼前。\n", me, npc, 1);

		// 進入 LABOR_D 等待其他人雇用
		LABOR_D->add_labor(npc);

		basename = base_name(npc);
		idname = npc->query_idname();

		// NPC 自殺, 永遠消失
		if( (query("relationship/"+me->query_id(1), npc) > 500 && random(5)) || !random(10) )
			suicide = 1;

		log_file("command/fire", sprintf("%s fired %O\n", me->query_id(1), npc ));

		npc->move(VOID_OB);
		destruct(npc);
		me->save();
		me->finish_input();

		if( suicide )
		{	
			broadcast(environment(me), 
			    HIY"只聽見大門口外突然傳來路人們的驚呼，"+idname+HIY"從此處的樓頂跳了下來，自殺了。\n"
			    "只見"+idname+HIY"手上握著一張沾滿"HIR"血"NOR RED"跡"HIY"的字條，草草寫了幾個字：「我不會怪老闆的，一切都是我自己不好...」。\n"NOR
			);

			LABOR_D->remove_labor(basename);
			rm(basename+".o");
		}

	}
	else
	{
		msg("$ME說道：好吧！就讓"+pnoun(2, npc)+"繼續跟在我身旁吧！\n", me, 0, 1);
		npc->do_command("jump");
		npc->do_command("say 老闆的大恩大德我永遠都不會忘記的！^_^");
		me->finish_input();
	}
}

// 開除人力
void do_fire(object me, string arg)
{
	object npc;

	if( !arg )
		return tell(me, pnoun(2, me)+"想要開除誰？\n");

	if( !objectp(npc = present(arg, environment(me))) )
		return tell(me, "這裡沒有 "+arg+" 這個人。\n");

	if( !npc->is_living() )
		return tell(me, pnoun(2, me)+"想要開除"+npc->query_idname()+"？直接丟掉不是比較快？\n");

	if( query("boss", npc) != me->query_id(1) )
		return tell(me, npc->query_idname()+"並不是你雇用的人力。\n");

	msg("$ME吩咐$YOU道："+pnoun(2, npc)+"走吧，我已經不再需要"+pnoun(2, npc)+"了。\n", me, npc, 1);
	npc->do_command("sob");
	npc->do_command("say 求求"+pnoun(2, me)+"！讓我繼續在"+pnoun(2, me)+"身旁做事好嗎？沒有這份工作我活不下去阿！");
	tell(me, "\n要繼續留下"+npc->query_idname()+"替"+pnoun(2, me)+"工作嗎？(Yes/No)");
	set_this_player(me);
	input_to((:confirm_fire, me, npc:));
}

void do_list(object me, string arg)
{
	int i, skill_num;
	int total_skill_level;
	string msg, skill;
	mapping skills, data;
	object ob;
	mapping labors = LABOR_D->query_labors();

	if( !sizeof(labors) )
		return tell(me, "目前沒有任何有經驗的人力可以雇用。\n");

	if( arg && (i = to_int(arg)) && i > 0 && i <= sizeof(labors) )
	{
		foreach(string file, mapping info in labors)
		{
			if( !(--i) )
			{
				ob = load_object(file);
				msg = COMMAND_D->find_command_object("score")->score_standard(ob);
				msg += COMMAND_D->find_command_object("skill")->skill_list(me, ob);
				
				me->more(msg);
				return;
			}
		}
	}
	
	i = 0;

	msg =  NOR WHT"─────────────────────────────────────\n"NOR;
	msg +=        "編號 人力名稱                         性別 技能數 等級總合 要求薪資(月)\n";
	msg += NOR WHT"─────────────────────────────────────\n"NOR;

	foreach( string file, mapping info in labors )
	{
		ob = load_object(file);

		skills = query("skills", ob);

		skill_num = 0;
		total_skill_level = 0;
		if( mapp(skills) )
		foreach( skill, data in skills )
		{
			skill_num++;

			total_skill_level += data["level"];
		}
		msg += sprintf("%-3d. %-33s"HIW" %s  "HIG"%-6d"HIC" %-9d"HIY"$%s\n"NOR, ++i, ob->query_idname(), query("gender", ob)==1?"男":"女", skill_num, total_skill_level, NUMBER_D->number_symbol(SALARY_D->query_labor_salary(ob)));
	}

	msg += NOR WHT"─────────────────────────────────────\n"NOR;
	msg += HIW"list '編號' "NOR WHT"查詢詳細屬性與技能資料\n"NOR;

	me->more(msg);
}
// 設定建築物內房間型態種類
nosave mapping action_info =
([
    "lobby"	:
    ([
	"short"	: NOR GRN"介紹所"NOR,
	"help"	:
	([
	    "topics":
@HELP
    介紹所提供工作與戰鬥人力的供給。
HELP,

	    "hire":
@HELP
雇用人力的指令，用法如下：
  hire boy		- 雇用一位全新的男性人力
  hire girl		- 雇用一位全新的女性人力
  hire 2		- 雇用編號為 2 的已存在人力
HELP,

	    "fire":
@HELP
開除人力的指令，用法如下：
  fire john		- 將名子叫 john 的人力開除
HELP,

	    "list":
@HELP
列出目前已存在的人力，用法如下：
  list			- 列出所有目前已存在但無人雇用的人力
  list '編號'		- 查詢該人力的屬性與技能資料
HELP,
	]),
	"action":
	([
	    "hire"		: (: do_hire :),
	    "fire"		: (: do_fire :),
	    "list"		: (: do_list :),
	]),
    ]),
]);


// 設定建築物資料
nosave array building_info = ({

    // 建築物之中文名稱
    HIG"人力介紹所"NOR

    // 開張此建築物之最少房間限制
    ,1

    // 城市中最大相同建築數量限制(0 代表不限制)
    ,0

    // 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
    ,GOVERNMENT

    // 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
    ,AGRICULTURE_REGION | INDUSTRY_REGION | COMMERCE_REGION

    // 開張儀式費用
    ,"5000000"

    // 建築物關閉測試標記
    ,0

    // 繁榮貢獻度
    ,20

    // 最高可加蓋樓層
    ,1

    // 最大相同建築物數量(0 代表不限制)
    ,0

    // 建築物時代
    ,1
});
