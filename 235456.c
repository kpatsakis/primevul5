int SGX_CDECL main(int argc, char *argv[])
{
  (void)(argc);
  (void)(argv);

#if defined(_MSC_VER)
  if (query_sgx_status() < 0) {
    /* either SGX is disabled, or a reboot is required to enable SGX */
    printf("Enter a character before exit ...\n");
    getchar();
    return -1;
  }
#endif

  /* Initialize the enclave */
  if(initialize_enclave() < 0){
    printf("Enter a character before exit ...\n");
    getchar();
    return -1;
  }

  /* Destroy the enclave */
  sgx_destroy_enclave(global_eid);

  printf("Info: SampleEnclave successfully returned.\n");

  return 0;
}