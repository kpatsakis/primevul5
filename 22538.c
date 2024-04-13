onigenc_ascii_apply_all_case_fold(OnigCaseFoldType flag ARG_UNUSED,
				  OnigApplyAllCaseFoldFunc f, void* arg,
				  OnigEncoding enc ARG_UNUSED)
{
  OnigCodePoint code;
  int i, r;

  for (i = 0;
       i < (int )(sizeof(OnigAsciiLowerMap)/sizeof(OnigPairCaseFoldCodes));
       i++) {
    code = OnigAsciiLowerMap[i].to;
    r = (*f)(OnigAsciiLowerMap[i].from, &code, 1, arg);
    if (r != 0) return r;

    code = OnigAsciiLowerMap[i].from;
    r = (*f)(OnigAsciiLowerMap[i].to, &code, 1, arg);
    if (r != 0) return r;
  }

  return 0;
}