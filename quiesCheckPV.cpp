//template <int NT>
int Engine::quiesCheckPV(Board &board, int alpha, int beta, int qdepth)  //1007, int check_depth)
{
    int val, incheck;
    int best, capture, bestmove; //hashflag,  //, kingidx;
    int futilityValue, futilityBase, old_alpha, piecedest;
    int evasionPrunable, moveCount;
    int ttdepth; //1021
//1004    bool givesCheck;

incheck=board.m_hisrecord[board.m_hisindex-1].htab.Chk;
// 2. 和棋裁剪；
    	// If this is too deep then just return the stand-pat score to avoid long
        // quiescence searches.
        if ( ((board.bitpiece & 0xfff00ffc) == 0) 
        	|| (qdepth <= -MAXQDEPTH) )  //test maxqdepth instead of MAXEXT or MAXPLY
        {
            return Evalscore(board);
            //return (qdepth <= -MAXQDEPTH && !incheck) ? Evalscore(board) : 0;
        }

    // 1. Return if there is a mate line bad enough
    if (board.ply-INF >= beta)
    	return (board.ply-INF);

    val=board.checkloop(1);  //1 bef927
    if (val)
    	return val;
 
#ifdef QHASH
int    hashmove = 0;
ttdepth = (incheck || qdepth >= 0) ? 0 : -1;   //1021
    // Transposition table lookup, only when not in PV
    //if ( (qdepth >=check_depth) ) // && !incheck)   //sf191 88i
    //{
    	//if (PVNode)
        hashmove = ProbeMoveQ(board);
      /*  
      else
    	{
//        val = ProbeHashQ(beta, hashmove);  //0
        if (val != UNKNOWN_VALUE)
        {
        	    //printf("probehashQ val=%d\n", val);
        	    return val;
        }
      }
      */
    //}

#endif

    old_alpha = alpha;
//    int opt_value;
//    best = -INF;  //927 need init?
    //int ncapsize;
    MoveTabStruct movetab[111]; //, ncapmovetab[64];
    MoveTabStruct tempmove;

    moveCount = 0;
  
    if (incheck)
    {
        best = futilityBase = -INF;   //927
    } 
    else
    {
        best = val = Evalscore(board);
        if (val >= beta)
        {
            if (hashmove==0)  //90i
               RecordHash(HASH_BETA, 0, val, 0, board); //sf191 88e //2019k chg back from RecordHashQ
               //1021 RecordHash(HASH_BETA, 0, val, 0, board);
            return val; //beta;
        }
        
                 
            //if (PVNode && val > alpha) //sf191 //88e
            if (best > alpha) //sf191 //88e	
                alpha = best;
       
		futilityBase = best + DELTA_MARGIN; //sf10=128;  //927
    }
    
bestmove=0;
int num=0;
int qphase;
//int gen_count=0;
for ( qphase=3; qphase--;)	// 2=hashmove, 1=capture, 0=genchk
//1004for ( qphase=2; qphae>=0; qphase--)	// 2=hashmove, 1=capture, 0=genchk
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
    				//if (num==0 && qdepth < check_depth)  //1021
    				//	return best;	                 //1021
    		}			
    	  else
    	  {
    	  	  num=board.GenChkEvasCap(&movetab[0], incheck);          
    	  }
    		break;

//sf10 to search the QS moves. Because the depth is <= 0 here, only captures,
// queen promotions and checks (only if depth >= DEPTH_QS_CHECKS) will be generated.
    	case 0:	//genchk or evasion
    		if (!incheck)
    		{
        	if (qdepth >=check_depth)
            //&& alpha < beta
          {  num=board.generate_checks(&movetab[0]);
             
          }  	
        	else
            num=0;
        }
    		else
        		num=board.GenChkEvasNCap(&movetab[0], incheck);
    		
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
		  case 0:	//genchk or evasion
      	tempmove.tabentry = GetNextMove(i, num, movetab);
      	if (tempmove.table.move==hashmove)
        		continue;
      break;  
     } // end switch qphase
     
        moveCount++;
        piecedest = board.piece[tempmove.dest];	
        //1003
        
       //1004 	givesCheck = board.gives_check(tempmove.from, tempmove.dest);
