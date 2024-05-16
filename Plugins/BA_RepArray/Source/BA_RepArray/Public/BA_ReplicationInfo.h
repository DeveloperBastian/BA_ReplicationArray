// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformTime.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBA_ArrayChange);

UCLASS(BlueprintType, NotPlaceable, ClassGroup = ("BA Replication Array"), Config = "BA_RepArray",
    meta = (DisplayName = "BA Replication Info", Category = "BA Rep Array|Replication Info Actor"
        , ToolTip = "Actor to manage one FFastArraySerializer.", ShortToolTip = "Actor to manage FFastArraySerializer"))
class BA_REPARRAY_API ABA_ReplicationInfo : public AInfo
{
    GENERATED_BODY()

    friend class UBA_RepArrayActorComponent;
public:

    ABA_ReplicationInfo();

    void BindEvents();

#pragma region Delegate Implementations

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised after a newly added entry was replicated."
        , ShortToolTip = "On Entry Post Replicated Add", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_SingleEntrySignature OnEntryPostReplicatedAdd;

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised after a changed entry was replicated."
        , ShortToolTip = "On Entry Post Replicated Change", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_SingleEntrySignature OnEntryPostReplicatedChange;

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised before replicating that an entry was removed."
        , ShortToolTip = "On Entry Pre Replicated Remove", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_SingleEntrySignature OnEntryPreReplicatedRemove;

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised after an replicated entry has been received."
        , ShortToolTip = "On Entry Post Replicated Receive", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_ArrayCountChange OnEntryPostReplicatedReceive;

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised after the array order has been changed through sorting."
        , ShortToolTip = "On Full Array Change Sort", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_ArrayChange OnFullArrayChangeSort;

    UPROPERTY(BlueprintAssignable, meta = (ToolTip = "Event raised after the array has been emtied."
        , ShortToolTip = "On Full Array Change Empty", Category = "BA Rep Array|Replication Info Actor|Events"))
    FBA_ArrayChange OnFullArrayChangeEmpty;

#pragma endregion

#pragma region All Authority Levels

    /**
     * Retrieves the count of all elements in the Replication Array.
     *
     * @return Returns the number of elements in the Replication Array as an int32.
     * @note This function is callable from Blueprints and does not modify the state of the object (BlueprintPure).
     * @meta ToolTip: "Get all Replication Array count."
     * @meta ShortToolTip: "Get Array Count"
     * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
     * @meta CompactNodeTitle: "Get Array Count"
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Get all Replication Array count."
        , ShortToolTip = "Get Array Count", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Get Array Count"))
    int32 GetArrayCount();

    /**
     * Retrieves all objects from the Replication Array.
     *
     * @return Returns a map where each entry consists of a GUID key and a UObject* value, representing all objects within the Replication Array.
     * @note This function is callable from Blueprints and does not modify the state of the object (BlueprintPure).
     * @meta ToolTip: "Get all Objects from the Replication Array."
     * @meta ShortToolTip: "Get All Objects"
     * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
     * @meta CompactNodeTitle: "Get All Objects"
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Get all Objects from the Replication Array."
        , ShortToolTip = "Get All Objects", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Get All Objects"))
    TMap<FGuid, UObject*> GetArrayObjects();

    /**
    * Retrieves an object by its instance GUID.
    *
    * @param Guid The unique identifier of the object to retrieve.
    * @param Found This will be set to true if the object is found, otherwise false.
    * @param ObjectFound A reference to a pointer of the UObject that was found.
    * @param InstanceIdentifier A string identifier for the instance.
    * @return void
    * @note This function is callable from Blueprints and does not modify the state of the object (BlueprintPure).
    * @meta ToolTip: "Get Object by its Instance Guid."
    * @meta ShortToolTip: "Object By Guid"
    * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
    * @meta CompactNodeTitle: "Object By Guid"
    */
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Get Object by its Instance Guid."
        , ShortToolTip = "Object By Guid", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Object By Guid"))
    void GetObjectByGuid(FGuid Guid, bool& Found, UObject*& ObjectFound, FString& InstanceIdentifier);

    /**
    * Retrieves an object by its identifier.
    *
    * @param Identifier The identifier of the object to retrieve.
    * @param Found This will be set to true if the object is found, otherwise false.
    * @param ObjectFound A reference to a pointer of the UObject that was found.
    * @return void
    * @note This function is callable from Blueprints and does not modify the state of the object (BlueprintPure).
    * @meta ToolTip: "Get Object by its Identifier."
    * @meta ShortToolTip: "Object By Id"
    * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
    * @meta CompactNodeTitle: "Object By Id"
    */
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Get Object by its Identifier."
        , ShortToolTip = "Object By Id", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Object By Id"))
    void GetObjectByIdentifier(FString Identifier, bool& Found, UObject*& ObjectFound);

