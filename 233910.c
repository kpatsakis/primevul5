MD5CTX md5_init(void) {
  MD5CTX c = malloc(sizeof(*c));
  if (c == NULL) {
    return NULL;
  }

  MD5_Init(c);

  return c;
}