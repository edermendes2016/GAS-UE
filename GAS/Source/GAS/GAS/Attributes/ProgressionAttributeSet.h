#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ProgressionAttributeSet.generated.h"

#ifndef ATTR_ACCESSORS
#define ATTR_ACCESSORS(ClassName, PropertyName) \
        GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

UCLASS()
class GAS_API UProgressionAttributeSet : public UAttributeSet
{
        GENERATED_BODY()

public:
        UProgressionAttributeSet();

        // Replicated progression attributes
        UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_Level)
        FGameplayAttributeData Level;
        ATTR_ACCESSORS(UProgressionAttributeSet, Level);

        UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_Experience)
        FGameplayAttributeData Experience;
        ATTR_ACCESSORS(UProgressionAttributeSet, Experience);

        UPROPERTY(BlueprintReadOnly, Category = "Progression", ReplicatedUsing = OnRep_XPToNext)
        FGameplayAttributeData XPToNext;
        ATTR_ACCESSORS(UProgressionAttributeSet, XPToNext);

        virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

        virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

        virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
        UFUNCTION()
        void OnRep_Level(const FGameplayAttributeData& OldValue) const;

        UFUNCTION()
        void OnRep_Experience(const FGameplayAttributeData& OldValue) const;

        UFUNCTION()
        void OnRep_XPToNext(const FGameplayAttributeData& OldValue) const;

        float ClampLevelValue(float Value) const;

        float ClampXPToNextValue(float Value) const;

        float ClampExperienceValue(float Value) const;
};

