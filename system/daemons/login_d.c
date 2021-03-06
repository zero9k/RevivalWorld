/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : login_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-9-21
 * Note   : 登錄精靈
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ip.h>
#include <ansi.h>
#include <daemon.h>
#include <login.h>
#include <object.h>
#include <secure.h>
#include <mudlib.h>
#include <message.h>
#include <time.h>
#include <gender.h>

private nosave string *welcome_messages, welcome_message;	// 歡迎訊息
private nosave mapping wrong_password;				// 錯誤密碼紀錄
private int wizlock;						// 巫師登入等級鎖定
nosave int total_user;						// 全部角色檔案數
mapping login_count;						// 登入統計
mapping ip_use;							// IP 使用紀錄
private nosave mapping login_attacker;				// 檢查登入攻擊		


int save()
{
	return save_object(DATA);
}
int remove()
{
	return save();
}

void clear_ip_use() { ip_use = ([]); }

/* IP 使用記錄 */
void remove_ip_use(string ip_or_id)
{
	map_delete(ip_use, ip_or_id);

	foreach(string ip, string *id in ip_use)
	{
		if( member_array(ip_or_id, id) != -1 )
			ip_use[ip] -= ({ ip_or_id });
	}
}
varargs mixed query_ip_use(string ip)
{
	if( !undefinedp(ip) )
		return ip_use[ip];

	return ip_use;
}

/* 登入等級權限 */
int set_wiz_lock(int i) // 設定登入等級權限
{
	wizlock = i;
	save();
	return i;
}
int query_wiz_lock() // 回傳登入等級權限
{
	return wizlock;
}


/* 登入資訊 */
mapping query_login_count() // 回傳登入計數資訊
{
	return login_count;
}
void set_login_count() // 設定登入資訊
{
	int hour = TIME_D->query_realtime_array()[HOUR];
	int users_size = sizeof(users());

	login_count["total"]++;

	if( users_size > query("login/maxusers/hour/"+hour, SYSDATABASE_D->query_ob()) )
		set("login/maxusers/hour/"+hour, users_size, SYSDATABASE_D->query_ob());

	if( users_size > login_count["max"] )
	{
		login_count["max"] = users_size;
		save();
	}
}
void reset_login_count(int hour)
{
	delete("login/maxusers/hour/"+hour, SYSDATABASE_D->query_ob());

	set_login_count();
}

/* 檢查登入攻擊者 */
int check_login_attacker(string ip)
{
	int time = time();

	foreach(string attacker, mapping data in login_attacker)
	{
		if( time - data["time"] > 600 )
			map_delete(login_attacker, attacker);
	}

	if( !mapp(login_attacker[ip]) )
	{
		login_attacker[ip] = allocate_mapping(0);
		login_attacker[ip]["times"] = 0;
	}
	
	login_attacker[ip]["time"] = time;
	
	if( ++login_attacker[ip]["times"] >= 20 )
		return 0;

	return 1;
}

mapping query_login_attacker()
{
	return login_attacker;
}

void set_login_attacker(string ip, int times)
{
	login_attacker[ip] = allocate_mapping(0);
	login_attacker[ip]["time"] = time();
	login_attacker[ip]["times"] = times;
}

// 檢查是否重覆登入
private nomask int is_repeat_login(string arg)
{
	foreach(object child in children(LOGIN_OB))
	if( query("id", child) == arg ) return 1;
	return 0;
}

// 隨機歡迎訊息
string random_welcome_message()
{
	return welcome_messages[random(sizeof(welcome_messages))];
}

// 顯示 motd
void show_motd(object user)
{
	string msg = terminal_colour(read_file(MOTD), ([
		"MUD_NAME"	: MUD_FULL_NAME,
		"MUDLIB_VERSION": MUDLIB_VERSION_NUMBER,
		"MUDOS_VERSION"	: __VERSION__ ,
		"ARCH"		: __ARCH__,
		"UPDATE"	: TIME_D->replace_ctime(stat(MOTD)[1]),
	    ]));
	tell(user, msg);
}

