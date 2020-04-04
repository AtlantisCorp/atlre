//
//  ShaderVariable.cpp
//  atl
//
//  Created by jacques tronconi on 25/03/2020.
//

#include "ShaderVariable.h"

namespace Atl
{
    ShaderVariable::ShaderVariable()
    : mName(), mIndex(-1), mType(SVT::Float1), mSize(0)
    {

    }

    ShaderVariable::ShaderVariable(const std::string& name, const void* data, ShaderVariableType type, unsigned size)
    : mName(name), mIndex(-1), mType(type), mSize(size)
    {
        const std::size_t sizeOf = SizeOfType(type) * mSize;

        if (!sizeOf)
            throw ShaderVariableNullType("ShaderVariable", "ShaderVariable", "Type %i is invalid.", 
                                         (int)type);

        mData.resize(sizeOf);
        setValue(data);
    }

    ShaderVariable::ShaderVariable(const std::string& name, int index, const void* data, ShaderVariableType type, unsigned size)
    : mName(name), mIndex(index), mType(type), mSize(size)
    {
        const std::size_t sizeOf = SizeOfType(type) * mSize;

        if (!sizeOf)
            throw ShaderVariableNullType("ShaderVariable", "ShaderVariable", "Type %i is invalid.", 
                                         (int)type);

        mData.resize(sizeOf);
        setValue(data);
    }

    ShaderVariable::ShaderVariable(const std::string& name, int index, const TexturePtr& texture)
    : mName(name), mIndex(index), mType(SVT::Texture), mSize(1), mTexture(texture)
    {
        if (!texture)
            throw NullError("ShaderVariable", "ShaderVariable", "Null Texture for ShaderVariable %s.",
                name.data());
    }

    ShaderVariable::ShaderVariable(const ShaderVariable& rhs)
    {
        mName = rhs.mName;
        mIndex = rhs.mIndex;
        mType = rhs.mType;
        mSize = rhs.mSize;
        mData = rhs.mData;
    }

    ShaderVariable& ShaderVariable::operator = (const ShaderVariable& rhs)
    {
        mName = rhs.mName;
        mIndex = rhs.mIndex;
        mType = rhs.mType;
        mSize = rhs.mSize;
        mData = rhs.mData;
        return *this;
    }

    ShaderVariable::~ShaderVariable()
    {
        if (mType == SVT::Texture)
            mTexture.~shared_ptr();
        else
            mData.~vector();

        memset(&mData, 0, std::max(sizeof(mTexture), sizeof(mData)));
    }

    void ShaderVariable::setName(const std::string& name)
    {
        mName = name;
    }

    const std::string& ShaderVariable::name() const
    {
        return mName;
    }

    void ShaderVariable::setIndex(int index)
    {
        mIndex = index;
    }

    int ShaderVariable::index() const
    {
        return mIndex;
    }

    void ShaderVariable::setType(ShaderVariableType type)
    {
        if (mType == SVT::Texture) 
        {
            mTexture.~shared_ptr();
            memset(&mTexture, 0, sizeof(mTexture));
        }

        else if (mType != SVT::Texture && type == SVT::Texture)
        {
            mData.~vector();
            memset(&mData, 0, sizeof(mData));
        }

        mType = type;

        const std::size_t sizeOf = SizeOfType(mType) * mSize;

        if (mData.size() != sizeOf)
            mData.resize(sizeOf);
    }

    ShaderVariableType ShaderVariable::type() const
    {
        return mType;
    }

    void ShaderVariable::setSize(unsigned size)
    {
        mSize = size;

        const std::size_t sizeOf = SizeOfType(mType) * mSize;

        if (mType != SVT::Texture && mData.size() != sizeOf)
            mData.resize(sizeOf);
    }

    unsigned ShaderVariable::size() const
    {
        return mSize;
    }

    void ShaderVariable::setValue(const void* value)
    {
        if (mType == SVT::Texture)
            throw ShaderVariableNullType("ShaderVariable", "setValue", "ShaderVariable %s is a Texture.",
                mName.data());

        if (!value)
        {
            memset(&mData[0], 0, mData.size());
            return;
        }

        memcpy(&mData[0], value, mData.size());
    }

