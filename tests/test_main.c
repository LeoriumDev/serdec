#include <stdio.h>
#include <string.h>

int test_buffer(void);
int test_arena(void);
int test_error(void);
int test_lexer(void);
int test_utf8(void);
int test_string(void);

static int run_all(void) {
      int fail = 0;
      fail |= test_buffer();
      fail |= test_arena();
      fail |= test_error();
      fail |= test_lexer();
      fail |= test_utf8();
      fail |= test_string();
      return fail;
  }


int main(int argc, char** argv) {                                             
    printf("serdec tests:\n\n");                                              
                                                                                
    if (argc == 1) {                                                          
        return run_all();                                                     
    }                                                                         
                                                                                
    const char* name = argv[1];                                               
    if (strcmp(name, "buffer") == 0) return test_buffer();                    
    if (strcmp(name, "arena") == 0) return test_arena();
    if (strcmp(name, "error") == 0) return test_error();
    if (strcmp(name, "lexer") == 0) return test_lexer();
    if (strcmp(name, "utf8") == 0) return test_utf8();
    if (strcmp(name, "string") == 0) return test_string();
    if (strcmp(name, "all") == 0) return run_all();                           
                                                                                
    fprintf(stderr, "Unknown: %s\n", name);                                   
    return 2;                                                                 
}
