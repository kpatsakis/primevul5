and_cclass(CClassNode* dest, CClassNode* cc, ScanEnv* env)
{
  OnigEncoding enc = env->enc;
  int r, not1, not2;
  BBuf *buf1, *buf2, *pbuf = 0;
  BitSetRef bsr1, bsr2;
  BitSet bs1, bs2;

  not1 = IS_NCCLASS_NOT(dest);
  bsr1 = dest->bs;
  buf1 = dest->mbuf;
  not2 = IS_NCCLASS_NOT(cc);
  bsr2 = cc->bs;
  buf2 = cc->mbuf;

  if (not1 != 0) {
    bitset_invert_to(bsr1, bs1);
    bsr1 = bs1;
  }
  if (not2 != 0) {
    bitset_invert_to(bsr2, bs2);
    bsr2 = bs2;
  }
  bitset_and(bsr1, bsr2);
  if (bsr1 != dest->bs) {
    bitset_copy(dest->bs, bsr1);
    bsr1 = dest->bs;
  }
  if (not1 != 0) {
    bitset_invert(dest->bs);
  }

  if (! ONIGENC_IS_SINGLEBYTE(enc)) {
    if (not1 != 0 && not2 != 0) {
      r = or_code_range_buf(enc, buf1, 0, buf2, 0, &pbuf, env);
    }
    else {
      r = and_code_range_buf(buf1, not1, buf2, not2, &pbuf, env);
      if (r == 0 && not1 != 0) {
	BBuf *tbuf = 0;
	r = not_code_range_buf(enc, pbuf, &tbuf, env);
	bbuf_free(pbuf);
	pbuf = tbuf;
      }
    }
    if (r != 0) {
	bbuf_free(pbuf);
	return r;
    }

    dest->mbuf = pbuf;
    bbuf_free(buf1);
    return r;
  }
  return 0;
}