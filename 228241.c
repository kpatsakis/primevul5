oldest_last(const void* a, const void *b) {
    monotonic_t ans = ((Image*)(b))->atime - ((Image*)(a))->atime;
    return ans < 0 ? -1 : (ans == 0 ? 0 : 1);
}