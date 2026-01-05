// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "Engine/AssetManager.h"
#include "UEAssetManager.generated.h"

UCLASS()
class PROJECTT_API UUEAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	UUEAssetManager();

public:
	static UUEAssetManager* Get();
	virtual void StartInitialLoading();
	virtual void FinishInitialLoading();
	UDataTable* FindDataTable(const FName& TableName) const;

	template <typename T>
	T* FindDataTableRow(const FName& TableName, const FName& RowName) const
	{
		UDataTable* DataTable = FindDataTable(TableName);
		if (DataTable)
		{
			return DataTable->FindRow<T>(RowName, TEXT(""));
		}
		return nullptr;
	}
};
