#pragma once

#include <vector>
#include <string>

namespace ui::detail {

    struct AddBookParams {
        std::string title;
        std::string author_id;
        int publication_year = 0;
    };

    struct AuthorInfo {
        std::string id;
        std::string name;
    };

    struct BookInfo {
        std::string title;
        int publication_year;
    };

    using AuthorsInfo = std::vector<detail::AuthorInfo>;
    using BooksInfo = std::vector<detail::BookInfo>;

}  // namespace detail