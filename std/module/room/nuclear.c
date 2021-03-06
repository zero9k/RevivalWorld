/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : nuclear.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-02-24
 * Note   : 核子反應爐
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


// 設定建築物內房間型態種類
nosave mapping action_info =
([
	"jail"	:
	([
		"short"	: HIR"核子"NOR RED"反應爐"NOR,
		"help"	:
			([
"topics":
@HELP
    用來囚禁犯人的地方。
HELP,

"arrest":
@HELP
逮捕某位玩家，用法如下：
  arrest '玩家ID'		- 將某位線上市民逮捕(市長指令)
HELP,

"release":
@HELP
釋放某位玩家，用法如下：
  release '玩家ID'		- 將某位被關在監獄中的市民釋放(市長與官員指令)
HELP,

			]),
		"action":
			([

			]),
	]),
]);


// 設定建築物資料
nosave array building_info = ({
	
	// 建築物之中文名稱
	HIR"核子"NOR RED"反應爐"NOR

	// 開張此建築物之最少房間限制
	,1

	// 城市中最大相同建築數量限制(0 代表不限制)
	,0

	// 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
	,GOVERNMENT

	// 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
	,INDUSTRY_REGION

	// 開張儀式費用
	,"99900000000"
	
	// 建築物關閉測試標記
	,1

	// 繁榮貢獻度
	,-999
	
	// 最高可加蓋樓層
	,1
	
	// 最大相同建築物數量(0 代表不限制)
	,0
	
	// 建築物時代
	,5
});

