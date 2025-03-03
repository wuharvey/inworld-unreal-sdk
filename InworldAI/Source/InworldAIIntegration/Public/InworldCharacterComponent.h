/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "CoreMinimal.h"
#include "InworldApi.h"
#include "InworldCharacter.h"
#include "Components/ActorComponent.h"

#include "InworldCharacterPlayback.h"
#include "InworldCharacterMessage.h"
#include "InworldEnums.h"
#include "InworldPackets.h"
#include "InworldSockets.h"

#include "Containers/Queue.h"

#include <GameFramework/OnlineReplStructs.h>

#include "InworldCharacterComponent.generated.h"

class UInworldPlayerComponent;
class FInternetAddr;

UCLASS(ClassGroup = (Inworld), meta = (BlueprintSpawnableComponent))
class INWORLDAIINTEGRATION_API UInworldCharacterComponent : public UActorComponent, public IInworldCharacterOwnerInterface, public ICharacterMessageVisitor
{
	GENERATED_BODY()

public:
	UInworldCharacterComponent();

	// IInworldCharacterOwnerInterface
	virtual UInworldCharacter* GetInworldCharacter_Implementation() const override { return InworldCharacter; }
	virtual void HandleTargetPlayerVoiceDetection(bool bVoiceDetected) override;
	// ~IInworldCharacterOwnerInterface

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInworldCharacterPlayerInteractionStateChanged, bool, bInteracting);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Interaction")
	FInworldCharacterPlayerInteractionStateChanged OnPlayerInteractionStateChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldCharacterPlayerTalk, const FCharacterMessagePlayerTalk&, PlayerTalk);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Interaction")
	FOnInworldCharacterPlayerTalk OnPlayerTalk;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInworldCharacterEmotionalBehaviorChanged, EInworldCharacterEmotionalBehavior, EmotionalBehavior, EInworldCharacterEmotionStrength, Strength);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emotion")
	FOnInworldCharacterEmotionalBehaviorChanged OnEmotionalBehaviorChanged;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldCharacterUtterance, const FCharacterMessageUtterance&, Utterance);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Utterance")
	FOnInworldCharacterUtterance OnUtterance;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Utterance")
	FOnInworldCharacterUtterance OnUtteranceInterrupt;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Utterance")
	FOnInworldCharacterUtterance OnUtterancePause;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Utterance")
	FOnInworldCharacterUtterance OnUtteranceResume;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldCharacterSilence, const FCharacterMessageSilence&, Silence);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Silence")
	FOnInworldCharacterSilence OnSilence;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Silence")
	FOnInworldCharacterSilence OnSilenceInterrupt;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldCharacterTrigger, const FCharacterMessageTrigger&, Trigger);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Trigger")
	FOnInworldCharacterTrigger OnTrigger;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldCharacterInteractionEnd, const FCharacterMessageInteractionEnd&, InteractionEnd);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|InteractionEnd")
	FOnInworldCharacterInteractionEnd OnInteractionEnd;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInworldPlayerVoiceDetection, bool, bVoiceDetected);
	UPROPERTY(BlueprintAssignable, Category = "Conversation")
	FOnInworldPlayerVoiceDetection OnVoiceDetection;

	UFUNCTION(BlueprintCallable, Category = "Inworld")
	void SetBrainName(const FString& Name);

	UFUNCTION(BlueprintCallable, Category = "Inworld")
	FString GetBrainName() const;

    UFUNCTION(BlueprintCallable, Category = "Inworld")
	FString GetAgentId() const;

    UFUNCTION(BlueprintCallable, Category = "Inworld")
	FString GetGivenName() const;

    UFUNCTION(BlueprintCallable, Category = "Inworld")
    const FString& GetUiName() const { return UiName; }
    UFUNCTION(BlueprintCallable, Category = "Inworld")
    void SetUiName(const FString& Name) { UiName = Name; }

	UFUNCTION(BlueprintCallable, Category = "Inworld", meta = (DeterminesOutputType = "Class"))
	UInworldCharacterPlayback* GetPlayback(TSubclassOf<UInworldCharacterPlayback> Class) const;

	UFUNCTION(BlueprintCallable, Category = "Interactions")
	bool IsInteractingWithPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "Emotions")
	EInworldCharacterEmotionalBehavior GetEmotionalBehavior() const { return EmotionalBehavior; }

	UFUNCTION(BlueprintPure, Category = "Emotions")
	EInworldCharacterEmotionStrength GetEmotionStrength() const { return EmotionStrength; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SendTextMessage(const FString& Text) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Interaction", meta = (AutoCreateRefTerm = "Params"))
	void SendTrigger(const FString& Name, const TMap<FString, FString>& Params) const;
	[[deprecated("UInworldCharacterComponent::SendCustomEvent is deprecated, please use UInworldCharacterComponent::SendTrigger")]]
	void SendCustomEvent(const FString& Name) const { SendTrigger(Name, {}); }
    
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SendNarrationEvent(const FString& Content);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StartAudioSession(UInworldPlayer* Player, EInworldMicrophoneMode MicrophoneMode = EInworldMicrophoneMode::OPEN_MIC);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void StopAudioSession();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	FVector GetTargetPlayerCameraLocation();

	void Interrupt(const FString& InteractionId);

	const TSharedPtr<FCharacterMessage> GetCurrentMessage() const
	{ 
		return MessageQueue->CurrentMessageQueueEntry ? MessageQueue->CurrentMessageQueueEntry->GetCharacterMessage() : nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Message")
	bool LockMessageQueue(UPARAM(ref) FInworldCharacterMessageQueueLockHandle& Handle) { return MessageQueue->Lock(Handle); }

	UFUNCTION(BlueprintCallable, Category = "Message")
	void UnlockMessageQueue(UPARAM(ref) FInworldCharacterMessageQueueLockHandle& Handle) { MessageQueue->Unlock(Handle); }

	template<class T>
	T* GetPlaybackNative()
	{
		for (auto* Pb : Playbacks)
		{
			if (auto* Playback = Cast<T>(Pb))
			{
				return Playback;
			}
		}
		return nullptr;
	}

	UPROPERTY(EditAnywhere, Category = "Inworld")
	TArray<TSubclassOf<UInworldCharacterPlayback>> PlaybackTypes;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FString UiName = "Character";

private:
	UFUNCTION()
	void OnInworldTextEvent(const FInworldTextEvent& Event);
	UFUNCTION()
	void OnInworldAudioEvent(const FInworldAudioDataEvent& Event);
	UFUNCTION()
	void OnInworldSilenceEvent(const FInworldSilenceEvent& Event);
	UFUNCTION()
	void OnInworldControlEvent(const FInworldControlEvent& Event);
	UFUNCTION()
	void OnInworldEmotionEvent(const FInworldEmotionEvent& Event);
	UFUNCTION()
	void OnInworldCustomEvent(const FInworldCustomEvent& Event);
	UFUNCTION()
	void OnInworldRelationEvent(const FInworldRelationEvent& Event);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitText(const FInworldTextEvent& Event);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitSilence(const FInworldSilenceEvent& Event);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitControl(const FInworldControlEvent& Event);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitEmotion(const FInworldEmotionEvent& Event);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitCustom(const FInworldCustomEvent& Event);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_VisitRelation(const FInworldRelationEvent& Event);

	bool IsCustomGesture(const FString& CustomEventName) const;

	void VisitAudioOnClient(const FInworldAudioDataEvent& Event);

	TQueue<FInworldAudioDataEvent> PendingRepAudioEvents;

	UPROPERTY()
	TArray<UInworldCharacterPlayback*> Playbacks;

	TSharedRef<FCharacterMessageQueue> MessageQueue;

	virtual void Handle(const FCharacterMessageUtterance& Message) override;
	virtual void Interrupt(const FCharacterMessageUtterance& Message) override;
	virtual void Pause(const FCharacterMessageUtterance& Event) override;
	virtual void Resume(const FCharacterMessageUtterance& Event) override;

	virtual void Handle(const FCharacterMessageSilence& Message) override;
	virtual void Interrupt(const FCharacterMessageSilence& Message) override;

	virtual void Handle(const FCharacterMessageTrigger& Message) override;

	virtual void Handle(const FCharacterMessageInteractionEnd& Message) override;

	TMap<FString, TArray<FString>> PendingCancelResponses;

    EInworldCharacterEmotionalBehavior EmotionalBehavior = EInworldCharacterEmotionalBehavior::NEUTRAL;
    EInworldCharacterEmotionStrength EmotionStrength = EInworldCharacterEmotionStrength::UNSPECIFIED;

	UPROPERTY(EditAnywhere, Category = "Inworld")
	FString BrainName;

private:
	UFUNCTION()
	void OnRep_InworldCharacter();

	UPROPERTY(ReplicatedUsing=OnRep_InworldCharacter)
	UInworldCharacter* InworldCharacter;

#if defined(WITH_GAMEPLAY_DEBUGGER) && WITH_GAMEPLAY_DEBUGGER
	friend class FInworldGameplayDebuggerCategory;
#endif
};
