#include "paging.h"
#include "processor.h"
#include "../../memory/utils.h"

// one note:
// paging is currently enabled, however up to 3GB+48MB it's almost identity mapped
// just substract 0xc0000000
int Arch::x86::Paging::Init(int32 placement, int32 * memmap, int32 memmapsize)
{
    placement += 4095;
    placement &= ~4095;
    
    // PAE paging
    if (Arch::x86::Processor::CheckPAE() == 1)
    {
        Arch::x86::PAEPaging::PageDirectoryPointerTable * pdpt = 
                    reinterpret_cast<Arch::x86::PAEPaging::PageDirectoryPointerTable *>(placement);
        Memory::Zero((char *)pdpt, sizeof(Arch::x86::PAEPaging::PageDirectoryPointerTable));
                    
        pdpt->pdptp = placement - 3 * 1024 * 1024 * 1024;
        
        placement += sizeof(Arch::x86::PAEPaging::PageDirectoryPointerTable);
        placement += 4095;
        placement &= ~4095;
        
        for (int i = 0; i < 4; i++)
        {
            pdpt->pdirs[i] = reinterpret_cast<Arch::x86::PAEPaging::PageDirectory *>(placement);
            Memory::Zero((char *)pdpt->pdirs[i], sizeof(Arch::x86::PAEPaging::PageDirectory));
            pdpt->dirs[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12;
            pdpt->dirs[i].present = (i == 3 || i == 0 ? 1 : 0); // currently, only stack (< 1GB) and kernel (> 3GB) are present...
            
            placement += sizeof(Arch::x86::PAEPaging::PageDirectory);
            placement += 4095;
            placement &= ~4095;
        }
        
        // identity map 0 - 16 MB
        for (int i = 0; i < 8; i++)
        {
            Arch::x86::PAEPaging::PageTable * table = reinterpret_cast<Arch::x86::PAEPaging::PageTable *>(placement);
            Memory::Zero((char *)table, sizeof(Arch::x86::PAEPaging::PageTable));
            
            pdpt->pdirs[0]->pTables[i] = table;
            pdpt->pdirs[0]->tables[i].present = 1;
            pdpt->pdirs[0]->tables[i].writeable = 1;
            pdpt->pdirs[0]->tables[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12;
            
            placement += 4096;
            
            for (int j = 0; j < 512; j++)
            {
                table->entries[j].present = 1;
                table->entries[j].writeable = 1;
                table->entries[j].address = (4 * 1024 * (i * 1024 + j)) >> 12;
            }
        }

        // map some pages to 3 GB - 3 GB 64 MB
        int64 address = 1024 * 1024 * 16; // 16 MB already mapped
        int k = 0;
        
        int64 * memmappae = (int64 *)memmap;
        
        for (int i = 0; i < 32; i++)
        {
            Arch::x86::PAEPaging::PageTable * table = reinterpret_cast<Arch::x86::PAEPaging::PageTable *>(placement);
            Memory::Zero((char *)table, sizeof(Arch::x86::PAEPaging::PageTable));
            
            pdpt->pdirs[4]->pTables[i] = table;
            pdpt->pdirs[4]->tables[i].present = 1;
            pdpt->pdirs[4]->tables[i].writeable = 1;
            pdpt->pdirs[4]->tables[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12; 
            
            placement += 4096;
            
            for (int j = 0; j < 512; j++)
            {
                while (*(memmappae + k * 3 + 2) != 1 ||
                    (*(memmappae + k * 3 + 1) + *(memmappae + k * 3)) <= address)
                {
                    k++;
                    
                    if (k >= memmapsize)
                    {
                        for (;;); // oops, not enough memory and still no console initialized
                                  // not really going to happen
                    }
                    
                    address = *(memmappae + k * 3);
                }
                
                table->entries[j].present = 1;
                table->entries[j].writeable = 1;
                table->entries[j].address = address >> 12;
            }
        }
        
        // this one is mythical function, said to move some part of kernel code
        // to identity mapped page, execute it there and then magically come back
        // here from there
        Arch::x86::Processor::ReloadPagingWithPAE(pdpt->pdptp);
        
        return placement;
    }
    
    // no PAE
    else
    {
        Arch::x86::Paging::PageDirectory * pd = reinterpret_cast<Arch::x86::Paging::PageDirectory *>(placement);
        Memory::Zero((char *)pd, sizeof(Arch::x86::Paging::PageDirectory));
        
        pd->directory = placement - 3 * 1024 * 1024 * 1024;
        
        placement += sizeof(Arch::x86::Paging::PageDirectory);
        placement += 4095;
        placement &= ~4095;
        
        // identity map 0 - 16 MB; doesn't really need to be usable...
        for (int i = 0; i < 4; i++)
        {
            Arch::x86::Paging::PageTable * table = reinterpret_cast<Arch::x86::Paging::PageTable *>(placement);
            Memory::Zero((char *)table, sizeof(Arch::x86::Paging::PageTable));
            
            pd->pTables[i] = table;
            pd->tables[i].present = 1;
            pd->tables[i].writeable = 1;
            pd->tables[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12;
            
            placement += 4096;
            
            for (int j = 0; j < 1024; j++)
            {
                table->entries[j].present = 1;
                table->entries[j].writeable = 1;
                table->entries[j].address = (4 * 1024 * (i * 1024 + j)) >> 12;
            }
        }
        
        // map 64 MB for kernel virtual space
        int32 address = 1024 * 1024 * 16; // 16 MB already mapped
        int k = 0;
        
        for (int i = 0; i < 16; i++)
        {
            Arch::x86::Paging::PageTable * table = reinterpret_cast<Arch::x86::Paging::PageTable *>(placement);
            Memory::Zero((char *)table, sizeof(Arch::x86::Paging::PageTable));
            
            pd->pTables[i] = table;
            pd->tables[i].present = 1;
            pd->tables[i].writeable = 1;
            pd->tables[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12; 
            
            placement += 4096;
            
            for (int j = 0; j < 1024; j++)
            {
                while (*(memmap + k * 6 + 4) != 1 ||
                       (*(memmap + k * 6 + 2) + *(memmap + k * 6)) <= address)
                {
                    k++;
                    
                    if (k >= memmapsize)
                    {
                        for (;;); // oops, not enough memory and still no console initialized
                                  // not really going to happen
                    }
                    
                    address = *(memmap + k * 6);
                }
                
                table->entries[j].present = 1;
                table->entries[j].writeable = 1;
                table->entries[j].address = address >> 12;
            }
        }
        
        Arch::x86::Processor::LoadCR3(pd->directory);
        
        return placement;
    }
}
