static int aa_read_header(AVFormatContext *s)
{
    int i, j, idx, largest_idx = -1;
    uint32_t nkey, nval, toc_size, npairs, header_seed = 0, start;
    char key[128], val[128], codec_name[64] = {0};
    uint8_t output[24], dst[8], src[8];
    int64_t largest_size = -1, current_size = -1, chapter_pos;
    struct toc_entry {
        uint32_t offset;
        uint32_t size;
    } TOC[MAX_TOC_ENTRIES];
    uint32_t header_key_part[4];
    uint8_t header_key[16] = {0};
    AADemuxContext *c = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st;

    /* parse .aa header */
    avio_skip(pb, 4); // file size
    avio_skip(pb, 4); // magic string
    toc_size = avio_rb32(pb); // TOC size
    avio_skip(pb, 4); // unidentified integer
    if (toc_size > MAX_TOC_ENTRIES)
        return AVERROR_INVALIDDATA;
    for (i = 0; i < toc_size; i++) { // read TOC
        avio_skip(pb, 4); // TOC entry index
        TOC[i].offset = avio_rb32(pb); // block offset
        TOC[i].size = avio_rb32(pb); // block size
    }
    avio_skip(pb, 24); // header termination block (ignored)
    npairs = avio_rb32(pb); // read dictionary entries
    if (npairs > MAX_DICTIONARY_ENTRIES)
        return AVERROR_INVALIDDATA;
    for (i = 0; i < npairs; i++) {
        memset(val, 0, sizeof(val));
        memset(key, 0, sizeof(key));
        avio_skip(pb, 1); // unidentified integer
        nkey = avio_rb32(pb); // key string length
        nval = avio_rb32(pb); // value string length
        avio_get_str(pb, nkey, key, sizeof(key));
        avio_get_str(pb, nval, val, sizeof(val));
        if (!strcmp(key, "codec")) {
            av_log(s, AV_LOG_DEBUG, "Codec is <%s>\n", val);
            strncpy(codec_name, val, sizeof(codec_name) - 1);
        } else if (!strcmp(key, "HeaderSeed")) {
            av_log(s, AV_LOG_DEBUG, "HeaderSeed is <%s>\n", val);
            header_seed = atoi(val);
        } else if (!strcmp(key, "HeaderKey")) { // this looks like "1234567890 1234567890 1234567890 1234567890"
            av_log(s, AV_LOG_DEBUG, "HeaderKey is <%s>\n", val);
            sscanf(val, "%"SCNu32"%"SCNu32"%"SCNu32"%"SCNu32,
                   &header_key_part[0], &header_key_part[1], &header_key_part[2], &header_key_part[3]);
            for (idx = 0; idx < 4; idx++) {
                AV_WB32(&header_key[idx * 4], header_key_part[idx]); // convert each part to BE!
            }
            av_log(s, AV_LOG_DEBUG, "Processed HeaderKey is ");
            for (i = 0; i < 16; i++)
                av_log(s, AV_LOG_DEBUG, "%02x", header_key[i]);
            av_log(s, AV_LOG_DEBUG, "\n");
        } else {
            av_dict_set(&s->metadata, key, val, 0);
        }
    }

    /* verify fixed key */
    if (c->aa_fixed_key_len != 16) {
        av_log(s, AV_LOG_ERROR, "aa_fixed_key value needs to be 16 bytes!\n");
        return AVERROR(EINVAL);
    }

    /* verify codec */
    if ((c->codec_second_size = get_second_size(codec_name)) == -1) {
        av_log(s, AV_LOG_ERROR, "unknown codec <%s>!\n", codec_name);
        return AVERROR(EINVAL);
    }

    /* decryption key derivation */
    c->tea_ctx = av_tea_alloc();
    if (!c->tea_ctx)
        return AVERROR(ENOMEM);
    av_tea_init(c->tea_ctx, c->aa_fixed_key, 16);
    output[0] = output[1] = 0; // purely for padding purposes
    memcpy(output + 2, header_key, 16);
    idx = 0;
    for (i = 0; i < 3; i++) { // TEA CBC with weird mixed endianness
        AV_WB32(src, header_seed);
        AV_WB32(src + 4, header_seed + 1);
        header_seed += 2;
        av_tea_crypt(c->tea_ctx, dst, src, 1, NULL, 0); // TEA ECB encrypt
        for (j = 0; j < TEA_BLOCK_SIZE && idx < 18; j+=1, idx+=1) {
            output[idx] = output[idx] ^ dst[j];
        }
    }
    memcpy(c->file_key, output + 2, 16); // skip first 2 bytes of output
    av_log(s, AV_LOG_DEBUG, "File key is ");
    for (i = 0; i < 16; i++)
        av_log(s, AV_LOG_DEBUG, "%02x", c->file_key[i]);
    av_log(s, AV_LOG_DEBUG, "\n");

    /* decoder setup */
    st = avformat_new_stream(s, NULL);
    if (!st) {
        av_freep(&c->tea_ctx);
        return AVERROR(ENOMEM);
    }
    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    if (!strcmp(codec_name, "mp332")) {
        st->codecpar->codec_id = AV_CODEC_ID_MP3;
        st->codecpar->sample_rate = 22050;
        st->need_parsing = AVSTREAM_PARSE_FULL_RAW;
        avpriv_set_pts_info(st, 64, 8, 32000 * TIMEPREC);
        // encoded audio frame is MP3_FRAME_SIZE bytes (+1 with padding, unlikely)
    } else if (!strcmp(codec_name, "acelp85")) {
        st->codecpar->codec_id = AV_CODEC_ID_SIPR;
        st->codecpar->block_align = 19;
        st->codecpar->channels = 1;
        st->codecpar->sample_rate = 8500;
        st->codecpar->bit_rate = 8500;
        st->need_parsing = AVSTREAM_PARSE_FULL_RAW;
        avpriv_set_pts_info(st, 64, 8, 8500 * TIMEPREC);
    } else if (!strcmp(codec_name, "acelp16")) {
        st->codecpar->codec_id = AV_CODEC_ID_SIPR;
        st->codecpar->block_align = 20;
        st->codecpar->channels = 1;
        st->codecpar->sample_rate = 16000;
        st->codecpar->bit_rate = 16000;
        st->need_parsing = AVSTREAM_PARSE_FULL_RAW;
        avpriv_set_pts_info(st, 64, 8, 16000 * TIMEPREC);
    }

    /* determine, and jump to audio start offset */
    for (i = 1; i < toc_size; i++) { // skip the first entry!
        current_size = TOC[i].size;
        if (current_size > largest_size) {
            largest_idx = i;
            largest_size = current_size;
        }
    }
    start = TOC[largest_idx].offset;
    avio_seek(pb, start, SEEK_SET);

    // extract chapter positions. since all formats have constant bit rate, use it
    // as time base in bytes/s, for easy stream position <-> timestamp conversion
    st->start_time = 0;
    c->content_start = start;
    c->content_end = start + largest_size;

    while ((chapter_pos = avio_tell(pb)) >= 0 && chapter_pos < c->content_end) {
        int chapter_idx = s->nb_chapters;
        uint32_t chapter_size = avio_rb32(pb);
        if (chapter_size == 0) break;
        chapter_pos -= start + CHAPTER_HEADER_SIZE * chapter_idx;
        avio_skip(pb, 4 + chapter_size);
        if (!avpriv_new_chapter(s, chapter_idx, st->time_base,
            chapter_pos * TIMEPREC, (chapter_pos + chapter_size) * TIMEPREC, NULL))
                return AVERROR(ENOMEM);
    }

    st->duration = (largest_size - CHAPTER_HEADER_SIZE * s->nb_chapters) * TIMEPREC;

    ff_update_cur_dts(s, st, 0);
    avio_seek(pb, start, SEEK_SET);
    c->current_chapter_size = 0;
    c->seek_offset = 0;

    return 0;
}