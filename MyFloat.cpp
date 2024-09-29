#include "MyFloat.h"

MyFloat::MyFloat(){
  sign = 0;
  exponent = 0;
  mantissa = 0;
}

MyFloat::MyFloat(float f){
  unpackFloat(f);
}

MyFloat::MyFloat(const MyFloat & rhs){
	sign = rhs.sign;
	exponent = rhs.exponent;
	mantissa = rhs.mantissa;
}

ostream& operator<<(std::ostream &strm, const MyFloat &f){
	strm << f.packFloat();
	return strm;
}

MyFloat MyFloat::operator+(const MyFloat& rhs) const{
  MyFloat float1(*this);
  MyFloat float2(rhs);
  MyFloat floatResult(0);

  if (float1.sign == float2.sign){
    if (float1.exponent != 0){
      float1.mantissa += (0b1 << 23);
    }
    if ((float2.exponent != 0)){
      float2.mantissa += (0b1 << 23);
    }

    if (float1.exponent != float2.exponent){
      int exponentDifference = float1.exponent - float2.exponent;
      if (exponentDifference > 0){
        float2.mantissa >>= exponentDifference;
        float2.exponent += exponentDifference;
      }
      else {
        exponentDifference = -exponentDifference;
        float1.mantissa >>= exponentDifference;
        float1.exponent += exponentDifference;
      }
    }

    floatResult.mantissa = float1.mantissa + float2.mantissa;
    floatResult.exponent = float1.exponent;
    floatResult.sign = float1.sign;

    if ((floatResult.mantissa >> 24) & 0b1){
      floatResult.mantissa >>= 1;
      floatResult.exponent += 1;
    }

    if (floatResult.exponent != 0){
      floatResult.mantissa -= (0b1 << 23);
    }

  }
  else if ((float1.sign == 1) && (float2.sign == 0)){
    float1.sign = 0;
    floatResult = float2.operator-(float1);
  }
  else if((float1.sign == 0) && (float2.sign == 1)){
    float2.sign = 0;
    floatResult = float1.operator-(float2);
  }
  return floatResult;
}

MyFloat MyFloat::operator-(const MyFloat& rhs) const{
  MyFloat float1(*this);
  MyFloat float2(rhs);
  MyFloat floatResult(0);

  if (float1.sign == float2.sign){
    if (float1.exponent == float2.exponent && float1.mantissa == float2.mantissa){
      floatResult.mantissa = 0;
      floatResult.sign = 0;
      floatResult.exponent = 0;
      return floatResult;
    }

    if (float1.exponent != 0){
      float1.mantissa += (0b1 << 23);
    }
    if ((float2.exponent != 0)){
      float2.mantissa += (0b1 << 23);
    }

    if (float1.exponent != float2.exponent){
      int exponentDifference = float1.exponent - float2.exponent;
      if (exponentDifference > 0){
        float2.mantissa >>= exponentDifference;
        float2.exponent += exponentDifference;
      }
      else {
        exponentDifference = -exponentDifference;
        float1.mantissa >>= exponentDifference;
        float1.exponent += exponentDifference;
      }
    }

    if (float1.mantissa >= float2.mantissa) {
      floatResult.mantissa = float1.mantissa + (~float2.mantissa+1);
      floatResult.exponent = float1.exponent;
      floatResult.sign = float1.sign;
    }
    else {
      floatResult.mantissa = float2.mantissa + (~float1.mantissa+1);
      floatResult.exponent = float2.exponent;
      floatResult.sign = !float1.sign;
    }

    if (((floatResult.mantissa >> 23) & 0b1) && (float1.mantissa != 0) && (float2.mantissa != 0)) {
      floatResult.mantissa -= 1;
    }
    while (!(floatResult.mantissa & (0b1 << 23))) {
      floatResult.mantissa <<= 1;
      floatResult.exponent -= 1;
    }
    
    if (floatResult.exponent != 0){
      floatResult.mantissa -= (0b1 << 23);
    }
  }
  else if ((float1.sign == 1) && (float2.sign == 0)){
    float2.sign = 1;
    floatResult = float1.operator+(float2);
  }
  else if ((float1.sign == 0) && (float2.sign == 1)){
    float2.sign = 0;
    floatResult = float1.operator+(float2);
  }
  return floatResult;
}

bool MyFloat::operator==(const float rhs) const{
  MyFloat float1(*this);
  MyFloat float2(rhs);
  if ((float1.sign == float2.sign) && (float1.exponent == float2.exponent) && (float1.mantissa == float2.mantissa)){
    return true;
  }
	return false;
}

void MyFloat::unpackFloat(float f) {
  // int sign = (float_int >> 31) & 1;
  // int exponent = (float_int >> 23) & 0b11111111;
  // int mantissa = (float_int) & 0b11111111111111111111111;
  asm (
    "movb $1, %%ah;"
    "movb $31, %%al;"
    "bextr %%eax, %%edx, %%eax;"

    "movb $8, %%bh;"
    "movb $23, %%bl;"
    "bextr %%ebx, %%edx, %%ebx;"

    "movb $23, %%ch;"
    "movb $0, %%cl;"
    "bextr %%ecx, %%edx, %%ecx;":

    "+a" (sign), "+b" (exponent), "+c" (mantissa):
    [f] "d" (f):
    "cc"
  );
  return;
}

float MyFloat::packFloat() const{
  float f = 0;
  // opposite of all the below
  // int sign = (float_int >> 31) & 1;
  // int exponent = (float_int >> 23) & 0b11111111;
  // int mantissa = (float_int) & 0b11111111111111111111111;
  asm (
    "movl $0, %%edx;"

    "shll $31, %%eax;"
    "orl %%eax, %%edx;"

    "shll $23, %%ebx;"
    "orl %%ebx, %%edx;"

    "orl %%ecx, %%edx;":

    "+d" (f):
    "a" (sign), "b" (exponent), "c" (mantissa):
    "cc"
  );
  return f;
}
