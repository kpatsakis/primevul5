static int dpcm_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    const uint8_t *buf_end = buf + buf_size;
    DPCMContext *s = avctx->priv_data;
    int out = 0, ret;
    int predictor[2];
    int ch = 0;
    int stereo = s->channels - 1;
    int16_t *output_samples;

    /* calculate output size */
    switch(avctx->codec->id) {
    case CODEC_ID_ROQ_DPCM:
        out = buf_size - 8;
        break;
    case CODEC_ID_INTERPLAY_DPCM:
        out = buf_size - 6 - s->channels;
        break;
    case CODEC_ID_XAN_DPCM:
        out = buf_size - 2 * s->channels;
        break;
    case CODEC_ID_SOL_DPCM:
        if (avctx->codec_tag != 3)
            out = buf_size * 2;
        else
            out = buf_size;
        break;
    }
    if (out <= 0) {
        av_log(avctx, AV_LOG_ERROR, "packet is too small\n");
        return AVERROR(EINVAL);
    }

    /* get output buffer */
    s->frame.nb_samples = out / s->channels;
    if ((ret = avctx->get_buffer(avctx, &s->frame)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }
    output_samples = (int16_t *)s->frame.data[0];

    switch(avctx->codec->id) {

    case CODEC_ID_ROQ_DPCM:
        buf += 6;

        if (stereo) {
            predictor[1] = (int16_t)(bytestream_get_byte(&buf) << 8);
            predictor[0] = (int16_t)(bytestream_get_byte(&buf) << 8);
        } else {
            predictor[0] = (int16_t)bytestream_get_le16(&buf);
        }

        /* decode the samples */
        while (buf < buf_end) {
            predictor[ch] += s->roq_square_array[*buf++];
            predictor[ch]  = av_clip_int16(predictor[ch]);
            *output_samples++ = predictor[ch];

            /* toggle channel */
            ch ^= stereo;
        }
        break;

    case CODEC_ID_INTERPLAY_DPCM:
        buf += 6;  /* skip over the stream mask and stream length */

        for (ch = 0; ch < s->channels; ch++) {
            predictor[ch] = (int16_t)bytestream_get_le16(&buf);
            *output_samples++ = predictor[ch];
        }

        ch = 0;
        while (buf < buf_end) {
            predictor[ch] += interplay_delta_table[*buf++];
            predictor[ch]  = av_clip_int16(predictor[ch]);
            *output_samples++ = predictor[ch];

            /* toggle channel */
            ch ^= stereo;
        }
        break;

    case CODEC_ID_XAN_DPCM:
    {
        int shift[2] = { 4, 4 };

        for (ch = 0; ch < s->channels; ch++)
            predictor[ch] = (int16_t)bytestream_get_le16(&buf);

        ch = 0;
        while (buf < buf_end) {
            uint8_t n = *buf++;
            int16_t diff = (n & 0xFC) << 8;
            if ((n & 0x03) == 3)
                shift[ch]++;
            else
                shift[ch] -= (2 * (n & 3));
            /* saturate the shifter to a lower limit of 0 */
            if (shift[ch] < 0)
                shift[ch] = 0;

            diff >>= shift[ch];
            predictor[ch] += diff;

            predictor[ch] = av_clip_int16(predictor[ch]);
            *output_samples++ = predictor[ch];

            /* toggle channel */
            ch ^= stereo;
        }
        break;
    }
    case CODEC_ID_SOL_DPCM:
        if (avctx->codec_tag != 3) {
            uint8_t *output_samples_u8 = s->frame.data[0];
            while (buf < buf_end) {
                uint8_t n = *buf++;

                s->sample[0] += s->sol_table[n >> 4];
                s->sample[0]  = av_clip_uint8(s->sample[0]);
                *output_samples_u8++ = s->sample[0];

                s->sample[stereo] += s->sol_table[n & 0x0F];
                s->sample[stereo]  = av_clip_uint8(s->sample[stereo]);
                *output_samples_u8++ = s->sample[stereo];
            }
        } else {
            while (buf < buf_end) {
                uint8_t n = *buf++;
                if (n & 0x80) s->sample[ch] -= sol_table_16[n & 0x7F];
                else          s->sample[ch] += sol_table_16[n & 0x7F];
                s->sample[ch] = av_clip_int16(s->sample[ch]);
                *output_samples++ = s->sample[ch];
                /* toggle channel */
                ch ^= stereo;
            }
        }
        break;
    }

    *got_frame_ptr   = 1;
    *(AVFrame *)data = s->frame;

    return buf_size;
}