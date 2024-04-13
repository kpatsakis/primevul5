int flb_gzip_uncompress(void *in_data, size_t in_len,
                        void **out_data, size_t *out_len)
{
    int status;
    uint8_t *p;
    void *out_buf;
    size_t out_size = 0;
    void *zip_data;
    size_t zip_len;
    unsigned char flg;
    unsigned int xlen, hcrc;
    unsigned int dlen, crc;
    mz_ulong crc_out;
    mz_stream stream;
    const unsigned char *start;

    /* Minimal length: header + crc32 */
    if (in_len < 18) {
        flb_error("[gzip] unexpected content length");
        return -1;
    }

    /* Magic bytes */
    p = in_data;
    if (p[0] != 0x1F || p[1] != 0x8B) {
        flb_error("[gzip] invalid magic bytes");
        return -1;
    }

    if (p[2] != 8) {
        flb_error("[gzip] invalid method");
        return -1;
    }

    /* Flag byte */
    flg = p[3];

    /* Reserved bits */
    if (flg & 0xE0) {
        flb_error("[gzip] invalid flag");
        return -1;
    }

    /* Skip base header of 10 bytes */
    start = p + FLB_GZIP_HEADER_OFFSET;

    /* Skip extra data if present */
    if (flg & FEXTRA) {
        xlen = read_le16(start);
        if (xlen > in_len - 12) {
            flb_error("[gzip] invalid gzip data");
            return -1;
        }
        start += xlen + 2;
    }

    /* Skip file name if present */
    if (flg & FNAME) {
        do {
            if (start - p >= in_len) {
                flb_error("[gzip] invalid gzip data (FNAME)");
                return -1;
            }
        } while (*start++);
    }

    /* Skip file comment if present */
    if (flg & FCOMMENT) {
        do {
            if (start - p >= in_len) {
                flb_error("[gzip] invalid gzip data (FCOMMENT)");
                return -1;
            }
        } while (*start++);
    }

    /* Check header crc if present */
    if (flg & FHCRC) {
        if (start - p > in_len - 2) {
            flb_error("[gzip] invalid gzip data (FHRC)");
            return -1;
        }

        hcrc = read_le16(start);
        crc = mz_crc32(MZ_CRC32_INIT, p, start - p) & 0x0000FFFF;
        if (hcrc != crc) {
            flb_error("[gzip] invalid gzip header CRC");
            return -1;
        }
        start += 2;
    }

    /* Get decompressed length */
    dlen = read_le32(&p[in_len - 4]);

    /* Get CRC32 checksum of original data */
    crc = read_le32(&p[in_len - 8]);

    /* Decompress data */
    if ((p + in_len) - p < 8) {
        flb_error("[gzip] invalid gzip CRC32 checksum");
        return -1;
    }

    /* Allocate outgoing buffer */
    out_buf = flb_malloc(dlen);
    if (!out_buf) {
        flb_errno();
        return -1;
    }
    out_size = dlen;

    /* Map zip content */
    zip_data = (uint8_t *) start;
    zip_len = (p + in_len) - start - 8;

    memset(&stream, 0, sizeof(stream));
    stream.next_in = zip_data;
    stream.avail_in = zip_len;
    stream.next_out = out_buf;
    stream.avail_out = out_size;

    status = mz_inflateInit2(&stream, -Z_DEFAULT_WINDOW_BITS);
    if (status != MZ_OK) {
        flb_free(out_buf);
        return -1;
    }

    status = mz_inflate(&stream, MZ_FINISH);
    if (status != MZ_STREAM_END) {
        mz_inflateEnd(&stream);
        flb_free(out_buf);
        return -1;
    }

    if (stream.total_out != dlen) {
        mz_inflateEnd(&stream);
        flb_free(out_buf);
        flb_error("[gzip] invalid gzip data size");
        return -1;
    }

    /* terminate the stream, it's not longer required */
    mz_inflateEnd(&stream);

    /* Validate message CRC vs inflated data CRC */
    crc_out = mz_crc32(MZ_CRC32_INIT, out_buf, dlen);
    if (crc_out != crc) {
        flb_free(out_buf);
        flb_error("[gzip] invalid GZip checksum (CRC32)");
        return -1;
    }

    /* set the uncompressed data */
    *out_len = dlen;
    *out_data = out_buf;

    return 0;
}