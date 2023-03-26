// Copyright 2023 Biznet It


#include "JumboVivoxGameInstance.h"
#include <Kismet/KismetMathLibrary.h>
#include "VivoxCore.h"
#include "VivoxCoreCommon.h"
#include <VivoxCore/Private/ChannelSession.h>

#define VIVOX_VOICE_SERVER TEXT("https://unity.vivox.com/appconfig/46736-jumbo-25100-udash")
#define VIVOX_VOICE_DOMAIN TEXT("mtu1xp.vivox.com")
#define VIVOX_VOICE_ISSUER TEXT("46736-jumbo-25100-udash")
#define VIVOX_VOICE_KEY TEXT("Ol5Sboc2EeTSTe8dXOkgpHGikEaPhJEe") // para producción esto debe estar guardado en un lugar más seguro, tal como en archivos ENV y no traspasarlo directo al cliente

void UJumboVivoxGameInstance::Init()
{
	Super::Init();
	InitVivox();
}

void UJumboVivoxGameInstance::Shutdown()
{
	Super::Shutdown();

	VivoxVoiceClient->Uninitialize();

}

void UJumboVivoxGameInstance::InitVivox()
{

	VivoxVoiceClient = &static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")))->VoiceClient();

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		VivoxVoiceClient->Initialize();
	}

}

// JumboVivoxGameInstance.cpp
void UJumboVivoxGameInstance::JoinVoiceChannel(const FString& ChannelName, FString LoginName)
{
	UE_LOG(LogTemp, Log, TEXT("JoinVoiceChannel called for user: %s"), *LoginName);

	ILoginSession& MyLoginSession = VivoxVoiceClient->GetLoginSession(LoggedInUserId);
	ChannelId channel = ChannelId(VIVOX_VOICE_ISSUER, ChannelName, VIVOX_VOICE_DOMAIN, ChannelType::NonPositional);
	IChannelSession& ChannelSession = MyLoginSession.GetChannelSession(channel);

	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString JoinToken = ChannelSession.GetConnectToken(VIVOX_VOICE_KEY, TokenExpiration);

	UE_LOG(LogTemp, Log, TEXT("JoinToken: %s"), *JoinToken);

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginChannelCompleted;

	OnBeginChannelCompleted.BindLambda([this, ChannelName, LoginName](VivoxCoreError Error)
		{

			UE_LOG(LogTemp, Log, TEXT("User: %s. In delegate!"), *LoginName);

			UE_LOG(LogTemp, Log, TEXT("OnBeginChannelCompleted called with error: %d"), Error);

			if (Error != VxErrorSuccess)
			{
				// Agrega esta línea para registrar el error
				UE_LOG(LogTemp, Warning, TEXT("Error connecting to voice channel: %d"), Error);
			}

			UE_LOG(LogTemp, Log, TEXT("User: %s. In Channel: %s"), *LoginName, *ChannelName);
		});


	if (&ChannelSession != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Calling BeginConnect for user: %s"), *LoginName);
		// Si es válido, procede a llamar a BeginConnect
		ChannelSession.BeginConnect(true, false, true, JoinToken, OnBeginChannelCompleted);
	}
	else
	{
		// Si no es válido, muestra un mensaje de error
		UE_LOG(LogTemp, Error, TEXT("ChannelSession is not valid for user: %s"), *LoginName);
	}

}


void UJumboVivoxGameInstance::JoinChannel()
{
	ILoginSession& MyLoginSession = VivoxVoiceClient->GetLoginSession(LoggedInUserId);
	// Para conectar dos usuarios a un canal se puede setear dinámicamente el ChannelId
	MyChannelId = ChannelId(VIVOX_VOICE_ISSUER, "ChannelId", VIVOX_VOICE_DOMAIN, ChannelType::Echo);
	IChannelSession& ChannelSession = MyLoginSession.GetChannelSession(MyChannelId);

	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString JoinToken = ChannelSession.GetConnectToken(VIVOX_VOICE_KEY, TokenExpiration);

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginChannelCompleted;

	OnBeginChannelCompleted.BindLambda([this, &MyLoginSession](VivoxCoreError Error)
		{

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Entraste al canal! :)"));
				
			}

	UE_LOG(LogTemp, Log, TEXT("Entraste al canal! :)"));
		}
	);

	ChannelSession.BeginConnect(true, false, true, JoinToken, OnBeginChannelCompleted);
}

void UJumboVivoxGameInstance::LeaveVoiceChannel()
{
	ILoginSession& MyLoginSession = VivoxVoiceClient->GetLoginSession(LoggedInUserId);

	MyLoginSession.GetChannelSession(MyChannelId).Disconnect();
	MyLoginSession.DeleteChannelSession(MyChannelId);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Saliste del canal"));
	}
}

void UJumboVivoxGameInstance::Login(FString ChannelName, FString LoginName)
{
	// Puedo hacer cualquier cosa antes del login, tal como obtener credenciales del usuario desde el backend

	// Obs: El player id debería ser el playerId que asigna OWS

	FString LName = FString::Printf(TEXT("Player%d"), FPlatformTime::Cycles());
	AccountId UserId = AccountId(VIVOX_VOICE_ISSUER, LName, VIVOX_VOICE_DOMAIN);

	LoggedInUserId = UserId;;

	ILoginSession& MyLoginSession(VivoxVoiceClient->GetLoginSession(UserId));

	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString LoginToken = MyLoginSession.GetLoginToken(VIVOX_VOICE_KEY, TokenExpiration); // la creación de token de accesos debería ser en el servidor (API)

	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleted;

	OnBeginLoginCompleted.BindLambda([this, &MyLoginSession, ChannelName, LName](VivoxCoreError Error)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Loggeado en Vivox! usuario: %s, al canal: %s"), *LName, *ChannelName));
			}
			JoinVoiceChannel(ChannelName, LName);
		});

	MyLoginSession.BeginLogin(VIVOX_VOICE_SERVER, LoginToken, OnBeginLoginCompleted);
}



void UJumboVivoxGameInstance::Logout()
{
	ILoginSession& MyLoginSession = VivoxVoiceClient->GetLoginSession(LoggedInUserId);

	MyLoginSession.GetChannelSession(MyChannelId).Disconnect();

	MyLoginSession.DeleteChannelSession(MyChannelId);
	MyLoginSession.Logout();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Cerraste sesion"));
	}
}