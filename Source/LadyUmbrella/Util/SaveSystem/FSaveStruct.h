// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSaveStruct.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="FSaveStruct"))
struct FSaveStructData //: public FTableRowBase
{
    GENERATED_BODY()

public:
    
    FSaveStructData();
    // maybe we dont need to have a transform everywhere, if its only data not need this
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FTransform WorldTransform;
    
};