#ifndef BOOLEAN_ALGEBRA_H_
#define BOOLEAN_ALGEBRA_H_

#include <algorithm>
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

  /// Check if it's a literal and if so, check if its value matches t_val
  bool is_literal(const bool t_val) const { return is_literal_ ? value_ == t_val : false; }

 private:
  std::string var_name_;
  bool pristine_;
  bool is_literal_;
  bool value_;
};

// Conjunction: AND of Atoms,
// each atom either stands by itself or is negated
class Conjunction {
 public:
  /// Check if Conjunction degenerates to a single atom and call is_literal on it
  bool is_literal(const bool t_val) const {
    assert(not atoms_.empty());
    return atoms_.size() == 1 ? atoms_.front().is_literal(t_val) : false;
  }

  explicit Conjunction(const Atom & t_atom) { atoms_.emplace_back(t_atom); }

  /// Simplify Conjunction by constant folding
  void simplify() {
    // Remove true atoms.
    atoms_.erase(std::remove_if(atoms_.begin(), atoms_.end(),
                                [] (const Atom & atom)
                                { return atom.is_literal(true); }),
                                atoms_.end());

    if (atoms_.empty()) {
      // remove + erase deleted all atoms, all of which were true
      // So, leave behind one true Atom
      atoms_ = {Atom::make_literal(true)};
    }

    // If there's a false atom, short circuit.
    auto it = std::find_if(atoms_.begin(), atoms_.end(),
                           [] (const Atom & atom)
                           { return (atom.is_literal(false)); });
    bool check_false = (it != atoms_.end());
    if (check_false) {
      atoms_ = {Atom::make_literal(false)};
    }
  }

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
  /// Simplify Dnf by constant folding
  void simplify() {
    // Simplify individual clauses
    for (auto & clause : clauses_) {
      clause.simplify();
    }

    // Remove false clauses
    clauses_.erase(std::remove_if(clauses_.begin(), clauses_.end(),
                   [] (const Conjunction & clause)
                   { return clause.is_literal(false); }),
                   clauses_.end());

    if (clauses_.empty()) {
      // remove + erase deleted all clauses, all of which were false
      // So, leave behind one false clause
      clauses_ = {Conjunction(Atom::make_literal(false))};
    }

    // Short if there are true clauses
    auto it = std::find_if(clauses_.begin(), clauses_.end(),
                           [] (const Conjunction & clause)
                           { return clause.is_literal(true); });
    bool check_true = (it != clauses_.end());
    if (check_true) {
      clauses_ = {Conjunction(Atom::make_literal(true))};
    }
  }

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
