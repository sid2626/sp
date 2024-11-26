#include <stdio.h>
#include <stdlib.h>

/* This is a multi-line comment
   testing the lexical analyzer */

// Function to calculate factorial
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

#define MAX_SIZE 100
#define PI 3.14159

int main() {
    // Variables of different types
    int number = 42;
    float pi = 3.14159;
    char letter = 'A';
    char string[] = "Hello, World!";
    
    // Testing operators
    int a = 10, b = 20;
    int sum = a + b;
    int diff = a - b;
    int mult = a * b;
    int div = b / a;
    
    // Testing compound operators
    int x = 5;
    x += 3;
    x -= 2;
    x *= 4;
    x /= 2;
    
    // Testing increment/decrement
    int i = 0;
    i++;
    ++i;
    i--;
    --i;
    
    // Testing logical operators
    if (a > b && x <= 10) {
        printf("Testing logical AND\n");
    }
    
    if (a < b || x >= 5) {
        printf("Testing logical OR\n");
    }
    
    // Testing bitwise operators
    int bits = 0xFF;
    int mask = 0x0F;
    int result = bits & mask;
    result = bits | mask;
    result = bits ^ mask;
    result = ~bits;
    result = bits << 2;
    result = bits >> 1;
    
    // Testing different number formats
    int decimal = 123;
    int octal = 0755;
    int hex = 0xFF;
    float scientific = 1.23e-4;
    
    // Testing escape sequences in strings
    printf("Line1\nLine2\tTabbed\n");
    printf("Quote: \"Hello\"\n");
    
    // Testing array access
    int array[MAX_SIZE];
    array[0] = 1;
    
    // Function call
    int fact5 = factorial(5);
    
    return 0;
}
