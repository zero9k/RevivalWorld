/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : terrain_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-3-18
 * Note   : 地形精靈
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <map.h>
#include <daemon.h>
#include <ansi.h>
#include <terrain.h>

// 城市地圖隨機地形產生器
varargs void create_city_map_terrain(array ref city_map, mapping ref city_data)
{
	int blurred_index;
	int x, y, fx, tx, fy, ty, ox, oy, trend = 0;
	int i, j;//, x, y;
	int forest = 3+random(5); 	// 樹林的群聚數量
	int mountains = 10-forest; 	// 山區的群聚數量
	int flow_dir;
	int half_i;

	// ＊＊ 製造樹林
	while( forest-- )
	{
		ox = random(MAP_WIDTH);
	        oy = random(MAP_LENGTH);
		fx = ox - CITY_FOREST_RADIUS < 0 ? 0 : ox - CITY_FOREST_RADIUS;
		fy = oy - CITY_FOREST_RADIUS < 0 ? 0 : oy - CITY_FOREST_RADIUS;
		tx = ox + CITY_FOREST_RADIUS > MAP_WIDTH ? MAP_WIDTH : ox + CITY_FOREST_RADIUS;
		ty = oy + CITY_FOREST_RADIUS > MAP_LENGTH ? MAP_LENGTH : oy + CITY_FOREST_RADIUS;
		
		for(i=fx;i<tx;i++)
		for(j=fy;j<ty;j++)
		{
			// 模糊化半徑, 拉大範圍半徑並減少允許半徑, 可提高錯綜程度
			blurred_index = CITY_FOREST_RADIUS + random(4) - random(6) - 4;
			
			if( pythagorean_thm(i-ox, j-oy) <= blurred_index )
			{
				city_map[j][i] = HIG"＊"NOR;
				city_data[j][i][TYPE] = FOREST;
			}
		}
	}

	// ∼∼製造河川
	// 縱向
	if( random(2) )
	{
		// 河流啟始範圍從邊界以內10格
		if( !x )
			x = 10 + random(MAP_LENGTH-19);
		
		if( x > 60 ) trend = -1;
		else if( x < 40 ) trend = 1;
		
		// 河流主要流向往北(1)或往南(-1)
		flow_dir = random(2) ? 1 : -1;
		
		for(y=flow_dir==1?0:99;y<MAP_LENGTH&&y>=0;y+=flow_dir)
		{
			// 方向趨勢調整
			if( !random(20) )
			{
				if( trend == 1 || trend == -1 ) trend = 0;
				else if( !trend ) trend = random(2) ? 1 : -1;
			}
			// 模糊偏移變化, 河寬與落差的偏移有相關性
			blurred_index = random(2) - random(2) + trend;
			
			x += blurred_index;
			
			i = CITY_RIVER_WIDTH - (blurred_index ? random(2):0);
			
			if( !blurred_index ) i += random(2) - random(2);
				
			half_i = i/2;
			
			while(i--)
			{
				if( x+i >= 0 && x+i < MAP_WIDTH )
				{
					city_map[y][x+i] = BBLU HIB"∼"NOR;
					city_data[y][x+i][TYPE] = RIVER;
					
					if( y && y!=99 && city_data[y-(flow_dir)][x+i][TYPE] == RIVER )
						city_data[y][x+i][FLAGS] |= (flow_dir==1?FLOW_NORTH:FLOW_SOUTH);
					else
						city_data[y][x+i][FLAGS] |= (i<half_i?FLOW_EAST:FLOW_WEST);
				}	
			}
		}  
	}
	// 橫向
	else
	{
		// 河流啟始範圍從邊界以內10格
		if( !y )
			y = 10 + random(MAP_WIDTH-19);
		
		if( y > 60 ) trend = -1;
		else if( y < 40 ) trend = 1;
		
		// 河流主要流向往西(1)或往東(-1)
		flow_dir = random(2) ? 1 : -1;
		
		for(x=flow_dir==1?0:99;x<MAP_WIDTH&&x>=0;x+=flow_dir)
		{
			// 方向趨勢調整
			if( !random(20) )
			{
				if( trend == 1 || trend == -1 ) trend = 0;
				else if( !trend ) trend = random(2) ? 1 : -1;
			}

			// 模糊偏移變化, 河寬與落差的偏移有相關性
			blurred_index = random(2) - random(2) + trend;
			
			y += blurred_index;
			
			i = CITY_RIVER_WIDTH - (blurred_index ? random(2):0);
			
			if( !blurred_index ) i += random(2) - random(2);
			
			half_i = i/2;
			
			while(i--)
			{
				if( y+i >= 0 && y+i < MAP_LENGTH )
				{
					city_map[y+i][x] = BBLU HIB"∼"NOR;
					city_data[y+i][x][TYPE] = RIVER;
					
					if( x && x!=99 && city_data[y+i][x-(flow_dir)][TYPE] == RIVER )
						city_data[y+i][x][FLAGS] |= (flow_dir==1?FLOW_WEST:FLOW_EAST);
					else
						city_data[y+i][x][FLAGS] |= (i<half_i?FLOW_SOUTH:FLOW_NORTH);
				}
			}	
		}
	}
	
	// ◢◣ 製造山脈
	while( mountains-- )
	{
		ox = random(MAP_WIDTH);
	        oy = random(MAP_LENGTH);
		fx = ox - CITY_MOUNTAIN_RADIUS < 0 ? 0 : ox - CITY_MOUNTAIN_RADIUS;
		fy = oy - CITY_MOUNTAIN_RADIUS < 0 ? 0 : oy - CITY_MOUNTAIN_RADIUS;
		tx = ox + CITY_MOUNTAIN_RADIUS > MAP_WIDTH ? MAP_WIDTH : ox + CITY_MOUNTAIN_RADIUS;
		ty = oy + CITY_MOUNTAIN_RADIUS > MAP_LENGTH ? MAP_LENGTH : oy + CITY_MOUNTAIN_RADIUS;

		for(j=fy;j<ty;j++) 
		for(i=fx;i<tx;i++)
		{
			// 模糊化半徑, 拉大範圍半徑並減少允許半徑, 可提高錯綜程度
			blurred_index = CITY_MOUNTAIN_RADIUS + random(4) - random(6) - 4;
			
			// 讓全部山脈都是完整的◢◣
			
			if( pythagorean_thm(i-ox, j-oy) <= blurred_index && city_map[j][i] == "．" && i+1 < MAP_WIDTH && city_map[j][i+1] == "．")
			{
				
				city_map[j][i] = YEL + "◢" + NOR;
				city_data[j][i][TYPE] = MOUNTAIN;
				
				city_map[j][++i] = YEL + "◣" + NOR;
				city_data[j][i][TYPE] = MOUNTAIN;		

				if( random(2) ) i++; // 增加山脈錯綜程度
			}
		}
	}
}

