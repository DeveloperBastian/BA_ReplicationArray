// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "FFAStructs/FBA_FFA_Object.h"
#include "Logging/StructuredLog.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Compression/CompressedBuffer.h"
#include "BA_RepArray.h"
#include "BA_Statics.h"

#pragma region Constructor

FBA_FFA_Object::FBA_FFA_Object(FString SerializedStorageObject, UClass* StorageObjectClass)
{
	if (!SerializedStorageObject.IsEmpty())
	{
		SerializedObject = SerializedStorageObject;
		InstanceGuid = FGuid::NewGuid();
		ClassToCastTo = StorageObjectClass;
		SourceObject = EBA_EEntrySource::E_Object;
	}
	else
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Serialized object is empty"
			, __FUNCTION__);
	}
}

FBA_FFA_Object::FBA_FFA_Object(FGuid Guid, FString SerializedStorageObject, UClass* StorageObjectClass)
{
	if (!SerializedStorageObject.IsEmpty())
	{
		SerializedObject = SerializedStorageObject;
		InstanceGuid = Guid;
		ClassToCastTo = StorageObjectClass;
		SourceObject = EBA_EEntrySource::E_Object;
	}
	else
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Serialized object is empty"
			, __FUNCTION__);
	}
}

#pragma endregion

void FBA_FFA_Object::GetIdentifier(FString& HumanReadableName)
{
	HumanReadableName = InstanceIdentifier;
}

void FBA_FFA_Object::GetInstanceGuid(FGuid EntryGuid)
{
	EntryGuid = InstanceGuid;
}

void FBA_FFA_Object::GetSortingIndex(int32 SortIndexInArray)
{
	SortIndexInArray = SortIndex;
}

void FBA_FFA_Object::GetStorageObjectClass(UClass*& StorageObjectClass)
{
	StorageObjectClass = ClassToCastTo;
}

FString FBA_FFA_Object::ToString()
{
	return FBA_FFA_Object::StaticStruct()->GetName()
		+ " '"
		+ InstanceIdentifier
		+ "' ["
		+ InstanceGuid.ToString()
		+ "], object type '"
		+ (IsValid(ClassToCastTo) ? ClassToCastTo->GetDisplayNameText().ToString() : "no class defined yet")
		+ "', sorting index "
		+ FString::FromInt(SortIndex);
}
