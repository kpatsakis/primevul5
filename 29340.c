char *gstrndup(const char *s, size_t n) {
  char *s1 = (char*)gmalloc(n + 1); /* cannot return NULL for size > 0 */
  s1[n] = '\0';
  memcpy(s1, s, n);
  return s1;
}
