/*
Eychessu - a Chinese Chess Program Designed by Edward Yu, Version: 1.52
this program is to simple,because i have not enough time on it.
if you have some programs,please sent Email to me.or add my QQ.
Email:ok-ly@163.com  QQ:410753046 MSN:chency_1981@hotmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#pragma once
#ifndef _BOARD_H
#define _BOARD_H
#include <stdio.h>
#include <assert.h>
#include <algorithm>  //for std::sort
#include <intrin.h>
extern int POPCNT_CPU;

#define MAX_MOVES 256
#define MAX_PLY   256 //128 //64=may cause loop  //sf10 = 128

typedef union _bitpieceStruct
{
    unsigned int bitpieceWORD;
    struct
    {
        unsigned short bitpieceLSB;
        unsigned short bitpieceMSB;
    };
} bitpieceStruct;

inline int color_of(int pc) {
  assert(pc != 0);
  //return Color(pc >> 3);
  return (pc<0 ? 1 : 0);   //1=WHITE, 0=BLACK
}
constexpr bool is_ok(int from, int dest) {
  return from != dest; // Catch MOVE_NULL and MOVE_NONE
}
//from stockfish-7
inline
unsigned int msb32(unsigned int x)
{
    static const unsigned int bval[] =
    {0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4};

    unsigned int r = 0;
    if (x & 0xFFFF0000) { r += 16/1; x >>= 16/1; }
    if (x & 0x0000FF00) { r += 16/2; x >>= 16/2; }
    if (x & 0x000000F0) { r += 16/4; x >>= 16/4; }
    return r + bval[x];
}

/* Binary constant generator macro
By Tom Torfs - donated to the public domain
*/

/* All macro's evaluate to compile-time constants */

/* *** helper macros *** */

/* turn a numeric literal into a hex constant
(avoids problems with leading zeroes)
8-bit constants max value 0x11111111, always fits in unsigned long
*/
#define HEX__(n) 0x##n##LU

/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)

/* *** user macros *** */

/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))

/* for upto 16-bit binary constants, MSB first */
#define B(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))

/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

/* Sample usage:
B8(01010101) = 85
B16(10101010,01010101) = 43605
B32(10000000,11111111,10101010,01010101) = 2164238933
*/

#define nRank(sq) (sq>>4)
#define nFile(sq) (sq&15)
#define his_table(p, sq) (m_his_table[thd_id][p][sq].HistVal)
/*
inline  bool HOMEHALF(int pos, int side)
{
  //return (int(pos <80) != side)
  return ((pos+48) & 0x80) == (side <<7);
}

inline  bool OPPHALF(int pos, int side)
{
  //return (int(pos <80) == side)
  return ((pos+48) & 0x80) != (side <<7);
}
*/
// use macro instead of function for HOMEHALF, OPPHALF
// pos=0, int(pos>=80)=0, side=0, i.e. HOMEHALF
//#define HOMEHALF(pos, side) (int(pos>=80)==side)
//#define OPPHALF(pos, side) (int(pos>=80)!=side)
//
//#define HOMEHALF(pos, side) (int(pos<80)!=side)
//#define OPPHALF(pos, side) (int(pos<80)==side)
//
// pos=0, (pos+48)&0x80=0, (side<<7)=0, i.e. HOMEHALF
#define HOMEHALF(pos, side) (((pos+48)&0x80)==(side<<7))
#define OPPHALF(pos, side) (((pos+48)&0x80)!=(side<<7))
//#define HOMEHALF(pos, side) ((((pos+48)&0x80)>>7)==side)
//#define OPPHALF(pos, side) ((((pos+48)&0x80)>>7)!=side)

