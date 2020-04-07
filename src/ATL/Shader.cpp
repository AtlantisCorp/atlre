//
//  Shader.cpp
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Shader.h"

namespace Atl
{
    // --------------------------------------------------------------------------------------------
    // ShaderManager

    ShaderPtr ShaderManager::findName(const std::string& name) const
    {
        return find([name](const ShaderPtr& rhs){ return rhs->name() == name; });
    }

    ShaderPtr ShaderManager::findFilename(const std::string& filename) const
    {
        return find([filename](const ShaderPtr& rhs){ return rhs->filename() == filename; });
    }

    // --------------------------------------------------------------------------------------------
    // Shader

    Shader::Shader(Renderer& renderer, const std::string& name)
    : RenderObject(renderer), Resource(name), mHasError(false)
    {

    }

    std::future < void > Shader::load(ShaderType type, const std::string& filename, const Params& params)
    {
        return std::async(std::launch::async, [this, filename, params, type]
        {
            if (filename.empty())
                return;

            send(&Listener::onResourceWillLoad, (Resource&)*this).get();

            {
                LockableGuard l((Lockable&)*this);
                mFilename = filename;
                mType = type;
                
                try 
                {
                    _load(filename, params);
                    setState(Loaded);
                }

                catch (ShaderCompileError const& e) 
                {
                    mLastError = e;
                    mHasError = true;
                    throw e;
                }
            }

            send(&Listener::onShaderCompiled, *this).get();
            send(&Listener::onResourceDidLoad, (Resource&)*this).get();
        });
    }

    std::future < void > Shader::unload()
    {
        return std::async(std::launch::async, [this]
        {
            send(&Listener::onResourceWillUnload, (Resource&)*this).get();

            {
                LockableGuard l((Lockable&)*this);
                mFilename.clear();
                
                _unload();
                setState(0);
            }

            send(&Listener::onResourceDidUnload, (Resource&)*this).get();
        });
    }
}