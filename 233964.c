void ssh_bind_free(ssh_bind sshbind){
  int i;

  if (sshbind == NULL) {
    return;
  }

  if (sshbind->bindfd >= 0) {
#ifdef _WIN32
    closesocket(sshbind->bindfd);
#else
    close(sshbind->bindfd);
#endif
  }
  sshbind->bindfd = SSH_INVALID_SOCKET;

  /* options */
  SAFE_FREE(sshbind->banner);
  SAFE_FREE(sshbind->bindaddr);

  SAFE_FREE(sshbind->dsakey);
  SAFE_FREE(sshbind->rsakey);
  SAFE_FREE(sshbind->ecdsakey);

  ssh_key_free(sshbind->dsa);
  sshbind->dsa = NULL;
  ssh_key_free(sshbind->rsa);
  sshbind->rsa = NULL;
  ssh_key_free(sshbind->ecdsa);
  sshbind->ecdsa = NULL;

  for (i = 0; i < 10; i++) {
    if (sshbind->wanted_methods[i]) {
      SAFE_FREE(sshbind->wanted_methods[i]);
    }
  }

  SAFE_FREE(sshbind);
}