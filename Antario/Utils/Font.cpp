#include "Font.h"
#include <exception>

#pragma warning(disable : 4244) // disable data loss warning

#include FT_BITMAP_H    // for bitmap conversion

constexpr DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

struct Vertex
{
    float x, y, z, rhw; /* D3DFVF_XYZRHW  */
    DWORD color;        /* D3DFVF_DIFFUSE */
    float tx, ty;       /* D3DFVF_TEX1    */
};

Font::Font(const char* strFontName, int height, LPDIRECT3DDEVICE9 pDevice, int outlineThickness)
{
    this->pDevice = pDevice;
    this->iOutlineThickness = outlineThickness;

    std::string path = R"(C:\Windows\Fonts\)";
    path += strFontName;

    /* Init freetype library  */
    if (FT_Init_FreeType(&ftLibrary))
        throw std::exception("An error occured during library initialization.");

    /* Load font face */
    if (FT_New_Face(ftLibrary, (path + ".ttf").c_str(), 0, &ftFace))
        if (FT_New_Face(ftLibrary, (path + ".ttc").c_str(), 0, &ftFace))
            throw std::exception("An error occured while creating the font.");

    /* Generate stroked outlines of the font */
    if (FT_Stroker_New(ftLibrary, &ftStroker))
        throw std::exception("An error occured while creating the stroker.");

    FT_Stroker_Set(ftStroker, 2 * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

    /* Set size of the char in pixels. Have to do some magic calc. Off by 1px after 35size.
     * More accurate than FT_Set_Char_Size(ftFace, height * 64, 0, 96, 0) */
    if (FT_Set_Pixel_Sizes(ftFace, height + int(height / 2.5), height + int(height / 2.5)))
        throw std::exception("An error occured while setting the char height");

    /* Prerender ASCII printable char textures so it wont have to do it later on. Leave special chars for runtime. */
    GenerateAsciiChars();

    /* Create vertex buffer for rendering purposes */
    this->pDevice->CreateVertexBuffer(sizeof(Vertex) * 6, NULL, D3DFVF_TLVERTEX,
                                      D3DPOOL_MANAGED, &pVertexBuffer, nullptr);

    SetupRenderStates();

    /* Get height of the string  */
    this->iHeight = GetTextDimensions(L"M", false).y;
}

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
    for (auto x : mapGlyphs)
    SAFE_RELEASE(x.second.texture);
    mapGlyphs.clear();
}

void Font::Reset(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice->Release();
    this->pDevice = pDevice;

    SAFE_RELEASE(pVertexBuffer);
    this->pDevice->CreateVertexBuffer(sizeof(Vertex) * 6, NULL, D3DFVF_TLVERTEX,
                                      D3DPOOL_MANAGED, &pVertexBuffer, nullptr);

    SAFE_RELEASE(pStateBlockOld);
    SAFE_RELEASE(pStateBlockRender);
    SetupRenderStates();

    /* Clear all glyphs data and generate it once again. */
    for (auto x : mapGlyphs)
    SAFE_RELEASE(x.second.texture);

    mapGlyphs.clear();
    GenerateAsciiChars();
}

void Font::Render(const std::string& strToRender, SPoint ptPos, DWORD flags, Color color, float scale)
{
    std::wstring ws;
    ws.assign(strToRender.begin(), strToRender.end());
    this->Render(ws, ptPos, flags, color, scale);
}

