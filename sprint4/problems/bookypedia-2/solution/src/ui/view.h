#pragma once
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "../app/use_cases.h"

namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {

    namespace detail {
        void PrintAuthorBooks(std::ostream& out, const domain::Books& container);
    }

    using namespace std::literals;
struct TextResponse {
    TextResponse() = delete;
    static const inline std::string_view ENTER_TAGS1 = "Enter tags (comma separated):"sv; //  or empty line to cancel:
    static const inline std::string_view ENTER_AUTHOR1 = "Enter author name or empty line to select from list:"sv;
    static const inline std::string_view ENTER_AUTHOR2 = "Enter author # or empty line to cancel"sv;
    static const inline std::string_view ENTER_BOOK = "Enter the book # or empty line to cancel:"sv;
    static const inline std::string_view SELECT_AUTHOR = "Select author:"sv;
    static const inline auto AUTHOR_NOT_FOUND1 = [](const std::string& name){return "No author found. Do you want to add "s + name + " (y/n)?"s;};
    static const inline auto ENTER_TITLE = [](const std::string& default_title){return "Enter new title or empty line to use the current one ("s + default_title + "):"s;};
    static const inline auto ENTER_TAGS2 = [](const std::string& default_tags){return "Enter tags (current tags: "s + default_tags + "):"s;};
    static const inline auto ENTER_YEAR = [](int default_year){return "Enter publication year or empty line to use the current one ("s + std::to_string(default_year) + "):"s;};
    static const inline std::string_view ENTER_NAME = "Enter new name:"sv;
    static const inline std::string_view BOOK_NOT_FOUND = "Book not found"sv ;
    static const inline std::string_view FAILED_EDIT_AUTHOR = "Failed to edit author"sv;
    static const inline std::string_view FAILED_DELETE_AUTHOR = "Failed to delete author"sv ;
    static const inline std::string_view FAILED_EDIT_BOOK = "Failed to edit book"sv;
    static const inline std::string_view FAILED_DELETE_BOOK = "Failed to delete book"sv ;
    static const inline std::string_view FAILED_ADD_BOOK = "Failed to add book"sv;
    static const inline std::string_view FAILED_ADD_AUTHOR = "Failed to add author"sv;

};

struct TextError {
    TextError() = delete;
    static const inline std::string AUTHOR_NOT_FOUND2 = "Author not found"s;
    static const inline std::string NAME_IS_EMPTY = "Name cannot be empty"s;
    static const inline std::string AUTHOR_EMPTY = "Author name cannot be empty"s;
    static const inline std::string INVALID_YEAR = "Invalid publication year"s;
    static const inline std::string INVALID_BOOK_NUM = "Invalid book num"s;
    static const inline std::string INVALID_AUTHOR_NUM = "Invalid author num"s;
    static const inline std::string FAILED_SHOW_BOOK = "Failed to Show Book"s ;
    static const inline std::string FAILED_SHOW_BOOKS = "Failed to Show Books"s ;
};

class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    [[nodiscard]] std::optional<detail::BookInfo> SelectBook(const std::vector<detail::BookInfo>& books) const;
    std::optional<detail::BookInfo> ReadBookByTitle(std::istream& cmd_input) const;
    std::optional<std::string> ReadAuthor(std::istream& cmd_input) const;
    [[nodiscard]] std::optional<std::string> ReadAuthor() const;
    [[nodiscard]] std::string ReadName() const;
    [[nodiscard]] std::string ReadBookTitle(const std::string& default_title) const;
    [[nodiscard]] int ReadBookPublicationYear(int default_year) const;

    ui::detail::BooksInfo ConvertBooksToBooksInfo(auto&& callback) const {
        ui::detail::BooksInfo books_info;
        auto books = callback();
        books_info.reserve(books.size());
        for (const auto& book : books) {
            const auto& author = use_cases_.GetAuthorById(book.GetAuthorId());
            books_info.emplace_back(book.GetId().ToString(), book.GetTitle(),
                                    author.GetName(), book.GetYear(), book.GetTags());
        }
        return books_info;
    }

    ui::detail::AuthorsInfo ConvertAuthorsToAuthorsInfo(auto&& callback) const {
        ui::detail::AuthorsInfo authors_info;
        for (auto& author : callback()) {
            authors_info.emplace_back(author.GetId().ToString(), author.GetName());
        }
        return authors_info;
    }

    bool AddAuthor(std::istream& cmd_input) const;
    bool EditAuthor(std::istream& cmd_input) const;
    bool DeleteAuthor(std::istream& cmd_input) const;
    bool AddBook(std::istream& cmd_input) const;
    bool EditBook(std::istream& cmd_input) const;
    bool DeleteBook(std::istream& cmd_input) const;
    [[nodiscard]] bool ShowAuthors() const;
    [[nodiscard]] bool ShowBooks() const;
    [[nodiscard]] bool ShowBook(std::istream& cmd_input) const;
    [[nodiscard]] bool ShowAuthorBooks() const;

    std::optional<detail::AddBookParams> GetBookParams(std::istream& cmd_input) const;
    [[nodiscard]] std::optional<std::string> SelectAuthor() const;
    [[nodiscard]] ui::detail::AuthorsInfo GetAuthors() const;
    [[nodiscard]] ui::detail::BooksInfo GetBooks() const;
    [[nodiscard]] ui::detail::Tags ReadTags(std::string_view message = TextResponse::ENTER_TAGS1) const;
    [[nodiscard]] ui::detail::Tags ReadBookTags(const ui::detail::Tags& default_tags) const;

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui