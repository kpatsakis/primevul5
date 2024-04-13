int buffer_add_attributes(ssh_buffer buffer, sftp_attributes attr) {
  uint32_t flags = (attr ? attr->flags : 0);

  flags &= (SSH_FILEXFER_ATTR_SIZE | SSH_FILEXFER_ATTR_UIDGID |
      SSH_FILEXFER_ATTR_PERMISSIONS | SSH_FILEXFER_ATTR_ACMODTIME);

  if (buffer_add_u32(buffer, htonl(flags)) < 0) {
    return -1;
  }

  if (attr) {
    if (flags & SSH_FILEXFER_ATTR_SIZE) {
      if (buffer_add_u64(buffer, htonll(attr->size)) < 0) {
        return -1;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_UIDGID) {
      if (buffer_add_u32(buffer,htonl(attr->uid)) < 0 ||
          buffer_add_u32(buffer,htonl(attr->gid)) < 0) {
        return -1;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
      if (buffer_add_u32(buffer, htonl(attr->permissions)) < 0) {
        return -1;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_ACMODTIME) {
      if (buffer_add_u32(buffer, htonl(attr->atime)) < 0 ||
          buffer_add_u32(buffer, htonl(attr->mtime)) < 0) {
        return -1;
      }
    }
  }

  return 0;
}