#ifndef MEMORY_UTILS_HPP
#define MEMORY_UTILS_HPP

#include <malloc/malloc.h>
#include <iostream>
#include <mach/mach.h>

inline void display_malloc_usage() {
    malloc_statistics_t stats;
    malloc_zone_statistics(nullptr, &stats);

    std::cout << "malloc: " << stats.size_in_use / 1024.0 / 1024.0
              << " MB in use, "
              << stats.max_size_in_use / 1024.0 / 1024.0
              << " MB peak\n";
}

inline void display_phys_footprint() {
    task_vm_info_data_t info;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO,
                  reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS) {
        std::cout << "Physical footprint: "
                  << info.phys_footprint / 1024.0 / 1024.0 << " MB\n";
    }
}

#endif