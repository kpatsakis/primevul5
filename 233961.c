static int ssh_bind_poll_callback(ssh_poll_handle sshpoll,
    socket_t fd, int revents, void *user){
  ssh_bind sshbind=(ssh_bind)user;
  (void)sshpoll;
  (void)fd;

  if(revents & POLLIN){
    /* new incoming connection */
    if(ssh_callbacks_exists(sshbind->bind_callbacks,incoming_connection)){
      sshbind->bind_callbacks->incoming_connection(sshbind,
          sshbind->bind_callbacks_userdata);
    }
  }
  return 0;
}