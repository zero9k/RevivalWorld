/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : dump.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-02-01
 * Note   : dump 指令。
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <feature.h>

inherit COMMAND;

string help = @HELP
        標準 dump 指令。
HELP;

private void command(object me, string arg)
{
	dumpallobj(arg || "/log/OBJ_DUMP");
	tell(me, "傾印載入物件之統計資料列表完畢。\n");
}