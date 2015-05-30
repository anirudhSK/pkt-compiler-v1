int func() {
  int i = 1;
  int j = 1;
  int k = 0;
  while (k < 100) {
    if (j < 20) {
      j = i;
      k = k + 1;
    } else {
      j = k;
      k = k + 2;
    }
  }
  return j;
}
