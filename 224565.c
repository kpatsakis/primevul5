int sqlite3VdbeFrameIsValid(VdbeFrame *pFrame){
  if( pFrame->iFrameMagic!=SQLITE_FRAME_MAGIC ) return 0;
  return 1;
}