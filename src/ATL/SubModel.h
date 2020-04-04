//
//  SubModel.h
//  atl
//
//  Created by jacques tronconi on 19/03/2020.
//

#ifndef SubModel_h
#define SubModel_h

#include "Platform.h"
#include "CachedRenderable.h"
#include "VertexInfos.h"
#include "RenderCommand.h"
#include "IndexBufferData.h"
#include "Material.h"

namespace Atl
{
    class Model;
    class SubModel;

    ATL_BASE_RESOURCE(SubModel)
    ATL_LOCKABLE_RESOURCE(SubModel)
    
    //! @brief Defines the basic element of a Model.
    //!
    //! A Model is always constitued of one or more SubModels. Each SubModel holds
    //! a VertexDeclaration and a VertexBufferBinding structures. Those structures
    //! defines the data set for the SubModel. This two structures may be shared
    //! with other submodels as VertexInfos has start and count members.
    //!
    //! A SubModel also holds a list of indexes used to draw itself. This buffer is
    //! optional but it can greatly improve performances when drawing the SubModel.
    //!
    //! @section A SubModel's rendering
    //! The SubModel derives from the Renderable interface and thus, is able to
    //! populate a RenderCommand with its \ref render() function. If the SubModel
    //! is not built yet, it calls \ref build() with the renderer provided with
    //! \ref RenderCommand::renderer().
    //!
    //! @section B Buffer's storages
    //! The SubModel stores its data either in MemBuffers or in Renderer's created
    //! HardwareBuffers. Originally, SubModel uses MemBuffers and on build, creates
    //! the HardwareBuffers to store the data on the GPU side. This allows us to use
    //! double buffering for SubModel's update (you can update freely the data in the
    //! SubModel, and the CPU/GPU memory synchronization happens only when updating
    //! the data into the buffers).
    class EXPORTED SubModel : public CachedRenderable < SubModel >
    {
    protected:
        mutable std::mutex mMutex;
        
        //! @brief The model associated to this sub model.
        Model& mModel;
        
        //! @brief The vertex data associated to this sub model. The vertex data may be shared
        //! between every other submodel of the same model (or even between models).
        VertexInfosPtr mVertexInfos;
        
        //! @brief A Pointer to the Material for this SubModel.
        MaterialPtr mMaterial;
        
        //! @brief Holds the data for indexes.
        IndexBufferDataPtr mIndexes;
        
    public:
        ATL_SHAREABLE(SubModel)
        
        //! @brief Constructs an empty SubModel.
        //! @param model The Model for which this SubModel is for.
        //! The SubModel creates a new VertexInfos data set. This data set is empty but may be
        //! accessed with \ref vertexInfos(). This constructor may be used to create new SubModels
        //! with \ref Model::makeSubModel().
        SubModel(Model& model);
        
        //! @brief Constructs a SubModel.
        //! @param model The Model for which this SubModel is for.
        //! @param vBuffer The vertex data set to use for this SubModel.
        //! @param material The material to use for this SubModel. If null, the default material
        //! loaded from \ref MaterialManager::defaultMaterial() is used.
        SubModel(Model& model, const VertexInfosPtr& vBuffer, const MaterialPtr& material = nullptr);
        
        //! @brief Constructs a copy of a SubModel.
        //! The data set is copied however, the buffers are not copied in memory. Instead, as we
        //! use std::shared_ptr everywhere, another instance is constructed.
        //!
        //! @note
        //! The cached data is not copied. Instead, \ref build() or \red render() must be used to
        //! regenerate any cached data.
        //!
        //! @param model The Model for which this new SubModel is for. It may be the same model as
        //! the copied SubModel, or another Model.
        //! @param rhs The copied SubModel.
        SubModel(Model& model, const SubModel& rhs);
        
        //! @brief Swaps this SubModel with given submodel.
        void swap(SubModel& rhs);
        
        //! @brief Changes the Material for this SubModel.
        void setMaterial(const MaterialPtr& material);
        
        //! @brief Returns the Material used with this SubModel.
        MaterialPtr material() const;
        
        //! @brief Locks the SubModel in order to work on data.
        void lock() const;
        
        //! @brief Unlocks the SubModel in order to work on data.
        void unlock() const;
        
