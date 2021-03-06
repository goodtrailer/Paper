// Copyright (c) 2019–2020 Alden Wu


#include "SkeletalUnit.h"
#include "GenericPlatform/GenericPlatformMath.h"

ASkeletalUnit::ASkeletalUnit()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkeletalMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
}

void ASkeletalUnit::Attack_Implementation(AUnit* Victim)
{
	FaceCoordinates(Victim->GetCoordinatesVector());
	Multicast_PlayAnimation(AttackAnim);
	Super::Attack_Implementation(Victim);
}

void ASkeletalUnit::FaceCoordinates(const FIntPoint& Target)
{
	FIntPoint Source = GetCoordinatesVector();
	SetActorRotation(FRotator(0, 180 / PI * FGenericPlatformMath::Atan2(Target.X - Source.X, Source.Y - Target.Y), 0));
}

void ASkeletalUnit::Multicast_PlayAnimation_Implementation(UAnimationAsset* AnimSequence)
{
	SkeletalMeshComponent->PlayAnimation(AnimSequence, false);
}
