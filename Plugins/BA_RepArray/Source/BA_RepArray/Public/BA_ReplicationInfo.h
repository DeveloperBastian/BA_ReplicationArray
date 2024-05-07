// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Interfaces/IPluginManager.h"
#include "Templates/SubclassOf.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Logging/StructuredLog.h"
#include "BA_RepArray.h"
#include "BA_FStatistics.h"
#include "FFAStructs/FBA_FFA_ObjectArray.h"
#include "BA_Statics.h"
#include "BA_ReplicationInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBA_SingleEntrySignature, FBA_FFA_Object, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBA_ArrayCountChange, int32, ArrayCount);


UCLASS(BlueprintType, NotPlaceable, ClassGroup = ("BA Replication Array"), Config = "BA_RepArray")
class BA_REPARRAY_API ABA_ReplicationInfo : public AInfo
{
    GENERATED_BODY()

public:

    ABA_ReplicationInfo();

    void BindEvents();

#pragma region Delegate Implementations

    UPROPERTY(BlueprintAssignable)
    FBA_SingleEntrySignature OnEntryPostReplicatedAdd;

    UPROPERTY(BlueprintAssignable)
    FBA_SingleEntrySignature OnEntryPostReplicatedChange;

    UPROPERTY(BlueprintAssignable)
    FBA_SingleEntrySignature OnEntryPreReplicatedRemove;

    UPROPERTY(BlueprintAssignable)
    FBA_ArrayCountChange OnEntryPostReplicatedReceive;

#pragma endregion

    UFUNCTION(BlueprintCallable, BlueprintPure)
    int32 GetArrayCount();

#pragma region All Authority Levels

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TMap<FGuid, UObject*> GetArrayObjects();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetObjectByGuid(FGuid Guid, bool& Found, UObject*& ObjectFound, FString& InstanceIdentifier);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetObjectByIdentifier(FString Identifier, bool& Found, UObject*& ObjectFound);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetRandomEntry(bool& Found, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetEntryObject(FGuid Guid, bool& ValidObjectFound, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier);

#pragma region Sorting & Filtering

    UFUNCTION(BlueprintCallable)
    void SortByArrayIndex();

    UFUNCTION(BlueprintCallable)
    void SortByObjectPropertyName(FString PropertyName);

#pragma endregion

#pragma endregion

#pragma region Authority Only

#pragma region Array CRUD
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddObject(UObject* StorageObject, bool& SuccessfullyAdded, FGuid& InstanceGuid, FString& InstanceIdentifier);
    
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void ClearArray();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    bool RemoveEntry(FGuid Guid, UObject*& DeletedEntry);
#pragma endregion

#pragma endregion

#pragma region DEBUG
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    TMap<FString, FString> GetObjectsPropertyMap()
    {
        return ReplicatedObjectArray.EntryObjectsPropertyMap;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<FString> GetSortableTypeArray()
    {
        return SortableTypesArray;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString DumpObjectProperties(UObject* Object)
    {
        if (Object)
        {
            return BA_Statics::GetObjectPropertiesAsString(Object);
        }
        else
        {
            return "";
        }
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FString DumpStatisticsProperties()
    {
        FString StatisticsResult;
        for (FBA_FStatistics Stat : StatisticsArray)
        {
            StatisticsResult += Stat.ToString() + LINE_TERMINATOR;
        }
        return StatisticsResult;
    }


#pragma endregion

#pragma region Networking & Replication
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
#pragma endregion

private:

    void UpdateStatistics_Add(UObject* Object);
    void UpdateStatistics_Remove(UObject* Object);

private:
    UPROPERTY(Replicated)
    FBA_FFA_ObjectArray ReplicatedObjectArray;

    UPROPERTY(Replicated)
    TArray<FBA_FStatistics> StatisticsArray;
	
	UPROPERTY(Config)
	TArray<FString> SortableTypesArray;

    UPROPERTY()
    TArray<FName> Adjectives;

    UPROPERTY()
    TArray<FName> Names;
    
    UPROPERTY(Replicated)
    FRandomStream RandomStream;

private:
    bool LoadFileToArray(FString FileName, TArray<FName>& TargetArray);

    FString GetUniqueName();

    bool CheckObjectPropertyForStatistics(TFieldIterator<FProperty> PropIt, UObject* Object, int32& StatPosition, double& PropertyValue);
    
};
