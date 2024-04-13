static void sm501_2d_engine_write(void *opaque, hwaddr addr,
                                  uint64_t value, unsigned size)
{
    SM501State *s = (SM501State *)opaque;
    SM501_DPRINTF("sm501 2d engine regs : write addr=%x, val=%x\n",
                  (unsigned)addr, (unsigned)value);

    switch (addr) {
    case SM501_2D_SOURCE:
        s->twoD_source = value;
        break;
    case SM501_2D_DESTINATION:
        s->twoD_destination = value;
        break;
    case SM501_2D_DIMENSION:
        s->twoD_dimension = value;
        break;
    case SM501_2D_CONTROL:
        s->twoD_control = value;

        /* do 2d operation if start flag is set. */
        if (value & 0x80000000) {
            sm501_2d_operation(s);
            s->twoD_control &= ~0x80000000; /* start flag down */
        }

        break;
    case SM501_2D_PITCH:
        s->twoD_pitch = value;
        break;
    case SM501_2D_FOREGROUND:
        s->twoD_foreground = value;
        break;
    case SM501_2D_BACKGROUND:
        s->twoD_background = value;
        break;
    case SM501_2D_STRETCH:
        s->twoD_stretch = value;
        break;
    case SM501_2D_COLOR_COMPARE:
        s->twoD_color_compare = value;
        break;
    case SM501_2D_COLOR_COMPARE_MASK:
        s->twoD_color_compare_mask = value;
        break;
    case SM501_2D_MASK:
        s->twoD_mask = value;
        break;
    case SM501_2D_CLIP_TL:
        s->twoD_clip_tl = value;
        break;
    case SM501_2D_CLIP_BR:
        s->twoD_clip_br = value;
        break;
    case SM501_2D_MONO_PATTERN_LOW:
        s->twoD_mono_pattern_low = value;
        break;
    case SM501_2D_MONO_PATTERN_HIGH:
        s->twoD_mono_pattern_high = value;
        break;
    case SM501_2D_WINDOW_WIDTH:
        s->twoD_window_width = value;
        break;
    case SM501_2D_SOURCE_BASE:
        s->twoD_source_base = value;
        break;
    case SM501_2D_DESTINATION_BASE:
        s->twoD_destination_base = value;
        break;
    case SM501_2D_ALPHA:
        s->twoD_alpha = value;
        break;
    case SM501_2D_WRAP:
        s->twoD_wrap = value;
        break;
    case SM501_2D_STATUS:
        /* ignored, writing 0 should clear interrupt status */
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "sm501: not implemented 2d engine register "
                      "write. addr=%" HWADDR_PRIx
                      ", val=%" PRIx64 "\n", addr, value);
    }
}