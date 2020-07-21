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
	struct ManagedMipMap;

	void OnStart() override;
	void Shutdown() override;
	void EOSPlatCreate();											// Not to be confused with EOS_Platform_Create.
	void EOSPlatRelease();											// Not to be confused with EOS_Platform_Release
	UFUNCTION(BlueprintCallable)
	void EOSSessionCreate_FStr(const FString& Name, uint8 MaxPlayers, bool bCanJoinInProgress, bool bIsPublic);
	void EOSSessionCreate(const char*, uint8, bool, bool);
	UFUNCTION(BlueprintCallable)
	void EOSSessionDestroy();
	UFUNCTION(BlueprintCallable)
	void EOSConnectLogin_FStr(const FString& Name);					// wrapper function for EOSConnectLogin that takes in FStrings
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
	UFUNCTION(BlueprintCallable)
	void AddSessionAttributeString(const FString& Name, const FString& Value);
	UFUNCTION(BlueprintCallable)
	void AddSessionAttributeInt64(const FString& Name, int64 Value);
	UFUNCTION(BlueprintCallable)
	void AddSessionAttributeBool(const FString& Name, bool Value);

	EOS_HPlatform EOSPlatHandle;
	EOS_HSessions EOSSessionsHandle;
	EOS_HConnect EOSConnectHandle;
	EOS_ProductUserId EOSProductUserId;
	std::string SessionName;
	std::string SessionId;
	bool bIsInSession;

protected:
	FDelegateHandle TickDelegateHandle;
	std::string DeviceModel;
	std::string PlayerName;
};
