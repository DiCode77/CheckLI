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
    int      error;
    vecstr_t dtbt;
    un_map_t um_data;
    
public:
    AcLight(){
        this->error = 0;
    }
    
    void updRequest(){
        std::u32string res;
        if (this->IsMakeRequest(API_SVBOT, res) && !res.empty()){
            this->DevideIntoGroup(this->dtbt, res);
            this->BreakDownTheLine(this->dtbt, this->um_data);
        }
    }
    
    const vac_place_t getStruct(std::u32string str){
        if (this->um_data.contains(str)){
            return this->um_data[str];
        }
        return {};
    }
    
private:
    bool IsMakeRequest(const char*, std::u32string&, const long=10L);
    void DevideIntoGroup(vecstr_t&, const std::u32string&);
    void BreakDownTheLine(const vecstr_t&, un_map_t&);
    std::u32string SetDataStructs(const ulong_t&, PLACE&, const std::u32string&);
    std::u32string DiBeFiel(const std::u32string&, std::u32string&);
};

#endif /* AcLight_hpp */