extern unsigned char lut[65536];  //init in Eychessu.cpp
//extern unsigned char lut[256]; //in Eychessu.cpp
/*
static const unsigned char lut[] =
{
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};
*/
/*
Single Populated Bitboards
If the bitboard is not empty, we can extract the LS1B to look whether it is equal with the bitboard.
Alternatively and faster, we can reset the LS1B to look whether the bitboard becomes empty.

if ( x != 0 && (x & (x-1)) == 0 ) -> population count is one

We can skip the leading x != 0 condition to test popcount <= 1:

if ( (x & (x-1)) == 0 ) -> population count is less or equal than one

Again the inverse relation tests, whether a bitboard has more than one bit set:

if ( x & (x-1) ) -> population count is greater than one
*/
#define bitCountEQ1(x) ((x!=0)&&((x)&((x)-1))==0)
#define bitCountLE1(x) (((x)&((x)-1))==0)
#define bitCountGT1(x) ((x)&((x)-1))
/*
//This is better when most bits in x are 0
//It uses 3 arithmetic operations and one comparison/branch per "1" bit in x.
int popcount_4(uint64 x) {
    uint64 count;
    for (count=0; x; count++)
        x &= x-1;
    return count;
}

//This is better if most bits in x are 0.
//It uses 2 arithmetic operations and one comparison/branch  per "1" bit in x.
//It is the same as the previous function, but with the loop unrolled.
#define f(y) if ((x &= x-1) == 0) return y;
int popcount_5(uint64 x) {
    if (x == 0) return 0;
    f( 1) f( 2) f( 3) f( 4) f( 5) f( 6) f( 7) f( 8)
    f( 9) f(10) f(11) f(12) f(13) f(14) f(15) f(16)
    f(17) f(18) f(19) f(20) f(21) f(22) f(23) f(24)
    f(25) f(26) f(27) f(28) f(29) f(30) f(31) f(32)
    f(33) f(34) f(35) f(36) f(37) f(38) f(39) f(40)
    f(41) f(42) f(43) f(44) f(45) f(46) f(47) f(48)
    f(49) f(50) f(51) f(52) f(53) f(54) f(55) f(56)
    f(57) f(58) f(59) f(60) f(61) f(62) f(63)
    return 64;
}

//Use this instead if most bits in x are 1 instead of 0
#define f(y) if ((x |= x+1) == hff) return 64-y;
*/
/*
unsigned int
ones32(register unsigned int x)
{
        // 32-bit recursive reduction using SWAR...
	   //but first step is mapping 2-bit values
	   //into sum of 2 1-bit values in sneaky way

        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return(x & 0x0000003f);
}
*/
inline
int bitCount(unsigned int x)
{
	/*
	x -= ((x>> 1) & 0x55555555);
  x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
  x = (((x >> 4) + x) & 0x0f0f0f0f);

  //return((x * 0x01010101) >> 24);
  x += (x >> 8);
  x += (x >> 16);
  return(x & 0x0000003f);
  */
	/*
	unsigned int w;
	w = v - ((v >> 1) & 0x55555555);
	w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
	//unsigned y = (x + (x >> 4)) & 0x0F0F0F0F;
	//return (y * 0x01010101) >> 24;
	return (((w + (w >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	*/

/*
	return (lut[v&255] +
          lut[(v>>8)&255] +
          lut[(v>>16)&255] +
          lut[v>>24]);
*/
 return (lut[x&0xFFFF] + lut[x>>16]) ;

}
inline
int PCbitCount(unsigned int x)
{
	return __popcnt(x) ;
}	
/*
unsigned int popcount(unsigned int v)
{
unsigned int retVal;
__asm {
mov eax, [v] ; v
mov edx, eax ; v
shr eax, 1 ; v >> 1
and eax, 055555555h ; (v >> 1) & 0x55555555
sub edx, eax ; w = v - ((v >> 1) & 0x55555555)
mov eax, edx ; w
shr edx, 2 ; w >> 2
and eax, 033333333h ; w & 0x33333333
and edx, 033333333h ; (w >> 2) & 0x33333333
add eax, edx ; x = (w & 0x33333333) + ((w >> 2) &
; 0x33333333)
mov edx, eax ; x
shr eax, 4 ; x >> 4
add eax, edx ; x + (x >> 4)
and eax, 00F0F0F0Fh ; y = (x + (x >> 4) & 0x0F0F0F0F)
imul eax, 001010101h ; y * 0x01010101
shr eax, 24 ; population count = (y *
; 0x01010101) >> 24
mov retVal, eax ; Store result.
}
return(retVal);
}
*/
/*
int popCount (U64 x) {
   int count = 0;
   while (x) {
       count++;
       x &= x - 1; // reset LS1B
   }
   return count;
}
*/

