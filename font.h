#ifndef _FFT_FONT_H_
#define _FFT_FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <wchar.h>
#include <cwchar>
#include <sstream>

#include "tprint.h"


// To select one of the two values GL_UNSIGNED_INT/GL_UNSIGNED_SHORT according to the width of wchar_t
static_assert((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4), "error: sizeof(wchar_t) is not 4 or 2");
template <size_t x = sizeof(wchar_t) == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT>
class __SizeOfWchar__
{
public:
    __SizeOfWchar__(unsigned int &gl_code) { gl_code = x; }
};
typedef __SizeOfWchar__<> SizeOfWchar; // To avoid writing "__SizeOfWchar__<>" when using

#define DEFAULT_FONT_SIZE    18 // Default font size
const GLubyte DEFAUL_TEXT_COLOR [] = {255, 255, 255, 255};   // Default text color

void split (const std::wstring& str, wchar_t delimiter, std::vector<std::wstring> &tokens) noexcept;
unsigned int next_p2 (unsigned int) noexcept;

#define DEFAULT_WIDTHS_COUNT 1024


class Freetype
{
private:
    unsigned int height;
    unsigned int parent_hight;
    unsigned int range_dl;
    unsigned int *chars_widths;
    unsigned int screen_hight;
    unsigned int screen_width;
    GLuint ch_range;
    GLuint gl_char_size_code;
    GLuint dl_ch_base;
    GLuint dl_pscm;
    GLuint dl_ppm;
    GLuint *textures;
    std::unordered_map <const char*, std::vector <GLuint>> ord_map_dict;
    std::unordered_map <const wchar_t*, GLuint> text_widths;
    float density;
    GLuint make_dlist (unsigned int ch, FT_Face face) noexcept;
public:
    float s0x;
    float s0y;
    Freetype(const std::string& file_name, unsigned int screen_hight, unsigned int screen_width,
        int font_height=DEFAULT_FONT_SIZE, float density=1.0f, int dpi=96,
        float line_spacing=1.5f) noexcept;
    ~Freetype(void) noexcept;
    GLuint get_text_width(const wchar_t *) noexcept;
	/** use coordinates in range -1.0 ... 1.0 */
    void draw_text (float, float, const GLubyte *color, const wchar_t *format, ...) const noexcept;
	/** use coordinates in range -1.0 ... 1.0 */
    void draw_text_rotate (float, float, const GLubyte *color, float rotate, const wchar_t *format, ...) const noexcept;
};


#endif // _FFT_FONT_H_