static int isAllZero(const char *z, int n){
  int i;
  for(i=0; i<n; i++){
    if( z[i] ) return 0;
  }
  return 1;
}