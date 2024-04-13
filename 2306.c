static int no_alternatives(PCRE2_SPTR cc)
{
int count = 0;
SLJIT_ASSERT((*cc >= OP_ASSERT && *cc <= OP_ASSERTBACK_NA) || (*cc >= OP_ONCE && *cc <= OP_SCOND));
do
  {
  cc += GET(cc, 1);
  count++;
  }
while (*cc == OP_ALT);
SLJIT_ASSERT(*cc >= OP_KET && *cc <= OP_KETRPOS);
return count;
}