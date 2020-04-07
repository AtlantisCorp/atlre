//
//  Shader.h
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Resource.h"
#include "RenderObject.h"
#include "Version.h"
#include "ShareableManager.h"

namespace Atl
{
    class Shader;
    class ShaderListener;
    class ShaderManager;

    ATL_BASE_RESOURCE(Shader)
    ATL_BASE_RESOURCE(ShaderListener)

    //! @brief Enumerates the stages for a shader.
    enum class ShaderType
    {
        Vertex, 
        Geometry,
        TessControl,
        TessEval,
        Fragment
    };

    //! @brief Launched when a shader compilation error is detected.
    struct ShaderCompileError : public Error 
    { using Error::Error; };

    //! @brief A Listener for Shader.
    struct ShaderListener : public ResourceListener
    {
        //! @brief Destructor.
        virtual ~ShaderListener() = default;
        //! @brief Launched when a Shader has been compiled.
        virtual void onShaderCompiled(Shader&){}
    };

    //! @brief Manages multiple Shaders.
    struct ShaderManager : public ShareableManager < Shader >
    {
        //! @brief Finds a shader from its name.
        ShaderPtr findName(const std::string& name) const;

        //! @brief Finds a shader from its filename.
        ShaderPtr findFilename(const std::string& filename) const;
    };

    //! @brief Interface for a Shader Object.
    class Shader : public RenderObject,
                   public Resource,
                   public Lockable
    {
        //! @brief The last error thrown while compiling.
        ShaderCompileError mLastError;

        //! @brief Boolean true if an error has been thrown.
        std::atomic < bool > mHasError;

        //! @brief The filename used to load this shader.
        std::string mFilename;

        //! @brief The ShaderType.
        std::atomic < ShaderType > mType;

        //! @brief The mutex.
        mutable std::mutex mMutex;

        public:

        //! @brief The Shader's listener class.
        typedef ShaderListener Listener;

        //! @brief Constructs a new Shader.
        Shader(Renderer& renderer, const std::string& name);

        //! @brief Destructor.
        virtual ~Shader() = default;

        //! @brief Loads a shader program from a file.
        //! This function calls _load() protected function which is the actual implementation
        //! of the shader loading. As the shader is created by the renderer, there is no need
        //! for a Loader/LoaderDb system. The derived shader class only implements the _load()
        //! function and loads the shader.
        //!
        //! \param type The Shader's type for the given filename.
        //! \param filename The filename we want to load for this shader.
        //! \param params The parameters passed for derived classes. For example, some languages
        //! does require a main function name (HLSL) which may be in 'hlsl.mainName' or other
        //! implementation defined parameter.
        //!
        //! This function sets the State flag to 'Loaded' after the execution of _load(). This 
        //! means the _load() must throw an exception for this flag not to be set (on error for
        //! example). This function also catch for ShaderCompileError exceptions to store it but
        //! rethrow the exception.
        //!
        //! \return An asynchroneous future that holds the shared loading state.
        virtual std::future < void > load(ShaderType type, const std::string& filename, const Params& params);

        //! @brief Unloads a shader program.
        //! This function calls _unload() protected function, which is the actual implementation
        //! of the shader unloading. The shader object should be totally unloaded.
        //! \return An asynchroneous future that holds the shared unloading state.
        virtual std::future < void > unload();

        //! @brief Returns the name of the Shader Language used.
        virtual const char* languageName() const = 0;

        //! @brief Returns the version of the Shader Language used.
        virtual Version languageVersion() const = 0;

        //! @brief Returns a native handle to the Shader.
        virtual GenHandle handle() const = 0;

        //! @brief Locks the Shader.
        virtual inline void lock() const { mMutex.lock(); }

        //! @brief Unlocks the Shader.
        virtual inline void unlock() const { mMutex.unlock(); }

        //! @brief Returns the filename used to load this Shader.
        virtual inline const std::string& filename() const { return mFilename; }

        //! @brief Returns \ref mType.
        virtual inline ShaderType type() const { return mType.load(); }

        protected:

        //! @brief Loads the actual shader file.
        //! This implementation defined functions should load the filename containing a shader
        //! file and throw an exception on error (eventually a ShaderCompileError). The Shader 
        //! is locked while calling this function.
        //!
        //! \param filename The filename we want to load for this shader.
        //! \param params The parameters passed for derived classes. For example, some languages
        //! does require a main function name (HLSL) which may be in 'hlsl.mainName' or other
        //! implementation defined parameter.
        virtual void _load(const std::string& filename, const Params& params) = 0;

        //! @brief Unloads the shader object.
        //! As for _load(), the Shader is locked while calling this function. Throw an error
        //! if something went bad.
        virtual void _unload() = 0;
    };
}