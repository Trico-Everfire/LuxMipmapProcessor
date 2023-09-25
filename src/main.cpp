
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "VTFFile.h"

#define LMP_readParam(key, argsList, arg1, arg2) auto key = hasParameter(argsList, arg1, arg2); key != argsList.end()

#define LMP_log(s) std::cout << s << std::endl;

std::vector<std::string>::const_iterator
hasParameter(const std::vector<std::string> &argsList, const std::string &arg, const std::string &arg2)
{
    return std::find_if(argsList.begin(), argsList.end(), [&arg, &arg2](const std::string& str ){ return str == arg || str == arg2;});
}

int main(int argc, char *argv[]) {

    std::string version = "Version: 1.0.0";
    int frame = 1;
    int face = 1;
    std::string filePath;

    std::vector<std::string> arguments;

    std::unique_ptr<VTFLib::CVTFFile> vtfFile;

    arguments.reserve(argc);

    for(int i = 0; i < argc; i++)
            arguments.emplace_back(argv[i]);

    if(LMP_readParam(key, arguments, "-v", "--version"))
    {
        LMP_log("Insert Help Section Here.");
    }


    if(LMP_readParam(key, arguments, "-h", "--help"))
    {
        LMP_log(version);
    }

    if(LMP_readParam(key, arguments, "-fa", "--face"))
    {
        try {
            face = stoi(*key);
        } catch (...)
        {
            LMP_log("INVALID FACE NUMBER: " << *key);
            return 1;
        }
    }

    if(LMP_readParam(key, arguments, "-fr", "--frame"))
    {
        try {
            frame = stoi(*key);
        } catch (...)
        {
            LMP_log("INVALID FRAME NUMBER: " << *key);
            return 1;
        }
    }

    if(LMP_readParam(key, arguments, "-i", "--input"))
    {
        key++;
        vtfFile = std::make_unique<VTFLib::CVTFFile>();
        filePath = *key;
        if(!vtfFile->Load(key->c_str()))
        {
            LMP_log(std::string("Invalid VTF File:") << *key);
            return 1;
        }
        LMP_log("Loaded file:");
        LMP_log(*key);
    }

    if(LMP_readParam(key, arguments, "-m", "--mipmaps"))
    {
        if(!vtfFile)
        {
            LMP_log("VTF File Not Loaded! \nThe VTF FIle was not loaded.");
            return 1;
        }
        key++;

        int count;
        try {
            count = stoi(*key);
        } catch (...)
        {
            LMP_log("INVALID MIPMAP NUMBER: " << *key);
            return 1;
        }

        int max = vtfFile->ComputeMipmapCount(vtfFile->GetWidth(), vtfFile->GetHeight(), vtfFile->GetDepth());

        if(count > max)
        {
            LMP_log("WARNING: Maximum mipmap level exceeds set mipmap count, discarding excess mipmaps.");
            count = max;
        }

        key++;

        std::string filePath;

        if(!std::filesystem::is_directory(key->c_str()) || !std::filesystem::exists(key->c_str()))
            return 1;

        filePath = *key;

        key++;

        const auto mipMapRegex = std::regex(R"(\$\{it\})");
        const auto mipFaceRegex = std::regex(R"(\$\{fr\})");
        const auto mipFrameRegex = std::regex(R"(\$\{fa\})");

        for(int i = 0; i < count; i++)
        {

            auto imgfileName = std::string();
            imgfileName.append(filePath);
            imgfileName.append("/");
            if((*key).contains("${it}"))
                imgfileName.append(std::regex_replace(*key, mipMapRegex, std::to_string(i)));
            if((*key).contains("${fr}"))
                imgfileName.append(std::regex_replace(*key, mipFaceRegex, std::to_string(face)));
            if((*key).contains("${fa}"))
                imgfileName.append(std::regex_replace(*key, mipFrameRegex, std::to_string(frame)));

            auto fmt = vtfFile->GetFormat();

            int x, y, n;

            if ( !stbi_is_hdr( imgfileName.c_str() ) )
            {
                vlByte *data = stbi_load( imgfileName.c_str(), &x, &y, &n, 4 );

                if ( !data )
                    return 1;

                vlUInt imgSize = VTFLib::CVTFFile::ComputeImageSize(x, y, 1, fmt);

                std::vector<vlByte> newImage(imgSize);
                newImage.reserve(imgSize);

                VTFLib::CVTFFile::Convert( data, newImage.data(),x,y,IMAGE_FORMAT_RGBA8888, fmt);

                if(!vtfFile->SetCustomMipmap(frame, face, 1, i + 1, newImage.data(), x, y, IMAGE_FORMAT_RGBA8888))
                    return 1;

                vtfFile->Save(filePath.c_str());

                stbi_image_free( data );
            }
            else
            {
                float *data = stbi_loadf( imgfileName.c_str(), &x, &y, &n, 0 );

                if ( !data )
                    return 1;

                auto convertedData = reinterpret_cast<vlByte *>( data );

                tagVTFImageFormat format = n > 3 ? IMAGE_FORMAT_RGBA32323232F : IMAGE_FORMAT_RGB323232F;

                if(!vtfFile->SetCustomMipmap(frame, face, 1, i + 1, convertedData, x, y, format))
                    return 1;

                vtfFile->Save(filePath.c_str());

                stbi_image_free( data );
            }

            
        }

    }

    return 0;
}