private void remove_player(object ob)
{
	if( ob ) LOGOUT_D->quit(ob, 0);
	return;
}

// 檢查重覆輸入錯誤密碼
int input_wrong_password(object user)
{
	return ++wrong_password[user];
}

void add_login_message(string id, string msg)
{
	object player;

	if( !objectp(player = load_user(id)) ) return;
	msg = sprintf("%s[%s]- %s",
	    query("login_message", player) || "",
	    TIME_D->replace_ctime(time()),
	    msg
	);
	set("login_message", msg, player);	
	player->save();

	if( !userp(player) )
		destruct(player);
}

nomask string show_login_message(object login_ob)
{
	string msg;
	int cities, logins, wizards, players;

	msg = random_welcome_message();
	/* 計算使用者數 */
	foreach(object ob in users())
	{
		if( ob->is_user_ob() )
			(wizardp(ob) ? wizards++ : players++);
		else if( ob->is_login_ob() )
			logins++;
	}

	cities = sizeof(CITY_D->query_cities());
	msg += sprintf(
	    HIW"重生的世界"NOR"目前共有 %d 座城市、%s 個角色，已計連線人次共 %s 次。\n"
	    "目前有 %d 位巫師、%d 位玩家，共計 %d 人正在線上，%d 人正在登入。\n", 
	    cities, NUMBER_D->number_symbol(total_user), NUMBER_D->number_symbol(login_count["total"]),
	    wizards, players, wizards + players, logins);
	
	tell(login_ob, msg, SYSMSG);
}



// 等待進入遊戲
private nomask varargs void waiting_enter_game(object user, int timer, string arg)
{
	string ip;
	mixed quit_place;

	if( !user ) return;

	if( --timer )
	{
		//tell(user, ".");
		set_temp("call_out_handle", call_out( (: waiting_enter_game, user, timer :), 1 ), user);
		return;
	}

	if( intp(query_temp("call_out_handle", user)) )
		remove_call_out(copy(query_temp("call_out_handle", user)));

	delete_temp("call_out_handle", user);
	remove_input_to(user);

	//tell(user, ".\n");
	//user->show_msg_buffer();

	user->clear_msg_buffer();

	quit_place = copy(query("quit_place", user));

	if( !query_temp("reconnect", user) )
		switch( typeof(quit_place) )
	{
	case "array":
		if( MAP_D->query_map_system(quit_place) )
		{
			startup_title_screen(user, 11);
			user->move(quit_place);
		}
		else
		{
			tell(user, pnoun(2, user)+"原本離線時的環境已經消失了。\n");

			if( wizardp(user) )
				user->move(WIZROOM);
			else
				user->move(STARTROOM);
		}
		break;
	case "string":

		intact_file_name(ref quit_place);

		if( file_exist(quit_place) )
		{
			if( quit_place[<2..<1] == ".o" )
				quit_place = quit_place[0..<3];

			if( load_object(quit_place) == load_object(VOID_OB) )
				user->move(STARTROOM);
			else
				user->move(quit_place);
		}
		else
		{
			tell(user, pnoun(2, user)+"原本離線時的環境已經消失了。\n");

			if( wizardp(user) )
				user->move(WIZROOM);
			else
				user->move(STARTROOM);
		}
		break;
	default:
		if( wizardp(user) )
			user->move(WIZROOM);
		else
			user->move(STARTROOM);
	}

	user->finish_input();

	if( query_temp("reconnect", user) )
	{
		// 顯示離線期間所接受到的訊息
		int net_dead_time = query_temp("net_dead_time", user);
		array msg_data = user->query_last_msg();

		if( net_dead_time )
			tell(user, YEL"\n"+pnoun(2, user)+"自斷線於 " + ctime(net_dead_time) + " 到現在過了 " + ((time()-net_dead_time)/60) + " 分 " + ((time()-net_dead_time)%60) + " 秒。\n"NOR);

		if( sizeof(msg_data) && net_dead_time && msg_data[<2] > net_dead_time )
		{
			tell(user, HIY"\n在"+pnoun(2, user)+"斷線的期間中有收到新的訊息，請使用 msg 指令查看。\n"NOR);
		}
		delete_temp("net_dead_time", user);
		msg("$ME重新連線進入遊戲。\n", user, 0, 1);
		CHANNEL_D->channel_broadcast("sys","登入："+user->query_idname()+WHT"由"+NETWORK_D->query_network_short(user)+"重新連線進入。", 1);
	}
	else
	{
		msg("$ME進入遊戲。\n", user, 0, 1);
		CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+WHT"由"+NETWORK_D->query_network_short(user)+"連線進入。", 1);
	}
	log_file(LOG, user->query_idname()+" login "+NETWORK_D->query_network_short(user)+NOR+"\n");

	// 紀錄使用過同一 IP 的玩家
	ip = query_ip_number(user);

	if( stringp(ip) )
	{
		if( undefinedp(ip_use[ip]) )
			ip_use[ip] = ({ user->query_id(1) });
		else
			ip_use[ip] |= ({ user->query_id(1) });
	}

	if(query("faint", user)) {
		set_temp("disable/msg", 1, user);
		set_temp("disable/cmd", 1, user);
	}

}

