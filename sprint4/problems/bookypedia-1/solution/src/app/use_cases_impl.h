#pragma once
#include "../domain/author_fwd.h"
#include "../domain/book_fwd.h"
#include "use_cases.h"

namespace app {

    class UseCasesImpl : public UseCases {
    public:
        explicit UseCasesImpl(domain::AuthorRepository& authors,
                              domain::BookRepository& book):
                              authors_{authors}, books_{book} {
        }

        void AddAuthor(const std::string& name) override;
        ui::detail::AuthorsInfo GetAuthors() override;
        void AddBook(ui::detail::AddBookParams& book_params) override;
        ui::detail::BooksInfo GetAuthorBooksInfo(const std::string& author_id) override;
        ui::detail::BooksInfo GetBooksInfo() override;

    private:
        domain::AuthorRepository& authors_;
        domain::BookRepository& books_;

        ui::detail::BooksInfo ConvertBooksToBooksInfo(auto&& callback) {
            ui::detail::BooksInfo books_info;
            for (const auto& book : callback()) {
                books_info.emplace_back(book.GetTitle(), book.GetYear().value());
            }
            return books_info;
        }
    };


}  // namespace app
