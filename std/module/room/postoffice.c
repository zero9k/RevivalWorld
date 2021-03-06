/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : postoffice.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-02-21
 * Note   : 郵局
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit ROOM_ACTION_MOD;

private void input_mail_confirm(object me, string receiver, string title, string content, string yn)
{
	int time;
	mapping maildata;
	string file;

	if( yn != "y" && yn != "yes " )
	{
		tell(me, "取消寄出信件。\n");
		return me->finish_input();
	}
	
	time = time();
	file = read_file(user_mail(receiver));
	
	if( stringp(file) )
		maildata = restore_variable(file);
	
	if( !mapp(maildata) )
		maildata = allocate_mapping(0);
		
	// 避免 key 重覆
	while( !undefinedp(maildata[time]) ) time++;
	
	maildata[time] = allocate_mapping(0);
	
	maildata[time]["time"] = time();
	maildata[time]["sender"] = me->query_id(1);
	maildata[time]["receiver"] = receiver;
	maildata[time]["title"] = kill_repeat_ansi(title+NOR);
	maildata[time]["content"] = kill_repeat_ansi(content+NOR);
	
	tell(me, "成功寄出「"+title+"」信件。\n");
	
	me->finish_input();

	if( find_player(receiver) )
		tell(find_player(receiver), HIY"\n郵局收到一封來自"+me->query_idname()+"的信件，標題為「"+title+"」。\n"NOR);

	set_temp("postoffice", time, me);

	write_file(user_mail(receiver), save_variable(maildata), 1);
}

private void input_mail_content(object me, string receiver, string title, string content)
{
	if( !content || !content[0] )
	{
		tell(me, GRN"請輸入"HIG"信件內容：\n"NOR);
		me->edit( (: input_mail_content, me, receiver, title :) );
		return;
	}

	tell(me, "確定寄出信件？(y/n)");
	input_to( (: input_mail_confirm, me, receiver, title, content :) );
}

private void input_mail_title(object me, string receiver, string title)
{
	if( !title || !title[0] )
	{
		tell(me, GRN"請輸入"HIG"信件標題："NOR);	
		input_to( (: input_mail_title, me, receiver :) );
		return;
	}
	if( noansi_strlen(title) > 40 )
	{
		tell(me, HIR"主題不得超過 40 個字元。\n"NOR);
		tell(me, GRN"請輸入"HIG"信件標題："NOR);	
		input_to( (: input_mail_title, me, receiver :) );
		return;
	}
	
	tell(me, GRN"請輸入"HIG"信件內容：\n"NOR);
	
	me->edit( (: input_mail_content, me, receiver, title :) );
}

// 寄出信件
private void do_mail(object me, string arg)
{
	if( !arg )
		return tell(me, "請輸入收件人的 ID。\n");
	
	if( !wizardp(me) && query_temp("postoffice", me) + 60 > time() )
		return tell(me, pnoun(2, me)+"不久前才寄出信件，請稍後一分鐘。\n");

	if( !user_exists(arg) )
		return tell(me, "這個世界中並不存在這個玩家。\n");
		
	tell(me, GRN"請輸入"HIG"信件標題："NOR);
	
	input_to( (: input_mail_title, me, arg :) );
}


