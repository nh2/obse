#include <map>
#include "InternalSerialization.h"
#include "StringVar.h"
#include "ArrayVar.h"
#include "GameData.h"
#include "common/IFileStream.h"
#include <algorithm>
#include <string>
#include "Script.h"

/*******************************
*	Callbacks
*******************************/
void SaveModList(OBSESerializationInterface* obse)
{
	DataHandler* dhand = *g_dataHandler;
	UInt8 modCount = dhand->numLoadedMods;

	obse->OpenRecord('MODS', 0);
	obse->WriteRecordData(&modCount, sizeof(modCount));
	for (UInt32 i = 0; i < modCount; i++)
	{
		UInt16 nameLen = strlen(dhand->modsByID[i]->name);
		obse->WriteRecordData(&nameLen, sizeof(nameLen));
		obse->WriteRecordData(dhand->modsByID[i]->name, nameLen);
	}
}

static UInt8	s_preloadModRefIDs[0xFF];
static UInt8	s_numPreloadMods = 0;

bool ReadModListFromCoSave(OBSESerializationInterface * intfc)
{
	_MESSAGE("Reading mod list from co-save");

	char name[0x104] = { 0 };
	UInt16 nameLen = 0;

	intfc->ReadRecordData(&s_numPreloadMods, sizeof(s_numPreloadMods));
	for (UInt32 i = 0; i < s_numPreloadMods; i++) {
		intfc->ReadRecordData(&nameLen, sizeof(nameLen));
		intfc->ReadRecordData(&name, nameLen);
		name[nameLen] = 0;

		s_preloadModRefIDs[i] = (*g_dataHandler)->GetModIndex(name);
	}
	return true;
}

bool ReadModListFromSaveGame(const char* path)
{
	_MESSAGE("Reading mod list from savegame");

	IFileStream savefile;
	if (!savefile.Open(path)) {
		_MESSAGE("Couldn't open .ess file when attempting to read plugin list");
		return false;
	}
	else {
		static const UInt32 kSaveHeaderSizeOffset = 34;
		savefile.SetOffset(kSaveHeaderSizeOffset);
		UInt32 headerSize = savefile.Read32();
		savefile.SetOffset(headerSize + kSaveHeaderSizeOffset + sizeof(UInt32));

		s_numPreloadMods = savefile.Read8();
		char pluginName[0x100];
		for (UInt32 i = 0; i < s_numPreloadMods; i++) {
			UInt8 nameLen = savefile.Read8();
			savefile.ReadBuf(pluginName, nameLen);
			pluginName[nameLen] = 0;
			_MESSAGE("Save file contains plugin %s", pluginName);
			s_preloadModRefIDs[i] = (*g_dataHandler)->GetModIndex(pluginName);
		}

		savefile.Close();
	}

	return true;
}

UInt8 ResolveModIndexForPreload(UInt8 modIndexIn)
{
	return (modIndexIn < s_numPreloadMods) ? s_preloadModRefIDs[modIndexIn] : 0xFF;
}

void Core_SaveCallback(void * reserved)
{
	SaveModList(&g_OBSESerializationInterface);
	g_StringMap.Save(&g_OBSESerializationInterface);
	g_ArrayMap.Save(&g_OBSESerializationInterface);
}

void Core_LoadCallback(void * reserved)
{
	OBSESerializationInterface* intfc = &g_OBSESerializationInterface;
	UInt32 type, version, length;

	while (intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		case 'STVS':
		case 'STVR':
		case 'STVE':
		case 'ARVS':
		case 'ARVR':
		case 'ARVE':
		case 'MODS':	
			break;		// processed during preload
		default:
			_MESSAGE("Unhandled chunk type in LoadCallback: %d", type);
			continue;
		}
	}
}

void Core_NewGameCallback(void * reserved)
{
	g_ArrayMap.Clean();
	g_StringMap.Clean();

	// below are commented out because it is possible for quest scripts to create string/array vars during
	// main menu - in which case they would get obliterated when a new game is started.

	//g_StringMap.Reset(&g_OBSESerializationInterface);
	//g_ArrayMap.Reset(&g_OBSESerializationInterface);
}

void Core_PostLoadCallback(bool bLoadSucceeded)
{
	g_ArrayMap.PostLoad(bLoadSucceeded);
	g_StringMap.PostLoad(bLoadSucceeded);
}

void Core_PreloadCallback(void * reserved)
{
	// this is invoked only if at least one other plugin registers a preload callback
	
	// reset refID fixup table. if save made prior to 0019, this will remain empty
	s_numPreloadMods = 0;	// no need to zero out table - unloaded mods will be set to 0xFF below

	OBSESerializationInterface* intfc = &g_OBSESerializationInterface;

	g_ArrayMap.Preload();
	g_StringMap.Preload();

	UInt32 type, version, length;

	while (intfc->GetNextRecordInfo(&type, &version, &length)) {
		switch (type) {
			case 'MODS':
				// as of 0019 mod list stored in co-save
				ReadModListFromCoSave(intfc);
				break;
			case 'STVS':
				if (!s_numPreloadMods) {
					// pre-0019 co-save doesn't contain mod list, read from .ess instead
					ReadModListFromSaveGame((const char*)reserved);
				}

				g_StringMap.Load(intfc);
				break;
			case 'ARVS':
				if (!s_numPreloadMods) {
					// pre-0019 co-save doesn't contain mod list, read from .ess instead
					ReadModListFromSaveGame((const char*)reserved);
				}

				g_ArrayMap.Load(intfc);
				break;
			default:
				break;
		}
	}
}


void Init_CoreSerialization_Callbacks()
{
	Serialization::InternalSetSaveCallback(0, Core_SaveCallback);
	Serialization::InternalSetLoadCallback(0, Core_LoadCallback);
	Serialization::InternalSetNewGameCallback(0, Core_NewGameCallback);
	Serialization::InternalSetPreloadCallback(0, Core_PreloadCallback);
}
