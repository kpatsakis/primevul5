int sqlite3VdbeMakeLabel(Parse *pParse){
  return --pParse->nLabel;
}