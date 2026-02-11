#pragma once                                                                  
                                                                                
#include <assert.h>                                                           
#include <stdio.h>
#include <stdlib.h>
#include <string.h>                                                            
#include <stdint.h>
                                                             
#define TEST(name) static void test_##name(void)                              
#define RUN(name) do { printf("    " #name "..."); test_##name(); printf("OK\n"); } while(0) 