/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : flag.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-07-05
 * Note   : included by Master Object
 * Update :
 *  o 2002-00-00

 -----------------------------------------
 */

/* 處理驅動程式啟動時所指定的 mudlib 特定旗標 */
/* 用來 debug... 平時無需載入 */
void flag( string arg )
{
	string file, func;
	if( previous_object() ) return;
	
	if( sscanf(arg,"load:%s %s",file,func) )
		printf( "MASTER: "+file+"'s "+func+"() 回傳"+call_other(file, func) );
	else
		printf( "MASTER: 未知旗標\n" );
	return;
}