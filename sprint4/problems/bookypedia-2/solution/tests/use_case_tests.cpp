#include <catch2/catch_test_macros.hpp>

#include <unordered_map>

#include "../src/app/use_cases_impl.h"

namespace {

    struct MockAuthorRepository : domain::AuthorRepository {
        domain::Authors saved_authors;

        void Save(const domain::Author& author) override { saved_authors.emplace_back(author); }

        [[nodiscard]] domain::Author GetAuthorById(const domain::AuthorId& id) const override {
            return *std::find_if(saved_authors.begin(), saved_authors.end(), [&](const auto& author) {
                return author.GetId() == id;
            });;
        }

        [[nodiscard]] std::optional<domain::Author> GetAuthorByName(const std::string& name) const override {
            auto it = std::find_if(saved_authors.begin(), saved_authors.end(), [&](const auto& author) {
                return author.GetName() == name; }
            );

            if (it == saved_authors.end()) {
                return std::nullopt;
            }
            return *it;
        }

        void EditAuthor(const domain::Author& author) override {
            auto it = std::find_if(saved_authors.begin(), saved_authors.end(), [&](const auto& a) {
                return a.GetId() == author.GetId();
            });
            *it = domain::Author{it->GetId(), author.GetName()};
        }

        void DeleteAuthor(const domain::AuthorId& id) override {
            std::erase_if(saved_authors, [&](const auto& author) {
                return author.GetId() == id;
            });
        }

        [[nodiscard]] domain::Authors GetAuthors() const override {
            return saved_authors;
        }
    };

    struct MockBookRepository : domain::BookRepository {
        domain::Books saved_books;

        void Save(const domain::Book& book) override {
            saved_books.emplace_back(book);
        }

        void Edit(const ui::detail::BookInfo& book_info) override {
            auto it = std::find_if(saved_books.begin(), saved_books.end(),[&](const auto& book) {
                return book.GetId().ToString() == book_info.id;
            });
            *it = domain::Book(domain::BookId::FromString(book_info.id),
                               it->GetAuthorId(),
                               book_info.title,
                               book_info.publication_year,
                               book_info.tags);
        }

        void DeleteBook(const domain::BookId& book_id) override {
            std::erase_if(saved_books, [&](const auto& book) {
                return book.GetId() == book_id;
            });
        }

        domain::Books GetBooks() override {
            return saved_books;
        }

        domain::Books GetAuthorBooks(const domain::AuthorId& id) override {
            domain::Books books = saved_books;
            std::erase_if(books, [&](const auto& book) {
                return book.GetAuthorId() != id;
            });
            return books;
        }
    };

    struct MockBookTagRepository : domain::BookTagRepository {
         std::unordered_map<std::string, ui::detail::Tags> saved_tags;

        void Save(const domain::BookId& bookId, const ui::detail::Tags& tags) override {
            saved_tags.emplace(bookId.ToString(), tags);
        }
    };

    struct MockUnitOfWork : app::UnitOfWork {
        MockAuthorRepository& authors;
        MockBookRepository& books;
        MockBookTagRepository& tags;

        MockUnitOfWork(MockAuthorRepository& authors, MockBookRepository& books, MockBookTagRepository& tags) :
                authors(authors), books(books), tags(tags){}

        void Commit() override {}

        domain::AuthorRepository& Authors() override { return authors;}
        domain::BookRepository& Books() override { return books;}
        domain::BookTagRepository& Tags() override { return tags;}
    };

    struct MockUnitOfWorkFactory : app::UnitOfWorkFactory {
        MockAuthorRepository& authors;
        MockBookRepository& books;
        MockBookTagRepository& tags;

        MockUnitOfWorkFactory(MockAuthorRepository& authors, MockBookRepository& books, MockBookTagRepository& tags):
                              authors(authors), books(books), tags(tags){
        }

        app::UnitOfWorkHolder CreateUnitOfWork() override {
            return std::make_unique<MockUnitOfWork>(authors, books, tags);
        }
    };

    struct Fixture {
        MockAuthorRepository authors;
        MockBookRepository books;
        MockBookTagRepository tags;
        MockUnitOfWorkFactory unit_factory{authors, books, tags};
    };

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        app::UseCasesImpl use_cases {unit_factory};

        WHEN("Adding an author") {
            const auto author_name = "Joanne Rowling";
            use_cases.AddAuthor(author_name);

            THEN("author with the specified name is saved to repository") {
                REQUIRE(authors.saved_authors.size() == 1);
                CHECK(authors.saved_authors.at(0).GetName() == author_name);
                CHECK(
                        authors.saved_authors.at(0).GetId() != domain::AuthorId {}
                );
            }
        }
    }
}