#ifndef _BIG_INT_INCL_
#define _BIG_INT_INCL_

#include <iostream>
#include <vector>
#include <array>
#include <limits.h>
#include <cassert>

class BigUint {
  std::vector<unsigned long long> _data;

public:
  //const unsigned long long radix = ULLONG_MAX / 2 + 1;
    static const unsigned long long radix = 1e+18;  //!!!hardcoded in mult10n and exponent

    BigUint():
        _data({0}) {}
    BigUint(unsigned long long from):
        _data({from % radix}) {
        unsigned long long next = from / radix;
        if (next)
            _data.push_back(next);
    }
    BigUint(const BigUint & from):
        _data(from._data) {}

    BigUint(BigUint && from):
        _data(from._data) {
        printf("Move cons");
    }

    BigUint & operator = (const BigUint & from) {
        _data = from._data;
        return *this;
    }

    BigUint & operator = (BigUint && from) {
        _data = from._data;
        printf("Move:)");
        return *this;
    }

    ~BigUint() {
    //vector destoyes automatically
    }

    unsigned num_blocks() const{
        int res = _data.size();
        while (res > 1 && _data[res - 1] == 0)
            res--;
        return res;
    }
    std::array<BigUint, 2> separate(unsigned decimal) const;

    BigUint & operator += (const BigUint & rhs);
    BigUint & operator *= (const unsigned long long by);
    BigUint & operator *= (const BigUint & by);
    BigUint & operator -= (const BigUint & rhs); //throws underflow_error
    BigUint & operator /= (const BigUint & rhs);
    const BigUint & mult10n(short pw);
    unsigned short exponent() const {
        unsigned short result = (num_blocks() - 1) * 18;
        unsigned long long grst_block = _data[num_blocks() - 1];
        while (grst_block > 0) {
            grst_block /= 10;
            result++;
        }
        return result;
    }

    int cmp(const BigUint & rhs) const {
        int len1 = num_blocks(), len2 = rhs.num_blocks();

        if (len1 < len2)
            return -1;
        else if (len1 > len2)
            return 1;
        else {
            int num_block = len1 - 1;
            unsigned long long v1, v2;
            while ((num_block + 1) && _data[num_block] == rhs._data[num_block])
                num_block--;
            //std::cout << "var1[grst] = " << v1 << "var2[grst] = " << v2 << std::endl;
            if (num_block >= 0) {
                v1 = _data[num_block];
                v2 = rhs._data[num_block];
                return ((v1 > v2) ? 1 : -1);
            }
            return 0;
        }
    }
    bool operator < (const BigUint & rhs) const {
    return cmp(rhs) < 0;
    }
    bool operator <= (const BigUint & rhs) const {
    return cmp(rhs) <= 0;
    }
    bool operator > (const BigUint & rhs) const {
    return cmp(rhs) > 0;
    }
    bool operator >= (const BigUint & rhs) const {
    return cmp(rhs) >= 0;
    }
    bool operator == (const BigUint & rhs) const {
    return cmp(rhs) == 0;
    }
    bool operator != (const BigUint & rhs) const {
    return cmp(rhs) != 0;
    }

    const BigUint operator + (const BigUint & rhs) const {  //must return a new object instead of reference ought to be deleted
        BigUint copy (*this);
        copy += rhs;
        return copy;
    }

    friend std::ostream & operator << (std::ostream & os, const BigUint & which);
};

const BigUint operator +(unsigned long long num, const BigUint & bigInt);
#endif //_BIG_INT_INCL_
