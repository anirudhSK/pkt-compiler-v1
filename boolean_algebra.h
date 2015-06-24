#ifndef BOOLEAN_ALGEBRA_H_
#define BOOLEAN_ALGEBRA_H_

#include <ostream>
#include <vector>
#include <string>

// Single atom: Variable or negated variable
class Atom {
 public:
  Atom(const std::string & t_name, const bool t_pristine) : var_name_(t_name), pristine_(t_pristine), is_literal_(false), value_(false) {}
  static Atom make_literal(const bool & t_value) {
    Atom ret = Atom("LITERAL" + std::to_string(t_value), true);
    ret.is_literal_ = true;
    ret.value_ = t_value;
    return ret;
  }
  friend std::ostream & operator<< (std::ostream & out, const Atom & atom) {
    out << (atom.pristine_ ? atom.var_name_ : ("~" + atom.var_name_));
    return out;
  };
  bool get_value() const { return value_; }
  bool is_literal() const { return is_literal_; }

 private:
  const std::string var_name_;
  const bool pristine_;
  bool is_literal_;
  bool value_;
};

// Conjunction: AND of Atoms,
// each atom either stands by itself or is negated
class Conjunction {
 public:
  explicit Conjunction(const Atom & t_atom) { atoms_.emplace_back(t_atom); }

  /// AND of two conjunctions, just concatenate vectors
  Conjunction operator*(const Conjunction & t_conjunction) const {
    auto ret(*this);
    for (const auto & atom : t_conjunction.atoms_) {
      ret.atoms_.emplace_back(atom);
    }
    return ret;
  }

  friend std::ostream & operator<< (std::ostream & out, const Conjunction & clause) {
    if (clause.atoms_.empty()) {
      return out;
    } else {
     assert(clause.atoms_.size() >= 1);
     out << " (";
     for (uint32_t i = 0; i < clause.atoms_.size() - 1; i++) {
       out << clause.atoms_.at(i) << " and ";
     }
     out << clause.atoms_.at(clause.atoms_.size() - 1) << ") ";
     return out;
    }
  };

 private:
  std::vector<Atom> atoms_ = {};
};

/// Disjunctive normal form for Boolean expressions
class Dnf {
 public:
  /// OR of Dnf with a Conjunction, just append to vector
  Dnf operator+(const Conjunction & t_conjunction) const {
    auto ret(*this);
    ret.clauses_.emplace_back(t_conjunction);
    return ret;
  }

  /// OR of 2 Dnfs, just concatenate vectors
  Dnf operator+(const Dnf & t_dnf) const {
    auto ret(*this);
    for (const auto & clause : t_dnf.clauses_) {
      ret.clauses_.emplace_back(clause);
    }
    return ret;
  }

  /// Use the fact that AND distributes over ORs,
  /// and AND each clause in turn
  Dnf operator*(const Conjunction & t_conjunction) const {
    auto ret(*this);
    for (auto & clause : ret.clauses_) {
      clause = clause * t_conjunction;
    }
    return ret;
  }

  friend std::ostream & operator<< (std::ostream & out, const Dnf & dnf) {
    if (dnf.clauses_.empty()) {
      return out;
    } else {
      assert(dnf.clauses_.size() >= 1);
      out << " {";
      for (uint32_t i = 0; i < dnf.clauses_.size() - 1; i++) {
        out << dnf.clauses_.at(i) << " or ";
      }
      out << dnf.clauses_.at(dnf.clauses_.size() - 1) << "} ";
      return out;
    }
  };

 private:
  std::vector<Conjunction> clauses_ = {};
};

#endif  // BOOLEAN_ALGEBRA_H_
