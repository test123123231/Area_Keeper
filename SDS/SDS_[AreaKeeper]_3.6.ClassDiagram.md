## 3.6 UI System
본 절은 C++ 기반 위젯과 블루프린트 위젯을 포함한 모든 사용자 인터페이스(UI)를 다룬다. 인게임 정보를 표시하는 UHUDWidget과 아이템 슬롯을 관리하는 UQuickSlot을 기술한다. 또한, UMainMenuWidget, UPauseMenuWidget, UTalismanWidget, UOptionMenuWidget, UControlsMenuWidget, UConfirmMainMenuWidget, UConfirmChangesWidget, UGameOverWidget, UGameClearWidget의 상세 내용을 기술한다.

본 절의 클래스 다이어그램에는 다음 시스템의 클래스들이 참조로 포함될 수 있으며, 해당 클래스들의 상세 기술서는 명시된 세부 절에 작성되어 있다.
- 3.1 Core Game System: APlayerCharacterController, AMainMenuPlayerController, AAreaKeeperGameMode, AAreaKeeperGameState
- 3.2 Character: APlayerCharacter
- 3.4 Item & Interaction System: AItemBase
- 3.7 Settings & Saving System: USettingSubsystem


![image](image/Class6_UISystem.png)
***

### UHUDWidget

#### 클래스 개요

인게임 플레이 중에 표시되는 메인 HUD 위젯이다. `UUserWidget`을 상속받는다. `APlayerCharacterController`와 `AAreaKeeperGameState`로부터 호출되어 체력, 지방(Amulet), 시간, 상호작용 텍스트 등 실시간 정보를 화면에 표시한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| HealthText | class UTextBlock* | protected | 체력을 표시하는 텍스트 블록이다. |
| AmuletText | class UTextBlock* | protected | 지방(Amulet) 개수를 표시하는 텍스트 블록이다. |
| CenterText | class UTextBlock* | protected | 화면 중앙에 텍스트(예: 충전 상태)를 표시하는 텍스트 블록이다. |
| TimeText | class UTextBlock* | protected | 화면 상단에 텍스트(예: 남은 시간)를 표시하는 텍스트 블록이다. |
| PenaltyText | class UTextBlock* | protected | 누적 패널티 수치를 표시하는 텍스트 블록이다. |


#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| UpdateHealth | void | public | HealthText의 내용을 CurrentHealth 값으로 갱신한다. (BlueprintCallable) |
| UpdateAmulet | void | public | AmuletText의 내용을 CurrentAmulet 값으로 갱신한다. (BlueprintCallable) |
| UpdatePenaltyText | void | public | PenaltyText의 내용을 CurrentPenalty 값으로 갱신한다. (BlueprintCallable) |
| ShowCenterText | void | public | CenterText를 보이도록 설정한다. (BlueprintCallable) |
| HideCenterText | void | public | CenterText를 숨기도록 설정한다. (BlueprintCallable) |
| UpdateCenterText | void | public | CenterText의 내용을 Text로 갱신한다. (BlueprintCallable) |
| ShowTimeText | void | public | TimeText를 보이도록 설정한다. (BlueprintCallable) |
| HideTimeText | void | public | TimeText를 숨기도록 설정한다. (BlueprintCallable) |
| UpdateTimeText | void | public | TimeText의 내용을 Text로 갱신한다. (BlueprintCallable) |

***

### UQuickSlot

#### 클래스 개요

