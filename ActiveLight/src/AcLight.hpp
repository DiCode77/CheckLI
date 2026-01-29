//
//  AcLight.hpp
//  ActiveLight
//
//  Created by DiCode77.
//

#ifndef AcLight_hpp
#define AcLight_hpp

#include <curl/curl.h>
#include <string>
#include <utf8cpp/utf8.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <utility>
#include <set>

constexpr const char     *API_SVBOT       = "https://api.svitlobot.in.ua/website/getChannelsForMap";
constexpr const char32_t FIND_IF_QUANTY[] = { 0x2D, 0x31, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_GROUPS[] = { 0x3B, 0x26, 0x26, 0x26, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_NEXT[]   = { 0x2D, 0x3E, 0x00 };
constexpr int            MAX_PARAMETERS   = 10;

constexpr unsigned long  ACNPOS           = ~0;
const     long           REQUEST_DELAY    = 30; // Specify in seconds. Requests to the server are allowed to be sent no more than once every 30 seconds.

enum class AC_INFO{
    AC_NONE,
    AC_OK,
    AC_REQUEST_DELAY,
    AC_ERROR_REQUEST,
    AC_ERROR_PARSE
};

typedef struct{
    long           condition;
    std::u32string date;
    std::u32string street;
    std::u32string tg_group;
    long           tg_users;
    std::u32string longitude;
    std::u32string latitude;
    long           device_type;
    std::u32string group;
    long           online; // ?
    std::u32string district;
} PLACE;

struct IMP_PLE{
    using vec_pir_t = std::vector<std::pair<std::u32string, std::u32string>>;
    using set_vec_t = std::set<std::pair<std::u32string, vec_pir_t>>;
    
    vec_pir_t f_city{};
    vec_pir_t f_group{};
    set_vec_t f_ditri{};
};

class AcLight{
public:
    using ulong_t     = unsigned long;
    using vecstr_t    = std::vector<std::u32string>;
    using un_map_t    = std::unordered_map<std::u32string, std::vector<PLACE>>;
    using vec_place_t = std::vector<PLACE>;
    using time_w_t    = std::chrono::steady_clock::time_point;
    
private:
    AC_INFO  error;
    vecstr_t dtbt;
    un_map_t um_data;
    time_w_t wait;
    IMP_PLE  imp_pl;
    
public:
    AcLight() : error(AC_INFO::AC_NONE){}
    
    // updating status information.
    void updRequest(){
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
    
    // Here we return the status of the program.
    const AC_INFO &isStatus() const{
        return this->error;
    }
    
    // Returns the total number of all fireflies.
    const ulong_t getTotalCount() const{
        return this->dtbt.size();
    }
    
    // Returns the total number of all fireflies in the city.
    const ulong_t getTotalCountOfCity(const std::u32string &city){
        return (this->um_data.contains(city)) ? this->um_data[city].size() : 0;
    }
    
    // Returns a list of all available fireflies in the city.
    const vec_place_t getListByCity(const std::u32string &city){
        if (!this->um_data.empty() && this->um_data.contains(city)){
            return this->um_data[city];
        }
        return {};
    }
    
    // Returns a specific structure in the city.
    const PLACE getStructByCityOfIndex(const std::u32string &city, const ulong_t &ix){
        if (!this->um_data.empty() && this->um_data.contains(city)){
            if (this->um_data[city].size() > ix){
                return this->um_data[city].at(ix);
            }
        }
        return {};
    }
    
    void showAllСities(){
        if (!this->um_data.empty()){
            for (const auto &[key, val] : this->um_data){
                printf("->%s<-\n", utf8::utf32to8(key).c_str());
            }
        }
    }
    
    void showAllStreets(std::u32string city){
        if (!this->um_data.empty() && this->um_data.contains(city)){
            for (ulong_t i = 0; i < this->um_data[city].size(); i++){
                printf("->%s<-\n", utf8::utf32to8(this->um_data[city].at(i).street).c_str());
            }
        }
    }
    
    void clear(){
        this->SetErrorStatus(AC_INFO::AC_NONE);
        this->dtbt.clear();
        this->um_data.clear();
    }
    
    // A method for correcting city names. Often, an error in a city name results in the creation of a separate section on the map.
    void setCorrectCityNames(IMP_PLE::vec_pir_t (*p)()){
        this->imp_pl.f_city = p();
    }
    
    // Method for setting a group for any address.
    void setSpecifyGroup(IMP_PLE::vec_pir_t (*p)()){
        this->imp_pl.f_group = p();
    }
    
    void addDtreetToACityDistrict(const std::u32string &city, IMP_PLE::vec_pir_t (*p)()){
        this->imp_pl.f_ditri.insert(std::make_pair(city, p()));
    }
    
private:
    bool IsMakeRequest(const char*, std::u32string&, const long=10L);
    void DevideIntoGroup(vecstr_t&, const std::u32string&);
    void BreakDownTheLine(const vecstr_t&, un_map_t&);
    std::u32string SetDataStructs(const ulong_t&, PLACE&, const std::u32string&);
    std::u32string DiBeFiel(const std::u32string&, std::u32string&);
    std::u32string RemExtChar(std::u32string);
    std::u32string ReworkNa(const IMP_PLE::vec_pir_t&, const std::u32string&);
    std::u32string ReworkSt(const IMP_PLE::vec_pir_t&, const std::u32string&, const std::u32string&);
    void SetErrorStatus(const AC_INFO);
    void SetWait(const time_w_t&);
    const time_w_t &GetWait() const;
    void AddStreetToTheDistrict(const IMP_PLE::set_vec_t&, un_map_t&);
};

#endif /* AcLight_hpp */
