unicode_unfold_key(OnigCodePoint code)
{
  static const struct ByUnfoldKey wordlist[] =
    {
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0},

      {0x1040a, 3267, 1},

      {0x1e0a, 1727, 1},

      {0x040a, 1016, 1},

      {0x010a, 186, 1},

      {0x1f0a, 2088, 1},

      {0x2c0a, 2451, 1},

      {0x0189, 619, 1},

      {0x1f89, 134, 2},

      {0x1f85, 154, 2},

      {0x0389, 733, 1},

      {0x03ff, 724, 1},

      {0xab89, 1523, 1},

      {0xab85, 1511, 1},

      {0x10c89, 3384, 1},

      {0x10c85, 3372, 1},

      {0x1e84, 1911, 1},

      {0x03f5, 752, 1},

      {0x0184, 360, 1},

      {0x1f84, 149, 2},

      {0x2c84, 2592, 1},

      {0x017d, 351, 1},

      {0x1ff3, 96, 2},

      {0xab84, 1508, 1},

      {0xa784, 3105, 1},

      {0x10c84, 3369, 1},

      {0xab7d, 1487, 1},

      {0xa77d, 1706, 1},

      {0x1e98, 38, 2},

      {0x0498, 1106, 1},

      {0x0198, 375, 1},

      {0x1f98, 169, 2},

      {0x2c98, 2622, 1},

      {0x0398, 762, 1},

      {0xa684, 2940, 1},

      {0xab98, 1568, 1},

      {0xa798, 3123, 1},

      {0x10c98, 3429, 1},

      {0x050a, 1277, 1},

      {0x1ffb, 2265, 1},

      {0x1e96, 16, 2},

      {0x0496, 1103, 1},

      {0x0196, 652, 1},

      {0x1f96, 199, 2},

      {0x2c96, 2619, 1},

      {0x0396, 756, 1},

      {0xa698, 2970, 1},

      {0xab96, 1562, 1},

      {0xa796, 3120, 1},

      {0x10c96, 3423, 1},

      {0x1feb, 2259, 1},

      {0x2ceb, 2736, 1},

      {0x1e90, 1929, 1},

      {0x0490, 1094, 1},

      {0x0190, 628, 1},

      {0x1f90, 169, 2},

      {0x2c90, 2610, 1},

      {0x0390, 25, 3},

      {0xa696, 2967, 1},

      {0xab90, 1544, 1},

      {0xa790, 3114, 1},

      {0x10c90, 3405, 1},

      {0x01d7, 444, 1},

      {0x1fd7, 31, 3},

      {0x1ea6, 1947, 1},

      {0x04a6, 1127, 1},

      {0x01a6, 676, 1},

      {0x1fa6, 239, 2},

      {0x2ca6, 2643, 1},

      {0x03a6, 810, 1},

      {0xa690, 2958, 1},

      {0xaba6, 1610, 1},

      {0xa7a6, 3144, 1},

      {0x10ca6, 3471, 1},

      {0x1ea4, 1944, 1},

      {0x04a4, 1124, 1},

      {0x01a4, 390, 1},

      {0x1fa4, 229, 2},

      {0x2ca4, 2640, 1},

      {0x03a4, 804, 1},

      {0x10a6, 2763, 1},

      {0xaba4, 1604, 1},

      {0xa7a4, 3141, 1},

      {0x10ca4, 3465, 1},

      {0x1ea0, 1938, 1},

      {0x04a0, 1118, 1},

      {0x01a0, 384, 1},

      {0x1fa0, 209, 2},

      {0x2ca0, 2634, 1},

      {0x03a0, 792, 1},

      {0x10a4, 2757, 1},

      {0xaba0, 1592, 1},

      {0xa7a0, 3135, 1},

      {0x10ca0, 3453, 1},

      {0x1eb2, 1965, 1},

      {0x04b2, 1145, 1},

      {0x01b2, 694, 1},

      {0x1fb2, 249, 2},

      {0x2cb2, 2661, 1},

      {0x03fd, 718, 1},

      {0x10a0, 2745, 1},

      {0xabb2, 1646, 1},

      {0xa7b2, 703, 1},

      {0x10cb2, 3507, 1},

      {0x1eac, 1956, 1},

      {0x04ac, 1136, 1},

      {0x01ac, 396, 1},

      {0x1fac, 229, 2},

      {0x2cac, 2652, 1},

      {0x0537, 1352, 1},

      {0x10b2, 2799, 1},

      {0xabac, 1628, 1},

      {0xa7ac, 637, 1},

      {0x10cac, 3489, 1},

      {0x1eaa, 1953, 1},

      {0x04aa, 1133, 1},

      {0x00dd, 162, 1},

      {0x1faa, 219, 2},

      {0x2caa, 2649, 1},

      {0x03aa, 824, 1},

      {0x10ac, 2781, 1},

      {0xabaa, 1622, 1},

      {0xa7aa, 646, 1},

      {0x10caa, 3483, 1},

      {0x1ea8, 1950, 1},

      {0x04a8, 1130, 1},

      {0x020a, 517, 1},

      {0x1fa8, 209, 2},

      {0x2ca8, 2646, 1},

      {0x03a8, 817, 1},

      {0x10aa, 2775, 1},

      {0xaba8, 1616, 1},

      {0xa7a8, 3147, 1},

      {0x10ca8, 3477, 1},

      {0x1ea2, 1941, 1},

      {0x04a2, 1121, 1},

      {0x01a2, 387, 1},

      {0x1fa2, 219, 2},

      {0x2ca2, 2637, 1},

      {0x118a6, 3528, 1},

      {0x10a8, 2769, 1},

      {0xaba2, 1598, 1},

      {0xa7a2, 3138, 1},

      {0x10ca2, 3459, 1},

      {0x2ced, 2739, 1},

      {0x1fe9, 2283, 1},

      {0x1fe7, 47, 3},

      {0x1eb0, 1962, 1},

      {0x04b0, 1142, 1},

      {0x118a4, 3522, 1},

      {0x10a2, 2751, 1},

      {0x2cb0, 2658, 1},

      {0x03b0, 41, 3},

      {0x1fe3, 41, 3},

      {0xabb0, 1640, 1},

      {0xa7b0, 706, 1},

      {0x10cb0, 3501, 1},

      {0x01d9, 447, 1},

      {0x1fd9, 2277, 1},

      {0x118a0, 3510, 1},

      {0x00df, 24, 2},

      {0x00d9, 150, 1},

      {0xab77, 1469, 1},

      {0x10b0, 2793, 1},

      {0x1eae, 1959, 1},

      {0x04ae, 1139, 1},

      {0x01ae, 685, 1},

      {0x1fae, 239, 2},

      {0x2cae, 2655, 1},

      {0x118b2, 3564, 1},

      {0xab73, 1457, 1},

      {0xabae, 1634, 1},

      {0xab71, 1451, 1},

      {0x10cae, 3495, 1},

      {0x1e2a, 1775, 1},

      {0x042a, 968, 1},

      {0x012a, 234, 1},

      {0x1f2a, 2130, 1},

      {0x2c2a, 2547, 1},

      {0x118ac, 3546, 1},

      {0x10ae, 2787, 1},

      {0x0535, 1346, 1},

      {0xa72a, 2988, 1},

      {0x1e9a, 0, 2},

      {0x049a, 1109, 1},

      {0xff37, 3225, 1},

      {0x1f9a, 179, 2},

      {0x2c9a, 2625, 1},

      {0x039a, 772, 1},

      {0x118aa, 3540, 1},

      {0xab9a, 1574, 1},

      {0xa79a, 3126, 1},

      {0x10c9a, 3435, 1},

      {0x1e94, 1935, 1},

      {0x0494, 1100, 1},

      {0x0194, 640, 1},

      {0x1f94, 189, 2},

      {0x2c94, 2616, 1},

      {0x0394, 749, 1},

      {0x118a8, 3534, 1},

      {0xab94, 1556, 1},

      {0xa69a, 2973, 1},

      {0x10c94, 3417, 1},

      {0x10402, 3243, 1},

      {0x1e02, 1715, 1},

      {0x0402, 992, 1},

      {0x0102, 174, 1},

      {0x0533, 1340, 1},

      {0x2c02, 2427, 1},

      {0x118a2, 3516, 1},

      {0x052a, 1325, 1},

      {0xa694, 2964, 1},

      {0x1e92, 1932, 1},

      {0x0492, 1097, 1},

      {0x2165, 2307, 1},

      {0x1f92, 179, 2},

      {0x2c92, 2613, 1},

      {0x0392, 742, 1},

      {0x2161, 2295, 1},

      {0xab92, 1550, 1},

      {0xa792, 3117, 1},

      {0x10c92, 3411, 1},

      {0x118b0, 3558, 1},

      {0x1f5f, 2199, 1},

      {0x1e8e, 1926, 1},

      {0x048e, 1091, 1},

      {0x018e, 453, 1},

      {0x1f8e, 159, 2},

      {0x2c8e, 2607, 1},

      {0x038e, 833, 1},

      {0xa692, 2961, 1},

      {0xab8e, 1538, 1},

      {0x0055, 59, 1},

      {0x10c8e, 3399, 1},

      {0x1f5d, 2196, 1},

      {0x212a, 27, 1},

      {0x04cb, 1181, 1},

      {0x01cb, 425, 1},

      {0x1fcb, 2241, 1},

      {0x118ae, 3552, 1},

      {0x0502, 1265, 1},

      {0x00cb, 111, 1},

      {0xa68e, 2955, 1},

      {0x1e8a, 1920, 1},

      {0x048a, 1085, 1},

      {0x018a, 622, 1},

      {0x1f8a, 139, 2},

      {0x2c8a, 2601, 1},

      {0x038a, 736, 1},

      {0x2c67, 2571, 1},

      {0xab8a, 1526, 1},

      {0x1e86, 1914, 1},

      {0x10c8a, 3387, 1},

      {0x0186, 616, 1},

      {0x1f86, 159, 2},

      {0x2c86, 2595, 1},

      {0x0386, 727, 1},

      {0xff35, 3219, 1},

      {0xab86, 1514, 1},

      {0xa786, 3108, 1},

      {0x10c86, 3375, 1},

      {0xa68a, 2949, 1},

      {0x0555, 1442, 1},

      {0x1ebc, 1980, 1},

      {0x04bc, 1160, 1},

      {0x01bc, 411, 1},

      {0x1fbc, 62, 2},

      {0x2cbc, 2676, 1},

      {0x1f5b, 2193, 1},

      {0xa686, 2943, 1},

      {0xabbc, 1676, 1},

      {0x1eb8, 1974, 1},

      {0x04b8, 1154, 1},

      {0x01b8, 408, 1},

      {0x1fb8, 2268, 1},

      {0x2cb8, 2670, 1},

      {0x01db, 450, 1},

      {0x1fdb, 2247, 1},

      {0xabb8, 1664, 1},

      {0x10bc, 2829, 1},

      {0x00db, 156, 1},

      {0x1eb6, 1971, 1},

      {0x04b6, 1151, 1},

      {0xff33, 3213, 1},

      {0x1fb6, 58, 2},

      {0x2cb6, 2667, 1},

      {0xff2a, 3186, 1},

      {0x10b8, 2817, 1},

      {0xabb6, 1658, 1},

      {0xa7b6, 3153, 1},

      {0x10426, 3351, 1},

      {0x1e26, 1769, 1},

      {0x0426, 956, 1},

      {0x0126, 228, 1},

      {0x0053, 52, 1},

      {0x2c26, 2535, 1},

      {0x0057, 65, 1},

      {0x10b6, 2811, 1},

      {0x022a, 562, 1},

      {0xa726, 2982, 1},

      {0x1e2e, 1781, 1},

      {0x042e, 980, 1},

      {0x012e, 240, 1},

      {0x1f2e, 2142, 1},

      {0x2c2e, 2559, 1},
      {0xffffffff, -1, 0},

      {0x2167, 2313, 1},
      {0xffffffff, -1, 0},

      {0xa72e, 2994, 1},

      {0x1e2c, 1778, 1},

      {0x042c, 974, 1},

      {0x012c, 237, 1},

      {0x1f2c, 2136, 1},

      {0x2c2c, 2553, 1},

      {0x1f6f, 2223, 1},

      {0x2c6f, 604, 1},

      {0xabbf, 1685, 1},

      {0xa72c, 2991, 1},

      {0x1e28, 1772, 1},

      {0x0428, 962, 1},

      {0x0128, 231, 1},

      {0x1f28, 2124, 1},

      {0x2c28, 2541, 1},
      {0xffffffff, -1, 0},

      {0x0553, 1436, 1},

      {0x10bf, 2838, 1},

      {0xa728, 2985, 1},

      {0x0526, 1319, 1},

      {0x0202, 505, 1},

      {0x1e40, 1808, 1},

      {0x10424, 3345, 1},

      {0x1e24, 1766, 1},

      {0x0424, 950, 1},

      {0x0124, 225, 1},
      {0xffffffff, -1, 0},

      {0x2c24, 2529, 1},

      {0x052e, 1331, 1},

      {0xa740, 3018, 1},

      {0x118bc, 3594, 1},

      {0xa724, 2979, 1},

      {0x1ef2, 2061, 1},

      {0x04f2, 1241, 1},

      {0x01f2, 483, 1},

      {0x1ff2, 257, 2},

      {0x2cf2, 2742, 1},

      {0x052c, 1328, 1},

      {0x118b8, 3582, 1},

      {0xa640, 2865, 1},

      {0x10422, 3339, 1},

      {0x1e22, 1763, 1},

      {0x0422, 944, 1},

      {0x0122, 222, 1},

      {0x2126, 820, 1},

      {0x2c22, 2523, 1},

      {0x0528, 1322, 1},

      {0x01f1, 483, 1},

      {0x118b6, 3576, 1},

      {0xa722, 2976, 1},

      {0x03f1, 796, 1},

      {0x1ebe, 1983, 1},

      {0x04be, 1163, 1},

      {0xfb02, 12, 2},

      {0x1fbe, 767, 1},

      {0x2cbe, 2679, 1},

      {0x01b5, 405, 1},

      {0x0540, 1379, 1},

      {0xabbe, 1682, 1},

      {0x0524, 1316, 1},

      {0x00b5, 779, 1},

      {0xabb5, 1655, 1},

      {0x1eba, 1977, 1},

      {0x04ba, 1157, 1},

      {0x216f, 2337, 1},

      {0x1fba, 2226, 1},

      {0x2cba, 2673, 1},

      {0x10be, 2835, 1},

      {0x0051, 46, 1},

      {0xabba, 1670, 1},

      {0x10b5, 2808, 1},

      {0x1e6e, 1878, 1},

      {0x046e, 1055, 1},

      {0x016e, 330, 1},

      {0x1f6e, 2220, 1},

      {0x2c6e, 664, 1},

      {0x118bf, 3603, 1},

      {0x0522, 1313, 1},

      {0x10ba, 2823, 1},

      {0xa76e, 3087, 1},

      {0x1eb4, 1968, 1},

      {0x04b4, 1148, 1},

      {0x2c75, 2583, 1},

      {0x1fb4, 50, 2},

      {0x2cb4, 2664, 1},

      {0xab75, 1463, 1},

      {0x1ec2, 1989, 1},

      {0xabb4, 1652, 1},

      {0xa7b4, 3150, 1},

      {0x1fc2, 253, 2},

      {0x2cc2, 2685, 1},

      {0x03c2, 800, 1},

      {0x00c2, 83, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff26, 3174, 1},

      {0x10b4, 2805, 1},

      {0x1eca, 2001, 1},

      {0x0551, 1430, 1},

      {0x01ca, 425, 1},

      {0x1fca, 2238, 1},

      {0x2cca, 2697, 1},

      {0x10c2, 2847, 1},

      {0x00ca, 108, 1},

      {0xff2e, 3198, 1},

      {0x1e8c, 1923, 1},

      {0x048c, 1088, 1},

      {0x0226, 556, 1},

      {0x1f8c, 149, 2},

      {0x2c8c, 2604, 1},

      {0x038c, 830, 1},
      {0xffffffff, -1, 0},

      {0xab8c, 1532, 1},

      {0xff2c, 3192, 1},

      {0x10c8c, 3393, 1},

      {0x1ec4, 1992, 1},

      {0x022e, 568, 1},

      {0x01c4, 417, 1},

      {0x1fc4, 54, 2},

      {0x2cc4, 2688, 1},
      {0xffffffff, -1, 0},

      {0x00c4, 89, 1},

      {0xff28, 3180, 1},

      {0xa68c, 2952, 1},

      {0x01cf, 432, 1},

      {0x022c, 565, 1},

      {0x118be, 3600, 1},

      {0x03cf, 839, 1},

      {0x00cf, 123, 1},

      {0x118b5, 3573, 1},
      {0xffffffff, -1, 0},

      {0x10c4, 2853, 1},

      {0x216e, 2334, 1},

      {0x24cb, 2406, 1},

      {0x0228, 559, 1},

      {0xff24, 3168, 1},
      {0xffffffff, -1, 0},

      {0x118ba, 3588, 1},

      {0x1efe, 2079, 1},

      {0x04fe, 1259, 1},

      {0x01fe, 499, 1},

      {0x1e9e, 24, 2},

      {0x049e, 1115, 1},

      {0x03fe, 721, 1},

      {0x1f9e, 199, 2},

      {0x2c9e, 2631, 1},

      {0x039e, 786, 1},

      {0x0224, 553, 1},

      {0xab9e, 1586, 1},

      {0xa79e, 3132, 1},

      {0x10c9e, 3447, 1},

      {0x01f7, 414, 1},

      {0x1ff7, 67, 3},

      {0xff22, 3162, 1},

      {0x03f7, 884, 1},

      {0x118b4, 3570, 1},

      {0x049c, 1112, 1},

      {0x019c, 661, 1},

      {0x1f9c, 189, 2},

      {0x2c9c, 2628, 1},

      {0x039c, 779, 1},

      {0x24bc, 2361, 1},

      {0xab9c, 1580, 1},

      {0xa79c, 3129, 1},

      {0x10c9c, 3441, 1},

      {0x0222, 550, 1},

      {0x1e7c, 1899, 1},

      {0x047c, 1076, 1},

      {0x1e82, 1908, 1},

      {0x24b8, 2349, 1},

      {0x0182, 357, 1},

      {0x1f82, 139, 2},

      {0x2c82, 2589, 1},

      {0xab7c, 1484, 1},
      {0xffffffff, -1, 0},

      {0xab82, 1502, 1},

      {0xa782, 3102, 1},

      {0x10c82, 3363, 1},

      {0x2c63, 1709, 1},

      {0x24b6, 2343, 1},

      {0x1e80, 1905, 1},

      {0x0480, 1082, 1},

      {0x1f59, 2190, 1},

      {0x1f80, 129, 2},

      {0x2c80, 2586, 1},

      {0x0059, 71, 1},

      {0xa682, 2937, 1},

      {0xab80, 1496, 1},

      {0xa780, 3099, 1},

      {0x10c80, 3357, 1},
      {0xffffffff, -1, 0},

      {0x1e4c, 1826, 1},

      {0x0145, 270, 1},

      {0x014c, 279, 1},

      {0x1f4c, 2184, 1},

      {0x0345, 767, 1},

      {0x0045, 12, 1},

      {0x004c, 31, 1},

      {0xa680, 2934, 1},

      {0xa74c, 3036, 1},

      {0x1e4a, 1823, 1},

      {0x01d5, 441, 1},

      {0x014a, 276, 1},

      {0x1f4a, 2178, 1},

      {0x03d5, 810, 1},

      {0x00d5, 141, 1},

      {0x004a, 24, 1},

      {0x24bf, 2370, 1},

      {0xa74a, 3033, 1},

      {0xa64c, 2883, 1},

      {0x1041c, 3321, 1},

      {0x1e1c, 1754, 1},

      {0x041c, 926, 1},

      {0x011c, 213, 1},

      {0x1f1c, 2118, 1},

      {0x2c1c, 2505, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xa64a, 2880, 1},

      {0x1041a, 3315, 1},

      {0x1e1a, 1751, 1},

      {0x041a, 920, 1},

      {0x011a, 210, 1},

      {0x1f1a, 2112, 1},

      {0x2c1a, 2499, 1},

      {0xabbd, 1679, 1},

      {0x0545, 1394, 1},

      {0x054c, 1415, 1},

      {0x10418, 3309, 1},

      {0x1e18, 1748, 1},

      {0x0418, 914, 1},

      {0x0118, 207, 1},

      {0x1f18, 2106, 1},

      {0x2c18, 2493, 1},

      {0x10bd, 2832, 1},

      {0x2163, 2301, 1},

      {0x054a, 1409, 1},

      {0x1040e, 3279, 1},

      {0x1e0e, 1733, 1},

      {0x040e, 1028, 1},

      {0x010e, 192, 1},

      {0x1f0e, 2100, 1},

      {0x2c0e, 2463, 1},

      {0x1efc, 2076, 1},

      {0x04fc, 1256, 1},

      {0x01fc, 496, 1},

      {0x1ffc, 96, 2},

      {0x051c, 1304, 1},

      {0x1040c, 3273, 1},

      {0x1e0c, 1730, 1},

      {0x040c, 1022, 1},

      {0x010c, 189, 1},

      {0x1f0c, 2094, 1},

      {0x2c0c, 2457, 1},

      {0x1f6d, 2217, 1},

      {0x2c6d, 607, 1},

      {0x051a, 1301, 1},

      {0x24be, 2367, 1},

      {0x10408, 3261, 1},

      {0x1e08, 1724, 1},

      {0x0408, 1010, 1},

      {0x0108, 183, 1},

      {0x1f08, 2082, 1},

      {0x2c08, 2445, 1},

      {0x04c9, 1178, 1},

      {0x0518, 1298, 1},

      {0x1fc9, 2235, 1},
      {0xffffffff, -1, 0},

      {0x24ba, 2355, 1},

      {0x00c9, 105, 1},

      {0x10416, 3303, 1},

      {0x1e16, 1745, 1},

      {0x0416, 908, 1},

      {0x0116, 204, 1},

      {0x050e, 1283, 1},

      {0x2c16, 2487, 1},

      {0x10414, 3297, 1},

      {0x1e14, 1742, 1},

      {0x0414, 902, 1},

      {0x0114, 201, 1},

      {0x042b, 971, 1},

      {0x2c14, 2481, 1},

      {0x1f2b, 2133, 1},

      {0x2c2b, 2550, 1},
      {0xffffffff, -1, 0},

      {0x050c, 1280, 1},

      {0x10406, 3255, 1},

      {0x1e06, 1721, 1},

      {0x0406, 1004, 1},

      {0x0106, 180, 1},

      {0x13fb, 1697, 1},

      {0x2c06, 2439, 1},

      {0x24c2, 2379, 1},

      {0x118bd, 3597, 1},
      {0xffffffff, -1, 0},

      {0x0508, 1274, 1},

      {0x10404, 3249, 1},

      {0x1e04, 1718, 1},

      {0x0404, 998, 1},

      {0x0104, 177, 1},

      {0x1f95, 194, 2},

      {0x2c04, 2433, 1},

      {0x0395, 752, 1},

      {0x24ca, 2403, 1},

      {0xab95, 1559, 1},

      {0x0531, 1334, 1},

      {0x10c95, 3420, 1},

      {0x0516, 1295, 1},

      {0x1e6c, 1875, 1},

      {0x046c, 1052, 1},

      {0x016c, 327, 1},

      {0x1f6c, 2214, 1},

      {0x216d, 2331, 1},

      {0x0514, 1292, 1},

      {0x0245, 697, 1},

      {0x024c, 598, 1},

      {0xa76c, 3084, 1},

      {0x10400, 3237, 1},

      {0x1e00, 1712, 1},

      {0x0400, 986, 1},

      {0x0100, 171, 1},

      {0x24c4, 2385, 1},

      {0x2c00, 2421, 1},

      {0x0506, 1271, 1},

      {0x024a, 595, 1},

      {0x1fab, 224, 2},

      {0xa66c, 2931, 1},

      {0x03ab, 827, 1},

      {0x24cf, 2418, 1},

      {0xabab, 1625, 1},

      {0xa7ab, 631, 1},

      {0x10cab, 3486, 1},
      {0xffffffff, -1, 0},

      {0x0504, 1268, 1},
      {0xffffffff, -1, 0},

      {0x021c, 544, 1},

      {0x01a9, 679, 1},

      {0x1fa9, 214, 2},

      {0x10ab, 2778, 1},

      {0x03a9, 820, 1},

      {0x212b, 92, 1},

      {0xaba9, 1619, 1},

      {0x1e88, 1917, 1},

      {0x10ca9, 3480, 1},

      {0x021a, 541, 1},

      {0x1f88, 129, 2},

      {0x2c88, 2598, 1},

      {0x0388, 730, 1},

      {0x13fd, 1703, 1},

      {0xab88, 1520, 1},

      {0x10a9, 2772, 1},

      {0x10c88, 3381, 1},
      {0xffffffff, -1, 0},

      {0x0218, 538, 1},

      {0x0500, 1262, 1},

      {0x1f4d, 2187, 1},

      {0x01a7, 393, 1},

      {0x1fa7, 244, 2},

      {0x004d, 34, 1},

      {0x03a7, 814, 1},

      {0xa688, 2946, 1},

      {0xaba7, 1613, 1},

      {0x020e, 523, 1},

      {0x10ca7, 3474, 1},

      {0x1e6a, 1872, 1},

      {0x046a, 1049, 1},

      {0x016a, 324, 1},

      {0x1f6a, 2208, 1},
      {0xffffffff, -1, 0},

      {0x216c, 2328, 1},

      {0x10a7, 2766, 1},

      {0x01d1, 435, 1},

      {0xa76a, 3081, 1},

      {0x020c, 520, 1},

      {0x03d1, 762, 1},

      {0x00d1, 129, 1},

      {0x1e68, 1869, 1},

      {0x0468, 1046, 1},

      {0x0168, 321, 1},

      {0x1f68, 2202, 1},
      {0xffffffff, -1, 0},

      {0xff31, 3207, 1},

      {0xa66a, 2928, 1},

      {0x0208, 514, 1},

      {0xa768, 3078, 1},

      {0x1e64, 1863, 1},

      {0x0464, 1040, 1},

      {0x0164, 315, 1},

      {0x054d, 1418, 1},

      {0x2c64, 673, 1},
      {0xffffffff, -1, 0},

      {0xff2b, 3189, 1},
      {0xffffffff, -1, 0},

      {0xa764, 3072, 1},

      {0xa668, 2925, 1},

      {0x0216, 535, 1},
      {0xffffffff, -1, 0},

      {0x118ab, 3543, 1},

      {0x1e62, 1860, 1},

      {0x0462, 1037, 1},

      {0x0162, 312, 1},

      {0x0214, 532, 1},

      {0x2c62, 655, 1},

      {0xa664, 2919, 1},

      {0x1ed2, 2013, 1},

      {0x04d2, 1193, 1},

      {0xa762, 3069, 1},

      {0x1fd2, 20, 3},

      {0x2cd2, 2709, 1},

      {0x118a9, 3537, 1},

      {0x00d2, 132, 1},

      {0x0206, 511, 1},

      {0x10420, 3333, 1},

      {0x1e20, 1760, 1},

      {0x0420, 938, 1},

      {0x0120, 219, 1},

      {0xa662, 2916, 1},

      {0x2c20, 2517, 1},

      {0x1e60, 1856, 1},

      {0x0460, 1034, 1},

      {0x0160, 309, 1},

      {0x0204, 508, 1},

      {0x2c60, 2562, 1},
      {0xffffffff, -1, 0},

      {0x24bd, 2364, 1},

      {0x216a, 2322, 1},

      {0xa760, 3066, 1},
      {0xffffffff, -1, 0},

      {0xfb16, 125, 2},

      {0x118a7, 3531, 1},

      {0x1efa, 2073, 1},

      {0x04fa, 1253, 1},

      {0x01fa, 493, 1},

      {0x1ffa, 2262, 1},

      {0xfb14, 109, 2},

      {0x03fa, 887, 1},

      {0xa660, 2913, 1},

      {0x2168, 2316, 1},

      {0x01b7, 700, 1},

      {0x1fb7, 10, 3},

      {0x1f6b, 2211, 1},

      {0x2c6b, 2577, 1},

      {0x0200, 502, 1},

      {0xabb7, 1661, 1},

      {0xfb06, 29, 2},

      {0x1e56, 1841, 1},

      {0x2164, 2304, 1},

      {0x0156, 294, 1},

      {0x1f56, 62, 3},

      {0x0520, 1310, 1},

      {0x004f, 40, 1},

      {0x0056, 62, 1},

      {0x10b7, 2814, 1},

      {0xa756, 3051, 1},

      {0xfb04, 5, 3},

      {0x1e78, 1893, 1},

      {0x0478, 1070, 1},

      {0x0178, 168, 1},

      {0x1e54, 1838, 1},

      {0x2162, 2298, 1},

      {0x0154, 291, 1},

      {0x1f54, 57, 3},

      {0xab78, 1472, 1},

      {0xa656, 2898, 1},

      {0x0054, 56, 1},

      {0x1e52, 1835, 1},

      {0xa754, 3048, 1},

      {0x0152, 288, 1},

      {0x1f52, 52, 3},

      {0x24c9, 2400, 1},

      {0x1e32, 1787, 1},

      {0x0052, 49, 1},

      {0x0132, 243, 1},

      {0xa752, 3045, 1},
      {0xffffffff, -1, 0},

      {0xfb00, 4, 2},

      {0xa654, 2895, 1},
      {0xffffffff, -1, 0},

      {0xa732, 2997, 1},

      {0x2160, 2292, 1},

      {0x054f, 1424, 1},

      {0x0556, 1445, 1},

      {0x1e50, 1832, 1},

      {0xa652, 2892, 1},

      {0x0150, 285, 1},

      {0x1f50, 84, 2},

      {0x017b, 348, 1},

      {0x1e4e, 1829, 1},

      {0x0050, 43, 1},

      {0x014e, 282, 1},

      {0xa750, 3042, 1},

      {0xab7b, 1481, 1},

      {0xa77b, 3093, 1},

      {0x004e, 37, 1},

      {0x0554, 1439, 1},

      {0xa74e, 3039, 1},

      {0x1e48, 1820, 1},
      {0xffffffff, -1, 0},

      {0x216b, 2325, 1},

      {0x1f48, 2172, 1},

      {0xa650, 2889, 1},

      {0x0552, 1433, 1},

      {0x0048, 21, 1},
      {0xffffffff, -1, 0},

      {0xa748, 3030, 1},

      {0xa64e, 2886, 1},

      {0x0532, 1337, 1},

      {0x1041e, 3327, 1},

      {0x1e1e, 1757, 1},

      {0x041e, 932, 1},

      {0x011e, 216, 1},

      {0x118b7, 3579, 1},

      {0x2c1e, 2511, 1},
      {0xffffffff, -1, 0},

      {0xa648, 2877, 1},

      {0x1ff9, 2253, 1},
      {0xffffffff, -1, 0},

      {0x03f9, 878, 1},

      {0x0550, 1427, 1},

      {0x10412, 3291, 1},

      {0x1e12, 1739, 1},

      {0x0412, 896, 1},

      {0x0112, 198, 1},

      {0x054e, 1421, 1},

      {0x2c12, 2475, 1},

      {0x10410, 3285, 1},

      {0x1e10, 1736, 1},

      {0x0410, 890, 1},

      {0x0110, 195, 1},
      {0xffffffff, -1, 0},

      {0x2c10, 2469, 1},

      {0x2132, 2289, 1},

      {0x0548, 1403, 1},

      {0x1ef8, 2070, 1},

      {0x04f8, 1250, 1},

      {0x01f8, 490, 1},

      {0x1ff8, 2250, 1},

      {0x0220, 381, 1},

      {0x1ee2, 2037, 1},

      {0x04e2, 1217, 1},

      {0x01e2, 462, 1},

      {0x1fe2, 36, 3},

      {0x2ce2, 2733, 1},

      {0x03e2, 857, 1},

      {0x051e, 1307, 1},

      {0x1ede, 2031, 1},

      {0x04de, 1211, 1},

      {0x01de, 456, 1},
      {0xffffffff, -1, 0},

      {0x2cde, 2727, 1},

      {0x03de, 851, 1},

      {0x00de, 165, 1},

      {0x1f69, 2205, 1},

      {0x2c69, 2574, 1},

      {0x1eda, 2025, 1},

      {0x04da, 1205, 1},

      {0x0512, 1289, 1},

      {0x1fda, 2244, 1},

      {0x2cda, 2721, 1},

      {0x03da, 845, 1},

      {0x00da, 153, 1},
      {0xffffffff, -1, 0},

      {0x0510, 1286, 1},

      {0x1ed8, 2022, 1},

      {0x04d8, 1202, 1},
      {0xffffffff, -1, 0},

      {0x1fd8, 2274, 1},

      {0x2cd8, 2718, 1},

      {0x03d8, 842, 1},

      {0x00d8, 147, 1},

      {0x1ed6, 2019, 1},

      {0x04d6, 1199, 1},
      {0xffffffff, -1, 0},

      {0x1fd6, 76, 2},

      {0x2cd6, 2715, 1},

      {0x03d6, 792, 1},

      {0x00d6, 144, 1},

      {0x1ec8, 1998, 1},
      {0xffffffff, -1, 0},

      {0x01c8, 421, 1},

      {0x1fc8, 2232, 1},

      {0x2cc8, 2694, 1},

      {0xff32, 3210, 1},

      {0x00c8, 102, 1},

      {0x04c7, 1175, 1},

      {0x01c7, 421, 1},

      {0x1fc7, 15, 3},

      {0x1ec0, 1986, 1},

      {0x04c0, 1187, 1},

      {0x00c7, 99, 1},
      {0xffffffff, -1, 0},

      {0x2cc0, 2682, 1},

      {0x0179, 345, 1},

      {0x00c0, 77, 1},

      {0x0232, 574, 1},

      {0x01b3, 402, 1},

      {0x1fb3, 62, 2},

      {0xab79, 1475, 1},

      {0xa779, 3090, 1},

      {0x10c7, 2859, 1},

      {0xabb3, 1649, 1},

      {0xa7b3, 3156, 1},

      {0x1fa5, 234, 2},

      {0x10c0, 2841, 1},

      {0x03a5, 807, 1},
      {0xffffffff, -1, 0},

      {0xaba5, 1607, 1},

      {0x01b1, 691, 1},

      {0x10ca5, 3468, 1},

      {0x10b3, 2802, 1},

      {0x2169, 2319, 1},

      {0x024e, 601, 1},

      {0xabb1, 1643, 1},

      {0xa7b1, 682, 1},

      {0x10cb1, 3504, 1},

      {0x10a5, 2760, 1},
      {0xffffffff, -1, 0},

      {0x01af, 399, 1},

      {0x1faf, 244, 2},
      {0xffffffff, -1, 0},

      {0x0248, 592, 1},

      {0x10b1, 2796, 1},

      {0xabaf, 1637, 1},

      {0x1fad, 234, 2},

      {0x10caf, 3498, 1},

      {0x04cd, 1184, 1},

      {0x01cd, 429, 1},

      {0xabad, 1631, 1},

      {0xa7ad, 658, 1},

      {0x10cad, 3492, 1},

      {0x00cd, 117, 1},

      {0x10af, 2790, 1},

      {0x021e, 547, 1},

      {0x1fa3, 224, 2},
      {0xffffffff, -1, 0},

      {0x03a3, 800, 1},

      {0x10ad, 2784, 1},

      {0xaba3, 1601, 1},
      {0xffffffff, -1, 0},

      {0x10ca3, 3462, 1},

      {0x10cd, 2862, 1},

      {0x1fa1, 214, 2},

      {0x24b7, 2346, 1},

      {0x03a1, 796, 1},

      {0x0212, 529, 1},

      {0xaba1, 1595, 1},

      {0x10a3, 2754, 1},

      {0x10ca1, 3456, 1},

      {0x01d3, 438, 1},

      {0x1fd3, 25, 3},

      {0x0210, 526, 1},
      {0xffffffff, -1, 0},

      {0x00d3, 135, 1},

      {0x1e97, 34, 2},

      {0x10a1, 2748, 1},

      {0x0197, 649, 1},

      {0x1f97, 204, 2},
      {0xffffffff, -1, 0},

      {0x0397, 759, 1},

      {0x1041d, 3324, 1},

      {0xab97, 1565, 1},

      {0x041d, 929, 1},

      {0x10c97, 3426, 1},

      {0x1f1d, 2121, 1},

      {0x2c1d, 2508, 1},

      {0x1e72, 1884, 1},

      {0x0472, 1061, 1},

      {0x0172, 336, 1},

      {0x118b3, 3567, 1},

      {0x2c72, 2580, 1},

      {0x0372, 712, 1},

      {0x1041b, 3318, 1},

      {0xab72, 1454, 1},

      {0x041b, 923, 1},

      {0x118a5, 3525, 1},

      {0x1f1b, 2115, 1},

      {0x2c1b, 2502, 1},

      {0x1e70, 1881, 1},

      {0x0470, 1058, 1},

      {0x0170, 333, 1},

      {0x118b1, 3561, 1},

      {0x2c70, 610, 1},

      {0x0370, 709, 1},

      {0x1e46, 1817, 1},

      {0xab70, 1448, 1},

      {0x1e66, 1866, 1},

      {0x0466, 1043, 1},

      {0x0166, 318, 1},

      {0x1e44, 1814, 1},

      {0x0046, 15, 1},

      {0x118af, 3555, 1},

      {0xa746, 3027, 1},
      {0xffffffff, -1, 0},

      {0xa766, 3075, 1},

      {0x0044, 9, 1},

      {0x118ad, 3549, 1},

      {0xa744, 3024, 1},

      {0x1e7a, 1896, 1},

      {0x047a, 1073, 1},

      {0x1e3a, 1799, 1},
      {0xffffffff, -1, 0},

      {0xa646, 2874, 1},

      {0x1f3a, 2154, 1},

      {0xa666, 2922, 1},

      {0xab7a, 1478, 1},

      {0x118a3, 3519, 1},

      {0xa644, 2871, 1},

      {0xa73a, 3009, 1},
      {0xffffffff, -1, 0},

      {0x1ef4, 2064, 1},

      {0x04f4, 1244, 1},

      {0x01f4, 487, 1},

      {0x1ff4, 101, 2},

      {0x118a1, 3513, 1},

      {0x03f4, 762, 1},

      {0x1eec, 2052, 1},

      {0x04ec, 1232, 1},

      {0x01ec, 477, 1},

      {0x1fec, 2286, 1},

      {0x0546, 1397, 1},

      {0x03ec, 872, 1},
      {0xffffffff, -1, 0},

      {0x013f, 261, 1},

      {0x1f3f, 2169, 1},

      {0x0544, 1391, 1},

      {0x1eea, 2049, 1},

      {0x04ea, 1229, 1},

      {0x01ea, 474, 1},

      {0x1fea, 2256, 1},
      {0xffffffff, -1, 0},

      {0x03ea, 869, 1},

      {0x1ee8, 2046, 1},

      {0x04e8, 1226, 1},

      {0x01e8, 471, 1},

      {0x1fe8, 2280, 1},

      {0x053a, 1361, 1},

      {0x03e8, 866, 1},

      {0x1ee6, 2043, 1},

      {0x04e6, 1223, 1},

      {0x01e6, 468, 1},

      {0x1fe6, 88, 2},

      {0x1f4b, 2181, 1},

      {0x03e6, 863, 1},

      {0x1e5e, 1853, 1},

      {0x004b, 27, 1},

      {0x015e, 306, 1},

      {0x2166, 2310, 1},

      {0x1ee4, 2040, 1},

      {0x04e4, 1220, 1},

      {0x01e4, 465, 1},

      {0x1fe4, 80, 2},

      {0xa75e, 3063, 1},

      {0x03e4, 860, 1},

      {0x1ee0, 2034, 1},

      {0x04e0, 1214, 1},

      {0x01e0, 459, 1},

      {0x053f, 1376, 1},

      {0x2ce0, 2730, 1},

      {0x03e0, 854, 1},

      {0x1edc, 2028, 1},

      {0x04dc, 1208, 1},

      {0xa65e, 2910, 1},
      {0xffffffff, -1, 0},

      {0x2cdc, 2724, 1},

      {0x03dc, 848, 1},

      {0x00dc, 159, 1},

      {0x1ed0, 2010, 1},

      {0x04d0, 1190, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0x2cd0, 2706, 1},

      {0x03d0, 742, 1},

      {0x00d0, 126, 1},

      {0x1ecc, 2004, 1},

      {0x054b, 1412, 1},
      {0xffffffff, -1, 0},

      {0x1fcc, 71, 2},

      {0x2ccc, 2700, 1},

      {0x1ec6, 1995, 1},

      {0x00cc, 114, 1},
      {0xffffffff, -1, 0},

      {0x1fc6, 67, 2},

      {0x2cc6, 2691, 1},

      {0x24c8, 2397, 1},

      {0x00c6, 96, 1},

      {0x04c5, 1172, 1},

      {0x01c5, 417, 1},
      {0xffffffff, -1, 0},

      {0x1fbb, 2229, 1},

      {0x24c7, 2394, 1},

      {0x00c5, 92, 1},

      {0x1fb9, 2271, 1},

      {0xabbb, 1673, 1},

      {0x24c0, 2373, 1},

      {0x04c3, 1169, 1},

      {0xabb9, 1667, 1},

      {0x1fc3, 71, 2},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0x00c3, 86, 1},

      {0x10c5, 2856, 1},

      {0x10bb, 2826, 1},

      {0x1ed4, 2016, 1},

      {0x04d4, 1196, 1},

      {0x10b9, 2820, 1},

      {0x13fc, 1700, 1},

      {0x2cd4, 2712, 1},

      {0x0246, 589, 1},

      {0x00d4, 138, 1},

      {0x10c3, 2850, 1},
      {0xffffffff, -1, 0},

      {0xff3a, 3234, 1},

      {0x0244, 688, 1},

      {0x019f, 670, 1},

      {0x1f9f, 204, 2},
      {0xffffffff, -1, 0},

      {0x039f, 789, 1},
      {0xffffffff, -1, 0},

      {0xab9f, 1589, 1},
      {0xffffffff, -1, 0},

      {0x10c9f, 3450, 1},

      {0x019d, 667, 1},

      {0x1f9d, 194, 2},

      {0x023a, 2565, 1},

      {0x039d, 783, 1},

      {0x1e5a, 1847, 1},

      {0xab9d, 1583, 1},

      {0x015a, 300, 1},

      {0x10c9d, 3444, 1},

      {0x1e9b, 1856, 1},

      {0x24cd, 2412, 1},

      {0x005a, 74, 1},

      {0x1f9b, 184, 2},

      {0xa75a, 3057, 1},

      {0x039b, 776, 1},

      {0x1ece, 2007, 1},

      {0xab9b, 1577, 1},

      {0x1e99, 42, 2},

      {0x10c9b, 3438, 1},

      {0x2cce, 2703, 1},

      {0x1f99, 174, 2},

      {0x00ce, 120, 1},

      {0x0399, 767, 1},

      {0xa65a, 2904, 1},

      {0xab99, 1571, 1},
      {0xffffffff, -1, 0},

      {0x10c99, 3432, 1},

      {0x0193, 634, 1},

      {0x1f93, 184, 2},

      {0x1e58, 1844, 1},

      {0x0393, 746, 1},

      {0x0158, 297, 1},

      {0xab93, 1553, 1},
      {0xffffffff, -1, 0},

      {0x10c93, 3414, 1},

      {0x0058, 68, 1},

      {0x042d, 977, 1},

      {0xa758, 3054, 1},

      {0x1f2d, 2139, 1},

      {0x2c2d, 2556, 1},

      {0x118bb, 3591, 1},

      {0x0191, 369, 1},

      {0x1f91, 174, 2},

      {0x118b9, 3585, 1},

      {0x0391, 739, 1},
      {0xffffffff, -1, 0},

      {0xab91, 1547, 1},

      {0xa658, 2901, 1},

      {0x10c91, 3408, 1},

      {0x018f, 625, 1},

      {0x1f8f, 164, 2},
      {0xffffffff, -1, 0},

      {0x038f, 836, 1},
      {0xffffffff, -1, 0},

      {0xab8f, 1541, 1},
      {0xffffffff, -1, 0},

      {0x10c8f, 3402, 1},

      {0x018b, 366, 1},

      {0x1f8b, 144, 2},
      {0xffffffff, -1, 0},

      {0x0187, 363, 1},

      {0x1f87, 164, 2},

      {0xab8b, 1529, 1},

      {0xa78b, 3111, 1},

      {0x10c8b, 3390, 1},

      {0xab87, 1517, 1},

      {0x04c1, 1166, 1},

      {0x10c87, 3378, 1},

      {0x1e7e, 1902, 1},

      {0x047e, 1079, 1},
      {0xffffffff, -1, 0},

      {0x00c1, 80, 1},

      {0x2c7e, 580, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xab7e, 1490, 1},

      {0xa77e, 3096, 1},

      {0x1e76, 1890, 1},

      {0x0476, 1067, 1},

      {0x0176, 342, 1},

      {0x1e42, 1811, 1},

      {0x10c1, 2844, 1},

      {0x0376, 715, 1},

      {0x1e36, 1793, 1},

      {0xab76, 1466, 1},

      {0x0136, 249, 1},

      {0x0042, 3, 1},

      {0x1e3e, 1805, 1},

      {0xa742, 3021, 1},

      {0x1e38, 1796, 1},

      {0x1f3e, 2166, 1},

      {0xa736, 3003, 1},

      {0x1f38, 2148, 1},
      {0xffffffff, -1, 0},

      {0x0587, 105, 2},

      {0xa73e, 3015, 1},
      {0xffffffff, -1, 0},

      {0xa738, 3006, 1},

      {0xa642, 2868, 1},

      {0x1e5c, 1850, 1},

      {0x1e34, 1790, 1},

      {0x015c, 303, 1},

      {0x0134, 246, 1},

      {0x1ef6, 2067, 1},

      {0x04f6, 1247, 1},

      {0x01f6, 372, 1},

      {0x1ff6, 92, 2},

      {0xa75c, 3060, 1},

      {0xa734, 3000, 1},

      {0x1ef0, 2058, 1},

      {0x04f0, 1238, 1},

      {0x01f0, 20, 2},
      {0xffffffff, -1, 0},

      {0x1e30, 1784, 1},

      {0x03f0, 772, 1},

      {0x0130, 261, 2},

      {0x0542, 1385, 1},

      {0xa65c, 2907, 1},

      {0x1f83, 144, 2},

      {0x0536, 1349, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xab83, 1505, 1},

      {0x053e, 1373, 1},

      {0x10c83, 3366, 1},

      {0x0538, 1355, 1},

      {0x1eee, 2055, 1},

      {0x04ee, 1235, 1},

      {0x01ee, 480, 1},

      {0x1f8d, 154, 2},
      {0xffffffff, -1, 0},

      {0x03ee, 875, 1},
      {0xffffffff, -1, 0},

      {0xab8d, 1535, 1},

      {0xa78d, 643, 1},

      {0x10c8d, 3396, 1},

      {0x0534, 1343, 1},

      {0x0181, 613, 1},

      {0x1f81, 134, 2},

      {0x013d, 258, 1},

      {0x1f3d, 2163, 1},
      {0xffffffff, -1, 0},

      {0xab81, 1499, 1},

      {0x017f, 52, 1},

      {0x10c81, 3360, 1},

      {0x2c7f, 583, 1},

      {0x037f, 881, 1},

      {0xff2d, 3195, 1},

      {0xab7f, 1493, 1},

      {0x1e74, 1887, 1},

      {0x0474, 1064, 1},

      {0x0174, 339, 1},

      {0x1e3c, 1802, 1},

      {0x0149, 46, 2},

      {0x1f49, 2175, 1},

      {0x1f3c, 2160, 1},

      {0xab74, 1460, 1},

      {0x0049, 3606, 1},

      {0x0143, 267, 1},

      {0x24cc, 2409, 1},

      {0xa73c, 3012, 1},
      {0xffffffff, -1, 0},

      {0x0043, 6, 1},

      {0x0141, 264, 1},

      {0x24c6, 2391, 1},

      {0x013b, 255, 1},

      {0x1f3b, 2157, 1},

      {0x0041, 0, 1},

      {0x0139, 252, 1},

      {0x1f39, 2151, 1},

      {0x24c5, 2388, 1},

      {0x24bb, 2358, 1},

      {0x13fa, 1694, 1},

      {0x053d, 1370, 1},

      {0x24b9, 2352, 1},

      {0x0429, 965, 1},

      {0x2183, 2340, 1},

      {0x1f29, 2127, 1},

      {0x2c29, 2544, 1},

      {0x24c3, 2382, 1},

      {0x10427, 3354, 1},

      {0x10425, 3348, 1},

      {0x0427, 959, 1},

      {0x0425, 953, 1},
      {0xffffffff, -1, 0},

      {0x2c27, 2538, 1},

      {0x2c25, 2532, 1},

      {0x0549, 1406, 1},

      {0x053c, 1367, 1},

      {0x10423, 3342, 1},
      {0xffffffff, -1, 0},

      {0x0423, 947, 1},

      {0x0543, 1388, 1},
      {0xffffffff, -1, 0},

      {0x2c23, 2526, 1},

      {0xff36, 3222, 1},
      {0xffffffff, -1, 0},

      {0x0541, 1382, 1},

      {0x10421, 3336, 1},

      {0x053b, 1364, 1},

      {0x0421, 941, 1},

      {0xff38, 3228, 1},

      {0x0539, 1358, 1},

      {0x2c21, 2520, 1},

      {0x10419, 3312, 1},

      {0x10417, 3306, 1},

      {0x0419, 917, 1},

      {0x0417, 911, 1},

      {0x1f19, 2109, 1},

      {0x2c19, 2496, 1},

      {0x2c17, 2490, 1},

      {0x023e, 2568, 1},

      {0xff34, 3216, 1},

      {0x10415, 3300, 1},

      {0x10413, 3294, 1},

      {0x0415, 905, 1},

      {0x0413, 899, 1},
      {0xffffffff, -1, 0},

      {0x2c15, 2484, 1},

      {0x2c13, 2478, 1},
      {0xffffffff, -1, 0},

      {0x24ce, 2415, 1},

      {0x1040f, 3282, 1},
      {0xffffffff, -1, 0},

      {0x040f, 1031, 1},

      {0xff30, 3204, 1},

      {0x1f0f, 2103, 1},

      {0x2c0f, 2466, 1},

      {0x1040d, 3276, 1},
      {0xffffffff, -1, 0},

      {0x040d, 1025, 1},

      {0x0147, 273, 1},

      {0x1f0d, 2097, 1},

      {0x2c0d, 2460, 1},

      {0x1040b, 3270, 1},

      {0x0047, 18, 1},

      {0x040b, 1019, 1},

      {0x0230, 571, 1},

      {0x1f0b, 2091, 1},

      {0x2c0b, 2454, 1},

      {0x10409, 3264, 1},

      {0x10405, 3252, 1},

      {0x0409, 1013, 1},

      {0x0405, 1001, 1},

      {0x1f09, 2085, 1},

      {0x2c09, 2448, 1},

      {0x2c05, 2436, 1},

      {0x10403, 3246, 1},

      {0x10401, 3240, 1},

      {0x0403, 995, 1},

      {0x0401, 989, 1},
      {0xffffffff, -1, 0},

      {0x2c03, 2430, 1},

      {0x2c01, 2424, 1},

      {0x13f9, 1691, 1},

      {0x042f, 983, 1},
      {0xffffffff, -1, 0},

      {0x1f2f, 2145, 1},

      {0x1041f, 3330, 1},
      {0xffffffff, -1, 0},

      {0x041f, 935, 1},

      {0x023d, 378, 1},

      {0x10411, 3288, 1},

      {0x2c1f, 2514, 1},

      {0x0411, 893, 1},

      {0x0547, 1400, 1},
      {0xffffffff, -1, 0},

      {0x2c11, 2472, 1},

      {0x10407, 3258, 1},
      {0xffffffff, -1, 0},

      {0x0407, 1007, 1},

      {0x24c1, 2376, 1},
      {0xffffffff, -1, 0},

      {0x2c07, 2442, 1},
      {0xffffffff, -1, 0},

      {0x13f8, 1688, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff39, 3231, 1},
      {0xffffffff, -1, 0},

      {0x0243, 354, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0x0241, 586, 1},

      {0xff29, 3183, 1},

      {0x023b, 577, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff27, 3177, 1},

      {0xff25, 3171, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff23, 3165, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff21, 3159, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0},

      {0xfb17, 117, 2},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xff2f, 3201, 1},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xfb15, 113, 2},

      {0xfb13, 121, 2},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},
      {0xffffffff, -1, 0},

      {0xfb05, 29, 2},
      {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0}, {0xffffffff, -1, 0},

      {0xfb03, 0, 3},

      {0xfb01, 8, 2}
    };

  if (0 == 0)
    {
      int key = hash(&code);

      if (key <= MAX_HASH_VALUE && key >= 0)
         {
           OnigCodePoint gcode = wordlist[key].code;
 
          if (code == gcode)
             return &wordlist[key];
         }
     }
  return 0;
}
