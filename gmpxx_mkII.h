/*
 * Copyright (c) 2024
 *      Nakata, Maho
 *      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#if __cplusplus < 201703L
#error "This class only runs on C++ 17 and later"
#endif

#include <gmp.h>
#include <limits>
#include <iostream>
#include <utility>
#include <cassert>
#include <cstring>
#include <sstream>

#define ___MPF_CLASS_EXPLICIT___ explicit

#define INT_COND(T, X) typename std::enable_if<std::is_integral<T>::value, X>::type
#define FLOAT_COND(T, X) typename std::enable_if<std::is_float<T>::value, X>::type
#define UNSIGNED_INT_COND(T, X) typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, X>::type
#define SIGNED_INT_COND(T, X) typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, X>::type
#define NON_INT_COND(T, X) typename std::enable_if<std::is_arithmetic<T>::value && !std::is_integral<T>::value, X>::type
#define NON_GMP_COND(T, X) typename std::enable_if<!std::is_same<T, mpf_class>::value && !std::is_same<T, mpq_class>::value && !std::is_same<T, mpz_class>::value, X>::type
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
namespace gmp {
#endif

class gmpxx_defaults {
  public:
    static int base;
    static inline void set_default_prec(const mp_bitcnt_t _prec) { mpf_set_default_prec(_prec); }
    static inline void set_default_prec_raw(const mp_bitcnt_t prec_raw) { mpf_set_default_prec(prec_raw); }
    static inline mp_bitcnt_t get_default_prec() { return mpf_get_default_prec(); }
    static inline void set_default_base(const int _base) { base = _base; }
};

class mpz_class;
class mpq_class;
class mpf_class;

class mpz_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.2 C++ Interface Integers
    // cf. https://gmplib.org/manual/C_002b_002b-Interface-Integers
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructors and destructors
    mpz_class() { mpz_init(value); }
    // The rule of 0/3/5
    // The rule 1 of 5 copy constructor
    mpz_class(const mpz_class &op) {
        mpz_init(value);
        mpz_set(value, op.value);
    }
    // The rule 2 of 5 copy assignment operator
    mpz_class &operator=(const mpz_class &op) noexcept {
        if (this != &op) {
            mpz_set(value, op.value);
        }
        return *this;
    }
    // The rule 3 of 5 default deconstructor
    ~mpz_class() { mpz_clear(value); }
    // The rule 4 of 5 move constructor
    mpz_class(mpz_class &&op) noexcept {
        mpz_init(value);
        mpz_swap(value, op.value);
    }
    // The rule 5 of 5 move assignment operator
    mpz_class &operator=(mpz_class &&op) noexcept {
        if (this != &op) {
            mpz_swap(value, op.value);
        }
        return *this;
    }
    // constructors
    explicit mpz_class(const mpz_t z) {
        mpz_init(value);
        mpz_set(value, z);
    }
    mpz_class(const mpq_t op) {
        mpz_init(value);
        mpz_set_q(value, op);
    }
    mpz_class(const mpf_t op) {
        mpz_init(value);
        mpz_set_f(value, op);
    }
    mpz_class(const char *str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpz_class(const std::string &str, int base = 0) {
        mpz_init(value);
        if (mpz_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpz_class(unsigned long int op) { mpz_init_set_ui(value, op); }
    mpz_class(signed long int op) { mpz_init_set_si(value, op); }
    mpz_class(double op) { mpz_init_set_d(value, op); }
    mpz_class(unsigned int op) { mpz_init_set_ui(value, static_cast<unsigned long int>(op)); }
    mpz_class(signed int op) { mpz_init_set_si(value, static_cast<signed long int>(op)); }
    // assignments from other objects
    mpz_class &operator=(double d) noexcept {
        mpz_set_d(value, d);
        return *this;
    }
    mpz_class &operator=(const char *str) {
        if (mpz_set_str(value, str, 0) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    mpz_class &operator=(const std::string &str) {
        if (mpz_set_str(value, str.c_str(), 0) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    mpz_class &operator=(const signed long int op);
    mpz_class &operator=(const unsigned long int op);
    mpz_class &operator=(const signed int op);
    mpz_class &operator=(const unsigned int op);
    mpz_class &operator=(const signed char op);
    mpz_class &operator=(const unsigned char op);
    mpz_class &operator=(const char op);
    // operators
    mpz_class operator~() const {
        mpz_class result;
        mpz_com(result.value, value);
        return result;
    }
    mpz_class &operator++() {
        mpz_add_ui(value, value, 1);
        return *this;
    }
    mpz_class operator++(int) {
        mpz_class original = *this;
        ++(*this);
        return original;
    }
    mpz_class &operator--() {
        mpz_sub_ui(value, value, 1);
        return *this;
    }
    mpz_class operator--(int) {
        mpz_class original = *this;
        --(*this);
        return original;
    }
    template <typename T> INT_COND(T, mpz_class &) operator<<=(T n) {
        mpz_mul_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> INT_COND(T, mpz_class &) operator>>=(T n) {
        mpz_tdiv_q_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> friend INT_COND(T, mpz_class) operator<<(const mpz_class &op1, T op2) {
        mpz_class result(op1);
        mpz_mul_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }
    template <typename T> friend INT_COND(T, mpz_class) operator>>(const mpz_class &op1, T op2) {
        mpz_class result(op1);
        mpz_fdiv_q_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }

    // mpz_class arithmetic operators
    inline friend mpz_class &operator+=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator-=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator*=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator/=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator%=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator&=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator|=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class &operator^=(mpz_class &lhs, const mpz_class &rhs);
    inline friend mpz_class operator+(const mpz_class &op);
    inline friend mpz_class operator-(const mpz_class &op);
    inline friend mpz_class operator+(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator-(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator*(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator/(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator%(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator&(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator|(const mpz_class &op1, const mpz_class &op2);
    inline friend mpz_class operator^(const mpz_class &op1, const mpz_class &op2);

    // mpz_class comparison operators
    inline friend bool operator==(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.value, op2.value) >= 0; }

    inline friend bool operator==(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) == 0; }
    inline friend bool operator!=(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) != 0; }
    inline friend bool operator<(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) < 0; }
    inline friend bool operator>(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) > 0; }
    inline friend bool operator<=(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) <= 0; }
    inline friend bool operator>=(const mpz_class &op1, const double &op2) { return mpz_cmp_d(op1.value, op2) >= 0; }

    inline friend bool operator==(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) == 0; }
    inline friend bool operator!=(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) != 0; }
    inline friend bool operator<(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) > 0; }
    inline friend bool operator>(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) < 0; }
    inline friend bool operator<=(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) >= 0; }
    inline friend bool operator>=(double &op1, const mpz_class &op2) { return mpz_cmp_d(op2.value, op1) <= 0; }

    // mpz_class comparison operators (template version)
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator==(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) == 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator!=(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) != 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) < 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) > 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<=(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) <= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>=(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) >= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator==(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) == 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator!=(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) != 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) > 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) < 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<=(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) >= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>=(T op1, const mpz_class &op2) { return mpz_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) <= 0; }

    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator==(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) == 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator!=(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) != 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) < 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) > 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<=(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) <= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>=(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.value, static_cast<signed long int>(op2)) >= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator==(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) == 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator!=(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) != 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) > 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) < 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<=(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) >= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>=(T op1, const mpz_class &op2) { return mpz_cmp_si(op2.value, static_cast<signed long int>(op1)) <= 0; }

    template <typename T> inline friend NON_INT_COND(T, bool) operator==(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) == 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator!=(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) != 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator<(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) < 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator>(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) > 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator<=(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) <= 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator>=(const mpz_class &op1, T op2) { return mpz_cmp(op1.value, mpz_class(op2).get_mpz_t()) >= 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator==(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) == 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator!=(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) != 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator<(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) > 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator>(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) < 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator<=(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) >= 0; }
    template <typename T> inline friend NON_INT_COND(T, bool) operator>=(T op1, const mpz_class &op2) { return mpz_cmp(op2.value, mpz_class(op1).get_mpz_t()) <= 0; }

    // mpz_class arithmetic and logical operators (template version)
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2);

    // mpz_class abs (mpz_class op)
    inline friend mpz_class abs(const mpz_class &op);

    // bool mpz_class::fits_sint_p (void)
    // bool mpz_class::fits_slong_p (void)
    // bool mpz_class::fits_sshort_p (void)
    // bool mpz_class::fits_uint_p (void)
    // bool mpz_class::fits_ulong_p (void)
    // bool mpz_class::fits_ushort_p (void)
    inline bool fits_sint_p() const { return mpz_fits_sint_p(value) != 0; }
    inline bool fits_slong_p() const { return mpz_fits_slong_p(value) != 0; }
    inline bool fits_sshort_p() const { return mpz_fits_sshort_p(value) != 0; }
    inline bool fits_uint_p() const { return mpz_fits_uint_p(value) != 0; }
    inline bool fits_ulong_p() const { return mpz_fits_ulong_p(value) != 0; }
    inline bool fits_ushort_p() const { return mpz_fits_ushort_p(value) != 0; }

    // double mpz_class::get_d (void)
    // long mpz_class::get_si (void)
    // unsigned long mpz_class::get_ui (void)
    inline double get_d() const { return mpz_get_d(value); }
    inline signed long int get_si() const { return mpz_get_si(value); }
    inline unsigned long int get_ui() const { return mpz_get_ui(value); }
    // string mpz_class::get_str (int base = 10)
    inline std::string get_str(int base = 10) const {
        char *temp = mpz_get_str(nullptr, base, value);
        std::string result(temp);
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(nullptr, nullptr, &freefunc);
        freefunc(temp, std::strlen(temp) + 1);
        return result;
    }
    // int mpz_class::set_str (const char *str, int base)
    // int mpz_class::set_str (const string& str, int base)
    int set_str(const char *str, int base) { return mpz_set_str(value, str, base); }
    int set_str(const std::string &str, int base) { return mpz_set_str(value, str.c_str(), base); }

    // int sgn (mpz_class op)
    // mpz_class sqrt (mpz_class op)
    // mpz_class gcd (mpz_class op1, mpz_class op2)
    // mpz_class lcm (mpz_class op1, mpz_class op2)
    friend int sgn(const mpz_class &op);
    friend mpz_class sqrt(const mpz_class &op);
    friend mpz_class gcd(const mpz_class &op1, const mpz_class &op2);
    friend mpz_class lcm(const mpz_class &op1, const mpz_class &op2);

    // mpz_class mpz_class::factorial (type op)
    // mpz_class factorial (mpz_class op)
    // mpz_class mpz_class::fibonacci (type op)
    // mpz_class fibonacci (mpz_class op)
    // mpz_class mpz_class::primorial (type op)
    // mpz_class primorial (mpz_class op)
    static mpz_class factorial(const mpz_class &n) {
        if (n < 0) {
            throw std::domain_error("factorial(negative)");
        }
        double log2_n = mpz_sizeinbase(n.get_mpz_t(), 2);
        if (log2_n > 300) {
            throw std::bad_alloc();
        }
        mpz_class result;
        try {
            mpz_fac_ui(result.get_mpz_t(), n.get_ui());
        } catch (const std::bad_alloc &) {
            throw;
        }
        return result;
    }
    static mpz_class primorial(const mpz_class &op) {
        if (op < 0) {
            throw std::domain_error("primorial(negative)");
        }
        double log2_n = mpz_sizeinbase(op.get_mpz_t(), 2);
        if (log2_n > 300) {
            throw std::bad_alloc();
        }
        mpz_class result;
        try {
            mpz_primorial_ui(result.get_mpz_t(), op.get_ui());
        } catch (const std::bad_alloc &) {
            throw;
        }
        return result;
    }
    friend mpz_class primorial(const mpz_class &op);
    static mpz_class fibonacci(const mpz_class &op) {
        double log2_op = mpz_sizeinbase(op.get_mpz_t(), 2);
        if (log2_op > 300) {
            throw std::bad_alloc();
        }
        mpz_class adjusted_op = op;
        bool isNegative = op < 0;
        if (isNegative) {
            adjusted_op = -op;
        }
        unsigned long int n = adjusted_op.get_ui();
        mpz_class result;
        mpz_fib_ui(result.get_mpz_t(), n);
        if (isNegative) {
            if ((op + 1) % 2 != 0) {
                result = -result;
            }
        }
        return result;
    }
    friend mpz_class fibonacci(const mpz_class &op);

    // void mpz_class::swap (mpz_class& op)
    // void swap (mpz_class& op1, mpz_class& op2)
    void swap(mpz_class &op) { mpz_swap(this->value, op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpz_class &op1, mpz_class &op2) { mpz_swap(op1.value, op2.value); }
#endif
    friend std::ostream &operator<<(std::ostream &os, const mpz_class &op);
    friend std::ostream &operator<<(std::ostream &os, const mpz_t op);

    friend std::istream &operator>>(std::istream &stream, mpz_class &op);
    friend std::istream &operator>>(std::istream &stream, mpz_t op);

    // casts
    operator mpf_class() const;
    operator mpq_class() const;
    operator unsigned long int() const { return mpz_get_ui(this->value); }
    operator signed long int() const { return mpz_get_si(this->value); }
    operator unsigned int() const { return static_cast<unsigned int>(mpz_get_ui(this->value)); }
    operator signed int() const { return static_cast<signed int>(mpz_get_si(this->value)); }

    mpz_srcptr get_mpz_t() const { return value; }
    mpz_ptr get_mpz_t() { return value; }

  private:
    mpz_t value;
};
inline mpz_class &operator+=(mpz_class &op1, const mpz_class &op2) {
    mpz_add(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator-=(mpz_class &op1, const mpz_class &op2) {
    mpz_sub(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator*=(mpz_class &op1, const mpz_class &op2) {
    mpz_mul(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator/=(mpz_class &op1, const mpz_class &op2) {
    mpz_tdiv_q(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class &operator&=(mpz_class &lhs, const mpz_class &rhs) {
    mpz_and(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpz_class &operator|=(mpz_class &lhs, const mpz_class &rhs) {
    mpz_ior(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpz_class &operator^=(mpz_class &lhs, const mpz_class &rhs) {
    mpz_xor(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpz_class &operator%=(mpz_class &op1, const mpz_class &op2) {
    mpz_tdiv_r(op1.value, op1.value, op2.value);
    return op1;
}
inline mpz_class operator+(const mpz_class &op) { return op; }
inline mpz_class operator-(const mpz_class &op) {
    mpz_class result;
    mpz_neg(result.value, op.value);
    return result;
}
inline mpz_class operator+(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_add(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator-(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_sub(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator*(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_mul(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator/(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_tdiv_q(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator%(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_tdiv_r(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator&(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_and(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator|(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_ior(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class operator^(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_xor(result.value, op1.value, op2.value);
    return result;
}

// +=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs) {
    mpz_add_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs) {
    if (rhs >= 0)
        mpz_add_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    else {
        unsigned long int _rhs = -rhs;
        mpz_sub_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator+=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_add(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// +
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    mpz_add_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2) {
    mpz_class result(op2);
    mpz_add_ui(result.value, result.value, static_cast<unsigned long int>(op1));
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_add_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    else {
        unsigned long int _op2 = -op2;
        mpz_sub_ui(result.value, result.value, static_cast<unsigned long int>(_op2));
    }
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2) {
    mpz_class result(op2);
    if (op1 >= 0)
        mpz_add_ui(result.value, result.value, static_cast<unsigned long int>(op1));
    else {
        unsigned long int _op1 = -op1;
        mpz_sub_ui(result.value, result.value, static_cast<unsigned long int>(_op1));
    }
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator+(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    result += op2;
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator+(const T op1, const mpz_class &op2) { return op2 + op1; }

// -=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs) {
    mpz_sub_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs) {
    if (rhs >= 0)
        mpz_sub_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    else {
        unsigned long int _rhs = -rhs;
        mpz_add_ui(lhs.value, lhs.value, _rhs);
    }
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator-=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_sub(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// -
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2) {
    mpz_class result;
    mpz_sub_ui(result.value, op1.value, op2);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ui_sub(result.value, op1, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_sub_ui(result.value, op1.value, static_cast<unsigned long int>(op2));
    else {
        unsigned long int _op2 = static_cast<unsigned long int>(-op2);
        mpz_add_ui(result.value, op1.value, _op2);
    }
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2) {
    mpz_class result;
    if (op1 >= 0) {
        mpz_ui_sub(result.value, static_cast<unsigned long int>(op1), op2.value);
    } else {
        unsigned long int _op1 = static_cast<unsigned long int>(-op1);
        mpz_add_ui(result.value, op2.value, _op1);
        mpz_neg(result.value, result.value);
    }
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator-(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_sub(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator-(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_sub(result.value, result.value, op2.value);
    return result;
}

// *=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs) {
    mpz_mul_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs) {
    mpz_mul_si(lhs.value, lhs.value, static_cast<signed long int>(rhs));
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator*=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_mul(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// *
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    mpz_mul_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2) {
    mpz_class result(op2);
    mpz_mul_ui(result.value, result.value, static_cast<unsigned long int>(op1));
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    mpz_mul_si(result.value, result.value, static_cast<signed long int>(op2));
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2) {
    mpz_class result(op2);
    mpz_mul_si(result.value, result.value, static_cast<signed long int>(op1));
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator*(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_mul(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator*(const T op1, const mpz_class &op2) { return op2 * op1; }

// /=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs) {
    mpz_tdiv_q_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs) {
    if (rhs >= 0)
        mpz_tdiv_q_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    else {
        unsigned long int _rhs = -static_cast<unsigned long int>(rhs);
        mpz_tdiv_q_ui(lhs.value, lhs.value, static_cast<unsigned long int>(_rhs));
        mpz_neg(lhs.value, lhs.value);
    }
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator/=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_tdiv_q(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// /
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    mpz_tdiv_q_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_q(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_tdiv_q_ui(result.value, result.value, static_cast<signed long int>(op2));
    else {
        unsigned long int _op2 = -static_cast<unsigned long int>(op2);
        mpz_tdiv_q_ui(result.value, result.value, static_cast<signed long int>(_op2));
        mpz_neg(result.value, result.value);
    }
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_q(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator/(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_tdiv_q(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator/(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_q(result.value, result.value, op2.value);
    return result;
}
// %=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs) {
    mpz_tdiv_r_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs) {
    if (rhs >= 0)
        mpz_tdiv_r_ui(lhs.value, lhs.value, static_cast<unsigned long int>(rhs));
    else {
        unsigned long int _rhs = -static_cast<unsigned long int>(rhs);
        mpz_tdiv_r_ui(lhs.value, lhs.value, static_cast<unsigned long int>(_rhs));
    }
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator%=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_tdiv_r(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// %
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    mpz_tdiv_r_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_r(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2) {
    mpz_class result(op1);
    if (op2 >= 0)
        mpz_tdiv_r_ui(result.value, result.value, static_cast<unsigned long int>(op2));
    else {
        unsigned long int _op2 = -static_cast<unsigned long int>(op2);
        mpz_tdiv_r_ui(result.value, result.value, static_cast<signed long int>(_op2));
    }
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_r(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator%(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_tdiv_r(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator%(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_tdiv_r(result.value, result.value, op2.value);
    return result;
}
// &=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_and(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_and(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator&=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_and(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// &
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_and(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_and(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_and(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_and(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator&(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_and(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator&(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_and(result.value, result.value, op2.value);
    return result;
}
// |=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_ior(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_ior(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator|=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_ior(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// |
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_ior(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ior(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_ior(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ior(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator|(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_ior(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator|(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_ior(result.value, result.value, op2.value);
    return result;
}
// ^=
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_xor(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_xor(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpz_class &) operator^=(mpz_class &lhs, const T rhs) {
    mpz_class _rhs(rhs);
    mpz_xor(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
// ^
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_xor(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_xor(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_xor(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_xor(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator^(const mpz_class &op1, const T op2) {
    mpz_class result(op2);
    mpz_xor(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpz_class) operator^(const T op1, const mpz_class &op2) {
    mpz_class result(op1);
    mpz_xor(result.value, result.value, op2.value);
    return result;
}

/////
inline mpz_class &mpz_class::operator=(const signed long int op) {
    mpz_set_si(this->value, op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned long int op) {
    mpz_set_ui(this->value, op);
    return *this;
}
inline mpz_class &mpz_class::operator=(const signed int op) {
    mpz_set_si(this->value, static_cast<signed long int>(op));
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned int op) {
    mpz_set_ui(this->value, static_cast<unsigned long int>(op));
    return *this;
}
inline mpz_class &mpz_class::operator=(const signed char op) {
    mpz_set_si(this->value, static_cast<signed long int>(op));
    return *this;
}
inline mpz_class &mpz_class::operator=(const unsigned char op) {
    mpz_set_ui(this->value, static_cast<unsigned long int>(op));
    return *this;
}
inline mpz_class &mpz_class::operator=(const char op) {
    if (std::is_signed<char>::value)
        return *this = static_cast<signed char>(op);
    else
        return *this = static_cast<unsigned char>(op);
}
inline mpz_class abs(const mpz_class &op) {
    mpz_class result;
    mpz_abs(result.value, op.value);
    return result;
}
inline int sgn(const mpz_class &op) { return mpz_sgn(op.value); }
inline mpz_class sqrt(const mpz_class &op) {
    mpz_class result;
    mpz_sqrt(result.value, op.value);
    return result;
}
inline mpz_class gcd(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_gcd(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class lcm(const mpz_class &op1, const mpz_class &op2) {
    mpz_class result;
    mpz_lcm(result.value, op1.value, op2.value);
    return result;
}
inline mpz_class factorial(const mpz_class &n) {
    if (n < 0) {
        throw std::domain_error("factorial(negative)");
    }
    double log2_n = mpz_sizeinbase(n.get_mpz_t(), 2);
    if (log2_n > 300) {
        throw std::bad_alloc();
    }
    mpz_class result;
    try {
        mpz_fac_ui(result.get_mpz_t(), n.get_ui());
    } catch (const std::bad_alloc &) {
        throw;
    }
    return result;
}
inline mpz_class primorial(const mpz_class &op) {
    if (op < 0) {
        throw std::domain_error("primorial(negative)");
    }
    double log2_n = mpz_sizeinbase(op.get_mpz_t(), 2);
    if (log2_n > 300) {
        throw std::bad_alloc();
    }
    mpz_class result;
    try {
        mpz_primorial_ui(result.get_mpz_t(), op.get_ui());
    } catch (const std::bad_alloc &) {
        throw;
    }
    return result;
}
inline mpz_class fibonacci(const mpz_class &op) {
    double log2_op = mpz_sizeinbase(op.get_mpz_t(), 2);
    if (log2_op > 300) {
        throw std::bad_alloc();
    }
    mpz_class adjusted_op = op;
    bool isNegative = op < 0;
    if (isNegative) {
        adjusted_op = -op;
    }
    unsigned long int n = adjusted_op.get_ui();
    mpz_class result;
    mpz_fib_ui(result.get_mpz_t(), n);
    if (isNegative) {
        if ((op + 1) % 2 != 0) {
            result = -result;
        }
    }
    return result;
}
void print_mpz(std::ostream &os, const mpz_t op) {
    std::ios_base::fmtflags flags = os.flags();
    std::streamsize width = os.width();
    char fill = os.fill();
    char *str = nullptr;
    bool is_hex = flags & std::ios::hex;
    bool is_oct = flags & std::ios::oct;
    bool show_base = flags & std::ios::showbase;
    bool uppercase = flags & std::ios::uppercase;

    if (mpz_sgn(op) == 0) {
        if (is_hex && show_base) {
            str = strdup(uppercase ? "0X0" : "0x0");
        } else if (is_oct) {
            str = strdup("0");
        } else {
            str = strdup("0");
        }
    } else {
        if (is_oct) {
            gmp_asprintf(&str, (show_base) ? "%#Zo" : "%Zo", op);
        } else if (is_hex) {
            if (show_base && uppercase) {
                gmp_asprintf(&str, "%#ZX", op);
            } else if (show_base && !uppercase) {
                gmp_asprintf(&str, "%#Zx", op);
            } else if (!show_base && uppercase) {
                gmp_asprintf(&str, "%ZX", op);
            } else {
                gmp_asprintf(&str, "%Zx", op);
            }
        } else {
            gmp_asprintf(&str, "%Zd", op);
        }
    }
    std::string s(str);
    free(str);

    if (flags & std::ios::showpos && mpz_sgn(op) > 0) {
        s.insert(0, "+");
    }

    std::streamsize len = s.length();
    if (len < width) {
        std::streamsize padding_length = width - len;
        if (flags & std::ios::left) {
            s.append(padding_length, fill);
        } else if (flags & std::ios::internal) {
            size_t pos = 0;
            if (s[0] == '-' || s[0] == '+') {
                pos = 1;
            }
            if (s.length() > pos + 1 && (s[pos] == '0' && (s[pos + 1] == 'x' || s[pos + 1] == 'X'))) {
                pos += 2;
            }
            s.insert(pos, padding_length, fill);
        } else {
            s.insert(0, padding_length, fill);
        }
    }
    os << s;
    os.width(0);
}
std::ostream &operator<<(std::ostream &os, const mpz_class &op) {
    print_mpz(os, op.get_mpz_t());
    return os;
}
std::ostream &operator<<(std::ostream &os, const mpz_t &op) {
    print_mpz(os, op);
    return os;
}
std::istream &operator>>(std::istream &stream, mpz_class &op) {
    std::string input;
    stream >> input;
    if (mpz_set_str(op.value, input.c_str(), 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
std::istream &operator>>(std::istream &stream, mpz_t op) {
    std::string input;
    stream >> input;
    if (mpz_set_str(op, input.c_str(), 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
class mpq_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.3 C++ Interface Rationals
    // cf. https://gmplib.org/manual/C_002b_002b-Interface-Rationals
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructors and destructors
    mpq_class() { mpq_init(value); }
    // The rule of 0/3/5
    // The rule 1 of 5 copy constructor
    mpq_class(const mpq_class &op) {
        mpq_init(value);
        mpq_set(value, op.value);
    }
    // The rule 2 of 5 copy assignment operator
    mpq_class &operator=(const mpq_class &op) noexcept {
        if (this != &op) {
            mpq_set(value, op.value);
        }
        return *this;
    }
    // The rule 3 of 5 default deconstructor
    ~mpq_class() { mpq_clear(value); }
    // The rule 4 of 5 move constructor
    mpq_class(mpq_class &&op) noexcept {
        mpq_init(value);
        mpq_swap(value, op.value);
    }
    // The rule 5 of 5 move assignment operator
    mpq_class &operator=(mpq_class &&op) noexcept {
        if (this != &op) {
            mpq_swap(value, op.value);
        }
        return *this;
    }
    // constructors
    explicit mpq_class(const mpq_t q) {
        mpq_init(value);
        mpq_set(value, q);
    }
    mpq_class(const mpz_t op) {
        mpq_init(value);
        mpq_set_z(value, op);
    }
    mpq_class(const mpf_t op) {
        mpq_init(value);
        mpq_set_f(value, op);
    }
    mpq_class(const mpz_class &op1, const mpz_class &op2) {
        mpq_init(value);
        mpq_set_num(value, op1.get_mpz_t());
        mpq_set_den(value, op2.get_mpz_t());
        if (op2 == 0) {
            mpq_clear(value);
            throw std::invalid_argument("Denominator cannot be zero in a rational number.");
        }
        mpq_canonicalize(value);
    }
    mpq_class(const mpz_class &op) {
        mpq_init(value);
        mpq_set_z(value, op.get_mpz_t());
    }
    mpq_class(const char *str, int base = 0) {
        mpq_init(value);
        if (mpq_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpq_class(const std::string &str, int base = 0) {
        mpq_init(value);
        if (mpq_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpq_class(unsigned long int op1, unsigned long int op2) {
        mpq_init(value);
        mpq_set_ui(value, op1, op2);
    }
    mpq_class(signed long int op1, signed long int op2) {
        mpq_init(value);
        mpq_set_si(value, op1, op2);
    }
    mpq_class(unsigned int op1, unsigned int op2) {
        mpq_init(value);
        mpq_set_ui(value, static_cast<unsigned long int>(op1), static_cast<unsigned long int>(op2));
    }
    mpq_class(int op1, int op2) {
        mpq_init(value);
        mpq_set_si(value, static_cast<signed long int>(op1), static_cast<signed long int>(op2));
    }
    mpq_class(unsigned long int op) {
        mpq_init(value);
        mpq_set_ui(value, op, static_cast<unsigned long int>(1));
    }
    mpq_class(signed long int op) {
        mpq_init(value);
        mpq_set_si(value, op, static_cast<unsigned long int>(1));
    }
    mpq_class(unsigned int op) {
        mpq_init(value);
        mpq_set_ui(value, static_cast<unsigned long int>(op), static_cast<unsigned long int>(1));
    }
    mpq_class(int op) {
        mpq_init(value);
        mpq_set_si(value, static_cast<signed long int>(op), static_cast<signed long int>(1));
    }
    mpq_class(double op) {
        mpq_init(value);
        mpq_set_d(value, op);
    }

    // assignments from other objects
    inline mpq_class &operator=(const mpz_class &op);
    inline mpq_class &operator=(const signed long int op);
    inline mpq_class &operator=(const unsigned long int op);
    inline mpq_class &operator=(const signed int op);
    inline mpq_class &operator=(const unsigned int op);
    inline mpq_class &operator=(const signed char op);
    inline mpq_class &operator=(const unsigned char op);
    inline mpq_class &operator=(const char op);
    inline mpq_class &operator=(const float op);
    inline mpq_class &operator=(const double op);
    inline mpq_class &operator=(const char *op);
    inline mpq_class &operator=(const std::string &op);

    // operators
    mpq_class &operator++() {
        mpq_class one = 1;
        mpq_add(value, value, one.value);
        return *this;
    }

    mpq_class operator++(int) {
        mpq_class original = *this;
        ++(*this);
        return original;
    }
    mpq_class &operator--() {
        mpq_class one = 1;
        mpq_sub(value, value, one.value);
        return *this;
    }
    mpq_class operator--(int) {
        mpq_class original = *this;
        --(*this);
        return original;
    }
    template <typename T> INT_COND(T, mpq_class &) operator<<=(T n) {
        mpq_mul_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> INT_COND(T, mpq_class &) operator>>=(T n) {
        mpq_div_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> friend INT_COND(T, mpq_class) operator<<(const mpq_class &op1, T op2) {
        mpq_class result(op1);
        mpq_mul_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }
    template <typename T> friend INT_COND(T, mpq_class) operator>>(const mpq_class &op1, T op2) {
        mpq_class result(op1);
        mpq_div_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }

    // mpq_class arithmetic operators
    inline friend mpq_class &operator+=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator-=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator*=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class &operator/=(mpq_class &lhs, const mpq_class &rhs);
    inline friend mpq_class operator+(const mpq_class &op);
    inline friend mpq_class operator-(const mpq_class &op);
    inline friend mpq_class operator+(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator-(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator*(const mpq_class &op1, const mpq_class &op2);
    inline friend mpq_class operator/(const mpq_class &op1, const mpq_class &op2);

    // mpq_class comparison operators
    inline friend bool operator==(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.value, op2.value) >= 0; }

    inline friend bool operator==(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) == 0; }
    inline friend bool operator!=(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) != 0; }
    inline friend bool operator<(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) < 0; }
    inline friend bool operator>(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) > 0; }
    inline friend bool operator<=(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) <= 0; }
    inline friend bool operator>=(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.value, op2.get_mpz_t()) >= 0; }
    inline friend bool operator==(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) == 0; }
    inline friend bool operator!=(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) != 0; }
    inline friend bool operator<(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) > 0; }
    inline friend bool operator>(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) < 0; }
    inline friend bool operator<=(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) >= 0; }
    inline friend bool operator>=(const mpz_class &op1, const mpq_class &op2) { return mpq_cmp_z(op2.value, op1.get_mpz_t()) <= 0; }

    // mpq_class comparison operators (template version)
    template <typename T> inline friend NON_GMP_COND(T, bool) operator==(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) == 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator!=(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) != 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator<(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) < 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator>(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) > 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator<=(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) <= 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator>=(const mpq_class &op1, T op2) { return mpq_cmp(op1.value, mpq_class(op2).get_mpq_t()) >= 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator==(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) == 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator!=(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) != 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator<(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) > 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator>(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) < 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator<=(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) >= 0; }
    template <typename T> inline friend NON_GMP_COND(T, bool) operator>=(T op1, const mpq_class &op2) { return mpq_cmp(op2.value, mpq_class(op1).get_mpq_t()) <= 0; }

    // mpq_class arithmetic operators (template version)
    template <typename T> inline friend mpq_class &operator+=(mpq_class &lhs, const T rhs) {
        mpq_class _rhs(rhs);
        mpq_add(lhs.value, lhs.value, _rhs.value);
        return lhs;
    }
    template <typename T> inline friend mpq_class operator+(const mpq_class &op1, const T op2) {
        mpq_class result(op2);
        mpq_add(result.value, op1.value, result.value);
        return result;
    }
    template <typename T> inline friend mpq_class operator+(const T op1, const mpq_class &op2) { return op2 + op1; }
    template <typename T> inline friend mpq_class &operator-=(mpq_class &lhs, const T rhs) {
        mpq_class _rhs(rhs);
        mpq_sub(lhs.value, lhs.value, _rhs.value);
        return lhs;
    }
    template <typename T> inline friend mpq_class operator-(const mpq_class &op1, const T op2) {
        mpq_class result(op2);
        mpq_sub(result.value, op1.value, result.value);
        return result;
    }
    template <typename T> inline friend mpq_class operator-(const T op1, const mpq_class &op2) {
        mpq_class result(op1);
        mpq_sub(result.value, result.value, op2.value);
        return result;
    }
    template <typename T> inline friend mpq_class &operator*=(mpq_class &lhs, const T rhs) {
        mpq_class _rhs(rhs);
        mpq_mul(lhs.value, lhs.value, _rhs.value);
        return lhs;
    }
    template <typename T> inline friend mpq_class operator*(const mpq_class &op1, const T op2) {
        mpq_class result(op2);
        mpq_mul(result.value, op1.value, result.value);
        return result;
    }
    template <typename T> inline friend mpq_class operator*(const T op1, const mpq_class &op2) { return op2 * op1; }
    template <typename T> inline friend mpq_class &operator/=(mpq_class &lhs, const T rhs) {
        mpq_class _rhs(rhs);
        mpq_div(lhs.value, lhs.value, _rhs.value);
        return lhs;
    }
    template <typename T> inline friend mpq_class operator/(const mpq_class &op1, const T op2) {
        mpq_class result(op2);
        mpq_div(result.value, op1.value, result.value);
        return result;
    }
    template <typename T> inline friend mpq_class operator/(const T op1, const mpq_class &op2) {
        mpq_class result(op1);
        mpq_div(result.value, result.value, op2.value);
        return result;
    }
    // void mpq_class::canonicalize ()
    // mpq_class abs (mpq_class op)
    // double mpq_class::get_d (void)
    void canonicalize() { mpq_canonicalize(value); }
    friend mpq_class abs(const mpq_class &op);
    double get_d() const { return mpq_get_d(value); }

    // string mpq_class::get_str (int base = 10)
    // int mpq_class::set_str (const char *str, int base)
    // int mpq_class::set_str (const string& str, int base)
    std::string get_str(int base = 10) const {
        char *str = mpq_get_str(NULL, base, value);
        std::string result(str);
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(NULL, NULL, &freefunc);
        freefunc(str, std::strlen(str) + 1);
        return result;
    }
    int set_str(const char *str, int base = 10) {
        int ret = mpq_set_str(value, str, base);
        if (ret == 0) {
            mpq_canonicalize(value);
        }
        return ret;
    }
    int set_str(const std::string &str, int base = 10) { return set_str(str.c_str(), base); }
    // int sgn (mpq_class op)
    // void mpq_class::swap (mpq_class& op)
    // void swap (mpq_class& op1, mpq_class& op2)
    void swap(mpq_class &op) { mpq_swap(this->value, op.value); }
    friend int sgn(const mpq_class &op) { return mpq_sgn(op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpq_class &op1, mpq_class &op2) { mpq_swap(op1.value, op2.value); }
#endif

    // mpz_class& mpq_class::get_num ()
    // mpz_class& mpq_class::get_den ()
    // mpz_t mpq_class::get_num_mpz_t ()
    // mpz_t mpq_class::get_den_mpz_t ()
    mpz_class get_num() const {
        mpz_class num(mpq_numref(value));
        return num;
    }
    mpz_class get_den() const {
        mpz_class den(mpq_denref(value));
        return den;
    }
    mpz_srcptr get_num_mpz_t() const { return mpq_numref(value); }
    mpz_srcptr get_den_mpz_t() const { return mpq_denref(value); }
    mpz_ptr get_num_mpz_t() { return mpq_numref(value); }
    mpz_ptr get_den_mpz_t() { return mpq_denref(value); }

    // istream& operator>> (istream& stream, mpq_class& rop)
    friend std::ostream &operator<<(std::ostream &os, const mpq_class &op);
    friend std::ostream &operator<<(std::ostream &os, const mpq_t op);

    friend std::istream &operator>>(std::istream &stream, mpq_class &op);
    friend std::istream &operator>>(std::istream &stream, mpq_t op);

    operator mpf_class() const;
    operator mpz_class() const;
    mpq_srcptr get_mpq_t() const { return value; }
    mpq_ptr get_mpq_t() { return value; }

  private:
    mpq_t value;
};

inline mpq_class &mpq_class::operator=(const mpz_class &op) {
    mpq_set_z(this->value, op.get_mpz_t());
    return *this;
}
inline mpq_class &mpq_class::operator=(signed long int op) {
    mpq_set_si(this->value, op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned long int op) {
    mpq_set_ui(this->value, op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(signed int op) {
    mpq_set_si(this->value, (signed long int)op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned int op) {
    mpq_set_ui(this->value, (unsigned long int)op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(signed char op) {
    mpq_set_si(this->value, (signed long int)op, (signed long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(unsigned char op) {
    mpq_set_ui(this->value, (unsigned long int)op, (unsigned long int)1);
    return *this;
}
inline mpq_class &mpq_class::operator=(char op) {
    if (std::is_signed<char>::value)
        return *this = static_cast<signed char>(op);
    else
        return *this = static_cast<unsigned char>(op);
}
inline mpq_class &mpq_class::operator=(float op) {
    mpq_set_d(this->value, (double)op);
    return *this;
}
inline mpq_class &mpq_class::operator=(double op) {
    mpq_set_d(this->value, op);
    return *this;
}
inline mpq_class &mpq_class::operator=(const char *op) {
    if (mpq_set_str(value, op, 10) != 0) {
        throw std::invalid_argument("Invalid string format for mpq_class");
    }
    return *this;
}
inline mpq_class &mpq_class::operator=(const std::string &op) {
    if (mpq_set_str(value, op.c_str(), 10) != 0) {
        throw std::invalid_argument("Invalid string format for mpq_class");
    }
    return *this;
}

inline mpq_class &operator+=(mpq_class &op1, const mpq_class &op2) {
    mpq_add(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator-=(mpq_class &op1, const mpq_class &op2) {
    mpq_sub(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator/=(mpq_class &op1, const mpq_class &op2) {
    mpq_div(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class &operator*=(mpq_class &op1, const mpq_class &op2) {
    mpq_mul(op1.value, op1.value, op2.value);
    return op1;
}
inline mpq_class operator+(const mpq_class &op) { return op; }
inline mpq_class operator-(const mpq_class &op) {
    mpq_class result;
    mpq_neg(result.value, op.value);
    return result;
}
inline mpq_class operator+(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_add(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator-(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_sub(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator*(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_mul(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class operator/(const mpq_class &op1, const mpq_class &op2) {
    mpq_class result;
    mpq_div(result.value, op1.value, op2.value);
    return result;
}
inline mpq_class abs(const mpq_class &op) {
    mpq_class rop(op);
    mpq_abs(rop.value, op.get_mpq_t());
    return rop;
}
void print_mpq(std::ostream &os, const mpq_t op) {
    std::ios_base::fmtflags flags = os.flags();
    std::streamsize width = os.width();
    char fill = os.fill();
    char *str = nullptr;
    bool is_hex = flags & std::ios::hex;
    bool is_oct = flags & std::ios::oct;
    bool show_base = flags & std::ios::showbase;
    bool uppercase = flags & std::ios::uppercase;
    std::string format;

    mpz_class den(mpq_denref(op));
    mpz_class num(mpq_numref(op));

    if ((num == 0 && den == 1) || (num == 0 && den == 0)) {
        if (is_oct) {         // is_oct, (show_base can be ignored since octal 0 = 0 or 00, and we use 0).
            if (width == 0) { // is_oct, width==0
                str = strdup("0");
            } else { // is_oct, width!=0
                str = strdup("0/0");
            }
        } else if (is_hex) {
            if (show_base) {      // is_hex, show_base
                if (width == 0) { // is_hex, show_base, width==0
                    str = strdup(uppercase ? "0X0" : "0x0");
                } else {
                    if (uppercase) {
                        str = strdup("0X0/0X0");
                    } else {
                        str = strdup("0x0/0x0");
                    }
                }
            } else {              // is_hex
                if (width == 0) { // is_hex, width==0
                    str = strdup("0");
                } else { // is_hex, width!=0
                    str = strdup("0/0");
                }
            }
        } else {              // is_dec
            if (width == 0) { // is_dec, width==0
                str = strdup("0");
            } else { // is_dec, width!=0
                str = strdup("0/0");
            }
        }
    } else if (den == 0) {
        if (is_oct) {        // is_oct, (show_base can be ignored since octal 0 = 0 or 00, and we use 0).
            if (show_base) { // is_oct, show_base
                gmp_asprintf(&str, "%#Qo", op);
            } else { // is_oct
                gmp_asprintf(&str, "%Qo", op);
            }
        } else if (is_hex) {
            if (show_base) { // is_hex, show_base
                if (uppercase) {
                    gmp_asprintf(&str, "%#QX", op);
                } else {
                    gmp_asprintf(&str, "%#Qx", op);
                }
            } else { // is_hex
                if (uppercase) {
                    gmp_asprintf(&str, "%QX", op);
                } else {
                    gmp_asprintf(&str, "%Qx", op);
                }
            }
            // Add 'x0' to "/0" to make it "/0x0"
            char *slashZero = strstr(str, "/0");
            size_t newLen = strlen(str) + 2;
            char *newStr = (char *)malloc(newLen + 1);
            if (!newStr) {
                free(str);
                throw std::bad_alloc();
            }
            size_t offset = slashZero - str;
            strncpy(newStr, str, offset);
            newStr[offset] = '\0';

            strcat(newStr, uppercase ? "/0X0" : "/0x0");
            strcat(newStr, slashZero + 2);
            free(str);
            str = newStr;
        } else { // is_dec
            gmp_asprintf(&str, "%Qd", op);
        }
    } else if (num == 0 && den != 1 && den != 0) {
        if (is_oct) {        // is_oct, (show_base can be ignored since octal 0 = 0 or 00, and we use 0).
            if (show_base) { // is_oct, show_base
                gmp_asprintf(&str, "%#Qo", op);
            } else { // is_oct
                gmp_asprintf(&str, "%Qo", op);
            }
        } else if (is_hex) {
            if (show_base) { // is_hex, show_base
                if (uppercase) {
                    gmp_asprintf(&str, "%#QX", op);
                } else {
                    gmp_asprintf(&str, "%#Qx", op);
                }
                // Add 'x0' to "0/" to make it "0x0/"
                char *zeroSlash = strstr(str, "0/");
                if (zeroSlash) {
                    size_t newLen = strlen(str) + 2;
                    char *newStr = (char *)malloc(newLen + 1);
                    if (!newStr) {
                        free(str);
                        throw std::bad_alloc();
                    }
                    size_t offset = zeroSlash - str;
                    strncpy(newStr, str, offset + 1);
                    newStr[offset + 1] = '\0';
                    strcat(newStr, uppercase ? "X0/" : "x0/");
                    strcat(newStr, zeroSlash + 2);
                    free(str);
                    str = newStr;
                }
            } else { // is_hex
                if (uppercase) {
                    gmp_asprintf(&str, "%QX", op);
                } else {
                    gmp_asprintf(&str, "%Qx", op);
                }
            }
        } else { // is_dec
            gmp_asprintf(&str, "%Qd", op);
        }
    } else {
        if (is_oct) {        // is_oct, (show_base can be ignored since octal 0 = 0 or 00, and we use 0).
            if (show_base) { // is_oct, show_base
                gmp_asprintf(&str, "%#Qo", op);
            } else { // is_oct
                gmp_asprintf(&str, "%Qo", op);
            }
        } else if (is_hex) {
            if (show_base) { // is_hex, show_base
                if (uppercase) {
                    gmp_asprintf(&str, "%#QX", op);
                } else {
                    gmp_asprintf(&str, "%#Qx", op);
                }
            } else { // is_hex
                if (uppercase) {
                    gmp_asprintf(&str, "%QX", op);
                } else {
                    gmp_asprintf(&str, "%Qx", op);
                }
            }
        } else { // is_dec
            gmp_asprintf(&str, "%Qd", op);
        }
    }
    std::string s(str);
    free(str);

    if (flags & std::ios::showpos && mpq_sgn(op) > 0) {
        s.insert(0, "+");
    }
    std::streamsize len = s.length();
    if (len < width) {
        std::streamsize padding_length = width - len;
        if (flags & std::ios::left) {
            s.append(padding_length, fill);
        } else if (flags & std::ios::internal) {
            size_t pos = 0;
            if (s[0] == '-' || s[0] == '+') {
                pos = 1;
            }
            if (s.length() > pos + 1 && (s[pos] == '0' && (s[pos + 1] == 'x' || s[pos + 1] == 'X'))) {
                pos += 2;
            }
            s.insert(pos, padding_length, fill);
        } else {
            s.insert(0, padding_length, fill);
        }
    }
    os << s;
    os.width(0);
}
std::ostream &operator<<(std::ostream &os, const mpq_class &op) {
    print_mpq(os, op.get_mpq_t());
    return os;
}
std::ostream &operator<<(std::ostream &os, const mpq_t &op) {
    print_mpq(os, op);
    return os;
}
std::istream &operator>>(std::istream &stream, mpq_class &rop) {
    std::string input;
    std::getline(stream, input);
    if (input.empty() || rop.set_str(input, 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
std::istream &operator>>(std::istream &stream, mpq_t op) {
    std::string input;
    stream >> input;
    if (mpq_set_str(op, input.c_str(), 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}

class mpf_class {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////
    // 12.4 C++ Interface Floats
    // https://gmplib.org/manual/C_002b_002b-Interface-Floats
    ////////////////////////////////////////////////////////////////////////////////////////
    // constructors and destructors
    mpf_class() { mpf_init(value); }
    // The rule of 0/3/5
    // The rule 1 of 5 copy constructor
    mpf_class(const mpf_class &op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
        mpf_init2(value, mpf_get_prec(op.value));
        mpf_set(value, op.value);
#else
        mpf_init_set(value, op.value);
#endif
    }
    mpf_class(const mpf_class &op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set(value, op.value);
    }
    // The rule 2 of 5 copy assignment operator
    mpf_class &operator=(const mpf_class &op) noexcept {
        if (this != &op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
            mpf_init2(value, mpf_get_prec(this->get_mpf_t()));
            mpf_set(value, op.value);
#else
            mpf_init_set(value, op.value);
#endif
        }
        return *this;
    }
    // The rule 3 of 5 default deconstructor
    ~mpf_class() { mpf_clear(value); }
    // The rule 4 of 5 move constructor
    mpf_class(mpf_class &&op) noexcept {
        mpf_init(value);
        mpf_swap(value, op.value);
    }
    // The rule 5 of 5 move assignment operator
    mpf_class &operator=(mpf_class &&op) noexcept {
        if (this != &op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
            mpf_init2(value, mpf_get_prec(this->get_mpf_t()));
            mpf_set(value, op.value);
#else
            mpf_swap(value, op.value);
#endif
        }
        return *this;
    }
    // constructors
    explicit mpf_class(const mpf_t op) {
#if !defined ___GMPXX_MKII_NOPRECCHANGE___
        mpf_init2(value, mpf_get_prec(op));
        mpf_set(value, op);
#else
        mpf_init_set(value, op);
#endif
    }
    mpf_class(const mpz_t op) {
        mpf_init(value);
        mpf_set_z(value, op);
    }
    mpf_class(const mpq_t op) {
        mpf_init(value);
        mpf_set_q(value, op);
    }
    mpf_class(const unsigned long int op) noexcept { mpf_init_set_ui(value, op); }
    mpf_class(const unsigned int op) noexcept { mpf_init_set_ui(value, static_cast<unsigned long int>(op)); }
    mpf_class(const signed long int op) noexcept { mpf_init_set_si(value, op); }
    mpf_class(const signed int op) noexcept { mpf_init_set_si(value, static_cast<signed long int>(op)); }
    mpf_class(const double op) noexcept { mpf_init_set_d(value, op); }
    mpf_class(const char *str) {
        if (mpf_init_set_str(value, str, gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpf_class(const std::string &str) {
        if (mpf_init_set_str(value, str.c_str(), gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
    }

    explicit mpf_class(const mpf_t op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set(value, op);
    }
    mpf_class(const mpz_t op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set_z(value, op);
    }
    mpf_class(const mpq_t op, mp_bitcnt_t prec) {
        mpf_init2(value, prec);
        mpf_set_q(value, op);
    }
    mpf_class(const unsigned long int op, mp_bitcnt_t prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, op);
    }
    mpf_class(const unsigned int op, mp_bitcnt_t prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_ui(value, static_cast<unsigned long int>(op));
    }
    mpf_class(const signed long int op, mp_bitcnt_t prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, op);
    }
    mpf_class(const signed int op, mp_bitcnt_t prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_si(value, static_cast<signed long int>(op));
    }
    mpf_class(const double op, mp_bitcnt_t prec) noexcept {
        mpf_init2(value, prec);
        mpf_set_d(value, op);
    }
    mpf_class(const char *str, mp_bitcnt_t prec, int base = gmpxx_defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str, base) != 0) {
            throw std::invalid_argument("");
        }
    }
    mpf_class(const std::string &str, mp_bitcnt_t prec, int base = gmpxx_defaults::base) {
        mpf_init2(value, prec);
        if (mpf_set_str(value, str.c_str(), base) != 0) {
            throw std::invalid_argument("");
        }
    }

    // assignments from other objects
    mpf_class &operator=(double d) noexcept {
        mpf_set_d(value, d);
        return *this;
    }
    mpf_class &operator=(unsigned long int d) noexcept {
        mpf_set_ui(value, d);
        return *this;
    }
    mpf_class &operator=(signed long int d) noexcept {
        mpf_set_si(value, d);
        return *this;
    }
    mpf_class &operator=(unsigned int d) noexcept {
        mpf_set_ui(value, static_cast<unsigned long int>(d));
        return *this;
    }
    mpf_class &operator=(signed int d) noexcept {
        mpf_set_si(value, static_cast<signed long int>(d));
        return *this;
    }
    mpf_class &operator=(const char *str) {
        if (mpf_set_str(value, str, gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    mpf_class &operator=(const std::string &str) {
        if (mpf_set_str(value, str.c_str(), gmpxx_defaults::base) != 0) {
            throw std::invalid_argument("");
        }
        return *this;
    }
    // operators
    inline mpf_class &operator++() {
        mpf_add_ui(value, value, 1);
        return *this;
    }
    inline mpf_class &operator--() {
        mpf_sub_ui(value, value, 1);
        return *this;
    }
    inline mpf_class operator++(int) {
        mpf_add_ui(value, value, 1);
        return *this;
    }
    inline mpf_class operator--(int) {
        mpf_sub_ui(value, value, 1);
        return *this;
    }
    template <typename T> INT_COND(T, mpf_class &) operator<<=(T n) {
        mpf_mul_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> INT_COND(T, mpf_class &) operator>>=(T n) {
        mpf_div_2exp(value, value, static_cast<mp_bitcnt_t>(n));
        return *this;
    }
    template <typename T> friend INT_COND(T, mpf_class) operator<<(const mpf_class &op1, T op2) {
        mpf_class result(op1);
        mpf_mul_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }
    template <typename T> friend INT_COND(T, mpf_class) operator>>(const mpf_class &op1, T op2) {
        mpf_class result(op1);
        mpf_div_2exp(result.value, result.value, static_cast<mp_bitcnt_t>(op2));
        return result;
    }

    // mpf_class arithmetic operators
    inline friend mpf_class &operator+=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpf_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op);
    inline friend mpf_class operator-(const mpf_class &op);
    inline friend mpf_class operator+(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpz_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator+(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator-(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator*(const mpz_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpz_class &op2);
    inline friend mpf_class operator/(const mpz_class &op1, const mpf_class &op2);

    inline friend mpf_class &operator+=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator-=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator*=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class &operator/=(mpf_class &lhs, const mpq_class &rhs);
    inline friend mpf_class operator+(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator+(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator-(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator-(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator*(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator*(const mpq_class &op1, const mpf_class &op2);
    inline friend mpf_class operator/(const mpf_class &op1, const mpq_class &op2);
    inline friend mpf_class operator/(const mpq_class &op1, const mpf_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2);

    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend NON_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2);
    template <typename T> inline friend UNSIGNED_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2);
    template <typename T> inline friend SIGNED_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2);
    template <typename T> inline friend NON_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2);

    // mpf_class comparison operators
    inline friend bool operator==(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) == 0; }
    inline friend bool operator!=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) != 0; }
    inline friend bool operator<(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) < 0; }
    inline friend bool operator>(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) > 0; }
    inline friend bool operator<=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) <= 0; }
    inline friend bool operator>=(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.value, op2.value) >= 0; }

    inline friend bool operator==(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) == 0; }
    inline friend bool operator!=(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) != 0; }
    inline friend bool operator<(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) < 0; }
    inline friend bool operator>(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) > 0; }
    inline friend bool operator<=(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) <= 0; }
    inline friend bool operator>=(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.value, op2.get_mpz_t()) >= 0; }

    inline friend bool operator==(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) == 0; }
    inline friend bool operator!=(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) != 0; }
    inline friend bool operator<(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) > 0; }
    inline friend bool operator>(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) < 0; }
    inline friend bool operator<=(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) >= 0; }
    inline friend bool operator>=(const mpz_class &op1, const mpf_class &op2) { return mpf_cmp_z(op2.value, op1.get_mpz_t()) <= 0; }

    inline friend bool operator==(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) == 0; }
    inline friend bool operator!=(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) != 0; }
    inline friend bool operator<(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) < 0; }
    inline friend bool operator>(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) > 0; }
    inline friend bool operator<=(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) <= 0; }
    inline friend bool operator>=(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.value, mpf_class(op2).value) >= 0; }

    inline friend bool operator==(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) == 0; }
    inline friend bool operator!=(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) != 0; }
    inline friend bool operator<(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) > 0; }
    inline friend bool operator>(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) < 0; }
    inline friend bool operator<=(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) >= 0; }
    inline friend bool operator>=(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(op2.value, mpf_class(op1).value) <= 0; }

    inline friend bool operator==(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) == 0; }
    inline friend bool operator!=(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) != 0; }
    inline friend bool operator<(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) < 0; }
    inline friend bool operator>(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) > 0; }
    inline friend bool operator<=(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) <= 0; }
    inline friend bool operator>=(const mpf_class &op1, double op2) { return mpf_cmp_d(op1.value, op2) >= 0; }

    inline friend bool operator==(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) == 0; }
    inline friend bool operator!=(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) != 0; }
    inline friend bool operator<(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) > 0; }
    inline friend bool operator>(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) < 0; }
    inline friend bool operator<=(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) >= 0; }
    inline friend bool operator>=(double op1, const mpf_class &op2) { return mpf_cmp_d(op2.value, op1) <= 0; }

    // mpf_class comparison operators (template version)
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator==(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) == 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator!=(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) != 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) < 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) > 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<=(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) <= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>=(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.value, static_cast<unsigned long int>(op2)) >= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator==(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) == 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator!=(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) != 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) > 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) < 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator<=(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) >= 0; }
    template <typename T> inline friend UNSIGNED_INT_COND(T, bool) operator>=(T op1, const mpf_class &op2) { return mpf_cmp_ui(op2.value, static_cast<unsigned long int>(op1)) <= 0; }

    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator==(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) == 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator!=(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) != 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) < 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) > 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<=(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) <= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>=(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.value, static_cast<signed long int>(op2)) >= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator==(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) == 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator!=(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) != 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) > 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) < 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator<=(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) >= 0; }
    template <typename T> inline friend SIGNED_INT_COND(T, bool) operator>=(T op1, const mpf_class &op2) { return mpf_cmp_si(op2.value, static_cast<signed long int>(op1)) <= 0; }

    // mpf_class abs (mpf_class op)
    // mpf_class ceil (mpf_class op)
    friend mpf_class abs(const mpf_class &op);
    friend mpf_class ceil(const mpf_class &op);

    // bool mpf_class::fits_sint_p (void)
    // bool mpf_class::fits_slong_p (void)
    // bool mpf_class::fits_sshort_p (void)
    // bool mpf_class::fits_uint_p (void)
    // bool mpf_class::fits_ulong_p (void)
    // bool mpf_class::fits_ushort_p (void)
    bool fits_sint_p() const { return mpf_fits_sint_p(value); }
    bool fits_slong_p() const { return mpf_fits_slong_p(value); }
    bool fits_sshort_p() const { return mpf_fits_sshort_p(value); }
    bool fits_uint_p() const { return mpf_fits_uint_p(value); }
    bool fits_ulong_p() const { return mpf_fits_ulong_p(value); }
    bool fits_ushort_p() const { return mpf_fits_ushort_p(value); }

    // mpf_class floor (mpf_class op)
    // mpf_class hypot (mpf_class op1, mpf_class op2)
    friend mpf_class floor(const mpf_class &op);
    friend mpf_class hypot(const mpf_class &op1, const mpf_class &op2);

    // double mpf_class::get_d (void)
    // long mpf_class::get_si (void)
    // unsigned long mpf_class::get_ui (void)
    // string mpf_class::get_str (mp_exp_t& exp, int base = 10, size_t digits = 0)
    double get_d() const { return mpf_get_d(value); }
    unsigned long get_ui() const { return mpf_get_ui(value); }
    long get_si() const { return mpf_get_si(value); }
    std::string get_str(mp_exp_t &exp, int base = 10, size_t digits = 0) const {
        char *temp = mpf_get_str(nullptr, &exp, base, digits, value);
        std::string result = temp;
        void (*freefunc)(void *, size_t);
        mp_get_memory_functions(nullptr, nullptr, &freefunc);
        freefunc(temp, std::strlen(temp) + 1);
        return result;
    }
    void div_2exp(mp_bitcnt_t exp) {
        mpf_ptr non_const_ptr = const_cast<mpf_ptr>(this->get_mpf_t());
        mpf_div_2exp(non_const_ptr, this->get_mpf_t(), exp);
    }
    void mul_2exp(mp_bitcnt_t exp) {
        mpf_ptr non_const_ptr = const_cast<mpf_ptr>(this->get_mpf_t());
        mpf_mul_2exp(non_const_ptr, this->get_mpf_t(), exp);
    }
    // int mpf_class::set_str (const char *str, int base)
    // int mpf_class::set_str (const string& str, int base)
    int set_str(const char *str, int base) { return mpf_set_str(value, str, base); }
    int set_str(const std::string &str, int base) { return mpf_set_str(value, str.c_str(), base); }

    // int sgn (mpf_class op)
    // mpf_class sqrt (mpf_class op)
    // void mpf_class::swap (mpf_class& op)
    // void swap (mpf_class& op1, mpf_class& op2)
    // mpf_class trunc (mpf_class op)
    friend int sgn(const mpf_class &op);
    friend mpf_class sqrt(const mpf_class &op);
    friend mpf_class neg(const mpf_class &op);
    void swap(mpf_class &op) { mpf_swap(this->value, op.value); }
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
    friend void swap(mpf_class &op1, mpf_class &op2) { mpf_swap(op1.value, op2.value); }
#endif
    friend mpf_class trunc(const mpf_class &op);

    // mp_bitcnt_t mpf_class::get_prec()
    // void mpf_class::set_prec (mp_bitcnt_t prec)
    // void mpf_class::set_prec_raw (mp_bitcnt_t prec)
    mp_bitcnt_t get_prec() const { return mpf_get_prec(value); }
    void set_prec(mp_bitcnt_t prec) { mpf_set_prec(value, prec); }
    void set_prec_raw(mp_bitcnt_t prec) { mpf_set_prec_raw(value, prec); }

    friend std::ostream &operator<<(std::ostream &os, const mpf_class &op);
    friend std::ostream &operator<<(std::ostream &os, const mpf_t op);

    friend std::istream &operator>>(std::istream &stream, mpf_class &op);
    friend std::istream &operator>>(std::istream &stream, mpf_t op);

    // gmpxx_mkII extension
    static mpf_class const_pi();
    static mpf_class const_e();
    static mpf_class const_log10();
    static mpf_class const_log2();
    static void reset_pi_cache();
    static void reset_e_cache();
    static void reset_log10_cache();
    static void reset_log2_cache();

    operator mpq_class() const;
    operator mpz_class() const;
    mpf_class &operator=(const mpz_class &other) {
        mpf_set_z(this->value, other.get_mpz_t());
        return *this;
    }
    mpf_class &operator=(const mpq_class &other) {
        mpf_set_q(this->value, other.get_mpq_t());
        return *this;
    }

    mpf_srcptr get_mpf_t() const { return value; }
    mpf_ptr get_mpf_t() { return value; }

  private:
    mpf_t value;

    static mpf_class pi_cached;
    static mpf_class e_cached;
    static mpf_class log10_cached;
    static mpf_class log2_cached;
};
// casts
mpf_class::operator mpq_class() const { return mpq_class(this->get_mpf_t()); }
mpf_class::operator mpz_class() const { return mpz_class(this->get_mpf_t()); }
mpz_class::operator mpf_class() const { return mpf_class(this->get_mpz_t()); }
mpq_class::operator mpf_class() const { return mpf_class(this->get_mpq_t()); }
mpz_class::operator mpq_class() const { return mpq_class(this->get_mpz_t()); }
mpq_class::operator mpz_class() const { return mpz_class(this->get_mpq_t()); }

inline mp_bitcnt_t largerprec(const mpf_class &op1, const mpf_class &op2) {
    mp_bitcnt_t prec1 = op1.get_prec(), prec2 = op2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_add(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_sub(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_mul(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, const mpf_class &rhs) {
    mpf_div(lhs.value, lhs.value, rhs.value);
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_add(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_add(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator-(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_sub(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_sub(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator*(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_mul(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_mul(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator/(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class result;
    mpf_div(result.value, op1.value, op2.value);
#else
    mpf_class result;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(result.value, prec);
    mpf_div(result.value, op1.value, op2.value);
#endif
    return result;
}
inline mpf_class operator+(const mpf_class &op) {
    mpf_class result(op.value);
    return result;
}
inline mpf_class operator-(const mpf_class &op) {
    mpf_class result(op.value);
    mpf_neg(result.value, op.value);
    return result;
}

// mpz_class cmp
inline int cmp(const mpz_class &op1, const mpz_class &op2) { return mpz_cmp(op1.get_mpz_t(), op2.get_mpz_t()); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const mpz_class &op1, T op2) { return mpz_cmp_ui(op1.get_mpz_t(), static_cast<unsigned long int>(op2)); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const T op1, mpz_class &op2) { return -mpz_cmp_ui(op2.get_mpz_t(), static_cast<unsigned long int>(op1)); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const mpz_class &op1, T op2) { return mpz_cmp_si(op1.get_mpz_t(), static_cast<signed long int>(op2)); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const T op1, mpz_class &op2) { return -mpz_cmp_si(op2.get_mpz_t(), static_cast<signed long int>(op1)); }
template <typename T> inline NON_INT_COND(T, int) cmp(const mpz_class &op1, T op2) { return mpz_cmp(op1.get_mpz_t(), mpz_class(op2).get_mpz_t()); }
template <typename T> inline NON_INT_COND(T, int) cmp(const T op1, mpz_class &op2) { return -mpz_cmp(op2.get_mpz_t(), mpz_class(op1).get_mpz_t()); }

// mpq_class cmp
inline int cmp(const mpq_class &op1, const mpq_class &op2) { return mpq_cmp(op1.get_mpq_t(), op2.get_mpq_t()); }
inline int cmp(const mpq_class &op1, const mpz_class &op2) { return mpq_cmp_z(op1.get_mpq_t(), op2.get_mpz_t()); }
inline int cmp(const mpz_class &op1, const mpq_class &op2) { return -mpq_cmp_z(op2.get_mpq_t(), op1.get_mpz_t()); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const mpq_class &op1, T op2) { return mpq_cmp(op1.get_mpq_t(), mpq_class(op2).get_mpq_t()); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const T op1, mpq_class &op2) { return -mpq_cmp(op2.get_mpq_t(), mpq_class(op1).get_mpq_t()); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const mpq_class &op1, T op2) { return mpq_cmp(op1.get_mpq_t(), mpq_class(op2).get_mpq_t()); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const T op1, mpq_class &op2) { return -mpq_cmp(op2.get_mpq_t(), mpq_class(op1).get_mpq_t()); }
template <typename T> inline NON_INT_COND(T, int) cmp(const mpq_class &op1, T op2) { return mpq_cmp(op1.get_mpq_t(), mpq_class(op2).get_mpq_t()); }
template <typename T> inline NON_INT_COND(T, int) cmp(const T op1, mpq_class &op2) { return -mpq_cmp(op2.get_mpq_t(), mpq_class(op1).get_mpq_t()); }

// mpf_class cmp
inline int cmp(const mpf_class &op1, const mpf_class &op2) { return mpf_cmp(op1.get_mpf_t(), op2.get_mpf_t()); }
inline int cmp(const mpf_class &op1, const mpq_class &op2) { return mpf_cmp(op1.get_mpf_t(), mpf_class(op2).get_mpf_t()); }
inline int cmp(const mpq_class &op1, const mpf_class &op2) { return mpf_cmp(mpf_class(op1).get_mpf_t(), op2.get_mpf_t()); }
inline int cmp(const mpf_class &op1, const mpz_class &op2) { return mpf_cmp_z(op1.get_mpf_t(), op2.get_mpz_t()); }
inline int cmp(const mpz_class &op1, const mpf_class &op2) { return -mpf_cmp_z(op2.get_mpf_t(), op1.get_mpz_t()); }
inline int cmp(const mpf_class &op1, const double op2) { return mpf_cmp_d(op1.get_mpf_t(), op2); }
inline int cmp(const double op1, const mpf_class &op2) { return -mpf_cmp_d(op2.get_mpf_t(), op1); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const mpf_class &op1, T op2) { return mpf_cmp_ui(op1.get_mpf_t(), static_cast<unsigned long int>(op2)); }
template <typename T> inline UNSIGNED_INT_COND(T, int) cmp(const T op1, mpf_class &op2) { return -mpf_cmp_ui(op2.get_mpf_t(), static_cast<unsigned long int>(op1)); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const mpf_class &op1, T op2) { return mpf_cmp_si(op1.get_mpf_t(), static_cast<signed long int>(op2)); }
template <typename T> inline SIGNED_INT_COND(T, int) cmp(const T op1, mpf_class &op2) { return -mpf_cmp_si(op2.get_mpf_t(), static_cast<signed long int>(op1)); }
template <typename T> inline NON_INT_COND(T, int) cmp(const mpf_class &op1, T op2) { return mpf_cmp(op1.get_mpf_t(), mpf_class(op2).get_mpf_t()); }
template <typename T> inline NON_INT_COND(T, int) cmp(const T op1, mpf_class &op2) { return -mpf_cmp(op2.get_mpf_t(), mpf_class(op1).get_mpf_t()); }

// implimentation of mpf_class operators
// improvements can be done using mpf_XXX_ui (note that they are not _si)
template <typename T> inline SIGNED_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_add(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2) { return op2 + op1; }
template <typename T> inline SIGNED_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_sub(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_sub(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_sub(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_mul(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_mul(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2) { return op2 * op1; }
template <typename T> inline SIGNED_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_div(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_div(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline SIGNED_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_div(result.value, result.value, op2.value);
    return result;
}
// improvements can be done using mpf_XXX_ui
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_add(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2) { return op2 + op1; }
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_sub(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_sub(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_sub(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_mul(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_mul(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2) { return op2 * op1; }
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_div(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_div(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline UNSIGNED_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_div(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class &) operator+=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_add(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator+(const mpf_class &op1, const T op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator+(const T op1, const mpf_class &op2) { return op2 + op1; }
template <typename T> inline NON_INT_COND(T, mpf_class &) operator-=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_sub(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator-(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_sub(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator-(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_sub(result.value, result.value, op2.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class &) operator*=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_mul(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator*(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_mul(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator*(const T op1, const mpf_class &op2) { return op2 * op1; }
template <typename T> inline NON_INT_COND(T, mpf_class &) operator/=(mpf_class &lhs, const T rhs) {
    mpf_class _rhs(rhs);
    mpf_div(lhs.value, lhs.value, _rhs.value);
    return lhs;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator/(const mpf_class &op1, const T op2) {
    mpf_class result(op2);
    mpf_div(result.value, op1.value, result.value);
    return result;
}
template <typename T> inline NON_INT_COND(T, mpf_class) operator/(const T op1, const mpf_class &op2) {
    mpf_class result(op1);
    mpf_div(result.value, result.value, op2.value);
    return result;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs += temp;
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs -= temp;
    return lhs;
}
inline mpf_class &operator*=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs *= temp;
    return lhs;
}
inline mpf_class &operator/=(mpf_class &lhs, const mpz_class &rhs) {
    mpf_class temp = mpf_class(rhs);
    lhs /= temp;
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 += _op2;
    return _op1;
}
inline mpf_class operator+(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op2 += _op1;
    return _op2;
}
inline mpf_class operator-(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator-(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator*(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator*(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator/(const mpf_class &op1, const mpz_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 /= _op2;
    return _op1;
}
inline mpf_class operator/(const mpz_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 /= _op2;
    return _op1;
}
inline mpf_class &operator+=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs += temp;
    return lhs;
}
inline mpf_class &operator-=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs -= temp;
    return lhs;
}

inline mpf_class &operator*=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs *= temp;
    return lhs;
}

inline mpf_class &operator/=(mpf_class &lhs, const mpq_class &rhs) {
    mpf_class temp(rhs);
    lhs /= temp;
    return lhs;
}
inline mpf_class operator+(const mpf_class &op1, const mpq_class &op2) {
    mpf_class result(op1);
    result += op2;
    return result;
}

inline mpf_class operator+(const mpq_class &op1, const mpf_class &op2) {
    mpf_class result(op2);
    result += op1;
    return result;
}

inline mpf_class operator-(const mpf_class &op1, const mpq_class &op2) {
    mpf_class result(op1);
    result -= op2;
    return result;
}
inline mpf_class operator-(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 -= _op2;
    return _op1;
}
inline mpf_class operator*(const mpf_class &op1, const mpq_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator*(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 *= _op2;
    return _op1;
}
inline mpf_class operator/(const mpf_class &op1, const mpq_class &op2) {
    mpf_class _op1(op1);
    mpf_class _op2(op2);
    _op1 /= _op2;
    return _op1;
}
inline mpf_class operator/(const mpq_class &op1, const mpf_class &op2) {
    mpf_class _op1(op2); // 'op2' is used for correct precision initialization, furthur optimization may be possible for ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class _op2(op2);
    _op1 = op1;
    _op1 /= _op2;
    return _op1;
}

// elementary functions
inline mpf_class trunc(const mpf_class &op) {
    mpf_class rop(op);
    mpf_trunc(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class sqrt(const mpf_class &op) {
    mpf_class rop(op);
    mpf_sqrt(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class neg(const mpf_class &op) {
    mpf_class rop(op);
    mpf_neg(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class abs(const mpf_class &op) {
    mpf_class rop(op);
    mpf_abs(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class ceil(const mpf_class &op) {
    mpf_class rop(op);
    mpf_ceil(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class floor(const mpf_class &op) {
    mpf_class rop(op);
    mpf_floor(rop.value, op.get_mpf_t());
    return rop;
}
inline mpf_class hypot(const mpf_class &op1, const mpf_class &op2) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    mpf_class rop;
    rop = sqrt(op1 * op1 + op2 * op2);
#else
    mpf_class rop;
    mp_bitcnt_t prec = largerprec(op1, op2);
    mpf_init2(rop.value, prec);
    rop = sqrt(op1 * op1 + op2 * op2);
#endif
    return rop;
}
inline int sgn(const mpf_class &op) {
    int flag = mpf_sgn(op.get_mpf_t());
    return flag;
}
std::string to_hex_string_default(const mpf_t value, int flags, int width, int prec, char fill) {
    mp_exp_t exp;
    int effective_prec = (prec == 0) ? 4 : prec;
    char *hex_cstr = mpf_get_str(nullptr, &exp, 16, effective_prec, value);
    bool is_showbase = flags & std::ios::showbase;
    std::string hex_str(hex_cstr);
    free(hex_cstr);

    std::string formatted_hex;
    if (mpf_sgn(value) < 0) {
        hex_str.erase(0, 1);
    }
    if (exp <= 0) {
        formatted_hex = "0.";
        formatted_hex.append(-exp + 1, '0');
        formatted_hex += hex_str;
    } else if (size_t(exp) > hex_str.length()) {
        formatted_hex = hex_str;
        formatted_hex.append(exp - hex_str.length(), '0');
    } else {
        formatted_hex = hex_str.substr(0, exp);
        if (exp < static_cast<mp_exp_t>(hex_str.size())) {
            formatted_hex += "." + hex_str.substr(exp);
        }
    }
    if (is_showbase) {
        formatted_hex.insert(0, "0x");
    }
    if (mpf_sgn(value) < 0) {
        formatted_hex.insert(0, "-");
    }
    // Apply width and alignment formatting
    if (width > formatted_hex.size()) {
        std::streamsize padding_length = width - formatted_hex.size();
        if (flags & std::ios_base::left) {
            formatted_hex.append(padding_length, fill);
        } else if (flags & std::ios_base::internal && formatted_hex.find("0x") == 0) {
            formatted_hex.insert(2, padding_length, fill); // Insert padding after the "0x"
        } else if (flags & std::ios_base::internal && formatted_hex.find("-0x") == 0) {
            formatted_hex.insert(3, padding_length, fill); // Insert padding after the "0x"
        } else {                                           // std::ios_base::right or default
            formatted_hex.insert(0, padding_length, fill);
        }
    }
    if (formatted_hex.back() == '.') {
        formatted_hex.erase(formatted_hex.size() - 1); // Remove trailing dot if it exists
    }
    return formatted_hex;
}
std::string to_dec_string_default(const mpf_t value, int flags, int width, int prec, char fill) {
    mp_exp_t exp;
    int effective_prec = (prec == 0) ? 6 : prec;
    char *dec_cstr = mpf_get_str(nullptr, &exp, 10, effective_prec, value);
    std::string dec_str(dec_cstr);
    free(dec_cstr);

    bool is_showpoint = flags & std::ios::showpoint;
    std::string formatted_dec;
    if (mpf_sgn(value) < 0) {
        dec_str.erase(0, 1);
    }
    if (exp <= 0) {
        formatted_dec = "0.";
        formatted_dec.append(-exp + 1, '0');
        formatted_dec += dec_str;
    } else if (size_t(exp) > dec_str.length()) {
        formatted_dec = dec_str.substr(0, 1) + "." + dec_str.substr(1);
        int adjusted_exp = exp - 1;
        std::string exp_str = adjusted_exp < 10 && adjusted_exp > -10 ? "0" + std::to_string(adjusted_exp) : std::to_string(adjusted_exp);
        formatted_dec += "e+" + exp_str;
    } else {
        formatted_dec = dec_str.substr(0, exp);
        if (exp < static_cast<mp_exp_t>(dec_str.size())) {
            formatted_dec += "." + dec_str.substr(exp);
        }
    }
    if (is_showpoint && formatted_dec.find('.') == std::string::npos) {
        formatted_dec += ".";
        while (formatted_dec.length() < static_cast<size_t>(effective_prec + 1)) {
            formatted_dec += '0';
        }
    }
    if (mpf_sgn(value) < 0) {
        formatted_dec.insert(0, "-");
    }
    if (width > formatted_dec.size()) {
        std::streamsize padding_length = width - formatted_dec.size();
        if (flags & std::ios_base::left) {
            formatted_dec.append(padding_length, fill);
        } else if (flags & std::ios_base::internal) {
            size_t pos = 0;
            if (formatted_dec[0] == '-' || formatted_dec[0] == '+') {
                pos = 1;
            }
            formatted_dec.insert(pos, padding_length, fill);
        } else {
            formatted_dec.insert(0, padding_length, fill);
        }
    }
    if (!is_showpoint) {
        if (formatted_dec.back() == '.') {
            formatted_dec.erase(formatted_dec.size() - 1);
        }
    }
    return formatted_dec;
}
void print_mpf(std::ostream &os, const mpf_t op) {
    std::ios_base::fmtflags flags = os.flags();
    std::streamsize prec = os.precision();
    std::streamsize width = os.width();
    bool is_hex = flags & std::ios::hex;
    bool is_oct = flags & std::ios::oct;
    bool is_dec = flags & std::ios::dec;
    bool is_fixed = flags & std::ios::fixed;
    bool is_scientific = flags & std::ios::scientific;
    bool is_showpoint = flags & std::ios::showpoint;
    bool is_showbase = flags & std::ios::showbase;
    char fill = os.fill();
    char *str = nullptr;

    std::string format;
    // op == 0 case
    if (mpf_sgn(op) == 0) {
        if (is_dec) {
            if (is_fixed) { // dec, fixed
                if (is_showpoint) {
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Ff";
                    } else {
                        format = "%.0Ff.";
                    }
                } else {
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Ff";
                    } else {
                        format = "%.0Ff";
                    }
                }
                gmp_asprintf(&str, format.c_str(), op);
            } else if (is_scientific) { // dec, fixed
                if (is_showpoint) {     // dec, fixed, showpoint
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Fe";
                    } else {
                        format = "%.6Fe";
                    }
                } else {
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Fe";
                    } else {
                        format = "%.6Fe";
                    }
                }
                gmp_asprintf(&str, format.c_str(), op);
            } else if (is_showpoint) { // showpoint only
                if (prec != 0)
                    format = "%." + std::to_string(static_cast<int>(prec - 1)) + "f";
                else
                    format = "%." + std::to_string(5) + "f";
                gmp_asprintf(&str, format.c_str(), op);
            } else
                str = strdup("0");
        } else if (is_hex) {
            gmp_asprintf(&str, "%FX", op);
        } else if (is_oct) {
            gmp_asprintf(&str, "%Fo", op);
        }
    } else {
        // op != 0 case
        if (is_dec) {
            if (is_fixed) {         // dec, fixed
                if (is_showpoint) { // dec, fixed, showpoint
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Ff";
                    } else {
                        format = "%.0Ff.";
                    }
                } else {
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Ff";
                    } else {
                        format = "%.0Ff";
                    }
                }
                gmp_asprintf(&str, format.c_str(), op);
            } else if (is_scientific) { // dec, scientific
                if (is_showpoint) {     // dec, scientific, showpoint
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Fe";
                    } else {
                        format = "%.6Fe";
                    }
                } else {
                    if (prec != 0) {
                        format = "%." + std::to_string(static_cast<int>(prec)) + "Fe";
                    } else {
                        format = "%.6Fe";
                    }
                }
                gmp_asprintf(&str, format.c_str(), op);
            } else { // dec, default
                std::string dec_string = to_dec_string_default(op, flags, width, prec, fill);
                str = strdup(dec_string.c_str());
            }
        } else if (is_hex) {
            if (is_fixed) { // hex, fixed
                gmp_asprintf(&str, "%#Fa", op);
                if (is_showpoint) { // hex, fixed, showpoint
                    gmp_asprintf(&str, "%#Fa", op);
                } else {
                    gmp_asprintf(&str, "%#Fa", op);
                }
            } else if (is_scientific) { // hex, scientific
                gmp_asprintf(&str, "%#Fa", op);
            } else { // hex, default
                std::string hex_string = to_hex_string_default(op, flags, width, prec, fill);
                str = strdup(hex_string.c_str());
            }
        } else if (is_oct) {
            gmp_asprintf(&str, "%Fo", op);
        }
    }
    std::string s(str);
    free(str);
    if (flags & std::ios::showpos && mpf_sgn(op) >= 0) {
        s.insert(0, "+");
    }
    std::streamsize len = s.length();
    if (len < width) {
        std::streamsize padding_length = width - len;
        if (flags & std::ios::left) {
            s.append(padding_length, fill);
        } else if (flags & std::ios::internal && s[0] == '-') {
            size_t pos = s.find_first_not_of('-');
            s.insert(pos, padding_length, fill);
        } else {
            s.insert(0, padding_length, fill);
        }
    }
    os << s;
    os.width(0);
}
std::ostream &operator<<(std::ostream &os, const mpf_class &op) {
    print_mpf(os, op.get_mpf_t());
    return os;
}
std::ostream &operator<<(std::ostream &os, const mpf_t &op) {
    print_mpf(os, op);
    return os;
}
std::istream &operator>>(std::istream &stream, mpf_class &op) {
    std::string input;
    stream >> input;
    if (mpf_set_str(op.value, input.c_str(), 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
std::istream &operator>>(std::istream &stream, mpf_t op) {
    std::string input;
    stream >> input;
    if (mpf_set_str(op, input.c_str(), 10) != 0) {
        stream.setstate(std::ios::failbit);
    }
    return stream;
}
mpf_class pi_cached;
mpf_class const_pi() {
    static bool calculated = false;
    static mp_bitcnt_t calculated_pi_precision = 0;
    mp_bitcnt_t _default_prec = mpf_get_default_prec();

    if (!calculated || (calculated && calculated_pi_precision != _default_prec)) {
        pi_cached = mpf_class();
        calculated_pi_precision = mpf_get_default_prec();
        // calculating approximate pi using arithmetic-geometric mean
        mpf_class one(1.0);
        mpf_class two(2.0);
        mpf_class four(4.0);
        mpf_class a(one), b(one / sqrt(two)), t(0.25), p(one);
        mpf_class a_next, b_next, t_next, tmp_pi, pi_previous;

        bool converged = false;
        int iteration = 0;

        mpf_class epsilon = one;
        epsilon.div_2exp(_default_prec);

        while (!converged) {
            iteration++;
            a_next = (a + b) / two;
            b_next = sqrt(a * b);
            t_next = t - p * (a - a_next) * (a - a_next);
            p = two * p;

            // Update values for the next iteration
            a = a_next;
            b = b_next;
            t = t_next;

            // Calculate pi
            pi_previous = tmp_pi;
            tmp_pi = (a + b) * (a + b) / (four * t);

            // Check for convergence
            if (abs(tmp_pi - pi_previous) < epsilon) {
                converged = true;
            }
        }
        calculated = true;
        pi_cached = tmp_pi;
    } else {
        //      std::cout << "pi cached\n";
    }
    return pi_cached;
}
mpf_class const_pi(mp_bitcnt_t req_precision) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    // calculating approximate pi using arithmetic-geometric mean
    mpf_class zero(0.0, req_precision);
    mpf_class quarter(0.25, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);
    mpf_class four(4.0, req_precision);

    mpf_class calculated_pi(zero);
    mpf_class a(one), b(one / sqrt(two)), t(quarter), p(one);
    mpf_class a_next(zero), b_next(zero), t_next(zero), tmp_pi(zero), pi_previous(zero);
    mpf_class epsilon(zero), tmp(zero);

    assert(calculated_pi.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(t.get_prec() == req_precision);
    assert(p.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(t_next.get_prec() == req_precision);
    assert(tmp_pi.get_prec() == req_precision);
    assert(pi_previous.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);

    bool converged = false;
    int iteration = 0;

    epsilon = one;
    epsilon.div_2exp(req_precision);
    while (!converged) {
        iteration++;
        a_next = (a + b) / two;
        b_next = sqrt(a * b);
        t_next = t - p * (a - a_next) * (a - a_next);
        p = two * p;

        // Update values for the next iteration
        a = a_next;
        b = b_next;
        t = t_next;

        // Calculate pi
        pi_previous = tmp_pi;
        tmp_pi = (a + b) * (a + b) / (four * t);

        // Check for convergence
        tmp = abs(tmp_pi - pi_previous);
        if (tmp < epsilon) {
            converged = true;
        }
    }
    calculated_pi = tmp_pi;

    assert(calculated_pi.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(t.get_prec() == req_precision);
    assert(p.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(t_next.get_prec() == req_precision);
    assert(tmp_pi.get_prec() == req_precision);
    assert(pi_previous.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);

    return calculated_pi;
}

mpf_class log2_cached;
mpf_class const_log2() {
    static mpf_class log2_cached;
    static bool calculated = false;
    static mp_bitcnt_t calculated_log2_precision = 0;
    mp_bitcnt_t _default_prec = mpf_get_default_prec();

    if (!calculated || (calculated && calculated_log2_precision != _default_prec)) {
        log2_cached = mpf_class();
        calculated_log2_precision = mpf_get_default_prec();
        // calculating approximate log2 using arithmetic-geometric mean
        mpf_class one(1.0);
        mpf_class two(2.0);
        mpf_class a(one);
        mpf_class epsilon = one;
        epsilon.div_2exp((_default_prec / 2) - 2);

        mpf_class b = epsilon;
        mpf_class sum = one;
        mpf_class a_next, b_next;
        mpf_class tmp;

        bool converged = false;

        while (!converged) {
            a_next = (a + b) / two;
            b_next = sqrt(a * b);

            // Check for convergence
            if (abs(a - b) < epsilon) {
                converged = true;
            }
            a = a_next;
            b = b_next;
        }
        log2_cached = const_pi() / (mpf_class(_default_prec) * a);
        calculated = true;
    }
    return log2_cached;
}
mpf_class const_log2(mp_bitcnt_t req_precision) {
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);

    mpf_class log2(zero);
    mpf_class epsilon(one), tmp(zero);
    mpf_class a(one), b(one);
    mpf_class a_next(zero), b_next(zero);
    mpf_class sum(one);

    bool converged = false;

    // calculating approximate log2 using arithmetic-geometric mean
    b.div_2exp((req_precision / 2) - 2);
    epsilon.div_2exp(req_precision);

    assert(log2.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(sum.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(one.get_prec() == req_precision);
    assert(two.get_prec() == req_precision);

    while (!converged) {
        a_next = (a + b) / two;
        b_next = sqrt(a * b);
        assert(b_next.get_prec() == req_precision);

        // Check for convergence
        if (abs(a - b) < epsilon) {
            converged = true;
        }
        a = a_next;
        b = b_next;
    }
    log2 = const_pi(req_precision) / (mpf_class(req_precision, req_precision) * a);

    assert(const_pi(req_precision).get_prec() == req_precision);
    assert(mpf_class(req_precision, req_precision).get_prec() == req_precision);
    assert(log2.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(sum.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(one.get_prec() == req_precision);
    assert(two.get_prec() == req_precision);

    return log2;
}
mpf_class log(const mpf_class &x) {
    mp_bitcnt_t req_precision = x.get_prec();
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class two(2.0, req_precision);
    mpf_class four(4.0, req_precision);

    mpf_class _log(zero);
    mpf_class epsilon(one), tmp(zero);
    mpf_class a(one), b(one);
    mpf_class a_next(zero), b_next(zero);
    mpf_class s(one);
    mpf_class _pi(const_pi(req_precision));
    mpf_class _log2(const_log2(req_precision));
    mp_exp_t m;
    bool converged = false;

    assert(_log.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(s.get_prec() == req_precision);
    assert(_pi.get_prec() == req_precision);
    assert(_log2.get_prec() == req_precision);

    // calculating approximate log2 using arithmetic-geometric mean
    b = one;
    b.mul_2exp(req_precision / 2);
    s = b / x;
    mpf_get_d_2exp(&m, s.get_mpf_t());

    b = one;
    b.mul_2exp(m);
    s = x * b;

    b = four / s;
    epsilon.div_2exp(req_precision);
    int counter = 0;
    while (!converged) {
        counter++;
        a_next = (a + b) / two;
        b_next = sqrt(a * b);

        // Check for convergence
        if (abs(a - b) < epsilon) {
            converged = true;
        }
        a = a_next;
        b = b_next;
    }
    _log = _pi / (two * b) - m * _log2;

    assert(_log.get_prec() == req_precision);
    assert(epsilon.get_prec() == req_precision);
    assert(tmp.get_prec() == req_precision);
    assert(a.get_prec() == req_precision);
    assert(b.get_prec() == req_precision);
    assert(a_next.get_prec() == req_precision);
    assert(b_next.get_prec() == req_precision);
    assert(s.get_prec() == req_precision);
    assert(_pi.get_prec() == req_precision);
    assert(_log2.get_prec() == req_precision);

    return _log;
}
mpf_class exp(const mpf_class &x) {
    // https://www.mpfr.org/algorithms.pdf section 4.4
    mp_bitcnt_t req_precision = x.get_prec();
#if defined ___GMPXX_MKII_NOPRECCHANGE___
    assert(req_precision == mpf_get_default_prec());
#endif
    mpf_class zero(0.0, req_precision);
    mpf_class one(1.0, req_precision);
    mpf_class _exp(one);
    mpf_class _x(x);
    mpf_class r(zero);
    mpf_class _pi(const_pi(req_precision));
    mpf_class _log2(const_log2(req_precision));
    mp_exp_t k = 0, l = 0, n = 0;

    if (x < zero)
        _x = -_x; // avoid cancellation of significant digits
    // calculating approximate exp
    // taking modulo of log2
    mpf_get_d_2exp(&k, _x.get_mpf_t());
    if (k > 0) {
        _x.div_2exp(k);    // 0.5 <= |x| < 1
        _log2.div_2exp(k); // log2/2 = 0.346574
        n = floor(_x / _log2).get_si();
        r = _x - n * _log2;
        l = req_precision / k;
    } else {
        k = 0;
        l = req_precision;
        r = _x;
        n = 0;
    }
    for (int i = l; i > 0; i--) {
        _exp = one + ((r * _exp) / mpf_class(i, req_precision));
    }
    for (int i = 0; i < k; i++) {
        _exp = _exp * _exp;
    }
    if (n > 0)
        _exp.mul_2exp(n);
    if (n < 0)
        _exp.div_2exp(-n);
    if (x < zero)
        _exp = one / _exp; // avoid cancellation of significant digits
    return _exp;
}
#if !defined ___GMPXX_STRICT_COMPATIBILITY___
} // namespace gmp
#endif

// mpf_class operator"" _mpf (const char *str)
// mpz_class operator"" _mpz (const char *str)
// mpq_class operator"" _mpq (const char *str)
#if defined ___GMPXX_STRICT_COMPATIBILITY___
mpz_class operator"" _mpz(unsigned long long int val) { return mpz_class(static_cast<unsigned long int>(val)); }
mpq_class operator"" _mpq(unsigned long long int val) { return mpq_class(static_cast<unsigned long int>(val), static_cast<unsigned long int>(1)); }
mpf_class operator"" _mpf(long double val) { return mpf_class(static_cast<double>(val)); }
#else
gmp::mpz_class operator"" _mpz(unsigned long long int val) { return gmp::mpz_class(static_cast<unsigned long int>(val)); }
gmp::mpq_class operator"" _mpq(unsigned long long int val) { return gmp::mpq_class(static_cast<unsigned long int>(val), static_cast<unsigned long int>(1)); }
gmp::mpf_class operator"" _mpf(long double val) { return gmp::mpf_class(static_cast<double>(val)); }
#endif
// in the manual, the following functions are avilable, but actually not.
// cf. https://gmplib.org/manual/C_002b_002b-Interface-Rationals
// "With C++11 compilers, integral rationals can be constructed with the syntax 123_mpq which is equivalent to mpq_class(123_mpz). Other rationals can be built as -1_mpq/2 or 0xb_mpq/123456_mpz."
#if !defined ___GMPXX_POSSIBLE_BUGS___
gmp::mpz_class operator"" _mpz(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpz_class(str); }
gmp::mpq_class operator"" _mpq(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpq_class(str); }
gmp::mpf_class operator"" _mpf(const char *str, [[maybe_unused]] std::size_t length) { return gmp::mpf_class(str); }
#endif

#if defined ___GMPXX_STRICT_COMPATIBILITY___
void swap(mpz_class &op1, mpz_class &op2) noexcept { op1.swap(op2); }
void swap(mpq_class &op1, mpq_class &op2) noexcept { op1.swap(op2); }
void swap(mpf_class &op1, mpf_class &op2) noexcept { op1.swap(op2); }
#endif

namespace std { // namespace std for numerical limits
template <>
#if defined ___GMPXX_STRICT_COMPATIBILITY___
class numeric_limits<mpz_class> {
#else
class numeric_limits<gmp::mpz_class> {
#endif
  public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
#if defined ___GMPXX_STRICT_COMPATIBILITY___
    static mpz_class min() noexcept { return mpz_class(0); }
    static mpz_class max() noexcept { return mpz_class(0); }
    static mpz_class lowest() noexcept { return mpz_class(0); }
    static mpz_class epsilon() noexcept { return mpz_class(0); }
    static mpz_class round_error() noexcept { return mpz_class(0); }
    static mpz_class infinity() noexcept { return mpz_class(0); }
    static mpz_class quiet_NaN() noexcept { return mpz_class(0); }
    static mpz_class signaling_NaN() noexcept { return mpz_class(0); }
    static mpz_class denorm_min() noexcept { return mpz_class(0); }
#else
    static gmp::mpz_class min() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class max() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class lowest() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class epsilon() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class round_error() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class infinity() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class quiet_NaN() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class signaling_NaN() noexcept { return gmp::mpz_class(0); }
    static gmp::mpz_class denorm_min() noexcept { return gmp::mpz_class(0); }
#endif
};
template <>
#if defined ___GMPXX_STRICT_COMPATIBILITY___
class numeric_limits<mpq_class> {
#else
class numeric_limits<gmp::mpq_class> {
#endif
  public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
#if defined ___GMPXX_STRICT_COMPATIBILITY___
    static mpq_class min() noexcept { return mpq_class(0); }
    static mpq_class max() noexcept { return mpq_class(0); }
    static mpq_class lowest() noexcept { return mpq_class(0); }
    static mpq_class epsilon() noexcept { return mpq_class(0); }
    static mpq_class round_error() noexcept { return mpq_class(0); }
    static mpq_class infinity() noexcept { return mpq_class(0); }
    static mpq_class quiet_NaN() noexcept { return mpq_class(0); }
    static mpq_class signaling_NaN() noexcept { return mpq_class(0); }
    static mpq_class denorm_min() noexcept { return mpq_class(0); }
#else
    static gmp::mpq_class min() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class max() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class lowest() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class epsilon() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class round_error() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class infinity() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class quiet_NaN() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class signaling_NaN() noexcept { return gmp::mpq_class(0); }
    static gmp::mpq_class denorm_min() noexcept { return gmp::mpq_class(0); }
#endif
};
template <>
#if defined ___GMPXX_STRICT_COMPATIBILITY___
class numeric_limits<mpf_class> {
#else
class numeric_limits<gmp::mpf_class> {
#endif
  public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;
    static constexpr float_denorm_style has_denorm = denorm_present;
    static constexpr bool has_denorm_loss = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
#if defined ___GMPXX_STRICT_COMPATIBILITY___
    static mpf_class min() noexcept { return mpf_class(0); }
    static mpf_class max() noexcept { return mpf_class(0); }
    static mpf_class lowest() noexcept { return mpf_class(0); }
    static mpf_class epsilon() noexcept { return mpf_class(0); }
    static mpf_class round_error() noexcept { return mpf_class(0); }
    static mpf_class infinity() noexcept { return mpf_class(0); }
    static mpf_class quiet_NaN() noexcept { return mpf_class(0); }
    static mpf_class signaling_NaN() noexcept { return mpf_class(0); }
    static mpf_class denorm_min() noexcept { return mpf_class(0); }
#else
    static gmp::mpf_class min() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class max() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class lowest() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class epsilon() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class round_error() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class infinity() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class quiet_NaN() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class signaling_NaN() noexcept { return gmp::mpf_class(0); }
    static gmp::mpf_class denorm_min() noexcept { return gmp::mpf_class(0); }
#endif
};
} // namespace std

#if defined ___GMPXX_STRICT_COMPATIBILITY___
int gmpxx_defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmpxx_defaults::set_default_prec(512);
        gmpxx_defaults::set_default_prec_raw(512);
        gmpxx_defaults::base = 10;
    }
};
#else
int gmp::gmpxx_defaults::base;

class mpf_class_initializer {
  public:
    mpf_class_initializer() {
        gmp::gmpxx_defaults::set_default_prec(512);
        gmp::gmpxx_defaults::set_default_prec_raw(512);
        gmp::gmpxx_defaults::base = 10;
    }
};
#endif

mpf_class_initializer global_mpf_class_initializer;
