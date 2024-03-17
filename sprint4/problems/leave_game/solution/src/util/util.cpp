#include "util.h"

namespace util {
    std::string UrlDecode(const std::string& encoded) {
        std::string decoded;
        decoded.reserve(encoded.size());
        for (size_t i = 0; i < encoded.size(); ++i) {
            if (encoded[i] == '%' && i + 2 < encoded.size()) {
                const std::string r = encoded.substr(i + 1, 2);
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

    bool IsSubPath(fs::path path, fs::path base) {
        // Приводим оба пути к каноничному виду (без . и ..)
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);

        // Проверяем, что все компоненты base содержатся внутри path
        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }
}