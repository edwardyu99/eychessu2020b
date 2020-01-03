//#define DEBUG
#undef DEBUG

#define EVALHASH
//#undef EVALHASH
/*
Eychessu - a Chinese Chess Program Designed by Edward Yu, Version: 1.47

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
#ifndef _SEARCHENGINE_H
#define _SEARCHENGINE_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <memory.h>

#include <atomic>
#include <algorithm>  //std::max std::min

#include "board.h"
#include "pregen.h"

extern void init_mersenne(void);
extern unsigned long genrand_int32(void);
extern unsigned long long genrand_int64(void);
//extern HistStruct m_his_table[90*32];

//typedef unsigned long long uint64;
/*
struct uint64
{
unsigned int x1;
unsigned int x2;
};
*/
/*
struct search_stack_t {
    int pv[128];
//    MoveStruct g_killer[2];
//    MoveStruct g_matekiller;
//  move_stack_t *ms_ptr;
//  move_t pv[MAX_DEPTH], move, killer, killer2, mate_killer, threat_move;
//  int eval;
//  bool evaluated;
//  eval_vector_t eval_vector;
//  int check;
//  int num_of_evasions;
//  int extension;
//  bool reduction;
//  bool last_moved_piece_hanging;
};
*/
/*
// 16 bytes
struct HashStruct
{
    uint64 hkey;
//	int value;
    unsigned short mvBest;
    //unsigned short wFresh:4, wAlphaDepth:6, wBetaDepth:6;
    unsigned char hAlphaDepth; 
    //unsigned char hSingular:1, hAlphaDepth:7;
    unsigned char hBetaDepth;	
    short hAlphaVal, hBetaVal;
    
//	unsigned char flag;
//	unsigned char depth;
};
*/
/*
typedef union _hVal_hFlag
{
	           short hVal:14, filler1:2;
    unsigned short filler2:14, hFlag:2;
	
} hVal_hFlag;
*/
//#define get_hmv(mv) ( ((((mv/89)/9*16)+((mv/89)%9)) <<8) + ((mv%89)/9*16)+((mv%89)%9) )
//#define set_hmv(mv) ( (nRank(mv>>8)*9+nFile(mv>>8)) * (nRank(mv&255)*9+nFile(mv&255)) )
//#define get_hmv(mv) ( (((int((mv>>7)/9)<<4)+((mv>>7)%9)) <<8) + (int((mv&127)/9)<<4)+((mv&127)%9) )
//#define set_hmv(mv) ( ((nRank(mv>>8)*9+nFile(mv>>8)) <<7) + (nRank(mv&255)*9+nFile(mv&255)) )

#define get_hmv(mv) (mv)  //1126
#define set_hmv(mv) (mv)  //1126
// for convert hmvbest in hash
//1106 #define get_hmv(mv) ( (((int((mv>>7)/9)<<4)+((mv>>7)%9)) <<8) + (int((mv&127)/9)<<4)+((mv&127)%9) )
//1126 #define get_hmv(mv) ( ((((int(mv>>7)/9)<<4)+((mv>>7)%9)) <<8) + ((int(mv&127)/9)<<4)+((mv&127)%9) )
//#define set_hmv(mv) ( ((nRank(mv>>8)*9+nFile(mv>>8)) <<7) + (nRank(mv&255)*9+nFile(mv&255)) )
//1126 #define set_hmv(mv) ( (((mv>>12)*9+(mv>>8)&15)<<7) + ((mv&255)>>4)*9+  (mv&15) )   //1106  4bit from-rank, 4bit from-file, 4bit to-rank, 4bit to-file-->7bitfrom,7bitto
//
/* 2019p 8 bytes
#pragma pack(push,1)  // push current alignment to stack 
//#pragma pack(1)     // set alignment to 1 byte boundary 
struct HashStruct
{
    unsigned long  hkey:26, hDepth:6;     //x64 long instead int
    unsigned short hFlag:2, hmvBest:14;   //1106 7bitfrom, 7bitto  
    short hVal;           //bef ABDADA     //1106 hval -32767 to +32767
//short hFlag:2, hVal:13, hExclusive:1;   //for ABDADA //1101 not use
};
#pragma pack(pop)   // restore original alignment from stack 
*/
// 8 bytes //2019q
#pragma pack(push,1)  // push current alignment to stack 
//#pragma pack(1)     // set alignment to 1 byte boundary 
struct HashStruct
{
    unsigned long  hkey:26, hDepth:6;     //x64 long instead int
    unsigned short hmvBest;               //1126 8bit from 8bit dest
    short hFlag:2, hVal:14;               //1126 hval -8192 to +8191
};
#pragma pack(pop)   // restore original alignment from stack 