//927 futility pruning for QSPV ref sf10
        if ( !incheck 
        	  && (qphase!=0 && !(board.gives_check(tempmove.from, tempmove.dest)) )    //1004  !givesCheck
        	  && futilityBase > -WIN_VALUE)
        { 
        	
        	futilityValue = futilityBase + abs(pointtable[PIECE_IDX(piecedest)][nRank(tempmove.dest)][nFile(tempmove.dest)]);
        	if (futilityValue <= alpha)
          {
              best = std::max(best, futilityValue);
              continue;
          }
//1001    if (futilityBase <= alpha)     //sf10 && !pos.see_ge(move, VALUE_ZERO + 1))
          if ((futilityBase <= alpha) && !board.see_ge(tempmove.from, tempmove.dest,  1))
          {   
             best = std::max(best, futilityBase);
             continue;          
          } 
        } 

        // Detect non-capture evasions that are candidates to be pruned		
		if ( (incheck)
			&& (qdepth != 0 || moveCount > 2)
			&& (best > -INF + 128)   //VALUE_MATED_IN_MAX_PLY
			&& (piecedest == 0))        //!pos.capture(move);
          evasionPrunable =1;
    else        
          evasionPrunable =0;     
                 
        // Don't search moves with negative SEE values
        if (  (!incheck || evasionPrunable) && !board.see_ge(tempmove.from, tempmove.dest,0))
      	{     continue;  //1002
          // && !pos.see_ge(move))
/*       
          if (piecedest < B_HORSE1
        	      //|| (board.piece[tempmove.from]>=B_ROOK1
        	      || ((board.piece[tempmove.from]>>2)==ROOK  //exclude king capture for SEE()
        	      && piecedest < B_ROOK1))
             {
//        	      if (board.see(tempmove.from, tempmove.dest) < 0)
        				continue;
             }  
*/                                 
        }

        capture = board.makemove(tempmove.table,
        	 (incheck
						|| (kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.dest]
						|| kingattk_incl_horse[kingindex[board.boardsq[32+board.m_side]]][tempmove.from]))
						);
				
				if (capture < 0)
				{ 
					moveCount--;	// illegal move?	
        	continue;
        }
				//gen_count++;

        //if (capture >=B_KING)
        //	val = INF-ply;
        //else
        val = -quiesCheckPV(board, -beta, -alpha, qdepth-1); //1007, check_depth);
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
					//if ((	qdepth >=check_depth) && !incheck) //1020
            RecordHashQ(HASH_BETA, ttdepth, val, tempmove.table.move, board);
            //1021 RecordHash(HASH_BETA, 0, val, tempmove.table.move, board);          
#endif
            //printf("beta cutoff val=%d\n", val);
            return val; // beta;
        }
        if (val > best)
        {
            best = val;
            if (val > alpha)
            {
                alpha = val;
                bestmove = tempmove.table.move;
                
//1021 sf10   if (PvNode) // Update pv even in fail-high case
//1021                  update_pv(ss->pv, move, (ss+1)->pv);

//1021              if (PvNode && value < beta) // Update alpha here!
//1021                  alpha = value;
            }
        }
    }	// end for i (< num)
} // end for qphase

// 8. 处理
    if ((best == -INF))
//			if (gen_count==0 && incheck)	//mate!
			{
        //printf("ply-INF val=%d\n", board.ply-INF);
        return board.ply - INF;
    	}

#ifdef QHASH
    //if ((qdepth >=check_depth) && !incheck)  //1020
    //if (qdepth >=check_depth)
    {
        //int hashflag; //, hashmv;
        //if (alpha == old_alpha)
        if (best <= old_alpha) //sf191	//88e
        {
            RecordHashQ(HASH_ALPHA, ttdepth, best, bestmove, board); //1021 
            //1021 RecordHash(HASH_ALPHA, 0, best, 0, board);
            //hashflag = HASH_ALPHA;
            //bestmove = 0;
        }
        else
        {
            //hashflag = HASH_PV;
            //hashmove = ss->pv[ply];
            RecordHashQ(HASH_PV, ttdepth, best, bestmove, board);
            //1021 RecordHash(HASH_PV, 0, best, bestmove, board);
        }
    }
    //1020 else QSPVhashrecmiss++;
#endif

    // 8. 处理
    //if (best == -INF) {
    //    return board.ply - INF;
    //}
    //printf("Q best val=%d\n", best);
    return best;
}