//
// Created by Devon Richards on 9/2/2019.
//

#ifndef ERASUREBENCHMARKS_COMPILEREGISTRY_H
#define ERASUREBENCHMARKS_COMPILEREGISTRY_H

#include "unconstexpr/tools/type_list.hpp"
#include "unconstexpr/meta_tlist.hpp"

template<typename... Args>
using type_list = uncostexpr::detail::type_list<Args...>;

namespace plugins {
    template<typename Created, typename... Args>
    struct basic_creator {

        using created = Created;
        using dependency_lists = type_list<type_list<Args...>>;

        constexpr static Created create(Args... args) {
            return Created{args...};
        }

    };

    using plugin_creators = meta_tlist<>;
    using plugin_directs = meta_tlist<>;
}

#endif //ERASUREBENCHMARKS_COMPILEREGISTRY_H
