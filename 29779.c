FoFiType1C *FoFiType1C::load(char *fileName) {
  FoFiType1C *ff;
  char *fileA;
  int lenA;

  if (!(fileA = FoFiBase::readFile(fileName, &lenA))) {
    return NULL;
  }
  ff = new FoFiType1C(fileA, lenA, gTrue);
  if (!ff->parse()) {
    delete ff;
    return NULL;
  }
  return ff;
}
