#pragma once
#include "structure.h"

namespace pkodev
{
	namespace hook
	{
		// void CHeadSay::Render(D3DXVECTOR3& pos)
		void __fastcall CHeadSay__Render(void* This, void* NotUsed, void* Pos);

		// void NetSwitchMap(stNetSwitchMap &switchmap)
		void __cdecl NetSwitchMap(stNetSwitchMap& switchmap);

		// void CGameScene::_RenderUI()
		void __fastcall CGameScene___RenderUI(void* This, void* NotUsed);
	}
}