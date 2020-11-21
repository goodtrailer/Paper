// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include "MovableTileInfo.h"
#include "GameFramework/PlayerController.h"
#include "PaperPlayerController.generated.h"

enum class ETeam : uint8;

UCLASS()
class PAPER_API APaperPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APaperPlayerController();
	UFUNCTION(BlueprintCallable)
	class APaperPlayerState* GetPaperPlayerState();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SpawnUnit(TSubclassOf<class AUnit> Type, int Coordinates);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_EndTurn();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(AUnit* Attacker, AUnit* Victim);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_ChangeTeam(ETeam Team);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SendMessage(const FText& Message);
	UFUNCTION(BlueprintImplementableEvent)
	void CheckVictory(ETeam WinningTeam);
	UFUNCTION(BlueprintImplementableEvent)
	void CheckDefeat(ETeam DefeatedTeam);
	void CheckDeadUnit(AUnit* Unit);
	UFUNCTION(BlueprintCallable)
	void ToggleMovableOverlay();
	UFUNCTION(BlueprintCallable)
	void ToggleAttackableOverlay();
	UFUNCTION(BlueprintCallable)
	void ToggleSpawnableOverlay(TSubclassOf<AUnit> Type);
	UFUNCTION(BlueprintCallable)
	void ResetCameraPosition();
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	const FString OpenFile(const FString& Title, const FString& DefaultPath, const TArray<FString>& FileTypes);

	UPROPERTY(BlueprintReadWrite)
	class UPaperUserInterface* UserInterface;
	UPROPERTY(BlueprintReadWrite)
	class ULobbyUserInterface* LobbyInterface;
	UPROPERTY(BlueprintReadWrite)
	class UChatUserInterface* ChatInterface;
	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* ScoreboardInterface;
	class APaperHUD* HUD;
	UPROPERTY(Replicated)
	bool bInGame;				// can't be put in PaperPlayerState because it needs to be used immediately. Also, no other player needs to know this value, only the server does.

protected:
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
	void ToggleMenu();
	void ShowScoreboard();
	void HideScoreboard();
	void ResizeHPBarShowRadiusStart();
	void ResizeHPBarShowRadiusStop();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveUnit(int Origin, int Destination);
	void MovableOverlayOn();
	void MovableOverlayOff();
	void AttackableOverlayOn();
	void AttackableOverlayOff();
	void SpawnableOverlayOn(TSubclassOf<AUnit> Type);
	void SpawnableOverlayOff();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_SetInGame(bool b);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	void FocusChatbox();

	class ACameraPawn* CameraPawn;
	UPROPERTY(BlueprintReadWrite)
	class AUnit* SelectedUnit;
	class AUnit* HoveredUnit;
	class AUnit* LastHoveredUnit;
	// int is Coords
	TMap<int, FMovableTileInfo> MovableTiles;
	TArray<AActor*> MovableOverlayArray;
	TArray<AActor*> MoveOverlayArray;
	// int is Coords
	TSet<int> AttackableTiles;
	// int is Coords
	TSet<int> ReachableTiles;
	// int is Coords
	TSet<int> SpawnableTiles;
	TArray<AActor*> AttackableOverlayArray;
	AActor* SelectOverlay;
	AActor* HoverOverlay;
	AActor* AttackOverlay;
	AActor* MoveOverlay;
	AActor* SpawnableOverlay;
	AActor* NonspawnableOverlay;
	class APaperGameState* GameState;
	class UPaperGameInstance* GameInstance;

	TSubclassOf<AUnit> UnitToSpawnBP;

	bool bMovableOverlayOn;
	bool bAttackableOverlayOn;
	bool bSpawnableOverlayOn;

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
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> SpawnableOverlayBP;
	UPROPERTY(EditAnywhere, Category = "Overlay Blueprints")
	TSubclassOf<AActor> NonspawnableOverlayBP;
	
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Blueprints")
	TSubclassOf<class UPaperUserInterface> UserInterfaceBP;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Blueprints")
	TSubclassOf<class ULobbyUserInterface> LobbyInterfaceBP;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Blueprints")
	TSubclassOf<class UChatUserInterface> ChatInterfaceBP;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Blueprints")
	TSubclassOf<class UUserWidget> ScoreboardInterfaceBP;

	class APaperPlayerState* UnsafePlayerState;
	friend class ACameraPawn;
};
