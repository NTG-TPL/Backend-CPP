#include <catch2/catch_test_macros.hpp>

#include "../src/app/use_cases_impl.h"
#include "../src/domain/author.h"
#include "../src/domain/book.h"

namespace {

class MockAuthorRepository : public domain::AuthorRepository {
public:
    void Save(const domain::Author& author) override { saved_authors.push_back(author);}
    domain::Authors GetAuthors() override { return saved_authors; }
private:
    domain::Authors saved_authors;
};

class MockBookRepository : public domain::BookRepository {
public:
    void Save(const domain::Book& book) override { saved_books.emplace_back(book); }
    domain::Books GetAuthorBooks(const domain::AuthorId& author_id) override { return saved_books; }
    domain::Books GetBooks() override { return saved_books; }
private:
    std::vector<domain::Book> saved_books;
};

struct Fixture {
    MockAuthorRepository authors;
    MockBookRepository books;
};

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        app::UseCasesImpl use_cases{authors, books};
        WHEN("Adding an author") {
            const auto author_name = "Joanne Rowling";
            use_cases.AddAuthor(author_name);

            THEN("author with the specified name is saved to repository") {
                auto saved_authors = authors.GetAuthors();
                REQUIRE(saved_authors.size() == 1);
                CHECK(saved_authors.at(0).GetName() == author_name);
                CHECK(saved_authors.at(0).GetId() != domain::AuthorId{});
            }
        }
    }
}