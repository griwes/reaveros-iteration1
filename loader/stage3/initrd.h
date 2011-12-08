#ifndef __initrd_h__
#define __initrd_h__

namespace InitRD
{
    class File
    {
    public:
        void Initialize(char *);
        char * GetContent();
        char * GetFilename();
        int GetSize();
    private:
        int m_iLength;
        char * m_pContent;      
    };

    class InitRD
    {   
    public:
        void Initialize(int);
        File * GetFile(int);
        File * GetFile(const char *);
    private:
        unsigned int m_iTimestamp;
        int m_iFiles;
        File * m_pFiles;
    };
}

#endif