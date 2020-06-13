#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PaperGameInstance.generated.h"

enum class ETeam : uint8;

struct FBoardInfo
{
	int SizeX, SizeY;
	uint8 SpawnNumber;
};

UCLASS()
class PAPER_API UPaperGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPaperGameInstance();

	FBoardInfo BoardInfo;
	UPROPERTY(BlueprintReadWrite)
	ETeam Team;
};
