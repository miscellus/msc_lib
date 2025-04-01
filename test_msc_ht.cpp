#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MSC_HT_IMPLEMENTATION
#include "msc_ht.h"

int main(void)
{
	msc_ht table;
	memset(&table, 0, sizeof(table));
	i32 size = 128;
	table.slots = (msc_ht_slot *)calloc(size, sizeof(*table.slots));
	table.size = size;
	table.hash = msc_ht_ptr_hash;
	table.equals = msc_ht_ptr_equals;
	assert(table.slots);

	void *value = 0;
	assert(!msc_ht_get(&table, (void *)1337, &value));
	assert(msc_ht_put(&table, (void *)1337, (void *)42));
	assert(msc_ht_get(&table, (void *)1337, &value));
	assert(value == (void *)42);

	// 31615346 is a second preimage of 42 using the default ptr hash function
	assert(msc_ht_put(&table, (void *)42, (void *)1));
	assert(msc_ht_put(&table, (void *)31615346, (void *)2));

	assert(msc_ht_get(&table, (void *)42, &value));
	assert(value == (void *)1);
	assert(msc_ht_get(&table, (void *)31615346, &value));
	assert(value == (void *)2);

	// i32 refhash = msc_ht_ptr_hash((void*)42);
	// for (uptr i = 0; i < 100000000; ++i)
	// {
	// 	i32 hash = msc_ht_ptr_hash((void *)i);
	// 	if (hash == refhash)
	// 	{
	// 		printf("%lu MORMOR\n", i);
	// 	}
	// }

	return 0;
}