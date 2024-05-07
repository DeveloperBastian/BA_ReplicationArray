// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "FFAStructs/FBA_FFA_Object.h"
#include "Logging/StructuredLog.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Compression/CompressedBuffer.h"
#include "BA_RepArray.h"
#include "BA_Statics.h"

FBA_FFA_Object::FBA_FFA_Object(FString SerializedStorageObject, UClass* StorageObjectClass)
{
	if (!SerializedStorageObject.IsEmpty())
	{
		SerializedObject = SerializedStorageObject;
		if (ClassToCastTo)
		{
			ClassToCastTo = StorageObjectClass;
		}
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
		if (ClassToCastTo)
		{
			ClassToCastTo = StorageObjectClass;
		}
	}
	else
	{
		UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Serialized object is empty"
			, __FUNCTION__);
	}
}


FString FBA_FFA_Object::ToString()
{
	return FBA_FFA_Object::StaticStruct()->GetName()
		+ " '"
		+ InstanceIdentifier
		+ "' ["
		+ InstanceGuid.ToString()
		+ "], sorting index "
		+ FString::FromInt(SortIndex);
}
