FastHufDecoder::enabled()
{
    #if defined(__INTEL_COMPILER) || defined(__GNUC__)  

        //
        // Enabled for ICC, GCC:
        //       __i386__   -> x86
        //       __x86_64__ -> 64-bit x86
        //

        #if defined (__i386__) || defined(__x86_64__)
            return true;
        #else
            return false;
        #endif

    #elif defined (_MSC_VER)

        //
        // Enabled for Visual Studio:
        //        _M_IX86 -> x86
        //        _M_X64  -> 64bit x86

        #if defined (_M_IX86) || defined(_M_X64)
            return true;
        #else
            return false;
        #endif

    #else

        //
        // Unknown compiler - Be safe and disable.
        //
        return false;
    #endif
}