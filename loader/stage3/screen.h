#ifndef __screen_h__
#define __screen_h__

namespace Screen
{
    void Initialize();

    // this is better than stdlib's way...
    enum ConsoleFlags
    {
        iHex,
        iDec,
        iBin,
        iOct
    };
    
    class Console
    {
    public:
        Console * Print(char);
        Console * Print(char *);
        Console * Print(long int);
        Console * Print(long int *);
        Console * Print(int);
        Console * Print(int *);
        Console * Print(double);
        Console * Print(double *);
        Console * Print(void *);
        void Initialize();
        void Scroll();
        void MoveCursor();
        void Clear();
        void Bin() { this->m_eMode = iBin; }
        void Hex() { this->m_eMode = iHex; }
        void Dec() { this->m_eMode = iDec; }
        void Oct() { this->m_eMode = iOct; }
        int m_iX, m_iY;
        int m_iMaxX, m_iMaxY;
        volatile char * m_pScreenMemory;
        char m_iAttrib;
        Screen::ConsoleFlags m_eMode;
    };
    
    extern Console * kout;
}

#endif