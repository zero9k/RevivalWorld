/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : idle.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-04
 * Note   : 發呆技能
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <skill.h>
#include <inherit.h>
#include <message.h>
#include <daemon.h>

inherit STANDARD_SKILL;

void create()
{
	// 技能中文
	set("name", 		"發呆技能");

	// 技能英文
	set("id",		"idle");

	// 技能頭銜
	set("title/100",	HIW"老骨灰"NOR WHT"宗師"NOR);
	
	// 技能註解
	set("note",		"降低腦部活動進入唯我寧靜冥想的技術");
}

// 限制可學習對象, 可用來設定各種學習條件
varargs int allowable_learn(object ob)
{
	if( undefinedp(ob) )
		return 0;
	else
	{
		if( ob->is_npc() )
			return 0;
		else
			return 0;
	}
}

// 可自設每個等級需要的經驗值, 或直接使用 SKILL_D 預設數學式
int level_exp(int lv)
{
	return SKILL_D->query_level_exp(lv);
}

// 可設定各種最高等級條件, 
int max_level(object ob)
{
	return 100;
}

// 可設定不同條件之經驗值乘數
float exp_multiple()
{
	return 1.0;
}

// 升級之前的事件
void after_level_up(object ob, int newlevel)
{
	if( newlevel == 100 )
	{
		ob->add_str(3);
		ob->add_phy(3);
		ob->add_int(3);
		ob->add_agi(3);
		ob->add_cha(3);
		tell(ob, HIY+pnoun(2, ob)+"的力量在發呆技能到達頂級時瞬間提升了 3 點！！！\n"NOR);
		tell(ob, HIY+pnoun(2, ob)+"的體格在發呆技能到達頂級時瞬間提升了 3 點！！！\n"NOR);
		tell(ob, HIY+pnoun(2, ob)+"的智商在發呆技能到達頂級時瞬間提升了 3 點！！！\n"NOR);
		tell(ob, HIY+pnoun(2, ob)+"的敏捷在發呆技能到達頂級時瞬間提升了 3 點！！！\n"NOR);
		tell(ob, HIY+pnoun(2, ob)+"的魅力在發呆技能到達頂級時瞬間提升了 3 點！！！\n"NOR);
		tell(ob, HIY+"此後發呆所得技能經驗值將持續自動轉為社會經驗值！！！\n"NOR, ENVMSG);
	}
}

// 降級之後的事件
void before_level_down(object ob, int oldlevel)
{
	if( oldlevel == 100 )
	{
		ob->add_str(-3);
		ob->add_phy(-3);
		ob->add_int(-3);
		ob->add_agi(-3);
		ob->add_cha(-3);
		tell(ob, HIR+pnoun(2, ob)+"的力量在發呆技能降級時瞬間降低了 3 點！！！\n"NOR);
		tell(ob, HIR+pnoun(2, ob)+"的體格在發呆技能降級時瞬間降低了 3 點！！！\n"NOR);
		tell(ob, HIR+pnoun(2, ob)+"的智商在發呆技能降級時瞬間降低了 3 點！！！\n"NOR);
		tell(ob, HIR+pnoun(2, ob)+"的敏捷在發呆技能降級時瞬間降低了 3 點！！！\n"NOR);
		tell(ob, HIR+pnoun(2, ob)+"的魅力在發呆技能降級時瞬間降低了 3 點！！！\n"NOR);
	}
}

// 經驗值增加後的事件
void added_exp(object ob, float exp)
{
	// 發呆技能滿了之後, exp 平均分配到其他技能
	if( ob->query_skill_level("idle") == 100 )
	{
		ob->add_social_exp(to_int(exp));
	}
	// 還未滿
	else
	{
		// 附加倒扣 strength 技能
		ob->add_skill_exp("strength", -exp/30.);

		// 附加倒扣 physique 技能
		ob->add_skill_exp("physique", -exp/30.);
			
		// 附加倒扣 intelligence 技能
		ob->add_skill_exp("intelligence", -exp/30.);

		// 附加倒扣 agility 技能
		ob->add_skill_exp("agility", -exp/30.);
				
		// 附加倒扣 charisma 技能
		ob->add_skill_exp("charisma", -exp/30.);
	}
}

// 是否只能自我學習
int self_learning()
{
	return 1;
}

// 特殊標記
string query_special_type()
{
	return HIG"傳"NOR GRN"奇"NOR;
}
