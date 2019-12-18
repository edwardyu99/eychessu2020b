/**
 * @(#) Eybook 1.0
 * @author  Edward Yu
 * @version 1.0    2006/02/19
 *
 *
 * This Chinese Chess program use the History Heuristic for move ordering.
 * It also reads EYBOOK.DAT for opening book, and POINTTABLE.DAT for board evaluation.
 * You may use this software, and it's source, as you wish.
 *
 * Edward Yu
 * eykm@yahoo.com


 *
 */
#include <fstream>
#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
#include <string>
//#include <time.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
//#include "StringTokenizer.h"
#include "Tokenizer.h"

extern char g_bookfile[120];

using namespace std;

//const byte[] iboard={3,2,1,19,0,27,29,4,31,33,35,8,25,7,6,5,
//                          84,83,82,64,81,54,56,58,85,60,62,89,70,88,87,86};
//B,E,H,C,R,P,P,K,P,P,P,R,C,H,E,B
//B,E,H,C,R,P,P,P,K,P,P,R,C,H,E,B;



const char CB_EMPTY = 90;  //-1; //90;
//	  static int    ext_ply;

const int    SIZE_X  =       9;
const int    SIZE_Y  =       10;
const int BOARD_SIZE =   SIZE_X*SIZE_Y;

/*
const int EMPTY      =   7;
const int BLACK       =   0;
const int WHITE      =   1;
const int PAWN       =   0;
const int BISHOP =       1;
const int ELEPHAN    =   2;
const int KNIGHT =       3;
const int CANNON =       4;
const int ROOK       =   5;
const int KING       =   6;
*/
const int EMPTY       =  0;
const int BLACK       =  0;
const int WHITE       =  1;
const int PAWN        =  1;
const int BISHOP      =  2;
const int ELEPHAN     =  3;
const int KNIGHT      =  4;
const int CANNON      =  5;
const int ROOK        =  6;
const int KING        =  7;
const int BPAWN       =   2;
const int BBISHOP     =   4;
const int BELEPHAN    =   6;
const int BKNIGHT     =   8;
const int BCANNON     =   10;
const int BROOK       =   12;
const int BKING       =   14;
const int WPAWN       =   3;
const int WBISHOP     =   5;
const int WELEPHAN    =   7;
const int WKNIGHT     =   9;
const int WCANNON     =   11;
const int WROOK       =   13;
const int WKING       =   15;
const int    MAXVAL =      20000;
const int    BIGVAL =      10000;
const int    NORMAL  =       0;
const int    SELECT  =       1;
const int    MOVETIME =  60000;   // 120sec=2mins   // original 182*6; = 60sec

int bbcount = 0;  // book board count
int bookfromdest;
int bestscore=0;
// int  side, xside, computerside;
// int bply, mply, follow_pv;
/* move newmove; */
int newmovefrom, newmovedest;
char cboard[32];
char cboard_cstr[33]; // last char will be '\0' end of string



bool stringToL(const std::string &str, long  &val)
{
	bool isOK = false;
	const char *nptr = str.c_str();
	char *endptr = NULL;
	errno = 0;
	val = strtol(nptr, &endptr, 10);
	//error ocur
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
		|| (errno != 0 && val == 0))
	{
 
	}
	//no digit find
	else if (endptr == nptr)
	{
 
	}
	else if (*endptr != '\0')
	{
		// printf("Further characters after number: %s\n", endptr);
	}
	else
	{
		isOK = true;
	}
 
	return isOK;
}

bool stringToI32(const std::string& str, int32_t& val)
{
	long temp_val = 0;
	bool isOK = stringToL(str, temp_val);
	val = temp_val;
	return isOK && (temp_val >= -0x7fffffff && temp_val <= 0x7fffffff/*32bit整形的有效范围*/);
}