inline  int bitCount8 (unsigned int x) {
   return lut[x];
}
inline  int PCbitCount8 (unsigned int x) {
   return __popcnt(x) ;
}
/*
//int popcount_lut8_v2(unsigned *buf, int n)
inline  int bitCount(unsigned int i)
{

  //return (lut[i&255] +
  //        lut[(i>>8)&255] +
  //      lut[(i>>16)&255] +
  //      lut[i>>24]);

  //return (lut[i&65535] + lut[(i>>16)]);
  bitpieceStruct temp;
  temp.bitpieceWORD=i;
  return (lut[temp.bitpieceMSB] + lut[temp.bitpieceLSB]);
}
*/
/*
inline  unsigned int bitCountLSB(unsigned int i)
{
  //return (lut[i]);
  return popCount(i);
}
*/
/* from stockfish
inline int bitCount15(unsigned int v) {  
  v -= (v >> 1) & 0x55555555; // 0-2 in 2 bits  
  v = ((v >> 2) & 0x33333333) + (v & 0x33333333); // 0-4 in 4 bits  
  //v *= 0x11111111;
  //return int(v >> 28);
  //return (v >> 28);
  return((v * 0x11111111) >> 28);
}
*/
inline  int bitCount16(unsigned int x) {
	/*
  int n;
  n = ((v >> 1) & 0x5555) + (v & 0x5555);
  n = ((n >> 2) & 0x3333) + (n & 0x3333);
  n = ((n >> 4) & 0x0f0f) + (n & 0x0f0f);
  return (n >> 8) + (n & 0x00ff);
  */
  /*
  return (lut[v&255] +
          lut[(v>>8)]);
  */
  return lut[x];
}
inline  int PCbitCount16(unsigned int x) {
return __popcnt(x) ;
}

inline  int bitCountMSB(unsigned int x)
{
  //bitpieceStruct temp;
  //temp.bitpieceWORD=x; 
  //return lut[temp.bitpieceMSB];
  return  (lut[x>>16]) ;                
  //return bitCount16(i>>16);
  //return lut[i>>24] + lut[(i>>16)&255];
}
inline  int PCbitCountMSB(unsigned int x)
{return __popcnt(x) ;
}
//-------------------------
//1015 - for RCHcount()	
inline  int bitCountRCH(unsigned int i)
{
  return (lut[(i & 0xF0000000)>>16])*2 + lut[(i & 0x0FF00000)>>16];  ;  
  //return ((i & 0x40000000) >>30) + ((i & 0x10000000) >>28);
}
inline  int PCbitCountRCH(unsigned int i)
{return (__popcnt(i & 0xF0000000))*2 + __popcnt(i & 0x0FF00000);
}	
//--------------------------
	
