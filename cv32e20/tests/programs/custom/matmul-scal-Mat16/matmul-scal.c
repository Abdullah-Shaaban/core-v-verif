#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Matrix multiplication assuming b is transposed
// void matmul(char a[AVL][AVL], char b[AVL][AVL], char c[AVL][AVL]) {
//     for (int i = 0; i < AVL; i++) {
//         for (int j = 0; j < AVL; j++) {
//             c[i][j] = 0;
//             for (int k = 0; k < AVL; k++) {
//                 c[i][j] += a[i][k] * b[j][k];
//             }
//         }
//     }
// }

// Assume 1D arrays. All matrices are of size AVL x AVL
// void matmul(int8_t a[], int8_t b[], int8_t c[], int AVL) {
void matmul(int a[], int b[], int c[], int AVL) {
    for (int i = 0; i < AVL; i++) {
        for (int j = 0; j <  AVL; j++) {
            c[i * AVL + j] = 0;
            for (int k = 0; k < AVL; k++)
                c[i * AVL + j] += a[i * AVL + k] * b[j * AVL + k];
        }
    }
}

// Optimized version that accumulates into a register in the inner loop
void matmul_opt1(int a[], int b[], int c[], int AVL) {
    for (int i = 0; i < AVL; i++) {
        for (int j = 0; j <  AVL; j++) {
            register int tmp = 0;
            for (int k = 0; k < AVL; k++){
                tmp += a[i * AVL + k] * b[j * AVL + k];
            }
            c[i * AVL + j] = tmp;
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
    // Copy 8bit matrices into 32bit matrices
    int matrix1_32[AVL * AVL];
    int matrix2_32[AVL * AVL];
    int output_32[AVL * AVL];
    for (int i = 0; i < AVL*AVL; i++) {
        matrix1_32[i] = (unsigned int)matrix1[i];
        matrix2_32[i] = (unsigned int)matrix2[i];
    }    

    // Issue fence instruction to start benchmark counter
    asm volatile("fence");
    asm volatile("fence");

    // Perform the operation
    // matmul(matrix1_32, matrix2_32, output_32, AVL);
    matmul_opt1(matrix1_32, matrix2_32, output_32, AVL);
    
    // Issue fence instruction to stop benchmark counter
    asm volatile("fence");

    return 0;
}