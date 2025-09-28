#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

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

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
        Super::PreAttributeChange(Attribute, NewValue);

        if (Attribute == GetMaxHealthAttribute())
        {
                const float ClampedMax = FMath::Max(1.f, NewValue);
                AdjustAttributeForMaxChange(Health, MaxHealth, ClampedMax, GetHealthAttribute());
                NewValue = ClampedMax;
        }
        else if (Attribute == GetMaxManaAttribute())
        {
                const float ClampedMax = FMath::Max(0.f, NewValue);
                AdjustAttributeForMaxChange(Mana, MaxMana, ClampedMax, GetManaAttribute());
                NewValue = ClampedMax;
        }
        else if (Attribute == GetMaxStaminaAttribute())
        {
                const float ClampedMax = FMath::Max(0.f, NewValue);
                AdjustAttributeForMaxChange(Stamina, MaxStamina, ClampedMax, GetStaminaAttribute());
                NewValue = ClampedMax;
        }
        else if (Attribute == GetHealthAttribute())
        {
                NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(GetMaxHealth(), 0.f));
        }
        else if (Attribute == GetManaAttribute())
        {
                NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(GetMaxMana(), 0.f));
        }
        else if (Attribute == GetStaminaAttribute())
        {
                NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(GetMaxStamina(), 0.f));
        }
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
        Super::PostGameplayEffectExecute(Data);

        if (Data.EvaluatedData.Attribute == GetHealthAttribute())
        {
                ClampAttribute(Health, 0.f, FMath::Max(GetMaxHealth(), 0.f), GetHealthAttribute());
        }
        else if (Data.EvaluatedData.Attribute == GetManaAttribute())
        {
                ClampAttribute(Mana, 0.f, FMath::Max(GetMaxMana(), 0.f), GetManaAttribute());
        }
        else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
        {
                ClampAttribute(Stamina, 0.f, FMath::Max(GetMaxStamina(), 0.f), GetStaminaAttribute());
        }
        else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
        {
                const float ClampedMax = FMath::Max(1.f, GetMaxHealth());
                SetMaxHealth(ClampedMax);
                AdjustAttributeForMaxChange(Health, MaxHealth, ClampedMax, GetHealthAttribute());
        }
        else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
        {
                const float ClampedMax = FMath::Max(0.f, GetMaxMana());
                SetMaxMana(ClampedMax);
                AdjustAttributeForMaxChange(Mana, MaxMana, ClampedMax, GetManaAttribute());
        }
        else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
        {
                const float ClampedMax = FMath::Max(0.f, GetMaxStamina());
                SetMaxStamina(ClampedMax);
                AdjustAttributeForMaxChange(Stamina, MaxStamina, ClampedMax, GetStaminaAttribute());
        }
}

void UBasicAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
        const float EffectiveMax = FMath::Max(0.f, NewMaxValue);
        ClampAttribute(AffectedAttribute, 0.f, EffectiveMax, AffectedAttributeProperty);
}

void UBasicAttributeSet::ClampAttribute(FGameplayAttributeData& AttributeData, float MinValue, float MaxValue, const FGameplayAttribute& AttributeProperty)
{
        const float EffectiveMax = FMath::Max(MinValue, MaxValue);
        const float CurrentValue = AttributeData.GetCurrentValue();
        const float ClampedCurrentValue = FMath::Clamp(CurrentValue, MinValue, EffectiveMax);

        if (!FMath::IsNearlyEqual(CurrentValue, ClampedCurrentValue))
        {
                if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
                {
                        ASC->ApplyModToAttributeUnsafe(AttributeProperty, EGameplayModOp::Additive, ClampedCurrentValue - CurrentValue);
                }
                else
                {
                        AttributeData.SetCurrentValue(ClampedCurrentValue);
                }
        }

        const float BaseValue = AttributeData.GetBaseValue();
        const float ClampedBaseValue = FMath::Clamp(BaseValue, MinValue, EffectiveMax);
        if (!FMath::IsNearlyEqual(BaseValue, ClampedBaseValue))
        {
                AttributeData.SetBaseValue(ClampedBaseValue);
        }
}
