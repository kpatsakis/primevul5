static sftp_attributes sftp_parse_attr_3(sftp_session sftp, ssh_buffer buf,
    int expectname) {
  ssh_string longname = NULL;
  ssh_string name = NULL;
  sftp_attributes attr;
  uint32_t flags = 0;
  int ok = 0;

  attr = malloc(sizeof(struct sftp_attributes_struct));
  if (attr == NULL) {
    ssh_set_error_oom(sftp->session);
    return NULL;
  }
  ZERO_STRUCTP(attr);

  /* This isn't really a loop, but it is like a try..catch.. */
  do {
    if (expectname) {
      if ((name = buffer_get_ssh_string(buf)) == NULL ||
          (attr->name = ssh_string_to_char(name)) == NULL) {
        break;
      }
      ssh_string_free(name);

      ssh_log(sftp->session, SSH_LOG_RARE, "Name: %s", attr->name);

      if ((longname=buffer_get_ssh_string(buf)) == NULL ||
          (attr->longname=ssh_string_to_char(longname)) == NULL) {
        break;
      }
      ssh_string_free(longname);

      /* Set owner and group if we talk to openssh and have the longname */
      if (ssh_get_openssh_version(sftp->session)) {
        attr->owner = sftp_parse_longname(attr->longname, SFTP_LONGNAME_OWNER);
        if (attr->owner == NULL) {
          break;
        }

        attr->group = sftp_parse_longname(attr->longname, SFTP_LONGNAME_GROUP);
        if (attr->group == NULL) {
          break;
        }
      }
    }

    if (buffer_get_u32(buf, &flags) != sizeof(uint32_t)) {
      break;
    }
    flags = ntohl(flags);
    attr->flags = flags;
    ssh_log(sftp->session, SSH_LOG_RARE,
        "Flags: %.8lx\n", (long unsigned int) flags);

    if (flags & SSH_FILEXFER_ATTR_SIZE) {
      if(buffer_get_u64(buf, &attr->size) != sizeof(uint64_t)) {
        break;
      }
      attr->size = ntohll(attr->size);
      ssh_log(sftp->session, SSH_LOG_RARE,
          "Size: %llu\n",
          (long long unsigned int) attr->size);
    }

    if (flags & SSH_FILEXFER_ATTR_UIDGID) {
      if (buffer_get_u32(buf, &attr->uid) != sizeof(uint32_t)) {
        break;
      }
      if (buffer_get_u32(buf, &attr->gid) != sizeof(uint32_t)) {
        break;
      }
      attr->uid = ntohl(attr->uid);
      attr->gid = ntohl(attr->gid);
    }

    if (flags & SSH_FILEXFER_ATTR_PERMISSIONS) {
      if (buffer_get_u32(buf, &attr->permissions) != sizeof(uint32_t)) {
        break;
      }
      attr->permissions = ntohl(attr->permissions);

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

    if (flags & SSH_FILEXFER_ATTR_ACMODTIME) {
      if (buffer_get_u32(buf, &attr->atime) != sizeof(uint32_t)) {
        break;
      }
      attr->atime = ntohl(attr->atime);
      if (buffer_get_u32(buf, &attr->mtime) != sizeof(uint32_t)) {
        break;
      }
      attr->mtime = ntohl(attr->mtime);
    }

    if (flags & SSH_FILEXFER_ATTR_EXTENDED) {
      if (buffer_get_u32(buf, &attr->extended_count) != sizeof(uint32_t)) {
        break;
      }

      attr->extended_count = ntohl(attr->extended_count);
      while (attr->extended_count &&
          (attr->extended_type = buffer_get_ssh_string(buf))
          && (attr->extended_data = buffer_get_ssh_string(buf))) {
        attr->extended_count--;
      }

      if (attr->extended_count) {
        break;
      }
    }
    ok = 1;
  } while (0);

  if (!ok) {
    /* break issued somewhere */
    ssh_string_free(name);
    ssh_string_free(longname);
    ssh_string_free(attr->extended_type);
    ssh_string_free(attr->extended_data);
    SAFE_FREE(attr->name);
    SAFE_FREE(attr->longname);
    SAFE_FREE(attr->owner);
    SAFE_FREE(attr->group);
    SAFE_FREE(attr);

    ssh_set_error(sftp->session, SSH_FATAL, "Invalid ATTR structure");

    return NULL;
  }

  /* everything went smoothly */
  return attr;
}