#include "big_int.h"
#include <stdexcept>
#include <string>
#include <cmath>

std::ostream & operator << (std::ostream & os, const BigUint & which) {
  auto last = which._data.crend();
  auto iter = which._data.crbegin();
  std::string str =  std::to_string(*(iter++));
  for (; iter != last; iter++) {
     unsigned long long lead_r;
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

  auto str_iter = str.begin();
  for (; *str_iter == '0'; str_iter++);
  if (str_iter == str.end())
    str_iter -= 1;
  str.erase(str.begin(), str_iter);

  os << str;

  /*for (auto elem: which._data)
    std::cout << "\n[DBG] " << elem;*/
  return os;
}

BigUint & BigUint::operator += (const BigUint & rhs) {
    int len1 = num_blocks(), len2 = rhs.num_blocks();
    int to_len = (len1 > len2) ? len2 : len1;
    int i = 0;
    int rem =  0;
    BigUint calc = *this;

    while (i < to_len) {
        calc._data[i] += rhs._data[i] + rem;
        if (calc._data[i] >= radix) {
              rem = 1;
              calc._data[i] %= radix;
        }
        else {
            rem = 0;
        }
        i++;
    }

    if (len1 != len2) {
        //std::cout << "Here" << std::endl;
        if (rem) {
            int upper = (len1 > len2) ? len1 : len2;
            for (; i < upper && rem; i++) {
                //std::cout << "Here[3]" << std::endl;
                if (len1 > len2) {
                    calc._data[i] += rem;
                }
                else {
                    calc._data.push_back(rhs._data[i] + rem);
                }
                if (calc._data[i] >= radix) {
                    rem = 1;
                    calc._data[i] %= radix;
                }
                else {
                    rem = 0;
                }
            }
        }
        while (i < len2) {
            //std::cout << "Here[1.1]" << std::endl;
            calc._data.push_back (rhs._data[i++]);
        }
    }
    if (rem == 1) {
        //std::cout << "Here[/]" << std::endl;
        calc._data.push_back(rem);
    }
    *this = calc;
    return *this;
}

BigUint & BigUint::operator *= (const unsigned long long by) {
    //length of cmds_stack will be no more than 100
  enum class Cmd {
    add = 0, mult2
  };
  BigUint orig_mult1 = *this;
  unsigned long long rem_mult = by;
  std::vector <Cmd> cmds_stack = {};

  //printf("Multiply by %u, size is %llu\n", rem_mult, cmds_stack.size());
  while (rem_mult != 1) {
        if (rem_mult % 2 == 0) {
            cmds_stack.push_back(Cmd::mult2);
            rem_mult /= 2;
        }
        else {
            cmds_stack.push_back(Cmd::add);
            rem_mult--;
        }
        //printf("New rem_mult %u\n", rem_mult);
  }
  //printf("Length of commands stack is %llu\n", cmds_stack.size());
  for (auto i = cmds_stack.crbegin(); i != cmds_stack.crend(); ++i) {
    if (*i == Cmd::mult2)
      *this += *this;
    else
      *this += orig_mult1;
    //std::cout << "New number is " << *this << std::endl;
  }
  return *this;
}

BigUint & BigUint::operator *= (const BigUint & by) {
    std::cout << "Long multiplication" << std::endl;

    enum class Cmd {
        mult10 = 0,
        add1 = 1, add2, add3, add4, add5, add6, add7, add8, add9,
    };
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

    int len1 = num_blocks(), len2 = rhs.num_blocks();
    int rem =  0;

    for (int i = 0; i < len2; i++) {
        unsigned long long calc = radix + _data[i] - rhs._data[i] - rem;
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
        unsigned long long calc = 0;
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
    return *this;
}

const BigUint operator +(unsigned long long num, const BigUint & bigInt) {
  return (bigInt + num);
}

const BigUint & BigUint::mult10n(short pw) { //may be less than zero
  int dir = !!(pw > 0) * 2 - 1;
  const int rad_pw = 18;
  pw *= dir;

  unsigned blocks_shift = (pw / rad_pw);
  //std::cout << "blocks_shift is " << blocks_shift << std::endl;

  if (dir == -1 && blocks_shift >= num_blocks()) {
    _data = {0};
    return *this;
  }
  if (dir < 0) {
    _data.erase(_data.begin(), _data.begin() + blocks_shift);
    pw %= rad_pw;

    if (pw) {
      unsigned long long rem = 0;
      int len = num_blocks();
      unsigned long long divider = pow (10., pw);

      int i = 0;
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
      unsigned long long rem = 0, rem_new = 0;
      int len = num_blocks();
      unsigned long long mult = pow (10., pw);
      unsigned long long dec_mask = pow (10., rad_pw - pw);

      for (int i = 0; i < len; i++) {
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
  return *this;
}


BigUint & BigUint::operator /= (const BigUint & rhs) {
  if (*this < rhs) {
      _data = {0};
      return (*this);
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
    result+= BigUint(digit).mult10n(pow10);
    //std::cout << "Cur result: " << result << std::endl;
    //std::cout << "Cur divident: " << divident << '\n' << std::endl;

    divider.mult10n(-1);
    pow10--;
  }

  *this = result;
  return *this;
}

std::array<BigUint, 2> BigUint::separate(const unsigned decimal) const {  //0 respects the case when all number is attributed to the first part of big integer
    if (decimal >= exponent())
        return {0, BigUint(*this)};

    unsigned rem_decimal = decimal;
    unsigned wh_blocks_lower = rem_decimal / 18;
    rem_decimal %= 18;
    unsigned long long gr_part_lower = _data[wh_blocks_lower] % (unsigned long long)pow(10, rem_decimal);

    BigUint upper = *this, lower = {gr_part_lower};
    upper.mult10n(-decimal);
    lower._data.insert(lower._data.begin(), _data.begin(), _data.begin() + wh_blocks_lower);
    return {upper, lower};
}
