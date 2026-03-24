bool hosei_for_preset(int headPosition);
    

bool hosei_for_preset(int headPosition){
    data_t limitData;
    file.read_hosei(headPosition); 
    auto_zero_point();
    limitData.min = -100;
    limitData.max = 100;
    limitData.lcd = OP_NAGAMONO_HOSEI2;//ON/OFFChenge_vaule表示

    for(int i=0; i < product.head; i++){
        lcd.set_option(OP_NAGAMONO_HOSEI2);
        lcd.locate(11,0);
        lcd.printf("%2dH",i+1);
        setHeadVolt(i, param.KURI);
        setHeadVolt(i, param.KURI);
        lcd.locate(7,1);
        lcd.printf("%5.0f",cel.getWeight(i));
        Chenge_vaule2(&hosei[i],limitData);
        iro[i] = C_NULL;
        
        if(btn.esc()) {
            file.read_hosei(headPosition); 
            led.REGIS(0xfff^stop_cells2);
            i -= 2;
            if(i<-1){
                return false;
            }
        }
    }
    
    file.save_hosei(headPosition); 
    file.read_hosei(headPosition); 
    
    return true;
}