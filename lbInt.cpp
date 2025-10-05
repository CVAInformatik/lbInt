
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
// returns the bit size of the internal representation
// 
size_t BitSize(const lbIntType &il) { 	  	
	if ( il.sign() == 0) return 0 ;		    	    	
	/* we ignore we only are using 30 bit and calculate as 32 bit */
	int leadingZeroes = sizeof(int) -1 ; // we know there is a least one '1'
	int temp =  il.Digit(il.DigitSize()-1) ;
	if (temp < 0) temp = -temp; // we use the numerical value 
    	
	const int m16 = 0xFFFF0000; // 16 bit mask
	const int m8  = 0xFF000000;
	const int m4  = 0xF0000000;
	const int m2  = 0xC0000000;
	const int m1  = 0x80000000; // 1 bit mask
      
	if ( temp & m16) leadingZeroes -= 16; else temp <<= 16;
	if ( temp & m8)  leadingZeroes -= 8 ; else temp <<= 8;
	if ( temp & m4)  leadingZeroes -= 4 ; else temp <<= 4;
	if ( temp & m2)  leadingZeroes -= 2 ; else temp <<= 2;
	if ( temp & m1)  leadingZeroes -= 1 ; 

	return (DIGITSIZE * il.DigitSize()) - leadingZeroes ;
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
#define FORMATSTRING "%09d"
#define DIGITS 9

	if (il.DigitSize() == 0) {
		std::string* s = new std::string();
		s->append("0");
		return *s;
	}
	else {
		char buffer[12];
		std::string s;// = new std::string();
		int sign = il.sign();

		for (int i = 0; i < il.DigitSize(); i++) {
			sprintf(buffer, FORMATSTRING, il.Digit(i));
			char* c1 = buffer;
			char* c2 = buffer + DIGITS - 1 ;
			while (c1 < c2)
			{
				char t = *c1;
				*c1 = *c2;
				*c2 = t;
				c1++; c2--;
			}
			s.append(buffer);
		}
		while (s.size() && (s.back() == '0')) s.pop_back();
		if (s.size() == 0) 
			s.append("0");
		else 
		    if (sign < 0)  s.append("-");
		std::reverse(s.begin(), s.end());
		return s;
	}
}
#endif // decimal

//
//     GDC(p,q) = pm *p + qm * q
//
void extendedGCD(const lbIntType &_p, const lbIntType &_q, lbIntType &gcd, lbIntType &pm, lbIntType &qm  )
{
	  lbIntType p(_p);
	  lbIntType q(_q);
	  
	  int shift = 0;
	  
	  if( p.sign() ==  0) { gcd = q ; pm = 0; qm = 1; return ; }
	  if( q.sign() ==  0) { gcd = p ; pm = q; qm = 0; return ; }
	  
	  if( p.sign() == -1) p.changeSign();
	  if( q.sign() == -1) q.changeSign();
	
	  while( 0 == ( ( p.Digit() | q.Digit()) & 1 )){
	  	p >>= 1;
	  	q >>= 1;
	  	shift++;
	  }
	  
	  lbIntType p0(p);
	  lbIntType q0(q);
	  lbIntType sp((int) 1);
	  lbIntType sq((int) 0);
	  lbIntType tp((int) 0);
	  lbIntType tq((int) 1);
	  
	  while ((p.Digit()& 1) == 0){
	  	
	  	  if( !(((sp.Digit()& 1) == 0)&&((sq.Digit()& 1) == 0)) ){
	  	  	sp -= q0 ;
	  	  	sq += p0 ;
	  	  }
	  	  p >>= 1;
	  	  sp >>= 1;
	  	  sq >>= 1;	  	
	  }
	  while (q.sign()!=0){
	  	
	  	while (( q.Digit() & 1) == 0){
	  	   if(!( ((tp.Digit() & 1) == 0 ) && ((tq.Digit() & 1) == 0 ))){
	  	   	tp -= q0 ;
	  	   	tq += p0 ;
	  	   }	
	  	   q >>= 1;
	  	   tp >>= 1;
	  	   tq >>= 1;	  	
	  	}
	  	if( ( p == q ) == 1)
	  	{
	  			lbIntType t(p); 	p = q; q = t; // swap 
	  			t = sp; sp = tp; 	tp = t;	  		// swap 
	  			t = sq; sq = tq; 	tq = t;				// swap 
	  			
	  	}
	  	q -= p;
	  	tp -= sp;
	  	tq -= sq;
	  	
	  }
	  gcd = p; gcd <<= shift;
	  pm = sp;
	  qm = sq;
}

