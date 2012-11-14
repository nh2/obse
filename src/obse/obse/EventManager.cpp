#include <list>
#include <stdarg.h>
#include "EventManager.h"
#include "ArrayVar.h"
#include "PluginAPI.h"
#include "GameAPI.h"
#include "Utilities.h"
#include "ScriptUtils.h"
#include "obse_common/SafeWrite.h"
#include "FunctionScripts.h"
#include "GameObjects.h"
#include "ThreadLocal.h"
#include "common/ICriticalSection.h"
#include "Hooks_Gameplay.h"

namespace EventManager {

void __stdcall HandleEventForCallingObject(eEventID id, TESObjectREFR* callingObj, void* arg0, void* arg1);

static ICriticalSection s_criticalSection;

//////////////////////
// Event definitions
/////////////////////

// Hook routines need to be forward declared so they can be used in EventInfo structs.
// ###TODO: Would be nice to move hooks out into a separate file
static void  InstallHook();
static void  InstallActivateHook();
static void  InstallOnVampireFeedHook();
static void  InstallOnSkillUpHook();
static void  InstallModPCSHook();
static void  InstallMapMarkerHook();
static void  InstallOnSpellCastHook();
static void  InstallOnFallImpactHook();
static void  InstallOnDrinkPotionHook();
static void  InstallOnEatIngredientHook();
static void	 InstallOnActorEquipHook();
static void  InstallOnHealthDamageHook();
static void  InstallOnMeleeAttackHook();
static void  InstallOnMeleeReleaseHook();
static void  InstallOnBowAttackHook();
static void  InstallOnBowReleaseHook();
static void  InstallOnBlockHook();
static void  InstallOnRecoilHook();
static void  InstallOnStaggerHook();
static void  InstallOnDodgeHook();

enum {
	kEventMask_OnActivate		= 0xFFFFFFFF,		// special case as OnActivate has no event mask
};

typedef void (* EventHookInstaller)();

struct EventInfo
{
	~EventInfo();

