int ssh_buffer_pack_va(struct ssh_buffer_struct *buffer,
                       const char *format,
                       size_t argc,
                       va_list ap)
{
    int rc = SSH_ERROR;
    const char *p;
    union {
        uint8_t byte;
        uint16_t word;
        uint32_t dword;
        uint64_t qword;
        ssh_string string;
        void *data;
    } o;
    char *cstring;
    bignum b;
    size_t len;
    size_t count;

    if (argc > 256) {
        return SSH_ERROR;
    }

    for (p = format, count = 0; *p != '\0'; p++, count++) {
        /* Invalid number of arguments passed */
        if (count > argc) {
            return SSH_ERROR;
        }

        switch(*p) {
        case 'b':
            o.byte = (uint8_t)va_arg(ap, unsigned int);
            rc = ssh_buffer_add_u8(buffer, o.byte);
            break;
        case 'w':
            o.word = (uint16_t)va_arg(ap, unsigned int);
            o.word = htons(o.word);
            rc = ssh_buffer_add_u16(buffer, o.word);
            break;
        case 'd':
            o.dword = va_arg(ap, uint32_t);
            o.dword = htonl(o.dword);
            rc = ssh_buffer_add_u32(buffer, o.dword);
            break;
        case 'q':
            o.qword = va_arg(ap, uint64_t);
            o.qword = htonll(o.qword);
            rc = ssh_buffer_add_u64(buffer, o.qword);
            break;
        case 'S':
            o.string = va_arg(ap, ssh_string);
            rc = ssh_buffer_add_ssh_string(buffer, o.string);
            o.string = NULL;
            break;
        case 's':
            cstring = va_arg(ap, char *);
            len = strlen(cstring);
            rc = ssh_buffer_add_u32(buffer, htonl(len));
            if (rc == SSH_OK){
                rc = ssh_buffer_add_data(buffer, cstring, len);
            }
            cstring = NULL;
            break;
        case 'P':
            len = va_arg(ap, size_t);

            o.data = va_arg(ap, void *);
            count++; /* increase argument count */

            rc = ssh_buffer_add_data(buffer, o.data, len);
            o.data = NULL;
            break;
        case 'B':
            b = va_arg(ap, bignum);
            o.string = ssh_make_bignum_string(b);
            if(o.string == NULL){
                rc = SSH_ERROR;
                break;
            }
            rc = ssh_buffer_add_ssh_string(buffer, o.string);
            SAFE_FREE(o.string);
            break;
        case 't':
            cstring = va_arg(ap, char *);
            len = strlen(cstring);
            rc = ssh_buffer_add_data(buffer, cstring, len);
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
        /* Check if our canary is intact, if not something really bad happened */
        uint32_t canary = va_arg(ap, uint32_t);
        if (canary != SSH_BUFFER_PACK_END) {
            abort();
        }
    }
    return rc;
}