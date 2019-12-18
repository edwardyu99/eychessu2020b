int Engine::search<PV>(Board &board, int alpha, int beta, int depth, int null_allow)
{
    int val,incheck,newdepth, best, nHashFlag, extension, evalscore;
    int moveCount, captureCount, quietCount;
    bool improving, givesCheck, moveCountPruning, skipQuiets; //captureOrPromotion; 
    MoveStruct mvBest;
    MoveTabStruct movesSearched[128];
    HistStruct *hisvptr;

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
            printf("info search<PV> stopped by GUI pv\n");
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


//    printf("info Search timout %d board.ply %d board.IMaxTime %d\n", (int)(GetTime() - board.m_startime), board.ply, board.IMaxTime);

            	    board.m_timeout= 1; //stop
            	    //printBestmove(board.m_bestmove);
                    return UNKNOWN_VALUE;
                    //longjmp(setjmpbuf,1);
            }
            if (t_remain < 4000)    				
    					PollNodes = 1023;
          
        }
//} //1006


// 2. 和棋裁剪；
    //if (pos.IsDraw())
    if ((board.bitpiece & 0xfff00ffc) == 0) return Evalscore(board);
    incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;			
    if (board.ply>= MAXPLY)  
       //return (!incheck ? Evalscore(board) : 0);   //1113 eg rook bug
         return Evalscore(board);
// mate-distance pruning
int old_alpha = alpha;
        alpha = std::max(-INF + board.ply, alpha);
        beta = std::min(INF - 1 - board.ply, beta);
        if (alpha >= beta)
            return (alpha);

//    incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
//draw pruning - not for PV   
        // if draw score and no rook & 1N/C vs 1N/C 
//      if (ext_p_endgame==1 && (!incheck) && (abs(alpha) <= ext_drawval))
//    	if ( bitCountMSB((board.bitpiece & 0xf0000000)) ==0 && bitCountMSB((board.bitpiece & 0x0ff00000)) <=2)
//    	{ printf("*** PV:draw alpha=%d, beta=%d, bitpiece=%x\n", alpha, beta, board.bitpiece);
//         return 0;
//      }
    // 3. 重复裁剪；
    val=board.checkloop(1);  //1 bef927
    if (val)
    {
     		return val;
    }
    
    board.currentMove[board.ply].move = board.excludedMove[board.ply+1] = 0; 
    int prevSqdest = board.currentMove[board.ply - 1].dest;  //to_sq((ss-1)->currentMove);  
	moveCount = captureCount = quietCount = board.moveCount[board.ply] = 0;  //1024
	
    
    // 5. 置换裁剪；
    HashStruct *hsho;
    int ttMove = 0;
    int ttValue=VALUE_NONE;
    int excludedMove;
    // Step 4. Transposition table lookup. We don't want the score of a partial
    // search to overwrite a previous full search TT value, so we use a different
    // position key in case of an excluded move.
    excludedMove = board.excludedMove[board.ply];
    /*
    int nBetaDepth = 0;
    int nBetaVal = 0;
//    int nSingular = 0;
      int nAlphaDepth = 0;
    */
    // Step 4. Transposition table lookup sf 1.7.1
    // At PV nodes, we don't use the TT for pruning, but only for move ordering.
    // This is to avoid problems in the following areas:
    //
    // * Repetition draw detection
    // * Fifty move rule detection
    // * Searching for a mate
    // * Printing of full PV line
    //if (node_type==NodePV)	//strelka
    //{
      	//ttMove = ProbeMove(nBetaDepth, nBetaVal, nAlphaDepth); //, nSingular);
      	hsho = ProbeMove(board);
		//}
		if (hsho)
		{
			ttMove = get_hmv(hsho->hmvBest);
			ttValue = value_from_tt(hsho->hVal, board.ply); 
		}

// Step 6. Static evaluation of the position
    mvBest.move = 0;
    if (incheck)
    {
        board.staticEval[board.ply] = evalscore = VALUE_NONE;
        improving = false;
    	  goto moves_loopPV;  // Skip early pruning when in check
    }	
    else  
    {
    	board.staticEval[board.ply] = evalscore = Evalscore(board);
    }	

/* sf 10
// Step 7. Razoring (~2 Elo)
    if (   depth < 2 * ONE_PLY
        && eval <= alpha - RazorMargin)
        return qsearch<NT>(pos, ss, alpha, beta);
*/  
//   evalscore = Evalscore(board);
   if (depth < 2 
//   	&& !incheck  //bypass by goto 
   	   && Evalscore(board) <= alpha - RazorMargin)
   	   return quiesCheckPV(board, alpha, beta, 0);
