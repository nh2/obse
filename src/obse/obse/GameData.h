#pragma once

#include "obse/GameForms.h"
#include "obse/GameObjects.h"

// 10
struct BoundObjectListHead
{
	UInt32			boundObjectCount;	// 0
	TESBoundObject	* first;			// 4
	TESBoundObject	* last;				// 8
	UInt32			unkC;				// C
};

// 8
class TESRegionDataManager
{
public:
	TESRegionDataManager();
	~TESRegionDataManager();

	virtual void	Destructor(void) = 0;	// 0
	virtual void	Unk_1(void) = 0;
	virtual void	Unk_2(void) = 0;
	virtual void	Unk_3(void) = 0;
	virtual void	Unk_4(void) = 0;
	virtual void	Unk_5(void) = 0;
	virtual void	Unk_6(void) = 0;
	virtual void	Unk_7(void) = 0;
	virtual void	Unk_8(void) = 0;

//	void	** _vtbl;
	UInt32	unk4;
};

// 10
class TESRegionList
{
public:
	TESRegionList();
	~TESRegionList();

	virtual void	Destructor(void) = 0;	// 0
	// no other virtual fns

	struct Entry {
		TESRegion	* region;
		Entry		* next;
	};

//	void	** _vtbl;	// 00
	Entry	regionList;	// 04
	UInt8	unkC;		// 0C
	UInt8	padD[3];
};

// note: this list contains all esm/esp files in the Data folder, even ones that are not loaded
struct ModEntry
{
	// 41C / 420
	struct Data		// referred to as 'TESFile' by Bethesda
	{
		enum
		{
			kFlag_IsMaster =	1 << 0,
			kFlag_Loaded =		1 << 2,
			kFlag_Active =		1 << 3
		};

		template <typename tData> struct Node
		{
			tData		* data;
			Node<tData>	* next;
		};

		struct	ChunkInfo
		{
			UInt32	type;		// e.g. 'GRUP', 'GLOB', etc
			UInt32	length;
		};

		struct	FormInfo	// ###TODO double check this, see 46B910 (TESForm virtual func, accepts Unk23C* as arg)
		{
			ChunkInfo	chunkInfo;
			UInt32		flags;
			UInt32		formID;
			UInt32		unk10;
		};

		struct  SizeInfo	// as seen in the editor
		{
			UInt32		fileSizeLow;			// WIN32_FIND_DATA::nFileSizeLow 
			UInt32		fileSizeHigh;			// WIN32_FIND_DATA::nFileSizeHigh
		};

		// static members: B33C1C, B33C20

		UInt32	unk000;							// 000 appears to indicate status of file (open, closed, etc) 2, 9, 0C do stuff
		UInt32	unk004;							// 004
		UInt32	unk008;							// 008
		UInt32	unk00C;							// 00C
		BSFile	* bsFile;						// 010
		UInt32	unk014;							// 014
		UInt32	unk018;							// 018
		char	name[0x104];					// 01C
		char	filepath[0x104];				// 120 relative to "Oblivion\"
		UInt32	unk224;							// 224
		UInt32	unk228;							// 228 init to *(0xB055CC), seen 0x2800
		UInt32	unk22C[(0x23C - 0x22C) >> 2];	// 22C
		FormInfo	formInfo;					// 23C
		UInt32	chunkType250;					// 250
		UInt32	unk254[(0x290 - 0x254) >> 2];	// 254
		WIN32_FIND_DATA	findData;				// 290 
		UInt32	version;						// 3D0 plugin version (0.8/1.0)
		UInt32	formCount;						// 3D4 record/form count
		UInt32	nextFormID;						// 3D8 used by TESFile::sub_486BF0 in the editor
		UInt32	flags;							// 3DC
		Node<char>		masterList;				// 3E0 linked list of .esm dependencies
		Node<SizeInfo>	masterSizeInfo;			// 3E8 linked list of file size info for above list
		UInt32	idx;							// 3F0
		void	* unk3F4;						// 3F4
		UInt32	unk3F8;							// 3F8
		UInt32	unk3FC;							// 3FC
		UInt8	unk400;							// 400 init to -1
		UInt8	pad401[3];
		String	authorName;						// 404
		String	modDescription;					// 40C
		UInt32	unk414;							// 414
		UInt32	unk418;							// 418
	};

	Data		* data;
	ModEntry	* next;

	ModEntry * Next() const	{	return next;	}
	Data * Info() const		{	return data;	}
	bool IsLoaded()	const	{	return (data && (data->flags & Data::kFlag_Loaded)) ? true : false;	}
};

