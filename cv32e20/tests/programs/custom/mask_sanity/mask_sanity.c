// Copyright 2021 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Author: Matheus Cavalcante <matheusd@iis.ee.ethz.ch>
//         Basile Bougenot <bbougenot@student.ethz.ch>

#include "vector_macros.h"

// Instructions with 2 operands.
void TEST_CASE1(void) {
  uint32_t vxsat;
  VSET(4, e8, m1);
  VLOAD_8(v0, 0xA, 0x0, 0x0, 0x0);
  VLOAD_8(v2, -80, 2, 100, 4);
  VLOAD_8(v3, -90, 2, 50, 4);
  __asm__ volatile("vsadd.vv v4, v2, v3, v0.t" ::);
  VCMP_U8(1, v4, 0, 4, 0, 8); // A = 1010
  read_vxsat(vxsat);
  check_vxsat(1, vxsat, 0);
  reset_vxsat;
}

void TEST_CASE2(void) {
  uint32_t vxsat;
  VSET(4, e32, m1);
  VLOAD_32(v0, 0x7, 0x0, 0x0, 0x0); // Mask first element
  VLOAD_32(v2, 0x1, 0x2, 0x6, 0x7FFFFFFC);
  __asm__ volatile("vsadd.vi v4, v2, 5, v0.t" ::);
  // First 32-bit should remain unchanged from previous instruction (contain 0x0, 0x4, 0x0, 0x8)
  VCMP_U32(2, v4, 0x08000400, 0x7, 0xb, 0x7FFFFFFF);
  read_vxsat(vxsat);
  check_vxsat(2, vxsat, 1);
  reset_vxsat;
}



// One case for instructions with 3 operands.
void TEST_CASE3(void){
  VSET(16, e16, m1);
  VLOAD_16(v0, 0xD); // Mask 2nd element
  VLOAD_16(v3, 0x1c20, 0x11e4, 0xde38, 0x642f);
  VLOAD_16(v2, 0x02cc, 0xd99c, 0xdba2, 0xf282);
  VLOAD_16(v1, 0xe3f0, 0x42db, 0x2fde, 0x1983);
  asm volatile("vmacc.vv v1, v2, v3, v0.t");
  VCMP_U16(3, v1, 0x8d70, 0x42db, 0xb74e, 0x6761); // 2nd element should NOT be 0x6dcb
}


// One case when VL>32, and we make another request for another 32 bits of v0

int main(void) {
  INIT_CHECK();
  // enable_vec();
  TEST_CASE1();
  TEST_CASE2();
  TEST_CASE3();
  EXIT_CHECK();
}
