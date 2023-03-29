/*
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ScriptInterface.h"
#include "UObject/Object.h"
#include "UObject/Interface.h"
#include "Interfaces/IHttpRequest.h"
#include "Delegates/Delegate.h"
#include "Interfaces/IHttpResponse.h"
#include "UApiRestClient.generated.h"

UCLASS(Blueprintable)
class OPENWORLDSTARTER_API UUApiRestClient : public UObject
{
    GENERATED_BODY()

public:

    UDELEGATE(BlueprintAuthorityOnly)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPostResponseDelegate, FString, ResponseData, bool, bSuccess);

    UDELEGATE(BlueprintAuthorityOnly)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGetResponseDelegate, FString, ResponseData, bool, bSuccess);

    UDELEGATE(BlueprintAuthorityOnly)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPutResponseDelegate, FString, ResponseData, bool, bSuccess);

    UDELEGATE(BlueprintAuthorityOnly)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeleteResponseDelegate, FString, ResponseData, bool, bSuccess);

    FGetResponseDelegate OnGetResponse;
    FPostResponseDelegate OnPostResponse;
    FPutResponseDelegate OnPutResponse;
    FDeleteResponseDelegate OnDeleteResponse;

    UFUNCTION(BlueprintCallable, Category = "ApiRestClient")
        void Get(const FString& URL);

    UFUNCTION(BlueprintCallable, Category = "ApiRestClient")
        void Post(const FString& URL, const FString& Content);

    UFUNCTION(BlueprintCallable, Category = "ApiRestClient")
        void Put(const FString& URL, const FString& Content);

    UFUNCTION(BlueprintCallable, Category = "ApiRestClient")
        void Delete(const FString& URL);

    

private:
    void OnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
*/