/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _edit.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-3-7
 * Note   : 編輯系統
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>

#define BEGIN	sprintf(NOR"╭───────────────────────────"WHT"["HIW"%18|s"NOR WHT"]"NOR"╮\n", "內容 Content")
#define END	sprintf(NOR"╰───────────────────────────"WHT"["HIW"%18|s"NOR WHT"]"NOR"╯\n", "結束 End")

void input_line(string arg, string *text, function f)
{
	int prompt;
	arg = ansi(arg);
	
	if( arg && query("encode/gb") ) 
		arg = G2B(arg);

	prompt = query("env/edit_prompt"); 
	
	switch(arg)
	{
		case ".":
			tell(this_object(), "\n" + END);
			evaluate(f, implode(text, "\n") + NOR);
			break;
		case "~q":
			tell(this_object(), "\n" + END + "取消編輯。\n");
			this_object()->finish_input();
			break;
		case "~b":
			if( sizeof(text) )
			{
				tell(this_object(), "\n" + END + "\n 刪除上一行文字訊息: " + text[<1] + "\n");
				text = text[0..<2];
				tell(this_object(), BEGIN + implode(text,"\n"));
				if( prompt ) tell(this_object(), "\n> ");
				input_to((: input_line :), 2, text, f);
				return;
			}
			tell(this_object(), "\n" + END + "取消編輯。\n");
			this_object()->finish_input();
			break;
		case "~p":
			tell(this_object(), "\n" + END + "\n 您已輸入的文字訊息如下: \n");
			tell(this_object(), BEGIN + implode(text,"\n"));
			if( prompt ) tell(this_object(), "\n> ");
			input_to((: input_line :), 2, text, f);
			return;
		case "~h":
			tell(this_object(), @HELP

------------------------------------------
- Revival World 文字編輯器 使用說明 -

在新的一行中輸入
'.' <句號>	完成文字編輯
~q		取消文字編輯
~p		顯示目前已輸入文字
~b		刪除上一行文字
~h		使用說明
-------------------------------------------
HELP
			);
			if( prompt ) tell(this_object(), "> ");
			input_to((: input_line :), 2, text, f);
			return;
		default:
			text += ({ arg });
			if( prompt ) tell(this_object(), "\n" + arg + "\n> ");
			input_to((: input_line :), 2, text, f);
			return;
	}
}

void edit(function f, string text)
{
	string bar;
	bar  = "\n ─ "HIW"文字編輯器"NOR" ─ 輸入 '"HIW"."NOR"' 結束 ─ "HIW"~q"NOR" 取消 ─ "HIW"~h"NOR" 說明 ─\n";
	bar += BEGIN;
	bar += text || "";

	tell(this_object(), bar);
	
	if( query("env/edit_prompt") ) 
		tell(this_object(), "> ");
	
	input_to((: input_line :), 2, text ? ({text}) : ({}), f);
}
