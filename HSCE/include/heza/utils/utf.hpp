#pragma once
#include <string>

namespace heza::utils {

    inline std::string u32_to_utf8(const std::u32string& s) {
        std::string result;
        result.reserve(s.size());
        for (char32_t c : s) {
            if (c < 0x80) {
                result += static_cast<char>(c);
            } else if (c < 0x800) {
                result += static_cast<char>(0xC0 | (c >> 6));
                result += static_cast<char>(0x80 | (c & 0x3F));
            } else if (c < 0x10000) {
                result += static_cast<char>(0xE0 | (c >> 12));
                result += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (c & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (c >> 18));
                result += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (c & 0x3F));
            }
        }
        return result;
    }

    inline std::u32string utf8_to_u32(const std::string& s) {
        std::u32string result;
        for (size_t i = 0; i < s.size(); ) {
            unsigned char c = s[i];
            char32_t cp;
            int extra;
            if      (c < 0x80) { cp = c;         extra = 0; }
            else if (c < 0xE0) { cp = c & 0x1F;  extra = 1; }
            else if (c < 0xF0) { cp = c & 0x0F;  extra = 2; }
            else               { cp = c & 0x07;  extra = 3; }
            ++i;
            for (int k = 0; k < extra; ++k) {
                cp = (cp << 6) | (s[i] & 0x3F);
                ++i;
            }
            result += cp;
        }
        return result;
    }

}