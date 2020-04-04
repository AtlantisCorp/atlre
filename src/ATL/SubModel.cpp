//
//  SubModel.cpp
//  atl
//
//  Created by jacques tronconi on 21/03/2020.
//

#include "SubModel.h"
#include "SubModelRenderCache.h"

namespace Atl
{
    SubModel::SubModel(Model& model)
    : mModel(model)
    {
        mVertexInfos = VertexInfos::New();
        mIndexes = IndexBufferData::New();
    }
    
    SubModel::SubModel(Model& model, const VertexInfosPtr& vBuffers, const MaterialPtr& material)
    : mModel(model), mVertexInfos(vBuffers), mMaterial(material), mIndexes(nullptr)
    {
        mIndexes = IndexBufferData::New();
        if (!vBuffers) throw NullError("SubModel", "SubModel", "Null VertexInfos Pointer.");
    }
    
    SubModel::SubModel(Model& model, const SubModel& rhs)
    : mModel(model)
    {
        SubModelLockGuard l(const_cast < SubModel& >(rhs));
        mVertexInfos = rhs.mVertexInfos;
        mMaterial = rhs.mMaterial;
        mIndexes = rhs.mIndexes;
    }
    
    void SubModel::swap(SubModel &rhs)
    {
        SubModelLockGuard l(rhs);
        std::swap(mVertexInfos, rhs.mVertexInfos);
        std::swap(mMaterial, rhs.mMaterial);
        std::swap(mIndexes, rhs.mIndexes);
    }
    
    void SubModel::setMaterial(const MaterialPtr& material)
    {
        std::atomic_store(&mMaterial, material);
    }
    
    MaterialPtr SubModel::material() const
    {
        return std::atomic_load(&mMaterial);
    }
    
    void SubModel::lock() const
    {
        mMutex.lock();
    }
    
    void SubModel::unlock() const
    {
        mMutex.unlock();
    }
    
    void SubModel::bufferElement(const std::string& name,
                                 void*& outPtr,
                                 std::size_t& offset,
                                 std::size_t& stride,
                                 std::size_t& count,
                                 HardwareBufferPtr& buffer)
    {
        const VertexElement& element = mVertexInfos->declaration()->findElement(name);
        buffer = mVertexInfos->binding()->bufferAt(element.source());
        
        if (!buffer)
        {
            outPtr = nullptr;
            offset = 0;
            stride = 0;
            return;
        }
        
        buffer->lock();
        
        const unsigned short source = element.source();
        stride = mVertexInfos->declaration()->vertexSizeForSource(source);
        const std::size_t base = mVertexInfos->baseVertex() * stride;
        offset = base + element.offset();
        outPtr = ((char*)buffer->data()) + offset;
        count = mVertexInfos->vertexesCount();
    }
    
    VertexInfos& SubModel::vertexInfos()
    {
        if (!mVertexInfos) throw NullError("SubModel", "vertexInfos", "Null VertexInfos.");
        return *std::atomic_load(&mVertexInfos);
    }
    
    const VertexInfos& SubModel::vertexInfos() const
    {
        if (!mVertexInfos) throw NullError("SubModel", "vertexInfos", "Null VertexInfos.");
        return *std::atomic_load(&mVertexInfos);
    }

    bool SubModel::hasIndexes() const
    {
        if (!mIndexes)
            return false;

        return mIndexes->elementsCount() > 0;
    }

    IndexBufferData& SubModel::indexes() 
    {
        return *mIndexes;
    }

    const IndexBufferData& SubModel::indexes() const
    {
        return *mIndexes;
    }

    RenderCachePtr < SubModel > SubModel::makeNewCache(Renderer& rhs)
    {
        return SubModelRenderCache::New(rhs, *this);
    }
}
