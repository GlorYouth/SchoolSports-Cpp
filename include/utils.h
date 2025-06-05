

#ifndef UTILS_H
#define UTILS_H
#include <functional>

namespace utils
{
    template<typename T>
    using Ref = std::reference_wrapper<T>;

    template<typename T>
    using RefConst = Ref<const T>;

}

#endif //UTILS_H
