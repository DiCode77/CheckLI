//
//  AcLight.cpp
//  ActiveLight
//
//  Created by DiCode77.
//

#include "AcLight.hpp"

void AcLight::updRequest(){
    std::u32string res;
    time_w_t       time;
    bool           isOk = false;
    
    if (time == this->GetWait()){
        isOk = true;
    }
    else{
        time = std::chrono::steady_clock::now();
        
        // The request delay algorithm is necessary to protect yourself from IP blocking by the server.
        if (static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(time - this->GetWait()).count()) > REQUEST_DELAY){
            isOk = true;
        }
        else{
            isOk = false;
        }
    }

    if (isOk){
        if (this->IsMakeRequest(API_SVBOT, res) && this->isStatus() == AC_INFO::AC_OK){
            this->SetWait(std::chrono::steady_clock::now()); // We save the time of the last successful response.
            this->clear(); // After each new request, you need to clear the data buffer to avoid collisions.

            this->DevideIntoGroup(this->dtbt, res);
            this->BreakDownTheLine(this->dtbt, this->um_data);
            
            this->AddStreetToTheDistrict(this->imp_pl.f_ditri, this->um_data);
        }
    }
    else{
        this->SetErrorStatus(AC_INFO::AC_REQUEST_DELAY);
    }
}

void AcLight::load(const std::u32string &data){
    if (!data.empty()){
        this->clear();
        
        this->DevideIntoGroup(this->dtbt, data);
        this->BreakDownTheLine(this->dtbt, this->um_data);
        
        this->AddStreetToTheDistrict(this->imp_pl.f_ditri, this->um_data);
    }
    else{
        this->SetErrorStatus(AC_INFO::AC_NONE_LOAD);
    }
}

const AC_INFO &AcLight::isStatus() const{
    return this->error;
}

const AcLight::ulong_t AcLight::getTotalCount() const{
    return static_cast<ulong_t>(this->dtbt.size());
}

const AcLight::ulong_t AcLight::getTotalCountOfCity(const std::u32string &city) const{
    if (auto it = this->um_data.find(city); it != this->um_data.end()){
        return static_cast<ulong_t>(it->second.size());
    }
    return 0;
}

const AcLight::vec_place_t *AcLight::getPlacesByCity(const std::u32string &city) const{
    if (auto it = this->um_data.find(city); it != this->um_data.end()){
        return &it->second;
    }
    return nullptr;
}

const PLACE *AcLight::getPlacesByCityAndIndex(const std::u32string &city, const AcLight::ulong_t &ix) const{
    if (const auto it = um_data.find(city); it != this->um_data.end()){
        if (it->second.size() > ix){
            return &it->second.at(ix);
        }
    }
    return nullptr;
}

void AcLight::showAllСities(){
    if (!this->um_data.empty()){
        for (const auto &[key, val] : this->um_data){
            printf("->%s<-\n", utf8::utf32to8(key).c_str());
        }
    }
}

void AcLight::showAllStreets(const std::u32string &city){
    if (!this->um_data.empty() && this->um_data.contains(city)){
        for (ulong_t i = 0; i < static_cast<ulong_t>(this->um_data[city].size()); i++){
            printf("->%s<-\n", utf8::utf32to8(this->um_data[city].at(i).street).c_str());
        }
    }
}

void AcLight::clear(){
    this->SetErrorStatus(AC_INFO::AC_NONE);
    this->dtbt.clear();
    this->um_data.clear();
}

void AcLight::setCorrectCityNames(IMP_PLE::vec_pir_t (*p)()){
    this->imp_pl.f_city = p();
}

void AcLight::setSpecifyGroup(IMP_PLE::vec_pir_t (*p)()){
    this->imp_pl.f_group = p();
}

void AcLight::addDistrictToStreet(const std::u32string &city, IMP_PLE::vec_pir_t (*p)()){
    this->imp_pl.f_ditri.insert(std::make_pair(city, p()));
}

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
            
            if (code == 200 && !data.empty()){
                response.assign(utf8::utf8to32(data));
                this->SetErrorStatus(AC_INFO::AC_OK);
                curl_easy_cleanup(curl);
                return true;
            }
        }
    }
    
    this->SetErrorStatus(AC_INFO::AC_ERROR_REQUEST);
    curl_easy_cleanup(curl);
    return false;
}

void AcLight::DevideIntoGroup(vecstr_t &inVec, const std::u32string &inData){
    ulong_t start = 0;
    ulong_t end   = 0;
    
    while ((start = std::u32string(inData).find(FIND_IF_QUANTY, end)) != std::u32string::npos){
        end = std::u32string(inData).find(FIND_IF_QUANTY, start +1);
        end = static_cast<ulong_t>((end == std::u32string::npos && start < inData.size()) ? inData.size() : end);
        
        inVec.emplace_back(inData, start, end - start);
    }
    this->SetErrorStatus((inVec.empty()) ? AC_INFO::AC_ERROR_PARSE : AC_INFO::AC_OK);
}

