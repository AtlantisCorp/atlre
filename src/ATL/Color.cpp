//
//  Color.cpp
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#include "Color.h"

namespace Atl
{
    RGBAColor::RGBAColor()
    : red(0.0f), green(0.0f), blue(0.0f), alpha(0.0f)
    {

    }

    RGBAColor::RGBAColor(float r, float g, float b, float a)
    : red(r), green(g), blue(b), alpha(a) 
    {
        
    }

    RGBAColor::RGBAColor(const float* arr)
    : red(arr[0]), green(arr[1]), blue(arr[2]), alpha(arr[3])
    {

    }

    RGBAColor::RGBAColor(const RGBAuColor& rhs)
    {
        red = ChannelFrom(rhs.red());
        green = ChannelFrom(rhs.green());
        blue = ChannelFrom(rhs.blue());
        alpha = ChannelFrom(rhs.alpha());
    }

    RGBAuColor::RGBAuColor()
    {
        mChannels.all = 0;
    }

    RGBAuColor::RGBAuColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        mChannels.red = r;
        mChannels.green = g;
        mChannels.blue = b;
        mChannels.alpha = a;
    }

    RGBAuColor::RGBAuColor(const RGBAColor& rhs)
    {
        mChannels.red = ChannelFrom(rhs.red);
        mChannels.green = ChannelFrom(rhs.green);
        mChannels.blue = ChannelFrom(rhs.blue);
        mChannels.alpha = ChannelFrom(rhs.alpha);
    }
}