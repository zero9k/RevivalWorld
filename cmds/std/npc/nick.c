/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : nick.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-10-14
 * Note   : nick 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
取個人綽號的指令

nick '綽號'

HELP;

private void do_command(object me, string arg)
{
	if( !arg )
	{
		if( me->is_npc() )
			return me->do_command("say 老闆想要我取什麼樣的綽號？\n");
		else
			return tell(me, pnoun(2, me)+"想要取什麼樣的綽號？\n");
	}

	if( lower_case(arg) == "-d" )
	{
		delete("nickname", me);
		me->save();
		return msg("$ME將綽號取消了。\n", me, 0, 1);
	}
	
	if( noansi_strlen(arg) > 16 )
	{
		if( me->is_npc() )
			return me->do_command("say 綽號最多只能有 16 個字元喔。\n");
		else
			return tell(me, "綽號最多只能有 16 個字元喔。\n");
	}
	
	arg = kill_repeat_ansi(arg+NOR);
	
	set("nickname", arg, me);
	me->save();
	msg("$ME將綽號設定為「"+arg+"」。\n", me, 0, 1);
}