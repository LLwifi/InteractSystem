// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include <Engine/DataTable.h>
#include "Kismet/KismetSystemLibrary.h"//EDrawDebugTrace需要
#include "Common/IS_GameplayTag.h"
#include "IS_Config.generated.h"

//UE的检测类型
UENUM(BlueprintType)
enum class EIS_TraceType :uint8
{
	Line = 0 UMETA(DisplayName = "Line线检测"),
	Sphere UMETA(DisplayName = "Sphere圆检测"),
	Box UMETA(DisplayName = "Box盒形检测"),
	Capsule UMETA(DisplayName = "Capsule胶囊体检测")
};

//检测位置类型
UENUM(BlueprintType)
enum class EIS_TraceLocationType :uint8
{
	OwnerLocation = 0 UMETA(DisplayName = "所有者位置"),
	//角度值为0表示前方 90右 180后 270左
	OwnerAngle UMETA(DisplayName = "从所有者位置的角度"),
	CameraLocation UMETA(DisplayName = "相机位置"),
	//角度值为0表示前方 90右 180后 270左
	CameraAngle UMETA(DisplayName = "从相机位置的角度"),
	WorldLocation UMETA(DisplayName = "世界位置"),
	//通过UIS_InteractComponent上的GetLocationFromTraceID获取最终值，注意使用该类型的枚举需要子类复现GetLocationFromTraceID函数
	DynamicGet UMETA(DisplayName = "动态获取")
};

//UE的被检测类型
UENUM(BlueprintType)
enum class EIS_BeTraceType :uint8
{
	Channel = 0 UMETA(DisplayName = "Channel通道"),
	Objects UMETA(DisplayName = "Objects对象类型"),
	Profile UMETA(DisplayName = "Profile预设")
};

/*检测信息
*/
USTRUCT(BlueprintType)
struct FIS_TraceInfo
{
	GENERATED_BODY()
public:
	FIS_TraceInfo() {}
	//构造默认的检测信息
	FIS_TraceInfo(FGameplayTag TagType, EIS_TraceType TraceType)
	{
		InteractTypeTag = TagType;
		InteractTraceType = TraceType;
		switch (InteractTraceType)
		{
		case EIS_TraceType::Line:
		{
			StartLocationType = EIS_TraceLocationType::CameraLocation;
			EndLocationType = EIS_TraceLocationType::CameraAngle;
			EndAngle = 0.0f;
			EndDistance = 1000.0f;
			break;
		}
		case EIS_TraceType::Sphere:
		{
			StartLocationType = EIS_TraceLocationType::OwnerLocation;
			EndLocationType = EIS_TraceLocationType::OwnerLocation;
			Radius = 1000.0f;
			break;
		}
		case EIS_TraceType::Box:
		{
			StartLocationType = EIS_TraceLocationType::CameraLocation;
			EndLocationType = EIS_TraceLocationType::CameraAngle;
			EndAngle = 0.0f;
			EndDistance = 1000.0f;
			HalfSize = FVector(50.0f);
			TraceRotatorType = EIS_TraceLocationType::CameraAngle;
			break;
		}
		case EIS_TraceType::Capsule:
		{
			StartLocationType = EIS_TraceLocationType::CameraLocation;
			EndLocationType = EIS_TraceLocationType::CameraAngle;
			EndAngle = 0.0f;
			EndDistance = 1000.0f;
			Radius = 50.0f;
			HalfHeight = 100.0f;
			break;
		}
		default:
			break;
		}
	}
public:
	//交互类型Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InteractTypeTag;
	//检测类型——使用什么样式进行检测
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_TraceType InteractTraceType = EIS_TraceType::Line;
	//被检测类型——哪些可以被检测到
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_BeTraceType BeTraceType = EIS_BeTraceType::Channel;

