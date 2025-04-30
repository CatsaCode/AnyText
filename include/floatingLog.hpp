#pragma once

#include <vector>
#include <string>

namespace AnyText::FloatingLog {

    extern std::vector<std::string> lines;

    void AddLine(std::string newLine);
    void UpdateUI();
    void InitUI();
    void DestroyUI();

}