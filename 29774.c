void FoFiType1C::getIndex(int pos, Type1CIndex *idx, GBool *ok) {
  idx->pos = pos;
  idx->len = getU16BE(pos, ok);
  if (idx->len == 0) {
    idx->offSize = 0;
    idx->startPos = idx->endPos = pos + 2;
  } else {
    idx->offSize = getU8(pos + 2, ok);
    if (idx->offSize < 1 || idx->offSize > 4) {
      *ok = gFalse;
    }
    idx->startPos = pos + 3 + (idx->len + 1) * idx->offSize - 1;
    if (idx->startPos < 0 || idx->startPos >= len) {
      *ok = gFalse;
    }
    idx->endPos = idx->startPos + getUVarBE(pos + 3 + idx->len * idx->offSize,
					    idx->offSize, ok);
    if (idx->endPos < idx->startPos || idx->endPos > len) {
      *ok = gFalse;
    }
  }
}
