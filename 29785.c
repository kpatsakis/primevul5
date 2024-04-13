void FoFiType1C::readTopDict() {
  Type1CIndexVal topDictPtr;
  int pos;

  topDict.firstOp = -1;
  topDict.versionSID = 0;
  topDict.noticeSID = 0;
  topDict.copyrightSID = 0;
  topDict.fullNameSID = 0;
  topDict.familyNameSID = 0;
  topDict.weightSID = 0;
  topDict.isFixedPitch = 0;
  topDict.italicAngle = 0;
  topDict.underlinePosition = -100;
  topDict.underlineThickness = 50;
  topDict.paintType = 0;
  topDict.charstringType = 2;
  topDict.fontMatrix[0] = 0.001;
  topDict.fontMatrix[1] = 0;
  topDict.fontMatrix[2] = 0;
  topDict.fontMatrix[3] = 0.001;
  topDict.fontMatrix[4] = 0;
  topDict.fontMatrix[5] = 0;
  topDict.hasFontMatrix = gFalse;
  topDict.uniqueID = 0;
  topDict.fontBBox[0] = 0;
  topDict.fontBBox[1] = 0;
  topDict.fontBBox[2] = 0;
  topDict.fontBBox[3] = 0;
  topDict.strokeWidth = 0;
  topDict.charsetOffset = 0;
  topDict.encodingOffset = 0;
  topDict.charStringsOffset = 0;
  topDict.privateSize = 0;
  topDict.privateOffset = 0;
  topDict.registrySID = 0;
  topDict.orderingSID = 0;
  topDict.supplement = 0;
  topDict.fdArrayOffset = 0;
  topDict.fdSelectOffset = 0;

  getIndexVal(&topDictIdx, 0, &topDictPtr, &parsedOk);
  pos = topDictPtr.pos;
  nOps = 0;
  while (pos < topDictPtr.pos + topDictPtr.len) {
    pos = getOp(pos, gFalse, &parsedOk);
    if (!parsedOk) {
      break;
    }
    if (!ops[nOps - 1].isNum) {
      --nOps; // drop the operator
      if (topDict.firstOp < 0) {
	topDict.firstOp = ops[nOps].op;
      }
      switch (ops[nOps].op) {
      case 0x0000: topDict.versionSID = (int)ops[0].num; break;
      case 0x0001: topDict.noticeSID = (int)ops[0].num; break;
      case 0x0c00: topDict.copyrightSID = (int)ops[0].num; break;
      case 0x0002: topDict.fullNameSID = (int)ops[0].num; break;
      case 0x0003: topDict.familyNameSID = (int)ops[0].num; break;
      case 0x0004: topDict.weightSID = (int)ops[0].num; break;
      case 0x0c01: topDict.isFixedPitch = (int)ops[0].num; break;
      case 0x0c02: topDict.italicAngle = ops[0].num; break;
      case 0x0c03: topDict.underlinePosition = ops[0].num; break;
      case 0x0c04: topDict.underlineThickness = ops[0].num; break;
      case 0x0c05: topDict.paintType = (int)ops[0].num; break;
      case 0x0c06: topDict.charstringType = (int)ops[0].num; break;
      case 0x0c07: topDict.fontMatrix[0] = ops[0].num;
	           topDict.fontMatrix[1] = ops[1].num;
	           topDict.fontMatrix[2] = ops[2].num;
	           topDict.fontMatrix[3] = ops[3].num;
	           topDict.fontMatrix[4] = ops[4].num;
	           topDict.fontMatrix[5] = ops[5].num;
		   topDict.hasFontMatrix = gTrue; break;
      case 0x000d: topDict.uniqueID = (int)ops[0].num; break;
      case 0x0005: topDict.fontBBox[0] = ops[0].num;
	           topDict.fontBBox[1] = ops[1].num;
	           topDict.fontBBox[2] = ops[2].num;
	           topDict.fontBBox[3] = ops[3].num; break;
      case 0x0c08: topDict.strokeWidth = ops[0].num; break;
      case 0x000f: topDict.charsetOffset = (int)ops[0].num; break;
      case 0x0010: topDict.encodingOffset = (int)ops[0].num; break;
      case 0x0011: topDict.charStringsOffset = (int)ops[0].num; break;
      case 0x0012: topDict.privateSize = (int)ops[0].num;
	           topDict.privateOffset = (int)ops[1].num; break;
      case 0x0c1e: topDict.registrySID = (int)ops[0].num;
	           topDict.orderingSID = (int)ops[1].num;
		   topDict.supplement = (int)ops[2].num; break;
      case 0x0c24: topDict.fdArrayOffset = (int)ops[0].num; break;
      case 0x0c25: topDict.fdSelectOffset = (int)ops[0].num; break;
      }
      nOps = 0;
    }
  }
}
