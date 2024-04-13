char *trim_whitespace(char *str) {
char *dud = str;
int i;

   /* beginning whitespace first */
   while( (int)*dud && isspace((int)*dud) )
      ++dud;
   /* now trailing whitespace */
   i = strlen(dud) - 1;
   while( i>=0 && isspace((int)dud[i]) )
      --i;
   dud[i+1] = 0;
   return dud;
}