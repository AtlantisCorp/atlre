//
//  RenderScene.cpp
//  atl
//
//  Created by jacques tronconi on 02/04/2020.
//

#include "RenderScene.h"

namespace Atl
{
    // --------------------------------------------------------------------------------
    // RenderSceneManager
    
    void RenderSceneManager::add(const RenderScenePtr& rhs)
    {
        {
            std::lock_guard l(mMutex);
            mResources.push_back(rhs);
        }
    }
    
    // --------------------------------------------------------------------------------
    // RenderScene

    RenderScene::RenderScene(const std::string& name, const RenderNodePtr& root, const CameraPtr& camera, const RenderTechniquePtr& technique)
    : Resource(name), mRoot(root), mTechnique(technique), mCamera(camera)
    {

    }
    
    RenderNodePtr RenderScene::root() const
    {
        return std::atomic_load(&mRoot);
    }
    
    void RenderScene::setRoot(const RenderNodePtr& node)
    {
        std::atomic_store(&mRoot, node);
        send(&Listener::onRenderSceneDidSetRoot, *this);
        touch();
    }
    
    MovableRenderNodePtr RenderScene::newMovableNode(const Node::Shared& node, const rvec3& position, const rvec3& target)
    {
        TransformationPtr tran = Transformation::New(Transformation::LookAt("MovableRenderNode", position, target));
        
        return MovableRenderNode::New(node, tran);
    }
    
    ModelRenderNodePtr RenderScene::newModelNode(const Node::Shared& node, const std::string& modelName, const std::string& modelFile, const Params& params)
    {
        ModelPtr model = ModelManager::Get().loadOrGet(modelName, modelFile, params)
        .get();
        
        return ModelRenderNode::New(node, model);
    }
    
    void RenderScene::setCamera(const CameraPtr& camera)
    {
        if (!camera)
            throw NullError("RenderScene", "setCamera", "Null Camera.");
        
        {
            std::lock_guard l(mMutex);
            mCamera = camera;
        }
        
        touch();
        send(&Listener::onRenderSceneDidSetCamera, *this, (const Camera&)*camera);
    }
    
    std::future < void > RenderScene::build(Renderer& renderer)
    {
        return std::async(std::launch::async, [this, &renderer](){});
    }
    
    std::future < void > RenderScene::render(RenderCommand& command) const
    {
        return std::async(std::launch::async, [this, &command]()
        {
            if (!isTouched())
                return;
            
            std::lock_guard l(mMutex);
            
            if (!mCamera)
                throw NullError("RenderScene", "render", "RenderScene %s has no Camera.", name().data());
            
            RenderTechniquePtr technique = std::atomic_load(&mTechnique);
            
            if (!technique)
            {
                std::vector < bool > shouldRender =
                send(&Listener::onRenderSceneShouldRenderNoTechnique, *this).get();
                
                if (std::find(shouldRender.begin(), shouldRender.end(), true) == shouldRender.end())
                    return;
                
                Frustum frustum(mCamera->matrix());
                mRoot->render(command, frustum);
                clean();
                return;
            }
            
            technique->render(command, *mRoot, *mCamera);
            clean();
        });
    }
    
    RenderTechniquePtr RenderScene::technique() const
    {
        return std::atomic_load(&mTechnique);
    }
    
    void RenderScene::setTechnique(const RenderTechniquePtr& rhs)
    {
        std::atomic_store(&mTechnique, rhs);
        touch();
    }
}
