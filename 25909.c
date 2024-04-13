int flag2str_sh(int flag, char *flag_str) {
    if (flag & 0x1)
        flag_str[2] = 'W';
    if (flag >> 1 & 0x1)
        flag_str[1] = 'A';
    if (flag >> 2 & 0x1)
        flag_str[0] = 'E';
    
    return 0;
}