STATIC_ASSERT(sizeof(ModEntry::Data) == 0x41C);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

// ### duplication of this class definition is ugly but several offsets changed between 1.1 and 1.2

// CDC
class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	template <tData> struct Node
	{
		tData		* data;
		Node<tData>	* next;
	};

	BoundObjectListHead		* boundObjects;					// 000
	Node<TESPackage>		packages;						// 004
	Node<TESWorldSpace>		worldSpaces;					// 00C
	Node<TESClimate>		climates;						// 014
	Node<TESWeather>		weathers;						// 01C
	Node<EnchantmentItem>	enchantmentItems;				// 024
	Node<SpellItem>			spellitems;						// 02C
	Node<TESHair>			hairs;							// 034
	Node<TESEyes>			eyes;							// 03C
	Node<TESRace>			races;							// 044
	Node<TESLandTexture>	landTextures;					// 04C
	Node<TESClass>			classes;						// 054
	Node<TESFaction>		factions;						// 05C
	Node<Script>			scripts;						// 064
	Node<TESSound>			sounds;							// 06C
	Node<TESGlobal>			globals;						// 074
	Node<TESTopic>			topics;							// 07C
	Node<TESQuest>			quests;							// 084
	Node<Birthsign>			birthsigns;						// 08C
	Node<TESCombatStyle>	combatStyles;					// 094
	Node<TESLoadScreen>		loadScreens;					// 09C
	Node<TESWaterForm>		waterForms;						// 0A4
	Node<TESEffectShader>	effectShaders;					// 0AC
	Node<TESObjectANIO>		objectAnios;					// 0B4
	TESRegionList			* regionList;					// 0BC
	NiTArray <TESObjectCELL *>	cellArray;					// 0C0
	TESSkill				skills[0x15];					// 0D0
	UInt32					unk8B0[(0x8C0 - 0x8B0) >> 2];	// 8B0
	ModEntry				modList;						// 8C0
	UInt32					numLoadedMods;					// 8C8
	ModEntry::Data			* modsByID[0xFF];				// 8CC
	UInt32					unkCC8[(0xCD4 - 0xCC8) >> 2];	// CC8
	TESRegionDataManager	* regionDataManager;			// CD4
	UInt32					unkCD8;							// CD8

	const ModEntry * LookupModByName(const char * modName);
	const ModEntry ** GetActiveModList();		// returns array of modEntry* corresponding to loaded mods sorted by mod index
	UInt8 GetModIndex(const char* modName);
	UInt8 GetActiveModCount();
	const char* GetNthModName(UInt32 modIndex);
	TESGlobal* GetGlobalVarByName(const char* varName, UInt32 nameLen = -1);
	TESQuest* GetQuestByEditorName(const char* questName, UInt32 nameLen = -1);
};

STATIC_ASSERT(sizeof(DataHandler) == 0xCDC);

#else

// CE0 / 1220 (editor, due entirely to difference in size of TESSkill)
class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	template <typename tData> struct Node
	{
		tData		* data;
		Node<tData>	* next;
	};

	struct _Unk8B8											// as seen in the editor
	{
		UInt32				unk00;							// 00
		UInt32				unk04;							// 04
		UInt32				unk08;							// 08 initialized to (numLoadedMods << 24) | 0x800 during plugin load
		ModEntry::Data*		activeFile;						// 0C active plugin
	};

	BoundObjectListHead		* boundObjects;					// 000
	Node<TESPackage>		packages;						// 004
	Node<TESWorldSpace>		worldSpaces;					// 00C
	Node<TESClimate>		climates;						// 014
	Node<TESWeather>		weathers;						// 01C
	Node<EnchantmentItem>	enchantmentItems;				// 024
	Node<SpellItem>			spellitems;						// 02C
	Node<TESHair>			hairs;							// 034
	Node<TESEyes>			eyes;							// 03C
	Node<TESRace>			races;							// 044
	Node<TESLandTexture>	landTextures;					// 04C
	Node<TESClass>			classes;						// 054
	Node<TESFaction>		factions;						// 05C
	Node<Script>			scripts;						// 064
	Node<TESSound>			sounds;							// 06C
	Node<TESGlobal>			globals;						// 074
	Node<TESTopic>			topics;							// 07C
	Node<TESQuest>			quests;							// 084
	Node<BirthSign>			birthsigns;						// 08C
	Node<TESCombatStyle>	combatStyles;					// 094
	Node<TESLoadScreen>		loadScreens;					// 09C
	Node<TESWaterForm>		waterForms;						// 0A4
	Node<TESEffectShader>	effectShaders;					// 0AC
	Node<TESObjectANIO>		objectAnios;					// 0B4
	TESRegionList			* regionList;					// 0BC
	NiTArray <TESObjectCELL *>	cellArray;					// 0C0
	UInt32					unk0D0[2];						// 0D0
	TESSkill				skills[0x15];					// 0D8
	_Unk8B8					unk8B8;							// 8B8
	ModEntry				modList;						// 8C8
	UInt32					numLoadedMods;					// 8D0
	ModEntry::Data			* modsByID[0xFF];				// 8D4
	UInt32					unkCD0[(0xCD8 - 0xCD0) >> 2];	// CD0
	TESRegionDataManager	* regionDataManager;			// CD8
	UInt32					unkCDC;							// CDC

	bool	ConstructObject(ModEntry::Data* tesFile, bool unk1);

	const ModEntry * LookupModByName(const char * modName);
	const ModEntry ** GetActiveModList();		// returns array of modEntry* corresponding to loaded mods sorted by mod index
	UInt8 GetModIndex(const char* modName);
	UInt8 GetActiveModCount();
	const char* GetNthModName(UInt32 modIndex);
	TESGlobal* GetGlobalVarByName(const char* varName, UInt32 nameLen);
	TESQuest* GetQuestByEditorName(const char* questName, UInt32 nameLen = -1);
};

