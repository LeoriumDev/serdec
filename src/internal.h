#pragma once

#include <serdec/serdec.h>
#define SERDEC_MAGIC_BUFFER 0x5EDEC00B
#define SERDEC_MAGIC_ARENA  0x5EDEC00A
#define SERDEC_MAGIC_FREED  0xDEADBEEF  // Set on free

#define SERDEC_DEFAULT_BUFFER_CAPACITY 100

#ifdef _WIN32                                                                 
    #include <malloc.h>                                                       
    #define serdec_aligned_alloc(align, size) _aligned_malloc(size, align)    
    #define serdec_aligned_free(ptr)          _aligned_free(ptr)              
#else                                                                         
    #include <stdlib.h>                                                       
    static inline void* serdec_aligned_alloc(size_t align, size_t size) {     
        void* ptr = NULL;                                                     
        posix_memalign(&ptr, align, size);                                    
        return ptr;                                                           
    }                                                                         
    #define serdec_aligned_free(ptr) free(ptr)                                
#endif

struct SerdecBuffer {
    uint32_t magic;           // 0x5EDEC001 for validation
    uint32_t ref_count;
    char *data;               // 64-byte aligned
    size_t size;
    size_t capacity;          // size + padding
};
