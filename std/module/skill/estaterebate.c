/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : estaterebate.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-05-01
 * Note   : 地產回收
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
	set("name", 		"地產仲介");

	// 技能英文
	set("id",		"estaterebate");

	// 技能頭銜
	set("title/40",		HIG"地產仲介員"NOR);
	set("title/100",	HIG"地產"NOR GRN"仲介師"NOR);
	
	// 技能註解
	set("note",		"可提高房地產回收價格(20%->70%)");
}

// 限制可學習對象, 可用來設定各種學習條件
varargs int allowable_learn(object ob)
{
	if( undefinedp(ob) )
		return PLAYER_SKILL;
	else
	{
		if( ob->is_npc() )
			return 0;
		else
			return 1;
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
	return 0.2;
}

// 升級之前的事件
void after_level_up(object ob, int newlevel)
{

}

// 降級之後的事件
void before_level_down(object ob, int oldlevel)
{
	
}

// 經驗值增加後的事件
void added_exp(object ob, float exp)
{

}

// 是否只能自我學習
int self_learning()
{
	return 0;
}

// 特殊標記
string query_special_type()
{
	return "";
}
