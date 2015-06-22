#ifndef BOOLEAN_ALGEBRA_H_
#define BOOLEAN_ALGEBRA_H_

#include <ostream>
#include <vector>
#include <string>

// Single atom: Variable or negated variable
class Atom {
 public:
  Atom(const std::string & t_name, const bool t_negated) : var_name_(t_name), negated_(t_negated) {}
  friend std::ostream & operator<< (std::ostream & out, const Atom & atom) {
    out << atom.negated_ ? ("~" + atom.var_name_) : atom.var_name_;
    return out;
  };

 private:
  const std::string var_name_;
  const bool negated_;
};

// Conjunction: AND of Atoms,
// each atom either stands by itself or is negated
class Conjunction {
 public:
  explicit Conjunction(const Atom & t_atom) { atoms_.emplace_back(t_atom); }

  Conjunction operator*(const Atom & t_atom) const {
    auto ret(*this);
    ret.atoms_.emplace_back(t_atom);
    return ret;
  }

  Conjunction operator*(const Conjunction & t_conjunction) const {
    // AND of two conjunctions, just concatenate vectors
    auto ret(*this);
    for (const auto & atom : t_conjunction.atoms_) {
      ret.atoms_.emplace_back(atom);
    }
    return ret;
  }

  friend std::ostream & operator<< (std::ostream & out, const Conjunction & clause) {
    out << " (";
    for (uint32_t i = 0; i < clause.atoms_.size() - 1; i++) {
      out << clause.atoms_.at(i) << " and ";
    }
    out << clause.atoms_.at(clause.atoms_.size() - 1) << ") ";
    return out;
  };

 private:
  std::vector<Atom> atoms_ = {};
};

// Disjunctive normal form for Boolean expressions
class Dnf {
 public:
  Dnf operator+(const Conjunction & t_conjunction) const {
    // OR of Dnf with a Conjunction, just append to vector
    auto ret(*this);
    ret.clauses_.emplace_back(t_conjunction);
    return ret;
  }

  Dnf operator+(const Dnf & t_dnf) const {
    // OR of 2 Dnfs, just concatenate vectors
    auto ret(*this);
    for (const auto & clause : t_dnf.clauses_) {
      ret.clauses_.emplace_back(clause);
    }
    return ret;
  }

  Dnf operator*(const Conjunction & t_conjunction) const {
    // Use the fact that AND distributes over ORs,
    // and AND each clause in turn
    auto ret(*this);
    for (auto & clause : ret.clauses_) {
      clause = clause * t_conjunction;
    }
    return ret;
  }

  friend std::ostream & operator<< (std::ostream & out, const Dnf & dnf) {
    out << " {";
    for (uint32_t i = 0; i < dnf.clauses_.size() - 1; i++) {
      out << dnf.clauses_.at(i) << " or ";
    }
    out << dnf.clauses_.at(dnf.clauses_.size() - 1) << "} ";
    return out;
  };

 private:
  std::vector<Conjunction> clauses_ = {};
};

#endif  // BOOLEAN_ALGEBRA_H_
