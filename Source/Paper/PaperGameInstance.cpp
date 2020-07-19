// Copyright (c) 2019–2020 Alden Wu

#include "PaperGameInstance.h"

#include "eos_sdk.h"
#include "eos_sessions.h"
#include "eos_common.h"

#include "Misc/FileHelper.h"
#include "Windows/WindowsPlatformMisc.h"

void UPaperGameInstance::Shutdown()
{
	EOSPlatRelease();
	Super::Shutdown();
}

void UPaperGameInstance::OnStart()
{
	Super::OnStart();
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UPaperGameInstance::Tick));
	EOSPlatCreate();
}

bool UPaperGameInstance::Tick(float DeltaSeconds)
{
	if (EOSPlatHandle)
		EOS_Platform_Tick(EOSPlatHandle);
	return true;
}

void UPaperGameInstance::EOSPlatCreate()
{
	// #include the EOSPlatCredentials.txt contents so that the credentials aren't exposed to the public, and it uses preprocessor directives so it's
	// figured out at compile time so it still works in distributables.
	// TL;DR: #include is compile-time file reading.

	// Make sure EOSCredentials.txt is laid out like so (with quotation marks and commas, without the numbers on the end):
	// "PRODUCT_ID",								0
	// "SANDBOX_ID",								1
	// "DEPLOYMENT_ID",								2
	// "CLIENT_ID",									3
	// "CLIENT_SECRET",								4
	// "ENCRYPTION_KEY"								5
	const char* CredentialsFile[6] =
	{
		#include "EOSPlatCredentials.txt"
	};

	// stored in FTCHARToUTF8 to guarantee lifetime lasting until after EOS_Platform_Create (to the end of the scope), since
	// TCHAR_TO_ANSI has an extremely short and inconsistent lifetime (meant for passing as function arguments).
	FString CacheDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForWrite(*FPaths::Combine(FPaths::ProjectSavedDir(), L"Cache"));
	if (!FPaths::DirectoryExists(CacheDir))
		IFileManager::Get().MakeDirectory(*CacheDir);
	const FTCHARToUTF8 CacheDirUTF8(*CacheDir);

	EOS_Platform_Options PlatOptions;
	PlatOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
	PlatOptions.Reserved = nullptr;
	PlatOptions.ProductId = CredentialsFile[0];
	PlatOptions.SandboxId = CredentialsFile[1];
	PlatOptions.ClientCredentials.ClientId = CredentialsFile[3];
	PlatOptions.ClientCredentials.ClientSecret = CredentialsFile[4];
	PlatOptions.EncryptionKey = CredentialsFile[5];
	PlatOptions.OverrideCountryCode = nullptr;
	PlatOptions.OverrideLocaleCode = nullptr;
	PlatOptions.DeploymentId = CredentialsFile[2];
#if WITH_EDITOR
	PlatOptions.Flags = EOS_PF_LOADING_IN_EDITOR;
#else
	PlatOptions.Flags = EOS_PF_DISABLE_OVERLAY;
#endif
	PlatOptions.bIsServer = false;
	PlatOptions.CacheDirectory = CacheDirUTF8.Get();

	EOSPlatHandle = EOS_Platform_Create(&PlatOptions);
	GLog->Logf(EOSPlatHandle ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS platform created %s.", EOSPlatHandle ? L"successfully" : L"unsuccessfully");

	
	// sessions interface that is going to be used for all the actual connection stuff (server list)
	EOSSessionsHandle = EOS_Platform_GetSessionsInterface(EOSPlatHandle);
	EOSConnectHandle = EOS_Platform_GetConnectInterface(EOSPlatHandle);
}

void UPaperGameInstance::EOSPlatRelease()
{
	if (EOSPlatHandle)
	{
		EOS_Platform_Release(EOSPlatHandle);
		EOSPlatHandle = nullptr;
		EOSSessionsHandle = nullptr;
		EOSConnectHandle = nullptr;
		EOSProductUserId = nullptr;
		GLog->Log(L"EOS platform released successfully.");
	}
	else
		GLog->Logf(ELogVerbosity::Warning, L"EOS platform released unsuccessfully. Maybe the platform has already been released? (This function is called twice on packaged builds.)");
}

void UPaperGameInstance::EOSSessionCreate(const char* Name, uint32 MaxPlayers)
{
	if (bIsInSession)
	{
		GLog->Log(L"EOS session created unsuccessfully: already in a session.");
		return;
	}

	SessionName = Name;

	EOS_Sessions_CreateSessionModificationOptions SessionModOptions;
	SessionModOptions.ApiVersion = EOS_SESSIONS_CREATESESSIONMODIFICATION_API_LATEST;
	SessionModOptions.SessionName = SessionName.c_str();
	SessionModOptions.BucketId = "";
	SessionModOptions.MaxPlayers = MaxPlayers;
}

void UPaperGameInstance::EOSConnectLogin_FStr(const FString& Name)
{
	PlayerName = TCHAR_TO_ANSI(*Name);
	EOSConnectLogin();
}

void UPaperGameInstance::EOSConnectLogin()
{
	EOS_Connect_LoginOptions LoginOptions;
	LoginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
	EOS_Connect_Credentials Credentials;
	Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
	Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;
	Credentials.Token = nullptr;
	LoginOptions.Credentials = &Credentials;
	EOS_Connect_UserLoginInfo LoginInfo;
	LoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
	LoginInfo.DisplayName = PlayerName.c_str();
	LoginOptions.UserLoginInfo = &LoginInfo;
	// passing this as "ClientData" (funny joke), since you can't pass member functions (expected, this is a c library where there are no classes)
	EOS_Connect_Login(EOSConnectHandle, &LoginOptions, this, [](const EOS_Connect_LoginCallbackInfo* Data)
	{
		GLog->Logf(Data->ResultCode == EOS_EResult::EOS_Success ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS logged in with %s.", ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		(*(UPaperGameInstance*)Data->ClientData).EOSProductUserId = Data->LocalUserId;

		// if connecting failed, it's probably because there is no device id, so create one.
		if (Data->ResultCode != EOS_EResult::EOS_Success)
			(*(UPaperGameInstance*)Data->ClientData).EOSConnectCreateDeviceId();
	});
}

void UPaperGameInstance::EOSConnectCreateDeviceId()
{
	DeviceModel = TCHAR_TO_ANSI(*FWindowsPlatformMisc::GetDeviceMakeAndModel());					// fine, despite what ue4 docs say; we are using std::string not const char*, there's some special sauce going on

	EOS_Connect_CreateDeviceIdOptions CDIOptions;
	CDIOptions.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;
	CDIOptions.DeviceModel = DeviceModel.c_str();
	EOS_Connect_CreateDeviceId(EOSConnectHandle, &CDIOptions, this, [](const EOS_Connect_CreateDeviceIdCallbackInfo* Data)
	{
		GLog->Logf(Data->ResultCode == EOS_EResult::EOS_Success ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS device id created with %s.", ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
		// if creating a device id was successful, attempt login. otherwise, fail.
		if (Data->ResultCode == EOS_EResult::EOS_Success)
			(*(UPaperGameInstance*)Data->ClientData).EOSConnectLogin();
	});
}

void UPaperGameInstance::EOSConnectDeleteDeviceId()
{
	EOS_Connect_DeleteDeviceIdOptions DeleteOptions;
	DeleteOptions.ApiVersion = EOS_CONNECT_DELETEDEVICEID_API_LATEST;
	EOS_Connect_DeleteDeviceId(EOSConnectHandle, &DeleteOptions, nullptr, [](const EOS_Connect_DeleteDeviceIdCallbackInfo* Data)
	{
		GLog->Logf(Data->ResultCode == EOS_EResult::EOS_Success ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS device id deleted with %s.", ANSI_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
	});
}

void UPaperGameInstance::Debug()
{
	EOSConnectDeleteDeviceId();
}

bool UPaperGameInstance::bIsLoggedIn() const
{
	if (EOSConnectHandle && EOSProductUserId
		&& EOS_Connect_GetLoginStatus(EOSConnectHandle, EOSProductUserId) == EOS_ELoginStatus::EOS_LS_LoggedIn)
		return true;
	else
		return false;
}

FString UPaperGameInstance::GetPlayerName_FStr() const
{
	return FString(ANSI_TO_TCHAR(PlayerName.c_str()));
}
