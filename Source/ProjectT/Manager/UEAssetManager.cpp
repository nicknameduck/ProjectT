// Fill out your copyright notice in the Description page of Project Settings.


#include "UEAssetManager.h"

UUEAssetManager::UUEAssetManager()
{
}

UUEAssetManager* UUEAssetManager::Get()
{
	UUEAssetManager* Manager = Cast<UUEAssetManager>(GEngine->AssetManager);

	return Manager;
}

void UUEAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

void UUEAssetManager::FinishInitialLoading()
{
	Super::FinishInitialLoading();
}

UDataTable* UUEAssetManager::FindDataTable(const FName& TableName) const
{
	FPrimaryAssetType AssetType = FPrimaryAssetType(TEXT("DataTable"));

	TArray<FPrimaryAssetId> AssetIds;
	GetPrimaryAssetIdList(AssetType, AssetIds);

	for (auto& AssetId : AssetIds)
	{
		FString AssetName = AssetId.PrimaryAssetName.ToString();

		if (AssetName == TableName.ToString())
		{
			FAssetData AssetData;
			GetPrimaryAssetData(AssetId, AssetData);

			UDataTable* Table = Cast<UDataTable>(AssetData.GetAsset());
			if (Table)
				return Table;
			else
			{
				Table = Cast<UDataTable>(GetPrimaryAssetObject(AssetId));
				return Table;
			}
		}
	}

	return nullptr;
}

