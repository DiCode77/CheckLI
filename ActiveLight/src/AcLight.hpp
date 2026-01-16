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
#include <vector>
#include <ranges>
#include <unordered_map>

#include <iostream>

constexpr const char *API_SVBOT = "https://api.svitlobot.in.ua/website/getChannelsForMap";

typedef struct{
    int         condition;
    std::string date;
    std::string street;
    std::string tg_group;
    int         tg_users;
    std::string longitude;
    std::string latitude;
    int         device_type;
    std::string group;
    int         online; // ?
} PLACE;

using un_map_t = std::unordered_map<std::string, std::vector<PLACE>>;

class AcLight{
    long                     timeout;
    bool                     error;
    std::vector<std::string> save_get_res;
    long                     users;
    un_map_t                 um_data;
    
public:
    AcLight(){
        this->timeout = 10L;
        this->error   = false;
        this->users   = 0;
    }
    
    void timeOut(long tout){
        this->timeout = tout;
    }
    
    void update(){
        std::string res;
        if (this->MakeRequest(API_SVBOT, res) && !res.empty()){
            this->DevideIntoGroup(this->save_get_res, res);
            this->users = this->save_get_res.size();
            this->BreakDownTheLine(this->save_get_res, this->um_data);
        }
        else{
            this->error = true;
            return;
        }
    }
    
    bool isError() const{ // if true == error
        return this->error;
    }
    
private:
    bool MakeRequest(const char*, std::string&, const long=10L);
    void DevideIntoGroup(std::vector<std::string>&, const std::string&);
    void BreakDownTheLine(const std::vector<std::string>&, un_map_t&);
    std::string SetDataStructs(const int&, PLACE&, const std::string&);
    std::string DiBeFiel(const std::string&, std::string&);
    
};

#endif /* AcLight_hpp */
