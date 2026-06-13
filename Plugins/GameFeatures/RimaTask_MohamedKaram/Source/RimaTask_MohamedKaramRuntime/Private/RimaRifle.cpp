#include "RimaRifle.h"

URimaRifle::URimaRifle()
{
}

void URimaRifle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    // Always call Super first so Lyra can handle weapon and equipment tracking natively
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    
   // Your custom C++ weapon logic goes here!
    UE_LOG(LogTemp, Log, TEXT("Custom C++ Weapon Ability successfully activated!"));

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}