#pragma once

#include <vector>
#include <optional>
#include <string>

#include "author.h"
#include "../ui/detail.h"


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
    Book(BookId book_id, AuthorId author_id, std::string title, int year, ui::detail::Tags tags):
            id_(std::move(book_id)), author_id_(std::move(author_id)),
            title_(std::move(title)), year_(year), tags_(std::move(tags)) {
    }

    [[nodiscard]] const BookId& GetId() const noexcept { return id_; }
    [[nodiscard]] const AuthorId& GetAuthorId() const noexcept { return author_id_; }
    [[nodiscard]] const std::string& GetTitle() const noexcept { return title_; }
    [[nodiscard]] int GetYear() const noexcept { return year_; }
    [[nodiscard]] ui::detail::Tags GetTags() const noexcept { return tags_; }

private:
    BookId id_;
    AuthorId author_id_;
    std::string title_;
    int year_;
    ui::detail::Tags tags_;
};

using Books = std::vector<Book>;

class BookRepository {
public:
    virtual void Save(const Book& book) = 0;
    virtual void Edit(const ui::detail::BookInfo& book_info) = 0;
    virtual Books GetAuthorBooks(const AuthorId& author_id) = 0;
    virtual Books GetBooks() = 0;
    virtual void DeleteBook(const domain::BookId& book_id) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain