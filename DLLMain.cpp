#include "SDK.h"
#include "Client.h"
#include "Panels.h"

COffsets g_offsets;
CGlobalVariables g_cvars;
CInterfaces interfaces;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;

DWORD WINAPI dwMainThread(LPVOID lpArguments)
{
	if (interfaces.Client == NULL)
	{
		VMTBaseManager* clientHook = new VMTBaseManager();
		VMTBaseManager* clientModeHook = new VMTBaseManager();
		VMTBaseManager* panelHook = new VMTBaseManager();

		ClientFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("client.dll"), "CreateInterface");
		EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
		VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");

		interfaces.Client = (CHLClient*)ClientFactory("VClient017", NULL);
		interfaces.EntList = (CEntList*)ClientFactory("VClientEntityList003", NULL);
		interfaces.Engine = (EngineClient*)EngineFactory("VEngineClient013", NULL);
		interfaces.Surface = (ISurface*)VGUIFactory("VGUI_Surface030", NULL);
		interfaces.EngineTrace = (IEngineTrace*)EngineFactory("EngineTraceClient003", NULL);
		interfaces.ModelInfo = (IVModelInfo*)EngineFactory("VModelInfoClient006", NULL);

		XASSERT(interfaces.Client);
		XASSERT(interfaces.EntList);
		XASSERT(interfaces.Engine);
		XASSERT(interfaces.Surface);
		XASSERT(interfaces.EngineTrace);
		XASSERT(interfaces.ModelInfo);

		//Setup the Panel hook so we can draw.
		if (!interfaces.Panels)
		{
			VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
			interfaces.Panels = (IPanel*)VGUI2Factory("VGUI_Panel009", NULL);
			XASSERT(interfaces.Panels);

			if (interfaces.Panels)
			{
				panelHook->Init(interfaces.Panels);
				panelHook->HookMethod(&Hooked_PaintTraverse, g_offsets.iPaintTraverseOffset);
				panelHook->Rehook();
			}
		}

		DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
		XASSERT(dwClientModeAddress);
		interfaces.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);
		LOGDEBUG("g_pClientModeShared_ptr client.dll+0x%X", (DWORD)interfaces.ClientMode - dwClientBase);

		clientHook->Init(interfaces.Client);
		clientHook->HookMethod(&Hooked_KeyEvent, g_offsets.iKeyEventOffset);
		clientHook->Rehook();

		clientModeHook->Init(interfaces.ClientMode);
		clientModeHook->HookMethod(&Hooked_CreateMove, g_offsets.iCreateMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
		clientModeHook->Rehook();
	}
	return 0; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		Log::Init(hInstance);
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL ); 
	}
	return true;
}