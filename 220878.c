alloc_keyskip()
{
  Id *keyskip = solv_calloc(3 + 256, sizeof(Id));
  keyskip[0] = 256; 
  keyskip[1] = keyskip[2] = 1; 
  return keyskip;
}