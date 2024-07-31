# Level03 Exploit

## 1. Initial Analysis

List the files in the directory:

```bash
level03@OverRide:~$ ls -l
total 8
-rwsr-s---+ 1 level04 users 7677 Sep 10  2016 level03
```

The `level03` binary has the SUID bit set, which means it will run with the permissions of the owner of the file, in this case `level04`.

## 2. Program Execution

Observe the program behavior:

```bash
level03@OverRide:~$ ./level03 
***********************************
*		level03		**
***********************************
Password:test

Invalid Password
level03@OverRide:~$ 
```

We can see that the program prompts for a password. We can see that the password `test` is incorrect.

## 3. Disassembling `main` Function

Use GDB to disassemble the `main` function:

```bash
level03@OverRide:~$ gdb level03
(gdb) disass main
```

You can convert the assembly code to C using the [CodeConvert](https://www.codeconvert.ai/assembly-to-c++-converter) tool. You can find the converted code in [source.c](source.c).

We also use Ghidra to decompile the binary. You can find the decompiled code by Ghidra in [ghidra_level03.c](Ressources/ghidra_level03.c).

## 4. Program Analysis

In the `main` function, we can see that the program call the `test` function with the input password as an argument. The `test` function contains a `switch`, which call the `decrypt` function in each case.

In the `decrypt` function, we can see that `system("/bin/sh")` is called. We will try to find a way to call this function, and pass the condition.

### 4.1. String reverse

In the `decrypt` function, we can see:
```c
local_21 = 0x757c7d51;
local_1d = 0x67667360;
local_19 = 0x7b66737e;
local_15 = 0x33617c7d;
local_11 = 0;
```
This looks like filling a char array, with `0` to finish the string.
With a ascii table, we can convert the hex values to characters.
For `0x757c7d51`:
`0x75` = `u`
`0x7c` = `|`
`0x7d` = `}`
`0x51` = `Q`
For `0x67667360`:
`0x67` = `g`
`0x66` = `f`
`0x73` = `s`
`0x60` = `
For `0x7b66737e`:
`0x7b` = `{`
`0x66` = `f`
`0x73` = `s`
`0x7e` = `~`
For `0x33617c7d`:
`0x33` = `3`
`0x61` = `a`
`0x7c` = `|`
`0x7d` = `}`
In little endian, it's reversed.
So the string is "Q}|u" + "\`sfg" + "~sf{" + "}|a3" = "Q}|u\`sfg~sf{}|a3".

### 4.2. Comparing

In the `decrypt` function, we can see:

```c
  iVar3 = 0x11; //17
  puVar4 = &local_21;
  pbVar5 = (byte *)"Congratulations!";
  do {
    if (iVar3 == 0) break;
    iVar3 = iVar3 + -1;
    bVar6 = *(byte *)puVar4 < *pbVar5;
    bVar7 = *(byte *)puVar4 == *pbVar5;
    puVar4 = (undefined4 *)((int)puVar4 + 1);
    pbVar5 = pbVar5 + 1;
  } while (bVar7);
  if ((!bVar6 && !bVar7) == bVar6) {
    iVar3 = system("/bin/sh");
  }
```

This `do` loop compares the string in `local_21` with the string "Congratulations!".
If the comparison is successful, the program will call `system("/bin/sh")`.

### 4.3 Crypting

Between the declaration of `local_21` and the comparing loop, we can see:
```c
  local_2c = 0; // used as index
  while( true ) {
    bVar6 = local_2c < ~uVar2 - 1;
    bVar7 = local_2c == ~uVar2 - 1;
    if (!bVar6) break;
    *(byte *)((int)&local_21 + local_2c) = (byte)ctx ^ *(byte *)((int)&local_21 + local_2c);
    local_2c = local_2c + 1;
  }
```
We can see that each byte of the string is XORed (`^`) with `ctx`, and stored back in `local_21`.

Q}|u`sfg~sf{}|a3
Congratulations!
Also, we can see that "Q}|u\`sfg~sf{}|a3" is "Congratulations!" with 

**So, `local_21` is defined as "Q}|u\`sfg~sf{}|a3", and is XORed with `ctx` before being compared with "Congratulations!".**

Now, what is `ctx`?

### 4.4. Finding `ctx`

We can see that `ctx` is an argument of the `decrypt` function. In the `test` function, we can see that the first argument of `decrypt` is `pEVar1`.
```c
decrypt(pEVar1,in_stack_ffffffd8,in_stack_ffffffdc,in_stack_ffffffe0,in_stack_ffffffe4);
```
`pEVar1` is the second argument of the `test` function - the first argument of the `test` function.
```c
pEVar1 = (EVP_PKEY_CTX *)(param_2 - param_1);
```
In the `main` function, we can see that the `test` function is called with:
```c
test(iStack_14,0x1337d00d);
```
`iStack_14` is the user input.

## 5. Exploitation

"Congratulations!" = `ctx` ^ "Q}|u\`sfg~sf{}|a3"

For the XOR operation, we can use the following [properties](https://fr.wikipedia.org/wiki/Fonction_OU_exclusif):
- `a` ^ `b` = `c` => `c` ^ `b` = `a`

So `ctx` = "Congratulations!" ^ "Q}|u\`sfg~sf{}|a3", for each byte.
`C` ^ `Q` = 0x43 ^ 0x51 = 0x12 = 18
`o` ^ `}` = 0x6f ^ 0x7d = 0x12 = 18
`n` ^ `|` = 0x6e ^ 0x7c = 0x12 = 18
etc...

So if `ctx` = 18, `local_21` = "Congratulations!" and we pass the condition for calling `system("/bin/sh")`.

`ctx` = 18 = `param_2 - param_1` in `decrypt` function.

`param_2` = 0x1337d00d = 322424845
`param_1` = user input
So:
18 = `param_2 - param_1`
18 = 322424845 - user input
user input = 322424845 - 18
user input = 322424827

With this input, we can get a shell with the permissions of `level04`.

```bash
level03@OverRide:~$ ./level03 
***********************************
*		level03		**
***********************************
Password:322424827
$ whoami
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

Congratulations! You have successfully exploited the `level03` binary and found the password for `level04`.

```bash
level03@OverRide:~$ su level04
Password: 
```

