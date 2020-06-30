// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PaperPlayerController.generated.h"

enum class EDirection : uint8;

UCLASS()
class PAPER_API APaperPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APaperPlayerController();
	UFUNCTION(BlueprintCallable)
	class APaperPlayerState* GetPaperPlayerState();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SpawnUnit(TSubclassOf<class AUnit> Type);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EndTurn();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(AUnit* Attacker, AUnit* Victim);
	UFUNCTION(BlueprintImplementableEvent)
	void CheckVictory(ETeam WinningTeam);
	UFUNCTION(BlueprintImplementableEvent)
	void CheckDefeat(ETeam DefeatedTeam);
	void CheckUpdatedUnit(AUnit* Unit, bool bUnselectUnit = false);
	UFUNCTION(BlueprintCallable)
	void ToggleMovableOverlay();
	UFUNCTION(BlueprintCallable)
	void ToggleAttackableOverlay();
	UFUNCTION(BlueprintCallable)
	void ResetCameraPosition;
	void StartGame();

	UPROPERTY(BlueprintReadWrite)
	class UPaperUserInterface* UserInterface;
	
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
	inline void UpdateSelectedUnit();
	void MoveUnit();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveUnit(int Origin, int Destination, uint8 EnergyLeft);
	void AttackUnit();
	void MovableOverlayOn();
	void MovableOverlayOff();
	void AttackableOverlayOn();
	void AttackableOverlayOff();

	class ACameraPawn* CameraPawn;
	class AUnit* SelectedUnit;
	class AUnit* HoveredUnit;
	class AUnit* LastHoveredUnit;
	// int is Coords
	TMap<int, MovableTileInfo> MovableTiles;
	TArray<AActor*> MovableOverlayArray;
	TArray<AActor*> MoveOverlayArray;
	// int is Coords
	TSet<int> AttackableTiles;
	// int is Coords
	TSet<int> ReachableTiles;
	TArray<AActor*> AttackableOverlayArray;
	AActor* SelectOverlay;
	AActor* HoverOverlay;
	AActor* AttackOverlay;
	AActor* MoveOverlay;
	class APaperGameState* GameState;
	class UPaperGameInstance* GameInstance;

	bool bMovableOverlayOn;
	bool bAttackableOverlayOn;
	bool bGameStarted;

	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> MovableOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> HoverOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> SelectOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> AttackOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> MoveOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> MoveArrowBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> MoveLineBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> MoveJointBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> AttackableOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Blueprints")
	TSubclassOf<class UPaperUserInterface> UserInterfaceBP;	

private:
	class APaperPlayerState* UnsafePlayerState;
	friend class ACameraPawn;
};
