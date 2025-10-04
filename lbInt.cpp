
#include <algorithm>
#include <iostream>
#include "lbInt.h"


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

