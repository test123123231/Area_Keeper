#include "Character/QuickSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

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

/*void UQuickSlot::AddItemToEmptySlot(AItemBase* NewItem)
{
    if (!NewItem)
    {
        return;
    }

    // 슬롯 순회하며 빈 공간 찾기
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (!Slots[i].bIsOccupied)
        {
            Slots[i].ItemRef = NewItem;
            Slots[i].Icon = NewItem->ItemIcon;
            Slots[i].bIsOccupied = true;



            UpdateSlotIcon(i, NewItem->ItemIcon);
            ForceRefreshUI();

            UE_LOG(LogTemp, Warning, TEXT("QuickSlot: %s added to slot %d"),
                *NewItem->GetName(), i);
            return;
        }
    }

    // 여기까지 왔다는 건 슬롯이 모두 찼다는 의미
    //UE_LOG(LogTemp, Warning, TEXT("QuickSlot: All slots are full!"));
}*/
/*
// 슬롯이 모두 찼는지 판단
bool UQuickSlot::IsFull() const
{
    //return Slots.Num() == 2 && Slots[0].bIsOccupied && Slots[1].bIsOccupied;
    return Slots[0].bIsOccupied && Slots[1].bIsOccupied;
}


// 아이콘 업데이트 (공통 함수)
void UQuickSlot::UpdateSlotIcon(int32 SlotIndex, UTexture2D* NewIcon)
{
    if (SlotIndex == 0 && Img_Icon1)
        Img_Icon1->SetBrushFromTexture(NewIcon);
    else if (SlotIndex == 1 && Img_Icon2)
        Img_Icon2->SetBrushFromTexture(NewIcon);
}

void UQuickSlot::SetCurrentSlot(int32 NewIndex)
{
    if (NewIndex < 0 || NewIndex >= Slots.Num())
        return;

    CurrentSlotIndex = NewIndex;
    UpdateSlotHighlight();

    UE_LOG(LogTemp, Warning, TEXT("Current QuickSlot selected: %d"), CurrentSlotIndex);
}

AItemBase* UQuickSlot::GetItemAt(int32 Index) const
{
    //if (Index < 0 || Index >= Slots.Num()) return nullptr;
    //return Slots[Index].ItemRef;
    if (Slots.IsValidIndex(Index)) return Slots[Index].ItemRef;
    return nullptr;
}

// 하이라이트 색상 변경 (선택된 슬롯만 노랗게)
void UQuickSlot::UpdateSlotHighlight()
{
    FLinearColor NormalColor = FLinearColor::White;
    FLinearColor HighlightColor = FLinearColor::Yellow;

    if (Img_Icon1)
        Img_Icon1->SetColorAndOpacity(CurrentSlotIndex == 0 ? HighlightColor : NormalColor);

    if (Img_Icon2)
        Img_Icon2->SetColorAndOpacity(CurrentSlotIndex == 1 ? HighlightColor : NormalColor);
}
*/

/*void UQuickSlot::RemoveItem(AItemBase* TargetItem)
{
    if (!TargetItem) return;

    int32 Index = FindSlotIndexByItem(TargetItem);
    if (Index != INDEX_NONE)
    {
        Slots[Index].ItemRef = nullptr;
        Slots[Index].Icon = nullptr;
        Slots[Index].bIsOccupied = false;

        UpdateSlotIcon(Index, nullptr);
        ForceRefreshUI();
        UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Removed %s from slot %d"), *TargetItem->GetName(), Index);
    }

    /*for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (Slots[i].ItemRef == TargetItem)
        {
            Slots[i].ItemRef = nullptr;
            Slots[i].Icon = nullptr;
            Slots[i].bIsOccupied = false;

            // UI 아이콘 제거
            UpdateSlotIcon(i, nullptr);
            UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Removed item from slot %d"), i);
            return;
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("QuickSlot: RemoveItem failed (item not found)"));
}*/

/*void UQuickSlot::ReplaceItem(AItemBase* OldItem, AItemBase* NewItem)
{
    if (!NewItem) return;

    int32 Index = (OldItem) ? FindSlotIndexByItem(OldItem) : 0;
    if (Index == INDEX_NONE) Index = 0;

    Slots[Index].ItemRef = NewItem;
    Slots[Index].Icon = NewItem->ItemIcon;
    Slots[Index].bIsOccupied = true;

    UpdateSlotIcon(Index, NewItem->ItemIcon);
    ForceRefreshUI();

    /*for (int32 i = 0; i < Slots.Num(); i++)
    {
        if (Slots[i].ItemRef == OldItem)
        {
            Slots[i].ItemRef = NewItem;
            Slots[i].Icon = NewItem->ItemIcon;
            Slots[i].bIsOccupied = true;

            UpdateSlotIcon(i, NewItem->ItemIcon);
            UE_LOG(LogTemp, Warning, TEXT("QuickSlot: Replaced item in slot %d"), i);
            return;
        }
    }

    //UE_LOG(LogTemp, Warning, TEXT("QuickSlot: ReplaceItem failed (old item not found)"));
}*/


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
