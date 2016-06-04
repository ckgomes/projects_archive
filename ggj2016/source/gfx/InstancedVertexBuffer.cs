using System;
using System.Collections.Generic;

using OpenTK.Graphics.OpenGL;

namespace jam.source.gfx
{

    enum InstanceAttributeSemantic
    {
        InstancePosition = VertexAttributeSemantic.VertexAttributesCount,
        InstanceColor,
        InstancedTextureCoordinate
    }

    struct InstanceAttribute
    {
        public InstanceAttributeSemantic Semantic { get; set; }
        public VertexElementType Format { get; set; }
        public int Offset { get; set; }
    }

    class InstancedVertexBuffer : VertexBuffer
    {

        public int InstancesCount { get; private set; }

        public InstancedVertexBuffer(List<VertexAttribute> vertexAttributes, List<InstanceAttribute> instanceAttributes)
            : base(vertexAttributes.ToArray())
        {

            Bind();
            
            instancesBuffer = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.ArrayBuffer, instancesBuffer);

            instancesStride = 0;

            foreach (var attribute in instanceAttributes)
                instancesStride += (int)attribute.Format * sizeof(float);

            foreach (var attribute in instanceAttributes)
            {
                GL.EnableVertexAttribArray((int)attribute.Semantic);
                GL.VertexAttribPointer((int)attribute.Semantic, (int)attribute.Format, VertexAttribPointerType.Float, false, instancesStride, attribute.Offset);
                GL.VertexAttribDivisor((int)attribute.Semantic, 1);
            }

            Unbind();
            
        }

        public void SetInstanceData(float[] data)
        {
            GL.BindBuffer(BufferTarget.ArrayBuffer, instancesBuffer);
            InstancesCount = (data.Length * sizeof(float)) / instancesStride;
            GL.BufferData(BufferTarget.ArrayBuffer, new IntPtr(data.Length * sizeof(float)), data, BufferUsageHint.DynamicDraw);
        }

        public new void Dispose()
        {
            GL.DeleteBuffer(instancesBuffer);
            base.Dispose();
        }

        int instancesStride;

        int instancesBuffer;

    }

}