/*
//2019r 8 bytes
#pragma pack(push,1)  // push current alignment to stack 
//#pragma pack(1)     // set alignment to 1 byte boundary 
struct HashStruct
{
	typedef union _hkey_hDepth
  {
	  unsigned long hkey:25, filler1:7;
             long filler2:25, hDepth:7;
  } hkey_hDepth;
//    unsigned long hkey:25,  hDepth:7;         //32bits  4bytes
    unsigned short hmvBest;      //16bits  2bytes
    short hFlag:2, hVal:14;      //16bits  2bytes      
};
#pragma pack(pop)   // restore original alignment from stack 
*/
/*
// 10 bytes 
#pragma pack(push)  // push current alignment to stack 
#pragma pack(2)     // set alignment to 1 byte boundary 
struct HashStruct
{
    unsigned int hkey;    
    unsigned short hmvBest;  
    short hVal;    
    unsigned char hDepth;
    unsigned char hFlag:2, hGen:6;	    
};
#pragma pack(pop)   // restore original alignment from stack 
*/
/*
typedef union _MoveStruct
{
	unsigned short move;
	struct
	{
	unsigned char dest;
	unsigned char from;
	};
} MoveStruct;
*/

/*
// 8 bytes
typedef union _EvalHash
{
    
    uint64 hkey64;
    struct
    {        
        short filler2;
        short value;
        unsigned int hkey32; //upper 32bits for lock
    };      
} EvalHash;
*/

#pragma pack(push,2)  // push current alignment to stack 
//#pragma pack(2)     // set alignment to 2 byte boundary 
// 6 bytes
typedef union _EvalHash
{
    struct
    {                
        //unsigned short hkey16_1;
        //unsigned short hkey16_2;
        unsigned long hkey32; //upper 32bits for lock  //x64 long instead int
        short value;
    };     
} EvalHash;
#pragma pack(pop)   // restore original alignment from stack 
/*
// 4 bytes
typedef union _EvalHash
{        
    unsigned int hkey32;
    struct
    {                
        //short filler2;
        //short filler3:4, value:12;  
        unsigned int fillerhkey:20, value:12;      
    };      
} EvalHash;
*/
/*
// 4 bytes
struct EvalHash {
	unsigned int hkey:20, value:12;  
};
*/	

// 10 bytes
/*
struct EvalHash
{
	uint64 hkey64;
	short value;
};
*/

// 置换表信息
extern unsigned int nHashMask;        // 置换表的大小，以及附加表中已经记录的局面数
extern HashStruct *hshItems; // 置换表的指针，ElephantEye采用多层的置换表

#ifdef EVALHASH
extern unsigned int nEvalHashMask;
extern EvalHash *evalhashItems;
#endif


inline void ClearHash() {         // 清空置换表
    memset(hshItems, 0, (nHashMask + 1) * sizeof(HashStruct));
#ifdef EVALHASH
    memset(evalhashItems, 0, (nEvalHashMask + 1) * sizeof(EvalHash));
#endif

}


