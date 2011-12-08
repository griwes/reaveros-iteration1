#include "initrd.h"
#include "physmem.h"
#include "screen.h"
#include "string.h"
#include "booter.h"

void InitRD::InitRD::Initialize(int pInitrd)
{
    this->m_iFiles = *((int *)pInitrd);
    this->m_iTimestamp = *(unsigned int *)(pInitrd + 4);
    this->m_pFiles = (File *)PhysMemory::Manager::Place(sizeof(File) * this->m_iFiles);
    
    Screen::kout->Print("InitRD built on ");
    
    // small explanation here. this is the only place in entire Booter
    // where an actual date from timestamp is displayed, so don't shout at me
    // for this code being put in InitRD's Initialize().
    
    unsigned int years = this->m_iTimestamp / 31436000;
    unsigned int leaps = years / 4;
    unsigned int not_leaps = (years + 1970) / 2000;
    unsigned int days_since_epoch = this->m_iTimestamp / 86400 - leaps + not_leaps;
    unsigned int days = days_since_epoch % 365;
    
    unsigned int month = 1;
    unsigned int months[12] = {
        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31
    };
    
    char * month_names[12] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    for (int i = 0; i < 12; i++)
    {
        if (months[i] >= days)
        {
            break;
        }

        days -= months[i];
        month++;
    }
    
    unsigned int seconds = this->m_iTimestamp - (days_since_epoch + leaps - not_leaps) * 86400;
    unsigned int hours = seconds / 3600;
    seconds -= hours * 3600;
    unsigned int minutes = seconds / 60;
    seconds -= minutes * 60;

    Screen::kout->Print(days)->Print(" of ");
    Screen::kout->Print(month_names[month - 1])->Print(' ');
    Screen::kout->Print(1970 + years)->Print(", ");
    if (hours < 10) Screen::kout->Print('0');
    Screen::kout->Print(hours)->Print(':');
    if (minutes < 10) Screen::kout->Print('0');
    Screen::kout->Print(minutes)->Print(':');
    if (seconds < 10) Screen::kout->Print('0');
    Screen::kout->Print(seconds);

    char * image = reinterpret_cast<char *>(pInitrd);
    
    this->m_iFiles = *((int *)image);
    if (this->m_iFiles == 0)
    {
        Booter::Panic("\n\nEmpty initrd found!");
    }
        
    this->m_pFiles = (::InitRD::File *)PhysMemory::Manager::Place(sizeof(::InitRD::File) * this->m_iFiles);
    
    image += 512;
    
    for (int i = 0; i < this->m_iFiles; i++)
    {
        this->m_pFiles[i].Initialize(image);
        Screen::kout->Print("\nFile \"")->Print(this->m_pFiles[i].GetFilename())->Print("\" found in initrd...");
    }
}

InitRD::File * InitRD::InitRD::GetFile(int index)
{
    if (index > this->m_iFiles - 1)
    {
        Booter::Panic("Tried to access file above last initrd file!");
    }
    return &this->m_pFiles[index];
}

InitRD::File * InitRD::InitRD::GetFile(const char * filename)
{
    for (int i = 0; i < this->m_iFiles; i++)
    {
        if (Compare(filename, this->m_pFiles[i].GetFilename()))
        {
            Screen::kout->Print("\nFile ")->Print(filename)->Print(" found.\n");
            return &this->m_pFiles[i];
        }
    }
    
    return (::InitRD::File *)0;
}

char * InitRD::File::GetContent()
{
    return this->m_pContent;
}

char * InitRD::File::GetFilename()
{
    return this->m_pContent - 512;
}

int InitRD::File::GetSize()
{
    return this->m_iLength;
}

void InitRD::File::Initialize(char * file)
{
    this->m_pContent = file + 512;
    this->m_iLength = *(int *)(file + 508);
    *(this->m_pContent + 508) = 0; // ensure that filename ends with '\0'
}
