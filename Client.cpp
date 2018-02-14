#include "SDK.h"
#include "Client.h"
#include "Util.h"

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd* g_cmd)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, CUserCmd*)>(g_offsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, g_cmd); //Call the original.
	try
	{
		if (!g_cmd->command_number)
			return false;

		CBaseEntity* pLocal = GetBaseEntity(me); 

		if (!pLocal)
			return bReturn;

		// stuff here

	}
	catch(...)
	{
		Log::Fatal("Failed Hooked_CreateMove");
	}
	return false;
}
//============================================================================================
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding)
{
	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	return hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(g_offsets.iKeyEventOffset)(CHLClient, eventcode, keynum, currentBinding); // Call the original.
}