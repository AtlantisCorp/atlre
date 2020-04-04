//
//  RenderSceneGroup.cpp
//  atlre
//
//  Created by jacques tronconi on 02/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "RenderSceneGroup.h"

namespace Atl
{
    RenderSceneGroup::RenderSceneGroup()
    {
        mScenes = RenderSceneManager::New();
        mCameras = CameraManager::New();
        mTechniques = RenderTechniqueManager::New();
        mCommands = RenderCommandManager::New();
    }

    RenderSceneGroup::RenderSceneGroup(const RenderSceneManagerPtr& scenes, const CameraManagerPtr& cameras, const RenderTechniqueManagerPtr& techniques, const RenderCommandManagerPtr& commands)
    {
        mScenes = scenes ? scenes : RenderSceneManager::New();
        mCameras = cameras ? cameras : CameraManager::New();
        mTechniques = techniques ? techniques : RenderTechniqueManager::New();
        mCommands = commands ? commands : RenderCommandManager::New();
    }

    std::size_t RenderSceneGroup::addScene(const RenderScenePtr& scene)
    {
        return mScenes->add(scene);
    }

    std::size_t RenderSceneGroup::addTechnique(const RenderTechniquePtr& technique)
    {
        return mTechniques->add(technique);
    }

    std::size_t RenderSceneGroup::addCamera(const CameraPtr& camera)
    {
        return mCameras->add(camera);
    }

    std::size_t RenderSceneGroup::addCommand(const RenderCommandPtr& command)
    {
        return mCommands->add(command);
    }

    RenderScene& RenderSceneGroup::sceneAt(std::size_t idx)
    {
        return mScenes->objectAt(idx);
    }

    const RenderScene& RenderSceneGroup::sceneAt(std::size_t idx) const
    {
        return mScenes->objectAt(idx);
    }

    std::size_t RenderSceneGroup::sceneCount() const
    {
        return mScenes->objectsCount();
    }

    RenderTechnique& RenderSceneGroup::techniqueAt(std::size_t idx)
    {
        return mTechniques->objectAt(idx);
    }

    const RenderTechnique& RenderSceneGroup::techniqueAt(std::size_t idx) const
    {
        return mTechniques->objectAt(idx);
    }

    RenderTechniquePtr RenderSceneGroup::techniquePtrAt(std::size_t idx) const
    {
        return mTechniques->objects()[idx];
    }

    std::size_t RenderSceneGroup::techniqueCount() const
    {
        return mTechniques->objectsCount();
    }

    Camera& RenderSceneGroup::cameraAt(std::size_t idx)
    {
        return mCameras->objectAt(idx);
    }

    const Camera& RenderSceneGroup::cameraAt(std::size_t idx) const
    {
        return mCameras->objectAt(idx);
    }

    std::size_t RenderSceneGroup::cameraCount() const
    {
        return mCameras->objectsCount();
    }

    RenderCommand& RenderSceneGroup::commandAt(std::size_t idx)
    {
        return mCommands->objectAt(idx);
    }

    const RenderCommand& RenderSceneGroup::commandAt(std::size_t idx) const
    {
        return mCommands->objectAt(idx);
    }

    std::size_t RenderSceneGroup::commandCount() const
    {
        return mCommands->objectsCount();
    }

    void RenderSceneGroup::selectTechnique(std::size_t techniqueIdx, std::size_t sceneIdx)
    {
        LockableGuard < RenderTechniqueManager, RenderSceneManager > l(*mTechniques, *mScenes);
        RenderTechnique& technique = mTechniques->objectAt(techniqueIdx);
        RenderScene& scene = mScenes->objectAt(sceneIdx);
        scene.setTechnique(technique.shared_from_this());
    }

    void RenderSceneGroup::selectCommand(std::size_t commandIdx, std::size_t sceneIdx)
    {
        std::lock_guard l(mMutex);
        mCommandForScene[commandIdx] = sceneIdx;
        touch();
    }

    void RenderSceneGroup::selectCamera(std::size_t cameraIdx, std::size_t sceneIdx)
    {
        LockableGuard < CameraManager, RenderSceneManager > l(*mCameras, *mScenes);
        Camera& camera = mCameras->objectAt(cameraIdx);
        RenderScene& scene = mScenes->objectAt(sceneIdx);
        scene.setCamera(camera.shared_from_this());
    }