// Step 6. Razoring (is omitted in PV nodes)
    // Step 7. Static null move pruning (is omitted in PV nodes)
    // Step 8. Null move search with verification search (is omitted in PV nodes)
		// nullmove pruning (not for search<PV>)
		
    //mvBest.move = 0; //move up bef goto
// Step 8. Futility pruning: child node (~30 Elo)    
 improving = board.ply < 2
              || board.staticEval[board.ply] >= board.staticEval[board.ply -2]
              || board.staticEval[board.ply -2] == VALUE_NONE;
    if (   depth < 7 
    	  && evalscore - futility_margin(depth, improving) >= beta
        && evalscore < VALUE_KNOWN_WIN) // Do not return unproven wins
        return evalscore;


/*    
    // Step 11. Internal iterative deepening (~2 Elo)   //sf10
    if (    depth >= 8 * ONE_PLY
        && !ttMove)
    {
        search<NT>(pos, ss, alpha, beta, depth - 7 * ONE_PLY, cutNode);

        tte = TT.probe(posKey, ttHit);
        ttValue = ttHit ? value_from_tt(tte->value(), ss->ply) : VALUE_NONE;
        ttMove = ttHit ? tte->move() : MOVE_NONE;
    }

*/


    if ( depth >= 8 //5  //IIDDepth  5(sf 1.7.1) //1107 sf10 5-->8
    	&& ttMove == 0
         //&& node_type==NodePV
         //&& null_allow
		 )
    {        
        val = search<PV>(board, alpha, beta, depth-7, null_allow); //, NULL_NO);   //1107 sf10 2-->7     
        hsho = ProbeMove(board);
        //ttValue = ttHit ? value_from_tt(tte->value(), board.ply) : VALUE_NONE;
        if (hsho) 
        {	ttMove = get_hmv(hsho->hmvBest);
        	ttValue = value_from_tt(hsho->hVal, board.ply);
        }	
        else
        { ttMove = 0;
        	ttValue = VALUE_NONE;
        }	
    }

moves_loopPV: // When in check, search starts from here
    MoveTabStruct movetab[64], ncapmovetab[111]; //capmovetab[111],
    MoveTabStruct badcapmovetab[32];  //1013 - separate badcap
    MoveStruct countermove = counterMoves[board.thd_id][PIECE_IDX(board.piece[prevSqdest])][prevSqdest];  //1017
    int badcapsize=0; 
    nHashFlag = HASH_ALPHA;
    int gen_count=0;
		int noncap_gen_count=0;
//    int CheckExt=0; //1003
//    int CaptureExt;
//1003 - chg checkext to givecheck ext
/*
    if (board.ply < MAXEXT) //60) //MAXPLY - 4) //60 //30) //40)
    {
        if (incheck
        	)
        {
            CheckExt=1;
        }
    }
*/
    MoveTabStruct tempmove;
    int size=0;
    long ncapsize=0;
    int capture=0;
	//int kingidx,
	//int opt_value;
	
    skipQuiets = false;  	
    //for (int phase=6; phase--;)
    //1013 sep badcap for (int phase=5; phase--;)
