#include <stdio.h>
    
int main()
{
    int errors = 0;

    //////////////////////////////////////////////////////////////////
    ///// Sanity check of XIF handshakes using CSR instructions //////
    //////////////////////////////////////////////////////////////////

    // Set the vector length to 8 elements with SEW=8b
    unsigned int vl_csrr, vl_vset;
    asm volatile("vsetivli %0, 8, e8\n" : "=r"(vl_vset));
    if (vl_vset != 8){
      printf("Error 1: vsetivli returned %d, expected 8\n", vl_vset);
      errors++;
    }
    asm volatile("csrr %0, vl\n" : "=r"(vl_csrr));
    if (vl_csrr != 8){
      printf("Error 2: vl CSR returned %d, expected 8\n", vl_csrr);
      errors++;
    }
    

    /////////////////////////////////////////////////////////////
    ///// Vector Addition test with four 32-bit elements ////////
    /////////////////////////////////////////////////////////////

    // Initialize integer arrays in memory
    volatile int a[4] = {12, 923, -123, 0};
    volatile int b[4] = {-4, 88, -555, 1};
    volatile int c[4];

    // Set the vector length to 4elements with SEW=32b
    asm volatile("vsetivli %0, 4, e32\n" : "=r"(vl_vset));
    if (vl_vset != 4){
      printf("Error 3: vsetivli returned %d, expected 4\n", vl_vset);
      errors++;
    }

    // Load the arrays into vector registers
    asm volatile("vle32.v v0, (%0)\n" : : "r" (a));
    asm volatile("vle32.v v1, (%0)\n" : : "r" (b));

    // Add the two vectors
    asm volatile("vadd.vv v2, v0, v1");

    // Store the result back into the c array
    asm volatile("vse32.v v2, (%0)\n" : : "r" (c));

    // Dummy wait for the result to be written back to memory
    for (int i = 0; i < 20; i++){
        asm volatile("nop");
    }

    // Check the result
    for (int i = 0; i < 4; i++){
      if (c[i] != a[i] + b[i]){
        printf("Error: element %d of c is %d, expected %d\n", i, c[i], a[i] + b[i]);
        errors++;
      }
    }

    /////////////////////////////////////////////////////////////
    ///// Vector Addition test with eight 16-bit elements ///////
    /////////////////////////////////////////////////////////////

    // Initialize integer arrays in memory. Make sure to test overflow in both directions
    volatile __int16_t d[8] = {55, 3200, 32767, 32767, -32768, -324, -3266, -3270};
    volatile __int16_t e[8] = {3444, 32767, 1, 37, -768, 324, -3266, -3270};
    volatile __int16_t f[8];

    // Set the vector length to 8 elements with SEW=16b
    asm volatile("vsetivli %0, 8, e16\n" : "=r"(vl_vset));
    if (vl_vset != 8){
      printf("Error 4: vsetivli returned %d, expected 8\n", vl_vset);
      errors++;
    }

    // Load the arrays into vector registers
    asm volatile("vle16.v v0, (%0)\n" : : "r" (d));
    asm volatile("vle16.v v1, (%0)\n" : : "r" (e));

    // Add the two vectors
    asm volatile("vadd.vv v2, v0, v1");

    // Store the result back into the f array
    asm volatile("vse16.v v2, (%0)\n" : : "r" (f));

    // Dummy wait for the result to be written back to memory
    for (int i = 0; i < 20; i++){
        asm volatile("nop");
    }

    // Check the result
    for (int i = 0; i < 8; i++){
      if (f[i] != (__int16_t)(d[i] + e[i])){
        printf("Error: element %d of f is %d, expected %d\n", i, f[i], d[i] + e[i]);
        errors++;
      }
    }

    /////////////////////////////////////////////////////////////
    ///// Vector Addition test with four 8-bit elements //////
    /////////////////////////////////////////////////////////////

    // Initialize integer arrays in memory
    volatile __int8_t x[4] = {12, 92, -123, 0};
    volatile __int8_t y[4] = {-4, 66, -100, 1};
    volatile __int8_t z[4];

    // Set the vector length to 4 elements with SEW=8b
    asm volatile("vsetivli %0, 4, e8\n" : "=r"(vl_vset));
    if (vl_vset != 4){
      printf("Error 3: vsetivli returned %d, expected 16\n", vl_vset);
      errors++;
    }

    // Load the arrays into vector registers
    asm volatile("vle8.v v1, (%0)\n" : : "r" (x));
    asm volatile("vle8.v v2, (%0)\n" : : "r" (y));

    // Add the two vectors
    asm volatile("vadd.vv v3, v1, v2");

    // Store the result back into the c array
    asm volatile("vse8.v v3, (%0)\n" : : "r" (z));

    // Dummy wait for the result to be written back to memory
    for (int i = 0; i < 20; i++){
        asm volatile("nop");
    }

    // Check the result
    for (int i = 0; i < 4; i++){
      if (z[i] != (__int8_t)(x[i] + y[i])){
          errors++;
          printf("Error: element %d of z is %d, expected %d\n", i, z[i], x[i] + y[i]);
      }
    }

    ////////////////////////////////////////////////////////////
    ///// Testing the priority of the data memory arbiter //////
    ////////////////////////////////////////////////////////////
    // Store value of d into e using vector store
    asm volatile("vse16.v v0, (%0)\n" : : "r" (e));
    // NOTE: introduce a NOP because the first store issued by CVE2 happens before VLSU of Spatz starts the vector store.
    // This way, both the scalar and vector memory requests are issued at the same time.
    asm volatile("nop");
    // At the same time edit e using scalar store
    e[0] = 9;
    e[1] = 9;
    e[2] = 9;
    e[3] = 9;
    e[4] = 9;
    e[5] = 9;
    e[6] = 9;
    e[7] = 9;

    // Check the result: the elements of e should be 9. The vector store has priority (executed first). 
    // Later, the scalar store will overwrite the elements of e.
    for (int i = 0; i < 8; i++){
      if (e[i] != 9) {
        errors++;
        printf("Error: element %d of e is %d, expected 9\n", i, e[i]);
      }
    }

    ///////////////////////////////////////
    //////////// Final test check /////////
    ///////////////////////////////////////
    if (errors == 0){
        printf("TEST PASSED!\n");
        return 0;
    } else {
        printf("TEST FAILED! Number of errors: %d\n", errors);
        return 1;
    }
}
