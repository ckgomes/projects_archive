using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace jam.source
{

    class Sprite : IDisposable
    {

        public int CurrentFrame { get; private set; } = 0;

        int startFrame = 0;
        int endFrame = 0;

        gfx.Texture2D texture;
        gfx.VertexBuffer vbo;

        float frameTime;
        float frameDuration;

        float offset;
        float scale;

        public Sprite(string path, int frames, float scale)
        {
            texture = new gfx.Texture2D(path);
            offset = 1.0f / frames;
            vbo = gfx.VertexBufferFactory.CreateQuad(2.0f, 2.0f);
            this.scale = scale;
        }

        public Sprite(string path, int frames)
            : this(path, frames, 1.0f)
        { }

        bool reverseLoop;
        int increment = 1;

        public void SetAnimationParameters(int start, int end, float frameDuration, bool reverseLoop = false)
        {
            startFrame = start;
            endFrame = end;
            frameTime = 0.0f;
            CurrentFrame = startFrame;
            this.frameDuration = frameDuration;
            this.reverseLoop = reverseLoop;
            increment = 1;
        }

        public void Reset()
        {
            CurrentFrame = 0;
            frameTime = 0.0f;
            increment = 1;
        }

        public void Update(float dt)
        {
            frameTime += dt;
            if (frameTime > frameDuration)
            {
                frameTime -= frameDuration;
                CurrentFrame += increment;

                if (!reverseLoop && (CurrentFrame > endFrame))
                    CurrentFrame = startFrame;
                else if (CurrentFrame > endFrame)
                {
                    increment = -1;
                    CurrentFrame = endFrame;
                }

                if (CurrentFrame < startFrame)
                {
                    CurrentFrame = startFrame;
                    increment = 1;
                }

            }

        }        

        public void Draw(gfx.Shader shader)
        {

            Vector2 min_uv = new Vector2(offset * CurrentFrame, 0.0f);
            Vector2 max_uv = new Vector2(offset * CurrentFrame + offset, 1.0f);

            shader.LoadVector2(max_uv, "max_uv");
            shader.LoadVector2(min_uv, "min_uv");

            shader.LoadFloat(scale, "scale");

            texture.Bind(0);
             
            vbo.Bind();
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, vbo.VertexCount);
            vbo.Unbind();

            texture.Unbind(0);               

        }

        public void Dispose()
        {
            texture?.Dispose();
            vbo?.Dispose();
        }
    }
}