	const char					* name;				// must be lowercase
	UInt8						* paramTypes;
	UInt8						numParams;
	bool						isDeferred;		// dispatch event in Tick() instead of immediately - currently unused
	std::list<EventCallback>	* callbacks;
	EventHookInstaller			* installHook;	// if a hook is needed for this event type, this will be non-null. 
												// install it once and then set *installHook to NULL. Allows multiple events
												// to use the same hook, installing it only once.
	
};

// hook installers
static EventHookInstaller s_MainEventHook = InstallHook;
static EventHookInstaller s_ActivateHook = InstallActivateHook;
static EventHookInstaller s_VampireHook = InstallOnVampireFeedHook;
static EventHookInstaller s_SkillUpHook = InstallOnSkillUpHook;
static EventHookInstaller s_ModPCSHook = InstallModPCSHook;
static EventHookInstaller s_MapMarkerHook = InstallMapMarkerHook;
static EventHookInstaller s_SpellScrollHook = InstallOnSpellCastHook;
static EventHookInstaller s_FallImpactHook = InstallOnFallImpactHook;
static EventHookInstaller s_DrinkHook = InstallOnDrinkPotionHook;
static EventHookInstaller s_EatIngredHook = InstallOnEatIngredientHook;
static EventHookInstaller s_ActorEquipHook = InstallOnActorEquipHook;
static EventHookInstaller s_HealthDamageHook = InstallOnHealthDamageHook;
static EventHookInstaller s_MeleeAttackHook = InstallOnMeleeAttackHook;
static EventHookInstaller s_MeleeReleaseHook = InstallOnMeleeReleaseHook;
static EventHookInstaller s_BowAttackHook = InstallOnBowAttackHook;
static EventHookInstaller s_BowReleaseHook = InstallOnBowReleaseHook;
static EventHookInstaller s_BlockHook = InstallOnBlockHook;
static EventHookInstaller s_RecoilHook = InstallOnRecoilHook;
static EventHookInstaller s_StaggerHook = InstallOnStaggerHook;
static EventHookInstaller s_DodgeHook = InstallOnDodgeHook;

// event handler param lists
static UInt8 kEventParams_GameEvent[2] =
{
	Script::eVarType_Ref, Script::eVarType_Ref
};

static UInt8 kEventParams_OneRef[1] =
{
	Script::eVarType_Ref,
};

static UInt8 kEventParams_GameMGEFEvent[2] =
{
	// MGEF gets converted to effect code when passed to scripts
	Script::eVarType_Ref, Script::eVarType_Integer
};

static UInt8 kEventParams_OneString[1] =
{
	Script::eVarType_String
};

static UInt8 kEventParams_OneInteger[1] =
{
	Script::eVarType_Integer
};

static UInt8 kEventParams_TwoIntegers[2] =
{
	Script::eVarType_Integer, Script::eVarType_Integer
};

static UInt8 kEventParams_OneFloat_OneRef[2] =
{
	 Script::eVarType_Float, Script::eVarType_Ref
};

static UInt8 kEventParams_OneRef_OneInt[2] =
{
	Script::eVarType_Ref, Script::eVarType_Integer
};

// EventInfo definitions
#define EVENT_INFO(name, params, hookInstaller, deferred) \
	{ name, kEventParams_ ## params, sizeof(kEventParams_ ## params), deferred, NULL, hookInstaller }

static EventInfo	s_eventInfos[kEventID_MAX] =
{
	EVENT_INFO("onhit", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onhitwith", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onmagiceffecthit", GameMGEFEvent, &s_MainEventHook, false),
	EVENT_INFO("onactorequip", GameEvent, &s_ActorEquipHook, false),
	EVENT_INFO("ondeath", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onmurder", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onknockout", OneRef, &s_MainEventHook, false),
	EVENT_INFO("onactorunequip", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onalarm trespass", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onalarm steal", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onalarm attack", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onalarm pickpocket", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onalarm murder", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onpackagechange", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onpackagestart", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onpackagedone", GameEvent, &s_MainEventHook, false),
	EVENT_INFO("onstartcombat", GameEvent, &s_MainEventHook, false),

	EVENT_INFO("onactivate", GameEvent, &s_ActivateHook, false),
	{ "onvampirefeed", NULL, 0, false, NULL, &s_VampireHook },
	EVENT_INFO("onskillup", OneInteger, &s_SkillUpHook, false),
	EVENT_INFO("onscriptedskillup", TwoIntegers, &s_ModPCSHook, false),
	EVENT_INFO("onmapmarkeradd", OneRef, &s_MapMarkerHook, false),
	EVENT_INFO("onspellcast", GameEvent, &s_SpellScrollHook, false),
	EVENT_INFO("onscrollcast", GameEvent, &s_SpellScrollHook, false),
	EVENT_INFO("onfallimpact", OneRef, &s_FallImpactHook, false),
	EVENT_INFO("onactordrop", GameEvent, NULL, false),
	EVENT_INFO("ondrinkpotion", GameEvent, &s_DrinkHook, false),
	EVENT_INFO("oneatingredient", GameEvent, &s_EatIngredHook, false),
	{ "onnewgame", NULL, 0, false, NULL, NULL },
	EVENT_INFO("onhealthdamage", OneFloat_OneRef, &s_HealthDamageHook, false),

	EVENT_INFO("onattack", OneRef, &s_MeleeAttackHook, false),
	EVENT_INFO("onrelease", OneRef, &s_MeleeReleaseHook, false),
	EVENT_INFO("onbowattack", OneRef, &s_BowAttackHook, false),
	EVENT_INFO("onbowarrowattach", OneRef, &s_BowReleaseHook, false),	// undocumented, not hugely useful
	EVENT_INFO("onblock", OneRef, &s_BlockHook, false),
	EVENT_INFO("onrecoil", OneRef, &s_RecoilHook, false),
	EVENT_INFO("onstagger", OneRef, &s_StaggerHook, false),
	EVENT_INFO("ondodge", OneRef, &s_DodgeHook, false),

	EVENT_INFO("onenchant", OneRef, NULL, false),
	EVENT_INFO("oncreatespell", OneRef, NULL, false),
	EVENT_INFO("oncreatepotion", OneRef_OneInt, NULL, false),

	EVENT_INFO("loadgame", OneString, NULL, false),
	EVENT_INFO("savegame", OneString, NULL, false),
	{ "exitgame", NULL, 0, false, NULL, NULL },
	{ "mainmenu", NULL, 0, false, NULL, NULL },
	{ "qqq", NULL, 0, false, NULL, NULL },
	EVENT_INFO("postloadgame", OneInteger, NULL, false),
};

#undef EVENT_INFO

STATIC_ASSERT(SIZEOF_ARRAY(s_eventInfos, EventInfo) == kEventID_MAX);

///////////////////////////
// internal functions
//////////////////////////

void __stdcall HandleEvent(eEventID id, void * arg0, void * arg1);
void __stdcall HandleGameEvent(UInt32 eventMask, TESObjectREFR* source, TESForm* object);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
static const UInt32 kVtbl_PlayerCharacter = 0x00A73A0C;
static const UInt32 kVtbl_Character = 0x00A6FC9C;
static const UInt32 kVtbl_Creature = 0x00A710F4;
static const UInt32 kVtbl_ArrowProjectile = 0x00A6F08C;
static const UInt32 kVtbl_MagicBallProjectile = 0x00A75944;
static const UInt32 kVtbl_MagicBoltProjectile = 0x00A75BC4;
static const UInt32 kVtbl_MagicFogProjectile = 0x00A75EFC;
static const UInt32 kVtbl_MagicSprayProjectile = 0x00A76594;
static const UInt32 kVtbl_TESObjectREFR = 0x00A46C44;

static const UInt32 kMarkEvent_HookAddr = 0x004FBF90;
static const UInt32 kMarkEvent_RetnAddr = 0x004FBF96;

static const UInt32 kActivate_HookAddr = 0x004DD286;
static const UInt32 kActivate_RetnAddr = 0x004DD28C;

static UInt32 s_PlayerCharacter_SetVampireHasFed_OriginalFn = 0x0066B120;

#else
#error unsupported Oblivion version
#endif


// cheap check to prevent duplicate events being processed in immediate succession
// (e.g. game marks OnHitWith twice per event, this way we only process it once)
static TESObjectREFR* s_lastObj = NULL;
static TESForm* s_lastTarget = NULL;
static UInt32 s_lastEvent = NULL;

// OnHitWith often gets marked twice per event. If weapon enchanted, may see:
//  OnHitWith >> OnMGEFHit >> ... >> OnHitWith. 
// Prevent OnHitWith being reported more than once by recording OnHitWith events processed
static TESObjectREFR* s_lastOnHitWithActor = NULL;
static TESForm* s_lastOnHitWithWeapon = NULL;

// And it turns out OnHit is annoyingly marked several times per frame for spells/enchanted weapons
static TESObjectREFR* s_lastOnHitVictim = NULL;
static TESForm* s_lastOnHitAttacker = NULL;

//////////////////////////////////
// Hooks
/////////////////////////////////

static __declspec(naked) void MarkEventHook(void)
{
	// volatile: ecx, edx, eax

	__asm {
		// grab args
		mov	eax, [esp+8]			// ExtraDataList*
		test eax, eax
		jnz	XDataListIsNotNull

		push ebx
		push esi
		mov esi, 0
		jmp [kMarkEvent_RetnAddr]

	XDataListIsNotNull:
		sub eax, 0x44				// TESObjectREFR* thisObj
		mov ecx, [esp+0xC]			// event mask
		mov edx, [esp+4]			// target

		pushad
		push edx
		push eax
		push ecx
		call HandleGameEvent
		popad

		// overwritten code
		push ebx
		push esi
		mov esi, eax		// thisObj
		add esi, 0x44

		jmp [kMarkEvent_RetnAddr]
	}
}		

void InstallHook()
{
	WriteRelJump(kMarkEvent_HookAddr, (UInt32)&MarkEventHook);
}

#if 0
static __declspec(naked) void OnActorEquipHook(void)
{
	// game fails to mark OnActorEquip event reliably
	// this additional hook hooks ExtraContainerChanges::Data::EquipItemForActor to rectify that
	// overwrites a jz rel32 instruction

#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 jzAddr = 0x00489D09;
	static const UInt32 jnzAddr = 0x00489C74;
	static const UInt32 argsOffset = 0x30;
#else
#error unsupported Oblivion version
#endif

	__asm {
		// figure out where we're returning to based on zero flag
		jz _JZ
		mov ebp, [jnzAddr]
		jmp DoHook

	_JZ:
		mov ebp, [jzAddr]

	DoHook:
		mov eax, esp

		add eax, [argsOffset]
		mov esi, [eax]			// item being equipped
		add eax, 8
		mov eax, [eax]			// actor equipping

		// make sure args are valid
		test eax, eax
		jz Done
		test esi, esi
		jz Done

		// invoke handler
		pushad
		push esi
		push eax
		push kEventID_OnActorEquip
		call HandleEvent
		popad

	Done:
		jmp	ebp
	}
}
#endif

static __declspec(naked) void OnActorEquipHook(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_callAddr = 0x00489C30;	// ExtraContainerChanges::Data::EquipForActor()
#else
#error unsupported Oblivion version
#endif

	static const UInt32 kEventMask = ScriptEventList::kEvent_OnActorEquip;

	__asm {
		pushad
		push edi
		push ebp
		push [kEventMask]
		call HandleGameEvent
		popad

		jmp	[s_callAddr]
	}
}

static void InstallOnActorEquipHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 patchAddr = 0x00489C6E;
#else
#error unsupported Oblivion version
#endif
	if (s_MainEventHook) {
		// OnActorEquip event also (unreliably) passes through main hook, so install that
		s_MainEventHook();
		// since it's installed, prevent it from being installed again
		s_MainEventHook = NULL;
	}

	// additional hook to overcome game's failure to consistently mark this event type

	// below is commented out b/c it reproducibly produces game instability in seemingly unrelated code.
	// WriteRelJump(patchAddr, (UInt32)&OnActorEquipHook);

	// this exhibits same problem
	// The issue is that our Console_Print routine interacts poorly with the game's debug text (turned on with TDT console command)
	// when called from a background thread.
	// So if the handler associated with this event calls Print, PrintC, etc, there is a chance it will crash.
	// ###TODO: fix!
	WriteRelCall(0x005F376D, (UInt32)&OnActorEquipHook);
}

static __declspec(naked) void TESObjectREFR_ActivateHook(void)
{
	__asm {
		pushad
		push edi		// activating refr
		push ecx		// this
		mov eax, kEventMask_OnActivate
		push eax
		call HandleGameEvent
		popad

		// overwritten code
		xor bl, bl
		test edi, edi
		mov esi, ecx
		jmp	[kActivate_RetnAddr]
	}
}

void InstallActivateHook()
{
	WriteRelJump(kActivate_HookAddr, (UInt32)&TESObjectREFR_ActivateHook);
}

void __stdcall OnVampireFeedHook(bool bHasFed)
{
	ASSERT(s_PlayerCharacter_SetVampireHasFed_OriginalFn != 0);

	if (bHasFed) {
		HandleEvent(kEventID_OnVampireFeed, NULL, NULL);
	}

	ThisStdCall(s_PlayerCharacter_SetVampireHasFed_OriginalFn, *g_thePlayer, bHasFed);
}

void InstallOnVampireFeedHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 vtblEntry = 0x00A73C70;
	SafeWrite32(vtblEntry, (UInt32)OnVampireFeedHook);
#else
#error unsupported Oblivion version
#endif
}