// 區域地圖隨機地形產生器
varargs void create_area_map_terrain(array ref area_map, mapping ref area_data, int maptype)
{
	if( maptype == MAP_OCEAN )
	{
		int x, y;
		int fx, tx, fy, ty, ox, oy;
		int island = random(5);
		int blurred_index;

		for(x=0;x<MAP_WIDTH;x++)
		for(y=0;y<MAP_LENGTH;y++)
		{
			area_map[x][y] = BBLU HIB"∴"NOR;
			area_data[x][y][TYPE] = SEA;
		}
	
		// 製造島嶼
		while(island--)
		{
			ox = SEA_ISLAND_RADIUS + random(MAP_WIDTH - SEA_ISLAND_RADIUS*2);
		        oy = SEA_ISLAND_RADIUS + random(MAP_LENGTH - SEA_ISLAND_RADIUS*2);
			fx = ox - SEA_ISLAND_RADIUS < 0 ? 0 : ox - SEA_ISLAND_RADIUS;
			fy = oy - SEA_ISLAND_RADIUS < 0 ? 0 : oy - SEA_ISLAND_RADIUS;
			tx = ox + SEA_ISLAND_RADIUS > MAP_WIDTH ? MAP_WIDTH : ox + SEA_ISLAND_RADIUS;
			ty = oy + SEA_ISLAND_RADIUS > MAP_LENGTH ? MAP_LENGTH : oy + SEA_ISLAND_RADIUS;
			
			for(x=fx;x<tx;x++)
			for(y=fy;y<ty;y++)
			{
				// 模糊化半徑, 拉大範圍半徑並減少允許半徑, 可提高錯綜程度
				blurred_index = SEA_ISLAND_RADIUS - random(4);
				
				if( pythagorean_thm(x-ox, y-oy) <= blurred_index )
				{
					area_map[x][y] = BYEL HIG"＊"NOR;
					area_data[x][y][TYPE] = FOREST;
				}
			}
		}
		
		return;	
	}
	
	else if( maptype == MAP_FOREST )
	{
		int x, y;
		int blurred_index;
		int fx, tx, fy, ty, ox, oy, trend = 0;
		int i, j;//, x, y;
		int forest = 10+random(5); 	// 樹林的群聚數量
		int mountains = 5+random(5); 	// 山區的群聚數量
		int flow_dir;
		int half_i;
		
		
		// ＊＊ 製造樹林
		while( forest-- )
		{
			ox = random(MAP_WIDTH);
		        oy = random(MAP_LENGTH);
			fx = ox - AREA_FOREST_RADIUS < 0 ? 0 : ox - AREA_FOREST_RADIUS;
			fy = oy - AREA_FOREST_RADIUS < 0 ? 0 : oy - AREA_FOREST_RADIUS;
			tx = ox + AREA_FOREST_RADIUS > MAP_WIDTH ? MAP_WIDTH : ox + AREA_FOREST_RADIUS;
			ty = oy + AREA_FOREST_RADIUS > MAP_LENGTH ? MAP_LENGTH : oy + AREA_FOREST_RADIUS;
			
			for(i=fx;i<tx;i++)
			for(j=fy;j<ty;j++)
			{
				// 模糊化半徑, 拉大範圍半徑並減少允許半徑, 可提高錯綜程度
				blurred_index = AREA_FOREST_RADIUS - random(20);
				
				if( pythagorean_thm(i-ox, j-oy) <= blurred_index )
				{
					area_map[j][i] = HIG"＊"NOR;
					area_data[j][i][TYPE] = FOREST;
				}
			}
		}
	
		// ∼∼製造河川
		// 縱向
		if( random(2) )
		{
			// 河流啟始範圍從邊界以內10格
			if( !x )
				x = 10 + random(MAP_LENGTH-19);
			
			if( x > 60 ) trend = -1;
			else if( x < 40 ) trend = 1;
			
			// 河流主要流向往北(1)或往南(-1)
			flow_dir = random(2) ? 1 : -1;
			
			for(y=flow_dir==1?0:99;y<MAP_LENGTH&&y>=0;y+=flow_dir)
			{
				// 方向趨勢調整
				if( !random(20) )
				{
					if( trend == 1 || trend == -1 ) trend = 0;
					else if( !trend ) trend = random(2) ? 1 : -1;
				}
				// 模糊偏移變化, 河寬與落差的偏移有相關性
				blurred_index = random(2) - random(2) + trend;
				
				x += blurred_index;
				
				i = AREA_RIVER_WIDTH - (blurred_index ? random(2):0);
				
				if( !blurred_index ) i += random(2) - random(2);
					
				half_i = i/2;
				
				while(i--)
				{
					if( x+i >= 0 && x+i < MAP_WIDTH )
					{
						area_map[y][x+i] = BBLU HIB"∼"NOR;
						area_data[y][x+i][TYPE] = RIVER;
						
						if( y && y!=99 && area_data[y-(flow_dir)][x+i][TYPE] == RIVER )
							area_data[y][x+i][FLAGS] |= (flow_dir==1?FLOW_NORTH:FLOW_SOUTH);
						else
							area_data[y][x+i][FLAGS] |= (i<half_i?FLOW_EAST:FLOW_WEST);
					}	
				}
			}  
		}
		// 橫向
		else
		{
			// 河流啟始範圍從邊界以內10格
			if( !y )
				y = 10 + random(MAP_WIDTH-19);
			
			if( y > 60 ) trend = -1;
			else if( y < 40 ) trend = 1;
			
			// 河流主要流向往西(1)或往東(-1)
			flow_dir = random(2) ? 1 : -1;
			
			for(x=flow_dir==1?0:99;x<MAP_WIDTH&&x>=0;x+=flow_dir)
			{
				// 方向趨勢調整
				if( !random(20) )
				{
					if( trend == 1 || trend == -1 ) trend = 0;
					else if( !trend ) trend = random(2) ? 1 : -1;
				}
	
				// 模糊偏移變化, 河寬與落差的偏移有相關性
				blurred_index = random(2) - random(2) + trend;
				
				y += blurred_index;
				
				i = AREA_RIVER_WIDTH - (blurred_index ? random(2):0);
				
				if( !blurred_index ) i += random(2) - random(2);
				
				half_i = i/2;
				
				while(i--)
				{
					if( y+i >= 0 && y+i < MAP_LENGTH )
					{
						area_map[y+i][x] = BBLU HIB"∼"NOR;
						area_data[y+i][x][TYPE] = RIVER;
						
						if( x && x!=99 && area_data[y+i][x-(flow_dir)][TYPE] == RIVER )
							area_data[y+i][x][FLAGS] |= (flow_dir==1?FLOW_WEST:FLOW_EAST);
						else
							area_data[y+i][x][FLAGS] |= (i<half_i?FLOW_SOUTH:FLOW_NORTH);
					}
				}	
			}
		}
		
		// ◢◣ 製造山脈
		while( mountains-- )
		{
			ox = random(MAP_WIDTH);
		        oy = random(MAP_LENGTH);
			fx = ox - AREA_MOUNTAIN_RADIUS < 0 ? 0 : ox - AREA_MOUNTAIN_RADIUS;
			fy = oy - AREA_MOUNTAIN_RADIUS < 0 ? 0 : oy - AREA_MOUNTAIN_RADIUS;
			tx = ox + AREA_MOUNTAIN_RADIUS > MAP_WIDTH ? MAP_WIDTH : ox + AREA_MOUNTAIN_RADIUS;
			ty = oy + AREA_MOUNTAIN_RADIUS > MAP_LENGTH ? MAP_LENGTH : oy + AREA_MOUNTAIN_RADIUS;
	
			for(j=fy;j<ty;j++) 
			for(i=fx;i<tx;i++)
			{
				// 模糊化半徑, 拉大範圍半徑並減少允許半徑, 可提高錯綜程度
				blurred_index = AREA_MOUNTAIN_RADIUS - random(20);
				
				// 讓全部山脈都是完整的◢◣
				
				if( pythagorean_thm(i-ox, j-oy) <= blurred_index 
					&& remove_ansi(area_map[j][i]) != "◢" 
					&& remove_ansi(area_map[j][i]) != "◣"
					&& i+1 < MAP_WIDTH 
					&& remove_ansi(area_map[j][i+1]) != "◢" 
					&& remove_ansi(area_map[j][i+1]) != "◣"
					)
				{
					
					area_map[j][i] = ansi_part(area_map[j][i]) + YEL + "◢" + NOR;
					area_data[j][i][TYPE] = MOUNTAIN;
					
					area_map[j][++i] = ansi_part(area_map[j][i]) + YEL + "◣" + NOR;
					area_data[j][i][TYPE] = MOUNTAIN;		
	
					if( random(2) ) i++; // 增加山脈錯綜程度
				}
			}
		}
	}
}

string query_name()
{
	return "地形產生系統(TERRAIN_D)";
}
