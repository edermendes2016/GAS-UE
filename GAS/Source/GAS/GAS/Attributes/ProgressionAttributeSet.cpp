#include "GAS/GAS/Attributes/ProgressionAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UProgressionAttributeSet::UProgressionAttributeSet()
{
        Level = 1.f;
        Experience = 0.f;
        XPToNext = 1.f;
}

void UProgressionAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);

        DOREPLIFETIME_CONDITION_NOTIFY(UProgressionAttributeSet, Level, COND_None, REPNOTIFY_Always);
        DOREPLIFETIME_CONDITION_NOTIFY(UProgressionAttributeSet, Experience, COND_None, REPNOTIFY_Always);
        DOREPLIFETIME_CONDITION_NOTIFY(UProgressionAttributeSet, XPToNext, COND_None, REPNOTIFY_Always);
}

void UProgressionAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
        Super::PreAttributeChange(Attribute, NewValue);

        if (Attribute == GetLevelAttribute())
        {
                NewValue = ClampLevelValue(NewValue);
        }
        else if (Attribute == GetXPToNextAttribute())
        {
                NewValue = ClampXPToNextValue(NewValue);
        }
}

void UProgressionAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
        Super::PostGameplayEffectExecute(Data);

        if (Data.EvaluatedData.Attribute == GetLevelAttribute())
        {
                SetLevel(ClampLevelValue(GetLevel()));
        }
        else if (Data.EvaluatedData.Attribute == GetXPToNextAttribute())
        {
                SetXPToNext(ClampXPToNextValue(GetXPToNext()));
        }
        else if (Data.EvaluatedData.Attribute == GetExperienceAttribute())
        {
                SetExperience(ClampExperienceValue(GetExperience()));
        }

        // Ensure interdependent attributes respect their clamps.
        SetLevel(ClampLevelValue(GetLevel()));
        SetXPToNext(ClampXPToNextValue(GetXPToNext()));
        SetExperience(ClampExperienceValue(GetExperience()));
}

void UProgressionAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue) const
{
        GAMEPLAYATTRIBUTE_REPNOTIFY(UProgressionAttributeSet, Level, OldValue);
}

void UProgressionAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue) const
{
        GAMEPLAYATTRIBUTE_REPNOTIFY(UProgressionAttributeSet, Experience, OldValue);
}

void UProgressionAttributeSet::OnRep_XPToNext(const FGameplayAttributeData& OldValue) const
{
        GAMEPLAYATTRIBUTE_REPNOTIFY(UProgressionAttributeSet, XPToNext, OldValue);
}

float UProgressionAttributeSet::ClampLevelValue(float Value) const
{
        return FMath::Max(1.f, FMath::FloorToFloat(Value));
}

float UProgressionAttributeSet::ClampXPToNextValue(float Value) const
{
        return FMath::Max(1.f, Value);
}

float UProgressionAttributeSet::ClampExperienceValue(float Value) const
{
        const float Requirement = ClampXPToNextValue(GetXPToNext());
        const float UpperBound = Requirement > 0.f ? FMath::Max(0.f, Requirement - KINDA_SMALL_NUMBER) : 0.f;
        return FMath::Clamp(Value, 0.f, UpperBound);
}