void AcLight::BreakDownTheLine(const vecstr_t &inVec, un_map_t &inData){
    if (this->isStatus() == AC_INFO::AC_OK){
        std::ranges::for_each(inVec.begin(), inVec.end(), [&inData, this](const std::u32string &str32){
            ulong_t        start  = 0;
            ulong_t        end    = 0;
            ulong_t        index  = 0;
            PLACE          lig{};
            std::u32string city;
            ulong_t        size_f = static_cast<ulong_t>(std::u32string(FIND_IF_GROUPS).size());
            
            while ((start = std::u32string(str32).find(FIND_IF_GROUPS, end)) != std::u32string::npos && index < MAX_PARAMETERS) {
                end = std::u32string(str32).find(FIND_IF_GROUPS, start +1);
                
                std::u32string data = std::u32string(str32, start + size_f, end - (start + size_f));
                std::u32string res  = this->SetDataStructs(index++, lig, data);
                if (!res.empty()){
                    city = this->ReworkNa(this->imp_pl.f_city, res);
                }
            }
            lig.district.assign(U"-");
            inData[city].push_back(lig);
        });
        this->SetErrorStatus((inData.empty()) ? AC_INFO::AC_ERROR_PARSE : AC_INFO::AC_OK);
    }
}

std::u32string AcLight::SetDataStructs(const ulong_t &ix, PLACE &d, const std::u32string &s){
    std::u32string inf;
    switch (ix) {
        case 0: d.condition   = std::stoi(utf8::utf32to8(s));                      break;
        case 1: d.date        = s;                                                 break;
        case 2: d.street      = this->DiBeFiel(s, inf);                            break;
        case 3: d.tg_group    = s;                                                 break;
        case 4: d.tg_users    = std::stoi(utf8::utf32to8(s));                      break;
        case 5: d.longitude   = s;                                                 break;
        case 6: d.latitude    = s;                                                 break;
        case 7: d.device_type = std::stoi(utf8::utf32to8(s));                      break;
        case 8: d.group       = this->ReworkSt(this->imp_pl.f_group, d.street, s); break;
        case 9: d.online      = std::stoi(utf8::utf32to8(s));                      break;
        default: break;
    }
    return inf;
}

std::u32string AcLight::DiBeFiel(const std::u32string &str, std::u32string &inf){
    std::u32string res;
    ulong_t en = str.find(FIND_IF_NEXT);
    ulong_t sz = static_cast<ulong_t>(std::u32string(FIND_IF_NEXT).size());
    
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

std::u32string AcLight::ReworkNa(const IMP_PLE::vec_pir_t &vec, const std::u32string &d){
    if (!vec.empty()){
        auto it = std::find_if(vec.begin(), vec.end(), [&d](const std::pair<std::u32string, std::u32string> &pr){
            return pr.first == d;
        });
        
        if (it != vec.end()){
            return it->second;
        }
    }
    
    return d;
}

std::u32string AcLight::ReworkSt(const IMP_PLE::vec_pir_t &vec, const std::u32string &stt, const std::u32string &def){
    if (!vec.empty()){
        auto it = std::find_if(vec.begin(), vec.end(), [&stt](const std::pair<std::u32string, std::u32string> &pr){
            return pr.first == stt;
        });
        
        if (it != vec.end()){
            return it->second;
        }
    }
    
    return def;
}

void AcLight::SetErrorStatus(const AC_INFO st){
    this->error = st;
}

void AcLight::SetWait(const time_w_t &t){
    this->wait = t;
}

const AcLight::time_w_t &AcLight::GetWait() const{
    return this->wait;
}

void AcLight::AddStreetToTheDistrict(const IMP_PLE::set_vec_t &s_vec, un_map_t &stt){
    if (this->isStatus() == AC_INFO::AC_OK && !s_vec.empty() && !stt.empty()){
        std::ranges::for_each(s_vec.begin(), s_vec.end(), [&stt](const std::pair<std::u32string, IMP_PLE::vec_pir_t> &pir){
            if (stt.contains(pir.first)){
                std::vector<PLACE> &vec = stt[pir.first];
                std::ranges::for_each(vec.begin(), vec.end(), [&pir](PLACE &p){
                    auto it = std::find_if(pir.second.begin(), pir.second.end(), [&p](const std::pair<std::u32string, std::u32string> &pr){
                        return p.street == pr.first;
                    });
                    
                    if (it != pir.second.end()){
                        p.district = it->second;
                    }
                });
            }
        });
    }
}
