#include <stdint.h>
#include "kernel/memory.hxx"
#include "kernel/gfx.hxx"

boot::gop_framebuffer 	gfx::gop;
uint32_t*				gfx::buffer;

gfx::shapes::positional_point gfx::shapes::pos(uint32_t x, uint32_t y) {

	gfx::shapes::positional_point posi;
	posi.x = x;
	posi.y = y;
	return posi;
}

gfx::shapes::dimensions gfx::shapes::dims(uint32_t w, uint32_t h) {

	gfx::shapes::dimensions dimens;
	dimens.w = w;
	dimens.h = h;
	return dimens;
}

void gfx::shapes::rect(gfx::shapes::positional_point posi, gfx::shapes::dimensions dimens, uint32_t color) {

	gfx::shapes::positional_point rect_relative_center = gfx::shapes::rect_center(pos(0, 0), pos(dimens.w, dimens.h));

	posi.x -= rect_relative_center.x;
	posi.y -= rect_relative_center.y;

	gfx::shapes::positional_point rect_absolute_center = pos(posi.x + rect_relative_center.x, posi.y + rect_relative_center.y);

	for (uint32_t xx = (rect_absolute_center.x - (dimens.w / 2)); (xx <= (rect_absolute_center.x + (dimens.w / 2))) && (xx < gop.x_resolution); xx++)
		for (uint32_t yy = (rect_absolute_center.y - (dimens.h / 2)); (yy <= rect_absolute_center.y + (dimens.h / 2)) && (yy < gop.y_resolution); yy++)
			gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), color);

	gfx::screen::buff();
}

void gfx::screen::buff() 
{
//	memory::operations::memcpy(gop.framebuffer_base, buffer, gop.x_resolution * gop.y_resolution * 4);
}

void gfx::screen::save_screen()
{
	memory::operations::memcpy(buffer, gop.framebuffer_base, gop.x_resolution * gop.y_resolution * 4);
}

void gfx::screen::restore_screen()
{
	memory::operations::memcpy(gop.framebuffer_base, buffer, gop.x_resolution * gop.y_resolution * 4);
}

gfx::shapes::positional_point gfx::shapes::rect_center(gfx::shapes::positional_point posTL, gfx::shapes::positional_point posBR)
{
	return gfx::shapes::pos((posTL.x + posBR.x) / 2, (posTL.y + posBR.y) / 2);
}

