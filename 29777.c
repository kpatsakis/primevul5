int FoFiType1C::getOp(int pos, GBool charstring, GBool *ok) {
  static char nybChars[16] = "0123456789.ee -";
  Type1COp op;
  char buf[65];
  int b0, b1, nyb0, nyb1, x, i;

  b0 = getU8(pos++, ok);
  op.isNum = gTrue;
  op.isFP = gFalse;

  if (b0 == 28) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x8000) {
      x |= ~0xffff;
    }
    op.num = x;

  } else if (!charstring && b0 == 29) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x80000000) {
      x |= ~0xffffffff;
    }
    op.num = x;

  } else if (!charstring && b0 == 30) {
    i = 0;
    do {
      b1 = getU8(pos++, ok);
      nyb0 = b1 >> 4;
      nyb1 = b1 & 0x0f;
      if (nyb0 == 0xf) {
	break;
      }
      buf[i++] = nybChars[nyb0];
      if (i == 64) {
	break;
      }
      if (nyb0 == 0xc) {
	buf[i++] = '-';
      }
      if (i == 64) {
	break;
      }
      if (nyb1 == 0xf) {
	break;
      }
      buf[i++] = nybChars[nyb1];
      if (i == 64) {
	break;
      }
      if (nyb1 == 0xc) {
	buf[i++] = '-';
      }
    } while (i < 64);
    buf[i] = '\0';
    op.num = gatof(buf);
    op.isFP = gTrue;

  } else if (b0 >= 32 && b0 <= 246) {
    op.num = b0 - 139;

  } else if (b0 >= 247 && b0 <= 250) {
    op.num = ((b0 - 247) << 8) + getU8(pos++, ok) + 108;

  } else if (b0 >= 251 && b0 <= 254) {
    op.num = -((b0 - 251) << 8) - getU8(pos++, ok) - 108;

  } else if (charstring && b0 == 255) {
    x = getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    x = (x << 8) | getU8(pos++, ok);
    if (x & 0x80000000) {
      x |= ~0xffffffff;
    }
    op.num = (double)x / 65536.0;
    op.isFP = gTrue;

  } else if (b0 == 12) {
    op.isNum = gFalse;
    op.op = 0x0c00 + getU8(pos++, ok);

  } else {
    op.isNum = gFalse;
    op.op = b0;
  }

  if (nOps < 49) {
    ops[nOps++] = op;
  }

  return pos;
}
