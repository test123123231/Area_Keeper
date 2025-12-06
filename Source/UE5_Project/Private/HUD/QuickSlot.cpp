#include "HUD/QuickSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "CoreMinimal.h"
#include "Item/ItemBase.h"


void UQuickSlot::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Error, TEXT("=== QuickSlot NativeConstruct ==="));
    UE_LOG(LogTemp, Error, TEXT("Img_Icon1: %s"), Img_Icon1 ? TEXT("OK") : TEXT("NULL!!!"));
    UE_LOG(LogTemp, Error, TEXT("Img_Icon2: %s"), Img_Icon2 ? TEXT("OK") : TEXT("NULL!!!"));
    // 슬롯 2개 초기화
    Slots.SetNum(2);
    for (FQuickSlotData& EachSlot : Slots)
    {
        EachSlot.ItemRef = nullptr;
        EachSlot.Icon = nullptr;
        EachSlot.bIsOccupied = false;
    }

    CurrentSlotIndex = 0;

    // 초기 아이콘 비우기 (UI에 아무것도 표시 안 함)
    if (Img_Icon1) Img_Icon1->SetVisibility(ESlateVisibility::Hidden);
    if (Img_Icon2) Img_Icon2->SetVisibility(ESlateVisibility::Hidden);
    UpdateSlotHighlight();
}

// 빈 슬롯 찾기
int32 UQuickSlot::FindEmptySlot() const
{
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (!Slots[i].bIsOccupied)
            return i;
    }
    return INDEX_NONE;
}

// 특정 아이템이 들어 있는 슬롯 인덱스 찾기
int32 UQuickSlot::FindSlotIndexByItem(AItemBase* TargetItem) const
{
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (Slots[i].ItemRef == TargetItem)
            return i;
    }
    return INDEX_NONE;
}

// UI 강제 새로고침
void UQuickSlot::ForceRefreshUI()
{
    if (Img_Icon1) Img_Icon1->InvalidateLayoutAndVolatility();
    if (Img_Icon2) Img_Icon2->InvalidateLayoutAndVolatility();
}

bool UQuickSlot::IsFull() const
{
    return Slots[0].bIsOccupied && Slots[1].bIsOccupied;
}

AItemBase* UQuickSlot::GetItemAt(int32 Index) const
{
    if (Slots.IsValidIndex(Index)) return Slots[Index].ItemRef;
    return nullptr;
}

void UQuickSlot::SetCurrentSlot(int32 NewIndex)
{
    if (!Slots.IsValidIndex(NewIndex)) return;
    CurrentSlotIndex = NewIndex;
    UpdateSlotHighlight();
}

void UQuickSlot::UpdateSlotHighlight()
{

    
    if (Img_Icon1 && Slots[0].bIsOccupied && Slots[0].Icon)
    {
        Img_Icon1->SetColorAndOpacity(FLinearColor::White);
        Img_Icon1->SetVisibility(ESlateVisibility::Visible);
    }
    else if (Img_Icon1)
    {
        Img_Icon1->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Img_Icon2 && Slots[1].bIsOccupied && Slots[1].Icon)
    {
        Img_Icon2->SetColorAndOpacity(FLinearColor::White);
        Img_Icon2->SetVisibility(ESlateVisibility::Visible);
    }
    else if (Img_Icon2)
    {
        Img_Icon2->SetVisibility(ESlateVisibility::Hidden);
    }

    // Border_Highlight1: 선택되면 노란색, 아니면 검은색
    if (Border_Highlight1)
    {
        Border_Highlight1->SetVisibility(ESlateVisibility::HitTestInvisible);
        Border_Highlight1->SetBrushColor(CurrentSlotIndex == 0 ? FLinearColor::Yellow : FLinearColor::Black);
    }

    // Border_Highlight2: 선택되면 노란색, 아니면 검은색
    if (Border_Highlight2)
    {
        Border_Highlight2->SetVisibility(ESlateVisibility::HitTestInvisible);
        Border_Highlight2->SetBrushColor(CurrentSlotIndex == 1 ? FLinearColor::Yellow : FLinearColor::Black);
    }
}

void UQuickSlot::UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon)
{
    
    UE_LOG(LogTemp, Error, TEXT("=== UpdateSlotIcon 호출 ==="));
    UE_LOG(LogTemp, Error, TEXT("SlotIndex: %d"), SlotIndex);
    UE_LOG(LogTemp, Error, TEXT("NewIcon: %s"), NewIcon ? *NewIcon->GetName() : TEXT("NULL"));

    if (SlotIndex == 0 && Img_Icon1)
    {
        if (NewIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(NewIcon);
            Brush.ImageSize = FVector2D(64.f, 64.f);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;

            Img_Icon1->SetBrush(Brush);
            Img_Icon1->SetColorAndOpacity(FLinearColor::White);
            Img_Icon1->SetOpacity(1.0f);
            Img_Icon1->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

            UE_LOG(LogTemp, Error, TEXT("Icon1 set!"));
        }
        else
        {
            Img_Icon1->SetVisibility(ESlateVisibility::Hidden);
        }
        
    }
    else if (SlotIndex == 1 && Img_Icon2)
    {
        if (NewIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(NewIcon);
            Brush.ImageSize = FVector2D(64.f, 64.f);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.Tiling = ESlateBrushTileType::NoTile;

            Img_Icon2->SetBrush(Brush);
            Img_Icon2->SetColorAndOpacity(FLinearColor::White);
            Img_Icon2->SetOpacity(1.0f);
            Img_Icon2->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

            UE_LOG(LogTemp, Error, TEXT("Icon2 set!"));
        }
        else
        {
            Img_Icon2->SetVisibility(ESlateVisibility::Hidden);
        }
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateSlotIcon FAILED - Widget is NULL or invalid index"));
    }
}


void UQuickSlot::AssignItemToSlot(int32 Index, AItemBase* NewItem)
{
    if (!Slots.IsValidIndex(Index) || !NewItem) return;
    
    Slots[Index].ItemRef = NewItem;
    Slots[Index].Icon = NewItem->ItemIcon;
    Slots[Index].bIsOccupied = true;

    UpdateSlotIcon(Index, NewItem->ItemIcon);
    UpdateSlotHighlight();
    ForceRefreshUI();

}

void UQuickSlot::RemoveItemAt(int32 Index)
{
    if (!Slots.IsValidIndex(Index)) return;

    Slots[Index].ItemRef = nullptr;
    Slots[Index].Icon = nullptr;
    Slots[Index].bIsOccupied = false;

    UpdateSlotIcon(Index, nullptr);
    ForceRefreshUI();

}
