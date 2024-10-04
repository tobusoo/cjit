struct S {
  int a;
  int b;
};

void init(struct S *s) {
  s->a = 42;
  s->b = 1337;
}

int foo(int x) { return x * 2 + 1; }
