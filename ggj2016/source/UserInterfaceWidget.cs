using System;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace jam.source
{

    class UserInterfaceWidget : IDisposable
    {

        gfx.Texture2D texture;
        gfx.VertexBuffer quad;

        public Vector2 Position { get; set; }

        public UserInterfaceWidget(string path, Vector2 size, Vector2 position)
        {
            texture = new gfx.Texture2D(path);
            quad = gfx.VertexBufferFactory.CreateQuad(size.X, size.Y);
            Position = position;
        }

        public void Draw(gfx.Shader shader)
        {
            shader.Bind();
            texture.Bind(0);
            quad.Bind();
            shader.LoadVector2(Position, "translation");
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, quad.VertexCount);
            quad.Unbind();
            texture.Unbind(0);
            shader.Unbind();
        }

        public void Dispose()
        {
            texture?.Dispose();
            quad?.Dispose();
        }
    }

}
