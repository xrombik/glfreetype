#include "font.h"

// define ALPHABET in your language file
#include "russian.h"


Freetype::Freetype(const std::string& file_name, int font_height, float density, int dpi, float line_spacing) noexcept
{
    init(file_name, ALPHABET, ARRAY_SIZE(ALPHABET), font_height, density, dpi, line_spacing);
}

Freetype::Freetype(const std::string& file_name, unsigned int const* alphabet, unsigned int chars_count, int font_height, float density, int dpi, float line_spacing) noexcept
{
    init(file_name, alphabet, chars_count, font_height, density, dpi, line_spacing);
}

void Freetype::init(const std::string& file_name, unsigned int const* alphabet, unsigned int chars_count, int font_height, float density, int dpi, float line_spacing) noexcept
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    screen_hight = viewport[3];
    screen_width = viewport[2];

    this->density = density;

    ch_range = 0;
    unsigned int ch_min = 0;
    unsigned int ch_max = 0;
    for (unsigned int i = 0; i < chars_count; ++ i)
    {
        if (ch_max < alphabet[i])
            ch_max = alphabet[i];
        if (ch_min > alphabet[i])
            ch_min = alphabet[i];
    }
    ch_range = ch_max - ch_min + 1;
    textures = (GLuint*)malloc(ch_range * sizeof(GLuint));
    chars_widths = (unsigned int*)malloc(ch_range * sizeof(*chars_widths));
    glGetIntegerv(GL_VIEWPORT, viewport);
    dl_pscm = glGenLists(1);
    glNewList(dl_pscm, GL_COMPILE);
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(viewport[0], viewport[2], viewport[1], screen_hight, -1.0, 1.0);
    glPopAttrib();
    glEnable(GL_TEXTURE_2D);
    glEndList();

    dl_ppm = glGenLists(1);
    glNewList(dl_ppm, GL_COMPILE);
    glDisable(GL_TEXTURE_2D);
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glEndList();

    height = (unsigned int) (line_spacing * float(font_height));
    FT_Library library;
    if (FT_Init_FreeType(&library) != 0)
    {
        tprint("%s:%u:\nerror: freetype library coudn't be initialized.\n", __FILE__, __LINE__);
        exit(1);
    }
    FT_Face face;
    if (FT_New_Face(library, file_name.c_str(), 0, &face) != 0)
    {
        tprint("%s:%u:\nerror: font file \"%s\" couldn't be loaded\n", __FILE__, __LINE__, file_name.c_str());
        perror("FT_New_Face");
        exit(1);
    }
    FT_Select_Charmap(face , FT_ENCODING_UNICODE);
    FT_Set_Char_Size(face, font_height << 6, font_height << 6, dpi, dpi);
    dl_ch_base = glGenLists(ch_range);
    glGenTextures(ch_range, textures);
    glListBase(dl_ch_base);
    for (unsigned int i = 0; i < chars_count; ++ i)
    {
        chars_widths[alphabet[i]] = make_dlist(face, alphabet[i]);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}


