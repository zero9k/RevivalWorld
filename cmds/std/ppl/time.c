/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : time.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-09-29
 * Note   : time 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <daemon.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
查詢時間

這個指令可以查詢重生的世界中的日期與時間

相關指令: date
HELP;

private void do_command(object me, string arg)
{
       	tell(me, TIME_D->game_time_description()+"。\n", CMDMSG);
}
