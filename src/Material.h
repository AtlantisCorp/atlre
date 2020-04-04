//
//  Material.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_MATERIAL_H
#define ATL_MATERIAL_H

#include "CachedRenderable.h"
#include "Resource.h"
#include "MaterialElement.h"
#include "ShaderVariable.h"
#include "Color.h"
#include "Texture.h"

namespace Atl
{
    class Material;
    class MaterialListener;
    class MaterialManager;
    class MaterialManagerListener;

    //! @brief Defines a Listener for a Material.
    struct MaterialListener :
    virtual public ResourceListener,
    virtual public RenderableListener
    {
        //! @brief Destructor.
        virtual ~MaterialListener() = default;

        //! @brief Launched when a MaterialElement has been modified.
        virtual void onMaterialElementModified(Material&, MaterialElement){}
    };

    //! @brief Defines a group of MaterialElements, renderable and loadable.
    //! A Material consist of a group of MaterialElements, associated to a ShaderVariable. Those
    //! ShaderVariable have a dedicated name to retrieve which ShaderVariable it should emplace
    //! in the ShaderProgram.
    //! A Material is also loadable, because you can have your own Material file format, or you
    //! can use other file formats.
    class Material : 
    virtual public CachedRenderable <
    Material >,
    virtual public TResource <
    Material,
    MaterialManager >
    {
    public:
        ATL_SHAREABLE(Material)

        //! @brief The listener for this class.
        typedef MaterialListener Listener;

        //! @brief The element's map.
        typedef std::map < MaterialElement, ShaderVariable > ElementMap;

    protected:
        //! @brief Associates a MaterialElement to a ShaderVariable.
        ElementMap mElements;

        //! @brief Boolean true if at least one color or one texture is transparent. (alpha channel
        //! is between [0.0, 1.0).) The boolean is cached when modifying an element of the Material
        //! using set*() functions. You can also use \ref setIsTransparent() to force transparency.
        std::atomic < bool > mIsTransparent;

        //! @brief The mutex.
        mutable std::mutex mMutex;

    public:
        //! @brief Constructs a new Material.
        Material(Manager& manager, const std::string& name);

        //! @brief Returns the ambient color.
        RGBAColor ambient() const;

        //! @brief Sets the ambient color.
        void setAmbient(const RGBAColor& color);

        //! @brief Returns the diffuse color.
        RGBAColor diffuse() const;

        //! @brief Sets the diffuse color.
        void setDiffuse(const RGBAColor& color);

        //! @brief Returns the specular color.
        RGBAColor specular() const;

        //! @brief Sets the specular color.
        void setSpecular(const RGBAColor& color);

        //! @brief Returns the emissive color.
        RGBAColor emissive() const;

        //! @brief Sets the emissive color.
        void setEmissive(const RGBAColor& color);

        //! @brief Returns the ambient texture.
        TexturePtr ambientTexture() const;

        //! @brief Sets the ambient texture.
        void setAmbientTexture(const TexturePtr& texture);

        //! @brief Returns the diffuse texture.
        TexturePtr diffuseTexture() const;

        //! @brief Sets the diffuse texture.
        void setDiffuseTexture(const TexturePtr& texture);

        //! @brief Returns the specular texture.
        TexturePtr specularTexture() const;

        //! @brief Sets the specular texture.
        void setSpecularTexture(const TexturePtr& texture);

        //! @brief Returns the emissive texture.
        TexturePtr emissiveTexture() const;

        //! @brief Sets the emissive texture.
        void setEmissiveTexture(const TexturePtr& texture);

        //! @brief Returns the shininess value.
        Real shininess() const;

        //! @brief Sets the shininess value.
        void setShininess(Real value);

        //! @brief Returns a reference to the ShaderVariable for a MaterialElement.
        //! The Material must be locked with MaterialLockGuard as no mutex locking is
        //! done in this function. It throws an OutOfRange Error if the MaterialElement
        //! isn't found.
        ShaderVariable& findShaderVariableRef(MaterialElement element);

        //! @brief Returns a reference to the ShaderVariable for a MaterialElement.
        //! The Material must be locked with MaterialLockGuardCst as no mutex locking is
        //! done in this function. It throws an OutOfRange Error if the MaterialElement
        //! isn't found.
        const ShaderVariable& findShaderVariableRef(MaterialElement element) const;

        //! @brief Returns true if this Material has the given MaterialElement.
        //! The Material must be locked with MaterialLockGuardCst as no mutex locking is
        //! done in this function.
        bool hasElement(MaterialElement element) const;

        //! @brief Returns the ShaderVariable for a MaterialElement.
        //! If the MaterialElement isn't found, then it returns an empty ShaderVariable. However, 
        //! prefer \ref hasElement() and \ref findShaderVariableRef() to iterate through elements
        //! in the Material, or even \ref forEachElement().
        ShaderVariable findShaderVariable(MaterialElement element) const;

        //! @brief Returns the value of a MaterialElement, or null if this MaterialElement 
        //! is not found in the Material.
        const void* findValueOf(MaterialElement element) const;

        //! @brief Locks the Material.
        //! This function must be used with \ref unlock() when planning to use \ref hasElement()
        //! or \ref findShaderVariableRef().
        void lock() const;

        //! @brief Unlocks the Material.
        //! This function must be used with \ref lock() when planning to use \ref hasElement()
        //! or \ref findShaderVariableRef().
        void unlock() const;

        //! @brief Returns the used size of this Resource.
        //! The returned size is the sum of all values in each \ref ShaderVariable.
        std::size_t usedSize() const;

        //! @brief Returns the elements map.
        //! The Material must be locked with \ref MaterialLockGuard to ensure no other
        //! thread is accessing the element map while working on it.
        ElementMap& elements();

        //! @brief Returns the elements map.
        //! The Material must be locked with \ref MaterialLockGuardCst to ensure no other
        //! thread is accessing the element map while working on it.
        const ElementMap& elements() const;

        //! @brief Creates a new RenderCache structure for the given Renderer.
        //! The cache class used is \ref MaterialCache.
        RenderCachePtr < Material > makeNewCache(Renderer& rhs);

        //! @brief Returns \ref mIsTransparent.
        bool isTransparent() const;

        //! @brief Forces transparency support by setting \ref mIsTransparent to true.
        void setIsTransparent(bool rhs);
    };
    
    ATL_BASE_RESOURCE(Material)
    ATL_LOCKABLE_RESOURCE(Material)
    
    //! @brief Defines a loader for a Material file.
    template < >
    struct LoaderTrait < Material > : public BaseLoader
    {
        //! @brief Destructor.
        virtual ~LoaderTrait() = default;
        
        //! @brief Loads a Material from a file.
        //! @param material The material we want to fill.
        //! @param filename The filename to load.
        //! @param params Some arbitrary parameters to pass to the implementation.
        virtual void load(Material& material, const std::string& filename, const Params& params) = 0;
    };
    
    ATL_LOADER_RESOURCE(Material)
    
    //! @brief The Material Manager List.
    struct MaterialManagerListener :
    virtual public ManagerListener <
    MaterialManager,
    Material,
    MaterialManagerListener >{};
    
    //! @brief Defines a Manager for Material.
    struct MaterialManager :
    virtual public Manager <
    MaterialManager,
    Material,
    MaterialManagerListener >
    {
        //! @brief Destructor.
        virtual ~MaterialManager() = default;
        
        //! @brief Creates a new Material in this Manager.
        //! We add this function to this manager because a Material can be loaded but can
        //! also be created without a Loader.
        virtual MaterialPtr makeNewMaterial(const std::string& name);
    };
}

#endif // ATL_MATERIAL_H
