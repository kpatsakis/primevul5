void FoFiType1C::readPrivateDict(int offset, int length,
				 Type1CPrivateDict *pDict) {
  int pos;

  pDict->hasFontMatrix = gFalse;
  pDict->nBlueValues = 0;
  pDict->nOtherBlues = 0;
  pDict->nFamilyBlues = 0;
  pDict->nFamilyOtherBlues = 0;
  pDict->blueScale = 0.039625;
  pDict->blueShift = 7;
  pDict->blueFuzz = 1;
  pDict->hasStdHW = gFalse;
  pDict->hasStdVW = gFalse;
  pDict->nStemSnapH = 0;
  pDict->nStemSnapV = 0;
  pDict->hasForceBold = gFalse;
  pDict->forceBoldThreshold = 0;
  pDict->languageGroup = 0;
  pDict->expansionFactor = 0.06;
  pDict->initialRandomSeed = 0;
  pDict->subrsOffset = 0;
  pDict->defaultWidthX = 0;
  pDict->defaultWidthXFP = gFalse;
  pDict->nominalWidthX = 0;
  pDict->nominalWidthXFP = gFalse;

  if (offset == 0 || length == 0) {
    return;
  }

  pos = offset;
  nOps = 0;
  while (pos < offset + length) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      break;
    }
    if (!ops[nOps - 1].isNum) {
      --nOps; // drop the operator
      switch (ops[nOps].op) {
      case 0x0006:
	pDict->nBlueValues = getDeltaIntArray(pDict->blueValues,
					      type1CMaxBlueValues);
	break;
      case 0x0007:
	pDict->nOtherBlues = getDeltaIntArray(pDict->otherBlues,
					      type1CMaxOtherBlues);
	break;
      case 0x0008:
	pDict->nFamilyBlues = getDeltaIntArray(pDict->familyBlues,
					       type1CMaxBlueValues);
	break;
      case 0x0009:
	pDict->nFamilyOtherBlues = getDeltaIntArray(pDict->familyOtherBlues,
						    type1CMaxOtherBlues);
	break;
      case 0x0c09:
	pDict->blueScale = ops[0].num;
	break;
      case 0x0c0a:
	pDict->blueShift = (int)ops[0].num;
	break;
      case 0x0c0b:
	pDict->blueFuzz = (int)ops[0].num;
	break;
      case 0x000a:
	pDict->stdHW = ops[0].num;
	pDict->hasStdHW = gTrue;
	break;
      case 0x000b:
	pDict->stdVW = ops[0].num;
	pDict->hasStdVW = gTrue;
	break;
      case 0x0c0c:
	pDict->nStemSnapH = getDeltaFPArray(pDict->stemSnapH,
					    type1CMaxStemSnap);
	break;
      case 0x0c0d:
	pDict->nStemSnapV = getDeltaFPArray(pDict->stemSnapV,
					    type1CMaxStemSnap);
	break;
      case 0x0c0e:
	pDict->forceBold = ops[0].num != 0;
	pDict->hasForceBold = gTrue;
	break;
      case 0x0c0f:
	pDict->forceBoldThreshold = ops[0].num;
	break;
      case 0x0c11:
	pDict->languageGroup = (int)ops[0].num;
	break;
      case 0x0c12:
	pDict->expansionFactor = ops[0].num;
	break;
      case 0x0c13:
	pDict->initialRandomSeed = (int)ops[0].num;
	break;
      case 0x0013:
	pDict->subrsOffset = offset + (int)ops[0].num;
	break;
      case 0x0014:
	pDict->defaultWidthX = ops[0].num;
	pDict->defaultWidthXFP = ops[0].isFP;
	break;
      case 0x0015:
	pDict->nominalWidthX = ops[0].num;
	pDict->nominalWidthXFP = ops[0].isFP;
	break;
      }
      nOps = 0;
    }
  }
}
