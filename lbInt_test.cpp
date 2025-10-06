#include <cmath>
#include <iostream>
#include <time.h>

#include "lbInt.h"
#include "lbIntRandom.h"
#include "lbIntUtility.h"


void testGCD()
{

  lbIntType p("2649124699216926596926156017501750175017507105701721469214162946192649");
  lbIntType q("26492146921416294619264692156921659236592639279");
  lbIntType gcd;
  lbIntType pm;
  lbIntType qm;

  lbIntType mult("269162");
  p *= mult;
  q *= mult;
  extendedGCD( p, q, gcd, pm, qm);
  
  std::cout << " p " << iToA(p) << std::endl;
  std::cout << " q " << iToA(q) << std::endl;
  std::cout << " gcd " << iToA(gcd) << std::endl;
  std::cout << " pm " << iToA(pm) << std::endl;
  std::cout << " qm " << iToA(qm) << std::endl;
  	
  p *= pm ;
  std::cout << " p * pm " << iToA(p) << std::endl;
  q *= qm ;
  std::cout << " q * qm " << iToA(q) << std::endl;
  p += q ;
  std::cout << " (p*pm)+ (q*qm) " << iToA(p) << std::endl;
}

void testMulRadix()
{
	  lbIntType m( 1);
	  
	  m.mulRadix(0);
	  std::cout << std::endl << "m = m * radix^0   " << iToA(m) << std::endl;
	  m.mulRadix();
	  std::cout << "m = m * radix^1   " << iToA(m) << std::endl;
	  m.mulRadix(2);
	  std::cout << "m = m * radix^2   " << iToA(m) << std::endl;
	  m.mulRadix();
	  std::cout << "m = m * radix^1   " << iToA(m) << std::endl;
	  std::cout << std::endl;
	 
}


void testDivRem()
{
			
		lbIntType b("86189793618632");
		lbIntType c("193719739186189793618632");
		
		lbIntType quot, rem;
		
		DivRem(c,b, quot, rem);
		printf(" c =     %s \n", iToA(c).c_str());
		printf(" b =     %s \n", iToA(b).c_str());
		printf(" qout =  %s \n", iToA(quot).c_str());
		printf(" rem  =  %s \n", iToA(rem).c_str());

    lbIntType check;
    check = b; 
    printf(" check=  %s \n", iToA(check).c_str());
		printf(" check*= quot  %s \n", iToA(quot).c_str());
		check *= quot ; 
		printf(" check=  %s \n", iToA(check).c_str());
		printf(" check+=rem  %s \n", iToA(rem).c_str());
		check += rem;
		printf(" check=  %s \n", iToA(check).c_str());
    printf(" c =     %s \n", iToA(c).c_str());

    printf("******************************\n");
    if( 0 == ( check == c) ) printf("test passed\n");
    else printf("test failed\n");
    printf("******************************\n");
    
    lbIntType d(c);
    d /= b ;
    printf(" c /= b;     %s \n", iToA(d).c_str());

    lbIntType e(c);
    e %= b ;
    printf(" c %%= b;     %s \n", iToA(e).c_str());
 
}



 void testJacobi()
 {
    for (int j = 3; j < 60; j += 2)
        for (int i = 1; i < 31; i++) {
            std::cout << "Jacobi ( ";
            lbIntType cj((long long) j);
            lbIntType ci((long long) i);
            std::cout << iToA(ci) << "/ ";
            std::cout << iToA(cj) << " ) = ";            
            std::cout << Jacobi( ci, cj) << std::endl;
        }
}



void testMR1(int &npcount, int &pcount, int width,  lbIntType& p)
{
    lbIntType prime = p;

    std::string s = iToA(p);
    std::string prefix = "";

    while (prefix.length() + s.length() < width) prefix = prefix + " ";

    if (MillerRabin(prime, 30)) {
        std::cout << "n " << prefix << s << " is probably prime  " <<   ++pcount << std::endl;
    }
    else {
        std::cout << "n " << prefix << s << " is not prime  " <<  ++npcount + pcount <<  std::endl;
    };
}



