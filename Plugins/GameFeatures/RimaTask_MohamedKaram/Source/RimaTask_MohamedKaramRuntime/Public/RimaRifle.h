#pragma once

#include "CoreMinimal.h"
#include "Weapons/LyraGameplayAbility_RangedWeapon.h" // Use LyraGameplayAbility_FromEquipment.h if not a gun
#include "RimaRifle.generated.h"

UCLASS()
class RIMATASK_MOHAMEDKARAMRUNTIME_API URimaRifle : public ULyraGameplayAbility_RangedWeapon
{
    GENERATED_BODY()

public:
    URimaRifle();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};