#pragma once

#include <cstdint>
#include <cstddef>

namespace screen
{
    class boot_mode
    {
    public:
        uint16_t mode_attribs;
        uint8_t window_a_attribs;
        uint8_t window_b_attribs;
        uint16_t window_granularity;
        uint16_t window_size;
        uint16_t window_a_seg;
        uint16_t window_b_seg;
        uint16_t window_func_ptr_off;
        uint16_t window_func_ptr_seg;
        uint16_t bytes_per_line;
        
        uint16_t resolution_x;
        uint16_t resolution_y;
        uint8_t char_x;
        uint8_t char_y;
        uint8_t number_of_planes;
        uint8_t bpp;
        uint8_t banks_num;
        uint8_t memory_model;
        uint8_t bank_size;
        uint8_t number_of_image_pages;
        uint8_t _;
        uint8_t red_size, red_pos;
        uint8_t green_size, green_pos;
        uint8_t blue_size, blue_pos;
        uint8_t rsvd_size, rsvd_pos;
        uint8_t direct_color_info;
        
        uint32_t addr;
        uint32_t __;
        uint16_t ___;
        
        uint16_t linear_bytes_per_line;
        uint8_t bank_num_of_image_pages;
        uint8_t linear_number_of_image_pages;
        uint8_t linear_red_size, linear_red_pos;
        uint8_t linear_green_size, linear_green_pos;
        uint8_t linear_blue_size, linear_blue_pos;
        uint8_t linear_rsvd_size, linear_rsvd_pos;
        uint32_t max_pixel_clock;
        
        uint8_t ____[189];
    } __attribute__((packed));
        
    class mode
    {
    public:
        mode(::screen::boot_mode * boot_mode) : addr(boot_mode->addr), resolution_x(boot_mode->resolution_x),
            resolution_y(boot_mode->resolution_y), bytes_per_line(boot_mode->linear_bytes_per_line ?
            boot_mode->linear_bytes_per_line : boot_mode->bytes_per_line), bpp(boot_mode->bpp), 
            red_size(boot_mode->linear_red_size ? boot_mode->linear_red_size : boot_mode->red_size), 
            red_pos(boot_mode->linear_red_pos ? boot_mode->linear_red_pos : boot_mode->red_pos), 
            green_size(boot_mode->linear_green_size ? boot_mode->linear_green_size : boot_mode->green_size),
            green_pos(boot_mode->linear_green_pos ? boot_mode->linear_green_pos : boot_mode->green_pos), 
            blue_size(boot_mode->linear_blue_size ? boot_mode->linear_blue_size : boot_mode->blue_size), 
            blue_pos(boot_mode->linear_blue_pos ? boot_mode->linear_blue_pos: boot_mode->blue_pos),
            rsvd_size(boot_mode->linear_rsvd_size ? boot_mode->linear_rsvd_size : boot_mode->rsvd_size),
            rsvd_pos(boot_mode->linear_rsvd_pos ? boot_mode->linear_rsvd_pos : boot_mode->rsvd_pos)
        {
        }
        
        uint32_t addr;
        uint16_t resolution_x;
        uint16_t resolution_y;
        uint16_t bytes_per_line;
        
        uint8_t bpp;
        uint8_t red_size, red_pos;
        uint8_t green_size, green_pos;
        uint8_t blue_size, blue_pos;
        uint8_t rsvd_size, rsvd_pos;
    } __attribute__((packed));
}
