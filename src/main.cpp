
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include <filesystem>
#include <regex>
#include "VTFFile.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define readParam(key, argsList, arg1, arg2) auto key = hasParameter(argsList, arg1, arg2); key != argsList.end()

#define log(s) std::cout << s << std::endl;

__gnu_cxx::__normal_iterator<const std::basic_string<char> *, std::vector<std::basic_string<char>>>
hasParameter(const std::vector<std::string> &argsList, const std::string &arg, const std::string &arg2)
{
    return std::find_if(argsList.begin(), argsList.end(), [&arg, &arg2](const std::string& str ){ return str == arg || str == arg2;});
}

int main(int argc, char *argv[]) {

    std::string version = "Version: 1.0.0";
    int frame = 1;
    int face = 1;

    std::vector<std::string> arguments;

    std::unique_ptr<VTFLib::CVTFFile> vtfFile;

    arguments.reserve(argc);

    for(int i = 0; i < argc; i++)
            arguments.emplace_back(argv[i]);

    if(readParam(key, arguments, "-v", "--version"))
    {
        log("Insert Help Section Here.");
    }


    if(readParam(key, arguments, "-h", "--help"))
    {
        log(version);
    }

    if(readParam(key, arguments, "-fa", "--face"))
    {
        try {
            face = stoi(*key.base());
        } catch (...)
        {
            log("INVALID FACE NUMBER: " << *key.base());
            return 1;
        }
    }

    if(readParam(key, arguments, "-fr", "--frame"))
    {
        try {
            frame = stoi(*key.base());
        } catch (...)
        {
            log("INVALID FRAME NUMBER: " << *key.base());
            return 1;
        }
    }

    if(readParam(key, arguments, "-i", "--input"))
    {
        key++;
        vtfFile = std::make_unique<VTFLib::CVTFFile>();
        if(!vtfFile->Load(key->c_str()))
        {
            log(std::string("Invalid VTF File:") << *key.base());
            return 1;
        }
        log("Loaded file:");
        log(*key.base());
    }

    if(readParam(key, arguments, "-m", "--mipmaps"))
    {
        if(!vtfFile)
        {
            log("VTF File Not Loaded! \nThe VTF FIle was not loaded.");
            return 1;
        }
        key++;

        int count;
        try {
            count = stoi(*key.base());
        } catch (...)
        {
            log("INVALID MIPMAP NUMBER: " << *key.base());
            return 1;
        }

        int max = vtfFile->ComputeMipmapCount(vtfFile->GetWidth(), vtfFile->GetHeight(), vtfFile->GetDepth());

        if(count > max)
        {
            log("WARNING: Maximum mipmap level exceeds set mipmap count, discarding excess mipmaps.");
            count = max;
        }

        key++;

        std::string filePath;

        if(!std::filesystem::is_directory(key->c_str()) || !std::filesystem::exists(key->c_str()))
            return 1;

        filePath = *key.base();

        key++;

        const auto mipMapRegex = std::regex(R"(\$\{it\})");
        const auto mipFaceRegex = std::regex(R"(\$\{fr\})");
        const auto mipFrameRegex = std::regex(R"(\$\{fa\})");

        for(int i = 0; i < count; i++)
        {

            auto fileName = std::string();
            fileName.append(filePath);
            fileName.append("/");
            fileName.append(std::regex_replace(*key.base(), mipMapRegex, std::to_string(i)));
            fileName.append(std::regex_replace(*key.base(), mipFaceRegex, std::to_string(face)));
            fileName.append(std::regex_replace(*key.base(), mipFrameRegex, std::to_string(frame)));

            auto fmt = vtfFile->GetFormat();

            int x, y, n;

            if ( !stbi_is_hdr( fileName.c_str() ) )
            {
                vlByte *data = stbi_load( fileName.c_str(), &x, &y, &n, 4 );

                if ( !data )
                    return 1;

                vlUInt imgSize = VTFLib::CVTFFile::ComputeImageSize(x, y, 1, fmt);

                std::vector<vlByte> newImage(imgSize);
                newImage.reserve(imgSize);

                VTFLib::CVTFFile::Convert( data, newImage.data(),x,y,IMAGE_FORMAT_RGBA8888, fmt);

                if(!vtfFile->SetCustomMipmap(frame, face, 1, i + 1, newImage.data(), x, y, IMAGE_FORMAT_RGBA8888))
                    return 1;

                vtfFile->Save("/home/trico/Documents/SDX/GithubProjects/LuxMipmapProcessor/lobby_woodwall001a_trim_res.vtf");

                stbi_image_free( data );
            }
            else
            {
                float *data = stbi_loadf( fileName.c_str(), &x, &y, &n, 0 );

                if ( !data )
                    return 1;

                auto convertedData = reinterpret_cast<vlByte *>( data );

                tagVTFImageFormat format = n > 3 ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGB323232F;

                if(!vtfFile->SetCustomMipmap(frame, face, 1, i + 1, convertedData, x, y, format))
                    return 1;

                vtfFile->Save("/home/trico/Documents/SDX/GithubProjects/LuxMipmapProcessor/lobby_woodwall001a_trim_res.vtf");

                stbi_image_free( data );
            }

            
        }

    }

    return 0;
}
