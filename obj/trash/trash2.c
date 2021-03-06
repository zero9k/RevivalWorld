/* This	program	is a part of RW	mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File	  : Trash.c
 * Author :
 * Date	  : 2003-5-3
 * Note	  : 垃圾
 * Update :
 *  o 2000-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <gender.h>

inherit	STANDARD_OBJECT;

void create()
{
	object *femaleuser = filter_array(users(), (: query("gender", $1) == FEMALE_GENDER :));
	int usersize = sizeof(femaleuser);

	if( usersize )
		set_idname(WHT"sanitary napkin"NOR, femaleuser[random(usersize)]->query_name()+"用過的衛生棉");
	else
		set_idname(WHT"sanitary napkin"NOR , "用過的衛生棉");

	set_temp("status", WHT"垃圾"NOR);

	if( this_object()->set_shadow_ob() ) return;

	set("unit", "個");
	set("value", -1);
	set("mass", 180);
	set("garbage", 1);
}

