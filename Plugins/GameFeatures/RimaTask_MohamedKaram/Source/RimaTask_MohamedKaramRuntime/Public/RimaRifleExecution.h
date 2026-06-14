// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RimaRifleExecution.generated.h"

/**
 * 
 */
UCLASS()
class RIMATASK_MOHAMEDKARAMRUNTIME_API URimaRifleExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

private:
	void Heal(FGameplayEffectCustomExecutionOutput& ExecutionOutput, AActor* ally, UAbilitySystemComponent* TargetASC) const;
	void Damage(FGameplayEffectCustomExecutionOutput& ExecutionOutput, AActor* enemy, UAbilitySystemComponent* TargetASC) const;
	void CheckNetworkMode(AActor* target) const;

public:
	UPROPERTY(EditAnywhere)
	float BaseDamage = 30.0f;

	UPROPERTY(EditAnywhere)
	float BaseHealing = 20.0f;


	URimaRifleExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& ExecutionOutput) const override;
};
