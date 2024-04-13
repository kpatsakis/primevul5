static int realloc_buffer(struct ssh_buffer_struct *buffer, size_t needed)
{
    size_t smallest = 1;
    uint8_t *new = NULL;

    buffer_verify(buffer);

    /* Find the smallest power of two which is greater or equal to needed */
    while(smallest <= needed) {
        if (smallest == 0) {
            return -1;
        }
        smallest <<= 1;
    }
    needed = smallest;

    if (needed > BUFFER_SIZE_MAX) {
        return -1;
    }

    if (buffer->secure) {
        new = malloc(needed);
        if (new == NULL) {
            return -1;
        }
        memcpy(new, buffer->data, buffer->used);
        explicit_bzero(buffer->data, buffer->used);
        SAFE_FREE(buffer->data);
    } else {
        new = realloc(buffer->data, needed);
        if (new == NULL) {
            return -1;
        }
    }
    buffer->data = new;
    buffer->allocated = needed;

    buffer_verify(buffer);
    return 0;
}