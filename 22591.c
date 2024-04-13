apply_all_case_fold(OnigCaseFoldType flag,
		    OnigApplyAllCaseFoldFunc f, void* arg,
		    OnigEncoding enc ARG_UNUSED)
{
  return onigenc_apply_all_case_fold_with_map(
             sizeof(CaseFoldMap)/sizeof(OnigPairCaseFoldCodes), CaseFoldMap, 1,
             flag, f, arg);
}