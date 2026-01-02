#include "textManager.hpp"
#include "main.hpp"

#include "findReplaceEntry.hpp"
#include "configs.hpp"

#include "boost/regex.hpp"

#include "UnityEngine/Font.hpp"
#include "UnityEngine/TextCore/LowLevel/GlyphRenderMode.hpp"
#include "TMPro/TMP_Text.hpp"
#include "TMPro/TMP_FontAsset.hpp"
#include "TMPro/AtlasPopulationMode.hpp"
#include "TMPro/ShaderUtilities.hpp"
#include "UnityEngine/TextCore/LowLevel/FontEngine.hpp"
#include "UnityEngine/TextCore/LowLevel/FontEngineError.hpp"
#include "UnityEngine/TextCore/GlyphRect.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Shader.hpp"

using namespace UnityEngine;
using namespace UnityEngine::TextCore::LowLevel;

DEFINE_TYPE(AnyText, TextManager);

/*

&fontAsset: 0x79e7d39d20
fontAsset name: ''
fontAsset &material: 0x0
&fontAsset: 0x7af4123a80
fontAsset name: 'Teko-Medium SDF'
fontAsset &material: 0x7d5a5e26a0
fontAsset shader name: 'TextMeshPro/Mobile/Distance Field Zero Alpha Write'
&fontAsset: 0x7d637187e0
fontAsset name: 'SourceHanSansCN-Bold-SDF-Uncommon(2k)'
fontAsset &material: 0x7af40b1360
fontAsset shader name: 'TextMeshPro/Distance Field'
&fontAsset: 0x7d637182a0
fontAsset name: 'NotoSansJP-Medium SDF'
fontAsset &material: 0x7af40b1320
fontAsset shader name: 'TextMeshPro/Distance Field'
&fontAsset: 0x7d63718000
fontAsset name: 'SourceHanSansCN-Bold-SDF-Common-1(2k)'
fontAsset &material: 0x7af40b12e0
fontAsset shader name: 'TextMeshPro/Distance Field'
&fontAsset: 0x7d4ba0a7e0
fontAsset name: 'Teko-Medium SDF Numbers Monospaced Curved'
fontAsset &material: 0x7d4b843360
fontAsset shader name: 'TextMeshPro/Mobile/Distance Field Zero Alpha Write'
&fontAsset: 0x7af41237e0
fontAsset name: 'NotoSansKR-Medium SDF'
fontAsset &material: 0x7d5a5e2660
fontAsset shader name: 'TextMeshPro/Distance Field'
&fontAsset: 0x7af4123d20
fontAsset name: 'SourceHanSansCN-Bold-SDF-Common-2(2k)'
fontAsset &material: 0x7d5a5e2620
fontAsset shader name: 'TextMeshPro/Distance Field'

*/

namespace AnyText {

    // DEBUG
    static Font* testFont = nullptr;
    static TMPro::TMP_FontAsset* testFontAsset = nullptr;

