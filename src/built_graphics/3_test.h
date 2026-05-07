
//{{BLOCK(__test)

//======================================================================
//
//	__test, 256x264@4, 
//	Transparent color : 00,FF,00
//	+ palette 16 entries, not compressed
//	+ 4 tiles (t|f reduced) lz77 compressed
//	+ regular map (flat), lz77 compressed, 32x33 
//	Total size: 32 + 40 + 264 = 336
//
//	Time-stamp: 2026-05-08, 00:26:43
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.6
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT___TEST_H
#define GRIT___TEST_H

#define __testTilesLen 40
extern const unsigned char __testTiles[40];

#define __testMapLen 264
extern const unsigned short __testMap[132];

#define __testPalLen 32
extern const unsigned char __testPal[32];

#endif // GRIT___TEST_H

//}}BLOCK(__test)
