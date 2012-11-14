#ifndef __OBSE_VERSION_H__
#define __OBSE_VERSION_H__

// these have to be macros so they can be used in the .rc
#define OBSE_VERSION_INTEGER		20
#define OBSE_VERSION_INTEGER_MINOR	6
#define OBSE_VERSION_VERSTRING		"0, 0, 20, 6"
#define OBSE_VERSION_PADDEDSTRING	"0020"

// build numbers are the month and date each build was made
#define MAKE_OBLIVION_VERSION(major, minor, build) (((major & 0xFF) << 24) | ((minor & 0xFF) << 16) | (build & 0xFFFF))
#define OBLIVION_VERSION_0_1 MAKE_OBLIVION_VERSION(0, 1, 228)		// 0x000100E4
#define OBLIVION_VERSION_1_1 MAKE_OBLIVION_VERSION(1, 1, 511)		// 0x010101FF
#define OBLIVION_VERSION_1_2 MAKE_OBLIVION_VERSION(1, 2, 214)		// 0x010200D6
#define OBLIVION_VERSION_1_2_410 MAKE_OBLIVION_VERSION(1, 2, 410)	// 0x0102019A
#define OBLIVION_VERSION_1_2_416 MAKE_OBLIVION_VERSION(1, 2, 416)	// 0x010201A0

#define CS_VERSION_1_0 MAKE_OBLIVION_VERSION(1, 0, 303)				// 0x0100012F
#define CS_VERSION_1_2 MAKE_OBLIVION_VERSION(1, 2, 0)				// 0x01020000

#endif /* __OBSE_VERSION_H__ */
