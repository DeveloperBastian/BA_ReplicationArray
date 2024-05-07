// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "FFAStructs/FBA_FFA_ObjectArray.h"
#include "FFAStructs/FBA_FFA_Object.h"
#include "BA_RepArray.h"
#include "BA_Statics.h"
#include "Logging/StructuredLog.h"
#include "Engine/ActorChannel.h"

FBA_FFA_ObjectArray::FBA_FFA_ObjectArray()
{

}

FBA_FFA_ObjectArray::~FBA_FFA_ObjectArray()
{
	if (Owner)
	{
		//for (FBA_FFA_Object Item : Items)
		//{
		//	if (IsValid(Item.ObjectPtr) && Owner->IsReplicatedSubObjectRegistered(Item.ObjectPtr))
		//	{
		//		Owner->RemoveReplicatedSubObject(Item.ObjectPtr);
		//	}
		//}
	}
}

#pragma region CRUD

bool FBA_FFA_ObjectArray::AddEntry(UObject* StorageObject, FGuid InstanceGuid, FString ReadableIdentifier)
{
	bool bReturn = false;

	if (!StorageObject)
	{
		return bReturn;
	}
	FString Serialized = BA_Statics::SerializeObject(StorageObject);
	FBA_FFA_Object Entry = FBA_FFA_Object(InstanceGuid, Serialized, StorageObject->GetClass());
	// ToDo: map of readable names and check uniqueness!
	if (!ReadableIdentifier.IsEmpty())
	{
		Entry.InstanceIdentifier = ReadableIdentifier;
	}
	
	if (int32 Position = Items.Add(MoveTemp(Entry));
		Position != INDEX_NONE)
	{
		// save position in map for easier access
		Items[Position].SortIndex = Position;
		//Entry.SortIndex = Position;
		GuidToArrayPos.Add(Items[Position].InstanceGuid, Position);
		IdentifierToArrayPos.Add(Items[Position].InstanceIdentifier, Position);
		// register with owner
		/*if (CheckForSubobjectListSupport(Items[Position]))
		{
			Owner->AddReplicatedSubObject(Items[Position].ObjectPtr);
		}*/
		// update item
		MarkItemDirty(Items[Position]);
		bReturn = true;
		UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Entry '{entry}' newly added to array at position {position}"
			, __FUNCTION__, Items[Position].ToString(), FString::FromInt(Position));
	}
	else
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Error adding Entry '{entry}' to array"
			, __FUNCTION__, Entry.ToString());
	}

	return bReturn;
}

void FBA_FFA_ObjectArray::Clear()
{
	//for (FBA_FFA_Object Item : Items)
	//{
	//	if (CheckForSubobjectListSupport(Item))
	//	{
	//		Owner->RemoveReplicatedSubObject(Item.ObjectPtr);
	//	}
	//}
	Items.Empty();
	GuidToArrayPos.Empty();
	IdentifierToArrayPos.Empty();

	MarkArrayDirty();
	UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: FFA Array cleared - Items count = {items}, guid count = {guid}"
		, __FUNCTION__, FString::FromInt(Items.Num()), FString::FromInt(GuidToArrayPos.Num()));
}

bool FBA_FFA_ObjectArray::RemoveEntry(FGuid InstanceGuid, UObject*& DeletedEntry)
{
	if (!InstanceGuid.IsValid())
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Guid '{guid}' is not valid"
			, __FUNCTION__, InstanceGuid.ToString());
		return false;
	}

	if (int32* PositionPtr = GuidToArrayPos.Find(InstanceGuid);
		PositionPtr
		&& *PositionPtr != INDEX_NONE
		&& Items.IsValidIndex(*PositionPtr))
	{
		// send a copy of the deleted back
		DeletedEntry = BA_Statics::DeserializeObjectFromString(Items[*PositionPtr].SerializedObject, Owner, Items[*PositionPtr].ClassToCastTo);
		// remove from subobject list 
		//if (CheckForSubobjectListSupport(Items[*PositionPtr]))
		//{
		//	Owner->RemoveReplicatedSubObject(Items[*PositionPtr].ObjectPtr);
		//}
		// generate log string before removing anything
		FString LogString = "Entry '" + InstanceGuid.ToString() + "' was swapped with '" 
			+ Items[Items.Num() - 1].ToString() + "' and removed from position " 
			+ FString::FromInt(*PositionPtr);
		// remove from Identifier map 
		IdentifierToArrayPos.Remove(Items[*PositionPtr].InstanceIdentifier);
		// push last to position to be removed
		// not updating local var 'Index', so sorting to original order still possible
		Items.Swap(*PositionPtr, Items.Num() - 1);
		// updated guid map for swapped 
		GuidToArrayPos.Add(Items[*PositionPtr].InstanceGuid, *PositionPtr);
		IdentifierToArrayPos.Add(Items[*PositionPtr].InstanceIdentifier, *PositionPtr);
		// remove from guid map
		GuidToArrayPos.Remove(InstanceGuid);
		
		// mark item dirty
		MarkItemDirty(Items[Items.Num() - 1]);
		MarkItemDirty(Items[*PositionPtr]);
		// shrink array
		Items.SetNum(Items.Num() - 1);

		UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: {logstring}"
			, __FUNCTION__, LogString);
		return true;
	}
	else
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Guid '{guid}' cannot be found in GuidToArrayPos"
			, __FUNCTION__, InstanceGuid.ToString());
	}
	return false;
}

