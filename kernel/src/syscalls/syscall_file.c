#include <fat.h>
#include <isr.h>

// uint32_t syscall_file_open(char *filename, mode, scratch_buffer, *fileinfo_out)
uint64_t syscall_file_open(isr_ctx_t *regs) {
	uint8_t *filename = (uint8_t*)regs->rdi;
	uint8_t mode = regs->rsi & 0xFF;
	uint8_t *scratch_buffer = (uint8_t *)regs->rdx;
	FILEINFO *file_info_out = (FILEINFO*)regs->rcx;

	return DFS_OpenFile(&dosfs_volume_info, filename, mode, scratch_buffer, file_info_out);
}

// syscall_file_read(*fileinfo, scratch_buffer, ptr, *read_count, size)
uint64_t syscall_file_read(isr_ctx_t *regs) {
	FILEINFO *file_info = (FILEINFO*)regs->rdi;
	uint8_t *scratch_buffer = (uint8_t *)regs->rsi;
	uint8_t *ptr = (uint8_t *)regs->rdx;
	uint32_t *read_bytes = (uint32_t*)regs->rdx;
	uint64_t size = regs->r8;

	return DFS_ReadFile(file_info, scratch_buffer, ptr, read_bytes, size);
}
