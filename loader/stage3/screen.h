#ifndef __screen_h__
#define __screen_h__

namespace Screen
{
    static void Initialize();

    // this is better than stdlib's way...
    enum ConsoleFlags
    {
        iHex,
        iDec,
        iBin,
        iOct
    };
    
    ConsoleFlags hex = ConsoleFlags::iHex;
    ConsoleFlags dec = ConsoleFlags::iDec;
    ConsoleFlags bin = ConsoleFlags::iBin;
    ConsoleFlags oct = ConsoleFlags::iOct;
    
    class Console
    {
    public:
        friend Console & operator<<(Console &, char);
        friend Console & operator<<(Console &, char *);
        friend Console & operator<<(Console &, long int);
        friend Console & operator<<(Console &, long int *);
        friend Console & operator<<(Console &, int);
        friend Console & operator<<(Console &, int *);
        friend Console & operator<<(Console &, double);
        friend Console & operator<<(Console &, double *);
        friend Console & operator<<(Console &, void *);
        friend Console & operator<<(Console &, ConsoleFlags);
        void Initialize();
        void Scroll();
        void MoveCursor();
    private:
        int m_iX, m_iY;
        static int s_iMaxX, s_iMaxY;
        volatile static char * s_pScreenMemory;
        char m_iAttrib;
        ConsoleFlags m_eMode;
    };
    
    Console * kout = 0;
}

#endif