inline  int bitCountRookB(unsigned int i)
{
  return lut[(i & 0x50000000)>>16];  
  //return ((i & 0x40000000) >>30) + ((i & 0x10000000) >>28);
}
inline  int PCbitCountRookB(unsigned int i)
{return __popcnt(i & 0x50000000);
}
inline  int bitCountRookR(unsigned int i)
{
  return lut[(i & 0xA0000000)>>16];  
  //return ((bitpiece & 0x80000000) >>31) + ((bitpiece & 0x20000000) >>29) ;
}
inline  int PCbitCountRookR(unsigned int i)
{return __popcnt(i & 0xA0000000);
}	
inline  int bitCountCannB(unsigned int i)
{
  return lut[(i & 0x05000000)>>16];  
  //return ((bitpiece & 0x04000000) >>26) + ((bitpiece & 0x01000000) >>24);
}
inline  int PCbitCountCannB(unsigned int i)
{return __popcnt(i & 0x05000000);
}
inline  int bitCountCannR(unsigned int i)
{
  return lut[(i & 0x0A000000)>>16];  
  //return ((bitpiece & 0x08000000) >>27) + ((bitpiece & 0x02000000) >>25);
}
inline  int PCbitCountCannR(unsigned int i)
{return __popcnt(i & 0x0A000000);
}
inline  int bitCountHorsB(unsigned int i)
{
  return lut[(i & 0x00500000)>>16];  
  //return ((bitpiece & 0x00400000) >>22) + ((bitpiece & 0x00100000) >>20);
}
inline  int PCbitCountHorsB(unsigned int i)
{return __popcnt(i & 0x00500000);
}
inline  int bitCountHorsR(unsigned int i)
{
  return lut[(i & 0x00A00000)>>16];  
  //return ((bitpiece & 0x00800000) >>23) + ((bitpiece & 0x00200000) >>21);
}
inline  int PCbitCountHorsR(unsigned int i)
{return __popcnt(i & 0x00A00000);
}
inline  int bitCountCanHorB(unsigned int i)
{
  return lut[(i & 0x05500000)>>16];
  
  //return bitCount8((i & 0x05500000)>>20);
}
inline  int PCbitCountCanHorB(unsigned int i)
{return __popcnt(i & 0x05500000);
}
inline  int bitCountCanHorR(unsigned int i)
{
  return lut[(i & 0x0AA00000)>>16]; 
  //return bitCount8((i & 0x0AA00000)>>20);
}
inline  int PCbitCountCanHorR(unsigned int i)
{ return __popcnt(i & 0x0AA00000);
}
//----------------------------------------
inline  int bitCountEleAdvB(unsigned int i)
{  return lut[(i & 0x00055000)>>12];  
  //return bitCount8((i & 0x00055000)>>12);
}
inline  int PCbitCountEleAdvB(unsigned int i)
{return __popcnt(i & 0x00055000);
}
inline  int bitCountEleAdvR(unsigned int i)
{  return lut[(i & 0x000AA000)>>12];  
  //return bitCount8((i & 0x000AA000)>>12);
}
inline  int PCbitCountEleAdvR(unsigned int i)
{return __popcnt(i & 0x000AA000);
}
//----------------------------------------
inline  int bitCountEleB(unsigned int i)
{  return lut[(i & 0x00005000)>>12];  
  //return bitCount8((i & 0x00005000)>>12);
}
inline  int PCbitCountEleB(unsigned int i)
{return __popcnt(i & 0x00005000);
}
inline  int bitCountEleR(unsigned int i)
{  return lut[(i & 0x0000A000)>>12];  
  //return bitCount8((i & 0x0000A000)>>12);
}
inline  int PCbitCountEleR(unsigned int i)
{return __popcnt(i & 0x0000A000);
}
//----------------------------------------
inline  int bitCountAdvB(unsigned int i)
{  return lut[(i & 0x00050000)>>12];  
  //return bitCount8((i & 0x00050000)>>12);
}
inline  int PCbitCountAdvB(unsigned int i)
{return __popcnt(i & 0x00050000);
}
inline  int bitCountAdvR(unsigned int i)
{  return lut[(i & 0x000A0000)>>12];  
  //return bitCount8((i & 0x000A0000)>>12);
}
inline  int PCbitCountAdvR(unsigned int i)
{return __popcnt(i & 0x000A0000);
}
//-----------------------------------------
inline  int bitCountPawnB(unsigned int i)
{
  return lut[(i & 0x00000554)];
  //return bitCount16(i & 0x00000554);
}
inline  int PCbitCountPawnB(unsigned int i)
{return __popcnt(i & 0x00000554);
}	
inline  int bitCountPawnR(unsigned int i)
{
  return lut[(i & 0x00000AA8)];
  //return bitCount16(i & 0x00000AA8);
}
inline  int PCbitCountPawnR(unsigned int i)
{  
  return __popcnt(i & 0x00000AA8);
}
/*
inline  int bitCountRCHB(unsigned int bitpiece)
{
  unsigned int i = (bitpiece & 0x55500000); //>>16;
  return (lut[(i&16)&255] +
            lut[(i>>24)]);
}
inline  int bitCountRCHR(unsigned int bitpiece)
{
  unsigned int i = (bitpiece & 0xAAA00000); //>>16;
  return (lut[(i>>16)&255] +
            lut[(i>>24)]);
}
*/

