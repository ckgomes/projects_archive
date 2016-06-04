using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace jam.source.gfx
{
    static class VertexBufferFactory
    {

        private static VertexBuffer InitializeVertexBuffer()
        {

            VertexAttribute positions = new VertexAttribute();
            positions.Format = VertexElementType.Vector2;
            positions.Semantic = VertexAttributeSemantic.Position;
            positions.Offset = 0;

            return new VertexBuffer(positions);
        }

        public static VertexBuffer CreateCircle(float radius)
        {

            const int sides = 100;

            int floatsPerVertex = 2;
            float[] circleData = new float[floatsPerVertex * (sides + 1)];

            VertexBuffer vbo = InitializeVertexBuffer();

            float theta = 0.0f;
            float thetaStep = 2.0f * (float)Math.PI / (sides - 1);

            circleData[0] = 0.0f;
            circleData[1] = 0.0f;

            for (int index = floatsPerVertex; index < circleData.Length; index += floatsPerVertex, theta += thetaStep)
            {

                float cosine = (float)Math.Cos(theta);
                float sine = (float)Math.Sin(theta);

                circleData[index] = cosine * radius;
                circleData[index + 1] = sine * radius;

            }

            vbo.SetVertexData(circleData);

            return vbo;

        }

        public static VertexBuffer CreateQuad(float width, float height)
        {

            float[] quadData =
            {
                -0.5f * width,  0.5f * height,
                 0.5f * width,  0.5f * height,
                 0.5f * width, -0.5f * height,
                -0.5f * width, -0.5f * height
            };

            VertexBuffer vbo = InitializeVertexBuffer();

            vbo.SetVertexData(quadData);

            return vbo;

        }

        public static InstancedVertexBuffer CreateInstancedCircle(float radius)
        {

            List<VertexAttribute> vertexAttributes = new List<VertexAttribute>(1);
            List<InstanceAttribute> instanceAttributes = new List<InstanceAttribute>(2);

            VertexAttribute positionVertex = new VertexAttribute();
            positionVertex.Semantic = VertexAttributeSemantic.Position;
            positionVertex.Format = VertexElementType.Vector2;
            positionVertex.Offset = 0;
            vertexAttributes.Add(positionVertex);

            InstanceAttribute positionInstance = new InstanceAttribute();
            positionInstance.Semantic = InstanceAttributeSemantic.InstancePosition;
            positionInstance.Format = VertexElementType.Vector2;
            positionInstance.Offset = 0;
            instanceAttributes.Add(positionInstance);

            InstanceAttribute colorInstance = new InstanceAttribute();
            colorInstance.Semantic = InstanceAttributeSemantic.InstanceColor;
            colorInstance.Format = VertexElementType.Vector4;
            colorInstance.Offset = 8;
            instanceAttributes.Add(colorInstance);

            InstancedVertexBuffer ivbo = new InstancedVertexBuffer(vertexAttributes, instanceAttributes);

            const int sides = 100;

            int floatsPerVertex = 2;
            float[] circleData = new float[floatsPerVertex * (sides + 1)];

            float theta = 0.0f;
            float thetaStep = 2.0f * (float)Math.PI / (sides - 1);

            circleData[0] = 0.0f;
            circleData[1] = 0.0f;

            for (int index = floatsPerVertex; index < circleData.Length; index += floatsPerVertex, theta += thetaStep)
            {
                circleData[index] = (float)Math.Cos(theta) * radius;
                circleData[index + 1] = (float)Math.Sin(theta) * radius;
            }

            ivbo.SetVertexData(circleData);

            return ivbo;

        }

    }
}
