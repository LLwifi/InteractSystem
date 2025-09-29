// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyTypeCustomization/IS_BeInteractInfoHandleCustomization.h"
#include <IDetailChildrenBuilder.h>
#include <Widgets/Input/SEditableTextBox.h>
#include <Misc/MessageDialog.h>
#include <Misc/PackageName.h>
#include <FileHelpers.h>
#include <DetailWidgetRow.h>
#include "SSearchableComboBox.h"
#include <PropertyCustomizationHelpers.h>
#include "../../../InteractSystem/Public/Common/IS_Config.h"

#define LOCTEXT_NAMESPACE "IS_BeInteractInfoHandleCustomization"

void IIS_BeInteractInfoHandleCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ];
}

void IIS_BeInteractInfoHandleCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    //通过名称拿结构体变量
    RowNameHandle = PropertyHandle->GetChildHandle("RowName");

    void* ValuePtr;
    PropertyHandle->GetValueData(ValuePtr);
    if (ValuePtr != nullptr)
    {
        IS_BeInteractInfoHandle = (FIS_BeInteractInfoHandle*)ValuePtr;
    }

    //Refresh(RowNames, ResourceNameOrIndexs);
    
    UDataTable* DT = UIS_Config::GetInstance()->BeInteractInfoDataTable.LoadSynchronous();
    TArray<FName> DTRowName;
    if (DT)
    {
        DTRowName = DT->GetRowNames();
        for (FName& name : DTRowName)
        {
            RowNames.Add(MakeShareable(new FString(name.ToString())));
        }
    }

    //slate
    ChildBuilder.AddCustomRow(FText())
        [
            SNew(SVerticalBox)          
            + SVerticalBox::Slot()
            [
                SAssignNew(SearchableComboBox_RowName, SSearchableComboBox)
                    .OptionsSource(&RowNames)//所有选项
                    .OnGenerateWidget(this, &IIS_BeInteractInfoHandleCustomization::OnGenerateWidget_RowName)//每个下拉选项的样式通过函数构造
                    .OnSelectionChanged(this, &IIS_BeInteractInfoHandleCustomization::OnSelectionChanged_RowName)//改变选择的回调
                    [
                        SAssignNew(ComboBox_Name_Text, STextBlock)
                            .Text(FText::FromString(IS_BeInteractInfoHandle->RowName.ToString()))
                    ]
            ]
        ];
}

void IIS_BeInteractInfoHandleCustomization::OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    RowNameHandle->SetValue(FName(*InItem.Get()));
    ComboBox_Name_Text->SetText(FText::FromString(*InItem));
    IS_BeInteractInfoHandle->RowName = FName(*InItem.Get());

    //Refresh(RowNames, ResourceNameOrIndexs);
}

TSharedRef<SWidget> IIS_BeInteractInfoHandleCustomization::OnGenerateWidget_RowName(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

//----------------------------------------------------------------------------------------------

void IIS_BeInteractExtendHandleCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ];
}

void IIS_BeInteractExtendHandleCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    //通过名称拿结构体变量
    RowNameHandle = PropertyHandle->GetChildHandle("RowName");

    void* ValuePtr;
    PropertyHandle->GetValueData(ValuePtr);
    if (ValuePtr != nullptr)
    {
        IS_BeInteractExtendHandle = (FIS_BeInteractExtendHandle*)ValuePtr;
    }

    //Refresh(RowNames, ResourceNameOrIndexs);

    UDataTable* DT = UIS_Config::GetInstance()->BeInteractExtendDataTable.LoadSynchronous();
    TArray<FName> DTRowName;
    if (DT)
    {
        DTRowName = DT->GetRowNames();
        for (FName& name : DTRowName)
        {
            RowNames.Add(MakeShareable(new FString(name.ToString())));
        }
    }

    //slate
    ChildBuilder.AddCustomRow(FText())
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SAssignNew(SearchableComboBox_RowName, SSearchableComboBox)
                        .OptionsSource(&RowNames)//所有选项
                        .OnGenerateWidget(this, &IIS_BeInteractExtendHandleCustomization::OnGenerateWidget_RowName)//每个下拉选项的样式通过函数构造
                        .OnSelectionChanged(this, &IIS_BeInteractExtendHandleCustomization::OnSelectionChanged_RowName)//改变选择的回调
                        [
                            SAssignNew(ComboBox_Name_Text, STextBlock)
                                .Text(FText::FromString(IS_BeInteractExtendHandle->RowName.ToString()))
                        ]
                ]
        ];
}

void IIS_BeInteractExtendHandleCustomization::OnSelectionChanged_RowName(TSharedPtr<FString> InItem, ESelectInfo::Type InSelectionInfo)
{
    RowNameHandle->SetValue(FName(*InItem.Get()));
    ComboBox_Name_Text->SetText(FText::FromString(*InItem));
    IS_BeInteractExtendHandle->RowName = FName(*InItem.Get());

    //Refresh(RowNames, ResourceNameOrIndexs);
}

TSharedRef<SWidget> IIS_BeInteractExtendHandleCustomization::OnGenerateWidget_RowName(TSharedPtr<FString> InItem)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InItem));
}

#undef LOCTEXT_NAMESPACE