enum pvphase{HASHPV,CAP,KILLER,CM,NOCAP,BADCAP};
    for (int phase=0; phase<=BADCAP; phase++)
    {
        //if (phase==0 && ncapsize==0)
        //	break; // break for phase

        int SingleExt=0;	//singleext in PV
        switch (phase)
        {
        	case HASHPV:     //0:  //hash pv
        		size=1;
        		break;
        		
//        	case MATEKILLER: //1:  //mate-killer
//        		size=1;
//        		break;
        		
	        case CAP:        //2: //capture
            if (incheck)
            {
            	size=board.GenChkEvasCap(&movetab[0], incheck);

							//if (node_type==NodePV && board.ply < MAXEXT) //MAXPLY - 4) //50)
					    if (board.ply < MAXEXT)
							{
								if (size <= 1)
									  SingleExt=1;
          			//single-reply extension
                else //if (size > 1 && board.ply < MAXEXT) //MAXPLY - 4) //50)
    						{

        					int gcount=0;
        					for (int j=0; j<size; j++)
        					{
        						//if (makemovechk(movetab[j].table) ==0)
            				if (board.makemovenoeval(movetab[j].table, 1) >=0) //, 1) >=0)
            				{
                			board.unmakemovenoeval();
                			gcount++;
                			if (gcount > 1)
                    		break;
            				}
        					}
        					if (gcount <= 1)
            				SingleExt=1;
       					}
       					//if (capsize <=1 && board.ply < 50)
       					//	Extended=2;
       				} // end if board.ply

            }
            else
            {
                size=board.GenCap(&movetab[0], &ncapmovetab[0], ncapsize);
                //size=(board.m_side ? board.GenCap<1>(&movetab[0], &ncapmovetab[0], ncapsize)
                //                   : board.GenCap<0>(&movetab[0], &ncapmovetab[0], ncapsize));
                //size=board.GenCapQS(&movetab[0]);
            }
             //sort capture
             //Quicksort(0, size-1, movetab);
            //no need to sort cap, use GetNextMove(i for short table
            //std::sort(movetab, movetab+size);
            break;
            
          case KILLER:   //3: //killer
          	if (incheck)  //evasion
          		size=0;
            else size=2; //NUM_KILLERS; //3; //4; //2;
            break;
          case CM:   //Countermove: //killer
          	if (incheck)  //evasion
          		size=0;
            else size=1;  
            break;
              
          case NOCAP:    //4:	// nocap
            if (incheck)
            {
              //size=board.GenChkEvasNCap(&movetab[0], incheck);
              size=board.GenChkEvasNCap(&ncapmovetab[0], incheck);
            }
            else
            {
    			    for (int j=0; j<ncapsize; j++)
              {
                MoveTabStruct *tabptr;
                tabptr = &ncapmovetab[j];
                //MoveTabStruct tabptr = ncapmovetab[j];
                // cal tabval for hors/pawn/bis/ele/king noncap
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
                	  //tabptr->tabval += his_table(piecefromidx, tabptr->dest);
                	  tabptr->tabval += m_his_table[board.thd_id][piecefromidx][tabptr->dest].HistVal;
                }
              }
    	          //size=board.GenNonCap(&movetab[0]);
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
//        	 if (badcapsize>5) printf("*** badcapsize=%d\n", badcapsize); 
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
//           CaptureExt=0;

          switch (phase)
          {
          	case HASHPV: //0:	//hashmove
            {
                tempmove.table.move=ttMove;
                if (tempmove.table.move==0
                	  || board.LegalKiller(tempmove.table)==0)
                    continue;
                    //PVhashhit++;
            }
            break;
//            case MATEKILLER: //1:	//matekiller
//            {
//                tempmove.table.move=g_matekiller[board.ply].move;
//                if (tempmove.table.move==0 
//                    || tempmove.table.move==ttMove
//                    || board.LegalKiller(tempmove.table)==0)
//                    continue;
//                    PVmatekillerhit++;
//            }
//            break;
            
            case CAP: //2:	//capture
            {
                tempmove.tabentry = GetNextMove(i, size, movetab);
                if ( tempmove.table.move==ttMove
//                     || tempmove.table.move==g_matekiller[board.ply].move
                     )
                    continue;
//1002                    
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
            
            case KILLER: //3:	//killer
           	{
                tempmove.table.move=g_killer[i][board.ply].move;
                if (tempmove.table.move==0
                        || tempmove.table.move==ttMove
//                        || tempmove.table.move==g_matekiller[board.ply].move
                        || board.LegalKiller(tempmove.table)==0
                   )
                    continue;
                    //PVkillerhit++;
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
               //PVcmhit++;
               //printf("*** PV CM hit: from=%d, dest=%d\n", countermove.from, countermove.dest);
            }
            break;
            
            case NOCAP: //4:	//nocap
            {
            	  //tempmove.tabentry = movetab[i].tabentry; //after std::sort noncap
            	  tempmove.tabentry = ncapmovetab[i].tabentry; //after std::sort noncap
            	  if (incheck)   //1019 evasion
            	  {  if (tempmove.table.move==ttMove)
            	  	    continue;
            	  }	
            	  else
            	  { 	
            	  if ( (skipQuiets 
            	  	//1013 sep badcap && board.piece[tempmove.dest]==SQ_EMPTY
//1013            	  	 && !board.gives_check(tempmove.from, tempmove.dest)
            	  	 )    //skipquiets not for badcapture 
        				|| tempmove.table.move==ttMove
//                || tempmove.table.move==g_matekiller[board.ply].move
                || tempmove.table.move==g_killer[0][board.ply].move
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
                || tempmove.table.move==g_matekiller[board.ply].move
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

            int piecefrom = board.piece[tempmove.from];
            //hisvptr = &(m_his_table[PIECE_IDX(board.piece[tempmove.from])][tempmove.dest]);
            hisvptr = &(m_his_table[board.thd_id][PIECE_IDX(piecefrom)][tempmove.dest]);      //1017

//1003 no singlereply extension      newdepth = depth - 1 + SingleExt;   //1003 + CheckExt 
//1003
            extension = 0;
            givesCheck = board.gives_check(tempmove.from, tempmove.dest);     
      //captureOrPromotion = pos.capture_or_promotion(move);
      //movedPiece = pos.moved_piece(move);
      //givesCheck = gives_check(pos, move);
//    ss->moveCount = ++moveCount;
      board.moveCount[board.ply] = ++moveCount;  //1024
      //1024 moveCount++;
      moveCountPruning =   depth < 16
                        && moveCount >= FutilityMoveCounts[improving][depth];  
//1109 implement sf10 Singular extension search 
      if (    depth >= 8 
          &&  tempmove.table.move == ttMove
//          && !rootNode   //const bool rootNode = PvNode && ss->ply == 0; //1109
          && board.ply != 0 //1109
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
          val = search<NonPV>(board, rBeta - 1, rBeta, depth/2, NULL_NO);
          board.excludedMove[board.ply] = 0;  //MOVE_NONE;
          if (val < rBeta)
              extension = 1;
      }
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
      {
          Value rBeta = std::max(ttValue - 2 * depth / ONE_PLY, -VALUE_MATE);
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
/* sf 2.0
// Singular extension search. If all moves but one fail low on a search of (alpha-s, beta-s),
      // and just one fails high on (alpha, beta), then that move is singular and should be extended.
      // To verify this we do a reduced search on all the other moves but the ttMove, if result is
      // lower then ttValue minus a margin then we extend ttMove.
      if (   singularExtensionNode
          && move == tte->move()
          && ext < ONE_PLY)
      {
          Value ttValue = value_from_tt(tte->value(), board.ply);

          if (abs(ttValue) < VALUE_KNOWN_WIN)
          {
              Value b = ttValue - SingularExtensionMargin;
              ss->excludedMove = move;
              ss->skipNullMove = true;
              Value v = search<NonPV>(pos, ss, b - 1, b, depth / 2, board.ply);
              ss->skipNullMove = false;
              ss->excludedMove = MOVE_NONE;
              ss->bestMove = MOVE_NONE;
              if (v < b)
                  ext = ONE_PLY;
          }
      }
*/


//1003 

     if (extension ==0)
     {	
     	 if (    givesCheck // Check extension (~2 Elo)
               &&  board.see_ge(tempmove.from, tempmove.dest, 0))    //pos.see_ge(move))
          extension = 1;
     }

// Calculate new depth for this move
      newdepth = depth - 1 + extension;
/* //1009
// Step 14. Pruning at shallow depth (~170 Elo)
      if (  !rootNode
          && pos.non_pawn_material(us)
          && bestValue > VALUE_MATED_IN_MAX_PLY)
      {
          if (   !captureOrPromotion
              && !givesCheck
              && (!pos.advanced_pawn_push(move) || pos.non_pawn_material() >= Value(5000)))
          {
              // Move count based pruning (~30 Elo)
              if (moveCountPruning)
              {
                  skipQuiets = true;
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
*/ //1009

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
//1024              int lmrDepth = std::max(newdepth - reduction<PV>(improving, depth, moveCount), 0) ;  //1023
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
                  { lmrpvhit1++;
                continue; }

              // Prune moves with negative SEE (~10 Elo)
//1023              if (!pos.see_ge(move, Value(-29 * lmrDepth * lmrDepth)))
              if (!board.see_ge(tempmove.from, tempmove.dest, (-21 * lmrDepth * lmrDepth)))	//1023
                 { lmrpvhit2++;
                continue; }
*/                
//1023                  
          }
          else if (   !extension // (~20 Elo)
                   && !board.see_ge(tempmove.from, tempmove.dest, -1357 * (depth )))  //1109 PawnValueEg
                 continue;
        }



// History pruning and Futility (not for search<PV>)
//if (phase<=1)	// noncap + badcap
//{
//}	//end if phase<=1

        // Update the current move (this must be done after singular extension search)
        board.currentMove[board.ply].move = tempmove.table.move;
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


// Step 16. Reduced depth search (LMR). If the move fails high it will be
      // re-searched at full depth.
         if (    depth >= 3
          &&  moveCount > 1
          && (capture==0 || moveCountPruning))
         {
           int r = reduction<PV>(improving, depth, moveCount);
          
          // Decrease reduction if opponent's move count is high (~10 Elo)
//1024          if ((ss-1)->moveCount > 15)
//1024              r -= ONE_PLY;
          if (board.moveCount[board.ply-1] > 15)
          	 r -= 1; 
/* //1024 not do
          if (capture==0)   //(!captureOrPromotion)
          {
              // Decrease reduction for exact PV nodes (~0 Elo)
              //1024 if (pvExact)
              //1024    r -= ONE_PLY;

              // Increase reduction if ttMove is a capture (~0 Elo)
              //1024 if (ttCapture)
              //1024   r += ONE_PLY;

              // Increase reduction for cut nodes (~5 Elo)
              if (null_allow)    //(cutNode)
                  reduced += 2;  //r += 2 * ONE_PLY;


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
              
          }
*/          
//1102             if (r > 0)
//1102              {
//1102              	 if (r==1) rhit1++; 
//1102              	 else 
//1102              	 if (r==2) rhit2++;
//1102              	 else rhit3++;		
//1102              }    	

         }
//1024          Depth d = std::max(newDepth - std::max(r, DEPTH_ZERO), ONE_PLY);
             //1024 int d = std::max(newdepth - std::max(reduced, 0), 1);  //1024
              

   			//if ((node_type==NodePV) && (board.ply < MAXEXT))
//1024				if (board.ply < MAXEXT)
//1024        {
//1024                		if (capture && board.p_endgame && board.m_hisrecord[board.m_hisindex-1].endgame == 0)
//1024                			newdepth=depth;	// extend if just enter board.p_endgame
//1024      } // end if node_type

            if (newdepth <= 0)

                val = -quiesCheckPV(board, -beta, -alpha, newdepth); //1007, qcheck_depth);

            else

            {
            //if (node_type !=NodePV || best == -INF)	// first move //toga
            if (best == -INF)	// first move //toga
						{                
                val = -search<PV>(board, -beta, -alpha, newdepth, NULL_NO); //, NULL_YES);
            }
            else	// other moves
            {
if (phase==NOCAP)  //4) 
{
#ifdef HISTPRUN
		if (depth>= HistoryDepth && !incheck && newdepth < depth && gen_count >= 6 //9 //6
			//&& (capture==0)  //diff from stockfish
			&& board.m_hisrecord[board.m_hisindex-1].htab.Chk==0)
		{

            //if (gen_count >= 18 && depth >=23)	//PV LMR
            //{
            //	reduced=2;
            //	newdepth -= 2;
            //}
            //else
        //    if (depth==3 && gen_count < 16)
				//;
        //    else
        if (board.p_endgame==1 //1105 || ((board.EvalSide<1>() < THREAT_MARGIN) //== 0
      	                     //1105 &&  (board.EvalSide<0>() < THREAT_MARGIN)) //== 0
      	//if ((board.m_side ? board.EvalSide<1>() : board.EvalSide<0>()) < THREAT_MARGIN
					 )
						{	reduced=1;
							newdepth--;
						}

					//newdepth -= reduced;
		}
#endif
} // end if phase<=1

            if (newdepth <= 0)
                //val = -quiesCheck<0>(-(alpha+1), -alpha, newdepth, qcheck_depth);
                val = -quiesCheck(board, 1-beta, newdepth); //1007, qcheck_depth);
            else
                
                val = -search<NonPV>(board, -alpha-1, -alpha, newdepth , NULL_YES);
                        // history-pruning re-search
#ifdef HISTPRUN
                if (reduced && val >alpha)  //>=beta)	//was >alpha
        				{
            			newdepth+=reduced;                
                  val = -search<NonPV>(board, -alpha-1, -alpha, newdepth, NULL_YES); // avoid double re-search
        				}
#endif
                // Step extra. pv search (only in PV nodes)
                if ((val > alpha) && (val < beta))
                {
            if (newdepth <= 0)
                val = -quiesCheckPV(board, -beta, -alpha, newdepth); //1007, qcheck_depth);
            else                    
                    val = -search<PV>(board, -beta, -alpha, newdepth, NULL_NO); //, NULL_YES);  //toga
                }
            }
						}


        board.unmakemove();

        if (board.m_timeout) //stop
        	return UNKNOWN_VALUE;



                    // 12. Alpha-Beta边界判定
/* sf191
// Step 17. Check for new best move
      if (value > bestValue)
      {
          bestValue = value;
          if (value > alpha)
          {
              if (PvNode && value < beta) // We want always alpha < beta
                  alpha = value;

              if (value == value_mate_in(board.ply + 1))
                  ss->mateKiller = move;

              ss->bestMove = move;
          }
      }
*/
        //  val >= best >= val >= best2nd >= val
        //1107 if (val <= best)
        //{
        //}
        else
        if (val > best)
        {
            best = val;
            if (val >= beta)
            { if (!excludedMove)  //1109
                RecordHash(HASH_BETA, depth, val, tempmove.table.move, board); //, nSingular);
//1016 sf not update quiet stat here                
/*                
                if (capture==0  //update killer and history for non-captures
                    && phase==NOCAP  //1016  0 //<=1
                    && !incheck
                    && !board.m_timeout  //sf191
                   )
                {
                    if (val >= WIN_VALUE)
                        g_matekiller[board.ply].move = tempmove.table.move;
                    else
                    {
                        if (g_killer[0][board.ply].move != tempmove.table.move)
                        {

                            g_killer[1][board.ply].move = g_killer[0][board.ply].move;
                            g_killer[0][board.ply].move = tempmove.table.move;

                        }
                    }
                    update_HistVal(hisvptr, depth);
                    update_history(hisvptr, depth, movesSearched, noncap_gen_count);
                    
                    
                }
*/
                return val;
            }


            if (val > alpha)
            {
            	  
            	  //if (PvNode && //sf191
            	  if (val < beta) // We want always alpha < beta  //sf191
                	alpha = val;
                mvBest.move = tempmove.table.move;
                //update_pv(sstack, board.ply, mvBest.move); //tempmove.table.move);
                //mvBest = mv;
                nHashFlag = HASH_PV;
            }
        }

        //no longer diff NodeCut and NodeAll
        //if (node_type == NodeCut) node_type = NodeAll;

        } // end for i
    }	// end for phase

//-----------------------------------------------
// Step 20. Check for mate and stalemate
    // All legal moves have been searched and if there are no legal moves, it
    // must be a mate or a stalemate. If we are in a singular extension search then
    // return a fail low score.

//    assert(moveCount || !inCheck || excludedMove || !MoveList<LEGAL>(pos).size());

//1109 sf10    if (!moveCount)
//1109 sf10        bestValue = excludedMove ? alpha
//1109 sf10                   :     inCheck ? mated_in(ss->ply) : VALUE_DRAW;
                   	
    if (gen_count==0)
    {	
//1109        return board.ply-INF;	// No moves in this pos, lost by checkmate or stalemate
       best = excludedMove ? alpha
                   :  -VALUE_MATE + board.ply;
//1113 eg rook bug                    :     incheck ? -VALUE_MATE + board.ply : 0;
    }
    // 13. 更新置换表、历史表和杀手着法表
//cut:
    if (board.m_timeout) //sf191
    	return best;

    if (mvBest.move)
    { if (!excludedMove)  //1109
        RecordHash(nHashFlag, depth, best, mvBest.move, board); //, 0);

        //if (capture==0 && node_type==NodePV && !incheck)
        if (capture==0) //1016 && !incheck)
        {
        	//1024 update_quiet_stats()  //killers, hist, countermove
//          if (best >= WIN_VALUE)
//                g_matekiller[board.ply].move = mvBest.move;
//          else
          {  	
			      if (g_killer[0][board.ply].move != mvBest.move)
		      	{
				       g_killer[1][board.ply].move = g_killer[0][board.ply].move;
				       g_killer[0][board.ply].move = mvBest.move;
			      }
                    //hisvptr = &(m_his_table[mvBest.dest][PIECE_IDX16(board.piece[mvBest.from])]);
                    hisvptr = &(m_his_table[board.thd_id][PIECE_IDX(board.piece[mvBest.from])][mvBest.dest]);
                    update_HistVal(hisvptr, depth, board.thd_id);
                    update_history(hisvptr, depth, movesSearched, noncap_gen_count, board.thd_id);
            
            if (board.ply >0 && (board.currentMove[board.ply -1].move !=0))
               counterMoves[board.thd_id][PIECE_IDX(board.piece[prevSqdest])][prevSqdest].move = mvBest.move;  //1017
          }
        }
    }
    else if (best <= old_alpha)
    	if (!excludedMove)  //1109
    	   RecordHash(nHashFlag, depth, best, mvBest.move, board); //, 0);

    return best; //thisalpha;

}