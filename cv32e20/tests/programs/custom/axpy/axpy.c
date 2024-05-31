#include <stdint.h>
#include <stdio.h>

void axpy_v16b(const uint16_t a, const uint16_t *x, const uint16_t *y, unsigned int avl) {
  unsigned int vl;

  // Stripmine and accumulate a partial vector
  do {
    // Set the vl
    asm volatile("vsetvli %0, %1, e16, m1, ta, ma" : "=r"(vl) : "r"(avl));

    // Load vectors
    asm volatile("vle16.v v0, (%0)" ::"r"(x));
    asm volatile("vle16.v v8, (%0)" ::"r"(y));

    // Multiply-accumulate
    asm volatile("vmacc.vx v8, %0, v0" ::"r"(a));

    // Store results
    asm volatile("vse16.v v8, (%0)" ::"r"(y));

    // Bump pointers
    x += vl;
    y += vl;
    avl -= vl;
  } while (avl > 0);
}

int main() {
    // 16 element vectors
    #define AVL 16
    // Create input data
    uint16_t a = 2;
    uint16_t x[AVL] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint16_t y1[AVL] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint16_t y2[AVL] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    // gcc sometimes uses vector instructions to assign the date. So, we allow time before starting the benchmark 
    printf("Starting the benchmark...");
    
    // Issue fence instruction to start benchmark counter
    asm volatile("fence");
    asm volatile("fence");

    // Perform the operation
    axpy_v16b(a, x, y1, AVL);
    
    // Issue fence instruction to stop benchmark counter
    asm volatile("fence");

    // Check the results
    for (int i = 0; i < AVL; i++) {
        if (y1[i] != a*x[i] + y2[i]) {
            return 1;
        }
    }

    return 0;
}