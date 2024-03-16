#pragma once

#include "string"
#include "../util/tagged_uuid.h"
#include "../ui/detail.h"
#include "book.h"

namespace domain {

    namespace detail {
        struct Tag {
        };
    }  // namespace detail

    struct TagsKeys {
        static inline std::string TAGS = "book_tags";
        static inline std::string BOOK_ID = "book_id";
        static inline std::string TAG = "tag";
        static inline std::string ALL = "book_id, tag";
    };

    class BookTag {
    public:
        BookTag(BookId book_id, std::string tag) :
                book_id_(std::move(book_id)), tag_(std::move(tag)) {
        }

        [[nodiscard]] const BookId &GetBookId() const noexcept { return book_id_; }
        [[nodiscard]] const std::string &GetTagString() const noexcept { return tag_; }

    private:
        BookId book_id_;
        std::string tag_;
    };

    class BookTagRepository {
    public:
        virtual void Save(const BookId& bookId, const ui::detail::Tags& tags) = 0;
    protected:
        ~BookTagRepository() = default;
    };

} // namespace domain