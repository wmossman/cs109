// $Id: ubigint.cpp,v 1.8 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that){
	auto tempThat = that;
	while (tempThat != 0){
		ubig_value.push_back(tempThat%10);
		tempThat /= 10;
   }
}

ubigint::ubigint (vector<unsigned char> that){
	ubig_value = that;
}

ubigint::ubigint (const string& that) {
	for (string::const_reverse_iterator rit=that.rbegin();
		rit!=that.rend(); ++rit){
		ubig_value.push_back(*rit - '0');
	}
}

ubigint ubigint::operator+ (const ubigint& that) const {
	ubigvalue_t newVector;
	char carry = 0;
	ubigvalue_t::const_iterator this_it = ubig_value.begin();
	ubigvalue_t::const_iterator that_it = that.ubig_value.begin();
	while(true){
		if(this_it == ubig_value.end() && that_it == that.ubig_value.end()){
			//both iterators are at the ends
			if(carry==1) newVector.push_back(carry);
			break;
		}
		else if(this_it == ubig_value.end()){
			char sum = *that_it + carry;
			if(sum>9){
				newVector.push_back((sum-10));
				carry = 1;
			}
			else{
				newVector.push_back(sum);
				carry = 0;
			}
			++that_it;
		}
		else if(that_it == that.ubig_value.end()){
			char sum = *this_it + carry;
			if(sum>9){
				newVector.push_back((sum-10));
				carry = 1;
			}
			else{
				newVector.push_back(sum);
				carry = 0;
			}
			++this_it;
		}
		else{
			//neither iterators are at the ends of their vectors
			char sum = *that_it + *this_it + carry;
			if(sum>9){
				newVector.push_back((sum-10));
				carry = 1;
			}
			else{
				newVector.push_back(sum);
				carry = 0;
			}
			
			++this_it;
			++that_it;
		}
	}
	while (newVector.size() > 0 and newVector.back() == 0) 
		newVector.pop_back();
	return ubigint (newVector);
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   
   ubigvalue_t newVector;
	char carry = 0;
	ubigvalue_t::const_iterator this_it = ubig_value.begin();
	ubigvalue_t::const_iterator that_it = that.ubig_value.begin();
	while(true){
		if(this_it == ubig_value.end() && that_it == that.ubig_value.end()){
			//both iterators are at the ends
			break;
		}
		else if(that_it == that.ubig_value.end()){
			//making sum +10 to avoid -1 char
			char sum = *this_it - carry + 10;
			if(sum<10){
				newVector.push_back((sum));
				carry = 1;
			}
			else{
				newVector.push_back(sum-10);
				carry = 0;
			}
			++this_it;
		}
		else{
			//neither iterators are at the ends of their vectors
			char sum = (*this_it - *that_it) - carry + 10;
			if(sum<10){
				newVector.push_back((sum));
				carry = 1;
			}
			else{
				newVector.push_back(sum-10);
				carry = 0;
			}
			
			++this_it;
			++that_it;
		}
	}
   
	while (newVector.size() > 0 and newVector.back() == 0) 
		newVector.pop_back();
	return ubigint (newVector);
}

ubigint ubigint::operator* (const ubigint& that) const {
   return 0;
   //return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   //uvalue *= 2;
}

void ubigint::divide_by_2() {
   //uvalue /= 2;
}


ubigint::quot_rem ubigint::divide (const ubigint& that) const {
   /*static const ubigint zero = 0;
   if (that == zero) throw domain_error ("ubigint::divide: by 0");
   ubigint power_of_2 = 1;
   ubigint divisor = that; // right operand, divisor
   ubigint quotient = 0;
   ubigint remainder = *this; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {quotient, remainder};
   */
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return divide (that).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return divide (that).second;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
	if (ubig_value.size() > that.ubig_value.size()) return false;
	if (ubig_value.size() < that.ubig_value.size()) return true;
   
	ubigvalue_t::const_reverse_iterator this_rit = ubig_value.rbegin();
	ubigvalue_t::const_reverse_iterator that_rit = that.ubig_value.rbegin();
	
	while (true){
		if (this_rit == ubig_value.rend() &&
			that_rit == that.ubig_value.rend()) return false;

		if (*this_rit > *that_rit) return false;
		else if (*this_rit < *that_rit) return true;
		else{
			++this_rit;
			++that_rit;
		}
	}
}

ostream& operator<< (ostream& out, const ubigint& that) {
	string valstring;
	for (vector<unsigned char>::const_reverse_iterator rit=that.ubig_value.rbegin();
		rit!=that.ubig_value.rend(); ++rit){
		valstring.push_back(*rit + '0');
	}
    return out << valstring;
}

