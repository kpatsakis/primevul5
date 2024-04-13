static int windowCacheFrame(Window *pMWin){
  Window *pWin;
  if( pMWin->regStartRowid ) return 1;
  for(pWin=pMWin; pWin; pWin=pWin->pNextWin){
    FuncDef *pFunc = pWin->pFunc;
    if( (pFunc->zName==nth_valueName)
     || (pFunc->zName==first_valueName)
     || (pFunc->zName==leadName)
     || (pFunc->zName==lagName)
    ){
      return 1;
    }
  }
  return 0;
}