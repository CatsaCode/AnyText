#pragma once

#include "metacore/shared/assets.hpp"

#define DECLARE_ASSET(name, binary)       \
    const IncludedAsset name {            \
        Externs::_binary_##binary##_start, \
        Externs::_binary_##binary##_end    \
    };

#define DECLARE_ASSET_NS(namespaze, name, binary) \
    namespace namespaze { DECLARE_ASSET(name, binary) }

namespace IncludedAssets {
    namespace Externs {
        extern "C" uint8_t _binary_add_png_start[];
        extern "C" uint8_t _binary_add_png_end[];
        extern "C" uint8_t _binary_delete_png_start[];
        extern "C" uint8_t _binary_delete_png_end[];
        extern "C" uint8_t _binary_down_png_start[];
        extern "C" uint8_t _binary_down_png_end[];
        extern "C" uint8_t _binary_edit_png_start[];
        extern "C" uint8_t _binary_edit_png_end[];
        extern "C" uint8_t _binary_keyboard_png_start[];
        extern "C" uint8_t _binary_keyboard_png_end[];
        extern "C" uint8_t _binary_settings_png_start[];
        extern "C" uint8_t _binary_settings_png_end[];
        extern "C" uint8_t _binary_up_png_start[];
        extern "C" uint8_t _binary_up_png_end[];
    }

    // add.png
    DECLARE_ASSET(add_png, add_png);
    // delete.png
    DECLARE_ASSET(delete_png, delete_png);
    // down.png
    DECLARE_ASSET(down_png, down_png);
    // edit.png
    DECLARE_ASSET(edit_png, edit_png);
    // keyboard.png
    DECLARE_ASSET(keyboard_png, keyboard_png);
    // settings.png
    DECLARE_ASSET(settings_png, settings_png);
    // up.png
    DECLARE_ASSET(up_png, up_png);
}
