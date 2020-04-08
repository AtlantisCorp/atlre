//
//  RenderPipeline.cpp
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "RenderPipeline.h"
#include "Renderer.h"

namespace Atl
{
    // --------------------------------------------------------------------------------------------
    // RenderPipelineManager

    RenderPipelinePtr RenderPipelineManager::findName(const std::string& name) const
    {
        return find([&name](const RenderPipelinePtr& rhs){ return rhs->name() == name; });
    }

    // --------------------------------------------------------------------------------------------
    // AliasInfos

    RenderPipeline::AliasInfos::AliasInfos(const std::string& alias, const std::string& name, ShaderType shader, int location)
    {
        mAliasName = alias;
        mRealName = name;
        mShader = shader;
        mLocation = location;
    }

    // --------------------------------------------------------------------------------------------
    // RenderPipeline

    RenderPipeline::RenderPipeline(Renderer& renderer, const std::string& name)
    : RenderObject(renderer), Resource(name)
    {

    }

    std::future < void > RenderPipeline::loadShaders(const std::vector < ShaderInfos >& shaders)
    {
        return std::async(std::launch::async, [this, shaders]()
        {
            if (isLoaded())
                throw AlreadyLoaded("RenderPipeline", "loadShaders", 
                    "%s: Cannot change shaders of already loaded pipeline. Please call RenderPipeline::unload() before.", 
                    name().data());

            for (auto& infos : shaders)
            {
                ShaderPtr shader = nullptr;

                if (!infos.name.empty())
                    shader = renderer().shaderManager().findName(infos.name);

                if (!shader)
                {
                    shader = renderer().newShader(infos.name, infos.filename, infos.type, infos.params).get();
                    send(&Listener::onPipelineDidLoadShader, *this, *shader).get();
                }

                if (!shader) 
                    throw NullError("RenderPipeline", "loadShaders", "Error loading shader %s.", infos.filename.data());

                LockableGuard l(*this);
                mShaders[infos.type] = shader;
            }
        });
    }

    std::future < void > RenderPipeline::unload()
    {
        return std::async(std::launch::async, [this]()
        {
            if (!isLoaded())
                return;

            LockableGuard l(*this);
            mShaders.clear();
            mAliases.clear();
            _unload();

            setState(0);
        });
    }

    std::future < void > RenderPipeline::build()
    {
        return std::async(std::launch::async, [this]()
        {
            if (isLoaded())
                throw AlreadyLoaded("RenderPipeline", "build", 
                    "%s: Cannot change shaders of already loaded pipeline. Please call RenderPipeline::unload() before.", 
                    name().data());
            
            LockableGuard l(*this);
            _build();

            setState(Loaded);
            send(&Listener::onPipelineBuilt, *this);
        });
    }

    void RenderPipeline::setAliases(const RenderPipeline::AliasesMap& aliases)
    {
        if (!isLoaded())
            throw Error("RenderPipeline", "setAliases", "Pipeline %s not loaded. Please call ::build() before.",
                name().data());

        LockableGuard l(*this);

        for (auto& pair : aliases)
        {
            _setAlias(pair.first, pair.second);
        }
    }

    void RenderPipeline::setAliasesInfos(const std::vector < AliasInfos >& infos)
    {
        if (!isLoaded())
            throw Error("RenderPipeline", "setAliases", "Pipeline %s not loaded. Please call ::build() before.",
                name().data());

        LockableGuard l(*this);

        for (auto& alias : infos)
        {
            _setAlias(alias.mAliasName, (AliasValue&)alias);
        }
    }

    void RenderPipeline::setAlias(const std::string& name, const std::string& realName, ShaderType type, int location)
    {
        if (!isLoaded())
            throw Error("RenderPipeline", "setAliases", "Pipeline %s not loaded. Please call ::build() before.",
                this->name().data());

        LockableGuard l(*this);
        _setAlias(name, AliasValue{ realName, type, location });
    }

    int RenderPipeline::findAliasLocation(const std::string& alias) const
    {
        LockableGuard l(*this);

        auto it = mAliases.find(alias);

        if (it == mAliases.end())
            return findUniform(alias).mLocation;

        const AliasValue& value = it->second;

        if (value.mLocation == -1)
            const_cast < AliasValue& >(value).mLocation = findUniformLocation(value.mShader, value.mRealName);

        return value.mLocation;
    }

    std::string RenderPipeline::findAliasName(const std::string& alias) const
    {
        LockableGuard l(*this);

        auto it = mAliases.find(alias);

        if (it != mAliases.end())
            return it->second.mRealName;

        return findUniform(alias).mName;
    }

    void RenderPipeline::_setAlias(const std::string& alias, const RenderPipeline::AliasValue& value)
    {
        if (!value.mRealName.empty())
        {
            int location = findUniformLocation(value.mShader, value.mRealName);
            if (location == -1)
                throw Error("RenderPipeline", "setAliases", "Uniform name %s not found in pipeline %s.", 
                    value.mRealName.data(), name().data());

            mAliases[alias] = AliasValue{
                .mRealName = value.mRealName,
                .mShader = value.mShader,
                .mLocation = location
            };
        }

        else  
        {
            std::string realName = findUniformName(value.mShader, value.mLocation);
            if (realName.empty())
                throw Error("RenderPipeline", "setAliases", "No uniform with location %i found in pipeline %s.",
                    value.mLocation, name().data());

            mAliases[alias] = AliasValue{
                .mRealName = realName,
                .mShader = value.mShader,
                .mLocation = value.mLocation
            };
        }
    }
}