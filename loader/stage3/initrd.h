#ifndef __initrd_h__
#define __initrd_h__

namespace InitRD
{
    class File
    {
    public:
        void Initialize();
        char * GetContent();
        char * GetFilename();
    private:
        int m_iLength;
        char * filename;
        char * m_pContent;      
    };

    class InitRD
    {   
    public:
        void Initialize(int);
        File * GetFile(int);
        File * GetFile(char *);
    private:
        unsigned int m_iTimestamp;
        int m_iFiles;
        File * m_pFiles;
    };
}

#endif