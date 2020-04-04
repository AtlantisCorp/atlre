//
//  Camera.cpp
//  atl
//
//  Created by jacques tronconi on 26/03/2020.
//

#include "Camera.h"
#include "RenderCommand.h"

namespace Atl
{
    Camera::Camera(const std::string& name,
        const rvec3& position, 
        const rvec3& target,
        const std::string& transformationName,
        const rvec3& fixedForward,
        const rvec3& fixedRight,
        const rvec3& fixedUp)
    : mName(name), mPosition(position), mTransformation(transformationName),
        mFixForward(fixedForward), mFixRight(fixedRight), mFixUp(fixedUp)
    {
        const rvec3 direction = glm::normalize(mPosition - target);
        mOrientation = glm::quatLookAt(direction, rvec3(0.0, 1.0, 0.0));
        TimeTouchable::touch();
    }

    void Camera::translate(const rvec3& value)
    {
        {
            std::lock_guard l(mMutex);
            mPosition = mPosition + value;
        }

        TimeTouchable::touch();
        send(&Listener::onCameraDidMove, *this, value);
    }

    rvec3 Camera::position() const
    {
        std::lock_guard l(mMutex);
        return mPosition;
    }

    void Camera::setPosition(const rvec3& value)
    {
        {
            std::lock_guard l(mMutex);
            mPosition = value;
        }

        TimeTouchable::touch();
        send(&Listener::onCameraDidMove, *this, value);
    }

    rvec3 Camera::target() const
    {
        std::lock_guard l(mMutex);
        const rvec3 direction = mFixForward * mOrientation;
        return mPosition + direction;
    }

    void Camera::setTarget(const rvec3& value)
    {
        {
            std::lock_guard l(mMutex);
            const rvec3 direction = glm::normalize(mPosition - value);
            mOrientation = glm::quatLookAt(direction, mFixUp);
        }

        TimeTouchable::touch();
    }

    rvec3 Camera::direction() const
    {
        std::lock_guard l(mMutex);
        return mFixForward * mOrientation;
    }

    rvec3 Camera::forward() const
    {
        return direction();
    }

    rvec3 Camera::right() const
    {
        std::lock_guard l(mMutex);
        return mFixRight * mOrientation;
    }

    rvec3 Camera::up() const
    {
        std::lock_guard l(mMutex);
        return mFixUp * mOrientation;
    }

    void Camera::rotate(const rvec3& axis, Real angle)
    {
        {
            std::lock_guard l(mMutex);
            mOrientation = mOrientation * glm::angleAxis(angle, axis);
        }

        TimeTouchable::touch();
    }

    void Camera::rotateX(Real angle)
    {
        rotate(rvec3(1.0, 0.0, 0.0), angle);
    }

    void Camera::rotateY(Real angle)
    {
        rotate(rvec3(0.0, 1.0, 0.0), angle);
    }

    void Camera::rotateZ(Real angle)
    {
        rotate(rvec3(0.0, 0.0, 1.0), angle);
    }

    std::future < void > Camera::build(Renderer& renderer)
    {
        return std::async(std::launch::async, [this, &renderer]()
        {
            std::lock_guard l(mMutex);

            // We have to make the Matrix4x4 from our Position and Quaternion, 
            // and then we can set the matrix for the Transformation and use
            // Transformation::build().

            const rmat4x4 Translation = glm::translate(mPosition);
            const rmat4x4 Rotation = glm::toMat4(mOrientation);
            const rmat4x4 ViewMatrix = Translation * Rotation;

            mTransformation.setMatrix(ViewMatrix);
            mTransformation.build(renderer).get();

            TimeTouchable::clean();
        });
    }

    std::future < void > Camera::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]()
        {
            if (TimeTouchable::isTouched())
                const_cast < Camera& >(*this).build(command.renderer()).get();

            std::lock_guard l(mMutex);
            mTransformation.render(command);
        });
    }

    std::size_t Camera::size(Renderer&) const
    {
        return 0;
    }

    rmat4x4 Camera::matrix() const
    {
        std::lock_guard l(mMutex);
        const rmat4x4 Translation = glm::translate(mPosition);
        const rmat4x4 Rotation = glm::toMat4(mOrientation);
        return Translation * Rotation;
    }

    rvec3 Camera::distance(const rvec3& rhs) const
    {
        return glm::abs(rhs - position());
    }
}
