//
//  RenderPipeline.h
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_RENDERPIPELINE_H
#define ATL_RENDERPIPELINE_H

#include "Shader.h"
#include "ShaderVariable.h"

namespace Atl
{
    class RenderPipeline;
    class RenderPipelineListener;
    class RenderPipelineManager;
    class ShaderInfos;

    ATL_BASE_RESOURCE(RenderPipeline)
    ATL_BASE_RESOURCE(RenderPipelineListener)

    //! @brief Defines a little structure to give to \ref RenderPipeline::loadShaders()
    //! to load multiple shaders at once.
    struct ShaderInfos
    {
        //! @brief Shader's name. It will be looked before the filename.
        std::string name;

        //! @brief Shader's filename. It will be the shader file if the shader's name
        //! has not been found.
        std::string filename;

        //! @brief Shader's stage.
        ShaderType type;

        //! @brief The parameters to pass to the renderer. It may even be the shader main function 
        //! name, for some language like HLSL.
        Params params;

        //! @brief Constructs a new ShaderInfos.
        ShaderInfos(const std::string& name, const std::string& filename, ShaderType type, const Params& params = Params{});
    };

    //! @brief Listener for RenderPipeline.
    struct RenderPipelineListener : public ResourceListener
    {
        //! @brief Destructor.
        virtual ~RenderPipelineListener() = default;

        //! @brief Launched when the render pipeline has been built.
        virtual void onPipelineBuilt(RenderPipeline&){}

        //! @brief Launched when a Shader has been loaded for this pipeline.
        virtual void onPipelineDidLoadShader(RenderPipeline&, Shader&){}
    };

    //! @brief Manager for RenderPipeline.
    struct RenderPipelineManager : public ShareableManager < RenderPipeline >
    {
        //! @brief Returns the RenderPipeline with name, or nullptr.
        RenderPipelinePtr findName(const std::string& name) const;
    };

