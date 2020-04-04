//
//  ShaderVariable.h
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#ifndef ShaderVariable_h
#define ShaderVariable_h

#include "Error.h"
#include "Texture.h"

#include <vector>

namespace Atl
{
    //! @brief Error launched if there was an error with the ShaderVariable type.
    struct EXPORTED ShaderVariableNullType : public Error 
    { using Error::Error; };

    //! @brief Enumerates all possible types for a ShaderVariable.
    enum class ShaderVariableType
    {
        Float1,
        Float2,
        Float3,
        Float4,

        MatrixF2x2,
        MatrixF2x3,
        MatrixF2x4,
        MatrixF3x2,
        MatrixF3x3,
        MatrixF3x4,
        MatrixF4x2,
        MatrixF4x3,
        MatrixF4x4,

        Double1,
        Double2,
        Double3,
        Double4,

        MatrixD2x2,
        MatrixD2x3,
        MatrixD2x4,
        MatrixD3x2,
        MatrixD3x3,
        MatrixD3x4,
        MatrixD4x2,
        MatrixD4x3,
        MatrixD4x4,

        Int1, 
        Int2,
        Int3,
        Int4,

        Unsigned1,
        Unsigned2,
        Unsigned3,
        Unsigned4,

        Bool1,
        Bool2,
        Bool3,
        Bool4,

        Texture,

    #   ifndef ATL_REAL_AS_DOUBLE
        Real1 = Float1,
        Real2 = Float2,
        Real3 = Float3,
        Real4 = Float4,

        MatrixR2x2 = MatrixF2x2,
        MatrixR2x3 = MatrixF2x3,
        MatrixR2x4 = MatrixF2x4,
        MatrixR3x2 = MatrixF3x2,
        MatrixR3x3 = MatrixF3x3,
        MatrixR3x4 = MatrixF3x4,
        MatrixR4x2 = MatrixF4x2,
        MatrixR4x3 = MatrixF4x3,
        MatrixR4x4 = MatrixF4x4

    #   else
        Real1 = Double1,
        Real2 = Double2,
        Real3 = Double3,
        Real4 = Double4,

        MatrixR2x2 = MatrixD2x2,
        MatrixR2x3 = MatrixD2x3,
        MatrixR2x4 = MatrixD2x4,
        MatrixR3x2 = MatrixD3x2,
        MatrixR3x3 = MatrixD3x3,
        MatrixR3x4 = MatrixD3x4,
        MatrixR4x2 = MatrixD4x2,
        MatrixR4x3 = MatrixD4x3,
        MatrixR4x4 = MatrixD4x4

    #   endif
    };

    using SVT = ShaderVariableType;

    //! @brief Defines a variable in a programmable shader.
    class EXPORTED ShaderVariable
    {
        //! @brief The variable's name.
        std::string mName;

        //! @brief The variable's index, if applicable, in the programmable shader. Default
        //! invalid value is -1.
        int mIndex;

        //! @brief The variable's type.
        ShaderVariableType mType;

        //! @brief The variable's number of element of type. Used merely for arrays, 1 is the
        //! default value.
        unsigned mSize;

        union 
        {
            //! @brief The data for this variable. Represented by an array of char, as it is convertible
            //! easily into a const void* or anything you like.
            std::vector < char > mData;

            //! @brief The texture pointer if this variable is a texture.
            TexturePtr mTexture;
        };

    public:

        //! @brief Constructs an empty variable.
        ShaderVariable();

        //! @brief Constructs a new variable.
        //! @param name The variable's name. This name will be looked up in the ShaderProgram
        //! when binding it, so choose a name it will find. Please \see ShaderProgram for more
        //! informations.
        //! @param data The variable's data. The size of the data is computed from type and 
        //! size. If a null pointer is passed, the data is initialized with zero.
        //! @param type The variable's data type.
        //! @param size The variable's data size, used only for arrays. For non array data, uses
        //! the default value 1.
        ShaderVariable(const std::string& name, const void* data, ShaderVariableType type, unsigned size = 1);

        //! @brief Constructs a new variable.
        //! @param name The variable's name. This name will be looked up in the ShaderProgram
        //! when binding it, so choose a name it will find. Please \see ShaderProgram for more
        //! informations.
        //! @param index The index of the variable in the ShaderProgram that will use this
        //! variable. Don't use this index if you don't know which ShaderProgram will use it. 
        //! Instead, create a \ref ShaderVariableCommand which will save the variable's value.
        //! @param data The variable's data. The size of the data is computed from type and 
        //! size. If a null pointer is passed, the data is initialized with zero.
        //! @param type The variable's data type.
        //! @param size The variable's data size, used only for arrays. For non array data, uses
        //! the default value 1.
        ShaderVariable(const std::string& name, int index, const void* data, ShaderVariableType type, unsigned size = 1);

        //! @brief Constructs a new variable.
        //! @param name The variable's name. This name will be looked up in the ShaderProgram
        //! when binding it, so choose a name it will find. Please \see ShaderProgram for more
        //! informations.
        //! @param index The index of the variable in the ShaderProgram that will use this
        //! variable. Don't use this index if you don't know which ShaderProgram will use it. 
        //! Instead, create a \ref ShaderVariableCommand which will save the variable's value.
        //! @param texture The texture to set for this variable. Must not be null, or it will 
        //! throw a NullError Error. 
        ShaderVariable(const std::string& name, int index, const TexturePtr& texture);

        //! @brief Constructs a variable from another one.
        ShaderVariable(const ShaderVariable& rhs);

        //! @brief Assignment operator.
        ShaderVariable& operator = (const ShaderVariable& rhs);

        //! @brief Destructor.
        ~ShaderVariable();

        //! @brief Changes the variable's name.
        void setName(const std::string& name);

        //! @brief Returns the variable's name.
        const std::string& name() const;

        //! @brief Changes the index in this variable. Please \see ShaderVariableCommand for
        //! an utilisation of this index.
        void setIndex(int index);

        //! @brief Returns the variable's index, -1 if invalid.
        int index() const;

        //! @brief Changes the variable's type. Please notes that changing the type may resize
        //! the data in this variable and thus, erase old data.
        void setType(ShaderVariableType type);

        //! @brief Returns the variable's type.
        ShaderVariableType type() const;

        //! @brief Changes the size of this variable (in number of elements, as in an array).
        //! Please notes that changing the size may resize the data in this variable and thus, 
        //! erase old data.
        void setSize(unsigned size);

        //! @brief Returns the variable's number of elements.
        unsigned size() const;

        //! @brief Changes the value in this variable.
        void setValue(const void* data);

        //! @brief Returns a pointer to the value in this variable.
        const void* value() const;

        //! @brief Returns a pointer to the value in this variable.
        void* value();

        //! @brief Returns the Texture held by this variable if type is Texture.
        TexturePtr texture() const;

        //! @brief Sets \ref mTexture. \ref mType must be \ref ShaderVariableType::Texture.
        void setTexture(const TexturePtr& texture);

        //! @brief Returns the size, in bytes, for a type.
        static std::size_t SizeOfType(ShaderVariableType type);

        //! @brief Returns the size of the value held in this variable. If this value is
        //! a texture, it returns sizeof(int) as it only is a texture unit.
        std::size_t valueSize() const;

        //! @brief Returns true if this variable holds a Texture.
        bool isTexture() const;
    };
}

#endif // ShaderVariable_h