인게임 퀵슬롯 UI 위젯이다. `UUserWidget`을 상속받는다. `APlayerCharacterController`에 의해 생성되며, `APlayerCharacter`가 참조하여 아이템을 관리한다. 2개의 슬롯(`Slots`) 데이터를 관리하며, 아이템을 슬롯에 배정(`AssignItemToSlot`)하거나 제거(`RemoveItemAt`)하고, 현재 선택된 슬롯(`CurrentSlotIndex`)의 하이라이트(`UpdateSlotHighlight`)를 갱신한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Slots | TArray<FQuickSlotData> | private | 2개의 슬롯 데이터를 저장하는 배열이다.  |
| CurrentSlotIndex | int32 | private | 현재 선택된 슬롯 인덱스(0 또는 1)이다. (기본값: 0) |
| Img_Icon1 | UImage* | private | 1번 슬롯의 아이콘 이미지이다. |
| Img_Icon2 | UImage* | private | 2번 슬롯의 아이콘 이미지이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| SetCurrentSlot | void | public | CurrentSlotIndex를 NewIndex로 설정하고 UpdateSlotHighlight를 호출한다. |
| GetItemAt | AItemBase* | public | Index에 해당하는 슬롯의 ItemRef를 반환한다. |
| IsFull | bool | public | 2개의 슬롯이 모두 채워졌는지 여부를 반환한다. |
| FindSlotIndexByItem | int32 | public | TargetItem이 있는 슬롯의 인덱스를 찾아 반환한다. |
| UpdateSlotIcon | void | public | SlotIndex에 해당하는 이미지(Img_Icon1/2)의 텍스처를 NewIcon으로 설정한다. |
| ForceRefreshUI | void | public | 슬롯 이미지의 레이아웃을 강제로 갱신한다. |
| GetCurrentSlotIndex | int32 | public | CurrentSlotIndex 값을 반환한다. |
| AssignItemToSlot | void | public | Index 슬롯의 FQuickSlotData에 NewItem 정보를 저장하고 아이콘을 갱신한다. |
| RemoveItemAt | void | public | Index 슬롯의 FQuickSlotData를 비우고 아이콘을 nullptr로 갱신한다. |
| NativeConstruct | void | protected | Slots 배열을 2개로 초기화하고 CurrentSlotIndex를 0으로 설정하며 아이콘을 정리한다. (Override) |

***

### UGameOverWidget

#### 클래스 개요

게임 오버 화면을 관리하는 C++ 기반 위젯이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 '재도전' 및 '메인 메뉴' 버튼에 C++ 함수를 바인딩하며, `InitializeWidget`을 통해 게임 오버 사유(사망 또는 패널티)를 받아 텍스트로 표시한다.


#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| InitializeWidget | void | public | 게임 오버 사유(bPlayerDied)를 받아 Text_GameOverReason의 텍스트를 설정한다. (BlueprintCallable) |

***

### UGameClearWidget

#### 클래스 개요

게임 클리어 시 표시되는 위젯이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 '메인 메뉴' 버튼을 바인딩하며, `InitializeWidget`을 통해 최종 게임 결과(패널티 스택, 해결한 이상현상 등)를 받아 텍스트로 표시한다.

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| InitializeWidget | void | public | PenaltyStack, AnomaliesSolved, ChasingHits 값을 받아 UI 텍스트를 설정한다. (BlueprintCallable) |

***

### UMainMenuWidget

#### 클래스 개요

메인 메뉴(WBP_MainMenu)의 C++ 기반 클래스이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 `PlayerControllerRef`를 캐시하고 '시작', '설정', '종료' 버튼의 `OnClicked` 델리게이트를 C++ 함수에 바인딩한다. '시작' 시 `GameLevelName` 레벨을 열고, '설정' 시 `OptionMenuWidgetClass` 인스턴스를 생성하여 표시한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Button_Start | TObjectPtr<UButton> | private | '시작하기' 버튼의 참조이다. |
| Button_Option | TObjectPtr<UButton> | private | '설정' 버튼의 참조이다. |
| Button_Exit | TObjectPtr<UButton> | private | '종료' 버튼의 참조이다. |
| GameLevelName | FName | private | '시작하기' 버튼 클릭 시 열릴 레벨 이름이다. (EditDefaultsOnly) |
| OptionMenuWidgetClass | TSubclassOf<UOptionMenuWidget> | private | '설정' 버튼 클릭 시 열릴 옵션 메뉴 위젯 클래스이다. (EditDefaultsOnly) |
| OptionMenuInstance | TObjectPtr<UOptionMenuWidget> | private | OnOptionClicked 시 생성/캐시되는 옵션 메뉴 위젯의 인스턴스이다. |
| PlayerControllerRef | TObjectPtr<APlayerCharacterController> | private | NativeConstruct에서 캐시되는 플레이어 컨트롤러 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| NativeConstruct | void | protected | 버튼 이벤트 바인딩 |
| OnStartClicked | void | private | 게임 레벨로 이동 |
| OnOptionClicked | void | private | 옵션 메뉴 표시 |
| OnExitClicked | void | private | 게임 종료 |

***

### UPauseMenuWidget

#### 클래스 개요