const uint64_t kernel_font[] =
{
	0x0000000000000000,
	0x0000000000000000,
	0x000000FF00000000,
	0x000000FF00FF0000,
	0x1818181818181818,
	0x6C6C6C6C6C6C6C6C,
	0x181818F800000000,
	0x6C6C6CEC0CFC0000,
	0x1818181F00000000,
	0x6C6C6C6F607F0000,
	0x000000F818181818,
	0x000000FC0CEC6C6C,
	0x0000001F18181818,
	0x0000007F606F6C6C,
	0x187E7EFFFF7E7E18,  // circle  0x00187EFFFF7E1800
	0x0081818181818100,  // square
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0008000000000000,//
	0x0000000000000000,//
	0x00180018183C3C18,//!
	0x0000000000121236,//"
	0x006C6CFE6CFE6C6C,//#
	0x00187ED07C16FC30,//$$
	0x0060660C18306606,//%
	0x00DC66B61C36361C,//&
	0x0000000000181818,//'
	0x0030180C0C0C1830,//(
	0x000C18303030180C,//)
	0x0000187E3C7E1800,//*
	0x000018187E181800,//+
	0x0C18180000000000,//,
	0x000000007E000000,//-
	0x0018180000000000,//.
	0x0000060C18306000,///
	0x003C42464A52623C,//0
	0x007E101010101C10,//1
	0x007E04081020423C,//2
	0x003C42403840423C,//3
	0x0020207E22242830,//4
	0x003C4240403E027E,//5
	0x003C42423E020438,//6
	0x000404081020407E,//7
	0x003C42423C42423C,//8
	0x001C20407C42423C,//9
	0x0018180018180000,//:
	0x0C18180018180000,//;
	0x0030180C060C1830,//<
	0x0000007E007E0000,//=
	0x000C18306030180C,//>
	0x001800181830663C,//?
	0x003C06765676663C,//@
	0x0042427E42422418,//A
	0x003E42423E42423E,//B
	0x003C42020202423C,//C
	0x001E22424242221E,//D
	0x007E02023E02027E,//E
	0x000202023E02027E,//F
	0x003C42427202423C,//G
	0x004242427E424242,//H
	0x007C10101010107C,//I
	0x001C22202020207E,//J
	0x004222120E0A1222,//K
	0x007E020202020202,//L
	0x0082828292AAC682,//M
	0x00424262524A4642,//N
	0x003C42424242423C,//O
	0x000202023E42423E,//P
	0x005C22424242423C,//Q
	0x004242423E42423E,//R
	0x003C42403C02423C,//S
	0x001010101010107C,//T
	0x003C424242424242,//U
	0x0018244242424242,//V
	0x0044AAAA92828282,//W
	0x0042422418244242,//X
	0x0010101038444444,//Y
	0x007E04081020407E,//Z
	0x003E02020202023E,//[
	0x00006030180C0600,  /* //\ */
	0x007C40404040407C,  //]
	0x000000000000663C,//^
	0xFF00000000000000,//_
	0x000000000030180C,//`
	0x007C427C403C0000,//a
	0x003E4242423E0202,//b
	0x003C4202423C0000,//c
	0x007C4242427C4040,//d
	0x003C027E423C0000,//e
	0x000404043E040438,//f
	0x3C407C42427C0000,//g
	0x00424242423E0202,//h
	0x003C1010101C0018,//i
	0x0E101010101C0018,//j
	0x0042221E22420200,//k
	0x003C101010101018,//l
	0x00829292AA440000,//m
	0x00424242423E0000,//n
	0x003C4242423C0000,//o
	0x02023E42423E0000,//p
	0xC0407C42427C0000,//q
	0x00020202463A0000,//r
	0x003E403C027C0000,//s
	0x00380404043E0404,//t
	0x003C424242420000,//u
	0x0018244242420000,//v
	0x006C929292820000,//w
	0x0042241824420000,//x
	0x3C407C4242420000,//y
	0x007E0418207E0000,//z
	0x003018180E181830,//{
	0x0018181818181818,//|
	0x000C18187018180C,//}
	0x000000000062D68C,//~
	0xFFFFFFFFFFFFFFFF,
	0x1E30181E3303331E,//€
	0x007E333333003300,//
	0x001E033F331E0038,//‚
	0x00FC667C603CC37E,//ƒ
	0x007E333E301E0033,//„
	0x007E333E301E0007,//…
	0x007E333E301E0C0C,//†
	0x3C603E03033E0000,//‡
	0x003C067E663CC37E,//ˆ
	0x001E033F331E0033,//‰
	0x001E033F331E0007,//Š
	0x001E0C0C0C0E0033,//‹
	0x003C1818181C633E,//Œ
	0x001E0C0C0C0E0007,//
	0x00333F33331E0C33,//Ž
	0x00333F331E000C0C,//
	0x003F061E063F0038,//
	0x00FE33FE30FE0000,//‘
	0x007333337F33367C,//’
	0x001E33331E00331E,//“
	0x001E33331E003300,//”
	0x001E33331E000700,//•
	0x007E33333300331E,//–
	0x007E333333000700,//—
	0x1F303F3333003300,//˜
	0x001C3E63633E1C63,//™
	0x001E333333330033,//š
	0x18187E03037E1818,//›
	0x003F67060F26361C,//œ
	0x000C3F0C3F1E3333,//
	0x70337B332F1B1B0F,//ž
	0x0E1B18187E18D870,//Ÿ
	0x007E333E301E0038,//
	0x001E0C0C0C0E001C,//¡
	0x001E33331E003800,//¢
	0x007E333333003800,//£
	0x003333331F001F00,//¤
	0x00333B3F3733003F,//¥
	0x00007E007C36363C,//¦
	0x00007E003C66663C,//§
	0x001E3303060C000C,//¨
	0x000003033F000000,//©
	0x000030303F000000,//ª
	0xF81973C67C1B3363,//«
	0xC0F9F3E6CF1B3363,//¬
	0x183C3C1818001800,//­
	0x0000CC663366CC00,//®
	0x00003366CC663300,//¯
	0x1144114411441144,//°
	0x55AA55AA55AA55AA,//±
	0xEEBBEEBBEEBBEEBB,//²
	0x1818181818181818,//³
	0x1818181F18181818,//´
	0x1818181F181F1818,//µ
	0x6C6C6C6F6C6C6C6C,//¶
	0x6C6C6C7F00000000,//·
	0x1818181F181F0000,//¸
	0x6C6C6C6F606F6C6C,//¹
	0x6C6C6C6C6C6C6C6C,//º
	0x6C6C6C6F607F0000,//»
	0x0000007F606F6C6C,//¼
	0x0000007F6C6C6C6C,//½
	0x0000001F181F1818,//¾
	0x1818181F00000000,//¿
	0x000000F818181818,//À
	0x000000FF18181818,//Á
	0x181818FF00000000,//Â
	0x181818F818181818,//Ã
	0x000000FF00000000,//Ä
	0x181818FF18181818,//Å
	0x181818F818F81818,//Æ
	0x6C6C6CEC6C6C6C6C,//Ç
	0x000000FC0CEC6C6C,//È
	0x6C6C6CEC0CFC0000,//É
	0x000000FF00EF6C6C,//Ê
	0x6C6C6CEF00FF0000,//Ë
	0x6C6C6CEC0CEC6C6C,//Ì
	0x000000FF00FF0000,//Í
	0x6C6C6CEF00EF6C6C,//Î
	0x000000FF00FF1818,//Ï
	0x000000FF6C6C6C6C,//Ð
	0x181818FF00FF0000,//Ñ
	0x6C6C6CFF00000000,//Ò
	0x000000FC6C6C6C6C,//Ó
	0x000000F818F81818,//Ô
	0x181818F818F80000,//Õ
	0x6C6C6CFC00000000,//Ö
	0x6C6C6CEF6C6C6C6C,//×
	0x181818FF00FF1818,//Ø
	0x0000001F18181818,//Ù
	0x181818F800000000,//Ú
	0xFFFFFFFFFFFFFFFF,//Û
	0xFFFFFFFF00000000,//Ü
	0x0F0F0F0F0F0F0F0F,//Ý
	0xF0F0F0F0F0F0F0F0,//Þ
	0x00000000FFFFFFFF,//ß
	0x006E3B133B6E0000,//à
	0x03031F331F331E00,//á
	0x0003030303637F00,//â
	0x0036363636367F00,//ã
	0x007F660C180C667F,//ä
	0x001E3333337E0000,//å
	0x03063E6666666600,//æ
	0x00181818183B6E00,//ç
	0x3F0C1E33331E0C3F,//è
	0x001C36637F63361C,//é
	0x007736366363361C,//ê
	0x001E33333E180C38,//ë
	0x00007EDBDB7E0000,//ì
	0x03067EDBDB7E3060,//í
	0x003C06033F03063C,//î
	0x003333333333331E,//ï
	0x00003F003F003F00,//ð
	0x003F000C0C3F0C0C,//ñ
	0x003F00060C180C06,//ò
	0x003F00180C060C18,//ó
	0x1818181818D8D870,//ô
	0x0E1B1B1818181818,//õ
	0x000C0C003F000C0C,//ö
	0x0000394E00394E00,//÷
	0x000000001C36361C,//ø
	0x0000001818000000,//ù
	0x0000001800000000,//ú
	0x383C3637303030F0,//û
	0x000000363636361E,//ü
	0x0000003E061C301E,//ý
	0x00003C3C3C3C0000,//þ
	0xFFFFFFFFFFFFFFFF //ÿ
};

uint64_t gfx::fonts::get_character_font(char c) { return kernel_font[(int)c]; }