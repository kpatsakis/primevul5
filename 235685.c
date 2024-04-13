static bool jsi_csSBitSetGet(int isSet, uchar *tbuf, int bits, Jsi_Wide *valPtr) {
    union bitfield *bms = (union bitfield *)tbuf;
    Jsi_Wide val = *valPtr;
    union bitfield {
        Jsi_Wide b1:1; Jsi_Wide b2:2; Jsi_Wide b3:3; Jsi_Wide b4:4; Jsi_Wide b5:5; Jsi_Wide b6:6;
        Jsi_Wide b7:7; Jsi_Wide b8:8; Jsi_Wide b9:9; Jsi_Wide b10:10; Jsi_Wide b11:11; Jsi_Wide b12:12;
        Jsi_Wide b13:13; Jsi_Wide b14:14; Jsi_Wide b15:15; Jsi_Wide b16:16; Jsi_Wide b17:17; 
        Jsi_Wide b18:18; Jsi_Wide b19:19; Jsi_Wide b20:20; Jsi_Wide b21:21; Jsi_Wide b22:22;
        Jsi_Wide b23:23; Jsi_Wide b24:24; Jsi_Wide b25:25; Jsi_Wide b26:26; Jsi_Wide b27:27;
        Jsi_Wide b28:28; Jsi_Wide b29:29; Jsi_Wide b30:30; Jsi_Wide b31:31; Jsi_Wide b32:32;
        Jsi_Wide b33:33; Jsi_Wide b34:34; Jsi_Wide b35:35; Jsi_Wide b36:36; Jsi_Wide b37:37;
        Jsi_Wide b38:38; Jsi_Wide b39:39; Jsi_Wide b40:40; Jsi_Wide b41:41; Jsi_Wide b42:42;
        Jsi_Wide b43:43; Jsi_Wide b44:44; Jsi_Wide b45:45; Jsi_Wide b46:46; Jsi_Wide b47:47;
        Jsi_Wide b48:48; Jsi_Wide b49:49; Jsi_Wide b50:50; Jsi_Wide b51:51; Jsi_Wide b52:52;
        Jsi_Wide b53:53; Jsi_Wide b54:54; Jsi_Wide b55:55; Jsi_Wide b56:56; Jsi_Wide b57:57;
        Jsi_Wide b58:58; Jsi_Wide b59:59; Jsi_Wide b60:60; Jsi_Wide b61:61; Jsi_Wide b62:62;
        Jsi_Wide b63:63; Jsi_Wide b64:64;
    };
    if (isSet) {
        switch (bits) {
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