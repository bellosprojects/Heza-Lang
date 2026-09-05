#define SETS_H
#ifdef SETS_H

#define _USE_MATH_DEFINES
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#define MAX_SET_ITEMS_PRINT 100

namespace py = pybind11;

class Set;
using SetPtr = std::shared_ptr<Set>;

class Set : public std::enable_shared_from_this<Set> {
    protected:
        int dimension_;
        SetPtr inferred_universe;
    public:
        virtual ~Set() = default;
        virtual bool contains(double) const = 0;
        virtual std::set<double> print() const {
            return std::set<double>();
        };
        virtual int get_dimension() {
            return dimension_;
        }
        virtual SetPtr get_inferred_universe(){
            return inferred_universe;
        }
        virtual SetPtr union_(const SetPtr&other);
        virtual SetPtr intersection(const SetPtr&other);
        virtual SetPtr difference(const SetPtr&other);
        virtual SetPtr symetric_difference(const SetPtr&other);
        virtual SetPtr complement(const SetPtr&universe);
        virtual SetPtr complement_self();
};

class SetUnion : public Set {
    SetPtr left_;
    SetPtr right_;
    public:
        SetUnion(SetPtr left, SetPtr right): left_(left), right_(right) {}
        bool contains(double value) const override {
            return left_->contains(value) || right_->contains(value);
        }      
};

class SetIntersection: public Set {
    SetPtr left_;
    SetPtr right_;
    public:
        SetIntersection(SetPtr left, SetPtr right): left_(left), right_(right) {}
        bool contains(double value) const override {
            return left_->contains(value) && right_->contains(value);
        }      
};

class SetDifference: public Set {
    SetPtr left_;
    SetPtr right_;
    public:
        SetDifference(SetPtr left, SetPtr right): left_(left), right_(right) {}
        bool contains(double value) const override {
            return left_->contains(value) && !right_->contains(value);
        }      
};

class SetSymetricDifference: public Set {
    SetPtr left_;
    SetPtr right_;
    public:
        SetSymetricDifference(SetPtr left, SetPtr right): left_(left), right_(right) {}
        bool contains(double value) const override {
            return (
                (left_->contains(value) && !right_->contains(value)) ||
                (right_->contains(value) && !left_->contains(value))
            );
        }      
};

SetPtr Set::union_(const SetPtr&other){
    return std::make_shared<SetUnion>(std::const_pointer_cast<Set>(shared_from_this()), other);
}

SetPtr Set::intersection(const SetPtr&other){
    return std::make_shared<SetIntersection>(std::const_pointer_cast<Set>(shared_from_this()), other);
}

SetPtr Set::difference(const SetPtr&other){
    return std::make_shared<SetDifference>(std::const_pointer_cast<Set>(shared_from_this()), other);
}

SetPtr Set::symetric_difference(const SetPtr&other){
    return std::make_shared<SetSymetricDifference>(std::const_pointer_cast<Set>(shared_from_this()), other);
}

SetPtr Set::complement(const SetPtr&universe){
    return std::make_shared<SetDifference>(universe, std::const_pointer_cast<Set>(shared_from_this()));
}

SetPtr Set::complement_self(){
    return std::make_shared<SetDifference>(inferred_universe, std::const_pointer_cast<Set>(shared_from_this()));
}


class SetEmpty : public Set {
    public:
        SetEmpty() {}
        bool contains(double) const override {
            return false;
        }
        std::set<double> print() const override{
            return std::set<double>();
        }
        SetPtr union_(const SetPtr&other) override {
            return other;
        }
        SetPtr intersection(const SetPtr&other) override {
            return std::const_pointer_cast<Set>(shared_from_this());
        }
        SetPtr difference(const SetPtr&other) override {
            return std::const_pointer_cast<Set>(shared_from_this());
        }
        SetPtr symetric_difference(const SetPtr&other) override {
            return other;
        }
        SetPtr complement(const SetPtr&universe) override {
            return universe;
        }
};

