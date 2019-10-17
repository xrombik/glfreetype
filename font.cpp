#include "font.h"


unsigned int const ALPHABET [] =
	   {L'й', L'э', L'ц', L'у', L'к', L'е', L'н', L'г', L'ш', L'щ',
		L'з', L'х', L'ъ', L'ф', L'ы', L'в', L'а', L'п', L'р', L'о',
		L'л', L'д', L'ж', L'э', L'я', L'ч', L'с', L'м', L'и', L'т',
		L'ь', L'б', L'ю', L'Й', L'Ц', L'У', L'К', L'Е', L'Н', L'Г',
		L'Ш', L'Щ', L'З', L'Х', L'Ъ', L'Ф', L'Ы', L'В', L'А', L'П',
		L'Р', L'О', L'Л', L'Д', L'Ж', L'Э', L'Я', L'Ч', L'С', L'М',
		L'И', L'Т', L'Ь', L'Б', L'Ю', L'Q', L'W', L'E', L'R', L'T',
		L'Y', L'L', L'I', L'O', L'P', L'A', L'S', L'D', L'F', L'G',
		L'H', L'J', L'K', L'L', L'Z', L'X', L'C', L'V', L'B', L'N',
		L'M', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9',
		L'0', L'-', L'=', L'!', L'@', L'#', L'$', L'%', L'^', L'&',
		L'*', L'(', L')', L'_', L'+', L'`', L'ё', L'!', L'\"',L';',
		L'%', L':', L'?', L'.', L'/', L'\\',L'<', L'>', L',', L' ',
		L'q', L'w', L'e', L'r', L't', L'y', L'L', L'i', L'o', L'p',
		L'a', L's', L'd', L'f', L'g', L'h', L'j', L'k', L'l', L'z',
        L'x', L'c', L'v', L'b', L'n', L'm', L'[', L']', L'{', L'}',
        L'U', L'Ё'};


