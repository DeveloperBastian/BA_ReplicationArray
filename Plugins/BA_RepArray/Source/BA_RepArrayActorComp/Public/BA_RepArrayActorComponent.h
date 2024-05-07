// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BA_ReplicationInfo.h"
#include "BA_RepArrayActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBA_ReplicationArrayChangeSignature, FString, ArrayName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BA_REPARRAYACTORCOMP_API UBA_RepArrayActorComponent : public UActorComponent
{
    GENERATED_BODY()

public: 
    // Sets default values for this component's properties
    UBA_RepArrayActorComponent();

    UPROPERTY(BlueprintAssignable)
    FBA_ReplicationArrayChangeSignature OnRepArrayAdded;

    UPROPERTY(BlueprintAssignable)
    FBA_ReplicationArrayChangeSignature OnRepArrayDeleted;

    UFUNCTION(BlueprintCallable)
    void AddReplicationArray(FString ArrayName, bool& WasAdded);

    UFUNCTION(BlueprintCallable)
    void DeleteReplicationArray(FString ArrayName, bool& WasDeleted);

    UFUNCTION(BlueprintCallable)
    void GetReplicationArray(FString ArrayName, ABA_ReplicationInfo*& ReplicationArray, bool& WasFound);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetReplicationArrays(TArray<ABA_ReplicationInfo*>& CurrentReplicationArrays);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    void GetReplicationArrayNmes(TArray<FString>& CurrentReplicationArrayNames);

    
protected:
    // Called when the game starts
    virtual void BeginPlay() override;

#pragma region Overrides

private:
    virtual void PostInitProperties() override;
    virtual void ReadyForReplication() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

private:
    UPROPERTY(Replicated)
    TArray<ABA_ReplicationInfo*> ReplicationArrays;

    UPROPERTY(Replicated)
    TArray<FString> ReplicationArrayNames;

private:

    bool CheckArrayNameParameter(FString& ArrayName, bool NameShouldExist);

};