일시 정지 메뉴(WBP_PauseMenu)의 C++ 기반 클래스이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 `PlayerControllerRef`를 캐시하고 모든 버튼(`Button_Resume` 등)의 `OnClicked` 델리게이트를 함수(예: `OnResumeClicked`)에 바인딩한다. 각 버튼 클릭 시 `APlayerCharacterController`의 함수(예: `ClosePauseMenu`)를 호출하거나, 하위 메뉴 위젯(`OptionMenuWidgetClass` 등)의 인스턴스를 생성하여 뷰포트에 표시하고 입력 모드를 변경한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| PlayerControllerRef | TObjectPtr<APlayerCharacterController> | private | NativeConstruct에서 캐시되는 플레이어 컨트롤러 참조이다. |
| OptionMenuInstance | TObjectPtr<UOptionMenuWidget> | private | 생성된 옵션 메뉴 위젯의 인스턴스이다. (VisibleInstanceOnly) |
| ControlsMenuInstance | TObjectPtr<UControlsMenuWidget> | private | 생성된 조작법 메뉴 위젯의 인스턴스이다. (VisibleInstanceOnly) |
| ConfirmMainMenuPopupInstance | TObjectPtr<UConfirmMainMenuWidget> | private | 생성된 메인 메뉴 확인 팝업의 인스턴스이다. (VisibleInstanceOnly) |
| MainMenuLevelName | FName | private | '메인 메뉴로' 이동 시 로드할 레벨의 이름이다. (기본값: MainMenu) (EditDefaultsOnly) |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| NativeConstruct | void | protected | PlayerControllerRef를 캐시하고 모든 Button의 OnClicked 델리게이트를 C++ 함수에 바인딩한다. (Override) |
| OnResumeClicked | void | private | '계속하기' 버튼 클릭 시 PlayerCharacterControllerRef->ClosePauseMenu()를 호출한다. |
| OnOptionClicked | void | private | '설정' 버튼 클릭 시 OptionMenuInstance를 생성/표시하고 입력 모드를 변경하며 자신을 숨긴다. |
| OnControlsClicked | void | private | '조작법' 버튼 클릭 시 ControlsMenuInstance를 생성/표시하고 입력 모드를 변경하며 자신을 숨긴다. |
| OnMainMenuClicked | void | private | '메인 메뉴로' 버튼 클릭 시 ConfirmMainMenuPopupInstance를 생성/표시하고 입력 모드를 변경한다. |
| OnExitClicked | void | private | '게임 종료' 버튼 클릭 시 UKismetSystemLibrary::QuitGame()을 호출한다. |

***

### UOptionMenuWidget

#### 클래스 개요

옵션 메뉴(WBP_OptionMenu)의 C++ 기반 클래스이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 `USettingSubsystem`의 참조를 얻고 모든 UI 컴포넌트(슬라이더, 콤보박스, 버튼)의 델리게이트를 함수에 바인딩한다. `FDisplaySettings` 구조체를 이용해 임시(Staged), 저장된(Saved), 기본(Default) 설정을 관리하며, `UpdateUIState`를 통해 버튼 활성화 상태를 갱신한다. `OnOKClicked` 또는 `NativeOnKeyDown`(ESC) 호출 시, 변경 사항이 있으면 `ConfirmChangesPopupClass` 팝업을 띄운다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| SettingSubsystem | TObjectPtr<USettingSubsystem> | private | USettingSubsystem의 캐시된 참조이다. |
| PlayerControllerRef | TObjectPtr<APlayerCharacterController> | private | APlayerController의 캐시된 참조이다. |
| StagedSettings | FDisplaySettings | private | UI에서 변경되었으나 아직 적용/저장되지 않은 임시 설정 값이다. |
| SavedSettings | FDisplaySettings | private | 현재 디스크에 저장된(로드된) 설정 값이다. |
| DefaultSettings | FDisplaySettings | private | 게임의 하드코딩된 기본 설정 값이다. |
| ConfirmChangesPopupClass | TSubclassOf<UConfirmChangesWidget> | private | '변경사항 저장' 팝업 위젯의 블루프린트 클래스이다. (EditDefaultsOnly) |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| InitializeSettings | void | private | SettingSubsystem에서 SavedSettings와 DefaultSettings를 로드하고 UI를 초기화한다. |
| PopulateUIFromStagedSettings | void | private | StagedSettings의 데이터를 실제 UI 컴포넌트(슬라이더, 콤보박스)에 반영한다. |
| ShowConfirmChangesPopup | void | private | ConfirmChangesPopupClass 위젯을 생성하여 뷰포트에 추가하고 포커스를 설정한다. |
| OnApplyClicked | void | private | '적용' 버튼 클릭 시. StagedSettings를 SettingSubsystem에 적용하고 저장한다. |
| OnResetAllClicked | void | private | '초기화' 버튼 클릭 시. StagedSettings를 DefaultSettings로 되돌리고 UI를 갱신한다. |
| OnOKClicked | void | private | '확인' 버튼 클릭 시. 변경 사항이 있으면 ShowConfirmChangesPopup을, 없으면 CloseMenu(false)를 호출한다. |
| OnCancelClicked | void | private | '취소' 버튼 클릭 시. CloseMenu(false)를 호출한다. |
| OnConfirmChangesYes | void | public | ConfirmChangesPopupClass의 '예' 버튼 클릭 시 호출된다. CloseMenu(true)를 실행한다. (BlueprintCallable) |
| OnConfirmChangesNo | void | public | ConfirmChangesPopupClass의 '아니오' 버튼 클릭 시 호출된다. CloseMenu(false)를 실행한다. (BlueprintCallable) |

