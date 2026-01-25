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

constexpr const char     *API_SVBOT       = "https://api.svitlobot.in.ua/website/getChannelsForMap";
constexpr const char32_t FIND_IF_QUANTY[] = { 0x2D, 0x31, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_GROUPS[] = { 0x3B, 0x26, 0x26, 0x26, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_NEXT[]   = { 0x2D, 0x3E, 0x00 };
constexpr int            MAX_PARAMETERS   = 10;

constexpr unsigned long  ACNPOS           = ~0;

enum class AC_INFO{
    AC_NONE,
    AC_OK,
    AC_ERROR_REQUEST,
    AC_ERROR_PARSE
};

enum class AC_BEHAVIOR{ // tested.
    NONE,
    UPDATE
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
} PLACE;

class AcLight{
public:
    using ulong_t     = unsigned long;
    using vecstr_t    = std::vector<std::u32string>;
    using un_map_t    = std::unordered_map<std::u32string, std::vector<PLACE>>;
    using vac_place_t = std::vector<PLACE>;
    
private:
    AC_INFO  error;
    vecstr_t dtbt;
    un_map_t um_data;
    
public:
    AcLight(){
        this->error = AC_INFO::AC_NONE;
    }
    
    // Test function is still being tested.
    AcLight(AC_BEHAVIOR) : AcLight(){ //
        this->updRequest();
    }
    
    // updating status information.
    void updRequest(){
        std::u32string res;
        if (this->IsMakeRequest(API_SVBOT, res) && this->isStatus() == AC_INFO::AC_OK){
            this->DevideIntoGroup(this->dtbt, res);
            this->BreakDownTheLine(this->dtbt, this->um_data);
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
    const vac_place_t getListByCity(const std::u32string &city){
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
    
private:
    bool IsMakeRequest(const char*, std::u32string&, const long=10L);
    void DevideIntoGroup(vecstr_t&, const std::u32string&);
    void BreakDownTheLine(const vecstr_t&, un_map_t&);
    std::u32string SetDataStructs(const ulong_t&, PLACE&, const std::u32string&);
    std::u32string DiBeFiel(const std::u32string&, std::u32string&);
    std::u32string RemExtChar(std::u32string);
    void SetErrorStatus(const AC_INFO);
};

#endif /* AcLight_hpp */
