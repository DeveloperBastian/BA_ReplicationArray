// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.
#include "BA_ReplicationInfo.h"
#include "BA_RepArray.h"
#include "Math/NumericLimits.h"
#include "Async/Async.h"
#include "UObject/UnrealTypePrivate.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

ABA_ReplicationInfo::ABA_ReplicationInfo()
{
    ReplicatedObjectArray = FBA_FFA_ObjectArray(this);

    bReplicates = true;
    NetUpdateFrequency = 50.0f;
    SetHidden(true);
    SetReplicatingMovement(false);
    SetCanBeDamaged(false);

    this->bReplicateUsingRegisteredSubObjectList = true;
    //ReplicatedObjectArray = CreateDefaultSubobject<FBA_FFA_ObjectArray>(FName(FGuid::NewGuid().ToString()));
    BindEvents();
    if (this->HasAuthority())
    {
        // Load convenience names and adjectives to give better object names
        LoadFileToArray("Adjectives.txt", Adjectives);
        LoadFileToArray("Names.txt", Names);
    }
    RandomStream = FMath::Rand();
}

#pragma region Authority Only

void ABA_ReplicationInfo::AddObject(UObject* StorageObject, bool& SuccessfullyAdded, FGuid& InstanceGuid, FString& InstanceIdentifier, const int64 NumberOfNewObjects)
{
    SuccessfullyAdded = false;
    if (NumberOfNewObjects <= 0)
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: NumberOfNewObject was set to '{number}'"
            , __FUNCTION__, FString::FromInt(NumberOfNewObjects));
        return;
    }
    
    if (StorageObject)
    {
        int64 SuccessCounter = 0;
        
        TMap<FString, FGuid> Identifiers;
        while (Identifiers.Num() < NumberOfNewObjects)
        {
            Identifiers.Emplace(this->GetUniqueName(), FGuid::NewGuid());
        }
        for (auto& KvP : Identifiers)
        {
            if (SuccessfullyAdded = ReplicatedObjectArray.AddEntry(StorageObject, KvP.Value, KvP.Key);
                SuccessfullyAdded == true)
            {
                InstanceIdentifier = KvP.Key;
                InstanceGuid = KvP.Value;
                SuccessCounter++;
                UpdateStatistics_Add(StorageObject);
            }
        }
        SuccessfullyAdded = SuccessCounter == NumberOfNewObjects;
    }
    else
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Object provided is not valid and cannot be added"
            , __FUNCTION__);
    }
}

void ABA_ReplicationInfo::ClearArray()
{
    ReplicatedObjectArray.Clear();
    StatisticsArray.Empty();
    OnFullArrayChangeEmpty.Broadcast();
}

bool ABA_ReplicationInfo::RemoveEntry(FGuid Guid, UObject*& DeletedEntry)
{
    if (ReplicatedObjectArray.RemoveEntry(Guid, DeletedEntry);
        IsValid(DeletedEntry))
    {
        UpdateStatistics_Remove(DeletedEntry);
        return true;
    }
    return false;
}

#pragma endregion

#pragma region All Authority Levels
int32 ABA_ReplicationInfo::GetArrayCount()
{
    return ReplicatedObjectArray.Items.Num();
}

TMap<FGuid, UObject*> ABA_ReplicationInfo::GetArrayObjects()
{
    TMap<FGuid, UObject*> Results;
    ReplicatedObjectArray.ForEachChildren([&Results, this](FBA_FFA_Object Entry)
        {
            if (UObject* Object = BA_Statics::DeserializeObjectFromString(Entry.SerializedObject, this, Entry.ClassToCastTo);
                Object)
            {
                Results.Emplace(Entry.InstanceGuid, Object);
                return true;
            }
            return false;
        });
    return Results;
}

