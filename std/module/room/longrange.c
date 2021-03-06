/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : longrange.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-03-16
 * Note   : 遠距武器廠
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
#include <npc.h>

inherit ROOM_ACTION_MOD;

inherit __DIR__"inherit/_mod_factory.c";

// 設定建築物內房間型態種類
nosave mapping action_info =
([
	"line"	:
	([
		"short"	: HIM"生產"NOR MAG"線"NOR,
		"heartbeat":30,	// 實際時間 1 秒為單位
		"job": WORKER,
		"master":1,
		"help"	:
			([
"topics":
@HELP
    生產線提供產品的製造與科技研發。
HELP,

"line":
@HELP
生產線控制指令，用法如下：[管理指令]
  line 2			- 設定製造生產研發中心編號為 2 的產品
  line start			- 啟動生產線的運作
  line stop			- 停止生產線的運作
HELP,

"setup":
@HELP
設定生產線資料的指令，用法如下：[管理指令]
  setup reserach 1 21,33 	- 設定全球研發中心座標在第一衛星都市的 21,33
  setup warehouse 1 75,90	- 設定倉庫中心座標在第一衛星都市的 75,90
HELP,

"list":
@HELP
列出可生產的產品資訊，用法如下：
  list			 	- 列出可生產的產品資訊
HELP,
			]),
		"action":
			([
				"line"		: (: do_line_action($1, $2) :),
				"list"		: (: do_list_action($1, $2) :),
				"setup"		: (: do_setup_action($1, $2) :),
			]),
	]),
]);

// 設定建築物資料
nosave array building_info = ({
	
	// 建築物之中文名稱
	HIR"遠距"NOR RED"武器廠"NOR

	// 開張此建築物之最少房間限制
	,4

	// 城市中最大相同建築數量限制(0 代表不限制)
	,0

	// 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
	,INDIVIDUAL

	// 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
	,INDUSTRY_REGION

	// 開張儀式費用
	,"15000000"

	// 建築物關閉測試標記
	,1

	// 繁榮貢獻度
	,9
	
	// 最高可加蓋樓層
	,2
	
	// 最大相同建築物數量(0 代表不限制)
	,0
	
	// 建築物時代
	,2
});
