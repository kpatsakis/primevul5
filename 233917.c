ssh_poll_handle ssh_bind_get_poll(ssh_bind sshbind){
  if(sshbind->poll)
    return sshbind->poll;
  sshbind->poll=ssh_poll_new(sshbind->bindfd,POLLIN,
      ssh_bind_poll_callback,sshbind);
  return sshbind->poll;
}