/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : force.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-02-05
 * Note   : force 指令
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
	標準 force 指令。
HELP;

private void command(object me, string arg)
{
	string cmd;
	object you;
	if( !is_command() ) return;
	
        if( !arg || sscanf(arg,"%s to %s",arg, cmd) != 2 )
		return tell(me, "輸入格式: force 人物 to 指令。\n", CMDMSG);
	
	you = find_player(arg) || present(arg);
	
	if( !objectp(you) || !you->is_living() )
		return tell(me, "沒有 "+arg+" 這個人。\n", CMDMSG);

	if( !userp(you) && you->is_npc() )
	{
		you->do_command(cmd);
		return;
	}

	if( SECURE_D->level_num(me->query_id(1)) < SECURE_D->level_num(you->query_id(1)) )
		return tell(me, "你沒有權限強迫 "+you->query_idname()+" 執行指令。\n", CMDMSG);
		
	you->force_me(cmd);
}
