#include "RimaGameplayAbility_Grapple.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h" 
#include "AbilitySystemComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

URimaGameplayAbility_Grapple::URimaGameplayAbility_Grapple()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void URimaGameplayAbility_Grapple::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogTemp, Log, TEXT("Grapple not commit abiltiy"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    APlayerController* PC = ActorInfo->PlayerController.Get();
    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (!PC || !Avatar)
    {
        UE_LOG(LogTemp, Log, TEXT("Grapple Pc or avatar null"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * MaxGrappleDistance);

    DrawDebugLine(
        GetWorld(),
        TraceStart,
        TraceEnd,
        FColor::Cyan,
        false,
        5.0f,
        0,
        2.0f
    );

    FCollisionQueryParams TraceParams(FName(TEXT("GrappleTrace")), true, Avatar);
    FHitResult HitResult;

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

    if (GrappleFireCueTag.IsValid())
    {
        FGameplayEffectContextHandle Context = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        Context.AddHitResult(HitResult); 
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GrappleFireCueTag, Context);
    }

    if (bHit && HitResult.GetActor())
    {
        FVector TargetLocation = HitResult.ImpactPoint;

        // Default fallback offsets
        float SafetyBuffer = 10.0f;
        float CoreOffset = 40.0f;

        // Query the actual capsule size to prevent collision crunch
        if (ACharacter* Character = Cast<ACharacter>(Avatar))
        {
            if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
            {
                float Radius = Capsule->GetScaledCapsuleRadius();
                float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

                // Smoothly interpolate between Radius and HalfHeight depending on the surface angle.
                // If hitting a flat floor (ImpactNormal.Z == 1), we use HalfHeight.
                // If hitting a vertical wall (ImpactNormal.Z == 0), we use Radius.
                CoreOffset = FMath::Lerp(Radius, HalfHeight, FMath::Abs(HitResult.ImpactNormal.Z));
            }
        }
        // Apply the precise offset along the surface normal plus a tiny safety air-buffer
        TargetLocation += HitResult.ImpactNormal * (CoreOffset + SafetyBuffer); 

        DrawDebugSphere(
            GetWorld(),
            TargetLocation,
            30.0f,     
            12,        
            FColor::Cyan,
            false,     
            5.0f       
        );

        float Distance = FVector::Distance(Avatar->GetActorLocation(), TargetLocation);
        float CalculatedDuration = (TravelSpeed > 0.0f) ? (Distance / TravelSpeed) : 0.5f;

        UAbilityTask_ApplyRootMotionMoveToForce* MoveTask =
            UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
                this,
                FName("GrapplePullTask"),
                TargetLocation,
                CalculatedDuration,
                true,                                                          
                MOVE_Flying,                                                   
                false,                                                        
                nullptr,                                                       
                ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, 
                FVector::ZeroVector,                                           
                0.0f                                                           
            );
        if (MoveTask)
        {
            MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &URimaGameplayAbility_Grapple::OnGrappleMovementFinished);

            MoveTask->ReadyForActivation();
            UE_LOG(LogTemp, Log, TEXT("Grapple activated"));
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Grapple Move Task null"));
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        }
    }
    else
    {
        if (GrappleRetractCueTag.IsValid())
        {
            FGameplayEffectContextHandle Context = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GrappleRetractCueTag, Context);
        }

        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void URimaGameplayAbility_Grapple::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    UE_LOG(LogTemp, Log, TEXT("Grapple End Ability"));
}

void URimaGameplayAbility_Grapple::OnGrappleMovementFinished()
{
    bool bReplicateEndAbility = true;
    UE_LOG(LogTemp, Log, TEXT("Grapple Movement Finished"));

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, false);
}