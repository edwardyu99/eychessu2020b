/*
pregen.h/pregen.cpp - Source Code for ElephantEye, Part II

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 1.2, Last Modified: Feb. 2006
Copyright (C) 2004-2006 www.elephantbase.net

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
#include <stdio.h>
//#include "../utility/x86asm.h"
//#include "../utility/longrand.h"
#include "pregen.h"

#ifndef _WIN32
#define __int64 long long
#endif

/* 棋盘区域表，这张表可以明显看出ElephantEye的16x16棋盘结构。
 * 布局：信封式结构，红下黑上，上下左边线留3行，右边线留4行；
 * 技巧：判断格子是否过河，可以用"sd == 0 ? (sq & 0x80) == 0 : (sq & 0x80) != 0"；
 * 技巧：判断着法是否过河，可以用"((sqSrc ^ sqDst) & 0x80) == 0"；
 * 技巧：要得到格子的行号/列号，可以用"sq / 16"/"sq % 16"；
 * 技巧：根据行号和列号得到格子编号，可以用"y * 16 + x"；
 * 技巧：判断格子是否在棋盘上，可以用"cbcInBoard[sq]"；
 * 技巧：判断格子是否在九宫里，可以用"cbcInFort[sq]"。
 */
/*
const BoolChar cbcInBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const BoolChar cbcInFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
*/
/* 守子着法合理性表和马腿表，用来快速判断守子的着法是否合理，或者哪个格子是马腿。
 * 技巧：守子从 sqSrc 走到 sqDst，由"ccLegalMoveTab[sqDst - sqSrc + 256]"来判断合理性；
 * 技巧：马从 sqSrc 走到 sqDst，马腿的格子是："sqSrc + ccHorseLegTab[sqDst - sqSrc + 256]"；
 *                                    或者是："sqSrc - ccHorseLegTab[sqSrc - sqDst + 256]"。
 */
/*
const char ccLegalMoveTab[512] = {
                       0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

const char ccHorseLegTab[512] = {
                               0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0
};
*/
PreGenStruct PreGen;

