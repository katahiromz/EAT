# EAT

EAT (Eyeball Allocation Table) is a simple, enlargeable and mergeable
virtual memory management system (and filesystem?), written by katahiromz.

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

/////////////////////////////////////////////////////
// Katayama Hirofumi MZ (katahiromz) [ARMYANT]
// Homepage     http://katahiromz.web.fc2.com/eindex.html
// BBS          http://katahiromz.bbs.fc2.com/
// E-Mail       katayama.hirofumi.mz@gmail.com
/////////////////////////////////////////////////////
