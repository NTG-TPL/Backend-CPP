#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

    void AuthorRepositoryImpl::Save(const domain::Author& author) {
        pqxx::work work{connection_};
        work.exec_prepared(Prepared::INSERT_AUTHOR, work.esc(author.GetId().ToString()), work.esc(author.GetName()));
        work.commit();
    }

    domain::Authors AuthorRepositoryImpl::GetAuthors() {
        pqxx::read_transaction r(connection_);
        return ConvertResponseToAutors(r.query<std::string, std::string>(QueryTexts::authors()));
    }

    void BookRepositoryImpl::Save(const domain::Book& book) {
        pqxx::work work{connection_};
        work.exec_prepared(Prepared::INSERT_BOOK,
               work.esc(book.GetId().ToString()),
               work.esc(book.GetAuthorId().ToString()),
               work.esc(book.GetTitle()),
               book.GetYear());
        work.commit();
    }

    domain::Books BookRepositoryImpl::GetAuthorBooks(const domain::AuthorId& author_id) {
        using opt_string = std::optional<std::string>;
        using opt_int = std::optional<int>;
        pqxx::read_transaction r{connection_};
        const auto& query =  QueryTexts::books_by_author(r.quote(author_id.ToString()));
        return ConvertResponseToBooks(r.query<opt_string, opt_string, opt_string, opt_int>(query));
    }

    domain::Books BookRepositoryImpl::GetBooks() {
        using opt_str = std::optional<std::string>;
        using opt_int = std::optional<int>;
        pqxx::read_transaction r{connection_};
        return ConvertResponseToBooks(r.query<opt_str, opt_str, opt_str, opt_int>(QueryTexts::books()));
    }

}  // namespace postgres