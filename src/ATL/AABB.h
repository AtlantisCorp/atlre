//
//  AABB.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef AABB_h
#define AABB_h

#include "Platform.h"

namespace Atl
{
    struct EXPORTED AABB
    {
        rvec3 min, max;

        //! @brief Returns the center point of the AABB.
        inline rvec3 center() const;
    };

    inline rvec3 AABB::center() const
    {
        return min + ((max - min) * rvec3(0.5, 0.5, 0.5));
    }
}

#endif // AABB_h