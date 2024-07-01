#ifndef STRING_HPP_
#define STRING_HPP_

#include <algorithm>
#include <cstring>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <
    typename Allocator = std::allocator<char>,
    std::size_t SSO_BUFFER_SIZE = 128>
class BasicString {
    static_assert(
        std::is_default_constructible_v<Allocator>,
        "Allocator must be default constructible!"
    );

    static_assert(
        sizeof(char) == 1,
        "Not supported platform/compiler configuration :c"
    );

    static constexpr bool m_allocate_noexcept =
        noexcept(Allocator().allocate(1));

    // please, be true! noexcept(false) in operator=(BasicString &&) otherwise.
    static constexpr bool m_deallocate_noexcept =
        noexcept(Allocator().deallocate(nullptr, 1));

private:
    union {
        char small_string[SSO_BUFFER_SIZE]{};
        char *long_string;
    } m_data{};

    std::size_t m_size = 0;
    std::size_t m_capacity = SSO_BUFFER_SIZE;

    [[nodiscard]] bool is_small() const noexcept {
        return m_capacity <= SSO_BUFFER_SIZE;
    }

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
          m_capacity(std::max(SSO_BUFFER_SIZE, eval_capacity(m_size + 1))) {
        if (is_small()) {
            std::memcpy(m_data.small_string, str, m_size + 1);
        } else {
            m_data.long_string = Allocator().allocate(m_capacity);
            std::memcpy(m_data.long_string, str, m_size + 1);
        }
    }

    BasicString(std::size_t count, char ch) noexcept(m_allocate_noexcept)
        : m_size(count),
          m_capacity(std::max(SSO_BUFFER_SIZE, eval_capacity(m_size + 1))) {
        if (!is_small()) {
            m_data.long_string = Allocator().allocate(m_capacity);
        }

        for (char *it = data(); it < data() + count; ++it) {  // NOLINT
            *it = ch;
        }
        data()[m_size] = 0;  // NOLINT
    }

    BasicString(const BasicString &other) noexcept(m_allocate_noexcept)
        : m_size(other.m_size), m_capacity(other.m_capacity) {
        if (!is_small()) {
            m_data.long_string = Allocator().allocate(m_capacity);
            std::memcpy(
                m_data.long_string, other.m_data.long_string, m_size + 1
            );
        } else {
            std::memcpy(
                m_data.small_string, other.m_data.small_string, m_size + 1
            );
        }
    }

    BasicString &operator=(const BasicString &other
    ) noexcept(m_allocate_noexcept && m_deallocate_noexcept) {
        if (this == &other) {
            return *this;
        }

        // case 1: we are small string, and so is other
        if (is_small() && other.is_small()) {
            m_size = other.m_size;
            std::memcpy(
                m_data.small_string, other.m_data.small_string, m_size + 1
            );
            return *this;
        }

        // case 2: we are small string, other is long. making long_string active
        // m_data member going to next step
        if (is_small() && !other.is_small()) {
            m_data.long_string = nullptr;
        }

        // case 3: we are long string
        m_size = other.m_size;
        if (m_capacity < other.m_capacity) {
            if (m_data.long_string != nullptr) {
                Allocator().deallocate(m_data.long_string, m_capacity);
            }
            m_capacity = eval_capacity(m_size);
            m_data.long_string = Allocator().allocate(m_capacity);
        }
        std::memcpy(m_data.long_string, other.data(), m_size + 1);
        return *this;
    }

    BasicString(BasicString &&other) noexcept
        : m_data(std::exchange(other.m_data, {})),
          m_size(std::exchange(other.m_size, 0)),
          m_capacity(std::exchange(other.m_capacity, 0)) {
    }

    // NOLINTNEXTLINE
    BasicString &operator=(BasicString &&other) noexcept(m_deallocate_noexcept
    ) {
        if (this == &other) {
            return *this;
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
        return is_small() ? m_data.small_string : m_data.long_string;
    }

    [[nodiscard]] char *data() noexcept {
        return is_small() ? m_data.small_string : m_data.long_string;
    }

    [[nodiscard]] const char *c_str() const noexcept {
        return is_small() ? m_data.small_string : m_data.long_string;
    }

    [[nodiscard]] const char &operator[](std::size_t i) const noexcept {
        return data()[i];
    }

    [[nodiscard]] char &operator[](std::size_t i) noexcept {
        return data()[i];
    }

    [[nodiscard]] const char &at(std::size_t i) const {
        if (i >= m_size) {
            throw std::out_of_range("BasicString out of range!");
        }
        return data()[i];  // NOLINT
    }

    [[nodiscard]] char &at(std::size_t i) {
        if (i >= m_size) {
            throw std::out_of_range("BasicString out of range!");
        }
        return data()[i];  // NOLINT
    }

    [[nodiscard]] const char &front() const noexcept {
        return data()[0];  // NOLINT
    }

    [[nodiscard]] char &front() noexcept {
        return data()[0];  // NOLINT
    }

    [[nodiscard]] const char &back() const noexcept {
        return data()[m_size - 1];  // NOLINT
    }

    [[nodiscard]] char &back() noexcept {
        return data()[m_size - 1];  // NOLINT
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
            const std::size_t capacity_snapshot = m_capacity;
            const bool was_small = is_small();

            m_capacity = eval_capacity(m_size + other.m_size + 1);
            char *new_data = Allocator().allocate(m_capacity);
            if (was_small) {
                std::memcpy(new_data, m_data.small_string, m_size);
            } else {
                std::memcpy(new_data, m_data.long_string, m_size);
            }

            if (!was_small && m_data.long_string != nullptr) {
                Allocator().deallocate(m_data.long_string, capacity_snapshot);
            }
            std::swap(m_data.long_string, new_data);
        }

        std::memcpy(data() + m_size, other.data(), other.m_size + 1);  // NOLINT
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
        return std::strcmp(data(), other.data()) == 0;
    }

    [[nodiscard]] friend bool
    operator==(const char *lhs, const BasicString &rhs) noexcept {
        return rhs == lhs;
    }

    friend std::ostream &operator<<(std::ostream &os, const BasicString &str) {
        return os << str.c_str();
    }

    /*
     * Destructor
     */

    ~BasicString() {
        if (!is_small() && m_data.long_string != nullptr) {
            Allocator().deallocate(m_data.long_string, m_capacity);
        }
    }
};

using String = BasicString<>;

#endif  // STRING_HPP_