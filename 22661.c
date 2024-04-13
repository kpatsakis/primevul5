utf16be_get_case_fold_codes_by_str(OnigCaseFoldType flag,
				   const OnigUChar* p, const OnigUChar* end,
				   OnigCaseFoldCodeItem items[],
				   OnigEncoding enc)
{
  return onigenc_unicode_get_case_fold_codes_by_str(enc,
						    flag, p, end, items);
}