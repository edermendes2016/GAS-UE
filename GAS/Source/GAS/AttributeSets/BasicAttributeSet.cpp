// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


UBasicAttributeSet::UBasicAttributeSet()
{
	// Initialize attributes with default values
	Health = 50.f;
	MaxHealth = 100.f;
	Mana = 100.f;
	MaxMana = 100.f;
	Stamina = 100.f;
	MaxStamina = 100.f;	
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}


void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float ClampedHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(ClampedHealth);
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		const float ClampedMana = FMath::Clamp(GetMana(), 0.0f, GetMaxMana());
		SetMana(ClampedMana);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		const float ClampedStamina = FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina());
		SetStamina(ClampedStamina);
	}
}




