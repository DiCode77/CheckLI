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

constexpr const char     *AC_VERSION      = "0.0.1";
constexpr const char     *API_SVBOT       = "https://api.svitlobot.in.ua/website/getChannelsForMap";
constexpr const char32_t FIND_IF_QUANTY[] = { 0x2D, 0x31, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_GROUPS[] = { 0x3B, 0x26, 0x26, 0x26, 0x3B, 0x00 };
constexpr const char32_t FIND_IF_NEXT[]   = { 0x2D, 0x3E, 0x00 };
constexpr int            MAX_PARAMETERS   = 10;

constexpr unsigned long  ACNPOS           = ~0;
const     long           REQUEST_DELAY    = 60; // Specify in seconds. Requests to the server are allowed to be sent no more than once every 30 seconds.

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
    void updRequest();
    
    // Here we return the status of the program.
    const AC_INFO &isStatus() const;
    
    // Returns the total number of all fireflies.
    const ulong_t getTotalCount() const;
    
    // Returns the total number of all fireflies in the city.
    const ulong_t getTotalCountOfCity(const std::u32string&) const;
    
    // Returns a list of all available fireflies in the city.
    const vec_place_t *getPlacesByCity(const std::u32string&) const;
    
    // Returns a specific structure in the city.
    const PLACE *getPlacesByCityAndIndex(const std::u32string&, const ulong_t&) const;
    
    // I added this method and the next one so that I could display the entire list of cities or streets for a given city in order to see if any individual corrections were needed.
    void showAllСities();
    void showAllStreets(const std::u32string&);
    
    void clear();
    
    // A method for correcting city names. Often, an error in a city name results in the creation of a separate section on the map. Call once before updRequest().
    void setCorrectCityNames(IMP_PLE::vec_pir_t (*)());
    
    // Method for setting a group for any address. Call once before updRequest().
    void setSpecifyGroup(IMP_PLE::vec_pir_t (*)());
    
    /*
     This method adds multiple additions of different specific cities, with a whole list of streets, i.e. if you need to determine which district a street belongs to, you need to transfer the street and district here, after which a search for the street in the specified city will take place. If such a street is found in the city, the district will be recorded there. This method must be called once before updRequest().
     
     The data will be automatically updated even with a new API request.
     The main thing is not to forget to add new streets and the district to which they belong.
     */
    void addDistrictToStreet(const std::u32string&, IMP_PLE::vec_pir_t (*)());
    
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
