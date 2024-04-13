int ssh_buffer_unpack_va(struct ssh_buffer_struct *buffer,
                         const char *format,
                         size_t argc,
                         va_list ap)
{
    int rc = SSH_ERROR;
    const char *p = format, *last;
    union {
        uint8_t *byte;
        uint16_t *word;
        uint32_t *dword;
        uint64_t *qword;
        ssh_string *string;
        char **cstring;
        bignum *bignum;
        void **data;
    } o;
    size_t len, rlen, max_len;
    ssh_string tmp_string = NULL;
    va_list ap_copy;
    size_t count;

    max_len = ssh_buffer_get_len(buffer);

    /* copy the argument list in case a rollback is needed */
    va_copy(ap_copy, ap);

    if (argc > 256) {
        rc = SSH_ERROR;
        goto cleanup;
    }

    for (count = 0; *p != '\0'; p++, count++) {
        /* Invalid number of arguments passed */
        if (count > argc) {
            rc = SSH_ERROR;
            goto cleanup;
        }

        rc = SSH_ERROR;
        switch (*p) {
        case 'b':
            o.byte = va_arg(ap, uint8_t *);
            rlen = ssh_buffer_get_u8(buffer, o.byte);
            rc = rlen==1 ? SSH_OK : SSH_ERROR;
            break;
        case 'w':
            o.word = va_arg(ap,  uint16_t *);
            rlen = ssh_buffer_get_data(buffer, o.word, sizeof(uint16_t));
            if (rlen == 2) {
                *o.word = ntohs(*o.word);
                rc = SSH_OK;
            }
            break;
        case 'd':
            o.dword = va_arg(ap, uint32_t *);
            rlen = ssh_buffer_get_u32(buffer, o.dword);
            if (rlen == 4) {
                *o.dword = ntohl(*o.dword);
                rc = SSH_OK;
            }
            break;
        case 'q':
            o.qword = va_arg(ap, uint64_t*);
            rlen = ssh_buffer_get_u64(buffer, o.qword);
            if (rlen == 8) {
                *o.qword = ntohll(*o.qword);
                rc = SSH_OK;
            }
            break;
        case 'B':
            o.bignum = va_arg(ap, bignum *);
            *o.bignum = NULL;
            tmp_string = ssh_buffer_get_ssh_string(buffer);
            if (tmp_string == NULL) {
                break;
            }
            *o.bignum = ssh_make_string_bn(tmp_string);
            ssh_string_burn(tmp_string);
            SSH_STRING_FREE(tmp_string);
            rc = (*o.bignum != NULL) ? SSH_OK : SSH_ERROR;
            break;
        case 'S':
            o.string = va_arg(ap, ssh_string *);
            *o.string = ssh_buffer_get_ssh_string(buffer);
            rc = *o.string != NULL ? SSH_OK : SSH_ERROR;
            o.string = NULL;
            break;
        case 's': {
            uint32_t u32len = 0;

            o.cstring = va_arg(ap, char **);
            *o.cstring = NULL;
            rlen = ssh_buffer_get_u32(buffer, &u32len);
            if (rlen != 4){
                break;
            }
            len = ntohl(u32len);
            if (len > max_len - 1) {
                break;
            }

            rc = ssh_buffer_validate_length(buffer, len);
            if (rc != SSH_OK) {
                break;
            }

            *o.cstring = malloc(len + 1);
            if (*o.cstring == NULL){
                rc = SSH_ERROR;
                break;
            }
            rlen = ssh_buffer_get_data(buffer, *o.cstring, len);
            if (rlen != len){
                SAFE_FREE(*o.cstring);
                rc = SSH_ERROR;
                break;
            }
            (*o.cstring)[len] = '\0';
            o.cstring = NULL;
            rc = SSH_OK;
            break;
        }
        case 'P':
            len = va_arg(ap, size_t);
            if (len > max_len - 1) {
                rc = SSH_ERROR;
                break;
            }

            rc = ssh_buffer_validate_length(buffer, len);
            if (rc != SSH_OK) {
                break;
            }

            o.data = va_arg(ap, void **);
            count++;

            *o.data = malloc(len);
            if(*o.data == NULL){
                rc = SSH_ERROR;
                break;
            }
            rlen = ssh_buffer_get_data(buffer, *o.data, len);
            if (rlen != len){
                SAFE_FREE(*o.data);
                rc = SSH_ERROR;
                break;
            }
            o.data = NULL;
            rc = SSH_OK;
            break;
        default:
            SSH_LOG(SSH_LOG_WARN, "Invalid buffer format %c", *p);
        }
        if (rc != SSH_OK) {
            break;
        }
    }

    if (argc != count) {
        rc = SSH_ERROR;
    }

cleanup:
    if (rc != SSH_ERROR){
        /* Check if our canary is intact, if not something really bad happened */
        uint32_t canary = va_arg(ap, uint32_t);
        if (canary != SSH_BUFFER_PACK_END){
            abort();
        }
    }

    if (rc != SSH_OK){
        /* Reset the format string and erase everything that was allocated */
        last = p;
        for(p=format;p<last;++p){
            switch(*p){
            case 'b':
                o.byte = va_arg(ap_copy, uint8_t *);
                if (buffer->secure) {
                    explicit_bzero(o.byte, sizeof(uint8_t));
                    break;
                }
                break;
            case 'w':
                o.word = va_arg(ap_copy, uint16_t *);
                if (buffer->secure) {
                    explicit_bzero(o.word, sizeof(uint16_t));
                    break;
                }
                break;
            case 'd':
                o.dword = va_arg(ap_copy, uint32_t *);
                if (buffer->secure) {
                    explicit_bzero(o.dword, sizeof(uint32_t));
                    break;
                }
                break;
            case 'q':
                o.qword = va_arg(ap_copy, uint64_t *);
                if (buffer->secure) {
                    explicit_bzero(o.qword, sizeof(uint64_t));
                    break;
                }
                break;
            case 'B':
                o.bignum = va_arg(ap_copy, bignum *);
                bignum_safe_free(*o.bignum);
                break;
            case 'S':
                o.string = va_arg(ap_copy, ssh_string *);
                if (buffer->secure) {
                    ssh_string_burn(*o.string);
                }
                SAFE_FREE(*o.string);
                break;
            case 's':
                o.cstring = va_arg(ap_copy, char **);
                if (buffer->secure) {
                    explicit_bzero(*o.cstring, strlen(*o.cstring));
                }
                SAFE_FREE(*o.cstring);
                break;
            case 'P':
                len = va_arg(ap_copy, size_t);
                o.data = va_arg(ap_copy, void **);
                if (buffer->secure) {
                    explicit_bzero(*o.data, len);
                }
                SAFE_FREE(*o.data);
                break;
            default:
                (void)va_arg(ap_copy, void *);
                break;
            }
        }
    }
    va_end(ap_copy);

    return rc;
}