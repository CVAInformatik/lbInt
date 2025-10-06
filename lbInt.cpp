
/*
Copyright  © 2025 Claus Vind-Andreasen

This program is free software; you can redistribute it and /or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 - 1307 USA
This General Public License does not permit incorporating your program into proprietary programs.If your program is a subroutine library, you may consider it more useful to permit linking proprietary applications with the library.
If this is what you want to do, use the GNU Library General Public License instead of this License.
*/


#include <algorithm>
#include <iostream>
#include "lbInt.h"
#include "lbIntRandom.h"


lbIntType::lbIntType (const int64_t l) {
	
	 int64_t _l = l ;
	 if (l < 0 ) _l = -_l ;
	 	
	 longInt.clear();
	 while (_l){
	 	  longInt.push_back( _l % UL );
	 	  _l = _l/UL ;
	 }	
	 if( l < 0) _changeSign(longInt);
}




//
//  changes the sign and  returns the new sign  
//  -1 for negative, 0 for 0, 1 for positive 
//
int _changeSign(std::vector<baseType> &a )	{
			 for ( int i = 0; i < a.size() ; i++) a[i] = -a[i];
			 return _sign(a);
}

//
// returns the sign  -1 for negative, 0 for 0, 1 for positive 
//
int _sign(const std::vector<baseType> &a ){
	
			if( a.size() == 0 ) return 0;
	
			return (a.back() > 0) ? 1 : -1 ;
}	

//
//   calculates c = a, c*= b, a and c can be idential
//   checks for c == b  do nothing in this case
//
void lbIntType::_mul(const std::vector<baseType> &a, const std::vector<baseType> &b, std::vector<baseType> &c)
{
	int sign = _sign(a) * _sign(b);
	if( c == b) return ; 
	if(sign == 0) { c.clear(); return;}
  std::vector<baseType> t1 = a;
	if( sign < 0 ) _changeSign(t1);
	
  c.clear();
  
  for( size_t i = 0; i < ( t1.size() + b.size() + 1); i++) c.push_back(0);
  
  for( size_t i1 = 0; i1 < t1.size(); i1++) 
    for( size_t i2 = 0; i2 < b.size(); i2++){
			    int64_t acc = ((int64_t) t1[i1]) *((int64_t) b[i2]);// acc always >= 0 
			    c[i2 + i1] += ( acc % UL);
   				c[i2 + i1 + 1] +=  acc/( int64_t) UL;
    			 /* adjust carry */
		   		int carry = 0;
		   		if( c[i2+i1] >= UL){
       	  	c[i2+i1] -= UL;
       	  	c[i2+i1+1]++;
		   		}
		   		if( c[i2+i1+1] >= UL){
   	  	 		c[i2+i1+1] -= UL;
			   		carry = 1;
			 		}
       		for( size_t i3 = i2+i1+2 ; carry && (i3 < a.size()) ; i3++ ) {
       	  	c[i3] += carry ; 
       	  	if( c[i3] >= UL){
       	  		carry = 1;
       	  		c[i3] -= UL;
       	  	} 
       	  	else
 		       	  carry = 0;
       		}      
    }
  while((c.size() > 0) && (c.back() == 0)) c.pop_back(); 
  if( sign < 0) _changeSign(c);
}

//
//     a >>= 1 
//
void shiftDown1(std::vector<baseType> &a)
{
	  int sign  = _sign(a);
	  if( sign == -1) _changeSign(a);
	  for(int i = a.size()-1; i > 0 ; i--)
	      if(a[i] & 1) a[i-1] += UL ;	
	  for(int i = 0 ; i < a.size() ; i++)   a[i] >>= 1 ;	
	  if( sign == -1) _changeSign(a);
	  while( a.size() && a.back()== 0)	 a.pop_back();
}

#ifdef DECIMAL

// Helper for aToI 
void lbIntType::mul10() {
	int carry = 0;
	int64_t t = 0;

	for (int i = 0; i < longInt.size(); i++)
	{
		t = (int64_t)longInt[i];
		t *= 10;
		t += carry;
		carry = 0;
		if (t >= MODULUS) {
			carry = (int)(t / MODULUS);
			t = t % MODULUS;
		}
		longInt[i] = (int)t;
	}
	if (carry) longInt.push_back(carry);
}


void lbIntType::aToI(const std::string& x){

	longInt.clear();

	int index1 = 0;
	int sign = 1; //default positive

	while (index1 < x.length() && isspace(x[index1])) index1++;

	if (x[index1] == '-') {
		sign = -1;
		index1++;
	}

	int index2 = index1;

	while (index2 < x.length() && isdigit(x[index2])) index2++;

	if (index2 == index1)
		std::cerr << "unknown format: " << x << std::endl;
	// we assume we have something number like 
	longInt.push_back(0);
	while (index1 < x.length() && isdigit(x[index1])) {
		mul10();
		longInt[0] += x[index1] - '0';
		index1++;
	}
	_normalize(longInt);
	for (int i = 0; i < longInt.size(); i++) 
	  if( sign < 0 ) longInt[i] = -longInt[i];

}

std::string iToA(const lbIntType &il)
{
#define FORMATSTRING1 "%d"
#define FORMATSTRING2 "%09d"

	if (il.DigitSize() == 0) {
		std::string* s = new std::string();
		s->append("0");
		return *s;
	}
	else {
		char buffer[12];
		std::string s;
		int sign = il.sign();

		if( sign < 0) s.append("-")	;
		sprintf(buffer, FORMATSTRING1, il.Digit(il.DigitSize()-1));
		s.append(buffer);
		for (int i = il.DigitSize()-2 ; i >= 0; i--) {
			sprintf(buffer, FORMATSTRING2, il.Digit(i));
			s.append(buffer);
		}
		return s;
	}
}




#endif // decimal
