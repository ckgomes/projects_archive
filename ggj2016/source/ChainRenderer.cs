using System;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace jam.source
{

    class ChainRenderer : IDisposable
    {

        UserInterfaceWidget chain1;
        UserInterfaceWidget chain2;
        UserInterfaceWidget skull1;
        UserInterfaceWidget skull2;
        UserInterfaceWidget ruby;

        gfx.VertexBuffer stencilQuad;

        float aspect;
        float direction = 0.0f;
        float currentOffset = 0.0f;

        MusicStream audioPlayer;
        int chainAudioHandle = 0;

        public ChainRenderer(float aspect, MusicStream audioPlayer)
        {

            chain1 = new UserInterfaceWidget($"{ContentDirectory}chain1.png", new Vector2(0.1f  / aspect, 0.1f), Vector2.Zero);
            chain2 = new UserInterfaceWidget($"{ContentDirectory}chain2.png", new Vector2(0.1f  / aspect, 0.1f ), Vector2.Zero);
            skull1 = new UserInterfaceWidget($"{ContentDirectory}skull1.png", new Vector2(0.65f / aspect, 0.65f), new Vector2(-0.8f, 0.7f));
            skull2 = new UserInterfaceWidget($"{ContentDirectory}skull2.png", new Vector2(0.65f / aspect, 0.65f), new Vector2( 0.8f, 0.7f));

            ruby = new UserInterfaceWidget($"{ContentDirectory}ruby.png",
                new Vector2(0.2f / aspect, 0.2f),
                new Vector2(0.0f, 0.85f));

            stencilQuad = gfx.VertexBufferFactory.CreateQuad(1.5f, 0.5f);

            this.aspect = aspect;

            this.audioPlayer = audioPlayer;

        }

        public void Update(float dt, float offset)
        {

            const float speed = 0.05f;
            float dist = offset - currentOffset;

            direction = Math.Sign(dist);
            currentOffset += direction * speed * (1.0f + Math.Abs(dist) * 8.5f) * dt;

            if ((Math.Abs(dist) > 0.01f) && (chainAudioHandle == 0))
            {
                chainAudioHandle = audioPlayer.Play($"{ContentDirectory}chain.ogg", true);
            }
            else if (Math.Abs(dist) <= 0.01f)
            {
                audioPlayer.Stop(chainAudioHandle);
                chainAudioHandle = 0;
            }

            if (Math.Abs(dist) < speed * dt) currentOffset = offset;

        }
        
        public void Draw(gfx.Shader shader)
        {
            float o = currentOffset % (0.1f / aspect);

            GL.ColorMask(false, false, false, false);
            GL.StencilMask(0xFF);
            GL.StencilFunc(StencilFunction.Always, 1, 0xFF);
            GL.StencilOp(StencilOp.Keep, StencilOp.Keep, StencilOp.Replace);
            stencilQuad.Bind();
            shader.LoadVector2(new Vector2(0.0f, 0.6f), "translation");
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, stencilQuad.VertexCount);
            GL.StencilFunc(StencilFunction.Equal, 1, 0xFF);
            stencilQuad.Unbind();
            GL.StencilMask(0x00);
            GL.ColorMask(true, true, true, true);

            for (float x = -0.9f; x <= 0.9f; x += (0.1f / aspect))
            {
                chain1.Position = new Vector2(x + o, 0.58f);
                chain1.Draw(shader);
            }

            ruby.Position = new Vector2(currentOffset, 0.58f);
            ruby.Draw(shader);

            GL.BlendFunc(BlendingFactorSrc.One, BlendingFactorDest.One);
            ruby.Draw(shader);
            GL.BlendFunc(BlendingFactorSrc.SrcAlpha, BlendingFactorDest.OneMinusSrcAlpha);

            for (float x = -0.9f; x <= 0.9f; x += (0.1f / aspect))
            {
                chain1.Position = new Vector2(x - o, 0.54f);
                chain1.Draw(shader);
            }

            GL.StencilFunc(StencilFunction.Always, 1, 0xFF);

            skull1.Draw(shader);
            skull2.Draw(shader);

        }

        public void Dispose()
        {
            audioPlayer.Stop(chainAudioHandle);
            chain1?.Dispose();
            chain2?.Dispose();
            skull1?.Dispose();
            skull2?.Dispose();
            ruby?.Dispose();
            stencilQuad?.Dispose();
        }

        private static string ContentDirectory = "content/";

    }

}