// 允許重覆登入
int valid_multi_login(object user1, object user2)
{
	string *allowmulti1 = query("login/allowmulti", user1);
	string *allowmulti2 = query("login/allowmulti", user2);

	if( arrayp(allowmulti1) && member_array(user2->query_id(1), allowmulti1) != -1 )
		return 1;

	if( arrayp(allowmulti2) && member_array(user1->query_id(1), allowmulti2) != -1 )
		return 1;

	return 0;
}

// 準備進入遊戲
void enter_game(object user)
{
	string my_ip, my_ident;

	// 啟動互動性質
	user->enable_interactive();

	// 啟動生命現象
	user->startup_living();

	set_temp("login/ip", query_ip_number(user), user);
	set_temp("login/ip_name", query_ip_name(user), user);
	set_temp("login/ip_port", query_ip_port(user), user);

	// 計算登錄人次
	set_login_count();

	/* 顯示公佈欄 */
	show_motd(user);

	set("last_on/time", time(), user);

	// 取回身上物品
	if( !query_temp("reconnect", user) )
	{
		user->restore_inventory();
		set_temp("login/time", time(), user);
	}

	// 登錄附加訊息
	if( query("login_message", user) )
	{
		tell(user, "\n" HIG "系統訊息：\n"+query("login_message", user)+NOR+"\n\n");
		write_file(file_name(user)+"/old_login_message",query("login_message", user));
		delete("login_message", user);
	}


	my_ip = query_ip_number(user);
	my_ident = query_temp("login/ident", user);

	foreach(object ppl in users())
	{
		if( ppl == user || !environment(ppl) || !interactive(ppl) ) continue;

		if( query_ip_number(ppl) == my_ip )
		{				
			if( SECURE_D->level_num(ppl->query_id(1)) >= GUEST || SECURE_D->level_num(user->query_id(1)) >= GUEST )
				CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+"與"+ppl->query_idname()+"相同 IP 合法登入(巫師角色)。");
			else if( valid_multi_login(user, ppl) )
				CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+"與"+ppl->query_idname()+"相同 IP 合法登入。");
			else
			{
				tell(user, "\n"HIR+pnoun(2, user)+"與"+ppl->query_idname()+HIR+"重覆登錄，請詳閱本遊戲規則(help rule)。\n您的違規動作已向巫師通報，同時您將會在 30 秒後自動由系統踢除離線。\n若是利用 IP 分享器或是其他特殊正當原因者可向巫師申請開放 IP 限制。\n"NOR);
				CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+"與"+ppl->query_idname()+"相同 IP 違規登入，30 秒後踢除。");
				
				if( query_ip_number(ppl) == "75.84.17.242" || query_ip_name(ppl)[<13..] == "csupomona.edu" )
				{
					CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+"與"+ppl->query_idname()+"相同 IP 違規登入，馬上踢除。");
					destruct(user);
					destruct(ppl);
					
				} else { 
					call_out( (: remove_player, user :), 30);
					set("cmdlimit", 30, user);
				}
			}
		}
	}

	if( IP_D->query_ip_status(my_ip) == CMDLIMIT && SECURE_D->level_num(user->query_id(1)) < GUEST)
	{
		set("cmdlimit", 30, user);
		CHANNEL_D->channel_broadcast("sys", "登入："+user->query_idname()+"所使用的 IP 位置將受到指令執行速度限制。");
	}
	else
		delete("cmdlimit", user);

	tell(user, NOR WHT"請輸入任意鍵繼續或 "+WAITING_ENTER_TIME+" 秒後自動進入遊戲\n"NOR);
	set_temp("call_out_handle", call_out( (: waiting_enter_game, user, WAITING_ENTER_TIME :), 1 ), user);

	input_to( (: waiting_enter_game, user, 1 :) );
		
	if( query_temp("new_player", user) )
		CHANNEL_D->channel_broadcast("news", "新玩家 "+user->query_idname()+"-"+(query("gender", user)==MALE_GENDER?HIG"男"NOR:HIR"女"NOR)+" 羞澀地進入了這個世界，一臉茫然的樣子...。", 1);
}

