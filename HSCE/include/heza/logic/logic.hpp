#pragma once

#include <heza/core/expr.hpp>
#include <cstdint>
#include <unordered_map>
#include <algorithm>

namespace heza::logic {

    class LogicExpr;

    using LogicExprPtr = std::shared_ptr<const LogicExpr>;

    class BoolExpr;
    class ABoolExpr;
    class AndExpr;
    class OrExpr;
    class NotExpr;

    LogicExprPtr makeAnd(const std::vector<LogicExprPtr>&);
    LogicExprPtr makeOr(const std::vector<LogicExprPtr>&);
    LogicExprPtr makeNot(const LogicExprPtr&);

    class LogicExpr : public heza::core::Expr {

        public:

        virtual ~LogicExpr() = default;

        LogicExprPtr and_(const LogicExprPtr& other) const;
        LogicExprPtr or_(const LogicExprPtr& other) const; 
        LogicExprPtr not_() const;

        static LogicExprPtr from_expr(const heza::core::ExprPtr& expr) {
            return std::static_pointer_cast<const LogicExpr>(expr);
        }

    };

    // =============================== ROBDD ===============================

    struct BDDNode {int var, lo, hi; };

    struct Key { 
        int v, lo, hi; 
        bool operator==(const Key& o) const {
            return v==o.v && lo==o.lo && hi==o.hi; 
        } 
    };

    struct KeyHash {
        size_t operator()(const Key& k) const {
            size_t h = 14695981039346656037ULL;
            h = (h ^ k.v) * 1099511628211ULL;
            h = (h ^ k.lo) * 1099511628211ULL;
            h = (h ^ k.hi) * 1099511628211ULL;
            return h;
        }
    };

    struct PairKey {
        int a, b;
        bool operator==(const PairKey& o) const {
            return a==o.a && b == o.b;
        }
    };

    struct PairHash {
        size_t operator()(const PairKey& k) const {
            return (std::hash<int>()(k.a) << 1) ^ std::hash<int>()(k.b);
        }
    };

    class BDDManager {
        public:

        static constexpr int F = 0, T = 1;
        std::vector<BDDNode> nodes;
        std::unordered_map<Key, int, KeyHash> unique;
        std::unordered_map<std::string, int> varIdx;
        std::vector<std::string> varNames;
        std::unordered_map<PairKey, int, PairHash> andCache, orCache;
        std::unordered_map<int, int> notCache;
        std::unordered_map<int, LogicExprPtr> toLogicMemo;

        BDDManager() {
            nodes.push_back({-1, -1, -1});
            nodes.push_back({-1, -1, -1});
        }

        int var(const std::string& n);
        int mk(int v, int lo, int hi);
        int applyAnd(int a, int b);
        int applyOr(int a, int b);
        int applyNot(int a);
        int fromLogic(const LogicExpr* e);
        LogicExprPtr toLogic(int id);
    };

    class Simplifier {
        public:

        static LogicExprPtr simplify(const LogicExpr* e);

        private:

        static void collect(const LogicExpr* e, std::vector<std::string>& out);
    };

};