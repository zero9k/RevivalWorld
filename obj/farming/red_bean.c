/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : rice seedlings.c
 * Author : 
 * Date   : 2003-5-1
 * Note   : 紅豆
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_OBJECT;

void create()
{
        set_idname("red bean","紅豆");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("unit", "顆");
        set("mass", 5);
        set("value", 5);
        set("badsell", 1);

        // 種子資訊
        set("seed_data",
                ([
                        //插秧播種, 灌溉, 收割所耗體力設定
                        "str_cost": ([
                                "sow"           :2, //插秧或播種時需要的體力
                                "irrigate"      :3, //灌溉每升水需要的體力 
                                "reap"          :3 //收割時需要的體力
                                ]),
                        
                        //種子種類
                        "seed":"紅豆",
                        
                        //設定插秧或是播種
                        "sow_type":"播種",
                        
                        //收割產品的路徑與數量設定
                        "reap":({ 20, "/obj/farming/red_bean.c"}),
                        
                        //各個步驟的農作訊息
                        "msg":
                        ([
                                // 插秧播種時的訊息
                                "sow":
                                        "紅豆整齊地佈滿整片田地，一片欣欣向榮的感覺。\n" ,
                                
                                // 灌溉時的訊息
                                "irrigate":
                                ({
                                        3,"紅豆田上小小的幼苗已經漸漸長大了。\n",
                                        5,"只見紅豆田一片綠意盎然，充滿了生氣。\n",
                                        10,"紅豆田上的紅豆已經漸漸成熟了。\n",
                                        15,"只見一整片綠油油的紅豆，已經不用再灌溉了，耐心等一陣子便可以收割。\n",
                                }),
                                
                                // 成熟收割時的訊息
                                "reap":
                                ({
                                        20,
                                        "這片紅豆田上的紅豆已經全部成熟了，終於可以收割了。\n",
                                        "這片紅豆田經過多日來的灌溉，終於成熟收割了。\n共收割了$NUM顆的紅豆。\n"
                                }),
                                
                                // 荒廢時的訊息
                                "neglect":
                                ({      
                                        200,
                                        "這片紅豆田由於多日來缺乏灌溉，上面的紅豆芽漸漸枯萎了。\n",
                                        "這片紅豆田由於多日來缺乏灌溉，已經完全荒廢了。\n",
                                }),
                                
                                "weather":
                                ([
                                        "RAINY":"連日來的雨量使得一些紅豆泡水腐爛了。\n",
                                        "SNOWY":"大雪連連嚴重使得紅豆苗們陸續地枯萎了。\n",
                                        "WINDY":"狂風吹襲使得部份紅豆苗被強風攔腰折斷。\n",
                                ]),
                        ]),
                ])
        );
}


