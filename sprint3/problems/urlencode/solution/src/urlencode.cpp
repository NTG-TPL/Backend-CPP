#include "urlencode.h"

std::string UrlEncode(std::string_view str) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << static_cast<int>(c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}