void ABA_ReplicationInfo::GetObjectByGuid(FGuid Guid, bool& Found, UObject*& ObjectFound, FString& InstanceIdentifier)
{
    Found = false;
    FBA_FFA_Object Entry;
    if (!ReplicatedObjectArray.GetEntryByGuid(Guid, Entry))
    {

        Found = false;
        return;
    }
    if (ObjectFound = BA_Statics::DeserializeObjectFromString(
        Entry.SerializedObject,
        this,
        Entry.ClassToCastTo);
        ObjectFound)
    {
        InstanceIdentifier = Entry.InstanceIdentifier;
        Found = true;
    }
}

void ABA_ReplicationInfo::GetObjectByIdentifier(FString Identifier, bool& Found, UObject*& ObjectFound)
{
    Found = false;
    FBA_FFA_Object Entry;
    if (!ReplicatedObjectArray.GetEntryByIdentifier(Identifier, Entry))
    {
        Found = false;
        return;
    }
    if (ObjectFound = BA_Statics::DeserializeObjectFromString(
        Entry.SerializedObject,
        this,
        Entry.ClassToCastTo);
        ObjectFound)
    {
        Found = true;
    }
}

void ABA_ReplicationInfo::GetRandomEntry(bool& Found, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier)
{
    Found = false;
    if (ReplicatedObjectArray.Items.Num() == 0)
    {
        return;
    }
    int32 RandomEntryNumber = RandomStream.RandRange(0, (ReplicatedObjectArray.Items.Num() - 1));
    
    if (ObjectFound = BA_Statics::DeserializeObjectFromString(
        ReplicatedObjectArray.Items[RandomEntryNumber].SerializedObject,
        this,
        ReplicatedObjectArray.Items[RandomEntryNumber].ClassToCastTo);
        ObjectFound)
    {
        InstanceGuid = ReplicatedObjectArray.Items[RandomEntryNumber].InstanceGuid;
        InstanceIdentifier = ReplicatedObjectArray.Items[RandomEntryNumber].InstanceIdentifier;
        if (InstanceGuid.IsValid())
        {
            Found = true;
        }
    }
}

void ABA_ReplicationInfo::GetEntryObject(FGuid Guid, bool& ValidObjectFound, UObject*& ObjectFound, FGuid& InstanceGuid, FString& InstanceIdentifier)
{
    ValidObjectFound = false;
    if (!Guid.IsValid())
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Object provided is not valid and cannot be added"
            , __FUNCTION__);
        return;
    }
    
    if (int32* Position = ReplicatedObjectArray.GuidToArrayPos.Find(Guid);
        Position && *Position != INDEX_NONE)
    {
        if (!ReplicatedObjectArray.Items.IsValidIndex(*Position))
        {
            UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Detected mismatch between array and position map - position {pos} is not valid!"
                , __FUNCTION__, FString::FromInt(*Position));
            return;
        }

        if (ObjectFound = BA_Statics::DeserializeObjectFromString(
            ReplicatedObjectArray.Items[*Position].SerializedObject,
            this,
            ReplicatedObjectArray.Items[*Position].ClassToCastTo);
            ObjectFound)
        {
            InstanceGuid = ReplicatedObjectArray.Items[*Position].InstanceGuid;
            InstanceIdentifier = ReplicatedObjectArray.Items[*Position].InstanceIdentifier;
            ValidObjectFound = true;
        }
    }
    UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Guid not found in array"
        , __FUNCTION__);
    return;
}

void ABA_ReplicationInfo::SortByArrayIndex()
{
    ReplicatedObjectArray.SortByIndex();
    // if server side, replicate changed array
    if (this->HasAuthority())
    {
        ReplicatedObjectArray.MarkArrayDirty();
        UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Full array replication triggered as sorting was done on server side"
            , __FUNCTION__);
        return;
    }
    OnFullArrayChangeSort.Broadcast();
}

