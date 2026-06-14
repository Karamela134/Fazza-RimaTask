    #include "RimaRifleExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Teams/LyraTeamSubsystem.h"
#include "DrawDebugHelpers.h"


URimaRifleExecution::URimaRifleExecution()
{
}

void URimaRifleExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& ExecutionOutput) const
{
    // 1. Get the Ability System Components for Target and Source
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

    AActor* TargetActor = TargetASC ? TargetASC->GetOwnerActor() : nullptr;
    AActor* SourceActor = SourceASC ? SourceASC->GetOwnerActor() : nullptr;

    if (!TargetActor || !SourceActor) return;


    // 2. Grab Lyra's global Team Subsystem from the world
    UWorld* World = TargetActor->GetWorld();
    ULyraTeamSubsystem* TeamSubsystem = World ? World->GetSubsystem<ULyraTeamSubsystem>() : nullptr;

    if (!TeamSubsystem)
    {
        UE_LOG(LogTemp, Log, TEXT("Team subsystem missing"));
        return;
    }

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FGameplayTag SplashTag = FGameplayTag::RequestGameplayTag(FName("Data.Modifier.SplashMultiplier"));
    float FinalMultiplier = Spec.GetSetByCallerMagnitude(SplashTag, false, 1.0f);

    UE_LOG(LogTemp, Warning, TEXT("=== GEC RUNNING FOR TARGET: %s (Multiplier: %f) ==="), *TargetActor->GetName(), FinalMultiplier);
    if (FinalMultiplier == 0.5f)
    {
        UE_LOG(LogTemp, Log, TEXT("found surrounding ally/enemy"));
    }

    if (TeamSubsystem)
    {
        // 3. Compare the shooter and the victim
        ELyraTeamComparison Comparison =
            TeamSubsystem->CompareTeams(SourceActor, TargetActor);

        switch (Comparison)
        {
        case ELyraTeamComparison::OnSameTeam:
            Heal(ExecutionOutput, TargetActor,TargetASC, FinalMultiplier);
            break;

        case ELyraTeamComparison::DifferentTeams:
            Damage(ExecutionOutput, TargetActor, TargetASC, FinalMultiplier);
            break;

        default:
            return;
        }
    }
}
void URimaRifleExecution::Heal(FGameplayEffectCustomExecutionOutput& ExecutionOutput, AActor* ally, UAbilitySystemComponent* TargetASC,float FinalMultiplier) const
{
    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, BaseHealing* FinalMultiplier));
    CheckNetworkMode(ally);
    if (TargetASC && ally)
    {
        // Pack context parameters to send down to the client visual pipeline
        FGameplayCueParameters CueParams;
        CueParams.Location = ally->GetActorLocation();
        CueParams.RawMagnitude = BaseHealing; // Optional: Pass numbers to scale effects dynamically

        // This triggers a replicated broadcast across the network
        TargetASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.RimaRifle.Heal")), CueParams);
    }
    UE_LOG(LogTemp, Log, TEXT("Healed %s"),*ally->GetName());
}

void URimaRifleExecution::Damage(FGameplayEffectCustomExecutionOutput& ExecutionOutput, AActor* enemy, UAbilitySystemComponent* TargetASC,float FinalMultiplier) const
{
    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -BaseDamage* FinalMultiplier));
    CheckNetworkMode(enemy);
    if (TargetASC && enemy)
    {
        FGameplayCueParameters CueParams;
        CueParams.Location = enemy->GetActorLocation();
        CueParams.RawMagnitude = BaseDamage;

        TargetASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.RimaRifle.Damage")), CueParams);
    }
    UE_LOG(LogTemp, Log, TEXT("Damaged %s"), *enemy->GetName());
}

void URimaRifleExecution::CheckNetworkMode(AActor* target) const
{
    ENetMode NetMode = target->GetWorld()->GetNetMode();

    FString ModeString;

    switch (NetMode)
    {
    case NM_Standalone:
        ModeString = TEXT("Standalone");
        break;

    case NM_ListenServer:
        ModeString = TEXT("ListenServer");
        break;

    case NM_DedicatedServer:
        ModeString = TEXT("DedicatedServer");
        break;

    case NM_Client:
        ModeString = TEXT("Client");
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Heal executed on %s"), *ModeString);
}
