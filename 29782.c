void FoFiType1C::readFD(int offset, int length, Type1CPrivateDict *pDict) {
  int pos, pSize, pOffset;
  double fontMatrix[6] = {0};
  GBool hasFontMatrix;

  hasFontMatrix = gFalse;
  fontMatrix[0] = fontMatrix[1] = fontMatrix[2] = 0; // make gcc happy
  fontMatrix[3] = fontMatrix[4] = fontMatrix[5] = 0;
  pSize = pOffset = 0;
  pos = offset;
  nOps = 0;
  while (pos < offset + length) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      return;
    }
    if (!ops[nOps - 1].isNum) {
      if (ops[nOps - 1].op == 0x0012) {
        if (nOps < 3) {
          parsedOk = gFalse;
          return;
        }
        pSize = (int)ops[0].num;
        pOffset = (int)ops[1].num;
        break;
      } else if (ops[nOps - 1].op == 0x0c07) {
        fontMatrix[0] = ops[0].num;
        fontMatrix[1] = ops[1].num;
        fontMatrix[2] = ops[2].num;
        fontMatrix[3] = ops[3].num;
        fontMatrix[4] = ops[4].num;
        fontMatrix[5] = ops[5].num;
        hasFontMatrix = gTrue;
      }
      nOps = 0;
    }
  }
  readPrivateDict(pOffset, pSize, pDict);
  if (hasFontMatrix) {
    pDict->fontMatrix[0] = fontMatrix[0];
    pDict->fontMatrix[1] = fontMatrix[1];
    pDict->fontMatrix[2] = fontMatrix[2];
    pDict->fontMatrix[3] = fontMatrix[3];
    pDict->fontMatrix[4] = fontMatrix[4];
    pDict->fontMatrix[5] = fontMatrix[5];
    pDict->hasFontMatrix = gTrue;
  }
}
