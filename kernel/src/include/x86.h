#ifndef __X86_H
#define __X86_H

#include <stdint.h>

#define KB_TO_BYTE(x) ((x) * 1024)
#define MB_TO_BYTE(x) ((x) * 1024 * 1024)
#define GB_TO_BYTE(x) ((x) * 1024 * 1024 * 1024)

static inline void x86_clts(void) {
    __asm__ __volatile__ ("clts"); 
}

static inline void x86_hlt(void) {
    __asm__ __volatile__ ("hlt"); 
}

static inline void x86_sti(void) {
    __asm__ __volatile__ ("sti"); 
}

static inline void x86_cli(void) {
    __asm__ __volatile__ ("cli"); 
}

static inline void x86_ltr(uint16_t sel) {
    __asm__ __volatile__ ("ltr %%ax" :: "a" (sel));
}

static inline uint8_t inp(uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0"
                          : "=a" (rv)
                          : "d" (_port));
    return (rv);
}

static inline uint16_t inpw(uint16_t _port) {
    uint16_t rv;
    __asm__ __volatile__ ("inw %1, %0"
                          : "=a" (rv)
                          : "d" (_port));
    return (rv);
}

static inline uint32_t inpd(uint16_t _port) {
    uint32_t rv;
    __asm__ __volatile__ ("inl %1, %0"
                          : "=a" (rv)
                          : "d" (_port));
    return (rv);
}

static inline void outp(uint16_t _port, uint8_t _data) {
    __asm__ __volatile__ ("outb %1, %0"
                          :
                          : "d" (_port),
                          "a" (_data));
}

static inline void outpw(uint16_t _port, uint16_t _data) {
    __asm__ __volatile__ ("outw %1, %0"
                          :
                          : "d" (_port),
                          "a" (_data));
}

static inline void outpd(uint16_t _port, uint32_t _data) {
    __asm__ __volatile__ ("outl %1, %0"
                          :
                          : "d" (_port),
                          "a" (_data));
}

static inline uint64_t x86_get_cr2(void) {
    uint64_t rv;

    __asm__ __volatile__ (
        "mov %%cr2, %0"
        : "=r" (rv)
    );

    return rv;
}

static inline uint64_t x86_get_cr3(void) {
    uint64_t rv;

    __asm__ __volatile__ (
        "mov %%cr3, %0"
        : "=r" (rv));
    return rv;
}

static inline void x86_set_cr3(uint64_t cr3)
{
  asm volatile (
    "mov %0, %%cr3"
    :
    : "a"(cr3)
    : "memory"
  );
}

static inline void x86_invlpg(void* addr) {
  /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
  asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

static inline void x86_tlb_flush_all(void) {
  x86_set_cr3(x86_get_cr3());
}

#endif