#if OBLIVION
STATIC_ASSERT(sizeof(DataHandler) == 0xCE0);
#else
STATIC_ASSERT(sizeof(DataHandler) == 0x1220);
#endif

#endif

typedef Visitor<ModEntry, ModEntry::Data> ModEntryVisitor;

class ChangesMap;
class InteriorCellNewReferencesMap;
class ExteriorCellNewReferencesMap;
class NumericIDBufferMap;

#if 0			// this is a redefinition of TESSaveLoadGame
// 1C8
class ChangeHandler
{
	ChangesMap						* changesMap000;		// 000
	ChangesMap						* changesMap004;		// 004
	InteriorCellNewReferencesMap	* interiorCellMap;		// 008
	ExteriorCellNewReferencesMap*	exteriorCellMaps[2];	// 00C
	UInt32							unk1[2];				// 014
	void*							unk2;					// 01C
	UInt32							unk3[3];				// 020

	struct RefIDList {
		UInt32	refID;
		RefIDList* next;
	};
	RefIDList						baseFormNewList;		// 02C
	UInt32							unk4[5];				// 034
	UInt32							unk5[2];				// 048
	void*							unk6;					// 050
	UInt32							unk7;					// 054
	NumericIDBufferMap*				bufferMaps[4];			// 058
	UInt32							unk8[4];				// 068
	NiTLargeArray*					largeArrays[2];			// 078
	UInt32							unk9[83];				// 080
};
#endif

class FileFinder
{
public:
	FileFinder();
	~FileFinder();

	virtual void Unk_00(void) = 0;
	virtual UInt32 FindFile(const char* filePath, UInt32 arg1, UInt32 arg2, UInt32 arg3) = 0;	//seen (char*, 0, 0, -1)
	virtual void Unk_02(void) = 0;	
	virtual UInt32 Unk_03(const char* filePath, UInt8 arg1, UInt32 arg2) = 0; //seen (char*,0xC1, 0xFFFF)
	virtual void Unk_04(void) = 0;

	enum {
		kFileStatus_NotFound = 0,
		kFileStatus_Unpacked,
		kFileStatus_Packed
	};	//return values for FindFile(), Unk_03()

	//vtbl				00
	NiTArray<UInt32>	array;	//NiTArray@PBD@@ - Single entry containing filepath "Data\"
};

extern FileFinder** g_FileFinder;

class TimeGlobals
{
	TESGlobal	* gameYear;
	TESGlobal	* gameMonth;
	TESGlobal	* gameDay;
	TESGlobal	* gameHour;
	TESGlobal	* gameDaysPassed;
	TESGlobal	* timeScale;
public:
	static TimeGlobals* Singleton();

	TimeGlobals();
	~TimeGlobals();

	static UInt32 GameYear() ;
	static UInt32 GameMonth() ;
	static UInt32 GameDay() ;
	static float  GameHour() ;
	static UInt32 GameDaysPassed() ;
	static float  TimeScale() ;
	static UInt32 GameHoursPassed() ;
	static UInt32 HoursToRespawnCell() ;
};

