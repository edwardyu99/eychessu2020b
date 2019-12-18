//#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Eychessu.cpp V2019f                                                                                //
// Author: Edward Yu                                                                                         //
// Reference: HIce 1.02 + EleEye 1.5
// *******************************************************************************************************//
// 功能：                                                                                                 //
// 1. 控制台应用程序的入口点                                                                              //
// 2. 通过ucci协议与界面程序之间进行通讯    
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 20191006 - 2019f try to ref stockfish 10 source for improvement
// 20190923 - 289bc add drwwval and fix time and moves remain 
// 20190903 - 2892z dec opening (no capture) IMaxtime, time control for 60 movesI
// 20190831 - 2892y Ncore=3, 1G hash, sf10 lazysmp, ncore=1 Mdepth 8 if maxtime< 120
// 20190831 - 2892x Ncore=3, 1G hash, sf10 lazysmp
// 20160731 - 2892v stockfish forum suggest 1GB may be too large. go for 512MB hash and also revert to sf skip depth policy
// 20160730 - 2892u win 6-2 2vs1core w/o timeout. IMaxTime = ((UcciComm.nTime-200)/(25 + min(25,p_movenum)) + UcciComm.nIncrement) * 3/4;
// 20160724 - 2892u use exchess time management: assume remain=30 moves 
// 20160724 - 2892u max(8, (UcciComm.nTime/25) to prevent timeout
// 20160721 - 2892t use stockfish-7 aspwin
// 20160717 - 2892s implement lazy smp (use c++11 thread)
// 20160715 - 2892r implement lazy smp
//                  to-do: putPV back in hash. timeout problem at IMaxTime < 10
// 20160703 - 2892q try to rewrite to use lazy smp - fix hash bug (need to include board in probe,recordhash functions
// 20140407 - 1892w change to use local board.killer for smp
// 20140324 - 1894a fix multiple p_banmove (max 16)
#include "locale.h"
#include <windows.h>
#include <string.h>
//#include "ucci.h"
//#include "Engine.h"
//#include "EFen.h"

#include <stdio.h>
#include "base.h"
#include "base2.h"
//#include "../utility/popcnt.h"
#include "ucci.h"
//#include "pregen.h"
//#include "position.h"
//#include "hash.h"
//#include "search.h"

// getprocessorcount not supported in vc2015
//#include <concrtrm.h>  //for GetProcessorCount
//using namespace concurrency;

//#include <ppl.h> //parallel pattern library
//#include "ppl_extras.h"
//using namespace Concurrency;

//#include <concrt.h>  //for SchedulerPolicy
//using namespace concurrency;

//#include "pool.hpp"  //hang
//threadpool::pool f1;

//#include "threadpool11.h"
//threadpool11::Pool pool;
    	
//#include "tbb/task_scheduler_init.h"
//#include "tbb/task.h"
//#include "tbb/task_group.h"
//#include "tbb/parallel_invoke.h"
//using namespace tbb;
//structured_task_group taskgp;

//#include <QuickThread.h>
//using namespace qt;
//#include "ThreadPool.h"
//ThreadPool pool(4);

#include "Engine.h"
//----------------------------
// Define a __cpuid() function for gcc compilers, for Intel and MSVC
// is already available as an intrinsic.
#if defined(_MSC_VER)
//#include <intrin.h>  //in board.h in Engine.h
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
inline void __cpuid(int CPUInfo[4], int InfoType)
{
  int* eax = CPUInfo + 0;
  int* ebx = CPUInfo + 1;
  int* ecx = CPUInfo + 2;
  int* edx = CPUInfo + 3;

  *eax = InfoType;
  *ecx = 0;
  __asm__("cpuid" : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                  : "0" (*eax), "2" (*ecx));
}
#else
inline void __cpuid(int CPUInfo[4], int)
{
   CPUInfo[0] = CPUInfo[1] = CPUInfo[2] = CPUInfo[3] = 0;
}
#endif

//---------------------------
//#include <windows.h>
#include <malloc.h>    
//#include <stdio.h>
#include <tchar.h>

typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);


// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;
    
    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

