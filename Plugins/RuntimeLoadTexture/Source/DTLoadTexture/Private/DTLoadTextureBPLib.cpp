// Copyright 2022-2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com


#include "DTLoadTextureBPLib.h"
#include "Async/Async.h"
#include "Engine/Texture2D.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "DTDXT.h"
#include "DTImage.h"

DEFINE_LOG_CATEGORY(LogDTLoadTexture);

// 本地加载图片
UTexture2D * UDTLoadTextureBPLib::LoadTexture(const FString& FilePath)
{
	int nWidth = 0;
	int nHeight = 0;
	int nChannels = 0;
	const char* pszFilePath = TCHAR_TO_UTF8(*FilePath);
	unsigned char * pImageData = dt::dt_stbi_load(pszFilePath, &nWidth, &nHeight, &nChannels, dt::STBI_rgb_alpha);
	UTexture2D* pTexture = nullptr;

	do
	{
		// 无效图片
		if (pImageData == nullptr)
		{
			UE_LOG(LogDTLoadTexture, Error, TEXT("Read Image Fail : %s"), *FilePath);
			break;
		}

		// 载入错误
		if (nWidth == 0 || nHeight == 0 || nChannels == 0)
		{
			UE_LOG(LogDTLoadTexture, Error, TEXT("%s : %s"), UTF8_TO_TCHAR(pImageData), *FilePath);
			break;
		}

		// 是否含有透明通道
		const bool bIsHasAlpha = (nChannels == dt::STBI_grey_alpha || nChannels == dt::STBI_rgb_alpha);
		bool bUseRGBA = false;

		// 创建纹理
		pTexture = UTexture2D::CreateTransient(nWidth, nHeight, bIsHasAlpha ? PF_DXT5 : PF_DXT1);
		if ( pTexture == nullptr )
		{
			bUseRGBA = true;
			pTexture = UTexture2D::CreateTransient(nWidth, nHeight, PF_R8G8B8A8);
			if ( pTexture == nullptr )
			{
				UE_LOG(LogDTLoadTexture, Error, TEXT("Create Texture2D Fail : %s"), *FilePath);
				break;
			}
		}

		// 更新像素信息
		FByteBulkData & BulkData = pTexture->GetPlatformData()->Mips[0].BulkData;
		uint8* TextureData = (uint8*)(BulkData.Lock(LOCK_READ_WRITE));
		if ( bUseRGBA )
		{
			FMemory::Memcpy(TextureData, pImageData, BulkData.GetBulkDataSize());
		}
		else
		{
			dt::DT_DXTCompress(TextureData, pImageData, nWidth, nHeight, bIsHasAlpha ? 1 : 0);
		}
		BulkData.Unlock();

		// 当前线程执行
		if (IsInGameThread())
		{
			pTexture->UpdateResource();
		}
		else
		{
			// 回调到主线程更新资源
			AsyncTask(ENamedThreads::GameThread, [pTexture]{ pTexture->UpdateResource();} );
		}

	} while (false);

	// 释放内存，返回对象
	dt::dt_stbi_image_free(pImageData);
	return pTexture;
}

// 网络加载图片
void UDTLoadTextureBPLib::HttpTexture(const FString& URL, FHttpTexture OnHttpTexture)
{
	// 访问访问网页
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda([URL, OnHttpTexture](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		// 图片访问成功
		if ( bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 )
		{
			int nWidth = 0;
			int nHeight = 0;
			int nChannels = 0;
			unsigned char * pImageData = dt::dt_stbi_load_from_memory(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength(), &nWidth, &nHeight, &nChannels, dt::STBI_rgb_alpha);
			UTexture2D* pTexture = nullptr;

			do
			{
				// 无效图片
				if (pImageData == nullptr)
				{
					UE_LOG(LogDTLoadTexture, Error, TEXT("Read Image Fail : %s"), *URL);
					break;
				}

				// 载入错误
				if (nWidth == 0 || nHeight == 0 || nChannels == 0)
				{
					UE_LOG(LogDTLoadTexture, Error, TEXT("%s : %s"), UTF8_TO_TCHAR(pImageData), *URL);
					break;
				}

				// 是否含有透明通道
 				const bool bIsHasAlpha = (nChannels == dt::STBI_grey_alpha || nChannels == dt::STBI_rgb_alpha);
				bool bUseRGBA = false;

				// 创建纹理
				pTexture = UTexture2D::CreateTransient(nWidth, nHeight, bIsHasAlpha ? PF_DXT5 : PF_DXT1);
				if ( pTexture == nullptr )
				{
					bUseRGBA = true;
					pTexture = UTexture2D::CreateTransient(nWidth, nHeight, PF_R8G8B8A8);
					if ( pTexture == nullptr )
					{
						UE_LOG(LogDTLoadTexture, Error, TEXT("Create Texture2D Fail : %s"), *URL);
						break;
					}
				}

				// 更新像素信息
				FByteBulkData & BulkData = pTexture->GetPlatformData()->Mips[0].BulkData;
				uint8* TextureData = (uint8*)(BulkData.Lock(LOCK_READ_WRITE));
				if ( bUseRGBA )
				{
					FMemory::Memcpy(TextureData, pImageData, BulkData.GetBulkDataSize());
				}
				else
				{
					dt::DT_DXTCompress(TextureData, pImageData, nWidth, nHeight, bIsHasAlpha ? 1 : 0);
				}
				BulkData.Unlock();

				// 当前线程执行
				if (IsInGameThread())
				{
					pTexture->UpdateResource();
				}
				else
				{
					// 回调到主线程更新资源
					AsyncTask(ENamedThreads::GameThread, [pTexture]{ pTexture->UpdateResource();} );
				}

			} while (false);

			// 释放内存，返回对象
			dt::dt_stbi_image_free(pImageData);
			OnHttpTexture.ExecuteIfBound(URL, pTexture);
			return;
		}

		OnHttpTexture.ExecuteIfBound(URL, nullptr);
		return;
	});
	
	// 发起请求
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}