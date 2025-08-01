// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IS_BeInteractExtendBase.generated.h"

/*可被交互物的扩展基类
* 会向Owner寻找任意包含自身ComponentTag的UIS_BeInteractComponent组件
*/
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_BeInteractExtendBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_BeInteractExtendBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LinkInteractEnter(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);
	virtual void LinkInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LinkInteractEnd(UIS_InteractComponent* InteractComponent);
	virtual void LinkInteractEnd_Implementation(UIS_InteractComponent* InteractComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LinkInteractComplete(UIS_InteractComponent* InteractComponent);
	virtual void LinkInteractComplete_Implementation(UIS_InteractComponent* InteractComponent);

public:
};
