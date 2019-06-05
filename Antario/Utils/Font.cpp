#include "Font.h"
#include <exception>

/* SHGetFolderPathA */
#include <shlobj_core.h>

/* disable data loss warning */
#pragma warning(disable : 4244) 

/* FVF for the vertex */
constexpr DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

/* for bitmap conversion */
#include FT_BITMAP_H    

struct Vertex
{
    float x, y, z, rhw; /* D3DFVF_XYZRHW  */
    DWORD color;        /* D3DFVF_DIFFUSE */
    float tx, ty;       /* D3DFVF_TEX1    */
};


Font::Font(const char* strFontName, int height, bool bAntialias, LPDIRECT3DDEVICE9 pDevice, int outlineThickness)
{
    this->pDevice =           pDevice;
    this->bIsAntialiased =    bAntialias;
    this->iOutlineThickness = outlineThickness;

    /* Init freetype library  */
    if (FT_Init_FreeType(&ftLibrary))
        throw std::exception("An error occured during library initialization.");
    
    /* Load font face */
    if (FT_New_Face(ftLibrary, GetFontPath(strFontName).c_str(), 0, &ftFace))
        throw std::exception("An error occured while creating the font.");

    ///* Generate stroked outlines of the font */
    //if (FT_Stroker_New(ftLibrary, &ftStroker))
    //    ftStroker = nullptr;

    ///* Set stroker attributes */
    //if (ftStroker)
    //    FT_Stroker_Set(ftStroker, 2 * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

    /* Set size of the char in pixels. Have to do some magic calc. Off by 1px after 35size.
     * More accurate than FT_Set_Char_Size(ftFace, height * 64, 0, 96, 0) */
    if (FT_Set_Pixel_Sizes(ftFace, height + int(height / 2.5), height + int(height / 2.5)))
        throw std::exception("An error occured while setting the char height");

    /* Prerender ASCII printable char textures so it wont have to do it later on. Leave special chars for runtime. */
    GenerateAsciiChars();

    /* Create vertex buffer for rendering purposes */
    this->pDevice->CreateVertexBuffer(sizeof(Vertex) * 12, NULL, D3DFVF_TLVERTEX,
                                      D3DPOOL_MANAGED, &pVertexBuffer, nullptr);

    SetupRenderStates();

    /* Get height of the string  */
    this->iHeight = GetTextDimensions(L"M", true).y;
}

\
void Font::Release()
{
    if (!ftLibrary)
        return;

    FT_Done_FreeType(ftLibrary);
    ftLibrary = nullptr;

    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pVertexBuffer);
    SAFE_RELEASE(pStateBlockOld);
    SAFE_RELEASE(pStateBlockRender);

    /* Clear all glyphs data */
    for (auto x : mapGlyphs)
        SAFE_RELEASE(x.second.texture);

    mapGlyphs.clear();
}


void Font::OnLostDevice()
{
    pDevice = nullptr;
    SAFE_RELEASE(pVertexBuffer);
    SAFE_RELEASE(pStateBlockOld);
    SAFE_RELEASE(pStateBlockRender);
}


void Font::OnResetDevice(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice = pDevice;
    this->pDevice->CreateVertexBuffer(sizeof(Vertex) * 12, NULL, D3DFVF_TLVERTEX,
                                      D3DPOOL_MANAGED, &pVertexBuffer, nullptr);
    SetupRenderStates();
}

template <typename T>
void Font::Render(const T* strToRender, SPoint ptPos, DWORD flags, Color color, float scale)
{
    /* If we're using C-style string - convert it to iterable stl-style one */
    using string_type = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;
    this->Render(string_type(strToRender), ptPos, flags, color, scale);
}

