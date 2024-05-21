// Copyright Developer Bastian 2024. Contact: developer.bastian@gmail.com or https://discord.gg/8JStx9XZGP. License Creative Commons 4.0 DEED (https://creativecommons.org/licenses/by/4.0/deed.en).

#pragma once

/**
 * Enum for different stati of the current entry
 */

UENUM(BlueprintType)
enum class EBA_EEntryStatus : uint8 {
		E_Filtered_Visible		UMETA(DisplayName = "Filtered: Visible"),
		E_Filtered_InVisible	UMETA(DisplayName = "Filtered: InVisible"),
		E_NotConfirmedAdded		UMETA(DisplayName = "Not Confirmed: Added"),
		E_NotConfirmedChanged	UMETA(DisplayName = "Not Confirmed: Changed"),
		E_NotConfirmedDeleted	UMETA(DisplayName = "Not Confirmed: Removed"),
		E_UNDEFINED	UMETA(DisplayName = "UNDEFINED", Hidden)
	};

