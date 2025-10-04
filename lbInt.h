
#pragma once
#include <climits>
#include <ctype.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <random>

typedef int baseType;

// the digits are 2s complement integers in the openrange ]-MODULUS...MODULUS[
// this range must leave a one-bit head room, at least, but gives other advantages.

#define DECIMAL

#ifdef BINARY
#define MOD  30
#define MODULUS (1 << MOD )
#define UL  MODULUS
#define LL (-MODULUS)
#define DIGITSIZE  MOD
#define BITMASK (MODULUS - 1 )
#endif

#ifdef DECIMAL
// modulo 10^9, 29.89735... bits needed 
#define MODULUS 1000000000
#define UL  MODULUS
#define LL (-MODULUS)
#define DIGITSIZE  30
#define BITMASK (( 1<< DIGITSIZE) -1 )
#endif

/* used internally */
int _changeSign(std::vector<baseType> &a );
int _sign(const std::vector<baseType> &a );
void shiftDown1(std::vector<baseType> &a)	;


class lbIntType  {
public: 
	
	  lbIntType (){longInt.clear();}
	  lbIntType (const std::string& x) { aToI(x); }
	  lbIntType (const int64_t l) ;
	  lbIntType (const lbIntType  &lb) { longInt = lb.longInt;}
	  lbIntType ( const std::vector<baseType> &a) { longInt = a ; }
	  ~lbIntType (){longInt.clear();}
	  
	  lbIntType& operator=(const lbIntType& lb)
	  {
				if ( this != &lb) {
					longInt.clear();
					longInt = lb.longInt;
				}
				return *this;
		}

		//
		//  0 if this == lb , -1  if this < lb 1 if this > lb
		//
	  int operator==(const lbIntType& lb)
	  {  
	  	  lbIntType a(longInt);
	  	  a -= lb ;
	  	  return a.sign();
		}
	  
	  inline lbIntType & operator+=(const lbIntType  &lb )
	  { 
	  	_add(longInt, lb.longInt);
	  	return *this;
	  }
	  
	  //
	  // a = a-b = -(-a+b) to avoid an allocation of copy of b
	  // and allow reuse a+= b instead of making a subtraction function
	  //
	  inline lbIntType & operator-=(const lbIntType  &lb )
	  { 
	  	_changeSign(longInt);
	  	_add(longInt, lb.longInt);
	  	_changeSign(longInt);
	  	return *this;
	  }
	  
	  inline lbIntType& operator<<=(const unsigned int shift )
	  { 
	  	unsigned int t = shift ;
	  	if(_sign(longInt))
		  	while(t--)_add(longInt, longInt);	  		
	  	return *this;
	  }

	  inline lbIntType& operator>>=(const unsigned int shift )
	  { 
	  	unsigned int t = shift ;
	  	if(_sign(longInt)) 
		  	while(t--) shiftDown1(longInt);	  		
	  	return *this;
	  }

	  inline lbIntType & operator*=(const lbIntType  &lb )
	  { 
	  	_mul(longInt, lb.longInt, longInt);
	  	return *this;
	  }

	  //
	  // 0 if 0, 1 if positive -1 if negative
	  //
	  int sign() const {return _sign(longInt);}	  
	  
	  //
	  // inverts sign and returns the new sign
	  //
	  int changeSign() { return _changeSign(longInt); }
	  
	  /*
	      Low level access to the internal  representation, which is useful for
	      various  near bit-level  arithmetics and debugging
	  */	  
	  
	  //
	  // returns the numerical value of a Digit in the representation 
	  // or -1 if the number is 0 (and has no Digit ).
	  // Default is the LSD, but you can get the value of other digits 
	  // with an offset > 0
	  //
	  baseType Digit(unsigned int offset = 0)  const
	  {
	  	if(offset < longInt.size())
	  		switch (_sign(longInt)){
	  			case -1: return -longInt[offset];  	break;
	  			case 0 : return -1 ;    	  				break;
	  			default : return longInt[offset];		break;
	  		}
  		return -1 ; 
	  }

