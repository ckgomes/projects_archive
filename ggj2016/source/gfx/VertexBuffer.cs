using System;

using OpenTK.Graphics.OpenGL;

namespace jam.source.gfx
{

    enum VertexAttributeSemantic
    {
        Position,
        Color,
        TextureCoordinate,
        Normal,
        Tangent,
        Bitangent,

        VertexAttributesCount
    }

    enum VertexElementType
    {
        Float = 1,
        Vector2,
        Vector3,
        Vector4       
    }

    struct VertexAttribute
    {

        public VertexAttributeSemantic Semantic { get; set; }
        public VertexElementType Format { get; set; }
        public int Offset { get; set; }

    }

    class VertexBuffer : IDisposable
    {

        public int VertexCount { get; private set; }

        public VertexBuffer(params VertexAttribute[] vertexAttributes)
        {

            vao = GL.GenVertexArray();

            Bind();

            attributeBuffer = GL.GenBuffer();

            GL.BindBuffer(BufferTarget.ArrayBuffer, attributeBuffer);

            vertexStride = 0;

            foreach (var attribute in vertexAttributes)
                vertexStride += (int)attribute.Format * sizeof(float);

            foreach (var attribute in vertexAttributes)
            {
                GL.EnableVertexAttribArray((int)attribute.Semantic);
                GL.VertexAttribPointer((int)attribute.Semantic, (int)attribute.Format, VertexAttribPointerType.Float, false, vertexStride, attribute.Offset);
            }

            Unbind();

        }

        public void SetVertexData(float[] data)
        {
            GL.BindBuffer(BufferTarget.ArrayBuffer, attributeBuffer);
            VertexCount = (data.Length * sizeof(float)) / vertexStride;
            GL.BufferData(BufferTarget.ArrayBuffer, new IntPtr(data.Length * sizeof(float)), data, BufferUsageHint.StaticDraw);
        }

        public void Bind()
        {
            GL.BindVertexArray(vao);
        }

        public void Unbind()
        {
            GL.BindVertexArray(0);
        }

        public void Dispose()
        {
            GL.DeleteBuffer(attributeBuffer);
            GL.DeleteVertexArray(vao);
        }

        int vertexStride;

        int vao;
        int attributeBuffer;

    }

}
