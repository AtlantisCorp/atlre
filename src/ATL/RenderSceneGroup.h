//
//  RenderSceneGroup.h
//  atlre
//
//  Created by jacques tronconi on 02/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_RENDERSCENEGROUP_H
#define ATL_RENDERSCENEGROUP_H

#include "RenderScene.h"
#include "LockableManager.h"

namespace Atl
{
    //! @brief Defines a Group of RenderScene to manage multiple Scenes with the
    //! same set of Cameras and RenderTechniques.
    class RenderSceneGroup : 
        virtual public Renderable, 
        virtual public TimeTouchable,
        virtual public Lockable,
        virtual public LockableManagerListener < RenderScene >,
        virtual public LockableManagerListener < Camera >,
        virtual public LockableManagerListener < RenderTechnique >,
        virtual public LockableManagerListener < RenderCommand >
    {
        //! @brief The RenderScene's manager.
        typedef LockableManager < RenderScene > RenderSceneManager;
        //! @brief The Camera's manager.
        typedef LockableManager < Camera > CameraManager;
        //! @brief The RenderTechnique's manager.
        typedef LockableManager < RenderTechnique > RenderTechniqueManager;
        //! @brief The RenderCommand's manager.
        typedef LockableManager < RenderCommand > RenderCommandManager;

        //! @brief The RenderScene's manager.
        typedef LockableManagerPtr < RenderScene > RenderSceneManagerPtr;
        //! @brief The Camera's manager.
        typedef LockableManagerPtr < Camera > CameraManagerPtr;
        //! @brief The RenderTechnique's manager.
        typedef LockableManagerPtr < RenderTechnique > RenderTechniqueManagerPtr;
        //! @brief The RenderCommand's manager.
        typedef LockableManagerPtr < RenderCommand > RenderCommandManagerPtr;

        //! @brief The RenderScenes in this group.
        RenderSceneManagerPtr mScenes;

        //! @brief The shared Cameras.
        CameraManagerPtr mCameras;

        //! @brief The shared Techniques.
        RenderTechniqueManagerPtr mTechniques;

        //! @brief A list of RenderCommands.
        RenderCommandManagerPtr mCommands;

        //! @brief The shared root node tree.
        RenderNodePtr mSharedNode;

        //! @brief A map associating a RenderScene index and a RenderCommand index, 
        //! used to render each RenderScene inside a specific RenderCommand.
        std::map < std::size_t, std::size_t > mCommandForScene;

        //! @brief The mutex.
        mutable std::recursive_mutex mMutex;

    public:
        //! @brief A Technique Pointer or index value.
        typedef std::variant < RenderTechniquePtr, std::size_t > TechniqueOrIdx;

    public:

        //! @brief Constructs a new empty RenderSceneGroup.
        RenderSceneGroup();

        //! @brief Constructs a new RenderSceneGroup.
        //! @param scenes The manager for RenderScenes.
        //! @param cameras The manager for Camera.
        //! @param techniques The manager for RenderTechnique.
        //! @param commands The manager for RenderCommands.
        RenderSceneGroup(const RenderSceneManagerPtr& scenes, const CameraManagerPtr& cameras, const RenderTechniqueManagerPtr& techniques, const RenderCommandManagerPtr& commands);

        //! @brief Adds a RenderScene to the group.
        //! The scene is automatically added the cameras in this group. The root
        //! node is also set to the shared root node, \ref mSharedNode.
        //! @return The index of the added scene.
        std::size_t addScene(const RenderScenePtr& scene);

        //! @brief Adds a \ref RenderTechnique for this Group.
        //! @return The index of the added technique.
        std::size_t addTechnique(const RenderTechniquePtr& technique);

        //! @brief Adds a \ref Camera for this Group.
        //! @return The index of the added camera.
        std::size_t addCamera(const CameraPtr& camera);

        //! @brief Adds a \ref RenderCommand to this Group.
        std::size_t addCommand(const RenderCommandPtr& command);

        //! @brief Returns the scene at given index.
        RenderScene& sceneAt(std::size_t idx);

        //! @brief Returns the scene at given index.
        const RenderScene& sceneAt(std::size_t idx) const;

        //! @brief Returns the number of RenderScenes in this Group.
        std::size_t sceneCount() const;

        //! @brief Returns the \ref RenderTechnique at given index.
        RenderTechnique& techniqueAt(std::size_t idx);

        //! @brief Returns the \ref RenderTechnique at given index.
        const RenderTechnique& techniqueAt(std::size_t idx) const;

        //! @brief Returns a pointer to a RenderTechnique in this group.
        RenderTechniquePtr techniquePtrAt(std::size_t index) const;

        //! @brief Returns the number of RenderTechnique in this Group.
        std::size_t techniqueCount() const;

        //! @brief Returns the \ref Camera at given index.
        Camera& cameraAt(std::size_t idx);

        //! @brief Returns the \ref Camera at given index.
        const Camera& cameraAt(std::size_t idx) const;

        //! @brief Returns the number of Camera in this Group.
        std::size_t cameraCount() const;

        //! @brief Returns the \ref RenderCommand at given index.
        RenderCommand& commandAt(std::size_t idx);

        //! @brief Returns the \ref RenderCommand at given index.
        const RenderCommand& commandAt(std::size_t idx) const;

        //! @brief Returns the number of RenderCommands in this Group.
        std::size_t commandCount() const;

        //! @brief Selects the technique for a scene.
        void selectTechnique(std::size_t techniqueIdx, std::size_t sceneIdx);

        //! @brief Selects a RenderCommand for a RenderScene.
        virtual void selectCommand(std::size_t commandIdx, std::size_t sceneIdx);

        //! @brief Selects a Camera for a RenderScene.
        virtual void selectCamera(std::size_t cameraIdx, std::size_t sceneIdx);

        //! @brief Selects a Technique, Command and Camera for a RenderScene.
        virtual void selectAll(std::size_t technique, std::size_t camera, std::size_t command, std::size_t scene);

        //! @brief Renders the RenderSceneGroup.
        //! For each associated Command/Scene, it renders the Scene in the Command
        //! only if the Scene is touched. If the scene is left untouched, then the
        //! RenderCommand is left as-is.
        //! When rendering into the RenderCommand, the RenderCommand is locked and 
        //! cleared to ensure everything is empty.
        virtual std::future < void > render(RenderCommand& command) const;

        //! @brief Does nothing.
        virtual std::future < void > build(Renderer&) { return {}; }

        //! @brief Always return zero.
        virtual std::size_t size(Renderer&) const { return 0; }

        //! @brief Creates a new RenderCommand and adds it.
        //! @return The index of the added \ref RenderCommand.
        virtual std::size_t makeCommand(Renderer& renderer);

        //! @brief Adds a RenderScene and makes a RenderCommand for this scene.
        //! @brief Returns the index of the added scene.
        virtual std::size_t addSceneCommand(const RenderScenePtr& scene, Renderer& renderer);

        //! @brief Sets \ref mSharedNode.
        virtual void setSharedNode(const RenderNodePtr& node);

        //! @brief Creates a new RenderScene and makes a RenderCommand for this scene.
        //! @param name The RenderScene's name.
        //! @param renderer The Renderer for the RenderCommand to create for the Scene.
        //! @param technique The RenderTechnique to use for this RenderScene, or an index
        //! for an already registered Technique.
        //! @return The index of the newly made RenderScene.
        virtual std::size_t makeNewScene(const std::string& name, Renderer& renderer, const TechniqueOrIdx& technique);

        //! @brief Locks all managers.
        virtual void lock() const;

        //! @brief Unlocks all managers.
        virtual void unlock() const;

        //! @brief Returns the Scene Manager.
        virtual RenderSceneManager& sceneManager();
        //! @brief Returns the Scene Manager.
        virtual const RenderSceneManager& sceneManager() const;

        //! @brief Returns the Technique Manager.
        virtual RenderTechniqueManager& techniqueManager();
        //! @brief Returns the Technique Manager.
        virtual const RenderTechniqueManager& techniqueManager() const;

        //! @brief Returns the Camera Manager.
        virtual CameraManager& cameraManager();
        //! @brief Returns the Camera Manager.
        virtual const CameraManager& cameraManager() const;

        //! @brief Returns the Command Manager.
        virtual RenderCommandManager& commandManager();
        //! @brief Returns the Command Manager.
        virtual const RenderCommandManager& commandManager() const;

    public:

        //! @brief Touches the Group.
        virtual void onObjectAdded(RenderSceneManager& manager, RenderScene& scene);
        //! @brief Touches the Group.
        virtual void onObjectRemoved(RenderSceneManager& manager, RenderScene& scene);

        //! @brief Touches the Group.
        virtual void onObjectAdded(CameraManager& manager, Camera& scene);
        //! @brief Touches the Group.
        virtual void onObjectRemoved(CameraManager& manager, Camera& scene);

        //! @brief Touches the Group.
        virtual void onObjectAdded(RenderTechniqueManager& manager, RenderTechnique& scene);
        //! @brief Touches the Group.
        virtual void onObjectRemoved(RenderTechniqueManager& manager, RenderTechnique& scene);

        //! @brief Touches the Group.
        virtual void onObjectAdded(RenderCommandManager& manager, RenderCommand& scene);
        //! @brief Touches the Group.
        virtual void onObjectRemoved(RenderCommandManager& manager, RenderCommand& scene);
    };
}

#endif // ATL_RENDERSCENEGROUP_H