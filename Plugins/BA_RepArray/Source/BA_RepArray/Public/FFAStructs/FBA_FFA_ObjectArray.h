// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

#pragma once
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "FFAStructs/FBA_FFA_Object.h"
#include "FBA_FFA_ObjectArray.generated.h"

DECLARE_DELEGATE_OneParam(FEntryChange, FBA_FFA_Object /* Entry */)
DECLARE_DELEGATE_OneParam(FArrayCountChange, int32 /* Entry */)

USTRUCT(BlueprintType)
struct BA_REPARRAY_API FBA_FFA_ObjectArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FBA_FFA_ObjectArray();
	FBA_FFA_ObjectArray(AActor* Owner) { this->Owner = Owner; }

	~FBA_FFA_ObjectArray();

	friend class ABA_ReplicationInfo;

public:

#pragma region Networking & Replication
#pragma region FFastArraySerializer
	// Called after updating all existing elements with new data and after the elements themselves are notified. 
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	// Called before removing elements and after the elements themselves are notified.
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	// Called after adding all new elements and after the elements themselves are notified.
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	// It will be called after each call to NetDeltaSerialize on the receiving end, including if we have mapped some unmapped objects
	void PostReplicatedReceive(const FBA_FFA_ObjectArray::FPostReplicatedReceiveParameters& Parameters);
#pragma endregion

#pragma region Networking
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
	bool ReplicateFFAObjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);
#pragma endregion  
#pragma endregion

	bool AddEntry(UObject* StorageObject, FGuid InstanceGuid, FString ReadableIdentifier);
	bool CheckForSubobjectListSupport(FBA_FFA_Object& Entry);
	void ForEachChildren(const TFunctionRef<void(FBA_FFA_Object)>& Func);
	bool RemoveEntry(FGuid InstanceGuid, UObject*& DeletedEntry);
	bool GetEntryByGuid(FGuid Guid, FBA_FFA_Object& ResultEntry);
	bool GetEntryByIdentifier(FString Identifier, FBA_FFA_Object& ResultEntry);
	void Clear();
	void SortByIndex();
	void SortByPropertyName(const FString PropertyName, TArray<FString> SortableTypesArray);
private:

	UPROPERTY()
	TArray<FBA_FFA_Object> Items;

	UPROPERTY(NotReplicated)
	TMap<FGuid, int32> GuidToArrayPos;

	UPROPERTY(NotReplicated)
	TMap<FString, int32> IdentifierToArrayPos;

	UPROPERTY(NotReplicated, Transient)
	TObjectPtr<AActor> Owner;

	UPROPERTY(NotReplicated, Transient)
	TMap<FString, FString> EntryObjectsPropertyMap;
	
	FEntryChange OnEntryPreReplicatedRemove;
	FEntryChange OnEntryPostReplicatedAdd;
	FEntryChange OnEntryPostReplicatedChange;
	FArrayCountChange OnEntryPostReplicatedReceive;
};

template<>
struct TStructOpsTypeTraits< FBA_FFA_ObjectArray > : public TStructOpsTypeTraitsBase2< FBA_FFA_ObjectArray >
{	
	enum
	{
		WithNetDeltaSerializer = true,
	};
};