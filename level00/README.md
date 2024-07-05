# Level00 Exploit

## Steps

### 1. Initial Analysis

List the files in the directory:

```bash
level00@RainFall:~$ ls -l
total 8
-rwsr-s---+ 1 level01 users 5138 Mar  6  2016 level00
```

### 2. Program Execution

Observe the program behavior with different inputs:

```bash
level00@RainFall:~$ ./level00
***********************************
*           -Level00 -            *
***********************************
Password: 1234
Invalid Password!
```

### 3. Disassembling `main` Function

Use GDB to disassemble the `main` function:

```bash
level00@RainFall:~$ gdb level00
(gdb) disassemble main
```

You can convert the assembly code to C using the [CodeConvert](https://www.codeconvert.ai/assembly-to-c++-converter) tool for better understanding. Here is the converted code:

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("***********************************\n");
    printf("*           -Level00 -            *\n");
    printf("***********************************\n");
    printf("Password: ");
    int num;
    scanf("%d", &num);
    if (num == 5276) {
        printf("Authenticated!\n");
        system("/bin/sh");
        return 0;
    } else {
        printf("Invalid Password!\n");
        return 1;
    }
}
```

### 4. Finding the Hardcoded Password

From the disassembled code, we identify that the hardcoded password is `5276`.

### 5. Executing the Exploit

Run the program and input the hardcoded password to gain access:

```bash
level00@RainFall:~$ ./level00
***********************************
*           -Level00 -            *
***********************************
Password: 5276
Authenticated!
$ whoami
level01
$ cat /home/users/level01/.pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```

### Explanation

1. **Disassembling the `main` Function**: Use GDB to understand the flow of the program and identify hardcoded values.
2. **Hardcoded Password**: Found to be `5276`, which allows the program to authenticate and open a shell.
3. **Executing the Program**: Input the identified password to exploit the program and gain access to the next level's password.

By following these steps, we successfully exploit `level00` and retrieve the password for `level01`.