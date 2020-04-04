//
//  Color.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_COLOR_H
#define ATL_COLOR_H

#include "Platform.h"

namespace Atl
{
    struct RGBAColor;
    struct RGBAuColor; 

    //! @brief A RGBA Float Color.
    struct RGBAColor
    {
        union 
        {
            struct 
            {
                float red, green, blue, alpha;
            };

            struct 
            {
                float values[4];
            };
        };
        

        //! @brief Constructs an empty color.
        RGBAColor();

        //! @brief Constructs a RGBAColor.
        RGBAColor(float r, float g, float b, float a = 1.0f);

        //! @brief Constructs a RGBAColor from an array of 4 floats.
        explicit RGBAColor(const float* arr);

        //! @brief Constructs a RGBAColor from a RGBAuColor.
        explicit RGBAColor(const RGBAuColor& rhs);

        //! @brief Converts a 8 bits channel to a float channel.
        static inline float ChannelFrom(uint8_t channel) { return 1.0f / (float)channel; }
    };

    //! @brief A RGBA Color in an unsigned int.
    struct RGBAuColor
    {
        //! @brief The 4 channels components.
        union 
        {
            struct 
            {
                uint8_t red, green, blue, alpha;
            };

            uint32_t all;
            uint8_t channels[4];

        } mChannels;

        //! @brief Constructs an empty color.
        RGBAuColor();

        //! @brief Constructs a RGBAuColor from 4 8 bits values.
        RGBAuColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        //! @brief Constructs a RGBAuColor from a RGBAColor.
        explicit RGBAuColor(const RGBAColor& rhs);

        //! @brief Returns the red channel.
        inline uint8_t& red() { return mChannels.red; }
        //! @brief Returns the green channel.
        inline uint8_t& green() { return mChannels.green; }
        //! @brief Returns the blue channel.
        inline uint8_t& blue() { return mChannels.blue; }
        //! @brief Returns the alpha channel.
        inline uint8_t& alpha() { return mChannels.alpha; }

        //! @brief Returns the red channel.
        inline const uint8_t& red() const { return mChannels.red; }
        //! @brief Returns the green channel.
        inline const uint8_t& green() const { return mChannels.green; }
        //! @brief Returns the blue channel.
        inline const uint8_t& blue() const { return mChannels.blue; }
        //! @brief Returns the alpha channel.
        inline const uint8_t& alpha() const { return mChannels.alpha; }

        //! @brief Returns the 32 bits value.
        inline uint32_t& value() { return mChannels.all; }
        //! @brief Returns the 32 bits value.
        inline const uint32_t& value() const { return mChannels.all; }

        //! @brief Converts a float channel to a 8 bits channel.
        static inline uint8_t ChannelFrom(float channel) { return static_cast < uint8_t >(channel * 255); }
    };
}

#endif