class GridDistantArray;
class NiNode;
class BSTempNodeManager;
class NiDirectionalLight;
class BSFogProperty;
class NiSourceTexture;
class Sky;

// 04
class GridArray
{
public:
	GridArray();
	~GridArray();

	virtual void Fn_00(void) = 0;
	virtual void Fn_01(void) = 0;
	virtual void Fn_02(void) = 0;
	virtual void Fn_03(void) = 0;
	virtual void Fn_04(void) = 0;
	virtual void Fn_05(void) = 0;
	virtual void Fn_06(void) = 0;
	virtual void Fn_07(void) = 0;
	virtual void Fn_08(void) = 0;
	virtual void Fn_09(void) = 0;

	// void **		vtbl
	// no data members
};

// 028
class GridCellArray : public GridArray
{
public:
	GridCellArray();
	~GridCellArray();

	// size?
	struct CellInfo
	{
		UInt32		unk00;
		NiNode		* niNode;
		// ...
	};

	// 04
	struct GridEntry
	{
		TESObjectCELL	* cell;
		CellInfo		* info;
	};

	// void **		vtbl
	UInt32			worldX;		// 04 worldspace x coordinate of cell at center of grid (player's cell)
	UInt32			worldY;		// 08 worldspace y
	UInt32			size;		// 0C grid is size^2, size = uGridsToLoad
	GridEntry		* grid;		// 10 dynamically alloc'ed array of GridEntry[size^2]
	float			posX;		// 14 4096*worldX (exterior cells are 4096 square units)
	float			posY;		// 18 4096*worldY
	UInt32			unk1C;		// 1C seen 0
	UInt32			unk20;		// 20 seen 1
	UInt32			unk24;		// 24 seem 0

	GridEntry* GetGridEntry(UInt32 x, UInt32 y);	// x and y range from 0 to (size-1)
};		

// 0AC
class TES
{
public:
	TES();
	~TES();

	// only 1 virtual fn. If worldSpace is null, uses this->worldSpace 
	virtual void Unk_00(UInt32 arg0, UInt32 arg1, UInt32 arg2, UInt32 arg3, TESWorldSpace* worldSpace);

	static TES* GetSingleton();

	// 08
	struct Unk8C
	{
		TESNPC	* npc;
		UInt32	unk4;	// size?
	};

	// void		** vtbl >> oddly, vtbl pointer is NULL in global TES object though c'tor initializes it...
	GridDistantArray	* gridDistantArray;		// 04
	GridCellArray		* gridCellArray;		// 08
	NiNode				* niNode0C;				// 0C
	NiNode				* niNode10;				// 10
	BSTempNodeManager	* tempNodeManager;		// 14
	NiDirectionalLight	* niDirectionalLight;	// 18
	BSFogProperty		* fogProperty;			// 1C
	UInt32				unk20;					// 20 cell grid x coordinate within current worldspace
	UInt32				unk24;					// 24 grid y
	UInt32				unk28;					// 28 same as unk20?
	UInt32				unk2C;					// 2C same as unk24?
	TESObjectCELL		* currentExteriorCell;	// 30
	TESObjectCELL		* currentInteriorCell;	// 34
	void				* unk38;				// 38
	TESObjectCELL		** cellArray;			// 3C ?
	UInt32				unk40;					// 40
	UInt32				unk44;					// 44
	UInt32				unk48;					// 48 seen 0x7FFFFFFF; seen caching unk20 in editor
	UInt32				unk4C;					// 4C seen 0x7FFFFFFF; seen caching unk24 in editor
	UInt32				unk50;					// 50
	void				* unk54;				// 54 some struct; seen { 0, 0, BSRenderedTexture*, ... }
	void				* unk58;				// 58
	Sky					* sky;					// 5C
	UInt32				unk60;					// 60
	UInt32				unk64;					// 64
	UInt32				unk68;					// 68
	float				unk6C;					// 6C
	float				unk70;					// 70
	TESWorldSpace		* currentWorldSpace;	// 74
	UInt32				unk78[5];				// 78
	tList<Unk8C>		list8C;					// 8C
	NiSourceTexture		* bloodDecals[3];		// 94 blood.dds, lichblood.dds, whillothewispblood.dds
	tList<void*>		listA0;					// A0 data is some struct containing NiNode*
	UInt32				unkA8;					// A8

	bool GetTerrainHeight(float* posVec3, float* outHeight);
};

STATIC_ASSERT(sizeof(TES) == 0xAC);

bool GetWaterShaderProperty(const char* propName, float& out);