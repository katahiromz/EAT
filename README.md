# EAT (Eyeball Allocation Table)

[![Build Status](https://travis-ci.org/katahiromz/EAT.svg?branch=master)](https://travis-ci.org/katahiromz/EAT)
[![Build status](https://ci.appveyor.com/api/projects/status/93n6laak96l7xkhs?svg=true)](https://ci.appveyor.com/project/katahiromz/eat)

EAT is a simple, enlargeable and mergeable virtual memory management 
system (and filesystem?), written in C++, by katahiromz.

You can do `malloc`, `calloc`, `strdup` etc. in EAT without modern OS.

## "THE MASTER IMAGE"

```txt
+---------------------------+(top) == this
|HEAD                       |
+---------------------------+(head_size)
|DATA #0 (variable length)  |
|DATA #1                    |
|  :                        |
|  :     DATA_AREA          | | |
|  :                        | | |
|DATA #n-1 (grows downward) | V V
+---------------------------+(boundary_1)
|                           |
|        FREE_AREA          |
|                           |
+---------------------------+(boundary_2)
|ENTRY #n-1 (grows upward)  | A A
|  :                        | | |
|  :       TABLE            | | |
|  :                        |
|ENTRY #1                   |
|ENTRY #0                   |
+---------------------------+(bottom)
```

## Contact Us

Katayama Hirofumi MZ (katahiromz)

katayama.hirofumi.mz@gmail.com
