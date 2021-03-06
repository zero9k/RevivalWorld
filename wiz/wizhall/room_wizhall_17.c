/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_wizhall_17.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-01-10
 * Note   : 標準房間
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

void create()
{
	set("short", HIW"巫師"NOR WHT"通道"NOR);
	set("long",@TEXT
　　巫師通道通往各個巫師神殿的神秘角落，只見每個方向望過去都是深不見底的
路口，通道的兩側石璧上有著各式各樣的雕刻作品與繪畫，但完全無法理解其作品
的含意，讓人在這個通道轉角感覺到一股神秘的氣息。
TEXT);

	set("exits", ([
		"down" : "/wiz/wizhall/room_wizhall_7",
		"up" : "/wiz/wizhall/room_wizhall_8",
		"north" : "/wiz/wizhall/room_wizhall_9",
	]));
	
	::reset_objects();
}
