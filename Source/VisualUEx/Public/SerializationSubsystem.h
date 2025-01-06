// Copyright (c) 2024 Evgeny Shustov

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SerializationSubsystem.generated.h"

class UVisualVersioningSubsystem;
class UVisualController;
class ULocalPlayer;
class FArchive;

/**
 * Game instance subsystem that handles saving and loading gameplay data.
 */
UCLASS()
class VISUALUEX_API USerializationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USerializationSubsystem();

	UFUNCTION(BlueprintCallable, Category = "Serialization Subsystem")
	bool LoadVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, ULocalPlayer* LocalPlayer, const FString& Filename);

	UFUNCTION(BlueprintCallable, Category = "Serialization Subsystem")
	bool SaveVisualU(UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController, ULocalPlayer* LocalPlayer, const FString& Filename);

	UFUNCTION(BlueprintCallable, Category = "Serialization Subsystem")
	bool GetAllSaveGameFilenames(TArray<FString>& Filenames, int32 UserIndex);

	UFUNCTION(BlueprintCallable, Category = "Serialization Subsystem")
	bool DoesSaveGameExist(const FString& Filename, int32 UserIndex);

	UFUNCTION(BlueprintCallable, Category = "Serialization Subsystem")
	bool DeleteSaveGame(const FString& Filename, int32 UserIndex);

protected:
	virtual void SerializeCompressed(FArchive& Ar, TArray<uint8>& UncompressedData);

	virtual void SerializeVisualU(FArchive& Ar, UVisualVersioningSubsystem* VersioningSubsystem, UVisualController* VisualController);

};
