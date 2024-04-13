static int ssh_buffer_pack_allocate_va(struct ssh_buffer_struct *buffer,
                                       const char *format,
                                       size_t argc,
                                       va_list ap)
{
    const char *p = NULL;
    ssh_string string = NULL;
    char *cstring = NULL;
    size_t needed_size = 0;
    size_t len;
    size_t count;
    int rc = SSH_OK;

    for (p = format, count = 0; *p != '\0'; p++, count++) {
        /* Invalid number of arguments passed */
        if (count > argc) {
            return SSH_ERROR;
        }

        switch(*p) {
        case 'b':
            va_arg(ap, unsigned int);
            needed_size += sizeof(uint8_t);
            break;
        case 'w':
            va_arg(ap, unsigned int);
            needed_size += sizeof(uint16_t);
            break;
        case 'd':
            va_arg(ap, uint32_t);
            needed_size += sizeof(uint32_t);
            break;
        case 'q':
            va_arg(ap, uint64_t);
            needed_size += sizeof(uint64_t);
            break;
        case 'S':
            string = va_arg(ap, ssh_string);
            needed_size += 4 + ssh_string_len(string);
            string = NULL;
            break;
        case 's':
            cstring = va_arg(ap, char *);
            needed_size += sizeof(uint32_t) + strlen(cstring);
            cstring = NULL;
            break;
        case 'P':
            len = va_arg(ap, size_t);
            needed_size += len;
            va_arg(ap, void *);
            count++; /* increase argument count */
            break;
        case 'B':
            va_arg(ap, bignum);
            /*
             * Use a fixed size for a bignum
             * (they should normaly be around 32)
             */
            needed_size += 64;
            break;
        case 't':
            cstring = va_arg(ap, char *);
            needed_size += strlen(cstring);
            cstring = NULL;
            break;
        default:
            SSH_LOG(SSH_LOG_WARN, "Invalid buffer format %c", *p);
            rc = SSH_ERROR;
        }
        if (rc != SSH_OK){
            break;
        }
    }

    if (argc != count) {
        return SSH_ERROR;
    }

    if (rc != SSH_ERROR){
        /*
         * Check if our canary is intact, if not, something really bad happened.
         */
        uint32_t canary = va_arg(ap, uint32_t);
        if (canary != SSH_BUFFER_PACK_END) {
            abort();
        }
    }

    rc = ssh_buffer_allocate_size(buffer, needed_size);
    if (rc != 0) {
        return SSH_ERROR;
    }

    return SSH_OK;
}