#include "big_int.h"
#include <stdexcept>
#include <string>
#include <cmath>

std::ostream & operator << (std::ostream & os, const BigUint & which) {
    auto last = which._data.crend();
    auto iter = which._data.crbegin();
    std::string str =  std::to_string(*(iter++));
    for (; iter != last; iter++) {
        BigUint::DataType lead_r;
        if (*iter)
            lead_r = (which.radix - 1) / (*iter);
        else
            lead_r = (which.radix - 1);
        while (lead_r > 9) {
            str += "0";
            lead_r /= 10;
        }
        str += std::to_string(*iter);
    }

    /*auto str_iter = str.begin();
    for (; *str_iter == '0'; str_iter++);
    if (str_iter == str.end())
        str_iter -= 1;
    str.erase(str.begin(), str_iter);*/

    os << str;

    /*for (auto elem: which._data)
        std::cout << "\n[DBG] " << elem << " ";*/
    return os;
}

BigUint & BigUint::operator += (const BigUint & rhs) {
    unsigned len1 = _data.size(), len2 = rhs._data.size();
    unsigned to_len = (len1 > len2) ? len2 : len1;
    unsigned i = 0;
    int rem =  0;

    while (i < to_len) {
        _data[i] += rhs._data[i] + rem;
        if (_data[i] >= radix) {
              rem = 1;
              _data[i] %= radix;
        }
        else {
            rem = 0;
        }
        i++;
    }

    if (len1 != len2) {
        //std::cout << "Here" << std::endl;
        if (rem) {
            unsigned upper = (len1 > len2) ? len1 : len2;
            for (; i < upper && rem; i++) {
                //std::cout << "Here[3]" << std::endl;
                if (len1 > len2)
                    _data[i] += rem;
                else
                    _data.push_back(rhs._data[i] + rem);
                if (_data[i] >= radix) {
                    rem = 1;
                    _data[i] %= radix;
                }
                else {
                    rem = 0;
                }
            }
        }
        while (i < len2) {
            //std::cout << "Here[1.1]" << std::endl;
            _data.push_back(rhs._data[i++]);
        }
    }
    if (rem == 1) {
        //std::cout << "Here[/]" << std::endl;
        _data.push_back(rem);
    }
    return *this;
}

BigUint & BigUint::operator *= (const DataType by) {
    //length of cmds_stack will be no more than 100
    enum class Cmd {
        mult10 = 0, mult2, add, add5
    };
    BigUint orig_mult1 = *this, orig_add5 = *this;
    orig_add5 += *this;
    orig_add5 += orig_add5;
    orig_add5 += orig_mult1;

    DataType rem_mult = by;
    std::vector <Cmd> cmds_stack = {};

    if (*this == 0 || by == 0) {
        _data.clear();
        _data.push_back(0);
        return *this;
    }

    //printf("Multiply by %u, size is %llu\n", rem_mult, cmds_stack.size());
    //printf(":");
    while (rem_mult != 0) {
        if (rem_mult % 10 == 0) {
            cmds_stack.push_back(Cmd::mult10);
            rem_mult /= 10;
            //printf("*");
        }
        else if (rem_mult % 2 == 0) {
            cmds_stack.push_back(Cmd::mult2);
            rem_mult /= 2;
        }
        else if (rem_mult % 10 >= 5) {
            cmds_stack.push_back(Cmd::add5);
            rem_mult -= 5;
            //printf("5");
        }
        else {
            cmds_stack.push_back(Cmd::add);
            rem_mult--;
            //printf("+");
        }
    }
  //printf("Length of commands stack is %llu\n", cmds_stack.size());
    BigUint result = 0;
    for (auto i = cmds_stack.crbegin(); i != cmds_stack.crend(); ++i) {
        if (*i == Cmd::mult10)
            result.mult10n(1);
        else if (*i == Cmd::mult2)
            result += result;
        else if (*i == Cmd::add5)
            result += orig_add5;
        else
            result += orig_mult1;
    //std::cout << "New number is " << *this << std::endl;
    }
    *this = std::move(result);
    return *this;
}