void ABA_ReplicationInfo::SortByObjectPropertyName(FString PropertyName)
{
    if (PropertyName.IsEmpty())
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Not sorting as PropertyName is empty"
            , __FUNCTION__);
        return;
    }
    ReplicatedObjectArray.SortByPropertyName(PropertyName, SortableTypesArray);
    // if server side, replicate changed array
    if (this->HasAuthority())
    {
        ReplicatedObjectArray.MarkArrayDirty();
        UE_LOGFMT(Log_BA_IM_RepArray, Warning, "{function}: Full array replication triggered as sorting was done on server side"
            , __FUNCTION__);
        return;
    }
    OnFullArrayChangeSort.Broadcast();
}

#pragma endregion

#pragma region Misc Helper

void ABA_ReplicationInfo::BindEvents()
{
    ReplicatedObjectArray.OnEntryPostReplicatedAdd.BindLambda([this](FBA_FFA_Object Entry)
        {
            this->OnEntryPostReplicatedAdd.Broadcast(Entry);
            UE_LOGFMT(Log_BA_IM_RepArray, Verbose, "{function}: OnEntryPostReplicatedAdd: {entry}"
                , __FUNCTION__, Entry.ToString());
        });
    ReplicatedObjectArray.OnEntryPostReplicatedChange.BindLambda([this](FBA_FFA_Object Entry)
        {
            this->OnEntryPostReplicatedChange.Broadcast(Entry);
            UE_LOGFMT(Log_BA_IM_RepArray, Verbose, "{function}: OnEntryPostReplicatedChange: {entry}"
                , __FUNCTION__, Entry.ToString());
        });
    ReplicatedObjectArray.OnEntryPreReplicatedRemove.BindLambda([this](FBA_FFA_Object Entry)
        {
            this->OnEntryPreReplicatedRemove.Broadcast(Entry);
            UE_LOGFMT(Log_BA_IM_RepArray, Verbose, "{function}: OnEntryPreReplicatedRemove: {entry}"
                , __FUNCTION__, Entry.ToString());
        });
    ReplicatedObjectArray.OnEntryPostReplicatedReceive.BindLambda([this](int32 OldArrayCount)
        {
            this->OnEntryPostReplicatedReceive.Broadcast(OldArrayCount);
            UE_LOGFMT(Log_BA_IM_RepArray, Verbose, "{function}: OnEntryPostReplicatedReceive: Array Count {count}"
                , __FUNCTION__, FString::FromInt(OldArrayCount));
        });
}

bool ABA_ReplicationInfo::LoadFileToArray(FString FileName, TArray<FName>& TargetArray)
{
    // check plugin name 
    FString PluginName = TEXT("BA_RepArray");
    TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName);
    if (!Plugin.IsValid())
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Plugin name '{plugin}' is not valid"
            , __FUNCTION__, PluginName);
        return false;
    }
    // get resource dir and check file
    const FString ResourceDir = "Resources";
    const FString DataFilesDir = "DataFiles";
    const FString FilePath = FPaths::Combine(
        *Plugin->GetBaseDir()
        , *ResourceDir
        , *DataFilesDir
        , *FileName);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.FileExists(*FilePath))
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: File '{file}' does not exist"
            , __FUNCTION__, FilePath);
        return false;
    }
    // load file
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *FilePath))
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Error loading file '{file}'"
            , __FUNCTION__, FilePath);
        return false;
    }
    TargetArray.Empty();
    for (FString Line : Lines)
    {
        if (!Line.IsEmpty())
        {
            TargetArray.Push(FName(Line));
        }
    }

    UE_LOGFMT(Log_BA_IM_RepArray, Log, "{function}: Loaded '{file}' with {count} entries"
        , __FUNCTION__, FilePath, TargetArray.Num());
    return true;
}

