/*
 * proptest.c
 * Copyright (C) 2020 Adrian Perez de Castro <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#undef NDEBUG
#define DEBUG

#include "../dbuf.c"
#include <theft.h>


static enum theft_alloc_res
t_random_dbuf_alloc(struct theft *t, void *env, void **instance)
{
    size_t size = theft_random_bits(t, 20);
    struct dbuf *buffer = dbuf_new(size);
    if (buffer) {
        *((size_t*) theft_hook_get_env(t)) = size;
        *instance = buffer;
        return THEFT_ALLOC_OK;
    } else {
        return THEFT_ALLOC_ERROR;
    }
}

static void
t_dbuf_free(void *instance, void *env)
{
    dbuf_free(instance);
}

static const struct theft_type_info random_dbuf_info = {
    .alloc = t_random_dbuf_alloc,
    .free = t_dbuf_free,
    .autoshrink_config = { .enable = true },
};


static enum theft_trial_res
prop_alloc_prealloc_size(struct theft *t, void *b)
{
    size_t prealloc_size = *((size_t*) theft_hook_get_env(t));
    struct dbuf *buffer = b;

    if (buffer->alloc >= prealloc_size)
        return THEFT_TRIAL_PASS;

    return THEFT_TRIAL_FAIL;
}

static bool 
test_preallocate_dbuf(void)
{
    theft_seed seed = theft_seed_of_time();

    size_t prealloc_size = 0;

    struct theft_run_config config = {
        .seed = seed,
        .name = __func__,
        .prop1 = prop_alloc_prealloc_size,
        .type_info = {
            &random_dbuf_info,
        },
        .fork = {
            .enable = true,
            .timeout = 5000,
        },
        .hooks = {
            .env = &prealloc_size,
        },
    };

    return theft_run(&config) == THEFT_RUN_PASS;
}


struct buf_and_str {
    struct dbuf b;
    char *sptr;
    size_t slen;
};

static enum theft_alloc_res
t_buf_and_str_alloc(struct theft *t, void *env, void **instance)
{
    struct buf_and_str *bs = calloc(1, sizeof (struct buf_and_str));
    if (!bs)
        return THEFT_ALLOC_ERROR;

    bs->b = DBUF_INIT;
    bs->slen = theft_random_bits(t, 20);
    bs->sptr = calloc(bs->slen + 1, sizeof (char));
    if (!bs->sptr) {
        free(bs);
        return THEFT_ALLOC_ERROR;
    }

    size_t i = 0;
    while (i < bs->slen) {
        uint64_t x = theft_random_bits(t, 8);
        if (x) bs->sptr[i++] = (char) x;
    }

    *instance = bs;
    return THEFT_ALLOC_OK;
}

static void
t_buf_and_str_free(void *instance, void *env)
{
    struct buf_and_str *bs = instance;
    dbuf_clear(&bs->b);
    free(bs->sptr);
}

static const struct theft_type_info dbuf_addstr_info = {
    .alloc = t_buf_and_str_alloc,
    .free = t_buf_and_str_free,
    .autoshrink_config = { .enable = true },
};


static enum theft_trial_res
prop_addstr_sizes(struct theft *t, void *data)
{
    struct buf_and_str *bs = data;

    if (bs->b.size != 0)
        return THEFT_TRIAL_FAIL;

    dbuf_addstr(&bs->b, bs->sptr);

    if (bs->b.size != bs->slen)
        return THEFT_TRIAL_FAIL;

    if (bs->b.alloc <= bs->b.size)
        return THEFT_TRIAL_FAIL;

    if (memcmp(dbuf_str(&bs->b), bs->sptr, bs->slen) != 0)
        return THEFT_TRIAL_FAIL;

    return THEFT_TRIAL_PASS;
}

static bool
test_dbuf_addstr(void)
{
    struct theft_run_config config = {
        .seed = theft_seed_of_time(),
        .name = __func__,
        .prop1 = prop_addstr_sizes,
        .type_info = {
                &dbuf_addstr_info,
        },
        .fork = {
            .enable = true,
            .timeout = 5000,
        },
    };
    return theft_run(&config) == THEFT_RUN_PASS;
}


int
main(int argc, char **argv)
{
    assert(test_preallocate_dbuf());
    assert(test_dbuf_addstr());
}
