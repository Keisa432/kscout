/*
 * kscout_player_creator.h
 */

#ifndef KSCOUT_PLAYER_CREATOR_H
#define KSCOUT_PLAYER_CREATOR_H

#include <stddef.h>
#include <stdint.h>

#include "kscout_player.h"
#include "kscout_parser.h"
#include "kscout_memblock.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * API
 * ---------------------------------------------------------------------- */

typedef struct kscout_player_creator kscout_player_creator_t;

/**
 * Allocate and initialise a player creator.
 * Returns 0 on success, -1 on OOM.
 */
int kscout_player_creator_new(kscout_player_creator_t **c);

/**
 * Free a player creator returned by kscout_player_creator_new().
 */
void kscout_player_creator_destroy(kscout_player_creator_t *creator);

/**
 * Populate @player from a fixed-order NULL-terminated-string token array.
 *
 * @creator     Creator instance.
 * @tokens      Array of at least num_tokens pointers to token strings.
 *              Tokens must match the FM export column order exactly.
 *              A NULL entry is treated as an empty string.
 * @num_tokens  Number of entries in @tokens.
 * @player      Output struct — zeroed then filled in by this function.
 *              The caller owns the struct; string members point into @mb.
 * @mb          Memory block used for all string allocations.
 *              Must outlive @player.
 *
 * Returns KSCOUT_OK on success, KSCOUT_ERR_INVALID if arguments are bad.
 */
int kscout_player_creator_player_from_tokens(
    const kscout_player_creator_t *creator, const kscout_parser_token_t *tokens,
    size_t num_tokens, kscout_player_t *player, kscout_memblock_t *mb);

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_PLAYER_CREATOR_H */