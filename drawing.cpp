#include "drawing.h"

#include "pastelpalette.h"

static bool InitDoneFor3D = false;
void InitMeshDrawing()
{
    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0); // Position 3 x f32
    // GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0); // Normals 3 x f32
    // GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); // TextureCoords 2x f32
    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0); // Color 1x r,g,b
    InitDoneFor3D = true;
}

void DrawMesh(glm::vec3 position, u_int color,  Mesh& mesh)
{
    gdl_assert(InitDoneFor3D, "Drawing has not been initialized for 3D drawing!");
    if (InitDoneFor3D == false){
        return;
    }

    gdl_assert(mesh.GetTriangleCount() > 0, "Given mesh has no triangles to draw");

    gdl::Set3DMode(1000.0f); // Far plane

    // Set up camera
    guVector camera = {0.0f, 0.0f, 0.0f};
    guVector up = {0.0f, 1.0f, 0.0f};
    guVector look = {0.0f, 0.0f, -1.0f};

    Mtx viewMatrix;

    guLookAt(viewMatrix, &camera, &up, &look);

    // Translate and set the position matrix
    guMtxIdentity(gdl::wii::ModelMtx);
    guMtxApplyTrans(gdl::wii::ModelMtx, gdl::wii::ModelMtx, position.x, position.y, position.z);
    guMtxConcat(viewMatrix, gdl::wii::ModelMtx, gdl::wii::ModelMtx);
    GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);

    // GX_ClearVtxDesc(); // Done by Set3DMode
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR); // No textures

    GX_SetCullMode(GX_CULL_FRONT);

    // What to draw, in what format, how many vertices
    GX_Begin(GX_TRIANGLES, GX_VTXFMT1, mesh.positions.size());
    {
        for(int i = 0; i < mesh.GetTriangleCount(); i++)
        {
            glm::vec3& vrtx = mesh.positions[i*3 + 0];
            GX_Position3f32(vrtx.x, vrtx.y, vrtx.z);
            GX_Color3u8(RED(color), GREEN(color), BLUE(color));

            vrtx = mesh.positions[i*3 + 1];
            GX_Position3f32(vrtx.x, vrtx.y, vrtx.z);
            GX_Color3u8(RED(color), GREEN(color), BLUE(color));

            vrtx = mesh.positions[i*3 + 2];
            GX_Position3f32(vrtx.x, vrtx.y, vrtx.z);
            GX_Color3u8(RED(color), GREEN(color), BLUE(color));
        }
    }
    GX_End();

    // Return to 2D
    gdl::Set2DMode(100);
    GX_SetCullMode(GX_CULL_NONE);
}


void DrawTextDouble(std::string text, short x, short y, float scale, gdl::FFont* font, 
int lastLetter, float lastLetterProgress, u_int color, int letterWidth)
{
    gdl_assert(((u_int)lastLetter < text.length()), "DrawTextDouble index out of text %u>=%u", lastLetter, text.length());
    int lw = letterWidth;
    if (lw < 0)
    {
        lw = font->GetWidth();
    }
    int wordWidth = lw * text.length() * scale;
    int w = wordWidth/text.length();
    char xx[2];
    int startX = x-wordWidth/2;

    int dx;
    int dy;
    for (int i = 0; i <= lastLetter; i++)
    {
        dx = startX + i * w;
        dy = y;
        if (i == lastLetter)
        {
            // Center the letter that is growing bigger
            // Add to halfway and go back
            float hw = lw * scale * 0.5f;
            float hh = font->GetHeight()* scale * 0.5f;
            dx += (hw) - (hw*lastLetterProgress);
            dy += (hh) - (hh*lastLetterProgress);
            scale *= lastLetterProgress;
        }
        gdl_assert((text[i]!=0), "DrawTextDouble tried to draw NULL character");
        sprintf(xx, "%c", text[i]);

        // Move the shadow with line progress

        font->DrawText(xx, dx + 4*scale, dy + 4*scale, scale, palette[0]);
        font->DrawText(xx, dx, dy, scale, color);
    }
}

Vector2 GetFruitPosition(std::string text, short x, short y, float scale, gdl::SpriteSet* sprites, 
int letterIndex)
{
    const gdl::Sprite* first = sprites->SpriteInfo(0);
    short spriteW = first->w * scale;
    int wordWidth = text.length() * spriteW;
    int w = wordWidth/text.length();
    int startX = x-wordWidth/2;

    return Vector2(startX + letterIndex * w, y);
}

void DrawFruitsDouble(std::string text, short x, short y, float scale, gdl::SpriteSet* sprites, 
int firstLetter, int lastLetter, u_int spriteIndex)
{
    gdl_assert(((u_int)lastLetter < text.length()), "DrawFruitsDouble index out of text %u>=%u", lastLetter, text.length());

    const gdl::Sprite* first = sprites->SpriteInfo(0);
    short spriteW = first->w * scale;
    int wordWidth = text.length() * spriteW;
    int w = wordWidth/text.length();
    int startX = x-wordWidth/2;

    int dx;
    int dy;
    for (int i = firstLetter; i <= lastLetter; i++)
    {
        if (text[i] == ' ')
        {
            continue;
        }
        dx = startX + i * w;
        dy = y;
        sprites->Put(dx + 4*scale, dy + 4*scale, spriteIndex, palette[0], 0,0, scale);
        sprites->Put(dx, dy, spriteIndex, gdl::Color::White, 0,0, scale);
    }

}