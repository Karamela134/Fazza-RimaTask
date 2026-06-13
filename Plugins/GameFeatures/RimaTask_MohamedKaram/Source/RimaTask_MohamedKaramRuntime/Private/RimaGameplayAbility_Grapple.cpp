#include "RimaGameplayAbility_Grapple.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h" // The real header!
#include "AbilitySystemComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"

URimaGameplayAbility_Grapple::URimaGameplayAbility_Grapple()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void URimaGameplayAbility_Grapple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    APlayerController* PC = ActorInfo->PlayerController.Get();
    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (!PC || !Avatar)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 1. Calculate Aim Trajectory
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * MaxGrappleDistance);

    FCollisionQueryParams TraceParams(FName(TEXT("GrappleTrace")), true, Avatar);
    FHitResult HitResult;

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

    // 2. ALWAYS Trigger Fire Visuals (Predictive execution)
    if (GrappleFireCueTag.IsValid())
    {
        FGameplayEffectContextHandle Context = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        Context.AddHitResult(HitResult); // Pass the hit point down for visual tracer rendering
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GrappleFireCueTag, Context);
    }

    // 3. Process Logic Branching
    if (bHit && HitResult.GetActor())
    {
        FVector TargetLocation = HitResult.ImpactPoint;
        TargetLocation += (HitResult.ImpactNormal * 40.0f); // Offset to prevent mesh clipping

        // Dynamic Duration Calculation to maintain a constant travel speed
        float Distance = FVector::Distance(Avatar->GetActorLocation(), TargetLocation);
        float CalculatedDuration = (TravelSpeed > 0.0f) ? (Distance / TravelSpeed) : 0.5f;

        UAbilityTask_ApplyRootMotionMoveToForce* MoveTask =
            UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
                this,
                FName("GrapplePullTask"),
                TargetLocation,
                CalculatedDuration,
                true,                                                          // bSetNewMovementMode -> CHANGE TO TRUE
                MOVE_Flying,                                                   // NewMovementMode -> CHANGE TO MOVE_Flying
                false,                                                         // bRestrictSpeedToExpected
                nullptr,                                                       // PathOffsetCurve
                ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, // FinishVelocityMode
                FVector::ZeroVector,                                           // FinishSetVelocity
                0.0f                                                           // FinishClampVelocity
            );
        if (MoveTask)
        {
            // Change .OnFinished to .OnTimedOutAndMoveFinished
            MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &URimaGameplayAbility_Grapple::OnGrappleMovementFinished);

            MoveTask->ReadyForActivation();
        }
        else
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        }
    }
    else
    {
        // Trace missed: Trigger quick retract sequence and close the ability out
        if (GrappleRetractCueTag.IsValid())
        {
            FGameplayEffectContextHandle Context = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GrappleRetractCueTag, Context);
        }

        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void URimaGameplayAbility_Grapple::OnGrappleMovementFinished()
{
    // Ensure bReplicateEndAbility is true so both sides match closure states
    bool bReplicateEndAbility = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, false);
}