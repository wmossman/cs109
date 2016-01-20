// $Id: bigint.cpp,v 1.73 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
   if(is_negative and that[1] == '0') is_negative = false; //for -0
}

bigint bigint::operator+() const {
   return *this;
}

bigint bigint::operator-() const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
	bigint result{};
	if(is_negative){
		if(that.is_negative){
			result.is_negative = true;
			result.uvalue = uvalue + that.uvalue;
		}
		else{
			//if this is larger than that, then the result must be negative, otherwise positive
			if (that.uvalue < uvalue){
				result.uvalue = uvalue - that.uvalue;
				result.is_negative = true;
			}
			else{
				result.uvalue = that.uvalue - uvalue;
				result.is_negative = false;
			}
		}
	}
	else{
		if(that.is_negative){
			//if that is larger than this, then the result must be negative, otherwise positive
			if (uvalue < that.uvalue){
				result.uvalue = that.uvalue - uvalue;
				result.is_negative = true;
			}
			else{
				result.uvalue = uvalue - that.uvalue;
				result.is_negative = false;
			}
		}
		else{
			result.is_negative = false;
			result.uvalue = uvalue + that.uvalue;
		}
	}
	
	return result;
}

bigint bigint::operator- (const bigint& that) const {
	bigint result{};
	if(is_negative){
		if(that.is_negative){
			//if this is larger than that, then the result must be negative, otherwise positive
			if (that.uvalue < uvalue){
				result.uvalue = uvalue - that.uvalue;
				result.is_negative = true;
			}
			else{
				result.uvalue = that.uvalue - uvalue;
				result.is_negative = false;
			}
		}
		else{
			result.is_negative = true;
			result.uvalue = uvalue + that.uvalue;
			
		}
	}
	else{
		if(that.is_negative){
			result.is_negative = false;
			result.uvalue = uvalue + that.uvalue;
		}
		else{
			//if that is larger than this, then the result must be negative, otherwise positive
			if (uvalue < that.uvalue){
				result.uvalue = that.uvalue - uvalue;
				result.is_negative = true;
			}
			else{
				result.uvalue = uvalue - that.uvalue;
				result.is_negative = false;
			}
		}
	}
	
	return result;
}

bigint bigint::operator* (const bigint& that) const {
	bigint result{};
	ubigint zerovalue = ubigint ("0");
	ubigint onevalue = ubigint ("1");
	if(uvalue == zerovalue || that.uvalue == zerovalue){
		result.is_negative = false;
		result.uvalue = ubigint ("0");
	}
	else if(uvalue == onevalue){
		result.uvalue = that.uvalue;
		result.is_negative = (is_negative != that.is_negative);
	}
	else if(that.uvalue == onevalue){
		result.uvalue = uvalue;
		result.is_negative = (is_negative != that.is_negative);
	}
	else{
		result.uvalue = uvalue * that.uvalue;
		result.is_negative = (is_negative != that.is_negative);
	}
	return result;
}


bigint bigint::operator/ (const bigint& that) const {
   bigint result{};
	result.uvalue = uvalue / that.uvalue;
	result.is_negative = (is_negative != that.is_negative);
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
              << that.uvalue;
}

