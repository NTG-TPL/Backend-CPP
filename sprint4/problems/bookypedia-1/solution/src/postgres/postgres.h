#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include <string>

#include "../domain/author.h"
#include "../domain/book.h"

namespace postgres {

struct Prepared {
    Prepared() = delete;
    static const inline std::string INSERT_AUTHOR = "insert_author";
    static const inline std::string INSERT_BOOK = "insert_book";
};

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::connection& connection):
            connection_{connection} {
        using key = domain::AuthorsKeys;
        connection_.prepare(Prepared::INSERT_AUTHOR, "INSERT INTO " + key::AUTHORS +
                            " (" + key::ID +", "+ key::NAME +") " +
                            "VALUES ($1, $2) ON CONFLICT (" + key::ID  +") DO UPDATE SET "+ key::NAME +"=$2;");
    }

    void Save(const domain::Author& author) override;
    domain::Authors GetAuthors() override;
private:

    struct QueryTexts {
        QueryTexts() = delete;
        static inline auto authors = [](){
            using key = domain::AuthorsKeys;
            return "SELECT "+ key::ID + ", "+ key::NAME +" FROM "+
                    key::AUTHORS+" ORDER BY "+key::NAME+" ASC;";
        };
    };

    domain::Authors ConvertResponseToAutors(auto&& response) {
        domain::Authors authors;
        for (const auto& [id, name] : response) {
            authors.emplace_back(domain::AuthorId::FromString(id), name);
        }
        return authors;
    }

private:
    pqxx::connection& connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:

    explicit BookRepositoryImpl(pqxx::connection& connection)
            : connection_{connection} {
        using key = domain::BooksKeys;
        connection_.prepare(Prepared::INSERT_BOOK, "INSERT INTO " + key::BOOKS +
                                              " (" + key::ID + ", " + key::AUTHOR_ID + ", " +
                                              key::TITLE + ", " + key::YEAR + ") VALUES ($1, $2, $3, $4);");
    }

    void Save(const domain::Book& book) override;
    domain::Books GetAuthorBooks(const domain::AuthorId& author_id) override;
    domain::Books GetBooks() override;

private:

    struct QueryTexts {
        QueryTexts() = delete;
        static inline auto books_by_author = [](const std::string& author_id){
            using key = domain::BooksKeys;
            return "SELECT " + key::ALL + " FROM " + domain::BooksKeys::BOOKS +
                   " WHERE " + key::AUTHOR_ID + "=" + author_id +
                   "ORDER BY " + key::YEAR + " ASC, " + domain::BooksKeys::TITLE + " ASC;";
        };
        static inline auto books =[](){
            using key = domain::BooksKeys;
            return "SELECT " + key::ALL +
            " FROM "+ key::BOOKS + " ORDER BY "+key::TITLE+" ASC;";
        };
    };

    domain::Books ConvertResponseToBooks(auto response) {
        domain::Books books;
        for (const auto& [id, author_id, title, year] : response) {
            books.emplace_back(domain::BookId::FromString(*id), domain::AuthorId::FromString(*author_id), *title, year);
        }
        return books;
    }
private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection) :
        connection_{std::move(InitDB(connection))}, authors_(connection_),
        books_(connection_) {
    }

    AuthorRepositoryImpl& GetAuthors() & { return authors_; }
    BookRepositoryImpl& GetBooks() & { return books_; }

private:
    struct QueryTexts {
        QueryTexts() = delete;
        static inline auto create_authors =[](){
            using key = domain::AuthorsKeys;
            return "CREATE TABLE IF NOT EXISTS "+key::AUTHORS+" ("+key::ID+" UUID CONSTRAINT author_id_constraint PRIMARY KEY,"
                                                                  +key::NAME+" varchar(100) UNIQUE NOT NULL);";
        };
        static inline auto create_books =[](){
            using key = domain::BooksKeys;
            return " CREATE TABLE IF NOT EXISTS "+ key::BOOKS +" (" + key::ID + " UUID CONSTRAINT book_id_constraint PRIMARY KEY,"
                   + key::AUTHOR_ID + " UUID REFERENCES " + domain::AuthorsKeys::AUTHORS + " (id) NOT NULL,"
                   + key::TITLE + " varchar(100) NOT NULL,"
                   + key::YEAR + " integer);";
        };
    };

    static pqxx::connection&& InitDB(pqxx::connection& connection){
        pqxx::work work{connection};
        work.exec(QueryTexts::create_authors());
        work.exec(QueryTexts::create_books());
        work.commit();
        return std::move(connection);
    }


private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_;
    BookRepositoryImpl books_;
};

}  // namespace postgres