//NewHash(25);     //(25); // 22=4MB, 23=8MB, 24=16MB, 25=32MB, 26=64MB, 27=128MB  ... sizeof(HashStruct) = 16 bytes
inline void NewHash(long nHashScale) { // 分配置换表，大小是 2^nHashScale 字节

    nHashMask = ((1 << nHashScale) / sizeof(HashStruct)) - 1;
    hshItems = new HashStruct[nHashMask +1+16]; //befx64 +1]; // + 4]; //+4(8byte)// + 16]; // + 32]; //+8 (12byte)] //+16(9 10byte)]; // + 4 (8byte)]; // + 2 (16byte)];
     
    //hshItems += 2; //16byte hash
#ifdef _WIN64    
    hshItems += 2; //8byte hash  8x2=16 filler (ptr arith * sizeof hshItems
#endif    
    
    //hshItems += 16; //10byte hash
    //hshItems += 8; //12byte hash
    //hshItems += 32; //9byte hash

    if ( ((unsigned long)hshItems % 64 ) == 0 && hshItems != NULL)
    {
        //printf("     hshItems pointer, %d is aligned on 64\n", hshItems);
        //fflush(stdout);
    }
    else
    {	
        printf("     ***hshItems pointer, %d is not aligned on 64, need filler %d\n",hshItems, 64 - ((unsigned long)hshItems % 64 ));
        fflush(stdout);
    }
#ifdef EVALHASH
//evalhash = 1<<16 = 64k*8 = 512kB
    //nEvalHashMask = ((1 << (nHashScale - 1)) / sizeof(HashStruct)) - 2;  // - 2		// -1 half hash
    nEvalHashMask = ((1 << (19 )) ) - 1; //20 (84b) //21=16MB, 22=32MB 
    //evalhashItems = new EvalHash[nEvalHashMask + 1 + 4]; // + 4]; //16];   //8byte evalhash
    //evalhashItems += 4; //4; //8; //8byte evalhash
    //evalhashItems = new EvalHash[nEvalHashMask + 1 + 14];    //4byte evalhash
    //evalhashItems += 14;  //4byte evalhash
    evalhashItems = new EvalHash[nEvalHashMask+1+24]; //befx64 + 1]; // + 16];    //6byte evalhash
#ifdef _WIN64
    evalhashItems += 24;   //24*6=144= filler16+(64*2)
#endif   
    if ( ((unsigned long)evalhashItems % 64 ) == 0 && evalhashItems != NULL)
    {
        //printf("     evalhashItems pointer, %d is aligned on 64\n", evalhashItems);
        //fflush(stdout);
    }
    else
    {	
        printf("     ***evalhashItems pointer, %d is not aligned on 64, need filler %d\n",evalhashItems,64-((unsigned long)evalhashItems % 64 ) );
        fflush(stdout);
    }   
#endif

    ClearHash();
}
    


inline void DelHash(void) {           // 释放置换表



#ifdef EVALHASH
    //evalhashItems -= 4; //4; //8;  //8byte evalhash
    //evalhashItems -= 14; //4; //8;  //4byte evalhash
#ifdef _WIN64    
    evalhashItems -= 24; //4; //8;  //6byte evalhash
#endif    
    delete[] evalhashItems;
#endif

    //hshItems -= 2; //16byte hash
#ifdef _WIN64    
    hshItems -= 2; //8byte hash
#endif    
//      hshItems -= 16; //10byte hash
    //hshItems -= 8; //12byte hash
    //hshItems -= 32; //9byte hash
    delete[] hshItems;

}








/*
struct PVline
{

	int num;
	//MOVE move[40];
	MoveStruct PVmove[40];
	PVline(){num=0;}
};
*/


class Engine
{
    //初始化
public:
    //Engine(int cside=BLACK,int depth=64,int time=0xefff):IComSide(cside),IMaxDepth(depth),IMaxTime(time)
    //Engine(int depth=64,int time=0xefff):IMaxDepth(depth),IMaxTime(time)	
    Engine(int depth=64):IMaxDepth(depth)
    {
        init_mersenne();
        init_parms(); // re-init BasicValues for optimization 1892q
        PreMoveGen();
        PreGenInit();
        Setpointtable(1);
        Setpointtable(0);	//leave pointtableABS at p_endgame==0
        init_hvalue();
//        m_timeout=0;  //moved to board.h for smp 
//        m_nodes=0;                        
        //ply=0;
        //Ilog[0]='\0';
        //IHkey=init_hkey(IBoard); //, IZkey);
        //board.hkey=IHkey;
    }
    ~Engine()
    {
//		delete []m_hash;
//		delete []m_evalhash;
        //fclose(traceout);
    }
    /*
    	void ReSet()
    	{	Setpointtable(0);
    		p_endgame = 0;
    		init_hvalue();
    		Board temp;
    		IBoard=temp;
    		board=temp;
    		IComSide=BLACK;
    		IMaxDepth=32; //64;  // 32
    		IMaxTime=0xefff;
    //		m_hsize=0x080000; //0x140000; //0x100000;
    //		m_hsize_1=m_hsize-1;
    		m_timeout=0;
    		//m_hisindex=0;
    //		memset(RepHash,0,sizeof(RepHash));
    		ply=0;
    		IHkey=init_hkey(IBoard); //, IZkey);
    		hkey=IHkey;
    //		zkey=IZkey;
    //		memset(m_hash,0,sizeof(HashStruct[2])*m_hsize);
    //		memset(m_evalhash,0,sizeof(EvalHash)*m_hsize);
    //		memset(m_his_table,1,sizeof(m_his_table[90*32]) * 90 * 32);

    		//memset(HistVal,0,sizeof(HistVal));
    		//memset(HistTot,1,sizeof(HistTot));
    		//memset(HistHit,1,sizeof(HistHit));


    		//p_endpoint_chg = 0;
    		bookfound = 1;
    		nullreduction=3; //2; //3;
    		nullreductver=5; //6; //5 //9; //8; //endgame=6; //6; //5;
    		HistPrunShift=1; //2; //1;
    		HistCutShift=3; //endgame=4
    		ExtHistPrunShift=2; //2;
    		ExtHistDepth=6;
    		razor_depth=2; //endgame=0
    		histcut_depth=4; //endgame=0
    		//HistPrunShiftNeg=4;
    		//HistPrunBound=7373; //6554;  //45%=7373  40%=6554, 35%=5734  30%=4915
    		//Qcheckdepth=-1;  //endgame=-3
    	}
    */




//内部相关
private:
    //int ply;
    //uint64 hkey;
//	unsigned long hkey0, hkey1;
//  	unsigned long zkey;
//    Board board;
    //int  m_hisindex;
//	int  m_hsize, m_hsize_1;	// for evalhash only
//    unsigned long long  m_nodes;  //for smp, move to board.h 
    //unsigned int  m_time_check;
    //int m_timeout;  //for smp, move to board.h

