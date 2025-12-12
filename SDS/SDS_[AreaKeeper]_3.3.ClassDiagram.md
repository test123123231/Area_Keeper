## 3.3 Anomaly System
본 절은 게임의 핵심 위협 요소인 이상 현상을 관리, 스폰, 정의하는 클래스들을 다룬다. EAnomalyType, EAnomalyCategory 등 정의된 타입을 기반으로 동작하며, AAreaKeeperGameState(3.1절)의 상태에 따라 이상 현상을 스폰하는 AAnomalyManager, 플레이어가 '소지'해야 하는 정지된 이상 현상인 AStationaryAnomaly, 그리고 플레이어를 추적하는 AChasingAnomaly의 상세 내용을 기술한다.

본 절의 클래스 다이어그램에는 다음 시스템의 클래스들이 참조로 포함될 수 있으며, 해당 클래스들의 상세 기술서는 명시된 세부 절에 작성되어 있다.
- 3.1 Core Game System: AAreaKeeperGameState
- 3.2 Character: APlayerCharacter
- 3.4 Item & Interaction System: IInteractableInterface

![image](image/Class3_AnomalySystem.pn)

***

### AAnomalyManager
#### 클래스 개요
게임 내 모든 이상 현상의 스폰, 타이밍, 생명 주기를 관리하는 액터이다. `AActor`를 상속받는다. `AAreaKeeperGameState`의 상태(`CurrentPlayState`)에 따라 `StartSpawning`/`StopSpawning`이 호출되며, `OnSpawnTimerExpired` 타이머를 통해 주기적으로 `SpawnStationaryAnomaly`를 실행한다. 또한 `AStationaryAnomaly`의 요청을 받아 `SpawnChasingAnomaly`를 실행하는 스포너(Spawner) 역할을 한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| StationaryAnomalyClassList | TArray<TSubclassOf<AStationaryAnomaly>> | protected | 스폰할 '정지된 이상현상'의 블루프린트 클래스 목록이다. (EditDefaultsOnly) |
| ChasingAnomalyClassList | TArray<TSubclassOf<AChasingAnomaly>> | protected | 스폰할 '쫓아오는 이상현상'의 블루프린트 클래스 목록이다. (EditDefaultsOnly) |
| StationaryAnomalyLifeSpan | float | private | '정지된 이상현상'의 생성 유지 시간(초)이다. (EditAnywhere) |
| SpawnTimerHandle | FTimerHandle | private | OnSpawnTimerExpired를 주기적으로 호출하기 위한 타이머 핸들이다. |
| SpawnRadius | float | private | GetRandomSpawnLocation에서 사용할 네비게이션 반경이다. (EditAnywhere) |
| CurrentSpawnInterval | float | private | 현재 이상현상 생성 주기(초)이다. (EditAnywhere) |
| GameStateRef | TWeakObjectPtr<AAreaKeeperGameState> | private | AAreaKeeperGameState의 캐시된 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| AAnomalyManager |  | public | 생성자이다. Tick을 false로 설정한다. |
| BeginPlay | void | protected | GameStateRef를 캐시한다. (Override) |
| StartSpawning | void | public | OnSpawnTimerExpired()를 즉시 호출하여 이상 현상의 주기적 스폰을 시작한다. (BlueprintCallable) |
| StopSpawning | void | public | SpawnTimerHandle을 정지시켜 이상 현상 스폰을 중지한다. (BlueprintCallable) |
| SpawnChasingAnomaly | void | public | AStationaryAnomaly의 만료 요청에 따라 지정된 Location에 ChasingAnomalyClassList 중 하나를 스폰한다. (BlueprintCallable) |
| SpawnStationaryAnomaly | void | public | GetRandomSpawnLocation을 호출하여 StationaryAnomalyClassList 중 하나를 스폰하고, InitializeAnomaly를 호출하여 랜덤 속성을 부여한다. |
| OnSpawnTimerExpired | void | private | GameStateRef에서 CurrentSpawnInterval을 갱신받아 타이머를 재설정하고 SpawnStationaryAnomaly를 호출한다. |
| GetRandomSpawnLocation | bool | private | NavMesh 반경 내 랜덤 위치를 OutLocation에 반환한다. |
| GetRandomAnomalyType | EAnomalyType | private | EAnomalyType (Disappearing/Chasing) 중 하나를 랜덤 반환한다. |
| GetRandomStationaryAnomalyClass | TSubclassOf<AStationaryAnomaly> | private | StationaryAnomalyClassList 배열에서 랜덤 클래스를 반환한다. |
| GetRandomChasingAnomalyClass | TSubclassOf<AChasingAnomaly> | private | ChasingAnomalyClassList 배열에서 랜덤 클래스를 반환한다. |