static __declspec(naked) void OnSkillUpHook(void)
{
	// on entry: edi = TESSkill*
	// retn addr determined by zero flag (we're overwriting a jnz rel32 instruction)
	// ecx, eax volatile
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_jnzAddr = 0x00668129;		// if zero flag set
	static const UInt32 s_jzAddr = 0x006680A6;		// if zero flag not set
#else
#error unsupported Oblivion version
#endif

	__asm {
		jnz	ZeroFlagSet
		mov	ecx, [s_jzAddr]
		jmp DoHook
	ZeroFlagSet:
		mov ecx, [s_jnzAddr]
	DoHook:
		pushad
		push 0
		mov eax, [edi+0x2C]		// skill->skill actor value code
		push eax
		push kEventID_OnSkillUp
		call HandleEvent		// HandleEvent(kEventID_OnSkillUp, (void*)skill->skill, NULL)
		popad
		jmp ecx
	}
}

void InstallOnSkillUpHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 hookAddr = 0x006680A0;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(hookAddr, (UInt32)&OnSkillUpHook);
}

static __declspec(naked) void ModPCSHook(void)
{
	// on entry: esi = TESSkill*, [esp+0x21C-0x20C] = amount. amount may be zero or negative.
	// hook overwrites a jz instruction following a comparison of amount to zero
	// eax, edx volatile
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 jz_retnAddr = 0x0050D1CA;
	static const UInt32 jnz_retnAddr = 0x0050D0ED;
	static const UInt32 amtStackOffset = 0x10;
#else
#error unsupported Oblivion version
#endif

	__asm {
		mov edx, esp
		jz _JZ
		mov eax, [jnz_retnAddr]
		jmp DoHook
	_JZ:
		mov eax, [jz_retnAddr]
	DoHook:
		push eax				// retn addr
		pushad

		// grab amt, skill
		mov eax, [amtStackOffset]
		mov eax, [eax+edx]
		push eax				// amount
		mov eax, [esi+0x2C]		// TESSkill* skill->skill
		push eax
		push kEventID_OnScriptedSkillUp
		call HandleEvent		// HandleEvent(kEventID_OnScriptedSkillUp, skill->skill, amount)

		popad
		// esp now points to saved retn addr
		retn
	}
}
	
void InstallModPCSHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 hookAddr = 0x0050D0E7;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(hookAddr, (UInt32)&ModPCSHook);
}

static __declspec(naked) void OnMapMarkerAddHook(void)
{
	// on entry, we know the marker is being set as visible
	// ecx: ExtraMapMarker::Data* mapMarkerData
	// Only report event if marker was not *already* visible
	// This can be called from 3 locations in game code, 2 of which we're interested in
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_HUDMainMenuRetnAddr = 0x005A7058;		// from HUDMainMenu when player discovers a new marker
	static const UInt32 s_ShowMapRetnAddr = 0x0050AD95;			// from Cmd_ShowMap_Execute
#else
#error unsupported Oblivion  version
#endif

	__asm {
		// is marker already visible?
		test byte ptr [ecx+0x0C], 1				// flags, bit 1 is "visible"
		jnz Done

		// not visible, mark it
		or byte ptr [ecx+0x0C], 1

		// get the map marker refr based on calling code
		mov eax, [s_HUDMainMenuRetnAddr]
		cmp [esp], eax
		jnz CheckShowMapRetnAddr
		mov eax, [edi+0x4]
		jmp GotRefr

	CheckShowMapRetnAddr:
		mov eax, [s_ShowMapRetnAddr]
		cmp [esp], eax
		jnz Done			// unknown caller, so don't handle the event since we can't get the refr
		mov eax, [esp+0x0C]
		
	GotRefr:
		// we have a mapmarker refr, report the event
		pushad
		push 0
		push eax			// TESObjectREFR* marker
		push kEventID_OnMapMarkerAdd
		call HandleEvent
		popad

	Done:
		retn 0x4;
	}
}

void InstallMapMarkerHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 patchAddr = 0x0042B327;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(patchAddr, (UInt32)&OnMapMarkerAddHook);
}

static void DoSpellCastHook(MagicCaster* caster)
{
	MagicItemForm* magicItemForm = OBLIVION_CAST(caster->GetQueuedMagicItem(), MagicItem, MagicItemForm);
	TESObjectREFR* casterRef = OBLIVION_CAST(caster, MagicCaster, TESObjectREFR);
	if (magicItemForm && casterRef) {
		eEventID eventID = OBLIVION_CAST(magicItemForm, MagicItemForm, EnchantmentItem) ? kEventID_OnScrollCast : kEventID_OnSpellCast;
		HandleEvent(eventID, casterRef, magicItemForm);
	}
}

static __declspec(naked) void OnSpellCastHook(void)
{
	// on entry, we know the spell is valid to cast
	// edi: MagicCaster
	// spell can be obtained from MagicCaster::GetQueuedMagicItem()
	static const UInt32 s_retnAddr = 0x005F3F04;

	__asm {
		pushad
		push edi
		call DoSpellCastHook
		pop edi
		popad
		jmp [s_retnAddr]
	}
}

static void InstallOnSpellCastHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	// overwriting jnz rel32 when MagicCaster->CanCast() returns true
	static const UInt32 s_patchAddr = 0x005F3E71;
#else
#error unsupported Oblivion version
#endif
	
	WriteRelJnz(s_patchAddr, (UInt32)&OnSpellCastHook);
}

static __declspec(naked) void OnFallImpactHook(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_retnAddr = 0x005EFD57;
#else
#error unsupported Oblivion version
#endif

	// on entry: esi=Actor*
	__asm {
		pushad
		push 0
		push esi
		push kEventID_OnFallImpact
		call HandleEvent
		popad

		// overwritten code
		and dword ptr [ecx+0x1F4], 0xFFFFFF7F
		jmp [s_retnAddr]
	}
}

