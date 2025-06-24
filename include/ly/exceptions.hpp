#ifndef __LY_EXCEPTIONS__
#define __LY_EXCEPTIONS__

#include <stdexcept>

namespace ly {
class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& msg)
        : std::runtime_error(msg) {}
};

} // namespace ly
#define LY_THROW(msg)                                      \
    throw ly::RuntimeError([&] {                           \
        std::ostringstream oss;                            \
        oss << __FILE__ << ":" << __LINE__ << " - "        \
            << msg;                                        \
        return oss.str();                                  \
    }())

#endif
