using System;

using OpenTK.Graphics.OpenGL;
using OpenTK;

namespace jam.source.gfx
{

    enum ShaderType
    {
        VertexShader = OpenTK.Graphics.OpenGL.ShaderType.VertexShader,
        FragmentShader = OpenTK.Graphics.OpenGL.ShaderType.FragmentShader,
        GeometryShader = OpenTK.Graphics.OpenGL.ShaderType.GeometryShader
    }

    struct ShaderDescriptor
    {
        public ShaderType Type { get; set; }
        public string SourceCode { get; set; }
    }

    class Shader : IDisposable
    {

        public string Name { get; private set; }

        public Shader(string name, params ShaderDescriptor[] descriptors)
        {

            Name = name;
            int status;

            int[] shaders = new int[descriptors.Length];
            program = GL.CreateProgram();

            for (int index = 0; index < shaders.Length; ++index)
            {
                shaders[index] = GL.CreateShader((OpenTK.Graphics.OpenGL.ShaderType)descriptors[index].Type);
                GL.ShaderSource(shaders[index], descriptors[index].SourceCode);
                GL.CompileShader(shaders[index]);
                GL.AttachShader(program, shaders[index]);
            }

            GL.BindAttribLocation(program, (int)VertexAttributeSemantic.Position, "position");
            GL.BindAttribLocation(program, (int)VertexAttributeSemantic.Color, "color");
            GL.BindAttribLocation(program, (int)VertexAttributeSemantic.TextureCoordinate, "textureCoordinate");
            GL.BindAttribLocation(program, (int)VertexAttributeSemantic.Normal, "normal");

            GL.BindAttribLocation(program, (int)InstanceAttributeSemantic.InstancePosition, "instancePosition");
            GL.BindAttribLocation(program, (int)InstanceAttributeSemantic.InstanceColor, "instanceColor");

            GL.LinkProgram(program);
            GL.GetProgram(program, GetProgramParameterName.LinkStatus, out status);

            if (status != (int)OpenTK.Graphics.OpenGL.Boolean.True)
            {
                string errors = GL.GetProgramInfoLog(program);
                Console.WriteLine($"Program {name} compilation error:{Environment.NewLine}{errors}");
            }
            else {
                GL.ValidateProgram(program);
                GL.GetProgram(program, GetProgramParameterName.ValidateStatus, out status);

                if (status != (int)OpenTK.Graphics.OpenGL.Boolean.True)
                {
                    string errors = GL.GetProgramInfoLog(program);
                    Console.WriteLine($"Program {name} validation error:{Environment.NewLine}{errors}");
                }
            }

            foreach (int shader in shaders)
            {
                GL.DetachShader(program, shader);
                GL.DeleteShader(shader);
            }

        }

        public void SetSamplerUnit(int unit, string samplerName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, samplerName);
            GL.Uniform1(location, unit);
        }

        public void LoadFloat(float value, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.Uniform1(location, value);
        }

        public void LoadVector2(Vector2 vector, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.Uniform2(location, vector);
        }

        public void LoadVector3(Vector3 vector, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.Uniform3(location, vector);
        }

        public void LoadVector4(Vector4 vector, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.Uniform4(location, vector);
        }

        public void LoadMatrix2(Matrix2 matrix, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.UniformMatrix2(location, true, ref matrix);
        }

        public void LoadMatrix3(Matrix3 matrix, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.UniformMatrix3(location, true, ref matrix);
        }

        public void LoadMatrix4(Matrix4 matrix, string variableName)
        {
            Bind();

            int location = GL.GetUniformLocation(program, variableName);
            GL.UniformMatrix4(location, true, ref matrix);
        }

        public void Bind()
        {
            GL.UseProgram(program);
        }

        public void Unbind()
        {
            GL.UseProgram(0);
        }

        public void Dispose()
        {
            GL.DeleteProgram(program);
        }

        int program;

    }
}
