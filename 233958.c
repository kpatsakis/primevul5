SHACTX sha1_init(void) {
  SHACTX c = malloc(sizeof(*c));
  if (c == NULL) {
    return NULL;
  }
  SHA1_Init(c);

  return c;
}