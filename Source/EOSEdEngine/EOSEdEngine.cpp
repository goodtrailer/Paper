// Copyright (c) 2019–2020 Alden Wu

#include "EOSEdEngine.h"
#include "Modules/ModuleManager.h"

#include "eos_sdk.h"
#include "eos_common.h"
#include "eos_logging.h"

IMPLEMENT_GAME_MODULE(FDefaultGameModuleImpl, EOSEdEngine);

void UEOSEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	// stored in FTCHARToUTF8 to guarantee lifetime lasting until after EOS_Initialize (to the end of the scope), since
	// TCHAR_TO_ANSI has an extremely short and inconsistent lifetime (meant for passing as function arguments).
	const FTCHARToUTF8 ProjectNameUTF8(*GConfig->GetStr(L"/Script/EngineSettings.GeneralProjectSettings", L"ProjectName", GGameIni));
	const FTCHARToUTF8 ProjectVersionUTF8(*GConfig->GetStr(L"/Script/EngineSettings.GeneralProjectSettings", L"ProjectVersion", GGameIni));

	EOS_InitializeOptions InitOptions;
	InitOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
	InitOptions.ProductName = ProjectNameUTF8.Get();
	InitOptions.ProductVersion = ProjectVersionUTF8.Get();
	InitOptions.Reserved = nullptr;
	InitOptions.AllocateMemoryFunction = nullptr;
	InitOptions.ReallocateMemoryFunction = nullptr;
	InitOptions.ReleaseMemoryFunction = nullptr;
	InitOptions.SystemInitializeOptions = nullptr;

	EOS_EResult InitResult = EOS_Initialize(&InitOptions);
	GLog->Logf(InitResult == EOS_EResult::EOS_Success ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS initialized with %s.", ANSI_TO_TCHAR(EOS_EResult_ToString(InitResult)));

	EOS_Logging_SetCallback([](const EOS_LogMessage* InLogMessage)
	{
		FString LogLevelString;
		ELogVerbosity::Type LogVerbosity;
		switch (InLogMessage->Level)
		{
		case EOS_ELogLevel::EOS_LOG_Error:
			LogLevelString = "Error";
			LogVerbosity = ELogVerbosity::Warning;
			break;
		case EOS_ELogLevel::EOS_LOG_Fatal:
			LogLevelString = "Fatal";
			LogVerbosity = ELogVerbosity::Warning;
			break;
		case EOS_ELogLevel::EOS_LOG_Info:
			LogLevelString = "Info";
			LogVerbosity = ELogVerbosity::Log;
			break;
		case EOS_ELogLevel::EOS_LOG_Verbose:
			LogLevelString = "Verbose";
			LogVerbosity = ELogVerbosity::Log;
			break;
		case EOS_ELogLevel::EOS_LOG_VeryVerbose:
			LogLevelString = "VeryVerbose";
			LogVerbosity = ELogVerbosity::Log;
			break;
		case EOS_ELogLevel::EOS_LOG_Warning:
			LogLevelString = "Warning";
			LogVerbosity = ELogVerbosity::Warning;
			break;
		default:
			LogLevelString = "Unknown";
			LogVerbosity = ELogVerbosity::Warning;
		}
		// i.e. EOS_LOG LogEOSP2P (Verbose): Using Port Range 7777-7876 for P2P traffic
		GLog->Logf(LogVerbosity, L"EOS_LOG %s (%s): %s", ANSI_TO_TCHAR(InLogMessage->Category), *LogLevelString, ANSI_TO_TCHAR(InLogMessage->Message));
	});
	EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_Warning);
}

void UEOSEdEngine::PreExit()
{
	EOS_EResult Result = EOS_Shutdown();
	GLog->Logf(Result == EOS_EResult::EOS_Success ? ELogVerbosity::Log : ELogVerbosity::Warning, L"EOS shutdown with %s.", ANSI_TO_TCHAR(EOS_EResult_ToString(Result)));

	Super::PreExit();
}
