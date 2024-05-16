// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Compression/CompressedBuffer.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"
#include "Logging/StructuredLog.h"
#include "Misc/Parse.h"
#include "PropertyPath.h"

class BA_REPARRAY_API BA_Statics
{

public:

#pragma region Reflection
    // Function to create FPropertyInfo from a property name
    static FPropertyInfo CreateFPropertyInfoFromName(UObject* ObjectPtr, const FName& PropertyName, bool& Result)
    {
        Result = false;
        // Ensure the ObjectPtr object is valid
        if (!ObjectPtr)
        {
            return FPropertyInfo();
        }

        // Find the property using the property name
        FProperty* Property = ObjectPtr->GetClass()->FindPropertyByName(PropertyName);
        if (!Property)
        {
            return FPropertyInfo();
        }

        // Create FPropertyInfo from the found property
        FPropertyInfo PropertyInfo(Property, INDEX_NONE); // Assuming the property is not an array
        Result = true;
        return PropertyInfo;
    }

    static void StorePropertiesNameAndType(UObject* ObjectPtr, TMap<FString, FString>& PropertyMap)
    {
        PropertyMap.Empty();
        if (!ObjectPtr)
        {
            return;
        }
        // Iterate over all properties of the object
        for (TFieldIterator<FProperty> PropIt(ObjectPtr->GetClass()); PropIt; ++PropIt)
        {
            FString PropertyName = PropIt->GetName();
            FString PropertyType = PropIt->GetCPPType();
            PropertyMap.Add(MoveTemp(PropertyName), MoveTemp(PropertyType));
        }
    }

    static FString GetClassNameFromObject(UObject* ObjectPtr, bool& Result)
    {
        Result = false;
        if (ObjectPtr != nullptr)
        {
            Result = true;
            return ObjectPtr->GetClass()->GetName();
        }
        return "";
    }

    template<typename T>
    static T* CastToObjectFromString(FString ClassName, UObject* Object, bool& Result)
    {
        Result = false;
        UClass* Class = FindObject<UClass>(nullptr, *ClassName);
        if (Class != nullptr && Object->IsA(Class))
        {
            Result = true;
            return Cast<T>(Object);
        }
        return nullptr;
    }

    static FString GetObjectPropertiesAsString(UObject* Object)
    {
        FString Result;

        // Check if the object is valid
        if (!Object)
        {
            return Result;
        }

        // Iterate over all properties of the object
        for (TFieldIterator<FProperty> PropIt(Object->GetClass()); PropIt; ++PropIt)
        {
            FProperty* Prop = *PropIt;

            FString PropName = Prop->GetName();
            FString PropValue;

            if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
            {
                PropValue = StrProp->GetPropertyValue_InContainer(Object);
            }
            else if (FIntProperty* IntProp = CastField<FIntProperty>(Prop))
            {
                PropValue = FString::Printf(TEXT("%d"), IntProp->GetPropertyValue_InContainer(Object));
            }
            // Add more else if blocks here for other property types you are interested in

            Result += PropName + ": " + PropValue + "\t";
        }

        return Result;
    }
#pragma endregion

#pragma region Outcommented compression - buggy
    //static TArray<uint8> SerializeObjectIntoUnit8Array(UObject* ObjectToSerialize, int32& UncompressedSize)
//{
//    if (ObjectToSerialize != nullptr)
//    {
//        // Create a binary archive to serialize data into a byte array
//        FBufferArchive BinaryData;

//        // Wrap the binary archive with FObjectAndNameAsStringProxyArchive
//        FObjectAndNameAsStringProxyArchive Ar(BinaryData, true);

//        // Serialize the UObject
//        ObjectToSerialize->Serialize(Ar); 

//        // Convert the binary data to a base64 string for storage
//        FString SerializedObjectData;
//        SerializedObjectData = FBase64::Encode(BinaryData);

//        auto& CharArray = SerializedObjectData.GetCharArray();
//        UncompressedSize = CharArray.Num();

//        FName CompressionFormatToUse = NAME_LZ4;
//        
//        TArray<uint8> CompressedMemory;
//        int32 CompressedSizeEstimate = FCompression::CompressMemoryBound(CompressionFormatToUse, CharArray.Num());
//        CompressedMemory.AddDefaulted(CompressedSizeEstimate);

//        const bool bCompressed = FCompression::CompressMemory(
//            CompressionFormatToUse,
//            CompressedMemory.GetData(),
//            CompressedSizeEstimate,
//            CharArray.GetData(),
//            CharArray.Num());
//        // 
//        // if the compression results are worse, just store uncompressed data
//        /*if (!bCompressed || CompressedSizeEstimate >= CharArray.Num())
//        {
//            return CharArray;
//        }
//        else
//        {
//            return CompressedMemory;
//        }*/
//        return CompressedMemory;
//    }
//    return TArray<uint8>();
//}

//static UObject* DeserializeObjectFromUint8Array(const TArray<uint8>& CompressedData, int32 UncompressedSize, UObject* Outer)
//{
//    if (CompressedData.Num() > 0)
//    {
//        // compression format
//        FName CompressionFormatToUse = NAME_LZ4;
//        
//        //FString::ToHexBlob(ToProcess->HexData, CompressedData.GetData(), ToProcess->CompressedSize);

//        TArray<uint8> DestData; 
//        DestData.Reset(UncompressedSize);
//        DestData.AddUninitialized(UncompressedSize);

//        // Reader.Serialize(SrcData.GetData(), CompressedSize);
//        bool bDecompressed = FCompression::UncompressMemory(
//            NAME_Zlib, 
//            DestData.GetData(), 
//            UncompressedSize, 
//            CompressedData.GetData(), 
//            CompressedData.Num());
//        
//        if (bDecompressed)
//        {
//            FString Base64Data = FString::FromBlob(DestData.GetData(), DestData.Num());
//            TArray<uint8> BinaryData;

//            FBase64::Decode(Base64Data, BinaryData);
//            FMemoryReader Reader(BinaryData, true);

//            FObjectAndNameAsStringProxyArchive Ar(Reader, true);
//            UObject* DeserializedObject = NewObject<UObject>(Outer);
//            if (DeserializedObject)
//            {
//                DeserializedObject->Serialize(Ar);
//                return DeserializedObject;
//            }
//        }
//    }

//    return nullptr;
//}

#pragma endregion

    static FString SerializeObject(UObject* StorageObject)
    {
        if (StorageObject)
        {
            FBufferArchive BinaryData;
            FObjectAndNameAsStringProxyArchive Ar(BinaryData, true);
            StorageObject->Serialize(Ar);
            return FBase64::Encode(BinaryData);
        }
        return TEXT("");
    }

    static UObject* DeserializeObjectFromString(const FString& SerializedObj, UObject* Outer, UClass* CastToClass)
    {
        if (!SerializedObj.IsEmpty())
        {
            TArray<uint8> BinaryData;

            if (FBase64::Decode(SerializedObj, BinaryData))
            {
                FMemoryReader Reader(BinaryData, true);
                FObjectAndNameAsStringProxyArchive Ar(Reader, true);
                UObject* DeserializedObject = NULL;
                if (CastToClass)
                {
                    if (DeserializedObject = NewObject<UObject>(Outer, CastToClass);
                        DeserializedObject)
                    {
                        DeserializedObject->Serialize(Ar);
                        return DeserializedObject;
                    }
                }
            }
        }
        return nullptr;
    }
};
