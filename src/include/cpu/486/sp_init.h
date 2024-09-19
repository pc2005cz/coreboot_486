/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _486_SP_INIT_H_
#define _486_SP_INIT_H_

// #include <cpu/x86/smm.h>
#include <types.h>

struct cpu_info;
struct bus;

static inline void mfence(void)
{
	__asm__ __volatile__("lock; addl $0,0(%%esp)": : : "memory");
}

/* The sequence of the callbacks are in calling order. */
struct mp_ops {
	/*
	 * Optionally provide a callback prior to kicking off MP
	 * startup. This callback is done prior to loading the SIPI
	 * vector but after gathering the MP state information. Please
	 * see the sequence below.
	 */
	void (*pre_mp_init)(void);
	/*
	 * Return the number of logical x86 execution contexts that
	 * need to be brought out of SIPI state as well as have SMM
	 * handlers installed.
	 */
	int (*get_cpu_count)(void);
	/*
	 * Optionally fill in permanent SMM region and save state size. If
	 * this callback is not present no SMM handlers will be installed.
	 * The perm_smsize is the size available to house the permanent SMM
	 * handler.
	 */
	void (*get_smm_info)(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size);
	/*
	 * Optionally fill in pointer to microcode and indicate if the APs
	 * can load the microcode in parallel.
	 */
	void (*get_microcode_info)(const void **microcode, int *parallel);
	/*
	 * Optionally provide a callback prior to the APs starting SMM
	 * relocation or CPU driver initialization. However, note that
	 * this callback is called after SMM handlers have been loaded.
	 */
	void (*pre_mp_smm_init)(void);
	/*
	 * Optional function to use to trigger SMM to perform relocation. If
	 * not provided, smm_initiate_relocation() is used.
	 * This function is called on each CPU.
	 * On platforms that select CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER) to
	 * not relocate in SMM, this function can be used to relocate CPUs.
	 */
	void (*per_cpu_smm_trigger)(void);
	/*
	 * This function is called while each CPU is in the SMM relocation
	 * handler. Its primary purpose is to adjust the SMBASE for the
	 * permanent handler. The parameters passed are the current cpu
	 * running the relocation handler, current SMBASE of relocation handler,
	 * and the pre-calculated staggered CPU SMBASE address of the permanent
	 * SMM handler.
	 * This function is only called with !CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER) set.
	 */
	void (*relocation_handler)(int cpu, uintptr_t curr_smbase,
		uintptr_t staggered_smbase);
	/*
	 * Optionally provide a callback that is called after the APs
	 * and the BSP have gone through the initialion sequence.
	 */
	void (*post_mp_init)(void);
};

/*
 * The mp_ops argument is used to drive the multiprocess initialization. Unless
 * otherwise stated each callback is called on the BSP only. The sequence of
 * operations is the following:
 * 1. pre_mp_init()
 * 2. get_cpu_count()
 * 3. get_smm_info()
 * 4. get_microcode_info()
 * 5. adjust_cpu_apic_entry() for each number of get_cpu_count()
 * 6. pre_mp_smm_init()
 * 7. per_cpu_smm_trigger() in parallel for all cpus which calls
 *    relocation_handler() in SMM.
 * 8. mp_initialize_cpu() for each cpu
 * 9. post_mp_init()
 */
// enum cb_err sp_init_with_smm(struct bus *cpu_bus, struct mp_ops *mp_ops);

#endif /* _486_SP_INIT_H_ */