    /**
     * Retrieves a random entry from the Replication Array.
     *
     * @param Found This will be set to true if a random entry is found, otherwise false.
     * @param ObjectFound A reference to a pointer of the UObject that was randomly selected.
     * @param InstanceGuid The unique identifier of the instance that was randomly selected.
     * @param InstanceIdentifier A string identifier for the instance that was randomly selected.
     * @return void
     * @note This function is callable from Blueprints and does not modify the state of the object (BlueprintPure).
     * @meta ToolTip: "Get Random Entry."
     * @meta ShortToolTip: "Random Entry"
     * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
     * @meta CompactNodeTitle: "Random Entry"
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ToolTip = "Get Random Entry."
        , ShortToolTip = "Random Entry", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Random Entry"))
    void GetRandomEntry(bool& Found, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier);

#pragma region Sorting & Filtering

    /**
     * Sorts the elements of the array by their index.
     *
     * @note This function is callable from Blueprints.
     * @meta ToolTip: "Sort by Index."
     * @meta ShortToolTip: "Sort By Index"
     * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
     * @meta CompactNodeTitle: "Sort By Index"
     */
    UFUNCTION(BlueprintCallable, meta = (ToolTip = "Sort by Index."
        , ShortToolTip = "Sort By Index", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Sort By Index"))
    void SortByArrayIndex();

    /**
     * Sorts the elements of the array by a specified property name.
     *
     * @param PropertyName The name of the property to sort the array elements by.
     * @note This function is callable from Blueprints.
     * @meta ToolTip: "Sort By Property."
     * @meta ShortToolTip: "Sort By Property"
     * @meta Category: "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
     * @meta CompactNodeTitle: "Sort By Property"
     */
    UFUNCTION(BlueprintCallable, meta = (ToolTip = "Sort By Property."
        , ShortToolTip = "Sort By Propery", Category = "BA Rep Array|Replication Info Actor|Non Authoritive Functions"
        , CompactNodeTitle = "Sort By Property"))
    void SortByObjectPropertyName(FString PropertyName);

#pragma endregion

#pragma endregion

#pragma region Authority Only

#pragma region Array CRUD
    
    /**
     * Adds an object to a specified storage object.
     *
     * @param StorageObject The storage object to which the new object will be added.
     * @param SuccessfullyAdded This will be set to true if the object was successfully added.
     * @param InstanceGuid The unique identifier for the instance that was added.
     * @param InstanceIdentifier A string identifier for the instance.
     * @param NumberOfNewObject (Optional) The number of new objects to add. Defaults to 1.
     * @note This function is callable from Blueprints and is only authoritative on the server.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddObject(UObject* StorageObject, bool& SuccessfullyAdded, FGuid& InstanceGuid, FString& InstanceIdentifier, int64 NumberOfNewObject = 1);

    /**
     * Clears all elements from the array.
     *
     * @note This function is callable from Blueprints and is only authoritative on the server.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void ClearArray();

    /**
     * Removes an entry from a collection based on the provided GUID.
     *
     * @param Guid The unique identifier of the entry to be removed.
     * @param DeletedEntry A reference to a pointer of the UObject that was removed.
     * @return Returns true if the entry was successfully removed; otherwise, false.
     * @note This function is callable from Blueprints and is only authoritative on the server.
     */
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

    UFUNCTION(BlueprintCallable)
    void StopWatchInMillisecondsStart()
    {
        StartStopWatchTime = FPlatformTime::Seconds();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    double StopWatchInMillisecondsStop()
    {
        double EndTime = FPlatformTime::Seconds();
        double ElapsedSeconds = EndTime - StartStopWatchTime;
        return ElapsedSeconds * 1e3; 
    }

    UFUNCTION(BlueprintCallable)
    void CPUCyclesMeasureStart()
    {
        StartCycles = FPlatformTime::Cycles();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    double CPUCyclesMeasureStop()
    {
        return FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles() - StartCycles);
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

    UPROPERTY(Replicated)
    FString Name;

private:

    UFUNCTION()
    bool LoadFileToArray(FString FileName, TArray<FName>& TargetArray);

    UFUNCTION()
    FString GetUniqueName();

    bool CheckObjectPropertyForStatistics(TFieldIterator<FProperty> PropIt, UObject* Object, int32& StatPosition, double& PropertyValue);

    UFUNCTION()
    void GetEntryObject(FGuid Guid, bool& ValidObjectFound, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier);

private:

    double StartStopWatchTime = 0;

    uint64 StartCycles = 0;
    
};
