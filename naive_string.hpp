#ifndef STRING_HPP_
#define STRING_HPP_

#include <cstring>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename Allocator = std::allocator<char>>
class BasicString {
    static_assert(
        std::is_default_constructible_v<Allocator>,
        "allocator must be default constructible!"
    );

    static constexpr bool m_allocate_noexcept =
        noexcept(Allocator().allocate(1));

    static constexpr bool m_deallocate_noexcept =
        noexcept(Allocator().deallocate(nullptr, 1));

private:
    std::size_t m_size = 0;
    std::size_t m_capacity = 0;
    char *m_data = nullptr;

    [[nodiscard]] static constexpr std::size_t eval_capacity(std::size_t size
    ) noexcept {
        if (size == 0) {
            return 0;
        }
        std::size_t newcap = 1;
        while (newcap < size) {
            newcap *= 2;
        }
        return newcap;
    }

public:
    /*
     * Constructors, assignment operators
     */

    BasicString() noexcept = default;

    BasicString(const char *str) noexcept(m_allocate_noexcept)
        : m_size(std::strlen(str)),
          m_capacity(eval_capacity(m_size + 1)),
          m_data(Allocator().allocate(m_capacity)) {
        std::memcpy(m_data, str, m_size + 1);
    }

    BasicString(std::size_t count, char ch) noexcept(m_allocate_noexcept)
        : m_size(count),
          m_capacity(eval_capacity(m_size + 1)),
          m_data(Allocator().allocate(m_capacity)) {
        for (char *it = m_data; it < m_data + count; ++it) {  // NOLINT
            *it = ch;
        }
        m_data[m_size] = 0;  // NOLINT
    }

    BasicString(const BasicString &other) noexcept(m_allocate_noexcept)
        : m_size(other.m_size),
          m_capacity(other.m_capacity),
          m_data(Allocator().allocate(m_capacity)) {
        if (this == &other) {
            return;
        }
        std::memcpy(m_data, other.m_data, (m_size + 1) * sizeof(char));
    }

    BasicString &operator=(const BasicString &other
    ) noexcept(m_allocate_noexcept && m_deallocate_noexcept) {
        if (this == &other) {
            return *this;
        }

        m_size = other.m_size;
        if (m_capacity < other.m_capacity) {
            if (m_data != nullptr) {
                Allocator().deallocate(m_data, m_capacity);
            }
            m_capacity = eval_capacity(m_size);
            m_data = Allocator().allocate(m_capacity);
        }
        std::memcpy(m_data, other.m_data, (m_size + 1) * sizeof(char));
        return *this;
    }

    BasicString(BasicString &&other) noexcept
        : m_size(std::exchange(other.m_size, 0)),
          m_capacity(std::exchange(other.m_capacity, 0)),
          m_data(std::exchange(other.m_data, nullptr)) {
    }

    BasicString &operator=(BasicString &&other) noexcept(m_deallocate_noexcept
    ) {
        if (this == &other) {
            return *this;
        }

        if (m_data != nullptr) {
            Allocator().deallocate(m_data, m_capacity);
        }
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_data, other.m_data);
        return *this;
    }

    /*
     * Element access
     */

    [[nodiscard]] const char *data() const noexcept {
        return m_data;
    }

    [[nodiscard]] char *data() noexcept {
        return m_data;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        return m_data;
    }

    [[nodiscard]] const char &operator[](std::size_t i) const noexcept {
        return m_data[i];  // NOLINT
    }

    [[nodiscard]] char &operator[](std::size_t i) noexcept {
        return m_data[i];  // NOLINT
    }

    [[nodiscard]] const char &at(std::size_t i) const {
        if (i >= m_size) {
            throw std::out_of_range("BasicString out of range!");
        }
        return m_data[i];  // NOLINT
    }

    [[nodiscard]] char &at(std::size_t i) {
        if (i >= m_size) {
            throw std::out_of_range("BasicString out of range!");
        }
        return m_data[i];  // NOLINT
    }

    [[nodiscard]] const char &front() const noexcept {
        return m_data[0];  // NOLINT
    }

    [[nodiscard]] char &front() noexcept {
        return m_data[0];  // NOLINT
    }

    [[nodiscard]] const char &back() const noexcept {
        return m_data[m_size - 1];  // NOLINT
    }

    [[nodiscard]] char &back() noexcept {
        return m_data[m_size - 1];  // NOLINT
    }

    /*
     * Capacity
     */

    [[nodiscard]] std::size_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]] std::size_t length() const noexcept {
        return m_size;
    }

    [[nodiscard]] std::size_t capacity() const noexcept {
        return m_capacity;
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_size == 0;
    }

    /*
     * Operators
     */

    BasicString &operator+=(const BasicString &other
    ) noexcept(m_allocate_noexcept && m_deallocate_noexcept) {
        if (m_capacity <= m_size + other.m_size) {
            std::size_t capacity_snapshot = m_capacity;
            m_capacity = eval_capacity(m_size + other.m_size + 1);
            char *new_data = Allocator().allocate(m_capacity);
            for (std::size_t i = 0; i < m_size; ++i) {
                new_data[i] = m_data[i];  // NOLINT
            }
            if (m_data != nullptr) {
                Allocator().deallocate(m_data, capacity_snapshot);
            }
            std::swap(m_data, new_data);
        }

        std::memcpy(m_data + m_size, other.m_data, other.m_size + 1);  // NOLINT
        m_size += other.m_size;
        return *this;
    }

    // rvo purpose
    friend BasicString operator+(
        const BasicString &lhs,
        const BasicString &rhs
    ) noexcept(m_allocate_noexcept && m_deallocate_noexcept) {
        BasicString str = lhs;
        str += rhs;
        return str;
    }

    [[nodiscard]] bool operator==(const BasicString &other) const noexcept {
        if (m_size != other.m_size) {
            return false;
        }
        if (m_size == 0 && other.m_size == 0) {
            return true;
        }
        return std::strcmp(m_data, other.m_data) == 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const BasicString &str) {
        return os << str.m_data;
    }

    /*
     * Destructor
     */

    ~BasicString() {
        if (m_data != nullptr) {
            Allocator().deallocate(m_data, m_capacity);
        }
    }
};

template <typename Allocator>
[[nodiscard]] inline bool
operator==(const char *lhs, const BasicString<Allocator> &rhs) noexcept {
    return rhs == lhs;
}

using String = BasicString<>;

#endif  // STRING_HPP_