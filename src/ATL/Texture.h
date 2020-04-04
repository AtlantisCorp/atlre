//
//  Texture.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_TEXTURE_H
#define ATL_TEXTURE_H

#include "Platform.h"

namespace Atl
{
    class Texture 
    {
    public:

        virtual bool isTransparent() const = 0;
    };

    typedef std::shared_ptr < Texture > TexturePtr;
}

#endif // ATL_TEXTURE_H