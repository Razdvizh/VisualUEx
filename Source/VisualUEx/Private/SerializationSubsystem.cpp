// Copyright (c) 2024 Evgeny Shustov


#include "Misc/CoreMiscDefines.h"
#include "SerializationSubsystem.h"
#include "VisualController.h"
#include "VisualVersioningSubsystem.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"
#include "Engine/LocalPlayer.h"
#include "Serialization/Archive.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

USerializationSubsystem::USerializationSubsystem() = default;

bool USerializationSubsystem::LoadVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, const FPlatformUserId& UserId, const FString& Filename)
{
	check(!Filename.IsEmpty());

	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	TArray<uint8> CompressedData;

	const bool bAttemptToUseNativeUI = true;
	if (SaveGameSystem->LoadGame(bAttemptToUseNativeUI,
		*Filename,
		UserId,
		CompressedData))
	{
		TArray<uint8> Data;
		FMemoryReader CompressedMemoryReader = FMemoryReader(CompressedData);
		SerializeCompressed(CompressedMemoryReader, Data);

		FMemoryReader MemoryReader = FMemoryReader(Data);
		SerializeVisualU(MemoryReader, VersioningSubsystem, VisualController);

		return true;
	}

	return false;
}

bool USerializationSubsystem::SaveVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, const FPlatformUserId& UserId, const FString& Filename)
{
	check(!Filename.IsEmpty());

	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	TArray<uint8> Data;
	FMemoryWriter MemoryWriter = FMemoryWriter(Data);
	SerializeVisualU(MemoryWriter, VersioningSubsystem, VisualController);

	TArray<uint8> CompressedData;
	FMemoryWriter CompressedMemoryWriter = FMemoryWriter(CompressedData);
	SerializeCompressed(CompressedMemoryWriter, Data);

	const bool bAttemptToUseNativeUI = true;
	return SaveGameSystem->SaveGame(bAttemptToUseNativeUI,
		*Filename,
		UserId,
		CompressedData);
}

bool USerializationSubsystem::GetAllSaveGameFilenames(TArray<FString>& Filenames, int32 UserIndex)
{
	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	return SaveGameSystem->GetSaveGameNames(Filenames, UserIndex);
}

bool USerializationSubsystem::DoesSaveGameExist(const FString& Filename, int32 UserIndex)
{
	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	return SaveGameSystem->DoesSaveGameExist(*Filename, UserIndex);
}

bool USerializationSubsystem::DeleteSaveGame(const FString& Filename, int32 UserIndex)
{
	ISaveGameSystem* SaveGameSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	check(SaveGameSystem);

	return SaveGameSystem->DeleteGame(/*bAttemptToUseUI=*/true, *Filename, UserIndex);
}

void USerializationSubsystem::SerializeCompressed(FArchive& Ar, TArray<uint8>& UncompressedData)
{
	int64 Length;

	if (Ar.IsSaving())
	{
		Length = UncompressedData.Num();
	}

	Ar << Length;

	if (Ar.IsLoading())
	{
		UncompressedData.SetNumUninitialized(Length);
	}

	Ar.SerializeCompressedNew(UncompressedData.GetData(), Length);
}

void USerializationSubsystem::SerializeVisualU(FArchive& Ar, UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController)
{
	check(VersioningSubsystem);
	check(VisualController);

	FObjectAndNameAsStringProxyArchive ProxyAr(Ar, /*bInLoadIfFindFails=*/false);
	VersioningSubsystem->SerializeSubsystem(ProxyAr);
	VisualController->SerializeController(ProxyAr);
}
