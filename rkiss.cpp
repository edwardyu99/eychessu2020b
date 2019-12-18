/*  
  This file is based on original code by Heinz van Saanen and is
  available under the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

 ** A small "keep it simple and stupid" RNG with some fancy merits:
 **
 ** Quite platform independent
 ** Passes ALL dieharder tests! Here *nix sys-rand() e.g. fails miserably:-)
 ** ~12 times faster than my *nix sys-rand()
 ** ~4 times faster than SSE2-version of Mersenne twister
 ** Average cycle length: ~2^126
 ** 64 bit seed
 ** Return doubles with a full 53 bit mantissa
 ** Thread safe
 **
 ** (c) Heinz van Saanen

*/
//#include <stdio.h>
// Keep variables always together
  struct S { unsigned long long a, b, c, d; } s;

  // Return 64 bit unsigned integer in between [0,2^64-1]
__declspec(noinline)
unsigned long long genrand_int64(void) {            
      const unsigned long long 
        e = s.a - ((s.b <<  7) | (s.b >> 57));
      s.a = s.b ^ ((s.c << 13) | (s.c >> 51));
      s.b = s.c + ((s.d << 37) | (s.d >> 27));
      s.c = s.d + e;
      return s.d = e + s.a;
  }
  
// Init seed and scramble a few rounds
void init_mersenne(void)
{
    s.a = 0xf1ea5eed;
    s.b = s.c = s.d = 0xd4e12c77;
      for (int i = 0; i < 73; i++)           
          genrand_int64();
        
//s.a=5106333759607911744;
//s.b=14125394949596747899;
//s.c=13543688828748884056;
//s.d=15267332272266263326;    
//    printf("s.a=%16llu \n", s.a); 
//    printf("s.b=%16llu \n", s.b);
//    printf("s.c=%16llu \n", s.c);
//    printf("s.d=%16llu \n", s.d);
    
}

