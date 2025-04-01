#ifndef MISCELLUS_HASH_TABLE_H
#define MISCELLUS_HASH_TABLE_H
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef MISCELLUS_TYPES_H
#define MISCELLUS_TYPES_H
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef uintptr_t uptr;
typedef intptr_t iptr;
#endif // MISCELLUS_TYPES_H

#ifndef msc_is_pow2
#define msc_is_pow2(v) (0 == ((v) & ((v) - 1)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *key;
	void *value;
} msc_ht_slot;

typedef struct {
	i32 (*hash)(void *);
	bool (*equals)(void *, void *);
	msc_ht_slot *slots;
	i32 size;
	i32 used_count;
} msc_ht;

typedef i32 msc_ht_slot_status;
enum {
	MSC_HT_SLOT_FREE = 0,
	MSC_HT_SLOT_REMOVED = 1,
	MSC_HT_SLOT_MATCH = 2,
	MSC_HT_SLOT_MISMATCH = 3,
};

bool msc_ht_put(msc_ht *table, void *key, void *value);
bool msc_ht_get(msc_ht *table, void *key, void **value);
bool msc_ht_remove(msc_ht *table, void *key);
msc_ht_slot_status msc_ht_get_slot(msc_ht *table, void *key, bool get_removed, msc_ht_slot **out_slot);
i32 msc_ht_ptr_hash(void *ptr);
bool msc_ht_ptr_equals(void *a, void *b);

#ifdef __cplusplus
}
#endif

#endif // MISCELLUS_HASH_TABLE_H

#ifdef MSC_HT_IMPLEMENTATION

i32 msc_ht_ptr_hash(void *ptr) {
	uptr v = (uptr)ptr;
	v = (~v) + (v << 18);
	v ^= (v >> 31);
	v *= 21;
	v ^= (v >> 11);
	v += (v << 6);
	v ^= (v >> 22);
	return (i32)v;
}

bool msc_ht_ptr_equals(void *a, void *b) {
	return a == b;
}

static void msc_ht_assert_invariants_(msc_ht *table) {
	assert(table);
	assert(table->hash);
	assert(table->equals);
	assert(table->used_count >= 0);
	assert(table->size >= 0);
	assert(msc_is_pow2(table->size));
}

static bool msc_ht_slot_is_free_(msc_ht_slot slot) {
	return slot.key == NULL && slot.value == NULL;
}

static bool msc_ht_slot_is_deleted_(msc_ht_slot slot) {
	return slot.key == (void *)(iptr)-1 && slot.value == (void *)(iptr)-1;
}

bool msc_ht_put(msc_ht *table, void *key, void *value) {
	msc_ht_assert_invariants_(table);

	msc_ht_slot *slot;
	msc_ht_slot_status status = msc_ht_get_slot(table, key, true, &slot);

	if (status == MSC_HT_SLOT_MISMATCH) return false;
	if (status == MSC_HT_SLOT_MATCH) return false;

	assert(status == MSC_HT_SLOT_FREE || status == MSC_HT_SLOT_REMOVED);

	slot->key = key;
	slot->value = value;
	return true;
}

bool msc_ht_get(msc_ht *table, void *key, void **value) {
	msc_ht_assert_invariants_(table);

	msc_ht_slot *slot;
	msc_ht_slot_status status = msc_ht_get_slot(table, key, false, &slot);

	if (status != MSC_HT_SLOT_MATCH) return false;

	*value = slot->value;
	return true;
}

bool msc_ht_remove(msc_ht *table, void *key) {
	msc_ht_assert_invariants_(table);

	msc_ht_slot *slot;
	msc_ht_slot_status status = msc_ht_get_slot(table, key, false, &slot);

	if (status != MSC_HT_SLOT_MATCH) return false;

	// This is how we represent a removed slot
	slot->key = (void *)(iptr)-1;
	slot->value = (void *)(iptr)-1;
	return true;
}

msc_ht_slot_status msc_ht_get_slot(msc_ht *table, void *key, bool get_removed, msc_ht_slot **out_slot) {
	msc_ht_assert_invariants_(table);

	i32 hash = table->hash(key);
	i32 stride = 1; // TODO(jkk) secondary hash functon for stride (this is just linear probing)

	// Ensure stride is odd to make it relatively prime with the table size,
	// ensuring all slots will eventually be hit in the search.
	stride |= 1;

	for (i32 i = 0; i < table->size; ++i) {
		msc_ht_slot *slot = &table->slots[hash & (table->size - 1)];

		if (msc_ht_slot_is_free_(*slot)) {
			*out_slot = slot;
			return MSC_HT_SLOT_FREE;
		}

		if (get_removed && msc_ht_slot_is_deleted_(*slot)) {
			*out_slot = slot;
			return MSC_HT_SLOT_REMOVED;
		}

		if (table->equals(key, slot->key)) {
			*out_slot = slot;
			return MSC_HT_SLOT_MATCH;
		}

		hash += stride;
	}

	return MSC_HT_SLOT_MISMATCH;
}

#endif //MSC_HT_IMPLEMENTATION