#pragma once
#include <string>
#include <vector>
#include <optional>

#include "../util/tagged_uuid.h"

namespace domain {

namespace detail {
struct AuthorTag {};
}  // namespace detail

using AuthorId = util::TaggedUUID<detail::AuthorTag>;

struct AuthorsKeys {
    static inline std::string AUTHORS = "authors";
    static inline std::string ID      = "id";
    static inline std::string NAME    = "name";
};

class Author {
public:
    Author(AuthorId id, std::string name):
        id_(std::move(id)),
        name_(std::move(name)) {
    }

    [[nodiscard]] const AuthorId& GetId() const noexcept { return id_; }
    [[nodiscard]] const std::string& GetName() const noexcept { return name_; }

private:
    AuthorId id_;
    std::string name_;
};

using Authors = std::vector<Author>;

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;
    [[nodiscard]] virtual Authors GetAuthors() const = 0;
    [[nodiscard]] virtual domain::Author GetAuthorById(const domain::AuthorId &id) const = 0;
    [[nodiscard]] virtual std::optional<domain::Author> GetAuthorByName(const std::string& name) const = 0;
    virtual void DeleteAuthor(const AuthorId& author_id) = 0;
    virtual void EditAuthor(const Author& author) = 0;

protected:
    ~AuthorRepository() = default;
};

}  // namespace domain
