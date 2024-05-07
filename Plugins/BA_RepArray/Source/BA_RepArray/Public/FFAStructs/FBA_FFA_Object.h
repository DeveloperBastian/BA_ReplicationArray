// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "FBA_FFA_Object.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct BA_REPARRAY_API FBA_FFA_Object : public FFastArraySerializerItem
{
	GENERATED_BODY()

    FBA_FFA_Object() : SerializedObject(""), ClassToCastTo(UObject::StaticClass()), SortIndex(INDEX_NONE), InstanceGuid(FGuid()), InstanceIdentifier("") { }
    FBA_FFA_Object(FString SerializedStorageObject, UClass* StorageObjectClass);
    FBA_FFA_Object(FGuid Guid, FString SerializedStorageObject, UClass* StorageObjectClass);

	friend struct FBA_FFA_ObjectArray;
    friend class ABA_ReplicationInfo;

public:
    FString ToString();

#pragma region Compare and Sort

#pragma region Compare by InstanceGuid
    friend uint32 GetTypeHash(const FBA_FFA_Object& Struct)
    {
        return GetTypeHash(Struct.InstanceGuid.ToString());
    }

    FORCEINLINE bool operator==(const FBA_FFA_Object& StructToCompare) const
    {
        return InstanceGuid == StructToCompare.InstanceGuid;
    }

    FORCEINLINE bool Equals(const FBA_FFA_Object& Other) const
    {
        return InstanceGuid == Other.InstanceGuid;
    }

    FORCEINLINE bool operator!=(const FBA_FFA_Object& Other) const
    {
        return InstanceGuid != Other.InstanceGuid;
    }
#pragma endregion

#pragma region Compare by SortIndex
    FORCEINLINE bool operator>(const FBA_FFA_Object& Other) const
    {
        return SortIndex > Other.SortIndex;
    }

    FORCEINLINE bool operator<(const FBA_FFA_Object& Other) const
    {
        return SortIndex < Other.SortIndex;
    }

    FORCEINLINE bool operator<=(const FBA_FFA_Object& Other) const
    {
        return SortIndex <= Other.SortIndex;
    }

    FORCEINLINE bool operator>=(const FBA_FFA_Object& Other) const
    {
        return SortIndex >= Other.SortIndex;
    }
#pragma endregion

#pragma endregion

private:

    UPROPERTY()
    FString SerializedObject;

    UPROPERTY()
    UClass* ClassToCastTo;

    UPROPERTY()
	int32 SortIndex;

	UPROPERTY()
	FGuid InstanceGuid = FGuid::NewGuid();

    UPROPERTY()
    FString InstanceIdentifier = InstanceGuid.ToString();

};