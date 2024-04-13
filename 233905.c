static int ssh_bind_import_keys(ssh_bind sshbind) {
  int rc;

  if (sshbind->ecdsakey == NULL &&
      sshbind->dsakey == NULL &&
      sshbind->rsakey == NULL) {
      ssh_set_error(sshbind, SSH_FATAL,
                    "ECDSA, DSA, or RSA host key file must be set");
      return SSH_ERROR;
  }

#ifdef HAVE_ECC
  if (sshbind->ecdsa == NULL && sshbind->ecdsakey != NULL) {
      rc = ssh_pki_import_privkey_file(sshbind->ecdsakey,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &sshbind->ecdsa);
      if (rc == SSH_ERROR || rc == SSH_EOF) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "Failed to import private ECDSA host key");
          return SSH_ERROR;
      }

      if (ssh_key_type(sshbind->ecdsa) != SSH_KEYTYPE_ECDSA) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "The ECDSA host key has the wrong type");
          ssh_key_free(sshbind->ecdsa);
          sshbind->ecdsa = NULL;
          return SSH_ERROR;
      }
  }
#endif

  if (sshbind->dsa == NULL && sshbind->dsakey != NULL) {
      rc = ssh_pki_import_privkey_file(sshbind->dsakey,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &sshbind->dsa);
      if (rc == SSH_ERROR || rc == SSH_EOF) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "Failed to import private DSA host key");
          return SSH_ERROR;
      }

      if (ssh_key_type(sshbind->dsa) != SSH_KEYTYPE_DSS) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "The DSA host key has the wrong type: %d",
                  ssh_key_type(sshbind->dsa));
          ssh_key_free(sshbind->dsa);
          sshbind->dsa = NULL;
          return SSH_ERROR;
      }
  }

  if (sshbind->rsa == NULL && sshbind->rsakey != NULL) {
      rc = ssh_pki_import_privkey_file(sshbind->rsakey,
                                       NULL,
                                       NULL,
                                       NULL,
                                       &sshbind->rsa);
      if (rc == SSH_ERROR || rc == SSH_EOF) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "Failed to import private RSA host key");
          return SSH_ERROR;
      }

      if (ssh_key_type(sshbind->rsa) != SSH_KEYTYPE_RSA &&
          ssh_key_type(sshbind->rsa) != SSH_KEYTYPE_RSA1) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "The RSA host key has the wrong type");
          ssh_key_free(sshbind->rsa);
          sshbind->rsa = NULL;
          return SSH_ERROR;
      }
  }

  return SSH_OK;
}