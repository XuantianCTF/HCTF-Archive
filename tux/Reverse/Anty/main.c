#include <stdbool.h>
#include <stdio.h>

bool check_flag_asm(const char *input);

int main() {
  char input[32] = {0};
  printf("Enter Flag: ");
  scanf("%s", input);

  if (check_flag_asm(input)) {
    printf("Correct Flag!\n");
  } else {
    printf("Wrong flag\n");
  }
  return 0;
}
