static int displayComment(
  const Op *pOp,     /* The opcode to be commented */
  const char *zP4,   /* Previously obtained value for P4 */
  char *zTemp,       /* Write result here */
  int nTemp          /* Space available in zTemp[] */
){
  const char *zOpName;
  const char *zSynopsis;
  int nOpName;
  int ii, jj;
  char zAlt[50];
  zOpName = sqlite3OpcodeName(pOp->opcode);
  nOpName = sqlite3Strlen30(zOpName);
  if( zOpName[nOpName+1] ){
    int seenCom = 0;
    char c;
    zSynopsis = zOpName += nOpName + 1;
    if( strncmp(zSynopsis,"IF ",3)==0 ){
      if( pOp->p5 & SQLITE_STOREP2 ){
        sqlite3_snprintf(sizeof(zAlt), zAlt, "r[P2] = (%s)", zSynopsis+3);
      }else{
        sqlite3_snprintf(sizeof(zAlt), zAlt, "if %s goto P2", zSynopsis+3);
      }
      zSynopsis = zAlt;
    }
    for(ii=jj=0; jj<nTemp-1 && (c = zSynopsis[ii])!=0; ii++){
      if( c=='P' ){
        c = zSynopsis[++ii];
        if( c=='4' ){
          sqlite3_snprintf(nTemp-jj, zTemp+jj, "%s", zP4);
        }else if( c=='X' ){
          sqlite3_snprintf(nTemp-jj, zTemp+jj, "%s", pOp->zComment);
          seenCom = 1;
        }else{
          int v1 = translateP(c, pOp);
          int v2;
          sqlite3_snprintf(nTemp-jj, zTemp+jj, "%d", v1);
          if( strncmp(zSynopsis+ii+1, "@P", 2)==0 ){
            ii += 3;
            jj += sqlite3Strlen30(zTemp+jj);
            v2 = translateP(zSynopsis[ii], pOp);
            if( strncmp(zSynopsis+ii+1,"+1",2)==0 ){
              ii += 2;
              v2++;
            }
            if( v2>1 ){
              sqlite3_snprintf(nTemp-jj, zTemp+jj, "..%d", v1+v2-1);
            }
          }else if( strncmp(zSynopsis+ii+1, "..P3", 4)==0 && pOp->p3==0 ){
            ii += 4;
          }
        }
        jj += sqlite3Strlen30(zTemp+jj);
      }else{
        zTemp[jj++] = c;
      }
    }
    if( !seenCom && jj<nTemp-5 && pOp->zComment ){
      sqlite3_snprintf(nTemp-jj, zTemp+jj, "; %s", pOp->zComment);
      jj += sqlite3Strlen30(zTemp+jj);
    }
    if( jj<nTemp ) zTemp[jj] = 0;
  }else if( pOp->zComment ){
    sqlite3_snprintf(nTemp, zTemp, "%s", pOp->zComment);
    jj = sqlite3Strlen30(zTemp);
  }else{
    zTemp[0] = 0;
    jj = 0;
  }
  return jj;
}