#pragma endregion

#pragma region Networking

// called first before add or remove
void FBA_FFA_ObjectArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		if (!Items.IsValidIndex(Index)) { continue; }

		FBA_FFA_Object& Entry = Items[Index];
		OnEntryPreReplicatedRemove.ExecuteIfBound(Entry);
	}
}

// called second after add or remove
void FBA_FFA_ObjectArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		if (!Items.IsValidIndex(Index)) { continue; }

		FBA_FFA_Object& Entry = Items[Index];
		if (!Entry.SerializedObject.IsEmpty())
		{
			UObject* StorageObject = BA_Statics::DeserializeObjectFromString(Entry.SerializedObject, Owner, Entry.ClassToCastTo);
			// ToDo: Update Add/Remove Maps
		}
		// check index map storage
		if (int32* Position = GuidToArrayPos.Find(Entry.InstanceGuid);
			Position && *Position == INDEX_NONE)
		{
			GuidToArrayPos.Add(Entry.InstanceGuid, Entry.SortIndex);
			IdentifierToArrayPos.Add(Entry.InstanceIdentifier, Entry.SortIndex);
		}
		OnEntryPostReplicatedAdd.ExecuteIfBound(Entry);
	}
}

// called third after add or remove
void FBA_FFA_ObjectArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		if (!Items.IsValidIndex(Index)) { continue; }

		FBA_FFA_Object& Entry = Items[Index];
		// check index map storage
		if (int32* Position = GuidToArrayPos.Find(Entry.InstanceGuid);
			Position && *Position == INDEX_NONE)
		{
			GuidToArrayPos.Add(Entry.InstanceGuid, Entry.SortIndex);
			IdentifierToArrayPos.Add(Entry.InstanceIdentifier, Entry.SortIndex);
		}
		OnEntryPostReplicatedChange.ExecuteIfBound(Entry);
	}
}

// called fourth after add or remove
void FBA_FFA_ObjectArray::PostReplicatedReceive(const FBA_FFA_ObjectArray::FPostReplicatedReceiveParameters& Parameters)
{
	OnEntryPostReplicatedReceive.ExecuteIfBound(Parameters.OldArraySize);
}

bool FBA_FFA_ObjectArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FBA_FFA_Object, FBA_FFA_ObjectArray>(Items, DeltaParms, *this);
}

bool FBA_FFA_ObjectArray::ReplicateFFAObjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = false;

	/*for (FBA_FFA_Object& Entry : Items)
	{
		if (IsValid(Entry.ObjectPtr))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.ObjectPtr, *Bunch, *RepFlags);
		}
	}*/
	return bWroteSomething;
}

#pragma endregion

// ToDo: Decide where to put search logic
bool FBA_FFA_ObjectArray::GetEntryByGuid(FGuid Guid, FBA_FFA_Object& ResultEntry)
{
	if (!Guid.IsValid())
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Guid '{guid}' is not valid"
			, __FUNCTION__, Guid.ToString());
		return false;
	}
	if (int32* PositionPtr = GuidToArrayPos.Find(Guid);
		PositionPtr
		&& *PositionPtr != INDEX_NONE
		&& Items.IsValidIndex(*PositionPtr))
	{
		ResultEntry = Items[*PositionPtr];
		return true;
	}
	return false;
}

#pragma region Misc Helper

bool FBA_FFA_ObjectArray::CheckForSubobjectListSupport(FBA_FFA_Object& Entry)
{
	if (!Owner)
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Owner provided not valid!"
			, __FUNCTION__);
		return false;
	}
	if (!Owner->IsUsingRegisteredSubObjectList())
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Owner->IsUsingRegisteredSubObjectList is false - but needs to be true."
			, __FUNCTION__);
		return false;
	}
	//if (!IsValid(Entry.ObjectPtr))
	if (Entry.SerializedObject.IsEmpty())
	{
		return false;
	}
	return true;
}

void FBA_FFA_ObjectArray::ForEachChildren(const TFunctionRef<void(FBA_FFA_Object)>& Func)
{
	for (FBA_FFA_Object& Slot : Items)
	{
		Func(Slot);
	}
}

#pragma endregion
