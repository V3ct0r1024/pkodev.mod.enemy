#pragma once
#include <string>
#include <vector>

namespace pkodev
{
	// Modification settings structure
	struct stSettings
	{
		// Highlight main character
		bool main_cha;

		// Colored names
		bool colored_name;

		// Green/Red circle
		bool colored_circle;

		// Warning icon
		struct
		{
			bool show;
			int x;
			int y;
		}
		warning;

		// Enemy name color
		unsigned int enemy_color;

		// Friend name color
		unsigned int friend_color;

		// List of allowed maps
		std::vector<std::string> maps;

		// Constructor
		stSettings() :
			main_cha(true),
			colored_name(true),
			colored_circle(true),
			enemy_color(0xFFFF0000),
			friend_color(0xFF00FF00)
		{
			warning.show = true;
			warning.x = 32;
			warning.y = 32;
			maps.reserve(16);
		}
	};

	// stNetSwitchMap structure from client source code
	struct stNetSwitchMap
	{
		short sEnterRet;
		char* szMapName;
		char chEnterType;
		bool bIsNewCha;
		bool bCanTeam;
	};

	// ChaRecord structure from client source code
	struct stChaRecord
	{
		char nop[0x010C];
		float height;
	};

	// CCharacter structure from client source code
	struct stCharacter
	{
		char nop0[0xD4];
		int hp;
		char nop1[0x5c];
		int unknown0;
		char nop2[0x14];
		int max_hp;
		char nop3[0x02c4];
		stChaRecord* Info;
		char nop33[0x04];
		const char name[0x21];
		char nop4[0xA7];
		int  guild_id;
		int  guild_color;
		char nop5[0x003c];
		int  side_id;
		char nop6[4];
		void* pStateMgr;
		char nop7[0x2c];
		int area_type;
		char nop8[0x03fa];
		short int ring_id;
		char nop9[0x01E4];
		short int neck_id;
		char nop10[0x0156];
		int  party_id;
		char nop11[0x0038];
		int type;
		char nop12[0x08];
		int cha_id;
	};
}