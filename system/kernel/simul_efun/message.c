/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : message.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-7-2
 * Note   : included by Simul_Efun Object
 * Update :
 *  o 2003-02-14 Clode 三大訊息函式 tell(), broadcast(), msg()

 -----------------------------------------
 */

#include <location.h>
#include <message.h>
#include <daemon.h>

//
// write(), tell_object(), 等單人訊息函式
// 全部改用 tell() 處理, 以完整支援地圖系統與 Client 訊息傳輸
//
varargs void tell(mixed target, string msg, string classes)
{
	if( objectp(target) || arrayp(target) )
		target->catch_tell(msg, classes);
}

/* 對所有使用者放出訊息 */
void shout(string msg)
{
	tell(filter_array(users(), (: !$1->is_login_ob() :)), msg, SYSMSG);
}

//
// say(), tell_room() 等多人訊息函式
// 全部改用 broadcast() 處理, 以完整支援地圖系統與 Client 訊息傳輸
//
varargs void broadcast(mixed target, string msg, string classes, mixed exclude)
{
	object *receivers = allocate(0);

	if( !target ) return;

	if( stringp(target) )
		target = load_object(target);

	// 廣播對象若為一個物件, 則對其內容物件與同環境物件傳送訊息
	if( objectp(target) )
	{
		//object env = environment(target);

		//if( objectp(env) )
		//{
		//	if( env->is_maproom() )
		//		return broadcast(query_temp("location", target), msg, classes, exclude);

		//	receivers += all_inventory(env);
		//}
		receivers += all_inventory(target);
	}
	// 若廣播對象為地圖系統座標, 則處理座標過濾
	else if( arrayp(target) )
		receivers += MAP_D->coordinate_inventory(target);

	// 除外對象
	if( !undefinedp(exclude) )
	{
		if( arrayp(exclude) )
			receivers -= exclude;
		else if( objectp(exclude) )
			receivers -= ({ exclude });
	}

	// 對所有接收者廣播訊息
	tell(receivers, msg, classes);
}


/* 統一代名詞轉換之訊息發送函式 */
varargs void msg(string msg, object me, object you, int onlooker, string classes, object *exclude, mapping translate)
{
	string my_msg, your_msg;
	string my_idname, your_idname;

	if( !msg || !objectp(me) ) return;

	// 取得 Me 物件資料
	my_idname = me->query_idname() || "無名稱";


	// 額外輸入關鍵字之轉換, 即除了 $ME 與 $YOU 以外之轉換
	if( mapp(translate) )
		foreach(string pattern, string replace in translate)
		msg = replace_string(msg, pattern, replace);

	// Me 訊息之第二人稱轉換
	my_msg = replace_string(msg, "$ME", pnoun(2, me));

	// 若有對象物件 you, 則對 you 作訊息處理
	if( objectp(you) )
	{
		your_idname = you->query_idname() || "無名稱";

		// Me 訊息之第三人稱轉換
		my_msg = replace_string(my_msg, "$YOU", your_idname, 1);
		my_msg = replace_string(my_msg, "$YOU", pnoun(3, you));

		// You 訊息之第二人稱轉換
		your_msg = replace_string(msg, "$ME", my_idname, 1);
		your_msg = replace_string(your_msg, "$ME", pnoun(3, me));
		your_msg = replace_string(your_msg, "$YOU", pnoun(2, you));


		// 對物件 you 送出訊息
		if( !arrayp(exclude) || member_array(you, exclude) == -1 )
			tell(you, your_msg, classes);
	}

	// 對物件 Me 送出訊息
	if( !arrayp(exclude) || member_array(me, exclude) == -1 )
		tell(me, my_msg, classes);

	// 若有 onlooker(旁觀者) 參數, 則對與 me 或 you 同一環境中的生物發出訊息
	if( onlooker )
	{
		string onlookers_msg;
		object *onlookers = present_objects(me) + ({environment(me)})||({});

		if( objectp(you) && environment(me) != environment(you) ) 
			onlookers += present_objects(you) + ({environment(you)})||({});

		onlookers -= ({ me, you }) + (exclude || ({}));

		onlookers_msg = replace_string(msg, "$ME", my_idname);

		if( objectp(you) ) 
			onlookers_msg = replace_string(onlookers_msg, "$YOU", your_idname);

		// 對所有旁觀者物件送出訊息
		tell(onlookers, onlookers_msg, classes);
	}
}

void write(mixed args...)
{
	error("write() 已經廢止使用, 請改用 tell()");
}

void tell_object(mixed args...)
{
	error("tell_object() 已經廢止使用, 請改用 tell()");
}

void say(mixed args...)
{
	error("say() 已經廢止使用, 請改用 broadcast()");
}

void tell_room(mixed args...)
{
	error("tell_room() 已經廢止使用, 請改用 broadcast()");
}

void message(mixed args...)
{
	error("message() 已經廢止使用, 請改用 msg()");
}

void printf(string format, mixed args...)
{
	efun::printf("printf():\n"+format, args...);
}
