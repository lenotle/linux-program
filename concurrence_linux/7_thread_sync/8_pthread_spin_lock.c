#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



/*
 *
 * int pthread_spin_init(pthread_spinlock_t *lock int pshared);
 *     
 * int pthread_spin_destroy(pthread_spinlock_t *lock);
 *
 * 如果锁被加他线程占用 则开始自旋  
 * int pthread_spin_lock(pthread_spinlock_t *lock);
 * int pthread_spin_trylock(pthread_spinlock_t *lock);
 * 
 * int pthread_spin_unlock(pthread_spinlock_t *lock);
 *
 *
 */ 

// 自旋锁  使用方式和互斥锁一
int main()
{

	return 0;
}
