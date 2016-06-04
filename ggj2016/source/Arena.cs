using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace jam.source
{

    class TrapDescriptor
    {
        public Vector2 position;
        public Sprite sprite;
        public bool active;
        public float minTTL;
    }

    class Arena : IDisposable
    {

        gfx.VertexBuffer vbo;
        gfx.Texture2D mapTexture;
        gfx.Texture2D glowmapTexture;

        const int MaxActiveTraps = 15;
        public List<TrapDescriptor> TrapList { get; private set; }

        Random r;

        private bool CanPlaceTrap(Vector2 where)
        {
            if (where.Length > 13.0f) return false;
            foreach (TrapDescriptor trap in TrapList)
            {
                if (trap.active && ((trap.position - where).Length < 4.0f))
                    return false;
            }
            return true;
        }

        public Arena(float radius)
        {
            vbo = gfx.VertexBufferFactory.CreateCircle(radius);
            mapTexture = new gfx.Texture2D($"{ContentDirectory}floor.png", gfx.TextureWrapMode.Repeat, gfx.TextureFilterMode.NearestMipmap);
            glowmapTexture = new gfx.Texture2D($"{ContentDirectory}glowmap.png", gfx.TextureWrapMode.Repeat, gfx.TextureFilterMode.NearestMipmap);
            TrapList = new List<TrapDescriptor>(MaxActiveTraps);
            r = new Random();

            for (int i = 0; i < MaxActiveTraps; ++i)
            {
                TrapDescriptor trap = new TrapDescriptor();
                trap.active = false;
                trap.sprite = new Sprite($"{ContentDirectory}spike.png", 5, 1.5f);
                trap.sprite.SetAnimationParameters(0, 4, 1.0f, true);
                TrapList.Add(trap);
            }

        }

        public void Update(float dt)
        {
            foreach (TrapDescriptor trap in TrapList)
            {
                if (!trap.active)
                {
                    if ((r.NextDouble() < 0.005f))
                    {
                        trap.position = new Vector2(((float)r.NextDouble() - 0.5f) * 20.0f, ((float)r.NextDouble() - 0.5f) * 25.0f);
                        if (CanPlaceTrap(trap.position))
                        {
                            trap.active = true;
                            trap.sprite.Reset();
                            trap.minTTL = 1.5f;
                        }
                    }
                }
                else
                {
                    trap.minTTL -= dt;
                    trap.sprite.Update(dt);
                    if (((trap.minTTL < 0.0f)) && (r.NextDouble() < 0.005f) && (trap.sprite.CurrentFrame == 0))
                    {
                        trap.active = false;
                    }
                }
            }
        }

        public void DrawMap(gfx.Shader shader)
        {
            shader.Bind();

            vbo.Bind();

            mapTexture.Bind(0);
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, vbo.VertexCount);
            mapTexture.Unbind(0);

            GL.BlendFunc(BlendingFactorSrc.One, BlendingFactorDest.One);

            glowmapTexture.Bind(0);
            GL.DrawArrays(PrimitiveType.TriangleFan, 0, vbo.VertexCount);
            glowmapTexture.Unbind(0);

            GL.BlendFunc(BlendingFactorSrc.SrcAlpha, BlendingFactorDest.OneMinusSrcAlpha);

            vbo.Unbind();
            shader.Unbind();
        }

        public void DrawTraps(gfx.Shader shader)
        {
            shader.Bind();

            foreach (TrapDescriptor trap in TrapList)
            {
                if (trap.active)
                {
                    shader.LoadVector2(trap.position, "translation");
                    shader.LoadFloat((float)Math.PI, "rotation");
                    trap.sprite.Draw(shader);
                }
            }

            shader.Unbind();
        }

        public void Dispose()
        {
            vbo?.Dispose();
            mapTexture?.Dispose();
            glowmapTexture?.Dispose();
        }

        private static string ContentDirectory = "content/";

    }

}
