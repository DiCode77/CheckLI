#include <iostream>
#include <TGbot.hpp>
#include <AcLight.hpp>

int main(){
    TGBOT::TGbot bot("TOKEN");
    
    bot.Event().timeOut(3);
    bot.Event().getUpdate().UpdateAndisStatus();
    
    if (bot.getIsStatusBot()){
        std::cout << "The bot is active!" << std::endl;
        
        while (bot.Event().getUpdate().UpdateAndisStatus()){
            long upde_id(0);
            long mess_id(0);
            long chat_id(0);
            
            bot.Result().get().id().isNum(upde_id);
            
            if (bot.Result().Message().get().text().isCommand("/start")){
                if (bot.Result().Message().Chat().get().id().isNum(chat_id)){
                    bot.Event().send().message(chat_id, "Hello");
                    bot.Event().clear().updates().First(upde_id);
                }
            }
            else{
                if (upde_id != 0)
                    bot.Event().clear().updates().First(upde_id);
            }

            CURL_UPDATES().NewSleep(500);
        }
    }
    
    std::cout << "The bot is not active!" << std::endl;
    
    return 0;
}
