#include "config.h"
#include "logger.hpp"
#include "FunctionFwd.hpp"
#include "SDL_main.h"
#include "Io.hpp"
#include "Sys.hpp"
#include "../AliveLibAE/Sound/PsxSpuApi.hpp"
#include "../AliveLibAE/LvlArchive.hpp"

#define FLUIDSYNTH_NOT_A_DLL

#include "fluidsynth.h"
#include <string>

bool CC RunningAsInjectedDll()
{
    return false;
}

static std::vector<u8> ReadLvlFile(LvlArchive& archive, const char_type* fileName)
{
    LvlFileRecord* pFileRec = archive.Find_File_Record_433160(fileName);
    if (!pFileRec)
    {
        ALIVE_FATAL("Failed find file in lvl");
    }
    std::vector<u8> fileContent;
    fileContent.resize(pFileRec->field_10_num_sectors * 2048);
    sLvlArchive_5BC520.Read_File_4330A0(pFileRec, fileContent.data());
    fileContent.resize(pFileRec->field_14_file_size);
    return fileContent;
}

static void WriteVec(const char_type* fileName, const std::vector<u8>& vec)
{
    auto hFile = fopen(fileName, "wb");
    if (hFile)
    {
        fwrite(vec.data(), 1, vec.size(), hFile);
        fclose(hFile);
    }
}

s32 CC SND_SoundsDat_Get_Sample_Len_4FC400(VabHeader* pVabHeader, VabBodyRecord* pVabBody, s32 idx);
BOOL CC sub_4FC470(VabHeader* pVabHeader, VabBodyRecord* pVabBody, s32 idx);
u32* CC SND_SoundsDat_Get_Sample_Offset_4FC3D0(VabHeader* pVabHeader, VabBodyRecord* pBodyRecords, s32 idx);

s32 CC SND_SoundsDat_Read(FILE* file, VabHeader* pVabHeader, VabBodyRecord* pVabBody, s32 idx, void* pBuffer)
{
    const s32 sampleOffset = *SND_SoundsDat_Get_Sample_Offset_4FC3D0(pVabHeader, pVabBody, idx); // = field_8_fileOffset
    const s32 sampleLen = SND_SoundsDat_Get_Sample_Len_4FC400(pVabHeader, pVabBody, idx);
    if (sampleOffset == -1)
    {
        return 0;
    }

    fseek(file, sampleOffset, 0);
    fread(pBuffer, 1, sampleLen * 2, file);

    return sampleLen;
}


void PCToPsxVab(bool isAe, const char_type* lvlName, const char_type* vhName, const char_type* vbName)
{
    ResourceManager::Init_49BCE0();

    LvlArchive archive;

    if (!archive.Open_Archive_432E80(lvlName))
    {
        ALIVE_FATAL("Failed to open lvl");
    }

    auto vhData = ReadLvlFile(archive, vhName);
    auto pHeader = reinterpret_cast<VabHeader*>(vhData.data());
    WriteVec(vhName, vhData);

    auto vbData = ReadLvlFile(archive, vbName);
    auto pBody = reinterpret_cast<VabBodyRecord*>(vbData.data());

    s32 vagCount = pHeader->field_16_num_vags;
    if (vagCount > 255)
    {
        vagCount = 255;
    }

    if (isAe)
    {
        auto hDat = fopen("sounds.dat", "rb");
        if (!hDat)
        {
            ALIVE_FATAL("Failed to open sounds.dat");
        }

        for (s32 i = 0; i < vagCount; i++)
        {
            s32 sampleLen = SND_SoundsDat_Get_Sample_Len_4FC400(pHeader, pBody, i);
            if (sampleLen < 4000 && !sub_4FC470(pHeader, pBody, i))
            {
                sampleLen *= 2;
            }
            sampleLen = pBody->field_0_length_or_duration;

            std::vector<u8> tempBuffer;
            tempBuffer.resize(sampleLen);
            // SND_SoundsDat_Read(hDat, pHeader, pBody, i, tempBuffer.data());

            fseek(hDat, pBody->field_8_fileOffset, SEEK_SET);
            fread(tempBuffer.data(), 1, sampleLen, hDat);

            const std::string name = std::string(vbName) + "_" + std::to_string(i) + ".raw";
            WriteVec(name.c_str(), tempBuffer);
        }
        fclose(hDat);
    }

    archive.Free_433130();
}

void play_midi()
{
    fluid_settings_t* settings = new_fluid_settings();

    fluid_synth_t* synth = new_fluid_synth(settings);

    //fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, synth);

    const int sfont_id = fluid_synth_sfload(synth, "C:\\Users\\paul\\Downloads\\Abe2MidiPlayer\\oddworld.sf2", 1);
    if (sfont_id == FLUID_FAILED)
    {
        LOG_ERROR("Failed to load sound font");
    }

    fluid_player_t* player1 = new_fluid_player(synth);
    if (fluid_player_add(player1, "C:\\Users\\paul\\Downloads\\Abe2MidiPlayer\\midi\\PS1 SEQ.mid") != FLUID_OK)
    {
        LOG_ERROR("Failed to open midi");
    }
    fluid_player_play(player1);

    fluid_player_t* player2 = new_fluid_player(synth);
    if (fluid_player_add(player2, "C:\\Users\\paul\\Downloads\\Abe2MidiPlayer\\midi\\19PS1 SEQ.mid") != FLUID_OK)
    {
        LOG_ERROR("Failed to open midi");
    }
    fluid_player_play(player2);

    FILE* outFile = fopen("output.bin", "wb");
    if (!outFile)
    {
        LOG_ERROR("Failed to open out file");
    }
    else
    {
        bool done = false;
        do
        {
            const int lenBytes = 1024;
            short stream[lenBytes / 2] = {};
            fluid_synth_write_s16(synth, lenBytes / (2 * sizeof(short)), stream, 0, 2, stream, 1, 2);
            fwrite(stream, 2, lenBytes / 2, outFile);
            done = fluid_player_get_status(player1) == FLUID_PLAYER_DONE;
            done |= fluid_player_get_status(player2) == FLUID_PLAYER_DONE;
        } while (!done);
        fclose(outFile);
    }

    fluid_player_join(player1);
    fluid_player_join(player2);



    //delete_fluid_audio_driver(adriver);
    delete_fluid_player(player1);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

s32 main(s32 /*argc*/, char_type** /*argv*/)
{
#if _WIN32
    ::AllocConsole();
    ::freopen("CONOUT$", "w", stdout);
    ::SetConsoleTitleA("Debug Console");
    ::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    RedirectIoStream(true);
#endif

    LOG_INFO("fluid synth version: " << fluid_version_str());
    play_midi();

    //PCToPsxVab(true, "ST.LVL", "MONK.VH", "MONK.VB");

    return 0;
}
