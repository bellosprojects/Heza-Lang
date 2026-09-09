#pragma once

#include <memory>

namespace heza::core {

    class Visitor;
    using VisitorPtr = std::shared_ptr<Visitor>;

    class Visitor {
        public:
            Visitor() {}
    };

};