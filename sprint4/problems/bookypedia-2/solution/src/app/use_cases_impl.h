#pragma once
#include "../domain/author_fwd.h"
#include "../domain/book_fwd.h"
#include "../domain/book_tags_fwd.h"
#include "use_cases.h"
#include "unit_of_work.h"

namespace app {

    class UseCasesImpl : public UseCases {
    public:
        explicit UseCasesImpl(UnitOfWorkFactory& unit_factory) :
                unit_factory_(unit_factory) {
        }

        void AddAuthor(const std::string& name) override;
        [[nodiscard]] domain::Authors GetAllAuthors() const override;
        [[nodiscard]] domain::Author GetAuthorById(const domain::AuthorId& id) const override;
        std::optional<domain::Author> GetAuthorByName(const std::string& name) override;
        void EditAuthor(const ui::detail::AuthorInfo& info) override;
        void DeleteAuthor(const domain::AuthorId& id) override;

        void AddBook(const ui::detail::AddBookParams& book_params) override;
        void EditBook(const ui::detail::BookInfo& book_params) override;
        [[nodiscard]] domain::Books GetAuthorBooks(const std::string& author_id) const override;
        [[nodiscard]] domain::Books GetAllBooks() const override;
        void DeleteBook(const domain::BookId& id) override;

    private:
        UnitOfWorkFactory& unit_factory_;

    };


}  // namespace app