//20190912 int readBook(std::string cboardstr)
int readBook(char cboard_cstr[])
{
//    printf("*** call readBook() ...\n");
//    printf("cboard_cstr 3 = %s len=%d\n",cboard_cstr, strlen(cboard_cstr));
    
    string cboardstr(cboard_cstr);
//    printf("cboardstr 3 = %s length=%d\n",cboardstr.c_str(), cboardstr.length());
    
    ifstream bookdatf;
    //char buffer[128 * 1024];
    //bookdatf.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
    //bookdatf.open("EYBOOK.DAT");
    bookdatf.open(g_bookfile, ios::in);  //bookfile name with path get from Eychessu.cpp
    if (bookdatf.fail()) 
    { 
    	  //printf("*** readBook %s error !!!\n", g_bookfile);
    	  bookdatf.close();
    	  bookdatf.clear();
    		return 0;  // no open book, return nullmove
    }
    //debug x64
//#ifdef _WIN64    
//    bookdatf.close();
//    printf("*** readBook %s open and close OK\n", g_bookfile);
//    return 0;  // no open book, return nullmove
//#endif    
    int j,ct,intval[30],intcountsum,intrand; //intfromdest, intcount,
    int retfromdest=0;  //not found
    //long longval;
    //static char linechar[256];
    char * pEnd = NULL;
    //string linestr;
    string bookstr;
    string intvalstr;
	string linestr;
	int linelength;
//    while (!bookdatf.eof())
    while(getline(bookdatf, linestr))
    {
        //getline(bookdatf, line, '\n')
        //bookdatf.getline(linechar,256); //where 256 is the size of the array
        //string linestr(linechar);
        
       
        //getline(bookdatf, linestr); //20190914
        //if (!(getline(bookdatf, linestr)) )
        //{
        //	if ( bookdatf.eof())
        //		break;
        //	 bookdatf.close();
        //	 bookdatf.clear();
        //   printf("*** readBook %s getline error\n", g_bookfile);
        //   return 0;  // no open book, return nullmove
        //}	
        linelength=linestr.length();
//#ifdef _WIN64   
//        printf("*** readBook getline linestr=%s length=%d\n", linestr.c_str(), linelength);
//        fflush(stdout);
        //bookdatf.close();   //debug x64
        //bookdatf.clear();
        //return 0;           //debug x64
//#endif

        if (linestr == "") 
        {
        	bookdatf.close();
        	bookdatf.clear();
           printf("*** readBook %s getline empty linestr \n", g_bookfile);
           fflush(stdout);
           return retfromdest;  // may be empty linestr is eof, return retfromdest so far
        }	
        	//continue;
        	
        bookstr = linestr.substr(0,32);
//#ifdef _WIN64        
//        printf("*** readBook bookstr=%s length=%d\n", bookstr.c_str(), bookstr.length());
//        fflush(stdout);
//#endif        
        
        if (bookstr.length() != cboardstr.length())
        	{
			printf("*** readBook bookstr=%s length=%d not eq cboardstr length=32\n", bookstr.c_str(), bookstr.length());
        		fflush(stdout);
        		continue;
        	}
        
        if (bookstr < cboardstr)                 	
        	continue;
        	
        if (bookstr > cboardstr)
        {	//fprintf(stderr,"bookstr > %s\n",bookstr.c_str());
        	  bookdatf.close();
        	  bookdatf.clear();
//           printf("*** readBook bookstr=%s notfound\n", bookstr.c_str());
            return 0;
        }
        // now bookstr == cboardstr
/*        	
        StringTokenizer strtoken = StringTokenizer(linestr," ");
        ct = strtoken.countTokens();
        bookstr = strtoken.nextToken();
        if (bookstr < cboardstr) continue;
        if (bookstr > cboardstr)
        {	//fprintf(stderr,"bookstr > %s\n",bookstr.c_str());
            return 0;
        }
*/       

        string bookstr33 = linestr.substr(33,linelength-33-1); //no leading trailing space
//        printf("*** bookstr33=%s length=%d\n", bookstr33.c_str(), bookstr33.length());

//20190915        StringTokenizer strtoken = StringTokenizer(bookstr33, " ");
//20190915        ct = strtoken.countTokens();
        // instanciate Tokenizer class
//    Tokenizer str("This is a very long string.");
//    string token;
    // Tokenizer::next() returns a next available token from source string
    // If it reaches EOS, it returns zero-length string, "".
//    while((token = str.next()) != "")
//    {
//        cout << token << endl;
//    }
        Tokenizer strtoken(bookstr33, " ");
        
        //printf("!!! after Tokenizer --- ");
        //printf("bookstr33=%s length=%d\n",bookstr33.c_str(), bookstr33.length());
        intcountsum=0;
        
        //for (int j = 0; j < ct; j++)
        ct = 0;
        j = 0;
        while((intvalstr = strtoken.next()) != "")
        {
        	  ct++;
            //20190915 intvalstr=strtoken.nextToken();
            //20190914 intval[j] = strtol (intvalstr.c_str(),&pEnd,10);  // radix 10
            if (!stringToI32(intvalstr, intval[j]))
            {
            		printf("*** error intvalstr=%s\n", intvalstr.c_str());
            		fflush(stdout);
					      bookdatf.close();
					      bookdatf.clear();
					      return 0;
            }
            //intval[j] = longval;
//20190915      printf("*** intval[%d]=%10d ", j,intval[j]);

//            intfromdest=intval &16383;
            //intcount=(intval[j]>>14) + 1;
            intcountsum += (intval[j]>>14) + 1; //intcount;
//            newmovefrom = intfromdest >>7; // / 256;
//            newmovedest = intfromdest &127;
            //System.out.print(" " + newmovefrom + "-" + newmovedest + " " +intcount+",");
            //fprintf(stderr," %d-%d %d \n", newmovefrom, newmovedest, intcount);
            
            j++;
        }
//20190915        printf(" ct=%d\n", ct);
        fflush(stdout);
        //intrand=(int)(Math.random() * intcountsum);
        //intrand=((double)rand() / (double)32767 ) * intcountsum;
        intrand=(int)(intcountsum * (rand() / (RAND_MAX + 1.0)));


        //intrand=rand() * intcountsum / 32767;

        //fprintf(stderr,"rand=%d\n",intrand);

        intcountsum=0;

        //t = new StringTokenizer(line);
        //ct = t.countTokens();
        //t.nextToken();
/*
        StringTokenizer strtoken2 = StringTokenizer(linestr," ");
        ct = strtoken2.countTokens();
        bookstr = strtoken2.nextToken();
*/
//        StringTokenizer strtoken2 = StringTokenizer(bookstr, " ");
        //for (j=1; j<ct; j++)
        for (int j=0; j<ct; j++)
        {	//intval=Integer.valueOf(t.nextToken());
/*        	
            intvalstr=strtoken2.nextToken();
            longval = strtol (intvalstr.c_str(),&pEnd,10);  // radix 10
            intval = longval;
            //fprintf(stderr,"intval= %d\n", intval);
*/
            //intcount=(intval[j]>>14) + 1;
            intcountsum += (intval[j]>>14) + 1; //intcount;
            if (intcountsum>intrand)
            {	//intfromdest=intval &16383;
                //newmovefrom = intfromdest >>7; // / 256;
                //newmovedest = intfromdest &127;
                retfromdest=intval[j] &16383;          
//20190915                printf("*** intval[%d]=%d chosen\n", j, intval[j]);      
                break;
            }

        }

        break;

    } // while
    bookdatf.close();
    bookdatf.clear();


    return retfromdest;
}


