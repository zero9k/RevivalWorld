/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : parse.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-17
 * Note   : included by Simul_Efun Object
 * Update :
 *  o 2003-00-00

 -----------------------------------------
 */

object object_parse(string arg, object* objects)
{
	int which = 1;
	int which_temp;
	string amount;
	string objectname;
	object ob;

	if( !arg || !arg[0] || !sizeof(objects) ) return 0;
	
	// 試著取得 which
	if( sscanf(arg, "%s %d", objectname, which) != 2 )
		objectname = arg;

	if( which < 1 ) return 0;

	// 若送入的語法包括 amount, 則取出 amount 的部分
	if( sscanf(objectname, "%s %s", amount, objectname) == 2 )
	{
		if( !big_number_check(amount) )
			objectname = amount +" "+ objectname;
	}

	objectname = lower_case(objectname);

	which_temp = which;

	foreach(ob in objects)
		if( ob->query_id(1) == objectname && !(--which_temp) )
			return ob;

	foreach(ob in objects)
		if( ob->id(objectname) && !(--which) )
			return ob;

	return 0;
}
