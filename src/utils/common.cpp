#include <cstdint>
#include <ctime>

namespace cl
{
    uint32_t timestamp()
    {
        return static_cast<uint32_t>(time(nullptr));
    }
} // namespace cl