        //! @brief Accesses directly to the data stored in this SubModel for the given
        //! vertex element.
        //! @param meaning The Meaning of the VertexElement to access.
        //! @param outPtr [out] A reference to an address where the data will be available. If
        //! no data is available, this pointer is set to nullptr.
        //! @param offset [out] Returns the offset in the buffer where the data begins. This offset
        //! is already added to the pointer returned.
        //! @param stride [out] Returns the number of bytes between two vertex data. You can access
        //! the next element by adding this stride to your current pointer.
        //! @param count [out] Returns the number of elements in the data set.
        //! @param buffer [out] Returns the buffer involved in the data set.
        //!
        //! @note
        //! You can use \ref SubModel::VertexElementIterator to iterate over those elements directly,
        //! or instantiate it with \ref iterate(). However, you must lock/unlock the SubModel, either
        //! by manual locking or by using \ref SubModelLockGuard.
        //!
        //! @warning
        //! This function access directly the data in the buffer. If your buffer is a memory buffer,
        //! then you can do whatever you want. If your buffer is stored in the GPU, then you must
        //! be aware of the GPU/CPU memory synchronization, which may do some BUS overdrive and affect
        //! your application's performances.
        void bufferElement(const std::string& meaning,
                           void*& outPtr,
                           std::size_t& offset,
                           std::size_t& stride,
                           std::size_t& count,
                           HardwareBufferPtr& buffer);
        
        //! @brief Returns a reference to the vertex data set.
        //! Please use a \ref SubModelLockGuard before modifying anything in this reference. Other
        //! objects may use this data set (for example the renderer) and thus access concurrently
        //! the data set. Please also call \ref touch() to ensure the SubModel is updated.
        VertexInfos& vertexInfos();
        
        //! @brief Returns a reference to the vertex data set.
        //! Please use a \ref SubModelLockGuard before reading anything in this reference. Other
        //! objects may use this data set (for example the renderer) and thus access concurrently
        //! the data set. Please also call \ref touch() to ensure the SubModel is updated.
        const VertexInfos& vertexInfos() const;

        //! @brief Returns true if this SubModel has indexes data.
        virtual bool hasIndexes() const;

        //! @brief Returns a reference to the IndexBufferData in this SubModel.
        virtual IndexBufferData& indexes();

        //! @brief Returns a reference to the IndexBufferData in this SubModel.
        virtual const IndexBufferData& indexes() const;

        //! @brief Creates a new SubModelRenderCache. 
        virtual RenderCachePtr < SubModel > makeNewCache(Renderer& rhs);
        
    public:
        
        /* EXAMPLE OF VERTEXELEMENTITERATOR USE
         void a()
         {
            SubModel aSubModel;
            std::lock_guard l(aSubModel);
         
            auto iterator = aSubModel.iterate < float >("aPosition");
         
            do
            {
                float* data = anIterator.data();
                data[1] = 1.0f;
         
            } while (anIterator.next());
         
            anIterator.undata();
         }
         */

        //! @brief Iterates through Vertex Elements in a SubModel.
        template < typename T >
        class VertexElementIterator
        {
            std::string mName;
            SubModel& mSubModel;
            char *mStart, *mEnd, *mCurr;
            size_t mStride;
            HardwareBufferPtr mBuffer;
            
        public:
            
            VertexElementIterator(const std::string& name, SubModel& subModel)
            : mName(name), mSubModel(subModel)
            {
                char* data = nullptr;
                size_t offset, stride, count;
                HardwareBufferPtr buffer;
                mSubModel.bufferElement(name, data, offset, stride, count, buffer);
                
                if (!buffer || !data || !count)
                {
                    mStart = mEnd = mCurr = nullptr;
                    mStride = 0;
                    return;
                }
                
                mStart = data;
                mEnd = data + stride * count;
                mCurr = mStart;
                mStride = stride;
                mBuffer = buffer;
            }
            
            VertexElementIterator(const VertexElementIterator& rhs)
            : mName(rhs.mName), mSubModel(rhs.mSubModel), mStart(rhs.mStart), mEnd(rhs.mEnd),
            mCurr(rhs.mCurr), mStride(rhs.mStride), mBuffer(rhs.mBuffer)
            {
                
            }
            
            VertexElementIterator& next()
            {
                if (mCurr != mEnd)
                {
                    mCurr = mCurr + mStride;
                    return *this;
                }
                
                return *this;
            }
            
            VertexElementIterator& operator ++ ()
            {
                return next();
            }
            
            bool isValid() const
            {
                return mCurr != mEnd;
            }
            
            operator bool () const
            {
                return isValid();
            }
            
            void undata()
            {
                if (mBuffer)
                {
                    mBuffer->undata();
                }
            }
            
            T* data()
            {
                return reinterpret_cast < T* >(mCurr);
            }
        };
        
        template < typename T >
        auto iterate(const std::string& element) { return VertexElementIterator < T >(element, *this); }
    };
    
    template < typename T >
    using SubModelVertexElementIterator = SubModel::VertexElementIterator < T >;
}

#endif /* SubModel_h */
