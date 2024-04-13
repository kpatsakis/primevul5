FoFiType1C::FoFiType1C(char *fileA, int lenA, GBool freeFileDataA):
  FoFiBase(fileA, lenA, freeFileDataA)
{
  name = NULL;
  encoding = NULL;
  privateDicts = NULL;
  fdSelect = NULL;
  charset = NULL;
  charsetLength = 0;
}
