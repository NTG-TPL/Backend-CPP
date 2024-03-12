#include <iostream>

#include <pqxx/pqxx>
#include <boost/json/src.hpp>

#include <string>
#include <string_view>

using namespace std::literals;
namespace json = boost::json;

using pqxx::operator"" _zv;

struct Prepared{
    Prepared() = delete;
    static const constexpr auto tag_add_book = "add_book"_zv;
    static const constexpr auto tag_all_books = "all_books"_zv;
    static const constexpr auto tag_exit = "exit"_zv;
};

struct Keys {
    Keys() = delete;

    static inline std::string RESULT = "result";
    static inline std::string BOOKS  = "books";
    static inline std::string ID     = "id";
    static inline std::string TITLE  = "title";
    static inline std::string AUTHOR = "author";
    static inline std::string YEAR   = "year";
    static inline std::string ISBN   = "ISBN";
    static inline std::string PARAMS = "title, author, year, ISBN";
};

json::object GetResult(bool value){
    json::object obj;
    obj[Keys::RESULT] = value;
    return obj;
}

struct Requests{
    static inline std::string ACTION  = "action";
    static inline std::string PAYLOAD = "payload";
};

struct Query{
    static inline std::string get_all_books = "SELECT * FROM "s + Keys::BOOKS + " ORDER BY "s +
                                              Keys::YEAR + " DESC, "s +
                                              Keys::TITLE + " ASC, "s +
                                              Keys::AUTHOR + " ASC, "s +
                                              Keys::ISBN + " ASC;"s;
};

struct Book {
    std::string title, author;
    int year{};
    std::optional<std::string> isbn;
};

json::value GetAllBooks(pqxx::read_transaction& r){
    json::array arr;
    for (auto const& [id, title, author, year, isbn] : r.query<int, std::string, std::string, int, std::optional<std::string>>(Query::get_all_books)) {
        json::object obj = {
                {Keys::ID,     id},
                {Keys::TITLE,  title},
                {Keys::AUTHOR, author},
                {Keys::YEAR,   year},
                {Keys::ISBN,   isbn ? *isbn : "null"s}};
        arr.push_back(obj);
    }
    return arr;
}

Book GetBook(const json::object& payload){
    Book book;
    book.title = payload.at(Keys::TITLE).as_string();
    book.author = payload.at(Keys::AUTHOR).as_string();
    book.year = static_cast<int>(payload.at(Keys::YEAR).as_int64());

    const auto* isbn = payload.at(Keys::ISBN).if_string();
    if (isbn != nullptr) {
        book.isbn = *isbn;
    }
    return book;
}

void AddBook(pqxx::work& w, const Book& book){
    w.exec_prepared(Prepared::tag_add_book,
                    w.esc(book.title),
                    w.esc(book.author),
                    book.year,
                    (book.isbn.has_value()) ? w.esc(*book.isbn): book.isbn);
    w.commit();
}


int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            std::cout << "Usage: connect_db <conn-string>\n"sv;
            return EXIT_SUCCESS;
        } else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};
        pqxx::work w(conn);

        // Используя транзакцию создадим таблицу в выбранной базе данных:
        w.exec(
                "CREATE TABLE IF NOT EXISTS "s + Keys::BOOKS + " ("s
                + Keys::ID + " SERIAL PRIMARY KEY,"s
                + Keys::TITLE + " varchar(100) NOT NULL,"s
                + Keys::AUTHOR + " varchar(100) NOT NULL,"s
                + Keys::YEAR + " integer NOT NULL,"s
                + Keys::ISBN + " char(13) UNIQUE);"s);

        w.exec("DELETE FROM "s + Keys::BOOKS + ";"s);
        w.commit();

        conn.prepare(Prepared::tag_add_book, "INSERT INTO " + Keys::BOOKS + " (" + Keys::PARAMS + ") VALUES ($1, $2, $3, $4)"s);

        std::string input;
        while (std::getline(std::cin, input)) {
            const json::value jv = json::parse(input);
            const std::string req = std::string(jv.as_object().at(Requests::ACTION).as_string());
            const json::object payload = jv.as_object().at(Requests::PAYLOAD).as_object();
            if (Prepared::tag_add_book == req) {
                auto book = GetBook(payload);
                try {
                    AddBook(w, book);
                } catch (const pqxx::sql_error& e) {
                    std::cout << json::serialize(GetResult(false)) << std::endl;
                    continue;
                }
                std::cout << json::serialize(GetResult(true)) << std::endl;
            } else if (Prepared::tag_all_books == req) {
                pqxx::read_transaction r(conn);
                std::cout << json::serialize(GetAllBooks(r)) << std::endl;
            } else if (Prepared::tag_exit == req) {
                break;
            } else {
                std::cout << R"({"result", "The command was not found"})" << std::endl;
            }
        }
        conn.close();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}