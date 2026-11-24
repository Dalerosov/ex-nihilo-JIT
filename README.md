# Ex-nihilo JIT

A JIT compiler and runtime written in C++, targeting x86-64 on Windows.

> **Status: early but working.** IL programs are parsed, compiled to native x86-64 at runtime, and executed. The language is close to Turing complete; arrays and pointers are the main missing pieces. This is a personal learning project exploring runtime and JIT internals.

## What this is

Ex-nihilo is an experiment in building a JIT compiler and runtime from scratch: take a simple intermediate representation, generate x86-64 machine code for it directly (no LLVM, no assembler library), and execute it. It's inspired by how production runtimes like the JVM and .NET CLR work under the hood, scaled down to something one person can build and understand end to end.

## How it works

```
 IL source file ──► parser ──► IL in memory ──► JIT Compiler ──► x86-64 emitter ──► executable memory ──► run
                                                (encoding, regalloc,             (VirtualAlloc,
                                                 ABI, jump patching)              RW → RX)
```

- **IL**: a three-address language with named, typed variables. Programs are loaded from a file at runtime. Operands are stored using type punning.
- **Emitter**: encodes x86-64 Win directly - will probably not work on some CPUs, on MacOS and basically anything that isnt Windows
- **Calling convention**: Windows x64 ABI prologue/epilogue generation and calls.
- **External functions**: For now allows only two types of number printing as a part of Proof of concept, this will be extended
- **Register allocation**: basic allocation with spill handling; a shadow value stack that tracks where values live so the emitter avoids redundant stack-to-register round trips is WIP for now.
- **Jumps and labels**: forward and backward jumps are resolved with a patch list, fixed up when the label is emitted.
- **Executable memory**: code is written to `VirtualAlloc`'d pages as read/write, then flipped to read/execute before running.
- **Instrumentation**: each function's prologue increments a call counter, which is the groundwork for tiered compilation.
- **Hexdump**: the emitted machine code can be dumped for inspection.

## IL example

```
fn print_factors($n : u64) -> u64
    $small : u64 = lt $n, 2
    jnz $small, done
    $d : u64 = 2
loop:
    $q : u64 = div $n, $d
    $over : u64 = gt $d, $q
    jnz $over, tail
    $r : u64 = rem $n, $d
    jz $r, hit
    $d : u64 = add $d, 1
    jmp loop
hit:
    $p : u64 = ext print_u64, $d
    $n : u64 = div $n, $d 
    jmp loop
tail:
    $big : u64 = gt $n, 1
    jz $big, done
    $p : u64 = ext print_u64, $n
done:
    $zero : u64 = 0
    ret $zero

fn main() -> u64
    $n : u64 = 360
    $r : u64 = call print_factors, $n
    ret $r

```
## Status

| Area | State |
|---|---|
| IL parser (loads from file at runtime) | Working |
| x86-64 emission, Windows x64 ABI | Working |
| Arithmetic, calls, returns | Working |
| Conditional/unconditional jumps, labels (forward and backward) | Working |
| Register allocation | Working, basic |
| Per-function call counters | Working |
| Hexdump of emitted code | Working |
| Arrays | Not yet (planned heap-only) |
| Pointers | Not yet |
| Instruction coverage | Minimal |
| Optimization passes | None yet |
| Tiered compilation | Planned; call counters implemented for now, probably will come back to it later |

## Design decisions

- **Allocator lives in C++**, exposed to the IL as a host function.
- **OS and syscall functionality goes through C++ host functions** instead of being exposed in the IL directly for now.
- **Direct machine code emission.** More work than using an assembler, but the encoding is the interesting part.


## Running

The program currently loads its IL from `C:\test.sil`. The path is softcoded; upon startup you will be prompted to change it.
You will also be prompted if you want to see debug stuff: n declines, anything else accepts, this has been added for speed when testing repeatedly.

## Roadmap

- [ ] Take the IL file path as a command-line argument
- [ ] Pointers
- [ ] Heap arrays
- [ ] Tiered compilation driven by the call counters
- [ ] Broader instruction coverage
- [ ] Optimization passes
