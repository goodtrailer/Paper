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
	UFUNCTION(BlueprintCallable)
	bool ParseBoardLayoutTexture(const UTexture2D* Texture);				// Sets values in PaperGameState
	UFUNCTION(BlueprintCallable)
	bool ParseBoardLayoutFile(const FString& Filename);
	bool ParseBoardLayout(const uint8* BoardImage, const int BoardLayoutWidth, const int BoardLayoutHeight);
	APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	void Logout(AController* Exiting) override;

	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AGround> GroundBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> WallBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> MineBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class ACastle> CastleBP;
	UPROPERTY(EditAnywhere, Category = "Tile Blueprints")
	TSubclassOf<class AUnit> SpawnBP;

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
	const FColor* GetColorArray() const;
	FColor* GetColorArray();
	const FTexture2DMipMap* GetMipMap() const;
	FTexture2DMipMap* GetMipMap();
	FTexture2DMipMap* operator->();
	FTexture2DMipMap& operator*();
private:
	FTexture2DMipMap* MipMap;
	FColor* ColorArray;
};

namespace ColorCode
{
	const static FColor SpawnGreen(0, 127, 14);
	const static FColor SpawnRed(127, 0, 0);
	const static FColor SpawnPurple(107, 63, 127);
	const static FColor SpawnBrown(127, 89, 63);
	const static FColor SpawnWhite(160, 160, 160);
	const static FColor SpawnBlack(48, 48, 48);
	const static FColor CastleGreen(0, 254, 33);
	const static FColor CastleRed(254, 0, 0);
	const static FColor CastlePurple(161, 127, 255);
	const static FColor CastleBrown(255, 178, 127);
	const static FColor CastleWhite(192, 192, 192);
	const static FColor CastleBlack(64, 64, 64);
	const static FColor Wall(0, 148, 255);
	const static FColor Bounds(0, 0, 0);
	const static FColor Mine(255, 216, 0);
	const static FColor Ground(255, 255, 255);
	const static FColor Hole(255, 106, 0);

	const static FColor OneWayUL(72, 1, 255);
	const static FColor OneWayU(255, 0, 218);
	const static FColor OneWayUR(178, 1, 255);
	const static FColor OneWayL(128, 0, 111);
	const static FColor OneWayR(255, 0, 110);
	const static FColor OneWayDL(32, 0, 125);
	const static FColor OneWayD(127, 0, 55);
	const static FColor OneWayDR(86, 0, 127);
}
