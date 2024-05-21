// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

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