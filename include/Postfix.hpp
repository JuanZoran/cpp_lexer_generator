#pragma once
#include <Type.hpp>
#include <cassert>
#include <map>
#include <stack>

/**
 * @class Postfix
 * @brief This class is used to convert infix RE to postfix RE
 *
 */
class Postfix
{
public:
    using str = Type::str;
    using map = std::map<char, int>;

public:
    Postfix() = default;
    /**
     * @brief Build a new Postfix object with infix RE
     *
     * @param infix
     */
    Postfix(const std::string& infix);
    ~Postfix() = default;

    Postfix& operator= (Postfix&&) = default;
    Postfix& operator= (const Postfix&) = default;
    Postfix(Postfix&&) = default;
    Postfix(const Postfix&) = default;

    friend std::ostream& operator<< (std::ostream& os, const Postfix& postfix) {
        os << postfix.postfix;
        return os;
    }

    /**
     * @brief Get the Postfix object [Must be called after Constructor with infix]
     *
     * @return Postfix::str
     */
    const str& getPostfix() const;

    /**
     * @brief Get the Postfix object
     *
     * @param infix be converted
     * @return Postfix::str
     */
    const str& getPostfix(const std::string infix);


private:
    void setPostfix(const std::string& infix);


private:
    str postfix;
    static const map Priorities;
};

inline const Postfix::map Postfix::Priorities {
    {'|',  2},
    { '^', 2},
    { '*', 4},
    { '(', 0},
    { ')', 0}
};

inline void Postfix::setPostfix(const std::string& infix) {
    using stack = std::stack<char>;
    auto isOperator = [](const char ch) {
        return Priorities.find(ch) != Priorities.end();
    };


    postfix.clear();
    stack st;


    auto processOperators = [&st, this](const char ch) {
        if (ch == ')') {
            // WARNING : if there is no '(' in the stack, it will cause an error
            while (st.top() != '(') {
                postfix.push_back(st.top());
                st.pop();
            }

            st.pop();
        }
        else if (st.empty()) {
            st.push(ch);
        }
        else {
            while (!st.empty() && Priorities.at(st.top()) >= Priorities.at(ch)) {
                postfix.push_back(st.top());
                st.pop();
            }

            st.push(ch);
        }
    };

    auto processOperands = [this](const char ch) {
        postfix.push_back(ch);
    };

    for (const auto ch : infix)
        if (isOperator(ch))
            processOperators(ch);
        else
            processOperands(ch);

    while (!st.empty()) {
        postfix.push_back(st.top());
        st.pop();
    }
}

inline Postfix::Postfix(const std::string& infix) {
    setPostfix(infix);
}

inline const Postfix::str& Postfix::getPostfix() const {
    // TODO :
    assert(postfix.size() >= 0);
    return postfix;
}

inline const Postfix::str& Postfix::getPostfix(const std::string infix) {
    // TODO :
    setPostfix(infix);

    return postfix;
}