***

### AChasingAnomaly
#### 클래스 개요
'정지된 이상 현상'(AStationaryAnomaly)에서 생성되는 추격 개체이다. ACharacter와 IInteractableInterface를 상속받는다. AIPerception을 사용해 플레이어를 감지(OnSeePawn)하고, EAnomalyState에 따라 추적(ChaseTarget)한다. 플레이어와 접촉(OnAnomalyOverlap) 시 피해를 1 주고(ApplyDamageToPlayer), GameState의 IncrementChasingHits를 호출한 뒤, 자신은 소멸(Banish)한다. APlayerCharacter가 RequiredToolType이 일치하는 도구를 사용하면 Banish될 수 있다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| CurrentState | EAnomalyState | protected | 현재 AI 상태(Idle 또는 Chasing)이다. (VisibleAnywhere, BlueprintReadOnly) |
| AIPerception | UAIPerceptionComponent* | protected | AI 시야 감지를 위한 퍼셉션 컴포넌트이다. (EditAnywhere, BlueprintReadOnly) |
| SightConfig | UAISenseConfig_Sight* | protected | AIPerception에 사용될 시야 설정 오브젝트이다. (EditAnywhere, BlueprintReadOnly) |
| PlayerTarget | TObjectPtr<AActor> | protected | 현재 추적 중인 플레이어 대상의 참조이다. (VisibleAnywhere, BlueprintReadOnly) |
| GameStateRef | TWeakObjectPtr<AAreaKeeperGameState> | protected | AAreaKeeperGameState의 캐시된 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| AChasingAnomaly |  | public | 생성자이다. AIPerception, SightConfig를 생성하고 OnComponentBeginOverlap 이벤트를 바인딩한다. |
| Highlight_Implementation | void | public | 플레이어가 바라볼 때 하이라이트 효과를 적용한다. (현재 내용 비어있음) (Override) |
| Interact_Implementation | void | public | 플레이어가 상호작용 시 호출된다. (실제 로직은 APlayerCharacter::UseTool에서 처리하므로 비어있음) (Override) |
| GetInteractText_Implementation | FString | public | 상호작용 UI에 도구 사용 텍스트를 반환한다. (Override) |
| Banish | void | public | 이 액터를 즉시 소멸(Destroy)시킨다. (BlueprintCallable) |
| GetRequiredToolType | EToolType | public | 이 개체를 퇴치하는 데 필요한 도구 타입을 반환한다. (BlueprintPure) |
| BeginPlay | void | protected | AnomalyController, GameStateRef 참조를 얻고, AIPerception->OnTargetPerceptionUpdated 델리게이트를 바인딩하며, ChasingSpeed를 계산한다. (Override) |
| Tick | void | protected | CurrentState가 EAS_Chasing일 때, PlayerTarget과의 거리를 계산하여 StopChasing, MoveDirectlyToTarget, MoveToTarget 중 하나를 호출한다. (Override) |
| OnSeePawn | void | protected | AIPerception이 플레이어를 감지하면 ChaseTarget을 호출한다. |
| ChaseTarget | void | protected | PlayerTarget을 설정하고, CurrentState를 EAS_Chasing로 변경하며 이동 속도를 높인다.  |
| StopChasing | void | protected | PlayerTarget을 해제하고, CurrentState를 EAS_Idle로 변경하며 이동을 멈춘다. |
| OnAnomalyOverlap | void | protected | 캡슐 컴포넌트가 APlayerCharacter와 오버랩될 때 호출된다. 플레이어가 무적이 아니면 ApplyDamageToPlayer를 호출하고, 무적이면 OnPlayerInvincibilityEnd에 바인딩한다. |
| ApplyDamageToPlayer | void | private | Player->HandleDamage(1.0f)를 호출하고, GameStateRef->IncrementChasingHits()를 호출한 뒤, Banish()를 호출하여 자신을 소멸시킨다. |
| OnPlayerInvincibilityEnd | void | protected | 플레이어의 무적이 끝났을 때 델리게이트에 의해 호출된다. 이 때 아직 플레이어와 오버랩 중이고 생존 상태라면 ApplyDamageToPlayer를 호출한다.  |

