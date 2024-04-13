static bool jsi_csBitSetGet(int isSet, uchar *tbuf, int bits, Jsi_UWide *valPtr) {
    union bitfield *bms = (union bitfield *)tbuf;
    Jsi_UWide val = *valPtr;
    union bitfield {
        Jsi_UWide b1:1; Jsi_UWide b2:2; Jsi_UWide b3:3; Jsi_UWide b4:4; Jsi_UWide b5:5; Jsi_UWide b6:6;
        Jsi_UWide b7:7; Jsi_UWide b8:8; Jsi_UWide b9:9; Jsi_UWide b10:10; Jsi_UWide b11:11; Jsi_UWide b12:12;
        Jsi_UWide b13:13; Jsi_UWide b14:14; Jsi_UWide b15:15; Jsi_UWide b16:16; Jsi_UWide b17:17; 
        Jsi_UWide b18:18; Jsi_UWide b19:19; Jsi_UWide b20:20; Jsi_UWide b21:21; Jsi_UWide b22:22;
        Jsi_UWide b23:23; Jsi_UWide b24:24; Jsi_UWide b25:25; Jsi_UWide b26:26; Jsi_UWide b27:27;
        Jsi_UWide b28:28; Jsi_UWide b29:29; Jsi_UWide b30:30; Jsi_UWide b31:31; Jsi_UWide b32:32;
        Jsi_UWide b33:33; Jsi_UWide b34:34; Jsi_UWide b35:35; Jsi_UWide b36:36; Jsi_UWide b37:37;
        Jsi_UWide b38:38; Jsi_UWide b39:39; Jsi_UWide b40:40; Jsi_UWide b41:41; Jsi_UWide b42:42;
        Jsi_UWide b43:43; Jsi_UWide b44:44; Jsi_UWide b45:45; Jsi_UWide b46:46; Jsi_UWide b47:47;
        Jsi_UWide b48:48; Jsi_UWide b49:49; Jsi_UWide b50:50; Jsi_UWide b51:51; Jsi_UWide b52:52;
        Jsi_UWide b53:53; Jsi_UWide b54:54; Jsi_UWide b55:55; Jsi_UWide b56:56; Jsi_UWide b57:57;
        Jsi_UWide b58:58; Jsi_UWide b59:59; Jsi_UWide b60:60; Jsi_UWide b61:61; Jsi_UWide b62:62;
        Jsi_UWide b63:63; Jsi_UWide b64:64;
    };
    if (isSet) {
        switch (bits) {
    #define CBSN(n) \
            case n: bms->b##n = val; return (bms->b##n == val)
           CBSN(1); CBSN(2); CBSN(3); CBSN(4); CBSN(5); CBSN(6); CBSN(7); CBSN(8);
           CBSN(9); CBSN(10); CBSN(11); CBSN(12); CBSN(13); CBSN(14); CBSN(15); CBSN(16);
           CBSN(17); CBSN(18); CBSN(19); CBSN(20); CBSN(21); CBSN(22); CBSN(23); CBSN(24);
           CBSN(25); CBSN(26); CBSN(27); CBSN(28); CBSN(29); CBSN(30); CBSN(31); CBSN(32);
           CBSN(33); CBSN(34); CBSN(35); CBSN(36); CBSN(37); CBSN(38); CBSN(39); CBSN(40);
           CBSN(41); CBSN(42); CBSN(43); CBSN(44); CBSN(45); CBSN(46); CBSN(47); CBSN(48);
           CBSN(49); CBSN(50); CBSN(51); CBSN(52); CBSN(53); CBSN(54); CBSN(55); CBSN(56);
           CBSN(57); CBSN(58); CBSN(59); CBSN(60); CBSN(61); CBSN(62); CBSN(63); CBSN(64);
        }
        assert(0);
    }
    switch (bits) {
#define CBGN(n) \
        case n: val = bms->b##n; break
       CBGN(1); CBGN(2); CBGN(3); CBGN(4); CBGN(5); CBGN(6); CBGN(7); CBGN(8);
       CBGN(9); CBGN(10); CBGN(11); CBGN(12); CBGN(13); CBGN(14); CBGN(15); CBGN(16);
       CBGN(17); CBGN(18); CBGN(19); CBGN(20); CBGN(21); CBGN(22); CBGN(23); CBGN(24);
       CBGN(25); CBGN(26); CBGN(27); CBGN(28); CBGN(29); CBGN(30); CBGN(31); CBGN(32);
       CBGN(33); CBGN(34); CBGN(35); CBGN(36); CBGN(37); CBGN(38); CBGN(39); CBGN(40);
       CBGN(41); CBGN(42); CBGN(43); CBGN(44); CBGN(45); CBGN(46); CBGN(47); CBGN(48);
       CBGN(49); CBGN(50); CBGN(51); CBGN(52); CBGN(53); CBGN(54); CBGN(55); CBGN(56);
       CBGN(57); CBGN(58); CBGN(59); CBGN(60); CBGN(61); CBGN(62); CBGN(63); CBGN(64);
       default: assert(0);
    }
    *valPtr = val;
    return 1;
}