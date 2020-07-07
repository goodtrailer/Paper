// Copyright (c) 2019–2020 Alden Wu

#pragma once
#include "CoreMinimal.h"
#include "Math/Color.h"
#include "GameFramework/GameModeBase.h"
#include "PaperGameMode.generated.h"

UCLASS()
class PAPER_API APaperGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	APaperGameMode();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void BeginGame();

protected:
	struct ManagedMipMap;

	bool ColorsNearlyEqual(FColor, FColor);			// Somewhat unnecessary since the switch to pngs. Was previously used due to jpg compression.
	void ParseBoardLayout(UTexture2D*);				// Sets values in PaperGameState
	APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	void Logout(AController* Exiting) override;

	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> GroundBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> WallBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> MineBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class ACastle> CastleBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> SpawnBP;

	UPROPERTY(EditAnywhere, Category = "Misc")
	class UTexture2D* DefaultBoardLayoutTexture;
	UPROPERTY(EditAnywhere, Category = "Misc")
	float ColorsNearlyEqualThreshold;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	int StartingGold;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	int StartingPassiveIncome;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint8 StartingCastleHP;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	uint8 StartingCastleHPMax;
	UPROPERTY(VisibleAnywhere, Category = "Misc")
	TMap<FString, uint8> NameCount;
	
	class APaperGameState* GameState;
};

struct APaperGameMode::ManagedMipMap
{
public:
	ManagedMipMap(FTexture2DMipMap* Source);
	~ManagedMipMap();
	inline FColor* GetColorArray() const;
	inline const FTexture2DMipMap* GetMipMap() const;
	FTexture2DMipMap* operator->();
	FTexture2DMipMap& operator*();
private:
	FTexture2DMipMap* MipMap;
	FColor* ColorArray;
};

namespace ColorCode
{
	const FColor SpawnGreen = FColor(0, 127, 14);
	const FColor SpawnRed = FColor(127, 0, 0);
	const FColor CastleGreen = FColor(0, 254, 33);
	const FColor CastleRed = FColor(254, 0, 0);
	const FColor Wall = FColor(0, 148, 255);
	const FColor Bounds = FColor(0, 0, 0);
	const FColor Mine = FColor(255, 216, 0);
	const FColor Ground = FColor(255, 255, 255);
	const FColor Hole = FColor(255, 106, 0);

	const FColor OneWayUL = FColor(72, 1, 255);
	const FColor OneWayU = FColor(255, 0, 218);
	const FColor OneWayUR = FColor(178, 1, 255);
	const FColor OneWayL = FColor(128, 0, 111);
	const FColor OneWayR = FColor(255, 0, 110);
	const FColor OneWayDL = FColor(32, 0, 125);
	const FColor OneWayD = FColor(127, 0, 55);
	const FColor OneWayDR = FColor(86, 0, 127);
}