***

### UControlsMenuWidget

#### 클래스 개요

'조작법' 메뉴의 클래스이다. `UUserWidget`을 상속받는다. `UPauseMenuWidget`에 의해 생성되며, `NativeConstruct`에서 '뒤로가기' 버튼을 바인딩한다. ESC 키(`NativeOnKeyDown`) 또는 '뒤로가기' 버튼 클릭 시 `CloseMenu`를 호출하여 뷰포트에서 자신을 제거하고 부모 메뉴(`ParentMenu`)로 복귀한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Button_Back | TObjectPtr<UButton> | private | '뒤로가기' 버튼의 참조이다. |
| ParentMenu | TObjectPtr<UUserWidget> | private | 이 위젯을 생성한 부모 위젯(UPauseMenuWidget)의 참조이다. |
| PlayerControllerRef | TObjectPtr<APlayerCharacterController> | private | NativeConstruct에서 캐시되는 플레이어 컨트롤러 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| SetParentMenu | void | public | 이 위젯을 연 부모 메뉴(ParentMenu)를 설정한다. |
| NativeConstruct | void | protected | bIsFocusable을 true로 설정하고, PlayerControllerRef를 캐시하며 Button_Back의 OnClicked 델리게이트를 바인딩한다. (Override) |
| NativeOnKeyDown | FReply | protected | ESC 키 입력을 감지하여 CloseMenu를 호출하고 입력을 처리(Handled)한다. (Override) |
| NativeOnMouseButtonDown | FReply | protected | 위젯 배경 클릭 시 키보드 포커스를 유지하고 입력을 처리(Handled)한다. (Override) |
| OnBackClicked | void | private | '뒤로가기' 버튼 클릭 시 CloseMenu()를 호출한다. |
| CloseMenu | void | private | ParentMenu를 다시 보이게 하고 입력 모드를 복원한 뒤, 자신을 뷰포트에서 제거한다. |

***

### UConfirmMainMenuWidget

#### 클래스 개요

'메인 메뉴로 돌아가기' 확인 팝업의 C++ 기반 클래스이다. `UUserWidget`을 상속받는다. `UPauseMenuWidget`에 의해 생성되며, `NativeConstruct`에서 '예'/'아니오' 버튼을 바인딩한다. '예' 클릭 시 `MainMenuLevelName` 레벨을 로드하고, '아니오' 클릭 시 `CloseMenu`를 호출하여 팝업을 닫고 부모 메뉴로 복귀한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Button_Yes | TObjectPtr<UButton> | private | '예' 버튼의 참조이다. |
| Button_No | TObjectPtr<UButton> | private | '아니오' 버튼의 참조이다. |
| MainMenuLevelName | FName | private | '예' 버튼 클릭 시 로드할 레벨의 이름이다. (기본값: MainMenu) (EditDefaultsOnly) |
| ParentMenu | TObjectPtr<UUserWidget> | private | 이 팝업을 생성한 부모 위젯(UPauseMenuWidget)의 참조이다. |
| PlayerControllerRef | TObjectPtr<APlayerCharacterController> | private | NativeConstruct에서 캐시되는 플레이어 컨트롤러 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| SetParentMenu | void | public | 이 팝업을 연 부모 메뉴(ParentMenu)를 설정한다. |
| NativeConstruct | void | protected | PlayerControllerRef를 캐시하고 Button_Yes, Button_No의 OnClicked 델리게이트를 C++ 함수에 바인딩한다. (Override) |
| OnYesClicked | void | private | '예' 버튼 클릭 시 호출된다. 게임을 재개하고 MainMenuLevelName 레벨을 로드한다. |
| OnNoClicked | void | private | '아니오' 버튼 클릭 시 호출된다. CloseMenu()를 호출한다. |
| CloseMenu | void | private | ParentMenu를 다시 보이게 하고 입력 모드를 복원한 뒤, 자신을 뷰포트에서 제거한다. |

