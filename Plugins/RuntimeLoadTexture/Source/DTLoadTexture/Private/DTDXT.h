// Copyright 2022-2023 Dexter.Wan. All Rights Reserved. 
// EMail: 45141961@qq.com


#pragma once

namespace dt
{

#ifndef STB_INCLUDE_STB_DXT_H
#define STB_INCLUDE_STB_DXT_H


#define NEW_OPTIMISATIONS

#define STB_DXT_NORMAL    0
#define STB_DXT_DITHER    1   
#define STB_DXT_HIGHQUAL  2   

void DT_DXTCompress( unsigned char *dst, unsigned char *src, int w, int h, int isDxt5 );
void DT_DXTCompressYCoCg( unsigned char *dst, unsigned char *src, int w, int h );
void DT_Linearize( unsigned char * dst, const unsigned char * src, int n );
void DT_Compress_DXT_Block(unsigned char *dest, const unsigned char *src, int alpha, int mode);


#endif // STB_INCLUDE_STB_DXT_H

};
