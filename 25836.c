    static void writeTimeAndDate (OutputStream& target, Time t)
    {
        target.writeShort ((short) (t.getSeconds() + (t.getMinutes() << 5) + (t.getHours() << 11)));
        target.writeShort ((short) (t.getDayOfMonth() + ((t.getMonth() + 1) << 5) + ((t.getYear() - 1980) << 9)));
    }