/*
// 这四个数组用来判断棋子的走子方向，以马为例就是：sqDst = sqSrc + cnKnightMoveTab[i]
static const int cnKingMoveTab[4]    = {-0x10, -0x01, +0x01, +0x10};
static const int cnAdvisorMoveTab[4] = {-0x11, -0x0f, +0x0f, +0x11};
static const int cnBishopMoveTab[4]  = {-0x22, -0x1e, +0x1e, +0x22};
static const int cnKnightMoveTab[8]  = {-0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21};
*/
void PreGenInit(void) {
    //printf("info PreGenInit start\n");
    int i, j, k, sqSrc; //n,sqDst;
//  unsigned long RandSeed;
    SlideMoveStruct smv;
    SlideMaskStruct sms;
    /*
      RandSeed = 1;
      PreGen.dwZobristKeyPlayer = LongRand(RandSeed);
      for (i = 0; i < 14; i ++) {
        for (j = 0; j < 256; j ++) {
          PreGen.dwZobristKeyTable[i][j] = LongRand(RandSeed);
        }
      }
      ((unsigned long *) &PreGen.qwZobristLockPlayer)[0] = LongRand(RandSeed);
      ((unsigned long *) &PreGen.qwZobristLockPlayer)[1] = LongRand(RandSeed);
      for (i = 0; i < 14; i ++) {
        for (j = 0; j < 256; j ++) {
          ((unsigned long *) &PreGen.qwZobristLockTable[i][j])[0] = LongRand(RandSeed);
          ((unsigned long *) &PreGen.qwZobristLockTable[i][j])[1] = LongRand(RandSeed);
        }
      }
    */
    /*  for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
        if (cbcInBoard[sqSrc]) {
          PreGen.wBitRankMask[sqSrc] = 1 << ((sqSrc % 16) - 3);
          PreGen.wBitFileMask[sqSrc] = 1 << ((sqSrc / 16) - 3);
        } else {
          PreGen.wBitRankMask[sqSrc] = 0;
          PreGen.wBitFileMask[sqSrc] = 0;
        }
      }
    */
    for (sqSrc = 0; sqSrc < 160-7; sqSrc ++) {

        //PreGen.wBitRankMask[sqSrc] = 1 << ((sqSrc % 16) );
        //PreGen.wBitFileMask[sqSrc] = 1 << ((sqSrc / 16) );

        if ((sqSrc &15) >=9)
        {
            PreGen.wBitRankMask[sqSrc] = 0;
            PreGen.wBitFileMask[sqSrc] = 0;
        }
        else
        {
            PreGen.wBitRankMask[sqSrc] = 1 << ((sqSrc &15) );
            PreGen.wBitFileMask[sqSrc] = 1 << ((sqSrc >>4) );
        }
    }

    // 首先生成车炮着法的预置数组(数组的应用参阅"pregen.h")
    for (i = 0; i < 9; i ++) {
        for (j = 0; j < 512; j ++) {
            // 初始化借助于“位行”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"SlideMoveTab"，假设没有着法，用起始格填充
            smv.NonCap[0] = smv.NonCap[1] = smv.RookCap[0] = smv.RookCap[1] =
                            smv.CannonCap[0] = smv.CannonCap[1] = smv.SuperCap[0] = smv.SuperCap[1] = i; //i + 3;
            smv.NonCapCannonCnt[0] = smv.NonCapCannonCnt[1] = 0;
            sms.NonCap = sms.RookCap = sms.CannonCap = sms.SuperCap = 0;
            // 提示：参阅"pregen.h"，...[0]表示最大一格，向右移动和下移动都用[0]，反之亦然
            // 2. 考虑向右移动的目标格，填充...[0]，
            for (k = i + 1; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.RookCap[0] = k; //k + 3;
                    sms.RookCap |= 1 << k;
                    break;
                }
                smv.NonCap[0] = k; //k + 3;
                sms.NonCap |= 1 << k;
            }
            for (k ++; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.CannonCap[0] = k; //k + 3;
                    sms.CannonCap |= 1 << k;
                    break;
                }
            }
            for (k ++; k <= 8; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.SuperCap[0] = k; //k + 3;
                    sms.SuperCap |= 1 << k;
                    break;
                }
            }

            if (smv.NonCap[0] != i)
                smv.NonCapCannonCnt[0] = smv.NonCap[0] - i;


            // 3. 考虑向左移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.RookCap[1] = k; //k + 3;
                    sms.RookCap |= 1 << k;
                    break;
                }
                smv.NonCap[1] = k; //k + 3;
                sms.NonCap |= 1 << k;
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.CannonCap[1] = k; //k + 3;
                    sms.CannonCap |= 1 << k;
                    break;
                }
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.SuperCap[1] = k; //k + 3;
                    sms.SuperCap |= 1 << k;
                    break;
                }
            }

            if (smv.NonCap[1] != i)
                smv.NonCapCannonCnt[1] = i - smv.NonCap[1] ;

            //smv.NonCapCnt = smv.NonCapCannonCnt[0] + smv.NonCapCannonCnt[1];
						/* suppress to debug kingsuicide
						// change to 0 for faster check: (if DstSq != SrcSq) becomes (if DstSq !=0) 
						for (int n=0; n<2; n++) {
						if (smv.NonCap[n] == i) smv.NonCap[n] = 0; else  smv.NonCap[n]++; 
						if (smv.RookCap[n] == i) smv.RookCap[n] = 0; else smv.RookCap[n]++;
						if (smv.CannonCap[n] == i) smv.CannonCap[n] = 0; else smv.CannonCap[n]++;		 
						if (smv.SuperCap[n] == i) smv.SuperCap[n] = 0; else smv.SuperCap[n]++;
					}	
					*/
					
            // 4. 将临时变量"SlideMoveTab"拷贝到着法预生成数组中
            PreGen.smvRankMoveTab[i][j] = smv;
            PreGen.smsRankMaskTab[i][j] = sms;
            
            
        }
    }

    for (i = 0; i < 10; i ++) {
    	int i16 = (i<<4); 
        for (j = 0; j < 1024; j ++) {
            // 初始化借助于“位列”的车和炮的着法预生成数组，包括以下几个步骤：
            // 1. 初始化临时变量"smv"，假设没有着法，用起始格填充
            smv.NonCap[0] = smv.NonCap[1] = smv.RookCap[0] = smv.RookCap[1] =
                                                smv.CannonCap[0] = smv.CannonCap[1] = smv.SuperCap[0] = smv.SuperCap[1] = (i16); //i * 9; //(i + 3) * 16;
            smv.NonCapCannonCnt[0] = smv.NonCapCannonCnt[1] = 0;
            sms.NonCap = sms.RookCap = sms.CannonCap = sms.SuperCap = 0;
            // 2. 考虑向下移动的目标格，填充...[0]
            for (k = i + 1; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.RookCap[0] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.RookCap |= 1 << k;
                    break;
                }
                smv.NonCap[0] = (k<<4); //k * 9; //(k + 3) * 16;
                sms.NonCap |= 1 << k;
            }
            for (k ++; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.CannonCap[0] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.CannonCap |= 1 << k;
                    break;
                }
            }
            for (k ++; k <= 9; k ++) {
                if ((j & (1 << k)) != 0) {
                    smv.SuperCap[0] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.SuperCap |= 1 << k;
                    break;
                }
            }

            if (smv.NonCap[0] != (i16)) //(i * 9))
                //smv.NonCapCannonCnt[0] = (smv.NonCap[0] - (i * 9)) / 9;
                smv.NonCapCannonCnt[0] = ((smv.NonCap[0] - (i16)) >>4);

            // 3. 考虑向上移动的目标格，填充...[1]
            for (k = i - 1; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.RookCap[1] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.RookCap |= 1 << k;
                    break;
                }
                smv.NonCap[1] = (k<<4); //k * 9; //(k + 3) * 16;
                sms.NonCap |= 1 << k;
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.CannonCap[1] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.CannonCap |= 1 << k;
                    break;
                }
            }
            for (k --; k >= 0; k --) {
                if ((j & (1 << k)) != 0) {
                    smv.SuperCap[1] = (k<<4); //k * 9; //(k + 3) * 16;
                    sms.SuperCap |= 1 << k;
                    break;
                }
            }

            if (smv.NonCap[1] != (i16)) //(i * 9))
                //smv.NonCapCannonCnt[1] = ((i * 9) - smv.NonCap[1] ) / 9;
                smv.NonCapCannonCnt[1] = (((i16) - smv.NonCap[1] ) >>4);

            //smv.NonCapCnt = smv.NonCapCannonCnt[0] + smv.NonCapCannonCnt[1];
          /*  
            	// change to 0 for faster check: (if DstSq != SrcSq) becomes (if DstSq !=0) 
					for (int n=0; n<2; n++) {
						if (smv.NonCap[n] == i16) smv.NonCap[n] = 0; else  smv.NonCap[n]++; 
						if (smv.RookCap[n] == i16) smv.RookCap[n] = 0; else smv.RookCap[n]++;
						if (smv.CannonCap[n] == i16) smv.CannonCap[n] = 0; else smv.CannonCap[n]++;		 
						if (smv.SuperCap[n] == i16) smv.SuperCap[n] = 0; else smv.SuperCap[n]++;
					}	
*/
            // 4. 将临时变量"smv"拷贝到着法预生成数组中
            PreGen.smvFileMoveTab[i][j] = smv;
            PreGen.smsFileMaskTab[i][j] = sms;
        }
    }
    /*
      // 接下来生成着法预生成数组，连同将军预判数组
      for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
        if (cbcInFort[sqSrc]) {
          // 生成帅(将)的着法预生成数组
          n = 0;
          for (i = 0; i < 4; i ++) {
            sqDst = sqSrc + cnKingMoveTab[i];
            if (cbcInFort[sqDst]) {
              PreGen.ucsqKingMoves[sqSrc][n] = sqDst;
              n ++;
            }
          }
          PreGen.ucsqKingMoves[sqSrc][n] = 0;
          // 生成仕(士)的着法预生成数组
          n = 0;
          for (i = 0; i < 4; i ++) {
            sqDst = sqSrc + cnAdvisorMoveTab[i];
            if (cbcInFort[sqDst]) {
              PreGen.ucsqAdvisorMoves[sqSrc][n] = sqDst;
              n ++;
            }
          }
          PreGen.ucsqAdvisorMoves[sqSrc][n] = 0;
        } else {
          PreGen.ucsqKingMoves[sqSrc][0] = 0;
          PreGen.ucsqAdvisorMoves[sqSrc][0] = 0;
        }
        if (cbcInBoard[sqSrc]) {
          // 生成相(象)的着法预生成数组，包括象眼数组
          n = 0;
          for (i = 0; i < 4; i ++) {
            sqDst = sqSrc + cnBishopMoveTab[i];
            if (cbcInBoard[sqDst] && ((sqSrc ^ sqDst) & 0x80) == 0) {
              PreGen.ucsqBishopMoves[sqSrc][n] = sqDst;
              PreGen.ucsqElephantEyes[sqSrc][n] = (sqSrc + sqDst) >> 1;
              n ++;
            }
          }
          PreGen.ucsqBishopMoves[sqSrc][n] = 0;
          // 生成马的着法预生成数组，包括马腿数组
          n = 0;
          for (i = 0; i < 8; i ++) {
            sqDst = sqSrc + cnKnightMoveTab[i];
            if (cbcInBoard[sqDst]) {
              PreGen.ucsqKnightMoves[sqSrc][n] = sqDst;
              PreGen.ucsqHorseLegs[sqSrc][n] = sqSrc + ccHorseLegTab[sqDst - sqSrc + 256];
              n ++;
            }
          }
          PreGen.ucsqKnightMoves[sqSrc][n] = 0;
          // 生成兵(卒)的着法预生成数组
          for (i = 0; i < 2; i ++) {
            n = 0;
            sqDst = (i == 0 ? sqSrc - 16 : sqSrc + 16);
            if (cbcInBoard[sqDst]) {
              PreGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
              n ++;
            }
            if (i == 0 ? (sqSrc & 0x80) == 0 : (sqSrc & 0x80) != 0) {
              for (j = -1; j <= 1; j += 2) {
                sqDst = sqSrc + j;
                if (cbcInBoard[sqDst]) {
                  PreGen.ucsqPawnMoves[i][sqSrc][n] = sqDst;
                  n ++;
                }
              }
            }
            PreGen.ucsqPawnMoves[i][sqSrc][n] = 0;
          }
        }
      }

    */
}
