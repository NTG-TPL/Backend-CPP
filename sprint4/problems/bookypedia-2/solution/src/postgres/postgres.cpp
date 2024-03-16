#include "postgres.h"

#include <pqxx/zview.hxx>
#include <pqxx/pqxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    work_.exec_prepared(Prepared::INSERT_AUTHOR,
                        work_.esc(author.GetId().ToString()),
                        work_.esc(author.GetName()));
}

domain::Authors AuthorRepositoryImpl::GetAuthors() const {
    return ConvertResponseToAutors(work_.query<std::string, std::string>(QueryTexts::authors()));
}

[[nodiscard]] domain::Author AuthorRepositoryImpl::GetAuthorById(const domain::AuthorId& id) const {
    const auto& [_, name] = work_.query1<std::string, std::string>(QueryTexts::author_by_id(work_.quote(id.ToString())));
    return domain::Author {id, name};
}

[[nodiscard]] std::optional<domain::Author> AuthorRepositoryImpl::GetAuthorByName(const std::string& name) const {
    const auto& data = work_.query01<std::string, std::string>(QueryTexts::author_by_name(work_.quote(name)));
    if (!data.has_value()) {
        return std::nullopt;
    }
    const auto& [id, author_name] = *data;
    return domain::Author {domain::AuthorId::FromString(id), author_name};
}
void AuthorRepositoryImpl::DeleteAuthor(const domain::AuthorId& author_id) {
    std::string id = work_.esc(author_id.ToString());
    for (auto &[book_id] : work_.query<std::string>(QueryTexts::books_ids(work_.quote(id)))) {
        work_.exec_prepared(Prepared::Prepared::DELETE_TAG_BY_BOOK_ID, work_.esc(book_id));
    }
    work_.exec_prepared(Prepared::DELETE_BOOKS_BY_AUTHOR_ID, id);
    work_.exec_prepared(Prepared::DELETE_AUTHOR, id);
    work_.commit();
}

void AuthorRepositoryImpl::EditAuthor(const domain::Author& author) {
    auto result = work_.exec_prepared(Prepared::EDIT_AUTHOR, work_.esc(author.GetName()), work_.esc(author.GetId().ToString()));

}

void BookRepositoryImpl::Save(const domain::Book& book) {
    work_.exec_prepared(Prepared::INSERT_BOOK,
           work_.esc(book.GetId().ToString()),
           work_.esc(book.GetAuthorId().ToString()),
           work_.esc(book.GetTitle()),
           book.GetYear());
}

void BookRepositoryImpl::Edit(const ui::detail::BookInfo &book_info) {
    auto id = work_.esc(book_info.id);
    work_.exec_prepared(Prepared::EDIT_BOOK, work_.esc(book_info.title), book_info.publication_year, id);
    work_.exec_prepared(Prepared::DELETE_TAG, id);
    for (const auto& tag : book_info.tags) {
        work_.exec_prepared(Prepared::INSERT_TAG, id, work_.esc(tag));
    }
}

domain::Books BookRepositoryImpl::GetAuthorBooks(const domain::AuthorId& author_id) {
    const auto& query = QueryTexts::books_by_author(work_.quote(author_id.ToString()));
    return ConvertResponseToBooks(work_.query<std::string, std::string, std::string, int>(query));
}

domain::Books BookRepositoryImpl::GetBooks() {
    return ConvertResponseToBooks(work_.query<std::string, std::string, std::string, int>(QueryTexts::books()));
}

void BookRepositoryImpl::DeleteBook(const domain::BookId& book_id) {
    auto id = work_.esc(book_id.ToString());
    work_.exec_prepared(Prepared::DELETE_TAG_BY_BOOK_ID, id);
    work_.exec_prepared(Prepared::DELETE_BOOK, id);
}

ui::detail::Tags BookRepositoryImpl::GetBookTags(const std::string& id) const {
    ui::detail::Tags tags;
    for (const auto& [tag] :work_.query<std::string>(QueryTexts::get_tags_by_book_id(work_.quote(id)))) {
        tags.insert(tag);
    }
    return tags;
}

void BookTagRepositoryImpl::Save(const domain::BookId& book_id, const ui::detail::Tags& tags) {
    for (auto& tag:tags) {
        work_.exec_prepared(Prepared::INSERT_TAG, work_.esc(book_id.ToString()), work_.esc(tag));
    }
}

Database::Database(pqxx::connection connection) :
    connection_{std::move(connection)}, unit_factory_(connection_) {
    pqxx::work work{connection_};
    work.exec(QueryTexts::create_authors());
    work.exec(QueryTexts::create_books());
    work.exec(QueryTexts::create_tags());
    work.commit();

    InitPrepared();
}

void Database::InitPrepared(){
    InitBooksPrepared();
    InitAuthorsPrepared();
    InitTagsPrepared();
}

void Database::InitBooksPrepared() {
    using key = domain::BooksKeys;
    connection_.prepare(Prepared::INSERT_BOOK, "INSERT INTO " + key::BOOKS +
                                               " (" + key::ID + ", " + key::AUTHOR_ID + ", " +
                                               key::TITLE + ", " + key::YEAR + ") VALUES ($1, $2, $3, $4);");

    connection_.prepare(Prepared::EDIT_BOOK, "UPDATE "+key::BOOKS+" SET "+key::TITLE+
                                             " = $1, "+key::YEAR+" = $2 WHERE "+key::ID+" = $3");

    connection_.prepare(Prepared::DELETE_BOOK, "DELETE FROM "+key::BOOKS+" WHERE "+key::ID+" = $1");
    connection_.prepare(Prepared::DELETE_BOOKS_BY_AUTHOR_ID, "DELETE FROM "+key::BOOKS+" WHERE "+key::AUTHOR_ID+"=$1;");
}

void Database::InitAuthorsPrepared() {
    using key = domain::AuthorsKeys;
    connection_.prepare(Prepared::INSERT_AUTHOR,
                        "INSERT INTO " + key::AUTHORS +
                        " (" + key::ID +", "+ key::NAME +") " +
                        "VALUES ($1, $2) ON CONFLICT (" + key::ID  +") DO UPDATE SET "+ key::NAME +"=$2;");

    connection_.prepare(Prepared::DELETE_AUTHOR, "DELETE FROM "+ key::AUTHORS+" WHERE "+key::ID+" = $1");
    connection_.prepare(Prepared::EDIT_AUTHOR, "UPDATE "+ key::AUTHORS+" SET "+key::NAME+" = $1 WHERE "+key::ID+" = $2 RETURNING id;");
}

void Database::InitTagsPrepared() {
    using key = domain::TagsKeys;
    connection_.prepare(Prepared::INSERT_TAG, "INSERT INTO "+key::TAGS+" ("+key::ALL+") VALUES($1, $2);");
    connection_.prepare(Prepared::DELETE_TAG, "DELETE FROM "+key::TAGS+ " WHERE "+key::BOOK_ID+"= $1 ;");
    connection_.prepare(Prepared::DELETE_TAG_BY_BOOK_ID, "DELETE FROM "+key::TAGS+" WHERE "+key::BOOK_ID+"= $1 ;");
}

}  // namespace postgres