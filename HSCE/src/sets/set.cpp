#include <heza/sets/sets.hpp>

namespace heza::sets {

    SetExprPtr SetExpr::set_union(const SetExprPtr& other) const {
        auto self = std::static_pointer_cast<SetExpr>(
            std::const_pointer_cast<Expr>(shared_from_this())
        );
        return std::make_shared<SetUnion>(self, other);
    };

    SetExprPtr SetExpr::intersect(const SetExprPtr& other) const {
        auto self = std::static_pointer_cast<SetExpr>(
            std::const_pointer_cast<Expr>(shared_from_this())
        );
        return std::make_shared<SetIntersection>(self, other);
    };

    SetExprPtr SetExpr::difference(const SetExprPtr& other) const {
        auto self = std::static_pointer_cast<SetExpr>(
            std::const_pointer_cast<Expr>(shared_from_this())
        );
        return std::make_shared<SetDifference>(self, other);
    };

}