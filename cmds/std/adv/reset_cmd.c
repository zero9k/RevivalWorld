/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : reset_cmd.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-5
 * Note   : reset_cmd 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 reset_cmd 指令。
HELP;

private void command(object me, string arg)
{
	if( !is_command() ) return;

	if( COMMAND_D->do_reset() )
		tell(me, "重新載入指令集完畢。\n", CMDMSG);
	else
		tell(me, "重新載入指令集失敗。\n", CMDMSG);
}