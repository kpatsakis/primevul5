void sqlite3VdbeFrameMemDel(void *pArg){
  VdbeFrame *pFrame = (VdbeFrame*)pArg;
  assert( sqlite3VdbeFrameIsValid(pFrame) );
  pFrame->pParent = pFrame->v->pDelFrame;
  pFrame->v->pDelFrame = pFrame;
}