const BigUint BigUint::karatsuba_mult(const std::array <BigUint, 2> & mults, int depth = 0) {
    //std::cout << "Arguments:\n" << mults[0] << "\n\nAND\n\n" << mults[1] << "\n" << std::endl;
    if (depth > 99) {
        if (depth > 102) {
            exit(-1);
        }
        std::cout << "Depth " << depth << std::endl;
        std::cout << "Arguments:\n" << mults[0] << "\n\nAND\n\n" << mults[1] << "\n" << std::endl;
    }
    int grst_num = (mults[0] > mults[1]) ? 0 : 1;
    int lwst_num = (grst_num + 1) % 2;

    if (mults[lwst_num]._data.size() < 1) {
        BigUint answer = mults[grst_num];
        //std::cout << "!!!!!!!" << std::endl;
        return (answer *= mults[lwst_num]._data[0]);
    }
    else if (mults[lwst_num]._data.size() < 21) {
        BigUint answer = mults[grst_num];
        //std::cout << "Here" << std::endl;
        return (answer.naive_mult(mults[lwst_num]));
    }

    int separate_by = (mults[lwst_num].exponent() + 1) / 2;
    auto A = std::move(mults[grst_num].separate(separate_by));
    if (depth > 99 && depth < 103) {
        std::cout << "Separated by " << separate_by << "\n" << A[0] << "\nAND\n" << A[1] << std::endl;
        std::cout << "Where exponent equals " << mults[lwst_num].exponent(1) << std::endl;
    }

    auto B = std::move(mults[lwst_num].separate(separate_by));
    if (depth > 99 && depth < 103)
        std::cout << "Separated:\n" << B[0] << "\nAND\n" << B[1] << std::endl;

    //!!!may be should think better about separating
    auto Grst_mult = std::move(karatsuba_mult({A[0], B[0]}, depth + 1));
    Grst_mult._data.shrink_to_fit();
    auto Lwst_mult = std::move(karatsuba_mult({A[1], B[1]}, depth + 1));
    Lwst_mult._data.shrink_to_fit();

    auto D = std::move(karatsuba_mult({A[0] + A[1], B[0] + B[1]}, depth + 1));
    //std::cout << "Grst_mult equals " << Grst_mult << "\n" << std::endl;
    //std::cout << "Lwst_mult equals " << Lwst_mult << "\n" << std::endl;
    //std::cout << "D is " << D << std::endl;

    D -= Grst_mult; D -= Lwst_mult;
    return Grst_mult.mult10n(separate_by * 2) + D.mult10n(separate_by) + Lwst_mult;
}

BigUint & BigUint::operator *= (const BigUint & by) {
    std::array<BigUint, 2> mults = {*this, by};
    //std::cout << "KARATSUBA!)" << std::endl;
    std::cout << "10th degrees are " << (*this).exponent() << " and " << by.exponent() << std::endl;
    *this = std::move(karatsuba_mult(mults));
    return *this;
}

//BigUint & BigUint::operator *= (const BigUint & by) {
BigUint & BigUint::naive_mult (const BigUint & by) {
    //std::cout << "Long multiplication" << std::endl;

    enum class Cmd {
        mult10 = 0,
        add1 = 1, add2, add3, add4, add5, add6, add7, add8, add9,
    };

    if (*this == 0 || by == 0) {
        _data.clear();
        _data.push_back(0);
        return *this;
    }

    BigUint orig_mult1 = *this, rem_by = by;
    BigUint orig_addx[9];

    //std::cout << "Cache of digit-multipliers of original num:" << std::endl;
    BigUint cur = orig_mult1;
    for (int i = 0; i < 9; i++) {
        orig_addx[i] = cur;
        //std::cout << i << "-th is " << cur << std::endl;
        cur += orig_mult1;
    }
    std::vector <Cmd> cmds_stack = {};

    BigUint final(0);
    while (rem_by != final) {
        int last_digit = rem_by._data[0] % 10;
        cmds_stack.push_back(static_cast<Cmd>(last_digit));
        if (last_digit == 0)
            rem_by.mult10n(-1);
        else
            rem_by -= last_digit;
        //std::cout << "New rem_mult " << rem_by << std::endl;
    }

    auto i = cmds_stack.crbegin();
    assert(*i != Cmd::mult10);
    *this = orig_addx[static_cast<int>(*i) - 1];
    ++i;
    for (; i != cmds_stack.crend(); ++i) {
        if (*i == Cmd::mult10)
            (*this).mult10n(1);
        else
            *this += orig_addx[static_cast<int>(*i) - 1];
    //std::cout << "New number is " << *this << std::endl;
  }
  return *this;
}