inline  int bitCountCHEBB(unsigned int i)
{

  //unsigned int i = (bitpiece & 0x05555000); //>>12;
  //return (lut[(i>>12)&255] +
  //          lut[(i>>20)]);

  return (lut[(i & 0x05555000) >>12]);
  
  //return bitCount16((bitpiece & 0x05555000)>>12);
  //unsigned int i = bitpiece & 0x05555000;
  //return lut[i>>20] + lut[(i>>12)&255];
}
inline  int PCbitCountCHEBB(unsigned int i)
{return __popcnt(i & 0x05555000);
}
inline  int bitCountCHEBR(unsigned int i)
{

  //unsigned int i = (bitpiece & 0x0AAAA000); //>>12;
  //return (lut[(i>>12)&255] +
  //          lut[(i>>20)]);

  return (lut[(i & 0x0AAAA000) >>12]);
  
  //return bitCount16((bitpiece & 0x0AAAA000)>>12);
  //unsigned int i = bitpiece & 0x0AAAA000;
  //return lut[i>>20] + lut[(i>>12)&255];
}
inline  int PCbitCountCHEBR(unsigned int i)
{return __popcnt(i & 0x0AAAA000);
}
/*
inline  int bitCount(unsigned int n)
{
  // This is for 32 bit numbers.  Need to adjust for 64 bits
  register unsigned int tmp;

  tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);

  return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}
*/

/**
 *  Calculates the number of set bits in a 32-bit integer.
 */
/*
inline
int popcnt( unsigned int x )
{
    // Avoid branches, and the potential for cache misses which
    // could be incurred with a table lookup.

    // We need to mask alternate bits to prevent the
    // sum from overflowing.
    // add neighbouring bits. Each bit is 0 or 1.
    x = x - ((x>>1) & 0x5555_5555);
    // now each two bits of x is a number 00,01 or 10.
    // now add neighbouring pairs
    x = ((x&0xCCCC_CCCC)>>2) + (x&0x3333_3333);
    // now each nibble holds 0000-0100. Adding them won't
    // overflow any more, so we don't need to mask any more

    // Now add the nibbles, then the bytes, then the words
    // We still need to mask to prevent double-counting.
    // Note that if we used a rotate instead of a shift, we
    // wouldn't need the masks, and could just divide the sum
    // by 8 to account for the double-counting.
    // On some CPUs, it may be faster to perform a multiply.

    x += (x>>4);
    x &= 0x0F0F_0F0F;
    x += (x>>8);
    x &= 0x00FF_00FF;
    x += (x>>16);
    x &= 0xFFFF;
    return x;
}
*/

//#include <intrin.h>
//#pragma intrinsic(_bittestandset, _bittestandreset, _bittestandcomplement, _bittest)
//static const int c_piece_idx[34] = {0,1,0,1,0,1,0,1,0,1,0,1,6,7,6,7,8,9,8,9,10,11,10,11,12,13,12,13,14,15,14,15,0,1};
static const int c_piece_idx[34] = {10,10,0,1,0,1,0,1,0,1,0,1,2,3,2,3,2,3,2,3,4,5,4,5,6,7,6,7,8,9,8,9,0,1};
//macros
#define ADJ9_16(x) ((x / 9) * 16 + (x % 9))
#define PIECE_VALU(x) ((x*64))
//#define PIECE_VALU(x) (PIECE_VALUE[x])
#define PIECE_IDX16(x) (((x&29)+(x&1))>>1)
#define PIECE_IDX(x) (c_piece_idx[x])
typedef unsigned long long uint64;

//925 extern int p_endgame;
//extern int p_endpoint_chg;
//#define MAX_MOVE_NUM 256 //512 //1024;  // 局面能容纳的历史着法数，局面的合理着法数也不会超过"MAX_MOVE_NUM / 2"

//    2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 32,33
//    p p  p p p p  p p p  p   b  b  b  b   e  e  e  e   n  n  n  n   c  c  c  c   r  r  r  r   k  k
//&011101 (& 0x1D)  ( &29)
//0 1 0 1   4 5 4 5   8 9 8 9    12 13 12 13  16 17 16 17   20 21 20 21   24 25 24 25  28 29 28 29  0 1
//    bprb bprbbprb  bprbbprb    bb rb bb rb
//0 1 2 3   4 5 6 7   8 9 10 11  12 13 14 15  16 17 18 19   20 21 22 23   24 25 26 27  28 29 30 31 32 33
//
//0 1       4 5       8 9       12 13         16 17         20 21         24 25        28 29             &29
//0 1       2 3       4 5        6  7          8  9         10 11         12 13        14 15   (n+ n&1)/2
//king/p    p         p          b             e            n             c            r
//8                   2          3             4            5             6            7
//(n&29 + n&29&1)/2 = (n&29 + n&1)/2
// piecetype = piece>>2; // piece/4;
#define PAWN      2
#define ADVISOR    3
#define ELEPHAN   4
#define HORSE    5
#define CANNON    6
#define ROOK      7
#define KING      8

