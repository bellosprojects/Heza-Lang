#include <heza/logic/logic.hpp>
#include <heza/logic/andd.hpp>
#include <heza/logic/orr.hpp>
#include <heza/logic/nott.hpp>
#include <heza/logic/bool.hpp>
#include <heza/logic/abool.hpp>

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


    // ======================= ROBDD ============

    LogicExprPtr Simplifier::simplify(const LogicExpr* e){
        std::vector<std::string> vars;
        collect(e, vars);
        std::sort(vars.begin(), vars.end());
        vars.erase(std::unique(vars.begin(), vars.end()), vars.end());

        BDDManager bdd;
        for(auto & v : vars) bdd.var(v);
        int root = bdd.fromLogic(e);

        return bdd.toLogic(root);
    }

    void Simplifier::collect(const LogicExpr* e, std::vector<std::string>& out){
        switch (e->get_type_id()){
            case heza::core::TypeId::ABOOL:
                out.push_back(static_cast<const ABoolExpr*>(e)->to_str());
                break;

            case heza::core::TypeId::NOT:
                collect(static_cast<const NotExpr*>(e)->expr_.get(), out);
                break;

            case heza::core::TypeId::AND:
                for(auto& c : static_cast<const AndExpr*>(e)->exprs_) collect(c.get(), out);
                break;

            case heza::core::TypeId::OR:
                for(auto& c : static_cast<const OrExpr*>(e)->exprs_) collect(c.get(), out);
                break;
            
            default:
                break;
        }
    }

    LogicExprPtr makeNot(const LogicExprPtr& x0){
        auto x  = x0;
        switch (x->get_type_id())
        {
        case heza::core::TypeId::BOOL:
            return std::make_shared<BoolExpr>(!static_cast<const BoolExpr&>(*x).get_value());

        case heza::core::TypeId::ABOOL:
            return std::make_shared<NotExpr>(x);

        case heza::core::TypeId::NOT:
            return static_cast<const NotExpr&>(x).expr_;

        case heza::core::TypeId::AND: {
            std::vector<LogicExprPtr> a;
            for(auto& c : static_cast<const AndExpr&>(*x).exprs_) a.push_back(makeNot(c));
            return makeOr(a);
        }

        case heza::core::TypeId::OR: {
            std::vector<LogicExprPtr> a;
            for(auto& c : static_cast<const OrExpr&>(*x).exprs_) a.push_back(makeNot(c));
            return makeAnd(a);
        }
        }
        
        return x;
    }

    LogicExprPtr makeAnd(const std::vector<LogicExprPtr>& args) {
        std::vector<LogicExprPtr> xs;

        for(auto& a : args) {
            if(a -> get_type_id() == heza::core::TypeId::BOOL) {
                if(!static_cast<const BoolExpr&>(*a).get_value()) return std::make_shared<BoolExpr>(false);
                continue;
            }
            if(a->get_type_id() == heza::core::TypeId::AND) {
                for(auto& c : static_cast<const AndExpr&>(*a).exprs_) xs.push_back(c);
            }
            else {
                xs.push_back(a);
            }
        }

        bool changed = true;
        while(changed) {
            changed = false;

            std::vector<LogicExprPtr> nb;
            for(auto& x : xs){
                if(x->get_type_id() == heza::core::TypeId::BOOL) {
                    if(!static_cast<const BoolExpr&>(*x).get_value()) return std::make_shared<BoolExpr>(false);
                    changed = true;
                } else {
                    nb.push_back(x);
                }
            }
            xs = nb;

            std::sort(xs.begin(), xs.end(), [](auto& a, auto& b){ return a->compare(*b) < 0; });

            std::vector<LogicExprPtr> t;
            for(auto& x : xs){
                if(t.empty() || t.back()->compare(*x) != 0) t.push_back(x);
            }

            xs = t;

            for(auto& x : xs){
                auto nx = makeNot(x);
                for(auto& y : xs){
                    if(y->compare(*nx) == 0) return std::make_shared<BoolExpr>(false);
                }
            }

            std::vector<LogicExprPtr> t2;
            for(auto& x : xs){
                bool abs = false;
                if(x->get_type_id() == heza::core::TypeId::OR) {
                    for(auto& y : xs){
                        if(y.get() == x.get() || y->get_type_id() == heza::core::TypeId::OR) continue;
                        for(auto& c : static_cast<const OrExpr&>(*x).exprs_)
                            if(c->compare(*y) == 0){
                                abs = true;
                                break;
                            }
                        if (abs) break;
                    }
                }
                if(abs) { changed = true; }
                else t2.push_back(x);
            }

            xs = t2;

            std::vector<LogicExprPtr> units;
            for(auto& x : xs) if(x->get_type_id() == heza::core::TypeId::ABOOL) units.push_back(x);

            std::vector<LogicExprPtr> t3;

            for(auto& x : xs){
                if(x->get_type_id() == heza::core::TypeId::OR){
                    std::vector<LogicExprPtr> lits(static_cast<const OrExpr&>(*x).exprs_);
                    for(auto& u : units){
                        auto nu = makeNot(u);
                        auto it = std::find_if(lits.begin(), lits.end(), 
                            [&](auto &l){ return l->compare(*nu) == 0; });

                        if(it != lits.end()) {
                            lits.erase(it);
                            changed = true;
                        }
                    }
                    if(lits.empty()) return std::make_shared<BoolExpr>(false);
                    t3.push_back(makeOr(lits));
                } else {
                    t3.push_back(x);
                }
            }

            xs = t3;

        } // WHILE

        if(xs.empty()) return std::make_shared<BoolExpr>(true);
        if(xs.size() == 1) return xs[0];
        return std::make_shared<AndExpr>(xs);

    }


    LogicExprPtr makeOr(const std::vector<LogicExprPtr>& args) {
    std::vector<LogicExprPtr> xs;
    for (auto& a : args) {
        if (a->get_type_id() == heza::core::TypeId::BOOL) {
            if (static_cast<const BoolExpr&>(*a).get_value()) return std::make_shared<BoolExpr>(true);
            continue;
        }
        if (a->get_type_id() == heza::core::TypeId::OR)
            for (auto& c : static_cast<const OrExpr&>(*a).exprs_) xs.push_back(c);
        else
            xs.push_back(a);
    }
    bool changed = true;
    while (changed) {
        changed = false;

        std::vector<LogicExprPtr> nb;
        for (auto& x : xs) {
            if (x->get_type_id() == heza::core::TypeId::BOOL) {
                if (static_cast<const BoolExpr&>(*x).get_value()) return std::make_shared<BoolExpr>(true);
                changed = true;
            } else nb.push_back(x);
        }
        xs = nb;

        std::sort(xs.begin(), xs.end(),
                  [](auto& a, auto& b){ return a->compare(*b) < 0; });
        std::vector<LogicExprPtr> t;
        for (auto& x : xs)
            if (t.empty() || t.back()->compare(*x) != 0) t.push_back(x);
        xs = t;

        for (auto& x : xs) {
            auto nx = makeNot(x);
            for (auto& y : xs)
                if (y->compare(*nx) == 0) return std::make_shared<BoolExpr>(true);
        }

        // absorción dual x ∨ (x ∧ y) = x
        std::vector<LogicExprPtr> t2;
        for (auto& x : xs) {
            bool abs = false;
            if (x->get_type_id() == heza::core::TypeId::AND) {
                for (auto& y : xs) {
                    if (y.get() == x.get() || y->get_type_id() == heza::core::TypeId::AND) continue;
                    for (auto& c : static_cast<const AndExpr&>(*x).exprs_)
                        if (c->compare(*y) == 0) { abs = true; break; }
                    if (abs) break;
                }
            }
            if (abs) { changed = true; }
            else t2.push_back(x);
        }
        xs = t2;

        // propagación dual a ∨ (¬a ∧ b) = a ∨ b
        std::vector<LogicExprPtr> units;
        for (auto& x : xs) if (x->get_type_id() == heza::core::TypeId::ABOOL) units.push_back(x);
        std::vector<LogicExprPtr> t3;
        for (auto& x : xs) {
            if (x->get_type_id() == heza::core::TypeId::AND) {
                std::vector<LogicExprPtr> lits(static_cast<const AndExpr&>(*x).exprs_);
                for (auto& u : units) {
                    auto nu = makeNot(u);
                    auto it = std::find_if(lits.begin(), lits.end(),
                        [&](auto& l){ return l->compare(*nu) == 0; });
                    if (it != lits.end()) { lits.erase(it); changed = true; }
                }
                if (lits.empty()) return std::make_shared<BoolExpr>(true);
                t3.push_back(makeAnd(lits));
            } else t3.push_back(x);
        }
        xs = t3;
    }
    if (xs.empty()) return std::make_shared<BoolExpr>(false);
    if (xs.size() == 1) return xs[0];
    return std::make_shared<OrExpr>(xs);
}


    int BDDManager::var(const std::string& n){
        auto it = varIdx.find(n);
        if(it != varIdx.end()) return it->second;
        int i = (int)varNames.size();
        varNames.push_back(n);
        varIdx[n] = i;
        return i;
    }

    int BDDManager::mk(int v, int lo, int hi){
        if( lo == hi) return lo;
        Key k{v, lo, hi};
        auto it = unique.find(k);
        if(it != unique.end()) return it->second;
        int id = (int)nodes.size();
        nodes.push_back({v, lo, hi});
        unique[k] = id;
        return id;
    }

    int BDDManager::applyAnd(int a, int b){
        if(a == F || b == F) return F;
        if(a == T) return b;
        if(b == T) return a;
        if(a == b) return a;

        PairKey k{std::min(a,b), std::max(a,b)};
        auto it = andCache.find(k);
        if(it != andCache.end()) return it->second;
        auto& na = nodes[a];
        auto& nb = nodes[b];
        int v = std::min(na.var, nb.var);
        int aL = na.var==v ? na.lo : a, aH = na.var == v ? na.hi : a;
        int bL = nb.var==v ? nb.lo : b, bH = nb.var == v ? nb.hi : b;
        int r = mk(v, applyAnd(aL, bL), applyAnd(aH, bH));
        andCache[k] = r;
        return r;
    }

    int BDDManager::applyOr(int a, int b){
        if(a == T || b == T) return T;
        if(a == F) return b;
        if(b == F) return a;
        if(a == b) return a;

        PairKey k{std::min(a,b), std::max(a,b)};
        auto it = orCache.find(k);
        if(it != orCache.end()) return it->second;
        auto& na = nodes[a];
        auto& nb = nodes[b];
        int v = std::min(na.var, nb.var);
        int aL = na.var == v ? na.lo : a, aH = na.var == v ? na.hi : a;
        int bL = nb.var == v ? nb.lo : b, bH = nb.var == v ? nb.hi : b;
        int r = mk(v, applyOr(aL, bL), applyOr(aH, aH));
        orCache[k] = r;
        return r;
    }

    int BDDManager::applyNot(int a){
        if(a == F) return T;
        if(a == T) return F;
        auto it = notCache.find(a);
        if(it != notCache.end()) return it->second;
        auto& n = nodes[a];
        int r = mk(n.var, applyNot(n.lo), applyNot(n.hi));
        notCache[a] = r;
        return r;
    }

    int BDDManager::fromLogic(const LogicExpr* e){
        switch (e->get_type_id())
        {
        case heza::core::TypeId::BOOL:
            return static_cast<const BoolExpr*>(e)->get_value() ? T : F;

        case heza::core::TypeId::ABOOL: {
            auto name_ = static_cast<const ABoolExpr*>(e)->to_str();
            int v = varIdx.at(name_);
            return mk(v, F, T);
        }

        case heza::core::TypeId::NOT:
            return applyNot(fromLogic(static_cast<const NotExpr*>(e)->expr_.get()));
        
        case heza::core::TypeId::AND: {
            int acc = T;

            for(auto& c : static_cast<const AndExpr*>(e)->exprs_) {
                acc = applyAnd(acc, fromLogic(c.get()));
                if(acc == F) return F;
            }

            return acc;
        }

        case heza::core::TypeId::OR: {
            int acc = F;
            for(auto& c : static_cast<const OrExpr*>(e)->exprs_){
                acc = applyOr(acc, fromLogic(c.get()));
                if(acc == T) return T;
            }
            return acc;
        }

        }

        return F;
    }

    LogicExprPtr BDDManager::toLogic(int id){
        if(id == F) return std::make_shared<BoolExpr>(false);
        if(id == T) return std::make_shared<BoolExpr>(true);

        auto it = toLogicMemo.find(id);
        if(it != toLogicMemo.end()) return it->second;

        auto& n = nodes[id];
        auto v = std::make_shared<ABoolExpr>(varNames[n.var]);
        auto hi = toLogic(n.hi);
        auto lo = toLogic(n.lo);

        auto pos = makeAnd({v, hi});
        auto neg = makeAnd({makeNot(v), lo});
        auto r = makeOr({pos, neg});

        toLogicMemo[id] = r;
        return r;
    }

};