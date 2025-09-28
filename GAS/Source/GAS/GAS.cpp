#include "GAS.h"
#include "Modules/ModuleManager.h"
#include "HAL/IConsoleManager.h"
#include "Engine/World.h"
#include "GAS/Character/CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

namespace GASConsoleCommands
{
        static void GrantXPCommandHandler(const TArray<FString>& Args, UWorld* World)
        {
                if (!World)
                {
                        return;
                }

                if (Args.Num() == 0)
                {
                        UE_LOG(LogTemp, Warning, TEXT("Usage: GAS.GrantXP <Amount>"));
                        return;
                }

                const float Amount = FCString::Atof(*Args[0]);
                if (Amount <= 0.f)
                {
                        UE_LOG(LogTemp, Warning, TEXT("GAS.GrantXP requires a positive amount."));
                        return;
                }

                for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
                {
                        APlayerController* PlayerController = It->Get();
                        if (!PlayerController)
                        {
                                continue;
                        }

                        APawn* Pawn = PlayerController->GetPawn();
                        if (ACharacterBase* Character = Cast<ACharacterBase>(Pawn))
                        {
                                if (Character->HasAuthority())
                                {
                                        Character->GrantExperience(Amount);
                                }
                                else
                                {
                                        Character->ServerGrantExperience(Amount);
                                }
                        }
                }
        }

        static FAutoConsoleCommandWithWorldAndArgs GrantXPConsoleCommand(
                TEXT("GAS.GrantXP"),
                TEXT("Grants experience to all possessed GAS characters. Usage: GAS.GrantXP <Amount>"),
                FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&GrantXPCommandHandler));
}

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, GAS, "GAS" );
