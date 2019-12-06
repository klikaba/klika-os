#include <isr.h>
#include <kernel.h>
#include <x86.h>
#include <string.h>
#include <process.h>
#include <mmu_heap.h>

#define NB_REGISTERS_PUSHED_BEFORE_CALL 15

static void page_fault_handler(isr_ctx_t *regs);

isr_t interrupt_handlers[256];
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void init_kernel_isr()
{
    memset(interrupt_handlers, 0, 256 * sizeof(isr_t));

    DEBUG("ISR: init gates ...\n");
    set_idt_gate(0, (uint64_t) isr_stub_0);
    set_idt_gate(1, (uint64_t) isr_stub_1);
    set_idt_gate(2, (uint64_t) isr_stub_2);
    set_idt_gate(3, (uint64_t) isr_stub_3);
    set_idt_gate(4, (uint64_t) isr_stub_4);
    set_idt_gate(5, (uint64_t) isr_stub_5);
    set_idt_gate(6, (uint64_t) isr_stub_6);
    set_idt_gate(7, (uint64_t) isr_stub_7);
    set_idt_gate(8, (uint64_t) isr_stub_8);
    set_idt_gate(9, (uint64_t) isr_stub_9);
    set_idt_gate(10, (uint64_t) isr_stub_10);
    set_idt_gate(11, (uint64_t) isr_stub_11);
    set_idt_gate(12, (uint64_t) isr_stub_12);
    set_idt_gate(13, (uint64_t) isr_stub_13);
    set_idt_gate(14, (uint64_t) isr_stub_14);
    set_idt_gate(15, (uint64_t) isr_stub_15);
    set_idt_gate(16, (uint64_t) isr_stub_16);
    set_idt_gate(17, (uint64_t) isr_stub_17);
    set_idt_gate(18, (uint64_t) isr_stub_18);
    set_idt_gate(19, (uint64_t) isr_stub_19);
    set_idt_gate(20, (uint64_t) isr_stub_20);
    set_idt_gate(21, (uint64_t) isr_stub_21);
    set_idt_gate(22, (uint64_t) isr_stub_22);
    set_idt_gate(23, (uint64_t) isr_stub_23);
    set_idt_gate(24, (uint64_t) isr_stub_24);
    set_idt_gate(25, (uint64_t) isr_stub_25);
    set_idt_gate(26, (uint64_t) isr_stub_26);
    set_idt_gate(27, (uint64_t) isr_stub_27);
    set_idt_gate(28, (uint64_t) isr_stub_28);
    set_idt_gate(29, (uint64_t) isr_stub_29);
    set_idt_gate(30, (uint64_t) isr_stub_30);
    set_idt_gate(31, (uint64_t) isr_stub_31);

    set_idt_gate(ISR_IRQ0, (uint64_t) isr_stub_32);
    set_idt_gate(ISR_IRQ1, (uint64_t) isr_stub_33);
    set_idt_gate(ISR_IRQ2, (uint64_t) isr_stub_34);
    set_idt_gate(ISR_IRQ3, (uint64_t) isr_stub_35);
    set_idt_gate(ISR_IRQ4, (uint64_t) isr_stub_36);
    set_idt_gate(ISR_IRQ5, (uint64_t) isr_stub_37);
    set_idt_gate(ISR_IRQ6, (uint64_t) isr_stub_38);
    set_idt_gate(ISR_IRQ7, (uint64_t) isr_stub_39);
    set_idt_gate(ISR_IRQ8, (uint64_t) isr_stub_40);
    set_idt_gate(ISR_IRQ9, (uint64_t) isr_stub_41);
    set_idt_gate(ISR_IRQA, (uint64_t) isr_stub_42);
    set_idt_gate(ISR_IRQB, (uint64_t) isr_stub_43);
    set_idt_gate(ISR_IRQC, (uint64_t) isr_stub_44);
    set_idt_gate(ISR_IRQD, (uint64_t) isr_stub_45);
    set_idt_gate(ISR_IRQE, (uint64_t) isr_stub_46);
    set_idt_gate(ISR_IRQF, (uint64_t) isr_stub_47);
    set_idt_gate(SYSCALL_NO, (uint64_t) isr_stub_128);

    register_interrupt_handler(14, page_fault_handler);

    set_idt();
}

void isr_handler(isr_ctx_t *regs)
{
    uint8_t int_no = regs->int_no;
    if (interrupt_handlers[int_no] != 0) {
        isr_t handler = interrupt_handlers[int_no];
        handler(regs);
        return;
    }

    DEBUG(
        "Received interrupt: %d - %s\n"
        "  process             = %i\n"
        "  error_code          = 0x%X\n"
        "  instruction_pointer = 0x%X\n"
        "  code_segment        = 0x%X\n"
        "  cpu_flags           = 0x%X\n"
        "  stack_pointer       = 0x%X\n"
        "  stack_segment       = 0x%X\n",
        int_no, exception_messages[int_no],
        task_list_current->id,
        regs->error_code,
        regs->rip,
        regs->cs,
        regs->rflags,
        regs->rsp,
        regs->ss
    );
    DEBUG("KILLING PROCESS %i!!!!\n", task_list_current->id);
    kill_process(task_list_current);
    debug_heap_dump();
    do_first_task_jump();
}

void register_interrupt_handler(uint64_t id, isr_t handler) {
    interrupt_handlers[id] = handler;
}

static void page_fault_handler(isr_ctx_t *regs __attribute__((unused)) ) {
    uint64_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    DEBUG("!!!!!PAGW FAULT on 0x%X\n", faulting_address);
    kill_process(task_list_current);
    // debug_heap_dump();
    do_first_task_jump();
}