FString ABA_ReplicationInfo::GetUniqueName()
{
    if (Adjectives.Num() == 0 || Names.Num() == 0)
    {
        return FString(); 
    }
    int32 RandomEntryNumberAdj01 = RandomStream.RandRange(0, (Adjectives.Num() - 1));
    int32 RandomEntryNumberAdj02 = RandomStream.RandRange(0, (Adjectives.Num() - 1));
    int32 RandomEntryNumberNames = RandomStream.RandRange(0, (Names.Num() - 1));

    return Adjectives[RandomEntryNumberAdj01].ToString() + Adjectives[RandomEntryNumberAdj02].ToString() + Names[RandomEntryNumberNames].ToString();
}

bool ABA_ReplicationInfo::CheckObjectPropertyForStatistics(TFieldIterator<FProperty> PropIt, UObject* Object, int32& StatPosition, double& PropertyValue)
{
    bool Result = false;
    if (!IsValid(Object) || !PropIt->IsA(FNumericProperty::StaticClass()))
    {
        return Result;
    }
    StatPosition = INDEX_NONE;

    FString PropertyName = PropIt->GetName();
    FString PropertyType = PropIt->GetCPPType();

#pragma region Get Index of FBA_FStatistics
    int32 Index = StatisticsArray.IndexOfByPredicate([PropertyName](FBA_FStatistics Stat)
        {
            return PropertyName.Equals(Stat.GetPropertyName().ToString(), ESearchCase::IgnoreCase);
        }
    );
    // add if new
    if (Index == INDEX_NONE)
    {
        FBA_FStatistics Stat = FBA_FStatistics(FName(PropertyName), FName(PropertyType));
        // get value
        Index = StatisticsArray.Add(Stat);
    }
    // error here
    if (Index == INDEX_NONE)
    {
        UE_LOGFMT(Log_BA_IM_RepArray, Error, "{function}: Error adding a new FBA_FStatistics to the StatisticsArray"
            , __FUNCTION__, PropertyName, PropertyType);
        return Result;
    }
#pragma endregion

    constexpr double Min = TNumericLimits<double>::Min();
    double Value = Min;

    FProperty* Property = *PropIt;
    // check numeric and cast
    if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
    {
        if (NumericProperty && NumericProperty->IsFloatingPoint())
        {
            Value = NumericProperty->GetFloatingPointPropertyValue(Property->ContainerPtrToValuePtr<float>(Object));
        }
        else if (NumericProperty && NumericProperty->IsInteger())
        {
            int64 IntValue = NumericProperty->GetSignedIntPropertyValue(Property->ContainerPtrToValuePtr<int64>(Object));
            Value = static_cast<double>(IntValue);
        }
    }

    if (Value > Min && StatisticsArray.IsValidIndex(Index))
    {
        StatPosition = Index;
        PropertyValue = Value;
        Result = true;
    }
    return Result;
}

void ABA_ReplicationInfo::UpdateStatistics_Add(UObject* Object)
{
    if (!IsValid(Object)) { return; }

    for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
    {
        int32 Position; double PropertyValue;
        
        if (CheckObjectPropertyForStatistics(PropIt, Object, Position, PropertyValue))
        {
            StatisticsArray[Position].AddValue(PropertyValue);
        }
    }
}

void ABA_ReplicationInfo::UpdateStatistics_Remove(UObject* Object)
{
    if (!IsValid(Object)) { return; }

    for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
    {
        int32 Position; double PropertyValue;

        if (CheckObjectPropertyForStatistics(PropIt, Object, Position, PropertyValue))
        {
            StatisticsArray[Position].RemoveValue(PropertyValue);
        }
    }
}


#pragma endregion

#pragma region Network & Replication

void ABA_ReplicationInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams Params;
    Params.bIsPushBased = false;
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedObjectArray, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, RandomStream, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, StatisticsArray, Params);
    DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Name, Params);
}

bool ABA_ReplicationInfo::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    if (!bReplicateUsingRegisteredSubObjectList)
    {
        bWroteSomething |= ReplicatedObjectArray.ReplicateFFAObjects(Channel, Bunch, RepFlags);
    }
    return bWroteSomething;
}

#pragma endregion
