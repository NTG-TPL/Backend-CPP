#ifndef GAME_SERVER_UTIL_H
#define GAME_SERVER_UTIL_H
#include <filesystem>

namespace util {
    namespace fs = std::filesystem;
    // Декодирование url
    std::string UrlDecode(const std::string& encoded);

    // Возвращает true, если каталог path содержится внутри base.
    bool IsSubPath(fs::path path, fs::path base);

}
#endif //GAME_SERVER_UTIL_H
