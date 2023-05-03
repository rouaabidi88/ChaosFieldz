// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Runtime/Core/Public/Delegates/Delegate.h"
#include "BRGameInstance.generated.h"

/*
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnCreateSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnUpdateSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnStartSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnEndSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnDestroySessionComplete, bool, Successful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCSOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FCSOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
/*

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UBRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	/*
public:
	UMyGameInstance();

	UFUNCTION(BlueprintCallable)
		void CreateSession(int32 NumPublicConnections, bool IsLANMatch);

	UFUNCTION(BlueprintCallable)
		void UpdateSession();

	UFUNCTION(BlueprintCallable)
		void StartSession();

	UFUNCTION(BlueprintCallable)
		void EndSession();

	UFUNCTION(BlueprintCallable)
		void DestroySession();

	UFUNCTION(BlueprintCallable)
		void FindSessions(int32 MaxSearchResults, bool IsLANQuery);

	
	UFUNCTION(BlueprintCallable)
		void JoinGameSession(const class FOnlineSessionSearchResult& SessionResult);
	

	FCSOnCreateSessionComplete OnCreateSessionCompleteEvent;
	FCSOnUpdateSessionComplete OnUpdateSessionCompleteEvent;
	FCSOnStartSessionComplete OnStartSessionCompleteEvent;
	FCSOnEndSessionComplete OnEndSessionCompleteEvent;
	FCSOnDestroySessionComplete OnDestroySessionCompleteEvent;
	FCSOnFindSessionsComplete OnFindSessionsCompleteEvent;
	FCSOnJoinSessionComplete OnJoinGameSessionCompleteEvent;

protected:
	void OnCreateSessionCompleted(FName SessionName, bool Successful);
	void OnUpdateSessionCompleted(FName SessionName, bool Successful);
	void OnStartSessionCompleted(FName SessionName, bool Successful);
	void OnEndSessionCompleted(FName SessionName, bool Successful);
	void OnDestroySessionCompleted(FName SessionName, bool Successful);
	void OnFindSessionsCompleted(bool Successful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	bool TryTravelToCurrentSession();

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	FOnUpdateSessionCompleteDelegate UpdateSessionCompleteDelegate;
	FDelegateHandle UpdateSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	FOnEndSessionCompleteDelegate EndSessionCompleteDelegate;
	FDelegateHandle EndSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	*/
};
