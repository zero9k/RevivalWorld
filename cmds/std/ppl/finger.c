/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : finger.c
 * Author : Clode && Cookys
 * Date   : 2001-6-10
 * Note   : finger 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>

inherit COMMAND;

string help = @HELP
使用者查詢指令

這個指令可以查詢使用者資訊，如暱稱、代號、權限、連線時間、狀態 ... 等。

指令格式:
finger <使用者代號>
	
相關指令: chfn, who
HELP;

private void do_command(object me, string arg)
{
	if( !arg )
	{
		if( wizardp(me) ) 
			me->more(FINGER_D->finger_all()+"\n");
		else
			tell(me, pnoun(2, me)+"想要查閱誰的資料？\n");
	}
	else
	{
		arg = replace_string(arg, "#", "");
		tell(me, FINGER_D->finger_user(arg));
	}
}
