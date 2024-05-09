#pragma once
// Pastel POP-16
// https://lospec.com/palette-list/pastel-pop-16-color

enum PaletteColos {
    PastelWhite = 1,
    PastelDarkPurple = 4,
    PastelNavy = 5
};

static u_int palette[] = 
{
    0x000000FF, // 0: Black
    0xfffcffFF, // 1: White
    0xe6e1f2FF, // 2: Grey
    0xb2b7e1FF, // 3: light purple

    0x595b7dFF, // 4: Dark purple
    0x7b8ac6FF, // 5: Navy
    0xffd9f4FF, // 6: Grey rose
    0xe1adc3FF, // 7: brown rose

    0xad80a6FF, // 8: dark rose
    0xfba2d7FF, // 9: rose pink
    0xfae0c7FF, // 10: Yellow
    0xf0ababFF, // 11: orange

    0x97c4aaFF, // 12: green
    0xbfedf5FF, // 13: Light teal
    0x73c9ebFF, // 14: Teal
    0xcaaff5FF, // 15: violet
};
