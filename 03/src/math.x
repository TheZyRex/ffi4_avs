/* math.x */
struct intpair {
  int a;
  int b;
};

program MATHPROG {
  version MATHVERS {
    int ADD(intpair) = 1;
    int MULTIPLY(intpair) = 2;
    int CUBE(int) = 3;
  }= 1; /*version*/
} = 0x2000001; /*id userdefined*/