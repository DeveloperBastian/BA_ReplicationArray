// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "BA_RepArrayActorComponent.h"
#include "BA_RepArrayActorComp.h"
#include "Net/UnrealNetwork.h"
#include "Logging/StructuredLog.h"

// Sets default values for this component's properties
UBA_RepArrayActorComponent::UBA_RepArrayActorComponent()
{
    SetIsReplicatedByDefault(true);
}

#pragma region Overrides

void UBA_RepArrayActorComponent::BeginPlay()
{
    Super::BeginPlay();
    bool EnableTick = false;
    this->SetComponentTickEnabled(EnableTick);
    PrimaryComponentTick.bStartWithTickEnabled = PrimaryComponentTick.bCanEverTick = EnableTick;

    bAutoActivate = true;
}

void UBA_RepArrayActorComponent::PostInitProperties()
{
    Super::PostInitProperties();
}

void UBA_RepArrayActorComponent::ReadyForReplication()
{
    Super::ReadyForReplication();
    this->bReplicateUsingRegisteredSubObjectList = true;
    GetOwner()->AllowActorComponentToReplicate(this);

    if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
    {
        if (IsUsingRegisteredSubObjectList())
        {
            // AddReplicatedSubObject(...);
        }
    }
}

void UBA_RepArrayActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    FDoRepLifetimeParams SharedParams;
    SharedParams.bIsPushBased = false;

    DOREPLIFETIME_WITH_PARAMS(ThisClass, ReplicationArrays, SharedParams);
    DOREPLIFETIME_WITH_PARAMS(ThisClass, ReplicationArrayNames, SharedParams);
}

void UBA_RepArrayActorComponent::OnRegister()
{
    Super::OnRegister();
}

void UBA_RepArrayActorComponent::OnUnregister()
{
    Super::OnUnregister();
}

// Called every frame
void UBA_RepArrayActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma endregion

void UBA_RepArrayActorComponent::AddReplicationArray(FString ArrayName, bool& WasAdded)
{
    WasAdded = false;
    if (!CheckArrayNameParameter(ArrayName, false))
    {
        return;
    }

    UWorld* World = Cast<UWorld>(GetOuter());
    if (!World)
    {
        UE_LOGFMT(Log_BA_RepArrayActorComponent, Error, "{function}: Fail to create World reference"
            , __FUNCTION__);
        return;
    }

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnInfo.Name = ABA_ReplicationInfo::StaticClass()->GetFName();
    SpawnInfo.bHideFromSceneOutliner = true;
    ABA_ReplicationInfo* RepArrayActor = World->SpawnActor<ABA_ReplicationInfo>(ABA_ReplicationInfo::StaticClass(), SpawnInfo);

    if (RepArrayActor == nullptr)
    {
        UE_LOGFMT(Log_BA_RepArrayActorComponent, Error, "{function}: Fail to spawn ABA_ReplicationInfo in World '{word}'"
            , __FUNCTION__, World->GetName());
        return;
    }
    ReplicationArrays.Push(RepArrayActor);
    ReplicationArrayNames.Push(ArrayName);
    WasAdded = true;
    this->OnRepArrayAdded.Broadcast(ArrayName);
}

void UBA_RepArrayActorComponent::DeleteReplicationArray(FString ArrayName, bool& WasDeleted)
{
    WasDeleted = false;
    if (!CheckArrayNameParameter(ArrayName, true))
    {
        return;
    }
    if (int32 Position = ReplicationArrayNames.Find(ArrayName);
        Position != INDEX_NONE && ReplicationArrays.IsValidIndex(Position))
    {
        ReplicationArrayNames.RemoveAt(Position, EAllowShrinking::Yes);
        ReplicationArrays.RemoveAt(Position, EAllowShrinking::Yes);
        WasDeleted = true;
        this->OnRepArrayDeleted.Broadcast(ArrayName);
    }
}

void UBA_RepArrayActorComponent::GetReplicationArray(FString ArrayName, ABA_ReplicationInfo*& ReplicationArray, bool& WasFound)
{
    if (!CheckArrayNameParameter(ArrayName, true))
    {
        return;
    }
    if (int32 Position = ReplicationArrayNames.Find(ArrayName);
        Position != INDEX_NONE && ReplicationArrays.IsValidIndex(Position))
    {
        ReplicationArray = ReplicationArrays[Position];
    }
}

void UBA_RepArrayActorComponent::GetReplicationArrays(TArray<ABA_ReplicationInfo*>& CurrentReplicationArrays)
{
    CurrentReplicationArrays = this->ReplicationArrays;
}

void UBA_RepArrayActorComponent::GetReplicationArrayNmes(TArray<FString>& CurrentReplicationArrayNames)
{
    CurrentReplicationArrayNames = ReplicationArrayNames;
}

#pragma region PrivateFunctions

bool UBA_RepArrayActorComponent::CheckArrayNameParameter(FString& ArrayName, bool NameShouldExist)
{
    if (ArrayName.IsEmpty())
    {
        UE_LOGFMT(Log_BA_RepArrayActorComponent, Error, "{function}: Array name cannot be empty"
            , __FUNCTION__);
        return false;
    }
    if (NameShouldExist)
    {
        if (ReplicationArrayNames.Find(ArrayName) == INDEX_NONE)
        {
            UE_LOGFMT(Log_BA_RepArrayActorComponent, Error, "{function}: Array name does not exist"
                , __FUNCTION__);
            return false;
        }
        return true;
    }
    else
    {
        if (ReplicationArrayNames.Find(ArrayName) != INDEX_NONE)
        {
            UE_LOGFMT(Log_BA_RepArrayActorComponent, Error, "{function}: Array name already exists (name is case insensitive)"
                , __FUNCTION__);
            return false;
        }
        return true;
    }
}

#pragma endregion