// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "SkeletalUnit.generated.h"

UCLASS()
class PAPER_API ASkeletalUnit : public AUnit
{
	GENERATED_BODY()
public:
	ASkeletalUnit();
	void Attack_Implementation(AUnit* Victim) override;
	class UMeshComponent* GetMeshComponent() override;
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_PlayAnimation();
	UFUNCTION(BlueprintCallable)
	void FaceCoordinates(const FIntPoint& Target);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meta")
	class UAnimationAsset* AnimationAsset;
};
