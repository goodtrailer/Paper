#pragma once

#include "PaperPlayerState.h"
#include "PaperGameState.h"
#include "CameraPawn.h"
#include "Async/AsyncWork.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PaperPlayerController.generated.h"

UCLASS()
class PAPER_API APaperPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APaperPlayerController();
	UFUNCTION(BlueprintCallable)
	APaperPlayerState* GetPaperPlayerState() const;
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SpawnUnit(TSubclassOf<AUnit> Type);
	
protected:
	struct MovableTileInfo
	{
		uint8 EnergyLeft;
		int SourceTileCoordinates;
		EDirection DirectionToSourceTile;
	};

	void BeginPlay() override;
	void PlayerTick(float) override;
	void SetupInputComponent() override;
	void RotateStart();
	void RotateStop();
	void PanStart();
	void PanStop();
	void ZoomIn();
	void ZoomOut();
	void MouseX(float);
	void MouseY(float);
	void Debug();
	void SelectUnit();
	void MoveUnit();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveUnit(int Origin, int Destination, uint8 EnergyLeft);
	void MovableOverlayOn();
	void MovableOverlayOff();

	ACameraPawn* CameraPawn;
	AUnit* SelectedUnit;
	AUnit* HoveredUnit;
	AUnit* LastHoveredForMoveUnit;
	TArray<AActor*> MovableOverlayArray;
	TArray<AActor*> MoveOverlayArray;
	//int : Coords, uint8 : EnergyLeft
	TMap<int, MovableTileInfo> MovableTiles;
	AActor* SelectOverlay;
	AActor* HoverOverlay;
	APaperGameState* GameState;

	bool bMoveOverlayOn;

	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> MovableOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> HoverOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> SelectOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> MoveArrowBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> MoveLineBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
		TSubclassOf<AActor> MoveJointBP;

	friend class ACameraPawn;
};
