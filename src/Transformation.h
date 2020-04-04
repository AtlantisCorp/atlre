//
//  Transformation.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef Transformation_h
#define Transformation_h

#include "Renderable.h"
#include "PerRendererCache.h"

namespace Atl
{
    //! @brief Holds a Transformation's matrix for a Renderable.
    //! The Transformation holds a name, that will be used to bind the Transformation 
    //! into the rendering context.
    class EXPORTED Transformation : public Renderable
    {
        //! @brief The Matrix for this transformation.
        rmat4x4 mMatrix;

        //! @brief The Transformation's name.
        std::string mName;

        //! @brief The mutex.
        mutable std::mutex mMutex;

        //! @brief Holds RenderCaches for each Renderer.
        mutable PerRendererCache < Transformation > mCache;
        
    public:
        ATL_SHAREABLE(Transformation)

        //! @brief Constructs a new Transformation.
        Transformation(const std::string& name);

        //! @brief Constructs a new Transformation.
        Transformation(const std::string& name, const rmat4x4& matrix);
        
        //! @brief Constructs a new Transformation.
        //! @param name The Transformation's name.
        //! @param position The initial translation for the Transformation.
        //! @param scale The initial scale for the Transformation. Uses the vector
        //! (1.0, 1.0, 1.0) for no scale.
        Transformation(const std::string& name, const rvec3& position, const rvec3& scale);
        
        //! @brief Constructs a new Transformation.
        //! @param name The Transformation's name.
        //! @param axis The rotation's axis.
        //! @param angle The rotation's angle, in degrees.
        Transformation(const std::string& name, const rvec3& axis, const Real& angle);

        //! @brief Constructs a new Transformation from another one.
        Transformation(const Transformation& rhs);

        //! @brief Assignment operator.
        Transformation& operator = (const Transformation& rhs);
        
        //! @brief Translates the inner matrix.
        Transformation& translate(const rvec3& rhs);

        //! @brief Scales the inner matrix.
        Transformation& scale(const rvec3& rhs);

        //! @brief Rotates the inner matrix.
        //! @param axis The rotation's axis.
        //! @param angle The rotation's angle, in degree.
        Transformation& rotate(const rvec3& axis, const Real& angle);

        //! @brief Returns a Transformation with a look at matrix.
        static Transformation LookAt(const std::string& name, 
                                     const rvec3& position, 
                                     const rvec3& target, 
                                     const rvec3& up = rvec3(0.0, 1.0, 0.0));

        //! @brief Updates the TransformationRenderCache in this Transformation.
        //! If the RenderCache doesn't exist, creates it. If it exists, updates
        //! the cache's value with the current matrix.
        std::future < void > build(Renderer& renderer);

        //! @brief Renders the cache in the command.
        std::future < void > render(RenderCommand& command) const;

        //! @brief Returns the size of the matrix, i.e. 16 floats.
        std::size_t size(Renderer&) const;

        //! @brief Returns the translation part.
        rvec3 translation() const;

        //! @brief Returns the scale part.
        rvec3 scale() const;

        //! @brief Returns the rotation part.
        rquat rotation() const;

        //! @brief Returns the matrix.
        rmat4x4 matrix() const;

        //! @brief Returns the Transformation's name.
        std::string name() const;

        //! @brief Directly changes the Matrix.
        void setMatrix(const rmat4x4& rhs);
    };

    //! @brief Pointer to a Transformation.
    typedef std::shared_ptr < Transformation > TransformationPtr;
}

#endif // Transformation_h