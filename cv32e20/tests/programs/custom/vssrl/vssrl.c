// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE1() {
  VSET(4, e8, m1);
  VLOAD_8(v2, 0xff, 0x00, 0xf0, 0x0f);
  VLOAD_8(v3, 1, 2, 3, 4);
  __asm__ volatile("csrw vxrm, 1"); // set the rounding mode to round-to-nearest-even
  __asm__ volatile("vssrl.vv v4, v2, v3");
  VCMP_U8(1, v4, 0x80, 0x00, 0x1e, 0x01); //v4=0x011e0080
}

void TEST_CASE2() {
  VSET(4, e8, m1);
  VLOAD_8(v2, 0xff, 0x00, 0xf0, 0x0f);
  __asm__ volatile("csrw vxrm, 3"); // set the rounding mode to round-to-odd
  __asm__ volatile("vssrl.vi v4, v2, 5");
  VCMP_U8(2, v4, 0x07, 0x00, 0x07, 0x01); // v4=0x01070007
}

void TEST_CASE3() {
  VSET(4, e8, m1);
  VLOAD_8(v2, 0xff, 0x00, 0xf0, 0x0f);
  uint32_t scalar = 5;
  __asm__ volatile("csrw vxrm, 0"); // set the rounding mode to round-to-nearest-up
  __asm__ volatile("vssrl.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U8(3, v4, 0x08, 0x00, 0x08, 0x00); // v4=0x00080008
}

void TEST_CASE4() {
  VSET(4, e8, m1);
  VLOAD_8(v2, 0xff, 0x00, 0xf0, 0x0f);
  uint32_t scalar = 5;
  __asm__ volatile("csrw vxrm, 2"); // set the rounding mode to round-down (truncate)
  __asm__ volatile("vssrl.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U8(4, v4, 0x07, 0x00, 0x07, 0x00); // v4=0x00070007
}

int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE2();
  TEST_CASE3();
  TEST_CASE4();
  EXIT_CHECK();
}