***

### UConfirmChangesWidget

#### 클래스 개요

`UOptionMenuWidget`에서 저장되지 않은 변경 사항이 있을 때 띄우는 '예/아니오' 확인 팝업 위젯이다. `UUserWidget`을 상속받는다. `NativeConstruct`에서 '예'/'아니오' 버튼을 바인딩하며, 클릭 시 `OptionMenuOwner`의 해당 함수(예: `OnConfirmChangesYes`)를 호출하고 자신을 닫는다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Button_Yes | TObjectPtr<UButton> | private | '예' 버튼의 참조이다. |
| Button_No | TObjectPtr<UButton> | private | '아니오' 버튼의 참조이다. |
| OptionMenuOwner | TObjectPtr<UOptionMenuWidget> | private | 이 팝업을 생성한 UOptionMenuWidget의 캐시된 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| SetParentMenu | void | public | 이 팝업을 연 부모 위젯(OptionMenuOwner)을 설정한다. |
| NativeConstruct | void | protected | Button_Yes, Button_No의 OnClicked 델리게이트를 C++ 함수에 바인딩한다. (Override) |
| OnYesClicked | void | private | '예' 버튼 클릭 시 OptionMenuOwner->OnConfirmChangesYes()를 호출하고 팝업을 닫는다. |
| OnNoClicked | void | private | '아니오' 버튼 클릭 시 OptionMenuOwner->OnConfirmChangesNo()를 호출하고 팝업을 닫는다. |

***

### UTalismanWidget

#### 클래스 개요

'소지' UI (지방 선택)를 위한 C++ 기반 위젯 클래스이다. `UUserWidget`을 상속받는다. `APlayerCharacterController`에 의해 `OpenTalismanUI`가 호출될 때 생성되며, `NativeConstruct`에서 플레이어 참조를 캐시하고 각 범주 버튼의 `OnClicked` 델리게이트에 함수를 바인딩한다. 버튼 클릭 시 `HandleSelection`을 통해 `APlayerCharacter::FinishTalismanRitual` 함수를 호출하여 선택된 `EAnomalyCategory`를 전달한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Button_VisualSense | TObjectPtr<UButton> | private | '시각' 범주 버튼의 참조이다. |
| Button_AuditorySense | TObjectPtr<UButton> | private | '청각' 범주 버튼의 참조이다. |
| Button_Object | TObjectPtr<UButton> | private | '물체' 범주 버튼의 참조이다. |
| Button_Environment | TObjectPtr<UButton> | private | '환경' 범주 버튼의 참조이다. |
| Button_GoBack | TObjectPtr<UButton> | private | '돌아가기'(취소) 버튼의 참조이다. |
| PlayerCharacterRef | TObjectPtr<APlayerCharacter> | private | NativeConstruct에서 캐시되는 플레이어 캐릭터 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| NativeConstruct | void | protected | 버튼 OnClicked 델리게이트를 C++ 함수에 바인딩하고 PlayerCharacterRef를 캐시한다. (Override) |
| OnVisualSenseClicked | void | private | '시각' 버튼 클릭 시 HandleSelection(AC_Visual)을 호출한다. |
| OnAuditorySenseClicked | void | private | '청각' 버튼 클릭 시 HandleSelection(AC_Auditory)을 호출한다. |
| OnObjectClicked | void | private | '물체' 버튼 클릭 시 HandleSelection(AC_Object)을 호출한다. |
| OnEnvironmentClicked | void | private | '환경' 버튼 클릭 시 HandleSelection(AC_Environmental)을 호출한다. |
| OnGoBackClicked | void | private | '뒤로가기' 버튼 클릭 시 HandleSelection(AC_None)을 호출한다. |
| HandleSelection | void | private | PlayerCharacterRef->FinishTalismanRitual을 SelectedCategory와 함께 호출한다. |
