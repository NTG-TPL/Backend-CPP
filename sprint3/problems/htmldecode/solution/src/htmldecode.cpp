#include "htmldecode.h"

std::string HtmlDecode(std::string_view str) {
    std::regex  lt("&(lt|LT);?"),
                gt("&(gt|GT);?"),
                amp("&(amp|AMP);?") ,
                apos("&(apos|APOS);?"),
                qout("&(quot|QUOT);?");

    std::string ret(str);

    ret = std::regex_replace(ret, lt, "<");
    ret = std::regex_replace(ret, gt, ">");
    ret = std::regex_replace(ret, amp, "&");

    ret = std::regex_replace(ret, apos, "\'");
    ret = std::regex_replace(ret, qout, "\"");

    return ret;
}