void Font::Render(const std::wstring& strToRender, SPoint ptPos, DWORD flags, Color color, float scale)
{
    D3DCOLOR col =       D3DCOLOR_ARGB(color.alpha, color.red, color.green, color.blue);
    D3DCOLOR shadowCol = D3DCOLOR_ARGB(color.alpha, 15, 15, 15);

    /* Capture and save actual state block */
    pDevice->CreateStateBlock(D3DSBT_ALL, &pStateBlockOld);
    /* Apply custom state block */
    pStateBlockRender->Apply();

    SPoint offset{0, 0};
    SPoint textDimensions{0, 0};

    /* optimalization lmao */
    if (flags) textDimensions = GetTextDimensions(strToRender, flags & FONT_DROPSHADOW);

    /* Get proper offsets for center flags */
    if (flags & FONT_CENTERED_X)
        offset.x -= textDimensions.x * 0.5;
    if (flags & FONT_CENTERED_Y)
        offset.y -= textDimensions.y * 0.5;

    for (auto it : strToRender)
    {
        /* no map entry, non-ascii char or never used before */
        if (mapGlyphs.count(it) == 0)
            CreateCharTexture(it);

        GlyphInfo gc = mapGlyphs[it];

        /* No need to render space duhh */
        if (it != ' ')
        {
            /* Adjust pos. by the size and account for half-pixel-offsets */
            float flPosX = ptPos.x + gc.bearing.x * scale - 0.5;
            /* Compare with bearing of M as it's the biggest letter.     */
            float flPosY = ptPos.y + mapGlyphs['M'].bearing.y - gc.bearing.y * scale - 0.5;

            /* Apply offsets from center flags */
            flPosX += offset.x * scale;
            flPosY += offset.y * scale;

            float w = gc.size.x * scale + 0.5;
            float h = gc.size.y * scale + 0.5;

            pDevice->SetFVF(D3DFVF_TLVERTEX);
            pDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(Vertex));

            /* Every glyph has its own texture so it has to be set on every loop */
            pDevice->SetTexture(0, gc.texture);

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
                addVertices(flPosX + textDimensions.y * 0.035, flPosY + textDimensions.y * 0.035, shadowCol);

            /* And render the original string */
            addVertices(flPosX, flPosY, col);
        }

        /* Offset position by the width of the glyph */
        ptPos.x += (gc.advance >> 6) * scale;
        ///TODO: \n and tab support
    }

    /* Apply captured state block */
    pStateBlockOld->Apply();
}


SPoint Font::GetTextDimensions(const std::wstring& str, bool bDropShadow)
{
    SPoint size = {0, 0};

    for (auto it : str)
    {
        /* no map entry, non-ascii char or never used before */
        if (mapGlyphs.count(it) == 0)
            CreateCharTexture(it);

        GlyphInfo glyph = mapGlyphs[it];

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


void Font::CreateCharTexture(wchar_t ch)
{
    FT_Bitmap bmp;
    GlyphInfo glyph;
    D3DLOCKED_RECT lockRect;

    /* Load character from font and render it to bitmap */
    FT_Load_Char(ftFace, ch, FT_LOAD_RENDER);

    /* Create new bitmap for the conversion */
    FT_Bitmap_New(&bmp);

    /* Convert bitmap to use 4 bytes per pixel */
    auto x = FT_Bitmap_Convert(ftLibrary, &ftFace->glyph->bitmap, &bmp, 4);
    ///TODO: Catch the errors

    /* save glyph info to use with rendering & text length calculations */
    glyph.size = {int(bmp.width), int(bmp.rows)};
    glyph.bearing = {int(ftFace->glyph->bitmap_left), int(ftFace->glyph->bitmap_top)};
    glyph.advance = ftFace->glyph->advance.x;

    /* create new texture for our glyph */
    IDirect3DTexture9* tx = nullptr;
    auto err = D3DXCreateTexture(pDevice, bmp.width, bmp.rows, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &tx);
    ///TODO: Catch the errors

    /* Render to texture by copying bitmap data to locked rect */
    tx->LockRect(0, &lockRect, nullptr, D3DLOCK_DISCARD);
    memcpy(lockRect.pBits, bmp.buffer, glyph.size.y * bmp.pitch);
    tx->UnlockRect(0);

    /* Save texture pointer within stored glyph info */
    glyph.texture = tx;

    /* save glyph with the texture inside glyph map  */
    mapGlyphs.insert({ int(ch), glyph });
}

void Font::GenerateAsciiChars()
{
    for (int c = 32; c < 127; c++)
        CreateCharTexture(c);
}

void Font::SetupRenderStates()
{
    pDevice->BeginStateBlock();

    /* Alphablending to remove black background from texture */
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
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
    pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    pDevice->EndStateBlock(&pStateBlockRender);
}
