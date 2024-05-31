// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE0(void) {
  uint32_t vxsat;
  // VSET(4, e8, m1);
  unsigned int vl_vset;
  asm volatile("vsetivli %0, 4, e8\n" : "=r"(vl_vset));
  if (vl_vset != 4){
    printf("Error 1: vsetivli returned %d, expected 4\n", vl_vset);
  }
  volatile int8_t v1[] = {-80, 2, 100, 4};
  volatile int8_t v2[] = {-90, 2, 50, 4};
  asm volatile("vle8.v v1, (%0)" ::"r"(v1));
  asm volatile("vle8.v v2, (%0)" ::"r"(v2));
  asm volatile("vsadd.vv v3, v1, v2" ::);
  volatile int8_t res_golden[] = {-128, 4, 127, 8};
  volatile int8_t res[4];
  asm volatile("vse8.v v3, (%0)" ::"r"(res));
  // Dummy wait for the result to be written back to memory
  for (int i = 0; i < 20; i++){
    asm volatile("nop");
  }
  // Check the results
  for (int i = 0; i < 4; i++) {
    if (res[i] != res_golden[i]) {
      printf("v3[%d]=%d, expected %d\n", i, res[i], res_golden[i]);
    }
  }
  // Check vxsat
  read_vxsat(vxsat);
  check_vxsat(1, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE1(void) {
  uint32_t vxsat;
  VSET(4, e8, m1);
  VLOAD_8(v1, -80, 2, 100, 4);
  VLOAD_8(v2, -90, 2, 50, 4);
  __asm__ volatile("vsadd.vv v3, v1, v2" ::);
  VCMP_U8(1, v3, 0x80, 4, 127, 8);
  read_vxsat(vxsat);
  check_vxsat(1, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE2(void) {
  uint32_t vxsat;
  VSET(4, e8, m1);
  VLOAD_8(v1, -80, 100, 2, 0);
  const int8_t scalar = 50;
  __asm__ volatile("vsadd.vx v3, v1, %[A]" ::[A] "r"(scalar));
  VCMP_U8(2, v3, -30, 127, 52, 50);
  read_vxsat(vxsat);
  check_vxsat(2, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE3(void) {
  uint32_t vxsat;
  VSET(4, e16, m1);
  VLOAD_16(v1, 0x7FFB, 1, 3, 4);
  __asm__ volatile("vsadd.vi v3, v1, 5" ::);
  VCMP_U16(3, v3, 0x7FFF, 6, 8, 9);
  read_vxsat(vxsat);
  check_vxsat(3, vxsat, 1);
  reset_vxsat;
}


/// NOTE: Don't use V1 as operand for the 32-bit cases. The compiler uses V1 for loading the arrays into vector registers. 
//        If V1 is used as one of the operands, it can be overwritten by the compiler when loading other operands!!!

void TEST_CASE4(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 1, 2, -3, 0x7FFFFFFC);
  __asm__ volatile("vsadd.vi v3, v2, 5" ::);
  VCMP_U32(4, v3, 6, 7, 2, 0x7FFFFFFF);
  read_vxsat(vxsat);
  check_vxsat(4, vxsat, 1);
  reset_vxsat;
}

void TEST_CASE5(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v2, 0, -233, 34, -332);
  VLOAD_32(v3, 0, 4443, 4, -100);
  __asm__ volatile("vsadd.vv v4, v2, v3" ::);
  VCMP_U32(5, v4, 0, 4210, 38, -432);
  read_vxsat(vxsat);
  check_vxsat(5, vxsat, 0);
  reset_vxsat;
}

int main(void) {
  INIT_CHECK();
  // enable_vec();
  // TEST_CASE0();
  TEST_CASE1();
  TEST_CASE2();
  TEST_CASE3();
  TEST_CASE4();
  TEST_CASE5();
  EXIT_CHECK();
}
