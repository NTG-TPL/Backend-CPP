#pragma once

#include <vector>
#include <string>
#include <set>

namespace ui::detail {

    using Tags = std::set<std::string>;

    struct AddBookParams {
        std::string title;
        std::string author_id;
        int publication_year = 0;
        Tags tags;
    };

    struct AuthorInfo {
        std::string id;
        std::string name;
    };

    struct BookInfo {
        std::string id;
        std::string title;
        std::string author_name;
        int publication_year;
        Tags tags;
    };

    struct BookTagsInfo {
        std::string book_id;
        Tags tags;
    };

    using AuthorsInfo = std::vector<detail::AuthorInfo>;
    using BooksInfo = std::vector<detail::BookInfo>;

}  // namespace detail