// 更新使用者物件
nomask void update_player(object player)
{
	string id;
	object *inv, temp;
	mapping temp_database;
	mixed env;
	int status;

	if( !objectp(player) || !interactive(player) ) return;

	temp_database = player->query_temp_database();

	inv = all_inventory(player);

	player->save();

	temp = new(VOID_OB);

	inv->move(temp);

	exec( temp, player );

	id = player->query_id(1);

	env = copy(query_temp("location", player) || environment(player) || load_object(VOID_OB));

	destruct(player);

	player = load_user(id);

	// 先移動到 env
	player->move(env);

	exec( player, temp );

	player->enable_interactive();
	player->startup_living();

	inv->move(player);

	foreach(object ob in inv)
	if( ob->is_equipping() )
		player->equip(ob, &status);

	destruct(temp);

	player->set_temp_database(temp_database);

	tell(player, pnoun(2, player)+"突然感覺到自己"HIW"靈"NOR WHT"魂"NOR"進入了一個新的身體....。\n");
	CHANNEL_D->channel_broadcast("sys","更新："+player->query_idname()+NETWORK_D->query_network_short(player)+"更新身體完畢。");
}

/* 時間排程 (每小時) */
void time_schedule()
{
	int i;

	// 隨機更換歡迎訊息
	while(file_size(WELCOME_MSG + i) > 0)
	{
		welcome_messages += ({ kill_repeat_ansi(terminal_colour(read_file(WELCOME_MSG + i),
			    ([
				"DRIVER"	: __VERSION__,
				"VERSION"	: MUDLIB_VERSION_NUMBER,
			    ])))
		});
		i++;
	}

	// 重新計算總人數
	total_user = 0;
	foreach(string prefix in get_dir("/data/user/"))
	total_user += sizeof(get_dir("/data/user/" + prefix + "/"));

	set_login_count();

	save();
}
private nomask void initialize()
{
	welcome_messages = allocate(0);
	wrong_password = allocate_mapping(0);
	login_attacker = allocate_mapping(0);

	if( !mapp(login_count) )	login_count = allocate_mapping(0);
	if( !mapp(ip_use) )		ip_use = allocate_mapping(0);

	time_schedule();
}


private void create()
{
	if( clonep() )
	{
		destruct(this_object());
		return;
	}
	if( !restore_object(DATA) ) save();
	initialize();
	
	SCHEDULE_D->set_event( 60*69*24, 1, load_object(PPL_LOGIN_D), "reset_cmdlimit");
}
string query_name()
{
	return "登入系統(LOGIN_D)";
}
