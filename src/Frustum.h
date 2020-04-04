//
//  Frustum.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef Frustum_h
#define Frustum_h

#include "Platform.h"

namespace Atl
{
    class Frustum
    {
    public:
        Frustum() {}

        // m = ProjectionMatrix * ViewMatrix 
        Frustum(rmat4x4 m);

        // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
        bool isBoxVisible(const rvec3& minp, const rvec3& maxp) const;

    private:
        enum Planes
        {
            Left = 0,
            Right,
            Bottom,
            Top,
            Near,
            Far,
            Count,
            Combinations = Count * (Count - 1) / 2
        };

        template<Planes i, Planes j>
        struct ij2k
        {
            enum { k = i * (9 - i) / 2 + j - 1 };
        };

        template<Planes a, Planes b, Planes c>
        rvec3 intersection(const rvec3* crosses) const;
        
        rvec4   m_planes[Count];
        rvec3   m_points[8];
    };

    template<Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
    inline rvec3 Frustum::intersection(const rvec3* crosses) const
    {
        float D = glm::dot(rvec3(m_planes[a]), crosses[ij2k<b, c>::k]);
        rvec3 res = rmat3x3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
            rvec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
        return res * (-1.0f / D);
    }
}

#endif // Frustum_h