    //! @brief Define an interface to manipulate a set of shader's stages. 
    //! Multiple shader stages, once associated, give the rendering pipeline we have to 
    //! use when rendering a RenderSceneNode Tree with a given RenderTechnique.
    //! Each shader's stage can be set only *before* building the pipeline. If you want to
    //! set another shader for a stage, you must unload it and reload it. You can also
    //! prebuild some render pipelines.
    //!
    //! \note
    //! If the Renderer supports the feature RenderFeature::MutablePipelines, you can 
    //! instead use MutableRenderPipeline which doesn't require to unload the pipeline
    //! before setting other shaders.
    class RenderPipeline : public RenderObject,
                           public Resource,
                           public Lockable
    {
        public:

        //! @brief Defines a structure that holds some informations for an alias. However
        //! there isn't the alias in this struture. \see mAliases
        struct AliasValue 
        { 
            //! @brief The real name of the attribute/uniform for an Alias.
            std::string mRealName; 

            //! @brief The shader where this attribute/uniform belongs. For now, a vertex
            //! attribute can only belong to a Vertex shader.
            ShaderType mShader;

            //! @brief The location of the uniform. This value is present for caching 
            //! purpose: this way for an alias we don't have to find the uniform's informations.
            int mLocation;
        };

        //! @brief Defines an association between an Alias and an AliasValue structure.
        typedef std::map < std::string, AliasValue > AliasesMap;

        //! @brief Defines an infos structure to send when constructing multiple aliases values.
        struct AliasInfos : public AliasValue
        {
            //! @brief The Alias name.
            std::string mAliasName;

            //! @brief Constructs a new AliasInfos.
            AliasInfos(const std::string& alias, const std::string& name, ShaderType shader, int location = -1);
        };

        //! @brief Defines a very basic uniform structur.
        struct Uniform 
        {
            //! @brief The name of this uniform.
            std::string mName;

            //! @brief The location of this uniform.
            int mLocation;

            //! @brief The shader associated to this uniform.
            ShaderType mShader;

            //! @brief The size of this uniform, in bytes.
            std::size_t mSize;

            //! @brief The number of elements if this uniform is an array.
            std::size_t mElements;

            //! @brief The type of each element, if this uniform is an array.
            ShaderVariableType mType;
        };

        private:

        //! @brief Holds the shader's map.
        std::map < ShaderType, ShaderPtr > mShaders;

        //! @brief Holds the aliases for a ShaderVariable in a ShaderType.
        AliasesMap mAliases;

        //! @brief The mutex.
        mutable std::mutex mMutex;

        public:
        ATL_SHAREABLE(RenderPipeline)

        //! @brief The pipeline's listener.
        typedef RenderPipelineListener Listener;

        //! @brief Constructs a new RenderPipeline.
        RenderPipeline(Renderer& renderer, const std::string& name);

        //! @brief Destructor.
        virtual ~RenderPipeline() = default;

        //! @brief Loads multiple Shaders for this pipeline.
        //! \param shaders The shaders we will load for this pipeline. The map associated
        //! a ShaderType with the filename of the Shader we want to load.
        //! If one of the shaders cannot be loaded, a NullError is thrown.
        virtual std::future < void > loadShaders(const std::vector < ShaderInfos >& shaders);

        //! @brief Unloads the pipeline.
        //! Calls _unload() implementation after locking this pipeline. The loading state must 
        //! be 'Loaded' for this function to be effective. Next to that call, all members are 
        //! cleared.
        virtual std::future < void > unload();

        //! @brief Build the pipeline.
        //! Calls _build() implementation after locking this pipeline.
        virtual std::future < void > build();

        //! @brief Sets multiple Aliases at the same time.
        //! The real name must be present in the pipeline. The pipeline must be build() before
        //! a call to this function, because it involves \ref findUniformName(), \ref findUniformIndex()
        //! and other look up functions.
        virtual void setAliases(const AliasesMap& aliases);

        //! @brief Sets multiple Aliases at the same time.
        //! This is a much easier function because you can directly pass a vector of AliasInfos.
        virtual void setAliasesInfos(const std::vector < AliasInfos >& infos);

        //! @brief Sets one Alias.
        //! The real name must be present in the pipeline. The pipeline must be build() before
        //! a call to this function, because it involves \ref findUniformName(), \ref findUniformIndex()
        //! and other look up functions.
        //! \param alias The alias name.
        //! \param realName The uniform real name.
        //! \param shader The shader stage involved for the uniform binding.
        //! \param location An index indicating the location of the uniform. Default value is -1, meaning
        //! the location is loaded from the shader itself.
        virtual void setAlias(const std::string& name, const std::string& realName, ShaderType type, int location = -1);

        //! @brief Finds the location of a uniform name.
        //! \param shader The shader type (or stage) where to look for the uniform.
        //! \param name The uniform's name in the shader. This function should return a valid location
        //! (thus a value >= 0) only if this name is found, or it returns -1 on error. The uniform may be 
        //! a variable, or a block (uniform buffer, shader storage block, etc.).
        //! \note The RenderPipeline should be locked with LockableGuard before using this function because
        //! it doesn't lock this instance's mutex. However, derived implementations may do things differently.
        virtual int findUniformLocation(const ShaderType& shader, const std::string& name) const = 0;

        //! @brief Finds the name of a bound location.
        //! \param shader The shader type (or stage) where to look for the location.
        //! \param location An index where the uniform lies in the shader. This uniform may be a variable
        //! or a block (uniform buffer, shader storage block, etc.). A location of -1 is considered as an
        //! error and an empty name is returned.
        //! \note The RenderPipeline should be locked with LockableGuard before using this function because
        //! it doesn't lock this instance's mutex. However, derived implementations may do things differently.
        virtual std::string findUniformName(const ShaderType& shader, int location) const = 0;

        //! @brief Finds informations about a uniform from its name.
        //! The uniform is looked for in each Shader stages. The first uniform found with given name
        //! is returned from this function.
        //! \note The RenderPipeline should be locked with LockableGuard before using this function because
        //! it doesn't lock this instance's mutex. However, derived implementations may do things differently.
        virtual Uniform findUniform(const std::string& name) const = 0;

        //! @brief Finds the location of an alias or a uniform.
        //! \param alias The alias name to look for. If the alias index has not been cached, a call to
        //! function \ref findUniformLocation() will occur. If alias is an actual uniform name, it will
        //! successfully return the uniform index.
        //! \return The location of that alias, or -1 if not found.
        virtual int findAliasLocation(const std::string& alias) const;

        //! @brief Finds the name of an alias or a uniform.
        virtual std::string findAliasName(const std::string& alias) const;

        //! @brief Locks \ref mMutex.
        virtual inline void lock() const { mMutex.lock(); }

        //! @brief Unlocks \ref mMutex.
        virtual inline void unlock() const { mMutex.unlock(); }

        //! @brief Binds the pipeline for proper use.
        virtual void bind() = 0;

        protected:

        //! @brief Unloads the RenderPipeline.
        virtual void _unload() = 0;

        //! @brief Builds the pipeline.
        virtual void _build() = 0;

        //! @brief Sets an alias without locking (used by \ref setAliases() and \ref setAlias).
        virtual void _setAlias(const std::string& alias, const AliasValue& value);
    };
}

#endif // ATL_RENDERPIPELINE_H