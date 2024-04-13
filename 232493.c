run_selftests (int algo, int extended, selftest_report_func_t report)
{
  (void)extended;

  if (algo != GCRY_PK_ECC)
    return GPG_ERR_PUBKEY_ALGO;

  return selftests_ecdsa (report);
}