// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"
#include <Common/IS_BeInteractInterface.h>
#include "IS_BIEInteractCD.generated.h"


/*����������Ϣ_������ȴ���
* ͨ���ǿ����õĳ�ʼ����Ϣ
*/
USTRUCT(BlueprintType)
struct FIS_BeInteractInfo_CD
{
	GENERATED_BODY()
public:
	//����CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CDTime = 0.0f;

	//ÿ�������ٴν�������һ��CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnterCDInteractNum = 1;

	//�������һ�ν���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_InteractNumSubtractType InteractNumSubtractType = EIS_InteractNumSubtractType::Complete;

	/*��ͬ�����ߵĽ���CD�Ƿ�ֿ���¼
	* ��ֵΪtrueʱ�������ܣ���ʱCDTime��ʾ���������������CD
	* EveryoneCDTime��ʾÿ���˵�����CD
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractNumIsMultiplepeople = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	float EveryoneCDTime = 0.0f;

	//ÿ�������ٴν�������һ��CD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	int32 EveryoneEnterCDInteractNum = 1;

	//�������һ�ν���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bInteractNumIsMultiplepeople"))
	EIS_InteractNumSubtractType EveryoneInteractNumSubtractType = EIS_InteractNumSubtractType::Complete;
};

/**�ɱ�������Ĺ�����չ������ȴ
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INTERACTSYSTEM_API UIS_BIEInteractCD : public UIS_BeInteractExtendBase
{
	GENERATED_BODY()
	
public:
	//virtual void LinkInteractEnter_Implementation(UIS_InteractComponent* InteractComponent, EIS_InteractTraceType TraceType) override;
	//virtual void LinkInteractEnd_Implementation(UIS_InteractComponent* InteractComponent) override;
	//virtual void LinkInteractComplete_Implementation(UIS_InteractComponent* InteractComponent) override;

	//����CD

	//����CD
	UFUNCTION(BlueprintCallable)
	void EnterCD();

	//�ı佻������


	UFUNCTION()
	void CDTimerBack();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_BeInteractInfo_CD Config;

	//�����Ĵ���
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 InteractNum = 0;
	//�Ƿ���CD��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsCD = false;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle CDTimerHandle;
};