/** Method searchBook

    @return : return (newmovefrom<<7)+newmovedest if found in bookboard, else found=0
*/
int searchBook(int compMoveFirst)
{   //srand((unsigned)time(0)) ;
    //srand(GetTickCount());  //20190915
    srand((unsigned int)time(NULL));
    //int j;
    int intfromdest = 0;
    int found = 0;

//	printf("*** enter searchBook()...");

//  if computer move first, reverse cboard before searching book
    if (compMoveFirst == 1)
    {
        for (int j=0; j<16; j++)
        {
            char temp1=cboard[j];
            char temp2=cboard[31-j];
            if (temp2 == CB_EMPTY) cboard[j]=CB_EMPTY;
            else cboard[j]=(char)(BOARD_SIZE - 1 - temp2);
            if (temp1 == CB_EMPTY) cboard[31-j]=CB_EMPTY;
            else cboard[31-j]=(char)(BOARD_SIZE - 1 - temp1);
        }
    }

    for (int j=0; j<32; j++)
    {
        cboard_cstr[j] = cboard[j] + 33;
    }
    cboard_cstr[32] = '\0';
    string cboardstr(cboard_cstr);
    //printf("cboard_cstr 1 = %s\n",cboard_cstr);
    //20190912 intfromdest=readBook(cboardstr);
    intfromdest=readBook(cboard_cstr);
    //bookstring = Integer.toHexString(bookstring.hashCode());

    //bookint = new Integer(bookstring.hashCode());
    if (intfromdest!=0)
    {
        newmovefrom = intfromdest >>7; // / 256;
        newmovedest = intfromdest &127;

        if (compMoveFirst==1)
        {
            newmovefrom = BOARD_SIZE - 1 - newmovefrom;
            newmovedest = BOARD_SIZE - 1 - newmovedest;
        }
        //found = true;
        found = (newmovefrom<<7) + newmovedest;
    }


// check for horizontal symmetry
    if (found==0)
    {
        for (int j=0; j<7; j++)
        {
            char m=cboard_cstr[j];
            cboard_cstr[j]=cboard_cstr[15-j];
            cboard_cstr[15-j]=m;
        }
        for (int j=16; j<23; j++)
        {
            char m=cboard_cstr[j];
            cboard_cstr[j]=cboard_cstr[47-j];
            cboard_cstr[47-j]=m;
        }
        for (int j=0; j<32; j++)
        {
            char m=cboard_cstr[j];
            if (m != CB_EMPTY) cboard_cstr[j]=(char)(m+SIZE_X-1-2*(m%SIZE_X));
        }
//        case 0: return m;
//        case 1: return m+SIZE_X-1-2*(m%SIZE_X);
//        case 2: return m + (SIZE_Y-1 - 2*(m/SIZE_X))*SIZE_X;
//        case 3: return BOARD_SIZE-1-m;

        //bookstring = new String(cboard);
//          Integer oint = (Integer)(bookhash.get(bookstring));
        //	bookint = new Integer(bookstring.hashCode());
        //intfromdest=readBook(bookstring);

        string cboardstr2(cboard_cstr);
        //printf("cboard_cstr 2 = %s\n",cboard_cstr);
        //20190912 intfromdest=readBook(cboardstr2);
        intfromdest=readBook(cboard_cstr);
        if (intfromdest!=0)
        {
            newmovefrom = intfromdest >>7; // / 256;
            newmovedest = intfromdest &127;
            newmovefrom = newmovefrom+SIZE_X-1-2*(newmovefrom%SIZE_X);
            newmovedest = newmovedest+SIZE_X-1-2*(newmovedest%SIZE_X);

            if (compMoveFirst==1)
            {
                newmovefrom = BOARD_SIZE - 1 - newmovefrom;
                newmovedest = BOARD_SIZE - 1 - newmovedest;
            }
            //found = true;
            found = (newmovefrom<<7) + newmovedest;
        }
    }



//  System.out.println(newmovefrom + "," + newmovedest);


    //System.arraycopy(saveboard,0,cboard,0,32);
    //System.out.print("bookmove found: "+(char)(newmovefrom%SIZE_X + 65) + (SIZE_X - newmovefrom / SIZE_X) + "-" + (char)(newmovedest % SIZE_X + 65) + (SIZE_X - newmovedest / SIZE_X) + " ");

    return found;
}





//20190912 int bookmain(std::string strcboardline)
int bookmain(char cboardline[])
{
    int bookmove = 0;
    // char cboardline[67];
    // copying the contents of the 
    // string to char array 
    //20190912 strcpy(cboardline, strcboardline.c_str()); 

    for (int i=0; i<64; i=i+2)
    {
        cboard[i>>1] = ((cboardline[i] - '0') * 10) + (cboardline[i+1] - '0');
    }
    int compMoveFirst =  (cboardline[65] - '0');
    // debug
//    printf("*** bookmain - cboard=");
//    for (int j=0; j<32; j++)
//    {printf(" %d",cboard[j]);
//    }
//    printf("\n");

//    printf("compMoveFirst=%d\n",compMoveFirst);
    //
    bookmove = searchBook(compMoveFirst);
    return bookmove;

}

