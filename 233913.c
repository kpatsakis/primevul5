SHA256CTX sha256_init(void){
  SHA256CTX c = malloc(sizeof(*c));
  if (c == NULL) {
    return NULL;
  }
  SHA256_Init(c);

  return c;
}