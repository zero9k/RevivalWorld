/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : bulletin_board.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-3-21
 * Note   : 標準留言板繼承檔
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <daemon.h>
#include <message.h>

#define BOARD_DATA_PATH		"/data/board/"

#define AUTHOR		0
#define TITLE		1

inherit MOVE;
inherit IDNAME;
inherit DATABASE;
inherit ACTION;

mapping actions;

// 日期檔名排序陣列
// POST_SORT = ({ 檔案int陣列(已排序) })
//int *POST_SORT = ({});

// Author 與 Title 的摘要快取
// SUMMARY = ([ 檔案(日期): ({ 作者, 標題 }) ]);
//mapping SUMMARY = ([]);

void do_read(object me, string arg);
void do_post(object me, string arg);
void do_discard(object me, string arg);
void do_followup(object me, string arg);
void do_mail(object me, string arg);

int is_board()
{
	return 1;
}

void startup_action()
{
	actions = 
	([ 
	    "post"		:(: do_post :),
	    "read"		:(: do_read :),
	    "discard"		:(: do_discard :),
	    "followup"		:(: do_followup :),
	    "mail"		:(: do_mail :), 
	]);
}

// 留言板主體初始化載入資料
void initialize_board()
{
	int i, *post_sort = ({});
	string file, path, *elem;
	mapping summary = ([]);

	path = copy(set("savepath", BOARD_DATA_PATH + query("boardid") + "/"));

	// 若沒有留言板目錄則建立
	if( file_size(path) != -2 ) mkdir(path);

	// 讀取編排所有留言資料
	else foreach( file in get_dir(path) )
	{
		i = to_int(file);

		elem = allocate(2);
		sscanf( read_file(path+file,1,1), "%s|%s\n", elem[AUTHOR], elem[TITLE] );
		summary[i] = elem;
		post_sort += ({ i });
	}

	set("summary", summary);
	set("post_sort", post_sort);
	set("mass",-1);
}


string short()
{
	int *post_sort = copy(query("post_sort"));

	int unread, size, last_read = query("board_last_read/"+query("boardid"), this_player()) || 0;

	if( !(size = sizeof(post_sort)) ) return ::short()+" "+NOR WHT+"["NOR" 沒有任何留言 "WHT"]"NOR;

	unread = sizeof(filter_array(post_sort, (: $1 > $(last_read) :)));

	return ::short() +" "+NOR WHT+"[ "NOR+ size +" 張留言 " + (unread ? "﹐"+unread+" 張未讀"WHT"]"NOR : WHT"]"NOR);
}

string long()
{
	string str;
	int i, index, last_read = query("board_last_read/"+query("boardid"), this_player());
	int *post_sort = copy(query("post_sort"));
	mapping summary = copy(query("summary"));

	str = sprintf("\n%-34s%s", this_object()->query_idname(), NOR WHT"─────────────────────────\n\n"NOR);

	foreach( index in post_sort )
	str += sprintf("%s[%4d] %s %-26s %-30s <%15s>\n",(index>last_read?HIY:""), ++i, NOR, summary[index][0], summary[index][1], TIME_D->replace_ctime(index));

	str += NOR WHT"\n──────────────────────────────────────────\n\n"NOR;

	return str;
}

void done_post(object me, int time, string arg)
{
	// 簽名檔
	string sig = query("signature", me);
	string *info = copy(query("summary"))[time];

	arg = info[AUTHOR]+"|"+info[TITLE]+"\n"+arg+(sig ? "\n\n"NOR WHT"──\n"NOR+sig+NOR : NOR);

	if( write_file( (string)query("savepath")+time, arg ) )
	{
		set("post_sort", query("post_sort") + ({ time }), find_object(base_name(this_object())));
		tell(me, "留言完畢。\n");
		set_temp("lastpost/time", time(), me);
		me->finish_input();
	}
	else
		return tell(me, "留言失敗，請通知巫師處理。\n");
}

void do_post(object me, string arg)
{
	int time = time();
	mapping summary = copy(query("summary"));

	if(!arg)
		return tell(me, "請為您的文章指定一個標題。\n");

	if( noansi_strlen(arg) > 26 )
		return tell(me, "你的標題太長了，請縮短你的標題。\n");

	if( time() - query_temp("lastpost/time", me) < 20 )
		return tell(me, "抱歉目前發文時間必須間隔 20 秒。\n");

	while( member_array(time, query("post_sort"))+1 ) time++;
	summary[time] = ({ me->query_idname(), arg });
	set("summary", summary, find_object(base_name(this_object())));

	me->edit( (: done_post, me, time :) );
}

void do_discard(object me, string arg)
{
	int num, file;
	int *post_sort = copy(query("post_sort"));
	mapping summary = copy(query("summary"));

	if( !arg || !sscanf(arg, "%d", num) )
		return tell(me, "請輸入預刪除之文章編號。\n");

	if( num < 1 || num > sizeof(post_sort) )
		return tell(me, "沒有這張留言。\n");

	file = post_sort[--num];

	if( me->query_idname() != summary[file][AUTHOR] )
	{
		string author_id;
		sscanf( remove_ansi(summary[file][AUTHOR]), "%*s(%s)", author_id );
		author_id = lower_case(author_id);
		if( me->query_id(1) != author_id 
		    && (!wizardp(me) || SECURE_D->level_num(author_id) > SECURE_D->level_num(me->query_id(1)) )
		    && !CITY_D->is_mayor(query("city"), me))
			return tell(me, "這張留言不是"+ pnoun(2, me) +"寫的。\n");
	}

	if( rm(query("savepath")+(file<1000000000?"0":"")+file) )
	{
		set("post_sort", post_sort - ({ file }), find_object(base_name(this_object())));
		map_delete(summary, file);
		set("summary", summary, find_object(base_name(this_object())));
		tell(me, "刪除完畢。\n");
	}
	else tell(me, "刪除失敗，請通知巫師處理。\n");
}