BigUint & BigUint::operator -= (const BigUint & rhs){
    if (*this < rhs) {
        throw std::underflow_error("Minuend can't be less than subtrahend as integer is unsigned");
    }

    int len1 = _data.size(), len2 = rhs._data.size();
    int rem =  0;

    for (int i = 0; i < len2; i++) {
        DataType calc = radix + _data[i] - rhs._data[i] - rem;
        if (calc < radix) {
            rem = 1;
            _data[i] = calc;
        }
        else {
            rem = 0;
            _data[i] = calc - radix;
        }
        assert(_data[i] < radix);
    }
    if (len1 > len2) {
        DataType calc = 0;
        for (int i = len2; i < len1 && rem; i++) {
            //std::cout << "Here[-]" << std::endl;
            calc = radix + _data[i] - rem;
            if (calc < radix) {
                rem = 1;
                _data[i] = calc;
            }
            else {
                rem = 0;
                _data[i] = calc - radix;
            }
        assert(_data[i] < radix);
        }
    }
    assert(rem == 0);
    clear_zero_blocks();
    return *this;
}

const BigUint operator +(BigUint::DataType num, const BigUint & bigInt) {
  return (bigInt + num);
}

const BigUint & BigUint::mult10n(int pw) { //may be less than zero
  int dir = !!(pw > 0) * 2 - 1;
  const int rad_pw = 18;
  pw *= dir;

  unsigned blocks_shift = (pw / rad_pw);
  //std::cout << "blocks_shift is " << blocks_shift << std::endl;

  if (dir == -1 && blocks_shift >= _data.size()) {
    _data = {0};
    return *this;
  }
  if (dir < 0) {
    _data.erase(_data.begin(), _data.begin() + blocks_shift);
    pw %= rad_pw;

    if (pw) {
      DataType rem = 0;
      unsigned len = _data.size();
      DataType divider = pow (10., pw);

      unsigned i = 0;
      for (; i < len - 1; i++) {
        rem = _data[i+1] % divider;
        _data[i] = _data[i] / divider + rem * (radix / divider);
        assert(_data[i] < radix);
      }
      _data[i] /= divider;
    }
  }
  else {
    pw %= rad_pw;

    if (pw) {
      DataType rem = 0, rem_new = 0;
      unsigned len = _data.size();
      DataType mult = pow (10., pw);
      DataType dec_mask = pow (10., rad_pw - pw);

      for (unsigned i = 0; i < len; i++) {
        assert(_data[i] < radix);
        rem_new = _data[i] / dec_mask;
        //std::cout << "In mind " << rem_new << std::endl;
        //std::cout << "Mult " << mult << std::endl;

        _data[i] = ((_data[i] % dec_mask) * mult + rem);
        assert(_data[i] < radix);
        rem = rem_new;
      }
      if (rem)
        _data.push_back(rem);
    }
    _data.insert(_data.begin(), blocks_shift, 0);
  }
  clear_zero_blocks();
  return *this;
}


BigUint & BigUint::operator /= (const BigUint & rhs) {
    //std::cout << "Arguments:\n" << *this << "\n\nAND\n\n" << rhs << "\n" << std::endl;

    if (*this < rhs) {
        _data.clear();
        _data.push_back(0);
        return (*this);
    }

    else if (rhs == 0) {
        throw std::invalid_argument("Can't divide by zero!");
    }
    BigUint divident(*this), divider(rhs), result(0);
    int pow10 = divident.exponent() - divider.exponent();
    divider.mult10n(pow10);  //transform divider to the same exponent
    //std::cout << "Transform by " << pow10 << std::endl;

    int digit = 0;
    while (pow10 >= 0) {
        digit = 0;
        while (divider <= divident) {
            divident -= divider;
            digit ++;
        }
        //std::cout << "Digit: " << digit << std::endl;
        //std::cout << "Cur divider: " << divider << '\n' << std::endl;
        //std::cout << "Cur result: " << result << std::endl;
        result+= BigUint(digit).mult10n(pow10);

        divider.mult10n(-1);
        pow10--;
    }
    *this = std::move(result);
    return *this;
}

std::array<BigUint, 2> BigUint::separate(const unsigned decimal) const {  //0 respects the case when all number is attributed to the first part of big integer
    if (decimal >= exponent())
        return {0, BigUint(*this)};

    unsigned rem_decimal = decimal;
    unsigned wh_blocks_lower = rem_decimal / 18;
    rem_decimal %= 18;
    DataType gr_part_lower = _data[wh_blocks_lower] % (DataType)pow(10, rem_decimal);

    BigUint upper = *this, lower = {gr_part_lower};
    upper.mult10n(-decimal);
    lower._data.insert(lower._data.begin(), _data.begin(), _data.begin() + wh_blocks_lower);
    lower.clear_zero_blocks();
    return {upper, lower};
}
