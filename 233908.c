int ssh_bind_listen(ssh_bind sshbind) {
  const char *host;
  socket_t fd;
  int rc;

  if (ssh_init() < 0) {
    ssh_set_error(sshbind, SSH_FATAL, "ssh_init() failed");
    return -1;
  }

  rc = ssh_bind_import_keys(sshbind);
  if (rc != SSH_OK) {
    return SSH_ERROR;
  }

  if (sshbind->bindfd == SSH_INVALID_SOCKET) {
      host = sshbind->bindaddr;
      if (host == NULL) {
          host = "0.0.0.0";
      }

      fd = bind_socket(sshbind, host, sshbind->bindport);
      if (fd == SSH_INVALID_SOCKET) {
          ssh_key_free(sshbind->dsa);
          sshbind->dsa = NULL;
          ssh_key_free(sshbind->rsa);
          sshbind->rsa = NULL;
          return -1;
      }
      sshbind->bindfd = fd;

      if (listen(fd, 10) < 0) {
          ssh_set_error(sshbind, SSH_FATAL,
                  "Listening to socket %d: %s",
                  fd, strerror(errno));
          close(fd);
          ssh_key_free(sshbind->dsa);
          sshbind->dsa = NULL;
          ssh_key_free(sshbind->rsa);
          sshbind->rsa = NULL;
          return -1;
      }
  } else {
      SSH_LOG(SSH_LOG_INFO, "Using app-provided bind socket");
  }
  return 0;
}