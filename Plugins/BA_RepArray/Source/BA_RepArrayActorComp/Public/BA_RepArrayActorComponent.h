// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "BA_ReplicationInfo.h"
#include "BA_RepArrayActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBA_ReplicationArrayChangeSignature, FString, ArrayName);

UCLASS(ClassGroup = ("BA Replication Array"), meta = (BlueprintSpawnableComponent, DisplayName = "BA RepArray Comp", Category = "BA Rep Array|Actor Component"
    , ToolTip = "Actor Component to manage BA Replication Arrays.", ShortToolTip = "ActComp for BA RepArrays"))
class BA_REPARRAYACTORCOMP_API UBA_RepArrayActorComponent : public UActorComponent
{
    GENERATED_BODY()

public: 
    UBA_RepArrayActorComponent();

protected:
    virtual void BeginPlay() override;

#pragma region Delegates Implementations

public:
    /*
* Event raised when an entry was added to a replication array.
* Triggered by the OnRepArrayAdded property.
* Can be bound in Blueprint.
*/
    UPROPERTY(BlueprintAssignable
        , meta = (ToolTip = "Event raised when a replication array was added to the Actor Component 'UBA_RepArrayActorComponent'."
            , ShortToolTip = "Added RepArray", Category = "BA Rep Array|Actor Component|Events"))
    FBA_ReplicationArrayChangeSignature OnReplicationArrayAdded;

    /*
    * Event raised when an entry was deleted from from a replication array.
    * Triggered by the OnRepArrayDeleted property.
    * Can be bound in Blueprint.
    */
    UPROPERTY(BlueprintAssignable
        , meta = (ToolTip = "Event raised when an entry was removed from the Actor Component 'UBA_RepArrayActorComponent'."
            , ShortToolTip = "Removed RepArray", Category = "BA Rep Array|Actor Component|Events"))
    FBA_ReplicationArrayChangeSignature OnReplicationArrayDeleted;
#pragma endregion

#pragma region Authority Only
public:
    /**
     * @brief Initializes and adds a Replication Array actor to this Actor Component.
     *
     * @param ArrayName The name of the array to be added. This name can be used to identify and select the array later.
     * @param WasAdded A boolean reference that will be set to true if the array was successfully added, false otherwise.
     * @note This function can only be called on Authority.
     * @return void
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly
        , meta = (ToolTip = "Initialized and adds a Replication Array actor to this Actor Component. The ArrayName can be used to identify and select that array later. Can only called on Authority."
            , ShortToolTip = "Add Rep Array", CompactNodeTitle = "Add Replication Array", Category = "BA Rep Array|Actor Component|CRUD"))
    void AddReplicationArray(FString ArrayName, bool& WasAdded);

    /**
     * @brief Deletes a Replication Array actor from this Actor Component.
     *
     * @param ArrayName The case-sensitive name of the array to be removed.
     * @param WasDeleted A boolean reference that will be set to true if the array was successfully deleted, false otherwise.
     * @note All entries of the array will be deleted. This function can only be called on Authority.
     * @return void
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly
        , meta = (ToolTip = "Deletes a Replication Array actor from this Actor Component. The ArrayName is the case sensitive name of the array to be removed. All Entries of the Array will be deleted. Can only called on Authority."
            , ShortToolTip = "Del Rep Array", CompactNodeTitle = "Delete Replication Array", Category = "BA Rep Array|Actor Component|CRUD"))
    void DeleteReplicationArray(FString ArrayName, bool& WasDeleted);

#pragma endregion

#pragma region No Authority
public:
    /**
* @brief Retrieves a Replication Array actor from this Actor Component.
*
* @param ArrayName The name of the array to be retrieved.
* @param ReplicationArray A pointer reference to an ABA_ReplicationInfo object that will hold the Replication Array actor if found.
* @param WasFound A boolean reference that will be set to true if the array was successfully retrieved, false otherwise.
* @return void
*/
    UFUNCTION(BlueprintCallable
        , meta = (ToolTip = "Gets a Replication Array from this Actor Component by its name."
            , ShortToolTip = "Get Rep Array", CompactNodeTitle = "Get Replication Array", Category = "BA Rep Array|Actor Component|Getter"))
    void GetReplicationArray(FString ArrayName, ABA_ReplicationInfo*& ReplicationArray, bool& WasFound);

    /**
    * @brief Retrieves all Replication Array actors from this Actor Component.
    *
    * @param CurrentReplicationArrays An array reference that will be filled with pointers to ABA_ReplicationInfo objects representing the current Replication Arrays.
    * @return void
    * @note This function is marked as BlueprintPure, meaning it does not modify the state of the object and can be used in blueprints without executing additional logic.
    */
    UFUNCTION(BlueprintCallable, BlueprintPure
        , meta = (ToolTip = "Gets all Replication Arrays from this Actor Component."
            , ShortToolTip = "Get Rep Arrays", CompactNodeTitle = "Get Replication Arrays", Category = "BA Rep Array|Actor Component|Getter"))
    void GetReplicationArrays(TArray<ABA_ReplicationInfo*>& CurrentReplicationArrays);

    /**
    * @brief Retrieves the names of all Replication Array actors within this Actor Component.
    *
    * @param CurrentReplicationArrayNames An array reference that will be populated with the names of the current Replication Arrays.
    * @return void
    * @note This function is marked as BlueprintPure, which means it does not change the state of the object and can be called without side effects in blueprints.
    */
    UFUNCTION(BlueprintCallable, BlueprintPure
        , meta = (ToolTip = "Gets all Replication Arrays Names."
            , ShortToolTip = "Get Rep Arrays Names", CompactNodeTitle = "Get Replication Arrays Names", Category = "BA Rep Array|Actor Component|Getter"))
    void GetReplicationArrayNames(TArray<FString>& CurrentReplicationArrayNames);
#pragma endregion

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

#pragma region Misc Private Functions
private:

    bool CheckArrayNameParameter(FString& ArrayName, bool NameShouldExist);

#pragma endregion

};
