static av_cold int vqa_decode_init(AVCodecContext *avctx)
{
    VqaContext *s = avctx->priv_data;
    int i, j, codebook_index, ret;

    s->avctx = avctx;
    avctx->pix_fmt = AV_PIX_FMT_PAL8;

    /* make sure the extradata made it */
    if (s->avctx->extradata_size != VQA_HEADER_SIZE) {
        av_log(s->avctx, AV_LOG_ERROR, "expected extradata size of %d\n", VQA_HEADER_SIZE);
        return AVERROR(EINVAL);
    }

    /* load up the VQA parameters from the header */
    s->vqa_version = s->avctx->extradata[0];
    switch (s->vqa_version) {
    case 1:
    case 2:
        break;
    case 3:
        avpriv_report_missing_feature(avctx, "VQA Version %d", s->vqa_version);
        return AVERROR_PATCHWELCOME;
    default:
        avpriv_request_sample(avctx, "VQA Version %i", s->vqa_version);
        return AVERROR_PATCHWELCOME;
     }
     s->width = AV_RL16(&s->avctx->extradata[6]);
     s->height = AV_RL16(&s->avctx->extradata[8]);
    if ((ret = av_image_check_size(s->width, s->height, 0, avctx)) < 0) {
         s->width= s->height= 0;
         return ret;
     }
    s->vector_width = s->avctx->extradata[10];
    s->vector_height = s->avctx->extradata[11];
    s->partial_count = s->partial_countdown = s->avctx->extradata[13];

    /* the vector dimensions have to meet very stringent requirements */
    if ((s->vector_width != 4) ||
        ((s->vector_height != 2) && (s->vector_height != 4))) {
        /* return without further initialization */
        return AVERROR_INVALIDDATA;
    }

    if (s->width % s->vector_width || s->height % s->vector_height) {
        av_log(avctx, AV_LOG_ERROR, "Image size not multiple of block size\n");
        return AVERROR_INVALIDDATA;
    }

    /* allocate codebooks */
    s->codebook_size = MAX_CODEBOOK_SIZE;
    s->codebook = av_malloc(s->codebook_size);
    if (!s->codebook)
        goto fail;
    s->next_codebook_buffer = av_malloc(s->codebook_size);
    if (!s->next_codebook_buffer)
        goto fail;

    /* allocate decode buffer */
    s->decode_buffer_size = (s->width / s->vector_width) *
        (s->height / s->vector_height) * 2;
    s->decode_buffer = av_mallocz(s->decode_buffer_size);
    if (!s->decode_buffer)
        goto fail;

    /* initialize the solid-color vectors */
    if (s->vector_height == 4) {
        codebook_index = 0xFF00 * 16;
        for (i = 0; i < 256; i++)
            for (j = 0; j < 16; j++)
                s->codebook[codebook_index++] = i;
    } else {
        codebook_index = 0xF00 * 8;
        for (i = 0; i < 256; i++)
            for (j = 0; j < 8; j++)
                s->codebook[codebook_index++] = i;
    }
    s->next_codebook_buffer_index = 0;

    return 0;
fail:
    av_freep(&s->codebook);
    av_freep(&s->next_codebook_buffer);
    av_freep(&s->decode_buffer);
    return AVERROR(ENOMEM);
}
