#ifndef LIBPAWDFOX_H
#define LIBPAWDFOX_H
// MACOS 
#include <string>
#include <vector>

#ifdef __APPLE__
#define LIBPAWD_PROFILE_PATH std::string(getenv("HOME")) + "/Library/Application Support/Firefox/"
#elif __linux__
#define LIBPAWD_PROFILE_PATH std::string(getenv("HOME")) + "/.mozilla/firefox/";
#else
#define LIBPAWD_PROFILE_PATH std::string(getenv("APPDATA")) + "/Mozilla/Firefox/";
#endif

#define LIBPAWD_PROFILES_INI_FILE_NAME "profiles.ini"
#define LIBPAWD_PATH_SUBSTRING_PROFILES_INI "Path="
#define LIBPAWD_NAME_SUBSTRING_PROFILES_INI "Name="

#define LIBPAWD_STATUS_SUCCESS 0
#define LIBPAWD_STATUS_UNKNOWN -1
#define LIBPAWD_STATUS_FILE_DOESNT_EXIST -2
#define LIBPAWD_STATUS_DECRYPT_FAILED -3
#define LIBPAWD_STATUS_WRONG_PASSWORD -4
#define LIBPAWD_STATUS_FAIL_SHUTDOWN -5
#ifdef __cplusplus
extern "C" {
#endif

/* Versioning, past 1.0.0
    http://semver.org/
*/

namespace libpawdfox {
    struct firefox_profile {
        std::string name = "";
        std::string path = "";
        std::string password = "";
    };

    struct firefox_credential {
        std::string encrypted_username;
        std::string encrypted_password;
        std::string username;
        std::string hostname;
        std::string username_field;
        std::string password_field;
        int enc_type;
        int64_t time_created;
        int64_t time_last_used;
        int64_t time_password_changed;
        int times_used;
    };

    struct decrypt_exception : public std::exception {
        const char * what () const throw () {
            return "Fail to decrypt!";
        }
    };

    const std::string empty = std::string();

    class PawdFox {
        private:
            std::string profileIniPath = LIBPAWD_PROFILE_PATH;
            int NssOpenFile(const firefox_profile profile);
            int Decrypt(const std::string data, std::string &ret);
        public:
            std::vector<firefox_profile> profiles;
            std::vector<firefox_credential> credentials;
            PawdFox();
            int OpenIni(const std::string &path = empty);
            int ReadLogins(const firefox_profile profile);
            std::vector<firefox_credential> Filter(const std::string query);
            std::string GetPassword(const firefox_credential cretential);
            std::string GetPassword(const std::string encrypted_password);
            int CloseProfile();
    };
}

#ifdef __cplusplus
}
#endif

#endif // LIBPAWDFOX_H