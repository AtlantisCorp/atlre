//
//  Transformation.cpp
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#include "Transformation.h"
#include "TransformationRenderCache.h"

namespace Atl
{
    Transformation::Transformation(const std::string& name)
    {
        mMatrix = glm::identity < rmat4x4 >();
        mName = name;
    }
    
    Transformation::Transformation(const std::string& name, const rmat4x4& mat)
    {
        mMatrix = mat;
        mName = name;
    }

    Transformation::Transformation(const std::string& name, const rvec3& position, const rvec3& scale)
    {
        mMatrix = rmat4x4();
        mMatrix = glm::scale(glm::translate(mMatrix, position), scale);
        mName = name;
    }

    Transformation::Transformation(const std::string& name, const rvec3& axis, const Real& angle)
    {
        mMatrix = glm::rotate(rmat4x4(), angle, axis);
        mName = name;
    }

    Transformation::Transformation(const Transformation& rhs)
    {
        std::lock_guard l(rhs.mMutex);
        mMatrix = rhs.mMatrix;
        mName = rhs.mName;
    }

    Transformation& Transformation::operator = (const Transformation& rhs)
    {
        std::lock_guard l(rhs.mMutex);
        std::lock_guard ll(mMutex);
        mMatrix = rhs.mMatrix;
        mName = rhs.mName;
        mCache.touchAllCaches();
        return *this;
    }

    Transformation& Transformation::translate(const rvec3& rhs)
    {
        std::lock_guard l(mMutex);
        mMatrix = glm::translate(mMatrix, rhs);
        mCache.touchAllCaches();
        return *this;
    }

    Transformation& Transformation::scale(const rvec3& rhs)
    {
        std::lock_guard l(mMutex);
        mMatrix = glm::scale(mMatrix, rhs);
        mCache.touchAllCaches();
        return *this;
    }

    Transformation& Transformation::rotate(const rvec3& axis, const Real& angle)
    {
        std::lock_guard l(mMutex);
        mMatrix = glm::rotate(mMatrix, angle, axis);
        mCache.touchAllCaches();
        return *this;
    }

    Transformation Transformation::LookAt(const std::string& name, const rvec3& from, const rvec3& to, const rvec3& up)
    {
        return Transformation(name, glm::lookAt(from, to, up));
    }

    std::future < void > Transformation::build(Renderer& renderer) 
    {
        return std::async(std::launch::async, [this, &renderer]()
        {
            std::lock_guard l(mMutex);

            // Checks if the cache has been built for given renderer.

            RenderCachePtr < Transformation > cache = mCache.cacheFor(renderer);
            
            if (!cache)
            {
                // Creates our ShaderVariable.

                ShaderVariable variable(mName, &mMatrix[0][0], SVT::MatrixR4x4, 1);

                // Builds a new cache for renderer.

                TransformationRenderCachePtr newCache = TransformationRenderCache::New(renderer, *this);
                newCache->command()->setShaderVariable(variable);

                // Registers our new cache (and cleans it at we are sure it is up to date).

                mCache.addCache(newCache);
                mCache.cleanCache(newCache);

                return;
            }

            // Sets our updated shader variable.

            TransformationRenderCache* pCache = reinterpret_cast < TransformationRenderCache* >(cache.get());
            pCache->command()->setVariableValue(&mMatrix[0][0]);

            // Cleans our cache.

            mCache.cleanCache(cache);
        });
    }

