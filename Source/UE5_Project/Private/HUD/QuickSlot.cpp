#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "HUD/QuickSlot.h"
#include "Item/ItemBase.h"


void UQuickSlot::NativeConstruct()
{
    Super::NativeConstruct();

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
    if (Img_Icon1) Img_Icon1->SetBrushFromTexture(nullptr);
    if (Img_Icon2) Img_Icon2->SetBrushFromTexture(nullptr);
    UpdateSlotHighlight();
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
    FLinearColor NormalColor = FLinearColor::White;
    FLinearColor HighlightColor = FLinearColor::Yellow;

    if (Img_Icon1)
        Img_Icon1->SetColorAndOpacity(CurrentSlotIndex == 0 ? HighlightColor : NormalColor);
    if (Img_Icon2)
        Img_Icon2->SetColorAndOpacity(CurrentSlotIndex == 1 ? HighlightColor : NormalColor);
}

void UQuickSlot::UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon)
{
    if (SlotIndex == 0 && Img_Icon1)
        Img_Icon1->SetBrushFromTexture(NewIcon);
    else if (SlotIndex == 1 && Img_Icon2)
        Img_Icon2->SetBrushFromTexture(NewIcon);
}


void UQuickSlot::AssignItemToSlot(int32 Index, AItemBase* NewItem)
{
    if (!Slots.IsValidIndex(Index) || !NewItem) return;

    Slots[Index].ItemRef = NewItem;
    Slots[Index].Icon = NewItem->ItemIcon;
    Slots[Index].bIsOccupied = true;

    UpdateSlotIcon(Index, NewItem->ItemIcon);
    ForceRefreshUI();

    UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Assigned %s to slot %d"), *NewItem->GetName(), Index);
}

void UQuickSlot::RemoveItemAt(int32 Index)
{
    if (!Slots.IsValidIndex(Index)) return;

    Slots[Index].ItemRef = nullptr;
    Slots[Index].Icon = nullptr;
    Slots[Index].bIsOccupied = false;

    UpdateSlotIcon(Index, nullptr);
    ForceRefreshUI();

    UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Cleared slot %d"), Index);
}
