#include "floatingLog.hpp"
#include "main.hpp"

#include "BSML/shared/bsml.hpp"

using namespace UnityEngine;

namespace AnyText::FloatingLog {

    std::vector<std::string> lines;
    UnityW<BSML::FloatingScreen> screen;
    UnityW<HMUI::CurvedTextMeshPro> text;

    constexpr int maxLines = 100;

    void AddLine(std::string newLine) {
        if(newLine.find("pls don't log me") != std::string::npos) return;

        static int lineNum = 0;
        lineNum++;
        lines.push_back("<color=green>" + std::to_string(lineNum) + "</color> <noparse>" + newLine + "</noparse>");

        if(lines.size() > maxLines) lines.erase(lines.begin() + 1);

        UpdateUI();
    }

    void UpdateUI() {
        if(!text) return;

        std::string finalText = "";
        for(auto line : lines) finalText += line + '\n';
        text->set_text(finalText);
    }

    void InitUI() {
        lines.clear();
        lines.push_back("pls don't log me");
        if(screen) return;

        screen = BSML::Lite::CreateFloatingScreen({100, 100}, {0, 1, 2}, {0, 0, 0}, 0, true, true, BSML::Side::Top);
        RectTransform* screenTransform = screen->get_rectTransform();
        GameObject::DontDestroyOnLoad(screen->get_gameObject());

        // GameObject* scrollView = BSML::Lite::CreateScrollableSettingsContainer(screenTransform);
        // Transform* scrollViewTransform = scrollView->get_transform();

        text = BSML::Lite::CreateText(screenTransform, "Test", 1.0, {0, 0}, {100, 100});
        text->set_verticalAlignment(TMPro::VerticalAlignmentOptions::Bottom);
    }

    void DestroyUI() {
        if(screen) Object::Destroy(screen->get_gameObject());
    }

}