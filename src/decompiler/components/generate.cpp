#include <map>
#include <fstream>
#include <vector>

#include "generate.h"

std::string flagname = "fGotoFix_" + genRandomString(6);

std::map<std::string, std::string> callback_list =
{
        {"OnFilterScriptInit", ""},
        {"OnFilterScriptExit", ""},
        {"OnGameModeInit", ""},
        {"OnGameModeExit", ""},
        {"OnPlayerRequestClass", "playerid, classid"},
        {"OnPlayerConnect", "playerid"},
        {"OnPlayerDisconnect", "playerid, reason"},
        {"OnPlayerSpawn", "playerid"},
        {"OnPlayerDeath", "playerid, killerid, reason"},
        {"OnVehicleSpawn", "vehicleid"},
        {"OnVehicleDeath", "vehicleid, killerid"},
        {"OnPlayerText", "playerid, text[]"},
        {"OnPlayerCommandText", "playerid, cmdtext[]"},
        {"OnPlayerEnterVehicle", "playerid, vehicleid, ispassenger"},
        {"OnPlayerExitVehicle", "playerid, vehicleid"},
        {"OnPlayerStateChange", "playerid, newstate, oldstate"},
        {"OnPlayerEnterCheckpoint", "playerid"},
        {"OnPlayerLeaveCheckpoint", "playerid"},
        {"OnPlayerEnterRaceCheckpoint", "playerid"},
        {"OnPlayerLeaveRaceCheckpoint", "playerid"},
        {"OnRconCommand", "cmd[]"},
        {"OnPlayerRequestSpawn", "playerid"},
        {"OnObjectMoved", "objectid"},
        {"OnPlayerObjectMoved", "playerid, objectid"},
        {"OnPlayerPickUpPickup", "playerid, pickupid"},
        {"OnVehicleMod", "playerid, vehicleid, componentid"},
        {"OnVehiclePaintjob", "playerid, vehicleid, paintjobid"},
        {"OnVehicleRespray", "playerid, vehicleid, color1, color2"},
        {"OnPlayerSelectedMenuRow", "playerid, row"},
        {"OnPlayerExitedMenu", "playerid"},
        {"OnPlayerInteriorChange", "playerid, newinteriorid, oldinteriorid"},
        {"OnPlayerKeyStateChange", "playerid, newkeys, oldkeys"},
        {"OnRconLoginAttempt", "ip[], password[], success"},
        {"OnPlayerUpdate", "playerid"},
        {"OnPlayerStreamIn", "playerid, forplayerid"},
        {"OnPlayerStreamOut", "playerid, forplayerid"},
        {"OnVehicleStreamIn", "vehicleid, forplayerid"},
        {"OnVehicleStreamOut", "vehicleid, forplayerid"},
        {"OnDialogResponse", "playerid, dialogid, response, listitem, inputtext[]"},
        {"OnPlayerClickPlayer", "playerid, clickedplayerid, source"}
};

void opcodeGenerateFunc_1(std::ofstream& fout)
{
    //NOP
}

void opcodeGenerateFunc_2(std::ofstream& fout)
{
    writeCloseBracket(fout);

    if(params.size() == 3)
    {
        if(callback_list.count(params[2]))
        {
            fout << "public " << params[2] << "(" << callback_list.find(params[2])->second << ")" << std::endl;
        }
        else
        {
            fout << "forward " << params[2] << "();" << std::endl
                 << "public " << params[2] << "()" << std::endl;
        }
    }
    else if(params.size() == 2)
    {
        fout << "stock proc_" << params[0] << "()" << std::endl;
    }

    fout << "{" << std::endl;
    proc_checker = true;
}

void opcodeGenerateFunc_3(std::ofstream& fout)
{
    fout << writeTab(1) << "#emit retn" << std::endl;
}

void opcodeGenerateFunc_4(std::ofstream& fout)
{
    fout << writeTab(1) << "#emit " << params[1] << " 0x" << params[2] << std::endl;
}

