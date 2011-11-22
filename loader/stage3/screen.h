#ifndef __screen_h__
#define __screen_h__

// namespace holding screen operations
namespace Screen
{
    // get's memory for kout
    void Initialize();

    enum ConsoleFlags
    {
        iHex,
        iDec,
        iBin,
        iOct
    };
    
    // main class
    class Console
    {
    public:
        // number of Print functions for given types
        // most of them will never be used, but who cares
        Console * Print(char);
        Console * Print(char *);
        Console * Print(unsigned long long int);
        Console * Print(unsigned long long int *);
        Console * Print(unsigned int);
        Console * Print(unsigned int *);
        Console * Print(double);
        Console * Print(double *);
        Console * Print(void *);
        // I don't like constructors, because...
        // I still haven't written placement new!
        void Initialize();
        void Scroll();
        void MoveCursor();
        void Clear();
        void Bin() { this->m_eMode = iBin; }
        void Hex() { this->m_eMode = iHex; }
        void Dec() { this->m_eMode = iDec; }
        void Oct() { this->m_eMode = iOct; }
        // actual screen positions
        int m_iX, m_iY;
        // maximal screen positions
        int m_iMaxX, m_iMaxY;
        // screen memory (cpt obvious)
        volatile char * m_pScreenMemory;
        // attribute (bg and text color)
        char m_iAttrib;
        Screen::ConsoleFlags m_eMode;
    };
    
    extern Console * kout;
}

#endif