static void InstallOnFallImpactHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_patchAddr = 0x005EFD4D;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(s_patchAddr, (UInt32)&OnFallImpactHook);
}

static __declspec(naked) void OnDrinkPotionHook(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_arg2StackOffset = 0x18;
#else
#error unsupported Oblivion version
#endif

	// hooks bool Actor::UseAlchemyItem(AlchemyItem*, UInt32, bool)
	// is called recursively for player - on second call boolean arg is true
	// boolean arg always true for non-player actor
	// returns true if successfully used potion (false e.g. if max potion count exceeded)

	// on entry:
	//	bl: retn value (bool)
	//	esi: Actor*
	//	edi: AlchemyItem*
	
	__asm {
		// make sure retn value is true
		test bl, bl
		jz Done

		// make sure arg2 is true
		mov eax, esp
		add eax, s_arg2StackOffset
		mov al, byte ptr [eax]
		test al, al
		jz Done

		// invoke the handler
		pushad
		push edi
		push esi
		push kEventID_OnDrinkPotion
		call HandleEvent
		popad

	Done:
		// overwritten code
		mov al, bl
		pop ebx
		pop edi
		pop esi
		retn 0x0C
	}
}

static void InstallOnDrinkPotionHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_hookAddr = 0x005E0968;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(s_hookAddr, (UInt32)&OnDrinkPotionHook);
}

static __declspec(naked) void OnEatIngredientHook(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 arg2StackOffset = 0x00000024;
#else
#error unsupported Oblivion version
#endif

	// on entry:
	//	esi: 'eater' refr Actor*
	//	edi: IngredientItem*
	//	boolean arg2 must be true as this is called recursively for player (once with arg=false, then arg=true)

	__asm {
		// check boolean arg, make sure it's true
		mov eax, esp
		add eax, arg2StackOffset
		mov al, byte ptr [eax]
		test al, al
		jz Done

		// handle event
		pushad
		push edi			// ingredient
		push esi			// actor
		push kEventID_OnEatIngredient
		call HandleEvent
		popad

	Done:
		// overwritten code
		jmp MarkBaseExtraListScriptEvent
	}
}

static void InstallOnEatIngredientHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 s_hookAddr = 0x005E4515;	// overwrite call to MarkScriptEventList(actor, baseExtraList, kEvent_Equip)
#else
#error unsupported Oblivion version
#endif

	WriteRelCall(s_hookAddr, (UInt32)&OnEatIngredientHook);
}

static __declspec(naked) void OnHealthDamageHook(void)
{
	// hooks Actor::OnHealthDamage virtual fn
	// only runs if actor is not already dead. Runs *before* damage has a chance to kill actor, so possible to prevent death
	// overwrites a virtual call to Actor::GetCurAV(health)

	// on entry:
	//	edx: virtual fn addr GetCurAV()
	//	esi: Actor* this (actor taking damage)
	//	arg0: Actor* attacker (may be null)
	//	arg1: float damage (has been modified for game difficulty if applicable)

#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 argsOffset = 0x00000008;
	static const UInt32 retnAddr = 0x006034D1;
#else
#error unsupported Oblivion version
#endif

	__asm {
		mov eax, esp
		add eax, [argsOffset]
		pushad
		push [eax]					// attacker
		add eax, 4
		push [eax]					// damage
		push esi					// this
		push kEventID_OnHealthDamage
		call HandleEventForCallingObject
		popad

		// overwritten code
		push 8						// kActorVal_Health
		mov ecx, esi
		call edx

		jmp [retnAddr];
	}
}

static void InstallOnHealthDamageHook()
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 patchAddr = 0x006034CB;
#else
#error unsupported Oblivion version
#endif

	WriteRelJump(patchAddr, (UInt32)&OnHealthDamageHook);
}

// bitfield, set bit (1 << HighProcess::kAction_XXX) for actions which have event handlers registered
static UInt32 s_registeredActions = 0;

static __declspec(naked) void OnActionChangeHook(void)
{
	// overwrites call to HighProcess::SetCurrentAction(UInt16 action, BSAnimGroupSequence*)
	//	esi: Actor*
	//	eax: one of HighProcess::kAction_XXX
	//	ebp: BSAnimGroupSequence*
	//	edx: virtual fn address
	// volatile: ebp, esi (both popped after call), eax

	__asm {
		// -1 == no action
		cmp eax, 0xFFFFFFFF
		jz Done

		push ecx						// actor->process
		add ecx, 0x1F4					// process->currentAction
		mov cl, byte ptr [ecx]
		cmp cl, al						// is new action same as current action?
		jz NotInterested				// if we're interested, we've already reported it, so ignore.
		
		mov ecx, eax					// action
		mov ebp, 1
		shl ebp, cl						// bit for this action
		test [s_registeredActions], ebp	// are we interested in this action?
		jz NotInterested

		// k, we're interested, so invoke the handler
		pushad

		// this supports a linear subset of HighProcess::kAction_XXX, from kAction_Attack through kAction_Dodge
		// so we can calculate the event ID easily
		sub ecx, 2			// kAction_Attack
		add ecx, kEventID_OnMeleeAttack

		push 0
		push esi
		push ecx
		call HandleEvent
		popad

	NotInterested:
		pop ecx

	Done:
		// overwritten code
		call edx
		pop esi
		pop ebp
		retn 8
	}
}

