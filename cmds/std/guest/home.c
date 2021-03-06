/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : home.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-3-27
 * Note   : home 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 home 指令。
HELP;

private void command(object me, string arg)
{
	string file;
	object home;

	if( !is_command() ) return;
	
	file = wiz_home(arg || me->query_id(1))+"/workroom";
	
	intact_file_name(ref file);
	
	if( file_size(file) == -1 )
		return tell(me, "這個人沒有工作室。\n", CMDMSG);

	home = load_object(file);
	
	if( environment(me) == home ) 
		return tell(me, pnoun(2, me)+"已經在"+(arg?" "+capitalize(arg)+" ":"自己")+"的工作室了。\n", CMDMSG);
		
	msg("$ME化作一陣"HIG"綠"NOR GRN"光"NOR"往"+(arg?" "+capitalize(arg)+" ":"自己")+"的工作室飛了過去。\n",me, 0, 1, ENVMSG);

	me->move(home);

	msg("$ME化作一陣"HIG"綠"NOR GRN"光"NOR"往"+(arg?" "+capitalize(arg)+" ":"自己")+"的工作室飛了過來。\n",me, 0, 1, ENVMSG, ({me}));
}