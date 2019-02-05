/*
 * ShaderConductor
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <ShaderConductor/ShaderConductor.hpp>

#include <gtest/gtest.h>

#include <cassert>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

using namespace ShaderConductor;

namespace
{
    std::vector<uint8_t> LoadFile(const std::string& name, bool isText)
    {
        std::vector<uint8_t> ret;
        std::ios_base::openmode mode = std::ios_base::in;
        if (!isText)
        {
            mode |= std::ios_base::binary;
        }
        std::ifstream file(name, mode);
        if (file)
        {
            file.seekg(0, std::ios::end);
            ret.resize(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(ret.data()), ret.size());
        }
        return ret;
    }

    void TrimTailingZeros(std::vector<uint8_t>* data)
    {
        assert(data != nullptr);

        while (!data->empty() && (data->back() == '\0'))
        {
            data->pop_back();
        }
    }

    void CompareWithExpected(const std::vector<uint8_t>& actual, bool isText, const std::string& compareName)
    {
        std::vector<uint8_t> expected = LoadFile(TEST_DATA_DIR "Expected/" + compareName, isText);
        if (isText)
        {
            TrimTailingZeros(&expected);
        }

        if (expected != actual)
        {
            if (!actual.empty())
            {
                std::ios_base::openmode mode = std::ios_base::out;
                if (!isText)
                {
                    mode |= std::ios_base::binary;
                }
                std::ofstream actualFile(TEST_DATA_DIR "Result/" + compareName, mode);
                actualFile.write(reinterpret_cast<const char*>(actual.data()), actual.size());
            }

            EXPECT_TRUE(false);
        }
    }

    void HlslToAnyTest(const std::string& name, const Compiler::SourceDesc& source, const Compiler::Options& options,
                       const Compiler::TargetDesc& target, bool expectSuccess)
    {
        static const std::string extMap[] = { "dxil", "spv", "hlsl", "glsl", "essl", "msl" };
        static_assert(sizeof(extMap) / sizeof(extMap[0]) == static_cast<uint32_t>(ShadingLanguage::NumShadingLanguages),
                      "extMap doesn't match with the number of shading languages.");

        const auto result = Compiler::Compile(source, options, target);

        if (expectSuccess)
        {
            EXPECT_FALSE(result.hasError);
            EXPECT_EQ(result.errorWarningMsg, nullptr);
            EXPECT_TRUE(result.isText);

            std::string compareName = name;
            if (target.version != nullptr)
            {
                compareName += "." + std::string(target.version);
            }
            compareName += "." + extMap[static_cast<uint32_t>(target.language)];

            const uint8_t* target_ptr = reinterpret_cast<const uint8_t*>(result.target->Data());
            CompareWithExpected(std::vector<uint8_t>(target_ptr, target_ptr + result.target->Size()), result.isText, compareName);
        }
        else
        {
            EXPECT_TRUE(result.hasError);
            EXPECT_EQ(result.target, nullptr);
        }

        DestroyBlob(result.errorWarningMsg);
        DestroyBlob(result.target);
    }

    class TestBase : public testing::Test
    {
    public:
        void SetUp() override
        {
            for (auto& src : m_combinations)
            {
                const std::string& name = std::get<0>(src);
                Compiler::SourceDesc& source = std::get<1>(src);

                std::get<3>(src) = TEST_DATA_DIR "Input/" + name + ".hlsl";
                source.fileName = std::get<3>(src).c_str();

                std::vector<uint8_t> input = LoadFile(source.fileName, true);
                TrimTailingZeros(&input);
                std::get<4>(src) = std::string(reinterpret_cast<char*>(input.data()), input.size());
                source.source = std::get<4>(src).c_str();
            }
        }

        void RunTests(ShadingLanguage targetSl, const Compiler::Options& options = {})
        {
            for (const auto& combination : m_combinations)
            {
                for (const auto& target : std::get<2>(combination))
                {
                    if (std::get<1>(target).language == targetSl)
                    {
                        HlslToAnyTest(std::get<0>(combination), std::get<1>(combination), options, std::get<1>(target),
                                      std::get<0>(target));
                    }
                }
            }
        }

    protected:
        std::vector<
            std::tuple<std::string, Compiler::SourceDesc, std::vector<std::tuple<bool, Compiler::TargetDesc>>, std::string, std::string>>
            m_combinations;

        // clang-format off
        const std::vector<std::tuple<bool, Compiler::TargetDesc>> m_allTestTargets =
        {
            { true, { ShadingLanguage::Hlsl, "30" } },
            { true, { ShadingLanguage::Hlsl, "40" } },
            { true, { ShadingLanguage::Hlsl, "50" } },

            { true, { ShadingLanguage::Glsl, "300" } },
            { true, { ShadingLanguage::Glsl, "410" } },

            { true, { ShadingLanguage::Essl, "300" } },
            { true, { ShadingLanguage::Essl, "310" } },

            { true, { ShadingLanguage::Msl } },
        };
        // clang-format on
    };

    class VertexShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "Constant_VS",
                    { "", "", "VSMain", ShaderStage::VertexShader },
                    m_allTestTargets,
                    "",
                    ""
                },
                {
                    "PassThrough_VS",
                    { "", "", "VSMain", ShaderStage::VertexShader },
                    m_allTestTargets,
                    "",
                    ""
                },
                {
                    "Transform_VS",
                    { "", "", "", ShaderStage::VertexShader },
                    m_allTestTargets,
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }
    };

    class PixelShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "Constant_PS",
                    { "", "", "PSMain", ShaderStage::PixelShader },
                    m_allTestTargets,
                    "",
                    ""
                },
                {
                    "PassThrough_PS",
                    { "", "", "PSMain", ShaderStage::PixelShader },
                    m_allTestTargets,
                    "",
                    ""
                },
                {
                    "ToneMapping_PS",
                    { "", "", "", ShaderStage::PixelShader },
                    m_allTestTargets,
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }
    };

    class GeometryShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "Particle_GS",
                    { "", "", "", ShaderStage::GeometryShader, defines_.data(), static_cast<uint32_t>(defines_.size()) },
                    {
                        { false, { ShadingLanguage::Hlsl, "30" } }, // No GS in HLSL SM3
                        { false, { ShadingLanguage::Hlsl, "40" } }, // GS not supported yet
                        { false, { ShadingLanguage::Hlsl, "50" } }, // GS not supported yet

                        { true, { ShadingLanguage::Glsl, "300" } },
                        { true, { ShadingLanguage::Glsl, "410" } },

                        { true, { ShadingLanguage::Essl, "300" } },
                        { true, { ShadingLanguage::Essl, "310" } },

                        { false, { ShadingLanguage::Msl } }, // No GS in MSL
                    },
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }

    private:
        std::vector<MacroDefine> defines_ = { { "FIXED_VERTEX_RADIUS", "5.0" } };
    };

    class HullShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "DetailTessellation_HS",
                    { "", "", "", ShaderStage::HullShader },
                    {
                        { false, { ShadingLanguage::Hlsl, "30" } }, // No HS in HLSL SM3
                        { false, { ShadingLanguage::Hlsl, "40" } }, // No HS in HLSL SM4
                        { false, { ShadingLanguage::Hlsl, "50" } }, // HS not supported yet

                        { true, { ShadingLanguage::Glsl, "300" } },
                        { true, { ShadingLanguage::Glsl, "410" } },

                        { true, { ShadingLanguage::Essl, "300" } },
                        { true, { ShadingLanguage::Essl, "310" } },

                        { true, { ShadingLanguage::Msl } },
                    },
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }
    };

    class DomainShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "PNTriangles_DS",
                    { "", "", "", ShaderStage::DomainShader },
                    {
                        { false, { ShadingLanguage::Hlsl, "30" } }, // No HS in HLSL SM3
                        { false, { ShadingLanguage::Hlsl, "40" } }, // No HS in HLSL SM4
                        { false, { ShadingLanguage::Hlsl, "50" } }, // DS not supported yet

                        { true, { ShadingLanguage::Glsl, "300" } },
                        { true, { ShadingLanguage::Glsl, "410" } },

                        { true, { ShadingLanguage::Essl, "300" } },
                        { true, { ShadingLanguage::Essl, "310" } },

                        { true, { ShadingLanguage::Msl } },
                    },
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }
    };

    class ComputeShaderTest : public TestBase
    {
    public:
        void SetUp() override
        {
            // clang-format off
            m_combinations =
            {
                {
                    "Fluid_CS",
                    { "", "", "", ShaderStage::ComputeShader },
                    {
                        { false, { ShadingLanguage::Hlsl, "30" } }, // No CS in HLSL SM3
                        { false, { ShadingLanguage::Hlsl, "40" } }, // CS in HLSL SM4 is not supported
                        { true, { ShadingLanguage::Hlsl, "50" } },

                        { true, { ShadingLanguage::Glsl, "300" } },
                        { true, { ShadingLanguage::Glsl, "410" } },

                        { false, { ShadingLanguage::Essl, "300" } }, // No CS in OpenGL ES 3.0
                        { true, { ShadingLanguage::Essl, "310" } },

                        { true, { ShadingLanguage::Msl } },
                    },
                    "",
                    ""
                },
            };
            // clang-format on

            TestBase::SetUp();
        }
    };


    TEST_F(VertexShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(VertexShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(VertexShaderTest, ToGlslColumnMajor)
    {
        const std::string fileName = TEST_DATA_DIR "Input/Transform_VS.hlsl";

        std::vector<uint8_t> input = LoadFile(fileName, true);
        TrimTailingZeros(&input);
        const std::string source = std::string(reinterpret_cast<char*>(input.data()), input.size());

        Compiler::Options options;
        options.packMatricesInRowMajor = false;

        HlslToAnyTest("Transform_VS_ColumnMajor", { source.c_str(), fileName.c_str(), nullptr, ShaderStage::VertexShader }, options,
                      { ShadingLanguage::Glsl, "300" }, true);
    }

    TEST_F(VertexShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(VertexShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }


    TEST_F(PixelShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(PixelShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(PixelShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(PixelShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }


    TEST_F(GeometryShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(GeometryShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(GeometryShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(GeometryShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }


    TEST_F(HullShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(HullShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(HullShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(HullShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }


    TEST_F(DomainShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(DomainShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(DomainShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(DomainShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }


    TEST_F(ComputeShaderTest, ToHlsl)
    {
        RunTests(ShadingLanguage::Hlsl);
    }

    TEST_F(ComputeShaderTest, ToGlsl)
    {
        RunTests(ShadingLanguage::Glsl);
    }

    TEST_F(ComputeShaderTest, ToEssl)
    {
        RunTests(ShadingLanguage::Essl);
    }

    TEST_F(ComputeShaderTest, ToMsl)
    {
        RunTests(ShadingLanguage::Msl);
    }

    TEST(IncludeTest, IncludeExist)
    {
        const std::string fileName = TEST_DATA_DIR "Input/IncludeExist.hlsl";

        std::vector<uint8_t> input = LoadFile(fileName, true);
        TrimTailingZeros(&input);
        const std::string source = std::string(reinterpret_cast<char*>(input.data()), input.size());

        const auto result =
            Compiler::Compile({ source.c_str(), fileName.c_str(), "main", ShaderStage::PixelShader }, {}, { ShadingLanguage::Glsl, "30" });

        EXPECT_FALSE(result.hasError);
        EXPECT_EQ(result.errorWarningMsg, nullptr);
        EXPECT_TRUE(result.isText);

        const uint8_t* target_ptr = reinterpret_cast<const uint8_t*>(result.target->Data());
        CompareWithExpected(std::vector<uint8_t>(target_ptr, target_ptr + result.target->Size()), result.isText, "IncludeExist.glsl");

        DestroyBlob(result.errorWarningMsg);
        DestroyBlob(result.target);
    }

    TEST(IncludeTest, IncludeNotExist)
    {
        const std::string fileName = TEST_DATA_DIR "Input/IncludeNotExist.hlsl";

        std::vector<uint8_t> input = LoadFile(fileName, true);
        TrimTailingZeros(&input);
        const std::string source = std::string(reinterpret_cast<char*>(input.data()), input.size());

        const auto result =
            Compiler::Compile({ source.c_str(), fileName.c_str(), "main", ShaderStage::PixelShader }, {}, { ShadingLanguage::Glsl, "30" });

        EXPECT_TRUE(result.hasError);
        const char* errorStr = reinterpret_cast<const char*>(result.errorWarningMsg->Data());
        EXPECT_GE(std::string(errorStr, errorStr + result.errorWarningMsg->Size()).find("fatal error: 'Header.hlsli' file not found"), 0);

        DestroyBlob(result.errorWarningMsg);
        DestroyBlob(result.target);
    }

    TEST(IncludeTest, IncludeEmptyFile)
    {
        const std::string fileName = TEST_DATA_DIR "Input/IncludeEmptyHeader.hlsl";

        std::vector<uint8_t> input = LoadFile(fileName, true);
        TrimTailingZeros(&input);
        const std::string source = std::string(reinterpret_cast<char*>(input.data()), input.size());

        const auto result =
            Compiler::Compile({ source.c_str(), fileName.c_str(), "main", ShaderStage::PixelShader }, {}, { ShadingLanguage::Glsl, "30" });

        EXPECT_FALSE(result.hasError);
        EXPECT_EQ(result.errorWarningMsg, nullptr);
        EXPECT_TRUE(result.isText);

        const uint8_t* target_ptr = reinterpret_cast<const uint8_t*>(result.target->Data());
        CompareWithExpected(std::vector<uint8_t>(target_ptr, target_ptr + result.target->Size()), result.isText, "IncludeEmptyHeader.glsl");

        DestroyBlob(result.errorWarningMsg);
        DestroyBlob(result.target);
    }
} // namespace

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    int retVal = RUN_ALL_TESTS();
    if (retVal != 0)
    {
        getchar();
    }

    return retVal;
}
