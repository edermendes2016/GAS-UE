// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CharacterBase.generated.h"


class UBasicAttributeSet;
class UProgressionAttributeSet;
class UGameplayEffect;

UCLASS()
class GAS_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
        GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute Sets
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
        UBasicAttributeSet* BasicAttributeSet;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
        UProgressionAttributeSet* ProgressionAttributeSet;

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS|Progression")
        TSubclassOf<UGameplayEffect> GE_XPGainClass;

        UFUNCTION(BlueprintCallable, Category = "GAS|Progression")
        void GrantExperience(float Amount);

        UFUNCTION(Server, Reliable)
        void ServerGrantExperience(float Amount);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	bool bHasInitializedAbilitySystem = false;

	void InitializeAbilitySystem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
