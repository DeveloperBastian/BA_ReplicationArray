// Developer Bastian © 2024
// License Creative Commons DEED 4.0 (https://creativecommons.org/licenses/by-sa/4.0/deed.en)

#pragma once
#include "UObject/Object.h"
#include "Templates/TypeHash.h"
#include "BA_FStatistics.generated.h"

/**
* Struct to store some descriptive statistics about an entry
*/
USTRUCT()
struct FBA_FStatistics
{
	GENERATED_BODY()

#pragma region Constructor - Initialize all values

public:
	// default constructor
	FBA_FStatistics() = default;
	FBA_FStatistics(FName NameOfProperty, FName TypeOfProperty) 
		: PropertyName(NameOfProperty), PropertyTypeName(TypeOfProperty) { }
	
	friend class ABA_ReplicationInfo;

#pragma endregion

	FName GetPropertyName()	{ return PropertyName; }
	FName GetPropertyType()	{ return PropertyTypeName; }
	int64 GetCount()			{ return Count; }
	double GetLastValue()		{ return LastValue; }
	double GetFirstValue()		{ return FirstValue; }
	double GetMean()			{ return Mean; }
	
	FString ToString()
	{
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumFractionalDigits = 2;
		NumberFormat.MaximumFractionalDigits = 2;

		FString Result = "'" + PropertyName.ToString() + "' [" + PropertyTypeName.ToString()
			+ "]: Count " + FString::FromInt(Count)
			+ ", Sum " + FText::AsNumber(Sum, &NumberFormat).ToString()
			+ ", LastValue " + FText::AsNumber(LastValue, &NumberFormat).ToString()
			+ ", FirstValue " + FText::AsNumber(FirstValue, &NumberFormat).ToString()
			+ ", Mean " + FText::AsNumber(Mean, &NumberFormat).ToString()
			+ ", Rang " + FText::AsNumber(Rang, &NumberFormat).ToString()
			+ ", Min" + FText::AsNumber(Min, &NumberFormat).ToString()
			+ ", Max " + FText::AsNumber(Max, &NumberFormat).ToString()
			+ ", Updated " + LastUpdate.ToString();
			
		return Result;
	}
	
#pragma region Templates
	template <typename T>
	struct IsNumeric
	{
		static constexpr bool value = std::is_arithmetic_v<T>;
	};

	template <typename T>
	std::enable_if_t<IsNumeric<T>::value> AddValue(T TValue)
	{
		double Value = static_cast<double>(TValue);
		Count++;
		if (Count == 1)
		{
			FirstValue = Value;
		}
		LastValue = Value;
		LastUpdate = FDateTime::UtcNow();
		if (Value < Min)
		{
			LastMin = Min;
			Min = Value;
		}
		if (Value > Max)
		{
			LastMax = Max;
			Max = Value;
		}
		Rang = Max - Min;
		Mean = ((Mean * (Count - 1)) + Value) / Count;
		Sum += Value;
	}

	template <typename T>
	std::enable_if_t<IsNumeric<T>::value> RemoveValue(T TValue)
	{
		double Value = static_cast<double>(TValue);
		Count--;
		if (Count == 0)
		{
			ResetValues();
			return;
		}
		LastValue = Value;
		LastUpdate = FDateTime::UtcNow();
		if (Value == Min)
		{
			Min = LastMin;
			LastMin = 0;
		}
		if (Value == Max)
		{
			Max = LastMax;
			LastMax = 0;
		}
		Rang = Max - Min;
		Mean = ((Mean * Count - Value) / (Count - 1));
		Sum -= Value;
	}
#pragma endregion

#pragma region Helper functions

public:

#pragma region Sorting
	/**
* Struct will need a Hash function to be effectively usable e.g. in TMap as key or in TSet
*/
	friend uint32 GetTypeHash(const FBA_FStatistics& Other)
	{
		return GetTypeHash(Other.PropertyName.ToString());
	}

	/**
	* Override the comparison operator for sorting in C++
	*/
	FORCEINLINE bool operator==(const FBA_FStatistics& Other) const
	{
		return GetTypeHash(PropertyName.ToString()) == GetTypeHash(Other.PropertyName.ToString());
	}

	/**
	* We use this Equals function, to implement an override of the == operator, associated with our custom struct
	* This will enable any code, using the == operator, to compare two instances of the struct.
	*/
	FORCEINLINE bool Equals(const FBA_FStatistics& Other) const
	{
		return GetTypeHash(PropertyName.ToString()) == GetTypeHash(Other.PropertyName.ToString());
	}
#pragma endregion

	void ResetValues()
	{
		// reset
		FirstValue = 0;
		LastValue = 0;
		LastMax = 0;
		LastMin = 0;
		Min = 0;
		Max = 0;
		LastMin = 0;
		LastMax = 0;
		Mean = 0;
		Sum = 0;
		Rang = 0;
		return;
	}

#pragma endregion

#pragma region Variables

private:
	UPROPERTY()
	FName PropertyName;

	UPROPERTY()
	FName PropertyTypeName;

	UPROPERTY()
	int64 Count = 0;

	UPROPERTY()
	double LastValue = 0;

	UPROPERTY()
	double FirstValue = 0;

	UPROPERTY()
	double Mean = 0;

	UPROPERTY()
	double Sum = 0;

	UPROPERTY()
	double Rang = 0;

	UPROPERTY()
	double Min = 0;

	UPROPERTY()
	double LastMin = 0;

	UPROPERTY()
	double Max = 0;

	UPROPERTY()
	double LastMax = 0;

	UPROPERTY()
	FDateTime LastUpdate = FDateTime::MinValue();

#pragma endregion

};
