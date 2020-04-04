//
//  Material.cpp
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#include "Material.h"
#include "MaterialCache.h"

namespace Atl
{
    // ------------------------------------------------------------------------
    // MaterialManager

    MaterialPtr MaterialManager::makeNewMaterial(const std::string& name)
    {
        if (find(name).get())
            throw NameAlreadyExists("MaterialManager", "add", "Material %s already exists.", 
                name.data());

        MaterialPtr newMaterial = Material::New(*this, name);
        newMaterial->addListener(shared_from_this());
        
        {
            std::lock_guard l(mMutex);
            mResources.push_back(newMaterial);
        }

        send(&Listener::onResourceAdded, (Manager&)*this, *newMaterial);
        return newMaterial;
    }

    // ------------------------------------------------------------------------
    // Material

    Material::Material(Manager& rhs, const std::string& name)
    : CachedRenderable(), TResource(rhs, name), mIsTransparent(false)
    {

    }

    RGBAColor Material::ambient() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::ColorAmbient) ? 
            RGBAColor((const float*)findShaderVariableRef(MaterialElement::ColorAmbient).value()) :
            RGBAColor();
    }

    void Material::setAmbient(const RGBAColor& color)
    {
        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::ColorAmbient))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::ColorAmbient);
                variable.setValue(&color.values[0]);
            }

            else
            {
                mElements[MaterialElement::ColorAmbient] = ShaderVariable(
                    kMaterialElementColorAmbient, 
                    &color.values[0],
                    SVT::Float4);
            }

            if (color.alpha < 1.0)
                mIsTransparent = true;
        }
        
        send(&Listener::onMaterialElementModified, *this, MaterialElement::ColorAmbient);
        CachedRenderable::touch();
    }

    RGBAColor Material::diffuse() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::ColorDiffuse) ? 
            RGBAColor((const float*)findShaderVariableRef(MaterialElement::ColorDiffuse).value()) :
            RGBAColor();
    }

    void Material::setDiffuse(const RGBAColor& color)
    {
        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::ColorDiffuse))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::ColorDiffuse);
                variable.setValue(&color.values[0]);
            }

            else
            {
                mElements[MaterialElement::ColorDiffuse] = ShaderVariable(
                    kMaterialElementColorDiffuse, 
                    &color.values[0],
                    SVT::Float4);   
            }

            if (color.alpha < 1.0)
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::ColorDiffuse);
        CachedRenderable::touch();
    }

    RGBAColor Material::specular() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::ColorSpecular) ? 
            RGBAColor((const float*)findShaderVariableRef(MaterialElement::ColorSpecular).value()) :
            RGBAColor();
    }

    void Material::setSpecular(const RGBAColor& color)
    {
        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::ColorSpecular))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::ColorSpecular);
                variable.setValue(&color.values[0]);
            }

            else
            {
                mElements[MaterialElement::ColorSpecular] = ShaderVariable(
                    kMaterialElementColorSpecular, 
                    &color.values[0],
                    SVT::Float4);   
            }

            if (color.alpha < 1.0)
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::ColorSpecular);
        CachedRenderable::touch();
    }

    RGBAColor Material::emissive() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::ColorEmissive) ? 
            RGBAColor((const float*)findShaderVariableRef(MaterialElement::ColorEmissive).value()) :
            RGBAColor();
    }

    void Material::setEmissive(const RGBAColor& color)
    {
        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::ColorEmissive))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::ColorEmissive);
                variable.setValue(&color.values[0]);
            }

            else
            {
                mElements[MaterialElement::ColorEmissive] = ShaderVariable(
                    kMaterialElementColorEmissive, 
                    &color.values[0],
                    SVT::Float4);
            }

            if (color.alpha < 1.0)
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::ColorEmissive);
        CachedRenderable::touch();
    }

    TexturePtr Material::ambientTexture() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::TextureAmbient) ? 
            findShaderVariableRef(MaterialElement::TextureAmbient).texture() :
            nullptr;
    }

    void Material::setAmbientTexture(const TexturePtr& texture)
    {
        if (!texture)
            throw NullError("Material", "setAmbientTexture", "Material %s: Null Texture.",
                name().data());

        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::TextureAmbient))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::TextureAmbient);
                variable.setTexture(texture);
            }

            else
            {
                mElements[MaterialElement::TextureAmbient] = ShaderVariable(
                    kMaterialElementTextureAmbient, -1,
                    texture);
            }
            
            if (texture->isTransparent())
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::TextureAmbient);
        CachedRenderable::touch();
    }

    TexturePtr Material::diffuseTexture() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::TextureDiffuse) ? 
            findShaderVariableRef(MaterialElement::TextureDiffuse).texture() :
            nullptr;
    }

    void Material::setDiffuseTexture(const TexturePtr& texture)
    {
        if (!texture)
            throw NullError("Material", "setDiffuseTexture", "Material %s: Null Texture.",
                name().data());

        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::TextureDiffuse))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::TextureDiffuse);
                variable.setTexture(texture);
            }

            else
            {
                mElements[MaterialElement::TextureDiffuse] = ShaderVariable(
                    kMaterialElementTextureDiffuse, -1,
                    texture);
            }
            
            if (texture->isTransparent())
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::TextureDiffuse);
        CachedRenderable::touch();
    }

    TexturePtr Material::specularTexture() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::TextureSpecular) ? 
            findShaderVariableRef(MaterialElement::TextureSpecular).texture() :
            nullptr;
    }

    void Material::setSpecularTexture(const TexturePtr& texture)
    {
        if (!texture)
            throw NullError("Material", "setSpecularTexture", "Material %s: Null Texture.",
                name().data());

        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::TextureSpecular))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::TextureSpecular);
                variable.setTexture(texture);
            }

            else
            {
                mElements[MaterialElement::TextureSpecular] = ShaderVariable(
                    kMaterialElementTextureSpecular, -1,
                    texture);
            }
            
            if (texture->isTransparent())
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::TextureEmissive);
        CachedRenderable::touch();
    }

    TexturePtr Material::emissiveTexture() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::TextureEmissive) ? 
            findShaderVariableRef(MaterialElement::TextureEmissive).texture() :
            nullptr;
    }

    void Material::setEmissiveTexture(const TexturePtr& texture)
    {
        if (!texture)
            throw NullError("Material", "setEmissiveTexture", "Material %s: Null Texture.",
                name().data());

        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::TextureEmissive))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::TextureEmissive);
                variable.setTexture(texture);
            }

            else
            {
                mElements[MaterialElement::TextureEmissive] = ShaderVariable(
                    kMaterialElementTextureEmissive, -1,
                    texture);
            }
            
            if (texture->isTransparent())
                mIsTransparent = true;
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::TextureEmissive);
        CachedRenderable::touch();
    }

    Real Material::shininess() const
    {
        MaterialLockGuardCst l(*this);
        return hasElement(MaterialElement::Shininess) ? 
            ((const Real*)findShaderVariableRef(MaterialElement::Shininess).value())[0] :
            1.0;
    }

    void Material::setShininess(Real value)
    {
        {
            MaterialLockGuard l(*this);

            if (hasElement(MaterialElement::Shininess))
            {
                ShaderVariable& variable = findShaderVariableRef(MaterialElement::Shininess);
                variable.setValue(&value);
                return;
            }

            mElements[MaterialElement::Shininess] = ShaderVariable(
                kMaterialElementShininess,
                &value,
                SVT::Float1);
        }

        send(&Listener::onMaterialElementModified, *this, MaterialElement::Shininess);
        CachedRenderable::touch();
    }

    ShaderVariable& Material::findShaderVariableRef(MaterialElement element)
    {
        auto it = mElements.find(element);

        if (it == mElements.end())
            throw OutOfRange("Material", "findShaderVariableRef", "MaterialElement %i not found.",
                static_cast < int >(element));

        return it->second;
    }

    const ShaderVariable& Material::findShaderVariableRef(MaterialElement element) const
    {
        auto it = mElements.find(element);

        if (it == mElements.end())
            throw OutOfRange("Material", "findShaderVariableRef", "MaterialElement %i not found.",
                static_cast < int >(element));

        return it->second;
    }

    bool Material::hasElement(MaterialElement element) const
    {
        return mElements.find(element) != mElements.end();
    }

    ShaderVariable Material::findShaderVariable(MaterialElement element) const
    {
        MaterialLockGuardCst l(*this);

        if (!hasElement(element))
            return ShaderVariable();

        return findShaderVariableRef(element);
    }

    const void* Material::findValueOf(MaterialElement element) const
    {
        MaterialLockGuardCst l(*this);

        if (!hasElement(element))
            return nullptr;

        return findShaderVariableRef(element).value();
    }

    void Material::lock() const
    {
        mMutex.lock();
    }

    void Material::unlock() const
    {
        mMutex.unlock();
    }

    std::size_t Material::usedSize() const
    {
        std::size_t total = 0;
        MaterialLockGuardCst l(*this);

        for (auto const& pair : mElements)
            total += pair.second.size() * ShaderVariable::SizeOfType(pair.second.type());

        return total;
    }

    Material::ElementMap& Material::elements() 
    {
        return mElements;
    }

    const Material::ElementMap& Material::elements() const
    {
        return mElements;
    }

    RenderCachePtr < Material > Material::makeNewCache(Renderer& rhs)
    {
        return MaterialCache::New(rhs, *this);
    }

    bool Material::isTransparent() const
    {
        return mIsTransparent;
    }

    void Material::setIsTransparent(bool rhs)
    {
        mIsTransparent = rhs;
    }
}
