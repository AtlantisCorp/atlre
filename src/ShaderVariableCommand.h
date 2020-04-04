//
//  ShaderVariableCommand.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef ShaderVariableCommand_h
#define ShaderVariableCommand_h

#include "ShaderVariable.h"
#include "RenderCommand.h"

namespace Atl
{
    //! @brief A RenderCommand to bind a ShaderVariable.
    //! This command binds a ShaderVariable into the current context. The current binding is
    //! render API specific (\see GLShaderVariableCommand). This command is optimized for 
    //! setting once the \ref ShaderVariable and then modifying only the value. This command
    //! can be shared among multiple objects sharing the same variable (this can be the case
    //! if the ShaderVariable is a global variable).
    class ShaderVariableCommand : public RenderCommandBase
    {
        //! @brief The variable to bind.
        ShaderVariable mVariable;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:
        ATL_SHAREABLE(ShaderVariableCommand)

        //! @brief Constructs a new RenderCommand.
        //! The \ref ShaderVariable created is empty, and thus cannot be used as-is.
        ShaderVariableCommand(Renderer& renderer);

        //! @brief Changes the ShaderVariable in this command.
        void setShaderVariable(const ShaderVariable& rhs);

        //! @brief Changes the value inside the current variable.
        //! The value must corresponds to the type and size registered in the variable. If
        //! you are not sure of those values, please check \ref ShaderVariable::type() and
        //! \ref ShaderVariable::size().
        void setVariableValue(const void* value);

        //! @brief Returns a copy of the \ref ShaderVariable in this command.
        //! This function should be used with caution, because it copies the internal value
        //! and thus, is slow.
        ShaderVariable variable() const;

        //! @brief Returns the variable's name.
        std::string variableName() const;

        //! @brief Returns the variable's type.
        ShaderVariableType variableType() const;

        //! @brief Returns the variable's index.
        int variableIndex() const;

        //! @brief Returns the size of the value in \ref ShaderVariable.
        std::size_t variableValueSize() const;
    };

    //! @brief Pointer for a \ref ShaderVariableCommand.
    typedef std::shared_ptr < ShaderVariableCommand > ShaderVariableCommandPtr;

    //! @brief List of \ref ShaderVariableCommand.
    typedef std::vector < ShaderVariableCommandPtr > ShaderVariableCommandList;
}

#endif // ShaderVariableCommand_h