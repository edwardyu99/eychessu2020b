//#define THDINFO
#undef THDINFO  

//#define PREEVAL
#undef PREEVAL

//2892u - use stable_sort (stockfish-7)
#define ROOTLMR
//#undef ROOTLMR
#define HORSECK //1881e
//#undef HORSECK
//#define PERFT   //1880s
#undef PERFT
//#define DEBUG
#undef DEBUG
#define PRTBOARD
//#undef PRTBOARD
#undef PRINTEVA
#define HISTPRUN
//#undef HISTPRUN
#define HISTHIT
//#undef HISTHIT
#define QCHECK
//#undef QCHECK
#define QHASH
//#undef QHASH
#define EVALHASH
//#undef EVALHASH



//#define HISCUT
#undef HISCUT
//#define FUTILITY
#undef FUTILITY


//6A2 (no delta, no fut, no atthreat) -ufx (325)
//6A3 (no delta, no fut,    atthreat) -ufx (118)
//6A4 (no delta,    fut, no atthreat) -ufx (235)
//6A8 (no delta,    fut,    atthreat) -ufx (   )
//6A5 (   delta, no fut, no atthreat) -ufx (145)
//6A6 (   delta,    fut, no atthreat) -ufx (334)
//6A7 (   delta,    fut,    atthreat) -ufx (343)
//6A9 (   delta, no fut,    atthreat) -ufx (   )
//6AA (   delta,    fut,    atthreat) -ufx (325) combine rook eval()
//6B2 (   delta,    fut,    atthreat) -ufx (514) combine rook/pawn/cannon eval()
//6B4 -ufx (523) -ele (154)
//
/* Eychessu -
a Chinese Chess Program Designed by Edward Yu, Version: 1.885t

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

/* History
// 20191006 - 2019f no checktime for quiesce or non-main thread
// 20160806 - 2892v time usage similiar to XQMS
// 20160724 - 2892u max(8, (UcciComm.nTime/25) to prevent timeout
// 20160721 - 2892t use stockfish-7 aspwin
// 20160717 - 2892s abort helper threads if main thread ends, using atomic_bool thdabort
//            2892s - use stable_sort (stockfish-7)
// 20160717 - 2892s implement lazy smp (use c++11 thread), work but seems to create/destroy 
//                  with more overhead. Using future/async seems to reuse threads  
// 20160715 - 2892r implement lazy smp
2006-02-13  v0.93 - initial copy from HIce 1.02
2006-02-16  v0.93a - rename "xor" by "Xor". Fix mate finding for FEN 2b1ka3/4a4/4b4/9/9/2B3B2/9/3p5/3pp4/5K3 w - - 0 1
		disable nullmove at endgame.
2006-02-21  v0.94 - add srchbook.cpp using JNI invocation to read EYBOOK.DAT
2006-02-23  v0.95 - use c++ file-io and string tokenizer to read EYBOOK.DAT
2006-03-18  v0.96 - use aggressive nullmove R=3,4(depth>5) and history pruning 50%
2006-03-24  v0.97 - fix recapture extended bug, history pruning 25%, add repval
2006-04-14  v0.98 - 2level hash, MoveStruct
2006-05-17  v0.99 - nullred(3/3, 2/2), histprun(1/3, 2/4)
2006-05-23  v1.00 - use bitrank, bitfile for rook/cannon capture and isincheck
2006-05-27  v1.01 - this_pv[2]
2006-05-31  v1.02 - nullred(2/4, 2/2)+Eval(), histred(2/3, 2/3)
2006-05-31  v1.03 - fix fpv=1 after if val>thisalpha
2006-06-01  v1.03a- use phase loop. nullred(3/6, 2/2)+Eval(), histred(1/2, 2/3), no histprun, no futility
2006-06-04  v1.03b- null comment out depth <=nullreduction+1
2006-06-04  v1.03c- put root last pv to the back
2006-06-09  v1.04 - node_type, pv[2], attack_rook, nullred(3/6, 2/2)+Eval(), histred(1/2, 2/3)
2006-06-13  v1.04c- zkey, no evalhash, node_type, pv[2], attack_rook, nullred(3/6, 2/2)+pointsum, histred(1/2, 2/3)
2006-06-14  v1.04d- fix legalkiller elephan, fix gennoncapture() missing break error: rook and cannon illegal move
2006-06-15  v1.05a- update killer/hist if phase==3 only
2006-06-15  v1.05b- add RecordHash when Nullmove beta cutoff
2006-06-16  v1.06 - rewrite makemove, add have rook better than no rook in Eval
2006-06-16  v1.06a- g_killer[4]
2006-06-16  v1.06d- back to g_killer[2], remove rook better but add 20 in rook basicvalues in endgame
2006-06-19  v1.07 - add rook better. rewrite banned. fix legalmv isincheck to use wbitrank/file
2006-06-20  v1.08 - correct bottom cannon face king. add generate_checks if cannon superchecking.
2006-06-22  v1.09 - skip small protcapture (tabval<0) in quiesce. add endgame pointtable chg for low pawns.
2006-06-22  v1.09a -add if (!incheck && tabval<0) to fix loseLHA.fen bug. rewrite to add MoveTabStruct for tab and tabval
2006-06-26  v1.09d -use null_allow in a-b. null_allow in Nullmove and IID, else all null_allow=1 even at root. see glaurung.
2006-06-28  v1.09e -searchroot return immed if move_count==1. Try king move evasion extension.
2006-06-29  v1.1  - bug fix knight in legalkiller. add NonCapCnt in PreGen for Eval() rook mobility.
2006-06-30  v1.1a - rewrite cannon case in Eval() using pregen
2006-06-30  v1.1b - condense rook and cannon eval() in n,k loops
2006-07-06  v1.1h - add pointsum <= alpha in history pruning (prodeo 1.1)
2006-07-08  v1.12 - fix fen position moves ? (IMakeMove). use hist reduct% to 50%+12.5% = 62.5% and exthistshift=25%
2006-07-08  v1.13 - rewrite GenCap, Gen etc using tabptr. win JUPITER
2006-07-12  v1.14 - add SideKnight. revise rook mobility. reduce side pawn value.
2006-07-13  v1.14a - HistoryMoveNb=6, no exthistprune. win JUP
2006-07-16  v1.15 - add delta pruning
2006-07-19  v1.15a- delta margin=25 win eleeye1.6 (10min)
2006-08-10  v1.16 - aspiration window=40 (see Beowulf), win anita 0.2 (10min)
2006-08-14  v1.16e- HistoryMax=16384 win anita 0.1 (10min)
2006-09-15  v1.17 - add mate killer
2006-09-20  v1.18 - add quieskiller (see coonyl)
2006-09-25  v1.18f- nullreductver=8, (endgame=4), bis-ele value endgame 50->55  win anita
2006-09-28  v1.19 - nullred use eval()
2006-10-09  v1.20 - use 4-level hash (see eleeye 1.61)
2006-10-11  v1.21 - use 2 killers with killerscore (see lime). use pointtable for ordering in root, use histval alone in gen()
2006-10-17  v1.21win do not reassign tabval in root. use pointtableABS + histval in root and gen(). nullredver=9, endgame=6
2006-10-19  v1.21draw xena use smash nullreduction formula
2006-10-20  v1.22win add futility margin1=35, 2*RAZOR at endgame &&!mate_threat. (glaurung 121). incr hash to 64M. bug in TT (mate_threat)
2006-10-27  v1.22Ewin nullmove use eval-tempo, futilty use pointsum+25+razor[45], history 45%. endgame same razor and history.
		    endgame==0 for qcheck and king_evasion ext.  skip small capture even end_game.
2006-12-28  v1.24win3DC futility 40 extfut 100 extHist >>2 ( >>3 at endgame)	exthist capture==0
2006-12-30  v1.24awin3DC histshift >>2 at endgame, RAZOR*2 at endgame
2006-12-30  v1.24b  loop HistorySize only at 0,1  4,5  8,9 etc (see mask29.txt)
//0 0 2 3   4 5 6 7   8 9 10 11  12 13 14 15  16 17 18 19   20 21 22 23   24 25 26 27  28 29 30 31  32 33
//&011101 (& 0x1D)  ( &29)
//0 0 0 1   4 5 4 5   8 9 8 9    12 13 12 13  16 17 16 17   20 21 20 21   24 25 24 25  28 29 28 29  0 1
    bprb bprbbprb  bprbbprb    bb rb bb rb
2006-12-31  v1.25win-draw3DC   sort root moves by tabval in iterations
2007-01-05  v1.26draw anita    fix rook sac bug (suppress HashRecord in pv,cap,killer phase). update killer if capture==0
                               history pruning && movetab[i].tabval < 0
2007-01-07 v1.26a win anita    history pruning <<3 instead of <<4   pending fortattack and ADVISOR bonus
2007-01-08 v1.27  win anita    fix rook sac bug by if dropdown value of 40, board.m_bestmove = prev_bestmove
2007-01-14 v1.28  win 3DC      add qcheck. tempo=6, futility=50/125 end=60/150   no qskiller  nullmove if depth>0 recordhash
2007-01-16 v1.29  win 3DC      hist prune && gencount>=8 //10
2007-01-18 v1.30  win 3DC/ANI  unloop Eval() poscolor=0,1  remove Eval_legalmv and move central knight/cannon check to cannon/rook eval
2007-01-22 v1.31  win 3DC      HistCutShift=3 gen_count>=9 capture==0   Histprun tabval < 3500
2007-01-24 v1.32  win 3DC      HistCutShift=3 gen_count>=9 capture==0 all. remove tabval < 3500. add singleext in search/QS.
2007-01-27 v1.33  win 3DC50    add kingdoor checking, add pawn-isattackedby, add mate_threat, histcutshift revert to >>4
2007-01-31 v1.34  win xen      update_pv, qcheck, check_depth -1, qhash, remove mate_threat, nullreduction=3,nullreductver=5
2007-02-10 v1.35  win anita    add FORTATTK, window=30 hist gencount>=10 delta=25
2007-03-11 v1.36               tabval=0 for protcapt
2007-03-13 v1.36a              clearhash() instead of aging(). fix river pawn. wincap <<10
2007-03-14 v1.37               GenChkEvasion()
2007-03-15 v1.37a              remove tempo_bonus. GenChkEvasion() incheck 1=pawn, 2=horse+p, 3=rook/cannon/horse/pawn
2007-03-16 v1.37b              incheck 1=rook/c/p file, 2=rook/c/p rank, 3=horse
2007-03-30 v1.38 win4          use bitpiece. quiesCheck() for ordering root nodes. use ponder move as root PV
2007-04-02 v1.38a              use prev_boardsq to derive p_feedback_move
2007-04-03 v1.38b              align prev_boardsq same piece type
2007-04-04 v1.39               sort root moves by board.m_nodes - start_nodes
2007-04-06 v1.39a              fix rookcapbug by call legalkiller for pv
2007-04-10 v1.40               add mate_threat in board.Eval() for nullmove. extend based on mate_threat no improvement
2007-04-12 v1.41               add back evalhash.
2007-04-17 v1.42               add pawnhash
2007-04-25 v1.43a		add banmove. suppress nobestmove. change evalscore back to pointsum in nullmove and futility
				add board.p_endgame==0 in futility and histcut. Great improvement!!! win eleeye 4-2-2 !!!
2007-04-26 v1.44		futility/history pruning before makemove, see toga
2007-05-30 v1.47    use 10x16 board for faster nFile, nRank
2007-07-14 v1.48    add attack_threat extension
2007-10-30 v1.52A2  dropdown_val 30/40, add attack_threat extension for only node_type==NodePV (win ufx 5-2-3)
2007-11-01 v1.52A7  dropdown_val 40/40, only cannon threat, board.p_endgame==0, (win ufx 5-1-4 short 1 min)
2007-11-01 v1.52A8  dropdown_val 40/40, full attack threat, board.p_endgame==0, (win ufx 4-3-3 short 1 min)
2007-11-02 v1.52AB  futility endgame fut_depth=0 (from 2), dropdown_val 40/40, full attack threat, board.p_endgame==0, (win ufx 4-3-3 short 1 min)
2007-11-02 v1.52AD  futility endgame depth-- instead of pruning (win ufx 4-3-3 long 10 min)
2007-11-02 v1.53    atthreat win ufx 3-5-2
2007-11-10 v1.53E   use quiescCheck for all. QHash if qdepth<=check_depth (draw ufx 5-0-5, win 5-3-2)
2007-11-20 v1.54B5/6  use eleeye 300 hashstruct and pointtable (win ufx 8-1-1)
2007-11-21 v1.55    use left-right attack-defend eval
2008-07-12 v1.187   DsqSq-- to use 0 as terminator to shorten rookmove to unsigned char
2008-07-31 v1.840   return +8 for incheck by rook-cann double-check so that chkevas excludes capture
2008-08-22 v1.848   in gencap/qs gen cannon, rook first   win 3dc(110) kin(110) ele(110) lil(200) ufx(200) 730
2008-08-22 v1.848a  separate gencapqs for m_side==0 / 1   win 3dc(110) kin(110) ele(110) lil(200) ufx(200) 730
2008-08-24 v1.848c  combine gencapqs,                     win 3dc(101) kin(110) ele(200) lil(101) ufx(200) 712
2008-08-25 v1.848e  shiftfile unsigned short kingevas[4]  win 3dc(110) kin(110) ele(101) lil(101) ufx(200) 622
2008-09-14 v1.850j  supercap val 12 advele val 48         win 3dc(202) kin(121) ele(220)
2008-11-10 v1.854n  use qphase and phase
2008-11-30 v1.855x  noevalBE,attkpval=2                   los 121 202 112 022 103                          5510
2008-12-05 v1.856f  relhis - val+=dxd, dxd/tot++, hit++   win 112	220	202	121	202                          857
2008-12-05 v1.856g  relhis - no Histval, use Hit++/Tot++  los 3dc(031) kin(022) ele(202) bhw(121) ufx(211) 587
2008-12-05 v1.856p  relhis - histcut>>4->>7, end>>5->>7   win 3dc(301) kin(301) ele(121) bhw(121) ufx(301) 1145
2009-01-07 v1.857f  rookdraw                              los 3dc(004) cyc(022) ele(022)
2010-05-15 v1.882x  fix srchbook and bookmain
2010-05-16 v1.882y  fix endgame knight vs pawn bug
2010-05-22 v1.883b  fix missing refuted move bug,add pinned ADVISOR bonus
2010-08-02 v1.885b  fix chase unprot bug - but need check chase one step before
*/
#define MAXQDEPTH 54

//extern int MAXQDEPTH;  //=54;
//extern int panictime; //clop 52
//extern int PawnValueEg; 
//int PVcmhit, NPVcmhit;  //1016 debug
//int PVhashhit, NPVhashhit, PVmatekillerhit, NPVmatekillerhit, PVkillerhit, NPVkillerhit;
//int QShashmiss, QShashhit, 
//int hashhit;
//int lmrpvhit1, lmrpvhit2, lmrnpvhit1, lmrnpvhit2;
//1102 int rhit1, rhit2, rhit3;
extern int mf;  
extern int mf2;
//extern int lmrparm1;   //clop VR befclop 4530  aft 4570
//extern int lmrparm1;   //clop singular_margin befclop 140  aft 151
//extern int lmrparm1; //=5 (rootlmr) clop
//extern int lmrparm1,lmrparm2; //=197,111; 
//extern int lmrparm3; //=21;

//int QSPVhashrecmiss;
//int QShashrecmiss;
extern int POPCNT_CPU;
extern int NCORE;

#include <future>	
//#include <thread>


//#include <csetjmp>
#include ".\engine.h"
//#include "EFen.h"
//#include "srchbook.h"
#include "srchboob.h"
#include "ucci.h"


//#include "locale.h"
//static const char PieceChar[34] = {'.', '.', 'p', 'P', 'p', 'P', 'p', 'P', 'p', 'P', 'p', 'P', 'a', 'A', 'a', 'A', 'b', 'B', 'b', 'B',
//                                   'h', 'H', 'h', 'H', 'c', 'C', 'c', 'C', 'r', 'R', 'r', 'R', 'k', 'K'
//                                  };
#ifdef PRTBOARD
static const wchar_t PieceChar[34] =
{L'。', L'。', L'卒', L'兵', L'卒', L'兵', L'卒', L'兵', L'卒', L'兵', L'卒', L'兵', L'士', L'仕', L'士', L'仕',
 L'象', L'相', L'象', L'相', L'馬', L'傌', L'馬', L'傌', L'包', L'炮', L'包', L'炮', L'車', L'俥', L'車', L'俥', L'將', L'帥'
};
int prtboard=0;
int before_score=0;
char charmove[5];
void Engine::print_board(int score) //for debug
{
//setlocale( LC_ALL, "chs" );
	if (prtboard<50)
	{
	  printf("\n");
    for (int i=0; i<10; i++)
    {	if (i==5) printf(" ----------------------------------\n");
    	for (int j=0; j<9; j++)
    	{
    	wprintf(L" %c ", PieceChar[board.piece[(i*16)+j]] );
    	}
    	printf("\n");
    	fflush(stdout);
    }
    prtboard++;
    printf("score=%d, ", score);
		before_score = score;
	}
}
#endif

#define NULL_NO  0
#define NULL_YES 1
//1105 #define THREAT_MARGIN 960 //1101 96 //80 //96 //128 //192(86j) //96
//int movenum=0;
//int board.ply = 0;

//int RCH_count = 0;
//int pointsum = 0;
//1105 int evalthreat[2] = {0, 0};
//unsigned int bitpiece = 0; //RrRrCcCc HhHhEeEe BbBbPpPp PpPpPp00
// piececnt[piece&61]
//int piececnt[34] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//bitpieceStruct bitp;

//unsigned int bitattk[10] = {0,0,0,0,0,0,0,0,0,0}; //[left-right][poscolor]  [4]=black central, [5]=red central
unsigned char ATTKAREA[BOARD_SIZE-7] = {
/*
6,6,0,0,4,2,2,8,8,  0,0,0,0,0,0,0,
6,6,0,0,4,2,2,8,8,  0,0,0,0,0,0,0,
6,6,0,0,4,2,2,8,8,  0,0,0,0,0,0,0,
6,6,6,6,4,8,8,8,8,  0,0,0,0,0,0,0,
6,6,6,6,4,8,8,8,8,  0,0,0,0,0,0,0,

7,7,7,7,5,9,9,9,9,  0,0,0,0,0,0,0,
7,7,7,7,5,9,9,9,9,  0,0,0,0,0,0,0,
7,7,1,1,5,3,3,9,9,  0,0,0,0,0,0,0,
7,7,1,1,5,3,3,9,9,  0,0,0,0,0,0,0,
7,7,1,1,5,3,3,9,9};
*/
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,  //1102 bef
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
6,6,0,0,4,2,2,8,8,  0,0,0,0,0,0,0,
6,6,6,6,4,8,8,8,8,  0,0,0,0,0,0,0,

7,7,7,7,5,9,9,9,9,  0,0,0,0,0,0,0,
7,7,1,1,5,3,3,9,9,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3};
/*
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
0,0,0,0,4,2,2,2,2,  0,0,0,0,0,0,0,
6,6,6,6,4,8,8,8,8,  0,0,0,0,0,0,0,

7,7,7,7,5,9,9,9,9,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3,  0,0,0,0,0,0,0,
1,1,1,1,5,3,3,3,3};
*/
//int board.p_endgame_7 = 7;


#ifdef QCHECK
//int QNodes = 0;
#endif
//#define MAX_PLY 144  //1212 defined in board.h
#define MAXEXT 64
#define MAXDEPTH 64
#define DRAWVALUE 1 //16 //30  //32; // 20;  // 藐视因子，即和棋时返回的分数(取负值)
//int DRAWVALUE = 16; //endgame value 1
int TEMPO_BONUS = 5;  //endgame = 0;

//#define NUM_KILLERS 2 //2  //
MoveStruct g_killer[2][MAX_PLY];  //1210
//MoveStruct g_matekiller[256];
/// CounterMoveHistory stores counter moves indexed by [piece][to] of the previous
/// move, see chessprogramming.wikispaces.com/Countermove+Heuristic
MoveStruct counterMoves[7][10][BOARD_SIZE];  //similar to m_his_table  //1017
//[10]=0...9 PIECE_IDX(board.piece[sq]) p,P,b/e,B/E,h,H,c,C,r,R

uint64 h_value[10][9][10];   //[piecetype][nFile}[nRank]
uint64 h_rside;

// misc
//static const int NodeAll = -1;
//static const int NodePV  =  0;
//static const int NodeCut = +1;
#define NodeAll 1
#define NodePV 0
#define NodeCut 1
// macros
//#define NODE_OPP(type)     (-(type))
#define NODE_OPP(type) (type)
//for std::max min
#include <algorithm>   
//#define MAX(x,y) (std::max(x,y))
//#define MIN(x,y) (std::min(x,y))
//#define MAX(x,y) (((x)>(y))?(x):(y))
//#define MIN(x,y) (((x)<(y))?(x):(y))

//sf10 setting 

//DEPTH_QS_CHECKS     =  0 * ONE_PLY,
//DEPTH_QS_NO_CHECKS  = -1 * ONE_PLY,
//DEPTH_QS_RECAPTURES = -5 * ONE_PLY,
#define DEPTH_QS_CHECKS 0
#define DEPTH_QS_NO_CHECKS -1

//1007 int root_qcheck_depth = 0; //0; //-2;	//end_game = -4
//#define root_qcheck_depth 0 //-4
//int check_depth = 0; //-1(cause loop!!!); //0;  //1007 qcheck_depth
#define check_depth 0



//{-19, -17, -11, -7, 7, 11, 17, 19};
//static const int knightchecknew[8] = {9,9,-1,1,-1,1,-9,-9};
//       6   7
//     4       5
//         +
//     2       3
//       0   1
/*
static unsigned char knightmoves[BOARD_SIZE-7][8] =
   {{255,33,255, 18,255,255,255, 255},{ 32, 34,255, 19,255,255,255,255},{ 33, 35, 16, 20,255,255,255,255},{ 34, 36, 17, 21,255,255,255,255},{ 35, 37, 18, 22,255,255,255,255},{ 36, 38, 19, 23,255,255,255,255},{ 37, 39, 20, 24,255,255,255,255},{ 38, 40, 21,255,255,255,255,255},{ 39,255, 22,255,255,255,255,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255, 49,255, 34,255,  2,255,255},{ 48, 50,255, 35,255,  3,255,255},{ 49, 51, 32, 36,0,  4,255,255},{ 50, 52, 33, 37,  1,  5,255,255},{ 51, 53, 34, 38,  2,  6,255,255},{ 52, 54, 35, 39,  3,  7,255,255},{ 53, 55, 36, 40,  4,  8,255,255},{ 54, 56, 37,255,  5,255,255,255},{ 55,255, 38,255,  6,255,255,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255, 65,255, 50,255, 18,255,  1},{ 64, 66,255, 51,255, 19,0,  2},{ 65, 67, 48, 52, 16, 20,  1,  3},{ 66, 68, 49, 53, 17, 21,  2,  4},{ 67, 69, 50, 54, 18, 22,  3,  5},{ 68, 70, 51, 55, 19, 23,  4,  6},{ 69, 71, 52, 56, 20, 24,  5,  7},{ 70, 72, 53,255, 21,255,  6,  8},{ 71,255, 54,255, 22,255,  7,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255, 81,255, 66,255, 34,255, 17},{ 80, 82,255, 67,255, 35, 16, 18},{ 81, 83, 64, 68, 32, 36, 17, 19},{ 82, 84, 65, 69, 33, 37, 18, 20},{ 83, 85, 66, 70, 34, 38, 19, 21},{ 84, 86, 67, 71, 35, 39, 20, 22},{ 85, 87, 68, 72, 36, 40, 21, 23},{ 86, 88, 69,255, 37,255, 22, 24},{ 87,255, 70,255, 38,255, 23,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255, 97,255, 82,255, 50,255, 33},{ 96, 98,255, 83,255, 51, 32, 34},{ 97, 99, 80, 84, 48, 52, 33, 35},{ 98,100, 81, 85, 49, 53, 34, 36},{ 99,101, 82, 86, 50, 54, 35, 37},{100,102, 83, 87, 51, 55, 36, 38},{101,103, 84, 88, 52, 56, 37, 39},{102,104, 85,255, 53,255, 38, 40},{103,255, 86,255, 54,255, 39,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255,113,255, 98,255, 66,255, 49},{112,114,255, 99,255, 67, 48, 50},{113,115, 96,100, 64, 68, 49, 51},{114,116, 97,101, 65, 69, 50, 52},{115,117, 98,102, 66, 70, 51, 53},{116,118, 99,103, 67, 71, 52, 54},{117,119,100,104, 68, 72, 53, 55},{118,120,101,255, 69,255, 54, 56},{119,255,102,255, 70,255, 55,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255,129,255,114,255, 82,255, 65},{128,130,255,115,255, 83, 64, 66},{129,131,112,116, 80, 84, 65, 67},{130,132,113,117, 81, 85, 66, 68},{131,133,114,118, 82, 86, 67, 69},{132,134,115,119, 83, 87, 68, 70},{133,135,116,120, 84, 88, 69, 71},{134,136,117,255, 85,255, 70, 72},{135,255,118,255, 86,255, 71,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255,145,255,130,255, 98,255, 81},{144,146,255,131,255, 99, 80, 82},{145,147,128,132, 96,100, 81, 83},{146,148,129,133, 97,101, 82, 84},{147,149,130,134, 98,102, 83, 85},{148,150,131,135, 99,103, 84, 86},{149,151,132,136,100,104, 85, 87},{150,152,133,255,101,255, 86, 88},{151,255,134,255,102,255, 87,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255,255,255,146,255,114,255, 97},{255,255,255,147,255,115, 96, 98},{255,255,144,148,112,116, 97, 99},{255,255,145,149,113,117, 98,100},{255,255,146,150,114,118, 99,101},{255,255,147,151,115,119,100,102},{255,255,148,152,116,120,101,103},{255,255,149,255,117,255,102,104},{255,255,150,255,118,255,103,255},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {255,255,255,255,255,130,255,113},{255,255,255,255,255,131,112,114},{255,255,255,255,128,132,113,115},{255,255,255,255,129,133,114,116},{255,255,255,255,130,134,115,117},{255,255,255,255,131,135,116,118},{255,255,255,255,132,136,117,119},{255,255,255,255,133,255,118,120},{255,255,255,255,134,255,119,255}
}; //,{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
*/


// 0 1
// 2 3
/*
unsigned char g_advelemoves[BOARD_SIZE-7][4] =
 {{0,0,0,0},{0,0,0,0},{32,36,0,0}, { 0,0,0,20},{0,0,0,0},{ 0,0,0,20},      { 36, 40,0,0},{0,0,0,0},{0,0,0,0},   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0}, {0,0,0,0},{3,  5, 35, 37},{0,0,0,0},    {0,0,0,0},{0,0,0,0},{0,0,0,0},       {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
   {2,66,0,0},{0,0,0,0},{0,0,0,0}, { 0,0,0,20},{2,  6, 66, 70},{ 0,0,0,20},{0,0,0,0},{0,0,0,0},{  6, 70,0,0},   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0}, {0,0,0,0},{0,0,0,0},{0,0,0,0},          {0,0,0,0},{0,0,0,0},{0,0,0,0},       {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{32,36,0,0}, {0,0,0,0},{0,0,0,0},{0,0,0,0},          { 36, 40,0,0},{0,0,0,0},{0,0,0,0},   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{112,116,0,0}, {0,0,0,0},{0,0,0,0},{0,0,0,0},          {116,120,0,0},{0,0,0,0},{0,0,0,0},   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0}, {0,0,0,0},{0,0,0,0},{0,0,0,0},          {0,0,0,0},{0,0,0,0},{0,0,0,0},       {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
 {82,146,0,0},{0,0,0,0},{0,0,0,0}, {0,0,0,132},{82,86,146,150},{0,0,0,132},{0,0,0,0},{0,0,0,0},{ 86,150,0,0},   {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0}, {0,0,0,0},{115,117,147,149},{0,0,0,0},  {0,0,0,0},{0,0,0,0},{0,0,0,0},       {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{112,116,0,0}, {0,0,0,132},{0,0,0,0},{0,0,0,132},      {116,120,0,0},{0,0,0,0},{0,0,0,0}
}; //,{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
*/
unsigned char g_KnightMoves[BOARD_SIZE-7][16];
/*
= {
{ 34, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 33, 35, 20,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 34, 36, 17, 21,  0,  0,  0,  0,  0, 32, 36,  0,  0,  0,  0,  0},{ 35, 37, 18, 22,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0},{ 36, 38, 19, 23,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 37, 39, 20, 24,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0},{ 38, 40, 21, 25,  0,  0,  0,  0,  0, 36, 40,  0,  0,  0,  0,  0},{ 39, 41, 22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 40, 23,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{ 50, 35,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 49, 51, 36,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 50, 52, 33, 37,  1,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 51, 53, 34, 38,  2,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 52, 54, 35, 39,  3,  7,  0,  0,  0,  3,  5, 35, 37,  0,  0,  0},{ 53, 55, 36, 40,  4,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 54, 56, 37, 41,  5,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 55, 57, 38,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 56, 39,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{ 66, 51, 19,  2,  0,  0,  0,  0,  0,  2, 66,  0,  0,  0,  0,  0},{ 65, 67, 52, 20,  1,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 66, 68, 49, 53, 17, 21,  2,  4,  0,  0,  0,  0,  0,  0,  0,  0},{ 67, 69, 50, 54, 18, 22,  3,  5,  0, 20,  0,  0,  0,  0,  0,  0},{ 68, 70, 51, 55, 19, 23,  4,  6,  0,  2,  6, 66, 70,  0,  0,  0},{ 69, 71, 52, 56, 20, 24,  5,  7,  0, 20,  0,  0,  0,  0,  0,  0},{ 70, 72, 53, 57, 21, 25,  6,  8,  0,  0,  0,  0,  0,  0,  0,  0},{ 71, 73, 54, 22,  7,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 72, 55, 23,  8,  0,  0,  0,  0,  0,  6, 70,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{ 82, 67, 35, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 81, 83, 68, 36, 17, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 82, 84, 65, 69, 33, 37, 18, 20,  0,  0,  0,  0,  0,  0,  0,  0},{ 83, 85, 66, 70, 34, 38, 19, 21,  0,  0,  0,  0,  0,  0,  0,  0},{ 84, 86, 67, 71, 35, 39, 20, 22,  0,  0,  0,  0,  0,  0,  0,  0},{ 85, 87, 68, 72, 36, 40, 21, 23,  0,  0,  0,  0,  0,  0,  0,  0},{ 86, 88, 69, 73, 37, 41, 22, 24,  0,  0,  0,  0,  0,  0,  0,  0},{ 87, 89, 70, 38, 23, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 88, 71, 39, 24,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{ 98, 83, 51, 34,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 97, 99, 84, 52, 33, 35,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{ 98,100, 81, 85, 49, 53, 34, 36,  0, 32, 36,  0,  0,  0,  0,  0},{ 99,101, 82, 86, 50, 54, 35, 37,  0,  0,  0,  0,  0,  0,  0,  0},{100,102, 83, 87, 51, 55, 36, 38,  0,  0,  0,  0,  0,  0,  0,  0},{101,103, 84, 88, 52, 56, 37, 39,  0,  0,  0,  0,  0,  0,  0,  0},{102,104, 85, 89, 53, 57, 38, 40,  0, 36, 40,  0,  0,  0,  0,  0},{103,105, 86, 54, 39, 41,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{104, 87, 55, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{114, 99, 67, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{113,115,100, 68, 49, 51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{114,116, 97,101, 65, 69, 50, 52,  0,112,116,  0,  0,  0,  0,  0},{115,117, 98,102, 66, 70, 51, 53,  0,  0,  0,  0,  0,  0,  0,  0},{116,118, 99,103, 67, 71, 52, 54,  0,  0,  0,  0,  0,  0,  0,  0},{117,119,100,104, 68, 72, 53, 55,  0,  0,  0,  0,  0,  0,  0,  0},{118,120,101,105, 69, 73, 54, 56,  0, 116,120, 0,  0,  0,  0,  0},{119,121,102, 70, 55, 57,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{120,103, 71, 56,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{130,115, 83, 66,  0,  0,  0,  0,  0, 82,146,  0,  0,  0,  0,  0},{129,131,116, 84, 65, 67,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{130,132,113,117, 81, 85, 66, 68,  0,  0,  0,  0,  0,  0,  0,  0},{131,133,114,118, 82, 86, 67, 69,  0,  0,  0,  0,  0,  0,  0,  0},{132,134,115,119, 83, 87, 68, 70,  0,  0,  0,  0,  0,  0,  0,  0},{133,135,116,120, 84, 88, 69, 71,  0,  0,  0,  0,  0,  0,  0,  0},{134,136,117,121, 85, 89, 70, 72,  0,  0,  0,  0,  0,  0,  0,  0},{135,137,118, 86, 71, 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{136,119, 87, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{146,131, 99, 82,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{145,147,132,100, 81, 83,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{146,148,129,133, 97,101, 82, 84,  0,  0,  0,  0,  0,  0,  0,  0},{147,149,130,134, 98,102, 83, 85,  0,132,  0,  0,  0,  0,  0,  0},{148,150,131,135, 99,103, 84, 86,  0,  82,86,146,150,  0,  0,  0},{149,151,132,136,100,104, 85, 87,  0,132,  0,  0,  0,  0,  0,  0},{150,152,133,137,101,105, 86, 88,  0,  0,  0,  0,  0,  0,  0,  0},{151,153,134,102, 87, 89,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{152,135,103, 88,  0,  0,  0,  0,  0, 86,150,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{147,115, 98,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{148,116, 97, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{145,149,113,117, 98,100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{146,150,114,118, 99,101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{147,151,115,119,100,102,  0,  0,  0,  115,117,147,149,0,  0,  0},{148,152,116,120,101,103,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{149,153,117,121,102,104,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{150,118,103,105,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{151,119,104,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
{131,114,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{132,113,115,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{129,133,114,116,  0,  0,  0,  0,  0,112,116,  0,  0,  0,  0,  0},{130,134,115,117,  0,  0,  0,  0,  0,132,  0,  0,  0,  0,  0,  0},{131,135,116,118,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{132,136,117,119,  0,  0,  0,  0,  0,132,  0,  0,  0,  0,  0,  0},{133,137,118,120,  0,  0,  0,  0,  0, 116,120, 0,  0,  0,  0,  0},{134,119,121,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},{135,120,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}};
*/
static const unsigned char ele_pos[12]={2,6, 32,40, 66,70, 82,86, 112,120, 146,150};
static const unsigned char ele_moves[12][2]={{32, 36},{36, 40},{2, 66},{6, 70},{32, 36},{36, 40},
{112,116},{116,120},{82,146},{86,150},{112,116},{116,120}};
static const unsigned char advele_center_pos[4]={20,36,116,132};
static const unsigned char advele_center_moves[4][4]={{3,  5, 35, 37},{2,  6, 66, 70,},{82,86,146,150},{115,117,147,149}};
static const unsigned char bis_pos[8]={3,5,35,37, 115,117,147,149};
static const unsigned char bis_moves[8]={20,20,20,20, 132,132,132,132};

//unsigned char g_ElephantEyes[BOARD_SIZE][4];
unsigned char g_HorseLegs[BOARD_SIZE-7][8];
/*
 = {
{ 16,  1,  0,  0,  0,  0,  0,  0},{ 17, 17,  2,  0,  0,  0,  0,  0},{ 18, 18,  1,  3,  0,  0,  0,  0},{ 19, 19,  2,  4,  0,  0,  0,  0},{ 20, 20,  3,  5,  0,  0,  0,  0},{ 21, 21,  4,  6,  0,  0,  0,  0},{ 22, 22,  5,  7,  0,  0,  0,  0},{ 23, 23,  6,  0,  0,  0,  0,  0},{ 24,  7,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{ 32, 17, 17,  0,  0,  0,  0,  0},{ 33, 33, 18, 18,  0,  0,  0,  0},{ 34, 34, 17, 19, 17, 19,  0,  0},{ 35, 35, 18, 20, 18, 20,  0,  0},{ 36, 36, 19, 21, 19, 21,  0,  0},{ 37, 37, 20, 22, 20, 22,  0,  0},{ 38, 38, 21, 23, 21, 23,  0,  0},{ 39, 39, 22, 22,  0,  0,  0,  0},{ 40, 23, 23,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{ 48, 33, 33, 16,  0,  0,  0,  0},{ 49, 49, 34, 34, 17, 17,  0,  0},{ 50, 50, 33, 35, 33, 35, 18, 18},{ 51, 51, 34, 36, 34, 36, 19, 19},{ 52, 52, 35, 37, 35, 37, 20, 20},{ 53, 53, 36, 38, 36, 38, 21, 21},{ 54, 54, 37, 39, 37, 39, 22, 22},{ 55, 55, 38, 38, 23, 23,  0,  0},{ 56, 39, 39, 24,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{ 64, 49, 49, 32,  0,  0,  0,  0},{ 65, 65, 50, 50, 33, 33,  0,  0},{ 66, 66, 49, 51, 49, 51, 34, 34},{ 67, 67, 50, 52, 50, 52, 35, 35},{ 68, 68, 51, 53, 51, 53, 36, 36},{ 69, 69, 52, 54, 52, 54, 37, 37},{ 70, 70, 53, 55, 53, 55, 38, 38},{ 71, 71, 54, 54, 39, 39,  0,  0},{ 72, 55, 55, 40,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{ 80, 65, 65, 48,  0,  0,  0,  0},{ 81, 81, 66, 66, 49, 49,  0,  0},{ 82, 82, 65, 67, 65, 67, 50, 50},{ 83, 83, 66, 68, 66, 68, 51, 51},{ 84, 84, 67, 69, 67, 69, 52, 52},{ 85, 85, 68, 70, 68, 70, 53, 53},{ 86, 86, 69, 71, 69, 71, 54, 54},{ 87, 87, 70, 70, 55, 55,  0,  0},{ 88, 71, 71, 56,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{ 96, 81, 81, 64,  0,  0,  0,  0},{ 97, 97, 82, 82, 65, 65,  0,  0},{ 98, 98, 81, 83, 81, 83, 66, 66},{ 99, 99, 82, 84, 82, 84, 67, 67},{100,100, 83, 85, 83, 85, 68, 68},{101,101, 84, 86, 84, 86, 69, 69},{102,102, 85, 87, 85, 87, 70, 70},{103,103, 86, 86, 71, 71,  0,  0},{104, 87, 87, 72,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{112, 97, 97, 80,  0,  0,  0,  0},{113,113, 98, 98, 81, 81,  0,  0},{114,114, 97, 99, 97, 99, 82, 82},{115,115, 98,100, 98,100, 83, 83},{116,116, 99,101, 99,101, 84, 84},{117,117,100,102,100,102, 85, 85},{118,118,101,103,101,103, 86, 86},{119,119,102,102, 87, 87,  0,  0},{120,103,103, 88,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{128,113,113, 96,  0,  0,  0,  0},{129,129,114,114, 97, 97,  0,  0},{130,130,113,115,113,115, 98, 98},{131,131,114,116,114,116, 99, 99},{132,132,115,117,115,117,100,100},{133,133,116,118,116,118,101,101},{134,134,117,119,117,119,102,102},{135,135,118,118,103,103,  0,  0},{136,119,119,104,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{129,129,112,  0,  0,  0,  0,  0},{130,130,113,113,  0,  0,  0,  0},{129,131,129,131,114,114,  0,  0},{130,132,130,132,115,115,  0,  0},{131,133,131,133,116,116,  0,  0},{132,134,132,134,117,117,  0,  0},{133,135,133,135,118,118,  0,  0},{134,134,119,119,  0,  0,  0,  0},{135,135,120,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},{  0,  0,  0,  0,  0,  0,  0,  0},
{145,128,  0,  0,  0,  0,  0,  0},{146,129,129,  0,  0,  0,  0,  0},{145,147,130,130,  0,  0,  0,  0},{146,148,131,131,  0,  0,  0,  0},{147,149,132,132,  0,  0,  0,  0},{148,150,133,133,  0,  0,  0,  0},{149,151,134,134,  0,  0,  0,  0},{150,135,135,  0,  0,  0,  0,  0},{151,136,  0,  0,  0,  0,  0,  0}};
*/
#ifdef HORSECK
unsigned char g_KnightChecks[BOARD_SIZE-7][18][2];
#endif
/*
unsigned char g_RookMoves[BOARD_SIZE-7][4][16] =
{{{9, 18,27,36,45,54,63,72,81,255,0,0,0,0,0,0}, {1, 2, 3, 4, 5, 6, 7, 8,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}   },
    {{10,19,28,37,46,55,64,73,82,255,0,0,0,0,0,0}, {0,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}, {2, 3, 4, 5, 6, 7, 8,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{11,20,29,38,47,56,65,74,83,255,0,0,0,0,0,0}, {1, 0,255,255,255,255,255,255,255,255,0,0,0,0,0,0}, {3, 4, 5, 6, 7, 8,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{12,21,30,39,48,57,66,75,84,255,0,0,0,0,0,0}, {2, 1, 0,255,255,255,255,255,255,255,0,0,0,0,0,0}, {4, 5, 6, 7, 8,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{13,22,31,40,49,58,67,76,85,255,0,0,0,0,0,0}, {3, 2, 1, 0,255,255,255,255,255,255,0,0,0,0,0,0}, {5, 6, 7, 8,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{14,23,32,41,50,59,68,77,86,255,0,0,0,0,0,0}, {4, 3, 2, 1, 0,255,255,255,255,255,0,0,0,0,0,0}, {6, 7, 8,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{15,24,33,42,51,60,69,78,87,255,0,0,0,0,0,0}, {5, 4, 3, 2, 1, 0,255,255,255,255,0,0,0,0,0,0}, {7, 8,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{16,25,34,43,52,61,70,79,88,255,0,0,0,0,0,0}, {6, 5, 4, 3, 2, 1, 0,255,255,255,0,0,0,0,0,0}, {8,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{17,26,35,44,53,62,71,80,89,255,0,0,0,0,0,0}, {7, 6, 5, 4, 3, 2, 1, 0,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},



    {{  0,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{18,27,36,45,54,63,72,81,255,255,0,0,0,0,0,0},{10,11,12,13,14,15,16,17,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 1,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{19,28,37,46,55,64,73,82,255,255,0,0,0,0,0,0}, {9,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{11,12,13,14,15,16,17,255,255,255,0,0,0,0,0,0}},
    {{ 2,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{20,29,38,47,56,65,74,83,255,255,0,0,0,0,0,0}, {10,9,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{12,13,14,15,16,17,255,255,255,255,0,0,0,0,0,0}},
    {{ 3,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{21,30,39,48,57,66,75,84,255,255,0,0,0,0,0,0}, {11,10,9,255,255,255,255,255,255,255,0,0,0,0,0,0},{13,14,15,16,17,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 4,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{22,31,40,49,58,67,76,85,255,255,0,0,0,0,0,0}, {12,11,10,9,255,255,255,255,255,255,0,0,0,0,0,0},{14,15,16,17,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 5,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{23,32,41,50,59,68,77,86,255,255,0,0,0,0,0,0}, {13,12,11,10,9,255,255,255,255,255,0,0,0,0,0,0},{15,16,17,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 6,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{24,33,42,51,60,69,78,87,255,255,0,0,0,0,0,0}, {14,13,12,11,10,9,255,255,255,255,0,0,0,0,0,0},{16,17,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 7,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{25,34,43,52,61,70,79,88,255,255,0,0,0,0,0,0}, {15,14,13,12,11,10,9,255,255,255,0,0,0,0,0,0},{17,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{ 8,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{26,35,44,53,62,71,80,89,255,255,0,0,0,0,0,0}, {16,15,14,13,12,11,10,9,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{  9, 0,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{27,36,45,54,63,72,81,255,255,255,0,0,0,0,0,0},{19,20,21,22,23,24,25,26,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{10, 1,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{28,37,46,55,64,73,82,255,255,255,0,0,0,0,0,0},{18,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{20,21,22,23,24,25,26,255,255,255,0,0,0,0,0,0}},
    {{11, 2,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{29,38,47,56,65,74,83,255,255,255,0,0,0,0,0,0},{19,18,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{21,22,23,24,25,26,255,255,255,255,0,0,0,0,0,0}},
    {{12, 3,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{30,39,48,57,66,75,84,255,255,255,0,0,0,0,0,0},{20,19,18,255,255,255,255,255,255,255,0,0,0,0,0,0},{22,23,24,25,26,255,255,255,255,255,0,0,0,0,0,0}},
    {{13, 4,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{31,40,49,58,67,76,85,255,255,255,0,0,0,0,0,0},{21,20,19,18,255,255,255,255,255,255,0,0,0,0,0,0},{23,24,25,26,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{14, 5,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{32,41,50,59,68,77,86,255,255,255,0,0,0,0,0,0},{22,21,20,19,18,255,255,255,255,255,0,0,0,0,0,0},{24,25,26,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{15, 6,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{33,42,51,60,69,78,87,255,255,255,0,0,0,0,0,0},{23,22,21,20,19,18,255,255,255,255,0,0,0,0,0,0},{25,26,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{16, 7,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{34,43,52,61,70,79,88,255,255,255,0,0,0,0,0,0},{24,23,22,21,20,19,18,255,255,255,0,0,0,0,0,0},{26,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{17, 8,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{35,44,53,62,71,80,89,255,255,255,0,0,0,0,0,0},{25,24,23,22,21,20,19,18,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 18, 9, 0,255,255,255,255,255,255,255,0,0,0,0,0,0},{36,45,54,63,72,81,255,255,255,255,0,0,0,0,0,0},{28,29,30,31,32,33,34,35,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{19,10, 1,255,255,255,255,255,255,255,0,0,0,0,0,0},{37,46,55,64,73,82,255,255,255,255,0,0,0,0,0,0},{27,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{29,30,31,32,33,34,35,255,255,255,0,0,0,0,0,0}},
    {{20,11, 2,255,255,255,255,255,255,255,0,0,0,0,0,0},{38,47,56,65,74,83,255,255,255,255,0,0,0,0,0,0},{28,27,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{30,31,32,33,34,35,255,255,255,255,0,0,0,0,0,0}},
    {{21,12, 3,255,255,255,255,255,255,255,0,0,0,0,0,0},{39,48,57,66,75,84,255,255,255,255,0,0,0,0,0,0},{29,28,27,255,255,255,255,255,255,255,0,0,0,0,0,0},{31,32,33,34,35,255,255,255,255,255,0,0,0,0,0,0}},
    {{22,13, 4,255,255,255,255,255,255,255,0,0,0,0,0,0},{40,49,58,67,76,85,255,255,255,255,0,0,0,0,0,0},{30,29,28,27,255,255,255,255,255,255,0,0,0,0,0,0},{32,33,34,35,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{23,14, 5,255,255,255,255,255,255,255,0,0,0,0,0,0},{41,50,59,68,77,86,255,255,255,255,0,0,0,0,0,0},{31,30,29,28,27,255,255,255,255,255,0,0,0,0,0,0},{33,34,35,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{24,15, 6,255,255,255,255,255,255,255,0,0,0,0,0,0},{42,51,60,69,78,87,255,255,255,255,0,0,0,0,0,0},{32,31,30,29,28,27,255,255,255,255,0,0,0,0,0,0},{34,35,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{25,16, 7,255,255,255,255,255,255,255,0,0,0,0,0,0},{43,52,61,70,79,88,255,255,255,255,0,0,0,0,0,0},{33,32,31,30,29,28,27,255,255,255,0,0,0,0,0,0},{35,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{26,17, 8,255,255,255,255,255,255,255,0,0,0,0,0,0},{44,53,62,71,80,89,255,255,255,255,0,0,0,0,0,0},{34,33,32,31,30,29,28,27,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 27,18, 9, 0,255,255,255,255,255,255,0,0,0,0,0,0},{45,54,63,72,81,255,255,255,255,255,0,0,0,0,0,0},{37,38,39,40,41,42,43,44,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{28,19,10, 1,255,255,255,255,255,255,0,0,0,0,0,0},{46,55,64,73,82,255,255,255,255,255,0,0,0,0,0,0},{36,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{38,39,40,41,42,43,44,255,255,255,0,0,0,0,0,0}},
    {{29,20,11, 2,255,255,255,255,255,255,0,0,0,0,0,0},{47,56,65,74,83,255,255,255,255,255,0,0,0,0,0,0},{37,36,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{39,40,41,42,43,44,255,255,255,255,0,0,0,0,0,0}},
    {{30,21,12, 3,255,255,255,255,255,255,0,0,0,0,0,0},{48,57,66,75,84,255,255,255,255,255,0,0,0,0,0,0},{38,37,36,255,255,255,255,255,255,255,0,0,0,0,0,0},{40,41,42,43,44,255,255,255,255,255,0,0,0,0,0,0}},
    {{31,22,13, 4,255,255,255,255,255,255,0,0,0,0,0,0},{49,58,67,76,85,255,255,255,255,255,0,0,0,0,0,0},{39,38,37,36,255,255,255,255,255,255,0,0,0,0,0,0},{41,42,43,44,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{32,23,14, 5,255,255,255,255,255,255,0,0,0,0,0,0},{50,59,68,77,86,255,255,255,255,255,0,0,0,0,0,0},{40,39,38,37,36,255,255,255,255,255,0,0,0,0,0,0},{42,43,44,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{33,24,15, 6,255,255,255,255,255,255,0,0,0,0,0,0},{51,60,69,78,87,255,255,255,255,255,0,0,0,0,0,0},{41,40,39,38,37,36,255,255,255,255,0,0,0,0,0,0},{43,44,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{34,25,16, 7,255,255,255,255,255,255,0,0,0,0,0,0},{52,61,70,79,88,255,255,255,255,255,0,0,0,0,0,0},{42,41,40,39,38,37,36,255,255,255,0,0,0,0,0,0},{44,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{35,26,17, 8,255,255,255,255,255,255,0,0,0,0,0,0},{53,62,71,80,89,255,255,255,255,255,0,0,0,0,0,0},{43,42,41,40,39,38,37,36,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 36,27,18, 9, 0,255,255,255,255,255,0,0,0,0,0,0},{54,63,72,81,255,255,255,255,255,255,0,0,0,0,0,0},{46,47,48,49,50,51,52,53,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{37,28,19,10, 1,255,255,255,255,255,0,0,0,0,0,0},{55,64,73,82,255,255,255,255,255,255,0,0,0,0,0,0},{45,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{47,48,49,50,51,52,53,255,255,255,0,0,0,0,0,0}},
    {{38,29,20,11, 2,255,255,255,255,255,0,0,0,0,0,0},{56,65,74,83,255,255,255,255,255,255,0,0,0,0,0,0},{46,45,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{48,49,50,51,52,53,255,255,255,255,0,0,0,0,0,0}},
    {{39,30,21,12, 3,255,255,255,255,255,0,0,0,0,0,0},{57,66,75,84,255,255,255,255,255,255,0,0,0,0,0,0},{47,46,45,255,255,255,255,255,255,255,0,0,0,0,0,0},{49,50,51,52,53,255,255,255,255,255,0,0,0,0,0,0}},
    {{40,31,22,13, 4,255,255,255,255,255,0,0,0,0,0,0},{58,67,76,85,255,255,255,255,255,255,0,0,0,0,0,0},{48,47,46,45,255,255,255,255,255,255,0,0,0,0,0,0},{50,51,52,53,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{41,32,23,14, 5,255,255,255,255,255,0,0,0,0,0,0},{59,68,77,86,255,255,255,255,255,255,0,0,0,0,0,0},{49,48,47,46,45,255,255,255,255,255,0,0,0,0,0,0},{51,52,53,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{42,33,24,15, 6,255,255,255,255,255,0,0,0,0,0,0},{60,69,78,87,255,255,255,255,255,255,0,0,0,0,0,0},{50,49,48,47,46,45,255,255,255,255,0,0,0,0,0,0},{52,53,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{43,34,25,16, 7,255,255,255,255,255,0,0,0,0,0,0},{61,70,79,88,255,255,255,255,255,255,0,0,0,0,0,0},{51,50,49,48,47,46,45,255,255,255,0,0,0,0,0,0},{53,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{44,35,26,17, 8,255,255,255,255,255,0,0,0,0,0,0},{62,71,80,89,255,255,255,255,255,255,0,0,0,0,0,0},{52,51,50,49,48,47,46,45,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 45,36,27,18, 9, 0,255,255,255,255,0,0,0,0,0,0},{63,72,81,255,255,255,255,255,255,255,0,0,0,0,0,0},{55,56,57,58,59,60,61,62,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{46,37,28,19,10, 1,255,255,255,255,0,0,0,0,0,0},{64,73,82,255,255,255,255,255,255,255,0,0,0,0,0,0},{54,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{56,57,58,59,60,61,62,255,255,255,0,0,0,0,0,0}},
    {{47,38,29,20,11, 2,255,255,255,255,0,0,0,0,0,0},{65,74,83,255,255,255,255,255,255,255,0,0,0,0,0,0},{55,54,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{57,58,59,60,61,62,255,255,255,255,0,0,0,0,0,0}},
    {{48,39,30,21,12, 3,255,255,255,255,0,0,0,0,0,0},{66,75,84,255,255,255,255,255,255,255,0,0,0,0,0,0},{56,55,54,255,255,255,255,255,255,255,0,0,0,0,0,0},{58,59,60,61,62,255,255,255,255,255,0,0,0,0,0,0}},
    {{49,40,31,22,13, 4,255,255,255,255,0,0,0,0,0,0},{67,76,85,255,255,255,255,255,255,255,0,0,0,0,0,0},{57,56,55,54,255,255,255,255,255,255,0,0,0,0,0,0},{59,60,61,62,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{50,41,32,23,14, 5,255,255,255,255,0,0,0,0,0,0},{68,77,86,255,255,255,255,255,255,255,0,0,0,0,0,0},{58,57,56,55,54,255,255,255,255,255,0,0,0,0,0,0},{60,61,62,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{51,42,33,24,15, 6,255,255,255,255,0,0,0,0,0,0},{69,78,87,255,255,255,255,255,255,255,0,0,0,0,0,0},{59,58,57,56,55,54,255,255,255,255,0,0,0,0,0,0},{61,62,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{52,43,34,25,16, 7,255,255,255,255,0,0,0,0,0,0},{70,79,88,255,255,255,255,255,255,255,0,0,0,0,0,0},{60,59,58,57,56,55,54,255,255,255,0,0,0,0,0,0},{62,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{53,44,35,26,17, 8,255,255,255,255,0,0,0,0,0,0},{71,80,89,255,255,255,255,255,255,255,0,0,0,0,0,0},{61,60,59,58,57,56,55,54,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 54,45,36,27,18, 9, 0,255,255,255,0,0,0,0,0,0},{72,81,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{64,65,66,67,68,69,70,71,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{55,46,37,28,19,10, 1,255,255,255,0,0,0,0,0,0},{73,82,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{63,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{65,66,67,68,69,70,71,255,255,255,0,0,0,0,0,0}},
    {{56,47,38,29,20,11, 2,255,255,255,0,0,0,0,0,0},{74,83,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{64,63,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{66,67,68,69,70,71,255,255,255,255,0,0,0,0,0,0}},
    {{57,48,39,30,21,12, 3,255,255,255,0,0,0,0,0,0},{75,84,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{65,64,63,255,255,255,255,255,255,255,0,0,0,0,0,0},{67,68,69,70,71,255,255,255,255,255,0,0,0,0,0,0}},
    {{58,49,40,31,22,13, 4,255,255,255,0,0,0,0,0,0},{76,85,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{66,65,64,63,255,255,255,255,255,255,0,0,0,0,0,0},{68,69,70,71,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{59,50,41,32,23,14, 5,255,255,255,0,0,0,0,0,0},{77,86,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{67,66,65,64,63,255,255,255,255,255,0,0,0,0,0,0},{69,70,71,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{60,51,42,33,24,15, 6,255,255,255,0,0,0,0,0,0},{78,87,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{68,67,66,65,64,63,255,255,255,255,0,0,0,0,0,0},{70,71,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{61,52,43,34,25,16, 7,255,255,255,0,0,0,0,0,0},{79,88,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{69,68,67,66,65,64,63,255,255,255,0,0,0,0,0,0},{71,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{62,53,44,35,26,17, 8,255,255,255,0,0,0,0,0,0},{80,89,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{70,69,68,67,66,65,64,63,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 63,54,45,36,27,18, 9, 0,255,255,0,0,0,0,0,0},{81,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{73,74,75,76,77,78,79,80,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{64,55,46,37,28,19,10, 1,255,255,0,0,0,0,0,0},{82,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{72,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{74,75,76,77,78,79,80,255,255,255,0,0,0,0,0,0}},
    {{65,56,47,38,29,20,11, 2,255,255,0,0,0,0,0,0},{83,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{73,72,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{75,76,77,78,79,80,255,255,255,255,0,0,0,0,0,0}},
    {{66,57,48,39,30,21,12, 3,255,255,0,0,0,0,0,0},{84,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{74,73,72,255,255,255,255,255,255,255,0,0,0,0,0,0},{76,77,78,79,80,255,255,255,255,255,0,0,0,0,0,0}},
    {{67,58,49,40,31,22,13, 4,255,255,0,0,0,0,0,0},{85,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{75,74,73,72,255,255,255,255,255,255,0,0,0,0,0,0},{77,78,79,80,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{68,59,50,41,32,23,14, 5,255,255,0,0,0,0,0,0},{86,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{76,75,74,73,72,255,255,255,255,255,0,0,0,0,0,0},{78,79,80,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{69,60,51,42,33,24,15, 6,255,255,0,0,0,0,0,0},{87,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{77,76,75,74,73,72,255,255,255,255,0,0,0,0,0,0},{79,80,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{70,61,52,43,34,25,16, 7,255,255,0,0,0,0,0,0},{88,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{78,77,76,75,74,73,72,255,255,255,0,0,0,0,0,0},{80,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{71,62,53,44,35,26,17, 8,255,255,0,0,0,0,0,0},{89,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{79,78,77,76,75,74,73,72,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},


    {{ 72,63,54,45,36,27,18, 9, 0,255,0,0,0,0,0,0},{82,83,84,85,86,87,88,89,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{73,64,55,46,37,28,19,10, 1,255,0,0,0,0,0,0},{81,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{83,84,85,86,87,88,89,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{74,65,56,47,38,29,20,11, 2,255,0,0,0,0,0,0},{82,81,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{84,85,86,87,88,89,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{75,66,57,48,39,30,21,12, 3,255,0,0,0,0,0,0},{83,82,81,255,255,255,255,255,255,255,0,0,0,0,0,0},{85,86,87,88,89,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{76,67,58,49,40,31,22,13, 4,255,0,0,0,0,0,0},{84,83,82,81,255,255,255,255,255,255,0,0,0,0,0,0},{86,87,88,89,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{77,68,59,50,41,32,23,14, 5,255,0,0,0,0,0,0},{85,84,83,82,81,255,255,255,255,255,0,0,0,0,0,0},{87,88,89,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{78,69,60,51,42,33,24,15, 6,255,0,0,0,0,0,0},{86,85,84,83,82,81,255,255,255,255,0,0,0,0,0,0},{88,89,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{79,70,61,52,43,34,25,16, 7,255,0,0,0,0,0,0},{87,86,85,84,83,82,81,255,255,255,0,0,0,0,0,0},{89,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}},
    {{80,71,62,53,44,35,26,17, 8,255,0,0,0,0,0,0},{88,87,86,85,84,83,82,81,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0},{255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0}}
};
*/
/*
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
*/

/*
unsigned char g_PawnMoves[BOARD_SIZE-7][2][4] = {
    {{255,  0,  0,  0},{  1,255,  0,  0}},{{255,  0,  0,  0},{  0,  2,255,  0}},{{255,  0,  0,  0},{  1,  3,255,  0}},{{255,  0,  0,  0},{  2,  4,255,  0}},{{255,  0,  0,  0},{  3,  5,255,  0}},{{255,  0,  0,  0},{  4,  6,255,  0}},{{255,  0,  0,  0},{  5,  7,255,  0}},{{255,  0,  0,  0},{  6,  8,255,  0}},{{255,  0,  0,  0},{  7,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{255,  0,  0,  0},{  0, 17,255,  0}},{{255,  0,  0,  0},{  1, 16, 18,255}},{{255,  0,  0,  0},{  2, 17, 19,255}},{{255,  0,  0,  0},{  3, 18, 20,255}},{{255,  0,  0,  0},{  4, 19, 21,255}},{{255,  0,  0,  0},{  5, 20, 22,255}},{{255,  0,  0,  0},{  6, 21, 23,255}},{{255,  0,  0,  0},{  7, 22, 24,255}},{{255,  0,  0,  0},{  8, 23,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{255,  0,  0,  0},{ 16, 33,255,  0}},{{255,  0,  0,  0},{ 17, 32, 34,255}},{{255,  0,  0,  0},{ 18, 33, 35,255}},{{255,  0,  0,  0},{ 19, 34, 36,255}},{{255,  0,  0,  0},{ 20, 35, 37,255}},{{255,  0,  0,  0},{ 21, 36, 38,255}},{{255,  0,  0,  0},{ 22, 37, 39,255}},{{255,  0,  0,  0},{ 23, 38, 40,255}},{{255,  0,  0,  0},{ 24, 39,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 64,255,  0,  0},{ 32, 49,255,  0}},{{255,  0,  0,  0},{ 33, 48, 50,255}},{{ 66,255,  0,  0},{ 34, 49, 51,255}},{{255,  0,  0,  0},{ 35, 50, 52,255}},{{ 68,255,  0,  0},{ 36, 51, 53,255}},{{255,  0,  0,  0},{ 37, 52, 54,255}},{{ 70,255,  0,  0},{ 38, 53, 55,255}},{{255,  0,  0,  0},{ 39, 54, 56,255}},{{ 72,255,  0,  0},{ 40, 55,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 80,255,  0,  0},{ 48, 65,255,  0}},{{255,  0,  0,  0},{ 49, 64, 66,255}},{{ 82,255,  0,  0},{ 50, 65, 67,255}},{{255,  0,  0,  0},{ 51, 66, 68,255}},{{ 84,255,  0,  0},{ 52, 67, 69,255}},{{255,  0,  0,  0},{ 53, 68, 70,255}},{{ 86,255,  0,  0},{ 54, 69, 71,255}},{{255,  0,  0,  0},{ 55, 70, 72,255}},{{ 88,255,  0,  0},{ 56, 71,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 96, 81,255,  0},{ 64,255,  0,  0}},{{ 97, 80, 82,255},{255,  0,  0,  0}},{{ 98, 81, 83,255},{ 66,255,  0,  0}},{{ 99, 82, 84,255},{255,  0,  0,  0}},{{100, 83, 85,255},{ 68,255,  0,  0}},{{101, 84, 86,255},{255,  0,  0,  0}},{{102, 85, 87,255},{ 70,255,  0,  0}},{{103, 86, 88,255},{255,  0,  0,  0}},{{104, 87,255,  0},{ 72,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{112, 97,255,  0},{ 80,255,  0,  0}},{{113, 96, 98,255},{255,  0,  0,  0}},{{114, 97, 99,255},{ 82,255,  0,  0}},{{115, 98,100,255},{255,  0,  0,  0}},{{116, 99,101,255},{ 84,255,  0,  0}},{{117,100,102,255},{255,  0,  0,  0}},{{118,101,103,255},{ 86,255,  0,  0}},{{119,102,104,255},{255,  0,  0,  0}},{{120,103,255,  0},{ 88,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{128,113,255,  0},{255,  0,  0,  0}},{{129,112,114,255},{255,  0,  0,  0}},{{130,113,115,255},{255,  0,  0,  0}},{{131,114,116,255},{255,  0,  0,  0}},{{132,115,117,255},{255,  0,  0,  0}},{{133,116,118,255},{255,  0,  0,  0}},{{134,117,119,255},{255,  0,  0,  0}},{{135,118,120,255},{255,  0,  0,  0}},{{136,119,255,  0},{255,  0,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{144,129,255,  0},{255,  0,  0,  0}},{{145,128,130,255},{255,  0,  0,  0}},{{146,129,131,255},{255,  0,  0,  0}},{{147,130,132,255},{255,  0,  0,  0}},{{148,131,133,255},{255,  0,  0,  0}},{{149,132,134,255},{255,  0,  0,  0}},{{150,133,135,255},{255,  0,  0,  0}},{{151,134,136,255},{255,  0,  0,  0}},{{152,135,255,  0},{255,  0,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{145,255,  0,  0},{255,  0,  0,  0}},{{144,146,255,  0},{255,  0,  0,  0}},{{145,147,255,  0},{255,  0,  0,  0}},{{146,148,255,  0},{255,  0,  0,  0}},{{147,149,255,  0},{255,  0,  0,  0}},{{148,150,255,  0},{255,  0,  0,  0}},{{149,151,255,  0},{255,  0,  0,  0}},{{150,152,255,  0},{255,  0,  0,  0}},{{151,255,  0,  0},{255,  0,  0,  0}}
}; //,{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}}};
*/
unsigned char g_PawnMoves[BOARD_SIZE-7][2][4] = {
{{  0,  1,  1,  1},{  2,  0,  1,  1}},{{  0,  1,  1,  1},{  1,  3,  0,  1}},{{  0,  1,  1,  1},{  2,  4,  0,  1}},{{  0,  0,  4, 19},{  3,  5,  0,  1}},{{  0,  5,  3, 20},{  4,  6,  0,  1}},{{  0,  0,  4, 21},{  5,  7,  0,  1}},{{  0,  1,  1,  1},{  6,  8,  0,  1}},{{  0,  1,  1,  1},{  7,  9,  0,  1}},{{  0,  1,  1,  1},{  8,  0,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{  0,  1,  1,  1},{  1, 18,  0,  1}},{{  0,  1,  1,  1},{  2, 17, 19,  0}},{{  0,  1,  1,  1},{  3, 18, 20,  0}},{{  0, 20, 35,  3},{  4, 19, 21,  0}},{{ 21, 19, 36,  4},{  5, 20, 22,  0}},{{  0, 20, 37,  5},{  6, 21, 23,  0}},{{  0,  1,  1,  1},{  7, 22, 24,  0}},{{  0,  1,  1,  1},{  8, 23, 25,  0}},{{  0,  1,  1,  1},{  9, 24,  0,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{  0,  1,  1,  1},{ 17, 34,  0,  1}},{{  0,  1,  1,  1},{ 18, 33, 35,  0}},{{  0,  1,  1,  1},{ 19, 34, 36,  0}},{{  0,  0, 36, 19},{ 20, 35, 37,  0}},{{  0, 37, 35, 20},{ 21, 36, 38,  0}},{{  0,  0, 36, 21},{ 22, 37, 39,  0}},{{  0,  1,  1,  1},{ 23, 38, 40,  0}},{{  0,  1,  1,  1},{ 24, 39, 41,  0}},{{  0,  1,  1,  1},{ 25, 40,  0,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{ 65,  0,  1,  1},{ 33, 50,  0,  1}},{{  0,  1,  1,  1},{ 34, 49, 51,  0}},{{ 67,  0,  1,  1},{ 35, 50, 52,  0}},{{  0,  1,  1,  1},{ 36, 51, 53,  0}},{{ 69,  0,  1,  1},{ 37, 52, 54,  0}},{{  0,  1,  1,  1},{ 38, 53, 55,  0}},{{ 71,  0,  1,  1},{ 39, 54, 56,  0}},{{  0,  1,  1,  1},{ 40, 55, 57,  0}},{{ 73,  0,  1,  1},{ 41, 56,  0,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{ 81,  0,  1,  1},{ 49, 66,  0,  1}},{{  0,  1,  1,  1},{ 50, 65, 67,  0}},{{ 83,  0,  1,  1},{ 51, 66, 68,  0}},{{  0,  1,  1,  1},{ 52, 67, 69,  0}},{{ 85,  0,  1,  1},{ 53, 68, 70,  0}},{{  0,  1,  1,  1},{ 54, 69, 71,  0}},{{ 87,  0,  1,  1},{ 55, 70, 72,  0}},{{  0,  1,  1,  1},{ 56, 71, 73,  0}},{{ 89,  0,  1,  1},{ 57, 72,  0,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{ 97, 82,  0,  1},{ 65,  0,  1,  1}},{{ 98, 81, 83,  0},{  0,  1,  1,  1}},{{ 99, 82, 84,  0},{ 67,  0,  1,  1}},{{100, 83, 85,  0},{  0,  1,  1,  1}},{{101, 84, 86,  0},{ 69,  0,  1,  1}},{{102, 85, 87,  0},{  0,  1,  1,  1}},{{103, 86, 88,  0},{ 71,  0,  1,  1}},{{104, 87, 89,  0},{  0,  1,  1,  1}},{{105, 88,  0,  1},{ 73,  0,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{113, 98,  0,  1},{ 81,  0,  1,  1}},{{114, 97, 99,  0},{  0,  1,  1,  1}},{{115, 98,100,  0},{ 83,  0,  1,  1}},{{116, 99,101,  0},{  0,  1,  1,  1}},{{117,100,102,  0},{ 85,  0,  1,  1}},{{118,101,103,  0},{  0,  1,  1,  1}},{{119,102,104,  0},{ 87,  0,  1,  1}},{{120,103,105,  0},{  0,  1,  1,  1}},{{121,104,  0,  1},{ 89,  0,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{129,114,  0,  1},{  0,  1,  1,  1}},{{130,113,115,  0},{  0,  1,  1,  1}},{{131,114,116,  0},{  0,  1,  1,  1}},{{132,115,117,  0},{  0,  0,116,131}},{{133,116,118,  0},{  0,117,115,132}},{{134,117,119,  0},{  0,  0,116,133}},{{135,118,120,  0},{  0,  1,  1,  1}},{{136,119,121,  0},{  0,  1,  1,  1}},{{137,120,  0,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{145,130,  0,  1},{  0,  1,  1,  1}},{{146,129,131,  0},{  0,  1,  1,  1}},{{147,130,132,  0},{  0,  1,  1,  1}},{{148,131,133,  0},{  0,132,147,115}},{{149,132,134,  0},{133,131,148,116}},{{150,133,135,  0},{  0,132,149,117}},{{151,134,136,  0},{  0,  1,  1,  1}},{{152,135,137,  0},{  0,  1,  1,  1}},{{153,136,  0,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},{{  0,  1,  1,  1},{  0,  1,  1,  1}},
{{146,  0,  1,  1},{  0,  1,  1,  1}},{{145,147,  0,  1},{  0,  1,  1,  1}},{{146,148,  0,  1},{  0,  1,  1,  1}},{{147,149,  0,  1},{  0,  0,148,131}},{{148,150,  0,  1},{  0,149,147,132}},{{149,151,  0,  1},{  0,  0,148,133}},{{150,152,  0,  1},{  0,  1,  1,  1}},{{151,153,  0,  1},{  0,  1,  1,  1}},{{152,  0,  1,  1},{  0,  1,  1,  1}}};

/*
    {{255,  0,  0,  0},{  1,255,  0,  0}},{{255,  0,  0,  0},{  0,  2,255,  0}},{{255,  0,  0,  0},{  1,  3,255,  0}},  {{255,255, 3,  18},{  2,  4,255,  0}},{{255,  4,  2, 19},{  3,  5,255,  0}},{{255,255,  3, 20},{  4,  6,255,  0}},   {{255,  0,  0,  0},{  5,  7,255,  0}},{{255,  0,  0,  0},{  6,  8,255,  0}},{{255,  0,  0,  0},{  7,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{255,  0,  0,  0},{  0, 17,255,  0}},{{255,  0,  0,  0},{  1, 16, 18,255}},{{255,  0,  0,  0},{  2, 17, 19,255}},  {{255, 19, 34,  2},{  3, 18, 20,255}},{{20,  18, 35,  3},{  4, 19, 21,255}},{{255, 19, 36,  4},{  5, 20, 22,255}},   {{255,  0,  0,  0},{  6, 21, 23,255}},{{255,  0,  0,  0},{  7, 22, 24,255}},{{255,  0,  0,  0},{  8, 23,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{255,  0,  0,  0},{ 16, 33,255,  0}},{{255,  0,  0,  0},{ 17, 32, 34,255}},{{255,  0,  0,  0},{ 18, 33, 35,255}},  {{255,255, 35, 18},{ 19, 34, 36,255}},{{255, 36, 34, 19},{ 20, 35, 37,255}},{{255,255, 35, 20},{ 21, 36, 38,255}},   {{255,  0,  0,  0},{ 22, 37, 39,255}},{{255,  0,  0,  0},{ 23, 38, 40,255}},{{255,  0,  0,  0},{ 24, 39,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 64,255,  0,  0},{ 32, 49,255,  0}},{{255,  0,  0,  0},{ 33, 48, 50,255}},{{ 66,255,  0,  0},{ 34, 49, 51,255}},  {{255,  0,  0,  0},{ 35, 50, 52,255}},{{ 68,255,  0,  0},{ 36, 51, 53,255}},{{255,  0,  0,  0},{ 37, 52, 54,255}},   {{ 70,255,  0,  0},{ 38, 53, 55,255}},{{255,  0,  0,  0},{ 39, 54, 56,255}},{{ 72,255,  0,  0},{ 40, 55,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 80,255,  0,  0},{ 48, 65,255,  0}},{{255,  0,  0,  0},{ 49, 64, 66,255}},{{ 82,255,  0,  0},{ 50, 65, 67,255}},  {{255,  0,  0,  0},{ 51, 66, 68,255}},{{ 84,255,  0,  0},{ 52, 67, 69,255}},{{255,  0,  0,  0},{ 53, 68, 70,255}},   {{ 86,255,  0,  0},{ 54, 69, 71,255}},{{255,  0,  0,  0},{ 55, 70, 72,255}},{{ 88,255,  0,  0},{ 56, 71,255,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{ 96, 81,255,  0},{ 64,255,  0,  0}},{{ 97, 80, 82,255},{255,  0,  0,  0}},{{ 98, 81, 83,255},{ 66,255,  0,  0}},  {{ 99, 82, 84,255},{255,  0,  0,  0}},{{100, 83, 85,255},{ 68,255,  0,  0}},{{101, 84, 86,255},{255,  0,  0,  0}},   {{102, 85, 87,255},{ 70,255,  0,  0}},{{103, 86, 88,255},{255,  0,  0,  0}},{{104, 87,255,  0},{ 72,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{112, 97,255,  0},{ 80,255,  0,  0}},{{113, 96, 98,255},{255,  0,  0,  0}},{{114, 97, 99,255},{ 82,255,  0,  0}},  {{115, 98,100,255},{255,  0,  0,  0}},{{116, 99,101,255},{ 84,255,  0,  0}},{{117,100,102,255},{255,  0,  0,  0}},   {{118,101,103,255},{ 86,255,  0,  0}},{{119,102,104,255},{255,  0,  0,  0}},{{120,103,255,  0},{ 88,255,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{128,113,255,  0},{255,  0,  0,  0}},{{129,112,114,255},{255,  0,  0,  0}},{{130,113,115,255},{255,  0,  0,  0}},  {{131,114,116,255},{255,255,115,130}},{{132,115,117,255},{255,116,114,131}},{{133,116,118,255},{255,255,115,132}},   {{134,117,119,255},{255,  0,  0,  0}},{{135,118,120,255},{255,  0,  0,  0}},{{136,119,255,  0},{255,  0,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{144,129,255,  0},{255,  0,  0,  0}},{{145,128,130,255},{255,  0,  0,  0}},{{146,129,131,255},{255,  0,  0,  0}},  {{147,130,132,255},{255,131,146,114}},{{148,131,133,255},{132,130,147,115}},{{149,132,134,255},{255,131,148,116}},   {{150,133,135,255},{255,  0,  0,  0}},{{151,134,136,255},{255,  0,  0,  0}},{{152,135,255,  0},{255,  0,  0,  0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},{{255,0,0,0},{255,0,0,0}},
    {{145,255,  0,  0},{255,  0,  0,  0}},{{144,146,255,  0},{255,  0,  0,  0}},{{145,147,255,  0},{255,  0,  0,  0}},  {{146,148,255,  0},{255,255,147,130}},{{147,149,255,  0},{255,148,146,131}},{{148,150,255,  0},{255,255,147,132}},   {{149,151,255,  0},{255,  0,  0,  0}},{{150,152,255,  0},{255,  0,  0,  0}},{{151,255,  0,  0},{255,  0,  0,  0}}
}; //,{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}},{{-1,0,0,0},{-1,0,0,0}}};
*/
/*
int g_KingMoves[BOARD_SIZE-7][8] =
   {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{ 19,  4,0,0,0,0,0,0},{ 20,  3,  5,0,0,0,0,0},{ 21,  4,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{  3, 35, 20,0,0,0,0,0},{  4, 36, 19, 21,0,0,0,0},{  5, 37, 20,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{ 19, 36,0,0,0,0,0,0},{ 20, 35, 37,0,0,0,0,0},{ 21, 36,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{131,116,0,0,0,0,0,0},{132,115,117,0,0,0,0,0},{133,116,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{115,147,132,0,0,0,0,0},{116,148,131,133,0,0,0,0},{117,149,132,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{131,148,0,0,0,0,0,0},{132,147,149,0,0,0,0,0},{133,148,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}
}; //,{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
*/

char horsdiff_[(BOARD_SIZE-7) * 2] =
//{        0,0,0,0,0, 0,0,
    {	0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,-17, 0,-15, 0,0,0, 0,0,0,0,0,0,0,
      0,0,-17,0, 0, 0,-15,0,0, 0,0,0,0,0,0,0,
      0,0,  0,0, 0, 0, 0,0,0, 0,0,0,0,0,0,0,
      0,0, 15,0, 0, 0,17,0,0, 0,0,0,0,0,0,0,
      0,0, 0,15, 0,17, 0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0, 0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0, 0,0,0,0, //0,0,0,
// 0,0,0,0
    };
char *horsdiff = horsdiff_ + BOARD_SIZE-7;


//const int HistorySize = BOARD_SIZE*32;

HistStruct m_his_table[7][10][BOARD_SIZE];  //1017
//HistStruct m_his_table[10][9][10]; //[piecetype][nFile][nRank];
//HistRecord m_hisrecord[512]; //[384] [256]; //[MAX_MOVE_NUM];
//int  m_hisindex=0;
UcciCommStruct UcciComm;
UcciCommEnum BusyComm;

char * MoveStr(char *str, int from, int dest)
{
    sprintf (str, "%c%d%c%d",
             //(from % 9) + 'a', 9 - (from / 9),  (dest % 9) + 'a', 9 - (dest / 9)
             (nFile(from)) + 'a', 9 - (nRank(from)),  (nFile(dest)) + 'a', 9 - (nRank(dest))
            );

    return str;
}

// 这四个数组用来判断棋子的走子方向，以马为例就是：sqDst = sqSrc + cKnightMoveTab[i]
//static const int cKingMoveTab[4]    = {-9, -1, 1, 9};      //{-0x10, -0x01, +0x01, +0x10};
//static const int cADVISORMoveTab[4] = {-10, -8, 8, 10};    //{-0x11, -0x0f, +0x0f, +0x11};
//static const int cElephanMoveTab[4] = {-20, -16, 16, 20};; //{-0x22, -0x1e, +0x1e, +0x22};
//{-19, -17, -11, -7, 7, 11, 17, 19};
//static const int knightchecknew[8] = {9,9,-1,1,-1,1,-9,-9};
//       6   7
//     4       5
//         +
//     2       3
//       0   1
//static const int cKnightMoveTab[8]  = {-19, -17, -11, -7, 7, 11, 17, 19}; //
//static const int cKnightMoveTab[8]  = {-0x21, -0x1f, -0x12, -0x0e, +0x0e, +0x12, +0x1f, +0x21};
  static const int cKnightMoveTab[8]  = {+0x1f, +0x21, +0x0e, +0x12, -0x12, -0x0e, -0x21, -0x1f};

#define INF 8190 //2019q 8190 //2019p 20480 //1101 2048 
// 没有命中置换表而返回值的失败值，必须在-MATE_VALUE和MATE_VALUE以外
const int UNKNOWN_VALUE = INF + 1; //MATE_VALUE + 1;
const int BAN_VALUE = INF - 100;   //1101 #define BAN_VALUE 1948 //(INF-100) //1900;
const int WIN_VALUE = INF - 256;   //1101 #define WIN_VALUE 1848 //1792 //(INF-256) //1800;
const int VALUE_KNOWN_WIN = INF - 200; //1101 #define VALUE_KNOWN_WIN 1848  //=WIN_VALUE (INF-200)
//sf10
//constexpr int MAX_MOVES = 256;
//constexpr int MAX_PLY   = 128;
#define MAX_MOVES 256

const int VALUE_NONE = UNKNOWN_VALUE;  //sf10 2049
const int VALUE_MATE = INF - 1;        //sf10 2047 
const int VALUE_MATE_IN_MAX_PLY = VALUE_MATE - 2 * MAX_PLY;
const int VALUE_MATED_IN_MAX_PLY = -VALUE_MATE + 2 * MAX_PLY;
/* sf10 -------------------------------------------------------------
  PawnValueMg   = 136,   PawnValueEg   = 208,
  KnightValueMg = 782,   KnightValueEg = 865,
  BishopValueMg = 830,   BishopValueEg = 918,
  RookValueMg   = 1289,  RookValueEg   = 1378,
  QueenValueMg  = 2529,  QueenValueEg  = 2687,

  MidgameLimit  = 15258, EndgameLimit  = 3915
*/  


//3dc092 13,25,26,144,152,340  (p,b,e,n,c,r)
//#define ENDPAWNBONUS 32 //32(1891u) //25(87v) //27(77b) //28 //32(73k) //32(73b) //3dc92=16
// 1892q - real optimize VPE,VPER,VBE,VEE
int VP=96;   //240; //1101 24 //18 //24 //21 //24
int VPR=380; //952; //1101 95 //95(87v) //95(73b) //VP*2 //95 //95 //83	//cross-river pawn val
int VPE=224; //561; //1101 56  //24+32=56; 
int VPER=516; //1293; //bef clop 127;
int VB= 216;  //541;  //35 //VP*2-1 //47 //53 //53(72w) //54 //40 //37 //54(87w) //54 //56
int VE= 212;  //531;  //(VB+1) //53 //39 //36 //53(87w) //54 //56
int VBE=216;  //541;  // 54
int VEE=196; //49 212;  //531;  //bef clop 53;
int VC= 804;  //2018; //bef clop 202;
int VN= 770;  //1926; //bef clop 190; 
int VCE=796;  //1998; //203 //199 //200 //200(87w
int VNE=788;  //1976;  //bef clop 194;
int VR =1864; //466 4675; //4570; //4530; //4660  // 3460  //1101 4660 -1200 ; //bef clop 460; =VC+VN=2010+1920=3930  //1011 adj posval
int VRE=1824; //456 4575; //3600; // 4600;  //4560; //3360 //1101 4560 -1200 ; //bef clop 460; =VCE+VNE=1990+1970=3960  //1011 adj 

int PIECE_VALUE_side[46]; // =
//    {0,0,-VP,VP,-VP,VP,-VP,VP,-VP,VP,-VP,VP,-VB,VB,-VB,VB,-VE,VE,-VE,VE,-VNC,VNC,-VNC,VNC,-VNC,VNC,-VNC,VNC,-VR,VR,-VR,VR,-9999,9999,
//    0,0,-VPR,VPR,-VPR,VPR,-VPR,VPR,-VPR,VPR,-VPR,VPR};
int PIECE_VALUE_side_ENDGAME[46]; // =
//    {0,0,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VBE,VBE,-VBE,VBE,-VEE,VEE,-VEE,VEE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VRE,VRE,-VRE,VRE,-9999,9999,
//    0,0,-VPER,VPER,-VPER,VPER,-VPER,VPER,-VPER,VPER,-VPER,VPER};
//#define XX 0 //1891v 36
//static int PIECE_VALUE_NOROOK[46] =
//    {0,0,-VPE-XX,VPE+XX,-VPE-XX,VPE+XX,-VPE-XX,VPE+XX,-VPE-XX,VPE+XX,-VPE-XX,VPE+XX,-VBE,VBE,-VBE,VBE,-VEE,VEE,-VEE,VEE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VRE,VRE,-VRE,VRE,-9999,9999,
//    0,0,-VPER-XX,VPER+XX,-VPER-XX,VPER+XX,-VPER-XX,VPER+XX,-VPER-XX,VPER+XX,-VPER-XX,VPER+XX};    
//static int BasicValues[2][8] = {{VP,VP,VP,VB,VE,VN,VC,VR}, //9999}, // 56-> 55 //centrepawn,sidepawn,pawn,ADVISOR,elephant,knight,cannon,rook,king eyu
//    {VPE,VPE,VPE,VBE,VEE,VNE,VCE,VRE} //,9999}
//};   // 69-> 65 -> 64
//static int BasicValues[2][5] = {{VP,VE,VN,VC,VR}, //9999}, // 56-> 55 //centrepawn,sidepawn,pawn,ADVISOR,elephant,knight,cannon,rook,king eyu
//    {VPE,VEE,VNE,VCE,VRE} //,9999}    
//};   // 69-> 65 -> 64
int BasicValues[10]; // = {VP,VE,VN,VC,VR, VPE,VEE,VNE,VCE,VRE}; 

//extern int centrehorse, endcenhorse;   //58, 16    
//static char PosValues[2][5][BOARD_SIZE] =
//1101 - inc value by x10 static char PosValues[2][5][90] =
static short PosValues[2][5][90] =
{{{
        -60, -50, -50, -30,  30, -30, -50, -50, -60,
        340, 540, 940,1230,1270,1230, 940, 540, 340,
        340, 540, 840,1040,1040,1040, 840, 540, 340,
        340, 480, 540, 590, 620, 590, 540, 480, 340,
        110, 290, 300, 480, 500, 480, 300, 290, 110,
       -100,  0,  30,  0,  130,  0,  30,  0,-100,	//7->13
       -100,  0,-100,  0,  120,  0,-100,  0,-100,  //6->12
         0,  0,  0,  -7990, -7990, -7990,  0,  0,  0,
         0,  0,  0,  -7960, -7960, -7960,  0,  0,  0,
         0,  0,  0,  -7790, -7700, -7790,  0,  0,  0}, //   0,0,0,0,0,0,0,      // PAWN KING  -20

        {0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, -60,  0,  0,  0, -60,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,
        -40,  0,  0, -10,  80, -10,  0,  0, -40,
         0,  0,  0,  0,  50,  0,  0,  0,  0,
         0,  0,  10,  10,  0,  10,  10,  0,  0},        // ADVISOR ELEPHAN black

        {-100, -90, -70,  30,-100,  30, -70, -90,-100,
         -100,  40, 180,  40, -10,  40, 180,  40,-100,
         -40,   60,  90, 180,  90, 180,  90,  60, -40,
         -40,  270, 130, 250, 140, 250, 130, 270, -40,
         -100, 110,  90, 180, 190, 180,  90, 110,-100,
         -100,  80, 130, 150, 160, 150, 130,  80,-100,
         -60,  -10,  50,  00,  60,  00,  50, -10, -60,
         -40,  -60, -10,  00, -60,  00, -10, -60, -40,
         -190,-100, -60, -40,-580, -40, -60,-100,-190,   //-50->-54->58 //1012
         -130,-200,-100,-130,-100,-130,-100,-200,-130}, 					// knight black

{70,  70,  10,  -50,  -70,  -50,   10,   70,  7,
 40,  40,  10, -130,  -90, -130,   10,   40,  40, //-8->-9   
 20,  20,  10, -160,  -40, -160,   10,   20,  20,
 10,  60,   60,   40,   70,   40,   60,   60,  10, 
 10,  20,  10,   10,   60,   10,   10,   20,  10,
 -40, -20,  40,   10,   60,   10,   40,   -20,  -40, //-1->-2->-3 1->0  -1=>-2 2892w
 10,  -10,  10,   10,   10,   10,   10,   -10,  10,  //1->0
 30,  10,  70,   50,   90,   50,   70,   10,  30,
 10,  30,  40,   40,   40,   40,   40,   30,  10,
 10,  10,  30,   50,   50,   50,   30,   10,  10},   //cannon black

		   {160,180,140,230,240,230,140,180,160, //15->14
				160,220,190,260,530,260,190,220,160,
				120,180,170,240,260,240,170,180,120,	//16->14->13->12
				140,230,230,260,260,260,230,230,140, //14
				170,210,210,240,250,240,210,210,170,
				150,220,220,240,250,240,220,220,150,  //18->15
				140,190,140,220,240,220,140,190,140,
				-30, 180,140,220,220,220,140,180,-30,   // 6-> -3
				100,180,130,230, 30,230,130,180,100,		//12->11->10 //22->23 //14->13
       -240, 180,140,220, 50,220,140,180,-240}},	// ROOK black ey 320  = (200+120) late corner rook (from -8 to -10)

/*
  {260,280,240,330,340,330,240,280,260, //1101   add 100 to offset VR
				260,320,290,360,630,360,290,320,260,
				220,280,270,340,360,340,270,280,220,	//16->14->13->12
				240,330,330,360,360,360,330,330,240, //14
				270,310,310,340,350,340,310,310,270,
				250,320,320,340,350,340,320,320,250,  //18->15
				240,290,240,320,340,320,240,290,240,
				 70, 280,240,320,320,320,240,280,70,   // 6-> -3
				200,280,230,330, 130,330,230,280,200,		//12->11->10 //22->23 //14->13
       -140, 280,240,320, 150,320,240,280,-140}},	// ROOK black ey 320  = (200+120) late corner rook (from -8 to -10)
*/
// for endgame
    {
    		{	-180,-180,-180,-150,-150,-150,-180,-180,-180,
    			350, 440, 800, 1030, 1140,1030, 800, 440, 350, //103-> 100
    			350, 430, 680, 860, 870, 860, 680, 430, 350,
    			330, 380, 440, 590, 620, 590, 440, 380, 330, //62->80
    			130, 330, 350, 380, 400, 380, 350, 330, 130,	  //11->9   30->35
    			
          110,  00, 150,  00, 220,  00, 150,  00, 110,
    			80,  00,   80,  00, 170,  00,  80,  00,  80,		//-8->-6
    			00,  00,   00, -7990,-7960,-7990,  00,  00,  0,	//-97 -> -99
    			00,  00,   00, -7960,-7940,-7960,  00,  00,  0,
          00,  00,   00, -7780,-7700,-7780,  00,  00,  0},          // PAWN KING  -20

				{00,  00,  00,  00,  00,  00,  00,  00,  0,
         00,  00,  00,  00,  00,  00,  00,  00,  0,
         00,  00,  00,  00,  00,  00,  00,  00,  0,
         00,  00,  00,  00,  00,  00,  00,  00,  0,
         00,  00,  00,  00,  00,  00,  00,  00,  0,
         00,  00, -60,  00,  00,  00, -60,  00,  0,
         00,  00,  00,  00,  00,  00,  00,  00,  0,
        -40,  00,  00, -10,  80, -10,  00,  00, -40,
         00,  00,  00,  00,  40,  00,  00,  00,  00,  // 5->4
         00,  00,  10,  10,  00,  10,  10,  00,  0},        // ADVISOR ELEPHAN black

       {50, 100, 150, 150, 150, 150, 150, 100, 50,
        100, 150, 250,200,200,200,250,150, 100,
        150,200,250,270,250,270,250,200,150,   //corner chk 25->27
        140,250,240,250,250,250,240,250,140,
        140,200,240,250,250,250,240,200,140,

         100,150,200,200,200,200,200,150, 100,
         100,150,200,200,200,200,200,150, 100,
         50, 100,150,150,150,150,150,100, 50,    //15->12
         00, 50, 100, 50,-160, 50,100, 50, 00,    //1011 -5->-16
        -50, 00, 50, 00, 00, 00, 50, 0,-50},   //knight

        {70, 80, 80, 80,  80,  80, 80, 80, 70,
         80, 80, 80, 80,  80,  80, 80, 80, 80,
         80, 80, 80, 80,  80,  80, 80, 80, 80,
         80, 100, 100, 130, 190, 130, 100, 100, 80,
         80, 100, 100, 130, 190, 130, 100, 100, 80,
         80, 100, 100, 140, 190, 140, 100, 100, 80,	  // was 2 at side
         80, 100, 100, 140, 190, 140, 100, 100, 80,
         80, 100, 100, 170, 220, 170, 100, 100, 80,   //15-19-15 ->  17-22-17
        100, 100, 100, 190, 210, 190, 100, 100, 100,
        100, 100, 100, 190, 220, 190, 100, 100, 100},   // CANNON black  ey 154 ok
        
       {230,230,230,280,310,280,230,230,230,
        260,260,260,290,600,290,260,260,260,      //30->46->78->89->85->80
        230,230,230,260,290,260,230,230,230,
        200,200,200,240,280,240,200,200,200,
        200,200,200,240,260,240,200,200,200,
        200,200,200,240,260,240,200,200,200,
        180,190,190,240,260,240,190,190,180,
        180,190,190,240,260,240,190,190,180,
        180,190,190,200,220,200,190,190,180,      //1113 egbug 240->200, 260->220
        180,190,190,240,260,240,190,190,180}}     // ROOK black ey 320 = (200 + 120)

/*
       {330,330,330,380,410,380,330,330,330,      //1101 add 100 to offset VRE
        360,360,360,390,700,390,360,360,360,      //30->46->78->89->85->80
        330,330,330,360,390,360,330,330,330,
        300,300,300,340,380,340,300,300,300,
        300,300,300,340,360,340,300,300,300,
        300,300,300,340,360,340,300,300,300,
        280,290,290,340,360,340,290,290,280,
        280,290,290,340,360,340,290,290,280,
        280,290,290,340,360,340,290,290,280,
        280,290,290,340,360,340,290,290,280}}     // ROOK black ey 320 = (200 + 120)
*/
};  //0,0,0,0,0,0,0}}};

//unsigned char ATTKPVAL[34]   = {0,0,2,2,0,0,0,0,1,1,1,1,4,4,4,4,2,2,2,2, 7,7,7,7,7,7,7,7,9,9,9,9,1,1};
//1101 int ATTKPVAL[34]   = {0,0,2,2,0,0,0,0,1,1,1,1,4,4,4,4,3,3,3,3, 7,7,7,7,7,7,7,7,9,9,9,9,8,8};
//1101 int ATTKPVAL_3[34] = {0,0,5,5,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,10,10,10,10,10,10,10,10,12,12,12,12,11,11};
int ATTKPVAL[34]   = {0,0,20,20,0,0,0,0,10,10,10,10,40,40,40,40,30,30,30,30, 70,70,70,70,70,70,70,70,90,90,90,90,80,80};
int ATTKPVAL_3[34] = {0,0,50,50,30,30,30,30,40,40,40,40,50,50,50,50,60,60,60,60,100,100,100,100,100,100,100,100,120,120,120,120,110,110};

//static const int ATTKPVAL_EG[34]   = {0,0,5,5,5,5,5,5,5,5,5,5,4,4,4,4,2,2,2,2, 7,7,7,7,7,7,7,7,9,9,9,9,1,1};
//static const int ATTKPVAL_3_EG[34] = {0,0,8,8,8,8,8,8,8,8,8,8,5,5,5,5,6,6,6,6,10,10,10,10,10,10,10,10,12,12,12,12,4,4};

//char pointtableABS[BOARD_SIZE-7][16]; //[10]; //[32];  // for genNoncap() use
short abs_pointtable[10][10][9]; //[piecetype]{nRank][nFile]    //1101 char
//short pointtable[BOARD_SIZE-7][16][2]; //[10]; //[32]; //[34][90] [2]=board.p_endgame=0,1
short pointtable[10][10][9]; //[piecetype][nRank]{nFile]
short eg_pointtable[10][10][9]; //end-game
//short abspointtable[10][10][9]; //[piecetype][nRank]{nFile]
//short eg_abspointtable[10][10][9]; //end-game
//short pointtableAdvEleB[(BOARD_SIZE/2)-7][2]; //preEval
//short pointtableAdvEleR[(BOARD_SIZE/2)-7][2]; //preEval
//short pointtablePawnB[(7*16)-7][2]; //preEval
//short pointtablePawnR[(7*16)-7][2]; //preEval
#ifdef PREEVAL
short pointtableAdvEleB[5][9]; //preEval
short pointtableAdvEleR[5][9]; //preEval
short pointtablePawnB[7][9]; //preEval
short pointtablePawnR[7][9]; //preEval
#endif
//short pointtableHorsB[5][9]; //preEval
//short pointtableHorsR[5][9]; //preEval
//short eg_pointtableAdvEleB[9][5]; //preEval
//short eg_pointtableAdvEleR[9][5]; //preEval
//short eg_pointtablePawnB[9][7]; //preEval
//short eg_pointtablePawnR[9][7]; //preEval
//int SCORE80_20;
//int SCORE6_2;
extern int EMPTY_CANN_SCORE[9];  //1125
extern int BOTTOM_CANN_SCORE[9];  //1125
//1125 int EMPTY_CANN_SCORE[9] = {0,0,680,800,960,1200,1800,1800,1800};  //1111
//1125 int BOTTOM_CANN_SCORE[9] = {340,260,40,0,0,0,40,260,340};    //1111
//1111 int EMPTY_CANN_SCORE[9] = {0,0,170,200,240,300,450,450,450};   //1107  {0,0,340,400,480,600,900,900,900};
//1111int BOTTOM_CANN_SCORE[9] = {85,65,10,0,0,0,10,65,85};      //1107  {170,130,20,0,0,0,20,130,170};

//int EMPTY_CANN_SCORE[9] = {0,0,34,40,48,60,90,90,90};
//int EMPTY_CANN_SCORE_endgame[9] = {0,0,8,10,12,15,22,22,22};
//unsigned char EMPTY_CANN_SCORE[3][16] =
//{{0,0,8,12,14,17,22,22,22, 0,0,0,0,0,0,0},
// {0,0,27,33,39,48,66,66,66, 0,0,0,0,0,0,0},
// {0,0,38,44,52,64,90,90,90, 0,0,0,0,0,0,0}};
/* pre-84p
{{0,0,8,10,12,15,22,22,22, 0,0,0,0,0,0,0},
 {0,0,24,30,36,45,66,66,66, 0,0,0,0,0,0,0},
 {0,0,34,40,48,60,90,90,90, 0,0,0,0,0,0,0}};
*/
//int BOTTOM_CANN_SCORE[9] = {0,0,13,18,18,18,18,18,18};
//int BOTTOM_CANN_SCORE[2][9] = {{0,0,13,20,20,20,20,20,20},{0,0,13,20,20,20,20,20,20}};
//int BOTTOM_CANN_SCORE[2][9] = {{0,6,60,80,80,80,80,80,80},{0,6,60,80,80,80,80,80,80}};
//static const unsigned char BOTTOM_CANN_SCORE_ORI[9][2] = {{80,80},{60,60},{6,6},{0,0},{0,0},{0,0},{6,6},{60,60},{80,80}};
//static const unsigned char BOTTOM_CANN_SCORE_ORI[9][2] = {{40,40},{30,30},{3,3},{0,0},{0,0},{0,0},{3,3},{30,30},{40,40}};
//int BOTTOM_CANN_SCORE[9][2];
//int HORSE_MOBSCORE[9][2] = {{-14,14},{-10,10},{0,0},{6,-6}, {7,-7},{9,-9},{10,-10},{12,-12},{13,-13}};
//static const int HORSE_MOBSCORE_ENDGAME[9][2] = {{-20,20},{-15,15},{0,0},{8,-8}, {13,-13},{18,-18},{23,-23},{28,-28},{33,-33}};
//1101 int HORSE_MOBSCORE[9] = {-30,-13,-9,-6,0,3,6,8,8};
//1101 static const int HORSE_MOBSCORE_ENDGAME[9] = {-60,-26,-15,-6,0,3,6,8,8};
int HORSE_MOBSCORE[9] = {-30*8,-13*8,-9*8,-6*8,0,3*8,6*8,8*8,8*8};                         //1110
static const int HORSE_MOBSCORE_ENDGAME[9] = {-60*8,-26*8,-15*8,-6*8,0,3*8,6*8,8*8,8*8};   //1110
//int QCHECKDEPTH = 0;
#ifdef HORSECK
unsigned char kingidxpos[18]={3,4,5, 19,20,21, 35,36,37, 115,116,117, 131,132,133, 147,148,149};
#endif
char kingindex[BOARD_SIZE-7] =
    {-1,-1,-1,0,1,2,-1,-1,-1,      0,0,0,0,0,0,0,
     -1,-1,-1,3,4,5,-1,-1,-1,      0,0,0,0,0,0,0,
     -1,-1,-1,6,7,8,-1,-1,-1,      0,0,0,0,0,0,0,
     -1,-1,-1,-1,-1,-1,-1,-1,-1,   0,0,0,0,0,0,0,
     -1,-1,-1,-1,-1,-1,-1,-1,-1,      0,0,0,0,0,0,0,
     -1,-1,-1,-1,-1,-1,-1,-1,-1,      0,0,0,0,0,0,0,
     -1,-1,-1,-1,-1,-1,-1,-1,-1,     0,0,0,0,0,0,0,
     -1,-1,-1,9,10,11,-1,-1,-1,   0,0,0,0,0,0,0,
     -1,-1,-1,12,13,14,-1,-1,-1,   0,0,0,0,0,0,0,
     -1,-1,-1,15,16,17,-1,-1,-1
    }; //,   0,0,0,0,0,0,0};
unsigned char kingattk_incl_horse[18][BOARD_SIZE-7] =
{{
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0
 },
{1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0
 },
{1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0
 },

{0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0
 },
{0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0
 },
{0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0
 },

{0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0
 },
{0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0
 },
{0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0
 },

{0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0
 },
{0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0
 },
{0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0
 },

{0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0
 },
{0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0
 },
{0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0
 },

{0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,1,1,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1
 },
{0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1
 },
{0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0, 0,0,0,0,0,0,0,
 0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,
 0,0,0,1,1,1,1,1,0, 0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,1
 }};

//#define FUTPAWN 32 //28 //28 //24 //30 //28,35 //30 //24 //30 //26(1880g) //30(1880c) //24(1878n) //VP //24 //10 //16 //14 //28 //26
//int FUTPAWN=32;
//#define ENDFUTPAWN 28 //30 //31(1880u) //26(1880s) //33 //36
// sf10 Razor and futility margins
/*  constexpr int RazorMargin = 600;
  Value futility_margin(Depth d, bool improving) {
    return Value((175 - 50 * improving) * d / ONE_PLY);
  }

  // Futility and reductions lookup tables, initialized at startup
  int FutilityMoveCounts[2][16]; // [improving][depth]
  int Reductions[2][2][64][64];  // [pv][improving][depth][moveNumber]

  template <bool PvNode> Depth reduction(bool i, Depth d, int mn) {
    return Reductions[PvNode][i][std::min(d / ONE_PLY, 63)][std::min(mn, 63)] * ONE_PLY;
  }
*/

// razoring
int RazorMargin = 680; //razor; //69; //3*FUTPAWN  //sf10 =600   clop=68
int ENDRazorMargin = 800; //razor+razorinc; //78; //3*ENDFUTPAWN clop=80=68+12
//int TordRazorDepth =  3;
//int TordRazorMargin = 180; //150; //150; //125; //135; //300
// pawn val 24, futmargin1=1.50 * 24 = 36, margin2=3.00 * 24 = 72
//int FUT_MARGIN[8] = {0,36,72,195,240,285,325,370};
//int FUT_MARGIN[8] = {0,45,125,195,240,285,325,370};
//  int FUT_MARGIN[8] = {0,FUTPAWN, FUTPAWN*2, FUTPAWN*(32+5)/16, FUTPAWN*(32+10)/16, FUTPAWN*(48+4)/16, FUTPAWN*(48+10)/16, FUTPAWN*8}; //195}; //36 72;
// 1877f
//int FUT_MARGIN[6] = {0,FUTPAWN*3/2, FUTPAWN*3, FUTPAWN*6, FUTPAWN*15/2, FUTPAWN*15/2}; //36 72 144;
// 1880o 1880t
//int FUT_MARGIN[6] = {0,FUTPAWN, FUTPAWN*3, FUTPAWN*5, FUTPAWN*15/2, FUTPAWN*15/2}; //36 72 144;
// 1880u
//int FUT_MARGIN[4] = {0,FUTPAWN, FUTPAWN*3, FUTPAWN*5};
//if (depth >= 2) futility_margin = 200 + (depth % 2) * 100;  //toga 1.4.2SE
// 1878n
//int FUT_MARGIN[6] = {0,FUTPAWN, FUTPAWN*3, FUTPAWN*5, FUTPAWN*15/2, FUTPAWN*15/2};   //1880c
//int FUT_MARGIN[6] = {0,FUTPAWN, FUTPAWN*3, FUTPAWN*3, FUTPAWN*4, FUTPAWN*4}; //1880g
int fut_depth=6; //3; //3; //3; //5; //5=stockfish //2; //endgame=1 //2; //3; //toga1.4.1se=5 //3; //2; //endgame=0
//int DELTA_MARGIN=FUTPAWN; //*5/4; //VE; //FUTPAWN; // / 2; //12; //25; //30; //25; //30; //30; //25; //30; //25; //30; //25;
//int SINGULAR_MARGIN=FUTPAWN/2; //*3/4;   //*2; // /8; //32/4; end=28/4 //64-1889n  // for Singular ext
int FUTPAWN=230; //=32;
int ENDFUTPAWN=260; //=28; //30 //31(1880u) //26(1880s) //33 //36
//extern int futfact1, futfact2; //43,9 (aft clop), 35, 10 (bef clop) FUTPAWN * 3 / 2 - FUTPAWN * 3 / 7 
//extern int mvcntfact1, mvcntfact2, mvcntfact3, mvcntfact4; //2.4, 0.74, 5.0, 1.00 (sf10) //2.5, 0.73, 3.3, 0.81
int futility_margin(int d, bool improving) {
	return ((43 - 9 * improving) * d);   //sf10 175,50
}
// Futility and reductions lookup tables, initialized at startup
  int FutilityMoveCounts[2][16]; // [improving][depth]
// 1013 - todo  //1023 - startdo
  int Reductions[2][2][64][64];  // [pv][improving][depth][moveNumber] //NonPV=0, PV=1

template <bool PvNode> int reduction(bool i, int d, int mn) {       //1023
    return Reductions[PvNode][i][std::min(d, 63)][std::min(mn, 63)];  //1023
}
//extern int ext_deltamg; //clop 37
//extern int ext_deltaeg; //clop 24
//int DELTA_MARGIN=34; //=32;
int DELTA_MARGIN=370; //ext_deltamg; //34
//int DELTA_MARGIN_EG=22; //=28;
int DELTA_MARGIN_EG=240; //ext_deltaeg; //=22;
//extern int SINGULAR_MARGIN_EG; //=14; 
int SINGULAR_MARGIN=151; //1101 151 140; //=16;
int NullMoveMargin = VN;
//int delta_piecedest = B_ROOK1;
//int nullreduction=3;  //3; //2; //3;
#define nullreduction 3
//int nullreductver=5; //5; //5; //6; //gluarung=6 toga=5; //9; //8; //endgame=6; //4 //6; //5; //6;

#ifdef HISTPRUN

//int HistCutShift=3; //4;
int ExtHistPrunShift=2; //3(60p);
int ExtHistDepth=6; //7(60p); //6; //endgame=9
//int ExtHistCount=9;
int histcut_depth=7; //7=stockfish 1.3 //6=toga1.4.1se //4; //endgame=0 (mean no cut at endgame)
//int histcut_count=5; //9;
#define HistoryDepth  3 //2=stockfish //3=toga //2 //3 //3
int HistoryMoveNb=7; //6; //endgame=9 //=6; //5 //6 // 6 //7 //6 //5  //3   // >=5 crafty does not reduce first 4 history moves
//static /* const */ int HistoryValue = 9830; // 60%
//static /* const */ int HistoryBound = 2458; // * 16384 + 50) / 100 10%=1638 15%=2458 20%=3277
//#define HistPrunShift 1  // 历史表裁剪的比例阈值，1=50%，2=25%，3=12.5%  4=6.25%，等等
//#define ExtHistPrunShift 2
#endif
extern int ext_IMaxTime;
extern int ext_p_endgame;
int PollNodes = 32767;  // number of nodes between poll for checkstop/timeout. Depends on time remain
//int QPollNodes = 8191; //32767; 
//int DROPDOWN_VAL=45;



void Engine::Setpointtable(int endgame) // 0=start-midgame, 1=endgame
{	//printf("info Setpointtable start\n");
    int m,n,posval;

//    //king/pawn, bis/ele, hors, cann, rook
//		for (i=0; i<16; i+=2)
    for (int p=0; p<10; p+=2)
    {
        for (n=0; n<9;  n++)
        for (m=0; m<10; m++)
            {
                //posval = pointtableABS[p][n][m];
                posval = PosValues[endgame][p/2][((9-m)*9)+n] *4/10;    //1127
                //abspointtable[p][m][n]= posval + 100;
                abs_pointtable[p][m][n]= posval;
                abs_pointtable[p+1][9-m][n] = posval;
//1101                if (p==0 && posval <= -70) //king posval
//1101                	posval += 9999;	//inc king val
                //abspointtable[p+1][9-m][n] =
                //abspointtable[p][m][n]=
                //pointtable[p][m][n]= posval + BasicValues[endgame][p/2];
                pointtable[p][m][n]= posval + BasicValues[endgame*5 +(p/2)];

                pointtable[p+1][9-m][n] = -pointtable[p][m][n];	// [89-j]
                //abspointtable[p+1][9-m][n] = abspointtable[p][m][n];	// [89-j]
            }
    }

#ifdef PREEVAL
//save for preEval
/*
for (i=0; i<9; i++)
{
for (j=0; j<5; j++)
{
	pointtableAdvEleB[i][j] = pointtable[2][i][j];  //[2]
	pointtableAdvEleR[i][j] = pointtable[3][i][9-j]; //[3]
}

for (j=0; j<7; j++)
{
	pointtablePawnB[i][j] = pointtable[0][i][j+3];
	pointtablePawnR[i][j] = pointtable[1][i][9-(j+3)];
}
}
*/
memcpy(pointtableAdvEleB, &(pointtable[2][0][0]), sizeof(pointtableAdvEleB));
memcpy(pointtableAdvEleR, &(pointtable[3][5][0]), sizeof(pointtableAdvEleR));
//memcpy(pointtableHorsB, &(pointtable[4][0][0]), sizeof(pointtableHorsB));
//memcpy(pointtableHorsR, &(pointtable[5][5][0]), sizeof(pointtableHorsR));
memcpy(pointtablePawnB, &(pointtable[0][3][0]), sizeof(pointtablePawnB));
memcpy(pointtablePawnR, &(pointtable[1][0][0]), sizeof(pointtablePawnR));
#endif

if (endgame)
{
	memcpy(eg_pointtable, pointtable, sizeof(pointtable));
	//memcpy(eg_abspointtable, abspointtable, sizeof(abspointtable));
//	memcpy(eg_pointtableAdvEleB, pointtableAdvEleB, sizeof(pointtableAdvEleB));
//	memcpy(eg_pointtableAdvEleR, pointtableAdvEleR, sizeof(pointtableAdvEleR));
//	memcpy(eg_pointtablePawnB, pointtablePawnB, sizeof(pointtablePawnB));
//	memcpy(eg_pointtablePawnR, pointtablePawnR, sizeof(pointtablePawnR));
}
//debug
/*
    printf("info abspointtable: endgame=%d\n", endgame);
    //for (i=0; i<16; i++)
    for (int i=0; i<10; i++)
    {	printf("i=%d\n", i);
    	for (m=0; m<10; m++)
      {      for (n=0; n<9;  n++)
            {
                //j=(m*16) + n;
    						//printf("%5d", pointtableABS[j][i]);
    						printf("%5d", abspointtable[i][m][n]);
    				}
    				printf("\n");
  		}
    }

    printf("info eg_abspointtable: endgame=%d\n", endgame);
    //for (i=0; i<16; i++)
    for (int i=0; i<10; i++)
    {	printf("i=%d\n", i);
    	for (m=0; m<10; m++)
      {      for (n=0; n<9;  n++)
            {
                //j=(m*16) + n;
    						//printf("%5d", pointtable[j][i][endgame]);
    						printf("%5d", eg_abspointtable[i][m][n]);
    				}
    				printf("\n");
  		}
    }

    printf("info pointtable: endgame=%d\n", endgame);
    //for (i=0; i<16; i++)
    for (int i=0; i<10; i++)
    {	printf("i=%d\n", i);
    	for (m=0; m<10; m++)
      {      for (n=0; n<9;  n++)
            {
                //j=(m*16) + n;
    						//printf("%5d", pointtable[j][i][endgame]);
    						printf("%5d", pointtable[i][m][n]);
    				}
    				printf("\n");
  		}
    }

    printf("info eg_pointtable: endgame=%d\n", endgame);
    //for (i=0; i<16; i++)
    for (int i=0; i<10; i++)
    {	printf("i=%d\n", i);
    	for (m=0; m<10; m++)
      {      for (n=0; n<9;  n++)
            {
                //j=(m*16) + n;
    						//printf("%5d", pointtable[j][i][endgame]);
    						printf("%5d", eg_pointtable[i][m][n]);
    				}
    				printf("\n");
  		}
    }
*/
//endebug

}

void Engine::AdjustEndgame(Board &board)
{
//adjust pointtable depending on endgame reached
if (ext_p_endgame !=1)
{
	bool RCHcountLe2;
#ifndef _WIN64
  if (POPCNT_CPU)	
#endif  	
 	  RCHcountLe2 = PCbitCountMSB(board.bitpiece & 0x55500000) <=2 || PCbitCountMSB(board.bitpiece & 0xaaa00000) <=2;

#ifndef _WIN64
  else
 	  RCHcountLe2 = bitCountMSB(board.bitpiece & 0x55500000) <=2 || bitCountMSB(board.bitpiece & 0xaaa00000) <=2;
#endif 	  
	// endgame if attacking pieces (R,C,H) <=2 or no nooks
  if ( RCHcountLe2 || (board.bitpiece & 0xf0000000) ==0 // no rooks
  //  if (bitCount(bitpiece & 0xfff00000) <=6
  //if (bitCountLSB((bitpiece & 0xfff00000)>>16) <=6
  //if (bitCountMSB((bitpiece & 0xfff00000)) <=6 //6
             // 1rook+1hors/cann vs 1rook+1hors/cann
  //if ( bitCountMSB((bitpiece & 0xf0000000)) * 2 + bitCountMSB((bitpiece & 0x0ff00000)) <=8 //6
    )
  
    {
            	
      if ((board.bitpiece & 0xf0000000) ==0) // no rooks
      {
	      FUTPAWN = ENDFUTPAWN;
	      RazorMargin = ENDRazorMargin;   //1009
	      //DELTA_MARGIN = ENDFUTPAWN;
	      //SINGULAR_MARGIN = ENDFUTPAWN/2; //*3/4; //*2; ///8;
        DELTA_MARGIN = DELTA_MARGIN_EG;
        //    SINGULAR_MARGIN = SINGULAR_MARGIN_EG;

      }
      //else
   	   memcpy(PIECE_VALUE_side, PIECE_VALUE_side_ENDGAME, sizeof(PIECE_VALUE_side));             
                
                board.p_endgame=1;
                //PollNodes = 1023;
                //DRAWVALUE = 1; //endgame value
                TEMPO_BONUS = 0; //endgame value
                //root_qcheck_depth = -4; //83g
                //qcheck_depth = 0; //-2; 83f
                //board.p_endgame_7 = 6;
                //SetpointtableABS(1);
                memcpy(pointtable, eg_pointtable, sizeof(pointtable));
//              memcpy(abspointtable, eg_abspointtable, sizeof(abspointtable));
#ifdef PREEVAL
                memcpy(pointtableAdvEleB, &(pointtable[2][0][0]), sizeof(pointtableAdvEleB));
								memcpy(pointtableAdvEleR, &(pointtable[3][5][0]), sizeof(pointtableAdvEleR));
								//memcpy(pointtableHorsB, &(pointtable[4][0][0]), sizeof(pointtableHorsB));
								//memcpy(pointtableHorsR, &(pointtable[5][5][0]), sizeof(pointtableHorsR));
								memcpy(pointtablePawnB, &(pointtable[0][3][0]), sizeof(pointtablePawnB));
								memcpy(pointtablePawnR, &(pointtable[1][0][0]), sizeof(pointtablePawnR));
#endif
                //Setpointtable(1);

                //nullreduction=3;
                //nullreductver=5;
                //ClearHash();	// clearhash when entering endgame
                //fut_depth=2;

                //FUT_MARGIN[1]=ENDFUTPAWN;
                //FUT_MARGIN[2]=ENDFUTPAWN*3;
                //FUT_MARGIN[3]=ENDFUTPAWN*4; //5;
                //DELTA_MARGIN =ENDFUTPAWN;
                /* 1878n
                //fut_depth=2; // 1; //0; //1873a
                FUT_MARGIN[1]=FUTPAWN*3; //72; //65; // 65; //70; //70; //60; //70; //65; //70; //50; //60;
                FUT_MARGIN[2]=FUTPAWN*6;
                //delta_piecedest = 0;
                //DELTA_MARGIN = FUTPAWN*4/3; // 4/3 1875p
                */
                //1873b
                //DELTA_MARGIN=FUTPAWN*5/2; //3/2;


								//1873e
								//fut_depth=0;
								//delta_piecedest = 0;

                //histcut_depth=4;
                //HistCutShift=4; //5; //4;
                //histcut_count=8; //10;
								//HistoryMoveNb=9;
                //ExtHistDepth=8; //128;
                //ExtHistPrunShift=4;
								//delta_piecedest = 0;  //endgame 1864j
/* noendgame change 86f
#ifdef HISTPRUN

								HistoryMoveNb=9; //8; //start=6 end=7 //8
                //HistPrunShift=1; //1; //1; //1;
                //HistPrunShiftNeg=5; //5=47%  4=50%-6.25%=43.75%

                ExtHistPrunShift=4; //4; //3; //3;
                HistCutShift=5; //4; //3; //4; //4; //4;
                ExtHistDepth=8; //8; //start=6 //6; //6; //7; //8; //7; //8; //64; //99=no endgame //6; //6; //8;
//	              ExtHistCount=11;
//ele2	HistPrunBound=5734; //6554;  //45%=7373  40%=6554, 35%=5734  30%=4915
								histcut_depth=3; //3; //0=no endgame; // 3;
								//histcut_count=10;
                //HistoryDepth=4; //3

#endif
                //fut_depth=1; //2; //1; //3; //2; //2; //0; //1; //2; //0; //2; //0=no futility in board.p_endgame; //2; //1; //2; // 0;
                FUT_MARGIN[1]=VPE*3/2; //FUTPAWN*3; //72; //65; // 65; //70; //70; //60; //70; //65; //70; //50; //60;
                FUT_MARGIN[2]=VPE*3; //FUTPAWN*6; //144; //230; //210; //250; //250; //150;
                //FUT_MARGIN[3]=280;
                //DELTA_MARGIN=FUTPAWN*5/2; //72 //48  //70; //70; //50; //30; //25; //40; //70; //9999; //70; //50; //65; //40;  //65; //45;
								//DELTA_MARGIN=VPE; //FUTPAWN*2;
                //delta_piecedest = 0;	//board.p_endgame==1

                //TordRazorDepth=0; //3 0=no razor in board.p_endgame
                //TordRazorMargin=250;
                //printf("info sizeof HORSE_MOBSCORE=%d\n", sizeof(HORSE_MOBSCORE));
                //fflush(stdout);

                //FUT_MARGIN[1]=VPE*3/2;
                //FUT_MARGIN[2]=VPE*3;
*/
                

                memcpy(HORSE_MOBSCORE, HORSE_MOBSCORE_ENDGAME, sizeof(HORSE_MOBSCORE));

                for (int i=0; i<9; i++)
                {
                	EMPTY_CANN_SCORE[i] /= 2; //4;
                	BOTTOM_CANN_SCORE[i] /= 2; //4;
                }

								// increase pawn ATTKPVAL at endgame
								for (int i=2; i<12; i++)
								{
									ATTKPVAL[i]=50; //1101 5;   //6; 87e
									ATTKPVAL_3[i]=90; //1101 9; 87eed
								}
                //SCORE80_20=10;
                //SCORE6_2=0; //0;

//                DROPDOWN_VAL=50; //45; //35; //35; //40;


                
                // inc board.IMaxTime when endgame first reached
                // if (board.IMaxTime > 1000)
                //if (board.p_endgame==1)
                //{   board.IMaxTime += (board.IMaxTime>>2); //3);
                //    printf("     ***endgame reached, inc IMaxTime=%d\n", board.IMaxTime);
                    //fflush(stdout);
                //}
                //printf("     ***endgame reached\n");
                      
#ifdef DEBUG
                fprintf(out, "endgame reached\n");
#endif
    }
 }   
}

void Clear_Killer()   //1016 also clear counterMoves
{
    memset(g_killer, 0, sizeof(g_killer));
//    memset(g_matekiller, 0, sizeof(g_matekiller)); // [0]) * MAXPLY);   
    memset(counterMoves, 0, sizeof(counterMoves));
} 
  
void Clear_Hist(void)
{
		//for (int i=0; i<(BOARD_SIZE-7); i++)
		//for (int j=0; j<16; j++)
		for (int t=0; t<7; t++)   //1017
		for (int p=0; p<10; p++)
		for (int i=0; i<10; i++)
		for (int j=0; j<9; j++)
		{
#ifdef HISTHIT
        //m_his_table[p][i][j].HistHit = 1; //1;
        //m_his_table[p][i][j].HistTot = 1;
        m_his_table[t][p][(i*16)+j].HistHit = 1; //1;
        m_his_table[t][p][(i*16)+j].HistTot = 1;
#endif
        //m_his_table[p][i][j].HistVal = 0;
        m_his_table[t][p][(i*16)+j].HistVal = 0;
    }
}


void Engine::PreMoveGen()
{	//printf("info PreMoveGen start\n");

    int i, j,SrcSq, DstSq, Index;

    Clear_Killer();
    Clear_Hist();

//    for (i=0; i<46; i++)
//		{
//			PIECE_VALUE[i] = abs(PIECE_VALUE_side[i]);
//		}

    // adjust sq after change BOARD_SIZE from 90 to 160
    //for (SrcSq = 0; SrcSq < BOARD_SIZE-7; SrcSq ++)
    //{
        //for (i=0; i<8; i++)
        //{
            /*
            DstSq = knightmoves[SrcSq][i];
            if ((DstSq >8) && (DstSq <255))
            {	knightmoves[SrcSq][i] = ADJ9_16(DstSq);
            }


            */
       // }

       /*
        //for (i=0; i<5; i++)
        for (i=0; i<4; i++)
        {
            for (j=0; j<16; j++)
            {
                DstSq = g_RookMoves[SrcSq][i][j];
                if ( (DstSq <255))
                    g_RookMoves[SrcSq][i][j] = ADJ9_16(DstSq) + 1;
                else g_RookMoves[SrcSq][i][j] = 0; // -1

            }
        }
*/

    //} // end for SrcSq



    for (SrcSq = 0; SrcSq < BOARD_SIZE-7; SrcSq ++)
    {
        /*
        // 生成象眼数组
        Index = 0;
        for (i = 0; i<5; i++)
        {
        DstSq = g_advelemoves[SrcSq][i];
        if (DstSq == 0) break;
        //if (DstSq > 0)
        {
         g_ElephantEyes[SrcSq][Index] = (SrcSq + DstSq) >>1;
         Index ++;
        }
        }
        */

        // 生成马的着法预生成数组，包括马腿数组
      if (nFile(SrcSq) < 9)
      {
#ifdef HORSECK
        unsigned char knightchkidx[18];
        for (int k=0; k<18; k++)
        {
        	knightchkidx[k]=1;
        }
#endif
        Index = 0;
        for (i=0; i<8; i++)
        {
            //DstSq = knightmoves[SrcSq][i];
            //if (DstSq !=255) //>= 0)
            DstSq = SrcSq + cKnightMoveTab[i];
            //if (c_InBoard[DstSq])
            if (DstSq >=0 && DstSq <160 && nFile(DstSq) < 9)
            {
                g_KnightMoves[SrcSq][Index] = DstSq +1;
                g_HorseLegs[SrcSq][Index] = DstSq + horsdiff[SrcSq - DstSq];
                Index ++;
#ifdef HORSECK
                // generate g_KnightChecks
                for (int j=0; j<18; j++)
                {
                	if (DstSq != kingidxpos[j] && SrcSq != kingidxpos[j])
                	if (horsdiff[DstSq-kingidxpos[j]] !=0) //DstSq is knight check
                	{
                		g_KnightChecks[SrcSq][j][knightchkidx[j]] = DstSq;
                    knightchkidx[j]--;
                  }
                }
#endif
            }
        }
        g_KnightMoves[SrcSq][Index] = 0; //-1; //255; //-1; //0;

/*
				// 生成马腿数组
        //Index = 0;
        for (i=0; i<8; i++)
        {
            DstSq = g_KnightMoves[SrcSq][i];
            if (DstSq !=0)
            {
                DstSq--;
                g_HorseLegs[SrcSq][i] = (DstSq) + horsdiff[SrcSq - DstSq];
                //Index ++;
            }
						else break;
        }
*/
        // 生成兵(卒)的着法预生成数组
/*
        for (i = 0; i < 2; i ++)
        {
            for (j = 0; j < 4; j++)
            {
              DstSq = g_PawnMoves[SrcSq][i][j];
              if (DstSq !=255) //>= 0)
              {
                g_PawnMoves[SrcSq][i][j] = DstSq+1;
              }
              else
              	g_PawnMoves[SrcSq][i][j] = 0;
            }
        }
*/
    } // end if (nFile(SrcSq) < 9)
    }	// end for SrcSq

    //put advele moves in KnightMoves
    for (i=0; i<12; i++)
    {
    	for (j=0; j<2; j++)
    		g_KnightMoves[ele_pos[i]][j+9] = ele_moves[i][j];
    }

    for (i=0; i<4; i++)
    {
    	for (j=0; j<4; j++)
    		g_KnightMoves[advele_center_pos[i]][j+9] = advele_center_moves[i][j];
    }

		for (i=0; i<8; i++)
    {
    		g_KnightMoves[bis_pos[i]][9] = bis_moves[i];
    }
    /*
    //print g_PawnMoves for debug
    FILE *traceout = fopen("trace.txt", "a+"); //w+");   //use append
    fprintf(traceout, "g_PawnMoves\n");

    for (SrcSq=0; SrcSq<BOARD_SIZE-7; SrcSq++)
    {	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");

    		fprintf(traceout, "{");
    		for (i=0; i<2; i++)
    	 {

    	 	fprintf(traceout, "{");
    		for (Index=0; Index<4; Index++)
    		{	fprintf(traceout, "%3d", g_PawnMoves[SrcSq][i][Index]);
    			if (Index<3)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "}");

    		if (i<1)
    				fprintf(traceout, ",");
    	  }
    	 fprintf(traceout, "},");

    }
    fprintf(traceout, "\n");

    fclose(traceout);
    */


    /*
    //print g_KingMoves, g_advelemoves for debug
    FILE *traceout = fopen("trace.txt", "a+"); //w+");   //use append

    	for (SrcSq=0; SrcSq<BOARD_SIZE-7; SrcSq++)
    	{	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");
    		fprintf(traceout, "{");
    		for (Index=0; Index<8; Index++)
    		{	fprintf(traceout, "%3d", g_KingMoves[SrcSq][Index]);
    			if (Index<7)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "},");
    	}
    	fprintf(traceout, "\n");
    	fclose(traceout);
    */
    /*
    	for (SrcSq=0; SrcSq<BOARD_SIZE; SrcSq++)
    	{	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");
    		fprintf(traceout, "{");
    		for (Index=0; Index<8; Index++)
    		{	fprintf(traceout, "%3d", g_advelemoves[SrcSq][Index]);
    			if (Index<7)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "},");
    	}
    	fprintf(traceout, "\n");

    fclose(traceout);
    */
/*
    FILE *traceout = fopen("trace.txt", "a+"); //w+");   //use append
			fprintf(traceout, "g_KnightMoves\n");
    	for (SrcSq=0; SrcSq<BOARD_SIZE-7; SrcSq++)
    	{	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");
    		fprintf(traceout, "{");
    		for (Index=0; Index<16; Index++)
    		{	fprintf(traceout, "%3d", g_KnightMoves[SrcSq][Index]);
    			if (Index<15)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "},");
    	}
    	fprintf(traceout, "\n");

    	fprintf(traceout, "g_HorseLegs\n");
    	for (SrcSq=0; SrcSq<BOARD_SIZE-7; SrcSq++)
    	{	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");
    		fprintf(traceout, "{");
    		for (Index=0; Index<8; Index++)
    		{	fprintf(traceout, "%3d", g_HorseLegs[SrcSq][Index]);
    			if (Index<7)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "},");
    	}
    	fprintf(traceout, "\n");
    fclose(traceout);
*/
/*
FILE *traceout = fopen("trace.txt", "a+"); //w+");   //use append
			fprintf(traceout, "g_KnightChecks\n");
			for (int j=0; j<18; j++)
			{
				fprintf(traceout, "kingposidx = %d\n", j);
    	for (SrcSq=0; SrcSq<BOARD_SIZE-7; SrcSq++)
    	{	if ((SrcSq &15)==0)
    			fprintf(traceout, "\n");
    		fprintf(traceout, "{");
    		for (Index=0; Index<2; Index++)
    		{	fprintf(traceout, "%3d", g_KnightChecks[SrcSq][j][Index]);
    			if (Index<1)
    				fprintf(traceout, ",");
    		}
    		fprintf(traceout, "},");
    	}
    	fprintf(traceout, "\n");
    }
    fclose(traceout);
*/

/// Search::init() is called at startup to initialize various lookup tables

//sf10 void Search::init() {
#define NonPV 0
#define PV 1
// 1013 - Reductions todo //1023 startdo
  for (int imp = 0; imp <= 1; ++imp)
      for (int d = 1; d < 64; ++d)
          for (int mc = 1; mc < 64; ++mc)
          {
              double r = log(d) * log(mc) / 1.95;

              Reductions[NonPV][imp][d][mc] = int(std::round(r));
              Reductions[PV][imp][d][mc] = std::max(Reductions[NonPV][imp][d][mc] - 1, 0);

              // Increase reduction for non-PV nodes when eval is not improving
              if (!imp && r > 1.0)
                Reductions[NonPV][imp][d][mc]++;
          }
//1023
  for (int d = 0; d < 16; ++d)
  {
      //sf10 FutilityMoveCounts[0][d] = int(2.4 + 0.74 * pow(d, 1.78));
      //sf10 FutilityMoveCounts[1][d] = int(5.0 + 1.00 * pow(d, 2.00));
      FutilityMoveCounts[0][d] = int(2.4 + 0.74 * pow(d, 1.78));
      FutilityMoveCounts[1][d] = int(6.0 + 1.00 * pow(d, 2.00));  //1020 5.0->4.0->6.0
  }
//sf10 }


/*
  printf("PV reduction table\n");
  for (int i=1; i<64; i++)
  {
  	for (int j=1; j<64; j++)
  	{
  		printf("%3d", PVRedMatrix[i][j]);
  	}
  	printf("\n");
 }

 printf("NonPV reduction table\n");
  for (int i=1; i<64; i++)
  {
  	for (int j=1; j<64; j++)
  	{
  		printf("%3d", NonPVRedMatrix[i][j]);
  	}
  	printf("\n");
 }
*/

}



//const int HistoryMax = 16384; // 32765; //65534;    // 用于裁剪的历史表的最大值；
//const int HistValMax = 16384; // 32000; //64000;    // 用于启发的历史表的最大值；
#define HistoryMax 32000 //16384
#define HistValMax 32000 //16384 
#define BIGVAL     32700 //1212 must > HistValMax for sort tabval 


// 历史表启发的深度相关的增加值，采用Crafty、Fruit等程序的方案，即深度的平方
//static const unsigned int HistInc[64] = {
//    0,   1,   4,   9,  16,  25,  36,  49,  64,  81, 100, 121, 144, 169, 196, 225,
//  256, 289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729, 784, 841, 900, 961,
//  1024,1089,1156,1225,1296,1369,1444,1521,1600,1681,1764,1849,1936,2025,2116,2209,
//  2304,2401,2500,2601,2704,2809,2916,3025,3136,3249,3364,3481,3600,3721,3844,3969
//};

// for ABDADA, hFlag (2bits) definition
//#define HASH_EXCLUSIVE 1   //use 1 bit of hval instead //1101 not use
//#define HASH_ALPHA  2   
//#define HASH_BETA  1
//#define HASH_PV  3      //2019p for unsigned short in hashstruct  // HASH_ALPHA | HASH_BETA;

#define HASH_BETA  -1  //for short in hashstruct //1126 2019q
#define HASH_ALPHA  0
#define HASH_PV     1        

//const int HASH_LAYERS = 4;   // 置换表的层数
//const int MAX_FRESH = 15;    // 最大新鲜度
//int NULL_DEPTH = 2; //2; //3; //2;    // 空着裁剪的深度
#define HASH_LAYERS 4 //8 //4 //5 //6 //6x10byte //4 //2  //4   // 置换表的层数
//const int MAX_FRESH = 15;    // 最大新鲜度
//#define NULL_DEPTH 3 //= 2; //2; //3; //2;    // 空着裁剪的深度

// 没有命中置换表而返回值的失败值，必须在-MATE_VALUE和MATE_VALUE以外
//1101 const int UNKNOWN_VALUE = INF + 1; //MATE_VALUE + 1;

//1101 #define BAN_VALUE 1948 //(INF-100) //1900;
//1101 #define WIN_VALUE 1848 //1792 //(INF-256) //1800;
//1101 #define VALUE_KNOWN_WIN 1848  //=WIN_VALUE (INF-200)



#ifdef DEBUG
FILE *out = fopen("search.txt", "a+"); //w+");   //use append
#endif

bool operator<(const MoveTabStruct& a, const MoveTabStruct& b)  // for std::sort
{
//    return a.score < b.score;
return a.tabval > b.tabval;   //descending
}

// selection sort

#ifdef __GNUC__
inline
#else  
__forceinline
#endif
static int GetNextMove(int n, int size, MoveTabStruct movetab[])
{
short nval = movetab[n].tabval;
int q = n;
for (int i=n+1; i<size; i++)
	{
		if (movetab[i].tabval > nval)
		{
			nval = movetab[i].tabval;
			q = i;
		}
	}
		int tmp = movetab[q].tabentry;
		if (q != n)
		{
			movetab[q].tabentry = movetab[n].tabentry;
			movetab[n].tabentry = tmp;
		}

	return tmp;
}

//int printed=0;

/*
// 过滤禁止着法
int Engine::IsBanMove(int mv) {
  int i;
  for (i = 0; i < nBanMoves; i ++) {
    if (mv == wmvBanList[i]) {
      return 1;
    }
  }
  return 0;
}
*/
//static int FUTILITY_MARGIN = 50; //40;  //40

// pawn value=25, margin1=4p, margin2=12p
// toga 1.2.1a margin1=1p, margin2=3p, margin3(not used)=9.5p
// glaurung    margin1=2p, margin2=6p
// eychessu    margin1=1b, margin2=3b, margin3(not used)=7b where b=ADVISOR=50
// crafty 20.14 margin1=(ADVISOR+1)/2 margin2=(queen+1)/2


//unsigned int nEvalHits=0;
//unsigned int nEvalRecs=0;
//unsigned int nPawnHits=0;
//unsigned int nPawnMiss=0;
//unsigned int nThreat[12]={0,0,0,0,0,0,0,0,0,0,0,0}; //, {0,0,0,0,0,0,0,0,0,0,0,0}};
//unsigned int nThreat_justendgame=0;
//unsigned int nThreat_deependgame=0;
//unsigned int nHistCuts=0;

int ext_drawval =5; //aft clop
//extern int ext_drawval;

extern int p_movenum;
extern int p_bookfound;
extern int p_feedback_move;
//1213 static int ponder_move=0;
//1213 static int respmove_to_ponder=0;
//static int prev_boardsq[34]={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0};
//static unsigned char prev_piece[BOARD_SIZE-7];
static unsigned char prev_boardsq[34];
extern int p_nBanmove;
extern int p_banmove[16];
//static const int ADVISOR_POS[5]={3,5,13,21,23};
//static const int ELEPHAN_POS[7]={2,6,18,22,26,38,42};
// move to lazy smp id loop

//static const int WINDOW[64]=   //60; //30 //40 //30 //40
//{110,105,100,95,90,85,80,75,70,65,60, 55,50,45,40,35,35,35,35,35,35, 35,35,35,35,35,35,35,35,35,
// 25,25,25,25,25,25,25,25,25,25, 25,25,25,25,25,25,25,25,25,25, 25,25,25,25,25,25,25,25,25,25, 25,25,25,25};
//{80,70,70,65,60,50,45,45,40,40, 35,35,30,30,25,25,25,25,25,25, 25,25,25,25,25,25,25,25,25,25,
//25,25,25,25,25,25,25,25,25,25, 25,25,25,25,25,25,25,25,25,25, 25,25,25,25,25,25,25,25,25,25, 25,25,25,25};

//static int DROPDOWN_VAL=45; //40; //45; //45; //35; //35; //40; //[2] = {40,40};
static const int CENTRAL_PAWN[2] = {52, 100}; //159-52-7

#ifdef PERFT
static const uint64 STD_PERFT[7] = {
1ULL,
44ULL,
1920ULL,
79666ULL,
3290240ULL,
133312995ULL,
5392831844ULL
};
#endif // end PERFT
void Engine::printf_info_nps(int best, Board &board)
{
//printf("info time %d nodes %d\n", clock() - board.m_startime, board.m_nodes);
                        int TimeSpan = (int)(GetTime() - board.m_startime); //(clock() - board.m_startime) ;
                        //if (TimeSpan==0) TimeSpan=1;
                        //unsigned int u32board.m_nodes = board.m_nodes;
                        int nps = int((double)board.m_nodes * 1000 / (double)(TimeSpan==0 ? 1:TimeSpan));
                        //printf("info time %d nodes %d nps %d\n", TimeSpan, board.m_nodes, board.m_nodes/(TimeSpan+1));
                        //printf("info nps %d time %d nodes %d \n", board.m_nodes/(TimeSpan+1), TimeSpan, board.m_nodes);
                        char str[5];
    MoveStruct tempmove;
    tempmove.move=board.m_bestmove;
                    
    //printf("info depth %d score %d time %d nodes %d nps %d pv %s\n", m_depth-1, best, TimeSpan, u32board.m_nodes, nps,
//    printf("info depth %d score %d time %d nodes %llu nps %d pv %s\n", m_depth-1, best, TimeSpan, board.m_nodes, nps,
	printf("info depth %d score %d time %d nodes %llu nps %d pv %s\n", 0, best, TimeSpan, board.m_nodes, nps,
    MoveStr(str, tempmove.from, tempmove.dest) );
                        fflush(stdout);
}

//jmp_buf setjmpbuf;
//bool jmp_ok;
int Engine::searchRoot()
{
// copy from ext_p in eychessu.cpp to board.p_
    board.IMaxTime = ext_IMaxTime;
//    printf("*** board.IMaxTime=%d, ext_IMaxTime=%d\n", board.IMaxTime, ext_IMaxTime);
    //if (ext_p_endgame == 1)
    //	board.p_endgame = 1;
    //else    		
      board.p_endgame = ext_p_endgame;
//    if (ext_p_endgame!=0 && ext_p_endgame !=1)
// 	  printf("**ext_p_endgame=%d\n", ext_p_endgame);
//smp    int best; //, size; //, j; //RootAlpha, RootBeta, size, j; //, piecefrom; //j,banned;
//	int alpha, beta;
 //smp   MoveStruct tempmove;
    char charmove[5];


		board.Compress_index();

		board.bitpiece=0;
		memset(board.piececnt,0,sizeof(board.piececnt));

		memset(board.bitattk,0,sizeof(board.bitattk));

    //for (i = 0; i < 10; i ++) {
    //	board.wBitRanks[i] = 0;
    //}
    //for (i = 0; i < 9; i ++) {
    //	board.wBitFiles[i] = 0;
    //}
    memset(board.wBitRanks,0,sizeof(board.wBitRanks));
    memset(board.wBitFiles,0,sizeof(board.wBitFiles));


for (int i=2; i<34; i++)
    {
        int sq = board.boardsq[i];
        //if (sq >=0 )
        if (NOTSQEMPTY(sq))
        {
            //if (i>=B_HORSE && i<=R_ROOK)
            //if (i>=20 && i<=31)
            //    side_power_piece[(i&1)]++;
            //bitpiece, board.p_endgame not yet determine
            //pointsum += pointtable[sq][PIECE_IDX(i)][board.p_endgame]; //[i &29];
            board.piececnt[i &61]++;
            if (i<B_KING) // not king
            {
                board.bitpiece ^= (1 << i); //BITPIECE_MASK[i];

                board.bitattk[ATTKAREA[sq]] ^= (1 << i);

                //bitpiece ^= BITPIECE_MASK[i];
                //_bittestandcomplement(&bitpiece, i);
            }
            board.wBitRanks[nRank(sq)] ^= PreGen.wBitRankMask[sq];
            board.wBitFiles[nFile(sq)] ^= PreGen.wBitFileMask[sq];

        }
    }

// use ini_NBOOK for clop 
//int parm_bookfound = ini_NBOOK;
//printf("*** parm_bookfound = %d\n", parm_bookfound);
//fflush(stdout);
int parm_bookfound = 23; //23(clop) 40; //6; //24; //12; //10 //30 20
#ifdef PERFT
#else
    unsigned char book_boardsq[34];   //20190912 int
    int bookmove;

		//int old_IMaxTime = board.IMaxTime;
    //don't know why, but has to set m_side to black meaning computer's turn
    //board.m_side = IComSide;
int val=0;
    // search book using board.boardsq
    //1892g if (board.IMaxTime >= 5000)  //1891c - search book only if enough time
    //no need? AdjustEndgame(board);  //set board.p_endgame for pointsum recal //1892k move to here
    
    //printf("*** p_bookfound=%d, p_endgame=%d, IMaxTime=%d\n", p_bookfound, board.p_endgame,board.IMaxTime);
    //fflush(stdout);
    if (board.p_endgame == 0 && board.IMaxTime >= 1000) //1892k	
    if (p_bookfound<parm_bookfound) //30 20
    {
        board.m_startime=GetTime();
        for (int i=0; i<34; i++)
        {
            val=board.boardsq[i];
			if (NOTSQEMPTY(val))
				book_boardsq[i] = (nRank(val) * 9) + nFile(val);
			else
				book_boardsq[i] = SQ_EMPTY;   //20190912 -1
        }
        //bookmove = srchboob(board.boardsq, 1 - IComSide); //IComSide);   //IComSide=0 COMPUTER SIDE=BLACK
        //bookmove = srchboob(book_boardsq, 1 - IComSide);
        bookmove = srchboob(book_boardsq, BLACK); //1 - board.m_side);
//20190915        if (bookmove==0) 
//20190915        	 printf("*** bookmove=0 after srchboob\n");
        if (bookmove != 0)
        {
			MoveStruct tempmove;
			//board.m_bestmove = ((bookmove>>7) <<8) + (bookmove&127);
            tempmove.from = bookmove>>7;
            tempmove.dest = bookmove&127;

            tempmove.from = ADJ9_16(tempmove.from);
            //verify bookmove - must be same side to move (may be diff due to transposition)
            if ((board.piece[tempmove.from]&1) == board.m_side)
            //printf("from=%d, piece=%d, m_side=%d\n", tempmove.from, board.piece[tempmove.from], board.m_side);
            {
            	tempmove.dest = ADJ9_16(tempmove.dest);
            	//verify bookmove
            	if (board.LegalKiller(tempmove))
            	{
           				board.m_bestmove = tempmove.move;
            //FILE *traceout = fopen("trace.txt", "a+"); //w+");   //use append
            //fprintf(traceout, "%s%d %s%d\n", "board.m_bestmove.from=", board.m_bestmove.from, "board.m_bestmove.dest=", board.m_bestmove.dest);
            //fflush(traceout);
            //fclose(traceout);

//20190915            printf("*** book move found\n");
//20190915            printf("*** from=%d, dest=%d\n", tempmove.from, tempmove.dest);
//20190915            fflush(stdout);
            
            			p_bookfound++;
            			//printf("info book move found\n");
            			//fflush(stdout);
//            			printf_info_nps(0);
            			return board.m_bestmove;
          		}
          		printf("*** bookmove error\n");
          	}
        }

        //fprintf(traceout, "%s\n", "bookfound=0");
        //fflush(traceout);

        //printf("info Leave book\n");
        //fflush(stdout);
//        printf_info_nps(0);
        //
        // inc board.IMaxTime when leaving book
        if (p_bookfound!=0)
        {	p_bookfound=parm_bookfound; //20 0; 
        	//1021 board.IMaxTime += (board.IMaxTime>>4);
        	//1021 printf("*** inc board.IMaxTime to %d when leave book\n", board.IMaxTime);
          //1021 fflush(stdout);
        }

       
    }
    
     if (p_movenum >=parm_bookfound)  //30 20
        	p_bookfound=parm_bookfound; //30 20
        	
#endif //end PERFT
     if (board.bitpiece == 0xfffffffc) 
     { 
          board.IMaxTime -= (board.IMaxTime>>3);
        	//printf("*** dec board.IMaxTime to %d if opening no capture\n", board.IMaxTime);
          //fflush(stdout);
     }

    board.m_startime=GetTime(); //clock();
    board.m_nodes=0;
    board.m_timeout=0;

    board.ply=0;


  	//AdjustEndgame(board);  //set board.p_endgame for pointsum recal
  	//88c - ClearHash if endgame
  	//if (board.p_endgame)
  	//	ClearHash();

#ifdef PREEVAL
//preEval pointtable to adjust bis/ele val
//  	memcpy(pointtable[board.p_endgame][2], pointtableAdvEleB[board.p_endgame], sizeof(pointtableAdvEleB[board.p_endgame]));
//  	memcpy(&pointtable[board.p_endgame][3][BOARD_SIZE/2], pointtableAdvEleR[board.p_endgame], sizeof(pointtableAdvEleR[board.p_endgame]));
/*
for (int i=0; i<9; i++)
{
for (int j=0; j<5; j++)
{
	 pointtable[2][i][j] = pointtableAdvEleB[i][j];
	 pointtable[3][i][9-j] = pointtableAdvEleR[i][j];
}

for (int j=0; j<7; j++)
{
	 pointtable[0][i][j+3] = pointtablePawnB[i][j];
	 pointtable[1][i][9-(j+3)] = pointtablePawnR[i][j];
}
}
*/
memcpy(&(pointtable[2][0][0]), pointtableAdvEleB, sizeof(pointtableAdvEleB));
memcpy(&(pointtable[3][5][0]), pointtableAdvEleR, sizeof(pointtableAdvEleR));
memcpy(&(pointtable[0][3][0]), pointtablePawnB, sizeof(pointtablePawnB));
memcpy(&(pointtable[1][0][0]), pointtablePawnR, sizeof(pointtablePawnR));
//1015
    unsigned int bitattkB, bitattkR;
    int nSimpleB, nSimpleR, nattkR, nattkB, p;
  	bitattkB = (bitattk[0] | bitattk[2] | bitattk[4]);
  	bitattkR = (bitattk[1] | bitattk[3] | bitattk[5]);
  	
#ifndef _WIN64
  if (POPCNT_CPU)
#endif  	
  { 	  	
  	nattkR = PCbitCount(bitattkB & 0xAAA00AA8)
  	  //+ bitCount(bitattkB & 0xA0A00000)	//RH=2, CP=1
  	  //+ bitCountLSB((bitattkB & 0xA0A00000)>>16)	//RH=2, CP=1
  	  + PCbitCountMSB((bitattkB & 0xA0A00000))	//RH=2, CP=1
  	  + PCbitCountRookR(bitattk[6] | bitattk[8]);
  	
  	nattkB = PCbitCount(bitattkR & 0x55500554)
  	  //+ bitCount(bitattkR & 0x50500000)	//RH=2, CP=1
  	  //+ bitCountLSB((bitattkR & 0x50500000)>>16)	//RH=2, CP=1
  	  + PCbitCountMSB((bitattkR & 0x50500000))	//RH=2, CP=1
  	  + PCbitCountRookB(bitattk[7] | bitattk[9]);
/*
  	// 如果本方轻子数比对方多，那么每多一个轻子(车算2个轻子)威胁值加2。威胁值最多不超过8。
  nWhiteSimpleValue = PopCnt16(lppos->wBitPiece[0] & ROOK_BITPIECE) * 2 + PopCnt16(lppos->wBitPiece[0] & (HORSE_BITPIECE | CANNON_BITPIECE));
  nBlackSimpleValue = PopCnt16(lppos->wBitPiece[1] & ROOK_BITPIECE) * 2 + PopCnt16(lppos->wBitPiece[1] & (HORSE_BITPIECE | CANNON_BITPIECE));
  if (nWhiteSimpleValue > nBlackSimpleValue) {
    nWhiteAttacks += (nWhiteSimpleValue - nBlackSimpleValue) * 2;
  } else {
    nBlackAttacks += (nBlackSimpleValue - nWhiteSimpleValue) * 2;
  }
*/
  	nSimpleB = PCbitCountRookB(bitpiece) *2 + PCbitCountCanHorB(bitpiece);
  	nSimpleR = PCbitCountRookR(bitpiece) *2 + PCbitCountCanHorR(bitpiece);
  }
#ifndef _WIN64
  else
	{
		nattkR = bitCount(bitattkB & 0xAAA00AA8)
  	  //+ bitCount(bitattkB & 0xA0A00000)	//RH=2, CP=1
  	  //+ bitCountLSB((bitattkB & 0xA0A00000)>>16)	//RH=2, CP=1
  	  + bitCountMSB((bitattkB & 0xA0A00000))	//RH=2, CP=1
  	  + bitCountRookR(bitattk[6] | bitattk[8]);
  	
  	nattkB = bitCount(bitattkR & 0x55500554)
  	  //+ bitCount(bitattkR & 0x50500000)	//RH=2, CP=1
  	  //+ bitCountLSB((bitattkR & 0x50500000)>>16)	//RH=2, CP=1
  	  + bitCountMSB((bitattkR & 0x50500000))	//RH=2, CP=1
  	  + bitCountRookB(bitattk[7] | bitattk[9]);
  	  
  	nSimpleB = bitCountRookB(bitpiece) *2 + bitCountCanHorB(bitpiece);
  	nSimpleR = bitCountRookR(bitpiece) *2 + bitCountCanHorR(bitpiece);
	}	
#endif  	
  	
  	
  	if (nSimpleB > nSimpleR)
  		nattkB += (nSimpleB - nSimpleR) * 2;
  	else
  		nattkR += (nSimpleR - nSimpleB) * 2;
  	if (nattkR >=8) nattkR=8;
  	if (nattkB >=8) nattkB=8;

  	//if ( ((bitattk[0] | bitattk[2] | bitattk[4] ) & 0xAAA00AA8) ==0)
  	{
  		for (int n=0; n<9; n++)
  		for (int m=0; m<5; m++)
  		{
  			p=pointtable[2][m][n];
  			p = p*(8 + nattkR)/8;
  			pointtable[2][m][n]=p;

  			p=pointtable[3][9-m][n];
  			p = p*(8 + nattkB)/8;
  			pointtable[3][9-m][n]=p;
  		}
  	}

//adjust pointtablepawn
for (int n=0; n<9; n++)
  		for (int m=0; m<7; m++)
  		{
  			p=pointtable[0][m+3][n];
  			p = p*(8 + nattkB)/8;
  			pointtable[0][m+3][n]=p;

  			p=pointtable[1][6-m][n]; //9-(m+3)
  			p = p*(8 + nattkR)/8;
  			pointtable[1][6-m][n]=p;
  		}
#endif

//if only pawns and opp has cannon, adjust pointtablepawn not to move forward
if ((board.bitpiece & 0x55500000)==0 && (board.bitpiece & 0x0A000000)!=0)
{

	for (int j=7; j<10; j++)
  		for (int k=0; k<9; k++)
  		{
  			//pointtable[j*16+k][0][board.p_endgame] /=2;
  			pointtable[0][j][k] /=2;
  		}


}
if ((board.bitpiece & 0xAAA00000)==0 && (board.bitpiece & 0x05000000)!=0)
{
	for (int j=0; j<3; j++)
  		for (int k=0; k<9; k++)
  		{
  			//pointtable[j*16+k][1][board.p_endgame] /=2;
  			pointtable[1][j][k] /=2;
  		}
}
// 调整不受威胁方少掉的仕(士)相(象)分值
#ifdef PREEVAL
board.pointsum = (nattkB - nattkR) * 15;
#else
board.pointsum=0;
#endif

for (int i=2; i<34; i++) //i+=2)
    {
        int sq = board.boardsq[i];
        if (NOTSQEMPTY(sq))
        {
            board.pointsum += pointtable[PIECE_IDX(i)][nRank(sq)][nFile(sq)];
        }
    }
/*
for (i=3; i<34; i+=2)
    {
        int sq = board.boardsq[i];
        if (NOTSQEMPTY(sq))
        {
            pointsum -= pointtable[PIECE_IDX(i)][nRank(sq)][nFile(sq)];
        }
    }
*/
/*
//debug
    printf("info nattkB=%d\n", nattkB);
    printf("info nattkR=%d\n", nattkR);
    printf("info pointtable: endgame=%d\n", board.p_endgame);
    for (int i=0; i<4; i++)
    {	printf("i=%d\n", i);
    	for (int m=0; m<10; m++)
      {      for (int n=0; n<9;  n++)
            {
                int j=(m*16) + n;
    						printf("%5d", pointtable[j][i][board.p_endgame]);
    				}
    				printf("\n");
  		}
    }
printf("info pointsum=%d\n", pointsum);
*/



//七、初始化一些有用的变量
// 这些变量用于测试搜索树性能的各种参数
//    int old_IMaxTime = board.IMaxTime;

//	nHashMoves = nHashCuts = 0;

//	nTreeHashHit = nLeafHashHit = 0;

//debug
#ifdef DEBUG
    nTreeNodes = nLeafNodes = 0;
//	nBetaNodes = 0;
//	nHashCuts = 0;
//	nHashMoves = 0;
    nQuiescNodes = 0;
    nFutility = nExtFutility = 0;
    nHistPrunNodes = 0;
    nExtHistPrunNodes = 0;
//	nHistPrunVers = 0;
#endif
//	nEvalHits = nEvalRecs = 0;
//	nPawnHits = nPawnMiss = 0;
//

//	for (int j=0; j<12; j++)
//		nThreat[j]=0;
// nThreat_justendgame=0;
// nThreat_deependgame=0;
//
//	nHistCuts=0;
//	lazya = lazyb = 0;
//	nBetaMoveSum = nBetaCutAt1 = 0;
//	nNullMoveNodes = nNullMoveCuts = 0;

    //search_stack_t sstack[MAXPLY];
    //Clear_Killer(); //1891c - same with Clear_Hist
#ifdef PERFT
/*
#include <tchar.h>
#include <windows.h>
typedef union _LARGE_INTEGER
       {
           struct
           {
              int LowPart ;// 4字节整型数
              int HighPart;// 4字节整型数
           };
           long long QuadPart ;// 8字节整型数

        }LARGE_INTEGER ;
*/
//board.m_startime = GetTime();

for (int d=1; d<=6; d++)
{
//	LARGE_INTEGER ntime1,ntime2,freq;
//QueryPerformanceFrequency(&freq);
//QueryPerformanceCounter(&ntime1);
//uint64 nPerft = Perft(d);
//QueryPerformanceCounter(&ntime2);
//get ntime in millisecs
//TimeSpan = (ntime2.QuadPart-ntime1.QuadPart)/(freq.QuadPart/1000);
  board.m_startime = GetTime();
	printf("depth=%5d  ",d);
	fflush(stdout);
	//uint64 nPerft = Perft(d);
	printf("Perft(%d)=%16llu  Std Perft=%16llu  ", d, Perft(d), STD_PERFT[d]);
  printf("time=%d \n", (int)(GetTime() - board.m_startime));
  fflush(stdout);
}
return 0;
#endif //end PERFT

    // init_node

//debug

		//printf("m_hisindex=%d\n", m_hisindex);
		//for (int i=0; i<=m_hisindex; i++)
		//{	printf("i=%d Chk=%d\n", i, m_hisrecord[i].htab.Chk);
// printed=1;}

    //nCheckEvasions = 0;
    //nZugzwang = 0;
    //int incheck=board.IsInCheck(board.m_side );
    //m_hisrecord[m_hisindex].Chk =incheck;
    //int incheck=0;
    if (board.m_hisindex > 0)
        board.incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
    else
        //incheck=board.IsInCheck(board.m_side, 0);	//not singlechk
        board.incheck=board.IsInCheck<0>(board.m_side);	//not singlechk
        //m_hisrecord[0].htab.Chk = incheck;

    //printf("incheck=%d\n", incheck);
 //   int newdepth; //depth
    // 单方最多合理的走法111种： 将与单士(2+4=6), 双象(4+2＝6), 双车(17×2＝34), 双炮(17*2=34), 双马(8*2=16), 五个兵(3*5=15)
    //MoveStruct table[111];
    //int  tabval[111];
    //MoveTabStruct movetab[111];  //smp
    MoveTabStruct ncapmovetab[64];
    long ncapsize; //=0;
    //int max_root_nodes;
    board.nBanMoves = 0; 
    if (board.incheck)
    {
    	//1011 allow banmoves  //for (int k=0; k<p_nBanmove; k++)
      //1011                //   p_banmove[k] = 0;	//if incheck, not consider banmove
        //printf("info Before GenChkEvasion\n");
        //fflush(stdout);
        //size=board.GenChkEvasion(movetab, incheck);
        //if (incheck < 8)
        	board.size=board.GenChkEvasCap(&board.movetab[0], board.incheck);
        //else
        //	size=board.GenChkEvasRookCann(&movetab[0], incheck &7);


        //memcpy(&movetab[size], &ncapmovetab[0], ncapsize * 4);
        //size += ncapsize;
        ncapsize=board.GenChkEvasNCap(&board.movetab[board.size], board.incheck);
        //memcpy(&movetab[size], &ncapmovetab[0], ncapsize * 4);
        board.size += ncapsize;
        //printf("info Root GenChkEvasion OK\n");
        //fflush(stdout);
    }
    else
    {	//printf("info Before Gen\n");
        //fflush(stdout);

        //size=(board.m_side ? board.GenCap<1>(&movetab[0], &ncapmovetab[0], ncapsize)
        //: board.GenCap<0>(&movetab[0], &ncapmovetab[0], ncapsize));
        board.size=board.GenCap(&board.movetab[0], &ncapmovetab[0], ncapsize);
    
        memcpy(&board.movetab[board.size], &ncapmovetab[0], ncapsize * 4);
        board.size += ncapsize;

//        size=board.GenCapQS(&movetab[0]);
        ncapsize=board.GenNonCap(&board.movetab[board.size], 0);
        board.size += ncapsize;

        //ncapsize=board.GenNonCapPBEK(&movetab[size]);
        //size += ncapsize;

        //printf("info Root Gen OK\n");
        //fflush(stdout);
    }
//----------------------------- 
    MoveStruct tempmove;
    int best=-INF;
    board.m_bestmove = board.movetab[0].table.move; //init 
    for (int i=0;i<board.size;++i)
    {    	
    		tempmove.move = board.movetab[i].table.move;
    		int isBanmove = 0;
    		for (int k=0; k<p_nBanmove; k++)
    		{
        if (tempmove.move == p_banmove[k]
        //	 && !incheck && board.piece[tempmove.dest] < B_KING
        	)
          {

            board.movetab[i].tabval = -BIGVAL;
            board.nBanMoves ++;
            //1011 p_banmove[k] = 0;	//reset banmove
            isBanmove = 1;
            break;
          }
        }        
        if (isBanmove)
        	continue;  // next i
        	   
                //if ( makemove(tempmove) < 0 )
                if ( board.makemove(tempmove, 1) < 0) //, 1) < 0 )
                {
                    //wmvBanList[nBanMoves] = table[i].move;
                    board.movetab[i].tabval = -BIGVAL;
                    board.nBanMoves ++;
                }
                else if (  board.piece[tempmove.dest] < B_KING
//1011 fixbanmove              // && !board.incheck 
                	&& (abs(board.checkloop(3)) > DRAWVALUE) // ||  checkloop(3) != 0)
                	//&& (abs(checkloop(3)) > DRAWVALUE) // ||  checkloop(3) != 0)
                	)
                {
                    board.movetab[i].tabval = -BIGVAL;
                    board.nBanMoves ++;
                    board.unmakemove();
                }
                else
                {
                    board.movetab[i].tabval = -quiesCheck<PV>(board, -INF, INF, 0); //1210  
                    //board.movetab[i].tabval = -Evalscore(board); //1210                                        
                    board.unmakemove();                    
                    if (board.movetab[i].tabval > best)
                    {
                    	best = board.movetab[i].tabval;
                    	board.m_bestmove = board.movetab[i].table.move;
                    }
                    long long t_remain = board.IMaxTime - (GetTime()-board.m_startime);
    				        if (t_remain <= 0) 
    				        {	                     
                  	   board.m_timeout= 1; //stop   
                  	   printf("     *** panic! stopping at searchroot QS\n");
			                 fflush(stdout);			
			                 return board.m_bestmove;                	
                    	 
                    }
                }
    } //end for (i=

    std::sort(board.movetab, board.movetab + board.size);
    board.m_bestmove = board.movetab[0].table.move;
    com2char(charmove, board.movetab[0].table.from, board.movetab[0].table.dest );
    printf("info depth 0 score %d pv %s\n", best, charmove);	        
#ifdef PRTBOARD   

	  print_board(best);
    printf("\nAfter gen and sort rootmoves: incheck=%d, nBanMoves=%d, nRootMoves=%d", board.incheck, board.nBanMoves, board.size);
    printf("\nRoot moves: ");
    for (int i=0; i<board.size; i++)
    {
        com2char(charmove, board.movetab[i].table.from, board.movetab[i].table.dest );
        //fprintf(traceout, " %s", charmove);
        printf("  %s", charmove);
    }    
    printf("\n");
    fflush(stdout);
    
    printf("Root tbval: ");
    for (int i=0; i<board.size; i++)
    {
        printf("%6d", board.movetab[i].tabval);
        //printf("%6d", root_nodes[i]);
    }
    printf("\n"); 
    fflush(stdout);

#endif   
    board.size = board.size - board.nBanMoves;   //remove banmoves and illegal      
    
    board.m_nodes=0;
    board.root_depth=1;  //init for lazy smp 

//lazy smp - init from i=1, leave i=0 in lazy smp loop   
    for (int i=1;i<board.size;++i)
    {
        	board.movetab[i].tabval = -BIGVAL;
    }
//--------------------------------------
	        Board* spboard;     	        
//	         spboard = &board;              
          //Board spboardref[7];  
					//spboardref = board;
//					A myClass02( tempClass ); //copy constructor
           //Board spboardref(board);  // copy board to spboardref
					//spboard = &spboardref; 
//if (board.IMaxTime < 1000)
//{
//	IMaxDepth = 8;	
//	NCORE = 1; //do not start smp if short in time
//}
//else
//{
	IMaxDepth = 64;
//}		
	
//NCORE = 2;  // lazy smp --- testing stockfish thd skipping pattern
//#ifdef PRTBOARD
//  NCORE = 1; //for debug
//#endif
#ifdef THDINFO
printf("     num of threads = %d\n", NCORE);
fflush(stdout);  
#endif

std::function<int()> bind_search[7]; //for up to NCORE=8
std::future<int> f1[7];
//std::thread f1[7];
Board spboardref[7]; 	// use assignment operator
int thd_bestmove[7];
	
for (int k=0; k<NCORE-1; k++)
{ 
	spboardref[k] = board;         // use assignment operator 	
	spboard = &spboardref[k]; 
	
	bind_search[k] = std::bind(    
	  &Engine:: Lazy_smp_ID_loop, this,	
	  	k+1, spboard 
	  	);
  f1[k] = std::async(std::launch::async, bind_search[k]);
//    f1[k] = std::thread(bind_search[k]);
}

	  spboard = &board; //main thread 0 points to &board
	
    //spboard = &board; //main thread 0 points to &board
    board.m_bestmove = Lazy_smp_ID_loop(0, spboard);

    int completedDepth = board.root_depth;
    board.root_depth = -1;  // signal end of mainthread

    for (int k = 0; k < NCORE - 1; k++)
    { 
       thd_bestmove[k] = f1[k].get(); 
    //f1[k].join();
  
    // sf: Check if there are threads with a better score than main thread
    // for (Thread* th : Threads)
    //      if (   th->completedDepth > bestThread->completedDepth
    //          && th->rootMoves[0].score > bestThread->rootMoves[0].score)
    //          bestThread = th;
    
       if (spboardref[k].root_depth > completedDepth
       && spboardref[k].movetab[0].tabval > board.movetab[0].tabval  
        	)
       {
#ifdef THDINFO
      printf("     **thd %d root_depth= %d > completedDepth = %d && tabval= %d > board.tabval= %d\n", 
         k+1, spboardref[k].root_depth, completedDepth, spboardref[k].movetab[0].tabval, board.movetab[0].tabval);
      fflush(stdout);       
#endif    	    	
    	    completedDepth = spboardref[k].root_depth;
    	    board.movetab[0].tabval = spboardref[k].movetab[0].tabval;
    	    board.m_bestmove = spboardref[k].m_bestmove;
       }	
    } 
 
   return board.m_bestmove;
} //end of searchroot    
 
//---temp undo lazy smp    
int Engine::Lazy_smp_ID_loop(unsigned int idx, Board* spboard
		) //, MoveTabStruct movetab[])  // idx=thread# 0=mainthread

{
//------------------------------------------------
	  // copy idx to board.thd_id for tracing
	  spboard->thd_id = idx;
	  bool mainThread = (idx==0);      
 //   int pv_i; // = -1;
    //int m_depth; // move to board.h for smp
    int m_depth, alpha, beta, val, best, newdepth;
    MoveStruct tempmove;
    char charmove[5];
    unsigned long long start_nodes;
    int TimeSpan, nps;
    int moveCount;
    int old_IMaxTime = spboard->IMaxTime;    
    
//    int pv_tabval;  
#ifdef PRTBOARD   
if (mainThread) { 
//	  print_board(best);
    printf("\nStart of smp_ID_loop:");
    printf("\nRoot moves: ");
    for (int i=0; i<spboard->size; i++)
    {
        com2char(charmove, spboard->movetab[i].table.from, spboard->movetab[i].table.dest );
        //fprintf(traceout, " %s", charmove);
        printf("  %s", charmove);
    }    
    printf("\n");
    fflush(stdout);
    
    printf("Root tbval: ");
    for (int i=0; i<spboard->size; i++)
    {
        printf("%6d", spboard->movetab[i].tabval);
        //printf("%6d", root_nodes[i]);
    }
    printf("\n"); 
    fflush(stdout);
} //mainThread 
#endif        
    
int delta = 19; //asp_window
int lastbest = spboard->movetab[0].tabval;   //1231 init to first root tabval //INF;
//int ValueByIteration[MAXDEPTH];
//    ValueByIteration[0] = spboard->movetab[0].tabval;
int asp_save_ab;

    //Iterative deepening at searchroot
    //for(depth=start_depth; depth<IMaxDepth; ++depth) //depth=1 has been qsearch??

//lazy smp - limit to IMAXDepth to 3 if IMaxTime < 10 
//    if (board.IMaxTime < 10) IMaxDepth = 3;
//    printf("     **thd %d IMaxDepth=%d\n", idx, IMaxDepth);
//    fflush(stdout);	 
    best = delta = alpha = -INF;    //sf10 //1231 init bef m_depth loop
    beta = INF;	  
    move_t root_pv[MAX_PLY+1]; //1210 MAX_PLY=256 //smp, now local in thd 
    memset(root_pv, 0, sizeof(root_pv));  //1210                    
    for (m_depth=1; m_depth<IMaxDepth; ++m_depth) //depth=1 has been qsearch??
    {
   

    	    // Sizes and phases of the skip-blocks, used for distributing search depths across the threads
  constexpr int SkipSize[]  = { 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };
  constexpr int SkipPhase[] = { 0, 1, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7 };
  
// Distribute search depths across the helper threads
//                                depth:  1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8
// stockfish 10 skipping pattern: thd 1:  s   s   s   s   s   s   s   
//                                thd 2:    s   s   s   s   s   s   s      
//                                thd 3:    s s     s s     s s     s s
//                                thd 4:  s s     s s     s s     s s
//                                thd 5:  s     s s     s s     s s     s s   
//                                thd 6:      s s
//                                thd 7:      s s s   
      if (idx > 0)
      {
      	  //if (m_depth == 1)
      	  //{
          //	for (int s=1; s<=idx; s++) printf("     ");              	
          //        printf("\n***helper thd %d depth %d ...skipping, mainthd completed depth = %d\n", idx, m_depth, board.root_depth);  
          //        fflush(stdout);    
          //    continue;  // Retry with an incremented rootDepth
          //}
          int i = (idx - 1) % 20;
          if (((m_depth + SkipPhase[i]) / SkipSize[i]) % 2)
          {
          	//printf("\n");
          	//for (int s=1; s<=idx; s++) printf("**");              	
            //      printf("helper thd %d depth %d ...skipping, mainthd completed depth = %d\n", idx, m_depth, board.root_depth);  
            //      fflush(stdout);    
              continue;  // Retry with an incremented rootDepth
          }
      }
         	
        /* 根结点搜索例程，和完全搜索的区别有以下几点：
         *
         * 1. 省略无害裁剪(也不获取置换表着法)；
         * 2. 不使用空着裁剪；
         * 3. 选择性延伸只使用将军延伸；
         * 4. 过滤掉禁止着法；
         * 5. 搜索到最佳着法时要做很多处理(包括记录主要变例、结点排序等)；
         * 6. 不更新历史表和杀手着法表。
         */
      // Save the last iteration's scores before first PV line is searched and
      // all the move scores except the (new) PV are set to -VALUE_INFINITE.
      //sf10 for (RootMove& rm : rootMoves)
      //sf10    rm.previousScore = rm.score; 
        
      if (m_depth >=5) //stcokfish-7 sf10 use asspwin for depth >=5
	    {			
        //1231 if (abs(lastbest) < WIN_VALUE)
            delta = 19;       
            alpha = std::max(lastbest - delta, -INF);
            beta  = std::min(lastbest + delta,  INF);
    	}
      spboard->ply=0;
      // Start with a small aspiration window and, in the case of a fail
      // high/low, re-search with a bigger window until we don't fail high/low anymore.            
            	int faillow_cnt = 0;
            	int failedHighCnt = 0;
            	while (true) 
              {                      	
/* sf10       Depth adjustedDepth = std::max(ONE_PLY, rootDepth - failedHighCnt * ONE_PLY);
              bestValue = ::search<PV>(rootPos, ss, alpha, beta, adjustedDepth, false);
              // Bring the best move to the front. It is critical that sorting
              // is done with a stable algorithm because all the values but the
              // first and eventually the new best one are set to -VALUE_INFINITE
              // and we want to keep the same order for all the moves except the
              // new PV that goes to the front. Note that in case of MultiPV
              // search the already searched PV lines are preserved.
              std::stable_sort(rootMoves.begin() + pvIdx, rootMoves.begin() + pvLast);
              // If search has been stopped, we break immediately. Sorting is safe
              // because RootMoves is still valid, although it refers to the previous iteration.
              if (Threads.stop)  break;
              // In case of failing low/high increase aspiration window and
              // re-search, otherwise exit the loop.
              if (bestValue <= alpha)
              {   beta = (alpha + beta) / 2;
                  alpha = std::max(bestValue - delta, -VALUE_INFINITE);
                  if (mainThread)
                  {   failedHighCnt = 0;
                      failedLow = true;
                      Threads.stopOnPonderhit = false;
                  }
              }
              else if (bestValue >= beta)
              {   beta = std::min(bestValue + delta, VALUE_INFINITE);
                  if (mainThread)
                	  ++failedHighCnt;
              }
              else break;
              delta += delta / 4 + 5;
          }
          // Sort the PV lines searched so far and update the GUI
          std::stable_sort(rootMoves.begin() + pvFirst, rootMoves.begin() + pvIdx + 1);
          if (    mainThread
              && (Threads.stop || pvIdx + 1 == multiPV || Time.elapsed() > 3000))
              sync_cout << UCI::pv(rootPos, rootDepth, alpha, beta) << sync_endl;                	
*/       
                newdepth = std::max(1, m_depth - failedHighCnt);     	  
                //1231 best = searchrootnode(*spboard, alpha, beta, newdepth, NULL_NO, root_pv);   
                best = search<PV>(*spboard, alpha, beta, newdepth, NULL_NO, root_pv);   
                std::stable_sort(spboard->movetab, spboard->movetab + spboard->size);	
                if (spboard->m_timeout) break;                                                      
                if (best <= alpha)
                { beta = (alpha + beta) / 2;              
                  alpha = std::max(best - delta, -INF);  
                  if (mainThread)	
                  { failedHighCnt = 0;
                    faillow_cnt++; 
                  }
                }    
                else if (best >= beta)
                { beta = std::min(best + delta, INF);   
                  if (mainThread)	failedHighCnt++;
               	}               	
                else break; //while true
                	
                delta += delta / 4 + 5;                  
                assert(alpha >= -INF && beta <= INF);
              } //while true  	               
            
                        
        std::stable_sort(spboard->movetab, spboard->movetab + spboard->size);	

                
                spboard->m_bestmove = spboard->movetab[0].table.move;
                //1210 update root_pv
                root_pv[0] = spboard->m_bestmove;
//----------------------------------------------------------------                
//1213 print pv info immed after update root_pv and not after iteration. root_pv may be poluted in later root mv i                
              if (idx == 0)  //print pv only for main thread
              {	
                TimeSpan = (int)(GetTime() - spboard->m_startime); //(clock() - m_startime) ;
                nps = int((double)spboard->m_nodes * 1000 / (double)(TimeSpan==0 ? 1:TimeSpan));
                printf("info depth %d score %d time %d nodes %llu nps %d pv", m_depth, best, TimeSpan, spboard->m_nodes, nps);
                fflush(stdout);
//1213                ponder_move = 0;
//1213                respmove_to_ponder = 0;
                for (int j=0; j<MAX_PLY+1; ++j)
                {
                    	  if (root_pv[j] == 0)
                            break;
                        MoveStruct temppv;
                        temppv.move = root_pv[j];
                        com2char(charmove, temppv.from, temppv.dest );
                        printf(" %s", charmove); //fflush(stdout); 
//1213                        if (j==1) ponder_move = temppv.move;
//1213                        else if (j==2) respmove_to_ponder = temppv.move;                       
                }
                printf("\n"); fflush(stdout);
              }                                
            	
	    //save main and helper thread completed root_depth
      spboard->root_depth = m_depth; 
            
//if (m_depth>=7) //7) //7)  //6)
//lazy smp - print pvline only for mainthread idx=0
      TimeSpan = (int)(GetTime() - spboard->m_startime); //(clock() - m_startime) ;
//if (TimeSpan==0) TimeSpan=1;
//int nps = int((double)spboard->m_nodes * 1000 / (double)TimeSpan);	
   nps = int((double)spboard->m_nodes * 1000 / (double)(TimeSpan==0 ? 1:TimeSpan));
if (idx != 0)
{	 
#ifdef THDINFO	
  for (int s=1; s<=idx; s++) printf("     "); 
	printf("helper thd %d depth %d score %d time %d nodes %llu nps %d \n", idx, m_depth, best, TimeSpan, spboard->m_nodes, nps);
  fflush(stdout);
#endif  
}
else 
                {
                	//1004 
                  if (spboard->IMaxTime<= 52 && m_depth >=4 && TimeSpan>=1) //clop panictime=52  //1018 depth 5->4 time 2->1
                  {	spboard->m_timeout = 1;
                  	printf("*** panic timeout!\n");
                  }
                    
                }

            if (spboard->size==1 || (abs(best) > WIN_VALUE && abs(lastbest) > WIN_VALUE))
            	if (m_depth >= 6) //4 6
            	  break; 	  

#ifdef PRTBOARD   
if (mainThread) { 
	  
    printf("\nEnd of depth: m_depth=%d, lastbest=%d, delta=%d, alpha=%d, beta=%d, best=%d", m_depth, lastbest, delta, alpha, beta, best);
    printf("\nRoot moves: ");
    for (int i=0; i<spboard->size; i++)
    {
        com2char(charmove, spboard->movetab[i].table.from, spboard->movetab[i].table.dest );
        //fprintf(traceout, " %s", charmove);
        printf("  %s", charmove);
    }    
    printf("\n");
    fflush(stdout);
    
    printf("Root tbval: ");
    for (int i=0; i<spboard->size; i++)
    {
        printf("%6d", spboard->movetab[i].tabval);
        //printf("%6d", root_nodes[i]);
    }
    printf("\n"); 
    fflush(stdout);
} //mainThread 
#endif        
//      put back pv_tabval after sort 
// 20160804 - replaced by sf root move ordering
//        spboard->movetab[0].tabval = pv_tabval;	
        	
#ifdef DEBUG
        printf("info Root moves: ");
        for (int i=0; i<size; i++)
        {
            com2char(charmove, movetab[i].table.from, movetab[i].table.dest );
            //fprintf(traceout, " %s", charmove);
            printf("%10s", charmove);
        }
        //fprintf(traceout, "\n");
        printf("\n");
        //fflush(traceout);
        fflush(stdout);
        //fclose(traceout);
        //

        printf("info Root tbval: ");
        for (int i=0; i<size; i++)
        {
            printf("%10d", movetab[i].tabval);
            //printf("%10d", root_nodes[i]);
        }
        //fprintf(traceout, "\n");
        printf("\n");
        //fflush(traceout);
        fflush(stdout);
        //fclose(traceout);
        
        printf("info Root nodes: ");
        for (int i=0; i<size; i++)
        {
            //printf("%5d", movetab[i].tabval);
            printf("%10d", (unsigned int)root_nodes[i]);
        }
        //fprintf(traceout, "\n");
        printf("\n");
        //fflush(traceout);
        fflush(stdout);
        //fclose(traceout);
        //
#endif

        lastbest = best;

//	if (HCheckStop()) break;
                    long long t_remain = spboard->IMaxTime - (GetTime()-spboard->m_startime);
    				        if (t_remain <= 0) 
    				        {	                     
                  	   spboard->m_timeout= 1; //stop                   	
                    	 break;
                    }

    } // next depth


return spboard->m_bestmove;
}




//------------------------------------------------------------------------------------------------------------------
//#define EVAL_MARGIN1 345 //256
#ifdef __GNUC__
inline
#else  
__forceinline
#endif
int Engine::Evalscore(Board &board) //(int alpha,int beta)
{
    //if (board.IsInCheck(oppside(board.m_side)))
    //	return (board.m_side ? board.ply-INF : INF-ply);

    int val;
/*
    //lazy eval
    val=(board.m_side ? -pointsum : pointsum);


    if (val - EVAL_MARGIN1 >= beta)  {
        return val - EVAL_MARGIN1;
    }
    if (val + EVAL_MARGIN1 <= alpha) {
        return val + EVAL_MARGIN1;
    }
*/
#ifdef EVALHASH
    //val=ProbeEvalHash();
    EvalHash evalhsh;
	unsigned int offset = (board.hkey & nEvalHashMask);
    evalhsh = evalhashItems[offset];
    //if ((evalhsh.hkey64 & 0x0000FFFFFFFFFFFF) == (hkey & 0x0000FFFFFFFFFFFF))
        //if ((evalhsh.hkey64 ) == (hkey ))
    //if (evalhsh.hkey32 == ((hkey )>>32))  //8byte evalhash
    //if ( (evalhsh.hkey16_2 <<16)+(evalhsh.hkey16_1) == (hkey >>32 ) )  //6byte evalhash
    if (evalhsh.hkey32 == ((board.hkey )>>32))  //6byte evalhash
    {
        val=evalhsh.value;
        //val=evalhsh.value - INF;  //-2048  4byte evalhash
//      		nEvalHits++;
    }
    else
    {	/*lazy=0*/

#endif
        val= board.Eval(); //(alpha,beta); //+ 5; //+ pointsum;
#ifdef EVALHASH
        //if (val >= beta)	//stockfish 1.4
        {	//RecordEvalHash(val);
            //evalhsh.hkey64 = hkey;
            //evalhsh.hkey16_2 = ((hkey )>>32)>>16;
            //evalhsh.hkey16_1 = ((hkey )>>32)&0x00FF;  
            evalhsh.hkey32 = ((board.hkey )>>32);          
            evalhsh.value =  val;
            //evalhsh.hkey32 = ((hkey )>>32); 
            //evalhsh.hkey = hkey; //8byte evalhash
            //evalhsh.hkey32 = uint32(hkey ); //4byte evalhash            
            //evalhsh.value = val + INF; //+2048 4byte evalhash
			offset = (board.hkey & nEvalHashMask);
            evalhashItems[offset] = evalhsh;
//  			nEvalRecs++;
        }
    }
#endif
    return val;
}

  // update_history() registers a good move that produced a beta-cutoff
  // in history and marks as failures all the other moves of that board.ply.
#ifdef __GNUC__
inline
#else  
__forceinline
#endif
static void update_HistVal(HistStruct *hisvptr, int depth, int thd_id)  //1017
{
    hisvptr->HistVal += depth * depth; //HistInc[depth]; //
    if (hisvptr->HistVal >= HistValMax)
    {
			//for (int i=0; i<(BOARD_SIZE-7); i++)
			//for (int j=0; j<16; j++)
			for (int p=0; p<10; p++)
		  for (int i=0; i<10; i++)
			for (int j=0; j<9; j++)
			{
				//m_his_table[p][i][j].HistVal /= 2; // (m_his_table[p][i][j].HistVal + 1) / 2;
        m_his_table[thd_id][p][(i*16)+j].HistVal /= 2;
			}
    }
}

#ifdef __GNUC__
inline
#else  
__forceinline
#endif
static void update_history(HistStruct *hisvptr, int depth,
                      MoveTabStruct movesSearched[], int noncap_gen_count, int thd_id)   //1017
{
		hisvptr->HistHit++;
    for (int icnt = 0; icnt < noncap_gen_count - 1; icnt++)
    {
//        assert(m != movesSearched[i]);
        //if (ok_to_history(pos, movesSearched[i]))
        //    H.failure(pos.piece_on(move_from(movesSearched[i])), movesSearched[i]);
        //if (movesSearched[icnt].tabval != 0) //capture == 0 and tabval = piecefrom
        {
        		HistStruct *hisvptri;
        		//hisvptri = &(m_his_table[movesSearched[icnt].dest][PIECE_IDX16(movesSearched[icnt].tabval)]);
        		hisvptri = &(m_his_table[thd_id][PIECE_IDX(movesSearched[icnt].tabval)][movesSearched[icnt].dest]);
        		hisvptri->HistTot++;
        		if (hisvptri->HistTot >= HistoryMax)
            {
            	//for (int i=0; i<(BOARD_SIZE-7); i++)
							//for (int j=0; j<16; j++)
							for (int p=0; p<10; p++)
							for (int i=0; i<10; i++)
							for (int j=0; j<9; j++)
							{
								m_his_table[thd_id][p][(i*16)+j].HistHit /= 2; // (m_his_table[p][i][j].HistHit + 1) / 2;
        				m_his_table[thd_id][p][(i*16)+j].HistTot /= 2; // (m_his_table[p][i][j].HistTot + 1) / 2;
    					}
  					}

  					hisvptri->HistVal -= depth * depth;
            if (hisvptri->HistVal <= -HistValMax)
            {
    	        for (int p=0; p<10; p++)
		          for (int i=0; i<10; i++)
			        for (int j=0; j<9; j++)
			        {
				        m_his_table[thd_id][p][(i*16)+j].HistVal /= 2; // (m_his_table[p][i][j].HistVal + 1) / 2;
			        }
            }


        }
    }
  }

//static const int IIDDepth = 3;
//static const int IIDReduction = 2;
#define IIDDepth 3
#define IIDReduction 2
#define IIDMargin 48 //FUTPAWN*3/2  //32*2=64  //48    //1107 ?? todo 480--> rooksac
//int NullMoveMargin = IIDMargin*3/2;

template <int NT>
int Engine::search(Board &board, int alpha, int beta, int depth, int null_allow, move_t pv[])
{
	  constexpr bool PvNode = (NT == PV);
	  
//1212 Allocate PV for the child node, and terminate current PV
//1212     int ply = board.ply; //1212
    move_t childPv[MAX_PLY]; //1212 cannot alloc dynamic[MAX_PLY - ply], alloc [MAX_PLY] instead
//    if (PvNode)                 //1212  
       pv[0] = 0;               //1212 //1210
    
	  
// sf10 Dive into quiescence search when the depth reaches zero
//    if (depth < ONE_PLY)
//        return qsearch<NT>(pos, ss, alpha, beta);
    if (depth <= 0)
    {	 //if (PvNode)
    	 //	  return quiesCheckPV(board, alpha, beta, depth);
    	 // else	  
      		return quiesCheck<NT>(board, alpha, beta, depth); //1210     
    }  
    // Step 1. Initialize node  	  
    int val, capture, incheck, newdepth, best, nHashFlag, extension, evalscore; // , futpawn_x2depth_1;
    int moveCount, captureCount, quietCount;
    bool improving, givesCheck, doFullDepthSearch, moveCountPruning, skipQuiets; //captureOrPromotion; 
    MoveStruct mvBest;
    MoveTabStruct movesSearched[128];
    HistStruct *hisvptr;
    //1118 int alpha = beta - 1;
    best = -INF;
//debug
#ifdef DEBUG
    if (depth>0)
        nTreeNodes ++;	// 统计树枝节点
    else
        nLeafNodes ++;
//debug
#endif

////1006 checktime for main thread only
//if (board.thd_id==0)
//{ 	
// 6. 中断调用；
        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;
        board.m_nodes++;
      //m_time_check++;
        //if ((m_time_check &8191)==0)  //4095)==0)
        if ((board.m_nodes      & PollNodes)==0) //4095)==0) //8191)==0)  //4095)==0)
         {
            //if (board.IMaxTime <4000)
            if (board.thd_id==0)
          {
            BusyComm = BusyLine(UcciComm, false);
            if (BusyComm == UCCI_COMM_STOP)
            //if (BusyLine(UcciComm, false) == UCCI_COMM_STOP )
            {
            	// "stop"指令发送中止信号
            board.m_timeout= 1;	//stop
            //printBestmove(board.m_bestmove);
            printf("info searchAB stopped by GUI pv\n");
            fflush(stdout);
            	//return (board.m_side ? -INF-1 : INF+1);
            	return UNKNOWN_VALUE;
            	//longjmp(setjmpbuf,1);
            }

            else if (BusyComm == UCCI_COMM_QUIT)
            {
            	p_feedback_move = -1; //pass to Eychessu.cpp to quit the engine
            	//printf("info UCCI_COMM_QUIT p_feedback_move = %d\n", p_feedback_move);
			        //fflush(stdout);
            }
          }
          
            //if ((clock()-board.m_startime>=board.IMaxTime))  //m_depth>=14 &&
            //if ((GetTime()-board.m_startime>=board.IMaxTime))  //m_depth>=14 &&
            long long t_remain = board.IMaxTime - (GetTime()-board.m_startime);
    				if (t_remain <= 0)
            {                        
                  	 board.m_timeout= 1; //stop
    printf("info timout\n"); // %d board.ply %d board.IMaxTime %d\n", (int)(GetTime() - board.m_startime), board.ply, board.IMaxTime);
    fflush(stdout);
            	    //printBestmove(board.m_bestmove);
                  //if (jmp_ok)
                  //  longjmp(setjmpbuf,1);
                  return UNKNOWN_VALUE;
            }
            
          if (t_remain < 4000)    			
    					PollNodes = 1023;                  
        }
//} //1006

// 2. 和棋裁剪；
    //if (pos.IsDraw())
    if ((board.bitpiece & 0xfff00ffc) == 0) return Evalscore(board);
    incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
    if (board.ply>= MAX_PLY)  
    	 return Evalscore(board);
       //1113 return (!incheck ? Evalscore(board) : 0);
       
// sf10 Step 3. Mate distance pruning. Even if we mate at the next move our score
        // would be at best mate_in(ss->ply+1), but if alpha is already bigger because
        // a shorter mate was found upward in the tree then there is no need to search
        // because we will never beat the current alpha. Same logic but with reversed
        // signs applies also in the opposite condition of being mated instead of giving
        // mate. In this case return a fail-high score.
        //alpha = std::max(mated_in(ss->ply), alpha);
        //beta = std::min(mate_in(ss->ply+1), beta);
        //constexpr Value mated_in(int ply) {  return -VALUE_MATE + ply;}
        //constexpr Value mate_in(int ply) {  return VALUE_MATE - ply;}
        alpha = std::max(-VALUE_MATE + board.ply, alpha);
        beta = std::min(VALUE_MATE - (board.ply+1), beta);	
        if (alpha >= beta)
            return alpha;        
 // Step 3. Mate distance pruning
 //   if (value_mated_in(board.ply) >= beta)
//replace by sf10      if (-INF+board.ply >= beta)
//        return beta;

    //if (value_mate_in(board.ply + 1) < beta)
//    if (INF-(board.ply + 1) < beta)
//        return beta - 1;

int old_alpha = alpha;

    // 3. 重复裁剪；

    val=board.checkloop(1);  //1 bef927
    if (val)
     		return val;
    
    board.currentMove[board.ply].move = board.excludedMove[board.ply+1] = 0;  
    int prevSqdest = board.currentMove[board.ply - 1].dest;  //to_sq((ss-1)->currentMove);  
    moveCount = captureCount = quietCount = board.moveCount[board.ply] = 0;  //1024

// At non-PV nodes we check for an early TT cutoff   
    // 4. 置换裁剪；
    HashStruct *hsho;
    int ttValue=VALUE_NONE;
    int ttMove = 0;
    int excludedMove;
    excludedMove = board.excludedMove[board.ply];
/* sf10    	
// Step 4. Transposition table lookup. We don't want the score of a partial
    // search to overwrite a previous full search TT value, so we use a different
    // position key in case of an excluded move.
    excludedMove = ss->excludedMove;
    posKey = pos.key() ^ Key(excludedMove << 16); // Isn't a very good hash
    tte = TT.probe(posKey, ttHit);
    ttValue = ttHit ? value_from_tt(tte->value(), ss->ply) : VALUE_NONE;
    ttMove =  rootNode ? thisThread->rootMoves[thisThread->pvIdx].pv[0]
            : ttHit    ? tte->move() : MOVE_NONE;

    // At non-PV nodes we check for an early TT cutoff
    if (  !PvNode
        && ttHit
        && tte->depth() >= depth
        && ttValue != VALUE_NONE // Possible in case of TT access race
        && (ttValue >= beta ? (tte->bound() & BOUND_LOWER)
                            : (tte->bound() & BOUND_UPPER)))
    {
      // If ttMove is quiet, update move sorting heuristics on TT hit
        if (ttMove)
        {
            if (ttValue >= beta)
            {
                if (!pos.capture_or_promotion(ttMove))
                    update_quiet_stats(pos, ss, ttMove, nullptr, 0, stat_bonus(depth));

                // Extra penalty for a quiet TT move in previous ply when it gets refuted
                if ((ss-1)->moveCount == 1 && !pos.captured_piece())
                    update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, -stat_bonus(depth + ONE_PLY));
            }
            // Penalty for a quiet ttMove that fails low
            else if (!pos.capture_or_promotion(ttMove))
            {
                int penalty = -stat_bonus(depth);
                thisThread->mainHistory[us][from_to(ttMove)] << penalty;
                update_continuation_histories(ss, pos.moved_piece(ttMove), to_sq(ttMove), penalty);
            }
        }
        return ttValue;  	
    }
*/    	     
   if (!PvNode) 
   { ttValue = ProbeHash(depth, beta, null_allow, hsho, board);
    if (ttValue != UNKNOWN_VALUE)
    { if (ttValue >=beta) 
    	{ // If ttMove is quiet, update move sorting heuristics on TT hit
    	    //1024 update_quiet_stats()  //killers, hist, countermove
    	  	MoveStruct ttmove;
    	  	ttmove.move = get_hmv(hsho->hmvBest);
    	  	if (board.piece[ttmove.dest]==0)
    	  	{ //if (ttValue >= WIN_VALUE)
//                 g_matekiller[board.ply].move = ttmove.move;
//            else             	  
              if (g_killer[0][board.ply].move != ttmove.move)
              { g_killer[1][board.ply].move = g_killer[0][board.ply].move;
                g_killer[0][board.ply].move = ttmove.move;
              }    
                hisvptr = &(m_his_table[board.thd_id][PIECE_IDX(board.piece[ttmove.from])][ttmove.dest]);  //1017
                update_HistVal(hisvptr, depth, board.thd_id);         
                //1016 update_history(hisvptr, depth, movesSearched, noncap_gen_count);    
                if (board.ply >0 && (board.currentMove[board.ply -1].move !=0))
                   counterMoves[board.thd_id][PIECE_IDX(board.piece[prevSqdest])][prevSqdest].move = ttmove.move;  //1017
    	  	}	
    	}
//    	  if (hsho-> hDepth ==1 || hsho-> hDepth ==2)  //2019k debug whether probehash can use hash in QS
//         	hashhit++;
      return ttValue;
    }
   } 
   else //PVNode
   	hsho = ProbeMove(board); 
    
    if (hsho) 
    {	 ttMove = get_hmv(hsho->hmvBest);
       ttValue = value_from_tt(hsho->hVal, board.ply);
    }	
    
/* sf10
 // Step 5. Tablebases probe
 // Step 6. Static evaluation of the position
    if (inCheck)
    {
        ss->staticEval = eval = pureStaticEval = VALUE_NONE;
        improving = false;
        goto moves_loop;  // Skip early pruning when in check
    }
    else if (ttHit)
    {
        // Never assume anything on values stored in TT
        ss->staticEval = eval = pureStaticEval = tte->eval();
        if (eval == VALUE_NONE)
            ss->staticEval = eval = pureStaticEval = evaluate(pos);

        // Can ttValue be used as a better position evaluation?
        if (    ttValue != VALUE_NONE
            && (tte->bound() & (ttValue > eval ? BOUND_LOWER : BOUND_UPPER)))
            eval = ttValue;
    }
    else
    {
        if ((ss-1)->currentMove != MOVE_NULL)
        {
            int p = (ss-1)->statScore;
            int bonus = p > 0 ? (-p - 2500) / 512 :
                        p < 0 ? (-p + 2500) / 512 : 0;

            pureStaticEval = evaluate(pos);
            ss->staticEval = eval = pureStaticEval + bonus;
        }
        else
            ss->staticEval = eval = pureStaticEval = -(ss-1)->staticEval + 2 * Eval::Tempo;

        tte->save(posKey, VALUE_NONE, BOUND_NONE, DEPTH_NONE, MOVE_NONE, pureStaticEval);
    }
*/
    
// Step 6. Static evaluation of the position
    mvBest.move = 0;       	
    if (incheck)
    {
        board.staticEval[board.ply] = evalscore = VALUE_NONE;
        improving = false;
    	  goto moves_loop;  // Skip early pruning when in check
    }	
    else if (hsho)   // else if (ttHit)	
    { // Never assume anything on values stored in TT
    	board.staticEval[board.ply] = evalscore = ttValue;
    	if (ttValue == UNKNOWN_VALUE) 
    		 board.staticEval[board.ply] = evalscore = Evalscore(board);
    		 
    	 // Can ttValue be used as a better position evaluation?
       // if (    ttValue != VALUE_NONE
       //     && (tte->bound() & (ttValue > eval ? BOUND_LOWER : BOUND_UPPER)))
       //     eval = ttValue;	 
    	if (ttValue != UNKNOWN_VALUE)  
    	    evalscore = ttValue;
    }	
    else 
    {	
    	//sf10 if ((ss-1)->currentMove != MOVE_NULL)
      //     {
      //      int p = (ss-1)->statScore;
      //      int bonus = p > 0 ? (-p - 2500) / 512 :
      //                  p < 0 ? (-p + 2500) / 512 : 0;

      //      pureStaticEval = evaluate(pos);
      //      ss->staticEval = eval = pureStaticEval + bonus;
      //     }
      //     else
      //      ss->staticEval = eval = pureStaticEval = -(ss-1)->staticEval + 2 * Eval::Tempo;

      //  tte->save(posKey, VALUE_NONE, BOUND_NONE, DEPTH_NONE, MOVE_NONE, pureStaticEval);
    	//constexpr Value Tempo = Value(20); // Must be visible to search
    	//sf10 ss->staticEval = eval = pureStaticEval = -(ss-1)->staticEval + 2 * Eval::Tempo;
      //sf10  tte->save(posKey, VALUE_NONE, BOUND_NONE, DEPTH_NONE, MOVE_NONE, pureStaticEval);
        
        if (board.currentMove[board.ply-1].move != 0)
        	 board.staticEval[board.ply] = evalscore = Evalscore(board);
        else	
    	     board.staticEval[board.ply] = evalscore = -board.staticEval[board.ply-1] + 40; //2 * Eval::Tempo; //Evalscore(board);
    }	
    		
// sf10 Step 7. Razoring (~2 Elo)
//    if (   depth < 2 * ONE_PLY
//        && eval <= alpha - RazorMargin)
//        return qsearch<NT>(pos, ss, alpha, beta);
 
   if (depth < 2 
//   	&& !incheck  //bypass by goto
   	   && evalscore <= alpha - RazorMargin)
   	   {  //if (PvNode)
   	   	  //return quiesCheckPV(board, alpha, beta, 0);
   	   	  //else
   	      return quiesCheck<NT>(board, alpha, beta, 0);  //1210
   	   }
   	   
/* sf10	
improving =   ss->staticEval >= (ss-2)->staticEval
               || (ss-2)->staticEval == VALUE_NONE;
// Step 8. Futility pruning: child node (~30 Elo)
    if (   !rootNode
        &&  depth < 7 * ONE_PLY
        &&  eval - futility_margin(depth, improving) >= beta
        &&  eval < VALUE_KNOWN_WIN) // Do not return unproven wins
        return eval;
*/
    improving = board.ply < 2
              || board.staticEval[board.ply] >= board.staticEval[board.ply -2]
              || board.staticEval[board.ply -2] == VALUE_NONE;
    if (   depth < 7 
    	  && evalscore - futility_margin(depth, improving) >= beta
        && evalscore < VALUE_KNOWN_WIN) // Do not return unproven wins
        return evalscore;
    	          
/*        
// Step 9. Null move search with verification search (~40 Elo)
    if (   !PvNode
        && (ss-1)->currentMove != MOVE_NULL
        && (ss-1)->statScore < 23200
        &&  eval >= beta
        &&  pureStaticEval >= beta - 36 * depth / ONE_PLY + 225
        && !excludedMove
        &&  pos.non_pawn_material(us)
        && (ss->ply >= thisThread->nmpMinPly || us != thisThread->nmpColor))
    {
        assert(eval - beta >= 0);

        // Null move dynamic reduction based on depth and value
        Depth R = ((823 + 67 * depth / ONE_PLY) / 256 + std::min(int(eval - beta) / 200, 3)) * ONE_PLY;

        ss->currentMove = MOVE_NULL;
        ss->continuationHistory = &thisThread->continuationHistory[NO_PIECE][0];

        pos.do_null_move(st);

        Value nullValue = -search<NonPV>(pos, ss+1, -beta, -beta+1, depth-R, !cutNode);

        pos.undo_null_move();

        if (nullValue >= beta)
        {
            // Do not return unproven mate scores
            if (nullValue >= VALUE_MATE_IN_MAX_PLY)
                nullValue = beta;

            if (thisThread->nmpMinPly || (abs(beta) < VALUE_KNOWN_WIN && depth < 12 * ONE_PLY))
                return nullValue;

            assert(!thisThread->nmpMinPly); // Recursive verification is not allowed

            // Do verification search at high depths, with null move pruning disabled
            // for us, until ply exceeds nmpMinPly.
            thisThread->nmpMinPly = ss->ply + 3 * (depth-R) / 4;
            thisThread->nmpColor = us;

            Value v = search<NonPV>(pos, ss, beta-1, beta, depth-R, false);

            thisThread->nmpMinPly = 0;

            if (v >= beta)
                return nullValue;
        }
    }
*/

//1213 use sf10 Step 9. Null move search with verification search (~40 Elo)
    if (   !PvNode
    	  && null_allow && depth > 1
        //&& (ss-1)->currentMove != MOVE_NULL
        && board.currentMove[board.ply-1].move != 0
        //1213 && (ss-1)->statScore < 23200
        &&  evalscore >= beta  // - NullMoveMargin   //1214
        //1213 &&  pureStaticEval >= beta - 36 * depth / ONE_PLY + 225
        // stockfish 1.3 approximateEval >= beta - NullMoveMargin
            //|| (board.m_side ? -pointsum : pointsum) >= beta - VN    
        && !excludedMove
        //1213 &&  pos.non_pawn_material(us)
        && board.non_pawn_material(board.m_side)   
        //1213 && (ss->ply >= thisThread->nmpMinPly || us != thisThread->nmpColor)
        )
    {
        //1213 assert(eval - beta >= 0);

        // Null move dynamic reduction based on depth and value
        //1213 Depth R = ((823 + 67 * depth / ONE_PLY) / 256 + std::min(int(eval - beta) / 200, 3)) * ONE_PLY;
        int R = ((823 + 67 * depth ) / 256 + std::min(int(evalscore - beta) / mf, 3)) ;  //1215 260-270 sf10 /200
        //int R = (3 + depth / 4) + std::min(int(evalscore - beta) / 256, 3);  //1214 sf10 /200	
        //  int R = (3 + depth / 4) + (evalscore >= beta + VN);
        // const int nextDepth = depth - R, R = (3 + depth / 4) + (refinedEval >= beta + 167);
        // depth  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19  
        //   R    3  3  4  4  4  4  5  5  5  5  6  6  6  6  7  7  7  7 

        //1213 ss->currentMove = MOVE_NULL;
        board.currentMove[board.ply].move = 0;
        //1213 ss->continuationHistory = &thisThread->continuationHistory[NO_PIECE][0];

        //1213 pos.do_null_move(st);
        board.m_hisrecord[board.m_hisindex].htab.tabentry = 0; //move,capture,chk=0;
		    board.m_hisrecord[board.m_hisindex].mvpiece = 0;
        board.m_hisindex++;
        board.ply++;
        chgside(board.m_side);
        board.Xor(board.hkey,h_rside);

        //1213 Value nullValue = -search<NonPV>(pos, ss+1, -beta, -beta+1, depth-R, !cutNode);
        val = -search<NonPV>(board, -beta, -beta+1, depth-R, NULL_NO, childPv);  //1215 bef NULL_NO //sf10 !null_allow
        //1213 pos.undo_null_move();
        board.Xor(board.hkey,h_rside);
        chgside(board.m_side);
        board.ply--;
        board.m_hisindex--;

        if (val >= beta)
        {
            // Do not return unproven mate scores
            if (val >= VALUE_MATE_IN_MAX_PLY)
                val = beta;

            //1213 if (thisThread->nmpMinPly || (abs(beta) < VALUE_KNOWN_WIN && depth < 12 * ONE_PLY))
            //    return nullValue;
            if (abs(beta) < VALUE_KNOWN_WIN && depth < 12 )
            	    return val;

            //1213 assert(!thisThread->nmpMinPly); // Recursive verification is not allowed

            // Do verification search at high depths, with null move pruning disabled
            // for us, until ply exceeds nmpMinPly.
            //1213 thisThread->nmpMinPly = ss->ply + 3 * (depth-R) / 4;
            //1213 thisThread->nmpColor = us;
            //1213 Value v = search<NonPV>(pos, ss, beta-1, beta, depth-R, false);
            //1213 thisThread->nmpMinPly = 0;
            //1213 if (v >= beta)
            //1213    return nullValue;
            
                     
               int v = search<NonPV>(board, beta-1, beta, depth-R , NULL_NO, childPv);
               if (v >= beta)
            	    return val;   
            	 
            	      
        }
    }


/* //1213 use sf10 null move search 
		// step 9: nullmove pruning (bef sf10)
//    mvBest.move = 0;  //move up bef goto
    if (null_allow && depth>1 
       	&& board.non_pawn_material(board.m_side)   //1109
      	&&  evalscore >= beta                      //1014
      	&& !excludedMove  //1109
  	// && !incheck //bypaas by goto //&& node_type != NodePV
    	//probe TT to filter out nullmove 
    	// If a plain reduced search without nullmove won't produce a cutoff... 
    	//if (tt_depth >= depth - R - 1 && tt_score < beta && tt_type & UPPER) skip_null();     		
    	//slower //&& !(hsho && hsho->hAlphaDepth >= depth - nullreduction - 1 && hsho->hAlphaVal < beta)
    	
            //&& (beta <= ValueEvalInf)
            //&& (beta > -WIN_VALUE && beta < WIN_VALUE)
            //&& (abs(beta) < WIN_VALUE)
            //&& bitCountMSB(bitpiece & (0x55500000<<board.m_side))>=2  //major>=2
       )
    {
        if ( (depth <=4
            || (
            !(hsho && hsho->hFlag >= HASH_ALPHA && (hsho->hDepth) >= depth - nullreduction - 1 && hsho->hVal < beta)
              //1014 &&
              //Evalscore(board) >= beta //- NullMoveMargin //sf 1.8
              //  evalscore >= beta   //1014
            // stockfish 1.3 approximateEval >= beta - NullMoveMargin
            //|| (board.m_side ? -pointsum : pointsum) >= beta - VN             
               )
              )
              && (beta > -WIN_VALUE && beta < WIN_VALUE)            
        )
        {
//debug		nNullMoveNodes ++;
            //makenull();

		        board.m_hisrecord[board.m_hisindex].htab.tabentry = 0; //move,capture,chk=0;
		        board.m_hisrecord[board.m_hisindex].mvpiece = 0;
            board.m_hisindex++;
            board.ply++;
            chgside(board.m_side);
            board.Xor(board.hkey,h_rside);


            //newdepth=depth - (nullreduction+1); // - 1;  // -4  R=3
            //nullreduction = (depth >= 5 ? 4 : 3); // Null move dynamic reduction
            newdepth=depth - nullreduction - 1; // - 1;  // -4  R=3
            //if (depth>6) newdepth++;  //>5
						if (newdepth <= 0)
      			  val = -quiesCheck<NonPV>(board, -beta, -beta+1, newdepth, childPv); //1210  
      			else            	
            	val = -search<NonPV>(board, -beta, -beta+1, newdepth, NULL_NO, childPv);  //1210
            //unmakenull();

            board.Xor(board.hkey,h_rside);
            chgside(board.m_side);
            board.ply--;
            board.m_hisindex--;
            if (board.m_timeout)
						   return UNKNOWN_VALUE;

						if ((val >= WIN_VALUE) || (val <= -WIN_VALUE))
            {        	//val = WIN_VALUE; // do not return unproven mates
            }
            else

            if (val >= beta)
            {
                //if ((val >= WIN_VALUE) || (val <= -WIN_VALUE))
                //    	val = beta; // do not return unproven mates //sf 1.7.1
                if (
                	(depth <= nullreductver)
             //    || bitCountMSB(bitpiece & (0x55500000<<board.m_side))>=4  //major>=4
               ||               
               search<NonPV>(board, beta-1, beta, depth - nullreductver, NULL_NO, childPv) >= beta     //1210
                   )
                    {
                        if ((newdepth > 0) && !board.m_timeout)
                            RecordHash(HASH_BETA, std::max(depth, nullreduction), val, 0, board); //, 0); //mvBest.move);  //mvBest=0
                        return val; //beta
                    }
            }

        }
    }
*/  //1213 end of null move search bef sf10
    
// Step 10. ProbCut (~10 Elo) (not yet done)

    //八、内部深度迭代： Internal Iterative Deepening (IID for PV) if no HashMove found
    // 如果Hash表未命中，使用depth-2进行内部深度迭代，目的是为了获取HashMove
    // 感觉速度快10～15%，有的局面并不显得快，但也不会慢。
    // 残局能够增加Hash表的命中率，加快解体速度。即提前发现将军的局面。
/*
    // Step 11. Internal iterative deepening IID (~2 Elo)  //sf10
    if (    depth >= 8 * ONE_PLY
        && !ttMove)
    {
        search<NT>(pos, ss, alpha, beta, depth - 7 * ONE_PLY, cutNode);

        tte = TT.probe(posKey, ttHit);
        ttValue = ttHit ? value_from_tt(tte->value(), ss->ply) : VALUE_NONE;
        ttMove = ttHit ? tte->move() : MOVE_NONE;
    }

*/
    if ( depth >= 8  //IIDDepthNonPV  8(sf 1.7.1)
    	&& ttMove == 0
//    	&& !incheck  //bypass by goto
//    	&& Evalscore(board) >= beta - IIDMargin
//1107 sf10 suppress        && Evalscore(board) >= beta - IIDMargin
		 )
    {        
        val = search<NT>(board, alpha, beta, depth - 7, null_allow, childPv); //1210	//1107 sf10 2--> 7. NULL_NO-->null_allow, crafty NULL_YES // 不使用带风险的裁剪       
        hsho = ProbeMove(board);
        if (hsho) 
        {	ttMove = get_hmv(hsho->hmvBest);
        	ttValue = value_from_tt(hsho->hVal, board.ply);
        }	
        else
        { ttMove = 0;
        	ttValue = VALUE_NONE;
        }	       
    }
//1119 ... template search() to be continued ...
moves_loop: // When in check, search starts from here
    MoveTabStruct movetab[64], ncapmovetab[111]; //64]; //capmovetab[111],
    MoveTabStruct badcapmovetab[32];  //1013 - separate badcap
    MoveStruct countermove = counterMoves[board.thd_id][PIECE_IDX(board.piece[prevSqdest])][prevSqdest];  //1017
    int badcapsize=0;
    nHashFlag = HASH_ALPHA;
    int gen_count=0;
		int noncap_gen_count=0;
    MoveTabStruct tempmove;
    int size=0;
    long ncapsize=0;
	//int kingidx,
	int opt_value=INF;
//int phase;
    skipQuiets = false;
        //1013 sep badcap for (int phase=5; phase--;)
enum npvphase{HASHPV,CAP,KILLER,CM,NOCAP,BADCAP};
    for (int phase=0; phase<=BADCAP; phase++)
    {
        switch (phase)
        {
        	case HASHPV:   // 0 hash pv
        		size=1;
        		break;
//        	case MATEKILLER:   // 1 mate-killer
//        		size=1;
//        		break;
          
          case CAP:  // 2 capture
        {
            if (incheck)
            {
            		size=board.GenChkEvasCap(&movetab[0], incheck);
          			// no single-reply extension for nonPV
            }
            else
            {
                size=board.GenCap(&movetab[0], &ncapmovetab[0], ncapsize);
            }
             //sort capture
             //Quicksort(0, size-1, movetab);
        }
            break;
          case KILLER: //3: //killer
          	if (incheck)  //evasion
          		size=0;
          	else	
            size=2; //NUM_KILLERS; //3; //4; //2;
            break;
          case CM: //countermove
          	if (incheck)  //evasion
          		size=0;
          	else
            size=1; 
            break;
          case NOCAP: //4 nocap
    if (incheck)
    {
        //size=board.GenChkEvasNCap(&movetab[0], incheck);
        size=board.GenChkEvasNCap(&ncapmovetab[0], incheck);
    }
    else
    {
    	//size=board.GenNonCap(&movetab[0]);
    	//append pawn/bis/ele/king noncap
    		for (int j=0; j<ncapsize; j++)
        {
            MoveTabStruct *tabptr;
            tabptr = &ncapmovetab[j];
            //MoveTabStruct tabptr = ncapmovetab[j];
            // cal tabval for pawn/bis/ele/king noncap
            if (tabptr->tabval >=0)
            {
                int piecefromidx = PIECE_IDX(board.piece[tabptr->from]);
            		//tabptr->tabval = m_his_table[piecefromidx][nFile(tabptr->dest)][nRank(tabptr->dest)].HistVal
                //int hs = his_table(piecefromidx, tabptr->dest);

            		// Ensure history has always highest priority
                //if (hs > 0)
                //	hs += 512;
                // if horse/pawn attk opp king (potential checking) sort to front
                //int piecefromtype = board.piece[tabptr->from]>>2;
                //if (piecefromtype == HORSE || piecefromtype <= PAWN)
                //	hs +=	kingattk_incl_horse[kingindex[board.boardsq[33-board.m_side]]][tabptr->dest]<<13; //+8192
            		tabptr->tabval = 
                  (abs_pointtable[piecefromidx][nRank(tabptr->dest)][nFile(tabptr->dest)]
                 - abs_pointtable[piecefromidx][nRank(tabptr->from)][nFile(tabptr->from)]) ;
                if (depth > 1)
                	 //1017 tabptr->tabval += his_table(piecefromidx, tabptr->dest);   
                	  tabptr->tabval += m_his_table[board.thd_id][piecefromidx][tabptr->dest].HistVal;  
                	         
            }
            //movetab[size] = ncapmovetab[j];
            //size++;
        }
        //memcpy(&movetab[size], &ncapmovetab[0], sizeof(ncapmovetab[0]) * ncapsize);
        //size += ncapsize;
        size=board.GenNonCap(&ncapmovetab[ncapsize], depth);
        size += ncapsize;
    }
        		//sort noncap
        		//Quicksort(0, size-1, movetab);
        		//std::sort(movetab, movetab+size);
        		std::sort(ncapmovetab, ncapmovetab+size);
        		break;
        
         case BADCAP:    //5:	// badcap
         	if (incheck)  //evasion
          		size=0;
          	else
        	 size = badcapsize;  
//1013 debug        	 
//        	 if (badcapsize>6) printf("*** badcapsize=%d\n", badcapsize); 
        	 break; 
        } // end switch

//        if (phase==0 && size==0)
//        	break; // break for phase

				//if (incheck==0)
			 	//				kingidx=kingindex[board.boardsq[32+board.m_side]];
//1013 - loop thru each move within each phase
//       if some phase size=0, i<size will skip the for loop
        for (int i=0; i<size; i++)
        {
           switch (phase)
            {
            case HASHPV: 	// 0 hashmove
            {
                tempmove.table.move=ttMove;
                if (tempmove.table.move==0
                    || board.LegalKiller(tempmove.table)==0
                   )
                    continue;
                    //NPVhashhit++;
            }
            break;            
//            case MATEKILLER: 	// 1 matekiller
//            {
//                tempmove.table.move=g_matekiller[board.ply].move;
//                if (tempmove.table.move==0
//                        || tempmove.table.move==ttMove
//                        || board.LegalKiller(tempmove.table)==0
//                   )
//                    continue;
//                    NPVmatekillerhit++;
//            }
//            break;                   
           case CAP: 	// 2 capture
            {
                tempmove.tabentry = GetNextMove(i, size, movetab);
                if (
                        tempmove.table.move==ttMove
//                        || tempmove.table.move==g_matekiller[board.ply].move
                   )
                    continue;
                   
                if (!incheck && !board.see_ge(tempmove.from, tempmove.dest,0)) 
                {
                	//1013 instead of append to ncapmovetab, separate to badcapmovetab
                	//ncapmovetab[ncapsize].table.move = tempmove.table.move;
                  //ncapmovetab[ncapsize].tabval =  - HistValMax; // Be sure are at the bottom
                  //ncapsize++;                  
                  badcapmovetab[badcapsize] = movetab[i];  //original captab & val
                  badcapsize++;
        				  continue;
                }    
            }
            break;
            case KILLER: 	// 3 killer
           	{
                tempmove.table.move=g_killer[i][board.ply].move;
                if (tempmove.table.move==0
                        || tempmove.table.move==ttMove
//                        || tempmove.table.move==g_matekiller[board.ply].move
                        || board.LegalKiller(tempmove.table)==0
                   )
                    continue;
            }
            break;
            case CM: 
            {	        
               //1016 countermove after killer
                tempmove.table.move=countermove.move;
                if (tempmove.table.move==0
                        || tempmove.table.move==ttMove
//                        || tempmove.table.move==g_matekiller[board.ply].move
                        || tempmove.table.move==g_killer[0][board.ply].move
                        || tempmove.table.move==g_killer[1][board.ply].move
                        || board.LegalKiller(tempmove.table)==0    //consider countermove as killer
                   )
                    continue; 
                //1016 debug
               //NPVcmhit++;
               //printf("*** NPV CM hit: from=%d, dest=%d\n", countermove.from, countermove.dest);            
            }
            break;
            case NOCAP: 	// 4 noncap
            {
            	  //tempmove.tabentry = movetab[i].tabentry; //after std::sort noncap
            	  tempmove.tabentry = ncapmovetab[i].tabentry; //after std::sort noncap
            	  if (incheck)
            	  {  if (tempmove.table.move==ttMove)
            	  			continue;
            	  }
            	  else
        				{ if ( (skipQuiets 
        					//&& board.piece[tempmove.dest]!=SQ_EMPTY
//1013        				     && !board.gives_check(tempmove.from, tempmove.dest)	
        				     )    //skipquiets not for badcapture 
        				||	tempmove.table.move==ttMove
//                ||  tempmove.table.move==g_matekiller[board.ply].move
                ||  tempmove.table.move==g_killer[0][board.ply].move
                || tempmove.table.move==g_killer[1][board.ply].move
                || tempmove.table.move==countermove.move
           			)
            		continue;
            	}
            }
            break;

            case BADCAP: //5:	//badcap
            {
            	  //no need to sort for short table 
            	  //tempmove.tabentry = badcapmovetab[i].tabentry; //after std::sort badcap
            	  tempmove.tabentry = GetNextMove(i, size, badcapmovetab);
            	  if ( tempmove.table.move==ttMove
//                || tempmove.table.move==g_matekiller[board.ply].move
                || tempmove.table.move==g_killer[0][board.ply].move
                || tempmove.table.move==g_killer[1][board.ply].move
                || tempmove.table.move==countermove.move
           			)
            		continue;
            }
            break;	
						
           } // end switch

            if (tempmove.table.move == excludedMove)  //1109
               continue;
            //ss->moveCount = ++moveCount;   
            board.moveCount[board.ply] = ++moveCount;  
               
            int piecefrom = board.piece[tempmove.from];
            //capture = board.piece[tempmove.dest];
            //hisvptr = &(m_his_table[tempmove.dest][PIECE_IDX16(board.piece[tempmove.from])]);
            hisvptr = &(m_his_table[board.thd_id][PIECE_IDX(piecefrom)][tempmove.dest]);  //1017

//1003      newdepth = depth + CheckExt -1; // + SingleExt;
            extension = 0;
            givesCheck = board.gives_check(tempmove.from, tempmove.dest);             
            moveCountPruning =   depth < 16
                        && moveCount >= FutilityMoveCounts[improving][depth];  
/*

      // sf10 Step 13. Extensions (~70 Elo)

      // Singular extension search (~60 Elo). If all moves but one fail low on a
      // search of (alpha-s, beta-s), and just one fails high on (alpha, beta),
      // then that move is singular and should be extended. To verify this we do
      // a reduced search on all the other moves but the ttMove and if the
      // result is lower than ttValue minus a margin then we will extend the ttMove.
      if (    depth >= 8 * ONE_PLY
          &&  move == ttMove
          && !rootNode
          && !excludedMove // Recursive singular search is not allowed
          &&  ttValue != VALUE_NONE
          && (tte->bound() & BOUND_LOWER)
          &&  tte->depth() >= depth - 3 * ONE_PLY
          &&  pos.legal(move))
      { Value rBeta = std::max(ttValue - 2 * depth / ONE_PLY, -VALUE_MATE);
          ss->excludedMove = move;
          value = search<NonPV>(pos, ss, rBeta - 1, rBeta, depth / 2, cutNode);
          ss->excludedMove = MOVE_NONE;
          if (value < rBeta)
              extension = ONE_PLY;
      }
      else if (    givesCheck // Check extension (~2 Elo)
               &&  pos.see_ge(move))
          extension = ONE_PLY;
*/                                          
            //1109 implement sf10 Singular extension search 
      if (    depth >= 8 
          &&  tempmove.table.move == ttMove
//          && !rootNode   //const bool rootNode = PvNode && ss->ply == 0; //1109
//          && board.ply != 0 //1109 NPV 
          && !excludedMove // Recursive singular search is not allowed
          &&  ttValue != VALUE_NONE
//          && (tte->bound() & BOUND_LOWER)
          && (hsho)  //hashmove may be from IID
          && hsho->hFlag==HASH_BETA
//          &&  tte->depth() >= depth - 3 * ONE_PLY
          && hsho->hDepth >= depth -3 
//          &&  pos.legal(move)
         )
      {
//1109          int rBeta = std::max(ttValue - 2 * depth, -VALUE_MATE);
          	int rBeta = std::max(ttValue - 50 * depth, -VALUE_MATE);	//1109 use back sf2.0 SINGULAR_MARGIN
          board.excludedMove[board.ply] = tempmove.table.move;
//          value = search<NonPV>(pos, ss, rBeta - 1, rBeta, depth / 2, cutNode);
          val = search<NonPV>(board, rBeta - 1, rBeta, depth/2, null_allow, childPv);  //1210
          board.excludedMove[board.ply] = 0;  //MOVE_NONE;
          if (val < rBeta)
              extension = 1;
      }
/*
            	// recapture
                if (capture  && node_type==NodePV &&
                    CheckExt==0 &&
                   m_hisindex >=1 &&
                	 (m_hisrecord[m_hisindex-1].htab.capture && m_hisrecord[m_hisindex-1].htab.dest==tempmove.dest)
								//&& (capture>>2) == (m_hisrecord[m_hisindex-2].capture >>2)
                  	)
                  	newdepth=depth;
*/
     if (extension ==0)
     {	
     	 if (    givesCheck // Check extension (~2 Elo)
               &&  board.see_ge(tempmove.from, tempmove.dest, 0))    //pos.see_ge(move))
          extension = 1;
     }

    // Calculate new depth for this move
      newdepth = depth - 1 + extension;

/* 
// Step 14. Pruning at shallow depth (~170 Elo)
      if (  !rootNode
          && pos.non_pawn_material(us)
          && bestValue > VALUE_MATED_IN_MAX_PLY)
      {
          if (   !captureOrPromotion
              && !givesCheck
              && (!pos.advanced_pawn_push(move) || pos.non_pawn_material() >= Value(5000)))
          { // Move count based pruning (~30 Elo)
              if (moveCountPruning)
              {   skipQuiets = true;
                  continue;
              }
              // Reduced depth of the next LMR search
              int lmrDepth = std::max(newDepth - reduction<PvNode>(improving, depth, moveCount), DEPTH_ZERO) / ONE_PLY;

              // Countermoves based pruning (~20 Elo)
              if (   lmrDepth < 3 + ((ss-1)->statScore > 0)
                  && (*contHist[0])[movedPiece][to_sq(move)] < CounterMovePruneThreshold
                  && (*contHist[1])[movedPiece][to_sq(move)] < CounterMovePruneThreshold)
                  continue;

              // Futility pruning: parent node (~2 Elo)
              if (   lmrDepth < 7
                  && !inCheck
                  && ss->staticEval + 256 + 200 * lmrDepth <= alpha)
                  continue;

              // Prune moves with negative SEE (~10 Elo)
              if (!pos.see_ge(move, Value(-29 * lmrDepth * lmrDepth)))
                  continue;
          }
          else if (   !extension // (~20 Elo)
                   && !pos.see_ge(move, -PawnValueEg * (depth / ONE_PLY)))
                  continue;
      }
*/ 
#ifdef HISCUT
if (phase==NOCAP) //4) 	// noncap + badcap
{
        //evalthreat[0]=evalthreat[1]=0;
        if ( depth <= histcut_depth //7
              //&& depth>= HistoryDepth //2 //3
        && incheck == 0
        //&& node_type != NodePV
        && newdepth < depth
        && board.piece[tempmove.dest]==0
        && piecefrom < B_KING
                 // not moves threatening the King area
		//&& ((piecefrom < B_HORSE1 || piecefrom > R_ROOK2)
    && (kingattk_incl_horse[kingindex[board.boardsq[33-board.m_side]]][tempmove.dest] ==0)
    )
    {
          if (depth>= 3 //HistoryDepth=3
            	&& gen_count >= depth + 1 // + 1
            	&& (depth * hisvptr->HistHit) < hisvptr->HistTot
           )
            	continue;

        //futility pruning
        // Check to see if this node might be futile.
        // This is true if;
        // (1) We are not in check, nor is this a checking move
        // (3) We are not capturing or promoting on this move
        // (5) We are not on the first two board.ply (board.ply>1), so that the search has had a chance to go bad.
        // If this is well below alpha then prune by a whole board.ply


#ifdef FUTILITY
        if ( depth<= fut_depth //6 //3 //2  //2
        	&& alpha > -WIN_VALUE && beta < WIN_VALUE
//        	&& (board.m_side ? -pointsum : pointsum) < beta
           )
        {
                    // Move count based pruning
//- at depth 1, prune once you have searched 7 moves
//- at depth 2, prune once you have searched 11 moves
//- at depth 3, prune once you have searched 23 moves
//- at depth 4, prune once you have searched 35 moves
// not good 86m - 15.5
//        if (gen_count >= depth * 8)		// 7, 15, 23 moves
//        	continue;

            //fprintf(stderr,"%s%d %d %d \n","futility pruning, depth, newdepth, board.ply=",depth,newdepth,ply);
            //		newdepth --;
            //

            // optimistic evaluation

            if (opt_value == INF)
            {                
                //opt_value = Evalscore() + (depth*2-1)*FUTPAWN; // x1, x3, x5
                if (depth > 3)
                {
                	evalscore = Evalscore(board);
                	futpawn_x2depth_1 = (depth*2)*FUTPAWN; //(depth*2-1)*FUTPAWN;
                }	
                opt_value = evalscore + futpawn_x2depth_1; //(depth*2-1)*FUTPAWN;
            }

            //val = opt_value;

            // pruning
            if (opt_value <= alpha)
            {
                    if (opt_value > best)
                    {
                        //best = val;
                        best = opt_value;
                        //mvBest.move = tempmove.move;
                    }
                    continue;
            }
        }
#endif      // end ifdef FUTILITY 
      }
		}	//end if phase<=1
#endif   // end ifdef HISCUT

// Step 14. Pruning at shallow depth (~170 Elo)
      if (  // !rootNode
            board.non_pawn_material(board.m_side) 
            && best > VALUE_MATED_IN_MAX_PLY
         )
      {
          if (   board.piece[tempmove.dest] == SQ_EMPTY //!captureOrPromotion
              && !givesCheck
              && board.non_pawn_material() >= 5965   //or 6465
              //&& (!pos.advanced_pawn_push(move) || pos.non_pawn_material() >= Value(5000))
              )
          {
              // Move count based pruning (~30 Elo) 
              if (moveCountPruning)
              {
                  skipQuiets = true;
                  continue;
              }              
//1023       // Reduced depth of the next LMR search
//1024              int lmrDepth = std::max(newdepth - reduction<NonPV>(improving, depth, moveCount), 0) ;  //1023
/*
              // Countermoves based pruning (~20 Elo)
              if (   lmrDepth < 3 + ((ss-1)->statScore > 0)
                  && (*contHist[0])[movedPiece][to_sq(move)] < CounterMovePruneThreshold
                  && (*contHist[1])[movedPiece][to_sq(move)] < CounterMovePruneThreshold)
                  continue;
*/
/* //1024 no hits
              // Futility pruning: parent node (~2 Elo)
              if (   lmrDepth < 7
                  && !incheck
//1023                  && ss->staticEval + 256 + 200 * lmrDepth <= alpha)
                  && board.staticEval[board.ply]  + 197 + 111 * lmrDepth <= alpha)   //1023   32000 vs 2048 
                 { lmrnpvhit1++;
                 continue;}

              // Prune moves with negative SEE (~10 Elo)
//1023              if (!pos.see_ge(move, Value(-29 * lmrDepth * lmrDepth)))
              if (!board.see_ge(tempmove.from, tempmove.dest, (-21 * lmrDepth * lmrDepth)))	//1023
                 {lmrnpvhit2++;
                continue;}
*/                
          }                  
          else if (   !extension // (~20 Elo)
                  && !board.see_ge(tempmove.from, tempmove.dest, -1357 * (depth )))  //PawnValueEg
                  continue;
        }

        // Speculative prefetch as early as possible
        //prefetch(TT.first_entry(pos.key_after(move)));

        // Check for legality just before making the move
        //if (!rootNode && !pos.legal(move))
        //{
        //  ss->moveCount = --moveCount;
        //  continue;
        //}

        // Update the current move (this must be done after singular extension search)
        board.currentMove[board.ply].move = tempmove.table.move;
        
       // capture = makemove(tempmove.table,
       //  (incheck
			 //		|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
			//		|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
		//);
		// Step 15. Make the move
        if (incheck
					|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
					|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
        		capture = board.makemove(tempmove.table, 1);
        else
		        capture = board.makemove(tempmove.table, 0);

        if (capture < 0)
        { board.currentMove[board.ply].move = 0;
        	board.moveCount[board.ply] = --moveCount;   //1024
        	continue;
        }
        if (capture==0)
        {
        	movesSearched[noncap_gen_count].table.move = tempmove.table.move;
        	movesSearched[noncap_gen_count].tabval = piecefrom;
        	noncap_gen_count++;
        }
        gen_count++;




        int reduced=0;
if (phase==NOCAP) //4) 
{
#ifdef HISTPRUN
		if (depth>= HistoryDepth && !incheck && newdepth < depth && gen_count >= 3 //3 //HistoryMoveNb
			&& (capture==0)
			&& board.m_hisrecord[board.m_hisindex-1].htab.Chk==0
			 )
				{
						//if ((board.EvalSide<0>() < THREAT_MARGIN) //== 0
      		 	//	 && (board.EvalSide<1>() < THREAT_MARGIN) //== 0
					 //)
      		 	{
						//if (gen_count >= 11 && depth >= 7)  //NonPV LMR 86L
						if (gen_count >= 11 && depth >= 6)  //NonPV LMR 86m
						{	reduced=2;
					  	newdepth -= 2;
						}
						else
						{	reduced=1;
							newdepth --;
						}
						//newdepth -= reduced;
					}
				}
#endif
} // end if phase==NONCAP

//1119 ...to be continued ...

// Step 16. Reduced depth search (LMR). If the move fails high it will be
      // re-searched at full depth.
         if (    depth >= 3
          &&  moveCount > 1
          && (capture==0 || moveCountPruning))
         {
           //int r = reduction<PvNode>(improving, depth, moveCount);
           reduced += reduction<PvNode>(improving, depth, moveCount);
          
          // Decrease reduction if opponent's move count is high (~10 Elo)
//1024          if ((ss-1)->moveCount > 15)
//1024              r -= ONE_PLY;
          if (board.moveCount[board.ply-1] > 15)
          	 //r -= 1; 
          	 reduced -= 1;

          if (capture==0)   //(!captureOrPromotion)
          {
/*          
              // Decrease reduction for exact PV nodes (~0 Elo)
              //1024 if (pvExact)
              //1024    r -= ONE_PLY;

              // Increase reduction if ttMove is a capture (~0 Elo)
              //1024 if (ttCapture)
              //1024   r += ONE_PLY;
*/
              // Increase reduction for cut nodes (~5 Elo)
              if (null_allow)    //(cutNode)
                  //r += 2;  //r += 2 * ONE_PLY;
                  reduced += 2;

/*
              // Decrease reduction for moves that escape a capture. Filter out
              // castling moves, because they are coded as "king captures rook" and
              // hence break make_move(). (~5 Elo)
              else if (    type_of(move) == NORMAL
                       && !pos.see_ge(make_move(to_sq(move), from_sq(move))))
                  r -= 2 * ONE_PLY;

              ss->statScore =  thisThread->mainHistory[us][from_to(move)]
                             + (*contHist[0])[movedPiece][to_sq(move)]
                             + (*contHist[1])[movedPiece][to_sq(move)]
                             + (*contHist[3])[movedPiece][to_sq(move)]
                             - 4000;

              // Decrease/increase reduction by comparing opponent's stat score (~10 Elo)
              if (ss->statScore >= 0 && (ss-1)->statScore < 0)
                  r -= ONE_PLY;

              else if ((ss-1)->statScore >= 0 && ss->statScore < 0)
                  r += ONE_PLY;

              // Decrease/increase reduction for moves with a good/bad history (~30 Elo)
              r -= ss->statScore / 20000 * ONE_PLY;
*/              
          }         
            //     Depth d = std::max(newDepth - std::max(r, DEPTH_ZERO), ONE_PLY);
            int d = std::max(newdepth - std::max(reduced, 0), 1);  

            //value = -search<NonPV>(pos, ss+1, -(alpha+1), -alpha, d, true);
            val = -search<NonPV>(board, -(alpha+1), -alpha, d, NULL_YES, childPv); //1210    
            //doFullDepthSearch = (value > alpha && d != newDepth);
            doFullDepthSearch = (val > alpha && d != newdepth);
      }
      else          
            doFullDepthSearch = !PvNode || moveCount > 1;
/*
      // Step 17. Full depth search when LMR is skipped or fails high
      if (doFullDepthSearch)
          value = -search<NonPV>(pos, ss+1, -(alpha+1), -alpha, newDepth, !cutNode);

      // For PV nodes only, do a full PV search on the first move or after a fail
      // high (in the latter case search only if value < beta), otherwise let the
      // parent node fail low with value <= alpha and try another move.
      if (PvNode && (moveCount == 1 || (value > alpha && (rootNode || value < beta))))
      {
          (ss+1)->pv = pv;
          (ss+1)->pv[0] = MOVE_NONE;

          value = -search<PV>(pos, ss+1, -beta, -alpha, newDepth, false);
      }

      // Step 18. Undo move
      pos.undo_move(move);        
*/         


//              if (newdepth <= 0)
//                val = -quiesCheck(board, -beta+1, newdepth); //1007, qcheck_depth);
//              else
              if (doFullDepthSearch)
              {                
                //1119 val = -search<NonPV>(board, -beta, -beta+1, newdepth, NULL_YES); //for nonpv, alpha=beta-1
                val = -search<NonPV>(board, -(alpha+1), -alpha, newdepth, !null_allow, childPv);  //1210
                        // history-pruning re-search
#ifdef HISTPRUN
                if (reduced && val >=beta)	//was >alpha
                {
            	     newdepth+=reduced;                
                   val = -search<NonPV>(board, -beta, -beta+1, newdepth, NULL_YES, childPv);  //1210
                }
#endif
						  }
						  
			// For PV nodes only, do a full PV search on the first move or after a fail
      // high (in the latter case search only if value < beta), otherwise let the
      // parent node fail low with value <= alpha and try another move.
      if (PvNode && (moveCount == 1 || (val > alpha && (val < beta))))
      {
         // (ss+1)->pv = pv;
         // (ss+1)->pv[0] = MOVE_NONE;

          val = -search<PV>(board, -beta, -alpha, newdepth, NULL_NO, childPv); //1210
      }		  

        board.unmakemove();

        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;

      // Step 19. Check for a new best move
      /*
      if (value > bestValue)
      {   bestValue = value;
          if (value > alpha)
          {   bestMove = move;
              if (PvNode && !rootNode) // Update pv even in fail-high case
                  update_pv(ss->pv, move, (ss+1)->pv);
              if (PvNode && value < beta) // Update alpha! Always alpha < beta
                  alpha = value;
              else
              {   assert(value >= beta); // Fail high
                  ss->statScore = 0;
                  break;
              }
          }
      }
      if (move != bestMove)
      {   if (captureOrPromotion && captureCount < 32)
              capturesSearched[captureCount++] = move;
          else if (!captureOrPromotion && quietCount < 64)
              quietsSearched[quietCount++] = move;
      }
      */ 
                    // 12. Alpha-Beta边界判定        
        if (val > best)
        {   best = val;
        	  if (val > alpha)
            {   mvBest.move = tempmove.table.move;                
                nHashFlag = HASH_PV;
                if (PvNode && val < beta) // Update alpha! Always alpha < beta //1210
                {   
                   alpha = val;
                   //if (pvNode)          //1210
                   {
                     pv[0] = mvBest.move;  //1210
                     memcpy(pv+1, childPv, (MAX_PLY - 1) * sizeof(move_t));  //1210 may use memcpy for bug-free
                     pv[MAX_PLY - 1] = 0; //NULL_MOVE; //1210 to be sure we always end will a sential end-of-list 

//1212                    for (int i = 0; i < MAX_PLY - ply; i++)  //1212 1210
//1212                        if (!(pv[i + 1] = childPv[i]))       //1212 1210 note: copy done in if = 
//1212                            break;                           //1212 1210
                   }
                }   
                else
                { //   assert(value >= beta); // Fail high
                  //   ss->statScore = 0;
                  goto end_phaseloop; //break;
                }
            }
            //if (val >= beta)  //1119 defer to step 20 
            //{ if (!excludedMove)  //1109
            //    RecordHash(HASH_BETA, depth, val, tempmove.table.move, board); //, nSingular);
            //    return val;
            //}
        }
        
      } // end for i
    }	// end for phase
end_phaseloop:
// The following condition would detect a stop only after move loop has been
    // completed. But in this case bestValue is valid because we have fully
    // searched our subtree, and we can anyhow save the result in TT.
    /*
       if (Threads.stop)
        return VALUE_DRAW;
    */
//1210    if (board.m_timeout) //sf191
//1210    	return best;
//-----------------------------------------------
// Step 20. Check for mate and stalemate
    // All legal moves have been searched and if there are no legal moves, it
    // must be a mate or a stalemate. If we are in a singular extension search then
    // return a fail low score.
//    assert(moveCount || !inCheck || excludedMove || !MoveList<LEGAL>(pos).size());
/*
    if (!moveCount)
        bestValue = excludedMove ? alpha
                   :     inCheck ? mated_in(ss->ply) : VALUE_DRAW;
    else if (bestMove)
    {   // Quiet best move: update move sorting heuristics
        if (!pos.capture_or_promotion(bestMove))
            update_quiet_stats(pos, ss, bestMove, quietsSearched, quietCount,
                               stat_bonus(depth + (bestValue > beta + PawnValueMg ? ONE_PLY : DEPTH_ZERO)));
        update_capture_stats(pos, bestMove, capturesSearched, captureCount, stat_bonus(depth + ONE_PLY));
        // Extra penalty for a quiet TT move in previous ply when it gets refuted
        if ((ss-1)->moveCount == 1 && !pos.captured_piece())
            update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, -stat_bonus(depth + ONE_PLY));
    }
    // Bonus for prior countermove that caused the fail low
    else if (   (depth >= 3 * ONE_PLY || PvNode)
             && !pos.captured_piece()
             && is_ok((ss-1)->currentMove))
        update_continuation_histories(ss-1, pos.piece_on(prevSq), prevSq, stat_bonus(depth));
    if (PvNode)
        bestValue = std::min(bestValue, maxValue);
    if (!excludedMove)
        tte->save(posKey, value_to_tt(bestValue, ss->ply),
                  bestValue >= beta ? BOUND_LOWER :
                  PvNode && bestMove ? BOUND_EXACT : BOUND_UPPER,
                  depth, bestMove, pureStaticEval);
    assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);
    return bestValue;
*/      	
    if (!moveCount) //if (gen_count==0)
    {	//1109        return board.ply-INF;	// No moves in this pos, lost by checkmate or stalemate
        best = excludedMove ? alpha
         :  -VALUE_MATE + board.ply;   //1113 cannot return 0 else rook bug  
    }
    // 13. 更新置换表、历史表和杀手着法表
    else if (mvBest.move)
    {                 
        if (capture==0)  //1016  && node_type==NodePV && !incheck)
        {
        	//1024 update_quiet_stats()  //killers, hist, countermove
//          if (best >= WIN_VALUE)
//              g_matekiller[board.ply].move = mvBest.move;
//          else
          { 	  
            if (g_killer[0][board.ply].move != mvBest.move)
            {
                g_killer[1][board.ply].move = g_killer[0][board.ply].move;
                g_killer[0][board.ply].move = mvBest.move;
            }    
              //hisvptr = &(m_his_table[mvBest.dest][PIECE_IDX16(board.piece[mvBest.from])]);
              //hisvptr = &(m_his_table[PIECE_IDX(board.piece[mvBest.from])][nFile(mvBest.dest)][nRank(mvBest.dest)]);
                hisvptr = &(m_his_table[board.thd_id][PIECE_IDX(board.piece[mvBest.from])][mvBest.dest]);   //1017
                update_HistVal(hisvptr, depth, board.thd_id);         
                update_history(hisvptr, depth, movesSearched, noncap_gen_count, board.thd_id);                    
                if (board.ply >0 && (board.currentMove[board.ply -1].move !=0))
                   counterMoves[board.thd_id][PIECE_IDX(board.piece[prevSqdest])][prevSqdest].move = mvBest.move;  //1017
          }
        }    
    }
    
    //1119 else if (best <= old_alpha)
    //	if (!excludedMove)  //1109
    //	   RecordHash(nHashFlag, depth, best, mvBest.move, board); //, 0);
    if (PvNode)
        best = std::min(best, INF); //maxValue);
	if (!excludedMove)
	{	  //tte->save(posKey, value_to_tt(bestValue, ss->ply),
		//          bestValue >= beta ? BOUND_LOWER :
		//          PvNode && bestMove ? BOUND_EXACT : BOUND_UPPER,
		//          depth, bestMove, pureStaticEval);
		if (best >= beta)
			nHashFlag = HASH_BETA;
		else if (PvNode && mvBest.move)
			nHashFlag = HASH_PV;
		else
			nHashFlag = HASH_ALPHA;
		RecordHash(nHashFlag, depth, best, mvBest.move, board);
	}

    return best; 

}


template <int NT>
int Engine::quiesCheck(Board &board, int alpha, int beta,  int qdepth)  //1007, int check_depth)
{   constexpr bool PvNode = (NT == PV);
    int val, incheck;
    int best, capture, bestmove; //hashflag,  //, kingidx;
    int futilityValue, futilityBase, old_alpha, piecedest;
    int evasionPrunable, moveCount;
    int ttdepth; //1021
    
//1212    int ply = board.ply; //1212
    // Allocate PV for the child node  //1210
//    move_t childPv[MAX_PLY]; //1212 cannot alloc dynamic[MAX_PLY - ply], alloc [MAX_PLY] instead
    
    // Terminate current PV  //1210
//    if (PvNode)              //1210
//        pv[0] = 0;           //1210
        
//1004    bool givesCheck;
incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
// 2. 和棋裁剪；
    	// If this is too deep then just return the stand-pat score to avoid long quiescence searches.
        if ( ((board.bitpiece & 0xfff00ffc) == 0) 
        		|| (qdepth <= -MAXQDEPTH) )  //test maxqdepth instead of MAXEXT or MAX_PLY
        {
        	  //if (PvNode)          //1210 
            //    childPv[0] = 0;  //1210
            return Evalscore(board);
            //1113 return (qdepth <= -MAXQDEPTH && !incheck) ? Evalscore(board) : 0;
        }
    // 1. Return if there is a mate line bad enough
    if (board.ply-INF >= beta)
    	return (board.ply-INF);

    val=board.checkloop(1);
    if (val)
    	return val;

#ifdef QHASH
    // Transposition table lookup, only when not in PV
    //if ( (qdepth >=check_depth) ) // && !incheck)   //sf191 88i
    //{
    	/*
    	if (PVNode)
        hashmove = ProbeMoveQ(board);
      else
    	{
    	 // Decide whether or not to include checks: this fixes also the type of
    // TT entry depth that we are going to use. Note that in qsearch we use
    // only two types of depth in TT: DEPTH_QS_CHECKS or DEPTH_QS_NO_CHECKS.
    ttDepth = inCheck || depth >= DEPTH_QS_CHECKS ? DEPTH_QS_CHECKS     ( 0)
                                                  : DEPTH_QS_NO_CHECKS; (-1)
    // Transposition table lookup
    posKey = pos.key();
    tte = TT.probe(posKey, ttHit);
    ttValue = ttHit ? value_from_tt(tte->value(), ss->ply) : VALUE_NONE;
    ttMove = ttHit ? tte->move() : MOVE_NONE;

    if (  !PvNode
        && ttHit
        && tte->depth() >= ttDepth        
        && ttValue != VALUE_NONE // Only in case of TT access race
        && (ttValue >= beta ? (tte->bound() & BOUND_LOWER)
                            : (tte->bound() & BOUND_UPPER)))
        return ttValue;
    	*/
    	  HashStruct *hsho;
    	  ttdepth = (incheck || qdepth >= 0) ? 0 : -1;   //1021
    	  int hashmove = 0;
    	  int hashdepth = 0;	  
    if (  !PvNode)
    {	  	
//1129        val = ProbeHashQ(beta, hashmove, board, hashdepth);  //0
        val = ProbeHash(qdepth, beta, NULL_NO, hsho, board);   //1212 chg from NULL_YES
        if (val != UNKNOWN_VALUE)
        { //if (ttdepth == 0 || hsho->hDepth >= ttdepth + p_movenum)  //1210  
          { 
        	  return val;
        	}       
        }
    }
//1129 else // PvNode
//1129    	hashmove = ProbeMoveQ(board);
  
    else //PVNode
   	    hsho = ProbeMove(board); 
    
    if (hsho) 
    {	 hashmove = get_hmv(hsho->hmvBest);
       //ttValue = value_from_tt(hsho->hVal, board.ply);
    }	



    
#endif

    old_alpha = alpha;
    MoveTabStruct movetab[111]; //, ncapmovetab[64];
    MoveTabStruct tempmove;
//    best = -INF;  //927  need init?
    moveCount = 0;
    
    if (incheck)
    {
        best = futilityBase = -INF;   //927
    } 
    else	
    {
        best = Evalscore(board);
        if (best >= beta)
        {
            if (hashmove==0)  //
               RecordHash(HASH_BETA, 0, best, 0, board); //sf191 88e //2019k chg back from RecordHashQ
               //1021 RecordHash(HASH_BETA, 0, val, 0, board);
            return best; //beta;
        }
        //if (PVNode && val > alpha) //sf191 //88e
            if (PvNode && best > alpha) 
                alpha = best;        
        futilityBase = best + DELTA_MARGIN;        
    }
bestmove=0;
int num=0;
int qphase;
//int gen_count=0;
for (qphase=3; qphase--;)	// 2=hashmove, 1=capture, 0=genchk
{
    switch (qphase)
    {
    	case 2:	//hashmove
    		if (hashmove==0)
    			num=0;
    		else
    			num=1;
    		break;

    	case 1:	//capture
    		if (!incheck)
    		{
    				num = (board.m_side ? board.GenCapQS<1>(&movetab[0]) : board.GenCapQS<0>(&movetab[0]));
    					//1021 ifnot incheck and no capture and qdepth < check_depth, return QS to avoid long QS
    				if (num==0 && qdepth < check_depth)  //1021  //1123
    					return best;	                     //1021  //1123	
    	  }
    	  else
    	  {
    	  	  num=board.GenChkEvasCap(&movetab[0], incheck);    
    	  }
    		break;
    		
// sf10 to search the QS moves. Because the depth is <= 0 here, only captures,
// queen promotions and checks (only if depth >= DEPTH_QS_CHECKS) will be generated.
      case 0:	//genchk
    		if (incheck==0)
    		{
        	if (qdepth >=check_depth)
            //&& alpha < beta
            { num=board.generate_checks(&movetab[0]); 
            }
        	else
            num=0;
        }
    		else
    		{
        		num=board.GenChkEvasNCap(&movetab[0], incheck);
    		}
    		break;
    }

    //Quicksort(0, num-1, movetab); //table, tabval);
    //Insertsort(num, movetab);

    for (int i=0;i<num;++i)
    {
      switch (qphase)
     {
    	 case 2:	//hashmove
      	tempmove.table.move = hashmove;
      	//if (tempmove.table.move==0
        //   ||
        if (board.LegalKiller(tempmove.table)==0
           )
             continue;
        break;
    	
       case 1:	//capture
      	tempmove.tabentry = GetNextMove(i, num, movetab);
      	if (tempmove.table.move==hashmove)
        		continue;
		    break;
		  
       case 0:	//genchk
      	tempmove.tabentry = GetNextMove(i, num, movetab);

      	if (tempmove.table.move==hashmove)
        		continue;
        break;
     } // end switch qphase
        moveCount++;
        piecedest = board.piece[tempmove.dest];	
        //1003
        
        //1004	givesCheck = board.gives_check(tempmove.from, tempmove.dest);
//927 futility pruning ref sf10
        if ( !incheck 
        	  && (qphase!=0 && !(board.gives_check(tempmove.from, tempmove.dest)) )    //1004  !givesCheck
        	  && futilityBase > -WIN_VALUE)
        { 
        	//piecedest = board.piece[tempmove.dest];	
        	futilityValue = futilityBase + abs(pointtable[PIECE_IDX(piecedest)][nRank(tempmove.dest)][nFile(tempmove.dest)]);
        	if (futilityValue <= beta-1) //alpha)
          {
              best = std::max(best, futilityValue);
              continue;
          }
//1001      if (futilityBase <= beta-1) //alpha)     //sf10 && !pos.see_ge(move, VALUE_ZERO + 1))
          	if ((futilityBase <= beta-1) && !board.see_ge(tempmove.from, tempmove.dest,  1))
          { 

/*    	 
          	 if (piecedest < B_HORSE1
        	      //|| (board.piece[tempmove.from]>=B_ROOK1
        	      || ((board.piece[tempmove.from]>>2)==ROOK  //exclude king capture for SEE()
        	      && piecedest < B_ROOK1))
             {
//        	      if (board.see(tempmove.from, tempmove.dest) < 0) //1 for chess promotion, castling, en passant
        				{ best = std::max(best, futilityBase);
        				  continue;
        				}
             }          	
*/          


             best = std::max(best, futilityBase);
             continue;
          
          } 
        } 

/*
      // Detect non-capture evasions that are candidates to be pruned
      evasionPrunable =    inCheck
                       &&  (depth != DEPTH_ZERO || moveCount > 2)
                       &&  bestValue > VALUE_MATED_IN_MAX_PLY
                       && !pos.capture(move);
                       
      // Don't search moves with negative SEE values
      if (  (!inCheck || evasionPrunable)
          && !pos.see_ge(move))
          continue;      
*/          
        // Detect non-capture evasions that are candidates to be pruned
		  if ((incheck)
			&& (qdepth != 0 || moveCount > 2) //>2
			&& best > -INF + 128   //VALUE_MATED_IN_MAX_PLY
			&& (piecedest == 0))        //!pos.capture(move);
			   evasionPrunable = 1;
			else
				 evasionPrunable=0;
				 
        // Don't search moves with negative SEE values
        //if (  (!incheck || evasionPrunable))
        if (  (!incheck || evasionPrunable) && !board.see_ge(tempmove.from, tempmove.dest,0))
      	{     continue;  //1002	
      	
          // && !pos.see_ge(move))
/*          
             if (piecedest < B_HORSE1
        	      //|| (board.piece[tempmove.from]>=B_ROOK1
        	      || ((board.piece[tempmove.from]>>2)==ROOK  //exclude king capture for SEE()
        	      && piecedest < B_ROOK1))
             {
        	      //int see_val =board.see(tempmove.from, tempmove.dest);
        	      //if (evasionPrunable)
        	      //	{
    //printf("***incheck=%d, qdepth=%d, moveCount=%d, best=%d, evasionPrunable=%d\n",incheck, qdepth, moveCount, best, evasionPrunable);   	      		
		//printf("***from=%d, dest=%d, piecefrom=%d, piecedest=%d, SEE=%d\n",tempmove.from, tempmove.dest, board.piece[tempmove.from], piecedest, see_val);
        				//}
        	      //if (see_val < 0)	
//        	      if (board.see(tempmove.from, tempmove.dest) < 0)
        	      { 	
        	      	continue;
        			  }
             } 
*/                                  
        }

        capture = board.makemove(tempmove.table,
        	 (incheck
						|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
						|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
						);
				/*
				if (incheck
					|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
					|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
        		capture = board.makemove(tempmove.table, 1);
        else
		        capture = board.makemove(tempmove.table, 0);
        */
				if (capture < 0)
				{ moveCount--; 	
        	continue;
        }
				//gen_count++;

        //if (capture >=B_KING)
        //	val = INF-ply;
        //else
        //val = -quiesCheck(-beta, -alpha, qdepth-1, check_depth);
        
        
        val = -quiesCheck<NT>(board, -beta, -alpha, qdepth-1);  //1007, check_depth); //1210
        board.unmakemove();

        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;
/* ref sf10 
// Check for a new best move
      if (value > bestValue)
      {
          bestValue = value;

          if (value > alpha)
          {
              bestMove = move;

              if (PvNode) // Update pv even in fail-high case
                  update_pv(ss->pv, move, (ss+1)->pv);

              if (PvNode && value < beta) // Update alpha here!
                  alpha = value;
              else
                  break; // Fail high
          }
       }
*/
        if (val >= beta)
        {
#ifdef QHASH

					//if ((	qdepth >=check_depth) && !incheck)   //1020
            //1129 RecordHashQ(HASH_BETA, ttdepth, val, tempmove.table.move, board);
            
            RecordHash(HASH_BETA, qdepth, val, tempmove.table.move, board);   //1210
            //1021 int recqdepth = (qdepth==0 ? 0 : 1-(p_movenum&31) );
            //1021 RecordHash(HASH_BETA, recqdepth, val, tempmove.table.move, board);
          //  else QShashrecmiss++;
#endif
            //printf("beta cutoff val=%d\n", val);
            return val; // beta;
        }
        if (val > best)
        {
            best = val;
            if (val > alpha)
            {
                //1123 sf10 not for NonPV alpha = val;
                bestmove = tempmove.table.move;
                
                
                
//1021 sf10   if (PvNode) // Update pv even in fail-high case
//1021                  update_pv(ss->pv, move, (ss+1)->pv);

              if (PvNode && val < beta) // Update alpha here!
              {	
                 alpha = val;
                 
              }
              else
                  goto end_qphaseloop; //break; // Fail high   
            } 
        }

    }	// end for i (< num)

} // end for qphase
end_qphaseloop:
/* sf10  
// All legal moves have been searched. A special case: If we're in check
    // and no legal moves were found, it is checkmate.
    if (inCheck && bestValue == -VALUE_INFINITE)
        return mated_in(ss->ply); // Plies to mate from the root

    tte->save(posKey, value_to_tt(bestValue, ss->ply),
              bestValue >= beta ? BOUND_LOWER :
              PvNode && bestValue > oldAlpha  ? BOUND_EXACT : BOUND_UPPER,
              ttDepth, bestMove, ss->staticEval);

    assert(bestValue > -VALUE_INFINITE && bestValue < VALUE_INFINITE);

    return bestValue;
*/    

// 8. 处理
    if (incheck && (best == -INF))  //1123 add incheck
//			if (gen_count==0 && incheck)	//mate!
			{
        //printf("ply-INF val=%d\n", board.ply-INF);
        return board.ply - INF;
    	}

//sf10
//tte->save(posKey, value_to_tt(bestValue, ss->ply),
//              bestValue >= beta ? BOUND_LOWER :
//              PvNode && bestValue > oldAlpha  ? BOUND_EXACT : BOUND_UPPER,
//              ttDepth, bestMove, ss->staticEval);

#ifdef QHASH
//-------- original qpv
    //if ((qdepth >=check_depth) && !incheck)  //1020 //1123
    if (qdepth >=check_depth) //1123
    {
        if (PvNode && best > old_alpha) 
        {
        	  //1129 RecordHashQ(HASH_PV, ttdepth, best, bestmove, board);
                   RecordHash(HASH_PV, ttdepth, best, bestmove, board);   //1210         
        }
        else
        {
            //1129 RecordHashQ(HASH_ALPHA, ttdepth, best, bestmove, board); //1021 
            //1212 if (qdepth >= 0) //1210 
                   RecordHash(HASH_ALPHA, ttdepth, best, bestmove, board);   //1210 
        }
    }
#endif

    
    return best;
}

////////////////////////////////////////////////////////////////////////////
//界面接口
//void  Engine::IMakeMove(int from,int dest)

void  Engine::IMakeMove(Board &board, MoveStruct &moveS)
{
    //makemove(moveS);
    board.makemove(moveS, 0); // , 0);
}

void  Engine::printBestmove(int bestmove)
{
    char str[5];
    MoveStruct tempmove;
    tempmove.move=bestmove;
    int from=tempmove.from;
    int dest=tempmove.dest;
    p_movenum++;
#ifdef THDINFO    
    printf("     **p_movenum= #%d:\n", p_movenum);
    fflush(stdout);
#endif    
    printf("bestmove %s\n",MoveStr(str,from,dest) );
    fflush(stdout);
}



void  Engine::ISearch()
{
//1016 debug
//  PVcmhit=NPVcmhit=0; 
//  lmrpvhit1 = lmrpvhit2 = lmrnpvhit1 = lmrnpvhit2 = 0;   
    //1102 rhit1=rhit2=rhit3=0;
//  PVmatekillerhit=NPVmatekillerhit=0;  
//  PVkillerhit=NPVkillerhit=0; 
//    QShashhit = QShashmiss = 
//    hashhit = 0;
//    QSPVhashrecmiss=0;
//    QShashrecmiss=0;
    //char str[5];
    MoveStruct tempmove;
    tempmove.move = searchRoot();
//1016 debug  
//1102    printf("*** rhit1=%d, rhit2=%d, rhit3=%d\n", rhit1,rhit2,rhit3);
//printf("*** LMRPV hit1=%d, hit2=%d, LMRNPV hit1=%d, hit2=%d\n", lmrpvhit1,lmrpvhit2,lmrnpvhit1,lmrnpvhit2);
//  printf("*** PV hashhit=%d, NPV hashhit=%d\n", PVhashhit, NPVhashhit);         
//  printf("*** PV matekillerhit=%d, NPV matekillerhit=%d\n", PVmatekillerhit, NPVmatekillerhit);    
//  printf("*** PV killerhit=%d, NPV killerhit=%d\n", PVkillerhit, NPVkillerhit);
//  printf("*** PV cmhit=%d, NPV cmhit=%d\n", PVcmhit, NPVcmhit);    
//    printf("*** QSPVhashrecmiss=%d, QShashrecmiss=%d\n", QSPVhashrecmiss, QShashrecmiss);    
//  printf("*** hashhit=%d\n",  hashhit);
  // QShashhit=%d, QShashmiss=%d\n", hashhit, QShashhit,QShashmiss); 
    int from=tempmove.from;
    int dest=tempmove.dest;

    //if (board.m_timeout==0)
    	printBestmove(tempmove.move);
    /*
    p_movenum++;
    printf("info #%d:\n", p_movenum);
    fflush(stdout);
    printf("bestmove %s\n",MoveStr(str,from,dest) );
    fflush(stdout);
    */
		Clear_Killer(); //1891c - same with Clear_Hist
		Clear_Hist();
//    HashAging();
//    ClearHash();
//makemove bestmove
if (board.piece[dest])
    board.boardsq[board.piece[dest]]=SQ_EMPTY;
board.boardsq[board.piece[from]] = dest;
//board.piece[dest]=board.piece[from];
//board.piece[from]=EMPTY;

//memcpy(prev_piece, board.piece, sizeof(prev_piece));
memcpy(prev_boardsq, board.boardsq, sizeof(prev_boardsq));
//    return bestmove;
}
//////////////////////////////////////////////////////////////////////////


void Engine::init_parms()
{	//1892q - debug init VPE VPE=56, VPER=127, VBE=54, VEE=53
	//20190919 - clop VHE, VEE, VPER,
	//printf("*** VNE=%d, VEE=%d, VPER=%d\n", VNE, VEE, VPER);
	//printf("*** VR=%d, VC=%d, VN=%d\n", VR,VC,VN);		
	//printf("*** VR=%d, VRE=%d\n", VR,VRE);	
	//fflush(stdout);
	
		// re-init BasicValues, etc.
	//BasicValues = {VP,VE,VN,VC,VR, VPE,VEE,VNE,VCE,VRE};
	
	BasicValues[0]=VP; 
	BasicValues[1]=VE;
	BasicValues[2]=VN;
	BasicValues[3]=VC;
	BasicValues[4]=VR;
	

	BasicValues[5]=VPE;
	BasicValues[6]=VEE;
	BasicValues[7]=VNE;
	BasicValues[8]=VCE;
	BasicValues[9]=VRE;
	//PIECE_VALUE_side =
  //  {0,0,-VP,VP,-VP,VP,-VP,VP,-VP,VP,-VP,VP,-VB,VB,-VB,VB,-VE,VE,-VE,VE,-VNC,VNC,-VNC,VNC,-VNC,VNC,-VNC,VNC,-VR,VR,-VR,VR,-9999,9999,
  //  0,0,-VPR,VPR,-VPR,VPR,-VPR,VPR,-VPR,VPR,-VPR,VPR};
 PIECE_VALUE_side[0]=0;
 PIECE_VALUE_side[1]=0;
 PIECE_VALUE_side[2]= VP;  //1002
 PIECE_VALUE_side[3]=VP;
 PIECE_VALUE_side[4]= VP;
 PIECE_VALUE_side[5]=VP;
 PIECE_VALUE_side[6]= VP;
 PIECE_VALUE_side[7]=VP; 
 PIECE_VALUE_side[8]= VP;
 PIECE_VALUE_side[9]=VP;
 PIECE_VALUE_side[10]= VP;
 PIECE_VALUE_side[11]=VP;
 
 PIECE_VALUE_side[12]= VB;
 PIECE_VALUE_side[13]=VB;
 PIECE_VALUE_side[14]= VB;
 PIECE_VALUE_side[15]=VB;
 PIECE_VALUE_side[16]= VE;
 PIECE_VALUE_side[17]=VE;
 PIECE_VALUE_side[18]= VE;
 PIECE_VALUE_side[19]=VE;


 PIECE_VALUE_side[20]= VN;
 PIECE_VALUE_side[21]=VN;
 PIECE_VALUE_side[22]= VN;
 PIECE_VALUE_side[23]=VN;
 PIECE_VALUE_side[24]= VC;
 PIECE_VALUE_side[25]=VC;
 PIECE_VALUE_side[26]= VC;
 PIECE_VALUE_side[27]=VC;
 
 PIECE_VALUE_side[28]= VR;
 PIECE_VALUE_side[29]=VR;
 PIECE_VALUE_side[30]= VR;
 PIECE_VALUE_side[31]=VR;

 PIECE_VALUE_side[32]= 7999; //1101 9999;
 PIECE_VALUE_side[33]= 7999; //1101 9999;
 PIECE_VALUE_side[34]=0;
 PIECE_VALUE_side[35]=0;

 PIECE_VALUE_side[36]= VPR;
 PIECE_VALUE_side[37]=VPR;
 PIECE_VALUE_side[38]= VPR;
 PIECE_VALUE_side[39]=VPR;
 PIECE_VALUE_side[40]= VPR;
 PIECE_VALUE_side[41]=VPR; 
 PIECE_VALUE_side[42]= VPR;
 PIECE_VALUE_side[43]=VPR;
 PIECE_VALUE_side[44]= VPR;
 PIECE_VALUE_side[45]=VPR;
 
 //PIECE_VALUE_side_ENDGAME =
 //   {0,0,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VPE,VPE,-VBE,VBE,-VBE,VBE,-VEE,VEE,-VEE,VEE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VNCE,VNCE,-VRE,VRE,-VRE,VRE,-9999,9999,
 //   0,0,-VPER,VPER,-VPER,VPER,-VPER,VPER,-VPER,VPER,-VPER,VPER};
 PIECE_VALUE_side_ENDGAME[0]=0;
 PIECE_VALUE_side_ENDGAME[1]=0;
 PIECE_VALUE_side_ENDGAME[2]= VPE;  //1002
 PIECE_VALUE_side_ENDGAME[3]=VPE;
 PIECE_VALUE_side_ENDGAME[4]= VPE;
 PIECE_VALUE_side_ENDGAME[5]=VPE;
 PIECE_VALUE_side_ENDGAME[6]= VPE;
 PIECE_VALUE_side_ENDGAME[7]=VPE; 
 PIECE_VALUE_side_ENDGAME[8]= VPE;
 PIECE_VALUE_side_ENDGAME[9]=VPE;
 PIECE_VALUE_side_ENDGAME[10]= VPE;
 PIECE_VALUE_side_ENDGAME[11]=VPE;
 
 PIECE_VALUE_side_ENDGAME[12]= VBE;
 PIECE_VALUE_side_ENDGAME[13]=VBE;
 PIECE_VALUE_side_ENDGAME[14]= VBE;
 PIECE_VALUE_side_ENDGAME[15]=VBE;
 PIECE_VALUE_side_ENDGAME[16]= VEE;
 PIECE_VALUE_side_ENDGAME[17]=VEE;
 PIECE_VALUE_side_ENDGAME[18]= VEE;
 PIECE_VALUE_side_ENDGAME[19]=VEE;


 PIECE_VALUE_side_ENDGAME[20]= VNE;
 PIECE_VALUE_side_ENDGAME[21]=VNE;
 PIECE_VALUE_side_ENDGAME[22]= VNE;
 PIECE_VALUE_side_ENDGAME[23]=VNE;
 PIECE_VALUE_side_ENDGAME[24]= VCE; //-VNCE;
 PIECE_VALUE_side_ENDGAME[25]=VCE; //VNCE;
 PIECE_VALUE_side_ENDGAME[26]= VCE; //-VNCE;
 PIECE_VALUE_side_ENDGAME[27]=VCE;  //VNCE;
 
 PIECE_VALUE_side_ENDGAME[28]= VRE;
 PIECE_VALUE_side_ENDGAME[29]=VRE;
 PIECE_VALUE_side_ENDGAME[30]= VRE;
 PIECE_VALUE_side_ENDGAME[31]=VRE;

 PIECE_VALUE_side_ENDGAME[32]= 7999; //1101 9999;
 PIECE_VALUE_side_ENDGAME[33]=7999;  //1101 9999;
 PIECE_VALUE_side_ENDGAME[34]=0;
 PIECE_VALUE_side_ENDGAME[35]=0;

 PIECE_VALUE_side_ENDGAME[36]= VPER;
 PIECE_VALUE_side_ENDGAME[37]=VPER;
 PIECE_VALUE_side_ENDGAME[38]= VPER;
 PIECE_VALUE_side_ENDGAME[39]=VPER;
 PIECE_VALUE_side_ENDGAME[40]= VPER;
 PIECE_VALUE_side_ENDGAME[41]=VPER; 
 PIECE_VALUE_side_ENDGAME[42]= VPER;
 PIECE_VALUE_side_ENDGAME[43]=VPER;
 PIECE_VALUE_side_ENDGAME[44]= VPER;
 PIECE_VALUE_side_ENDGAME[45]=VPER;
/* 
for (int i=0; i<10; i++)
	{
		printf("info BasicValues[%d]=%d\n", i, BasicValues[i]);		
	}
for (int i=0; i<46; i++)
	{
		printf("info PIECE_VALUE_side[%d]=%d  PIECE_VALUE_side_ENDGAME[%d]=%d\n", i, PIECE_VALUE_side[i], i, PIECE_VALUE_side_ENDGAME[i]);		
	}	
	fflush(stdout);
*/	
}	

//置换表
void Engine::init_hvalue()
{	//printf("info init_hvalue start\n");
    //int i,j;
    //srand((unsigned)time(0));
//    2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 32,33
//    p p  p p p p  p p p  p   b  b  b  b   e  e  e  e   n  n  n  n   c  c  c  c   r  r  r  r   k  k
//&011101 (& 0x1D)  ( &29)
//0 1 0 1   4 5 4 5   8 9 8 9    12 13 12 13  16 17 16 17   20 21 20 21   24 25 24 25  28 29 28 29  0 1
    /*
    for (i=0; i<BOARD_SIZE-7; i++)
    //for (j=4; j<16; j++)
    for (j=0; j<10; j++)
    {
    		h_value[i][j]=genrand_int64();
    }
    */
    for (int p=0; p<10; p++)
    for (int i=0; i<9; i++)
    for (int j=0; j<10; j++)
    {
    		h_value[p][i][j]=genrand_int64();
    }
    /*
    // copy pawn h_value
		for (i=0; i<BOARD_SIZE-7; i++)
    for (j=0; j<4; j+=2)
    {
    		h_value[i][j] =h_value[i][4];
        h_value[i][j+1] =h_value[i][5];
    }
    */
    h_rside =genrand_int64();
//What I did was change from
//temp_hashkey = (wtm) ? HashKey : ~HashKey;
//to
//temp_hashkey = (wtm) ? HashKey : HashKey ^ ~hash_mask;
    uint64 nHashMask64 = nHashMask;
    h_rside &= ~nHashMask64;


    //printf("info init_hvalue end\n");
    //for (i=0; i<BOARD_SIZE; i++)
    //{
    //	if ((i&15)==0) printf("\n");
    //		printf("%u ",h_val32[i]);
    //}
}


uint64 Engine::init_hkey(const Board &board) //, unsigned long &zkey)
{	//printf("info init_hkey start\n");
    uint64 temp_hkey;
//	zkey = 0;
    //hkey.x1 =0;
    //hkey.x2 =0;
    temp_hkey = 0;


    for (int i=2;i<34;++i)
    {
        int Sq = board.boardsq[i];
        //if (board.boardsq[i]>=0) //* != SQ_EMPTY )
        	if (NOTSQEMPTY(Sq))
            //Xor(hkey,h_value[i][board.boardsq[i]]);
        {
            //board.Xor(temp_hkey,h_value[PIECE_IDX(i)][nFile(i)][nRank(i)]); //[((i&29)+(i&1))/2]);
			temp_hkey ^= h_value[PIECE_IDX(i)][nFile(i)][nRank(i)]; //[((i&29)+(i&1))/2]);
//			zkey  ^= z_value[board.boardsq[i]][i &29];
        }
    }

/*
// try to init hkey in the order of board for consistence between runs
for (int i=0; i<10; i++)
for (int j=0; j<9; j++)
    {

        int p = board.piece[(i*16)+j];
        if (p != EMPTY)
        {
            //Xor(hkey,h_value[Sq][PIECE_IDX(i)]); //[((i&29)+(i&1))/2]);
            Xor(hkey,h_value[PIECE_IDX(p)][i][j]);

        }
    }
*/

//	if(board.m_side!=BLACK)
    //board.Xor(temp_hkey,h_rside);
    temp_hkey ^= h_rside;

    //else
    //	Xor(hkey,h_bside);
//		zkey ^= z_rside;


    //printf("info init_hkey end\n");
    return temp_hkey;
}


// 以下是置换表和开局库的管理过程

unsigned int nHashMask;
HashStruct *hshItems;
/*
void Engine::HashAging(void) {
  int i;
  for (i = 0; i < nHashMask + 1; i ++) {
//    if (hshItems[i].wFresh > 0) {
//      hshItems[i].wFresh --;
//    }
      if (hshItems[i].hAlphaDepth >=2)
      	hshItems[i].hAlphaDepth -=2;
      if (hshItems[i].hBetaDepth >=2)
      	hshItems[i].hBetaDepth -=2;
  }
}
*/
#ifdef EVALHASH
unsigned int nEvalHashMask;
EvalHash *evalhashItems;
#endif

// 清空置换表 - ClearHash() in Engine.h

// 存储置换表局面信息
void Engine::RecordHash(int nFlag, int nDepth, int vl, int mv, Board &board) //, int nSingular)
	{
	  //nDepth += p_movenum;
	  //if (nDepth >255) nDepth=255;
	//nDepth = std::min(nDepth+p_movenum, 255);

    HashStruct *hsh;
    int i, nMinDepth, nMinLayer; //nFreshDepth,

    if (vl > INF) return; //do not record UNKNOWN_VALUE
    // 存储置换表局面信息的过程包括以下几个步骤：

    // 1. 对分值做杀棋步数调整；
    if (vl > WIN_VALUE)
	{
        if (mv == 0 && vl <= BAN_VALUE)
            return; // 导致长将的局面(不进行置换裁剪)如果连最佳着法也没有，那么没有必要写入置换表
        vl += board.ply; //pos.Distance();
    }
	else if (vl < -WIN_VALUE)
	{
        if (mv == 0 && vl >= -BAN_VALUE)
            return; // 同理
        vl -= board.ply; //pos.Distance();
    }
	else if (mv == 0
    	//  && vl == ((board.ply &1) <<5) - DRAWVALUE ) 	//drawvalue
    	&& vl == ((board.ply &1) * DRAWVALUE *2) - DRAWVALUE )
    	//&& vl == ((board.ply &1) ? DRAWVALUE : -DRAWVALUE))
        return;   // 同理

nDepth+=(p_movenum&31);
    // 2. 逐层试探置换表；
    //nMinDepth = (MAX_FRESH + 1) << 8;
    nMinDepth = 512;
    nMinLayer = 0;
    unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
    unsigned int poskey = (board.hkey>>38);
    for (i = 0; i < HASH_LAYERS; i++, hsh++) 
    {
         //hsh = hshItems + ((hkey + i) & nHashMask);
        // 3. 如果试探到一样的局面，那么更新置换表信息即可；

        if (hsh->hkey == poskey)  //upper 26bits        	
        {
        	// 注：如果深度更深，或者边界缩小，都可更新置换表的值
        	/*
            switch (nFlag)
            {
            	case HASH_BETA: 
            	// Beta结点要注意：不要用Null-Move的结点覆盖正常的结点
            	if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal <= vl) && (mv != 0 || hsh->hmvBest == 0)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;  
                hsh->hFlag = nFlag;              
            	}
            	break;	
            
            	case HASH_ALPHA:               	
            	if ( (hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;
            	}
            	break;
            	  
            	case HASH_PV:	
            	if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;
            	}
            	break;
          	} // end switch
          */
          	
          	/* 1890j
            if (nFlag==HASH_ALPHA)
            {
            	if ( (hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                //hsh->hFlag = nFlag;
            	}
            }
            else //HASH_BETA or PV         		
          	{
          		if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal <= vl) && (mv != 0 || hsh->hmvBest == 0)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;  
                hsh->hFlag = nFlag;              
            	}
            }            
            */
            // 1890k  
            
            //          
            if (mv != 0 || hsh->hmvBest == 0 
            	|| (nFlag==HASH_ALPHA && ( hsh->hVal >= vl) ) 
            	)
            {            	
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;            	
            }
            // 
            
                        
            // 最佳着法是始终覆盖的，否则输出主要变例时会出现问题
            if (mv != 0) {
                hsh->hmvBest = set_hmv(mv);
            }

            //hshItems[(hkey + i) & nHashMask] = hsh;
            return;
        }

        // 4. 如果不是一样的局面，那么获得深度最小的置换表项；
        //nFreshDepth = (hsh.wFresh << 8) + MAX(hsh.hAlphaDepth, hsh.hBetaDepth);
       // nFreshDepth = MAX((hsh->hDepth == 0 ? 0 : hsh->hDepth + 256),
       //                   (hsh->hmvBest == 0 ? hsh->hDepth : hsh->hDepth + 256));
                       	
       // if (nFreshDepth < nMinDepth) {
       //     nMinDepth = nFreshDepth;
       //     nMinLayer = i;
       // }
        if (hsh->hDepth < nMinDepth) {
            nMinDepth = hsh->hDepth;
            nMinLayer = i;
        }
    }

    // 5. 记录置换表。
    //hsh = hshItems + (uint32(hkey + nMinLayer) & nHashMask);
    offset = ((board.hkey + nMinLayer ) & nHashMask);
    hsh = hshItems + offset;
    
    hsh->hkey = poskey;  //upper 26bits
    hsh->hmvBest = set_hmv(mv);

    hsh->hFlag = nFlag;    
    hsh->hDepth = nDepth;
    //hsh->hDepth5 = (nDepth+p_movenum)>>2;
    //hsh->hDepth2 = (nDepth+p_movenum)&3;
    hsh->hVal = vl;   
    
    

    //hshItems[(hkey + nMinLayer) & nHashMask] = hsh;
}

/* 1129
// 存储置换表局面信息 for QS  //1021
inline
void Engine::RecordHashQ(int nFlag, int nDepth, int vl, int mv, Board &board) //, int nSingular)
	{
    HashStruct *hsh;
    int i, nMinDepth, nMinLayer; //nFreshDepth,

    if (vl > INF) return; //do not record UNKNOWN_VALUE
    // 存储置换表局面信息的过程包括以下几个步骤：
//1021 - try suppress below for qs hash
//1129*
    // 1. 对分值做杀棋步数调整；
    if (vl > WIN_VALUE)
	{
        if (mv == 0 && vl <= BAN_VALUE)
            return; // 导致长将的局面(不进行置换裁剪)如果连最佳着法也没有，那么没有必要写入置换表
        vl += board.ply; //pos.Distance();
    }
	else if (vl < -WIN_VALUE)
	{
        if (mv == 0 && vl >= -BAN_VALUE)
            return; // 同理
        vl -= board.ply; //pos.Distance();
    }
	else if (mv == 0
    	//  && vl == ((board.ply &1) <<5) - DRAWVALUE ) 	//drawvalue
    	&& vl == ((board.ply &1) * DRAWVALUE *2) - DRAWVALUE )
    	//&& vl == ((board.ply &1) ? DRAWVALUE : -DRAWVALUE))
        return;   // 同理
//1129
//1021 nDepth+=(p_movenum&31);
       nDepth+=2;  //1021 nDepth 0 or -1 is encoded as 2 or 1
       
    // 2. 逐层试探置换表；
    //nMinDepth = (MAX_FRESH + 1) << 8;
    nMinDepth = 512;
    nMinLayer = 0;
    unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
    unsigned int poskey = (board.hkey>>38);
    for (i = 0; i < HASH_LAYERS; i++, hsh++) 
    {
         //hsh = hshItems + ((hkey + i) & nHashMask);
        // 3. 如果试探到一样的局面，那么更新置换表信息即可；

        if (hsh->hkey == poskey)  //upper 26bits        	
        {
        	// 注：如果深度更深，或者边界缩小，都可更新置换表的值
        	//
            switch (nFlag)
            {
            	case HASH_BETA: 
            	// Beta结点要注意：不要用Null-Move的结点覆盖正常的结点
            	if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal <= vl) && (mv != 0 || hsh->hmvBest == 0)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;  
                hsh->hFlag = nFlag;              
            	}
            	break;	
            
            	case HASH_ALPHA:               	
            	if ( (hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;
            	}
            	break;
            	  
            	case HASH_PV:	
            	if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;
            	}
            	break;
          	} // end switch
          	//
          	
          	// 1890j
            if (nFlag==HASH_ALPHA)
            {
            	if ( (hsh->hDepth <= nDepth || hsh->hVal >= vl)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                //hsh->hFlag = nFlag;
            	}
            }
            else //HASH_BETA or PV         		
          	{
          		if ( (i==0 || hsh->hDepth <= nDepth || hsh->hVal <= vl) && (mv != 0 || hsh->hmvBest == 0)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;  
                hsh->hFlag = nFlag;              
            	}
            }            
            //
            
            // 1890k
            //           
            if (mv != 0 || hsh->hmvBest == 0 
            	|| (nFlag==HASH_ALPHA && ( hsh->hVal >= vl) ) 
            	)
            {            	
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                hsh->hFlag = nFlag;            	
            }
            //
            // 2019k
            if (nFlag==HASH_ALPHA)
            {
            	if (hsh->hVal >= vl)
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;
                //hsh->hFlag = nFlag;
                hsh->hmvBest = set_hmv(mv);
            	}
            }
            else //HASH_BETA or PV         		
          	{
          		if ( (mv != 0 || hsh->hmvBest == 0)) 
            	{
                hsh->hDepth = nDepth;                 
                hsh->hVal = vl;  
                hsh->hFlag = nFlag; 
                hsh->hmvBest = set_hmv(mv);             
            	}
            }            
            
            // 2019k            
            // 最佳着法是始终覆盖的，否则输出主要变例时会出现问题
            if (mv != 0) {
                hsh->hmvBest = set_hmv(mv);
            }
            //

            //hshItems[(hkey + i) & nHashMask] = hsh;
            return;
        }

        // 4. 如果不是一样的局面，那么获得深度最小的置换表项；
        //nFreshDepth = (hsh.wFresh << 8) + MAX(hsh.hAlphaDepth, hsh.hBetaDepth);
       // nFreshDepth = MAX((hsh->hDepth == 0 ? 0 : hsh->hDepth + 256),
       //                   (hsh->hmvBest == 0 ? hsh->hDepth : hsh->hDepth + 256));
                       	
       // if (nFreshDepth < nMinDepth) {
       //     nMinDepth = nFreshDepth;
       //     nMinLayer = i;
       // }
        if (hsh->hDepth < nMinDepth) {
            nMinDepth = hsh->hDepth;
            nMinLayer = i;
        }
    }

    // 5. 记录置换表。
    //hsh = hshItems + (uint32(hkey + nMinLayer) & nHashMask);
    offset = ((board.hkey + nMinLayer ) & nHashMask);
    hsh = hshItems + offset;
    
    hsh->hkey = poskey;  //upper 26bits
    hsh->hmvBest = set_hmv(mv);

    hsh->hFlag = nFlag;    
    hsh->hDepth = nDepth;
    //hsh->hDepth5 = (nDepth+p_movenum)>>2;
    //hsh->hDepth2 = (nDepth+p_movenum)&3;
    hsh->hVal = vl;   
    
    

    //hshItems[(hkey + nMinLayer) & nHashMask] = hsh;
}
*/ //1129
/*
//inline
//1021 old version: void Engine::RecordHashQ(int nFlag, int vl, int mv, Board &board) {

    HashStruct *hsh;
    int i, nMinDepth, nMinLayer; //nFreshDepth, 

    if (vl > INF) return; //do not record UNKNOWN_VALUE
    // 存储置换表局面信息的过程包括以下几个步骤：

    // 1. 对分值做杀棋步数调整；
    if (vl > WIN_VALUE)
	{
        if (mv == 0 && vl <= BAN_VALUE)
            return; // 导致长将的局面(不进行置换裁剪)如果连最佳着法也没有，那么没有必要写入置换表
        vl += board.ply; //pos.Distance();
    }
	else if (vl < -WIN_VALUE)
	{
        if (mv == 0 && vl >= -BAN_VALUE)
            return; // 同理
        vl -= board.ply; //pos.Distance();
    }
	else if (mv == 0
    	//  && vl == ((board.ply &1) <<5) - DRAWVALUE ) 	//drawvalue
    	&& vl == ((board.ply &1) * DRAWVALUE *2) - DRAWVALUE ) 
    	//&& vl == ((board.ply &1) ? DRAWVALUE : -DRAWVALUE))
        return;   // 同理

int nDepth = (p_movenum&31);
    // 2. 逐层试探置换表；
    //nMinDepth = (MAX_FRESH + 1) << 8;
    nMinDepth = 512;
    nMinLayer = 0;
    hsh = hshItems + (uint32(hkey ) & nHashMask);
    for (i = 0; i < HASH_LAYERS; i++, hsh++) 
    {
         //hsh = hshItems + ((hkey + i) & nHashMask);
        // 3. 如果试探到一样的局面，那么更新置换表信息即可；

        if (hsh->hkey == (hkey>>38))  //upper 26bits
        {
            // 注：如果深度更深，或者边界缩小，都可更新置换表的值
            

            if (nFlag==HASH_BETA)
            {
            		if ( (i==0 || hsh->hDepth <= nDepth 
            			|| hsh->hVal <= vl) && (mv != 0 || hsh->hmvBest == 0) ) 
            		{
                	hsh->hDepth = nDepth;                	
                	hsh->hVal = vl;
                	//hsh->hFlag = nFlag;
            		}
            }
            else
            {
            		if ( hsh->hDepth <= nDepth 
            			|| hsh->hVal >= vl ) 
            		{
                	hsh->hDepth = nDepth;                	
                	hsh->hVal = vl; 
                	//hsh->hFlag = nFlag;               
            		}
            }				
            // 最佳着法是始终覆盖的，否则输出主要变例时会出现问题
            if (mv != 0) {
                hsh->hmvBest = set_hmv(mv);
            }

            return;
        }

        // 4. 如果不是一样的局面，那么获得深度最小的置换表项；
        //nFreshDepth = (hsh.wFresh << 8) + MAX(hsh.hAlphaDepth, hsh.hBetaDepth);
        //nFreshDepth = MAX((hsh->hDepth == 0 ? 0 : hsh->hDepth + 256),
        //                  (hsh->hmvBest == 0 ? hsh->hDepth : hsh->hDepth + 256));
        
        //if (nFreshDepth < nMinDepth) {
        //    nMinDepth = nFreshDepth;
        //    nMinLayer = i;
        //}
        if (hsh->hDepth < nMinDepth) {
            nMinDepth = hsh->hDepth;
            nMinLayer = i;
        }
    }

    // 5. 记录置换表。
    hsh = hshItems + (uint32(hkey + nMinLayer) & nHashMask);
    hsh->hkey = (hkey>>38);  //upper 26bits
    hsh->hmvBest = set_hmv(mv);

    hsh->hFlag = nFlag;    
    hsh->hDepth = nDepth;
    //hsh->hDepth5 = (p_movenum)>>2;
    //hsh->hDepth2 = (p_movenum)&3;
    hsh->hVal = vl;    
}
*/

inline 
int Engine::value_from_tt(short vl, int ply)   //1107 for ttValue sf10
{
//1107    bBanNode = bMateNode = 0;
    if (vl > WIN_VALUE) {
        if (vl <= BAN_VALUE) {
            ; //bBanNode = 1;
        } else {
            //bMateNode = 1;
            vl -= ply; //pos.nDistance;
        }
    } else if (vl < -WIN_VALUE) {
        if (vl >= -BAN_VALUE) {
            ; //bBanNode = 1;
        } else {
            //bMateNode = 1;
            vl += ply; //pos.nDistance;
        }
    }
    
//    else if (vl == ((ply &1) * DRAWVALUE *2) - DRAWVALUE)
//    {
        //bBanNode = 1;
//    }
    return vl;
}

// 获取置换表着法
inline
//int Engine::ProbeMove(int &nBetaDepth, int &nBetaVal, int &nAlphaDepth) //, int &nSingular)
HashStruct* Engine::ProbeMove(Board &board)
{
  HashStruct *hsh;
  //hsh = hshItems + (uint32(hkey) & nHashMask);
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
  for (int i = 0; i < HASH_LAYERS; i++, hsh++) {
    if (hsh->hkey == (board.hkey>>38))  //upper 26bits
    {
    	return hsh;
    }
  }
  return NULL; //0;
}

inline
int Engine::ProbeMoveQ(Board &board)
{
  HashStruct *hsh;
  //hsh = hshItems + (uint32(hkey) & nHashMask);
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
  for (int i = 0; i < HASH_LAYERS; i++, hsh++) {
    if (hsh->hkey == (board.hkey>>38))  //upper 26bits
    {
      return get_hmv(hsh->hmvBest);
    }
  }
  return 0;
}

//ABDADA - not used //20191101
/*
inline
int Engine::ProbeHashExcl(Board &board, int nDepth)
{
  HashStruct *hsh;
  //hsh = hshItems + (uint32(hkey) & nHashMask);
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
  //for (int i = 0; i < HASH_LAYERS; i++, hsh++) //just the first layer for ABDADA
  {
    if (hsh->hkey == (board.hkey>>38)) // && hsh->hDepth >= nDepth)  //upper 26bits
    {
      
      return hsh->hExclusive;
    }
  }
  return 0;
}

inline
void Engine::RecordHashExcl(Board &board, int nHashExclusive, int nDepth)
{
  HashStruct *hsh;
  //hsh = hshItems + (uint32(hkey) & nHashMask);
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
     unsigned int poskey = (board.hkey>>38);
  //for (int i = 0; i < HASH_LAYERS; i++, hsh++) //just the first layer for ABDADA
  {
    //if (hsh->hkey == (board.hkey>>38))  //upper 26bits
    {
      
    
    
    hsh->hkey = poskey;  //upper 26bits
    hsh->hmvBest = 0; //set_hmv(mv);

    hsh->hFlag = 0; //nFlag;    
    hsh->hDepth = nDepth;
    //hsh->hDepth5 = (nDepth+p_movenum)>>2;
    //hsh->hDepth2 = (nDepth+p_movenum)&3;
    hsh->hVal = 0; //vl;   
      hsh->hExclusive = nHashExclusive;
    }
  }
  //return 0;
}
*/

/* 判断获取置换表要符合哪些条件，置换表的分值针对四个不同的区间有不同的处理：
 * 一、如果分值在"WIN_VALUE"以内(即介于"-WIN_VALUE"到"WIN_VALUE"之间，下同)，则只获取满足搜索深度要求的局面；
 * 二、如果分值在"WIN_VALUE"和"BAN_VALUE"之间，则不能获取置换表中的值(只能获取最佳着法仅供参考)，目的是防止由于长将而导致的“置换表的不稳定性”；
 * 三、如果分值在"BAN_VALUE"以外，则获取局面时不必考虑搜索深度要求，因为这些局面已经被证明是杀棋了；
 * 四、如果分值是"DrawValue()"(是第一种情况的特殊情况)，则不能获取置换表中的值(原因与第二种情况相同)。
 * 注意：对于第三种情况，要对杀棋步数进行调整！
 */
inline int ValueAdjust(int &bBanNode, int &bMateNode, int vl, int ply) {
    bBanNode = bMateNode = 0;
    if (vl > WIN_VALUE) {
        if (vl <= BAN_VALUE) {
            bBanNode = 1;
        } else {
            bMateNode = 1;
            vl -= ply; //pos.nDistance;
        }
    } else if (vl < -WIN_VALUE) {
        if (vl >= -BAN_VALUE) {
            bBanNode = 1;
        } else {
            bMateNode = 1;
            vl += ply; //pos.nDistance;
        }
    }
    //else if (vl == ((board.ply &1) <<5) - DRAWVALUE) //pos.DrawValue())
    else if (vl == ((ply &1) * DRAWVALUE *2) - DRAWVALUE)
    //else if (vl == ((board.ply &1) ? DRAWVALUE : -DRAWVALUE))
    {
        bBanNode = 1;
    }
    return vl;
}

// 获取置换表局面信息
inline
int Engine::ProbeHash(int nDepth, int vlBeta, int null_allow, HashStruct* &hsho, Board &board)
{

    HashStruct *hsh; //, *lphsh;
    //int i,vl;
    int i,vl, bBanNode,bMateNode;
    //int w_pvline[256];
   
    // 获取置换表局面信息的过程包括以下几个步骤：

nDepth+=(p_movenum&31);
// 1. 逐层获取置换表项

    //mv = 0;
    //hsh = hshItems + (uint32(hkey ) & nHashMask);
    unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
    for (i = 0; i < HASH_LAYERS; i++, hsh++) {
        //hsh = hshItems + ((hkey + i) & nHashMask);
        if (hsh->hkey == (board.hkey>>38))  //upper 26bits
        {
        	  hsho = hsh;        	  
            break;
        }
    }
    if (i == HASH_LAYERS) {
    	  hsho = NULL;
        return UNKNOWN_VALUE;
    }


       // 2. 判断获取置换表要符合哪些条件，置换表的分值针对三个不同的区间有不同的处理：
       // *    一、如果分值在"WIN_VALUE"以内(即介于"-WIN_VALUE"到"WIN_VALUE"之间，下同)，则只获取满足搜索深度要求的局面；
       // *    二、如果分值在"WIN_VALUE"和"BAN_VALUE"之间，则不能获取置换表中的值(只能获取最佳着法仅供参考)，目的是防止由于长将而导致的“置换表的不稳定性”；
       // *    三、如果分值在"BAN_VALUE"以外，则获取局面时不必考虑搜索深度要求，因为这些局面已经被证明是杀棋了。
       // *    注意：对于第三种情况，要对杀棋步数进行调整！
       // *

         // * 3. 对于空着裁剪的局面，判断局面判断是否命中，需要符合以下三个条件：
         // *    一、允许使用空着裁剪；
         // *    二、深度达到要求(注：深度在"NULL_DEPTH"以下，即达到要求)；
         // *    三、不是因禁止长将策略而搜索到的杀棋；
         // *    四、符合边界条件。
         // *

         // 4. 其他局面判断是否命中，需要符合以下四个条件：
         // *    一、深度达到要求(杀棋局面可以不考虑深度)；
         // *    二、不是因禁止长将策略而搜索到的杀棋；
         // *    三、符合边界条件；
         // *    四、不会产生循环的局面(目的是为进一步减轻“置换表的不稳定性”，和前面的相关措施有异曲同工之妙)。
         // *

    switch (hsh->hFlag)
    {
    	case HASH_BETA:  
    	case HASH_PV:		
// 2. 判断是否符合Beta边界

    //if (hsh->hDepth4*8+hsh->hDepth3 > 0) 
			{
        vl = ValueAdjust(bBanNode, bMateNode, hsh->hVal, board.ply);
        if (!bBanNode && (hsh->hmvBest || null_allow) && (hsh->hDepth >= nDepth 
//        	  || (hsh->hDepth==2 || hsh->hDepth==1 )    //2019k allow to get QS hash, not work cause qshash is rough
//            || (hsh->hFlag==HASH_PV && ( hsh->hDepth==2)  )       //2019k 1023 try some qs hash pv  no effect
        	  || bMateNode) && vl >= vlBeta)
        	  {
            // __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
            //2019k not just move=0    if (hsh->hmvBest == 0     
            	 //|| GetPvStable(w_pvline, hsh->hmvBest)
            //2019k	 )
            	  {
//2019k             	  	if (hsh->hFlag==HASH_PV && ( hsh->hDepth==2)  )  hashhit++;
                return vl;
            }
        }
      }
       break;
       
       case HASH_ALPHA:       
    // 3. 判断是否符合Alpha边界
    //if (hsh->hDepth4*8+hsh->hDepth3 > 0) 
		   {
        vl = ValueAdjust(bBanNode, bMateNode, hsh->hVal, board.ply);
        if (!bBanNode && (hsh->hDepth >= nDepth
//          	|| (hsh->hDepth==2 || hsh->hDepth==1 )    //2019k allow to get QS hash, not work
        	  || bMateNode) && vl < vlBeta)   //<= vlAlpha)
        {
            // __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
            //2019k not just move=0    if (hsh->hmvBest == 0 
            	//|| GetPvStable(w_pvline, hsh->hmvBest)
            //2019k	)
            	 {
                return vl;
            }
        }
    }
        break;
    } // end switch    

    // 4. 如果达不到获取局面的要求，那么可以返回最佳着法以便做迭代加深。
    //*lpmv = hshTemp.mvBest;
    //mv = hsh.mvBest;  //done above in hsh.hkey == hkey
    return UNKNOWN_VALUE;
}

inline int ValueAdjQ(int vl, int ply) {

    if (vl > WIN_VALUE)

            vl -= ply; //pos.nDistance;

    else if (vl < -WIN_VALUE)

            vl += ply; //pos.nDistance;

    return vl;
}
#ifdef QHASH
/* 1129
// 获取置换表局面信息
inline
int Engine::ProbeHashQ(int vlBeta, int &mv, Board &board, int &hashdepth)  //2019 input hashdepth=0
{
    HashStruct *hsh; //, *lphsh;
    //int i,vl;
    int i,vl, bBanNode,bMateNode;
    //int wmvPvLine[MAX_MOVE_NUM];
    // 获取置换表局面信息的过程包括以下几个步骤：

// 1. 逐层获取置换表项

    mv = 0;
    //hsh = hshItems + (uint32(hkey ) & nHashMask);
    unsigned int offset = ((board.hkey ) & nHashMask);
    hsh = hshItems + offset;
    for (i = 0; i < HASH_LAYERS; i++, hsh++) {
        //hsh = hshItems + ((hkey + i) & nHashMask);
        if (hsh->hkey == (board.hkey>>38))  //upper 26bits
        {
            mv = get_hmv(hsh->hmvBest);
            break;
        }
    }
    if (i == HASH_LAYERS) {
        return UNKNOWN_VALUE;
    }

//int nDepth = p_movenum;
//if (nDepth >255) nDepth=255;
//	int nDepth = std::min(p_movenum, 255);
switch (hsh->hFlag)
    {
    	case HASH_BETA:  
    	case HASH_PV:	
// 2. 判断是否符合Beta边界
    //if (hsh->hDepth4*8+hsh->hDepth3 > 0)
    {
    	vl = ValueAdjust(bBanNode, bMateNode, hsh->hVal, board.ply);
//2019k  ref ProbeHash --- if (!bBanNode && (hsh->hmvBest || null_allow) && (hsh->hDepth >= nDepth || bMateNode) && vl >= vlBeta) {
        
        if (!bBanNode) // && (hsh->hmvBest || null_allow) )   //2019k assume null_allow always true in QS
        if ( (hsh->hDepth >= (p_movenum&31)) || (hsh->hDepth == 2 || hsh->hDepth == 1) || bMateNode)   //2019 qhashdepth 0, -1 encoded as 2, 1	            
        if (vl >= vlBeta)
            { 	hashdepth = hsh->hDepth;   //1007 return hashdepth
                return vl;
            }    
        
    }
    break;

		case HASH_ALPHA:
    // 3. 判断是否符合Alpha边界
    //if (hsh->hDepth4*8+hsh->hDepth3 > 0)
    {   vl = ValueAdjust(bBanNode, bMateNode, hsh->hVal, board.ply); 
//2019k ref ProbeHash --- if (!bBanNode && (hsh->hDepth >= nDepth || bMateNode) && vl < vlBeta)   //<= vlAlpha)     	
        
        if (!bBanNode  )   //2019k
        if ((hsh->hDepth >= (p_movenum&31)) || (hsh->hDepth == 2 || hsh->hDepth == 1) || bMateNode ) //2019 qhashdepth 0, -1 encoded as 2, 1 
        if (vl < vlBeta)  //<= vlAlpha = vlBeta-1)
            {   hashdepth = hsh->hDepth;   //1007 return hashdepth
                return vl;
            }    
        
    }
    break;
  } // end switch

    // 4. 如果达不到获取局面的要求，那么可以返回最佳着法以便做迭代加深。
    //*lpmv = hshTemp.mvBest;
    //mv = hsh.mvBest;  //done above in hsh.hkey == hkey
    return UNKNOWN_VALUE;
}
*/
#endif
/* eleeye 3.15
// 获取置换表局面信息(静态搜索)
int ProbeHashQ(const PositionStruct &pos, int vlAlpha, int vlBeta) {
  volatile HashStruct *lphsh;
  int vlHashAlpha, vlHashBeta;

  lphsh = hshItemsQ + (pos.zobr.dwKey & nHashMask);
  if (lphsh->dwZobristLock0 == pos.zobr.dwLock0) {
    vlHashAlpha = lphsh->svlAlpha;
    vlHashBeta = lphsh->svlBeta;
    if (lphsh->dwZobristLock1 == pos.zobr.dwLock1) {
      if (vlHashBeta >= vlBeta) {
        __ASSERT(vlHashBeta > -WIN_VALUE && vlHashBeta < WIN_VALUE);
        return vlHashBeta;
      }
      if (vlHashAlpha <= vlAlpha) {
        __ASSERT(vlHashAlpha > -WIN_VALUE && vlHashAlpha < WIN_VALUE);
        return vlHashAlpha;
      }
    }
  }
  return -MATE_VALUE;
}
*/



/////////////////////////////////////////////////////////////////////////

void Engine::IRead(Board &board, const char *fen)
{
//	  printf("*** lmrparm1=%d\n", lmrparm1);
	  board.thd_id = 0; //1017
#ifdef PERFT
    printf("%s\n", fen);
#endif
    int piece_count[12]={0,0,0,0,0,0, 0,0,0,0,0,0};
    int i,len,sp;
    len=strlen(fen);
    sp=0;
    //for(i=0;i<BOARD_SIZE;++i)
    //{
    //	board.piece [i]=0;
    //}
    memset(board.piece,0,sizeof(board.piece));
    board.m_side=RED; //BLACK; //99; //0;
    for (i=0; i<len && sp<BOARD_SIZE-7; ++i)
    {
        if (fen[i]>='0'&&fen[i]<='9')
            sp+=fen[i]-'0';
        else if (fen[i]=='/')
            sp+=7;	//advance 7 spaces at end of rank
        else if (fen[i]>='a'&&fen[i]<='z')
        {
            switch (fen[i])
            {
            case 'k':
                board.piece [sp]=B_KING; //1;
                sp++;
                break;
            case 'a':
                //if (piece_count[1]==0)
                    board.piece [sp]=B_ADVISOR1; //2;
                //else board.piece [sp]=B_ADVISOR2;

                //piece_count[1]++;
                sp++;
                break;

            case 'b':
            case 'e':
                //if (piece_count[2]==0)
                    board.piece [sp]=B_ELEPHAN1; //3;
                //else board.piece [sp]=B_ELEPHAN2;

                piece_count[2]++;
                sp++;
                break;

            case 'n':
            case 'h':
                //if (piece_count[3]==0)
                    board.piece [sp]=B_HORSE1; //4;
                //else board.piece [sp]=B_HORSE2;

                piece_count[3]++;
                sp++;
                break;

            case 'r':
                //if (piece_count[5]==0)
                    board.piece [sp]=B_ROOK1; //5;
                //else board.piece [sp]=B_ROOK2;

                piece_count[5]++;
                sp++;
                break;

            case 'c':
                //if (piece_count[4]==0)
                    board.piece [sp]=B_CANNON1; //6;
                //else board.piece [sp]=B_CANNON2;

                piece_count[4]++;
                sp++;
                break;

            case 'p':
                //if (piece_count[0]==0)
                //    board.piece [sp]=B_PAWN2; //7;
                //else if (piece_count[0]==1) board.piece [sp]=B_PAWN4;
                //else if (piece_count[0]==2) board.piece [sp]=B_PAWN1;
                //else if (piece_count[0]==3) board.piece [sp]=B_PAWN5;
                //else 			    board.piece [sp]=B_PAWN3;

                //piece_count[0]++;
                board.piece [sp]=B_PAWN1;
                sp++;
                break;
            }
        }
        else if (fen[i]>='A'&&fen[i]<='Z')
        {
            switch (fen[i])
            {
            case 'K':
                board.piece [sp]=R_KING; //8;
                sp++;
                break;
            case 'A':

                //if (piece_count[7]==0)
                //    board.piece [sp]=R_ADVISOR2; //2;
                //else
                board.piece [sp]=R_ADVISOR1;

                //piece_count[7]++;
                sp++;
                break;

            case 'B':
            case 'E':

                //if (piece_count[8]==0)
                //    board.piece [sp]=R_ELEPHAN2; //3;
                //else
                board.piece [sp]=R_ELEPHAN1;

                piece_count[8]++;
                sp++;
                break;

            case 'N':
            case 'H':

                //if (piece_count[9]==0)
                //    board.piece [sp]=R_HORSE2; //4;
                //else
                board.piece [sp]=R_HORSE1;

                piece_count[9]++;
                sp++;
                break;

            case 'R':

                //if (piece_count[11]==0)
                //    board.piece [sp]=R_ROOK2; //5;
                //else
                board.piece [sp]=R_ROOK1;

                piece_count[11]++;
                sp++;
                break;

            case 'C':

                //if (piece_count[10]==0)
                //    board.piece [sp]=R_CANNON2; //6;
                //else
                board.piece [sp]=R_CANNON1;

                piece_count[10]++;
                sp++;
                break;

            case 'P':
                //if (piece_count[6]==0)
                //    board.piece [sp]=R_PAWN3; //7;
                //else if (piece_count[6]==1) board.piece [sp]=R_PAWN5;
                //else if (piece_count[6]==2) board.piece [sp]=R_PAWN1;
                //else if (piece_count[6]==3) board.piece [sp]=R_PAWN4;
                //else 			    board.piece [sp]=R_PAWN2;

                //piece_count[6]++;
                board.piece [sp]=R_PAWN1;
                sp++;
                break;
            }
        }
    }

    if (sp==BOARD_SIZE-7)
    {
    	  board.m_side = 9999;
        for (;i<len;i++)
        {
            if (fen[i]=='W'||fen[i]=='w'||fen[i]=='R'||fen[i]=='r')
            {
                board.m_side =RED;
                break;
            }
            else if (fen[i]=='B'||fen[i]=='b')
            {
                board.m_side=BLACK;
                break;
            }
        }    
            if (board.m_side == 9999)
            	{
				  board.m_side = RED;
            	  printf("Fen String no w/b found, default=w fen= %s\n", fen);
                  fflush(stdout);
            	}
        
    }
    else
    {
        printf("Fen String Error! fen= %s\n", fen);
        fflush(stdout);
    }

    //if(board.m_side==99) //0)
    //	board.m_side=RED;
    for (i=0;i<34;++i)
        board.boardsq[i]=SQ_EMPTY;
/*
    for (i=0;i<BOARD_SIZE-7;++i)
    {
        if (board.piece[i] != 0)
            board.boardsq[board.piece[i]]=i;
    }
*/
		// vertical scan of board to set the correct boardsq for openbook searching
//		for (i=0; i<12; i++)
//			piece_count[i]=0;

		for (i=0; i<9; i++)
		for (int j=0; j<10; j++)
		{
			int k = j*16+i;
			int piecek = board.piece[k];
			if (piecek != 0)
			{
				switch (piecek)
				{
					case B_PAWN1:
					//case B_PAWN2:
					//case B_PAWN3:
					//case B_PAWN4:
					//case B_PAWN5:
						/*
						if (piece_count[0]==0)
                    board.boardsq[B_PAWN2]=k; //7;
                else if (piece_count[0]==1) board.boardsq[B_PAWN4]=k;
                else if (piece_count[0]==2) board.boardsq[B_PAWN1]=k;
                else if (piece_count[0]==3) board.boardsq[B_PAWN5]=k;
                else 			    board.boardsq[B_PAWN3]=k;
            piece_count[0]++;
            */
            // (2) 4 1 5 3
            if (i<=1)
            {	if (board.boardsq[B_PAWN2]==SQ_EMPTY)
            		board.boardsq[B_PAWN2]=k;
            	else if (board.boardsq[B_PAWN4]==SQ_EMPTY)
            		board.boardsq[B_PAWN4]=k;
            	else if (board.boardsq[B_PAWN1]==SQ_EMPTY)
            		board.boardsq[B_PAWN1]=k;
            	else if (board.boardsq[B_PAWN5]==SQ_EMPTY)
            		board.boardsq[B_PAWN5]=k;
            	else board.boardsq[B_PAWN3]=k;
            }
            // 2 (4) 1 5 3
            else if (i==2 || i==3)
            {	if (board.boardsq[B_PAWN4]==SQ_EMPTY)
            		board.boardsq[B_PAWN4]=k;
            	else if (board.boardsq[B_PAWN2]==SQ_EMPTY)
            		board.boardsq[B_PAWN2]=k;
            	else if (board.boardsq[B_PAWN1]==SQ_EMPTY)
            		board.boardsq[B_PAWN1]=k;
            	else if (board.boardsq[B_PAWN5]==SQ_EMPTY)
            		board.boardsq[B_PAWN5]=k;
            	else board.boardsq[B_PAWN3]=k;
            }
            // 2 4 (1) 5 3
            else if (i==4)
            {	if (board.boardsq[B_PAWN1]==SQ_EMPTY)
            		board.boardsq[B_PAWN1]=k;
            	else if (board.boardsq[B_PAWN4]==SQ_EMPTY)
            		board.boardsq[B_PAWN4]=k;
            	else if (board.boardsq[B_PAWN5]==SQ_EMPTY)
            		board.boardsq[B_PAWN5]=k;
            	else if (board.boardsq[B_PAWN2]==SQ_EMPTY)
            		board.boardsq[B_PAWN2]=k;
            	else board.boardsq[B_PAWN3]=k;
            }
            // 2 4 1 (5) 3
            else if (i==5 || i==6)
            {	if (board.boardsq[B_PAWN5]==SQ_EMPTY)
            		board.boardsq[B_PAWN5]=k;
            	else if (board.boardsq[B_PAWN3]==SQ_EMPTY)
            		board.boardsq[B_PAWN3]=k;
            	else if (board.boardsq[B_PAWN1]==SQ_EMPTY)
            		board.boardsq[B_PAWN1]=k;
            	else if (board.boardsq[B_PAWN4]==SQ_EMPTY)
            		board.boardsq[B_PAWN4]=k;
            	else board.boardsq[B_PAWN2]=k;
            }
            // 2 4 1 5 (3)
            else //if (i==7 || i==8)
            {	if (board.boardsq[B_PAWN3]==SQ_EMPTY)
            		board.boardsq[B_PAWN3]=k;
            	else if (board.boardsq[B_PAWN5]==SQ_EMPTY)
            		board.boardsq[B_PAWN5]=k;
            	else if (board.boardsq[B_PAWN1]==SQ_EMPTY)
            		board.boardsq[B_PAWN1]=k;
            	else if (board.boardsq[B_PAWN4]==SQ_EMPTY)
            		board.boardsq[B_PAWN4]=k;
            	else board.boardsq[B_PAWN2]=k;
            }

						break;

					case B_ADVISOR1:
					//case B_ADVISOR2:
						/*
						if (piece_count[1]==0)
                    board.boardsq[B_ADVISOR1]=k; //2;
                else board.boardsq[B_ADVISOR2]=k;
            piece_count[1]++;
            */
            if (i<=4)
            {	if (board.boardsq[B_ADVISOR1]==SQ_EMPTY)
            		board.boardsq[B_ADVISOR1]=k;
            	else board.boardsq[B_ADVISOR2]=k;
            }
            else
            {	if (board.boardsq[B_ADVISOR2]==SQ_EMPTY)
            		board.boardsq[B_ADVISOR2]=k;
            	else board.boardsq[B_ADVISOR1]=k;
            }
						break;

					case B_ELEPHAN1:
					//case B_ELEPHAN2:
					/*
						if (piece_count[2]==0)
                    board.boardsq[B_ELEPHAN1]=k; //2;
                else board.boardsq[B_ELEPHAN2]=k;
            piece_count[2]++;
          */
            if (i<=4)
            {	if (board.boardsq[B_ELEPHAN1]==SQ_EMPTY)
            		board.boardsq[B_ELEPHAN1]=k;
            	else board.boardsq[B_ELEPHAN2]=k;
            }
            else
            {	if ((board.boardsq[B_ELEPHAN1]==SQ_EMPTY) && piece_count[2]==2)
            		board.boardsq[B_ELEPHAN1]=k;
            	else board.boardsq[B_ELEPHAN2]=k;
            }
						break;

					case B_HORSE1:
					//case B_HORSE2:
						/*
						if (piece_count[3]==0)
                    board.boardsq[B_HORSE1]=k; //2;
                else board.boardsq[B_HORSE2]=k;
            piece_count[3]++;
            */
            if (i<=4)
            {	if (board.boardsq[B_HORSE1]==SQ_EMPTY)
            		board.boardsq[B_HORSE1]=k;
            	else board.boardsq[B_HORSE2]=k;
            }
            else
            {	if ((board.boardsq[B_HORSE1]==SQ_EMPTY) && piece_count[3]==2)
            		board.boardsq[B_HORSE1]=k;
            	else board.boardsq[B_HORSE2]=k;
            }
						break;

					case B_CANNON1:
					//case B_CANNON2:
						/*
						if (piece_count[4]==0)
                    board.boardsq[B_CANNON1]=k; //2;
                else board.boardsq[B_CANNON2]=k;
            piece_count[4]++;
            */
            if (i<=4)
            {	if (board.boardsq[B_CANNON1]==SQ_EMPTY)
            		board.boardsq[B_CANNON1]=k;
            	else board.boardsq[B_CANNON2]=k;
            }
            else
            {	if ((board.boardsq[B_CANNON1]==SQ_EMPTY) && piece_count[4]==2)
            		board.boardsq[B_CANNON1]=k;
            	else board.boardsq[B_CANNON2]=k;
            }
						break;

					case B_ROOK1:
					//case B_ROOK2:
						/*
						if (piece_count[5]==0)
                    board.boardsq[B_ROOK1]=k; //2;
                else board.boardsq[B_ROOK2]=k;
            piece_count[5]++;
            */
            // R R |
            if (i<=4)
            {	if (board.boardsq[B_ROOK1]==SQ_EMPTY)
            		board.boardsq[B_ROOK1]=k;
            	else board.boardsq[B_ROOK2]=k;
            }
            else //  | R R
            {	if ((board.boardsq[B_ROOK1]==SQ_EMPTY) && piece_count[5]==2)
            		board.boardsq[B_ROOK1]=k;
            	else board.boardsq[B_ROOK2]=k;
            }
						break;

					case B_KING:
						board.boardsq[B_KING]=k;
						break;

					case R_PAWN1:
					//case R_PAWN2:
					//case R_PAWN3:
					//case R_PAWN4:
					//case R_PAWN5:
						/*
						if (piece_count[6]==0)
                    board.boardsq[R_PAWN3]=k; //7;
                else if (piece_count[6]==1) board.boardsq[R_PAWN5]=k;
                else if (piece_count[6]==2) board.boardsq[R_PAWN1]=k;
                else if (piece_count[6]==3) board.boardsq[R_PAWN4]=k;
                else 			    board.boardsq[R_PAWN2]=k;
            piece_count[6]++;
            */
            // (3) 5 1 4 2
            if (i<=1)
            {	if (board.boardsq[R_PAWN3]==SQ_EMPTY)
            		board.boardsq[R_PAWN3]=k;
            	else if (board.boardsq[R_PAWN5]==SQ_EMPTY)
            		board.boardsq[R_PAWN5]=k;
            	else if (board.boardsq[R_PAWN1]==SQ_EMPTY)
            		board.boardsq[R_PAWN1]=k;
            	else if (board.boardsq[R_PAWN4]==SQ_EMPTY)
            		board.boardsq[R_PAWN4]=k;
            	else board.boardsq[R_PAWN2]=k;
            }
            // 3 (5) 1 4 2
            else if (i==2 || i==3)
            {	if (board.boardsq[R_PAWN5]==SQ_EMPTY)
            		board.boardsq[R_PAWN5]=k;
            	else if (board.boardsq[R_PAWN1]==SQ_EMPTY)
            		board.boardsq[R_PAWN1]=k;
            	else if (board.boardsq[R_PAWN3]==SQ_EMPTY)
            		board.boardsq[R_PAWN3]=k;
            	else if (board.boardsq[R_PAWN4]==SQ_EMPTY)
            		board.boardsq[R_PAWN4]=k;
            	else board.boardsq[R_PAWN2]=k;
            }
            // 3 5 (1) 4 2
            else if (i==4)
            {	if (board.boardsq[R_PAWN1]==SQ_EMPTY)
            		board.boardsq[R_PAWN1]=k;
            	else if (board.boardsq[R_PAWN4]==SQ_EMPTY)
            		board.boardsq[R_PAWN4]=k;
            	else if (board.boardsq[R_PAWN5]==SQ_EMPTY)
            		board.boardsq[R_PAWN5]=k;
            	else if (board.boardsq[R_PAWN2]==SQ_EMPTY)
            		board.boardsq[R_PAWN2]=k;
            	else board.boardsq[R_PAWN3]=k;
            }
            // 3 5 1 (4) 2
            else if (i==5 || i==6)
            {	if (board.boardsq[R_PAWN4]==SQ_EMPTY)
            		board.boardsq[R_PAWN4]=k;
            	else if (board.boardsq[R_PAWN2]==SQ_EMPTY)
            		board.boardsq[R_PAWN2]=k;
            	else if (board.boardsq[R_PAWN1]==SQ_EMPTY)
            		board.boardsq[R_PAWN1]=k;
            	else if (board.boardsq[R_PAWN5]==SQ_EMPTY)
            		board.boardsq[R_PAWN5]=k;
            	else board.boardsq[R_PAWN3]=k;
            }
            // 3 5 1 4 (2)
            else //if (i==7 || i==8)
            {	if (board.boardsq[R_PAWN2]==SQ_EMPTY)
            		board.boardsq[R_PAWN2]=k;
            	else if (board.boardsq[R_PAWN4]==SQ_EMPTY)
            		board.boardsq[R_PAWN4]=k;
            	else if (board.boardsq[R_PAWN1]==SQ_EMPTY)
            		board.boardsq[R_PAWN1]=k;
            	else if (board.boardsq[R_PAWN5]==SQ_EMPTY)
            		board.boardsq[R_PAWN5]=k;
            	else board.boardsq[R_PAWN3]=k;
            }
						break;

					case R_ADVISOR1:
					//case R_ADVISOR2:
						/*
						if (piece_count[7]==0)
                    board.boardsq[R_ADVISOR2]=k; //2;
                else board.boardsq[R_ADVISOR1]=k;
            piece_count[7]++;
            */
            if (i<=4)
            {	if (board.boardsq[R_ADVISOR2]==SQ_EMPTY)
            		board.boardsq[R_ADVISOR2]=k;
            	else board.boardsq[R_ADVISOR1]=k;
            }
            else
            {	if (board.boardsq[R_ADVISOR1]==SQ_EMPTY)
            		board.boardsq[R_ADVISOR1]=k;
            	else board.boardsq[R_ADVISOR2]=k;
            }
						break;

					case R_ELEPHAN1:
					//case R_ELEPHAN2:
						/*
						if (piece_count[8]==0)
                    board.boardsq[R_ELEPHAN2]=k; //2;
                else board.boardsq[R_ELEPHAN1]=k;
            piece_count[8]++;
            */
            if (i<=4)
            {	if (board.boardsq[R_ELEPHAN2]==SQ_EMPTY)
            		board.boardsq[R_ELEPHAN2]=k;
            	else board.boardsq[R_ELEPHAN1]=k;
            }
            else
            {	if ((board.boardsq[R_ELEPHAN2]==SQ_EMPTY) && piece_count[8]==2)
            		board.boardsq[R_ELEPHAN2]=k;
            	else board.boardsq[R_ELEPHAN1]=k;
            }
						break;

					case R_HORSE1:
					//case R_HORSE2:
						/*
						if (piece_count[9]==0)
                    board.boardsq[R_HORSE2]=k; //2;
                else board.boardsq[R_HORSE1]=k;
            piece_count[9]++;
            */
            // N N |
            if (i<=4)
            {	if (board.boardsq[R_HORSE2]==SQ_EMPTY)
            		board.boardsq[R_HORSE2]=k;
            	else board.boardsq[R_HORSE1]=k;
            }
            else // | N N
            {	if ((board.boardsq[R_HORSE2]==SQ_EMPTY) && piece_count[9]==2)
            		board.boardsq[R_HORSE2]=k;
            	else board.boardsq[R_HORSE1]=k;
            }
						break;

					case R_CANNON1:
					//case R_CANNON2:
						/*
						if (piece_count[10]==0)
                    board.boardsq[R_CANNON2]=k; //2;
                else board.boardsq[R_CANNON1]=k;
            piece_count[10]++;
            */
            if (i<5)
            {	if (board.boardsq[R_CANNON2]==SQ_EMPTY)
            		board.boardsq[R_CANNON2]=k;
            	else board.boardsq[R_CANNON1]=k;
            }
            else
            {	if ((board.boardsq[R_CANNON2]==SQ_EMPTY) && piece_count[10]==2)
            		board.boardsq[R_CANNON2]=k;
            	else board.boardsq[R_CANNON1]=k;
            }
						break;

					case R_ROOK1:
					//case R_ROOK2:
						/*
						if (piece_count[11]==0)
                    board.boardsq[R_ROOK2]=k; //2;
                else board.boardsq[R_ROOK1]=k;
            piece_count[11]++;
            */
            if (i<5)
            {	if (board.boardsq[R_ROOK2]==SQ_EMPTY)
            		board.boardsq[R_ROOK2]=k;
            	else board.boardsq[R_ROOK1]=k;
            }
            else
            {	if ((board.boardsq[R_ROOK2]==SQ_EMPTY) && piece_count[11]==2)
            		board.boardsq[R_ROOK2]=k;
            	else board.boardsq[R_ROOK1]=k;
            }
						break;

					case R_KING:
						board.boardsq[R_KING]=k;
						break;
					}
				}
			}



    memset(board.wBitRanks,0,sizeof(board.wBitRanks));
    memset(board.wBitFiles,0,sizeof(board.wBitFiles));
	board.bitpiece=0;
	memset(board.piececnt,0,sizeof(board.piececnt));

	  memset(board.bitattk,0,sizeof(board.bitattk));
//unsigned int bitpiece = 0; //RrRrCcCc HhHhEeEe BbBbPpPp PpPpPp00
//unsigned char boardsq[34];
//    2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23, 24,25,26,27, 28,29,30,31, 32,33
//    p P  p P p P  p P p  P   b  B  b  B   e  E  e  E   h  H  h  H   c  C  c  C   r  R  r  R   k  K

    for (i=2; i<34; i++)
    {
        sp = board.boardsq[i];
        //if (sp >=0 /* != SQ_EMPTY */)
        	if (NOTSQEMPTY(sp))
        {
            board.wBitRanks[nRank(sp)] ^= PreGen.wBitRankMask[sp];
            board.wBitFiles[nFile(sp)] ^= PreGen.wBitFileMask[sp];
            board.piececnt[i &61]++;
            if (i<B_KING) // not king
            {
						board.bitpiece ^= (1 << i); //BITPIECE_MASK[i];  //set bitpiece from i in boardsq[i], in effect reverse bit order 

						board.bitattk[ATTKAREA[sp]] ^= (1 << i);

            }
						//update piece val after vertical scan of board
						board.piece[sp]=i;
        }
    }
board.hkey = init_hkey(board);
    //IHkey=init_hkey(board);
    

    //board=board;
	//board.hkey=IHkey;
    board.Init_index(); //compressed boardsq may affect booksearch
    board.m_timeout=0;

    board.m_hisindex=0;
    board.ply = 0;
    
    for (int i=0;i<34;++i)
    {        
        board.piececnt[i]=0;
     }

		for (int i=0; i<10; i++)
		{
			board.bitattk[i] = 0;
		}	
    board.pointsum = 0;
    board.p_endgame = 0;
//    IComSide=board.m_side ;


    /*
    // print board after read FEN for debug
    printf("\n");
    fflush(stdout);
    for (int i=0; i<10; i++)
    {	for (int j=0; j<9; j++)
    	{
    	//fprintf(traceout, " %c ", PieceChar[board.piece[(i*9)+j]] );
    	printf(" %c ", PieceChar[board.piece[(i*16)+j]] );
    	}
    	//fprintf(traceout, "\n");
    	printf("\n");
    	fflush(stdout);
    }


		*/
		AdjustEndgame(board);
		
//		printf("*** ext_p_endgame=%d, board.p_endgame=%d\n", ext_p_endgame, board.p_endgame);
		
		if (ext_p_endgame != 1 && board.p_endgame == 1)
		{
			ext_p_endgame = 1;
			//printf("     ext_IMaxTime=%d -->", ext_IMaxTime);
			//ext_IMaxTime += (ext_IMaxTime >> 4); //3 2);
			printf("     ***endgame reached, pass ext_p_endgame=1 to next go time\n");
			fflush(stdout);

			
		}
		
}

/* from eleeye 3.21 hash.cpp
// 检测下一个着法是否稳定，有助于减少置换表的不稳定性
inline bool MoveStable(PositionStruct &pos, int mv) {
  // 判断下一个着法是否稳定的依据是：
  // 1. 没有后续着法，则假定是稳定的；
  if (mv == 0) {
    return true;
  }
  // 2. 吃子着法是稳定的；
  __ASSERT(pos.LegalMove(mv));
  if (pos.ucpcSquares[DST(mv)] != 0) {
    return true;
  }
  // 3. 可能因置换表引起路线迁移，使得路线超过"MAX_MOVE_NUM"，此时应立刻终止路线，并假定是稳定的。
  if (!pos.MakeMove(mv)) {
    return true;
  }
  return false;
}

// 检测后续路线是否稳定(不是循环路线)，有助于减少置换表的不稳定性
static bool PosStable(const PositionStruct &pos, int mv) {
  HashStruct hsh;
  int i, nMoveNum;
  bool bStable;
  // pos会沿着路线变化，但最终会还原，所以被视为"const"，而让"posMutable"承担非"const"的角色
  PositionStruct &posMutable = (PositionStruct &) pos;

  __ASSERT(mv != 0);
  nMoveNum = 0;
  bStable = true;
  while (!MoveStable(posMutable, mv)) {
    nMoveNum ++; // "!MoveStable()"表明已经执行了一个着法，以后需要撤消
    // 执行这个着法，如果产生循环，那么终止后续路线，并确认该路线不稳定
    if (posMutable.RepStatus() > 0) {
      bStable = false;
      break;
    }
    // 逐层获取置换表项，方法同"ProbeHash()"
    for (i = 0; i < HASH_LAYERS; i ++) {
      hsh = HASH_ITEM(posMutable, i);
      if (HASH_POS_EQUAL(hsh, posMutable)) {
        break;
      }
    }
    mv = (i == HASH_LAYERS ? 0 : hsh.wmv);
  }
  // 撤消前面执行过的所有着法
  for (i = 0; i < nMoveNum; i ++) {
    posMutable.UndoMakeMove();
  }
  return bStable;
}
*/ 
// end of eleeye 3.21 hash.cpp
/*
// 从置换表中获得主要变例路线，找到循环路线则返回"TRUE"
int Engine::GetPvStable(int *lpwmvPvLine, int mvFirst) 
{
  HashStruct *hsho; //, *lphsh;
  int i, nPvLineNum, bStable;
  MoveStruct tempmove;
  
  nPvLineNum = 0;  
  lpwmvPvLine[nPvLineNum] = mvFirst;     
  tempmove.move = mvFirst;
  bStable = 1;       
    while (true)
    {
    	
    	// 2. 吃子着法是稳定的；
  if (board.piece[tempmove.dest] != 0) 
  	{
    break;
  }
    	
    	
    	nPvLineNum ++;    
    	if (nPvLineNum >= 256)
    		break;	
    	makemovenoeval<0>(tempmove); //, 0);    	   
      if (checkloop(1)) 
      {        
        bStable = 0;
        break;
      }
      
  
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsho = hshItems + offset;
  for (i = 0; i < HASH_LAYERS; i++, hsho++) 
  {
    if (hsho->hkey == (board.hkey>>38))  //upper 26bits
    {
    	tempmove.move = get_hmv(hsho->hmvBest);
      lpwmvPvLine[nPvLineNum] = tempmove.move;
    	break;
    	// return hsh;
    }
  }
  
  if (i==HASH_LAYERS || tempmove.move == 0)
  	break;

    }
    
    // 5. 在主要变例列表中加入结束标志，然后还原前面执行过的所有着法
    lpwmvPvLine[nPvLineNum] = 0;   
    for (int i=0; i<nPvLineNum; i++)
    {        
        unmakemovenoeval();
    }
    
    return bStable;
}
*/
// 从置换表中获得主要变例路线，找到循环路线则返回"TRUE"
void Engine::GetPvLine(Board &board, move_t *lpwmvPvLine, int mvFirst) 
{
  HashStruct *hsho; //, *lphsh;
  int i, nPvLineNum; //,bStable;
  MoveStruct tempmove;
  
  nPvLineNum = 0;  
  lpwmvPvLine[nPvLineNum] = mvFirst;     
  tempmove.move = mvFirst;
         
    while (true)
    {
    	nPvLineNum ++;  
    	if (nPvLineNum >= MAX_PLY)  //1210 256)
    		break;	  	
    	board.makemovenoeval(tempmove, 0); //, 0);    	   
      if (board.checkloop(1)) //3
      {
        
        //bStable = 0;
        break;
      }
      
  
  unsigned int offset = ((board.hkey ) & nHashMask);
    hsho = hshItems + offset;
  for (i = 0; i < HASH_LAYERS; i++, hsho++) 
  {
    if (hsho->hkey == (board.hkey>>38))  //upper 26bits
    {
    	tempmove.move = get_hmv(hsho->hmvBest);
      lpwmvPvLine[nPvLineNum] = tempmove.move;
    	break;
    	// return hsh;
    }
  }
  
  if (i==HASH_LAYERS || tempmove.move == 0 || board.LegalKiller(tempmove) == 0)
  	break;

    }
    
    // 5. 在主要变例列表中加入结束标志，然后还原前面执行过的所有着法
    lpwmvPvLine[nPvLineNum] = 0;   
    for (int i=0; i<nPvLineNum; i++)
    {        
        board.unmakemovenoeval();
    }
}


void Engine::PutPvLine(Board &board, int *lpwmvPvLine, int m_depth, int best) 
{
  //HashStruct *hsho; //, *lphsh;
  int nPvLineNum; //,bStable;
  MoveStruct tempmove;
  
  nPvLineNum = 0;  
  //lpwmvPvLine[nPvLineNum] = mvFirst;     
  tempmove.move = lpwmvPvLine[0]; //mvFirst;
         
    while (true)
    {
    	if (tempmove.move == 0 || board.LegalKiller(tempmove) == 0)
    		break;
    	nPvLineNum ++;  
    	if (nPvLineNum >= MAX_PLY)  //1210 256)
    		break;	  

if (nPvLineNum > 1)
{	     
	// lazy smp bug - solved in 2892r by add board as param
	int hashmove = 0;
  hashmove = ProbeMoveQ(board);
  if (hashmove == 0 || hashmove != board.m_bestmove) //tempmove.move)
  {
  	//
  	char c_hashmove[5], c_bestmove[5];  //, c_rootpv[5];
  	MoveStruct hash_tempmove;
  	hash_tempmove.move = hashmove;
  	com2char(c_hashmove, hash_tempmove.from, hash_tempmove.dest );
  	hash_tempmove.move = board.m_bestmove;
  	com2char(c_bestmove, hash_tempmove.from, hash_tempmove.dest );  	
  	printf("     **PutPv back at depth=%d hashmove=%s best move=%s\n", m_depth, c_hashmove, c_bestmove);
  	fflush(stdout);
  	//
  	RecordHash(HASH_PV, m_depth, best, board.m_bestmove, board); //tempmove.move);
  }		
// lazy smp  
  //int hashmove = 0;
  //hashmove = ProbeMoveQ(board);
  //if (hashmove == 0 || hashmove != tempmove.move)
  //{
  //	RecordHash(HASH_PV, 0, Evalscore(), tempmove.move, board);
  //}		
}  
      			
    	
    	board.makemovenoeval(tempmove, 0); 	   
      if (board.checkloop(1))  //(3)
      {        
        //bStable = 0;
        break;
      }

tempmove.move = lpwmvPvLine[nPvLineNum];
  
    } // end while true
    
       
    for (int i=0; i<nPvLineNum; i++)
    {        
        
        board.unmakemovenoeval();
    }
}


/*
// 从置换表中获得主要变例路线，找到循环路线则返回"FALSE"
int Engine::GetPvLine(search_stack_t &ss) //, int mvFirst)
{
    //HashStruct hsh; //, *lphsh;
    int nPvLineNum = 0;
    int bStablePos = 1;
    MoveStruct tempmove;

    //for (; ; )
    while (true)
    {
        // 3. 如果没有置换表项，或者是空着，那么主要变例终止，否则把着法写入主要变例列表
        //if (i == HASH_LAYERS || hsh.mvBest == 0)
        if (ss.pv[nPvLineNum]==0)
        {
            break;
        }
        //lpwmvPvLine[nPvLineNum] = hsh.mvBest;


        // 4. 执行这个着法，如果出现重复局面，那么主要变例终止

        tempmove.move = ss.pv[nPvLineNum];
        // 2. 吃子着法是稳定的；
        if (board.piece[tempmove.dest])
        {
            //bStablePos = 1;
            //nPvLineNum --;
            break;
        }
        //makemoveNochk(tempmove);
        makemovenoeval<0>(tempmove); //, 0);

        nPvLineNum ++;
        if (checkloop(1))
        {
            bStablePos = 0;
            break;
        }
        //nPvLineNum ++;
    }

    // 5. 在主要变例列表中加入结束标志，然后还原前面执行过的所有着法
    //lpwmvPvLine[nPvLineNum] = 0;
    //for (nPvLineNum --; nPvLineNum >= 0; nPvLineNum --)
    for (int i=0; i<nPvLineNum; i++)
    {
        //unmakemoveNochk();
        unmakemovenoeval();
    }

    return bStablePos;
}
*/

//debug
#ifdef DEBUG
void Engine::PrintLog(char *szFileName)
{
    unsigned int n; //m
    //int k, nSrc, nDst, nCaptured;

    //FILE *out = fopen(szFileName, "a+"); //w+");   //use append

    //fprintf(out, "************************** Search Log ***************************\n");

    fprintf(out, "Search Depth: %3d  ",searchdepth);

    n = nTreeNodes + nLeafNodes + nQuiescNodes;
    //float TimeSpan = (clock() - board.m_startime) / 1000.0f;  // StartTimer/1000.0f;
    float TimeSpan = (GetTime() - board.m_startime) / 1000.0f;  // StartTimer/1000.0f;
    //fprintf(out, "Search Time    :   %8.3f Sec", TimeSpan);
    //fprintf(out, "  Speed (excl QS):   %8.0f nps", (nTreeNodes+nLeafNodes)/TimeSpan);
    //fprintf(out, "  Speed (overall):   %8.0f nps\n", n/TimeSpan);

    fprintf(out, "Tree:%10u Leaf:%10u Q:%10u %10u %8.3f Sec %8.0f NQNPS% 8.0f nps\n",nTreeNodes,nLeafNodes,nQuiescNodes,n,TimeSpan,(nTreeNodes+nLeafNodes)/TimeSpan,n/TimeSpan);

    //fprintf(out, "NullMoveCuts   = %u", nNullMoveCuts);
    //fprintf(out, "  NullMoveNodes  = %u", nNullMoveNodes);
    //fprintf(out, "  NullMove CutRate = %.2f%%\n\n", nNullMoveCuts/(float)nNullMoveNodes*100.0f);
//	fprintf(out, "  NullMove: Reduc=%3d  Ver=%3d  Cuts=%10u  Moves=%10u  Hits= %.2f%%\n", nullreduction, nullreduction+2, nNullMoveCuts, nNullMoveNodes, nNullMoveCuts/(float)nNullMoveNodes*100.0f);
//	fprintf(out, "  HistPrun: Shift=%3d  Ext=%3d  Vers=%10u  Nodes=%10u  ExtNodes=%10u  PerctVers= %.2f%%\n\n",
//	 HistPrunShift, ExtHistPrunShift,nHistPrunVers, nHistPrunNodes, nExtHistPrunNodes,  nHistPrunVers/(float)(nHistPrunNodes+nExtHistPrunNodes)*100.0f);
    fprintf(out, "  HistPrun =%10u  ExtHistPrun =%10u\n", nHistPrunNodes,nExtHistPrunNodes);
    /*
    	fprintf(out, "Hash表大小: %d Bytes  =  %d M\n", pHash->nHashSize*2*sizeof(CHashRecord), pHash->nHashSize*2*sizeof(CHashRecord)/1024/1024);
    	fprintf(out, "Hash覆盖率: %d / %d = %.2f%%\n\n", pHash->nHashCovers, pHash->nHashSize*2, pHash->nHashCovers/float(pHash->nHashSize*2.0f)*100.0f);

    	unsigned int nHashHits = pHash->nHASH_ALPHA+pHash->nHashExact+pHash->nHASH_BETA;
    	fprintf(out, "Hash命中: %d = %d(alpha:%.2f%%) + %d(exact:%.2f%%) +%d(beta:%.2f%%)\n", nHashHits, pHash->nHASH_ALPHA, pHash->nHASH_ALPHA/(float)nHashHits*100.0f, pHash->nHashExact, pHash->nHashExact/(float)nHashHits*100.0f, pHash->nHASH_BETA, pHash->nHASH_BETA/(float)nHashHits*100.0f);
    	fprintf(out, "命中概率: %.2f%%\n", nHashHits/float(nTreeNodes+nLeafNodes)*100.0f);
    	fprintf(out, "树枝命中: %d / %d = %.2f%%\n", nTreeHashHit, nTreeNodes, nTreeHashHit/(float)nTreeNodes*100.0f);
    	fprintf(out, "叶子命中: %d / %d = %.2f%%\n\n", nLeafHashHit, nLeafNodes, nLeafHashHit/(float)nLeafNodes*100.0f);


    	fprintf(out, "杀手移动 : \n");
    	k = n = 0;
    	for(m=0; m<MaxKiller; m++)
    	{
    		fprintf(out, "    Killer   %d : %8d /%8d = %.2f%%\n", m+1, nKillerCuts[m], nKillerNodes[m], nKillerCuts[m]/float(nKillerNodes[m]+0.001f)*100.0f);
    		n += nKillerCuts[m];
    		k += nKillerNodes[m];
    	}
    	fprintf(out, "    杀手剪枝率 : %8d /%8d = %.2f%%\n\n", n, k, n/float(k+0.001f)*100.0f);

    	fprintf(out, "Hash冲突   : %d\n", pHash->nCollision);
    	fprintf(out, "Null&Kill  : %d\n", pHash->nCollision-nHashMoves);
    	fprintf(out, "HashMoves  : %d\n", nHashMoves);
    	fprintf(out, "HashCuts   : %d\n", nHashCuts);
    	fprintf(out, "Hash剪枝率 : %.2f%%\n\n", nHashCuts/(float)nHashMoves*100.0f);
    */
#ifdef DEBUG
//	fprintf(out, " Killer BetaNodes: %d", nBetaNodes);
//	fprintf(out, " HashMoves  : %d", nHashMoves);
//	fprintf(out, " HashCuts   : %d\n", nHashCuts);
    fprintf(out, " FUTILITY_MARGIN : %d,  nFutility : %d ", FUT_MARGIN[1], nFutility);
    fprintf(out, " FUT_MARGIN[2] : %d,  nExtFutility : %d\n", FUT_MARGIN[2], nExtFutility);
    fflush(out);
#endif
//fprintf(out, " BetaNodes, BetaMoveSum: %d %d %.2f%%", nBetaNodes, nBetaMoveSum, nBetaNodes/(float)(nBetaMoveSum)*100.0f);
//	fprintf(out, " BetaCutAt1: %d %.2f%%\n", nBetaCutAt1, nBetaCutAt1/(float)(nBetaNodes)*100.0f);
    /*
    	n = nCheckCounts[1] + nCheckCounts[2] + nCheckCounts[3] + nCheckCounts[4];
    	fprintf(out, "将军次数: %d\n", n);
    	fprintf(out, "探测次数: %d\n", nCheckCounts[0]);
    	fprintf(out, "成功概率: %.2f%%\n", n/(float)nCheckCounts[0]*100.0f);
    	fprintf(out, "    车帅: %d\n", nCheckCounts[1]);
    	fprintf(out, "    炮将: %d\n", nCheckCounts[2]);
    	fprintf(out, "    马将: %d\n", nCheckCounts[3]);
    	fprintf(out, "    兵卒: %d\n\n", nCheckCounts[4]);

    	fprintf(out, "CheckEvasions = %d\n", nCheckEvasions);
    	fprintf(out, "解将 / 将军   = %d / %d = %.2f%%\n\n", nCheckEvasions, n, nCheckEvasions/float(n)*100.0f);


    	// 显示主分支
    	int BoardStep[90];
    	for(n=0; n<90; n++)
    		BoardStep[n] = Board[n];

    	static const char ChessName[14][4] = {"帥","車","炮","马","象","士","卒", "將","車","炮","馬","相","仕","兵"};

    	fprintf(out, "\n主分支：PVLine***HashDepth**************************************\n");
    	for(m=0; m<nPvLineNum; m++)
    	{
    		nSrc = PvLine[m].src;
    		nDst = PvLine[m].dst;
    		nCaptured = BoardStep[nDst];

    		// 回合数与棋步名称
    		fprintf(out, "    %2d. %s", m+1, GetStepName( PvLine[m], BoardStep ));

    		// 吃子着法
    		if(nCaptured>=0 && nCaptured<32)
    			fprintf(out, " k-%s", ChessName[nPieceType[nCaptured]]);
    		else
    			fprintf(out, "     ");

    		// 搜索深度
    		fprintf(out, "  depth = %2d", (PvLine[m].key)>>16);

    		// 将军标志
    		nCaptured = short((PvLine[m].key)&0xFFFF);
    		if(nCaptured)
    			fprintf(out, "   Check Extended 1 board.ply ");
    		fprintf(out, "\n");

    		BoardStep[nDst] = BoardStep[nSrc];
    		BoardStep[nSrc] = -1;
    	}

    	fprintf(out, "\n\n***********************第%2d 回合********************************\n\n", (nCurrentStep+1)/2);
    	fprintf(out, "***********************电脑生成：%d 个分支**********************\n\n", nFirstLayerMoves);
    	for(m=0; m<(unsigned int)nFirstLayerMoves; m++)
    	{
    		nSrc = FirstLayerMoves[m].src;
    		nDst = FirstLayerMoves[m].dst;

    		// 寻找主分支
    		if(PvLine[0].src==nSrc && PvLine[0].dst==nDst)
    		{
    			fprintf(out, "*PVLINE=%d***********Nodes******History**************************\n", m+1);
    			fprintf(out, "*%2d.  ", m+1);
    		}
    		else
    			fprintf(out, "%3d.  ", m+1);

    		//n = m==0 ? FirstLayerMoves[m].key : FirstLayerMoves[m].key-FirstLayerMoves[m-1].key;	// 统计分支数目
    		n = FirstLayerMoves[m].key;																// 统计估值
    		fprintf(out, "%s = %6d    hs = %6d\n", GetStepName(FirstLayerMoves[m], Board), n, HistoryRecord[nSrc][nDst]);
    	}


    	fprintf(out, "\n\n***********************历史记录********************************\n\n", (nCurrentStep+1)/2);

    	char ArgPtr[5];
    	for(m=0; m<=(int)nCurrentStep; m++)
    	{
    		nSrc = StepRecords[m].src;
    		nDst = StepRecords[m].dst;

    		ArgPtr[0] = nSrc/10 + 'a';
    		ArgPtr[1] = nSrc%10 + '0';
    		ArgPtr[2] = nDst/10 + 'a';
    		ArgPtr[3] = nDst%10 + '0';
    		ArgPtr[4] = '\0';

    		fprintf(out, "%3d. %s  %2d  %2d  %12u\n", m, ArgPtr, StepRecords[m].capture, StepRecords[m].incheck, StepRecords[m].zobrist);
    	}
    */

    // 关闭文件
//debug
//	fclose(out);
}
//debug
#endif

#ifdef PERFT

uint64 Engine::Perft(int depth)
{
   //assert(depth >= 1);
    assert(depth >= 0);
    if(depth==0)
    {
       return 1;
    }
    //MOVE move_list[256];
    int i, size; //n_moves, i;
    uint64 nodes = 0;

    //n_moves = GenerateLegalMoves(move_list);//合法着法
    int incheck=0;
    if (m_hisindex > 0)
        incheck=m_hisrecord[m_hisindex-1].htab.Chk;
    else
    {
        //incheck=board.IsInCheck(board.m_side, 0);	//not singlechk
        incheck=board.IsInCheck<0>(board.m_side);	//not singlechk
        //m_hisrecord[0].htab.Chk = incheck;
    }
    MoveTabStruct movetab[111], ncapmovetab[64];
    MoveStruct tempmove;

    long ncapsize; //=0;

    //int nBanMoves=0;
    if (incheck)
    {
        size=board.GenChkEvasCap(&movetab[0], incheck);
        ncapsize=board.GenChkEvasNCap(&movetab[size], incheck);
        size += ncapsize;
    }
    else
    {
        size=board.GenCap(&movetab[0], &ncapmovetab[0], ncapsize);
        memcpy(&movetab[size], &ncapmovetab[0], ncapsize * 4);
        size += ncapsize;
        ncapsize=board.GenNonCap(&movetab[size], depth);
        size += ncapsize;
    }



//----------------------------------
    // if(depth <= 1)
    //{
    //   return size; //n_moves;
    //}

    for (int i = 0; i < size; i++)
    {
        //MakeMove(move_list);
    		tempmove.move = movetab[i].table.move;
    		if ( board.makemovenoeval(tempmove, 1) < 0) //, 1) < 0 )	//illegal move
    			continue;
        nodes += Perft(depth - 1);
        //UndoMove(move_list);
        board.unmakemovenoeval();
    }
    return nodes;
}
/*
uint64_t Perft(int depth)
{
   assert(depth >= 1);

    MOVE move_list[256];
    int n_moves, i;
    uint64_t nodes = 0;

    n_moves = GenerateLegalMoves(move_list);//合法着法

     if(depth == 1)
    {
       return n_moves;
    }

    for (i = 0; i < n_moves; i++)
    {
        MakeMove(move_list);
        nodes += Perft(depth - 1);
        UndoMove(move_list);
    }
    return nodes;
}

下面是bugchess的结果, nodes指的是合理着法的个数。
---------------------------------------------------------------------------------------------------
rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w
     depth     nodes    checks            captures
         1        44         0                   2
         2      1920         6                  72
         3     79666       384                3159
         4   3290240     19380              115365
         5 133312995    953251             4917734


--------------------------------------------------------------------------------------------------
r1ba1a3/4kn3/2n1b4/pNp1p1p1p/4c4/6P2/P1P2R2P/1CcC5/9/2BAKAB2 w
     depth     nodes    checks            captures
         1        38         1                   1
         2      1128        12                  10
         3     43929      1190                2105
         4   1339047     21299               31409
         5  53112976   1496697             3262495

---------------------------------------------------------------------------------------------------
r2akab1r/3n5/4b3n/p1p1pRp1p/9/2P3P2/P3P3c/N2C3C1/4A4/1RBAK1B2 w
     depth     nodes    checks            captures
         1        58         1                   4
         2      1651        54                  70
         3     90744      1849                6642
         4   2605437     70934              128926
         5 140822416   3166538            10858766
*/
#endif //end PERFT