class SetFinite : public Set {
    std::set<double> values_;
    public:
        SetFinite(const std::set<double>& values): values_(values) {}
        bool contains(double value) const override {
            for(auto v : values_){
                if(v == value) return true;
            }
            return false;
        }
        std::set<double> print() const override {
            return values_;
        }
        SetPtr union_(const SetPtr&other) override {
            if(auto o_fin = std::dynamic_pointer_cast<SetFinite>(other)){
                std::set<double> final_set(values_.begin(), values_.end());
                final_set.insert(o_fin->values_.begin(), o_fin->values_.end());
                return std::make_shared<SetFinite>(final_set);
            }else{
                return std::make_shared<SetUnion>(std::const_pointer_cast<Set>(shared_from_this()), other);
            }
        }
        SetPtr intersection(const SetPtr&other) override {
            if(auto o_fin = std::dynamic_pointer_cast<SetFinite>(other)){
                std::set<double> final_set;
                auto other_values = o_fin->values_;
                for(auto v : other_values){
                    if(values_.find(v) != values_.end()) final_set.insert(v);
                }
                return std::make_shared<SetFinite>(final_set);
            }else{
                return std::make_shared<SetIntersection>(std::const_pointer_cast<Set>(shared_from_this()), other);
            }
        }
        SetPtr difference(const SetPtr&other) override {
            if(auto o_fin = std::dynamic_pointer_cast<SetFinite>(other)){
                std::set<double> final_set;
                auto other_values = o_fin->values_;
                for(auto v : values_){
                    if(other_values.find(v) == other_values.end()) final_set.insert(v);
                }
                return std::make_shared<SetFinite>(final_set);
            }else{
                return std::make_shared<SetDifference>(std::const_pointer_cast<Set>(shared_from_this()), other);
            }
        }
        SetPtr symetric_difference(const SetPtr&other) override {
            if(auto o_fin = std::dynamic_pointer_cast<SetFinite>(other)){
                std::set<double> final_set;
                auto other_values = o_fin->values_;
                for(auto v : values_){
                    if(other_values.find(v) == other_values.end()) final_set.insert(v);
                }
                for(auto v : other_values){
                    if(values_.find(v) == values_.end()) final_set.insert(v);
                }
                return std::make_shared<SetFinite>(final_set);
            }else{
                return std::make_shared<SetSymetricDifference>(std::const_pointer_cast<Set>(shared_from_this()), other);
            }
        }
        SetPtr complement(const SetPtr&universe) override {
            if(auto o_fin = std::dynamic_pointer_cast<SetFinite>(universe)){
                std::set<double> final_set;
                auto other_values = o_fin->values_;
                for(auto v : other_values){
                    if(values_.find(v) == values_.end()) final_set.insert(v);
                }
                return std::make_shared<SetFinite>(final_set);
            }else{
                return std::make_shared<SetDifference>(universe, std::const_pointer_cast<Set>(shared_from_this()));
            }
        }
};

class SetReals : public Set {
    public:
        SetReals() {}
        bool contains(double value) const override {
            return true;
        }
        std::set<double> print() const override {
            std::set<double> result;
            for(double i = 0; i< MAX_SET_ITEMS_PRINT; i++) result.insert(i);
            return result;
        }
};

SetPtr make_reals(){
    return std::make_shared<SetReals>();
}

class Interval : public Set {
    double lim_inf_, lim_sup_;
    bool close_inf_, close_sup_;
    public:
        Interval(double lim_inf, double lim_sup, bool close_inf, bool close_sup): lim_inf_(lim_inf), lim_sup_(lim_sup), close_inf_(close_inf), close_sup_(close_sup) {
            inferred_universe = std::make_shared<SetReals>();
        }
        Interval(double lim_inf, double lim_sup): lim_inf_(lim_inf), lim_sup_(lim_sup), close_inf_(true), close_sup_(true) {}
        bool contains(double value) const override {
            return (
                value > lim_inf_ || (value == lim_inf_ && close_inf_)
            ) && (
                value < lim_sup_ || (value == lim_sup_ && close_sup_)
            );
        }
};


#endif SETS_H