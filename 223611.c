static uint64_t sm501_2d_engine_read(void *opaque, hwaddr addr,
                                     unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    uint32_t ret = 0;
    SM501_DPRINTF("sm501 2d engine regs : read addr=%x\n", (int)addr);

    switch (addr) {
    case SM501_2D_SOURCE:
        ret = s->twoD_source;
        break;
    case SM501_2D_DESTINATION:
        ret = s->twoD_destination;
        break;
    case SM501_2D_DIMENSION:
        ret = s->twoD_dimension;
        break;
    case SM501_2D_CONTROL:
        ret = s->twoD_control;
        break;
    case SM501_2D_PITCH:
        ret = s->twoD_pitch;
        break;
    case SM501_2D_FOREGROUND:
        ret = s->twoD_foreground;
        break;
    case SM501_2D_BACKGROUND:
        ret = s->twoD_background;
        break;
    case SM501_2D_STRETCH:
        ret = s->twoD_stretch;
        break;
    case SM501_2D_COLOR_COMPARE:
        ret = s->twoD_color_compare;
        break;
    case SM501_2D_COLOR_COMPARE_MASK:
        ret = s->twoD_color_compare_mask;
        break;
    case SM501_2D_MASK:
        ret = s->twoD_mask;
        break;
    case SM501_2D_CLIP_TL:
        ret = s->twoD_clip_tl;
        break;
    case SM501_2D_CLIP_BR:
        ret = s->twoD_clip_br;
        break;
    case SM501_2D_MONO_PATTERN_LOW:
        ret = s->twoD_mono_pattern_low;
        break;
    case SM501_2D_MONO_PATTERN_HIGH:
        ret = s->twoD_mono_pattern_high;
        break;
    case SM501_2D_WINDOW_WIDTH:
        ret = s->twoD_window_width;
        break;
    case SM501_2D_SOURCE_BASE:
        ret = s->twoD_source_base;
        break;
    case SM501_2D_DESTINATION_BASE:
        ret = s->twoD_destination_base;
        break;
    case SM501_2D_ALPHA:
        ret = s->twoD_alpha;
        break;
    case SM501_2D_WRAP:
        ret = s->twoD_wrap;
        break;
    case SM501_2D_STATUS:
        ret = 0; /* Should return interrupt status */
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "sm501: not implemented disp ctrl register "
                      "read. addr=%" HWADDR_PRIx "\n", addr);
    }

    return ret;
}