Freetype::Freetype (const std::string& file_name, unsigned int screen_hight,
    unsigned int screen_width, int font_height,
    float density, int dpi, float line_spacing) noexcept
{
    SizeOfWchar(this->gl_char_size_code);
    unsigned int range_dl = 0;
    GLint viewport[4];
    this->screen_hight = screen_hight;
    this->screen_width = screen_width;

    this->s0x = 0.0f;
    this->s0y = 0.0f;
    this->density = density;

    for (unsigned int i = 0; i < sizeof(ALPHABET) / sizeof(ALPHABET[0]); ++ i)
    {
        if (range_dl < ALPHABET[i])
            range_dl = ALPHABET[i];
    }
    this->ch_range = range_dl;
    this->textures = (GLuint*) std::malloc ((this->ch_range + 1) * sizeof(GLuint));
    this->chars_widths = (unsigned int *) std::malloc ((this->ch_range + 1) * sizeof(unsigned int));
    glGetIntegerv(GL_VIEWPORT, viewport);
    this->parent_hight = screen_hight;
    this->dl_pscm = glGenLists (1);
    glNewList(this->dl_pscm, GL_COMPILE);
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(viewport[0], viewport[2], viewport[1], screen_hight, -1, 1);
    glPopAttrib();
    glEnable(GL_TEXTURE_2D);
    glEndList();

    this->dl_ppm = glGenLists(1);
    glNewList(this->dl_ppm, GL_COMPILE);
    glDisable(GL_TEXTURE_2D);
    glPushAttrib(GL_TRANSFORM_BIT);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glEndList();

    this->height = (unsigned int) (line_spacing * float(font_height));
    this->range_dl = range_dl;
    FT_Library library;
    if (FT_Init_FreeType(&library) != 0)
    {
        tprint ("%s:%u:\nerror: freetype library не работает.\n", __FILE__, __LINE__);
        exit(1);
    }
    FT_Face face;
    if (FT_New_Face(library, file_name.c_str(), 0, &face) != 0)
    {
        tprint ("%s:%u:\nerror: font file \"%s\" couldn't be loaded\n", __FILE__, __LINE__, file_name.c_str());
        exit(1);
    }

    FT_Set_Char_Size(face, font_height << 6, font_height << 6, dpi, dpi);
    this->dl_ch_base = glGenLists(this->ch_range);
    glGenTextures(this->ch_range, this->textures);
    glListBase(this->dl_ch_base);
    for (unsigned int i = 0; i < (sizeof(ALPHABET) / sizeof(ALPHABET[0])); ++ i)
    {
        this->chars_widths[ALPHABET[i]] = this->make_dlist(ALPHABET[i], face);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}


GLuint Freetype::make_dlist(unsigned int ch, FT_Face face) noexcept
{
    FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT);
    FT_Error error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
    if (error != 0)
    {
        tprint ("%s:%u:\nsymbol with code 0x%x was not loaded.\n", __FILE__, __LINE__, ch);
    }
    FT_GlyphSlot glyph;
    glyph = face->glyph;
    unsigned int width = next_p2(glyph->bitmap.width);
    unsigned int height = next_p2(glyph->bitmap.rows);
    unsigned int l =  2 * width * height;
    GLubyte* expanded_data = (GLubyte*) std::malloc(l * sizeof(GLubyte));
    if (expanded_data == NULL)
    {
        perror("out of memory");
        exit(EXIT_FAILURE);
    }
	memset(expanded_data, 0, l * sizeof(GLubyte));
    for(unsigned int j = 0; j < height; ++j)
    {
        for(unsigned int i = 0; i < width; ++i)
        {
            expanded_data[2 * (i + j * width)] = expanded_data[2 * (i + j * width) + 1] =
            (i>=glyph->bitmap.width || j>=glyph->bitmap.rows) ?
            0 : glyph->bitmap.buffer[i + glyph->bitmap.width*j];
        }
    }
    glBindTexture(GL_TEXTURE_2D, this->textures[ch]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
    std::free((void*)expanded_data);
	
    float sy = float(glyph->bitmap_top) - float(glyph->bitmap.rows);
    float x = float(glyph->bitmap.width) / float(width);
    float y = float(glyph->bitmap.rows) / float(height);
    unsigned int dl_index = this->dl_ch_base + ch;
    glNewList(dl_index, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, this->textures[ch]);
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
    GLfloat adv_x = (GLfloat)(float(face->glyph->advance.x >> 6) * this->density);
    glTranslatef(adv_x, 0.0f, 0.0f);
    glEndList();
    return (GLuint) (GLfloat(adv_x) * this->density + 0.5f);
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
    unsigned int l = (unsigned int) wcslen(text);
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


void Freetype::draw_text (float x, float y, const GLubyte *color, const wchar_t *format, ...) const noexcept
{
    if (!format) return;

    glCallList(this->dl_pscm);
    glListBase(this->dl_ch_base);
    glColor4ubv(color);

    wchar_t str [2048];
    str[0] = L'\0';

    va_list args;
    va_start(args, format);
    vswprintf(str, sizeof(str) / sizeof(str[0]), format, args);
    va_end(args);

    std::vector<std::wstring> ss;
    split(str, L'\n', ss);
    unsigned int l = (unsigned int) ss.size();

    x = (float(x) + 1.0f) * float(this->screen_width) / 2.0f + float(this->s0x) * float(this->screen_width) / 2.0f;
    y = (float(y) + 1.0f) * float(this->screen_hight) / 2.0f + float(this->s0y) * float(this->screen_hight) / 2.0f;
    for (unsigned int i = 0; i < l; ++ i)
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(x, y, 0.0f);
        unsigned int l1 = (unsigned int) wcslen (ss[i].c_str());
        glCallLists(l1, this->gl_char_size_code, ss[i].c_str());
        glPopMatrix();
        if (l > 1)
        {
            y -= this->height;
        }
    }
    glCallList(this->dl_ppm);
}


void Freetype::draw_text_rotate (float x, float y, const GLubyte* color,
    float rotate, const wchar_t* format, ...) const noexcept
{
    if (format == NULL)
        return;

    glCallList(dl_pscm);
    glListBase(dl_ch_base);
    glColor4ubv(color);

    wchar_t str [2048];
    va_list args;

    va_start(args, format);
    vswprintf(str, sizeof(str) / sizeof(str[0]), format, args);
    va_end(args);

    std::vector<std::wstring> mystrings;
    split(str, L'\n', mystrings);
    unsigned int l = mystrings.size();

    x = float((x + 1) * (float(screen_width) / 2.0f)) + (s0x * (float(screen_width) / 2.0f));
    y = float((y + 1) * (float(screen_hight) / 2.0f)) + (s0y * (float(screen_hight) / 2.0f));
    for (unsigned int i = 0; i < l; ++ i)
    {
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(x, y, 0.0f);
        glRotatef(rotate, 0.0f, 0.0f, 1.0f);
        unsigned int l1 = (unsigned int) wcslen(mystrings[i].c_str());
        glCallLists(l1, gl_char_size_code, mystrings[i].c_str());
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
    std::free(textures);
    std::free(chars_widths);
}


void split(const std::wstring& str, wchar_t delimiter, std::vector<std::wstring> &tokens) noexcept
{
   std::wstring token;
   std::wistringstream tokenStream(str);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
}


unsigned int next_p2 (unsigned int const a) noexcept
{
    unsigned int rval = 1U;
    while (rval < a) rval <<= 1U;
    return rval;
}
