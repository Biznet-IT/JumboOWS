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
void UJumboVivoxGameInstance::JoinVoiceChannel(const FString& ChannelName)
{
	ILoginSession& MyLoginSession = VivoxVoiceClient->GetLoginSession(LoggedInUserId);
	MyChannelId = ChannelId(VIVOX_VOICE_ISSUER, ChannelName, VIVOX_VOICE_DOMAIN, ChannelType::NonPositional);
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
		});

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

void UJumboVivoxGameInstance::Login()
{
	// Puedo hacer cualquier cosa antes del login, tal como obtener credenciales del usuario desde el backend
	int RandomNumber = UKismetMathLibrary::RandomIntegerInRange(0, 500);


	// Obs: El player id debería ser el playerId que asigna OWS
	LoggedInUserId = AccountId(VIVOX_VOICE_ISSUER, FString::FromInt(RandomNumber), VIVOX_VOICE_DOMAIN);
	ILoginSession& MyLoginSession(VivoxVoiceClient->GetLoginSession(LoggedInUserId));

	FTimespan TokenExpiration = FTimespan::FromSeconds(90);
	FString LoginToken = MyLoginSession.GetLoginToken(VIVOX_VOICE_KEY, TokenExpiration); // la creación de token de accesos debería ser en el servidor (API)

	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleted;

	OnBeginLoginCompleted.BindLambda([this, &MyLoginSession](VivoxCoreError Error)
	{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Loggeado en Vivox! :)"));
			}
		UE_LOG(LogTemp, Log, TEXT("Logged into Vivox! :)"));
		JoinChannel();
	});

	MyLoginSession.BeginLogin(VIVOX_VOICE_SERVER, LoginToken, OnBeginLoginCompleted);
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

			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("Entraste al canal! :)"));
			}

			UE_LOG(LogTemp, Log, TEXT("Entraste al canal! :)"));
		}
	);

	ChannelSession.BeginConnect(true, false, true, JoinToken, OnBeginChannelCompleted);
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
