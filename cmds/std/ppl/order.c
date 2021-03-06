/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : order.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-19
 * Note   : 命令指令
 * Update :
 *  o 2003-00-00  
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <ansi.h>
#include <feature.h>
#include <message.h>
#include <npc.h>
#include <skill.h>
#include <secure.h>

inherit COMMAND;

string help = @HELP
   這可以讓你下命令給自己的人力，目前開放的命令如下：

特殊命令指令
  order '員工' to name '中文'		- 幫員工取中文名字
  order '員工' to id '英文'		- 幫員工取英文名字

  order '員工' to work clerk		- 命令員工從事店員的工作(工作種類有 clerk/worker/engineer)
  order '員工' to work -d		- 命令員工停止工作

  order '員工' to learn '技能'		- 命令員工向您學習技能(您本身必須具備口才技能與教學技能)
  order '員工' to learn '技能' 50%	- 一次耗費 50% 的精神來教導員工

  order '員工' to heal			- 用自己的體力來救助暈倒的員工(暫時指令)

標準系統指令
  order '員工' to command		- 查詢員工可以使用的系統指令
  order '員工' to '指令'		- 命令員工執行某個系統指令(指令用法與玩家平常的用法相同)


相關指令: labor
HELP;

private void do_command(object me, string arg)
{
	string id, key, value;
	object npc, env;

	if( !arg )
		return tell(me, pnoun(2, me)+"想要命令誰？\n");

	if( sscanf(arg, "%s to %s", id, key) != 2 )
		return tell(me, pnoun(2, me)+"想要命令誰做什麼事？(例 order boy to name 男孩)\n");
		
	env = environment(me);

	if( !objectp(npc = present(id, env)) )
		return tell(me, "這裡沒有 "+id+" 這個人。\n");
	
	if( !npc->is_living() )
		return tell(me, pnoun(2, me)+"無法命令一個無生命的東西。\n");
	
	if( !npc->is_npc() || (!wizardp(me) && query("boss", npc) != me->query_id(1)) )
		return tell(me, npc->query_idname()+"並不是"+pnoun(2, me)+"雇用的。\n");

	sscanf(key, "%s %s", key, value);

	if( query("faint", npc) && key != "heal" )
		return tell(me, pnoun(2, me)+"無法命令昏倒中的員工作任何事情。\n");

	switch(lower_case(key))
	{
		case "name":
		{
			if( !value || !value[0] )
				return tell(me, pnoun(2, me)+"想要幫"+npc->query_idname()+"取什麼中文名字？\n");

			value = kill_repeat_ansi(value+NOR);

			if( !is_chinese(value) )
				return tell(me, "請"+pnoun(2, me)+"用「中文」取名字。\n");
	
			if( noansi_strlen(value) > 12 || noansi_strlen(value) < 2 )
				return tell(me, "中文名字必須在 1 到 6 個中文字之間。\n");

			msg("$ME吩咐$YOU道："+pnoun(2, npc)+"以後中文名子就改名叫做「"+value+"」吧！\n", me, npc, 1);
			npc->set_idname(0, value);
			npc->do_command("say 謝謝老闆幫我取新的中文名字，我一定會不負您的期望...");

			npc->save();				
			break;
		}
		case "id":
		{
			if( !value || !value[0] )
				return tell(me, pnoun(2, me)+"想要幫"+npc->query_idname()+"取什麼英文名字？\n");
			
			value = kill_repeat_ansi(lower_case(value)+NOR);
			
			if( !is_english(value) )
				return tell(me, "請"+pnoun(2, me)+"用「英文」取名字。\n");
			
			if( noansi_strlen(value) > 16 || noansi_strlen(value) < 3 )
				return tell(me, "英文名字必須在 3 到 16 個英文字數之間。\n");
				
			if( user_exists(remove_ansi(value)) )
				return tell(me, "不得取名為現有玩家的名字。\n");
				
			msg("$ME吩咐$YOU道："+pnoun(2, npc)+"以後英文名子就改名叫做「"+value+"」吧！\n", me, npc, 1);
			npc->set_idname(value, 0);
			npc->do_command("say 謝謝老闆幫我取新的英文名字，我一定會不負您的期望...");
			
			npc->save();
			break;
		}
		case "work":
		{
			int job;
			string job_name;
			mapping action_info;

			if( !value || !value[0] )
				return tell(me, pnoun(2, me)+"想要叫"+npc->query_idname()+"做什麼樣的工作(engineer, worker, clerk, adventurer)？\n");
			
			//if( !wizardp(me) && query("owner", env) != me->query_id(1) )
			//	return tell(me, "這裡並不是"+pnoun(2, me)+"的建築物，無法指派工作。\n");

			switch( value )
			{
				case "-d":
					if( !query("job", npc) )
						return tell(me, npc->query_idname()+"原本就沒有在進行工作。\n");
					
					msg("$ME叫$YOU停止手邊所有工作。\n", me, npc, 1);
					delete("job", npc);
					delete("position", npc);
					npc->do_command("say 是的老闆。");
					return;
				case "engineer":
					job = ENGINEER;		
					job_name = HIR"工"NOR RED"程師"NOR;
					break;
				
				case "worker":
					job = WORKER;
					job_name = HIY"工"NOR YEL"人"NOR;
					break;
				
				case "clerk":
					job = CLERK;
					job_name = HIC"店"NOR CYN"員"NOR;
					break;
				
				case "adventurer":
					job = ADVENTURER;
					job_name = HIG"探"NOR GRN"險隊"NOR;
					break;
				default:
					return tell(me, "要做何種工作(engineer, worker, clerk, adventurer)？\n"NOR);
			}
			
			//if( query("job/cur", npc) == job )
			//	return tell(me, npc->query_idname()+"已經在做"+job_name+"的工作了。\n");

			action_info = env->query_action_info(query("function", env));
			
			//if( !mapp(action_info) || !(action_info["job"] & job) )
			//	return tell(me, "這裡並不需要"+job_name+"這種工作人力。\n");
			
			//if( env->query_master() != env )
			//	return tell(me, "員工只能在連鎖中心工作，在其他地方工作是沒有用的。\n");

			msg("$ME吩咐$YOU道：從今天起"+pnoun(2, npc)+"就做"+job_name+"的工作吧！\n", me, npc, 1);
			npc->do_command("say 好的！我一定會盡心盡力的工作的！");			
			npc->do_command("follow -d");

			//broadcast(env, "只見"+npc->query_idname()+"開始著手於工作.....\n");
			
			set("position", job_name, npc);
			set("job/cur", job, npc);
			set("job/efficiency", 500, npc);
			
			break;
		}

		case "learn":
		{
			int percent;
			int energy, costsocial;
			int max_level;
			int npc_level;

			if( !value || !value[0] )
				return tell(me, pnoun(2, me)+"想要"+npc->query_idname()+"學習什麼技能？\n");

			if( sscanf(value, "%s %d%%", value, percent) == 2 )
			{
				if( percent <= 0 || percent > 100 )
					return tell(me, "請輸入正確的精神耗費百分比。\n");
					
				energy = percent * me->query_energy_max() / 100;
			}
			else
				energy = 50;

			value = lower_case(value);
			
			npc_level = npc->query_skill_level(value);
			
			if( !SKILL_D->skill_exists(value) )
				return tell(me, "並沒有 "+value+" 這種技能。\n");

			if( npc_level >= me->query_skill_level("teach") )
				return tell(me, pnoun(2, me)+"的"+(SKILL("teach"))->query_idname()+"等級不足，"+npc->query_idname()+"的"+(SKILL(value))->query_idname()+"等級不能再往上提升。\n");

			max_level = (SKILL(value))->max_level(npc);
			
			if( npc_level >= max_level )
				return tell(me, npc->query_idname()+"的"+(SKILL(value))->query_idname()+"技能等級已經到達上限。\n");

			if( !(SKILL(value))->allowable_learn(npc) )
				return tell(me, npc->query_idname()+"無法學習這項技能。\n");
				
			if( !me->cost_energy(energy) )
				return tell(me, pnoun(2, me)+"的精神不足 "+energy+"，無法再傳授技能了。\n");
				
			costsocial = energy / 10;
			
			if( costsocial < 5 )
				costsocial = 5;
			
			if( !me->cost_social_exp(costsocial) )
				return tell(me, pnoun(2, me)+"的社會經驗值不足 "+costsocial+"，無法再傳授技能了。\n");
				
			msg("$ME耗費 "+energy+" 的精神仔細地傳授$YOU有關"+(SKILL(value))->query_idname()+"的經驗。\n"+npc->query_idname()+"對"+(SKILL(value))->query_idname()+"有了更進一步的瞭解。\n", me, npc, 1);
			
			npc->add_skill_exp(value, pow((me->query_skill_level("eloquence")+1) * energy, 1.001));
			
			npc_level = npc->query_skill_level(value);
			
			if( npc_level >= max_level )
			{
				set("skills/"+value+"/level", max_level, npc);
				set("skills/"+value+"/exp", (SKILL(value))->level_exp(max_level), npc);
				//tell(me, npc->query_idname()+"的"+(SKILL(value))->query_idname()+"技能等級已經不能再往上提升。\n");
			}
			break;
		}
		case "command":
		{
			npc->do_command("say 我目前可以遵行的系統指令有："+implode(COMMAND_D->query_available_command(STANDARD), ", "));
			break;
		}
		case "heal":
		{
			msg("$ME把雙手貼在$YOU背後，以平生之力將自己的生命活力注入$YOU體中。\n", me, npc, 1);
			
			npc->set_all_ability_full();
			
			if( npc->is_faint() )
				npc->wakeup();
			else
				msg("$ME感覺精神百倍。\n", npc, 0, 1);

			npc->save();
		
			me->faint();
			break;
		}
		default:
		{
			msg("$ME命令$YOU：「"HIY+(value?key+" "+value:key)+NOR"」。\n", me, npc, 1);

			if( !wizardp(me) && (COMMAND_D->find_chn_command(key) || COMMAND_D->find_emt_command(key)) )
				return npc->do_command("say 我不能在頻道上發言也不能作任何表情指令");

			npc->do_command(value?key+" "+value:key);
			break;
		}
	}
}