	  //
	  // returns the number of digits in the internal representation
	  //
	  size_t DigitSize() const { return longInt.size() ;}

	  
	  //
	  // integer division by radix, removing a digit from the number by removing the LSD.
	  // returns the numerical value of the digit dropped or -1 if number is 0
	  //
	  baseType  dropLSD(){
	  	baseType res = 0;
	  	switch (_sign(longInt)){
	  		case -1: res = -longInt[0]; 	break;
	  		case 0 : res =  -1 ;        	break;
	  		default : res=  longInt[0]; 	break;	  			
	  	}
	  	if(longInt.size() > 1 ) 
	  		for( int i = 1; i < longInt.size() ; i++)  longInt[i-1] = longInt[i];
  	  if (longInt.size() ) longInt.pop_back();
	  	return res;
	  }

		//
	  // integer multiplication  by a power of the radix, 
	  // 
	  void  mulRadix(unsigned int p = 1){
	  	if( longInt.size()> 0 ) { // 0 * Radix is still 0 
	  		for( int i = 0 ; i < p ;i++) 	longInt.push_back( 0 )	;
	  		for (size_t i = longInt.size()-1 ; i > 0; i-- ) longInt[i] = longInt[i-p];
	  		for( int i = 0 ; i < p ;i++)  	longInt[i] = 0;
	  	}
	  }
	  
	private:

		std::vector<baseType> longInt;
		// a += b 
		void _add(std::vector<baseType> &a,const std::vector<baseType> &b)
		{
			 if (b.size() == 0) return; // adding 0;			
			 int sign = _sign(a) *_sign(b);
			 while (a.size() < b.size()) a.push_back(0 );		
			 for(size_t ix = 0 ; ix < b.size(); ix++) a[ix] += b[ix];
       _carry( a);
       if( sign < 0) _normalize(a);       
		}
		
		//
		//   calculates c = a c*= b, a and c can be idential
		//   checks for c == b  do nothing in this case
		//
		void _mul(const std::vector<baseType> &a, const std::vector<baseType> &b, std::vector<baseType> &c);
			
		// adjust each digit to be in ]LL...UL[
		inline void _carry(std::vector<baseType> &a )
		{
   	   int carry = 0;
			 for(size_t ix = 0 ; ix < a.size(); ix++) {
					 	   a[ix] += carry ;
					 	   if(a[ix] > 0 ){
					 	   				if(a[ix] >= UL) { a[ix] -= UL; carry = 1 ;}
					 	   				else carry = 0;
					 	   }
					 	   else {
  						 	   if(a[ix] < 0 )
 												if(a[ix] <= LL) { a[ix] -= LL; carry = -1 ;}
 												else carry = 0;	
 							 }
			 }
       if( carry != 0 ) a.push_back(carry);
       while(a.size() &&  a.back() == 0) a.pop_back();
		}
		
	  inline void _normalize(std::vector<baseType> &a )
	  {
	     if(_sign(a) == 0) return;
   	   int carry = 0;
	     if(_sign(a) == 1)
					 for(size_t ix = 0 ; ix < a.size(); ix++) {
					 	   a[ix] += carry ;
					 	   if(a[ix] < 0 ) { a[ix] += UL; carry = -1 ;}
					 	   else carry = 0;
					 }
	     else		
					 for(size_t ix = 0 ; ix < a.size(); ix++) {
					 	   a[ix] += carry ;
					 	   if(a[ix] > 0 ) { a[ix] += LL; carry = 1 ;}
					 	   else carry = 0;
					 }
			 while((a.size()>0) && (a.back() == 0)) a.pop_back();
	  }
	  
	  void aToI(const std::string& x);
	  void mul10(); //helper for aToI 
};

/*  various nice to have utilities  */

size_t BitSize(const lbIntType &il); 	  	
std::string iToA(const lbIntType &i);
void extendedGCD(const lbIntType &a, const lbIntType &b, lbIntType &gcd, lbIntType &am, lbIntType &bm  );	
