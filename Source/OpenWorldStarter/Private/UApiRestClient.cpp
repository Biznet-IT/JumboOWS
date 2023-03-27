#include "UApiRestClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UUApiRestClient::Get(const FString& URL)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("GET");
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUApiRestClient::OnHttpRequestCompleted);
    HttpRequest->ProcessRequest();
}

void UUApiRestClient::Post(const FString& URL, const FString& Content)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("POST");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetContentAsString(Content);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUApiRestClient::OnHttpRequestCompleted);
    HttpRequest->ProcessRequest();
}

void UUApiRestClient::Put(const FString& URL, const FString& Content)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("PUT");
    HttpRequest->SetHeader("Content-Type", "application/json");
    HttpRequest->SetContentAsString(Content);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUApiRestClient::OnHttpRequestCompleted);
    HttpRequest->ProcessRequest();
}

void UUApiRestClient::Delete(const FString& URL)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb("DELETE");
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUApiRestClient::OnHttpRequestCompleted);
    HttpRequest->ProcessRequest();
}

void UUApiRestClient::OnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    FString ResponseString = "";
    if (Response.IsValid())
    {
        ResponseString = Response->GetContentAsString();
    }

    if (Request->GetVerb() == "GET")
    {
        OnGetResponse.Broadcast(ResponseString, bWasSuccessful);
    }
    else if (Request->GetVerb() == "POST")
    {
        OnPostResponse.Broadcast(ResponseString, bWasSuccessful);
    }
    else if (Request->GetVerb() == "PUT")
    {
        OnPutResponse.Broadcast(ResponseString, bWasSuccessful);
    }
    else if (Request->GetVerb() == "DELETE")
    {
        OnDeleteResponse.Broadcast(ResponseString, bWasSuccessful);
    }
}
