//
//  MaterialElement.h
//  atl
//
//  Created by jacques tronconi on 31/03/2020.
//

#ifndef ATL_MATERIALELEMENT_H
#define ATL_MATERIALELEMENT_H

namespace Atl
{
    //! @brief Defines some elements that can be used in a Material. 
    enum class MaterialElement
    {
        ColorAmbient, ColorDiffuse, 
        ColorSpecular, ColorEmissive,

        TextureAmbient, TextureDiffuse, 
        TextureSpecular, TextureEmissive,

        Shininess,

        Max
    };

    //! @brief Default name for MaterialElement::ColorAmbient.
    static std::string kMaterialElementColorAmbient = "material.ambient";

    //! @brief Default name for MaterialElement::ColorDiffuse.
    static std::string kMaterialElementColorDiffuse = "material.diffuse";

    //! @brief Default name for MaterialElement::ColorSpecular.
    static std::string kMaterialElementColorSpecular = "material.specular";

    //! @brief Default name for MaterialElement::ColorEmissive.
    static std::string kMaterialElementColorEmissive = "material.emissive";

    //! @brief Default name for MaterialElement::TextureAmbient.
    static std::string kMaterialElementTextureAmbient = "material.texAmbient";

    //! @brief Default name for MaterialElement::TextureDiffuse.
    static std::string kMaterialElementTextureDiffuse = "material.texDiffuse";

    //! @brief Default name for MaterialElement::TextureSpecular.
    static std::string kMaterialElementTextureSpecular = "material.texSpecular";

    //! @brief Default name for MaterialElement::TextureEmissive.
    static std::string kMaterialElementTextureEmissive = "material.texEmissive";

    //! @brief Default name for MaterialElement::Shininess.
    static std::string kMaterialElementShininess = "material.shininess";
}

#endif // ATL_MATERIALELEMENT_H