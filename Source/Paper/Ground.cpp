// Copyright (c) 2019–2020 Alden Wu

#include "Ground.h"
#include "PaperEnums.h"
#include "GlobalStatics.h"


void AGround::BuildArrows()
{
	if (!!(CollidableDirections & EDirection::Up))
	{
		UStaticMeshComponent* ArrowMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		ArrowMesh->SetRelativeTransform(FTransform(
			FRotator(0.f, EDirectionToDegrees(EDirection::Up), 0.f),	// rotation
			FVector(0.f, 0.f, 100.f),									// location
			FVector(100.f)												// scale
		));
		ArrowMesh->SetupAttachment(RootComponent);
		ArrowMesh->SetIsReplicated(true);
		ArrowMesh->SetStaticMesh(Cast<UGlobalStatics>(GEngine->GameSingleton)->GroundArrowMesh);
		ArrowMesh->RegisterComponent();
	}
	if (!!(CollidableDirections & EDirection::Right))
	{
		UStaticMeshComponent* ArrowMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		ArrowMesh->SetRelativeTransform(FTransform(
			FRotator(0.f, EDirectionToDegrees(EDirection::Right), 0.f),	// rotation
			FVector(0.f, 0.f, 100.f),									// location
			FVector(100.f)												// scale
		));
		ArrowMesh->SetupAttachment(RootComponent);
		ArrowMesh->SetIsReplicated(true);
		ArrowMesh->SetStaticMesh(Cast<UGlobalStatics>(GEngine->GameSingleton)->GroundArrowMesh);
		ArrowMesh->RegisterComponent();
	}
	if (!!(CollidableDirections & EDirection::Down))
	{
		UStaticMeshComponent* ArrowMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		ArrowMesh->SetRelativeTransform(FTransform(
			FRotator(0.f, EDirectionToDegrees(EDirection::Down), 0.f),	// rotation
			FVector(0.f, 0.f, 100.f),									// location
			FVector(100.f)												// scale
		));
		ArrowMesh->SetupAttachment(RootComponent);
		ArrowMesh->SetIsReplicated(true);
		ArrowMesh->SetStaticMesh(Cast<UGlobalStatics>(GEngine->GameSingleton)->GroundArrowMesh);
		ArrowMesh->RegisterComponent();
	}
	if (!!(CollidableDirections & EDirection::Left))
	{
		UStaticMeshComponent* ArrowMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		ArrowMesh->SetRelativeTransform(FTransform(
			FRotator(0.f, EDirectionToDegrees(EDirection::Left), 0.f),	// rotation
			FVector(0.f, 0.f, 100.f),									// location
			FVector(100.f)												// scale
		));
		ArrowMesh->SetupAttachment(RootComponent);
		ArrowMesh->SetIsReplicated(true);
		ArrowMesh->SetStaticMesh(Cast<UGlobalStatics>(GEngine->GameSingleton)->GroundArrowMesh);
		ArrowMesh->RegisterComponent();
	}
}