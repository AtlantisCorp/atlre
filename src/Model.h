//
//  Model.h
//  atl
//
//  Created by jacques tronconi on 18/03/2020.
//

#ifndef Model_h
#define Model_h

#include "Platform.h"
#include "Resource.h"
#include "SubModel.h"
#include "Emitter.h"

namespace Atl
{
    class Model;
    class ModelListener;
    class ModelManager;
    class ModelManagerListener;

    ATL_BASE_RESOURCE(Model)
    ATL_LOCKABLE_RESOURCE(Model)

    template < > struct LoaderTrait < Model > : public BaseLoader
    {
        //! @brief Destroys the Loader.
        virtual ~LoaderTrait() = default;

        //! @brief Loads a Model file into the Model object.
        //! @param model The Model where to load the file.
        //! @param filename The file to load (this may be a relative or an absolute path).
        //! @param params The parameters for the loader (specific to the loader's implementation).
        virtual void load(Model& model, const std::string& filename, const Params& params) = 0;
    };

    ATL_LOADER_RESOURCE(Model)

    //! @brief The ModelListener.
    class ModelListener : virtual public ResourceListener,
                          virtual public RenderableListener
    {
    public:
        //! @brief Destructor.
        virtual ~ModelListener() = default;
        //! @brief Launched when a SubModel has been added.
        virtual void onModelDidAddSubModel(Model&, SubModel&){}
        //! @brief Launched when a SubModel has been removed.
        virtual void onModelDidRemoveSubModel(Model&, SubModel&){}
        //! @brief Launched when a SubModel has removed all its SubModel.
        virtual void onModelDidRemoveAllSubModels(Model&){}
    };
    
    //! @brief The Model Manager Listener.
    struct ModelManagerListener :
    public ManagerListener <
    ModelManager,
    Model,
    ModelManagerListener >{};

    //! @brief Defines a Manager for the \ref Model class.
    struct ModelManager :
    public Manager <
    ModelManager,
    Model,
    ModelManagerListener >
    {
        //! @brief The resource we manage is \ref Model.
        typedef Model Resource;

        //! @brief Default destructor.
        virtual ~ModelManager() = default;

        //! @brief Loads all models in a directory.
        //! @param dir The path to the directory where lies the Models.
        //! @param params The Params to pass to Model::load().
        //! @return All the Models loaded.
        //! This function is synchroneous, but it loads all models asynchroneously
        //! and waits for all of them to be loaded. You can use std::async() to call
        //! this function asynchroneously as an async task.
        virtual ModelList loadModels(const std::string& dir, const Params& params);
    };

    //! @brief Defines a 3d Model.
    class EXPORTED Model :
    virtual public TResource <
    Model,
    ModelManager >,
    virtual public Renderable
    {
        mutable std::mutex mMutex;
        
        //! @brief The list of SubModels.
        SubModelList mSubModels;
        
    public:
        //! @brief Defines the listener's type for this resource.
        typedef ModelListener Listener;
        
        //! @brief Creates an empty Model.
        Model(Manager& manager, const std::string& name);
        
        //! @brief Creates a new SubModel for this Model.
        SubModelPtr makeSubModel();
        
        //! @brief Adds a new SubModel to this Model.
        void addSubModel(const SubModelPtr& subModel);
        
        //! @brief Inserts a SubModel at given position.
        void insertSubModel(unsigned index, const SubModelPtr& subModel);
        
        //! @brief Removes a SubModel.
        void removeSubModel(const SubModelPtr& subModel);
        
        //! @brief Removes all SubModels.
        void removeAllSubModels();
        
        //! @brief Returns all SubModels.
        SubModelList subModels() const;
        
        //! @brief Changes the list of SubModels.
        void setSubModels(const SubModelList& subModels);
        
        //! @brief Returns the number of SubModels.
        std::size_t subModelsCount() const;
        
        //! @brief Returns a reference to the SubModel at given index.
        SubModel& subModelAt(unsigned index);
        
        //! @brief Returns a reference to the SubModel at given index.
        const SubModel& subModelAt(unsigned index) const;
        
        //! @brief Locks the Model for reading/writing on SubModels.
        void lock() const;
        
        //! @brief Unlocks the Model.
        void unlock() const;
        
        //! @brief Renders each SubModels into the RenderCommand.
        std::future < void > render(RenderCommand& to) const;
        
        //! @brief Calls \ref SubModel::build() on each SubModels.
        std::future < void > build(Renderer& rhs);
        
        //! @brief Returns, in bytes, the memory used on the GPU RAM for this cache and for
        //! the given renderer.
        std::size_t size(Renderer&) const;
        
        //! @brief Returns the size used for this resource.
        inline std::size_t usedSize() const { return 0; }
    };
}

#endif /* Model_h */
