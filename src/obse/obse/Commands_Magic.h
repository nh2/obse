#pragma once

#include "CommandTable.h"

extern CommandInfo kCommandInfo_GetMagicItemValue;
extern CommandInfo kCommandInfo_GetMagicItemType;
extern CommandInfo kCommandInfo_GetSpellType;
extern CommandInfo kCommandInfo_GetSpellMagickaCost;
extern CommandInfo kCommandInfo_GetSpellMasteryLevel;
extern CommandInfo kCommandInfo_GetSpellSchool;
extern CommandInfo kCommandInfo_GetSpellExplodesWithNoTarget;
extern CommandInfo kCommandInfo_SetSpellExplodesWithNoTarget;
extern CommandInfo kCommandInfo_GetEnchantmentType;
extern CommandInfo kCommandInfo_GetEnchantmentCharge;
extern CommandInfo kCommandInfo_GetEnchantmentCost;
extern CommandInfo kCommandInfo_SetSpellType;
extern CommandInfo kCommandInfo_SetSpellMagickaCost;
extern CommandInfo kCommandInfo_ModSpellMagickaCost;
extern CommandInfo kCommandInfo_SetSpellMasteryLevel;
extern CommandInfo kCommandInfo_SetEnchantmentType;
extern CommandInfo kCommandInfo_SetEnchantmentCharge;
extern CommandInfo kCommandInfo_ModEnchantmentCharge;
extern CommandInfo kCommandInfo_SetEnchantmentCost;
extern CommandInfo kCommandInfo_ModEnchantmentCost;
extern CommandInfo kCommandInfo_IsMagicItemAutoCalc;
extern CommandInfo kCommandInfo_SetMagicItemAutoCalc;

// effect item list functions
extern CommandInfo kCommandInfo_MagicItemHasEffect;
extern CommandInfo kCommandInfo_MagicItemHasEffectCode;
extern CommandInfo kCommandInfo_MagicItemHasEffectCount;
extern CommandInfo kCommandInfo_MagicItemHasEffectCountCode;
extern CommandInfo kCommandInfo_MagicItemHasEffectItemScript;
extern CommandInfo kCommandInfo_GetMagicItemEffectCount;
extern CommandInfo kCommandInfo_GetNthEffectItemCode;
extern CommandInfo kCommandInfo_GetNthEffectItemMagnitude;
extern CommandInfo kCommandInfo_GetNthEffectItemArea;
extern CommandInfo kCommandInfo_GetNthEffectItemDuration;
extern CommandInfo kCommandInfo_GetNthEffectItemRange;
extern CommandInfo kCommandInfo_GetNthEffectItemActorValue;
extern CommandInfo kCommandInfo_SetNthEffectItemMagnitude;
extern CommandInfo kCommandInfo_ModNthEffectItemMagnitude;
extern CommandInfo kCommandInfo_SetNthEffectItemArea;
extern CommandInfo kCommandInfo_ModNthEffectItemArea;
extern CommandInfo kCommandInfo_SetNthEffectItemDuration;
extern CommandInfo kCommandInfo_ModNthEffectItemDuration;
extern CommandInfo kCommandInfo_SetNthEffectItemRange;
extern CommandInfo kCommandInfo_SetNthEffectItemActorValue;
extern CommandInfo kCommandInfo_SetNthEffectItemActorValueC;
extern CommandInfo kCommandInfo_RemoveNthEffectItem;
extern CommandInfo kCommandInfo_CopyNthEffectItem;
extern CommandInfo kCommandInfo_CopyAllEffectItems;
extern CommandInfo kCommandInfo_AddEffectItem;
extern CommandInfo kCommandInfo_AddEffectItemC;
extern CommandInfo kCommandInfo_AddFullEffectItem;
extern CommandInfo kCommandInfo_AddFullEffectItemC;
extern CommandInfo kCommandInfo_RemoveAllEffectItems;
extern CommandInfo kCommandInfo_AddScriptedEffectItem;
extern CommandInfo kCommandInfo_IsNthEffectItemScripted;
extern CommandInfo kCommandInfo_GetNthEffectItemScript;
extern CommandInfo kCommandInfo_SetNthEffectItemScript;
extern CommandInfo kCommandInfo_GetNthEffectItemScriptVisualEffect;
extern CommandInfo kCommandInfo_SetNthEffectItemScriptVisualEffect;
extern CommandInfo kCommandInfo_SetNthEffectItemScriptVisualEffectC;
extern CommandInfo kCommandInfo_GetNthEffectItemScriptSchool;
extern CommandInfo kCommandInfo_SetNthEffectItemScriptSchool;
extern CommandInfo kCommandInfo_IsNthEffectItemScriptHostile;
extern CommandInfo kCommandInfo_SetNthEffectItemScriptHostile;
extern CommandInfo kCommandInfo_SetNthEffectItemScriptName;
extern CommandInfo kCommandInfo_ModNthEffectItemScriptName;

extern CommandInfo kCommandInfo_IsSpellHostile;
extern CommandInfo kCommandInfo_SetSpellHostile;

extern CommandInfo kCommandInfo_GetNthEffectItemName;
extern CommandInfo kCommandInfo_GetSpells;

extern CommandInfo kCommandInfo_GetBirthsignSpells;
extern CommandInfo kCommandInfo_RemoveBaseSpell;
extern CommandInfo kCommandInfo_GetLeveledSpells;

extern CommandInfo kCommandInfo_GetSpellPCStart;
extern CommandInfo kCommandInfo_GetSpellImmuneToSilence;
extern CommandInfo kCommandInfo_GetSpellAreaEffectIgnoresLOS;
extern CommandInfo kCommandInfo_GetSpellScriptEffectAlwaysApplies;
extern CommandInfo kCommandInfo_GetSpellDisallowAbsorbReflect;

extern CommandInfo kCommandInfo_SetSpellPCStart;
extern CommandInfo kCommandInfo_SetSpellImmuneToSilence;
extern CommandInfo kCommandInfo_SetSpellAreaEffectIgnoresLOS;
extern CommandInfo kCommandInfo_SetSpellScriptEffectAlwaysApplies;
extern CommandInfo kCommandInfo_SetSpellDisallowAbsorbReflect;

extern CommandInfo kCommandInfo_GetNthEffectItem;