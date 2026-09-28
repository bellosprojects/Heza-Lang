#pragma once

#include <heza/core/arithmetic.hpp>
#include <utility>

namespace heza::algebra {

    class AddExpr;
    using AddExprPtr = std::shared_ptr<const AddExpr>;

    class AddExpr : public heza::core::ArithmeticExpr {
    private:
        std::vector<heza::core::ArithmeticExprPtr> terms_;

    public:
        explicit AddExpr(std::vector<heza::core::ArithmeticExprPtr> terms);

        const std::vector<heza::core::ArithmeticExprPtr>& get_terms() const { return terms_; }

        // --- Expr ---
        std::string    to_str()    const override;
        std::u32string to_latex()  const override;
        bool is_equal(const heza::core::ExprPtr& other) const override;
        heza::core::ExprPtr clone()    const override;
        heza::core::ExprPtr simplify() const override;
        void accept(const heza::core::VisitorPtr& visitor) const override;
        size_t hash() const override;
        int compare(const heza::core::Expr& other) const override;
        heza::core::TypeId get_type_id() const override;
        heza::core::ExprPtr subs(const heza::core::Substitution& subst) const override;
        bool is_atomic() const override;

        // --- ArithmeticExpr ---
        heza::core::ArithmeticExprPtr add(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr mul(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr pow(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr neg() const override;
        heza::core::ArithmeticExprPtr diff(const heza::atomic::VariableExprPtr& var) const override;
        bool is_zero()     const override;
        bool is_one()      const override;
        bool is_negative() const override;
        bool is_positive() const override;
        bool is_numeric()  const override;
        double to_double() const override;

    private:
        heza::core::ArithmeticExprPtr simplify_impl() const;

        // Descompone un término en (coeficiente, parte simbólica)
        static std::pair<double, heza::core::ArithmeticExprPtr>
        extract_coefficient(const heza::core::ArithmeticExprPtr& term);

        // Reconstruye un término a partir de (coeficiente, parte simbólica)
        static heza::core::ArithmeticExprPtr
        build_term(double coef, const heza::core::ArithmeticExprPtr& rest);
    };

} // namespace heza::algebra