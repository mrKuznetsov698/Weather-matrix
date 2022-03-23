#include <Arduino.h>

class Timer{
    public:
        Timer(uint32_t prd){
            this->prd = prd;
            this->handler = NULL;
        }

        void attach(void (*func)()){
            handler = func;
        }

        void detach(){
            handler = NULL;
        }

        void setTimeout(uint32_t new_prd){
            prd = new_prd;
        }

        bool tick(){
            if (millis() - tmr >= prd){
                tmr = millis();
                if (handler) handler();
                return true;
            }
            return false;
        }
    private:
        uint32_t tmr;
        uint32_t prd;
        void (*handler)() = NULL;
};