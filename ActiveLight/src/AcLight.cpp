//
//  AcLight.cpp
//  ActiveLight
//
//  Created by DiCode77.
//

#include "AcLight.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool AcLight::MakeRequest(const char *api, std::string &cbeck, const long timeout){

    CURL        *curl;
    CURLcode    res;
    
    curl = curl_easy_init();
    
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, api);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L); // for max time connected 5s
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); //
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cbeck);
        
        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK){
            long http_get_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_get_code);
            curl_easy_cleanup(curl);
            
            if (http_get_code != 200){
                return false;
            }
            else{
                return true;
            }
        }
    }
    
    curl_easy_cleanup(curl);
    return false;
}

void AcLight::DevideIntoGroup(std::vector<std::string> &inVec, const std::string &inData){
    const char *finfIF = "-1;";
    int64_t     start = 0;
    int64_t     end   = 0;

    while ((start = std::string(inData).find(finfIF, end)) != std::string::npos){
        end = std::string(inData).find(finfIF, start +1);
        end = (end == std::string::npos && start < inData.size()) ? inData.size() : end;
        
        inVec.emplace_back(inData, start, end - start);
    }
}

void AcLight::BreakDownTheLine(const std::vector<std::string> &inVec, un_map_t &inDet){
    const char *findIF   = ";&&&;";
    const int PAR        = 10;

    std::ranges::for_each(inVec.begin(), inVec.end(), [findIF, &inDet, this](const std::string &str){
        int64_t     start = 0;
        int64_t     end   = 0;
        int         index = 0;
        PLACE       lig{};
        std::string city;
        
        while ((start = std::string(str).find(findIF, end)) != std::string::npos && index < PAR){
            end   = std::string(str).find(findIF, start +1);
            
            std::string data = std::string(str, start + std::string(findIF).size(), end - (start + std::string(findIF).size()));
            std::string res = this->SetDataStructs(index++, lig, data);
            if (!res.empty()){
                city = res;
            }
        }
        inDet[city].push_back(lig);
    });
}

std::string AcLight::SetDataStructs(const int &ix, PLACE &d, const std::string &s){
    std::string inf;
    switch (ix) {
        case 0: d.condition = std::stoi(s);        break;
        case 1: d.date = s;                        break;
        case 2: d.street = this->DiBeFiel(s, inf); break;
        case 3: d.tg_group = s;                    break;
        case 4: d.tg_users = std::stoi(s);         break;
        case 5: d.longitude = s;                   break;
        case 6: d.latitude = s;                    break;
        case 7: d.device_type = std::stoi(s);      break;
        case 8: d.group = s;                       break;
        case 9: d.online = std::stoi(s);           break;
        default: break;
    }
    return inf;
}

std::string AcLight::DiBeFiel(const std::string &str, std::string &inf){
    const std::string findIF = "->";
    std::string res;
    int64_t en = str.find(findIF);
    
    if (en != std::string::npos){
        inf.append(str, 0, en -1);
        res.append(str, en + findIF.size(), str.size());
    }
    
    return res;
}