// replaced by concurrency - backout for vc2015
//int _cdecl _tmain ()
int _cdecl nCore ()
{
    LPFN_GLPI glpi;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
//    DWORD processorL1CacheCount = 0;
//    DWORD processorL2CacheCount = 0;
//    DWORD processorL3CacheCount = 0;
//    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
 //   PCACHE_DESCRIPTOR Cache;

    glpi = (LPFN_GLPI) GetProcAddress(
                            GetModuleHandle(TEXT("kernel32")),
                            "GetLogicalProcessorInformation");
    if (NULL == glpi) 
    {
        _tprintf(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
        return (1);
    }

    while (!done)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if (buffer) 
                    free(buffer);

                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        returnLength);

                if (NULL == buffer) 
                {
                    _tprintf(TEXT("\nError: Allocation failure\n"));
                    return (1); //(2);
                }
            } 
            else 
            {
                _tprintf(TEXT("\nError %d\n"), GetLastError());
                return (1); //(3);
            }
        } 
        else
        {
            done = TRUE;
        }
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
    {
        switch (ptr->Relationship) 
        {
        	
//        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
//            numaNodeCount++;
//            break;

        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
            break;

//        case RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
//            Cache = &ptr->Cache;
//            if (Cache->Level == 1)
//            {
//                processorL1CacheCount++;
//            }
//            else if (Cache->Level == 2)
//            {
//                processorL2CacheCount++;
//            }
//            else if (Cache->Level == 3)
//            {
//                processorL3CacheCount++;
//            }
//            break;

//        case RelationProcessorPackage:
            // Logical processors share a physical package.
//            processorPackageCount++;
//            break;

//        default:
//            _tprintf(TEXT("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));            
//            break;
            
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

//    _tprintf(TEXT("\nGetLogicalProcessorInformation results:\n"));
//    _tprintf(TEXT("Number of NUMA nodes: %d\n"), 
//             numaNodeCount);
//    _tprintf(TEXT("Number of physical processor packages: %d\n"), 
//             processorPackageCount);
//    _tprintf(TEXT("Number of processor cores: %d\n"), 
//             processorCoreCount);
//    _tprintf(TEXT("Number of logical processors: %d\n"), 
//             logicalProcessorCount);
//    _tprintf(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"), 
//             processorL1CacheCount,
//             processorL2CacheCount,
//             processorL3CacheCount);
    
    free(buffer);

    //return 0;
    //return processorCoreCount;  //PHYSICAL NCORE
    return logicalProcessorCount - 1;
}
//

 void Str2Move(unsigned int MoveStr, int &Src, int &Dst) {
    char *ArgPtr;
    ArgPtr = (char *) &MoveStr;
    //Src = (ArgPtr[0] - 'a' ) + (('9'  - ArgPtr[1]) * 9);
    //Dst = (ArgPtr[2] - 'a' ) + (('9'  - ArgPtr[3]) * 9);
    Src = (ArgPtr[0] - 'a' ) + (('9'  - ArgPtr[1]) <<4);
    Dst = (ArgPtr[2] - 'a' ) + (('9'  - ArgPtr[3]) <<4);
  } //  把字符串转换成着法

unsigned char lut[65536];
/*
unsigned char lut[256] =
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
#ifdef _WIN32
const int c_BackSlash = '\\';
#else
const int c_BackSlash = '/';
#endif

char g_bookfile[120]; 
//int egtime = 30; //32;
//int ext_deltamg = 37; //34;
//int ext_deltaeg = 24; //22;
//int ext_drawval = 5;  //5 aft clop   
//int ext_remainmv = 27;
//int ext_remaineg = 28;
                                                                                          
int ext_p_endgame = 0;
int ext_IMaxTime = 0x7fffffff;

//int p_endpoint_chg = 0;
int p_feedback_move = 0;
int p_nBanmove = 0;
int p_banmove[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//int p_banmove;  //single for original 1892w
int p_movenum = 1;
int p_bookfound = 0;

/*
int FUTPAWN=23; //28; //32; //33; //32;
int ENDFUTPAWN=26; //24; //28; //29; //28; //30 //31(1880u) //26(1880s) //33 //36
int DELTA_MARGIN=34; //36; //32; //33; //32;
int SINGULAR_MARGIN=14; //18; //16; //14; //16;
int DELTA_MARGIN_EG=22; //26; //28; //31; //28;
//int SINGULAR_MARGIN_EG=14; //17; //14; 

int min_rootsplit_depth=99;     //13
int min_pvsplit_depth=11; //17;     //13
int min_npvsplit_depth=11; //17;  //13
int min_size=2; //4; //8; //20; //4; //8;            //4
*/
//int pvdepth=13; //13
//int ini_NBOOK; //bef 24, aft 23
//int VNE;   //aft clop 197   
//int VEE;   //aft clop 49
//int VPER;  //aft clop 129
//int VC,VN; //bef clop 202,190, aft 201,192
//int VR,VRE; //bef clop 460,460, aft 466,456
//int panictime; //bef clop 100 aft 52
//int razor, razorinc; //aft clop 68 12
int NCORE;
int POPCNT_CPU; 



int main(int argc, char* argv[])
{
	
	//ThreadPool pool(4);
	//task_scheduler_init init; // tbb threads creation
	
	// Worker Threads = # hardware threads + 1 I/O thread
  //    qtInit      qtInit(-1); // -1 ==  use defaults
      
// detects support for popcnt instruction at runtime
	  int CPUInfo[4] = {-1};
  __cpuid(CPUInfo, 0x00000001);
  POPCNT_CPU = (CPUInfo[2] >> 23) & 1;

/*  
 SYSTEM_INFO sysinfo;
 GetSystemInfo( &sysinfo );
 NCORE = sysinfo.dwNumberOfProcessors;
//NCORE = NCORE/2;
if (NCORE==0) NCORE=1;
*/

NCORE = nCore();
//NCORE = GetProcessorCount();   //use MS Concurrency Runtime -- backout for vc2015
printf("info NCORE=%d\n", NCORE);
fflush(stdout);

//SchedulerPolicy policy(1, ContextPriority, THREAD_PRIORITY_HIGHEST);
//,SchedulingProtocol,
 //EnhanceScheduleGroupLocality
// EnhanceForwardProgress
//);
//concurrency::Scheduler::SetDefaultSchedulerPolicy(policy);
	
   // start the threadpool with num_threads maximum threads and threads
    // that despawn automatically after 1000 idle milliseconds
    //threadpool::pool f1(NCORE, 1000);
    
//threadpool11::Pool pool(NCORE);
//20190917 setup EYCHESSU.INI for clop in \real 

//	char inifile[120];
//  LocatePath(inifile, "EYCHESSU.INI");  
//  razor = GetPrivateProfileInt("param","razor",68,inifile);  //59-79 default 69 clop=68
//  razorinc = GetPrivateProfileInt("param","razorinc",12,inifile);  //0-18 default 9 clop=12
//	printf("inifile=%s\n", inifile);
//    panictime = GetPrivateProfileInt("param","panictime",100,inifile);  //5-195 default 100 
//    egtime = GetPrivateProfileInt("param","egtime",30,inifile);  //20-44 default 32 
//    ext_deltamg = GetPrivateProfileInt("param","deltamg",34,inifile);  //24-44 default 34 
//    ext_deltaeg = GetPrivateProfileInt("param","deltaeg",22,inifile);  //12-32 default 22 
//      ext_remainmv = GetPrivateProfileInt("param","remainmv",30,inifile);  //20-40 default 27 
//      ext_remaineg = GetPrivateProfileInt("param","remaineg",30,inifile);  //20-40 default 28
//		ext_drawval = GetPrivateProfileInt("param","drawval",5,inifile);  //0-10 default 5 
//		VR = GetPrivateProfileInt("param","VR",460,inifile);  //450-470 default 460 
//		VRE = GetPrivateProfileInt("param","VRE",460,inifile);  //450-470 default 460 
//		VC = GetPrivateProfileInt("param","VC",202,inifile);  //187-217 default 202 
//		VN = GetPrivateProfileInt("param","VN",190,inifile);  //175-205 default 190 
	
//	ini_NBOOK = GetPrivateProfileInt("param","param1",23,inifile);  //3-60 default 23 
//		VNE = GetPrivateProfileInt("param","VNE",194,inifile);  //184-204 default 194 
//		VEE = GetPrivateProfileInt("param","VEE",53,inifile);  //43-63 default 53 
//		VPER = GetPrivateProfileInt("param","VPER",127,inifile);  //117-137 default 127 
		
//Engine.cpp		// 2893a - clop VNE VEE VPER
//  int VNE=197;  //bef clop 194;
//  int VEE=49;   //bef clop 53;
//  int VPER=129; //bef clop 127;
	
//	pvdepth  = GetPrivateProfileInt("param","param1",13,inifile); 
//  min_rootsplit_depth  = GetPrivateProfileInt("param","param1",13,inifile); 
//	min_pvsplit_depth  = GetPrivateProfileInt("param","param2",13,inifile);
//  min_npvsplit_depth = GetPrivateProfileInt("param","param3",13,inifile); 
//  min_size           = GetPrivateProfileInt("param","param4",4,inifile); 
	 			 
//  FUTPAWN  = GetPrivateProfileInt("param","param1",28,inifile); 
//  ENDFUTPAWN = GetPrivateProfileInt("param","param2",24,inifile); 
//  DELTA_MARGIN = GetPrivateProfileInt("param","param3",36,inifile); 
//  SINGULAR_MARGIN = GetPrivateProfileInt("param","param4",18,inifile); 
//  DELTA_MARGIN_EG = GetPrivateProfileInt("param","param5",26,inifile); 
//  SINGULAR_MARGIN_EG = GetPrivateProfileInt("param","param6",14,inifile); 

	setlocale( LC_ALL, "chs" );
	int n, i;
	//for (i = 0; i < 65536; i++)
  //    lut[i] = PopCnt16((uint16)i);
  lut[0] = 0;
   for (int i = 1; i < 65536; i++)
      lut[i] = lut[i / 2] + (i & 1);    

	//const char *BoolValue[2] = { "false", "true" };
	//const char *ChessStyle[3] = { "solid", "normal", "risky" };
	UcciCommStruct UcciComm;
	UcciCommEnum IdleComm;
	//CommDetail Command;
	//int ThisMove;
//	char *BackSlashPtr;

	int Src,Dst;
	MoveStruct moveS;
#ifdef _WIN64
	printf("Eychessu64 2019f (logical NCORE=%d, popcnt=%d) by Edward Yu\n", NCORE, POPCNT_CPU);
#else	
	printf("Eychessu 2019f (logical NCORE=%d, popcnt=%d) by Edward Yu\n", NCORE, POPCNT_CPU);
#endif	
	fflush(stdout);

	//引擎接收"ucci"指令
	if(BootLine() == UCCI_COMM_UCCI) //e_CommUcci)
	{
		//printf("info ucci command received\n");
		//fflush(stdout);
		//2892w - chg (29)=512MB to (30)=1GB hash for lazy smp, (31)=2GB cannot loaded in BHGui 3.6 
//#ifdef _WIN64
//		NewHash(29); //(29) // (27); //(27); //(27)88i; //(26); // 24=16MB, 25=32MB, 26=64MB, 27=128MB, 28=256MB, 29=512MB  ...
//#else
    NewHash(30);
//#endif
    
		Engine EEngine;

		//printf("info Engine EEgnine ok\n");
		//fflush(stdout);

		// set bookfile path
		//printf("info argc=%d argv[0]=%s\n", argc, argv[0]);
		//fflush(stdout);
    /*
        	if (argc > 0) {
      			BackSlashPtr = strrchr(argv[0], c_BackSlash);
      			if (BackSlashPtr == NULL) {
        			strcpy(g_bookfile, "EYBOOK.DAT");
      			} else {
        			strncpy(g_bookfile,  argv[0], BackSlashPtr + 1 - argv[0]);
        			strcpy(g_bookfile + (BackSlashPtr + 1 - argv[0]), "EYBOOK.DAT");
      				}
    		} else {
      			strcpy(g_bookfile,  "EYBOOK.DAT");
    			}
    */			
//#ifdef _WIN64
//    LocatePath(g_bookfile, "EYBOOK64.TXT"); 
//#else    
    LocatePath(g_bookfile, "EYBOOK.TXT"); 
//#endif    
		//printf("info bookfile path=%s\n", g_bookfile);
		//fflush(stdout);

		//NewHash(27); //(27); //(26); // 24=16MB, 25=32MB, 26=64MB, 27=128MB, 28=256MB, 29=512MB  ...


		//显示引擎的名称、版本、作者和使用者
#ifdef _WIN64		
		printf ("id name Eychessu64 V2019f\n"); fflush(stdout);
#else		
    printf ("id name Eychessu V2019f\n"); fflush(stdout);
#endif
		printf ("id copyright Edward Yu \n"); fflush(stdout);
		printf ("id author Edward Yu \n");  fflush(stdout);
		printf ("id user YOU, 2008\n");  fflush(stdout);
		printf("option usemillisec type check default true\n"); fflush(stdout);
		printf("option hashsize type spin min 16 max 2048 default 1024\n");
    fflush(stdout);

		// ucciok 这是ucci指令的最后一条反馈信息，表示引擎已经进入用UCCI协议通讯的状态
		printf("ucciok\n\n");
		fflush(stdout);



		// 设定标准输出和初始局面
		//EEngine.IRead("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r");
		//printf("position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1\n\n");
		//fflush(stdout);


		// 开始解释执行UCCI命令
		do
		{
			// Debug��Ϊfalse
			IdleComm = IdleLine(UcciComm, false);
			switch (IdleComm)
			{
				// isready 检测引擎是否处于就绪状态，其反馈信息总是readyok，该指令仅仅用来检测引擎的“指令接收缓冲区”是否能正常容纳指令。
				// readyok 表明引擎处于就绪状态(即可接收指令的状态)，不管引擎处于空闲状态还是思考状态。
				case UCCI_COMM_ISREADY:
					printf("readyok\n");
					fflush(stdout);
					break;

				// stop 中断引擎的思考，强制出着。后台思考没有命中时，就用该指令来中止思考，然后重新输入局面。
				case UCCI_COMM_STOP:
					EEngine.IStopThink();
					//printf("nobestmove\n");
					//fflush(stdout);
					break;

				// position fen 设置“内置棋盘”的局面，用fen来指定FEN格式串，moves后面跟的是随后走过的着法
				case UCCI_COMM_POSITION:
					p_nBanmove = 0;
					// 将界面传来的Fen串转化为棋局信息
					//ThisSearch.fen.FenToBoard(Board, Piece, ThisSearch.Player, ThisSearch.nNonCapNum, ThisSearch.nCurrentStep, Command.Position.FenStr);
					//ThisSearch.InitBitBoard(ThisSearch.Player, ThisSearch.nCurrentStep);
					//printf("Info start Loading Fen\n");
					//fflush(stdout);
					
					//if (strstr(UcciComm.szFenStr, "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR") !=NULL)
					//{

						//ext_p_endgame = 0;

        		//p_movenum = 0;
        		
        		//if (p_movenum==0)
        		//p_bookfound = 0;
				  //}
					EEngine.IRead(EEngine.board, UcciComm.szFenStr);
					//printf("Info Load Fen OK!\n");
					//fflush(stdout);

					p_feedback_move = 0;
					// 将局面走到当前，主要是为了更新着法记录，用于循环检测。
          
					for(n=0; n<UcciComm.nMoveNum; n++)
					{

						Str2Move(UcciComm.lpdwMovesCoord[n],Src,Dst);
					   	//printf("info %d,%d\n",Src,Dst);
						//EEngine.IMakeMove(Src,Dst);
						//EEngine.IMakeMove(MOVE(Src,Dst,0));
						
						moveS.from=Src;
						moveS.dest=Dst;
						EEngine.IMakeMove(EEngine.board, moveS);
						p_feedback_move = moveS.move;

						//char charmove[5];
						//com2char(charmove, moveS.from, moveS.dest );
    				//printf("Info fenmove src=%d dst=%d %s\n",moveS.from,moveS.dest, charmove);
					}
					EEngine.board.ply = 0;  //1892w for checkloop for chase ???
					
					  //char charmove[5]; 
					  //printf("info m_hisindex=%d\n", EEngine.board.m_hisindex);
            //      	for (int k=0; k<EEngine.board.m_hisindex; k++)
            //      	{
            //      		com2char(charmove, EEngine.board.m_hisrecord[k].htab.from,
            //      		EEngine.board.m_hisrecord[k].htab.dest);
            //      		printf("  %s",charmove);
            //      	}		     
            //      	printf("\n"); 
					

					break;

				//  banmoves 为当前局面设置禁手，以解决引擎无法处理的长打问题。当出现长打局面时，棋手可以操控界面向引擎发出禁手指令。
				case UCCI_COMM_BANMOVES:
					//ThisSearch.nBanMoveNum = Command.BanMoves.MoveNum;
					p_nBanmove = UcciComm.nBanMoveNum;
					for(n=0; n<UcciComm.nBanMoveNum; n++)
					{	Str2Move(UcciComm.lpdwBanMovesCoord[n],Src,Dst);
						//ThisSearch.BanMoveList[n] = Move(Command.BanMoves.CoordList[n]);
						moveS.from=Src;
						moveS.dest=Dst;
						p_banmove[n] = moveS.move;
					
					//printf("Info banmoves num=%d of %d, Src=%d, Dst=%d\n", n, UcciComm.nBanMoveNum, Src, Dst); //, UcciComm.lpdwBanMovesCoord[n]);
					//		fflush(stdout);
					}		
					break;

				// setoption  设置引擎各种参数
				case UCCI_COMM_SETOPTION:
				 switch (UcciComm.Option) {
				 	case UCCI_OPTION_HASHSIZE:
				 		if (UcciComm.nSpin > 0)
				 		{
        		DelHash();
        		i = 19; // 小于1，分配0.5M置换表       		
        		while (UcciComm.nSpin > 0) {
          		UcciComm.nSpin /= 2;
          		i ++;
        		}
        		NewHash(MAX(i, 24)); // 最小的置换表设为16M
        	}
        		break;

        	case UCCI_OPTION_NEWGAME:

        		p_bookfound = 0;
        		p_movenum = 1;
            ext_p_endgame = 0;
        		break;

				 	default:
          	break;
        }

					break;

				// Prepare timer strategy according to "go depth %d" or "go ponder depth %d" command
				case UCCI_COMM_GO:
				//case e_CommGoPonder:
				  
					switch (UcciComm.Go)
					{
						// 固定深度
						case UCCI_GO_DEPTH:
							//ThisSearch.Ponder = 2;
							//ThisSearch.MainSearch(Command.Search.DepthTime.Depth);
							
							//IMaxTime = 0xefff; //infinity time for go_depth
							//EEngine.IMaxDepth = UcciComm.nDepth + 1;
							
							if (UcciComm.nDepth == 0)
							{	
								ext_IMaxTime = 30000;  //30 sec for depth 0
								EEngine.IMaxDepth = 32;
							}	
							else	
							{
								ext_IMaxTime = 0x0fffffff; //infinity time for go_depth	
								//IMaxTime = 120000; // 2 mins time for go_depth	
								//for go infinity, ucci.cpp will chg to go depth 48
							EEngine.IMaxDepth = UcciComm.nDepth + 1;
						  }
							
							//printf("Info Maxdepth:%d\n",EEngine.IMaxDepth);
							printf("Info Maxdepth:%d (NCORE=%d, popcnt=%d)\n",UcciComm.nDepth,NCORE,POPCNT_CPU);
							//printf("MaxTime:%d\n",IMaxTime);
							fflush(stdout);
							EEngine.ISearch();
							break;

						// 时段制： 分配时间 = 剩余时间 / 要走的步数
						case UCCI_GO_TIME_MOVESTOGO:
							// 对于时段制，把剩余时间平均分配到每一步，作为适当时限。
          // 剩余步数从1到5，最大时限依次是剩余时间的100%、90%、80%、70%和60%，5以上都是50%
          //Search.nProperTimer = UcciComm.nTime / UcciComm.nMovesToGo;
          //Search.nMaxTimer = UcciComm.nTime * MAX(5, 11 - UcciComm.nMovesToGo) / 10;

							//printf("%d\n", Command.Search.TimeMode.MovesToGo);
							//ThisSearch.MainSearch(127, Command.Search.DepthTime.Time * 1000 / Command.Search.TimeMode.MovesToGo, Command.Search.DepthTime.Time * 1000);
							//IMaxTime =  UcciComm.nTime * 1000 / UcciComm.nMovesToGo ;
							ext_IMaxTime =  UcciComm.nTime / (UcciComm.nMovesToGo + 4) ;
							//EEngine.IMaxTime =  Command.Search.DepthTime.Time / Command.Search.TimeMode.MovesToGo ;
	  					printf("Info MaxTime:%d\n",ext_IMaxTime);
	//2892w			printf("Info MaxTime:%d UCCITime:%d UCCIMovesToGo:%d \n", ext_IMaxTime, UcciComm.nTime, UcciComm.nMovesToGo);
							fflush(stdout);
							EEngine.ISearch();
							break;

						// 加时制： 分配时间 = 每步增加的时间 + 剩余时间 / 20 (即假设棋局会在20步内结束)    eyc=22, 3dc072=22, 3dc092=25
						case UCCI_GO_TIME_INCREMENT:
							// use increment==0 && 1min for uccileag testing as 40 moves in x minutes
							/*
							if (UcciComm.nIncrement==0 && UcciComm.nTime <=60000 && UcciComm.nTime >=200)
							{
                if (p_movenum <=40)
                  IMaxTime =  UcciComm.nTime / (40 - p_movenum + 4);
                else
                  IMaxTime = UcciComm.nTime / 20; //16;
              }
              else
              */
							// 对于加时制，假设棋局会在20回合内结束，算出平均每一步的适当时限，最大时限是剩余时间的一半
          //Search.nProperTimer = UcciComm.nTime / 20 + UcciComm.nIncrement;
          //Search.nMaxTimer = UcciComm.nTime / 2;

							//EEngine.IMaxTime =  (Command.Search.DepthTime.Time + Command.Search.TimeMode.Increment * 24) * CLOCKS_PER_SEC / 24;
							//IMaxTime =  ((UcciComm.nTime / 22) + UcciComm.nIncrement) * CLOCKS_PER_SEC / 1000;	// /22  / 1000
//bef 2892t							ext_IMaxTime =  ((UcciComm.nTime * 3 / 64) + UcciComm.nIncrement);  // /25 /28 /25 /22
//2892t 4-0							ext_IMaxTime =  max(20, ((UcciComm.nTime / 30) + UcciComm.nIncrement) );  // /25 /28 /25 /22
//bef exchess						ext_IMaxTime =  max(8, (UcciComm.nTime/25) + UcciComm.nIncrement );  //*3/64=/21 /2 // /25 /28 /25 /22        
                        // assume remain=25 moves   
                       
      // 2892v vs XQMS  //if (UcciComm.nTime < 1000) 
                        //	ext_IMaxTime = (200 + UcciComm.nIncrement) * 3/8;
                        //else	                      	                        
                        //ext_IMaxTime =  ((UcciComm.nTime)/(25 + min(35,p_movenum*4/3)) + UcciComm.nIncrement) * 4/3; //(4/3); // * 13/16;  // 5/8;
                        
                        // if(move_no <= 20) moves_left = 45 - move_no; // from talkchess
                        // else moves_left = 25;
                        // search_time = p_time / moves_left + inc; 
                        // 2892z - 37,23 moves // 40,20  60*0.618,0.382=37,23    
                        
                        //if (UcciComm.nTime <= 1500) 
                        //	ext_IMaxTime = (200 + UcciComm.nIncrement) * 3/8;
                        //else	
                        
                        if (p_movenum > 55)
                        	 ext_IMaxTime = (UcciComm.nTime / 30) + UcciComm.nIncrement ;	//egtime=30 32
                        else
                        if (p_movenum <= 30) //23 15
                           ext_IMaxTime =  (UcciComm.nTime/(60 - p_movenum)) + UcciComm.nIncrement ;	
											  else
											  if (ext_p_endgame ==1)	
											  	 ext_IMaxTime = (UcciComm.nTime / (28)) + UcciComm.nIncrement;
											  //	 ext_IMaxTime = (max(75,(UcciComm.nTime-200)) / (60 - p_movenum/2)) + UcciComm.nIncrement;
											  else	
							             ext_IMaxTime = (UcciComm.nTime / (27)) + UcciComm.nIncrement;
							             
						   					  	//ext_IMaxTime =  (UcciComm.nTime/30) + UcciComm.nIncrement;
							printf("Info MaxTime:%d\n", ext_IMaxTime);		
							//printf("Info MaxTime:%d, movenum=%d, endgame=%d\n", ext_IMaxTime, p_movenum, ext_p_endgame);		
							//printf("Info remainMV=%d, remainEG=%d\n", ext_remainmv, ext_remaineg);
							//printf("Info MaxTime:%d, Endgame=%d, nTime=%d, nOpptime=%d, movenum=%d\n",ext_IMaxTime, ext_p_endgame, UcciComm.nTime, UcciComm.nOpptime, p_movenum);
							//printf("Info MaxTime:%d assume moves remain: %d\n",ext_IMaxTime, int(UcciComm.nTime / ext_IMaxTime) );
							fflush(stdout);
							EEngine.ISearch();
							break;

						default:
							break;
					}
					break;
			}

			// for batch quit, p_feedback_move == -1 passed by Engine.cpp
			if (p_feedback_move == -1)
				break;

		} while (IdleComm != UCCI_COMM_QUIT);

		DelHash();
		printf("bye\n");
		fflush(stdout);
	}

//pool.joinAll();  //threadpool11

	return 0;
}

