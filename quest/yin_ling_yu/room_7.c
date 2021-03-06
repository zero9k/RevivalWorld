/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_7.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-12-19
 * Note   : 標準房間
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <quest.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

void create()
{
	set("short", HIW"隱密"NOR WHT"山洞"NOR);
	set("long",@TEXT
    這裡是一個極為隱密的山洞，裡面一片漆黑，往手邊摸去都是冷冰冰且潮濕的
石璧，頭頂上也不時地滴下水來，不過往裡頭一看，似乎格局還挺方正的，地上也
可以偶爾見到幾株正在生長的野山人參，而在這個方正的山洞正中央則有一個很顯
著的[1;34m藍玉石柱[m直直插在地上，完美無暇的玉石讓人看得目瞪口呆。
TEXT);
	
	set("exits", ([
		"north" 	: "/quest/yin_ling_yu/room_6",
	]));

	::reset_objects();
}

void do_push(object me, string arg)
{
	mapping stone;

	if( arg != "左" && arg != "右" )
		return tell(me, pnoun(2, me)+"想要把石柱往左右哪個方向推(左, 右)？\n");
		
	if( !query_temp("/quest/yin_ling_yu/stone/red", me) )
		return tell(me, pnoun(2, me)+"使出吃奶的力量也推不動這個石柱。\n");

	if( lower_case(arg) == "左" )
	{
		set_temp("/quest/yin_ling_yu/stone/blue", 1, me);
		msg("$ME用力將石柱往左邊推，只感覺到地板下似乎有東西在震動。\n", me, 0, 1);
	}
	else if( lower_case(arg) == "右" )
	{
		set_temp("/quest/yin_ling_yu/stone/blue", 2, me);
		msg("$ME用力將石柱往右邊推，只感覺到地板下似乎有東西在震動...。\n", me, 0, 1);
	}

	stone = query_temp("/quest/yin_ling_yu/stone", me);
	
	if( stone["green"] == 1 && stone["yellow"] == 2 && stone["red"] == 2 && stone["blue"] == 2 )
	{
		msg("$ME腳底下的石板突然鬆開，$ME就這樣掉了下去，石板隨即又關了起來。\n", me, 0, 1);
		me->move("/quest/yin_ling_yu/room_8");
		me->set_quest_note(QUEST_YIN_LING_YU_2, QUEST_YIN_LING_YU_2_NAME, "破解了山洞中的機關，進入到石洞底部。");
		me->set_quest_step(QUEST_YIN_LING_YU_2, 4);
		delete_temp("/quest/yin_ling_yu/stone", me);
	}
	else
	{
		delete_temp("/quest/yin_ling_yu/stone", me);
		msg("$ME的眼前突然一陣模糊，消失了。\n", me, 0, 1);
		me->move(({ 81, 71, 0, "lightforest", 8, 0, "81/71/0/lightforest/8/0" }));
		msg("$ME的眼前突然一陣模糊，眼睛一亮就跑到了這裡。\n", me, 0, 1);
		
		if( !random(5) )
			me->faint();
	}
}

mapping actions = 
([
	"push" : (: do_push :),
]);