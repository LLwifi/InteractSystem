// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <GameplayTagContainer.h>
#include "Net/UnrealNetwork.h"
#include "IS_InteractComponent.generated.h"

////任务角色
//UENUM(BlueprintType)
//enum class ETS_TaskRole :uint8
//{
//	None = 0 UMETA(DisplayName = "无"),
//	Player UMETA(DisplayName = "Player-做任务的单位"),
//	NPC UMETA(DisplayName = "NPC-发任务的单位"),
//	People UMETA(DisplayName = "People-与任务相关的单位")
//};

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskComponentDelegate, UTS_TaskComponent*, TaskComponent,  UTS_Task*, Task);

/*交互组件：该组件拥有交互其他资源的能力
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTSYSTEM_API UIS_InteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIS_InteractComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
public:

	//UPROPERTY(BlueprintAssignable)
	//FTaskComponentDelegate AddTaskEvent;
	//UPROPERTY(BlueprintAssignable)
	//FTaskComponentDelegate TaskEndEvent;

	/*交互距离：该组件的Actor和交互目标之间的距离不能小于该值
	* -1表示无距离限制
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InteractDistance = -1.0f;

	//全部交互目标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> AllInteractActor;
};