    std::future < void > Transformation::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]()
        {
            mMutex.lock();

            // Checks if we have a render cache.

            RenderCachePtr < Transformation > cache = mCache.cacheFor(command.renderer());

            if (!cache)
            {
                // Unlocks the mutex, rebuild the cache and relaunch this function.
                // All async functions are waited for here.

                mMutex.unlock();

                const_cast < Transformation& >(*this)
                .build(command.renderer()).get();

                return render(command).get();
            }

            // Checks if cache is touched, in this case, rebuild it.
            // Actually this doesn't recreate the cache, but only updates it with the new
            // value from our matrix.

            if (mCache.isCacheTouched(cache))
            {
                // Unlocks the mutex, rebuild the cache and relaunch this function.
                // All async functions are waited for here.

                mMutex.unlock();

                const_cast < Transformation& >(*this)
                .build(command.renderer()).get();

                return render(command).get();
            }

            // Renders the cache into the command.

            cache->render(command).get();
            mMutex.unlock();
            
            return;
        });
    }

    std::size_t Transformation::size(Renderer&) const
    {
        return sizeof(Real) * 16;
    }

    rvec3 Transformation::translation() const
    {
        std::lock_guard l(mMutex);
        return rvec3(mMatrix[3]);
    }

    namespace detail
    {
        template<typename T, glm::qualifier Q>
        glm::vec<3, T, Q> combine(glm::vec<3, T, Q> const& a, glm::vec<3, T, Q> const& b, T ascl, T bscl)
        {
            return (a * ascl) + (b * bscl);
        }

        template<typename T, glm::qualifier Q>
        glm::vec<3, T, Q> scale(glm::vec<3, T, Q> const& v, T desiredLength)
        {
            return v * desiredLength / length(v);
        }
    }

    template < typename T, glm::qualifier Q >
    bool decomposeScaleSkew(glm::mat<4, 4, T, Q> const& ModelMatrix, glm::vec<3, T, Q> & Scale, glm::vec<3, T, Q> & Skew)
    {
        using namespace glm;

        mat<4, 4, T, Q> LocalMatrix(ModelMatrix);

        // Normalize the matrix.
        if(epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), epsilon<T>()))
            return false;

        for(length_t i = 0; i < 4; ++i)
        for(length_t j = 0; j < 4; ++j)
            LocalMatrix[i][j] /= LocalMatrix[3][3];

        vec<3, T, Q> Row[3], Pdum3;

        // Now get scale and shear.
        for(length_t i = 0; i < 3; ++i)
        for(length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        Scale.x = length(Row[0]);// v3Length(Row[0]);

        Row[0] = detail::scale(Row[0], static_cast<T>(1));

        // Compute XY shear factor and make 2nd row orthogonal to 1st.
        Skew.z = dot(Row[0], Row[1]);
        Row[1] = detail::combine(Row[1], Row[0], static_cast<T>(1), -Skew.z);

        // Now, compute Y scale and normalize 2nd row.
        Scale.y = length(Row[1]);
        Row[1] = detail::scale(Row[1], static_cast<T>(1));
        Skew.z /= Scale.y;

        // Compute XZ and YZ shears, orthogonalize 3rd row.
        Skew.y = glm::dot(Row[0], Row[2]);
        Row[2] = detail::combine(Row[2], Row[0], static_cast<T>(1), -Skew.y);
        Skew.x = glm::dot(Row[1], Row[2]);
        Row[2] = detail::combine(Row[2], Row[1], static_cast<T>(1), -Skew.x);

        // Next, get Z scale and normalize 3rd row.
        Scale.z = length(Row[2]);
        Row[2] = detail::scale(Row[2], static_cast<T>(1));
        Skew.y /= Scale.z;
        Skew.x /= Scale.z;

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if(dot(Row[0], Pdum3) < 0)
        {
            for(length_t i = 0; i < 3; i++)
            {
                Scale[i] *= static_cast<T>(-1);
            }
        }

        return true;
    }

    rvec3 Transformation::scale() const
    {
        std::lock_guard l(mMutex);
        
        rvec3 rScale, rSkew;
        decomposeScaleSkew(mMatrix, rScale, rSkew);

        return rScale;
    }

    rquat Transformation::rotation() const
    {
        std::lock_guard l(mMutex);

        rvec3 rScale, rSkew, rTrans;
        rvec4 rPers;
        rquat rOrientation;
        glm::decompose(mMatrix, rScale, rOrientation, rTrans, rSkew, rPers);

        return rOrientation;
    }

    rmat4x4 Transformation::matrix() const
    {
        std::lock_guard l(mMutex);
        return mMatrix;
    }

    std::string Transformation::name() const
    {
        std::lock_guard l(mMutex);
        return mName;
    }

    void Transformation::setMatrix(const rmat4x4& rhs)
    {
        std::lock_guard l(mMutex);
        mMatrix = rhs;
        mCache.touchAllCaches();
    }
}