    void* ShaderVariable::value() 
    {
        if (mType == SVT::Texture)
            throw ShaderVariableNullType("ShaderVariable", "value", "ShaderVariable %s is a Texture.",
                mName.data());

        return &mData[0];
    }

    const void* ShaderVariable::value() const
    {
        if (mType == SVT::Texture)
            throw ShaderVariableNullType("ShaderVariable", "value", "ShaderVariable %s is a Texture.",
                mName.data());

        return &mData[0];
    }

    TexturePtr ShaderVariable::texture() const
    {
        if (mType != SVT::Texture)
            throw ShaderVariableNullType("ShaderVariable", "texture", "ShaderVariable %s is not a Texture.",
                mName.data());
        
        return mTexture;
    }

    void ShaderVariable::setTexture(const TexturePtr& texture)
    {
        if (mType != SVT::Texture)
            throw ShaderVariableNullType("ShaderVariable", "setTexture", "ShaderVariable %s is not a Texture.",
                mName.data());
        
        mTexture = texture;
    }

    std::size_t ShaderVariable::SizeOfType(ShaderVariableType type)
    {
        switch(type)
        {
            case SVT::Float1: return sizeof(float) * 1;
            case SVT::Float2: return sizeof(float) * 2;
            case SVT::Float3: return sizeof(float) * 3;
            case SVT::Float4: return sizeof(float) * 4;

            case SVT::MatrixF2x2: return sizeof(float) * 4;
            case SVT::MatrixF2x3: return sizeof(float) * 6;
            case SVT::MatrixF2x4: return sizeof(float) * 8;
            case SVT::MatrixF3x2: return sizeof(float) * 6;
            case SVT::MatrixF3x3: return sizeof(float) * 9;
            case SVT::MatrixF3x4: return sizeof(float) * 12;
            case SVT::MatrixF4x2: return sizeof(float) * 8;
            case SVT::MatrixF4x3: return sizeof(float) * 12;
            case SVT::MatrixF4x4: return sizeof(float) * 16;

            case SVT::Double1: return sizeof(double) * 1;
            case SVT::Double2: return sizeof(double) * 2;
            case SVT::Double3: return sizeof(double) * 3;
            case SVT::Double4: return sizeof(double) * 4;

            case SVT::MatrixD2x2: return sizeof(double) * 4;
            case SVT::MatrixD2x3: return sizeof(double) * 6;
            case SVT::MatrixD2x4: return sizeof(double) * 8;
            case SVT::MatrixD3x2: return sizeof(double) * 6;
            case SVT::MatrixD3x3: return sizeof(double) * 9;
            case SVT::MatrixD3x4: return sizeof(double) * 12;
            case SVT::MatrixD4x2: return sizeof(double) * 8;
            case SVT::MatrixD4x3: return sizeof(double) * 12;
            case SVT::MatrixD4x4: return sizeof(double) * 16;

            case SVT::Int1: 
            case SVT::Bool1:
                return sizeof(int) * 1;
            case SVT::Int2: 
            case SVT::Bool2:
                return sizeof(int) * 2;
            case SVT::Int3: 
            case SVT::Bool3:
                return sizeof(int) * 3;
            case SVT::Int4: 
            case SVT::Bool4:
                return sizeof(int) * 4;

            case SVT::Unsigned1: return sizeof(unsigned) * 1;
            case SVT::Unsigned2: return sizeof(unsigned) * 2;
            case SVT::Unsigned3: return sizeof(unsigned) * 3;
            case SVT::Unsigned4: return sizeof(unsigned) * 4;

            default:
                return 0;
        }
    }

    std::size_t ShaderVariable::valueSize() const
    {
        if (isTexture())
            return sizeof(int);

        return SizeOfType(mType) * mSize;
    }

    bool ShaderVariable::isTexture() const
    {
        return mType == SVT::Texture;
    }
}