// 列出信件
private void do_list(object me, string arg)
{
	int i;
	string msg;
	string myid = me->query_id(1);
	string file = read_file(user_mail(myid));
	mapping maildata;
	
	if( !stringp(file) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	maildata = restore_variable(file);
	
	if( !mapp(maildata) || !sizeof(maildata) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	msg = me->query_idname()+"的信件如下：\n";
	msg += WHT"────────────────────────────────────\n"NOR;
	msg += "    寄件人        日期             狀態  主題\n";
	msg += WHT"────────────────────────────────────\n"NOR;
	
	foreach(int time in sort_array(keys(maildata), -1))
	{
		msg += sprintf("%-4s%-14s%-17s%-6s%s\n", 
			++i+".",
			capitalize(maildata[time]["sender"]),
			HIC+TIME_D->replace_ctime(time)+NOR,
			maildata[time]["read"] ? HIG"已讀"NOR : HIR"未讀"NOR,
			HIY+maildata[time]["title"])+NOR;
	}
	
	msg += WHT"────────────────────────────────────\n"NOR;
	msg += HIW"讀取信件：read '編號'、刪除信件：delete '編號' \n\n"NOR;

	me->more(msg);
}


// 讀取信件
private void do_read(object me, string arg)
{
	int i;
	int selected;
	string msg;
	string myid = me->query_id(1);
	string file = read_file(user_mail(myid));
	mapping maildata;
	
	if( !stringp(file) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	maildata = restore_variable(file);
	
	if( !mapp(maildata) || !sizeof(maildata) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	i = to_int(arg);
	
	if( i < 1 || i > sizeof(maildata) )
		return tell(me, "輸入信件編號錯誤。\n");
	
	selected = sort_array(keys(maildata), -1)[i-1];
	
	msg =  "寄件人  ："+capitalize(maildata[selected]["sender"])+"\n";
	msg += "寄件日期："+HIG+TIME_D->replace_ctime(maildata[selected]["time"])+NOR+"\n";
	msg += "信件主題："+HIY+maildata[selected]["title"]+NOR+"\n";
	msg += "信件內容：\n";
	msg += WHT"────────────────────────────────────\n"NOR;
	msg += "\n"+maildata[selected]["content"]+"\n\n";
	msg += WHT"────────────────────────────────────\n"NOR;
	
	me->more(msg);
	
	if( undefinedp(maildata[selected]["read"]) )
	{
		maildata[selected]["read"] = 1;
		write_file(user_mail(myid), save_variable(maildata), 1);
	}
}

// 丟棄信件
private void do_delete(object me, string arg)
{
	int i;
	string myid = me->query_id(1);
	string file = read_file(user_mail(myid));
	mapping maildata;
	
	if( !stringp(file) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	maildata = restore_variable(file);
	
	if( !mapp(maildata) || !sizeof(maildata) )
		return tell(me, pnoun(2, me)+"沒有任何信件。\n");
		
	i = to_int(arg);
	
	if( i < 1 || i > sizeof(maildata) )
		return tell(me, "輸入信件編號錯誤。\n");
	
	map_delete(maildata, sort_array(keys(maildata), -1)[i-1]);

	tell(me, "刪除第 "+i+" 封信件完畢。\n");
	
	write_file(user_mail(myid), save_variable(maildata), 1);
}

// 設定建築物內房間型態種類
nosave mapping action_info =
([
	"lobby"	:
	([
		"short"	: HIG"郵局"NOR GRN"大廳"NOR,
		"help"	:
			([
"topics":
@HELP
    郵局大廳負責所有郵政相關業務。
HELP,

"mail":
@HELP
寄信給某位玩家，用法如下：
  mail '玩家ID'		- 寄信給某位玩家
HELP,

"read":
@HELP
取信，用法如下：
  read 2		- 讀取第 2 封信
HELP,

"list":
@HELP
列出所有信件，用法如下：
  list			- 顯示所有信件列表
HELP,

"delete":
@HELP
將某個信件刪除，用法如下：
  delete 2		- 刪除第 2 封信
HELP,
			]),
		"action":
			([
				"mail"		: (: do_mail :),
				"read"		: (: do_read :),
				"list"		: (: do_list :),
				"delete"	: (: do_delete :),
			]),
	]),
]);


// 設定建築物資料
nosave array building_info = ({
	
	// 建築物之中文名稱
	HIG"郵局"NOR

	// 開張此建築物之最少房間限制
	,1

	// 城市中最大相同建築數量限制(0 代表不限制)
	,0

	// 建築物建築種類, GOVERNMENT(政府), ENTERPRISE(企業集團), INDIVIDUAL(私人)
	,GOVERNMENT

	// 建築物可建築區域, AGRICULTURE_REGION(農業), INDUSTRY_REGION(工業), COMMERCE_REGION(商業)
	,AGRICULTURE_REGION | INDUSTRY_REGION | COMMERCE_REGION

	// 開張儀式費用
	,"5000000"
	
	// 建築物關閉測試標記
	,0

	// 繁榮貢獻度
	,30
	
	// 最高可加蓋樓層
	,1
	
	// 最大相同建築物數量(0 代表不限制)
	,0
	
	// 建築物時代
	,1
});

