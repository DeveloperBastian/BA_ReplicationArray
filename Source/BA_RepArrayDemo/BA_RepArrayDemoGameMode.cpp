// Copyright Developer Bastian 2024. Contact developer.bastian@gmail.com. All Rights Reserved.

#include "BA_RepArrayDemoGameMode.h"
#include "BA_RepArrayDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABA_RepArrayDemoGameMode::ABA_RepArrayDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
