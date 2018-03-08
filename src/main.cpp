#include "libpawdfox.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace libpawdfox;

int main(int argc, char *argv[])
{
    PawdFox *pfox = new PawdFox();
    int status = pfox->OpenIni();

    while (status != LIBPAWD_STATUS_SUCCESS) {
        cout << "Não achou profiles.ini." << endl;
        cout << "Insira o diretório completo." << endl;
        string fpath;
        getline(cin, fpath);
        string full_path = string(fpath);
        if (full_path == "exit") {
            return -3;
        }
        status = pfox->OpenIni(full_path);
    }

    firefox_profile profile;
    if (pfox->profiles.size() == 0) {
        cout << "Nenhum perfil do firefox encontrado"<< endl;
        return -10;
    } else if (pfox->profiles.size() > 1) {
        cout << "Selecione o profile desejado" << endl;
        int idx = 1;
        for (std::vector<firefox_profile>::const_iterator iterator = pfox->profiles.begin(), 
            end = pfox->profiles.end(); iterator != end; ++iterator) {
            cout << idx << ") " << iterator->name << " " << iterator->path << endl;
            idx++;
        }
        string choicestr;
        getline(cin, choicestr);

        int choice = stoi(choicestr);

        try {
            profile = pfox->profiles.at(choice-1);
            cout << "Profile Escolhido: " << profile.path << endl;
            profile.password = "";
        } catch (...) {
            cout << "Profile não encontrado" << endl;
            return -2;
        }
    } else {
        profile = pfox->profiles[0];
    }
    status = pfox->ReadLogins(profile);
    if (status == LIBPAWD_STATUS_WRONG_PASSWORD) {
        cout << "Protegido por senha!" << endl;
        cout << "Digite a senha: " << endl;
        
        string password;
        getline(cin, password);

        // Teste forçando 3 profiles, onde o ultimo não tem senha
        if (password == "") {
            profile = pfox->profiles[2];
            pfox->CloseProfile();
        }

        profile.password = password;

        status = pfox->ReadLogins(profile);
        if (status == LIBPAWD_STATUS_WRONG_PASSWORD) {
            cout << "Senha errada!" << endl;
            return LIBPAWD_STATUS_WRONG_PASSWORD;
        }
    }

    int idx = 1;
    for (std::vector<firefox_credential>::const_iterator iterator = pfox->credentials.begin(), 
        end = pfox->credentials.end(); iterator != end; ++iterator) {
        cout << idx << ") " << iterator->hostname << " " << iterator->username << endl;
        idx++;
    }

    cout << "Filtrar: " << endl;
    string query;
    
    getline(cin, query);
    vector<firefox_credential> elements = pfox->Filter(query);
    idx = 1;
    for (std::vector<firefox_credential>::const_iterator iterator = elements.begin(), 
        end = elements.end(); iterator != end; ++iterator) {
        cout << idx << ") " << iterator->hostname << " " << iterator->username << endl;
        idx++;
    }
    cout << "Escolha qual quer pegar a senha: ";

    
    string tmp;
    getline(cin, tmp);
    int item = stoi(tmp);
    string pwd = pfox->GetPassword(elements[item-1]);
    cout << endl << "Senha: " << pwd << endl;    
    return 0;
}