void FoFiType1C::getIndexVal(Type1CIndex *idx, int i,
			     Type1CIndexVal *val, GBool *ok) {
  int pos0, pos1;

  if (i < 0 || i >= idx->len) {
    *ok = gFalse;
    return;
  }
  pos0 = idx->startPos + getUVarBE(idx->pos + 3 + i * idx->offSize,
				   idx->offSize, ok);
  pos1 = idx->startPos + getUVarBE(idx->pos + 3 + (i + 1) * idx->offSize,
				   idx->offSize, ok);
  if (pos0 < idx->startPos || pos0 > idx->endPos ||
      pos1 <= idx->startPos || pos1 > idx->endPos ||
      pos1 < pos0) {
    *ok = gFalse;
  }
  val->pos = pos0;
  val->len = pos1 - pos0;
}