	//-------------------通用参数-------------------
	//开始位置的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_TraceLocationType StartLocationType = EIS_TraceLocationType::OwnerLocation;
	/*开始位置
	* 当StartLocationType != WorldLocation/世界位置时，该值都表示偏移值
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Start = FVector::Zero();
	//角度值为0表示前方 90右 180后 270左
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "StartLocationType == EIS_TraceLocationType::OwnerAngle || StartLocationType == EIS_TraceLocationType::CameraAngle"))
	float StartAngle = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "StartLocationType == EIS_TraceLocationType::OwnerAngle || StartLocationType == EIS_TraceLocationType::CameraAngle"))
	float StartDistance = 1000.0f;
	//结束位置的类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIS_TraceLocationType EndLocationType = EIS_TraceLocationType::OwnerLocation;
	/*结束位置
	* 当EndLocationType != WorldLocation/世界位置时，该值都表示偏移值
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector End = FVector::Zero();
	//角度值为0表示前方 90右 180后 270左
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "EndLocationType == EIS_TraceLocationType::OwnerAngle || EndLocationType == EIS_TraceLocationType::CameraAngle"))
	float EndAngle = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "EndLocationType == EIS_TraceLocationType::OwnerAngle || EndLocationType == EIS_TraceLocationType::CameraAngle"))
	float EndDistance = 1000.0f;
	//true针对复杂碰撞进行计算 false针对简单碰撞计算
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceComplex = false;
	//要忽略的目标（该值通常动态赋予）
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIgnoreSelf = true;

	//-------------------Line的相关参数（Line使用通用参数即可）-------------------

	//-------------------Sphere的相关参数-------------------
	//半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractTraceType == EIS_TraceType::Sphere || InteractTraceType == EIS_TraceType::Capsule"))
	float Radius = 100.0f;

	//-------------------Capsule的相关参数-------------------
	//胶囊体中心到半球末端顶部的距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractTraceType == EIS_TraceType::Capsule"))
	float HalfHeight = 50.0f;

	//-------------------Box的相关参数-------------------
	//盒体中心沿每个轴的距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractTraceType == EIS_TraceType::Box"))
	FVector HalfSize = FVector::Zero();
	/*盒体的朝向的类型
	* 使用EIS_TraceLocationType枚举来选择，实际上只有带Angle的选择，即：
	* OwnerAngle：拥有者的某个角度
	* CameraAngle：相机的某个角度
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractTraceType == EIS_TraceType::Box"))
	EIS_TraceLocationType TraceRotatorType = EIS_TraceLocationType::CameraAngle;
	/*盒体的朝向
	* 当TraceRotatorType != WorldRotator/世界方向时，该值都表示偏移值
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "InteractTraceType == EIS_TraceType::Box"))
	FRotator Orientation = FRotator::ZeroRotator;

	//-------------------被检测类型-------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "BeTraceType == EIS_BeTraceType::Channel"))
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "BeTraceType == EIS_BeTraceType::Profile"))
	FName ProfileName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "BeTraceType == EIS_BeTraceType::Objects"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	//-------------------射线调试信息-------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeBug")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeBug")
	FLinearColor TraceColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeBug")
	FLinearColor TraceHitColor = FLinearColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DeBug")
	float DrawTime = 1.0f;
};

/*交互检测信息
*/
USTRUCT(BlueprintType)
struct FIS_InteractTypeInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	FIS_InteractTypeInfo() {}
	//构造默认的检测信息
	FIS_InteractTypeInfo(FGameplayTag TagType, EIS_TraceType TraceType)
	{
		InteractTypeTag = TagType;
		InteractTraceInfo = FIS_TraceInfo(TagType, TraceType);
		BlockTraceInfo = FIS_TraceInfo(TagType, TraceType);
	}

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
		if (InteractTypeTag.IsValid())
		{
			InteractTraceInfo.InteractTypeTag = InteractTypeTag;
			BlockTraceInfo.InteractTypeTag = InteractTypeTag;
		}
	}
public:
	//交互类型Tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "InteractType"))
	FGameplayTag InteractTypeTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIS_TraceInfo InteractTraceInfo;

	/*是否开启阻挡判断
	* 开启后在检测到目标后会再次发射一根在开始到结束两点间的射线，该射线用于检测阻挡物
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsBlockCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditConditionHides, EditCondition = "bIsBlockCheck"))
	FIS_TraceInfo BlockTraceInfo;
};

/**
 * 编辑器下的通用Task配置
 */
UCLASS(config = IS_Config, defaultconfig)
class INTERACTSYSTEM_API UIS_Config : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	virtual FName GetCategoryName() const final override
	{
		return FName("GameEditorConfig");
	}
	static UIS_Config* GetInstance(){ return GetMutableDefault<UIS_Config>(); }
	UFUNCTION(BlueprintPure, BlueprintCallable)
		static UIS_Config* GetIS_Config() { return GetInstance(); }

public:

	//默认的交互提示面板
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractTipPanelClass;

	//交互描边的材质
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UMaterial> DefaultInteractOutLineMaterial;

	//默认交互动作
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftObjectPtr<UAnimMontage>> DefaultBeInteractMontage = {{FName("Default"),nullptr}};

	//交互音效
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSoftObjectPtr<USoundBase>> DefaultBeInteractSound = { {FName("Default"),nullptr} };

	//交互配置表
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDataTable> BeInteractInfoDataTable;

	//交互扩展配置表
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDataTable> BeInteractExtendDataTable;

	//交互检测信息配置表
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDataTable> InteractTraceTypeInfoDataTable;

	//交互跟踪类型映射<ID,跟踪检测信息>
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FIS_InteractTypeInfo> InteractTraceTypeMapping = { {TAG_InteractType_CameraTrace,FIS_InteractTypeInfo(TAG_InteractType_CameraTrace, EIS_TraceType::Line)}, {TAG_InteractType_SphereTrace,FIS_InteractTypeInfo(TAG_InteractType_SphereTrace, EIS_TraceType::Sphere)} };
};
