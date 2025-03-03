/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "CoreMinimal.h"

#include "InworldPackets.h"

#include "InworldCharacterMessage.generated.h"

struct FCharacterMessageUtterance;
struct FCharacterMessageSilence;
struct FCharacterMessageInteractionEnd;

class ICharacterMessageVisitor
{
public:
	virtual void Handle(const FCharacterMessageUtterance& Event) { }
	virtual void Interrupt(const FCharacterMessageUtterance& Event) { }
	virtual void Pause(const FCharacterMessageUtterance& Event) { }
	virtual void Resume(const FCharacterMessageUtterance& Event) { }

	virtual void Handle(const FCharacterMessageSilence& Event) { }
	virtual void Interrupt(const FCharacterMessageSilence& Event) { }

	virtual void Handle(const FCharacterMessageTrigger& Event) { }

	virtual void Handle(const FCharacterMessageInteractionEnd& Event) { }
};

USTRUCT(BlueprintType)
struct FCharacterMessage
{
	GENERATED_BODY()

	FCharacterMessage() {}
	virtual ~FCharacterMessage() = default;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString UtteranceId;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString InteractionId;

	virtual FString ToDebugString() const PURE_VIRTUAL(FCharacterMessage::ToDebugString, return FString();)
};

USTRUCT(BlueprintType)
struct FCharacterUtteranceVisemeInfo
{
	GENERATED_BODY()
	FCharacterUtteranceVisemeInfo() = default;
	FCharacterUtteranceVisemeInfo(const FString& InCode, float InTimestamp)
		: Code(InCode)
		, Timestamp(InTimestamp)
	{}

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString Code;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	float Timestamp = 0.f;
};

USTRUCT(BlueprintType)
struct FCharacterMessageUtterance : public FCharacterMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString Text;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	TArray<FCharacterUtteranceVisemeInfo> VisemeInfos;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	bool bTextFinal = false;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	bool bAudioFinal = false;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	TArray<uint8> SoundData;

	virtual FString ToDebugString() const override { return FString::Printf(TEXT("Utterance. Text: %s"), *Text); }
};

USTRUCT(BlueprintType)
struct FCharacterMessagePlayerTalk : public FCharacterMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString Text;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	bool bTextFinal = false;

	virtual FString ToDebugString() const override { return FString::Printf(TEXT("PlayerTalk. Text: %s"), *Text); }
};

USTRUCT(BlueprintType)
struct FCharacterMessageSilence : public FCharacterMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	float Duration = 0.f;

	virtual FString ToDebugString() const override { return FString::Printf(TEXT("Silence. Duration: %f"), Duration); }
};

USTRUCT(BlueprintType)
struct FCharacterMessageTrigger : public FCharacterMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Message")
	TMap<FString, FString> Params;

	virtual FString ToDebugString() const override { return FString::Printf(TEXT("Trigger. Name: %s"), *Name); }
};

USTRUCT(BlueprintType)
struct FCharacterMessageInteractionEnd : public FCharacterMessage
{
	GENERATED_BODY()

	virtual FString ToDebugString() const override { return TEXT("InteractionEnd"); }
};

void operator<<(FCharacterMessage& Message, const FInworldPacket& Packet);
void operator<<(FCharacterMessageUtterance& Message, const FInworldTextEvent& Event);
void operator<<(FCharacterMessageUtterance& Message, const FInworldAudioDataEvent& Event);
void operator<<(FCharacterMessagePlayerTalk& Message, const FInworldTextEvent& Event);
void operator<<(FCharacterMessageSilence& Message, const FInworldSilenceEvent& Event);
void operator<<(FCharacterMessageTrigger& Message, const FInworldCustomEvent& Event);
void operator<<(FCharacterMessageTrigger& Message, const FInworldRelationEvent& Event);
void operator<<(FCharacterMessageInteractionEnd& Message, const FInworldControlEvent& Event);
