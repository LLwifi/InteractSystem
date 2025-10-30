// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BIEGetComponent.h"
#include "IS_BIEHideHoldItem.generated.h"

/**可被交互物的功能扩展——隐藏持有物
 * 
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEHideHoldItem : public UIS_BIEGetComponent
{
	GENERATED_BODY()
public:
	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
};
