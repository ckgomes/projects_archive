using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using OpenTK.Graphics.OpenGL;

namespace jam.source.gfx
{
    class RenderTarget : IDisposable
    {

        private RenderTarget()
        {
           framebuffer = 0;
           texture = 0;
        }

        public RenderTarget(int width, int height)
        {

            texture = GL.GenTexture();

            GL.ActiveTexture(TextureUnit.Texture0);
            GL.BindTexture(TextureTarget.Texture2D, texture);
            GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, width, height, 0, PixelFormat.Rgba, PixelType.Byte, IntPtr.Zero);

            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Nearest);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Nearest);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.ClampToBorder);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.ClampToBorder);
             
            framebuffer = GL.GenFramebuffer();

            Bind();

            depthStencilRenderbuffer = GL.GenRenderbuffer();
            GL.BindRenderbuffer(RenderbufferTarget.Renderbuffer, depthStencilRenderbuffer);
            GL.RenderbufferStorage(RenderbufferTarget.Renderbuffer, RenderbufferStorage.Depth24Stencil8, width, height);
            GL.FramebufferRenderbuffer(FramebufferTarget.Framebuffer, FramebufferAttachment.DepthStencilAttachment, RenderbufferTarget.Renderbuffer, depthStencilRenderbuffer);

            GL.FramebufferTexture(FramebufferTarget.Framebuffer, FramebufferAttachment.ColorAttachment0, texture, 0);
            GL.DrawBuffer(DrawBufferMode.ColorAttachment0);

            GL.CheckFramebufferStatus(FramebufferTarget.Framebuffer);

            Unbind();
            
        }

        public void Dispose()
        {
            GL.DeleteRenderbuffer(depthStencilRenderbuffer);
            GL.DeleteFramebuffer(framebuffer);
            GL.DeleteTexture(texture);
        }

        public void Clear()
        {
            Bind();
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.StencilBufferBit);
        }

        public void Bind()
        {
            GL.BindFramebuffer(FramebufferTarget.Framebuffer, framebuffer);
        }

        public void Unbind()
        {
            GL.BindFramebuffer(FramebufferTarget.Framebuffer, 0);
        }

        public void BindToTexture(int unit)
        {
            GL.ActiveTexture(TextureUnit.Texture0 + unit);
            GL.BindTexture(TextureTarget.Texture2D, texture);
        }

        public static RenderTarget Backbuffer
        {
            get
            {
                RenderTarget backbuffer = new RenderTarget();
                return backbuffer;
            }
        }

        int framebuffer;
        int texture;
        int depthStencilRenderbuffer;

    }
}
