//
//  ShaderVariableCommand.cpp
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#include "ShaderVariableCommand.h"

namespace Atl
{
    ShaderVariableCommand::ShaderVariableCommand(Renderer& renderer)
    : RenderCommandBase(renderer)
    {

    }

    void ShaderVariableCommand::setShaderVariable(const ShaderVariable& rhs)
    {
        std::lock_guard l(mMutex);
        mVariable = rhs;
    }

    void ShaderVariableCommand::setVariableValue(const void* value)
    {
        std::lock_guard l(mMutex);
        mVariable.setValue(value);
    }

    ShaderVariable ShaderVariableCommand::variable() const
    {
        std::lock_guard l(mMutex);
        return mVariable;
    }

    std::string ShaderVariableCommand::variableName() const
    {
        std::lock_guard l(mMutex);
        return mVariable.name();
    }

    ShaderVariableType ShaderVariableCommand::variableType() const
    {
        std::lock_guard l(mMutex);
        return mVariable.type();
    }

    int ShaderVariableCommand::variableIndex() const
    {
        std::lock_guard l(mMutex);
        return mVariable.index();
    }

    std::size_t ShaderVariableCommand::variableValueSize() const
    {
        std::lock_guard l(mMutex);
        return mVariable.valueSize();
    }
}