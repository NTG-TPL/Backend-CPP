#pragma once
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "detail.h"

namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {

class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    bool AddAuthor(std::istream& cmd_input) const;
    bool AddBook(std::istream& cmd_input) const;
    [[nodiscard]] bool ShowAuthors() const;
    [[nodiscard]] bool ShowBooks() const;
    [[nodiscard]] bool ShowAuthorBooks() const;

    std::optional<detail::AddBookParams> GetBookParams(std::istream& cmd_input) const;
    [[nodiscard]] std::optional<std::string> SelectAuthor() const;
    [[nodiscard]] ui::detail::AuthorsInfo GetAuthors() const;
    [[nodiscard]] ui::detail::BooksInfo GetBooks() const;
    [[nodiscard]] ui::detail::BooksInfo GetAuthorBooks(const std::string& author_id) const;

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui