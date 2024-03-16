#pragma once

#include <string>
#include "../ui/detail.h"

#include "../domain/book.h"
#include "../domain/author.h"
#include "../domain/book_tags.h"

namespace app {
    class UseCases {
    public:
        virtual void AddAuthor(const std::string& name) = 0;
        [[nodiscard]] virtual domain::Authors GetAllAuthors() const = 0;
        [[nodiscard]] virtual domain::Author GetAuthorById(const domain::AuthorId& id) const = 0;
        virtual std::optional<domain::Author> GetAuthorByName(const std::string& name) = 0;
        virtual void EditAuthor(const ui::detail::AuthorInfo& info) = 0;
        virtual void DeleteAuthor(const domain::AuthorId& id) = 0;

        virtual void AddBook(const ui::detail::AddBookParams& book_params) = 0;
        virtual void EditBook(const ui::detail::BookInfo& book_params) = 0;
        [[nodiscard]] virtual domain::Books GetAuthorBooks(const std::string& author_id) const = 0;
        [[nodiscard]] virtual domain::Books GetAllBooks() const = 0;
        virtual void DeleteBook(const domain::BookId& id) = 0;

    protected:
        virtual ~UseCases() = default;
    };

}  // namespace app