#define EMPTY		0
#define B_KING		32
#define B_ADVISOR1	12
#define B_ELEPHAN1	16
#define B_HORSE1	20
#define B_ROOK1 	28
#define B_CANNON1 	24
#define B_ADVISOR2	14
#define B_ELEPHAN2	18
#define B_HORSE2	22
#define B_ROOK2 	30
#define B_CANNON2 	26
#define B_PAWN0		0    // for piececnt[B_PAWN0] + [B_PAWN2] + [B_PAWN4] after &61
#define B_PAWN1		2
#define B_PAWN2		4
#define B_PAWN3		6
#define B_PAWN4		8
#define B_PAWN5		10

#define R_KING 		33
#define R_ADVISOR1 	13
#define R_ELEPHAN1	17
#define R_HORSE1 	21
#define R_ROOK1 	29
#define R_CANNON1 	25
#define R_ADVISOR2 	15
#define R_ELEPHAN2	19
#define R_HORSE2 	23
#define R_ROOK2 	31
#define R_CANNON2 	27
#define R_PAWN0		1    // for piececnt[R_PAWN0] + [R_PAWN2] + [R_PAWN4] after &61
#define R_PAWN1		3
#define R_PAWN2		5
#define R_PAWN3		7
#define R_PAWN4		9
#define R_PAWN5		11
#define BLACK	0
#define RED	1
#define WHITE	1
#define BOARD_SIZE 160

//#define oppside(x) (1-x)
#define oppside(side) (side^1)
#define chgside(side) (side^=1)
//20190912 debug srchboob #define SQ_EMPTY 0xff
#define SQ_EMPTY 90

//#define SQ_EMPTY -1
#define NOTSQEMPTY(sq) ((sq!=SQ_EMPTY))
//#define NOTSQEMPTY(sq) ((sq>=0))

#define BIGVAL   32700 //    30000000
#define BIGVAL1  32710 //    30001000
#define KILLER1  22250 //3500000 //400000 //3000 //800
#define KILLER12 KILLER1 -1 //300000 //2500 //700
#define KILLER2  KILLER1 -2 //250000 //2000 //600
#define KILLER22 KILLER1 -3 //200000 //1500 //500
#define KILLER3  KILLER1 -4



//static const int c_PieceType[34] = {0,0, 0,1,0,1,0,1,0,1,0,1, 2,3,2,3,2,3,2,3, 4,5,4,5, 6,7,6,7, 8,9,8,9, 0,1};
//0 0 2 3   4 5 6 7   8 9 10 11  12 13 14 15  16 17 18 19   20 21 22 23   24 25 26 27  28 29 30 31  32 33
//&011101 (& 0x1D)  ( &29)
//0 0 0 1   4 5 4 5   8 9 8 9    12 13 12 13  16 17 16 17   20 21 20 21   24 25 24 25  28 29 28 29  0 1
//    bprb bprbbprb  bprbbprb    bb rb bb rb
/*
typedef union _MOVE
{   int move;
    struct
    {
    short value;
    short mv;
    };
    struct
    {   short val;
        char from;
        char dest;
    };
} MOVE;
*/
//typedef int MOVE;
/*
typedef union _MoveStruct
{
	short move;
	struct
	{
	char dest;
	char from;
	};
} MoveStruct;
*/

typedef union _MoveStruct
{
    unsigned short move;
    struct
    {
        unsigned char dest;
        unsigned char from;
    };
} MoveStruct;

union MoveTabStruct {

    int tabentry;
    struct {
        MoveStruct table;
        //unsigned short move;
        short tabval;
    };
    struct {
    		unsigned char dest, from;
    		unsigned char capture, Chk;
    	};
};


/*
// 复杂着法结构
union MoveStruct {
  uint32 dwmv;               // 填满整个结构用
  struct {
    uint16 wmv, wvl;         // 着法和分值
  };
  struct {
    uint8 Src, Dst;          // 起始格和目标格
    uint8 Chk:2, Cpt:6, Con; // 将军标志、被吃子和不吃子着法数
  };
}; // mvs
*/

