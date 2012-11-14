#include "loader_common/EXEChecksum.h"

IDebugLog	gLog("obse_steam_loader.log");

static void OnAttach(void);

BOOL WINAPI DllMain(HANDLE procHandle, DWORD reason, LPVOID reserved)
{
	if(reason == DLL_PROCESS_ATTACH)
	{
		OnAttach();
	}

	return TRUE;
}

static void OnAttach(void)
{
	gLog.SetPrintLevel(IDebugLog::kLevel_Error);
	gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

	std::string	dllSuffix;
	bool		steamVersion;

	// paranoia check
	if(!TestChecksum("oblivion.exe", &dllSuffix, &steamVersion))
	{
		_ERROR("checksum not found");
		return;
	}

	// /extreme/ paranoia check
	if(!steamVersion)
	{
		_ERROR("not a steam version!?");
		return;
	}

	// build full path to our dll
	std::string	dllPath;

	dllPath = GetCWD() + "\\obse_" + dllSuffix + ".dll";

	_MESSAGE("dll = %s", dllPath.c_str());

	// load it
	if(!LoadLibrary(dllPath.c_str()))
	{
		_ERROR("couldn't load dll");
	}
}