unsigned int Freetype::make_dlist(FT_Face face, unsigned int ch) noexcept
{
    FT_UInt char_index = FT_Get_Char_Index(face, ch);
    if (char_index == 0)
    {
        tprint("%s:%u:\nerror: can't get index for char code 0x%x.\n", __FILE__, __LINE__, ch);
    }
    FT_Error error = FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
<<<<<<< HEAD
    if (error != 0)
    {
        tprint("%s:%u:\nerror: can't load glyph for char with index 0x%x.\n", __FILE__, __LINE__, char_index);
    }
    error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
    if (error != 0)
    {
=======
    if (error != 0)
    {
        tprint("%s:%u:\nerror: can't load glyph for char with index 0x%x.\n", __FILE__, __LINE__, char_index);
    }
    error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
    if (error != 0)
    {
>>>>>>> edb31a824ba9dffd33ff998941f48a49a600c035
        tprint("%s:%u:\nerror: symbol with code 0x%x was not loaded.\n", __FILE__, __LINE__, ch);
    }
    FT_GlyphSlot glyph;
    glyph = face->glyph;
    unsigned int width = next_p2(glyph->bitmap.width);
    unsigned int height = next_p2(glyph->bitmap.rows);
    unsigned int l =  2 * width * height;
    GLubyte* expanded_data = (GLubyte*) malloc(l * sizeof(GLubyte));
    if (expanded_data == NULL)
    {
        perror("error: out of memory");
        exit(EXIT_FAILURE);
    }
	memset(expanded_data, 0, l * sizeof(GLubyte));
    for(unsigned int j = 0; j < height; ++j)
    {
        for(unsigned int i = 0; i < width; ++i)
        {
            expanded_data[2 * (i + j * width)]
                = expanded_data[2 * (i + j * width) + 1]
                = (i >= glyph->bitmap.width || j >= glyph->bitmap.rows)
                ?
                    0 : glyph->bitmap.buffer[i + glyph->bitmap.width * j];
        }
    }
    glBindTexture(GL_TEXTURE_2D, textures[ch]);
    if (!glIsTexture(textures[ch]))
    {
        tprint("%s:%u:\nerror: %u - is not texture, char code: %u char range: %u\n", __FILE__, __LINE__, textures[ch], ch, ch_range);
        return 0;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
    free((void*)expanded_data);
	
    float sy = float(glyph->bitmap_top) - float(glyph->bitmap.rows);
    float x = float(glyph->bitmap.width) / float(width);
    float y = float(glyph->bitmap.rows) / float(height);
    unsigned int dl_index = dl_ch_base + ch;
    glNewList(dl_index, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, textures[ch]);
    glTranslatef((GLfloat)glyph->bitmap_left, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, sy, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2f(0.0f, (GLfloat)glyph->bitmap.rows);
    glTexCoord2d(0, y);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2d(x, y);
    glVertex2f((GLfloat)glyph->bitmap.width, 0.0f);
    glTexCoord2d(x, 0);
    glVertex2f((GLfloat)glyph->bitmap.width, (GLfloat)glyph->bitmap.rows);
    glEnd();
    glPopMatrix();
    GLfloat adv_x = (GLfloat)(float(face->glyph->advance.x >> 6) * density);
    glTranslatef(adv_x, 0.0f, 0.0f);
    glEndList();
    return (unsigned int) (GLfloat(adv_x) * density + 0.5f);
}


GLuint Freetype::get_text_width(const wchar_t *text) noexcept
{
    auto search = text_widths.find(text);
    if (search != text_widths.end())
    {
        return search->second;
    }
    unsigned int text_width = 0;
    unsigned int text_width_line = 0;
    unsigned int l = (unsigned int) wcsnlen(text, DRAW_TEXT_MAX_STRLEN);
    for (unsigned int i = 0; i < l; ++ i)
    {
        text_width += chars_widths[text[i]];
        if (text[i] == L'\n')
        {
            if (text_width > text_width_line)
            {
                text_width_line = text_width;
            }
            text_width = 0;
        }
    }
    if (text_width > text_width_line)
    {
        text_width_line = text_width;
    }
    if (text_widths.size() > DEFAULT_WIDTHS_COUNT)
    {
        text_widths.clear();
    }
    text_widths[text] = text_width_line;
    return text_width_line;
}


void Freetype::draw_text(float x, float y, const wchar_t *format, ...) const noexcept
{
    va_list args;
<<<<<<< HEAD
    va_start(args, format);
=======
>>>>>>> edb31a824ba9dffd33ff998941f48a49a600c035
    va_draw_text(x, y, DEFAUL_TEXT_COLOR, format, args);
    va_end(args);
}


void Freetype::draw_text(float x, float y, const GLubyte* color, const wchar_t *format, ...) const noexcept
{
    va_list args;
<<<<<<< HEAD
    va_start(args, format);
=======
>>>>>>> edb31a824ba9dffd33ff998941f48a49a600c035
    va_draw_text(x, y, color, format, args);
    va_end(args);
}

void Freetype::va_draw_text (float x, float y, const GLubyte *color, const wchar_t *format, va_list args) const noexcept
{
    if (!format) return;

    glCallList(dl_pscm);
    glListBase(dl_ch_base);
    glColor4ubv(color);

    wchar_t str [DRAW_TEXT_MAX_STRLEN];
    str[0] = L'\0';

    vswprintf(str, ARRAY_SIZE(str), format, args);

    std::vector<std::wstring> ss;
    split(str, L'\n', ss);
    unsigned int l = (unsigned int) ss.size();

    x = (float(x) + 1.0f) * float(screen_width) / 2.0f;
    y = (float(y) + 1.0f) * float(screen_hight) / 2.0f;
    for (unsigned int i = 0; i < l; ++ i)
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(x, y, 0.0f);
        unsigned int l1 = (unsigned int) wcsnlen (ss[i].c_str(), DRAW_TEXT_MAX_STRLEN);
        glCallLists(l1, GL_CHAR_SIZE_CODE, ss[i].c_str());
        glPopMatrix();
        if (l > 1)
        {
            y -= height;
        }
    }
    glCallList(dl_ppm);
}


void Freetype::draw_text_rotate (float x, float y, const GLubyte* color,
    float rotate, const wchar_t* format, ...) const noexcept
{
    if (format == NULL)
        return;

    glCallList(dl_pscm);
    glListBase(dl_ch_base);
    glColor4ubv(color);

    wchar_t str [DRAW_TEXT_MAX_STRLEN];
    va_list args;

    va_start(args, format);
    vswprintf(str, ARRAY_SIZE(str), format, args);
    va_end(args);

    std::vector<std::wstring> mystrings;
    split(str, L'\n', mystrings);
    unsigned int l = mystrings.size();

    x = float((x + 1) * (float(screen_width) / 2.0f));
    y = float((y + 1) * (float(screen_hight) / 2.0f));
    for (unsigned int i = 0; i < l; ++ i)
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(x, y, 0.0f);
        glRotatef(rotate, 0.0f, 0.0f, 1.0f);
        unsigned int l1 = (unsigned int) wcsnlen(mystrings[i].c_str(), DRAW_TEXT_MAX_STRLEN);
        glCallLists(l1, GL_CHAR_SIZE_CODE, mystrings[i].c_str());
        glPopMatrix();
        if (l > 1)
        {
            y -= height;
        }
    }
    glCallList(dl_ppm);
}


Freetype::~Freetype(void) noexcept
{
    glDeleteTextures(ch_range, textures);
    glDeleteLists(dl_ppm, 1);
    glDeleteLists(dl_pscm, 1);
    glDeleteLists(dl_ch_base, ch_range);
    text_widths.clear();
    free(textures);
    free(chars_widths);
}


void split(const std::wstring& str, wchar_t delimiter, std::vector<std::wstring> &tokens) noexcept
{
   std::wstring token;
   std::wistringstream tokenStream(str);
   while (std::getline(tokenStream, token, delimiter)) tokens.push_back(token);
}


unsigned int next_p2 (unsigned int const a) noexcept
{
    unsigned int rval = 1U;
    while (rval < a) rval <<= 1U;
    return rval;
}
