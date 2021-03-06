/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : infodata.c
 * Author : 
 * Date   : 2005-10-04
 * Note   : infodata 指令
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
指令格式 : infodata <物件之名稱或檔名>

利用此一指令可得知一些有關該物件的資訊。
HELP;

private string query_info( object ob )
{
	string str = "";
	if( userp(ob) )                str += "使用者 ";
	if( interactive(ob) )  		   str += "交談式 ";
	if( wizardp(ob) )              str += "巫師 ";
	if( clonep(ob) )               str += "複製 ";
	if( virtualp(ob) )             str += "虛擬 ";
	if( query_heart_beat(ob) )     str += "心跳:" + query_heart_beat(ob) + " ";

	return str;
}

private void command(object me, string arg)
{
	object obj;
	string str;

	if( !is_command() ) return;

	if (!arg) 
		return tell( me, "指令格式 info <me|here|obj>\n", CMDMSG);

	if( arg=="me" ) obj = me;
	else 
	{
		intact_file_name(ref arg);
		obj = present(arg, me);
		if (!obj) obj = present(arg, environment(me));
		if (!obj) obj = find_object( resolve_path(me, arg) );
		if (!obj) return tell( me, "沒有這樣物件....。\n", CMDMSG);
	}

	str = sprintf( "\n" + @DATA
物件 [%O]
-----------------------------------------------------
檔案﹕          %s.c
等級﹕          %s
使用記憶體﹕    %d
屬性﹕		%s
複製個數﹕      %d
參考連結﹕      %d

DATA, 
	    obj,
	    base_name(obj),
	    SECURE_D->level_name(obj->query_id(1)),
	    memory_info(obj),
	    query_info(obj),
	    sizeof(children(base_name(obj)+".c")),
	    refs(obj)
	);

	return tell( me, str , CMDMSG );
}
