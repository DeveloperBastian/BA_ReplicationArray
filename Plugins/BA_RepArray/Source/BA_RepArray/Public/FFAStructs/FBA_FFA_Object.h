// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "Enums/BA_EEntrySource.h"
#include "FBA_FFA_Object.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct BA_REPARRAY_API FBA_FFA_Object : public FFastArraySerializerItem
{
	GENERATED_BODY()

    FBA_FFA_Object() : SourceObject(EBA_EEntrySource::E_Object), SerializedObject(""), ClassToCastTo(UObject::StaticClass()), SortIndex(INDEX_NONE), InstanceGuid(FGuid()), InstanceIdentifier("") { }
    FBA_FFA_Object(FString SerializedStorageObject, UClass* StorageObjectClass);
    FBA_FFA_Object(FGuid Guid, FString SerializedStorageObject, UClass* StorageObjectClass);

	friend struct FBA_FFA_ObjectArray;
    friend class ABA_ReplicationInfo;

public:

    void GetIdentifier(FString& HumanReadableName);

    void GetInstanceGuid(FGuid EntryGuid);

    void GetSortingIndex(int32 SortIndexInArray);

    void GetStorageObjectClass(UClass*& StorageObjectClass);

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
    EBA_EEntrySource SourceObject;

    UPROPERTY()
    FString SerializedObject;

public:

    UPROPERTY(BlueprintReadOnly)
    UClass* ClassToCastTo;

    UPROPERTY(BlueprintReadOnly)
	int32 SortIndex;

	UPROPERTY(BlueprintReadOnly)
	FGuid InstanceGuid = FGuid::NewGuid();

    UPROPERTY(BlueprintReadOnly)
    FString InstanceIdentifier = InstanceGuid.ToString();

};