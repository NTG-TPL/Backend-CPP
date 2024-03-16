#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include <string>

#include "../domain/author.h"
#include "../domain/book.h"
#include "../domain/book_tags.h"
#include "../app/unit_of_work.h"

namespace postgres {

struct Prepared {
    Prepared() = delete;
    static const inline std::string INSERT_AUTHOR = "insert_author";
    static const inline std::string DELETE_AUTHOR = "delete_author";
    static const inline std::string EDIT_AUTHOR = "edit_author";

    static const inline std::string INSERT_BOOK = "insert_book";
    static const inline std::string EDIT_BOOK = "edit_book";
    static const inline std::string DELETE_BOOK = "delete_book";
    static const inline std::string DELETE_BOOKS_BY_AUTHOR_ID = "delete_book_by_author_id";

    static const inline std::string INSERT_TAG = "insert_tag";
    static const inline std::string DELETE_TAG = "delete_tag";
    static const inline std::string DELETE_TAG_BY_BOOK_ID = "delete_tag_by_id";
};

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::work& work) : work_(work) {
    }

    void Save(const domain::Author& author) override;
    [[nodiscard]] domain::Authors GetAuthors() const override;
    [[nodiscard]] domain::Author GetAuthorById(const domain::AuthorId &id) const override;
    [[nodiscard]] std::optional<domain::Author> GetAuthorByName(const std::string& name) const override;
    void DeleteAuthor(const domain::AuthorId& author_id) override;
    void EditAuthor(const domain::Author& author) override;

private:
    struct QueryTexts {
        QueryTexts() = delete;
        static const inline auto authors = [](){
            using key = domain::AuthorsKeys;
            return "SELECT "+ key::ID + ", "+ key::NAME +" FROM "+
                    key::AUTHORS+" ORDER BY "+key::NAME+" ASC;";
        };
        static const inline auto author_by_id = [](const std::string& id){
            using key = domain::AuthorsKeys;
            return "SELECT "+ key::ID + ", "+ key::NAME +" FROM "+
                    key::AUTHORS+" WHERE id = " + id + ";";
        };
        static const inline auto author_by_name = [](const std::string& name){
            using key = domain::AuthorsKeys;
            return "SELECT "+ key::ID + ", "+ key::NAME +" FROM "+
                    key::AUTHORS+" WHERE name = " + name+ ";";
        };
        static const inline auto books_ids = [](const std::string& id){
            using key = domain::BooksKeys;
            return "SELECT id FROM "+key::BOOKS+" WHERE "+key::AUTHOR_ID+"=" + id + ";";
        };
    };

    domain::Authors ConvertResponseToAutors(auto&& response) const {
        domain::Authors authors;
        for (const auto& [id, name] : response) {
            authors.emplace_back(domain::AuthorId::FromString(id), name);
        }
        return authors;
    }
private:
    pqxx::work& work_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::work& work): work_(work) {}

    void Save(const domain::Book& book) override;
    void Edit(const ui::detail::BookInfo& book_info) override;
    domain::Books GetAuthorBooks(const domain::AuthorId& author_id) override;
    domain::Books GetBooks() override;
    void DeleteBook(const domain::BookId& book_id) override;

private:
    struct QueryTexts {
        QueryTexts() = delete;
        static const inline auto books_by_author = [](const std::string& author_id){
            using key = domain::BooksKeys;
            return "SELECT " + key::ALL + " FROM " + domain::BooksKeys::BOOKS +
                   " WHERE " + key::AUTHOR_ID + "=" + author_id +
                   "ORDER BY " + key::YEAR + " ASC, " + domain::BooksKeys::TITLE + " ASC;";
        };
        static const inline auto books =[](){
            using key = domain::BooksKeys;
            return "SELECT " + key::ALL +
            " FROM "+ key::BOOKS + " ORDER BY "+key::TITLE+" ASC;";
        };
        static const inline auto get_tags_by_book_id =[](const std::string& id){
            using key = domain::TagsKeys;
            return "SELECT tag FROM "+key::TAGS+" WHERE "+key::BOOK_ID+" = "+id+" ORDER BY "+key::TAG+" ASC;";
        };
    };

    [[nodiscard]] ui::detail::Tags GetBookTags(const std::string& id) const;

    domain::Books ConvertResponseToBooks(auto&& response) {
        domain::Books books;
        for (const auto& [id, author_id, title, year] : response) {
            books.emplace_back(domain::BookId::FromString(id),
                               domain::AuthorId::FromString(author_id),
                               title, year, GetBookTags(id));
        }
        return books;
    }
private:
    pqxx::work& work_;
};


class BookTagRepositoryImpl : public domain::BookTagRepository {
public:
    explicit BookTagRepositoryImpl(pqxx::work& work): work_(work) {}

    void Save(const domain::BookId& book_id, const ui::detail::Tags& tags) override;
private:
    pqxx::work& work_;
};

class UnitOfWorkImpl : public app::UnitOfWork {
public:
    explicit UnitOfWorkImpl(pqxx::connection& connection):
        work_(connection), authors_(work_), books_(work_), book_tags_(work_){
    }

    void Commit() override { work_.commit();}

    domain::AuthorRepository& Authors() override { return authors_;}
    domain::BookRepository& Books() override {return books_;}
    domain::BookTagRepository& Tags() override { return book_tags_; }

private:
    pqxx::work work_;
    AuthorRepositoryImpl authors_;
    BookRepositoryImpl books_;
    BookTagRepositoryImpl book_tags_;
};

class UnitOfWorkFactoryImpl : public app::UnitOfWorkFactory {
public:
    explicit UnitOfWorkFactoryImpl(pqxx::connection& connection): connection_(connection) {}

    app::UnitOfWorkHolder CreateUnitOfWork() override {
        return std::make_unique<UnitOfWorkImpl>(connection_);
    }
private:
    pqxx::connection& connection_;
};


class Database {
public:
    explicit Database(pqxx::connection connection);
    app::UnitOfWorkFactory& GetUnitOfWorkFactory() { return unit_factory_; }
private:
    void InitPrepared();
    void InitBooksPrepared();
    void InitAuthorsPrepared();
    void InitTagsPrepared();

    struct QueryTexts {
        QueryTexts() = delete;
        static const inline auto create_authors =[](){
            using key = domain::AuthorsKeys;
            return "CREATE TABLE IF NOT EXISTS "+key::AUTHORS+" ("+key::ID+" UUID CONSTRAINT author_id_constraint PRIMARY KEY,"
                                                                  +key::NAME+" varchar(100) UNIQUE NOT NULL);";
        };
        static const inline auto create_books =[](){
            using key = domain::BooksKeys;
            return "CREATE TABLE IF NOT EXISTS "+ key::BOOKS +" (" + key::ID + " UUID CONSTRAINT book_id_constraint PRIMARY KEY,"
                   + key::AUTHOR_ID + " UUID REFERENCES " + domain::AuthorsKeys::AUTHORS + " (id) NOT NULL,"
                   + key::TITLE + " varchar(100) NOT NULL,"
                   + key::YEAR + " integer NOT NULL);";
        };
        static const inline auto create_tags =[](){
            using key = domain::TagsKeys;
            return  "CREATE TABLE IF NOT EXISTS "+key::TAGS+" ("+
                    key::BOOK_ID+" UUID references "+domain::BooksKeys::BOOKS+"(id) NOT NULL, "+
                    key::TAG+" varchar(30) NOT NULL);";
        };
    };

private:
    pqxx::connection connection_;
    UnitOfWorkFactoryImpl unit_factory_ {connection_};
};

}  // namespace postgres