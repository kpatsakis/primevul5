static int find_and_clear_dirty_height(VncState *vs,
                                       int y, int last_x, int x, int height)
{
    int h;

    for (h = 1; h < (height - y); h++) {
        if (!test_bit(last_x, vs->dirty[y + h])) {
            break;
        }
        bitmap_clear(vs->dirty[y + h], last_x, x - last_x);
    }

    return h;
}