using System;
using System.Drawing;
using System.Drawing.Imaging;

using OpenTK.Graphics.OpenGL;

namespace jam.source.gfx
{

    enum TextureWrapMode
    {
        Repeat = OpenTK.Graphics.OpenGL.TextureWrapMode.Repeat,
        MirroredRepeat = OpenTK.Graphics.OpenGL.TextureWrapMode.MirroredRepeat,
        ClampToEdge = OpenTK.Graphics.OpenGL.TextureWrapMode.ClampToEdge,
        ClampToBorder = OpenTK.Graphics.OpenGL.TextureWrapMode.ClampToBorder
    }

    enum TextureFilterMode
    {
        Nearest,
        Linear,
        NearestMipmap,
        LinearMipmap
    }

    class Texture2D : IDisposable
    {

        public Texture2D(string path, TextureWrapMode wrapMode, TextureFilterMode filterMode)
        {

            texture = GL.GenTexture();

            Bitmap bitmap = new Bitmap(path);
            Rectangle rectangle = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            BitmapData bitmap_data = bitmap.LockBits(rectangle, ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            TextureMagFilter magFilter;
            TextureMinFilter minFilter;
            switch (filterMode)
            {
                case TextureFilterMode.Nearest:
                    magFilter = TextureMagFilter.Nearest;
                    minFilter = TextureMinFilter.Nearest;
                    break;
                case TextureFilterMode.Linear:
                    magFilter = TextureMagFilter.Linear;
                    minFilter = TextureMinFilter.Linear;
                    break;
                case TextureFilterMode.NearestMipmap:
                    magFilter = TextureMagFilter.Nearest;
                    minFilter = TextureMinFilter.NearestMipmapNearest;
                    break;
                case TextureFilterMode.LinearMipmap:
                    magFilter = TextureMagFilter.Linear;
                    minFilter = TextureMinFilter.LinearMipmapLinear;
                    break;
                default:
                    magFilter = TextureMagFilter.Nearest;
                    minFilter = TextureMinFilter.Nearest;
                    break;
            }

            Bind(0);

            GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, bitmap.Width, bitmap.Height, 0, OpenTK.Graphics.OpenGL.PixelFormat.Bgra, PixelType.UnsignedByte, bitmap_data.Scan0);

            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)minFilter);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)magFilter);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)wrapMode);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)wrapMode);

            if (filterMode == TextureFilterMode.NearestMipmap ||
                filterMode == TextureFilterMode.LinearMipmap)
            {
                GL.GenerateMipmap(GenerateMipmapTarget.Texture2D);
            }

            Unbind(0);

            bitmap.UnlockBits(bitmap_data);

        }

        public Texture2D(string path, TextureFilterMode filterMode) :
            this(path, TextureWrapMode.ClampToBorder, filterMode)
        { }

        public Texture2D(string path) :
            this(path, TextureWrapMode.ClampToBorder, TextureFilterMode.Nearest)
        { }

        public void Bind(int unit)
        {
            GL.ActiveTexture(TextureUnit.Texture0 + unit);
            GL.BindTexture(TextureTarget.Texture2D, texture);
        }

        public void Unbind(int unit)
        {
            GL.ActiveTexture(TextureUnit.Texture0 + unit);
            GL.BindTexture(TextureTarget.Texture2D, 0);
        }

        public void Dispose()
        {
            GL.DeleteTexture(texture);
        }

        int texture;

    }

}
