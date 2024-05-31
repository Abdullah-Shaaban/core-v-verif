// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

void TEST_CASE1(void) {
  set_vxrm(0); // setting vxrm to rnu rounding mode
  VSET(4, e8, m1);
  VLOAD_8(v2, 1, 2, 3, 5);
  VLOAD_8(v3, 1, 3, 8, 4);
  __asm__ volatile("vaaddu.vv v4, v2, v3" ::);
  VCMP_U8(1, v4, 1, 3, 6, 5);
}

void TEST_CASE3(void) {
  set_vxrm(2); // setting vxrm to rdn rounding mode
  VSET(4, e32, m1);
  VLOAD_32(v2, 1, 2, 3, 4);
  const uint32_t scalar = 5;
  __asm__ volatile("vaaddu.vx v4, v2, %[A]" ::[A] "r"(scalar));
  VCMP_U32(3, v4, 3, 3, 4, 4);
}

int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE3();
  EXIT_CHECK();
}