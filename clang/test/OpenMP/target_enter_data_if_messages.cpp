// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp -fopenmp-version=45 -ferror-limit 100 -o - %s -Wuninitialized

// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp-simd -fopenmp-version=45 -ferror-limit 100 -o - %s -Wuninitialized

// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp -ferror-limit 100 -o - %s -Wuninitialized
// RUN: %clang_cc1 -triple x86_64-apple-macos10.7.0 -verify -fopenmp-simd -ferror-limit 100 -o - %s -Wuninitialized

void foo() {
}

bool foobool(int argc) {
  return argc;
}

void xxx(int argc) {
  int cond; // expected-note {{initialize the variable 'cond' to silence this warning}}
#pragma omp target enter data map(to:argc) if(cond) // expected-warning {{variable 'cond' is uninitialized when used here}}
  for (int i = 0; i < 10; ++i)
    ;
}

struct S1; // expected-note {{declared here}}

int main(int argc, char **argv) {
  int i, z;
  #pragma omp target enter data map(to: i) if // expected-error {{expected '(' after 'if'}}
  #pragma omp target enter data map(to: i) if ( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if () // expected-error {{expected expression}}
  #pragma omp target enter data map(to: i) if (argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if (argc)) // expected-warning {{extra tokens at the end of '#pragma omp target enter data' are ignored}}
  #pragma omp target enter data map(to: i) if (argc > 0 ? argv[1] : argv[2])
  #pragma omp target enter data map(to: i) if (argc + argc)
  #pragma omp target enter data map(to: i) if (foobool(argc)), if (true) // expected-error {{directive '#pragma omp target enter data' cannot contain more than one 'if' clause}}
  #pragma omp target enter data map(to: i) if (S1) // expected-error {{'S1' does not refer to a value}}
  #pragma omp target enter data map(to: i) if (argv[1]=2) // expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if(target data : true) // expected-error {{directive name modifier 'target data' is not allowed for '#pragma omp target enter data'}}
  #pragma omp target enter data map(to: i) if(target enter data : // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if(target enter data : // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if(target enter data : argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
  #pragma omp target enter data map(to: i) if(target enter data : argc + z)
  #pragma omp target enter data map(to: i) if(target enter data : argc) if (for:argc) // expected-error {{directive name modifier 'for' is not allowed for '#pragma omp target enter data'}}
  #pragma omp target enter data map(to: i) if(target enter data : argc) if (target enter data:argc) // expected-error {{directive '#pragma omp target enter data' cannot contain more than one 'if' clause with 'target enter data' name modifier}}
  #pragma omp target enter data map(to: i) if(target enter data : argc) if (argc) // expected-error {{no more 'if' clause is allowed}} expected-note {{previous clause with directive name modifier specified here}}
  foo();

  return 0;
}
