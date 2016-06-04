using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Configuration;

namespace jam.source.gfx
{
    static class ShaderFactory
    {

        public static Shader CreateBackgroundShader()
        {
            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}background.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}background.fs");

            Shader shader = new Shader("Texture", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateTextureShader()
        {

            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}texture.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}texture.fs");

            Shader shader = new Shader("Texture", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateFloorShader()
        {
            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}floor.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}floor.fs");

            Shader shader = new Shader("Floor", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateFinalpassShader()
        {

            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}finalpass.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}finalpass.fs");

            Shader shader = new Shader("Finalpass", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateLightpassShader()
        {

            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}lightpass.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}lightpass.fs");

            Shader shader = new Shader("Lightpass", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateUserInterfaceShader()
        {

            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}interface.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}texture.fs");

            Shader shader = new Shader("User Interface", vertexShader, fragmentShader);
            return shader;
        }

        public static Shader CreateBloodyScreenShader()
        {

            ShaderDescriptor vertexShader = new ShaderDescriptor();
            vertexShader.Type = ShaderType.VertexShader;
            vertexShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}interface.vs");

            ShaderDescriptor fragmentShader = new ShaderDescriptor();
            fragmentShader.Type = ShaderType.FragmentShader;
            fragmentShader.SourceCode = System.IO.File.ReadAllText($"{ContentDirectory}bloodyscreen.fs");

            Shader shader = new Shader("Bloody Screen", vertexShader, fragmentShader);
            return shader;
        }

        static private string ContentDirectory = ConfigurationManager.AppSettings["ContentDirectory"];

    }
}
