/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : wiz_ob.c
 * Author : Clode@RevivalWorld
 * Date   : 2000-12-29
 * Note   : 巫師使用者物件主檔
 * Update :
 *  o 2000-00-00  

 -----------------------------------------
 */

#include <daemon.h>
#include <inherit.h>

inherit INTERACTIVE_WIZ;

int is_user_ob()
{
	return 1;
}

private nomask void dest()
{
	LOGOUT_D->quit(this_object());
}

private nomask void net_dead()
{
	set_temp("net_dead", call_out( (: dest :), 300 ));
	set_temp("net_dead_time", time());
        LOGOUT_D->net_dead(this_object());
}

nomask void reconnect()
{
	if( query_temp("net_dead") )
	{
		remove_call_out(query_temp("net_dead"));
		delete_temp("net_dead");
		// Net dead time delete at login_d.c
	}
}

nomask string query_save_file()
{
	if( this_object()->is_living() )
		return file_name(this_object())+"/data.o";
	
	return 0;
}

private void write_prompt()
{
	// 不加這個會造成 prompt 顯示問題
}

private void create()
{

}
