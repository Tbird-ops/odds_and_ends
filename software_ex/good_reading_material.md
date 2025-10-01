# Good reading material
## Getting started with Pwntools and other good lessons
- https://github.com/Gallopsled/pwntools-tutorial/blob/master/tubes.md
- https://docs.pwntools.com/en/stable/tubes/sockets.html
- https://docs.pwntools.com/en/stable/tubes.html
- https://docs.pwntools.com/en/stable/elf/elf.html#example-usage
- https://ir0nstone.gitbook.io/notes/types/stack/pie
- https://www.programiz.com/python-programming/methods/built-in/int

## Basics of stack overflows
- http://www.phrack.org/issues/49/14.html#article
- https://www.reddit.com/r/asm/comments/158l19i/shellcode_segfaults_for_unclear_reason/

## Stack Cookies and how to get around them
- https://www.sans.org/blog/stack-canaries-gingerly-sidestepping-the-cage/
- https://bananamafia.dev/post/binary-canary-bruteforce/

## DEP/NX (ROP chains to get around stack memory permission restrictions)
- https://blog.techorganic.com/2015/04/21/64-bit-linux-stack-smashing-tutorial-part-2/
- https://bordplate.no/blog/en/post/interactive-rop-tutorial/
- https://ctf101.org/binary-exploitation/return-oriented-programming/
- https://trustfoundry.net/basic-rop-techniques-and-tricks/
- https://blog.vero.site/post/baby-boi
- https://www.youtube.com/watch?v=zaQVNM3or7k
- https://book.hacktricks.xyz/binary-exploitation/rop-return-oriented-programing/rop-syscall-execv

## ASLR - The art of using predictable offsets despite randomization
- https://github.com/Naetw/CTF-pwn-tips#overflow
- https://undo.io/resources/gdb-watchpoint/how-search-byte-sequence-memory-gdb-command-find/	

## Uninit Variables - Did you know the memory is not cleaned upon returning?
- https://www.cprogramming.com/tutorial/function-pointers.html
- https://int0x33.medium.com/day-7-rop-emporium-write4-64bit-c6f9f64748ea
- https://book.hacktricks.xyz/reversing-and-exploiting/linux-exploiting-basic-esp/rop-syscall-execv

## Race Conditions - Time of check and time of use are subtle but powerful bugs
- https://en.wikipedia.org/wiki/Ioctl
- https://www.geeksforgeeks.org/posix-threads-in-os/
- https://dirtycow.ninja/
- https://www.cs.toronto.edu/~arnold/427/18s/427_18S/indepth/dirty-cow/demo.html

## Heap prep (Leveraging powerful tools in tandem)
- https://github.com/apogiatzis/gdb-peda-pwndbg-gef
- https://github.com/pwndbg/pwndbg

## Heap - How does malloc actually work and why are Use After Free bugs problematic
- https://azeria-labs.com/heap-exploitation-part-1-understanding-the-glibc-heap-implementation/
- https://azeria-labs.com/heap-exploitation-part-2-glibc-heap-free-bins/
- https://heap-exploitation.dhavalkapil.com/diving_into_glibc_heap
- https://sourceware.org/glibc/wiki/MallocInternals
- https://github.com/shellphish/how2heap/tree/master/glibc_2.31
- https://blog.quarkslab.com/heap-exploitation-glibc-internals-and-nifty-tricks.html
_Why the old ubuntu version for heap? Glibc differences_
- https://www.researchinnovations.com/post/bypassing-the-upcoming-safe-linking-mitigation

## Javascript Engine Browser Exploitation
_live overflow vids: an entry to Browser Exploitation_
- https://www.youtube.com/watch?v=5tEdSoZ3mmE&list=PLhixgUqwRTjwufDsT1ntgOY9yjZgg5H_t

_patching... because I went too long without knowing about this_
- https://man7.org/linux/man-pages/man1/patch.1.html

_exploitation walkthroughs from CTFs_
- https://faraz.faith/2019-12-13-starctf-oob-v8-indepth/
- https://changochen.github.io/2019-04-29-starctf-2019.html
- https://ir0nstone.gitbook.io/notes/binexp/browser-exploitation/ctf-2019-oob-v8

_V8 Debugging tricks_
- https://gist.github.com/totherik/3a4432f26eea1224ceeb

_V8 Pointer Compression dev blog_
- https://v8.dev/blog/pointer-compression

_Hex to Double_
- https://gregstoll.com/~gregstoll/floattohex/

_Exploiting logic bugs Phrack_
- http://www.phrack.org/issues/70/9.html

_Attacking clientside JIT compilers_
- https://www.youtube.com/watch?v=emt1yf2Fg9g

_JS Engines: The Good Parts_
- https://www.youtube.com/watch?v=5nmpokoRaZI

_Chrome Browser Exploitation_
- https://jhalon.github.io/chrome-browser-exploitation-1/

