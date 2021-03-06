/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _command.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-5
 * Note   : 指令繼承檔案
 * Update :
 *  o 2001-08-01 Clode 利用函式指標執行指令
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <origin.h>

private void command(object me, string arg);

private void do_command(object me, string arg)
{
	if( call_stack(1)[<1] == me || call_stack(1)[<2] == me || call_stack(1)[4] == me && call_stack(2)[5] == "force_me" ) 
		command(me, arg);
	else
		printf("權限檢查:\n%O\n%O\n%O\n指令使用權限不足，請恰詢管理者。\n", call_stack(1), call_stack(2), call_stack(3));
}

nomask function query_fp()
{
	return is_daemon(previous_object()) ? (: do_command($1, $2) :) : 0;
}