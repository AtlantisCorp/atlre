//
//  Frustum.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "Frustum.h"

namespace Atl
{
    Frustum::Frustum(rmat4x4 m)
    {
        m = glm::transpose(m);
        m_planes[Left]   = m[3] + m[0];
        m_planes[Right]  = m[3] - m[0];
        m_planes[Bottom] = m[3] + m[1];
        m_planes[Top]    = m[3] - m[1];
        m_planes[Near]   = m[3] + m[2];
        m_planes[Far]    = m[3] - m[2];

        rvec3 crosses[Combinations] = {
            glm::cross(rvec3(m_planes[Left]),   rvec3(m_planes[Right])),
            glm::cross(rvec3(m_planes[Left]),   rvec3(m_planes[Bottom])),
            glm::cross(rvec3(m_planes[Left]),   rvec3(m_planes[Top])),
            glm::cross(rvec3(m_planes[Left]),   rvec3(m_planes[Near])),
            glm::cross(rvec3(m_planes[Left]),   rvec3(m_planes[Far])),
            glm::cross(rvec3(m_planes[Right]),  rvec3(m_planes[Bottom])),
            glm::cross(rvec3(m_planes[Right]),  rvec3(m_planes[Top])),
            glm::cross(rvec3(m_planes[Right]),  rvec3(m_planes[Near])),
            glm::cross(rvec3(m_planes[Right]),  rvec3(m_planes[Far])),
            glm::cross(rvec3(m_planes[Bottom]), rvec3(m_planes[Top])),
            glm::cross(rvec3(m_planes[Bottom]), rvec3(m_planes[Near])),
            glm::cross(rvec3(m_planes[Bottom]), rvec3(m_planes[Far])),
            glm::cross(rvec3(m_planes[Top]),    rvec3(m_planes[Near])),
            glm::cross(rvec3(m_planes[Top]),    rvec3(m_planes[Far])),
            glm::cross(rvec3(m_planes[Near]),   rvec3(m_planes[Far]))
        };

        m_points[0] = intersection<Left,  Bottom, Near>(crosses);
        m_points[1] = intersection<Left,  Top,    Near>(crosses);
        m_points[2] = intersection<Right, Bottom, Near>(crosses);
        m_points[3] = intersection<Right, Top,    Near>(crosses);
        m_points[4] = intersection<Left,  Bottom, Far>(crosses);
        m_points[5] = intersection<Left,  Top,    Far>(crosses);
        m_points[6] = intersection<Right, Bottom, Far>(crosses);
        m_points[7] = intersection<Right, Top,    Far>(crosses);

    }

    // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
    bool Frustum::isBoxVisible(const rvec3& minp, const rvec3& maxp) const
    {
        // check box outside/inside of frustum
        for (int i = 0; i < Count; i++)
        {
            if ((glm::dot(m_planes[i], rvec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], rvec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
            {
                return false;
            }
        }

        // check frustum outside/inside box
        int out;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

        return true;
    }
}