void opcodeGenerateFunc_5(std::ofstream& fout)
{
    fout << writeTab(1) << "#emit " << params[1] << " 0x" << params[2] << " // " << params[3] << std::endl;
}

void opcodeGenerateFunc_6(std::ofstream& fout)
{
    if(params.size() == 3)
    {
        fout << writeTab(1) << "#emit " << params[1] << " proc_" << params[2] << std::endl;
    }
    else if(params.size() == 4)
    {
        fout << writeTab(1) << "#emit " << params[1] << " " << params[3] << std::endl;
    }
}

void opcodeGenerateFunc_7(std::ofstream& fout)
{
    fout << writeTab(1) << "label_" << params[2] << "_fix:" << std::endl
         << writeTab(1) << "if(" << flagname << ")" << std::endl
         << writeTab(1) << "{" << std::endl
         << writeTab(2) << flagname << " = false;" << std::endl
         << writeTab(2) << "goto label_" << params[2] << ";" << std::endl
         << writeTab(1) << "}" << std::endl
         << writeTab(1) << flagname << " = true;" << std::endl
         << writeTab(1) << "#emit " << params[1] << " label_" << params[2] << "_fix" << std::endl;
}

void opcodeGenerateFunc_8(std::ofstream& fout)
{
    if(!case_checker)
    {
        fout << writeTab(1) << "#emit " << params[1] << std::endl;
    }
    fout << writeTab(1) << "#emit case 0x"  << params[2] << " label_" << params[3] << std::endl;
    case_checker = true;
}

std::map<std::string, void(*)(std::ofstream&)> opcode_generate_list =
{
        {"break", opcodeGenerateFunc_1},
        {"halt", opcodeGenerateFunc_1},
        {"proc", opcodeGenerateFunc_2},
        {"retn", opcodeGenerateFunc_3},
        {"push.adr", opcodeGenerateFunc_4},
        {"push.s", opcodeGenerateFunc_4},
        {"push.c", opcodeGenerateFunc_4},
        {"sysreq.c", opcodeGenerateFunc_5},
        {"call", opcodeGenerateFunc_6},
        {"jump", opcodeGenerateFunc_7},
        {"jzer", opcodeGenerateFunc_7},
        {"jnz", opcodeGenerateFunc_7},
        {"jeq", opcodeGenerateFunc_7},
        {"jneq", opcodeGenerateFunc_7},
        {"jsless", opcodeGenerateFunc_7},
        {"jsleq", opcodeGenerateFunc_7},
        {"jsgrtr", opcodeGenerateFunc_7},
        {"jsgeq", opcodeGenerateFunc_7},
        {"casetbl", opcodeGenerateFunc_8}
};

void generateCode(std::ifstream &fin, std::ofstream &fout)
{
    std::string line, buffer;

    fout << "new " << flagname << " = false;" << std::endl << std::endl
         << "main() {}" << std::endl << std::endl;

    while (!fin.eof())
    {
        getline(fin, line);
        parseLine(line);

        if(labels.count(params[0]))
        {
            fout << writeTab(1) << "label_" << params[0] << ":" << std::endl;
        }

        std::map<std::string, void(*)(std::ofstream&)>::iterator it;
        it = opcode_generate_list.find(params[1]);

        if(it == opcode_generate_list.end())
        {
            for(int i = 1; i < params.size(); i++)
            {
                if(i == 1)
                {
                    fout << writeTab(1) << "#emit";
                }

                if(i != 2)
                {
                    fout << " " << params[i];
                }
                else
                {
                    fout << " 0x" << params[2];
                }
            }
            fout << std::endl;
        }
        else
        {
            if(params[1] != "casetbl")
            {
                case_checker = false;
            }
            it->second(fout);
        }

        params.clear();
    }

    writeCloseBracket(fout);
}