static void InstallOnActionChangeHook(UInt32 action)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 patchAddr = 0x005F01A5;
#else
#error unsupported Oblivion version
#endif

	ASSERT_STR((action >= HighProcess::kAction_Attack && action <= HighProcess::kAction_Dodge),
		"Invalid action supplied to InstallOnActionChangeHook()");

	// same hook used by multiple events, only install once
	static bool s_installed = false;
	if (!s_installed) {
		WriteRelJump(patchAddr, (UInt32)&OnActionChangeHook);
		s_installed = true;
	}

	// record our interest in this action
	s_registeredActions |= (1 << action);
}

static void InstallOnMeleeAttackHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_Attack);
}

static void InstallOnMeleeReleaseHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_AttackFollowThrough);
}

static void InstallOnBowAttackHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_AttackBow);
}

static void InstallOnBowReleaseHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_AttackBowArrowAttached);
}

static void InstallOnBlockHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_Block);
}

static void InstallOnRecoilHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_Recoil);
}

static void InstallOnStaggerHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_Stagger);
}

static void InstallOnDodgeHook()
{
	InstallOnActionChangeHook(HighProcess::kAction_Dodge);
}

eEventID EventIDForMask(UInt32 eventMask)
{
	switch (eventMask) {
		case kEventMask_OnActivate:
			return kEventID_OnActivate;
		case ScriptEventList::kEvent_OnHit:
			return kEventID_OnHit;
		case ScriptEventList::kEvent_OnHitWith:
			return kEventID_OnHitWith;
		case ScriptEventList::kEvent_OnMagicEffectHit:
			return kEventID_OnMagicEffectHit;
		case ScriptEventList::kEvent_OnActorEquip:
			return kEventID_OnActorEquip;
		case ScriptEventList::kEvent_OnDeath:
			return kEventID_OnDeath;
		case ScriptEventList::kEvent_OnMurder:
			return kEventID_OnMurder;
		case ScriptEventList::kEvent_OnKnockout:
			return kEventID_OnKnockout;
		case ScriptEventList::kEvent_OnActorUnequip:
			return kEventID_OnActorUnequip;
		case ScriptEventList::kEvent_OnAlarm_Trespass:
			return kEventID_OnAlarm_Trespass;
		case ScriptEventList::kEvent_OnAlarm_Steal:
			return kEventID_OnAlarm_Steal;
		case ScriptEventList::kEvent_OnAlarm_Attack:
			return kEventID_OnAlarm_Attack;
		case ScriptEventList::kEvent_OnAlarm_Pickpocket:
			return kEventID_OnAlarm_Pickpocket;
		case ScriptEventList::kEvent_OnAlarm_Murder:
			return kEventID_OnAlarm_Murder;
		case ScriptEventList::kEvent_OnPackageStart:
			return kEventID_OnPackageStart;
		case ScriptEventList::kEvent_OnPackageDone:
			return kEventID_OnPackageDone;
		case ScriptEventList::kEvent_OnPackageChange:
			return kEventID_OnPackageChange;
		case ScriptEventList::kEvent_OnStartCombat:
			return kEventID_OnStartCombat;
		default:
			return kEventID_MAX;
	}
}

eEventID EventIDForMessage(UInt32 msgID)
{
	switch (msgID) {
		case OBSEMessagingInterface::kMessage_LoadGame:
			return kEventID_LoadGame;
		case OBSEMessagingInterface::kMessage_SaveGame:
			return kEventID_SaveGame;
		case OBSEMessagingInterface::kMessage_ExitGame:
			return kEventID_ExitGame;
		case OBSEMessagingInterface::kMessage_ExitGame_Console:
			return kEventID_QQQ;
		case OBSEMessagingInterface::kMessage_ExitToMainMenu:
			return kEventID_ExitToMainMenu;
		case OBSEMessagingInterface::kMessage_PostLoadGame:
			return kEventID_PostLoadGame;
		default:
			return kEventID_MAX;
	}
}

eEventID EventIDForString(const char* eventStr)
{
	std::string name(eventStr);
	MakeLower(name);
	eventStr = name.c_str();
	for (UInt32 i = 0; i < kEventID_MAX; i++) {
		if (!strcmp(eventStr, s_eventInfos[i].name)) {
			return (eEventID)i;
		}
	}

	return kEventID_MAX;
}

bool EventCallback::Equals(const EventCallback& rhs) const
{
	return (script == rhs.script &&
		object == rhs.object &&
		source == rhs.source &&
		callingObj == rhs.callingObj);
}

typedef std::list<EventCallback>	CallbackList;

bool SetHandler(eEventID id, EventCallback& handler)
{
	ScopedLock lock(s_criticalSection);

	if (id < kEventID_MAX) {
		EventInfo* info = &s_eventInfos[id];
		// is hook installed for this event type?
		if (info->installHook) {
			if (*(info->installHook)) {
				// if this hook is used by more than one event type, prevent it being installed a second time
				(*info->installHook)();
				*(info->installHook) = NULL;
			}
			// mark event as having had its hook installed
			info->installHook = NULL;
		}

		if (!info->callbacks) {
			info->callbacks = new CallbackList();
		}
		else {
			// if an existing handler matches this one exactly, don't duplicate it
			for (CallbackList::iterator iter = info->callbacks->begin(); iter != info->callbacks->end(); ++iter) {
				if (iter->Equals(handler)) {
					// may be re-adding a previously removed handler, so clear the Removed flag
					iter->SetRemoved(false);
					return false;
				}
			}
		}

		info->callbacks->push_back(handler);
		return true;
	}
	else {
		return false;
	}
}

