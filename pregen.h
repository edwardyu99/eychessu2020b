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

//#include "../utility/bool.h"

#ifndef PREGEN_H
#define PREGEN_H

#ifndef _WIN32
#define __int64 long long
#endif

/*
extern const BoolChar cbcInBoard[256]; // 棋盘区域表
extern const BoolChar cbcInFort[256];  // 九宫序列表
extern const char ccLegalMoveTab[512]; // 守子着法合理性表
extern const char ccHorseLegTab[512];  // 马腿表
*/

// 借助“位行”和“位列”生成车炮着法的预置结构
struct SlideMoveStruct {
    unsigned char NonCap[2];    // 不吃子能走到的最大一格/最小一格
    unsigned char RookCap[2];   // 车吃子能走到的最大一格/最小一格
    unsigned char CannonCap[2]; // 炮吃子能走到的最大一格/最小一格
    unsigned char SuperCap[2];  // 超级炮(隔两子吃子)能走到的最大一格/最小一格
    unsigned char NonCapCannonCnt[2];  // space moveable by cannon in two directions
    //unsigned char NonCapCnt;    // NonCapCannonCnt[0] + NonCapCannonCnt[1]
}; // smv

// 借助“位行”和“位列”判断车炮着法合理性的预置结构
struct SlideMaskStruct {
    unsigned short NonCap, RookCap, CannonCap, SuperCap;
}; // sms

extern struct PreGenStruct {
        /*
          // Zobrist键值表，分Zobrist键值和Zobrist校验锁两部分
          unsigned long dwZobristKeyPlayer;
          unsigned long dwZobristKeyTable[14][256];
          unsigned __int64 qwZobristLockPlayer;
          unsigned __int64 qwZobristLockTable[14][256];
        */
        unsigned short wBitRankMask[160-7]; // 每个格子的位行的屏蔽位
        unsigned short wBitFileMask[160-7]; // 每个格子的位列的屏蔽位

        /* 借助“位行”和“位列”生成车炮着法和判断车炮着法合理性的预置数组
         *
         * “位行”和“位列”技术是ElephantEye的核心技术，用来处理车和炮的着法生成、将军判断和局面分析。
         * 以初始局面红方右边的炮在该列的行动为例，首先必须知道该列的“位列”，即"1010000101b"，
         * ElephantEye有两种预置数组，即"...MoveTab"和"...MaskTab"，用法分别是：
         * 一、如果要知道该子向前吃子的目标格(起始格是2，目标格是9)，那么希望查表就能知道这个格子，
         * 预先生成一个数组"FileMoveTab_CannonCap[10][1024]"，使得"FileMoveTab_CannonCap[2][1010000101b] == 9"就可以了。
         * 二、如果要判断该子能否吃到目标格(同样以起始格是2，目标格是9为例)，那么需要知道目标格的位列，即"0000000001b"，
         * 只要把"...MoveTab"的格子以“屏蔽位”的形式重新记作数组"...MaskTab"就可以了，用“与”操作来判断能否吃到目标格，
         * 通常一个"...MaskTab"单元会包括多个屏蔽位，判断能否吃到同行或同列的某个格子时，只需要做一次判断就可以了。
         */
        SlideMoveStruct smvRankMoveTab[9][512];   // 36,864 字节
        SlideMoveStruct smvFileMoveTab[10][1024]; // 81,920 字节
        SlideMaskStruct smsRankMaskTab[9][512];   // 36,864 字节
        SlideMaskStruct smsFileMaskTab[10][1024]; // 81,920 字节
        // 共计:  237,568 字节

        /* 其余棋子(不适合用“位行”和“位列”)的着法预生成数组
         *
         * 这部分数组是真正意义上的“着法预生成”数组，可以根据某个棋子的起始格直接查数组，得到所有的目标格。
         * 使用数组时，可以根据起始格来确定一个指针"g_...Moves[Square]"，这个指针指向一系列目标格，以0结束。
         * 为了对齐地址，数组[256][n]中n总是4的倍数，而且必须大于n(因为数组包括了结束标识符0)，除了象眼和马腿数组。
         */
        /*
          unsigned char ucsqKingMoves[256][8];
          unsigned char ucsqAdvisorMoves[256][8];
          unsigned char ucsqBishopMoves[256][8];
          unsigned char ucsqElephantEyes[256][4];
          unsigned char ucsqKnightMoves[256][12];
          unsigned char ucsqHorseLegs[256][8];
          unsigned char ucsqPawnMoves[2][256][4];
        */
    } PreGen;

void PreGenInit(void);

#endif
