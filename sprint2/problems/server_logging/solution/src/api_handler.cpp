#include "api_handler.h"

namespace http_handler {

    const std::unordered_map<std::string_view, std::string_view> ContentType::EXTENSION = {
            {FileExtension::HTML   , ContentType::TEXT_HTML       },
            {FileExtension::HTM    , ContentType::TEXT_HTML       },
            {FileExtension::CSS    , ContentType::TEXT_CSS        },
            {FileExtension::TXT    , ContentType::TEXT_PLAIN      },
            {FileExtension::JS     , ContentType::TEXT_JS         },
            {FileExtension::JSON   , ContentType::APPLICATION_JSON},
            {FileExtension::XML    , ContentType::APPLICATION_XML },
            {FileExtension::PNG    , ContentType::IMAGE_PNG       },
            {FileExtension::JPG    , ContentType::IMAGE_JPEG      },
            {FileExtension::JPE    , ContentType::IMAGE_JPEG      },
            {FileExtension::JPEG   , ContentType::IMAGE_JPEG      },
            {FileExtension::GIF    , ContentType::IMAGE_GIF       },
            {FileExtension::BMP    , ContentType::IMAGE_BMP       },
            {FileExtension::ICO    , ContentType::IMAGE_ICO       },
            {FileExtension::TIFF   , ContentType::IMAGE_TIFF      },
            {FileExtension::TIF    , ContentType::IMAGE_TIFF      },
            {FileExtension::SVG    , ContentType::IMAGE_SVG       },
            {FileExtension::SVGZ   , ContentType::IMAGE_SVG       },
            {FileExtension::MP3    , ContentType::AUDIO_MP3       },
            {FileExtension::UNKNOWN, ContentType::UNKNOWN         }
    };

    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive, std::string_view content_type){
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        return response;
    }

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

    bool ApiHandler::IsAPIRequest(const StringRequest& req) {
        return req.target().starts_with(EndPoint::API);
    }

}