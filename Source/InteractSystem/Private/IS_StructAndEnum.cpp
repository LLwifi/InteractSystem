#include <IS_StructAndEnum.h>
#include "Library/IS_BlueprintFunctionLibrary.h"
#include "ActorComponent/BeInteractExtend/IS_BeInteractExtendBase.h"

bool FIS_InteractVerifyInfo::Verify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (DistanceVerify(InteractComponent, BeInteractComponent, FailText) &&
		BeInteractAngleVerifyCheck(InteractComponent, BeInteractComponent, FailText) &&
		InteractorsAngleVerifyCheck(InteractComponent, BeInteractComponent, FailText))
	{
		return true;
	}
	return false;
}

bool FIS_InteractVerifyInfo::DistanceVerify(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (InteractComponent && BeInteractComponent)//双方均有效
	{
		if (bOverride_InteractDistanceVerify)
		{
			FVector Dir = InteractComponent->GetOwner()->GetActorLocation() - BeInteractComponent->GetOwner()->GetActorLocation();
			float InteractDistance = UKismetMathLibrary::VSize(Dir);
			if (!InteractDistanceVerify.Contains(InteractDistance))//距离验证
			{
				FailText = InteractDistance_FailText;
				return false;
			}
		}
		return true;
	}
	return false;
}

bool FIS_InteractVerifyInfo::BeInteractAngleVerifyCheck(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (InteractComponent && BeInteractComponent)//双方均有效
	{
		if (bOverride_BeInteractAngleVerify)
		{
			FVector Dir = InteractComponent->GetOwner()->GetActorLocation() - BeInteractComponent->GetOwner()->GetActorLocation();
			float InteractAngle = UIS_BlueprintFunctionLibrary::GetAngleFromMyForwardAndTargetDir(BeInteractComponent->GetOwner()->GetActorForwardVector(), Dir);
			if (!BeInteractAngleVerify.Contains(InteractAngle))//角度验证
			{
				FailText = BeInteractAngle_FailText;
				return false;
			}
		}
		return true;
	}
	return false;
}

bool FIS_InteractVerifyInfo::InteractorsAngleVerifyCheck(UActorComponent* InteractComponent, UActorComponent* BeInteractComponent, FText& FailText)
{
	FailText = FText();
	if (InteractComponent && BeInteractComponent)//双方均有效
	{
		if (bOverride_InteractorsAngleVerify)
		{
			FVector Dir = BeInteractComponent->GetOwner()->GetActorLocation() - InteractComponent->GetOwner()->GetActorLocation();
			FVector ForwardVector = InteractComponent->GetOwner()->GetActorForwardVector();
			if (!bAngleFrontIsActorOrCamera)//要摄像机前方
			{
				APawn* Pawn = Cast<APawn>(InteractComponent->GetOwner());
				if (Pawn && Pawn->GetController())
				{
					FVector CameraLocation;
					FRotator CameraRotation;
					Pawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
					Dir = BeInteractComponent->GetOwner()->GetActorLocation() - CameraLocation;
					ForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);
				}
			}
			//float InteractAngle = UIS_BlueprintFunctionLibrary::GetAngleFromMyForwardAndTargetDir(ForwardVector, Dir);//该方法计算结果是0~360
			float InteractAngle = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Vector_CosineAngle2D(ForwardVector, Dir));//该方法计算结果是0~180（左右方向上的极限都是180）
			if (!InteractorsAngleVerify.Contains(InteractAngle))//角度验证
			{
				FailText = InteractorsAngle_FailText;
				return false;
			}
		}
		return true;
	}
	return false;
}

void FIS_BeInteractExtend::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
	RowName = InRowName;
	if (BeInteractExtendClass)
	{
		if (!BeInteractExtend || BeInteractExtend->GetClass() != BeInteractExtendClass)
		{
			BeInteractExtend = NewObject<UIS_BeInteractExtendBase>(const_cast<UDataTable*>(InDataTable), BeInteractExtendClass);
		}
	}
	else
	{
		BeInteractExtend = nullptr;
	}
}

void FIS_BeInteractInfo::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

	//if (BeInteractExtendClass)
	//{
	//	if (!BeInteractExtend || BeInteractExtend->GetClass() != BeInteractExtendClass)
	//	{
	//		BeInteractExtend = NewObject<UIS_BeInteractExtendBase>(const_cast<UDataTable*>(InDataTable), BeInteractExtendClass);
	//	}
	//}
	//else
	//{
	//	BeInteractExtend = nullptr;
	//}
}
