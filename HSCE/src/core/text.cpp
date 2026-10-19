#include <heza/core/text.hpp>
#include <heza/utils/utf.hpp>

namespace heza::core {

    std::string   TextExpr::to_str()   const {
        return heza::utils::u32_to_utf8(value_);
    }

    std::u32string TextExpr::to_latex() const {
        return value_;
    }

    bool TextExpr::is_equal(const ExprPtr& other) const {
        if(auto other_text = std::dynamic_pointer_cast<const TextExpr>(other)){
            return other_text->value_ == value_;
        }
        return false;
    }

    ExprPtr TextExpr::clone()    const {
        return std::make_shared<TextExpr>(value_);
    }

    ExprPtr TextExpr::simplify() const {
        return clone();
    }

    void TextExpr::accept(const VisitorPtr& visitor) const {
        
    }

    size_t TextExpr::hash() const {
        return std::size_t(0l);
    }


    int TextExpr::compare(const Expr& other) const {
        return -1;
    }

    TypeId TextExpr::get_type_id() const {
        return heza::core::TypeId::TEXT;
    }

    ExprPtr TextExpr::subs(const Substitution& subst) const {
        return clone();
    }

    bool TextExpr::is_atomic() const {
        return true;
    }

    TextExprPtr TextExpr::concat(const TextExprPtr& other) const {
        return std::make_shared<TextExpr>(value_ + other->value_);
    }

    TextExprPtr TextExpr::repeat(const heza::atomic::NumberExprPtr& times) const {
        std::u32string result;
        for (int i = 0; i < times->to_double(); i++) result += value_;
        return std::make_shared<TextExpr>(result);
    }

    heza::atomic::NumberExprPtr TextExpr::length() const {
        return std::make_shared<heza::atomic::NumberExpr>((int)value_.size());
    }

    TextExprPtr TextExpr::charAt(const heza::atomic::NumberExprPtr& index) const {
        int index_int = index->to_double();

        if(index_int < 0 || index_int >= value_.size()) throw std::runtime_error("TextExpr::charAt: indice fuera de rango");
        return std::make_shared<TextExpr>(value_.substr(index_int, index_int + 1));
    }


};