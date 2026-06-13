#include "RimaRifleExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Teams/LyraTeamSubsystem.h"



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


    if (TeamSubsystem)
    {
        // 3. Compare the shooter and the victim
        ELyraTeamComparison Comparison =
            TeamSubsystem->CompareTeams(SourceActor, TargetActor);

        switch (Comparison)
        {
        case ELyraTeamComparison::OnSameTeam:
            Heal(ExecutionOutput);
            break;

        case ELyraTeamComparison::DifferentTeams:
            Damage(ExecutionOutput);
            break;

        default:
            return;
        }
    }
    //bool bIsFriendly = false;
    //// 5. Apply the calculated modifiers based on the team status
    //if (bIsFriendly)
    //{
    //    // Ally hit -> Heal them (Positive Additive change)
    //    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, BaseHealing));
    //}
    //else
    //{
    //    // Enemy hit -> Damage them (Negative Additive change)
    //    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -BaseDamage));
    //}
}
void URimaRifleExecution::Heal(FGameplayEffectCustomExecutionOutput& ExecutionOutput) const
{
    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, BaseHealing));
    UE_LOG(LogTemp, Log, TEXT("Healed"));
}

void URimaRifleExecution::Damage(FGameplayEffectCustomExecutionOutput& ExecutionOutput) const
{
    ExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(ULyraHealthSet::GetHealthAttribute(), EGameplayModOp::Additive, -BaseDamage));
    UE_LOG(LogTemp, Log, TEXT("Damaged"));
}