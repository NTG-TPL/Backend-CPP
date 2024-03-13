#pragma once

#include <vector>
#include <optional>
#include <string>

#include "../util/tagged_uuid.h"
#include "author.h"


namespace domain {

namespace detail {
    struct BookTag {};
}  // namespace detail

using BookId = util::TaggedUUID<detail::BookTag>;

struct BooksKeys {
    static inline std::string BOOKS     = "books";
    static inline std::string ID        = "id";
    static inline std::string AUTHOR_ID = "author_id";
    static inline std::string TITLE     = "title";
    static inline std::string YEAR      = "publication_year";
    static inline std::string ALL       = "id, author_id, title, publication_year";
};

class Book {
public:
    Book(BookId book_id, AuthorId author_id, std::string title, std::optional<int> year):
            id_(std::move(book_id)), author_id_(std::move(author_id)),
            title_(std::move(title)), year_(year) {
    }

    [[nodiscard]] const BookId& GetId() const noexcept { return id_; }
    [[nodiscard]] const AuthorId& GetAuthorId() const noexcept { return author_id_; }
    [[nodiscard]] const std::string& GetTitle() const noexcept { return title_; }
    [[nodiscard]] std::optional<int> GetYear() const noexcept { return year_; }

private:
    BookId id_;
    AuthorId author_id_;
    std::string title_;
    std::optional<int> year_;
};

using Books = std::vector<Book>;

class BookRepository {
public:
    virtual void Save(const Book& book) = 0;
    virtual Books GetAuthorBooks(const AuthorId& author_id) = 0;
    virtual Books GetBooks() = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain