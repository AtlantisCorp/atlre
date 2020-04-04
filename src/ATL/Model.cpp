//
//  Model.cpp
//  atl
//
//  Created by jacques tronconi on 22/03/2020.
//

#include "Model.h"

namespace Atl
{
    // ------------------------------------------------------------------------------------
    // ModelManager

    ModelList ModelManager::loadModels(const std::string& path, const Params& params)
    {
        std::vector < std::future < ModelPtr > > futures;
        ModelList results;

        for (auto& entree : fs::directory_iterator(path))
        {
            if (!entree.is_regular_file())
                continue;

            const std::string ext = File::Extension(entree.path().relative_path());
            
            if (!ModelLoaderDb::Get().isLoadable(ext))
                continue;

            const std::string fullName = entree.path().relative_path();
            auto future = loadOrGet(fullName, fullName, Params{});
            futures.push_back(std::move(future));
        }

        results.reserve(futures.size());
        for (auto& future : futures)
            results.push_back(future.get());

        return results;
    }

    // ------------------------------------------------------------------------------------
    // Model

    Model::Model(Manager& manager, const std::string& name)
    : TResource(manager, name)
    {
        
    }
    
    SubModelPtr Model::makeSubModel()
    {
        auto subModel = SubModel::New(*this);
        addSubModel(subModel);
        return subModel;
    }
    
    void Model::addSubModel(const SubModelPtr& subModel)
    {
        if (!subModel)
            throw NullError("Model", "addSubModel", "Null SubModel for Model %s.", name().data());

        {
            std::lock_guard l(mMutex);
            mSubModels.push_back(subModel);
        }

        send(&Listener::onModelDidAddSubModel, *this, (SubModel&)*subModel);
    }
    
    void Model::insertSubModel(unsigned index, const SubModelPtr& subModel)
    {
        if (!subModel)
            throw NullError("Model", "insertSubModel", "Null SubModel for Model %s.", name().data());

        {
            std::lock_guard l(mMutex);
        
            if (index >= mSubModels.size())
                throw OutOfRange("Model", "insertSubModel", "Model %s has no subModel index %i.",
                                name().data(), index);
            
            auto it = mSubModels.begin();
            std::advance(it, index);
            
            mSubModels.insert(it, subModel);
        }

        send(&Listener::onModelDidAddSubModel, *this, (SubModel&)*subModel);
    }
    
    void Model::removeSubModel(const SubModelPtr& subModel)
    {
        if (!subModel)
            throw NullError("Model", "removeSubModel", "Null SubModel for Model %s.", name().data());

        {
            std::lock_guard l(mMutex);
            
            auto it = std::find(mSubModels.begin(), mSubModels.end(), subModel);
            if (it != mSubModels.end()) mSubModels.erase(it);
        }   

        send(&Listener::onModelDidRemoveSubModel, *this, (SubModel&)*subModel);
    }
    
    void Model::removeAllSubModels()
    {
        {
            std::lock_guard l(mMutex);
            mSubModels.clear();
        }

        send(&Listener::onModelDidRemoveAllSubModels, *this);
    }
    
    SubModelList Model::subModels() const
    {
        std::lock_guard l(mMutex);
        return mSubModels;
    }
    
    void Model::setSubModels(const SubModelList &subModels)
    {
        std::lock_guard l(mMutex);
        mSubModels = subModels;
    }
    
    std::size_t Model::subModelsCount() const
    {
        std::lock_guard l(mMutex);
        return mSubModels.size();
    }
    
    SubModel& Model::subModelAt(unsigned index)
    {
        std::lock_guard l(mMutex);
        
        if (index >= mSubModels.size())
            throw OutOfRange("Model", "subModelAt", "No SubModel for index %i in Model %s.",
                             index, name().data());
        
        return *mSubModels.at(index);
    }
    
    const SubModel& Model::subModelAt(unsigned index) const
    {
        std::lock_guard l(mMutex);
        
        if (index >= mSubModels.size())
            throw OutOfRange("Model", "subModelAt", "No SubModel for index %i in Model %s.",
                             index, name().data());
        
        return *mSubModels.at(index);
    }
    
    void Model::lock() const
    {
        mMutex.lock();
    }
    
    void Model::unlock() const
    {
        mMutex.unlock();
    }
    
    std::future < void > Model::render(RenderCommand& to) const
    {   
        return std::async(std::launch::async, [this, &to]()
        {
            send(&Listener::onRenderableWillRender, (const Renderable&)*this, to).get();

            SubModelList subModels = this->subModels();
            
            for (const SubModelPtr& subModel : subModels)
            {
                if (!subModel) continue;
                subModel->render(to).get();
            }

            send(&Listener::onRenderableDidRender, (const Renderable&)*this, to);
        });
    }
    
    std::future < void > Model::build(Renderer &rhs)
    {
        return std::async(std::launch::async, [this, &rhs]()
        {
            send(&Listener::onRenderableWillBuild, (Renderable&)*this, rhs).get();

            SubModelList subModels = this->subModels();
            
            for (const SubModelPtr& subModel : subModels)
            {
                if (!subModel) continue;
                subModel->build(rhs).get();
            }

            send(&Listener::onRenderableDidBuild, (Renderable&)*this, rhs);
        });
    }
    
    std::size_t Model::size(Renderer& rhs) const
    {
        std::lock_guard l(mMutex);
        std::size_t totalSize = 0;
        
        for (const SubModelPtr& subModel : mSubModels)
            totalSize += subModel->size(rhs);
        
        return totalSize;
    }
}
