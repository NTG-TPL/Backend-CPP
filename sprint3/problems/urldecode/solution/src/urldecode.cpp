#include "urldecode.h"

#include <charconv>
#include <stdexcept>

std::string UrlDecode(std::string_view encoded) {
    std::string decoded;
    decoded.reserve(encoded.size());
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '%' && i + 2 < encoded.size()) {
            const std::string r = std::string{encoded.substr(i + 1, 2)};
            decoded.push_back(static_cast<char>(std::stoul(r, nullptr, 16)));
            i += 2;
            continue;
        } else if (encoded[i] == '+') {
            decoded.push_back(' ');
        } else {
            decoded.push_back(encoded[i]);
        }
    }
    return decoded;
}
