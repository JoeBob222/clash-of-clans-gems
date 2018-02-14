#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "Menu.h"

CScreenSize g_screensize;
CDrawManager g_drawmanager;
//===================================================================================
void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	try
	{
		VMTManager& hook = VMTManager::GetHook(pPanels);
		hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(g_offsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int vguiFocusOverlayPanel;

		if (vguiFocusOverlayPanel == NULL)
		{											//FocusOverlayPanel
			const char* szName = interfaces.Panels->GetName(vguiPanel);
			if (szName[0] == 'F' && szName[5] == 'O' &&  szName[12] == 'P')
			{
				vguiFocusOverlayPanel = vguiPanel;
				Intro();
			}
		}

		if (vguiFocusOverlayPanel == vguiPanel)
		{
			int iWidth, iHeight; //Resolution fix, so this can work in Fullscreen
			interfaces.Engine->GetScreenSize(iWidth, iHeight);
			if (g_screensize.iScreenWidth != iWidth || g_screensize.iScreenHeight != iHeight)
				interfaces.Engine->GetScreenSize(g_screensize.iScreenWidth, g_screensize.iScreenHeight);

			if (interfaces.Engine->IsDrawingLoadingImage() || !interfaces.Engine->IsInGame() || !interfaces.Engine->IsConnected() || interfaces.Engine->Con_IsVisible())
			{
				menu.DrawWindow();
				return;
			}

			CBaseEntity* pLocal = interfaces.EntList->GetClientEntity(me);

			if (!pLocal)
				return;

			// menu and esp
		}
	}
	catch (...)
	{
		Log::Fatal("Failed PaintTraverse");
	}
}
//===================================================================================
void Intro(void)
{
	try
	{
		g_drawmanager.Initialize(); //Initalize the drawing class.

		gNetVars.Initialize();
	}
	catch (...)
	{
		Log::Fatal("Failed Intro");
	}
}