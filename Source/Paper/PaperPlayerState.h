#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PaperPlayerState.generated.h"


enum class ETeam : uint8;

UCLASS()
class PAPER_API APaperPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	APaperPlayerState();
	UFUNCTION(BlueprintCallable)
	bool IsTurn();
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Meta")
	ETeam Team;
private:
	class APaperGameState* GameState;
};
