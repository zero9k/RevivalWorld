/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : version.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-4-20
 * Note   : version 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <mudlib.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
查詢系統版本

這個指令可以查詢目前重生的世界所使用的程式庫及驅動程式版本
HELP;

private void do_command(object me, string arg)
{
	string str;
	
	str =  "遊戲名稱 : "+MUD_FULL_NAME+"\n";
	str += "程式庫   : "+MUDLIB_NAME+" "+MUDLIB_VERSION_NUMBER+"\n";
	str += "驅動程式 : "+__VERSION__+" on "+__ARCH__+"\n";
	tell(me, str, CMDMSG);
}