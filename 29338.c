char *copyString(char *s) {
  char *s1;

  s1 = (char *)gmalloc(strlen(s) + 1);
  strcpy(s1, s);
  return s1;
}
