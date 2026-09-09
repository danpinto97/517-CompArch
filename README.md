# 517-CompArch
Repo for CPE 517 Computer Architecture Labs

## MIPS-32 Instruction-Level Simulator

The simulator is split into a provided shell (`shell.c`, `shell.h`) and the
simulation routine in `sim.c` (the file you implement). The shell loads a MIPS
program, provides an interactive command prompt, and calls
`process_instruction()` once per cycle.

### Build

```sh
make
```

This produces the `sim` executable.

### Run

The simulator takes one or more program files. Each program file contains one
MIPS instruction per line, written as a hexadecimal word (no comments). A
sample program is provided in `inputs/sample.x`:

```sh
./sim inputs/sample.x
```

At the `MIPS-SIM>` prompt:

| Command | Description |
| --- | --- |
| `go` | run the program until it halts |
| `run n` | execute `n` instructions |
| `rdump` | dump registers and PC |
| `mdump low high` | dump memory from `low` to `high` |
| `input reg val` | set register `reg` to `val` |
| `?` | show help |
| `quit` | exit |

Register and memory dumps are also written to a `dumpsim` file.

Example (non-interactive):

```sh
printf 'go\nrdump\nquit\n' | ./sim inputs/sample.x
```