void testMR()
{
    lbIntType prime("26959946667150639794667015087019630673557916260026308143510066298881");
    lbIntType prime1("5127821565631733");
    lbIntType prime2("2147483647");
    
#define WIDTH 75
    int np = 0, p = 0;
    testMR1(np, p, WIDTH, prime2);
    testMR1(np, p, WIDTH, prime1);
    testMR1(np, p, WIDTH, prime2);

    lbIntRandom Rands(MODULUS);

    lbIntType r = prime;
    r -= 3;
    Rands.SetSeed(r);

#define COUNT 1000

    np = 0;
    p = 0;
    for (int i = 0; i < COUNT; i++) {

        lbIntType pc = Rands.Rand();
        if ((pc.Digit() & 1) == 0) pc += 1;
        testMR1(np, p, WIDTH, pc);

    }
}


void testNotEqual()
{
    lbIntType prime("26959946667150639794667015087019630673557916260026308143510066298881");
    lbIntType prime1("5127821565631733");
  
    int a = (prime == prime); 
    int b = (prime != prime); 
    int c = (prime == prime1); 
    int d = (prime != prime1); 
    int e = (prime1 == prime); 
    int f = (prime1 != prime); 

    std::cout << "prime == prime   "  << a  << std::endl;
    std::cout << "prime != prime   "  << b	<< std::endl;
    std::cout << "prime == prime1   " << c  << std::endl;
    std::cout << "prime != prime1   " << d  << std::endl;
		std::cout << "prime1 == prime   " << e  << std::endl;
    std::cout << "prime1 != prime   " << f  << std::endl;
	
}

void testMisc()
{
	
	lbIntType x("642846921491246912416946");
	lbIntType y("24701974017401740712047642846921491246912416946");
	
	std::cout << " x " << iToA(x) << std::endl;
	std::cout << " y " << iToA(y) << std::endl;
	
	lbIntType z;
	z = x;
	z += x ;	
	std::cout << "0 z " << iToA( z) << std::endl;
	z -= x ;	
	std::cout << "1 z " << iToA( z) << std::endl;

	z += y ;	
	std::cout << "2 z " << iToA( z) << std::endl;
	z -= x ;	
	std::cout << "3 z " << iToA( z) << std::endl;
	z -= y ;	
	std::cout << "4 z " << iToA( z) << std::endl;

  z = 1;
  std::cout << "5 z " << iToA( z) << std::endl;
  for(int i = 0 ; i < 10; i++){
  		z <<= i ;
  		std::cout << "6.1 z "<< i <<"  "<< iToA( z) << std::endl;
	}
  for(int i = 9 ; i >= 0; i--){
  		z >>= i ;
  		std::cout << "6.2 z "<< i <<"  "<< iToA( z) << std::endl;
	}

  z = 1;
	z <<= 86243 ;
  std::cout << "7 z "<< 86243 <<"  "<< iToA( z) << std::endl;
	z -= 1 ;
  std::cout << "8 z "<< "  "<< iToA( z) << std::endl;

	baseType x1;
	z = 1;
	z <<= 862 ;
  std::cout << "9.0 z    "<< "  "<< iToA( z) << std::endl;
  x1 = z.divRadix(0);
  std::cout << "9.1 divRadix(0)  "<< "  "<< x1 << std::endl;

  do {
    std::cout << "9.0 z    "<< "  "<< iToA( z) << std::endl;
  	 x1 = z.divRadix();
    std::cout << "9.1 divRadix  "<< "  "<< x1 << std::endl;
  } while ( x1 >= 0 ) ;
  
  
  lbIntType a1("74714");
  lbIntType b1("7291744");
  std::cout << "a1 " << iToA(a1) << std::endl;
  std::cout << "b1 " << iToA(b1) << std::endl;
  a1 *= b1 ;
  
  std::cout << "a1 " << iToA(a1) << std::endl<< std::endl;
  

  z = 1;
	z <<= 86243 ;
  std::cout << "z 2^86243 " " BitSize "<< BitSize(z) << " DigitSize " << z.DigitSize()  << std::endl;
}

int main(int argc, char **argv)
{
  testMisc();
  testGCD();
  testMulRadix();  
  testDivRem(); 
  testJacobi();
  testMR();
  testNotEqual();
    
	return 0;
}