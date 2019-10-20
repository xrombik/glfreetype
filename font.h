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
#include <stdlib.h>

#include "tprint.h"


#define DEFAULT_WIDTHS_COUNT 1024
#define DRAW_TEXT_MAX_STRLEN 2048
#define DEFAULT_FONT_SIZE    18

/** select one of the two values GL_UNSIGNED_INT/GL_UNSIGNED_SHORT according to the width of wchar_t */
#define GL_CHAR_SIZE_CODE (sizeof(wchar_t) == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT)

/** ... */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static_assert((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4), "error: sizeof(wchar_t) is not 4 or 2");
const GLubyte DEFAUL_TEXT_COLOR [] = {255, 255, 255, 255};   // Default text color
void split (const std::wstring& str, wchar_t delimiter, std::vector<std::wstring> &tokens) noexcept;
unsigned int next_p2 (unsigned int) noexcept;


class Freetype
{
private:
    unsigned int height;
    unsigned int *chars_widths;
    unsigned int screen_hight;
    unsigned int screen_width;
    GLuint ch_range;
    GLuint dl_ch_base;
    GLuint dl_pscm;
    GLuint dl_ppm;
    GLuint *textures;
    std::unordered_map <const char*, std::vector <GLuint>> ord_map_dict;
    std::unordered_map <const wchar_t*, GLuint> text_widths;
    float density;
    unsigned int make_dlist(FT_Face face, unsigned int ch) noexcept;
    void va_draw_text (float, float, const GLubyte *color, const wchar_t *format, va_list argp) const noexcept;
    void init(const std::string& file_name, unsigned int const *alphabet, unsigned int alphabet_count, int font_height=DEFAULT_FONT_SIZE, float density=1.0f, int dpi=96, float line_spacing=1.5f) noexcept;
public:
    void draw_text(float x, float y, const wchar_t *format, ...) const noexcept;
    void draw_text(float x, float y, const GLubyte *color, const wchar_t *format, ...) const noexcept;
    Freetype(const std::string& file_name, int font_height=DEFAULT_FONT_SIZE, float density=1.0f, int dpi=96, float line_spacing=1.5f) noexcept;
    Freetype(const std::string& file_name, unsigned int const *alphabet, unsigned int alphabet_count, int font_height=DEFAULT_FONT_SIZE, float density=1.0f, int dpi=96, float line_spacing=1.5f) noexcept;
    ~Freetype(void) noexcept;
    GLuint get_text_width(const wchar_t*) noexcept;
    void draw_text_rotate(float, float, const GLubyte *color, float rotate, const wchar_t *format, ...) const noexcept;
};


#endif // _FFT_FONT_H_
