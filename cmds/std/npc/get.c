/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : get.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-10-10
 * Note   : get 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
將物品或金錢撿拾起來

get [數量|all] [物品] [排序]	- 將某數量或排序的某物品撿拾起來

HELP;

private void do_command(object me, string arg)
{
	int mass, from_environment=1;
	object ob, target, env;
	string from, msg="", amount;

	if( !arg )
	{
		if( me->is_npc() )
			return me->do_command("say 老闆想要我撿什麼東西？\n");
		else
			return tell(me, pnoun(2, me)+"想要撿什麼東西？\n");
	}
	
	// Delay 中不能下指令
	if( me->is_delaying() )
		return tell(me,	me->query_delay_msg());

	env = environment(me);
	
	if( sscanf(arg, "%s from %s", arg, from) == 2 )
	{
		if( !objectp(target = present(from)) )
			return tell(me, "這裡沒有 "+from+" 這個人或物品。\n");
		else
			from_environment = 0;
	}
	else target = env;

	if( sscanf(arg, "$%s %s", arg, amount) == 2 )
	{
		if( me->is_npc() )
			return me->do_command("say 我無法撿拾金錢。\n");

		if( !MONEY_D->money_unit_exist(arg) )
			return tell(me, "並沒有 "+arg+" 這種貨幣。\n");
		
		amount = big_number_check(amount);
		if( !amount )
			return tell(me, "請輸入正確的數字。\n");
		
		if( count(amount, "<=", 0) )
			return tell(me, "金額必須要大於零。\n");
		
		if( !objectp(ob = present(arg, target)) )
			return tell(me, "這裡並沒有 "+arg+" 這種貨幣。\n");

		if( count(amount, ">", query("money", ob)) )
			return tell(me, "這裡並沒有這麼多的 "+arg+" 金錢。\n");
		
		ob->move(me, amount);
		
		log_file("command/get", me->query_idname()+"撿起 "+money(arg, amount));
		CHANNEL_D->channel_broadcast("nch", me->query_idname()+"撿起 "+money(arg, amount));

		return msg("$ME把 "HIY+money(arg, amount)+" "NOR+(from_environment?"":"從"+target->query_idname()+"中")+"撿了起來。\n", me,0,1);
	}
	
	if( sscanf(arg, "%s %s", amount, arg) == 2 )
	{
		if( amount == "all") ;
		
		// 如果 amount 並不是數字單位
		else if( !big_number_check(amount) )
		{
			arg = amount + " " + arg;
			amount = "1";	
		}	
		else if( count(amount, "<", 1) ) amount = "1";
	}
	else amount = "1";
	
	if( objectp(ob = present(arg, target)) )
	{
		object base_ob = load_object(base_name(ob));

		if( me == ob )
			return tell(me, pnoun(2, me)+"如果真的可以把自己拿起來，請至巫師大廳領取十萬元獎金。\n");
		
		if( me == target )
			return tell(me, ob->query_idname()+"已經在"+pnoun(2, me)+"身上了！！。\n");
			
		if( target->is_living() )
			return msg("$ME在$YOU身上東摸西摸，試著把"+QUANTITY_D->obj(ob, amount)+"給拿走！\n", me, target, 1);
		
		if( ob->is_living() )
			return msg("$ME在$YOU身上東摸西摸，試著找到施力點把$YOU給拿起來！！\n", me, ob,1);
		
		if( ob->is_money() )
		{
			if( amount != "all" && count(amount, ">", query("money", ob)) )
				return tell(me, "這裡的錢沒有那麼多。\n");
			
			if( amount == "all" )
				amount = query("money", ob);
	
			msg("$ME把 "HIY+money(query("unit", ob), amount)+" "NOR+(from_environment?"":"從"+target->query_idname()+"中")+"撿了起來。\n", me,0,1);
			
			log_file("command/get", me->query_idname()+"撿起了 "+money(query("unit", ob), amount));
			CHANNEL_D->channel_broadcast("nch", me->query_idname()+"撿起了 "+money(query("unit", ob), amount));
			ob->move(me, amount);
			
			return;
		}
		
		if( amount != "all" && count(amount,">", query_temp("amount",ob)||1 ) )
		{
			if( me->is_npc() )
				return me->do_command("say "+"這裡的"+ob->query_idname()+"沒有那麼多個。\n");
			else
				return tell(me, "這裡的"+ob->query_idname()+"沒有那麼多個。\n");
		}
		
		if( query("mass", ob) == -1 )
		{
			msg("$ME隨手將"+QUANTITY_D->obj(ob, amount)+"一手舉起，只見$ME輕輕地說了一句「啊咧！好像太重了...」。\n",me,0,1);
			return me->faint();
		}
		
		if( amount == "all" )
		{
			amount = ((me->query_max_loading() - me->query_loading()) / (query("mass", ob)||1)) + "";
			
			if( count(amount, ">", query_temp("amount", ob)||1) )
				amount = query_temp("amount", ob)||"1";
				
			if( count(amount, "==", 0) )
				return tell(me, pnoun(2, me)+"拿不動"+ob->query_idname()+"了。\n");
		}
		
		if( me->get_object(ob, amount) )
			return msg("$ME把"+QUANTITY_D->obj(ob || base_ob, amount)+(from_environment?"":"從"+target->query_idname()+"中")+"撿了起來。\n", me,0,1);
		else
			return msg("$ME把"+QUANTITY_D->obj(ob || base_ob, amount)+(from_environment?"":"從"+target->query_idname()+"中")+"拿了起來，只聽到大腿一聲喀啦，大腿骨快斷了！又趕緊放手。\n",me,0,1);
	}
	
	if( lower_case(arg) == "all" )
	{
		object *all_ob = filter_array(present_objects(me), (: !$1->is_living() && query("mass",$1) >= 0 :));
		
		if( userp(target) )
			return msg("$ME在$YOU身上東摸西摸，試著把$YOU所有的東西全部拿走！\n",me,target,1);
		
		if( sizeof(all_ob) )
		{
			int too_heavy, loading, max_loading;
			
			max_loading = me->query_max_loading();
			
			foreach(ob in all_ob)
			{
				mass = query("mass",ob);

				if( mass == -1 ) continue;
		
				if( ob->is_money() )
				{
					msg += HIR"．"NOR+ob->query_idname()+NOR"\n";
					
					log_file("command/get", me->query_idname()+"撿起了"+QUANTITY_D->money_info(query("unit", ob), query("money", ob)));
					CHANNEL_D->channel_broadcast("nch", me->query_idname()+"撿起了"+QUANTITY_D->money_info(query("unit", ob), query("money", ob)));
					ob->move(me);	
					continue;
				}
				
				amount = copy(query_temp("amount",ob)) || "1";
				
				loading = max_loading - me->query_loading();
				
				if( count(loading, "<", count(amount,"*",mass)) )
				{
					if( loading < mass ) continue;
					
					msg += HIR"．"NOR+QUANTITY_D->obj(ob, loading/mass, 1)+"\n";
					
					ob->move(me, loading/mass);

					too_heavy = 1;
				}
				else
				{
					msg += HIR"．"NOR+remove_fringe_blanks(QUANTITY_D->obj(ob, amount))+"\n";
					ob->move(me , amount);
				}
			}
			
			if( msg == "" )
			{
				if( me->is_npc() )
					return me->do_command("say 我撿不起來任何的東西。\n");
				else
					return tell(me, pnoun(2, me)+"撿不起來任何的東西。\n");
			}
			
			msg("$ME把"+(from_environment?"地上撿得動的東西都撿了起來":"$YOU身上能拿的東西通通拿了出來")+"。\n"+msg+"\n", me, target,1);
			
			if( too_heavy ) tell(me, "剩下的東西太重拿不動了。\n\n");
			
			return;
		}
		return tell(me, (from_environment?"這個地方":target->query_idname())+"沒有任何物品可以撿耶...。\n");
	}
	
	if( me->is_npc() )
		return me->do_command("say 這裡沒有 "+arg+" 這件物品。\n");
	else
		return tell(me, "這裡沒有 "+arg+" 這件物品。\n");
}