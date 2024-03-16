#include "use_cases_impl.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    auto unit = unit_factory_.CreateUnitOfWork();
    unit->Authors().Save({AuthorId::New(), name});
    unit->Commit();
}

domain::Authors UseCasesImpl::GetAllAuthors() const {
    return  unit_factory_.CreateUnitOfWork()->Authors().GetAuthors();
}

domain::Author UseCasesImpl::GetAuthorById(const domain::AuthorId& id) const {
    return unit_factory_.CreateUnitOfWork()->Authors().GetAuthorById(id);
}

std::optional<domain::Author> UseCasesImpl::GetAuthorByName(const std::string& name) {
    return unit_factory_.CreateUnitOfWork()->Authors().GetAuthorByName(name);
}

void UseCasesImpl::EditAuthor(const ui::detail::AuthorInfo& info) {
    auto work = unit_factory_.CreateUnitOfWork();
    work->Authors().EditAuthor({domain::AuthorId::FromString(info.id), info.name});
    work->Commit();
}
void UseCasesImpl::DeleteAuthor(const domain::AuthorId& id) {
    auto work = unit_factory_.CreateUnitOfWork();
    work->Authors().DeleteAuthor(id);
    work->Commit();
}

void UseCasesImpl::AddBook(const ui::detail::AddBookParams& book_params){
    auto unit = unit_factory_.CreateUnitOfWork();
    auto id = BookId::New();
    unit->Books().Save({id,
                        AuthorId::FromString(book_params.author_id),
                        book_params.title,
                        book_params.publication_year,
                        {}}); // tags не используется

    unit->Tags().Save(id, book_params.tags);
    unit->Commit();
}
void UseCasesImpl::EditBook(const ui::detail::BookInfo& book_params) {
    auto work = unit_factory_.CreateUnitOfWork();
    work->Books().Edit(book_params);
    work->Commit();
}

domain::Books UseCasesImpl::GetAuthorBooks(const std::string& author_id) const {
    return unit_factory_.CreateUnitOfWork()->Books().GetAuthorBooks(AuthorId::FromString(author_id));
}

domain::Books UseCasesImpl::GetAllBooks() const {
    return unit_factory_.CreateUnitOfWork()->Books().GetBooks();
}

void UseCasesImpl::DeleteBook(const domain::BookId& id) {
    auto work = unit_factory_.CreateUnitOfWork();
    work->Books().DeleteBook(id);
    work->Commit();
}

}  // namespace app
