/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _skill_liv.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-04
 * Note   : 技能繼承檔
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <skill.h>
#include <daemon.h>

int query_skill_level(string skill)
{
	return query(SKILL_LV(skill));
}

float query_skill_exp(string skill)
{
	return query(SKILL_EXP(skill));
}

int remove_skill(string skill)
{
	if( !query(SKILL_KEY(skill)) ) return 0;
	delete(SKILL_KEY(skill));
	return 1;
}


// 進行升級
int skill_level_up(string skill)
{
	int new_lv;
	object me = this_object();
	
	new_lv = addn(SKILL_LV(skill), 1);

	// 顯示升級訊息
	tell(me, sprintf(NOR YEL"%s的"HIY"%s"NOR YEL"等級提升為 "HIY"%d"NOR YEL" 級。\n"NOR,
		pnoun(2, me), (SKILL(skill))->query_idname(), new_lv));
	
	// 執行升級相關動作, 先升級再產生升級事件
	(SKILL(skill))->after_level_up(me, new_lv);
	
	// 處理技能頭銜
	(SKILL(skill))->add_title(me, new_lv);
	
	return new_lv;
}

// 進行降級
int skill_level_down(string skill)
{
	int new_lv, cur_lv;
	object me = this_object();
	
	cur_lv = query_skill_level(skill);
	
	// 執行降級相關動作, 先產生降級事件再降級
	(SKILL(skill))->before_level_down(me, cur_lv);

	// 處理技能頭銜
	(SKILL(skill))->remove_title(me, cur_lv);

	new_lv = addn(SKILL_LV(skill), -1);
	
	// 顯示降級訊息
	tell(me, sprintf(NOR YEL"%s的"HIY"%s"NOR YEL"等級退步為 "HIY"%d"NOR YEL" 級。\n"NOR,
		pnoun(2, me), (SKILL(skill))->query_idname(), new_lv));

	return new_lv;
}

// 經驗值計算檢查是否升級
int exp_calculate(string skill)
{
	object me = this_object();
	int cur_exp = query_skill_exp(skill);
	int cur_lv, max_lv;
	int level_change;

	max_lv = (SKILL(skill))->max_level(this_object());

	// 用 while 作連續升級動作
	while(1)
	{
		cur_lv = query_skill_level(skill);

		if( cur_exp < (SKILL(skill))->level_exp(cur_lv + 1) || cur_lv >= max_lv ) break;
			
		skill_level_up(skill);
		
		level_change = 1;
	}

	// 用 while 作連續降級動作	
	while(1)
	{
		cur_lv = query_skill_level(skill);

		if( cur_exp >= (SKILL(skill))->level_exp(cur_lv) || cur_lv <= 0 ) break;
		
		skill_level_down(skill);
		
		level_change = 1;
	}

	if( level_change )
	{
		if( userp(me) && !SECURE_D->is_wizard(me->query_id(1)) )
			TOP_D->update_top("skill/"+skill, me->query_id(1), cur_lv, me->query_idname(), query("city"));
		
		me->save();
	}
}

// 增加經驗值
int add_skill_exp(string skill, mixed exp)
{
	float new_exp, max_exp;
	object skillob = load_object(SKILL(skill));

	// 檢查技能是否存在
	if( !objectp(skillob) ) return 0;
	
	exp = to_float(exp);

	// 經驗值乘數
	exp *= skillob->exp_multiple(this_object());

	new_exp = query_skill_exp(skill) + exp;
	max_exp = to_float(skillob->level_exp(skillob->max_level(this_object())));

	if( new_exp < 0 )
		set(SKILL_EXP(skill), 0.);
	else if( new_exp > max_exp )
		set(SKILL_EXP(skill), max_exp);
	else
		set(SKILL_EXP(skill), new_exp);

	// 經驗值增加之後的事件
	skillob->added_exp(this_object(), exp);
	
	// 計算經驗值檢查是否要升級
	exp_calculate(skill);
}


// 回傳下一等級需要經驗值
int next_level_exp(string skill)
{
	return (SKILL(skill))->level_exp(query_skill_level(skill) + 1);
}


// 回傳最高等級之技能
string query_highest_skill()
{
	mapping skills = query("skills");
	string *sortskill;
	
	if( !mapp(skills) )
		return 0;
	
	sortskill = sort_array(keys(skills), (: $(skills)[$1]["level"] >  $(skills)[$2]["level"] ? -1 : 1 :));
	
	return sortskill[0];
}
