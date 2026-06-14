// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "RimaGameplayAbility_Grapple.generated.h"

/**
 * 
 */
UCLASS()
class RIMATASK_MOHAMEDKARAMRUNTIME_API URimaGameplayAbility_Grapple : public ULyraGameplayAbility
{
	GENERATED_BODY()
public:
    URimaGameplayAbility_Grapple();
protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // Configuration Properties
    UPROPERTY(EditDefaultsOnly, Category = "Grapple|Settings")
    float MaxGrappleDistance = 2500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Grapple|Settings")
    float TravelSpeed = 3000.0f;

    // Gameplay Cues for Cosmetics
    UPROPERTY(EditDefaultsOnly, Category = "Grapple|Cosmetics")
    FGameplayTag GrappleFireCueTag;

    UPROPERTY(EditDefaultsOnly, Category = "Grapple|Cosmetics")
    FGameplayTag GrappleRetractCueTag;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UFUNCTION()
    void OnGrappleMovementFinished();
};
