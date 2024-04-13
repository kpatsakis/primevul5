static PCRE2_SPTR bracketend(PCRE2_SPTR cc)
{
SLJIT_ASSERT((*cc >= OP_ASSERT && *cc <= OP_ASSERTBACK_NA) || (*cc >= OP_ONCE && *cc <= OP_SCOND));
do cc += GET(cc, 1); while (*cc == OP_ALT);
SLJIT_ASSERT(*cc >= OP_KET && *cc <= OP_KETRPOS);
cc += 1 + LINK_SIZE;
return cc;
}