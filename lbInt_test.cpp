#include <cmath>
#include <iostream>
#include <time.h>

#include "lbInt.h"

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

int main(int argc, char **argv)
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
/*
  z = 1;
	z <<= 86243 ;
  std::cout << "7 z "<< 86243 <<"  "<< iToA( z) << std::endl;
	z -= 1 ;
  std::cout << "8 z "<< "  "<< iToA( z) << std::endl;
*/
  z = 1;
	z <<= 862 ;
	baseType x1;
  do {
    std::cout << "9.0 z    "<< "  "<< iToA( z) << std::endl;
  	 x1 = z.dropLSD();
    std::cout << "9.1 LSD  "<< "  "<< x1 << std::endl;
  } while ( x1 >= 0 ) ;
  
  
  lbIntType a1("74714");
  lbIntType b1("7291744");
  std::cout << "a1 " << iToA(a1) << std::endl;
  std::cout << "b1 " << iToA(b1) << std::endl;
  a1 *= b1 ;
  
  std::cout << "a1 " << iToA(a1) << std::endl<< std::endl;
  
  testGCD();

  z = 1;
	z <<= 86243 ;
  std::cout << "z 2^86243 " " BitSize "<< BitSize(z) << " DigitSize " << z.DigitSize()  << std::endl;
  
	return 0;
}