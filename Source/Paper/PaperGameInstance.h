// Copyright (c) 2019–2020 Alden Wu

#pragma once

#include "CoreMinimal.h"
#include <string>													// forgive me ue4 but fstring just isnt cutting it when im working with a c lib like eos
#include "Engine/GameInstance.h"
#include "PaperGameInstance.generated.h"


typedef struct EOS_PlatformHandle* EOS_HPlatform;								// two forward declarations for the price of one !?
typedef struct EOS_SessionsHandle* EOS_HSessions;
typedef struct EOS_ConnectHandle* EOS_HConnect;
typedef struct EOS_ProductUserIdDetails* EOS_ProductUserId;



UCLASS()
class PAPER_API UPaperGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void OnStart() override;
	void Shutdown() override;
	void EOSPlatCreate();											// Not to be confused with EOS_Platform_Create.
	void EOSPlatRelease();											// Not to be confused with EOS_Platform_Release
	void EOSSessionCreate(const char*, uint32);
	UFUNCTION(BlueprintCallable)
	void EOSConnectLogin_FStr(const FString& Name);
	void EOSConnectLogin();											// STATE MACHINE FUNC: Set PlayerName AND PlayerName_FStr before calling.
	void EOSConnectCreateDeviceId();								// Only call after deleting old DeviceId account
	UFUNCTION(BlueprintCallable)
	void EOSConnectDeleteDeviceId();
	UFUNCTION(BlueprintCallable)
	void Debug();
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName_FStr() const;
	UFUNCTION(BlueprintCallable)
	bool bIsLoggedIn() const;
	bool Tick(float DeltaSeconds);									// thank you Ian Breeg from the forums

	EOS_HPlatform EOSPlatHandle;
	EOS_HSessions EOSSessionsHandle;
	EOS_HConnect EOSConnectHandle;
	EOS_ProductUserId EOSProductUserId;

protected:
	FDelegateHandle TickDelegateHandle;
	std::string SessionName;
	std::string DeviceModel;
	bool bIsInSession;
	std::string PlayerName;
};
