/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2013 Michał "Griwes" Dominiak
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 **/

#include <memory/x64paging.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <processor/processor.h>
#include <screen/screen.h>
#include <memory/vm.h>

namespace
{
    class address_generator
    {
    public:
        address_generator(uint64_t id) : _id(id)
        {
            if (_id < 256)
            {
                PANIC("Tried to create address generator for lower half PML4 recursive entry");
            }
        }

        memory::x64::pml4 * pml4()
        {
            return (memory::x64::pml4 *)pt(256, 256, 256);
        }

        memory::x64::pdpt * pdpt(uint64_t pml4e)
        {
            return (memory::x64::pdpt *)pt(256, 256, pml4e);
        }

        memory::x64::page_directory * pd(uint64_t pml4e, uint64_t pdpte)
        {
            return (memory::x64::page_directory *)pt(256, pml4e, pdpte);
        }

        memory::x64::page_table * pt(uint64_t pml4e, uint64_t pdpte, uint64_t pde)
        {
            return (memory::x64::page_table *)(0xFFFF000000000000 + (_id << 39) + (pml4e << 30) + (pdpte << 21) + (pde << 12));
        }

    private:
        uint64_t _id;
    };
}

void memory::x64::invlpg(virt_addr_t addr)
{
    asm volatile ("invlpg (%0)" :: "r"(addr) : "memory");

    // scheduler::invlpg(addr);
}

void memory::x64::map(virt_addr_t virtual_start, virt_addr_t virtual_end, phys_addr_t physical_start, vm::attributes attrib)
{
    screen::debug("\nMapping ", virtual_start, "-", virtual_end, " to ", physical_start, attrib.foreign ? " for foreign VAS" : "");

    address_generator gen{ attrib.foreign ? 257u : 256u };

    if (virtual_start >= virt_addr_t{ 0xFFFF800000000000 } && virtual_start < virt_addr_t{ 0xFFFF800000000000 } + 2ull * 512 * 1024 * 1024 * 1024)
    {
        PANIC("Trying to map something in paging structs area");
    }

    virtual_start &= ~4095ull;
    virtual_end += 4095;
    virtual_end &= ~4095ull;

    if (virtual_end <= virtual_start)
    {
        return;
    }

    uint64_t startpml4e = (virtual_start >> 39) & 511;
    uint64_t startpdpte = (virtual_start >> 30) & 511;
    uint64_t startpde = (virtual_start >> 21) & 511;
    uint64_t startpte = (virtual_start >> 12) & 511;

    uint64_t endpml4e = (virtual_end >> 39) & 511;
    uint64_t endpdpte = (virtual_end >> 30) & 511;
    uint64_t endpde = (virtual_end >> 21) & 511;
    uint64_t endpte = (virtual_end >> 12) & 511;

    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {
        auto _ = gen.pml4()->entries[startpml4e].lock();

        pdpt * table = gen.pdpt(startpml4e);

        if (!gen.pml4()->entries[startpml4e].present)
        {
            gen.pml4()->entries[startpml4e] = memory::pmm::pop();
            new (gen.pdpt(startpml4e)) pdpt{};

            invlpg(table);
        }

        gen.pml4()->entries[startpml4e].user = attrib.user;
        gen.pml4()->entries[startpml4e].read_write = attrib.read_write;

        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            auto _ = (*table)[startpdpte].lock();

            page_directory * pd = gen.pd(startpml4e, startpdpte);

            if (!(*table)[startpdpte].present)
            {
                (*table)[startpdpte] = memory::pmm::pop();
                new (gen.pd(startpml4e, startpdpte)) page_directory{};

                invlpg(pd);
            }

            (*table)[startpdpte].user = attrib.user;
            (*table)[startpdpte].read_write = attrib.read_write;

            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                auto _ = (*pd)[startpde].lock();

                page_table * pt = gen.pt(startpml4e, startpdpte, startpde);

                if (!(*pd)[startpde].present)
                {
                    (*pd)[startpde] = memory::pmm::pop();
                    new (gen.pt(startpml4e, startpdpte, startpde)) page_table{};

                    invlpg(pt);
                }

                (*pd)[startpde].user = attrib.user;
                (*pd)[startpde].read_write = attrib.user;

                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    auto _ = (*pt)[startpte].lock();

                    if ((*pt)[startpte].present && physical_start != (*pt)[startpte].address << 12)
                    {
                        screen::print("\nAddress: ", (void *)virtual_start);
                        PANIC("Tried to map something at already mapped page");
                    }

                    (*pt)[startpte] = physical_start;
                    (*pt)[startpte].user = attrib.user;
                    (*pt)[startpte].read_write = attrib.read_write;
                    ++startpte;

                    invlpg(virtual_start);

                    physical_start += 4096;
                    virtual_start += 4096;
                }

                if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
                {
                    startpde++;
                    startpte = 0;
                }

                else
                {
                    return;
                }
            }

            if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
            {
                startpde = 0;
                startpdpte++;
            }

            else
            {
                return;
            }
        }

        if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
        {
            startpdpte = 0;
            startpml4e++;
        }

        else
        {
            return;
        }
    }
}

