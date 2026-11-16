// SPDX-License-Identifier: MIT
// Arm GNU 13.3's static libstdc++ uses the C23 entry point for its decimal
// runtime tunables. The original parser has the same behavior for these inputs.
// Only link this shim with --wrap=__isoc23_strtoul when using that sysroot.
extern "C" unsigned long legacy_strtoul(const char*, char**, int) asm("strtoul");

extern "C" unsigned long __wrap___isoc23_strtoul(const char* input, char** end, int base)
{
    return legacy_strtoul(input, end, base);
}
