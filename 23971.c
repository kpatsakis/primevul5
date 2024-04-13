static sftp_attributes sftp_parse_attr_4(sftp_session sftp, ssh_buffer buf,
    int expectnames) {
  sftp_attributes attr;
  ssh_string owner = NULL;
  ssh_string group = NULL;
  uint32_t flags = 0;
  int ok = 0;

  /* unused member variable */
  (void) expectnames;

  attr = malloc(sizeof(struct sftp_attributes_struct));
  if (attr == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(attr);

  /* This isn't really a loop, but it is like a try..catch.. */
  do {
    if (buffer_get_u32(buf, &flags) != 4) {
      break;
    }

    flags = ntohl(flags);
    attr->flags = flags;

    if (flags & SSH_FILEXFER_ATTR_SIZE) {
      if (buffer_get_u64(buf, &attr->size) != 8) {
        break;
      }
      attr->size = ntohll(attr->size);
    }

    if (flags & SSH_FILEXFER_ATTR_OWNERGROUP) {
      if((owner = buffer_get_ssh_string(buf)) == NULL ||
        (attr->owner = ssh_string_to_char(owner)) == NULL) {
        break;
      }
      if ((group = buffer_get_ssh_string(buf)) == NULL ||
        (attr->group = ssh_string_to_char(group)) == NULL) {
        break;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
      if (buffer_get_u32(buf, &attr->permissions) != 4) {
        break;
      }
      attr->permissions = ntohl(attr->permissions);

      /* FIXME on windows! */
      switch (attr->permissions & S_IFMT) {
        case S_IFSOCK:
        case S_IFBLK:
        case S_IFCHR:
        case S_IFIFO:
          attr->type = SSH_FILEXFER_TYPE_SPECIAL;
          break;
        case S_IFLNK:
          attr->type = SSH_FILEXFER_TYPE_SYMLINK;
          break;
        case S_IFREG:
          attr->type = SSH_FILEXFER_TYPE_REGULAR;
          break;
        case S_IFDIR:
          attr->type = SSH_FILEXFER_TYPE_DIRECTORY;
          break;
        default:
          attr->type = SSH_FILEXFER_TYPE_UNKNOWN;
          break;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_ACCESSTIME) {
      if (buffer_get_u64(buf, &attr->atime64) != 8) {
        break;
      }
      attr->atime64 = ntohll(attr->atime64);
    }

    if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
      if (buffer_get_u32(buf, &attr->atime_nseconds) != 4) {
        break;
      }
      attr->atime_nseconds = ntohl(attr->atime_nseconds);
    }

    if (flags & SSH_FILEXFER_ATTR_CREATETIME) {
      if (buffer_get_u64(buf, &attr->createtime) != 8) {
        break;
      }
      attr->createtime = ntohll(attr->createtime);
    }

    if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
      if (buffer_get_u32(buf, &attr->createtime_nseconds) != 4) {
        break;
      }
      attr->createtime_nseconds = ntohl(attr->createtime_nseconds);
    }

    if (flags & SSH_FILEXFER_ATTR_MODIFYTIME) {
      if (buffer_get_u64(buf, &attr->mtime64) != 8) {
        break;
      }
      attr->mtime64 = ntohll(attr->mtime64);
    }

    if (flags & SSH_FILEXFER_ATTR_SUBSECOND_TIMES) {
      if (buffer_get_u32(buf, &attr->mtime_nseconds) != 4) {
        break;
      }
      attr->mtime_nseconds = ntohl(attr->mtime_nseconds);
    }

    if (flags & SSH_FILEXFER_ATTR_ACL) {
      if ((attr->acl = buffer_get_ssh_string(buf)) == NULL) {
        break;
      }
    }

    if (flags & SSH_FILEXFER_ATTR_EXTENDED) {
      if (buffer_get_u32(buf,&attr->extended_count) != 4) {
        break;
      }
      attr->extended_count = ntohl(attr->extended_count);

      while(attr->extended_count &&
          (attr->extended_type = buffer_get_ssh_string(buf)) &&
          (attr->extended_data = buffer_get_ssh_string(buf))){
        attr->extended_count--;
      }

      if (attr->extended_count) {
        break;
      }
    }
    ok = 1;
  } while (0);

  if (ok == 0) {
    /* break issued somewhere */
    ssh_string_free(owner);
    ssh_string_free(group);
    ssh_string_free(attr->acl);
    ssh_string_free(attr->extended_type);
    ssh_string_free(attr->extended_data);
    SAFE_FREE(attr->owner);
    SAFE_FREE(attr->group);
    SAFE_FREE(attr);

    ssh_set_error(sftp->session, SSH_FATAL, "Invalid ATTR structure");

    return NULL;
  }

  return attr;
}