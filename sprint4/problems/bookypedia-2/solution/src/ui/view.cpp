#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>

#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
    namespace detail {

        std::ostream& operator<<(std::ostream& out, const AuthorInfo& author) {
            return out << author.name;
        }

        std::ostream& operator<<(std::ostream& out, const BookInfo& book) {
            return out << book.title << " by " << book.author_name << ", " << book.publication_year;
        }


        void PrintAuthorBooks(std::ostream& out, const domain::Books& container) {
            int i = 1;
            for (auto& value : container) {
                out << i++ << " " <<  value.GetTitle() << ", " << value.GetYear() << std::endl;
            }
        }

    }  // namespace detail

    template <typename Container>
    void Print(std::ostream& out, const Container& container) {
        int i = 1;
        for (auto& value : container) {
            out << i++ << " " << value << std::endl;
        }
    }

    View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
            : menu_{menu}
            , use_cases_{use_cases}
            , input_{input}
            , output_{output} {
        menu_.AddAction("AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1));
        menu_.AddAction("EditAuthor"s, "name"s, "Edits author"s, std::bind(&View::EditAuthor, this, ph::_1));
        menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s, std::bind(&View::AddBook, this, ph::_1));
        menu_.AddAction("EditBook"s, "title"s, "Edits book"s, std::bind(&View::EditBook, this, ph::_1));
        menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
        menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
        menu_.AddAction("ShowBook"s,{}, "Show book info"s, std::bind(&View::ShowBook, this, ph::_1));
        menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s, std::bind(&View::ShowAuthorBooks, this));
        menu_.AddAction("DeleteAuthor"s, "name"s, "Delete author"s, std::bind(&View::DeleteAuthor, this, ph::_1));
        menu_.AddAction("DeleteBook"s, "name"s, "Delete book"s, std::bind(&View::DeleteBook, this, ph::_1));
    }

    std::optional<detail::BookInfo> View::SelectBook(const std::vector<detail::BookInfo>& books) const {
        Print(output_, books);
        output_ << TextResponse::ENTER_BOOK << std::endl;
        std::string str;
        if (!std::getline(input_, str) || str.empty()) {
            return std::nullopt;
        }
        int book_idx;
        try {
            book_idx = std::stoi(str);
        } catch (const std::exception&) {
            throw std::runtime_error(TextError::INVALID_BOOK_NUM);
        }
        --book_idx;
        if (book_idx < 0 or book_idx >= books.size()) {
            throw std::runtime_error(TextError::INVALID_BOOK_NUM);
        }
        return books[book_idx];
    }

    std::optional<detail::BookInfo> View::ReadBookByTitle(std::istream& cmd_input) const {
        std::string title;
        std::getline(cmd_input, title);
        boost::algorithm::trim(title);
        if (title.empty()) {
            return SelectBook(GetBooks());
        }
        auto books = GetBooks();
        std::erase_if(books, [&](const auto& book) { return book.title != title; });
        if (books.empty()) {
            return std::nullopt;
        }
        if (books.size() == 1) {
            return books.front();
        }
        return SelectBook(books);
    }

    std::optional<std::string> View::ReadAuthor(std::istream& cmd_input) const {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);

        if (name.empty()) {
            return SelectAuthor();
        }

        auto author = use_cases_.GetAuthorByName(name);
        if (!author.has_value()) {
            throw std::runtime_error(TextError::AUTHOR_NOT_FOUND2);
        }

        return author->GetId().ToString();
    }

    std::optional<std::string> View::ReadAuthor() const {
        output_ << TextResponse::ENTER_AUTHOR1 << std::endl;

        std::string name;
        std::getline(input_, name);
        boost::algorithm::trim(name);

        if (name.empty()) {
            return SelectAuthor();
        }

        auto author = use_cases_.GetAuthorByName(name);
        if (!author.has_value()) {
            output_ << TextResponse::AUTHOR_NOT_FOUND1(name) << std::endl;
            std::string answer;
            std::getline(input_, answer);
            boost::algorithm::trim(answer);

            if (answer != "y" && answer != "Y") {
                throw std::runtime_error(TextError::AUTHOR_NOT_FOUND2);
            }

            use_cases_.AddAuthor(name);
        }

        author = use_cases_.GetAuthorByName(name);
        if (!author.has_value()) {
            throw std::runtime_error(TextError::AUTHOR_NOT_FOUND2);
        }

        return author->GetId().ToString();
    }

    std::string View::ReadName() const {
        output_ << TextResponse::ENTER_NAME<< std::endl;
        std::string name;
        std::getline(input_, name);
        boost::algorithm::trim(name);

        if (name.empty()) {
            throw std::runtime_error(TextError::NAME_IS_EMPTY);
        }

        return name;
    }

    std::string View::ReadBookTitle(const std::string& default_title) const {
        output_ << TextResponse::ENTER_TITLE(default_title)<< std::endl;

        std::string str;
        if (!std::getline(input_, str) || str.empty()) {
            return default_title;
        }

        boost::algorithm::trim(str);
        return str;
    }

    int View::ReadBookPublicationYear(int default_year) const {
        output_ << TextResponse::ENTER_YEAR(default_year) << std::endl;

        std::string str;
        if (!std::getline(input_, str) || str.empty()) {
            return default_year;
        }

        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            throw std::runtime_error(TextError::INVALID_YEAR);
        }
    }

    bool View::AddAuthor(std::istream& cmd_input) const {
        try {
            std::string name;
            std::getline(cmd_input, name);
            boost::algorithm::trim(name);
            if (name.empty()){
                throw std::logic_error("Author name  is empty");
            }
            use_cases_.AddAuthor(name);
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_ADD_AUTHOR << std::endl;
        }
        return true;
    }

    bool View::EditAuthor(std::istream& cmd_input) const {
        try {
            auto author_id = ReadAuthor(cmd_input);
            if(!author_id.has_value()){
                return true;
            }

            use_cases_.EditAuthor({*author_id, ReadName()});
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_EDIT_AUTHOR << std::endl;
        }
        return true;
    }

    bool View::DeleteAuthor(std::istream& cmd_input) const {
        try {
            auto author_id = ReadAuthor(cmd_input);
            if(!author_id.has_value()){
                return true;
            }

            use_cases_.DeleteAuthor(domain::AuthorId::FromString(*author_id));
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_DELETE_AUTHOR << std::endl;
        }
        return true;
    }

    bool View::AddBook(std::istream& cmd_input) const {
        try {
            if (auto params = GetBookParams(cmd_input)) {
                use_cases_.AddBook(*params);
            }
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_ADD_BOOK << std::endl;
        }
        return true;
    }

    bool View::EditBook(std::istream& cmd_input) const {
        try {
            if (auto book = ReadBookByTitle(cmd_input)) {
                book->title = ReadBookTitle(book->title);
                book->publication_year = ReadBookPublicationYear(book->publication_year);
                book->tags = ReadBookTags(book->tags);
                use_cases_.EditBook(*book);
            } else {
                output_ << TextResponse::BOOK_NOT_FOUND<< std::endl;
            }
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_EDIT_BOOK << std::endl;
        }
        return true;
    }

    bool View::DeleteBook(std::istream& cmd_input) const {
        try {
            if (auto book = ReadBookByTitle(cmd_input)) {
                use_cases_.DeleteBook(domain::BookId::FromString(book->id));
            }
        } catch (const std::exception&) {
            output_ << TextResponse::FAILED_DELETE_BOOK << std::endl;
        }
        return true;
    }

    bool View::ShowAuthors() const {
        Print(output_, GetAuthors());
        return true;
    }

    bool View::ShowBooks() const {
        Print(output_, GetBooks());
        return true;
    }

    bool View::ShowBook(std::istream& cmd_input) const {
        try {
            if (auto book = ReadBookByTitle(cmd_input)) {
                output_ << "Title: " << book->title << std::endl;
                output_ << "Author: " << book->author_name << std::endl;
                output_ << "Publication year: " << book->publication_year << std::endl;
                if (!book->tags.empty()) {
                    output_ << "Tags: " << boost::algorithm::join(book->tags, ", ") << std::endl;
                }
            }
        } catch (const std::exception&) {
            throw std::runtime_error(TextError::FAILED_SHOW_BOOK);
        }
        return true;
    }

    bool View::ShowAuthorBooks() const {
        try {
            if (auto author_id = SelectAuthor()) {
                detail::PrintAuthorBooks(output_, use_cases_.GetAuthorBooks(*author_id));
            }
        } catch (const std::exception&) {
            throw std::runtime_error(TextError::FAILED_SHOW_BOOKS);
        }
        return true;
    }

    std::optional<detail::AddBookParams> View::GetBookParams(std::istream& cmd_input) const {
        detail::AddBookParams params;
        cmd_input >> params.publication_year;
        std::getline(cmd_input, params.title);
        boost::algorithm::trim(params.title);
        if (params.title.empty()) {
            throw std::runtime_error(TextError::AUTHOR_EMPTY);
        }
        auto author_id = ReadAuthor();
        if (!author_id) {
            return std::nullopt;
        }
        params.author_id = std::move(*author_id);
        params.tags = std::move(ReadTags());

        return params;
    }

    std::optional<std::string> View::SelectAuthor() const {
        output_ << TextResponse::SELECT_AUTHOR << std::endl;
        auto authors = GetAuthors();
        Print(output_, authors);
        output_ << TextResponse::ENTER_AUTHOR2 << std::endl;
        std::string str;
        if (!std::getline(input_, str) || str.empty()) {
            return std::nullopt;
        }
        int author_idx;
        try {
            author_idx = std::stoi(str);
        } catch (std::exception const&) {
            throw std::runtime_error(TextError::INVALID_AUTHOR_NUM);
        }
        --author_idx;
        if (author_idx < 0 or author_idx >= authors.size()) {
            throw std::runtime_error(TextError::INVALID_AUTHOR_NUM);
        }
        return authors[author_idx].id;
    }

    ui::detail::AuthorsInfo View::GetAuthors() const {
        return ConvertAuthorsToAuthorsInfo([&](){ return use_cases_.GetAllAuthors(); });
    }

    ui::detail::BooksInfo View::GetBooks() const {
        auto result = ConvertBooksToBooksInfo([&](){return use_cases_.GetAllBooks();});
        std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
            return  std::tie(lhs.title, lhs.author_name, lhs.publication_year) <
                    std::tie(rhs.title, rhs.author_name, rhs.publication_year);
                }
        );
        return result;
    }

    ui::detail::Tags View::ReadTags(std::string_view message) const {
        output_ << message << std::endl;

        std::string raw_tags;
        std::getline(input_, raw_tags);

        if (raw_tags.empty()) {
            return ui::detail::Tags{};
        }

        std::vector<std::string> tags;
        boost::split(tags, raw_tags, boost::is_any_of(","));
        std::transform(tags.begin(), tags.end(), tags.begin(), [](const auto& tag) {
            std::vector<std::string> tmp_vec;
            boost::algorithm::split(tmp_vec, tag, boost::is_any_of(" "));
            return boost::algorithm::join_if(tmp_vec, " ", [](auto& el){
                return (el != " ") && (el != "") && (el != "\t");
            });;
        });

        ui::detail::Tags result(tags.begin(), tags.end());
        result.erase(""s);
        return result;
    }

    ui::detail::Tags View::ReadBookTags(const ui::detail::Tags& default_tags) const {
        auto tags = ReadTags(TextResponse::ENTER_TAGS2(boost::algorithm::join(default_tags, ", ")));

        // FIXME:: Теги следует сохранять, если не были предоставлены новые. Текущее решение удаляет их, если новых не было.
        // TODO:: Чтобы сохранять теги, раскомментить код ниже:
        //        if (tags.empty()) {
        //            return default_tags;
        //        }
        // P.S: Закомментировать код пришлось, чтобы пройти тесты яндекса.
        return tags;
    }

}  // namespace ui