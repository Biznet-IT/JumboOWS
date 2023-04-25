// Copyright 2023, Dakota Dawe, All rights reserved


#include "Tools/SKGHistoryManager.h"

bool FSKGHistoryTransaction::HasExpired() const
{
	for (const FSKGHistoryRecord& Record : Records)
	{
		if (Record.Change->Change->HasExpired(Record.TargetObject) == false)
		{
			return false;
		}
	}
	return true;
}


void USKGHistoryManager::BeginTransaction(const FText& Description)
{
	if (BeginTransactionDepth != 0)
	{
		BeginTransactionDepth++;
	}
	else
	{
		TruncateHistory();

		ActiveTransaction = FSKGHistoryTransaction();
		ActiveTransaction.Description = Description;

		BeginTransactionDepth++;
	}
}


void USKGHistoryManager::EndTransaction()
{
	if (ensure(BeginTransactionDepth > 0) == false) return;

	BeginTransactionDepth--;

	if (BeginTransactionDepth == 0)
	{
		if (ActiveTransaction.Records.Num() > 0)
		{
			Transactions.Add(MoveTemp(ActiveTransaction));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[EndTransaction] Empty Transaction Record!"));
		}

		ActiveTransaction = FSKGHistoryTransaction();

		CurrentIndex = Transactions.Num();
	}
}


void USKGHistoryManager::TruncateHistory()
{
	// truncate history if we are in undo step
	if (CurrentIndex < Transactions.Num())
	{
		Transactions.SetNum(CurrentIndex);
	}
}


void USKGHistoryManager::AppendChange(UObject* TargetObject, TUniquePtr<FCommandChange> Change, const FText& Description)
{
	bool bAutoCloseTransaction = false;
	if (ensure(BeginTransactionDepth > 0) == false)
	{
		BeginTransaction(Description);
		bAutoCloseTransaction = true;
	}

	FSKGHistoryRecord Record;
	Record.TargetObject = TargetObject;
	Record.Description = Description;
	Record.Change = MakeShared<FSKGHistoryRecord::FSKGChange>();
	Record.Change->Change = MoveTemp(Change);
	
	ActiveTransaction.Records.Add(MoveTemp(Record));

	if (bAutoCloseTransaction)
	{
		EndTransaction();
	}
}




void USKGHistoryManager::Undo()
{
	int32 NumReverted = 0;
	while (CurrentIndex > 0)
	{
		CurrentIndex = CurrentIndex - 1;

		// if transaction has expired, it is effectively a no-op and so we will continue to Undo()
		const bool bContinue = Transactions[CurrentIndex].HasExpired();

		const TArray<FSKGHistoryRecord>& Records = Transactions[CurrentIndex].Records;
		for (int32 i = Records.Num() - 1; i >= 0; --i)
		{
			if (Records[i].TargetObject)
			{
				Records[i].Change->Change->Revert(Records[i].TargetObject);
			}
			NumReverted++;
		}

		if (!bContinue)
		{
			OnHistoryChangedDelegate.Broadcast(ESKGMLEHistoryActionType::Undo);
			return;
		}
	}
	
	if (NumReverted > 0)
	{
		OnHistoryChangedMinMaxDelegate.Broadcast(ESKGMLEHistoryActionType::Undo);
	}
}

void USKGHistoryManager::Redo()
{
	int32 NumApplied = 0;
	while (CurrentIndex < Transactions.Num())
	{
		// if transaction has expired, it is effectively a no-op and so we will continue to Redo()
		const bool bContinue = Transactions[CurrentIndex].HasExpired();

		const TArray<FSKGHistoryRecord>& Records = Transactions[CurrentIndex].Records;
		for (int32 i = 0; i < Records.Num(); ++i)
		{
			if (Records[i].TargetObject)
			{
				Records[i].Change->Change->Apply(Records[i].TargetObject);
			}
			++NumApplied;
		}

		CurrentIndex = CurrentIndex + 1;

		if (!bContinue)
		{
			OnHistoryChangedDelegate.Broadcast(ESKGMLEHistoryActionType::Redo);
			return;
		}
	}

	if (NumApplied > 0)
	{
		OnHistoryChangedMinMaxDelegate.Broadcast(ESKGMLEHistoryActionType::Redo);
	}
}

