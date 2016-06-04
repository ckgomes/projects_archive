using System;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace jam.source
{

    class HealthBar
    {

        gfx.Texture2D ruby;
        gfx.VertexBuffer quad;

        public float PositionX { get; set; }

        public HealthBar()
        {
            ruby = new gfx.Texture2D($"{ContentDirectory}ruby.png");
            quad = gfx.VertexBufferFactory.CreateQuad(0.2f, 0.2f);
            PositionX = 0.0f;
        }

        public void Draw(gfx.Shader shader)
        {
            shader.Bind();
            ruby.Bind(0);
            quad.Bind();
            shader.LoadVector2(new Vector2(PositionX, 0.85f), "translation");
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, quad.VertexCount);
            quad.Unbind();
            ruby.Unbind(0);
            shader.Unbind();
        }

        private static string ContentDirectory = "content/";


    }

}