phys_addr_t memory::x64::get_physical_address(virt_addr_t addr, bool foreign)
{
    address_generator gen{ foreign ? 257u : 256u };

    auto _ = gen.pml4()->entries[(addr >> 39) & 511].lock();

    if (gen.pml4()->entries[(addr >> 39) & 511].present)
    {
        if (gen.pdpt((addr >> 39) & 511)->entries[(addr >> 30) & 511].present)
        {
            if (gen.pd((addr >> 39) & 511, (addr >> 30) & 511)->entries[(addr >> 21) & 511].present)
            {
                if (gen.pt((addr >> 39) & 511, (addr >> 30) & 511, (addr >> 21) & 511)->entries[(addr >> 12) & 511].present)
                {
                    return phys_addr_t { gen.pt((addr >> 39) & 511, (addr >> 30) & 511, (addr >> 21) & 511)->entries[(addr >> 12) & 511].address << 12 };
                }
            }
        }
    }

    PANIC("Tried to get physical address of not mapped page");
}

void memory::x64::unmap(virt_addr_t virtual_start, virt_addr_t virtual_end, bool push, bool foreign)
{
    screen::debug("\nUnmapping ", virtual_start, "-", virtual_end, foreign ? " from foreign VAS" : "");

    address_generator gen{ foreign ? 257u : 256u };

    virtual_start &= ~(uint64_t)4095;
    virtual_end += 4095;
    virtual_end &= ~(uint64_t)4095;

    uint64_t startpml4e = (virtual_start >> 39) & 511;
    uint64_t startpdpte = (virtual_start >> 30) & 511;
    uint64_t startpde = (virtual_start >> 21) & 511;
    uint64_t startpte = (virtual_start >> 12) & 511;

    uint64_t endpml4e = (virtual_end >> 39) & 511;
    uint64_t endpdpte = (virtual_end >> 30) & 511;
    uint64_t endpde = (virtual_end >> 21) & 511;
    uint64_t endpte = (virtual_end >> 12) & 511;

    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {
        auto _ = gen.pml4()->entries[startpml4e].lock();

        if (!gen.pml4()->entries[startpml4e].present)
        {
            PANIC("Tried to unmap something not mapped");
        }

        pdpt * table = gen.pdpt(startpml4e);

        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            auto _ = (*table)[startpdpte].lock();

            if (!(*table)[startpdpte].present)
            {
                PANIC("Tried to unmap something not mapped");
            }

            page_directory * pd = gen.pd(startpml4e, startpdpte);

            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                auto _ = (*pd)[startpde].lock();

                if (!(*pd)[startpde].present)
                {
                    PANIC("Tried to unmap something not mapped");
                }

                page_table * pt = gen.pt(startpml4e, startpdpte, startpde);

                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    auto _ = (*pt)[startpte].lock();

                    if (!(*pt)[startpte].present)
                    {
                        PANICEX("tried to unmap not mapped page", [=]{
                            screen::print("At address ", virtual_start);
                        });
                    }

                    (*pt)[startpte].present = 0;
                    invlpg(virtual_start);

                    if (push)
                    {
                        memory::pmm::push(phys_addr_t{ (*pt)[startpte].address << 12 });
                    }

                    ++startpte;

                    virtual_start += 4096;
                }

                if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
                {
                    startpde++;
                    startpte = 0;
                }

                else
                {
                    return;
                }
            }

            if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
            {
                startpde = 0;
                startpdpte++;
            }

            else
            {
                return;
            }
        }

        if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
        {
            startpdpte = 0;
            startpml4e++;
        }

        else
        {
            return;
        }
    }
}

