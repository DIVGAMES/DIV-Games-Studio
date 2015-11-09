
/*
 * Operation modes
 */
#define PLAYBACK 1
#define RECORD 2

/*
 * Configurable things.
 *
 * REMEMBER:
 * If you change CHANNELS or SIGNIFICANT_BITS or quality mixer constants, write
 * the changes also to JUDASCFG.INC. When finished with configuring, type
 * wmake /a to completely re-compile the JUDAS library.
 */

/*
 * How many times in second the DMA buffer goes round. The smaller this is,
 * the larger the buffer and the more there is delay in sound effects, but then
 * judas_update() doesn't have to be called so often. DMA buffer maximum size
 * (64K) sets a limit on the minimum value though. Don't fear, judas_init()
 * checks the buffer size and will simply limit it to 64K.
 */
#define PER_SECOND 15

/*
 * Number of digital channels. Increase or decrease according to your needs.
 * If it's below 32, all tunes can't be loaded. If it's above 32, you can
 * play 32-channel tunes and even have sound fx at the same time!
 * Number of channels will affect mixing speed just a tiny bit, however an
 * unused channel takes EXTREMELY little time compared to a used one.
 */
#define CHANNELS 32

/* SIGNIFICANT_BITS_16
 * Number of significant bits in mixing for fast mixer. Maximum is 16, because
 * fast mixer uses 16 bit mixing. If it's below 16, the leftover bits are used
 * for clipping. At the current setting 14, 2 bits are used for clipping, so
 * you can distort sound up to 4x without getting an overflow (nasty-sounding
 * distortion.) With 13 bits, you have 3 bits for clipping and can distort up
 * to 8x etc.
 *
 * SIGNIFICANT_BITS_32 (Fixed)
 * Number of significant bits in mixing for quality mixer. Maximum is 32.
 * However, DON'T CHANGE IT, because the routines of quality mixer only work
 * correctly if it's 30. Why is it here then? - Just to let you know how many
 * significant bits quality mixer uses.
 */
#define SIGNIFICANT_BITS_16 14
#define SIGNIFICANT_BITS_32 30

/* Quality mixer constants in judascfg.inc
 *
 * QMIXER_STARTMUTE defines how much the starts of sounds are muted.
 * 0 = no muting, 14 = zero volume, 2 = good;
 * QMIXER_ZEROLEVELDECAY defines how slow the shifted zero level gets back to
 * it's original position. 0 = instantly, 32 = never, 7 = good;
 * QMIXER_VOLUMESMOOTH defines how much volume changes are smoothed. 0 = not
 * at all, 22 = prevent volume changes, 6 = good;
 */
