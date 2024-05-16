// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#pragma once

/**
 * Enum for different sources of the serialization object 
 */
UENUM(BlueprintType)
enum class EBA_EEntrySource : uint8 {
		E_Object			UMETA(DisplayName = "Source: UObject"),
		E_Struct			UMETA(DisplayName = "Source: UStruct"),
		E_UNDEFINED			UMETA(DisplayName = "UNDEFINED", Hidden)
	};