void do_read(object me, string arg)
{
	string msg;
	int num, last_read, file;
	int *post_sort = copy(query("post_sort"));
	mapping summary = copy(query("summary"));

	last_read = query("board_last_read/"+query("boardid"), me);


	if( !arg || arg == "new" || arg == "next" )
		num = (last_read ? sizeof(filter_array(post_sort, (: $1 <= $(last_read) :))) : 0) + 1;

	else if( !sscanf(arg, "%d", num) )
		return tell(me, "你要讀第幾張留言﹖\n");

	if( num < 1 || num-1 > sizeof(post_sort) )
		return tell(me, "沒有這張留言。\n");

	else if( num-1 == sizeof(post_sort) )
		return tell(me, "沒有新的留言。\n");

	file = post_sort[--num];

	msg = sprintf("[1;46m篇 數[m %-27s[1;46m作 者[m %s\n[1;46m主 題[m %-27s[1;46m時 間[m %s[m\n─────────────[1;30m[[1;37mContent[m[1;30m][m──────────────\n%s\n%s",
	    "第"+CHINESE_D->chinese_number(num+1)+"篇",
	    summary[file][AUTHOR],
	    summary[file][TITLE],
	    TIME_D->replace_ctime(file),
	    read_file(query("savepath")+(file<1000000000?"0":"")+file, 2),
	    "──────────────[1;30m[[1;37mEnd[m[1;30m][m───────────────\n\n"
	);

	me->more( msg );

	if( file >= last_read ) set("board_last_read/"+query("boardid"), file, me);
}

void do_followup(object me, string arg)
{
	int num, file, time = time();
	string msg="", new_title;
	int *post_sort = copy(query("post_sort"));
	mapping summary = copy(query("summary"));

	if( !arg )
		return tell(me, "請輸入欲回覆的文章編號或 last 回覆最後一篇文章。\n");
	if( arg == "last" ) num = sizeof(post_sort)-1;
	else if( !sscanf(arg, "%d", num) || !sscanf(arg, "%d %s", num, new_title) )
		return tell(me, "請輸入欲回覆的文章編號。\n");
	else if( arg == "last" ) num = sizeof(post_sort);

	if( num < 1 || num > sizeof(post_sort) )
		return tell(me, "沒有這張留言。\n");

	if( new_title && noansi_strlen(new_title) > 26 )
		return tell(me, "你的標題太長了，請縮短你的標題。\n");

	file = post_sort[num-1];

	foreach( string word in explode( read_file(query("savepath")+(file<1000000000?"0":"")+file, 2), "\n" ) )
	{
		if( word == "" || word[0..7]=="[1;30m>" ) continue;
		else if( word == NOR WHT"──" ) break;
		else if( word[0..5] == "[33m>" ) msg += "[1;30m> "+word[5..]+"\n";
		else if( word[0..5] == "[36m>" ) msg += "[33m> "+word[5..]+"\n";
		else msg += "[36m> "+word+"\n";
	}

	if( msg[0..5] == "[36m>" ) 
		msg = "[36m> ◎"+summary[file][AUTHOR]+" 在 "+TIME_D->replace_ctime(file)+" 留下這篇留言﹕\n" + msg;

	msg += "\n◎"+me->query_idname()+" 在 "+TIME_D->replace_ctime(time)+" 留下這篇留言﹕\n";

	while( member_array(time, post_sort)+1 ) time++;
	set("summary", summary + ([ time : ({ me->query_idname(), (new_title || (summary[file][TITLE][0..5]!="回覆："?"回覆：":"")) + summary[file][TITLE] }) ]), find_object(base_name(this_object())));

	me->edit( (: done_post, me, time :), msg );
}

void do_mail(object me, string arg)
{
	int file, num, *post_sort = copy(query("post_sort"));
	string msg, email;
	mapping summary = copy(query("summary"));

	if( !arg || !sscanf(arg, "%d", num) )
		return tell(me, "指令格式: mail <文章編號>\n");

	if( num < 1 || num > sizeof(post_sort) )
		return tell(me, "沒有這張留言。\n");

	if( !wizardp(me) )
		return tell(me, "本功能尚末開放玩家使用。\n");

	if( !stringp(email = query("email", me)) )
		return tell(me, "您沒有設定電子郵件。\n");

	file = post_sort[--num];

	msg = "以下訊息轉載自 重生的世界(Revival World)\n\n";
	msg += sprintf("[1;46m篇 數[m %-27s[1;46m作 者[m %s\n[1;46m主 題[m %-27s[1;46m時 間[m %s[m\n─────────────[1;30m[[1;37mContent[m[1;30m][m──────────────\n%s\n%s",
	    "第"+CHINESE_D->chinese_number(num+1)+"篇",
	    summary[file][AUTHOR],
	    summary[file][TITLE],
	    TIME_D->replace_ctime(file),
	    read_file(query("savepath")+(file<1000000000?"0":"")+file, 2),
	    "──────────────[1;30m[[1;37mEnd[m[1;30m][m───────────────\n\n"
	);
	msg = remove_ansi(msg);
	SMTP_D->mail(email, msg, "重生的世界(Revival World) - 文章轉載");
	tell(me, "文章已送出至您的 E-mail 信箱。\n");
}