    // DEBUG
    TMPro::TMP_FontAsset* createFontAsset(Font *font, int samplingPointSize, int atlasPadding, int atlasWidth, int atlasHeight, TMPro::AtlasPopulationMode atlasPopulationMode, bool enableMultiAtlasSupport) {
        GlyphRenderMode renderMode = GlyphRenderMode::SDFAA; // Beat Saber only has signed distance field shaders

        FontEngine::InitializeFontEngine();
		if(FontEngine::LoadFontFace(font, samplingPointSize) != FontEngineError::Success) {
			PaperLogger.warn("Unable to load font face for [{}]. Very sad.", font->get_name());
			return nullptr;
		}
		TMPro::TMP_FontAsset* tMP_FontAsset = ScriptableObject::CreateInstance<TMPro::TMP_FontAsset*>();
		tMP_FontAsset->m_Version = "1.1.0";
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
        if(!originalMaterial) originalMaterial =  Resources::FindObjectsOfTypeAll<TMPro::TMP_FontAsset*>()->First([](TMPro::TMP_FontAsset* test){return test->get_name() == "Teko-Medium SDF Numbers Monospaced Curved";})->material;
        Material* material = Material::New_ctor(originalMaterial);;
		
        // int num = (static_cast<int>(renderMode) & 16) != 16;
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

    void TextManager::Awake() {
        PaperLogger.debug("&TextManager: {}", static_cast<void*>(this));
        text = GetComponent<TMPro::TMP_Text*>();
        if(!text) {
            PaperLogger.error("Couldn't find TMP_Text component");
            Object::Destroy(this);
            return;
        }

        updateOriginalStateWithDifferences();
        generateReplacementState();
        applyState(replacementState);


        // DEBUG
        if(!testFont) {
            PaperLogger.debug("Attempting to load testFont");
            testFont = UnityEngine::Font::New_ctor("/sdcard/ModData/com.beatgames.beatsaber/Mods/AnyText/UnifrakturCook-Bold.ttf");
            if(!testFont) {PaperLogger.debug("Failed to load testFont"); return;}
            if(!testFontAsset) {
                PaperLogger.debug("Attempting to create testFontAsset");
                // testFontAsset = TMPro::TMP_FontAsset::CreateFontAsset(testFont, 90, 9, UnityEngine::TextCore::LowLevel::GlyphRenderMode::SDFAA, 1024, 1024, TMPro::AtlasPopulationMode::Dynamic, true);
                testFontAsset = createFontAsset(testFont, 90, 9, 1024, 1024, TMPro::AtlasPopulationMode::Dynamic, true);
                // BeatGames uses padding 5, 1024x512, Static, false
                PaperLogger.debug("CreateFontAsset, &testFontAsset: {}", static_cast<void*>(testFontAsset));
                // TMPro::TMP_FontAsset::CreateFontAsset()
                // static auto CreateFontAsset = il2cpp_utils::resolve_icall<TMPro::TMP_FontAsset*, Font*>("TMPro.TMP_FontAsset::CreateFontAsset");
                // testFontAsset = CreateFontAsset(testFont);

                // This does work
                // TextCore::LowLevel::FontEngine::InitializeFontEngine();
                // if (TextCore::LowLevel::FontEngine::LoadFontFace(testFont, 90) != TextCore::LowLevel::FontEngineError::Success) {
                //     PaperLogger.error("Failed to load font, ur done for");
                // }
            }
        }
        if(!testFont) {PaperLogger.error("testFont is not loaded"); return;}
        if(!testFontAsset) {PaperLogger.error("testFontAsset is not loaded"); return;}
        text->set_font(testFontAsset);
    }

    void TextManager::OnDestroy() {
        PaperLogger.debug("&TextManager: {}", static_cast<void*>(this));
        if(!text || !text->get_text()) return;

        updateOriginalStateWithDifferences();
        applyState(originalState);
    }

    void TextManager::OnTextChange() {
        PaperLogger.debug("&TextManager: {}", static_cast<void*>(this));
        if(!text || !text->get_text()) return;

        if(updateOriginalStateWithDifferences()) {
            generateReplacementState();
            applyState(replacementState);
        }
    }

    bool TextManager::updateOriginalStateWithDifferences() {
        if(!text || !text->get_text()) return false;

        bool didChange = false;
        if(text->get_text() != replacementState.text) { originalState.text = std::string(text->get_text()); didChange = true; }
        if(text->get_fontStyle() != replacementState.fontStyle) { originalState.fontStyle = text->get_fontStyle(); didChange = true; }
        if(text->get_richText() != replacementState.richText) { originalState.richText = text->get_richText(); didChange = true; }
        return didChange;
    }

    void TextManager::applyState(const TextState& state) {
        if(!text || !text->get_text()) return;

        text->set_text(state.text);
        text->set_fontStyle(state.fontStyle);
        text->set_richText(state.richText);
    }

    void TextManager::generateReplacementState() {
        if(!text || !text->get_text() || !text->get_transform()) return;

        replacementState = originalState;

        bool hasReplacedText = false;
        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(!entry.getFindRegexIsValid()) continue;
                if(hasReplacedText && !entry.accumulate) continue;
                if(!boost::regex_search(replacementState.text, entry.getFindRegex())) continue;
                
                // boost::regex_constants::format_first_only (argument for regex_match)

                // TODO Assuming partial replacement
                std::string newText = boost::regex_replace(replacementState.text, entry.getFindRegex(), entry.replaceString);
                PaperLogger.info("Replacing '{}' -> ({}) -> '{}'", replacementState.text, entry.getFindString(), newText);
                replacementState.text = newText;
                replacementState.richText = true;

                hasReplacedText = true;
            }
        }
    }

    const TextState& TextManager::getOriginalState() const {
        return originalState;
    }

}