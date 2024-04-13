int flag2str(int flag, char *flag_str) {
    if (flag & 0x1)
        flag_str[2] = 'E';
    if (flag >> 1 & 0x1)
        flag_str[1] = 'W';
    if (flag >> 2 & 0x1)
        flag_str[0] = 'R';
    
    return 0;
}