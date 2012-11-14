#include "Commands_InventoryRef.h"
#include "ParamInfos.h"

#if OBLIVION

#include "GameObjects.h"
#include "GameExtraData.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "InventoryReference.h"
#include "ArrayVar.h"
#include "Script.h"

#if OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
static const _Cmd_Execute Cmd_RemoveMe_Execute = (_Cmd_Execute)0x00500450;
#else
#error unsupported oblivion version
#endif

static bool Cmd_RemoveMeIR_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj) {
		InventoryReference* iref = InventoryReference::GetForRefID(thisObj->refID);
		if (iref) {
			TESObjectREFR* dest = NULL;
			if (ExtractArgs(PASS_EXTRACT_ARGS, &dest)) {
				if (dest) {
					*result = iref->MoveToContainer(dest) ? 1.0 : 0.0;
				}
				else {
					*result = iref->RemoveFromContainer() ? 1.0 : 0.0;
				}
			}
		}
	}

	return true;
}

static bool Cmd_CopyIR_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj) {
		InventoryReference* iref = InventoryReference::GetForRefID(thisObj->refID);
		if (iref) {
			TESObjectREFR* dest = NULL;
			if (ExtractArgs(PASS_EXTRACT_ARGS, &dest) && dest) {
				*result = iref->CopyToContainer(dest) ? 1.0 : 0.0;
			}
		}
	}

	return true;
}

static bool Cmd_CreateTempRef_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	TESForm* form = NULL;
	if (ExtractArgs(PASS_EXTRACT_ARGS, &form) && form) {
		InventoryReference* iref = InventoryReference::Create(NULL, InventoryReference::Data(form, NULL, NULL), false);
		if (iref) {
			*refResult = iref->GetRef()->refID;
		}
	}

	return true;
}

static bool Cmd_GetInvRefsForItem_Execute(COMMAND_ARGS)
{
	// returns an array of inventory references for the specified base object in the calling object's inventory
	TESForm* item = NULL;
	ArrayID arrID = g_ArrayMap.Create(kDataType_Numeric, true, scriptObj->GetModIndex());
	*result = arrID;

	if (thisObj && ExtractArgs(PASS_EXTRACT_ARGS, &item) && item) {
		double arrIndex = 0.0;

		// get count for base container
		TESContainer* cont = OBLIVION_CAST(thisObj->baseForm, TESForm, TESContainer);
		if (cont) {
			TESContainer::Data* data = cont->DataByType(item);
			SInt32 baseCount = 0;
			if (data) {
				baseCount = (data->count > 0) ? data->count : 0;
			}

			// get container changes for refr
			ExtraContainerChanges* xChanges = (ExtraContainerChanges*)thisObj->baseExtraList.GetByType(kExtraData_ContainerChanges);
			if (xChanges && xChanges->data) {
				// locate entry for this item type
				ExtraContainerChanges::Entry* entry = xChanges->data->objList;
				for ( ; entry; entry = entry->next) {
					if (entry->data && entry->data->type == item) {
						break;
					}
				}

				// create temp refs for each stack
				if (entry && entry->data) {
					ExtraContainerChanges::EntryData* ed = entry->data;
					baseCount += ed->countDelta;
					if (baseCount) {
						for (ExtraContainerChanges::EntryExtendData* extend = ed->extendData; extend; extend = extend->next) {
							if (!extend->data) {
								extend->data = ExtraDataList::Create();
							}
							InventoryReference::Data data(item, entry, extend);
							InventoryReference* iref = InventoryReference::Create(thisObj, data, false);
							g_ArrayMap.SetElementFormID(arrID, arrIndex, iref->GetRef()->refID);
							arrIndex += 1.0;
							baseCount -= GetCountForExtraDataList(extend->data);
						}
					}
				}
			}

			if (baseCount > 0) {
				// create temp ref for items in base container not accounted for by container changes
				xChanges = ExtraContainerChanges::GetForRef(thisObj);
				if (!xChanges->data) {
					xChanges->data = ExtraContainerChanges::Data::Create(thisObj);
				}

				if (!xChanges->data->objList) {
					xChanges->data->objList = ExtraContainerChanges::Entry::Create();
				}

				ExtraContainerChanges::Entry* entry = xChanges->data->objList;
				ExtraContainerChanges::Entry* prev = NULL;
				for ( ; entry; entry = entry->next) {
					if (entry->data && entry->data->type == item) {
						break;
					}
					prev = entry;
				}

				if (!entry) {
					prev->next = ExtraContainerChanges::Entry::Create();
					entry = prev->next;
				}

				if (!entry->data) {
					entry->data = ExtraContainerChanges::EntryData::Create(baseCount, item);
				}

				std::vector<InventoryReference::Data> refData;
				InventoryReference::Data::CreateForUnextendedEntry(entry, baseCount, refData);
				for (std::vector<InventoryReference::Data>::iterator iter = refData.begin(); iter != refData.end(); ++iter) {
					InventoryReference* iref = InventoryReference::Create(thisObj, *iter, false);
					g_ArrayMap.SetElementFormID(arrID, arrIndex, iref->GetRef()->refID);
					arrIndex += 1.0;
				}
			}
		}
	}

	return true;
}

#endif

static ParamInfo kParams_OneOptionalContainerRef[1] =
{
	{ "containerRef",	kParamType_Container,	1	},
};

static ParamInfo kParams_OneContainerRef[1] =
{
	{	"containerRef",	kParamType_Container,	1	},
};

DEFINE_COMMAND(RemoveMeIR, "removes an inventory reference from its container, optionally transferring it to a different container, in much the same way as the vanilla RemoveMe command. The inventory reference becomes invalid once this command is called and should no longer be used.",
			   1, 1, kParams_OneOptionalContainerRef);
DEFINE_COMMAND(CopyIR, "copies an inventory reference to the specified container. The calling object needn't be in a container and remains valid after the command is called. If the calling object is equipped, the copy will not be equipped.",
			   1, 1, kParams_OneContainerRef);
DEFINE_COMMAND(CreateTempRef, "creates a temporary reference to the specified form. This reference does not exist in the gameworld or in a container, and remains valid for only one frame. It is mostly useful for creating a stack of one or more items to be added to a container with CopyIR",
			   0, 1, kParams_OneInventoryObject);
DEFINE_COMMAND(GetInvRefsForItem, returns an array of temp refs to objects of the specified type in the calling container,
			   1, 1, kParams_OneInventoryObject);