bool RemoveHandler(eEventID id, EventCallback& handler);
bool RemoveHandler(eEventID id, Script* fnScript);

class EventHandlerCaller : public InternalFunctionCaller
{
public:
	EventHandlerCaller(Script* script, EventInfo* eventInfo, void* arg0, void* arg1, TESObjectREFR* callingObj = NULL)
		: InternalFunctionCaller(script, callingObj), m_eventInfo(eventInfo)
	{
		UInt8 numArgs = 2;
		if (!arg1)
			numArgs = 1;
		if (!arg0)
			numArgs = 0;

		SetArgs(numArgs, arg0, arg1);
	}

	virtual bool ValidateParam(UserFunctionParam* param, UInt8 paramIndex)
	{
		return param->varType == m_eventInfo->paramTypes[paramIndex];
	}

	virtual bool PopulateArgs(ScriptEventList* eventList, FunctionInfo* info) {
		// make sure we've got the same # of args as expected by event handler
		DynamicParamInfo& dParams = info->ParamInfo();
		if (dParams.NumParams() != m_eventInfo->numParams || dParams.NumParams() > 2) {
			ShowRuntimeError(m_script, "Number of arguments to function script does not match those expected for event");
			return false;
		}

		return InternalFunctionCaller::PopulateArgs(eventList, info);
	}

private:
	EventInfo		* m_eventInfo;
};

// stack of event names pushed when handler invoked, popped when handler returns
// used by GetCurrentEventName
std::stack<const char*> s_eventStack;

// some events are best deferred until Tick() invoked rather than being handled immediately
// this stores info about such an event. Currently unused.
struct DeferredCallback
{
	DeferredCallback(CallbackList::iterator& _iter, TESObjectREFR* _callingObj, void* _arg0, void* _arg1, EventInfo* _eventInfo)
		: iterator(_iter), callingObj(_callingObj), arg0(_arg0), arg1(_arg1), eventInfo(_eventInfo) { }

	CallbackList::iterator	iterator;
	TESObjectREFR			* callingObj;
	void					* arg0;
	void					* arg1;
	EventInfo				* eventInfo;
};

std::list<DeferredCallback> s_deferredCallbacks;

void __stdcall HandleEventForCallingObject(eEventID id, TESObjectREFR* callingObj, void* arg0, void* arg1)
{
	ScopedLock lock(s_criticalSection);

	EventInfo* eventInfo = &s_eventInfos[id];
	if (eventInfo->callbacks) {
		for (CallbackList::iterator iter = eventInfo->callbacks->begin(); iter != eventInfo->callbacks->end(); ) {
			if (iter->IsRemoved()) {
				if (!iter->IsInUse()) {
					iter = eventInfo->callbacks->erase(iter);
				}
				else {
					++iter;
				}

				continue;
			}

			// Check filters
			if (iter->source && !((TESObjectREFR*)arg0 == iter->source)) {
				++iter;
				continue;
			}

			if (iter->callingObj && !(callingObj == iter->callingObj)) {
				++iter;
				continue;
			}

			if (iter->object) {
				if (id == kEventID_OnMagicEffectHit) {
					EffectSetting* setting = OBLIVION_CAST(iter->object, TESForm, EffectSetting);
					if (setting && setting->effectCode != (UInt32)arg1) {
						++iter;
						continue;
					}
				}
				else if (!(iter->object == (TESForm*)arg1)) {
					++iter;
					continue;
				}
			}

			if (eventInfo->isDeferred || GetCurrentThreadId() != g_mainThreadID) {
				// avoid potential issues with invoking handlers outside of main thread by deferring event handling
				if (!iter->IsRemoved()) {
					iter->SetInUse(true);
					s_deferredCallbacks.push_back(DeferredCallback(iter, callingObj, arg0, arg1, eventInfo));
					++iter;
				}
			}
			else {
				// handle immediately
				bool bWasInUse = iter->IsInUse();
				iter->SetInUse(true);
				s_eventStack.push(eventInfo->name);
				ScriptToken* result = UserFunctionManager::Call(EventHandlerCaller(iter->script, eventInfo, arg0, arg1, callingObj));
				s_eventStack.pop();
				iter->SetInUse(bWasInUse);

				// it's possible the handler decided to remove itself, so take care of that, being careful
				// not to remove a callback that is needed for deferred invocation
				if (!bWasInUse && iter->IsRemoved()) {
					iter = eventInfo->callbacks->erase(iter);
				}
				else {
					++iter;
				}

				// result is unused
				delete result;
			}
		}
	}
}

void __stdcall HandleEvent(eEventID id, void * arg0, void * arg1)
{
	// initial implementation didn't support a calling object; pass through to impl which does
	HandleEventForCallingObject(id, NULL, arg0, arg1);
}

////////////////
// public API
///////////////

const char* GetCurrentEventName()
{
	ScopedLock lock(s_criticalSection);

	return s_eventStack.size() ? s_eventStack.top() : NULL;
}

bool SetHandler(const char* id, EventCallback& handler)
{
	return SetHandler(EventIDForString(id), handler);
}

