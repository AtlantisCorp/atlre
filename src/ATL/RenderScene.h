//
//  RenderScene.h
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#ifndef ATL_RENDERSCENE_H
#define ATL_RENDERSCENE_H

#include "Renderable.h"
#include "Touchable.h"
#include "Resource.h"
#include "Manager.h"
#include "NodeTraversalTechnique.h"
#include "MovableRenderNode.h"
#include "ModelRenderNode.h"

namespace Atl
{
    class RenderScene;
    class RenderSceneListener;
    ATL_BASE_RESOURCE(RenderScene)
    
    class RenderSceneManager;
    class RenderSceneManagerListener;
    
    //! @brief Listener for RenderSceneManager.
    struct RenderSceneManagerListener :
    public ManagerListener <
    RenderSceneManager,
    RenderScene,
    RenderSceneManagerListener >{};
    
    //! @brief Manager for RenderScene.
    struct RenderSceneManager :
    public Manager <
    RenderSceneManager,
    RenderScene,
    RenderSceneManagerListener >
    {
        //! @brief Destructor.
        virtual ~RenderSceneManager() = default;
        
        //! @brief Adds a new Scene to this Manager.
        virtual void add(const RenderScenePtr& rhs);
    };
    
    //! @brief Listener for RenderScene.
    struct RenderSceneListener :
    virtual public ResourceListener,
    virtual public RenderableListener
    {
        //! @brief Destructor.
        virtual ~RenderSceneListener() = default;
        
        //! @brief Launched when \ref RenderScene did change its camera.
        virtual void onRenderSceneDidSetCamera(RenderScene&, const Camera&){}
        
        //! @brief Launched when \ref RenderScene did change its root node.
        virtual void onRenderSceneDidSetRoot(RenderScene&) {}
        
        //! @brief Launched when \ref RenderScene has no \ref RenderTechnique but it needs
        //! to render nodes with \ref RenderNode::render(). Returns true by default, but
        //! return false if you don't want to render the scene without RenderTechnique.
        virtual bool onRenderSceneShouldRenderNoTechnique(const RenderScene&) { return true; }
    };
    
    //! @brief Manages a Camera, a RenderTechnique and a RenderNode Tree.
    //! The RenderScene renders a RenderNode Tree thanks to its RenderTechnique, through
    //! the selected Camera.
    class RenderScene :
    public Resource,
    public Renderable,
    public TimeTouchable
    {
        //! @brief The mutex.
        mutable std::recursive_mutex mMutex;
        
        //! @brief The root node in the RenderNode Tree we want to render in this
        //! scene. The Tree can be shared among multiple scenes.
        RenderNodePtr mRoot;
        
        //! @brief The technique used to render this scene. If null, each node is
        //! rendered using \ref RenderNode::render(command, frustum). You should
        //! at least use the \ref NodeTraversalTechnique to render the scene.
        RenderTechniquePtr mTechnique;
        
        //! @brief The Camera to render this scene. It cannot be null, as a node's
        //! Tree should always cull its nodes.
        CameraPtr mCamera;
        
    public:
        ATL_SHAREABLE(RenderScene)
        
        //! @brief The scene's listener.
        typedef RenderSceneListener Listener;
        
        //! @brief Constructs a new RenderScene.
        RenderScene(const std::string& name, const RenderNodePtr& root, const CameraPtr& camera, const RenderTechniquePtr& technique = NodeTraversalTechnique::New());
        
        //! @brief Destructor.
        virtual ~RenderScene() = default;

        //! @brief Loads a scene from some parameters.
        //! This function does nothing, but a derived RenderScene class can implement
        //! it to load a scene from a file.
        virtual std::future < void > load(const Params&) { return {}; }
        
        //! @brief Unloads the scene.
        virtual std::future < void > unload(const Params&) { return {}; }
        
        //! @brief Returns the RenderNode's root.
        virtual RenderNodePtr root() const;
        
        //! @brief Sets \ref mRoot.
        virtual void setRoot(const RenderNodePtr& node);
        
        //! @brief Creates a new MovableRenderNode.
        //! The returned RenderNode has a direct access to the Transformation, without using
        //! a dynamic_cast. This is the only advantage of this Node.
        virtual MovableRenderNodePtr newMovableNode(const Node::Shared& node, const rvec3& position, const rvec3& target);
        
        virtual ModelRenderNodePtr newModelNode(const Node::Shared& node, const std::string& modelName, const std::string& modelFile, const Params& params);
        
        //! @brief Sets \ref mCamera.
        virtual void setCamera(const CameraPtr& camera);
        
        //! @brief Does nothing.
        virtual std::future < void > build(Renderer& renderer);
        
        //! @brief Renders the scene using the selected Technique.
        virtual std::future < void > render(RenderCommand& command) const;
        
        //! @brief Returns \ref mTechnique.
        virtual RenderTechniquePtr technique() const;
        
        //! @brief Sets \ref mTechnique.
        virtual void setTechnique(const RenderTechniquePtr& technique);
        
        //! @brief Returns always zero.
        virtual inline std::size_t size(Renderer&) const { return 0; }

        //! @brief Returns always zero: a RenderScene has no real size, only the 
        //! RenderNodes in it has the size.
        virtual inline std::size_t usedSize() const { return 0; }
    };
}

#endif /* ATL_RENDERSCENE_H */
