void moveFrom(UnicodeString &dest, UnicodeString &src) {
    dest = std::move(src);
}