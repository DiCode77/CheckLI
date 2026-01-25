//
//  AcLight.cpp
//  ActiveLight
//
//  Created by DiCode77.
//

#include "AcLight.hpp"

static AcLight::ulong_t WriteCallBack(void *contents, AcLight::ulong_t size, AcLight::ulong_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool AcLight::IsMakeRequest(const char *API, std::u32string &response, const long timeout){
    CURL        *curl;
    CURLcode    resc;
    std::string data;
    
    curl = curl_easy_init();
    
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, API);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L); // for max time connected 5s
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        
        resc = curl_easy_perform(curl);
        
        if (resc == CURLE_OK){
            ulong_t code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
            curl_easy_cleanup(curl);
            
            if (code != 200){
                return false;
            }
            else{
                response.assign(utf8::utf8to32(data));
                return true;
            }
        }
    }
    
    curl_easy_cleanup(curl);
    return false;
}

void AcLight::DevideIntoGroup(vecstr_t &inVec, const std::u32string &inData){
    ulong_t start = 0;
    ulong_t end   = 0;
    
    while ((start = std::u32string(inData).find(FIND_IF_QUANTY, end)) != std::u32string::npos){
        end = std::u32string(inData).find(FIND_IF_QUANTY, start +1);
        end = (end == std::u32string::npos && start < inData.size()) ? inData.size() : end;
        
        inVec.emplace_back(inData, start, end - start);
    }
}

void AcLight::BreakDownTheLine(const vecstr_t &inVec, un_map_t &inData){
    std::ranges::for_each(inVec.begin(), inVec.end(), [&inData, this](const std::u32string &str32){
        ulong_t        start  = 0;
        ulong_t        end    = 0;
        ulong_t        index  = 0;
        PLACE          lig{};
        std::u32string city;
        ulong_t        size_f = std::u32string(FIND_IF_GROUPS).size();
        
        while ((start = std::u32string(str32).find(FIND_IF_GROUPS, end)) != std::u32string::npos && index < MAX_PARAMETERS) {
            end = std::u32string(str32).find(FIND_IF_GROUPS, start +1);
            
            std::u32string data = std::u32string(str32, start + size_f, end - (start + size_f));
            std::u32string res  = this->SetDataStructs(index++, lig, data);
            if (!res.empty()){
                city = res;
            }
        }
        inData[city].push_back(lig);
    });
}

std::u32string AcLight::SetDataStructs(const ulong_t &ix, PLACE &d, const std::u32string &s){
    std::u32string inf;
    switch (ix) {
        case 0: d.condition   = std::stoi(utf8::utf32to8(s)); break;
        case 1: d.date        = s;                            break;
        case 2: d.street      = this->DiBeFiel(s, inf);       break;
        case 3: d.tg_group    = s;                            break;
        case 4: d.tg_users    = std::stoi(utf8::utf32to8(s)); break;
        case 5: d.longitude   = s;                            break;
        case 6: d.latitude    = s;                            break;
        case 7: d.device_type = std::stoi(utf8::utf32to8(s)); break;
        case 8: d.group       = s;                            break;
        case 9: d.online      = std::stoi(utf8::utf32to8(s)); break;
        default: break;
    }
    return inf;
}

std::u32string AcLight::DiBeFiel(const std::u32string &str, std::u32string &inf){
    std::u32string res;
    ulong_t en = str.find(FIND_IF_NEXT);
    ulong_t sz = std::u32string(FIND_IF_NEXT).size();
    
    if (en != std::u32string::npos){
        inf.assign(this->RemExtChar(std::u32string(str, 0, en)));
        res.assign(this->RemExtChar(std::u32string(str, en + sz, str.size())));
    }
    
    return res;
}


std::u32string AcLight::RemExtChar(std::u32string cstr){
    const std::vector<char32_t> svec = { U' ', U'-', U',' };
    
    auto lm = [&svec](const char32_t ch){
        for (const char32_t &s : svec){
            if (s == ch){
                return true;
            }
        }
        return false;
    };
    
    // This block of code removes extra characters at the beginning of a line.
    while (!cstr.empty()){
        auto it = cstr.begin();
        
        if (lm(*it)){
            cstr.erase(it);
        }
        else{
            break;
        }
    }
    
    // This block of code removes extra characters at the end of a line.
    while (!cstr.empty()) {
        auto it = cstr.rbegin();
        
        if (lm(*it)){
            cstr.erase(std::next(it).base());
        }
        else{
            break;
        }
    }
    
    return cstr;
}
