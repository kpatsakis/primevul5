static void printFavicon(HttpResponse res) {
        static size_t l;
        Socket_T S = res->S;
        static unsigned char *favicon = NULL;

        if (! favicon) {
                favicon = CALLOC(sizeof(unsigned char), strlen(FAVICON_ICO));
                l = decode_base64(favicon, FAVICON_ICO);
        }
        if (l) {
                res->is_committed = true;
                Socket_print(S, "HTTP/1.0 200 OK\r\n");
                Socket_print(S, "Content-length: %lu\r\n", (unsigned long)l);
                Socket_print(S, "Content-Type: image/x-icon\r\n");
                Socket_print(S, "Connection: close\r\n\r\n");
                if (Socket_write(S, favicon, l) < 0) {
                        LogError("Error sending favicon data -- %s\n", STRERROR);
                }
        }
}