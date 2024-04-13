select_str_opcode(int mb_len, OnigDistance str_len, int ignore_case)
{
  int op;

  if (ignore_case) {
    switch (str_len) {
    case 1:  op = OP_EXACT1_IC; break;
    default: op = OP_EXACTN_IC; break;
    }
  }
  else {
    switch (mb_len) {
    case 1:
      switch (str_len) {
      case 1:  op = OP_EXACT1; break;
      case 2:  op = OP_EXACT2; break;
      case 3:  op = OP_EXACT3; break;
      case 4:  op = OP_EXACT4; break;
      case 5:  op = OP_EXACT5; break;
      default: op = OP_EXACTN; break;
      }
      break;

    case 2:
      switch (str_len) {
      case 1:  op = OP_EXACTMB2N1; break;
      case 2:  op = OP_EXACTMB2N2; break;
      case 3:  op = OP_EXACTMB2N3; break;
      default: op = OP_EXACTMB2N;  break;
      }
      break;

    case 3:
      op = OP_EXACTMB3N;
      break;

    default:
      op = OP_EXACTMBN;
      break;
    }
  }
  return op;
}