void DivRem(const lbIntType &a, const lbIntType &m,lbIntType &Quotient, lbIntType &Remainder )
{
	if (m.sign() == 0) {
		Quotient  = 0;
		Remainder = 0;
		return;
	}

	if( (a.DigitSize() == 1 ) && (m.DigitSize() == 1 )) // two small numbers
	{
			Quotient =  a.Digit() / m.Digit();
			Remainder=  a.Digit() % m.Digit() ;
			return;
	}
	// at least on big number
	lbIntType _dividend(a);
	lbIntType _divisor(m);


	int reciprocal = UL/(2 + m.Digit( m.DigitSize() -1 ));
	int shift = (int) m.DigitSize();
	lbIntType _reciprocal(reciprocal);
	
	Remainder = _reciprocal;
	Remainder *= _dividend;
	

	if (Remainder.sign() != 0)  Remainder.divRadix(shift);
						
	while(1){
			Quotient = Remainder;
		  Remainder *= _divisor;
		  Remainder.changeSign();
		  Remainder += _dividend;
      if(Remainder.sign() < 0){
      	 lbIntType r(Remainder);
      	 r.changeSign();
      	 if ( 0 == (_divisor == r) ) break;
      } else 
		      if ((_divisor == Remainder) > 0 ) break;
 			Remainder *= _reciprocal;
      if (Remainder.sign() != 0)  Remainder.divRadix(shift);
		  if(Remainder.sign() == 0 ) Remainder = 1;
		  Remainder += Quotient;
	}					
	
	/* clean up */					
	if(Remainder.sign() < 0 ){
		 Remainder += _divisor;
		 Quotient  -= 1;
	}		
}


int Jacobi(const lbIntType& a, const lbIntType& b)
{
	lbIntType A(a);
	lbIntType M(b);
	int ResSign = 1;
	if (M.sign() == 0 )return 1; // b is zero
	else {
	  A %= M ;
		if (A.sign() == 0 ) {  //A|M 
			return 0;
		}
		else {
			while (A.sign() != 0 ) {
				while ((A.sign() != 0 ) && ((A.Digit() & 1) == 0))
				{
					A >>= 1;
					switch (M.Digit() & 0x7)
					{
					case 3: 
          case 5:   
               ResSign = -ResSign;
						break;
					default:
						break;
					}
				}
				lbIntType temp(A);
				A = M;
				M = temp;
				if ((A.sign() != 0 ) && (M.sign() != 0 ) && (3 == A.Digit() & 0x3) && (3 == (M.Digit() & 0x3))) 
                    ResSign = -ResSign;
        A %= M ;
			}
			if ((M == 1)==0)
				return ResSign;
			else
				return 0;
		}
	}
}


bool MillerRabin(const lbIntType& number, int witnesses)
{
    lbIntType m( number) ;

    if ( (!m.sign()) && ((m.Digit() & 1) == 0 )) {
        std::cout << "argument must be odd " << std::endl;
        return false;
    }
    else {

        lbIntRandom Rands(MODULUS);

        lbIntType r = number;
        r -= 3;

        Rands.SetSeed(r);

        lbIntType d = m;  
        
        d -= 1;
        
        int s = 0;
        while (!d.sign()  && ((d.Digit() & 1) == 0)) {
            d >>= 1;
            s++;
        }

        for (size_t ix = 0; ix < witnesses; ix++)
        {
            lbIntType a = Rands.Rand();
            a += 2;

            //std::cout << "a: " << a.ToString() << std::endl;
            lbIntType x = modpow(a, d, m);

            for (int i = 0; i < s; i++) {
               lbIntType y = modmult(x, x, m);
               lbIntType t = m;
               t -= x;
               
               if (((y == 1) == 0) && ((x == 1 )!= 0) && ((t==1 ) != 0 )) {
                   std::cout << "mr fail at " << ix << " ";
                    return false;  
                }
                x = y;
            }
            if ((x == 1) != 0) {
                std::cout << "mr fail at " << ix << " ";
                return false;
            }
        }
    }
    return true;
}

// Compute a*b % mod
lbIntType modmult(const lbIntType &_a, const lbIntType &_b, const lbIntType &mod)
{  
    lbIntType result;
    lbIntType a = _a;
    lbIntType b = _b;
    while (b.sign()!= 0 ) {

        if ((b.Digit() & 1) == 1) {
            result += a;
            result %= mod;
        }
        a +=  a;
        a %= mod ;
        b >>= 1;
    }
    return result;
}

 // Compute a^b % mod
lbIntType modpow(const lbIntType& _a, const lbIntType& _b, const lbIntType& mod)
{ 
    lbIntType result;
    lbIntType  a = _a;
    lbIntType  b = _b;
    
    result += 1 ;

    while (b.sign()!= 0) {
        if ((b.Digit() & 1) == 1) {
            result = modmult(result, a, mod);
        }
        a = modmult(a, a, mod);
        b >>= 1;
    }

    return result;
}