namespace
{
    utils::spinlock _lock;
}

phys_addr_t memory::x64::clone_kernel() // kernel shall use only one set of paging structures
{
    static virt_addr_t temp;

    INTL();
    LOCK(_lock);

    if (!temp)
    {
        temp = vm::allocate_address_range(4096);
    }

    address_generator current{ 256 };
    address_generator gen{ 257 };

    auto pml4_frame = memory::pmm::pop();

    vm::map(temp, pml4_frame);
    memory::zero(static_cast<uint8_t *>(temp), 4096);
    vm::unmap(temp, temp + 4096, false);

    set_foreign(pml4_frame);

    // 256 + 2 for recursive entries
    for (uint64_t startpml4e = 258; startpml4e < 512; ++startpml4e)
    {
        gen.pml4()->entries[startpml4e] = current.pml4()->entries[startpml4e];
    }

    return pml4_frame;
}

void memory::x64::set_foreign(phys_addr_t frame)
{
    address_generator current{ 256 };

    while (__sync_fetch_and_or((uint64_t *)&current.pml4()->entries[257], 1) & 1)
    {
        asm volatile ("pause");
    }

//    scheduler::disable();

    current.pml4()->entries[257] = frame;

    processor::reload_cr3();

    current.pdpt(257)->entries[256] = frame;
}

void memory::x64::release_foreign()
{
    address_generator current{ 256 };

    current.pml4()->entries[257] = phys_addr_t{};
    current.pml4()->entries[257].present = 0;

    processor::reload_cr3();

//    scheduler::enable();
}

// the function below may fail if some lock is acquired by different core while it is running
// but it doesn't really matter; its only reason to exist is to allow frame_stack's pop() not call
// _shrink() if it is called from mapping call that does something "near" the stack's region
bool memory::x64::locked(uint64_t address)
{
    address_generator current{ 256 };

    uint64_t pml4e = (address >> 39) & 511;
    uint64_t pdpte = (address >> 30) & 511;
    uint64_t pde = (address >> 21) & 511;

    return (current.pml4()->entries[pml4e].ignored2 & 1) || (current.pdpt(pml4e)->entries[pdpte].ignored2 & 1)
        || (current.pd(pml4e, pdpte)->entries[pde].ignored2 & 1);
}

// clears mappings in lower half and pushes paging structures' addresses, but doesn't
// push mapped frames, that may be used somewhere in current paging structures
void memory::x64::drop_bootloader_mapping(bool push)
{
    address_generator gen{ 256 };

    if (!push)
    {
        for (uint64_t pml4e = 0; pml4e < 256; ++pml4e)
        {
            gen.pml4()->entries[pml4e].present = 0;
        }

        return;
    }

    for (uint64_t pml4e = 0; pml4e < 256; ++pml4e)
    {
        if (!gen.pml4()->entries[pml4e].present)
        {
            continue;
        }

        for (uint64_t pdpte = 0; pdpte < 512; ++pdpte)
        {
            if (!gen.pdpt(pml4e)->entries[pdpte].present)
            {
                continue;
            }

            for (uint64_t pde = 0; pde < 512; ++pde)
            {
                if (!gen.pd(pml4e, pdpte)->entries[pde].present)
                {
                    continue;
                }

                pmm::push(phys_addr_t{ gen.pd(pml4e, pdpte)->entries[pde].address << 12 });
                gen.pd(pml4e, pdpte)->entries[pde].present = 0;
            }

            pmm::push(phys_addr_t{ gen.pdpt(pml4e)->entries[pdpte].address << 12 });
            gen.pdpt(pml4e)->entries[pdpte].present = 0;
        }

        pmm::push(phys_addr_t{ gen.pml4()->entries[pml4e].address << 12 });
        gen.pml4()->entries[pml4e].present = 0;
    }

    processor::reload_cr3();
}
