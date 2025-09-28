#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BasicAttributeSet.generated.h"

struct FGameplayEffectModCallbackData;

#ifndef ATTRIBUTE_ACCESSORS_BASIC
#define ATTRIBUTE_ACCESSORS_BASIC(ClassName, PropertyName) \
        GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS()
class GAS_API UBasicAttributeSet : public UAttributeSet
{
        GENERATED_BODY()

public:
        UBasicAttributeSet();

        //Health
        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
        FGameplayAttributeData Health;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Health);

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
        FGameplayAttributeData MaxHealth;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxHealth);

        // Mana
        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mana)
        FGameplayAttributeData Mana;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Mana);

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMana)
        FGameplayAttributeData MaxMana;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxMana);

        //Stamina
        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Stamina)
        FGameplayAttributeData Stamina;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, Stamina);

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxStamina)
        FGameplayAttributeData MaxStamina;
        ATTRIBUTE_ACCESSORS_BASIC(UBasicAttributeSet, MaxStamina);

public:
        //Funtions OnRep
        UFUNCTION()
        void OnRep_Health(const FGameplayAttributeData& OldHealth) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Health, OldHealth);
        }

        UFUNCTION()
        void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxHealth, OldMaxHealth);
        }

        virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
        virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

        UFUNCTION()
        void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Stamina, OldStamina);
        }

        UFUNCTION()
        void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxStamina, OldMaxStamina);
        }

        UFUNCTION()
        void OnRep_Mana(const FGameplayAttributeData& OldMana) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, Mana, OldMana);
        }

        UFUNCTION()
        void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
        {
                GAMEPLAYATTRIBUTE_REPNOTIFY(UBasicAttributeSet, MaxMana, OldMaxMana);
        }

        // Override GetLifetimeReplicatedProps
        virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
        void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
        void ClampAttribute(FGameplayAttributeData& AttributeData, float MinValue, float MaxValue, const FGameplayAttribute& AttributeProperty);
};
