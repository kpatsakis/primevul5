static int exprNodeIsConstant(Walker *pWalker, Expr *pExpr){

  /* If pWalker->eCode is 2 then any term of the expression that comes from
  ** the ON or USING clauses of a left join disqualifies the expression
  ** from being considered constant. */
  if( pWalker->eCode==2 && ExprHasProperty(pExpr, EP_FromJoin) ){
    pWalker->eCode = 0;
    return WRC_Abort;
  }

  switch( pExpr->op ){
    /* Consider functions to be constant if all their arguments are constant
    ** and either pWalker->eCode==4 or 5 or the function has the
    ** SQLITE_FUNC_CONST flag. */
    case TK_FUNCTION:
      if( (pWalker->eCode>=4 || ExprHasProperty(pExpr,EP_ConstFunc))
       && !ExprHasProperty(pExpr, EP_WinFunc)
      ){
        return WRC_Continue;
      }else{
        pWalker->eCode = 0;
        return WRC_Abort;
      }
    case TK_ID:
      /* Convert "true" or "false" in a DEFAULT clause into the
      ** appropriate TK_TRUEFALSE operator */
      if( sqlite3ExprIdToTrueFalse(pExpr) ){
        return WRC_Prune;
      }
      /* Fall thru */
    case TK_COLUMN:
    case TK_AGG_FUNCTION:
    case TK_AGG_COLUMN:
      testcase( pExpr->op==TK_ID );
      testcase( pExpr->op==TK_COLUMN );
      testcase( pExpr->op==TK_AGG_FUNCTION );
      testcase( pExpr->op==TK_AGG_COLUMN );
      if( ExprHasProperty(pExpr, EP_FixedCol) && pWalker->eCode!=2 ){
        return WRC_Continue;
      }
      if( pWalker->eCode==3 && pExpr->iTable==pWalker->u.iCur ){
        return WRC_Continue;
      }
      /* Fall through */
    case TK_IF_NULL_ROW:
    case TK_REGISTER:
      testcase( pExpr->op==TK_REGISTER );
      testcase( pExpr->op==TK_IF_NULL_ROW );
      pWalker->eCode = 0;
      return WRC_Abort;
    case TK_VARIABLE:
      if( pWalker->eCode==5 ){
        /* Silently convert bound parameters that appear inside of CREATE
        ** statements into a NULL when parsing the CREATE statement text out
        ** of the sqlite_master table */
        pExpr->op = TK_NULL;
      }else if( pWalker->eCode==4 ){
        /* A bound parameter in a CREATE statement that originates from
        ** sqlite3_prepare() causes an error */
        pWalker->eCode = 0;
        return WRC_Abort;
      }
      /* Fall through */
    default:
      testcase( pExpr->op==TK_SELECT ); /* sqlite3SelectWalkFail() disallows */
      testcase( pExpr->op==TK_EXISTS ); /* sqlite3SelectWalkFail() disallows */
      return WRC_Continue;
  }
}