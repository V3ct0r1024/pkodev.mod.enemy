#pragma once
#include "address.h"
#include "structure.h"

namespace pkodev
{
	namespace pointer
	{
		// void CHeadSay::Render(D3DXVECTOR3& pos)
		typedef void(__thiscall* CHeadSay__Render__Ptr)(void*, void*);
		CHeadSay__Render__Ptr CHeadSay__Render = (CHeadSay__Render__Ptr)(void*)(address::MOD_EXE_VERSION::CHeadSay__Render);

		// void CCharacter::setSideID(long side_id)
		typedef void(__thiscall* CCharacter__setSideID__Ptr)(void*, int);
		CCharacter__setSideID__Ptr CCharacter__setSideID = (CCharacter__setSideID__Ptr)(void*)(address::MOD_EXE_VERSION::CCharacter__setSideID);

		// void NetSwitchMap(stNetSwitchMap &switchmap)
		typedef void(__cdecl* NetSwitchMap__Ptr)(stNetSwitchMap&);
		NetSwitchMap__Ptr NetSwitchMap = (NetSwitchMap__Ptr)(void*)(address::MOD_EXE_VERSION::NetSwitchMap);

		// void CGameScene::_RenderUI()
		typedef void(__thiscall* CGameScene___RenderUI__Ptr)(void*);
		CGameScene___RenderUI__Ptr CGameScene___RenderUI = (CGameScene___RenderUI__Ptr)(void*)(address::MOD_EXE_VERSION::CGameScene___RenderUI);

		// CGuiPic::CGuiPic( CGuiData* pParent, unsigned int max )
		typedef void(__thiscall* CGuiPic__CGuiPic__Ptr)(void*, void*, unsigned int);
		CGuiPic__CGuiPic__Ptr CGuiPic__CGuiPic = (CGuiPic__CGuiPic__Ptr)(void*)(address::MOD_EXE_VERSION::CGuiPic__CGuiPic);

		// bool	CGuiPic::LoadImage( const char * file, int w, int h, int frame=0, int tx=0, int ty=0, float scalex=0.0, float scaley=0.0 )
		typedef bool(__thiscall* CGuiPic__LoadImage__Ptr)(void*, const char*, int, int, int, int, int, float, float);
		CGuiPic__LoadImage__Ptr CGuiPic__LoadImage = (CGuiPic__LoadImage__Ptr)(void*)(address::MOD_EXE_VERSION::CGuiPic__LoadImage);

		// inline void CGuiPic::Render( int x, int y )
		typedef void(__thiscall* CGuiPic__Render__Ptr)(void*, int, int, int);
		CGuiPic__Render__Ptr CGuiPic__Render = (CGuiPic__Render__Ptr)(void*)(address::MOD_EXE_VERSION::CGuiPic__Render);
	}
}