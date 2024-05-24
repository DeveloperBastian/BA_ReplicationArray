# BA_ReplicationArray
Unreal Replicated Array exposed to Blueprint


## Features
An Blueprint exposed Array (using [FFastArraySerializer](https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Engine/FFastArraySerializer/)) that accepts every Object and will replicate it between server and client.
Features:
-	Incremental replication on all operations: This minimizes the data between server and client and makes replication much faster
-	Automatic statistics for all properties stores: All Object properties will be automatically calculated with sum, mean, last/first value, min/max. This will make access to e.g. a total weight of all items lightning fast without any calculations over the full array required
-	Supports unlimited BA Replication Arrays per actor: Adds a ActorComponent that allowe easy initialization and deletion of these replicated arrays
-	Tracks numeric properties across Objects. You can store very different Objects within the same array. It will track all properties with the same name - so if all your Objects have a property "Weight", the it will calculate the above statistics across all properties (using relection)
-	Fast and stable - developed purely in C++, but fully exposed to Blueprint for all features
-	Extensive documentation and examples
-	The Replication Arrays are implementations of the 'FFastArraySerializer' struct class with entries of 'FFastArraySerializerItem' with all important functionality exposed to Blueprint.

## Links
-	Docs: https://developerbastian.tech/docs/category/ba-inventory-manager
-	Github: https://github.com/DeveloperBastian/BA_ReplicationArray

## Video ##

[Youtube tutorial en](https://youtu.be/rrGx6bwwM6U)

[Youtube tutorial de](https://youtu.be/pqe5AiD4tO8)