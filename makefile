CC = g++
CFLAGS = -g 
CPPFLAGS =  -O2  

%.o  :  %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@


clean:
	rm *.o

lbInt.o : lbInt.cpp lbInt.h

lbIntRandom.o :  lbIntRandom.cpp   lbIntRandom.h

lbIntUtility.o :  lbIntUtility.cpp   lbIntUtility.h

lbInt_test.o : lbInt_test.cpp  lbInt.cpp lbInt.h  lbIntRandom.cpp   lbIntRandom.h lbIntUtility.cpp   lbIntUtility.h

lbInt :  lbInt_test.o lbInt.o lbIntRandom.o lbIntUtility.o

