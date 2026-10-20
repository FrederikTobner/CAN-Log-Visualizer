#pragma once

#include <QString>

#include <utility>
#include <variant>

/// Error message that turns into a failed `Result`.
struct Failure {
    QString message;
};

/// A value or an error message. It mirrors the subset of `std::expected<T, QString>`
/// that the project uses, so it can be replaced by it once the CI toolchain
/// supports it (clang 18 with libstdc++ 13 does not enable `<expected>`).
template <typename T> class Result {
  public:
    // Implicit on purpose, like std::expected: `return value;` and `return Failure{...};` must work.
    // NOLINTBEGIN(cppcoreguidelines-explicit-constructor,misc-explicit-constructor)
    Result(T value) : m_state(std::in_place_index<0>, std::move(value)) {
    }

    Result(Failure failure) : m_state(std::in_place_index<1>, std::move(failure.message)) {
    }
    // NOLINTEND(cppcoreguidelines-explicit-constructor,misc-explicit-constructor)

    explicit operator bool() const {
        return m_state.index() == 0;
    }

    T const & operator*() const {
        return std::get<0>(m_state);
    }

    T const * operator->() const {
        return &std::get<0>(m_state);
    }

    QString const & error() const {
        return std::get<1>(m_state);
    }

  private:
    std::variant<T, QString> m_state;
};
