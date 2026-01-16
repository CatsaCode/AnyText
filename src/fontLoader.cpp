#include "fontLoader.hpp"
#include "main.hpp"

#include "TMPro/TMP_Text.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "TMPro/AtlasPopulationMode.hpp"
#include "TMPro/ShaderUtilities.hpp"

#include "UnityEngine/Font.hpp"
#include "UnityEngine/TextCore/LowLevel/GlyphRenderMode.hpp"
#include "UnityEngine/TextCore/LowLevel/FontEngine.hpp"
#include "UnityEngine/TextCore/LowLevel/FontEngineError.hpp"
#include "UnityEngine/TextCore/GlyphRect.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Resources.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include <map>
#include <filesystem>

using namespace UnityEngine;
using namespace UnityEngine::TextCore::LowLevel;

namespace AnyText {

    std::map<std::string, SafePtrUnity<TMPro::TMP_FontAsset>> fontAssets = {};
    
    // Transpile and fix TMPro::TMP_FontAsset::CreateFontAsset
    // Beat Saber uses padding 5, 1024x512, Static, false
    static TMPro::TMP_FontAsset* createFontAsset(Font *font, int samplingPointSize = 90, int atlasPadding = 9, int atlasWidth = 1024, int atlasHeight = 1024, TMPro::AtlasPopulationMode atlasPopulationMode = TMPro::AtlasPopulationMode::Dynamic, bool enableMultiAtlasSupport = true) {
        GlyphRenderMode renderMode = GlyphRenderMode::SDFAA; // Beat Saber only has signed distance field shaders

        FontEngine::InitializeFontEngine();
        FontEngineError errorCode = FontEngine::LoadFontFace(font, samplingPointSize);
		if(errorCode != FontEngineError::Success) {
			PaperLogger.error("Unable to load font face for '{}', Error: {}", font->get_name(), static_cast<int>(errorCode));
			return nullptr;
		}
		TMPro::TMP_FontAsset* tMP_FontAsset = ScriptableObject::CreateInstance<TMPro::TMP_FontAsset*>();
		tMP_FontAsset->m_Version = "1.1.0";
        tMP_FontAsset->set_name(font->get_name()); // Manually assign name
		tMP_FontAsset->faceInfo = FontEngine::GetFaceInfo();
		if (atlasPopulationMode == TMPro::AtlasPopulationMode::Dynamic) {
			tMP_FontAsset->sourceFontFile = font;
		}
		tMP_FontAsset->atlasPopulationMode = atlasPopulationMode;
		tMP_FontAsset->atlasWidth = atlasWidth;
		tMP_FontAsset->atlasHeight = atlasHeight;
		tMP_FontAsset->atlasPadding = atlasPadding;
		tMP_FontAsset->atlasRenderMode = renderMode;
        Texture2D* texture2D = Texture2D::New_ctor(2, 2, TextureFormat::Alpha8, false, false);
		tMP_FontAsset->atlasTextures = {texture2D};
		tMP_FontAsset->isMultiAtlasTexturesEnabled = enableMultiAtlasSupport;

        // Shader::Find("TextMeshPro/Mobile/Distance Field Zero Alpha Write") does not work for some reason
        // The FontAsset "Teko-Medium SDF" used on curved canvases stops curving when I use it. Not sure where "Teko-Medium SDF Numbers Monospaced Curved" is supposed to be used but it fixes my issue
        static UnityW<Material> originalMaterial = nullptr;
        // TODO If text is identified before everything loads (ie matching /.+/) then this won't find the First and crash
        if(!originalMaterial) originalMaterial = Resources::FindObjectsOfTypeAll<TMPro::TMP_FontAsset*>()->First([](TMPro::TMP_FontAsset* fontAsset){return fontAsset->get_name() == "Teko-Medium SDF Numbers Monospaced Curved";})->material;
        Material* material = Material::New_ctor(originalMaterial);;
		
        // Some renderMode logic is trimmed out
        int num = 1;
        material->SetTexture("_MainTex", texture2D);
        material->SetFloat("_TextureWidth", atlasWidth);
        material->SetFloat("_TextureHeight", atlasHeight);
        material->SetFloat("_GradientScale", atlasPadding + num);
        material->SetFloat("_WeightNormal", tMP_FontAsset->normalStyle);
        material->SetFloat("_WeightBold", tMP_FontAsset->boldStyle);
        tMP_FontAsset->material = material;
        
        tMP_FontAsset->freeGlyphRects = System::Collections::Generic::List_1<TextCore::GlyphRect>::New_ctor(8);
        tMP_FontAsset->freeGlyphRects->_items = {TextCore::GlyphRect(0, 0, atlasWidth - num, atlasHeight - num)};
        tMP_FontAsset->usedGlyphRects = System::Collections::Generic::List_1<TextCore::GlyphRect>::New_ctor(8);
		tMP_FontAsset->ReadFontAssetDefinition();
		return tMP_FontAsset;
    }

    void loadFonts() {
        TMPro::TMP_FontAsset* defaultFontAsset = Resources::FindObjectsOfTypeAll<TMPro::TMP_FontAsset*>()->First([](TMPro::TMP_FontAsset* fontAsset){return fontAsset->get_name() == "Teko-Medium SDF";});
        fontAssets[""] = defaultFontAsset;

        for(auto& file : std::filesystem::recursive_directory_iterator(getAnyTextDir())) {
            if(file.path().extension() != ".ttf") continue;
            PaperLogger.info("Loading font from path: '{}'", file.path().string());
            
            std::string fontName = file.path().stem();
            if(fontAssets.contains(fontName) && fontAssets[fontName]) {PaperLogger.info("Already loaded font '{}'", fontName); continue;}

            Font* font = Font::New_ctor(file.path().string());
            if(!font) {PaperLogger.error("Failed to load font from path: '{}'", file.path().string()); continue;}
            font->set_name(fontName);
            
            TMPro::TMP_FontAsset* fontAsset = createFontAsset(font, 90, 5, 1024, 512);
            if(!fontAsset) {PaperLogger.error("Failed to load font asset for path: '{}'", file.path().string()); continue;}
            
            fontAssets[fontName] = fontAsset;
        }

        // TODO Load font names that are requested from configs
    }

}