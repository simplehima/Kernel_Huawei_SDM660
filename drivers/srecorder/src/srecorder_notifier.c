

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>

#include "srecorder_notifier.h"
#include "srecorder_log.h"

extern int atomic_notifier_chain_register(struct atomic_notifier_head *nh, struct notifier_block *n);
extern int atomic_notifier_chain_unregister(struct atomic_notifier_head *nh, struct notifier_block *n);

extern int blocking_notifier_chain_register(struct blocking_notifier_head *nh, struct notifier_block *n);
extern int blocking_notifier_chain_unregister(struct blocking_notifier_head *nh, struct notifier_block *n);

extern int raw_notifier_chain_register(struct raw_notifier_head *nh, struct notifier_block *n);
extern int raw_notifier_chain_unregister(struct raw_notifier_head *nh, struct notifier_block *n);

/**
    @function: static int srecorder_panic_notifier_handler(struct notifier_block *this, unsigned long event, void *panic_reason)
    @brief: 
    @return: 
    @note: 
**/
static int srecorder_panic_notifier_handler(struct notifier_block *this, unsigned long event, void *panic_reason)
{
#ifdef CONFIG_PREEMPT
    /* Ensure that cond_resched() won't try to preempt anybody */
	__preempt_count_add(PREEMPT_DISABLE_OFFSET);
//	__preempt_count_add(PREEMPT_ACTIVE);
#endif
    srecorder_notify_framework(RESET_APANIC);
    
    return 0;
}

static struct notifier_block s_panic_notifier_block = 
{
    .notifier_call = srecorder_panic_notifier_handler, 
};

#ifndef CONFIG_KPROBES
static int srecorder_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason);
static int srecorder_emergency_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason);

static struct notifier_block s_reboot_notifier_block = 
{
    .notifier_call = srecorder_reboot_notifier_handler, 
};

static struct notifier_block s_emergency_reboot_notifier_block = 
{
    .notifier_call = srecorder_emergency_reboot_notifier_handler, 
};

/**
    @function: static int srecorder_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason)
    @brief: 
    @return: 
    @note: 
**/
static int srecorder_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason)
{
    srecorder_notify_framework(RESET_NORMAL);
    
    return 0;
}

/**
    @function: static int srecorder_emergency_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason)
    @brief: 
    @return: 
    @note: 
**/
static int srecorder_emergency_reboot_notifier_handler(struct notifier_block *this, unsigned long code, void *reboot_reason)
{
    srecorder_notify_framework(RESET_NORMAL);

    return 0;
}
#endif

/**
    @function: void srecorder_resigter_notifiers(void)
    @brief: dump 
    @param: reason
    @return: none
    @note:
*/
void srecorder_resigter_notifiers(void)
{
    atomic_notifier_chain_register(&panic_notifier_list, &s_panic_notifier_block);
#ifndef CONFIG_KPROBES
    blocking_notifier_chain_register(&reboot_notifier_list, &s_reboot_notifier_block);
    raw_notifier_chain_register(&emergency_reboot_notifier_list, &s_emergency_reboot_notifier_block);
#endif
}

/**
    @function: void srecorder_unresigter_notifiers(void)
    @brief: dump 
    @param: reason
    @return: none
    @note:
*/
void srecorder_unresigter_notifiers(void)
{
    atomic_notifier_chain_unregister(&panic_notifier_list, &s_panic_notifier_block);
#ifndef CONFIG_KPROBES
    blocking_notifier_chain_unregister(&reboot_notifier_list, &s_reboot_notifier_block);
    raw_notifier_chain_unregister(&emergency_reboot_notifier_list, &s_emergency_reboot_notifier_block);
#endif
}
