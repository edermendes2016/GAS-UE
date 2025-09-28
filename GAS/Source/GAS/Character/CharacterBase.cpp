// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSets/BasicAttributeSet.h"
#include "GAS/Attributes/ProgressionAttributeSet.h"
#include "GameplayTagContainer.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add Ability System Component
        AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

        BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
        ProgressionAttributeSet = CreateDefaultSubobject<UProgressionAttributeSet>(TEXT("ProgressionAttributeSet"));

        if (AbilitySystemComponent)
        {
                AbilitySystemComponent->SetIsReplicated(true);
                AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

                if (BasicAttributeSet)
                {
                        AbilitySystemComponent->AddAttributeSetSubobject(BasicAttributeSet);
                }

                if (ProgressionAttributeSet)
                {
                        AbilitySystemComponent->AddAttributeSetSubobject(ProgressionAttributeSet);
                }
        }

	//set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	
        // Attribute sets
}



void ACharacterBase::InitializeAbilitySystem()
{
	if (AbilitySystemComponent && !bHasInitializedAbilitySystem)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		bHasInitializedAbilitySystem = true;
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystem();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
        return AbilitySystemComponent;
}

void ACharacterBase::GrantExperience(float Amount)
{
        if (!HasAuthority())
        {
                return;
        }

        if (!AbilitySystemComponent || !GE_XPGainClass)
        {
                return;
        }

        const float ClampedAmount = FMath::Max(0.f, Amount);
        if (ClampedAmount <= 0.f)
        {
                return;
        }

        FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_XPGainClass, 1.f, ContextHandle);
        if (!SpecHandle.IsValid())
        {
                return;
        }

        FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
        if (!Spec)
        {
                return;
        }

        static const FGameplayTag DataXPTag = FGameplayTag::RequestGameplayTag(FName("Data.XP"));
        Spec->SetLevel(1.f);
        Spec->SetSetByCallerMagnitude(DataXPTag, ClampedAmount);
        AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
}

void ACharacterBase::ServerGrantExperience_Implementation(float Amount)
{
        GrantExperience(Amount);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
        Super::BeginPlay();

}