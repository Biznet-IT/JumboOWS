// Copyright 2023, Dakota Dawe, All rights reserved

// Credit to Ryan Schmidt for the bulk of the transaction logic.
#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGToolsDataTypes.h"
#include "Misc/Change.h"
#include "SKGHistoryManager.generated.h"

USTRUCT()
struct FSKGHistoryRecord
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* TargetObject = nullptr;

	UPROPERTY()
	FText Description;

	struct FSKGChange
	{
		TUniquePtr<FCommandChange> Change;
	};

	TSharedPtr<FSKGChange> Change;
};

USTRUCT()
struct FSKGHistoryTransaction
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSKGHistoryRecord> Records;

	UPROPERTY()
	FText Description;

	/** @return true if all contained FCommandChange's have Expired. In this case, the entire Transaction will have no effect, and should be skipped in Undo/Redo. */
	bool HasExpired() const;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FHistoryChangedMinMaxEvent, ESKGMLEHistoryActionType);
DECLARE_MULTICAST_DELEGATE_OneParam(FHistoryChangedEvent, ESKGMLEHistoryActionType);
UCLASS()
class SKGGIZMO_API USKGHistoryManager : public UObject
{
	GENERATED_BODY()
public:

	/** Open a new Transaction, ie list of (UObject,FCommandChange) pairs */
	void BeginTransaction(const FText& Description);

	/** Append a Change to the open Transaction */
	void AppendChange(UObject* TargetObject, TUniquePtr<FCommandChange> Change, const FText& Description);

	/** Close the current Transaction and add it to the History sequence */
	void EndTransaction();

	/** @return true if we are inside an open Transaction */
	bool IsBuildingTransaction() const { return BeginTransactionDepth > 0;  }

	/** Roll back in History to previous state by Revert()'ing intermediate Changes */
	void Undo();
	/** Roll forward in History to next state by Apply()'ing intermediate Changes */
	void Redo();

	// Fires when you max out history (hit end of undo/redo)
	FHistoryChangedMinMaxEvent OnHistoryChangedMinMaxDelegate;
	// Fires when you change history (undo/redo)
	FHistoryChangedEvent OnHistoryChangedDelegate;

protected:
	
	// undo history, stored as a set of transactions, which are themselves list of (UObject,FCommandChange) pairs
	UPROPERTY()
	TArray<FSKGHistoryTransaction> Transactions;
	
	// current index in Transactions list, will be Transactions.Num() unless user is Undo()/Redo()-ing
	int32 CurrentIndex = 0;

	// remove any elements of Transactions list beyond CurrentIndex (called if we are in Undo state and a new transaction is opened)
	void TruncateHistory();

	// transaction currently being built
	UPROPERTY()
	FSKGHistoryTransaction ActiveTransaction;
	
	int BeginTransactionDepth = 0;
};
