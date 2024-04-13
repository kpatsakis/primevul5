set_add_response(const char *code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t sz = sizeof(add_response)/sizeof(add_response[0]);
    int num = snprintf(add_response, sz, "%s:", code);
    vsnprintf(add_response + num, sz - num, fmt, args);
    va_end(args);
    has_add_respose = true;
}