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

#pragma once

namespace acpi
{
    struct address_structure
    {
        uint8_t address_space_id;
        uint8_t register_bit_width;
        uint8_t register_bit_offset;
        uint8_t reserved;
        phys_addr_t address;
    } __attribute__((packed));

    struct rsdp
    {
        char signature[8];
        uint8_t checksum;
        char oemid[6];
        uint8_t revision;
        phys_addr32_t rsdt_ptr;
        uint32_t length;
        phys_addr_t xsdt_ptr;
        uint8_t ext_checksum;
        uint8_t reserved[3];

        bool validate()
        {
            if (signature[0] == 'R' && signature[1] == 'S' && signature[2] == 'D' && signature[3] == ' '
                && signature[4] == 'P' && signature[5] == 'T' && signature[6] == 'R' && signature[7] == ' ')
            {
                uint8_t checksum = 0;

                for (uint8_t i = 0; i < sizeof(rsdp); ++i)
                {
                    checksum += *(reinterpret_cast<uint8_t *>(this) + i);
                }

                return !checksum;
            }

            return false;
        }
    } __attribute__((packed));

    struct description_table_header
    {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oemid[6];
        uint64_t oem_tableid;
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;

        bool validate(const char * sign)
        {
            if (signature[0] == sign[0] && signature[1] == sign[1] && signature[2] == sign[2] && signature[3] == sign[3])
            {
                uint8_t checksum = 0;

                for (uint32_t i = 0; i < length; ++i)
                {
                    checksum += *(reinterpret_cast<uint8_t *>(this) + i);
                }

                return !checksum;
            }

            return false;
        }
    } __attribute__((packed));

    struct rsdt : public description_table_header
    {
        phys_addr32_t entries[1];
    } __attribute__((packed));

    struct xsdt : public description_table_header
    {
        phys_addr_t entries[1];
    } __attribute__((packed));

    struct madt_lapic_entry
    {
        uint8_t acpi_id;
        uint8_t apic_id;
        uint32_t flags;
    } __attribute__((packed));

    struct madt_ioapic_entry
    {
        uint8_t apic_id;
        uint8_t reserved;
        phys_addr32_t base_address;
        uint32_t base_int;
    } __attribute__((packed));

    struct madt_int_override_entry
    {
        uint8_t bus;
        uint8_t source;
        uint32_t int_number;
        uint16_t flags;
    } __attribute__((packed));

    struct madt_nmi_source_entry
    {
        uint16_t flags;
        uint32_t int_number;
    } __attribute__((packed));

    struct madt_lapic_nmi_entry
    {
        uint8_t acpi_id;
        uint16_t flags;
        uint8_t int_number;
    } __attribute__((packed));

    struct madt_lapic_address_override_entry
    {
        uint16_t reserved;
        phys_addr_t base_address;
    } __attribute__((packed));

    struct madt_x2apic_entry
    {
        uint16_t reserved;
        uint32_t x2apic_id;
        uint32_t flags;
        uint32_t acpi_uuid;
    } __attribute__((packed));

    struct madt_x2apic_nmi_entry
    {
        uint16_t flags;
        uint32_t acpi_uuid;
        uint8_t int_number;
        uint8_t reserved;
        uint16_t reserved2;
    } __attribute__((packed));

    struct madt_entry
    {
        uint8_t type;
        uint8_t length;
    } __attribute__((packed));

    struct madt : public description_table_header
    {
        phys_addr32_t lic_address;
        uint32_t flags;
        madt_entry entries[1];
    } __attribute__((packed));

    struct hpet : public description_table_header
    {
        uint8_t hardware_rev_id;
        uint8_t comparator_count:5;
        uint8_t counter_size:1;
        uint8_t reserved:1;
        uint8_t legacy_replacement:1;
        pci_vendor_t pci_vendor_id;
        address_structure address;
        uint8_t hpet_number;
        uint16_t minimum_tick;
        uint8_t page_protection;
    } __attribute__((packed));
}