struct HistRecord
{
    uint64 hkey;
    //int from, dest,
    /*
    int capture;
    MoveStruct hmove;
    short pointsum;
    unsigned char Chk;
    */
    MoveTabStruct htab;
    //unsigned int pawnbit;
    short pointsum;
    unsigned char mvpiece;
    unsigned char endgame;

};

struct HistStruct
{
		//unsigned short HistVal;  //1886o
		short HistVal;             //1886r
    unsigned short HistHit;
    unsigned short HistTot;
};

/// Stack struct keeps track of the information we need to remember from nodes
/// shallower and deeper in the tree during the search. Each search thread has
/// its own array of Stack objects, indexed by the current ply.
/*
struct Stack {
    MoveStruct* pv;
//  PieceToHistory* continuationHistory;
    int ply;
    MoveStruct currentMove;
    MoveStruct excludedMove;
    MoveStruct killers[2];
  int staticEval;
  int statScore;
  int moveCount;
};
*/

/*
struct MOVE{int from,dest,value;
MOVE(){from=dest=value=0;}
MOVE(int from,int dest,int value)
{this->from =from,this->dest =dest;this->value =value;}
bool operator==(const MOVE &move)
{
	//if(from==move.from &&dest==move.dest &&value==move.value )return true;
	if(from==move.from &&dest==move.dest )return true;
	else return false;
}
bool operator!=(const MOVE &move)
{
	//if(from==move.from &&dest==move.dest &&value==move.value )return false;
	if(from==move.from && dest==move.dest )return false;
	else return true;
}

};
const MOVE NullMove=MOVE(0,0,0); // MOVE(-1,0,0);
*/
//const int NullMove=0;
#define NullMove 0


void mov2char (char *move,int fromdest);
bool com2char (char *move,int from ,int dest);
bool char2com(char*move,int &from,int &dest);
/*
struct Killer
{
	int size;
	MOVE move[2];
	Killer(){size=0;}
};
*/


class Board
{
public:
    int m_side;
    int ply;
    uint64 hkey;
    //int m_endgame;
    //int boardsq[34];
    int m_hisindex;
    unsigned char boardsq[34];
//    2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 32,33
//    p P  p P p P  p P p  P   b  B  b  B   e  E  e  E   h  H  h  H   c  C  c  C   r  R  r  R   k  K
    unsigned char piece[BOARD_SIZE-7];

    unsigned short wBitRanks[10];  // 位行数组，注意用法是"p_BitRanks[Squares / 9]"   // /16 = >>4
    unsigned short wBitFiles[9];   // 位列数组，注意用法是"p_BitFiles[Squares % 9]"   // %16 = &15
HistRecord m_hisrecord[384]; //MAX_MOVES]; //256 befsf10 384
    
