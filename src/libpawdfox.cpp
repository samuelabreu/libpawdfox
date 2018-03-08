#include "libpawdfox.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <rapidjson/document.h>
#include "rapidjson/filereadstream.h"
#include <nss/nss.h>
#include <nss/pk11sdr.h>
#include <nss/pk11pub.h>
#include <nss/seccomon.h>
#include <nss/nssb64.h>

using namespace std;
using namespace libpawdfox;

#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

PK11SlotInfo *keyslot;

PawdFox::PawdFox() {
}

int PawdFox::OpenIni(const string &path) {
    DEBUG_MSG("OpenIni: " << path);
    if (path != empty) {
        profileIniPath = path;
    }
    //TODO: Confirmar separador entre profileIniPath e INI_FILE_NAME

    string profilePath = profileIniPath + "/" + LIBPAWD_PROFILES_INI_FILE_NAME;
    ifstream iniFile;
    iniFile.open(profilePath);
    if (iniFile.is_open()) {
        string line;
        string pathSubStr = LIBPAWD_PATH_SUBSTRING_PROFILES_INI;
        string nameSubStr = LIBPAWD_NAME_SUBSTRING_PROFILES_INI;
        firefox_profile *p = new firefox_profile();
        while (getline(iniFile, line))
        {
            size_t pathPos = line.find(pathSubStr);
            if (pathPos != string::npos)
            {
                if (line.length() > (pathPos + pathSubStr.length())) {
                    string userProfilePath = profileIniPath + "/" + line.substr(pathPos + pathSubStr.length());
                    p->path = userProfilePath;
                }
            }
            size_t namePos = line.find(nameSubStr);
            if (namePos != string::npos)
            {
                if (line.length() > (namePos + nameSubStr.length())) {
                    string profileName = line.substr(namePos + nameSubStr.length());
                    p->name = profileName;
                }
            }
            if (p->path.size() > 0 && p->name.size() > 0) {
                profiles.push_back(*p);
                p = new firefox_profile;             
            }
        }
        iniFile.close();
        return LIBPAWD_STATUS_SUCCESS;
    } else {
        iniFile.close();
        return LIBPAWD_STATUS_UNKNOWN;
    }
}

int PawdFox::ReadLogins(const firefox_profile profile) {
    DEBUG_MSG("Opening profile " + profile.path + "/logins.json");
    string path = profile.path + "/logins.json";
    FILE* fp = fopen(path.c_str(), "rb");
    if (fp == NULL)
    {
        return LIBPAWD_STATUS_FILE_DOESNT_EXIST;
    }
    int status = NssOpenFile(profile);
    if (status != LIBPAWD_STATUS_SUCCESS)
    {
        fclose(fp);
        return status;
    }
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(is);

    const rapidjson::Value& loginsValue = d["logins"];

    if (loginsValue.IsArray()) {
        credentials.clear();
        for (rapidjson::SizeType i = 0; i < loginsValue.Size(); i++)
        {
            firefox_credential credential;
            credential.hostname = loginsValue[i]["hostname"].GetString();
            credential.encrypted_username = loginsValue[i]["encryptedUsername"].GetString();
            credential.encrypted_password = loginsValue[i]["encryptedPassword"].GetString();
            string out;
            if (Decrypt(credential.encrypted_username, out) != 0)
            {
                credentials.clear();
                return LIBPAWD_STATUS_DECRYPT_FAILED;
            }
            credential.username = out;
            credentials.push_back(credential);
        }
    }

    fclose(fp);
    return 0;
}

vector<firefox_credential> PawdFox::Filter(const string query) {
    vector<firefox_credential> ret;
    for (std::vector<firefox_credential>::const_iterator iterator = credentials.begin(), 
                end = credentials.end(); iterator != end; ++iterator) {
        if (iterator->hostname.find(query) != string::npos ||
                iterator->username.find(query) != string::npos) {
            ret.push_back(*iterator);
        }
    }
    return ret;
}

int PawdFox::NssOpenFile(const firefox_profile profile) {
    if (NSS_IsInitialized() == PR_FALSE)
    {
        string fullPath = "sql:" + profile.path;
        SECStatus rv = NSS_Initialize(fullPath.c_str(), "", "", SECMOD_DB, NSS_INIT_NOROOTINIT | NSS_INIT_OPTIMIZESPACE | NSS_INIT_READONLY);
        if (rv != SECSuccess) {
            return LIBPAWD_STATUS_FAIL_SHUTDOWN;
        }
        keyslot = PK11_GetInternalKeySlot();
    }
    SECStatus result = PK11_CheckUserPassword(keyslot, profile.password.c_str());
    if (result == SECSuccess)
    {
        return LIBPAWD_STATUS_SUCCESS;
    }
    else
    {
        return LIBPAWD_STATUS_WRONG_PASSWORD;
    }
}

int PawdFox::CloseProfile() {
    if (NSS_IsInitialized() == PR_TRUE) {
        DEBUG_MSG("Closing Profile");
        PK11_FreeSlot(keyslot);
        SECStatus shutdownResult = NSS_Shutdown();
        if (shutdownResult != SECSuccess) {
            DEBUG_MSG("Fail NSS_Shutdown");
            return LIBPAWD_STATUS_FAIL_SHUTDOWN;
        }
    }
    return LIBPAWD_STATUS_SUCCESS;
}

int PawdFox::Decrypt(const string data, string &ret) {
    DEBUG_MSG("Decrypting " << data);
    SECItem *item = NSSBase64_DecodeBuffer(NULL, NULL, data.c_str(), strlen(data.c_str()));
    SECItem out;
    SECStatus status = PK11SDR_Decrypt(item, &out, 0);
    SECITEM_FreeItem(item, PR_TRUE);
    if (status == SECSuccess) {
        string decrypted(reinterpret_cast<const char*>(out.data), out.len);
        ret = decrypted;
        DEBUG_MSG("Decrypted succesfuly: " << ret);
        return LIBPAWD_STATUS_SUCCESS;
    }
    DEBUG_MSG("Error decrypting, PK11SDR_Decrypt return: " << status);
    return LIBPAWD_STATUS_UNKNOWN;
}

string PawdFox::GetPassword(const firefox_credential credential) {
    string out;
    if (Decrypt(credential.encrypted_password, out) != 0)
    {
        credentials.clear();
        throw decrypt_exception();
    }
    return out;
}

string PawdFox::GetPassword(const string encrypted_password) {
    string out;
    if (Decrypt(encrypted_password, out) != 0)
    {
        credentials.clear();
        throw decrypt_exception();
    }
    return out;
}