    //int  m_startime;
    //unsigned long long m_startime; //for smp, move to board.h
//lazy smp    int m_depth;	//depth at root 

//    int nBanMoves;  // lazy smp - move to board.h            // 随机性屏蔽位和禁着数
    //short wmvBanList[128]; //[MAX_MOVE_NUM]; // 禁着列表





//    int bookfound;


    //int m_bestmove;   //lazy smp move to board.h
    //unsigned int nQcheck;
    //Hist m_hisrecord[MAX_MOVE_NUM];
//	int RepHash[REP_HASH_LEN]; // 判断重复局面的迷你置换表
//	int  m_pvline[MAX_MOVE_NUM];
//	int  LegalKiller(MoveStruct &moveS);
    void printf_info_nps(int best, Board &board);
    
    uint64 Perft(int depth);
// 1891j - functions moved from Engine.h to Board.h        
/*
    int checkloop(int n);
//	int IsBanMove(int mv);

 
    //int makemove(MoveStruct &moveS);  //bhws said it can improve speed 
//template<int chkreq>    
    int makemove(Board &board, MoveStruct &moveS, int chkreq);  //bhws said it can improve speed    
    void unmakemove(Board &board); //MoveStruct &moveS);
template<int chkreq>    
    int makemovenoeval(Board &board, MoveStruct &moveS); //, int chkreq);  //no eval e.g. pointsum/bitpiece update  
    void unmakemovenoeval(Board &board); //MoveStruct &moveS);
*/
  int Lazy_smp_ID_loop(unsigned int idx, Board* spboard); //, MoveTabStruct movetab[]);  // idx=thread# 0=mainthread
//  int Lazy_smp_ID_loop(unsigned int idx, Board* spboard, std::atomic_long &thddepth); //, MoveTabStruct movetab[]);  // idx=thread# 0=mainthread
//    int Lazy_smp_ID_loop(unsigned int idx, Board* spboard, std::atomic_bool &thdabort);
	
    int searchRoot();
//nodetype 0=NonPV, 1=PV    
template<int nodetype>    
    int search(Board &board, int alpha, int beta, int depth, int null_allow, move_t pv[]);  //1210 use recursive pv
//    int searchPV(Board &board, int alpha, int beta, int depth, int null_allow);   
//    int searchNonPV(Board &board, int alpha, int beta, int depth, int null_allow);
 
//template<int PVNode>
//1007    int quiesCheckPV(Board &board, int alpha,int beta, int qdepth, int check_depth); //, int PVNode); //, int qcheck);
//1007    int quiesCheck(Board &board, int beta, int qdepth, int check_depth); //, int qcheck);
//1123    int quiesCheckPV(Board &board, int alpha,int beta, int qdepth); 
template<int nodetype>       
int quiesCheck(Board& board, int alpha, int beta, int qdepth); //1224 , move_t pv[]);  //1210 use recursive pv 
    void PrintLog(char *FileName, Board &board);    
    int Evalscore(Board &board); //(int alpha,int beta);    
    void print_board(int score);

    //uint64 h_value[BOARD_SIZE-7][10]; //[16]; // [32]; //[34][90]; 
//    uint64 h_value[10][9][10];   //[piecetype][nFile}[nRank]
//    uint64 h_rside;

//	uint64 h_bside;