bool RemoveHandler(const char* id, EventCallback& handler)
{
	ScopedLock lock(s_criticalSection);

	eEventID eventType = EventIDForString(id);
	bool bRemovedAtLeastOne = false;
	if (eventType < kEventID_MAX && s_eventInfos[eventType].callbacks) {
		CallbackList* callbacks = s_eventInfos[eventType].callbacks;
		for (CallbackList::iterator iter = callbacks->begin(); iter != callbacks->end(); ) {
			if (iter->script == handler.script) {
				bool bMatches = true;
				if (handler.object && handler.object != iter->object) {
					bMatches = false;
				}
				else if (handler.source && handler.source != iter->source) {
					bMatches = false;
				}

				if (bMatches) {
					if (iter->IsInUse()) {
						// this handler is currently active, flag it for later removal
						iter->SetRemoved(true);
						++iter;
					}
					else {
						iter = callbacks->erase(iter);
					}

					bRemovedAtLeastOne = true;
				}
				else {
					++iter;
				}
			}
			else {
				++iter;
			}
		}
	}
	
	return bRemovedAtLeastOne;
}

bool TryGetReference(TESObjectREFR* refr)
{
	// ### HACK HACK HACK
	// MarkEventList() may have been called for a BaseExtraList not associated with a TESObjectREFR
	bool bIsRefr = false;
	__try 
	{
		switch (*((UInt32*)refr)) {
			case kVtbl_PlayerCharacter:
			case kVtbl_Character:
			case kVtbl_Creature:
			case kVtbl_ArrowProjectile:
			case kVtbl_MagicBallProjectile:
			case kVtbl_MagicBoltProjectile:
			case kVtbl_MagicFogProjectile:
			case kVtbl_MagicSprayProjectile:
			case kVtbl_TESObjectREFR:
				bIsRefr = true;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{
		bIsRefr = false;
	}

	return bIsRefr;
}

void __stdcall HandleGameEvent(UInt32 eventMask, TESObjectREFR* source, TESForm* object)
{
	if (!TryGetReference(source)) {
		return;
	}

	ScopedLock lock(s_criticalSection);

	// ScriptEventList can be marked more than once per event, cheap check to prevent sending duplicate events to scripts
	if (source != s_lastObj || object != s_lastTarget || eventMask != s_lastEvent) {
		s_lastObj = source;
		s_lastEvent = eventMask;
		s_lastTarget = object;
	}
	else {
		// duplicate event, ignore it
		return;
	}

	eEventID eventID = EventIDForMask(eventMask);
	if (eventID < kEventID_MAX) {
		// special-case OnMagicEffectHit
		if (eventID == kEventID_OnMagicEffectHit) {
			EffectSetting* setting = OBLIVION_CAST(object, TESForm, EffectSetting);
			HandleEvent(eventID, source, setting ? (void*)setting->effectCode : 0);
		}
		else if (eventID == kEventID_OnHitWith) {
			// special check for OnHitWith, since it gets called redundantly
			if (source != s_lastOnHitWithActor || object != s_lastOnHitWithWeapon) {
				s_lastOnHitWithActor = source;
				s_lastOnHitWithWeapon = object;
				HandleEvent(eventID, source, object);
			}
		}
		else if (eventID == kEventID_OnHit) {
			if (source != s_lastOnHitVictim || object != s_lastOnHitAttacker) {
				s_lastOnHitVictim = source;
				s_lastOnHitAttacker = object;
				HandleEvent(eventID, source, object);
			}
		}
		else
			HandleEvent(eventID, source, object);
	}
}

void HandleOBSEMessage(UInt32 msgID, void* data)
{
	eEventID eventID = EventIDForMessage(msgID);
	if (eventID < kEventID_MAX)
		HandleEvent(eventID, data, NULL);
}

EventInfo::~EventInfo()
{
	if (callbacks) {
		delete callbacks;
		callbacks = NULL;
	}
}

void Tick()
{
	ScopedLock lock(s_criticalSection);

	// handle deferred events
	if (s_deferredCallbacks.size()) {
		std::list< std::list<DeferredCallback>::iterator > s_removedCallbacks;

		std::list<DeferredCallback>::iterator iter = s_deferredCallbacks.begin();
		while (iter != s_deferredCallbacks.end()) {
			if (!iter->iterator->IsRemoved()) {
				s_eventStack.push(iter->eventInfo->name);
				ScriptToken* result = UserFunctionManager::Call(
					EventHandlerCaller(iter->iterator->script, iter->eventInfo, iter->arg0, iter->arg1, iter->callingObj));
				s_eventStack.pop();

				if (iter->iterator->IsRemoved()) {
					s_removedCallbacks.push_back(iter);
					++iter;
				}
				else {
					iter = s_deferredCallbacks.erase(iter);
				}

				// result is unused
				delete result;
			}
		}

		// get rid of any handlers removed during processing above
		while (s_removedCallbacks.size()) {
			(*s_removedCallbacks.begin())->eventInfo->callbacks->erase(iter->iterator);
			s_removedCallbacks.pop_front();
		}

		s_deferredCallbacks.clear();
	}

	s_lastObj = NULL;
	s_lastTarget = NULL;
	s_lastEvent = NULL;
	s_lastOnHitWithActor = NULL;
	s_lastOnHitWithWeapon = NULL;
	s_lastOnHitVictim = NULL;
	s_lastOnHitAttacker = NULL;
}

};	// namespace

