// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BIEGetComponent.h"
#include "IS_BIEHideHoldItem.generated.h"

/**可被交互物的功能扩展——交互中隐藏持有物
 * 在GetComponent的配置时，目标组件需要继承或就是USceneComponent
 */
UCLASS()
class INTERACTSYSTEM_API UIS_BIEHideHoldItem : public UIS_BIEGetComponent
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void Init_Implementation(UIS_BeInteractComponent* BeInteractCom, UIS_BeInteractExtendBase* Data) override;

	virtual void InteractStart_Implementation(UIS_InteractComponent* InteractComponent) override;
	virtual void InteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;


public:
	//搜索到要隐藏的USceneComponent
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<USceneComponent*> AllSceneComponent;
};