    // Zobrist键值表，分Zobrist键值和Zobrist校验锁两部分
//  unsigned long z_rside;
//  unsigned long z_value[90][32];

    /*
    unsigned long  rand32(){
    	//return rand()^((long)rand()<<15)^((long)rand()<<30);
    	//return rand() ^ ((long)rand() << 15) ^ ((long)rand() << 30);
    //许多资料使用上面的随机数，0<=rand()<=32767, 只能产生0～max(unsigned long)/2之间的随机数，显然很不均匀，会增加一倍的冲突几率

    return ((unsigned long)(rand()+rand())<<16) ^ (unsigned long)(rand()+rand());	//改进后应该是均匀分布的

    	}
    uint64 rand64() {
    	uint64 x, y;
      	x = rand32(); y = rand32();
      	return (x<<32)|y;
    }
    */
/*    
    void Xor(uint64 &m1, const uint64 &m2)
    {
        m1 ^= m2;
    }
    //{m1.x1 ^= m2.x1; m1.x2 ^= m2.x2;}
    void Xor32(unsigned int &m1, const unsigned int &m2)
    {
        m1 ^= m2;
    }
*/    
    void init_parms();
    void PreMoveGen();
    void Setpointtable(int endgame);
    void AdjustEndgame(Board &board);
    void init_hvalue();
    //uint64 get_hkey();////测试用
    uint64 init_hkey(const Board &board); //, unsigned long &zkey);
    void HashAging(void); 
    int value_from_tt(short vl, int ply);   //1107 for ttValue sf10
    void RecordHash(int flag,int depth,int value, int mv, Board &board); //, int nSingular);
    //void RecordHashQ(int flag, int value, int mv, Board &board);
    void RecordHashQ(int flag, int ttdepth, int value, int mv, Board &board);  //1021 ttdepth = 0 or -1 encoded as 2 or 1
    int ProbeHash(int depth, int beta, int null_allow, HashStruct* &hsho, Board &board); 
    int ProbeHashQ(int beta, int &mv, Board &board, int &hashdepth);  //1007 return hashdepth    
    HashStruct* ProbeMove(Board &board);
    int ProbeMoveQ(Board &board);
//1101    int ProbeHashExcl(Board &board, int nDepth);  //ABDADA
//1101     void RecordHashExcl(Board &board, int hExclusive, int nDepth);   //ABDADA
    void GetPvLine(Board &board, move_t *lpwmvPvLine, int mvFirst);                  // 从置换表中获得主要变例路线
    //int GetPvStable(int *lpwmvPvLine, int mvFirst);
    void PutPvLine(Board &board, int *PvLine, int depth, int best);
    //JNIEnv* m_env;
// statistics?
#ifdef DEBUG
    int searchdepth;
    unsigned int nTreeNodes;
    unsigned int nLeafNodes;
    unsigned int nQuiescNodes;

    unsigned int nTreeHashHit;
    unsigned int nLeafHashHit;

    unsigned int nNullMoveNodes;
    unsigned int nNullMoveCuts;

    unsigned int nHistPrunNodes;
    unsigned int nExtHistPrunNodes;
    unsigned int nHistPrunVers;

    unsigned int nHashMoves;
    unsigned int nHashCuts;

    //unsigned int nKillerNodes[2];
    //unsigned int nKillerCuts[2];

    unsigned int nBetaNodes;
    unsigned int nBetaMoveSum;
    unsigned int nBetaCutAt1;

    unsigned int nFutility;
    unsigned int nExtFutility;
//	unsigned int nSingleExt;


    //unsigned int nCheckCounts[5];
    //unsigned int nCheckEvasions;
#endif

//界面接口
public:
    Board board;
//    Board spboardref;  //for smp
//    Board IBoard;
//    uint64 IHkey;
//	unsigned long IZkey;
//	unsigned long IHkey0, IHkey1;
//    char  Ilog[4096];
//    int  IComSide;
    int  IMaxDepth;
    
//    int  IMaxTime;
    //H ADD 2006
    int  CheckStop();

    //void  IMakeMove(int from,int dest);
    void  IMakeMove(Board &board, MoveStruct &move);
    //void  CmdIMakeMove(int from_x,int from_y,int dest_x,int dest_y);
//	void  IUnMakeMove();
    void  printBestmove(int bestmove);
    void  ISearch();
    void  IRead(Board &board, const char *fen);
//	void  IComMove();
    void  IStopThink(){
    //    m_timeout= -1;
    }
};
#endif
