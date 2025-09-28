#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_AddXP.generated.h"

class UAbilitySystemComponent;
class UCurveFloat;

/**
 * Execution calculation responsible for applying experience gains and handling level ups.
 */
UCLASS()
class GAS_API UExecCalc_AddXP : public UGameplayEffectExecutionCalculation
{
        GENERATED_BODY()

public:
        UExecCalc_AddXP();

        virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
        /** Curve mapping Level -> XP required for the next level. */
        UPROPERTY(EditDefaultsOnly, Category = "Progression")
        TObjectPtr<UCurveFloat> XPRequirementCurve = nullptr;

        /** Highest level allowed through this execution. */
        UPROPERTY(EditDefaultsOnly, Category = "Progression", meta = (ClampMin = "1"))
        int32 MaxLevel = 100;

        float CalculateXPRequirement(int32 TargetLevel, int32 StartingLevel, float StartingRequirement) const;
};
