static av_cold int vqa_decode_init(AVCodecContext *avctx)
{
    VqaContext *s = avctx->priv_data;
    int i, j, codebook_index;

    s->avctx = avctx;
    avctx->pix_fmt = PIX_FMT_PAL8;

    /* make sure the extradata made it */
    if (s->avctx->extradata_size != VQA_HEADER_SIZE) {
        av_log(s->avctx, AV_LOG_ERROR, "  VQA video: expected extradata size of %d\n", VQA_HEADER_SIZE);
        return -1;
    }

    /* load up the VQA parameters from the header */
    s->vqa_version = s->avctx->extradata[0];
    s->width = AV_RL16(&s->avctx->extradata[6]);
    s->height = AV_RL16(&s->avctx->extradata[8]);
    if(av_image_check_size(s->width, s->height, 0, avctx)){
        s->width= s->height= 0;
        return -1;
    }
    s->vector_width = s->avctx->extradata[10];
    s->vector_height = s->avctx->extradata[11];
    s->partial_count = s->partial_countdown = s->avctx->extradata[13];

    /* the vector dimensions have to meet very stringent requirements */
    if ((s->vector_width != 4) ||
        ((s->vector_height != 2) && (s->vector_height != 4))) {
        /* return without further initialization */
        return -1;
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
    s->decode_buffer = av_malloc(s->decode_buffer_size);
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

    s->frame.data[0] = NULL;

    return 0;
fail:
    av_freep(&s->codebook);
    av_freep(&s->next_codebook_buffer);
    av_freep(&s->decode_buffer);
    return AVERROR(ENOMEM);
}