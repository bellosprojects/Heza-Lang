#include <heza/logic/logic.hpp>
#include <heza/logic/andd.hpp>
#include <heza/logic/orr.hpp>
#include <heza/logic/nott.hpp>

namespace heza::logic {

    LogicExprPtr heza::logic::LogicExpr::and_(const LogicExprPtr& other) const {
        auto this_logic = std::static_pointer_cast<const LogicExpr>(shared_from_this());
        return std::make_shared<AndExpr>(std::vector<heza::logic::LogicExprPtr>{this_logic, other});
    }

    LogicExprPtr heza::logic::LogicExpr::or_(const LogicExprPtr& other) const {
        auto this_logic = std::static_pointer_cast<const LogicExpr>(shared_from_this());
        return std::make_shared<OrExpr>(std::vector<heza::logic::LogicExprPtr>{this_logic, other});
    }

    LogicExprPtr heza::logic::LogicExpr::not_() const {
        auto this_logic = std::static_pointer_cast<const LogicExpr>(shared_from_this());
        return std::make_shared<NotExpr>(this_logic);
    }


};