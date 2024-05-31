#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// void matmul(char a[AVL][AVL], char b[AVL][AVL], char c[AVL][AVL], int n, int m, int o) {
//   size_t vlmax = __riscv_vsetvlmax_e8m1();
//   for (int i = 0; i < n; ++i) {
//     for (int j = 0; j < m; ++j) {
//       // Consider subset of the row/column if we can't process the whole row/column at once
//       char *ptr_a = &a[i][0];
//       char *ptr_b = &b[j][0];
//       int k = o;
//       vint8m1_t vec_s = __riscv_vmv_v_x_i8m1 (0, vlmax);
//       vint8m1_t vec_zero = __riscv_vmv_v_x_i8m1 (0, vlmax);
//       for (size_t vl; k > 0; k -= vl, ptr_a += vl, ptr_b += vl) { // Process vl elements at a time
//         vl = __riscv_vsetvl_e8m1(k);
//         vint8m1_t vec_a = __riscv_vle8_v_i8m1 (ptr_a, vl);
//         vint8m1_t vec_b = __riscv_vle8_v_i8m1 (ptr_b, vl);
//         vec_s = __riscv_vmacc_vv_i8m1(vec_s, vec_a, vec_b, vl);
//       }
//       vint8m1_t vec_sum;
//       vec_sum = __riscv_vredsum_vs_i8m1_i8m1(vec_s, vec_zero, vlmax);
//       char sum = __riscv_vmv_x_s_i8m1_i8(vec_sum);
//       c[i][j] = sum;
//     }
//   }
// }

// Without intrinsics, using 2D arrays
// void matmul(int8_t a[][], int8_t b[][], int8_t c[][], int AVL) {

//   // Put 0 into v30[0] to use for redsum
//   size_t vl;
//   asm volatile("vsetivli %[vl], 1, e8\n" : [vl] "=r"(vl));
//   asm volatile("vmv.v.i v30, 0");

//   // Loop over the rows of matrix a
//   for (int i = 0; i < AVL; ++i) {  
//     // Loop over the columns of matrix b
//     for (int j = 0; j < AVL; ++j) {
//         // Current vector length
//         size_t vl;      
        
//         // Initialize the result in v4 to 0
//         asm volatile("vsetvli %[vl], %[avl], e8, m1\n" : [vl] "=r"(vl) : [avl] "r" (M));
//         asm volatile("vmv.v.i v4, 0");
      
//         // Consider a vl subset of the row/column assuming we can't process the whole row/column at once
//         char *ptr_a = &a[i][0];
//         char *ptr_b = &b[j][0];

//         for (int k = AVL; k > 0; k -= vl, ptr_a += vl, ptr_b += vl) {
//             // Configure vtype
//             asm volatile("vsetvli %[vl], %[avl], e8, m1\n" : [vl] "=r"(vl) : [avl] "r" (k));
//             // Load part of the row of matrix a
//             asm volatile("vle8.v v0, (%0)\n" : : "r" (ptr_a));
//             // Load part of the column of matrix b
//             asm volatile("vle8.v v2, (%0)\n" : : "r" (ptr_b));
//             // MAC
//             asm volatile("vmacc.vv v4, v0, v2");
//         }

//         // Horizontal sum
//         asm volatile("vredsum.vs v4, v4, v30");
        
//         // Send result to scalar register file then store it in the result matrix
//         int8_t sum;
//         asm volatile("vmv.x.s %[sum], v4" : [sum] "=r"(sum));
//         c[i][j] = sum;
    
//     }
//   }
// }

// Without intrinsics, using 1D arrays
void matmul(int8_t a[], int8_t b[], int8_t c[], int AVL) {

  // Put 0 into v30[0] to use for redsum
  size_t vl;
  asm volatile("vsetivli %[vl], 1, e8\n" : [vl] "=r"(vl));
  asm volatile("vmv.v.i v30, 0");

  // Loop over the rows of matrix a
  for (int i = 0; i < AVL; ++i) {  
    // Loop over the columns of matrix b
    for (int j = 0; j < AVL; ++j) {
        // Current vector length
        size_t vl;      
        
        // Initialize the result in v4 to 0
        asm volatile("vsetvli %[vl], %[avl], e8, m1\n" : [vl] "=r"(vl) : [avl] "r" (AVL));
        asm volatile("vmv.v.i v4, 0");
      
        // Consider a vl subset of the row/column assuming we can't process the whole row/column at once
        int8_t *ptr_a = &a[i * AVL];
        int8_t *ptr_b = &b[j * AVL];

        for (int k = AVL; k > 0; k -= vl, ptr_a += vl, ptr_b += vl) {
            // Configure vtype
            asm volatile("vsetvli %[vl], %[avl], e8, m1\n" : [vl] "=r"(vl) : [avl] "r" (k));
            // Load part of the row of matrix a
            asm volatile("vle8.v v0, (%0)\n" : : "r" (ptr_a));
            // Load part of the column of matrix b
            asm volatile("vle8.v v2, (%0)\n" : : "r" (ptr_b));
            // MAC
            asm volatile("vmacc.vv v4, v0, v2");
        }

        // Horizontal sum
        asm volatile("vredsum.vs v4, v4, v30");
        
        // Send result to scalar register file then store it in the result matrix
        int8_t sum;
        asm volatile("vmv.x.s %[sum], v4" : [sum] "=r" (sum));
        c[i * AVL + j] = sum;
    }
  }
}


// Matrices defined in data.S
extern int AVL __attribute__((aligned(4))); // Dimensions of the matrices
extern int8_t matrix1[] __attribute__((aligned(4)));
extern int8_t matrix2[] __attribute__((aligned(4)));
extern int8_t golden_o[] __attribute__((aligned(4)));
extern int8_t output[] __attribute__((aligned(4)));

int main() {   

    // gcc sometimes uses vector instructions to assign the date. So, we allow time before starting the benchmark 
    printf("Starting the benchmark...\n");
    
    // Issue fence instruction to start benchmark counter
    asm volatile("fence");
    asm volatile("fence");

    // Perform the operation
    matmul(matrix1, matrix2, output, AVL);
    
    // Issue fence instruction to stop benchmark counter
    asm volatile("fence");

    // Check the result
    int error = 0;
    for (int i = 0; i < AVL; i++) {
        for (int j = 0; j < AVL; j++) {
            if (output[i*AVL + j] != golden_o[i*AVL + j]) {
                error += 1;
                // printf("Error at [%d][%d]: c_vec[%d][%d]= %d, but c_ref[%d][%d] = %d\n", i, j, i, j, c_vec[i][j], i, j, c_ref[i][j]);
            }
        }
    }

    if (error>0) {
        printf("Test failed. Number of errors: %d\n", error);
        return 1;
    } else {
        return 0;
    }
}