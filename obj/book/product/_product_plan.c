/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _product_plan.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-03-15
 * Note   : 產品設計圖
 * Update :
 *  o 2003-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <skill.h>

inherit STANDARD_OBJECT;

#define COST_SOCIAL_EXP	100000

mapping actions;
	
void do_readplan(object me, string arg)
{
	int not_enough_skill;
	string product = query("product");
	string module = "/std/module/product/"+product+"/module.c";
	mapping product_info;
	
	if( !module || !load_object(module) ) return;
		
	product_info = module->query_product_info();

	foreach(string sk, int level in product_info["skill"])
	{
		if( me->query_skill_level(sk) < level )
		{
			not_enough_skill = 1;
			tell(me, pnoun(2, me)+"的「"+(SKILL(sk))->query_idname()+"」技能等級不足 "+level+" 級，看不懂"+this_object()->query_idname()+"。\n");
		}
	}
	
	if( not_enough_skill )
		return;

	if( !me->cost_social_exp(COST_SOCIAL_EXP) )
		return tell(me, pnoun(2, me)+"的社會經驗值不足 "+COST_SOCIAL_EXP+"，無法進行學習。\n");

 	msg("$ME開始認真讀起$YOU，只見$ME豁然開朗似的不斷的點頭，似乎從裡頭得到了很多的東西。\n", me, this_object(), 1);

	msg("$ME學會了「"+product_info["name"]+"("+capitalize(product_info["id"])+")」的製作方法。\n", me, 0, 1);
	
	set("learned_products", (copy(query("learned_products", me))||allocate(0)) | ({ product }), me);
	me->save();

	msg("$YOU在$ME閱讀完後便化為碎片。\n", me, this_object(), 1);

	destruct(this_object(), 1);
}

void create()
{
	string product = query("product");
	string module = "/std/module/product/"+product+"/module.c";
	mapping product_info;
	
	if( !module || !load_object(module) ) return;
		
	product_info = module->query_product_info();

	set_idname(HIW+product_info["id"]+NOR CYN" plan"+NOR, HIW+product_info["name"]+NOR CYN"產品設計圖"NOR);
	set_temp("status", HIC"稀"NOR CYN"有");

	actions = ([ "readplan" : (: do_readplan :) ]);

	if( this_object()->set_shadow_database() ) return;
	
	set("long", "一本極為稀有的"+product_info["name"]+"("+capitalize(product_info["id"])+")產品設計圖，或許您能從中學習到一些特殊的產品生產方法。\n\n  ");
	set("unit", "張");
	set("mass", 100);
	set("value", 10000);
	set("badsell", 1);
}
