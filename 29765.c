void FoFiType1C::cvtGlyph(int offset, int nBytes, GooString *charBuf,
			  Type1CIndex *subrIdx, Type1CPrivateDict *pDict,
			  GBool top) {
  Type1CIndexVal val;
  GBool ok, dFP;
  double d, dx, dy;
  Gushort r2;
  Guchar byte;
  int pos, subrBias, start, i, k;

  start = charBuf->getLength();
  if (top) {
    charBuf->append((char)73);
    charBuf->append((char)58);
    charBuf->append((char)147);
    charBuf->append((char)134);
    nOps = 0;
    nHints = 0;
    firstOp = gTrue;
    openPath = gFalse;
  }

  pos = offset;
  while (pos < offset + nBytes) {
    ok = gTrue;
    pos = getOp(pos, gTrue, &ok);
    if (!ok) {
      break;
    }
    if (!ops[nOps - 1].isNum) {
      --nOps; // drop the operator
      switch (ops[nOps].op) {
      case 0x0001:		// hstem
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	}
	d = 0;
	dFP = gFalse;
	for (k = 0; k < nOps; k += 2) {
	  if (ops[k+1].num < 0) {
	    d += ops[k].num + ops[k+1].num;
	    dFP |= ops[k].isFP | ops[k+1].isFP;
	    cvtNum(d, dFP, charBuf);
	    cvtNum(-ops[k+1].num, ops[k+1].isFP, charBuf);
	  } else {
	    d += ops[k].num;
	    dFP |= ops[k].isFP;
	    cvtNum(d, dFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    d += ops[k+1].num;
	    dFP |= ops[k+1].isFP;
	  }
	  charBuf->append((char)1);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0003:		// vstem
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	}
	d = 0;
	dFP = gFalse;
	for (k = 0; k < nOps; k += 2) {
	  if (ops[k+1].num < 0) {
	    d += ops[k].num + ops[k+1].num;
	    dFP |= ops[k].isFP | ops[k+1].isFP;
	    cvtNum(d, dFP, charBuf);
	    cvtNum(-ops[k+1].num, ops[k+1].isFP, charBuf);
	  } else {
	    d += ops[k].num;
	    dFP |= ops[k].isFP;
	    cvtNum(d, dFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    d += ops[k+1].num;
	    dFP |= ops[k+1].isFP;
	  }
	  charBuf->append((char)3);
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0004:		// vmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 2, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 1) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	charBuf->append((char)4);
	nOps = 0;
	break;
      case 0x0005:		// rlineto
	if (nOps < 2 || nOps % 2 != 0) {
	}
	for (k = 0; k < nOps; k += 2) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	  charBuf->append((char)5);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0006:		// hlineto
	if (nOps < 1) {
	}
	for (k = 0; k < nOps; ++k) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  charBuf->append((char)((k & 1) ? 7 : 6));
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0007:		// vlineto
	if (nOps < 1) {
	}
	for (k = 0; k < nOps; ++k) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  charBuf->append((char)((k & 1) ? 6 : 7));
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0008:		// rrcurveto
	if (nOps < 6 || nOps % 6 != 0) {
	}
	for (k = 0; k < nOps; k += 6) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	  cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	  cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	  cvtNum(ops[k+5].num, ops[k+5].isFP, charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x000a:		// callsubr
	if (nOps >= 1) {
	  subrBias = (subrIdx->len < 1240)
	               ? 107 : (subrIdx->len < 33900) ? 1131 : 32768;
	  k = subrBias + (int)ops[nOps - 1].num;
	  --nOps;
	  ok = gTrue;
	  getIndexVal(subrIdx, k, &val, &ok);
	  if (ok) {
	    cvtGlyph(val.pos, val.len, charBuf, subrIdx, pDict, gFalse);
	  }
	} else {
	}
	break;
      case 0x000b:		// return
	break;
      case 0x000e:		// endchar / seac
	if (firstOp) {
	  cvtGlyphWidth(nOps == 1 || nOps == 5, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps == 4) {
	  cvtNum(0, gFalse, charBuf);
	  cvtNum(ops[0].num, ops[0].isFP, charBuf);
	  cvtNum(ops[1].num, ops[1].isFP, charBuf);
	  cvtNum(ops[2].num, ops[2].isFP, charBuf);
	  cvtNum(ops[3].num, ops[3].isFP, charBuf);
	  charBuf->append((char)12)->append((char)6);
	} else if (nOps == 0) {
	  charBuf->append((char)14);
	} else {
	}
	nOps = 0;
	break;
      case 0x000f:		// (obsolete)
	if (firstOp) {
	  cvtGlyphWidth(nOps > 0, charBuf, pDict);
	  firstOp = gFalse;
	}
	nOps = 0;
	break;
      case 0x0010:		// blend
	nOps = 0;
	break;
      case 0x0012:		// hstemhm
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0013:		// hintmask
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps > 0) {
	  if (nOps & 1) {
	  }
	  nHints += nOps / 2;
	}
	pos += (nHints + 7) >> 3;
	nOps = 0;
	break;
      case 0x0014:		// cntrmask
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps > 0) {
	  if (nOps & 1) {
	  }
	  nHints += nOps / 2;
	}
	pos += (nHints + 7) >> 3;
	nOps = 0;
	break;
      case 0x0015:		// rmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 3, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 2) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	cvtNum(ops[1].num, ops[1].isFP, charBuf);
	charBuf->append((char)21);
	nOps = 0;
	break;
      case 0x0016:		// hmoveto
	if (firstOp) {
	  cvtGlyphWidth(nOps == 2, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (openPath) {
	  charBuf->append((char)9);
	  openPath = gFalse;
	}
	if (nOps != 1) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	charBuf->append((char)22);
	nOps = 0;
	break;
      case 0x0017:		// vstemhm
	if (firstOp) {
	  cvtGlyphWidth(nOps & 1, charBuf, pDict);
	  firstOp = gFalse;
	}
	if (nOps & 1) {
	}
	nHints += nOps / 2;
	nOps = 0;
	break;
      case 0x0018:		// rcurveline
	if (nOps < 8 || (nOps - 2) % 6 != 0) {
	}
	for (k = 0; k < nOps - 2; k += 6) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	  cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	  cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	  cvtNum(ops[k+5].num, ops[k+5].isFP, charBuf);
	  charBuf->append((char)8);
	}
	cvtNum(ops[k].num, ops[k].isFP, charBuf);
	cvtNum(ops[k+1].num, ops[k].isFP, charBuf);
	charBuf->append((char)5);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0019:		// rlinecurve
	if (nOps < 8 || (nOps - 6) % 2 != 0) {
	}
	for (k = 0; k < nOps - 6; k += 2) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(ops[k+1].num, ops[k].isFP, charBuf);
	  charBuf->append((char)5);
	}
	cvtNum(ops[k].num, ops[k].isFP, charBuf);
	cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	cvtNum(ops[k+5].num, ops[k+5].isFP, charBuf);
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001a:		// vvcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	}
	if (nOps % 2 == 1) {
	  cvtNum(ops[0].num, ops[0].isFP, charBuf);
	  cvtNum(ops[1].num, ops[1].isFP, charBuf);
	  cvtNum(ops[2].num, ops[2].isFP, charBuf);
	  cvtNum(ops[3].num, ops[3].isFP, charBuf);
	  cvtNum(0, gFalse, charBuf);
	  cvtNum(ops[4].num, ops[4].isFP, charBuf);
	  charBuf->append((char)8);
	  k = 5;
	} else {
	  k = 0;
	}
	for (; k < nOps; k += 4) {
	  cvtNum(0, gFalse, charBuf);
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	  cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	  cvtNum(0, gFalse, charBuf);
	  cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001b:		// hhcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	}
	if (nOps % 2 == 1) {
	  cvtNum(ops[1].num, ops[1].isFP, charBuf);
	  cvtNum(ops[0].num, ops[0].isFP, charBuf);
	  cvtNum(ops[2].num, ops[2].isFP, charBuf);
	  cvtNum(ops[3].num, ops[3].isFP, charBuf);
	  cvtNum(ops[4].num, ops[4].isFP, charBuf);
	  cvtNum(0, gFalse, charBuf);
	  charBuf->append((char)8);
	  k = 5;
	} else {
	  k = 0;
	}
	for (; k < nOps; k += 4) {
	  cvtNum(ops[k].num, ops[k].isFP, charBuf);
	  cvtNum(0, gFalse, charBuf);
	  cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	  cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	  cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  cvtNum(0, gFalse, charBuf);
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001d:		// callgsubr
	if (nOps >= 1) {
	  k = gsubrBias + (int)ops[nOps - 1].num;
	  --nOps;
	  ok = gTrue;
	  getIndexVal(&gsubrIdx, k, &val, &ok);
	  if (ok) {
	    cvtGlyph(val.pos, val.len, charBuf, subrIdx, pDict, gFalse);
	  }
	} else {
	}
	break;
      case 0x001e:		// vhcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	}
	for (k = 0; k < nOps && k != nOps-5; k += 4) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    charBuf->append((char)30);
	  } else {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    charBuf->append((char)31);
	  }
	}
	if (k == nOps-5) {
	  if (k % 8 == 0) {
	    cvtNum(0, gFalse, charBuf);
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	  } else {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(0, gFalse, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  }
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x001f:		// hvcurveto
	if (nOps < 4 || !(nOps % 4 == 0 || (nOps-1) % 4 == 0)) {
	}
	for (k = 0; k < nOps && k != nOps-5; k += 4) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    charBuf->append((char)31);
	  } else {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    charBuf->append((char)30);
	  }
	}
	if (k == nOps-5) {
	  if (k % 8 == 0) {
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(0, gFalse, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	  } else {
	    cvtNum(0, gFalse, charBuf);
	    cvtNum(ops[k].num, ops[k].isFP, charBuf);
	    cvtNum(ops[k+1].num, ops[k+1].isFP, charBuf);
	    cvtNum(ops[k+2].num, ops[k+2].isFP, charBuf);
	    cvtNum(ops[k+3].num, ops[k+3].isFP, charBuf);
	    cvtNum(ops[k+4].num, ops[k+4].isFP, charBuf);
	  }
	  charBuf->append((char)8);
	}
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c00:		// dotsection (should be Type 1 only?)
	nOps = 0;
	break;
      case 0x0c03:		// and
      case 0x0c04:		// or
      case 0x0c05:		// not
      case 0x0c08:		// store
      case 0x0c09:		// abs
      case 0x0c0a:		// add
      case 0x0c0b:		// sub
      case 0x0c0c:		// div
      case 0x0c0d:		// load
      case 0x0c0e:		// neg
      case 0x0c0f:		// eq
      case 0x0c12:		// drop
      case 0x0c14:		// put
      case 0x0c15:		// get
      case 0x0c16:		// ifelse
      case 0x0c17:		// random
      case 0x0c18:		// mul
      case 0x0c1a:		// sqrt
      case 0x0c1b:		// dup
      case 0x0c1c:		// exch
      case 0x0c1d:		// index
      case 0x0c1e:		// roll
	nOps = 0;
	break;
      case 0x0c22:		// hflex
	if (nOps != 7) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	cvtNum(ops[1].num, ops[1].isFP, charBuf);
	cvtNum(ops[2].num, ops[2].isFP, charBuf);
	cvtNum(ops[3].num, ops[3].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	charBuf->append((char)8);
	cvtNum(ops[4].num, ops[4].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	cvtNum(ops[5].num, ops[5].isFP, charBuf);
	cvtNum(-ops[2].num, ops[2].isFP, charBuf);
	cvtNum(ops[6].num, ops[6].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c23:		// flex
	if (nOps != 13) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	cvtNum(ops[1].num, ops[1].isFP, charBuf);
	cvtNum(ops[2].num, ops[2].isFP, charBuf);
	cvtNum(ops[3].num, ops[3].isFP, charBuf);
	cvtNum(ops[4].num, ops[4].isFP, charBuf);
	cvtNum(ops[5].num, ops[5].isFP, charBuf);
	charBuf->append((char)8);
	cvtNum(ops[6].num, ops[6].isFP, charBuf);
	cvtNum(ops[7].num, ops[7].isFP, charBuf);
	cvtNum(ops[8].num, ops[8].isFP, charBuf);
	cvtNum(ops[9].num, ops[9].isFP, charBuf);
	cvtNum(ops[10].num, ops[10].isFP, charBuf);
	cvtNum(ops[11].num, ops[11].isFP, charBuf);
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c24:		// hflex1
	if (nOps != 9) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	cvtNum(ops[1].num, ops[1].isFP, charBuf);
	cvtNum(ops[2].num, ops[2].isFP, charBuf);
	cvtNum(ops[3].num, ops[3].isFP, charBuf);
	cvtNum(ops[4].num, ops[4].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	charBuf->append((char)8);
	cvtNum(ops[5].num, ops[5].isFP, charBuf);
	cvtNum(0, gFalse, charBuf);
	cvtNum(ops[6].num, ops[6].isFP, charBuf);
	cvtNum(ops[7].num, ops[7].isFP, charBuf);
	cvtNum(ops[8].num, ops[8].isFP, charBuf);
	cvtNum(-(ops[1].num + ops[3].num + ops[7].num),
	       ops[1].isFP | ops[3].isFP | ops[7].isFP, charBuf);
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      case 0x0c25:		// flex1
	if (nOps != 11) {
	}
	cvtNum(ops[0].num, ops[0].isFP, charBuf);
	cvtNum(ops[1].num, ops[1].isFP, charBuf);
	cvtNum(ops[2].num, ops[2].isFP, charBuf);
	cvtNum(ops[3].num, ops[3].isFP, charBuf);
	cvtNum(ops[4].num, ops[4].isFP, charBuf);
	cvtNum(ops[5].num, ops[5].isFP, charBuf);
	charBuf->append((char)8);
	cvtNum(ops[6].num, ops[6].isFP, charBuf);
	cvtNum(ops[7].num, ops[7].isFP, charBuf);
	cvtNum(ops[8].num, ops[8].isFP, charBuf);
	cvtNum(ops[9].num, ops[9].isFP, charBuf);
	dx = ops[0].num + ops[2].num + ops[4].num + ops[6].num + ops[8].num;
	dy = ops[1].num + ops[3].num + ops[5].num + ops[7].num + ops[9].num;
	if (fabs(dx) > fabs(dy)) {
	  cvtNum(ops[10].num, ops[10].isFP, charBuf);
	  cvtNum(-dy, ops[1].isFP | ops[3].isFP | ops[5].isFP |
		      ops[7].isFP | ops[9].isFP, charBuf);
	} else {
	  cvtNum(-dx, ops[0].isFP | ops[2].isFP | ops[4].isFP |
		      ops[6].isFP | ops[8].isFP, charBuf);
	  cvtNum(ops[10].num, ops[10].isFP, charBuf);
	}
	charBuf->append((char)8);
	nOps = 0;
	openPath = gTrue;
	break;
      default:
	nOps = 0;
	break;
      }
    }
  }

  if (top) {
    r2 = 4330;
    for (i = start; i < charBuf->getLength(); ++i) {
      byte = charBuf->getChar(i) ^ (r2 >> 8);
      charBuf->setChar(i, byte);
      r2 = (byte + r2) * 52845 + 22719;
    }
  }
}