    void RenderSceneGroup::selectAll(std::size_t technique, std::size_t camera, std::size_t command, std::size_t scene)
    {
        LockableGuard < RenderSceneManager,
        RenderTechniqueManager,
        CameraManager > l(*mScenes, *mTechniques, *mCameras);

        RenderTechnique& _technique = mTechniques->objectAt(technique);
        Camera& _camera = mCameras->objectAt(camera);
        RenderScene& _scene = mScenes->objectAt(scene);
        _scene.setCamera(_camera.shared_from_this());
        _scene.setTechnique(_technique.shared_from_this());

        std::lock_guard ll(mMutex);
        mCommandForScene[command] = scene;

        TimeTouchable::touch();
    }

    std::future < void > RenderSceneGroup::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]()
        {
            if (!isTouched())
                return;

            std::lock_guard l(mMutex);

            for (auto const& pair : mCommandForScene)
            {
                const RenderScene& scene = sceneAt(pair.second);
                RenderCommand& command = const_cast < RenderCommand& >(commandAt(pair.first));

                if (!scene.isTouched())
                    continue;

                command.removeAllSubCommands();
                scene.render(command);
            }
        });
    }

    std::size_t RenderSceneGroup::makeCommand(Renderer& renderer)
    {
        return addCommand(renderer.newCommand < RenderCommand >());
    }

    std::size_t RenderSceneGroup::addSceneCommand(const RenderScenePtr& scene, Renderer& renderer)
    {
        std::size_t sceneId = addScene(scene);
        selectCommand(makeCommand(renderer), sceneId);
        return sceneId;
    }

    void RenderSceneGroup::setSharedNode(const RenderNodePtr& node)
    {
        LockableGuard l(*mScenes);
        std::atomic_store(&mSharedNode, node);

        for (std::size_t i = 0; i < mScenes->objectsCount(); ++i)
        {
            RenderScene& scene = mScenes->objectAt(i);
            scene.setRoot(node);
        }
    }

    std::size_t RenderSceneGroup::makeNewScene(const std::string& name, Renderer& renderer, const TechniqueOrIdx& technique)
    {
        LockableGuard l(*mTechniques);

        RenderTechniquePtr _technique = std::holds_alternative < RenderTechniquePtr >(technique) ? 
            std::get < RenderTechniquePtr >(technique) :
            techniqueAt(std::get < std::size_t >(technique)).shared_from_this();

        RenderScenePtr _scene = RenderScene::New(name, std::atomic_load(&mSharedNode), nullptr, _technique);
        return addSceneCommand(_scene, renderer);
    }

    void RenderSceneGroup::lock() const
    {
        mMutex.lock();
        mScenes->lock();
        mCameras->lock();
        mTechniques->lock();
        mCommands->lock();
    }

    void RenderSceneGroup::unlock() const
    {
        mMutex.unlock();
        mScenes->unlock();
        mCameras->unlock();
        mTechniques->unlock();
        mCommands->unlock();
    }

    RenderSceneGroup::RenderSceneManager& RenderSceneGroup::sceneManager()
    {
        return *mScenes;
    }

    const RenderSceneGroup::RenderSceneManager& RenderSceneGroup::sceneManager() const
    {
        return *mScenes;
    }

    RenderSceneGroup::RenderTechniqueManager& RenderSceneGroup::techniqueManager()
    {
        return *mTechniques;
    }

    const RenderSceneGroup::RenderTechniqueManager& RenderSceneGroup::techniqueManager() const
    {
        return *mTechniques;
    }

    RenderSceneGroup::CameraManager& RenderSceneGroup::cameraManager()
    {
        return *mCameras;
    }

    const RenderSceneGroup::CameraManager& RenderSceneGroup::cameraManager() const
    {
        return *mCameras;
    }

    RenderSceneGroup::RenderCommandManager& RenderSceneGroup::commandManager()
    {
        return *mCommands;
    }

    const RenderSceneGroup::RenderCommandManager& RenderSceneGroup::commandManager() const
    {
        return *mCommands;
    }

    void RenderSceneGroup::onObjectAdded(RenderSceneManager& manager, RenderScene& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectRemoved(RenderSceneManager& manager, RenderScene& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectAdded(CameraManager& manager, Camera& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectRemoved(CameraManager& manager, Camera& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectAdded(RenderTechniqueManager& manager, RenderTechnique& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectRemoved(RenderTechniqueManager& manager, RenderTechnique& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectAdded(RenderCommandManager& manager, RenderCommand& scene)
    {
        touch();
    }

    void RenderSceneGroup::onObjectRemoved(RenderCommandManager& manager, RenderCommand& scene)
    {
        touch();
    }
}
