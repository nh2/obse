#include "GameActorValues.h"
#include "Utilities.h"

float ActorValues::GetAV(UInt32 avCode)
{
	float result = 0;
	ThisStdCall(0x0065CB80, this, avCode);
	__asm { fstp [result] }
	return result;
}

void ActorValues::ModAV(UInt32 avCode, float modBy, bool bAllowPositive)
{
	ThisStdCall(0x0065CA60, this, avCode, modBy, bAllowPositive);
}