    int pointsum; // = 0;
unsigned int bitpiece; // = 0; //RrRrCcCc HhHhEeEe BbBbPpPp PpPpPp00
// piececnt[piece&61]
unsigned char piececnt[34]; // = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned int bitattk[10]; // = {0,0,0,0,0,0,0,0,0,0}; //[left-right][poscolor]  [4]=black central, [5]=red central
unsigned int bitpiece_attk; // = 0; //RrRrCcCc HhHhEeEe BbBbPpPp PpPpPpKk  //for eval forked rooks 

MoveTabStruct movetab[128]; // for smp  //111= 局面的合理着法数也不会超过"MAX_MOVE_NUM / 2"
int size; //for smp
int incheck; //for smp 
unsigned long long  m_nodes; //for smp
unsigned long long m_startime; //for smp
int m_bestmove; //for smp
int m_timeout; //for smp

int IMaxTime; // for smp
int root_depth;  // for smp
unsigned int thd_id; // for smp tracing in Board::functiions    
int nBanMoves;  // lazy smp - move to board.h 
int p_endgame;  //for smp
int moveCount[MAX_PLY];    //1024
int staticEval[MAX_PLY];   //indexed by board.ply, MAX_PLY=64, sf10=128   //1010 for futility improving setting 
MoveStruct currentMove[MAX_PLY]; //indexed by board.ply for countermove Square prevSq = to_sq((ss-1)->currentMove);
int excludedMove[MAX_PLY]; //indexed by board.ply for prevent recursive singular extension and nullmovereduction  //1109 sf10  
unsigned short* pv[MAX_PLY+1]; //1206 store pv to be printed out to GUI pv[ss]
public:
    Board();
    void print_board();
    void print_board(int score);
		void Init_index();
		void Compress_index();
    void ReSet();
    void Xor(uint64 &m1, const uint64 &m2);
    
//		int  MoveInChk(int from, int dest, int piecedest);
//	int  GenChkEvasion(MoveTabStruct movetab[], int incheck);	//1=rook/c/p only, 2=horse only, 3=both
//	int  GenChkEvasCap(MoveTabStruct movetab[], int incheck);	//1=rook/c/p file, 2=R rank, 3=L rank, 4=horse
    int  GenChkEvasCap(MoveTabStruct *movetab, int incheck); //, MoveTabStruct *ncapmovetab, int *ncapsize );
    int  GenChkEvasRookCann(MoveTabStruct *movetab, int incheck);
    //int  GenChkEvasNCap(MoveTabStruct movetab[], int incheck);
    int  GenChkEvasNCap(MoveTabStruct *movetab, int incheck);	//1=rook/c/p only, 2=horse only, 3=both
    //int  Gen(MoveTabStruct movetab[]); //, short pv);
//template<int t_side>
    int  GenCap(MoveTabStruct *movetab, MoveTabStruct *ncapmovetab, long &ncapsize );
//    int  GenCapQS(MoveTabStruct *movetab, MoveTabStruct *ncapmovetab, int &ncap );
template<int q_side>
    int  GenCapQS(MoveTabStruct *movetab);
    int  GenNonCap(MoveTabStruct *movetab, int depth);

    int  generate_checks(MoveTabStruct *movetab);
    bool IsEnd()const;

//	bool Attacked(int pos,int side)const;
//	bool LegalMove(int from, int dest);
    bool LegalMove(unsigned short move);
//	int  MakeMove(int from,int dest);
    int  MakeMove(MoveStruct &move);
    void UnMove(int from,int dest,int capture);
//1002    int see(int from, int dest);
//1002    int see(int from, int dest, int &val);
    bool Board::non_pawn_material(int m_side); //true if rrccnn !=0
    int non_pawn_material();  //Rr * VRE + Cc * VCE + Hh * VNE
    bool see_ge(int from, int dest, int threshold); 

    int  makemovesee(int from, int dest);
    void unmakemovesee();

    int IsCannonDiscovCheck(int side);
    int IsInSuperCheck(int side);
template<int singlechk>
    int IsInCheck(int side); //, int singlechk);
    int IsInCheckbyRookCann(int DstSq, int bbr);

    bool gives_check(int from, int dest); //1003
    int IsChasing(int chaserpos, int pos);
template<int side>
    int IsAttackedBy(int pos); //(int side, int pos);
template<int side>
    int Countkingattkpt(int pos, int oppkingpos); //(int side, int pos);
    int get_smallest_attacker(int pos); //, int &see_getcnt);
    int  LegalKiller(MoveStruct &moveS);
//	int GoodCap(MoveStruct mv);
//	void Eval_legalmv(int from, int dest, int color);
    int endgameKnowledge();
    int Evalattk_B();
    int Evalattk_R();
template<int side>
    int EvalKingSafety();
template<int side>
    int EvalHorsPawn();
    int Eval(); //(int alpha, int beta);
template<int poscolor>
    int EvalSide();
    //int EvalSide(int poscolor);

    int Eval_attack_threat(int side);
    int Eval_mate_threat();
// 1891j - functions moved from Engine.h to Board.h    
    int checkloop(int n);
    //template<int chkreq>    
    int makemove(MoveStruct &moveS, int chkreq);  //bhws said it can improve speed    
    void unmakemove(); //MoveStruct &moveS);
//template<int chkreq>    
    int makemovenoeval(MoveStruct &moveS, int chkreq);  //no eval e.g. pointsum/bitpiece update  
    void unmakemovenoeval(); //MoveStruct &moveS);

};
#endif