template<typename T> 
void Font::Render(const T& strToRender, SPoint ptPos, DWORD flags, Color color, float scale)
{
    D3DCOLOR col =       D3DCOLOR_ARGB(color.alpha, color.red, color.green, color.blue);
    D3DCOLOR shadowCol = D3DCOLOR_ARGB(color.alpha, 15, 15, 15);

    /* Capture current state block */
    pStateBlockOld->Capture();
    /* Apply custom state block */
    pStateBlockRender->Apply();

    SPoint offset{0, 0};
    SPoint textDimensions{0, 0};

    /* Get text timensions if we use any flags (for text positioning etc.) */
    if (flags) textDimensions = GetTextDimensions(strToRender, flags & FONT_DROPSHADOW);

    /* Get proper offsets for center flags */
    if (flags & FONT_CENTERED_X)
        offset.x -= textDimensions.x * 0.5;
    if (flags & FONT_CENTERED_Y)
        offset.y -= textDimensions.y * 0.5;

    /* Height of "M" letter - the highest one. */
    const auto bearingM = mapGlyphs['M'].bearing.y;

    for (auto it : strToRender)
    {
        const GlyphInfo glyph = mapGlyphs[it];

        /* no texture so default generated - non-ascii char or never used before */
        if (!glyph.texture)
            CreateCharTexture(it);

        /* No need to render space duhh */
        if (it != ' ')
        {
            /* Adjust pos. by the size and account for half-pixel-offsets             */
            float flPosX = ptPos.x + glyph.bearing.x * scale - 0.5;
            /* Compare with bearingM so its lined up on the bottom instead of the top */
            float flPosY = ptPos.y + bearingM - glyph.bearing.y * scale - 0.5;

            /* Apply offsets from center flags */
            flPosX += offset.x * scale;
            flPosY += offset.y * scale;

            float w = glyph.size.x * scale + 0.5;
            float h = glyph.size.y * scale + 0.5;

            pDevice->SetFVF(D3DFVF_TLVERTEX);
            pDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(Vertex));
            pDevice->SetTextureStageState(0, D3DTSS_COLOROP, glyph.colored ? D3DTOP_SELECTARG2 : D3DTOP_SELECTARG1);

            /* Every glyph has its own texture so it has to be set on every loop */
            pDevice->SetTexture(0, glyph.texture);

            Vertex* vertices;

            /* Create lambda function so we wont write the same code twice... */
            const auto addVertices = [&](float xpos, float ypos, D3DCOLOR dwcol)
            {
                /* Lock the vertex buffer */
                pVertexBuffer->Lock(0, 0, (void**)&vertices, NULL);
                *vertices++ = {xpos,     ypos + h, 1.0, 1.0, dwcol, 0.0, 1.0};
                *vertices++ = {xpos + w, ypos + h, 1.0, 1.0, dwcol, 1.0, 1.0};
                *vertices++ = {xpos,     ypos,     1.0, 1.0, dwcol, 0.0, 0.0};

                *vertices++ = {xpos + w, ypos + h, 1.0, 1.0, dwcol, 1.0, 1.0};
                *vertices++ = {xpos + w, ypos,     1.0, 1.0, dwcol, 1.0, 0.0};
                *vertices++ = {xpos,     ypos,     1.0, 1.0, dwcol, 0.0, 0.0};

                /* Unlock the vertex buffer */
                pVertexBuffer->Unlock();
                pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
            };

            /* Render dark shadow below the text we draw and offset it by a fraction of the height */
            if (flags & FONT_DROPSHADOW)
            {
                auto shadowOffset = textDimensions.y * 0.035;
                shadowOffset = shadowOffset < 1.1f ? 1.f : shadowOffset;
                addVertices(flPosX + shadowOffset, flPosY + shadowOffset, shadowCol);
            }

            /* And render the original string */
            addVertices(flPosX, flPosY, col);
        }

        /* Offset position by the width of the glyph */
        ptPos.x += (glyph.advance >> 6) * scale;
        ///TODO: \n and tab support
    }

    /* Apply captured state block */
    pStateBlockOld->Apply();
}

template<typename T>
SPoint Font::GetTextDimensions(const T& str, bool bDropShadow)
{
    SPoint size = {0, 0};

    for (auto it : str)
    {
        GlyphInfo glyph = mapGlyphs[it];

        /* no texture so default generated - non-ascii char or never used before */
        if (!glyph.texture)
            CreateCharTexture(it);

        /* Horizontal size grows with every letter */
        size.x += int(glyph.advance >> 6);

        /* While vertical size is equal to max height of the letter. */
        if (size.y < glyph.size.y)
            size.y = glyph.size.y;

        ///TODO: Support for \n
    }
    if (bDropShadow)
        size += {int(size.y * 0.035), int(size.y * 0.035) };

    return size;
}


