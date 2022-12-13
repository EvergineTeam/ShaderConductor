using System.Runtime.InteropServices;

namespace ShaderConductorTest
{
    class Program
    {
        static void Main(string[] args)
        {
            string source = File.ReadAllText("shader.hlsl");

            ShaderConductorWrapper.SourceDesc sourceDesc = new ShaderConductorWrapper.SourceDesc()
            {
                entryPoint = "PS",

                stage = ShaderConductorWrapper.ShaderStage.PixelShader,
                source = source,
            };

            ShaderConductorWrapper.OptionsDesc optionsDesc = ShaderConductorWrapper.OptionsDesc.Default;
            //optionsDesc.packMatricesInRowMajor = true;
            //optionsDesc.enable16bitTypes = true;
            //optionsDesc.enableDebugInfo = true;
            //optionsDesc.disableOptimizations = true;
            //optionsDesc.optimizationLevel = 3;
            //optionsDesc.shaderModel = new ShaderConductorWrapper.ShaderModel(6, 2);
            //optionsDesc.shiftAllCBuffersBindings = 10;
            //optionsDesc.shiftAllTexturesBindings = 20;
            //optionsDesc.shiftAllSamplersBindings = 30;
            //optionsDesc.shiftAllUABuffersBindings = 40;

            ShaderConductorWrapper.TargetDesc targetDesc = new ShaderConductorWrapper.TargetDesc()
            {
                language = ShaderConductorWrapper.ShadingLanguage.Glsl,
                version = "460",
            };

            ShaderConductorWrapper.Compile(ref sourceDesc, ref optionsDesc, ref targetDesc, out ShaderConductorWrapper.ResultDesc result);

            if (result.hasError)
            {
                string warning = Marshal.PtrToStringAnsi(ShaderConductorWrapper.GetShaderConductorBlobData(result.errorWarningMsg),
                    ShaderConductorWrapper.GetShaderConductorBlobSize(result.errorWarningMsg));
                Console.WriteLine("*************************\n" +
                                  "**  Error output       **\n" +
                                  "*************************\n");
                Console.WriteLine(warning);
            }
            else
            {
                if (!result.isText)
                {
                    ShaderConductorWrapper.DisassembleDesc disassembleDesc = new ShaderConductorWrapper.DisassembleDesc()
                    {
                        language = targetDesc.language,
                        binarySize = ShaderConductorWrapper.GetShaderConductorBlobSize(result.target),
                        binary = ShaderConductorWrapper.GetShaderConductorBlobData(result.target),
                    };

                    ShaderConductorWrapper.Disassemble(ref disassembleDesc, out ShaderConductorWrapper.ResultDesc disassembleResult);
                    ShaderConductorWrapper.DestroyShaderConductorBlob(result.target);
                    ShaderConductorWrapper.DestroyShaderConductorBlob(result.errorWarningMsg);
                    result = disassembleResult;
                }

                if (result.isText)
                {
                    string translation = Marshal.PtrToStringAnsi(ShaderConductorWrapper.GetShaderConductorBlobData(result.target),
                        ShaderConductorWrapper.GetShaderConductorBlobSize(result.target));
                    Console.WriteLine("*************************\n" +
                                      "**  Translation        **\n" +
                                      "*************************\n");
                    Console.WriteLine(translation);
                }
            }

            ShaderConductorWrapper.DestroyShaderConductorBlob(result.target);
            ShaderConductorWrapper.DestroyShaderConductorBlob(result.errorWarningMsg);

            Console.Read();
        }
    }
}
