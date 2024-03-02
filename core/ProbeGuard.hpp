#pragma once

namespace QtAda::core {
class ProbeGuard {
public:
    ProbeGuard() noexcept;
    ~ProbeGuard() noexcept;

    static bool locked() noexcept;
    static void setLocked(bool isLocked) noexcept;

private:
    bool previosState_;
};
} // namespace QtAda::core
