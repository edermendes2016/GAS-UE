#include "GAS/GAS/ExecCalcs/ExecCalc_AddXP.h"

#include "AbilitySystemComponent.h"
#include "Curves/CurveFloat.h"
#include "GAS/GAS/Attributes/ProgressionAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"

namespace
{
        struct FProgressionCaptureDefinitions
        {
                DECLARE_ATTRIBUTE_CAPTUREDEF(Level);
                DECLARE_ATTRIBUTE_CAPTUREDEF(Experience);
                DECLARE_ATTRIBUTE_CAPTUREDEF(XPToNext);

                FProgressionCaptureDefinitions()
                {
                        DEFINE_ATTRIBUTE_CAPTUREDEF(UProgressionAttributeSet, Level, Target, false);
                        DEFINE_ATTRIBUTE_CAPTUREDEF(UProgressionAttributeSet, Experience, Target, false);
                        DEFINE_ATTRIBUTE_CAPTUREDEF(UProgressionAttributeSet, XPToNext, Target, false);
                }
        };

        const FProgressionCaptureDefinitions& GetProgressionCaptureDefinitions()
        {
                static const FProgressionCaptureDefinitions Statics;
                return Statics;
        }

        const FGameplayTag& GetXPSetByCallerTag()
        {
                static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Data.XP"));
                return Tag;
        }

        const FGameplayTag& GetLevelUpEventTag()
        {
                static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Event.LevelUp"));
                return Tag;
        }
}

UExecCalc_AddXP::UExecCalc_AddXP()
{
        const FProgressionCaptureDefinitions& Definitions = GetProgressionCaptureDefinitions();

        RelevantAttributesToCapture.Add(Definitions.LevelDef);
        RelevantAttributesToCapture.Add(Definitions.ExperienceDef);
        RelevantAttributesToCapture.Add(Definitions.XPToNextDef);
}

void UExecCalc_AddXP::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
        const FProgressionCaptureDefinitions& Definitions = GetProgressionCaptureDefinitions();

        const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
        const float IncomingXP = FMath::Max(0.f, Spec.GetSetByCallerMagnitude(GetXPSetByCallerTag(), false, 0.f));
        if (IncomingXP <= 0.f)
        {
                return;
        }

        FAggregatorEvaluateParameters EvaluateParameters;
        EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
        EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

        float CurrentLevelValue = 1.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Definitions.LevelDef, EvaluateParameters, CurrentLevelValue);
        CurrentLevelValue = FMath::Max(1.f, FMath::FloorToFloat(CurrentLevelValue));

        float CurrentXPValue = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Definitions.ExperienceDef, EvaluateParameters, CurrentXPValue);
        CurrentXPValue = FMath::Max(0.f, CurrentXPValue);

        float CurrentXPToNextValue = 1.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Definitions.XPToNextDef, EvaluateParameters, CurrentXPToNextValue);
        CurrentXPToNextValue = FMath::Max(1.f, CurrentXPToNextValue);

        const int32 StartingLevel = static_cast<int32>(CurrentLevelValue);
        const int32 ClampedMaxLevel = FMath::Max(1, MaxLevel);

        int32 WorkingLevel = FMath::Max(1, StartingLevel);
        float WorkingRequirement = CurrentXPToNextValue;
        float WorkingXP = CurrentXPValue + IncomingXP;
        int32 LevelsEarned = 0;

        while (WorkingXP >= WorkingRequirement && WorkingLevel < ClampedMaxLevel)
        {
                WorkingXP -= WorkingRequirement;
                ++WorkingLevel;
                ++LevelsEarned;
                WorkingRequirement = CalculateXPRequirement(WorkingLevel, StartingLevel, CurrentXPToNextValue);
        }

        WorkingRequirement = FMath::Max(1.f, WorkingRequirement);
        WorkingXP = FMath::Clamp(WorkingXP, 0.f, FMath::Max(0.f, WorkingRequirement - KINDA_SMALL_NUMBER));

        const int32 ResultLevel = FMath::Min(WorkingLevel, ClampedMaxLevel);
        const float LevelDelta = static_cast<float>(FMath::Max(0, ResultLevel - StartingLevel));
        const float ExperienceDelta = WorkingXP - CurrentXPValue;

        float TargetRequirement = WorkingRequirement;
        if (WorkingLevel != ResultLevel)
        {
                TargetRequirement = CalculateXPRequirement(ResultLevel, StartingLevel, CurrentXPToNextValue);
                TargetRequirement = FMath::Max(1.f, TargetRequirement);
                WorkingXP = FMath::Clamp(WorkingXP, 0.f, FMath::Max(0.f, TargetRequirement - KINDA_SMALL_NUMBER));
        }

        const float RequirementDelta = TargetRequirement - CurrentXPToNextValue;

        if (!FMath::IsNearlyZero(ExperienceDelta))
        {
                OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Definitions.ExperienceProperty, EGameplayModOp::Additive, ExperienceDelta));
        }

        if (!FMath::IsNearlyZero(LevelDelta))
        {
                OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Definitions.LevelProperty, EGameplayModOp::Additive, LevelDelta));
        }

        if (!FMath::IsNearlyZero(RequirementDelta))
        {
                OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Definitions.XPToNextProperty, EGameplayModOp::Additive, RequirementDelta));
        }

        if (LevelsEarned > 0)
        {
                if (UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent())
                {
                        FGameplayEventData EventData;
                        EventData.EventTag = GetLevelUpEventTag();
                        EventData.EventMagnitude = static_cast<float>(LevelsEarned);
                        EventData.Instigator = ExecutionParams.GetSourceAbilitySystemComponent() ? ExecutionParams.GetSourceAbilitySystemComponent()->GetAvatarActor() : nullptr;
                        EventData.Target = TargetASC->GetAvatarActor();
                        TargetASC->HandleGameplayEvent(GetLevelUpEventTag(), &EventData);
                }
        }
}

float UExecCalc_AddXP::CalculateXPRequirement(int32 TargetLevel, int32 StartingLevel, float StartingRequirement) const
{
        const int32 EffectiveLevel = FMath::Max(1, TargetLevel);

        if (XPRequirementCurve)
        {
                const float CurveValue = XPRequirementCurve->GetFloatValue(static_cast<float>(EffectiveLevel));
                if (CurveValue > 0.f)
                {
                        return CurveValue;
                }
        }

        const int32 LevelDelta = FMath::Max(0, EffectiveLevel - StartingLevel);
        return FMath::Max(1.f, StartingRequirement + LevelDelta * StartingRequirement);
}
