//
//  Camera.h
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#ifndef Camera_h
#define Camera_h

#include "Renderable.h"
#include "Touchable.h"
#include "Emitter.h"
#include "Transformation.h"

namespace Atl
{
    class Camera;

    class CameraListener : public ListenerBase
    {
    public:
        virtual ~CameraListener() = default;

        virtual void onCameraDidMove(Camera&, const rvec3&) {}
    };

    class Camera : 
        virtual public std::enable_shared_from_this < Camera >,
        virtual public Renderable,
        virtual public TimeTouchable,
        virtual public Emitter
    {
        std::string mName;

        rvec3 mPosition;

        rquat mOrientation;

        rvec3 mFixForward;

        rvec3 mFixRight;

        rvec3 mFixUp;

        mutable Transformation mTransformation;

        mutable std::mutex mMutex;

    public:
        ATL_SHAREABLE(Camera)

        typedef CameraListener Listener;

        Camera(const std::string& name, 
            const rvec3& position, 
            const rvec3& target,
            const std::string& transformationName = "view",
            const rvec3& fixedForward = rvec3(0.0, 0.0, -1.0),
            const rvec3& fixedRight = rvec3(1.0, 0.0, 0.0),
            const rvec3& fixedUp = rvec3(0.0, 1.0, 0.0));

        virtual ~Camera() = default;

        virtual void translate(const rvec3& value);

        virtual rvec3 position() const;

        virtual void setPosition(const rvec3& value);

        virtual rvec3 target() const;

        virtual void setTarget(const rvec3& value);

        virtual rvec3 direction() const;

        virtual rvec3 forward() const;

        virtual rvec3 right() const;

        virtual rvec3 up() const;

        virtual void rotate(const rvec3& axis, Real angle);

        virtual void rotateX(Real angle);

        virtual void rotateY(Real angle);

        virtual void rotateZ(Real angle);

        virtual std::future < void > build(Renderer& renderer);

        virtual std::future < void > render(RenderCommand& command) const;

        virtual std::size_t size(Renderer&) const;

        virtual rmat4x4 matrix() const;

        //! @brief Computes the distance between the Position of this camera and
        //! the given point (returns |rhs - mPosition|).
        virtual rvec3 distance(const rvec3& rhs) const;
    };

    typedef std::shared_ptr < Camera > CameraPtr;
    typedef std::vector < CameraPtr > CameraList;
}

#endif // Camera_h