template<typename T>
void Font::CreateCharTexture(T ch)
{
    FT_Bitmap bmp;
    GlyphInfo glyph;
    D3DLOCKED_RECT lockRect;

    uint32_t flags = FT_LOAD_RENDER;
    flags |= bIsAntialiased       ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO;    
    flags |= FT_HAS_COLOR(ftFace) ? FT_LOAD_COLOR : 0;         /* Some fonts (emoji lol) contain colors. */

    /* Load character from font and render it to bitmap */
    FT_Load_Char(ftFace, ch, flags);    

    /* Create new bitmap for the conversion */
    FT_Bitmap_New(&bmp);

    /* Convert bitmap to use 4 bytes per pixel */
    auto err = FT_Bitmap_Convert(ftLibrary, &ftFace->glyph->bitmap, &bmp, 4);
    ///TODO: Catch the errors

    /* save glyph info to use with rendering & text length calculations */
    glyph.size =    {int(bmp.width), int(bmp.rows)};
    glyph.bearing = {int(ftFace->glyph->bitmap_left), int(ftFace->glyph->bitmap_top)};
    glyph.advance = ftFace->glyph->advance.x;

    /* Monochrome mode contains only 1 and 0 (lit / not lit). Change it to 0 - 255 for A8 format. */
    if (!bIsAntialiased)        
        for (auto it = bmp.buffer; it != &bmp.buffer[bmp.rows * bmp.pitch]; it++)
            *it *= 255;

    glyph.colored = ftFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA;

    /* create new texture for our glyph */
    IDirect3DTexture9* tx = nullptr;
    HRESULT error = D3DXCreateTexture(pDevice, bmp.width, bmp.rows, 1, 0, glyph.colored ? D3DFMT_A8R8G8B8 : D3DFMT_A8, D3DPOOL_MANAGED, &tx);
    ///TODO: Catch the errors

    /* Render to texture by copying bitmap data to locked rect */
    tx->LockRect(0, &lockRect, nullptr, D3DLOCK_DISCARD);

    /* If the glyph is colored - use original non-converted bitmap(since its using argb/brga format) */
    if (glyph.colored)
        memcpy(lockRect.pBits, ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.rows * ftFace->glyph->bitmap.pitch);
    else
        memcpy(lockRect.pBits, bmp.buffer, glyph.size.y * bmp.pitch);

    tx->UnlockRect(0);

    /* Save texture pointer within stored glyph info */
    glyph.texture = tx;

    /* save glyph with the texture inside glyph map. */
    mapGlyphs[ch] = glyph;

    FT_Bitmap_Done(ftLibrary, &bmp);
}


void Font::GenerateAsciiChars()
{
    for (int c = 32; c < 127; c++)
        CreateCharTexture(c);
}


void Font::SetupRenderStates()
{
    /* Create stateblocks. Need to create old by hand and capture later on */
    for (int i = 0; i < 2; i++)
    {
        pDevice->BeginStateBlock();


        ///* Alphablending to remove black background from texture */
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
        pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);


        pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
        pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
        pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

        pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
        pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

        pDevice->EndStateBlock(i == 0 ? &pStateBlockRender : &pStateBlockOld);
    }
}

/* Windows only, will be totally different on linux */
std::string Font::GetFontPath(const char* strFontName)
{
    HKEY hkey;
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hkey);

    std::string strPath;

    char strBuffer[MAX_PATH];
    int iterator = 0;
    /* Interate through all values of that key (to get font names etc) */
    while (true)
    {
        /* Set buffer values to null */
        memset(strBuffer, 0, MAX_PATH);

        DWORD retBufSize = MAX_PATH;
        /* Export font name as a name of the value  */
        auto retCode = RegEnumValueA(hkey, iterator, strBuffer, &retBufSize, 0, 0, 0, 0);

        if (retCode != ERROR_SUCCESS)
            return nullptr;

        /* Check if that font name is the one we are looking for */
        if (std::string(strBuffer).find(strFontName) != std::string::npos)
        {
            retBufSize = MAX_PATH;
            RegQueryValueExA(hkey, strBuffer, 0, 0, (LPBYTE)strBuffer, &retBufSize);
            strPath = strBuffer;
            break;
        }
        iterator++;
    }

    memset(strBuffer, 0, MAX_PATH);
    /* Get default font folder */
    SHGetFolderPathA(0, CSIDL_FONTS, 0, 0, strBuffer);

    return std::string(strBuffer) + '\\' + strPath;
}


/* Declare proper template arguments so it uses only these (+ can stay in cpp file :)) */

template void Font::Render<std::string>(const std::string& strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<std::wstring>(const std::wstring& strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<std::u16string>(const std::u16string& strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<std::u32string>(const std::u32string& strToRender, SPoint ptPos, DWORD flags, Color color, float scale);

template void Font::Render<char>(const char* strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<wchar_t>(const wchar_t* strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<char16_t>(const char16_t* strToRender, SPoint ptPos, DWORD flags, Color color, float scale);
template void Font::Render<char32_t>(const char32_t* strToRender, SPoint ptPos, DWORD flags, Color color, float scale);

template SPoint Font::GetTextDimensions<std::string>(const std::string& str, bool bDropShadow);
template SPoint Font::GetTextDimensions<std::wstring>(const std::wstring& str, bool bDropShadow);
template SPoint Font::GetTextDimensions<std::u16string>(const std::u16string& str, bool bDropShadow);
template SPoint Font::GetTextDimensions<std::u32string>(const std::u32string& str, bool bDropShadow);

template void Font::CreateCharTexture<int>(int ch);
template void Font::CreateCharTexture<char>(char ch);
template void Font::CreateCharTexture<wchar_t>(wchar_t ch);
template void Font::CreateCharTexture<char16_t>(char16_t ch);
template void Font::CreateCharTexture<char32_t>(char32_t ch);