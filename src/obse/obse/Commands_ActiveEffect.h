#pragma once

#include "CommandTable.h"

extern CommandInfo kCommandInfo_GetActiveEffectCount;
extern CommandInfo kCommandInfo_GetNthActiveEffectCode;
extern CommandInfo kCommandInfo_GetNthActiveEffectMagnitude;
extern CommandInfo kCommandInfo_GetNthActiveEffectDuration;
extern CommandInfo kCommandInfo_GetNthActiveEffectTimeElapsed;
extern CommandInfo kCommandInfo_GetNthActiveEffectMagicItem;
extern CommandInfo kCommandInfo_GetNthActiveEffectCaster;
extern CommandInfo kCommandInfo_GetNthActiveEffectData;
extern CommandInfo kCommandInfo_GetNthActiveEffectMagicItemIndex;

extern CommandInfo kCommandInfo_SetNthActiveEffectMagnitude;
extern CommandInfo kCommandInfo_ModNthActiveEffectMagnitude;

extern CommandInfo kCommandInfo_GetTotalActiveEffectMagnitude;
extern CommandInfo kCommandInfo_GetTotalAENonAbilityMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEAbilityMagnitude;
extern CommandInfo kCommandInfo_GetTotalAESpellMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEDiseaseMagnitude;
extern CommandInfo kCommandInfo_GetTotalAELesserPowerMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEPowerMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEAllSpellsMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEEnchantmentMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEAlchemyMagnitude;
extern CommandInfo kCommandInfo_GetTotalAEIngredientMagnitude;

extern CommandInfo kCommandInfo_GetTotalActiveEffectMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAENonAbilityMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEAbilityMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAESpellMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEDiseaseMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAELesserPowerMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEPowerMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEAllSpellsMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEEnchantmentMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEAlchemyMagnitudeC;
extern CommandInfo kCommandInfo_GetTotalAEIngredientMagnitudeC;

extern CommandInfo kCommandInfo_GetScriptActiveEffectIndex;

extern CommandInfo kCommandInfo_DispelNthActiveEffect;

extern CommandInfo kCommandInfo_GetActiveEffectCodes;
extern CommandInfo kCommandInfo_GetActiveEffectCasters;
extern CommandInfo kCommandInfo_GetTelekinesisRef;

extern CommandInfo kCommandInfo_DumpAE;	// debug
extern CommandInfo kCommandInfo_GetNthActiveEffectEnchantObject;
extern CommandInfo kCommandInfo_GetNthActiveEffectSummonRef;
extern CommandInfo kCommandInfo_GetNthActiveEffectBoundItem;

extern CommandInfo kCommandInfo_IsNthActiveEffectApplied;

extern CommandInfo kCommandInfo_GetNthActiveEffectActorValue;