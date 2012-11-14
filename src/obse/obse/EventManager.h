#pragma once

class Script;
class TESForm;
class TESObjectREFR;

// For dispatching events to scripts.
// Scripts can register an event handler for any of the supported events.
// Can optionally specify filters to match against the event arguments.
// Event handler is a function script which must take the expected number and types of arguments associated with the event.
// Supporting hooks only installed if at least one handler is registered for a particular event.
// ###TODO: at present only supports 0-2 arguments per event. All we need for now, but would be nice to support arbitrary # of args.

namespace EventManager
{
	enum eEventID {
		// correspond to ScriptEventList event masks
		kEventID_OnHit,
		kEventID_OnHitWith,
		kEventID_OnMagicEffectHit,
		kEventID_OnActorEquip,
		kEventID_OnDeath,
		kEventID_OnMurder,
		kEventID_OnKnockout,
		kEventID_OnActorUnequip,
		kEventID_OnAlarm_Trespass,
		kEventID_OnAlarm_Steal,
		kEventID_OnAlarm_Attack,
		kEventID_OnAlarm_Pickpocket,
		kEventID_OnAlarm_Murder,
		kEventID_OnPackageChange,
		kEventID_OnPackageStart,
		kEventID_OnPackageDone,
		kEventID_OnStartCombat,

		kEventID_ScriptEventListMAX,

		// special-cased game events
		kEventID_OnActivate = kEventID_ScriptEventListMAX,
		kEventID_OnVampireFeed,		// player feeds as a vampire
		kEventID_OnSkillUp,			// natural skill increase through use
		kEventID_OnScriptedSkillUp,	// from ModPCSkill/AdvanceSkill cmd
		kEventID_OnMapMarkerAdd,	// map marker becomes visible
		kEventID_OnSpellCast,		// actor casts spell
		kEventID_OnScrollCast,		// actor casts using scroll (same hook as OnSpellCast)
		kEventID_OnFallImpact,		// actor lands from a fall. Does not check fall duration/damage/if creature can fly
		kEventID_OnActorDrop,		// actor drops item from inventory into the game world.
		kEventID_OnDrinkPotion,		// actor drinks a potion, including via EquipItem command
		kEventID_OnEatIngredient,	// actor eats an ingredient
		kEventID_OnNewGame,			// player starts a new game
		kEventID_OnHealthDamage,	// actor takes health damage

		// events related to changes in HighProcess::currentAction
		// these are slightly misnamed
		kEventID_OnMeleeAttack,			// beginning of anim for melee/staff attack, spell cast
		kEventID_OnMeleeRelease,		// release of bow/melee/staff attack, spell cast
		kEventID_OnBowAttack,			// beginning of bow attack anim
		kEventID_OnBowRelease,			// on arrow attach, undocumented (not hugely useful)
		kEventID_OnBlock,
		kEventID_OnRecoil,
		kEventID_OnStagger,
		kEventID_OnDodge,

		kEventID_OnEnchant,
		kEventID_OnCreateSpell,
		kEventID_OnCreatePotion,

		kEventID_GameEventMAX,

		// OBSE internal events, correspond to OBSEMessagingInterface messages
		kEventID_LoadGame = kEventID_GameEventMAX,
		kEventID_SaveGame,
		kEventID_ExitGame,
		kEventID_ExitToMainMenu,
		kEventID_QQQ,
		kEventID_PostLoadGame,

		kEventID_MAX,
	};

	// Represents an event handler registered for an event.
	struct EventCallback
	{
		EventCallback(Script* funcScript, TESObjectREFR* sourceFilter = NULL, TESForm* objectFilter = NULL, TESObjectREFR* thisObj = NULL)
			: script(funcScript), source(sourceFilter), object(objectFilter), callingObj(thisObj), flags(0) { }
		~EventCallback() { }

		
		enum {
			kFlag_Removed		= 1 << 0,		// set when RemoveEventHandler called while handler is in use
			kFlag_InUse			= 1 << 1,		// is in use by HandleEvent (so removing it would invalidate an iterator)
		};

		Script			* script;
		TESObjectREFR	* source;				// first arg to handler
		TESForm			* object;				// second arg to handler
		TESObjectREFR	* callingObj;			// invoking object for handler
		UInt8			flags;

		bool IsInUse() const { return flags & kFlag_InUse ? true : false; }
		bool IsRemoved() const { return flags & kFlag_Removed ? true : false; }
		void SetInUse(bool bSet) { flags = bSet ? flags | kFlag_InUse : flags & ~kFlag_InUse; }
		void SetRemoved(bool bSet) { flags = bSet ? flags | kFlag_Removed : flags & ~kFlag_Removed; }
		bool Equals(const EventCallback& rhs) const;	// compare, return true if the two handlers are identical
	};

	bool SetHandler(const char* id, EventCallback& handler);

	// removes handler only if all filters match
	bool RemoveHandler(const char* id, EventCallback& handler);

	// removes all instances of handler regardless of filters
	bool RemoveHandler(const char* id, Script* fnScript);

	// handle an OBSEMessagingInterface message
	void HandleOBSEMessage(UInt32 msgID, void* data);

	// handle an eventID directly
	void __stdcall HandleEvent(eEventID id, void * arg0, void * arg1);

	// name of whatever event is currently being handled
	const char* GetCurrentEventName();

	// called each frame to update internal state
	void Tick();
};