***

### AStationaryAnomaly
#### 클래스 개요
플레이어가 '소지(Talisman Ritual)'를 수행해야 하는 정지된 이상 현상 액터이다. AActor와 IInteractableInterface를 상속받는다. AAnomalyManager에 의해 스폰되며, InitializeAnomaly를 통해 '사라지는 유형(Disappearing)' 또는 '쫓아오는 유형(Chasing)'과 '정답 범주(CorrectCategory)'를 할당받는다. Interact 시 APlayerCharacter의 소지 UI를 호출하며, OnTalismanRitualFinished를 통해 소지 결과를 처리한다.

#### 멤버 변수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| Mesh | TObjectPtr<UStaticMeshComponent> | protected | 이상 현상의 외형을 나타내는 스태틱 메시 컴포넌트이다. (VisibleAnywhere, BlueprintReadOnly) |
| AnomalyType | EAnomalyType | protected | 미해결 시 '사라지는' 유형인지 '쫓아오는' 유형인지 결정한다. (EditAnywhere, BlueprintReadOnly) |
| CorrectCategory | EAnomalyCategory | protected | '소지' 성공을 위해 플레이어가 선택해야 하는 정답 범주이다. (EditAnywhere, BlueprintReadOnly) |
| ResolveTimerHandle | FTimerHandle | protected | 이상 현상의 생성 유지 시간(Lifespan)을 측정하는 타이머 핸들이다. |
| DynamicMaterial | UMaterialInstanceDynamic* | protected | BeginPlay 시 Mesh에서 생성되는 하이라이트용 동적 머티리얼이다.  |
| GameStateRef | TWeakObjectPtr<AAreaKeeperGameState> | private | 패널티 스택 증감을 위해 캐시된 AAreaKeeperGameState의 참조이다. |
| AnomalyManagerRef | TWeakObjectPtr<AAnomalyManager> | private | 소지 실패 또는 만료 시 다른 이상 현상을 스폰하기 위해 캐시된 AAnomalyManager의 참조이다. |

#### 멤버 함수
| 이름 | 타입 | 가시성 | 설명 |
|:---:|:---:|:---:|:---:|
| AStationaryAnomaly |  | public | 생성자이다. Mesh를 루트 컴포넌트로 생성하고 충돌을 설정한다. |
| Highlight_Implementation | void | public | 플레이어가 바라볼 때 호출된다. DynamicMaterial의 Color 파라미터를 변경하여 하이라이트 효과를 준다. (Override) |
| Interact_Implementation | void | public | 플레이어가 E키로 상호작용할 때 호출된다. Interactor(플레이어)의 TalismanRitual 함수를 호출하여 '소지' UI를 연다. (Override) |
| GetInteractText_Implementation | FString | public | 상호작용 UI에 소지하기 텍스트를 반환한다. (Override) |
| InitializeAnomaly | void | public | AAnomalyManager가 스폰 직후 호출한다. AnomalyType, CorrectCategory, Lifespan을 설정하고 OnResolveTimerExpired 타이머를 시작한다. |
| OnTalismanRitualFinished | void | public | APlayerCharacter가 '소지' UI를 닫을 때 호출된다. SelectedCategory가 CorrectCategory와 일치하면 DecrementPenaltyStack을 호출하고 자신을 파괴한다. 불일치 시 SpawnFailedAnomaly를 호출한다. (BlueprintCallable) |
| BeginPlay | void | protected | DynamicMaterial을 생성하고 GameStateRef, AnomalyManagerRef를 캐시한다. (Override) |
| OnResolveTimerExpired | void | protected | ResolveTimerHandle에 의해 생성 유지 시간이 만료되면 호출된다. AnomalyType에 따라 SpawnChasingAnomaly 또는 IncrementPenaltyStack을 호출한 뒤 자신을 파괴한다. |
| IncrementPenaltyStack | void | private | GameStateRef가 유효하면 IncrementPenaltyStack 함수를 호출한다. |
| DecrementPenaltyStack | void | private | GameStateRef가 유효하면 DecrementPenaltyStack 함수를 호출한다. |
| SpawnFailedAnomaly | void | private | AnomalyManagerRef가 유효하면 SpawnStationaryAnomaly 함수를 호출한다. |
