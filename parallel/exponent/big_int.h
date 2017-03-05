#ifndef _BIG_INT_INCL_
#define _BIG_INT_INCL_

#include <iostream>
#include <vector>
#include <array>
#include <limits.h>
#include <cassert>

class BigUint {
public:
    using DataType = unsigned long long;
private:
    std::vector<DataType> _data;

public:
    using DataType = unsigned long long;
  //const unsigned long long radix = ULLONG_MAX / 2 + 1;
    static const DataType radix = 1e+18;  //!!!hardcoded in mult10n and exponent and separate

    BigUint():
        _data({0}) {}
    BigUint(DataType from):
        _data({from % radix}) {
        DataType next = from / radix;
        if (next)
            _data.push_back(next);
    }
    BigUint(std::vector<DataType> && from):
        _data(from) {}
    BigUint(const BigUint & from):
        _data(from._data) {}

    BigUint(BigUint && from):
        _data(std::move(from._data)) {
        //printf("Move cons");
    }

    std::vector<DataType> cp_data () {
        return _data;
    }

    BigUint & operator = (const BigUint & from) {
        _data = from._data;
        return *this;
    }

    BigUint & operator = (BigUint && from) {
        _data = std::move(from._data);
        //printf("Move:)");
        return *this;
    }

    ~BigUint() {
    //vector destoyes automatically
    }

    void clear_zero_blocks() {
        unsigned num_bl = _data.size();
        while (num_bl > 1 && _data[num_bl - 1] == 0) {
            num_bl--;
            _data.pop_back();
        }
    }

    std::array<BigUint, 2> separate(unsigned decimal) const;

    BigUint & operator += (const BigUint & rhs);
    BigUint & operator *= (const DataType by);
    BigUint & operator *= (const BigUint & by);
    BigUint & naive_mult (const BigUint & by);
    static const BigUint karatsuba_mult(const std::array<BigUint, 2> & mults, int depth);
    BigUint & operator -= (const BigUint & rhs); //throws underflow_error
    BigUint & operator /= (const BigUint & rhs);
    const BigUint & mult10n(int pw);
    unsigned exponent(int flag = 0) const {
        unsigned result = (_data.size() - 1) * 18;
        DataType grst_block = _data.back();
        if (flag) {
            std::cout << "[DBG] Size = " << _data.size() << std::endl;
            std::cout << "Grst block = " << grst_block << std::endl;
        }
        while (grst_block > 0) {
            grst_block /= 10;
            result++;
        }
        return result;
    }

    int cmp(const BigUint & rhs) const {
        int len1 = _data.size(), len2 = rhs._data.size();

        assert(len1 && len2);
        if (len1 < len2)
            return -1;
        else if (len1 > len2)
            return 1;
        else {
            int num_block = len1 - 1;
            DataType v1, v2;
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

const BigUint operator +(BigUint::DataType num, const BigUint & bigInt);
#endif //_BIG_INT_INCL_
