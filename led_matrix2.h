
//*****************************matrix**************************************************
void matrix_rank_column_RED (int i, int char_nn, int char_rn,  int col){
    /*
    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
        cel_led[i]= cel_led_bit_red(i,char_nn ,char_antei[col] ,col);
    } else {
        cel_led[i]= cel_led_bit_red(i,char_rn ,char_rantei[col],col);
    }*/
    if(option.c_rotate==R_OFF){
        cel_led[i]= cel_led_bit_red(i,char_nn ,char_antei[col] ,col);
    } else {
        cel_led[i]= cel_led_bit_red(i,char_rn ,char_rantei[col],col);
    }
}
void matrix_rank_column_GRE (int i, int char_nn, int char_rn,  int col){
    /*
    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
        cel_led[i]= cel_led_bit_green(i,char_nn ,char_antei[col] ,col);
    } else {
        cel_led[i]= cel_led_bit_green(i,char_rn ,char_rantei[col],col);
    }*/
    if(option.c_rotate==R_OFF){
        cel_led[i]= cel_led_bit_green(i,char_nn ,char_antei[col] ,col);
    } else {
        cel_led[i]= cel_led_bit_green(i,char_rn ,char_rantei[col],col);
    }

}
void matrix_column_RED (int i, int h_d, int char_nn, int char_rn,  int col){
    /*
    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
        if(h_d == 0){ cel_led[i]=( 0xFF << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 1){ cel_led[i]=( char_100[col] << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 2){ cel_led[i]=( char_200[col] << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 3){ cel_led[i]=( char_300[col] << 16 | char_nn  << 8) | (1 << col); }
    } else {
        if(h_d == 0){ cel_led[i]=( 0xFF << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 1){ cel_led[i]=( char_r100[col] << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 2){ cel_led[i]=( char_r200[col] << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 3){ cel_led[i]=( char_r300[col] << 16 | char_rn  << 8) | (1 << col); }
    }*/
    if(option.c_rotate==R_OFF){
        if(h_d == 0){ cel_led[i]=( 0xFF << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 1){ cel_led[i]=( char_100[col] << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 2){ cel_led[i]=( char_200[col] << 16 | char_nn  << 8) | (1 << col); }
        if(h_d == 3){ cel_led[i]=( char_300[col] << 16 | char_nn  << 8) | (1 << col); }
    } else {
        if(h_d == 0){ cel_led[i]=( 0xFF << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 1){ cel_led[i]=( char_r100[col] << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 2){ cel_led[i]=( char_r200[col] << 16 | char_rn  << 8) | (1 << col); }
        if(h_d == 3){ cel_led[i]=( char_r300[col] << 16 | char_rn  << 8) | (1 << col); }
    }

}
void matrix_column_GRE (int i, int char_nn, int char_rn,  int col){
    /*
    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
        cel_led[i]=( char_nn << 16 | 0xFF  << 8) | (1 << col);
    } else {
        cel_led[i]=( char_rn << 16 | 0xFF  << 8) | (1 << col);
    }*/
    if(option.c_rotate==R_OFF){
            cel_led[i]=( char_nn << 16 | 0xFF  << 8) | (1 << col);
    } else {
        cel_led[i]=( char_rn << 16 | 0xFF  << 8) | (1 << col);
    }
}
void matrix_column_YEL (int i, int char_nn, int char_rn,  int col){
    /*
    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
        cel_led[i]=( char_nn << 16 | char_nn  << 8) | (1 << col);
    } else {
        cel_led[i]=( char_rn << 16 | char_rn  << 8) | (1 << col);
    }*/
    if(option.c_rotate==R_OFF){
        cel_led[i]=( char_nn << 16 | char_nn  << 8) | (1 << col);
    } else {
        cel_led[i]=( char_rn << 16 | char_rn  << 8) | (1 << col);
    }
}


void matrix()
{
    int i;
    static int col = 0;
    for(i=0; i<product.head; i++) {
        if(iro[i] == C_NULL) {
            cel_led[i]=0;
} else if(iro[i] == CR_00                    ) {matrix_column_RED(i,0,char_00[col],char_r00[col],col);
} else if(iro[i] == CR_01 || iro[i] == CRR_01) {matrix_column_RED(i,0,char_01[col],char_r01[col],col);
} else if(iro[i] == CR_02 || iro[i] == CRR_02) {matrix_column_RED(i,0,char_02[col],char_r02[col],col);
} else if(iro[i] == CR_03 || iro[i] == CRR_03) {matrix_column_RED(i,0,char_03[col],char_r03[col],col);
} else if(iro[i] == CR_04 || iro[i] == CRR_04) {matrix_column_RED(i,0,char_04[col],char_r04[col],col);
} else if(iro[i] == CR_05 || iro[i] == CRR_05) {matrix_column_RED(i,0,char_05[col],char_r05[col],col);
} else if(iro[i] == CR_06 || iro[i] == CRR_06) {matrix_column_RED(i,0,char_06[col],char_r06[col],col);
} else if(iro[i] == CR_07 || iro[i] == CRR_07) {matrix_column_RED(i,0,char_07[col],char_r07[col],col);
} else if(iro[i] == CR_08 || iro[i] == CRR_08) {matrix_column_RED(i,0,char_08[col],char_r08[col],col);
} else if(iro[i] == CR_09 || iro[i] == CRR_09) {matrix_column_RED(i,0,char_09[col],char_r09[col],col);
} else if(iro[i] == CR_10 || iro[i] == CRR_10) {matrix_column_RED(i,0,char_10[col],char_r10[col],col);
} else if(iro[i] == CR_11 || iro[i] == CRR_11) {matrix_column_RED(i,0,char_11[col],char_r11[col],col);
} else if(iro[i] == CR_12 || iro[i] == CRR_12) {matrix_column_RED(i,0,char_12[col],char_r12[col],col);
} else if(iro[i] == CR_13 || iro[i] == CRR_13) {matrix_column_RED(i,0,char_13[col],char_r13[col],col);
} else if(iro[i] == CR_14 || iro[i] == CRR_14) {matrix_column_RED(i,0,char_14[col],char_r14[col],col);
} else if(iro[i] == CR_15 || iro[i] == CRR_15) {matrix_column_RED(i,0,char_15[col],char_r15[col],col);
} else if(iro[i] == CR_16 || iro[i] == CRR_16) {matrix_column_RED(i,0,char_16[col],char_r16[col],col);
} else if(iro[i] == CR_17 || iro[i] == CRR_17) {matrix_column_RED(i,0,char_17[col],char_r17[col],col);
} else if(iro[i] == CR_18 || iro[i] == CRR_18) {matrix_column_RED(i,0,char_18[col],char_r18[col],col);
} else if(iro[i] == CR_19 || iro[i] == CRR_19) {matrix_column_RED(i,0,char_19[col],char_r19[col],col);
} else if(iro[i] == CR_20 || iro[i] == CRR_20) {matrix_column_RED(i,0,char_20[col],char_r20[col],col);
} else if(iro[i] == CR_21 || iro[i] == CRR_21) {matrix_column_RED(i,0,char_21[col],char_r21[col],col);
} else if(iro[i] == CR_22 || iro[i] == CRR_22) {matrix_column_RED(i,0,char_22[col],char_r22[col],col);
} else if(iro[i] == CR_23 || iro[i] == CRR_23) {matrix_column_RED(i,0,char_23[col],char_r23[col],col);
} else if(iro[i] == CR_24 || iro[i] == CRR_24) {matrix_column_RED(i,0,char_24[col],char_r24[col],col);
} else if(iro[i] == CR_25 || iro[i] == CRR_25) {matrix_column_RED(i,0,char_25[col],char_r25[col],col);
} else if(iro[i] == CR_26 || iro[i] == CRR_26) {matrix_column_RED(i,0,char_26[col],char_r26[col],col);
} else if(iro[i] == CR_27 || iro[i] == CRR_27) {matrix_column_RED(i,0,char_27[col],char_r27[col],col);
} else if(iro[i] == CR_28 || iro[i] == CRR_28) {matrix_column_RED(i,0,char_28[col],char_r28[col],col);
} else if(iro[i] == CR_29 || iro[i] == CRR_29) {matrix_column_RED(i,0,char_29[col],char_r29[col],col);
} else if(iro[i] == CR_30 || iro[i] == CRR_30) {matrix_column_RED(i,0,char_30[col],char_r30[col],col);
} else if(iro[i] == CR_31 || iro[i] == CRR_31) {matrix_column_RED(i,0,char_31[col],char_r31[col],col);
} else if(iro[i] == CR_32 || iro[i] == CRR_32) {matrix_column_RED(i,0,char_32[col],char_r32[col],col);
} else if(iro[i] == CR_33 || iro[i] == CRR_33) {matrix_column_RED(i,0,char_33[col],char_r33[col],col);
} else if(iro[i] == CR_34 || iro[i] == CRR_34) {matrix_column_RED(i,0,char_34[col],char_r34[col],col);
} else if(iro[i] == CR_35 || iro[i] == CRR_35) {matrix_column_RED(i,0,char_35[col],char_r35[col],col);
} else if(iro[i] == CR_36 || iro[i] == CRR_36) {matrix_column_RED(i,0,char_36[col],char_r36[col],col);
} else if(iro[i] == CR_37 || iro[i] == CRR_37) {matrix_column_RED(i,0,char_37[col],char_r37[col],col);
} else if(iro[i] == CR_38 || iro[i] == CRR_38) {matrix_column_RED(i,0,char_38[col],char_r38[col],col);
} else if(iro[i] == CR_39 || iro[i] == CRR_39) {matrix_column_RED(i,0,char_39[col],char_r39[col],col);
} else if(iro[i] == CR_40 || iro[i] == CRR_40) {matrix_column_RED(i,0,char_40[col],char_r40[col],col);
} else if(iro[i] == CR_41 || iro[i] == CRR_41) {matrix_column_RED(i,0,char_41[col],char_r41[col],col);
} else if(iro[i] == CR_42 || iro[i] == CRR_42) {matrix_column_RED(i,0,char_42[col],char_r42[col],col);
} else if(iro[i] == CR_43 || iro[i] == CRR_43) {matrix_column_RED(i,0,char_43[col],char_r43[col],col);
} else if(iro[i] == CR_44 || iro[i] == CRR_44) {matrix_column_RED(i,0,char_44[col],char_r44[col],col);
} else if(iro[i] == CR_45 || iro[i] == CRR_45) {matrix_column_RED(i,0,char_45[col],char_r45[col],col);
} else if(iro[i] == CR_46 || iro[i] == CRR_46) {matrix_column_RED(i,0,char_46[col],char_r46[col],col);
} else if(iro[i] == CR_47 || iro[i] == CRR_47) {matrix_column_RED(i,0,char_47[col],char_r47[col],col);
} else if(iro[i] == CR_48 || iro[i] == CRR_48) {matrix_column_RED(i,0,char_48[col],char_r48[col],col);
} else if(iro[i] == CR_49 || iro[i] == CRR_49) {matrix_column_RED(i,0,char_49[col],char_r49[col],col);
} else if(iro[i] == CR_50 || iro[i] == CRR_50) {matrix_column_RED(i,0,char_50[col],char_r50[col],col);
} else if(iro[i] == CR_51 || iro[i] == CRR_51) {matrix_column_RED(i,0,char_51[col],char_r51[col],col);
} else if(iro[i] == CR_52 || iro[i] == CRR_52) {matrix_column_RED(i,0,char_52[col],char_r52[col],col);
} else if(iro[i] == CR_53 || iro[i] == CRR_53) {matrix_column_RED(i,0,char_53[col],char_r53[col],col);
} else if(iro[i] == CR_54 || iro[i] == CRR_54) {matrix_column_RED(i,0,char_54[col],char_r54[col],col);
} else if(iro[i] == CR_55 || iro[i] == CRR_55) {matrix_column_RED(i,0,char_55[col],char_r55[col],col);
} else if(iro[i] == CR_56 || iro[i] == CRR_56) {matrix_column_RED(i,0,char_56[col],char_r56[col],col);
} else if(iro[i] == CR_57 || iro[i] == CRR_57) {matrix_column_RED(i,0,char_57[col],char_r57[col],col);
} else if(iro[i] == CR_58 || iro[i] == CRR_58) {matrix_column_RED(i,0,char_58[col],char_r58[col],col);
} else if(iro[i] == CR_59 || iro[i] == CRR_59) {matrix_column_RED(i,0,char_59[col],char_r59[col],col);
} else if(iro[i] == CR_60 || iro[i] == CRR_60) {matrix_column_RED(i,0,char_60[col],char_r60[col],col);
} else if(iro[i] == CR_61 || iro[i] == CRR_61) {matrix_column_RED(i,0,char_61[col],char_r61[col],col);
} else if(iro[i] == CR_62 || iro[i] == CRR_62) {matrix_column_RED(i,0,char_62[col],char_r62[col],col);
} else if(iro[i] == CR_63 || iro[i] == CRR_63) {matrix_column_RED(i,0,char_63[col],char_r63[col],col);
} else if(iro[i] == CR_64 || iro[i] == CRR_64) {matrix_column_RED(i,0,char_64[col],char_r64[col],col);
} else if(iro[i] == CR_65 || iro[i] == CRR_65) {matrix_column_RED(i,0,char_65[col],char_r65[col],col);
} else if(iro[i] == CR_66 || iro[i] == CRR_66) {matrix_column_RED(i,0,char_66[col],char_r66[col],col);
} else if(iro[i] == CR_67 || iro[i] == CRR_67) {matrix_column_RED(i,0,char_67[col],char_r67[col],col);
} else if(iro[i] == CR_68 || iro[i] == CRR_68) {matrix_column_RED(i,0,char_68[col],char_r68[col],col);
} else if(iro[i] == CR_69 || iro[i] == CRR_69) {matrix_column_RED(i,0,char_69[col],char_r69[col],col);
} else if(iro[i] == CR_70 || iro[i] == CRR_70) {matrix_column_RED(i,0,char_70[col],char_r70[col],col);
} else if(iro[i] == CR_71 || iro[i] == CRR_71) {matrix_column_RED(i,0,char_71[col],char_r71[col],col);
} else if(iro[i] == CR_72 || iro[i] == CRR_72) {matrix_column_RED(i,0,char_72[col],char_r72[col],col);
} else if(iro[i] == CR_73 || iro[i] == CRR_73) {matrix_column_RED(i,0,char_73[col],char_r73[col],col);
} else if(iro[i] == CR_74 || iro[i] == CRR_74) {matrix_column_RED(i,0,char_74[col],char_r74[col],col);
} else if(iro[i] == CR_75 || iro[i] == CRR_75) {matrix_column_RED(i,0,char_75[col],char_r75[col],col);
} else if(iro[i] == CR_76 || iro[i] == CRR_76) {matrix_column_RED(i,0,char_76[col],char_r76[col],col);
} else if(iro[i] == CR_77 || iro[i] == CRR_77) {matrix_column_RED(i,0,char_77[col],char_r77[col],col);
} else if(iro[i] == CR_78 || iro[i] == CRR_78) {matrix_column_RED(i,0,char_78[col],char_r78[col],col);
} else if(iro[i] == CR_79 || iro[i] == CRR_79) {matrix_column_RED(i,0,char_79[col],char_r79[col],col);
} else if(iro[i] == CR_80 || iro[i] == CRR_80) {matrix_column_RED(i,0,char_80[col],char_r80[col],col);
} else if(iro[i] == CR_81 || iro[i] == CRR_81) {matrix_column_RED(i,0,char_81[col],char_r81[col],col);
} else if(iro[i] == CR_82 || iro[i] == CRR_82) {matrix_column_RED(i,0,char_82[col],char_r82[col],col);
} else if(iro[i] == CR_83 || iro[i] == CRR_83) {matrix_column_RED(i,0,char_83[col],char_r83[col],col);
} else if(iro[i] == CR_84 || iro[i] == CRR_84) {matrix_column_RED(i,0,char_84[col],char_r84[col],col);
} else if(iro[i] == CR_85 || iro[i] == CRR_85) {matrix_column_RED(i,0,char_85[col],char_r85[col],col);
} else if(iro[i] == CR_86 || iro[i] == CRR_86) {matrix_column_RED(i,0,char_86[col],char_r86[col],col);
} else if(iro[i] == CR_87 || iro[i] == CRR_87) {matrix_column_RED(i,0,char_87[col],char_r87[col],col);
} else if(iro[i] == CR_88 || iro[i] == CRR_88) {matrix_column_RED(i,0,char_88[col],char_r88[col],col);
} else if(iro[i] == CR_89 || iro[i] == CRR_89) {matrix_column_RED(i,0,char_89[col],char_r89[col],col);
} else if(iro[i] == CR_90 || iro[i] == CRR_90) {matrix_column_RED(i,0,char_90[col],char_r90[col],col);
} else if(iro[i] == CR_91 || iro[i] == CRR_91) {matrix_column_RED(i,0,char_91[col],char_r91[col],col);
} else if(iro[i] == CR_92 || iro[i] == CRR_92) {matrix_column_RED(i,0,char_92[col],char_r92[col],col);
} else if(iro[i] == CR_93 || iro[i] == CRR_93) {matrix_column_RED(i,0,char_93[col],char_r93[col],col);
} else if(iro[i] == CR_94 || iro[i] == CRR_94) {matrix_column_RED(i,0,char_94[col],char_r94[col],col);
} else if(iro[i] == CR_95 || iro[i] == CRR_95) {matrix_column_RED(i,0,char_95[col],char_r95[col],col);
} else if(iro[i] == CR_96 || iro[i] == CRR_96) {matrix_column_RED(i,0,char_96[col],char_r96[col],col);
} else if(iro[i] == CR_97 || iro[i] == CRR_97) {matrix_column_RED(i,0,char_97[col],char_r97[col],col);
} else if(iro[i] == CR_98 || iro[i] == CRR_98) {matrix_column_RED(i,0,char_98[col],char_r98[col],col);
} else if(iro[i] == CR_99 || iro[i] == CRR_99) {matrix_column_RED(i,0,char_99[col],char_r99[col],col);
/*
} else if(iro[i] == CR_100 ) {
     if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) { 
        cel_led[i]=( char_100[col] << 16 | char_00[col] << 8) | (1 << col);
    } else { 
        cel_led[i]=( char_r100[col] << 16 | char_r00[col] << 8) | (1 << col); 
    }
*/
} else if(iro[i] == CR_100 ) {matrix_column_RED(i,1,char_00[col],char_r00[col],col);
} else if(iro[i] == CR_101 ) {matrix_column_RED(i,1,char_01[col],char_r01[col],col);
} else if(iro[i] == CR_102 ) {matrix_column_RED(i,1,char_02[col],char_r02[col],col);
} else if(iro[i] == CR_103 ) {matrix_column_RED(i,1,char_03[col],char_r03[col],col);
} else if(iro[i] == CR_104 ) {matrix_column_RED(i,1,char_04[col],char_r04[col],col);
} else if(iro[i] == CR_105 ) {matrix_column_RED(i,1,char_05[col],char_r05[col],col);
} else if(iro[i] == CR_106 ) {matrix_column_RED(i,1,char_06[col],char_r06[col],col);
} else if(iro[i] == CR_107 ) {matrix_column_RED(i,1,char_07[col],char_r07[col],col);
} else if(iro[i] == CR_108 ) {matrix_column_RED(i,1,char_08[col],char_r08[col],col);
} else if(iro[i] == CR_109 ) {matrix_column_RED(i,1,char_09[col],char_r09[col],col);
} else if(iro[i] == CR_110 ) {matrix_column_RED(i,1,char_10[col],char_r10[col],col);
} else if(iro[i] == CR_111 ) {matrix_column_RED(i,1,char_11[col],char_r11[col],col);
} else if(iro[i] == CR_112 ) {matrix_column_RED(i,1,char_12[col],char_r12[col],col);
} else if(iro[i] == CR_113 ) {matrix_column_RED(i,1,char_13[col],char_r13[col],col);
} else if(iro[i] == CR_114 ) {matrix_column_RED(i,1,char_14[col],char_r14[col],col);
} else if(iro[i] == CR_115 ) {matrix_column_RED(i,1,char_15[col],char_r15[col],col);
} else if(iro[i] == CR_116 ) {matrix_column_RED(i,1,char_16[col],char_r16[col],col);
} else if(iro[i] == CR_117 ) {matrix_column_RED(i,1,char_17[col],char_r17[col],col);
} else if(iro[i] == CR_118 ) {matrix_column_RED(i,1,char_18[col],char_r18[col],col);
} else if(iro[i] == CR_119 ) {matrix_column_RED(i,1,char_19[col],char_r19[col],col);
} else if(iro[i] == CR_120 ) {matrix_column_RED(i,1,char_20[col],char_r20[col],col);
} else if(iro[i] == CR_121 ) {matrix_column_RED(i,1,char_21[col],char_r21[col],col);
} else if(iro[i] == CR_122 ) {matrix_column_RED(i,1,char_22[col],char_r22[col],col);
} else if(iro[i] == CR_123 ) {matrix_column_RED(i,1,char_23[col],char_r23[col],col);
} else if(iro[i] == CR_124 ) {matrix_column_RED(i,1,char_24[col],char_r24[col],col);
} else if(iro[i] == CR_125 ) {matrix_column_RED(i,1,char_25[col],char_r25[col],col);
} else if(iro[i] == CR_126 ) {matrix_column_RED(i,1,char_26[col],char_r26[col],col);
} else if(iro[i] == CR_127 ) {matrix_column_RED(i,1,char_27[col],char_r27[col],col);
} else if(iro[i] == CR_128 ) {matrix_column_RED(i,1,char_28[col],char_r28[col],col);
} else if(iro[i] == CR_129 ) {matrix_column_RED(i,1,char_29[col],char_r29[col],col);
} else if(iro[i] == CR_130 ) {matrix_column_RED(i,1,char_30[col],char_r30[col],col);
} else if(iro[i] == CR_131 ) {matrix_column_RED(i,1,char_31[col],char_r31[col],col);
} else if(iro[i] == CR_132 ) {matrix_column_RED(i,1,char_32[col],char_r32[col],col);
} else if(iro[i] == CR_133 ) {matrix_column_RED(i,1,char_33[col],char_r33[col],col);
} else if(iro[i] == CR_134 ) {matrix_column_RED(i,1,char_34[col],char_r34[col],col);
} else if(iro[i] == CR_135 ) {matrix_column_RED(i,1,char_35[col],char_r35[col],col);
} else if(iro[i] == CR_136 ) {matrix_column_RED(i,1,char_36[col],char_r36[col],col);
} else if(iro[i] == CR_137 ) {matrix_column_RED(i,1,char_37[col],char_r37[col],col);
} else if(iro[i] == CR_138 ) {matrix_column_RED(i,1,char_38[col],char_r38[col],col);
} else if(iro[i] == CR_139 ) {matrix_column_RED(i,1,char_39[col],char_r39[col],col);
} else if(iro[i] == CR_140 ) {matrix_column_RED(i,1,char_40[col],char_r40[col],col);
} else if(iro[i] == CR_141 ) {matrix_column_RED(i,1,char_41[col],char_r41[col],col);
} else if(iro[i] == CR_142 ) {matrix_column_RED(i,1,char_42[col],char_r42[col],col);
} else if(iro[i] == CR_143 ) {matrix_column_RED(i,1,char_43[col],char_r43[col],col);
} else if(iro[i] == CR_144 ) {matrix_column_RED(i,1,char_44[col],char_r44[col],col);
} else if(iro[i] == CR_145 ) {matrix_column_RED(i,1,char_45[col],char_r45[col],col);
} else if(iro[i] == CR_146 ) {matrix_column_RED(i,1,char_46[col],char_r46[col],col);
} else if(iro[i] == CR_147 ) {matrix_column_RED(i,1,char_47[col],char_r47[col],col);
} else if(iro[i] == CR_148 ) {matrix_column_RED(i,1,char_48[col],char_r48[col],col);
} else if(iro[i] == CR_149 ) {matrix_column_RED(i,1,char_49[col],char_r49[col],col);
} else if(iro[i] == CR_150 ) {matrix_column_RED(i,1,char_50[col],char_r50[col],col);
} else if(iro[i] == CR_151 ) {matrix_column_RED(i,1,char_51[col],char_r51[col],col);
} else if(iro[i] == CR_152 ) {matrix_column_RED(i,1,char_52[col],char_r52[col],col);
} else if(iro[i] == CR_153 ) {matrix_column_RED(i,1,char_53[col],char_r53[col],col);
} else if(iro[i] == CR_154 ) {matrix_column_RED(i,1,char_54[col],char_r54[col],col);
} else if(iro[i] == CR_155 ) {matrix_column_RED(i,1,char_55[col],char_r55[col],col);
} else if(iro[i] == CR_156 ) {matrix_column_RED(i,1,char_56[col],char_r56[col],col);
} else if(iro[i] == CR_157 ) {matrix_column_RED(i,1,char_57[col],char_r57[col],col);
} else if(iro[i] == CR_158 ) {matrix_column_RED(i,1,char_58[col],char_r58[col],col);
} else if(iro[i] == CR_159 ) {matrix_column_RED(i,1,char_59[col],char_r59[col],col);
} else if(iro[i] == CR_160 ) {matrix_column_RED(i,1,char_60[col],char_r60[col],col);
} else if(iro[i] == CR_161 ) {matrix_column_RED(i,1,char_61[col],char_r61[col],col);
} else if(iro[i] == CR_162 ) {matrix_column_RED(i,1,char_62[col],char_r62[col],col);
} else if(iro[i] == CR_163 ) {matrix_column_RED(i,1,char_63[col],char_r63[col],col);
} else if(iro[i] == CR_164 ) {matrix_column_RED(i,1,char_64[col],char_r64[col],col);
} else if(iro[i] == CR_165 ) {matrix_column_RED(i,1,char_65[col],char_r65[col],col);
} else if(iro[i] == CR_166 ) {matrix_column_RED(i,1,char_66[col],char_r66[col],col);
} else if(iro[i] == CR_167 ) {matrix_column_RED(i,1,char_67[col],char_r67[col],col);
} else if(iro[i] == CR_168 ) {matrix_column_RED(i,1,char_68[col],char_r68[col],col);
} else if(iro[i] == CR_169 ) {matrix_column_RED(i,1,char_69[col],char_r69[col],col);
} else if(iro[i] == CR_170 ) {matrix_column_RED(i,1,char_70[col],char_r70[col],col);
} else if(iro[i] == CR_171 ) {matrix_column_RED(i,1,char_71[col],char_r71[col],col);
} else if(iro[i] == CR_172 ) {matrix_column_RED(i,1,char_72[col],char_r72[col],col);
} else if(iro[i] == CR_173 ) {matrix_column_RED(i,1,char_73[col],char_r73[col],col);
} else if(iro[i] == CR_174 ) {matrix_column_RED(i,1,char_74[col],char_r74[col],col);
} else if(iro[i] == CR_175 ) {matrix_column_RED(i,1,char_75[col],char_r75[col],col);
} else if(iro[i] == CR_176 ) {matrix_column_RED(i,1,char_76[col],char_r76[col],col);
} else if(iro[i] == CR_177 ) {matrix_column_RED(i,1,char_77[col],char_r77[col],col);
} else if(iro[i] == CR_178 ) {matrix_column_RED(i,1,char_78[col],char_r78[col],col);
} else if(iro[i] == CR_179 ) {matrix_column_RED(i,1,char_79[col],char_r79[col],col);
} else if(iro[i] == CR_180 ) {matrix_column_RED(i,1,char_80[col],char_r80[col],col);
} else if(iro[i] == CR_181 ) {matrix_column_RED(i,1,char_81[col],char_r81[col],col);
} else if(iro[i] == CR_182 ) {matrix_column_RED(i,1,char_82[col],char_r82[col],col);
} else if(iro[i] == CR_183 ) {matrix_column_RED(i,1,char_83[col],char_r83[col],col);
} else if(iro[i] == CR_184 ) {matrix_column_RED(i,1,char_84[col],char_r84[col],col);
} else if(iro[i] == CR_185 ) {matrix_column_RED(i,1,char_85[col],char_r85[col],col);
} else if(iro[i] == CR_186 ) {matrix_column_RED(i,1,char_86[col],char_r86[col],col);
} else if(iro[i] == CR_187 ) {matrix_column_RED(i,1,char_87[col],char_r87[col],col);
} else if(iro[i] == CR_188 ) {matrix_column_RED(i,1,char_88[col],char_r88[col],col);
} else if(iro[i] == CR_189 ) {matrix_column_RED(i,1,char_89[col],char_r89[col],col);
} else if(iro[i] == CR_190 ) {matrix_column_RED(i,1,char_90[col],char_r90[col],col);
} else if(iro[i] == CR_191 ) {matrix_column_RED(i,1,char_91[col],char_r91[col],col);
} else if(iro[i] == CR_192 ) {matrix_column_RED(i,1,char_92[col],char_r92[col],col);
} else if(iro[i] == CR_193 ) {matrix_column_RED(i,1,char_93[col],char_r93[col],col);
} else if(iro[i] == CR_194 ) {matrix_column_RED(i,1,char_94[col],char_r94[col],col);
} else if(iro[i] == CR_195 ) {matrix_column_RED(i,1,char_95[col],char_r95[col],col);
} else if(iro[i] == CR_196 ) {matrix_column_RED(i,1,char_96[col],char_r96[col],col);
} else if(iro[i] == CR_197 ) {matrix_column_RED(i,1,char_97[col],char_r97[col],col);
} else if(iro[i] == CR_198 ) {matrix_column_RED(i,1,char_98[col],char_r98[col],col);
} else if(iro[i] == CR_199 ) {matrix_column_RED(i,1,char_99[col],char_r99[col],col);

} else if(iro[i] == CR_200 ) {matrix_column_RED(i,2,char_00[col],char_r00[col],col);
} else if(iro[i] == CR_201 ) {matrix_column_RED(i,2,char_01[col],char_r01[col],col);
} else if(iro[i] == CR_202 ) {matrix_column_RED(i,2,char_02[col],char_r02[col],col);
} else if(iro[i] == CR_203 ) {matrix_column_RED(i,2,char_03[col],char_r03[col],col);
} else if(iro[i] == CR_204 ) {matrix_column_RED(i,2,char_04[col],char_r04[col],col);
} else if(iro[i] == CR_205 ) {matrix_column_RED(i,2,char_05[col],char_r05[col],col);
} else if(iro[i] == CR_206 ) {matrix_column_RED(i,2,char_06[col],char_r06[col],col);
} else if(iro[i] == CR_207 ) {matrix_column_RED(i,2,char_07[col],char_r07[col],col);
} else if(iro[i] == CR_208 ) {matrix_column_RED(i,2,char_08[col],char_r08[col],col);
} else if(iro[i] == CR_209 ) {matrix_column_RED(i,2,char_09[col],char_r09[col],col);
} else if(iro[i] == CR_210 ) {matrix_column_RED(i,2,char_10[col],char_r10[col],col);
} else if(iro[i] == CR_211 ) {matrix_column_RED(i,2,char_11[col],char_r11[col],col);
} else if(iro[i] == CR_212 ) {matrix_column_RED(i,2,char_12[col],char_r12[col],col);
} else if(iro[i] == CR_213 ) {matrix_column_RED(i,2,char_13[col],char_r13[col],col);
} else if(iro[i] == CR_214 ) {matrix_column_RED(i,2,char_14[col],char_r14[col],col);
} else if(iro[i] == CR_215 ) {matrix_column_RED(i,2,char_15[col],char_r15[col],col);
} else if(iro[i] == CR_216 ) {matrix_column_RED(i,2,char_16[col],char_r16[col],col);
} else if(iro[i] == CR_217 ) {matrix_column_RED(i,2,char_17[col],char_r17[col],col);
} else if(iro[i] == CR_218 ) {matrix_column_RED(i,2,char_18[col],char_r18[col],col);
} else if(iro[i] == CR_219 ) {matrix_column_RED(i,2,char_19[col],char_r19[col],col);
} else if(iro[i] == CR_220 ) {matrix_column_RED(i,2,char_20[col],char_r20[col],col);
} else if(iro[i] == CR_221 ) {matrix_column_RED(i,2,char_21[col],char_r21[col],col);
} else if(iro[i] == CR_222 ) {matrix_column_RED(i,2,char_22[col],char_r22[col],col);
} else if(iro[i] == CR_223 ) {matrix_column_RED(i,2,char_23[col],char_r23[col],col);
} else if(iro[i] == CR_224 ) {matrix_column_RED(i,2,char_24[col],char_r24[col],col);
} else if(iro[i] == CR_225 ) {matrix_column_RED(i,2,char_25[col],char_r25[col],col);
} else if(iro[i] == CR_226 ) {matrix_column_RED(i,2,char_26[col],char_r26[col],col);
} else if(iro[i] == CR_227 ) {matrix_column_RED(i,2,char_27[col],char_r27[col],col);
} else if(iro[i] == CR_228 ) {matrix_column_RED(i,2,char_28[col],char_r28[col],col);
} else if(iro[i] == CR_229 ) {matrix_column_RED(i,2,char_29[col],char_r29[col],col);
} else if(iro[i] == CR_230 ) {matrix_column_RED(i,2,char_30[col],char_r30[col],col);
} else if(iro[i] == CR_231 ) {matrix_column_RED(i,2,char_31[col],char_r31[col],col);
} else if(iro[i] == CR_232 ) {matrix_column_RED(i,2,char_32[col],char_r32[col],col);
} else if(iro[i] == CR_233 ) {matrix_column_RED(i,2,char_33[col],char_r33[col],col);
} else if(iro[i] == CR_234 ) {matrix_column_RED(i,2,char_34[col],char_r34[col],col);
} else if(iro[i] == CR_235 ) {matrix_column_RED(i,2,char_35[col],char_r35[col],col);
} else if(iro[i] == CR_236 ) {matrix_column_RED(i,2,char_36[col],char_r36[col],col);
} else if(iro[i] == CR_237 ) {matrix_column_RED(i,2,char_37[col],char_r37[col],col);
} else if(iro[i] == CR_238 ) {matrix_column_RED(i,2,char_38[col],char_r38[col],col);
} else if(iro[i] == CR_239 ) {matrix_column_RED(i,2,char_39[col],char_r39[col],col);
} else if(iro[i] == CR_240 ) {matrix_column_RED(i,2,char_40[col],char_r40[col],col);
} else if(iro[i] == CR_241 ) {matrix_column_RED(i,2,char_41[col],char_r41[col],col);
} else if(iro[i] == CR_242 ) {matrix_column_RED(i,2,char_42[col],char_r42[col],col);
} else if(iro[i] == CR_243 ) {matrix_column_RED(i,2,char_43[col],char_r43[col],col);
} else if(iro[i] == CR_244 ) {matrix_column_RED(i,2,char_44[col],char_r44[col],col);
} else if(iro[i] == CR_245 ) {matrix_column_RED(i,2,char_45[col],char_r45[col],col);
} else if(iro[i] == CR_246 ) {matrix_column_RED(i,2,char_46[col],char_r46[col],col);
} else if(iro[i] == CR_247 ) {matrix_column_RED(i,2,char_47[col],char_r47[col],col);
} else if(iro[i] == CR_248 ) {matrix_column_RED(i,2,char_48[col],char_r48[col],col);
} else if(iro[i] == CR_249 ) {matrix_column_RED(i,2,char_49[col],char_r49[col],col);
} else if(iro[i] == CR_250 ) {matrix_column_RED(i,2,char_50[col],char_r50[col],col);
} else if(iro[i] == CR_251 ) {matrix_column_RED(i,2,char_51[col],char_r51[col],col);
} else if(iro[i] == CR_252 ) {matrix_column_RED(i,2,char_52[col],char_r52[col],col);
} else if(iro[i] == CR_253 ) {matrix_column_RED(i,2,char_53[col],char_r53[col],col);
} else if(iro[i] == CR_254 ) {matrix_column_RED(i,2,char_54[col],char_r54[col],col);
} else if(iro[i] == CR_255 ) {matrix_column_RED(i,2,char_55[col],char_r55[col],col);
} else if(iro[i] == CR_256 ) {matrix_column_RED(i,2,char_56[col],char_r56[col],col);
} else if(iro[i] == CR_257 ) {matrix_column_RED(i,2,char_57[col],char_r57[col],col);
} else if(iro[i] == CR_258 ) {matrix_column_RED(i,2,char_58[col],char_r58[col],col);
} else if(iro[i] == CR_259 ) {matrix_column_RED(i,2,char_59[col],char_r59[col],col);
} else if(iro[i] == CR_260 ) {matrix_column_RED(i,2,char_60[col],char_r60[col],col);
} else if(iro[i] == CR_261 ) {matrix_column_RED(i,2,char_61[col],char_r61[col],col);
} else if(iro[i] == CR_262 ) {matrix_column_RED(i,2,char_62[col],char_r62[col],col);
} else if(iro[i] == CR_263 ) {matrix_column_RED(i,2,char_63[col],char_r63[col],col);
} else if(iro[i] == CR_264 ) {matrix_column_RED(i,2,char_64[col],char_r64[col],col);
} else if(iro[i] == CR_265 ) {matrix_column_RED(i,2,char_65[col],char_r65[col],col);
} else if(iro[i] == CR_266 ) {matrix_column_RED(i,2,char_66[col],char_r66[col],col);
} else if(iro[i] == CR_267 ) {matrix_column_RED(i,2,char_67[col],char_r67[col],col);
} else if(iro[i] == CR_268 ) {matrix_column_RED(i,2,char_68[col],char_r68[col],col);
} else if(iro[i] == CR_269 ) {matrix_column_RED(i,2,char_69[col],char_r69[col],col);
} else if(iro[i] == CR_270 ) {matrix_column_RED(i,2,char_70[col],char_r70[col],col);
} else if(iro[i] == CR_271 ) {matrix_column_RED(i,2,char_71[col],char_r71[col],col);
} else if(iro[i] == CR_272 ) {matrix_column_RED(i,2,char_72[col],char_r72[col],col);
} else if(iro[i] == CR_273 ) {matrix_column_RED(i,2,char_73[col],char_r73[col],col);
} else if(iro[i] == CR_274 ) {matrix_column_RED(i,2,char_74[col],char_r74[col],col);
} else if(iro[i] == CR_275 ) {matrix_column_RED(i,2,char_75[col],char_r75[col],col);
} else if(iro[i] == CR_276 ) {matrix_column_RED(i,2,char_76[col],char_r76[col],col);
} else if(iro[i] == CR_277 ) {matrix_column_RED(i,2,char_77[col],char_r77[col],col);
} else if(iro[i] == CR_278 ) {matrix_column_RED(i,2,char_78[col],char_r78[col],col);
} else if(iro[i] == CR_279 ) {matrix_column_RED(i,2,char_79[col],char_r79[col],col);
} else if(iro[i] == CR_280 ) {matrix_column_RED(i,2,char_80[col],char_r80[col],col);
} else if(iro[i] == CR_281 ) {matrix_column_RED(i,2,char_81[col],char_r81[col],col);
} else if(iro[i] == CR_282 ) {matrix_column_RED(i,2,char_82[col],char_r82[col],col);
} else if(iro[i] == CR_283 ) {matrix_column_RED(i,2,char_83[col],char_r83[col],col);
} else if(iro[i] == CR_284 ) {matrix_column_RED(i,2,char_84[col],char_r84[col],col);
} else if(iro[i] == CR_285 ) {matrix_column_RED(i,2,char_85[col],char_r85[col],col);
} else if(iro[i] == CR_286 ) {matrix_column_RED(i,2,char_86[col],char_r86[col],col);
} else if(iro[i] == CR_287 ) {matrix_column_RED(i,2,char_87[col],char_r87[col],col);
} else if(iro[i] == CR_288 ) {matrix_column_RED(i,2,char_88[col],char_r88[col],col);
} else if(iro[i] == CR_289 ) {matrix_column_RED(i,2,char_89[col],char_r89[col],col);
} else if(iro[i] == CR_290 ) {matrix_column_RED(i,2,char_90[col],char_r90[col],col);
} else if(iro[i] == CR_291 ) {matrix_column_RED(i,2,char_91[col],char_r91[col],col);
} else if(iro[i] == CR_292 ) {matrix_column_RED(i,2,char_92[col],char_r92[col],col);
} else if(iro[i] == CR_293 ) {matrix_column_RED(i,2,char_93[col],char_r93[col],col);
} else if(iro[i] == CR_294 ) {matrix_column_RED(i,2,char_94[col],char_r94[col],col);
} else if(iro[i] == CR_295 ) {matrix_column_RED(i,2,char_95[col],char_r95[col],col);
} else if(iro[i] == CR_296 ) {matrix_column_RED(i,2,char_96[col],char_r96[col],col);
} else if(iro[i] == CR_297 ) {matrix_column_RED(i,2,char_97[col],char_r97[col],col);
} else if(iro[i] == CR_298 ) {matrix_column_RED(i,2,char_98[col],char_r98[col],col);
} else if(iro[i] == CR_299 ) {matrix_column_RED(i,2,char_99[col],char_r99[col],col);

} else if(iro[i] == CR_300 ) {matrix_column_RED(i,3,char_00[col],char_r00[col],col);
} else if(iro[i] == CR_301 ) {matrix_column_RED(i,3,char_01[col],char_r01[col],col);
} else if(iro[i] == CR_302 ) {matrix_column_RED(i,3,char_02[col],char_r02[col],col);
} else if(iro[i] == CR_303 ) {matrix_column_RED(i,3,char_03[col],char_r03[col],col);
} else if(iro[i] == CR_304 ) {matrix_column_RED(i,3,char_04[col],char_r04[col],col);
} else if(iro[i] == CR_305 ) {matrix_column_RED(i,3,char_05[col],char_r05[col],col);
} else if(iro[i] == CR_306 ) {matrix_column_RED(i,3,char_06[col],char_r06[col],col);
} else if(iro[i] == CR_307 ) {matrix_column_RED(i,3,char_07[col],char_r07[col],col);
} else if(iro[i] == CR_308 ) {matrix_column_RED(i,3,char_08[col],char_r08[col],col);
} else if(iro[i] == CR_309 ) {matrix_column_RED(i,3,char_09[col],char_r09[col],col);
} else if(iro[i] == CR_310 ) {matrix_column_RED(i,3,char_10[col],char_r10[col],col);
} else if(iro[i] == CR_311 ) {matrix_column_RED(i,3,char_11[col],char_r11[col],col);
} else if(iro[i] == CR_312 ) {matrix_column_RED(i,3,char_12[col],char_r12[col],col);
} else if(iro[i] == CR_313 ) {matrix_column_RED(i,3,char_13[col],char_r13[col],col);
} else if(iro[i] == CR_314 ) {matrix_column_RED(i,3,char_14[col],char_r14[col],col);
} else if(iro[i] == CR_315 ) {matrix_column_RED(i,3,char_15[col],char_r15[col],col);
} else if(iro[i] == CR_316 ) {matrix_column_RED(i,3,char_16[col],char_r16[col],col);
} else if(iro[i] == CR_317 ) {matrix_column_RED(i,3,char_17[col],char_r17[col],col);
} else if(iro[i] == CR_318 ) {matrix_column_RED(i,3,char_18[col],char_r18[col],col);
} else if(iro[i] == CR_319 ) {matrix_column_RED(i,3,char_19[col],char_r19[col],col);
} else if(iro[i] == CR_320 ) {matrix_column_RED(i,3,char_20[col],char_r20[col],col);
} else if(iro[i] == CR_321 ) {matrix_column_RED(i,3,char_21[col],char_r21[col],col);
} else if(iro[i] == CR_322 ) {matrix_column_RED(i,3,char_22[col],char_r22[col],col);
} else if(iro[i] == CR_323 ) {matrix_column_RED(i,3,char_23[col],char_r23[col],col);
} else if(iro[i] == CR_324 ) {matrix_column_RED(i,3,char_24[col],char_r24[col],col);
} else if(iro[i] == CR_325 ) {matrix_column_RED(i,3,char_25[col],char_r25[col],col);
} else if(iro[i] == CR_326 ) {matrix_column_RED(i,3,char_26[col],char_r26[col],col);
} else if(iro[i] == CR_327 ) {matrix_column_RED(i,3,char_27[col],char_r27[col],col);
} else if(iro[i] == CR_328 ) {matrix_column_RED(i,3,char_28[col],char_r28[col],col);
} else if(iro[i] == CR_329 ) {matrix_column_RED(i,3,char_29[col],char_r29[col],col);
} else if(iro[i] == CR_330 ) {matrix_column_RED(i,3,char_30[col],char_r30[col],col);
} else if(iro[i] == CR_331 ) {matrix_column_RED(i,3,char_31[col],char_r31[col],col);
} else if(iro[i] == CR_332 ) {matrix_column_RED(i,3,char_32[col],char_r32[col],col);
} else if(iro[i] == CR_333 ) {matrix_column_RED(i,3,char_33[col],char_r33[col],col);
} else if(iro[i] == CR_334 ) {matrix_column_RED(i,3,char_34[col],char_r34[col],col);
} else if(iro[i] == CR_335 ) {matrix_column_RED(i,3,char_35[col],char_r35[col],col);
} else if(iro[i] == CR_336 ) {matrix_column_RED(i,3,char_36[col],char_r36[col],col);
} else if(iro[i] == CR_337 ) {matrix_column_RED(i,3,char_37[col],char_r37[col],col);
} else if(iro[i] == CR_338 ) {matrix_column_RED(i,3,char_38[col],char_r38[col],col);
} else if(iro[i] == CR_339 ) {matrix_column_RED(i,3,char_39[col],char_r39[col],col);
} else if(iro[i] == CR_340 ) {matrix_column_RED(i,3,char_40[col],char_r40[col],col);
} else if(iro[i] == CR_341 ) {matrix_column_RED(i,3,char_41[col],char_r41[col],col);
} else if(iro[i] == CR_342 ) {matrix_column_RED(i,3,char_42[col],char_r42[col],col);
} else if(iro[i] == CR_343 ) {matrix_column_RED(i,3,char_43[col],char_r43[col],col);
} else if(iro[i] == CR_344 ) {matrix_column_RED(i,3,char_44[col],char_r44[col],col);
} else if(iro[i] == CR_345 ) {matrix_column_RED(i,3,char_45[col],char_r45[col],col);
} else if(iro[i] == CR_346 ) {matrix_column_RED(i,3,char_46[col],char_r46[col],col);
} else if(iro[i] == CR_347 ) {matrix_column_RED(i,3,char_47[col],char_r47[col],col);
} else if(iro[i] == CR_348 ) {matrix_column_RED(i,3,char_48[col],char_r48[col],col);
} else if(iro[i] == CR_349 ) {matrix_column_RED(i,3,char_49[col],char_r49[col],col);
} else if(iro[i] == CR_350 ) {matrix_column_RED(i,3,char_50[col],char_r50[col],col);
} else if(iro[i] == CR_351 ) {matrix_column_RED(i,3,char_51[col],char_r51[col],col);
} else if(iro[i] == CR_352 ) {matrix_column_RED(i,3,char_52[col],char_r52[col],col);
} else if(iro[i] == CR_353 ) {matrix_column_RED(i,3,char_53[col],char_r53[col],col);
} else if(iro[i] == CR_354 ) {matrix_column_RED(i,3,char_54[col],char_r54[col],col);
} else if(iro[i] == CR_355 ) {matrix_column_RED(i,3,char_55[col],char_r55[col],col);
} else if(iro[i] == CR_356 ) {matrix_column_RED(i,3,char_56[col],char_r56[col],col);
} else if(iro[i] == CR_357 ) {matrix_column_RED(i,3,char_57[col],char_r57[col],col);
} else if(iro[i] == CR_358 ) {matrix_column_RED(i,3,char_58[col],char_r58[col],col);
} else if(iro[i] == CR_359 ) {matrix_column_RED(i,3,char_59[col],char_r59[col],col);
} else if(iro[i] == CR_360 ) {matrix_column_RED(i,3,char_60[col],char_r60[col],col);
} else if(iro[i] == CR_361 ) {matrix_column_RED(i,3,char_61[col],char_r61[col],col);
} else if(iro[i] == CR_362 ) {matrix_column_RED(i,3,char_62[col],char_r62[col],col);
} else if(iro[i] == CR_363 ) {matrix_column_RED(i,3,char_63[col],char_r63[col],col);
} else if(iro[i] == CR_364 ) {matrix_column_RED(i,3,char_64[col],char_r64[col],col);
} else if(iro[i] == CR_365 ) {matrix_column_RED(i,3,char_65[col],char_r65[col],col);
} else if(iro[i] == CR_366 ) {matrix_column_RED(i,3,char_66[col],char_r66[col],col);
} else if(iro[i] == CR_367 ) {matrix_column_RED(i,3,char_67[col],char_r67[col],col);
} else if(iro[i] == CR_368 ) {matrix_column_RED(i,3,char_68[col],char_r68[col],col);
} else if(iro[i] == CR_369 ) {matrix_column_RED(i,3,char_69[col],char_r69[col],col);
} else if(iro[i] == CR_370 ) {matrix_column_RED(i,3,char_70[col],char_r70[col],col);
} else if(iro[i] == CR_371 ) {matrix_column_RED(i,3,char_71[col],char_r71[col],col);
} else if(iro[i] == CR_372 ) {matrix_column_RED(i,3,char_72[col],char_r72[col],col);
} else if(iro[i] == CR_373 ) {matrix_column_RED(i,3,char_73[col],char_r73[col],col);
} else if(iro[i] == CR_374 ) {matrix_column_RED(i,3,char_74[col],char_r74[col],col);
} else if(iro[i] == CR_375 ) {matrix_column_RED(i,3,char_75[col],char_r75[col],col);
} else if(iro[i] == CR_376 ) {matrix_column_RED(i,3,char_76[col],char_r76[col],col);
} else if(iro[i] == CR_377 ) {matrix_column_RED(i,3,char_77[col],char_r77[col],col);
} else if(iro[i] == CR_378 ) {matrix_column_RED(i,3,char_78[col],char_r78[col],col);
} else if(iro[i] == CR_379 ) {matrix_column_RED(i,3,char_79[col],char_r79[col],col);
} else if(iro[i] == CR_380 ) {matrix_column_RED(i,3,char_80[col],char_r80[col],col);
} else if(iro[i] == CR_381 ) {matrix_column_RED(i,3,char_81[col],char_r81[col],col);
} else if(iro[i] == CR_382 ) {matrix_column_RED(i,3,char_82[col],char_r82[col],col);
} else if(iro[i] == CR_383 ) {matrix_column_RED(i,3,char_83[col],char_r83[col],col);
} else if(iro[i] == CR_384 ) {matrix_column_RED(i,3,char_84[col],char_r84[col],col);
} else if(iro[i] == CR_385 ) {matrix_column_RED(i,3,char_85[col],char_r85[col],col);
} else if(iro[i] == CR_386 ) {matrix_column_RED(i,3,char_86[col],char_r86[col],col);
} else if(iro[i] == CR_387 ) {matrix_column_RED(i,3,char_87[col],char_r87[col],col);
} else if(iro[i] == CR_388 ) {matrix_column_RED(i,3,char_88[col],char_r88[col],col);
} else if(iro[i] == CR_389 ) {matrix_column_RED(i,3,char_89[col],char_r89[col],col);
} else if(iro[i] == CR_390 ) {matrix_column_RED(i,3,char_90[col],char_r90[col],col);
} else if(iro[i] == CR_391 ) {matrix_column_RED(i,3,char_91[col],char_r91[col],col);
} else if(iro[i] == CR_392 ) {matrix_column_RED(i,3,char_92[col],char_r92[col],col);
} else if(iro[i] == CR_393 ) {matrix_column_RED(i,3,char_93[col],char_r93[col],col);
} else if(iro[i] == CR_394 ) {matrix_column_RED(i,3,char_94[col],char_r94[col],col);
} else if(iro[i] == CR_395 ) {matrix_column_RED(i,3,char_95[col],char_r95[col],col);
} else if(iro[i] == CR_396 ) {matrix_column_RED(i,3,char_96[col],char_r96[col],col);
} else if(iro[i] == CR_397 ) {matrix_column_RED(i,3,char_97[col],char_r97[col],col);
} else if(iro[i] == CR_398 ) {matrix_column_RED(i,3,char_98[col],char_r98[col],col);
} else if(iro[i] == CR_399 ) {matrix_column_RED(i,3,char_99[col],char_r99[col],col);


//} else if(iro[i] == CG_00) {matrix_column_GRE(i,char_00[col],char_r00[col],col);
} else if(iro[i] == CG_01)   {matrix_column_GRE(i,char_01[col],char_r01[col],col);
} else if(iro[i] == CG_02)   {matrix_column_GRE(i,char_02[col],char_r02[col],col);
} else if(iro[i] == CG_03)   {matrix_column_GRE(i,char_03[col],char_r03[col],col);
} else if(iro[i] == CG_04)   {matrix_column_GRE(i,char_04[col],char_r04[col],col);
} else if(iro[i] == CG_05)   {matrix_column_GRE(i,char_05[col],char_r05[col],col);
} else if(iro[i] == CG_06)   {matrix_column_GRE(i,char_06[col],char_r06[col],col);
} else if(iro[i] == CG_07)   {matrix_column_GRE(i,char_07[col],char_r07[col],col);
} else if(iro[i] == CG_08)   {matrix_column_GRE(i,char_08[col],char_r08[col],col);
} else if(iro[i] == CG_09)   {matrix_column_GRE(i,char_09[col],char_r09[col],col);
} else if(iro[i] == CG_10)   {matrix_column_GRE(i,char_10[col],char_r10[col],col);
} else if(iro[i] == CG_11)   {matrix_column_GRE(i,char_11[col],char_r11[col],col);
} else if(iro[i] == CG_12)   {matrix_column_GRE(i,char_12[col],char_r12[col],col);
} else if(iro[i] == CG_13)   {matrix_column_GRE(i,char_13[col],char_r13[col],col);
} else if(iro[i] == CG_14)   {matrix_column_GRE(i,char_14[col],char_r14[col],col);
} else if(iro[i] == CG_15)   {matrix_column_GRE(i,char_15[col],char_r15[col],col);
} else if(iro[i] == CG_16)   {matrix_column_GRE(i,char_16[col],char_r16[col],col);
} else if(iro[i] == CG_17)   {matrix_column_GRE(i,char_17[col],char_r17[col],col);
} else if(iro[i] == CG_18)   {matrix_column_GRE(i,char_18[col],char_r18[col],col);
} else if(iro[i] == CG_19)   {matrix_column_GRE(i,char_19[col],char_r19[col],col);
} else if(iro[i] == CG_20)   {matrix_column_GRE(i,char_20[col],char_r20[col],col);
} else if(iro[i] == CG_21)   {matrix_column_GRE(i,char_21[col],char_r21[col],col);
} else if(iro[i] == CG_22)   {matrix_column_GRE(i,char_22[col],char_r22[col],col);
} else if(iro[i] == CG_23)   {matrix_column_GRE(i,char_23[col],char_r23[col],col);
} else if(iro[i] == CG_24)   {matrix_column_GRE(i,char_24[col],char_r24[col],col);
} else if(iro[i] == CG_25)   {matrix_column_GRE(i,char_25[col],char_r25[col],col);
} else if(iro[i] == CG_26)   {matrix_column_GRE(i,char_26[col],char_r26[col],col);
} else if(iro[i] == CG_27)   {matrix_column_GRE(i,char_27[col],char_r27[col],col);
} else if(iro[i] == CG_28)   {matrix_column_GRE(i,char_28[col],char_r28[col],col);
} else if(iro[i] == CG_29)   {matrix_column_GRE(i,char_29[col],char_r29[col],col);
} else if(iro[i] == CG_30)   {matrix_column_GRE(i,char_30[col],char_r30[col],col);
} else if(iro[i] == CG_31)   {matrix_column_GRE(i,char_31[col],char_r31[col],col);
} else if(iro[i] == CG_32)   {matrix_column_GRE(i,char_32[col],char_r32[col],col);
} else if(iro[i] == CG_33)   {matrix_column_GRE(i,char_33[col],char_r33[col],col);
} else if(iro[i] == CG_34)   {matrix_column_GRE(i,char_34[col],char_r34[col],col);
} else if(iro[i] == CG_35)   {matrix_column_GRE(i,char_35[col],char_r35[col],col);
} else if(iro[i] == CG_36)   {matrix_column_GRE(i,char_36[col],char_r36[col],col);
} else if(iro[i] == CG_37)   {matrix_column_GRE(i,char_37[col],char_r37[col],col);
} else if(iro[i] == CG_38)   {matrix_column_GRE(i,char_38[col],char_r38[col],col);
} else if(iro[i] == CG_39)   {matrix_column_GRE(i,char_39[col],char_r39[col],col);
} else if(iro[i] == CG_40)   {matrix_column_GRE(i,char_40[col],char_r40[col],col);
} else if(iro[i] == CG_41)   {matrix_column_GRE(i,char_41[col],char_r41[col],col);
} else if(iro[i] == CG_42)   {matrix_column_GRE(i,char_42[col],char_r42[col],col);
} else if(iro[i] == CG_43)   {matrix_column_GRE(i,char_43[col],char_r43[col],col);
} else if(iro[i] == CG_44)   {matrix_column_GRE(i,char_44[col],char_r44[col],col);
} else if(iro[i] == CG_45)   {matrix_column_GRE(i,char_45[col],char_r45[col],col);
} else if(iro[i] == CG_46)   {matrix_column_GRE(i,char_46[col],char_r46[col],col);
} else if(iro[i] == CG_47)   {matrix_column_GRE(i,char_47[col],char_r47[col],col);
} else if(iro[i] == CG_48)   {matrix_column_GRE(i,char_48[col],char_r48[col],col);
} else if(iro[i] == CG_49)   {matrix_column_GRE(i,char_49[col],char_r49[col],col);
} else if(iro[i] == CG_50)   {matrix_column_GRE(i,char_50[col],char_r50[col],col);
} else if(iro[i] == CG_51)   {matrix_column_GRE(i,char_51[col],char_r51[col],col);
} else if(iro[i] == CG_52)   {matrix_column_GRE(i,char_52[col],char_r52[col],col);
} else if(iro[i] == CG_53)   {matrix_column_GRE(i,char_53[col],char_r53[col],col);
} else if(iro[i] == CG_54)   {matrix_column_GRE(i,char_54[col],char_r54[col],col);
} else if(iro[i] == CG_55)   {matrix_column_GRE(i,char_55[col],char_r55[col],col);
} else if(iro[i] == CG_56)   {matrix_column_GRE(i,char_56[col],char_r56[col],col);
} else if(iro[i] == CG_57)   {matrix_column_GRE(i,char_57[col],char_r57[col],col);
} else if(iro[i] == CG_58)   {matrix_column_GRE(i,char_58[col],char_r58[col],col);
} else if(iro[i] == CG_59)   {matrix_column_GRE(i,char_59[col],char_r59[col],col);
} else if(iro[i] == CG_60)   {matrix_column_GRE(i,char_60[col],char_r60[col],col);
} else if(iro[i] == CG_61)   {matrix_column_GRE(i,char_61[col],char_r61[col],col);
} else if(iro[i] == CG_62)   {matrix_column_GRE(i,char_62[col],char_r62[col],col);
} else if(iro[i] == CG_63)   {matrix_column_GRE(i,char_63[col],char_r63[col],col);
} else if(iro[i] == CG_64)   {matrix_column_GRE(i,char_64[col],char_r64[col],col);
} else if(iro[i] == CG_65)   {matrix_column_GRE(i,char_65[col],char_r65[col],col);
} else if(iro[i] == CG_66)   {matrix_column_GRE(i,char_66[col],char_r66[col],col);
} else if(iro[i] == CG_67)   {matrix_column_GRE(i,char_67[col],char_r67[col],col);
} else if(iro[i] == CG_68)   {matrix_column_GRE(i,char_68[col],char_r68[col],col);
} else if(iro[i] == CG_69)   {matrix_column_GRE(i,char_69[col],char_r69[col],col);
} else if(iro[i] == CG_70)   {matrix_column_GRE(i,char_70[col],char_r70[col],col);
} else if(iro[i] == CG_71)   {matrix_column_GRE(i,char_71[col],char_r71[col],col);
} else if(iro[i] == CG_72)   {matrix_column_GRE(i,char_72[col],char_r72[col],col);
} else if(iro[i] == CG_73)   {matrix_column_GRE(i,char_73[col],char_r73[col],col);
} else if(iro[i] == CG_74)   {matrix_column_GRE(i,char_74[col],char_r74[col],col);
} else if(iro[i] == CG_75)   {matrix_column_GRE(i,char_75[col],char_r75[col],col);
} else if(iro[i] == CG_76)   {matrix_column_GRE(i,char_76[col],char_r76[col],col);
} else if(iro[i] == CG_77)   {matrix_column_GRE(i,char_77[col],char_r77[col],col);
} else if(iro[i] == CG_78)   {matrix_column_GRE(i,char_78[col],char_r78[col],col);
} else if(iro[i] == CG_79)   {matrix_column_GRE(i,char_79[col],char_r79[col],col);
} else if(iro[i] == CG_80)   {matrix_column_GRE(i,char_80[col],char_r80[col],col);
} else if(iro[i] == CG_81)   {matrix_column_GRE(i,char_81[col],char_r81[col],col);
} else if(iro[i] == CG_82)   {matrix_column_GRE(i,char_82[col],char_r82[col],col);
} else if(iro[i] == CG_83)   {matrix_column_GRE(i,char_83[col],char_r83[col],col);
} else if(iro[i] == CG_84)   {matrix_column_GRE(i,char_84[col],char_r84[col],col);
} else if(iro[i] == CG_85)   {matrix_column_GRE(i,char_85[col],char_r85[col],col);
} else if(iro[i] == CG_86)   {matrix_column_GRE(i,char_86[col],char_r86[col],col);
} else if(iro[i] == CG_87)   {matrix_column_GRE(i,char_87[col],char_r87[col],col);
} else if(iro[i] == CG_88)   {matrix_column_GRE(i,char_88[col],char_r88[col],col);
} else if(iro[i] == CG_89)   {matrix_column_GRE(i,char_89[col],char_r89[col],col);
} else if(iro[i] == CG_90)   {matrix_column_GRE(i,char_90[col],char_r90[col],col);
} else if(iro[i] == CG_91)   {matrix_column_GRE(i,char_91[col],char_r91[col],col);
} else if(iro[i] == CG_92)   {matrix_column_GRE(i,char_92[col],char_r92[col],col);
} else if(iro[i] == CG_93)   {matrix_column_GRE(i,char_93[col],char_r93[col],col);
} else if(iro[i] == CG_94)   {matrix_column_GRE(i,char_94[col],char_r94[col],col);
} else if(iro[i] == CG_95)   {matrix_column_GRE(i,char_95[col],char_r95[col],col);
} else if(iro[i] == CG_96)   {matrix_column_GRE(i,char_96[col],char_r96[col],col);
} else if(iro[i] == CG_97)   {matrix_column_GRE(i,char_97[col],char_r97[col],col);
} else if(iro[i] == CG_98)   {matrix_column_GRE(i,char_98[col],char_r98[col],col);
} else if(iro[i] == CG_99)   {matrix_column_GRE(i,char_99[col],char_r99[col],col);


        } else if (iro[i] == CR_M0) {matrix_column_RED(i,0,char_m0[col],char_rm0[col],col);
        } else if (iro[i] == CR_M1) {matrix_column_RED(i,0,char_m1[col],char_rm1[col],col);
        } else if (iro[i] == CR_M2) {matrix_column_RED(i,0,char_m2[col],char_rm2[col],col);
        } else if (iro[i] == CR_M3) {matrix_column_RED(i,0,char_m3[col],char_rm3[col],col);
        } else if (iro[i] == CR_M4) {matrix_column_RED(i,0,char_m4[col],char_rm4[col],col);
        } else if (iro[i] == CR_M5) {matrix_column_RED(i,0,char_m5[col],char_rm5[col],col);
        } else if (iro[i] == CR_M6) {matrix_column_RED(i,0,char_m6[col],char_rm6[col],col);
        } else if (iro[i] == CR_M7) {matrix_column_RED(i,0,char_m7[col],char_rm7[col],col);
        } else if (iro[i] == CR_M8) {matrix_column_RED(i,0,char_m8[col],char_rm8[col],col);
        } else if (iro[i] == CR_M9) {matrix_column_RED(i,0,char_m9[col],char_rm9[col],col);
        } else if (iro[i] == PENTA_G) {
            /*if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                    //   && option.c_rotate==R_01  ||
                    option.c_rotate==R_02   ) {*/
            /*
            if(((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && load.katamen==0)|| (load.katamen==1&& (i>=product.head/2))) {
                cel_led[i]=( penta[col] << 16 | 0xFF << 8) | (1 << col);
            } else {
                cel_led[i]=( pentar[col] << 16 | 0xFF << 8) | (1 << col);
            }*/
            if(option.c_rotate==R_OFF){
                cel_led[i]=( penta[col] << 16 | 0xFF << 8) | (1 << col);
            } else {
                cel_led[i]=( pentar[col] << 16 | 0xFF << 8) | (1 << col);
            }
        } else if (iro[i] == PENTA_R) {
            /*
            if(((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && load.katamen==0)|| (load.katamen==1&& (i>=product.head/2))) {
                cel_led[i]=( 0xFF << 16 | penta[col] << 8) | (1 << col);
            } else {
                cel_led[i]=( 0xFF << 16 | pentar[col] << 8) | (1 << col);
            }*/
            if(option.c_rotate==R_OFF){
                cel_led[i]=( 0xFF << 16 | penta[col] << 8) | (1 << col);
            } else {
                cel_led[i]=( 0xFF << 16 | pentar[col] << 8) | (1 << col);
            }
        } else if (iro[i] == PENTA_Y) {
            /*
            if(((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && load.katamen==0)|| (load.katamen==1&& (i>=product.head/2))) {
                cel_led[i]=( penta[col] << 16 | penta[col] << 8) | (1 << col);
            } else {
                cel_led[i]=( pentar[col] << 16 | pentar[col] << 8) | (1 << col);
            }*/
            if(option.c_rotate==R_OFF){
                cel_led[i]=( penta[col] << 16 | penta[col] << 8) | (1 << col);
            } else {
                cel_led[i]=( pentar[col] << 16 | pentar[col] << 8) | (1 << col);
            }
        } else if (iro[i] == C_SA) {matrix_rank_column_RED(i,char_SA[col],char_rSA[col],col);
            /*if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF || option.c_rotate==R_02   ) {
                cel_led[i]= cel_led_bit(i,char_SA[col]  ,char_antei[col] ,col);
            } else {
                cel_led[i]= cel_led_bit(i,char_rSA[col] ,char_rantei[col],col);
            }*/
        } else if (iro[i] == C_BUI_G) {matrix_column_GRE(i,char_bui[col],char_rbui[col],col);
        } else if (iro[i] == C_BUI_R) {matrix_column_RED(i,0,char_bui[col],char_rbui[col],col);
        } else if (iro[i] == C_BUI_Y) {matrix_column_YEL(i,char_bui[col],char_rbui[col],col);
        
        
        
        
        
        
        } else if (iro[i] == C_MA) {matrix_rank_column_RED(i,char_MA[col],char_rMA[col],col);
        } else if (iro[i] == C_9S) {matrix_rank_column_RED(i,char_9S[col],char_r9S[col],col);
        } else if (iro[i] == C_8S) {matrix_rank_column_RED(i,char_8S[col],char_r8S[col],col);
        } else if (iro[i] == C_7S) {matrix_rank_column_RED(i,char_7S[col],char_r7S[col],col);
        } else if (iro[i] == C_6S) {matrix_rank_column_RED(i,char_6S[col],char_r6S[col],col);
        } else if (iro[i] == C_5S) {matrix_rank_column_RED(i,char_5S[col],char_r5S[col],col);
        } else if (iro[i] == C_4S) {matrix_rank_column_RED(i,char_4S[col],char_r4S[col],col);
        } else if (iro[i] == C_3S) {matrix_rank_column_RED(i,char_3S[col],char_r3S[col],col);
        } else if (iro[i] == C_2S) {matrix_rank_column_RED(i,char_2S[col],char_r2S[col],col);
        } else if (iro[i] == C_S)  {matrix_rank_column_RED(i,char_S[col], char_rS[col],col );
        } else if (iro[i] == C_M)  {matrix_rank_column_RED(i,char_M[col], char_rM[col],col );
        } else if (iro[i] == C_L)  {matrix_rank_column_RED(i,char_L[col], char_rL[col],col );
        } else if (iro[i] == C_2L) {matrix_rank_column_RED(i,char_2L[col],char_r2L[col],col);
        } else if (iro[i] == C_3L) {matrix_rank_column_RED(i,char_3L[col],char_r3L[col],col);
        } else if (iro[i] == C_4L) {matrix_rank_column_RED(i,char_4L[col],char_r4L[col],col);
        } else if (iro[i] == C_5L) {matrix_rank_column_RED(i,char_5L[col],char_r5L[col],col);
        } else if (iro[i] == C_6L) {matrix_rank_column_RED(i,char_6L[col],char_r6L[col],col);
        } else if (iro[i] == C_7L) {matrix_rank_column_RED(i,char_7L[col],char_r7L[col],col);
        } else if (iro[i] == C_8L) {matrix_rank_column_RED(i,char_8L[col],char_r8L[col],col);
        } else if (iro[i] == C_9L) {matrix_rank_column_RED(i,char_9L[col],char_r9L[col],col);
        } else if (iro[i] == C_SS) {
            if((comp_kumi & oneBit[i]) > 0) {
                cel_led[i]=( ~char_SS[col] << 16 | char_SS[col] << 8) | (1 << col);
            } else if(led_r==false) {
                cel_led[i]=( char_SS[col] << 16 | 0xFF << 8) | (1 << col);
            } else {
                cel_led[i]=( 0xFF << 16 | char_SS[col] << 8) | (1 << col);
            }
        } else if (iro[i] == CG_SA) {matrix_rank_column_GRE(i,char_SA[col],char_rSA[col],col);
        } else if (iro[i] == CG_MA) {matrix_rank_column_GRE(i,char_MA[col],char_rMA[col],col);
        } else if (iro[i] == CG_9S) {matrix_rank_column_GRE(i,char_9S[col],char_r9S[col],col);
        } else if (iro[i] == CG_8S) {matrix_rank_column_GRE(i,char_8S[col],char_r8S[col],col);
        } else if (iro[i] == CG_7S) {matrix_rank_column_GRE(i,char_7S[col],char_r7S[col],col);
        } else if (iro[i] == CG_6S) {matrix_rank_column_GRE(i,char_6S[col],char_r6S[col],col);
        } else if (iro[i] == CG_5S) {matrix_rank_column_GRE(i,char_5S[col],char_r5S[col],col);
        } else if (iro[i] == CG_4S) {matrix_rank_column_GRE(i,char_4S[col],char_r4S[col],col);
        } else if (iro[i] == CG_3S) {matrix_rank_column_GRE(i,char_3S[col],char_r3S[col],col);
        } else if (iro[i] == CG_2S) {matrix_rank_column_GRE(i,char_2S[col],char_r2S[col],col);
        } else if (iro[i] == CG_S)  {matrix_rank_column_GRE(i,char_S[col], char_rS[col],col );
        } else if (iro[i] == CG_M)  {matrix_rank_column_GRE(i,char_M[col], char_rM[col],col );
        } else if (iro[i] == CG_L)  {matrix_rank_column_GRE(i,char_L[col], char_rL[col],col );
        } else if (iro[i] == CG_2L) {matrix_rank_column_GRE(i,char_2L[col],char_r2L[col],col);
        } else if (iro[i] == CG_3L) {matrix_rank_column_GRE(i,char_3L[col],char_r3L[col],col);
        } else if (iro[i] == CG_4L) {matrix_rank_column_GRE(i,char_4L[col],char_r4L[col],col);
        } else if (iro[i] == CG_5L) {matrix_rank_column_GRE(i,char_5L[col],char_r5L[col],col);
        } else if (iro[i] == CG_6L) {matrix_rank_column_GRE(i,char_6L[col],char_r6L[col],col);
        } else if (iro[i] == CG_7L) {matrix_rank_column_GRE(i,char_7L[col],char_r7L[col],col);
        } else if (iro[i] == CG_8L) {matrix_rank_column_GRE(i,char_8L[col],char_r8L[col],col);
        } else if (iro[i] == CG_9L) {matrix_rank_column_GRE(i,char_9L[col],char_r9L[col],col);
        
        } else if (iro[i] == C_MARU)   {cel_led[i]=( char_maru[col] << 16 | 0xFF << 8) | (1 << col);
        } else if (iro[i] == C_R_MARU) {cel_led[i]=( 0xFF << 16 | char_maru[col] << 8) | (1 << col);
        } else if (iro[i] == C_BATU)   {cel_led[i]=( 0xFF << 16 | char_batu[col] << 8) | (1 << col);
        } else if (iro[i] == C_OK)     {cel_led[i]=( char_OK[col] << 16 | 0xFF << 8) | (1 << col);
        } else if (iro[i] == C_NG)     {cel_led[i]=( 0xFF << 16 | char_NG[col] << 8) | (1 << col);
        } else if (iro[i] == C_QUES)   {cel_led[i]=( char_question[col] << 16 | 0xFF << 8) | (1 << col);
        } else if (iro[i] == C_R_EXCLA) {matrix_column_RED(i,0,char_exclamation[col],char_rexclamation[col],col);
        } else if (iro[i] == C_G_EXCLA) {matrix_column_GRE(i,char_exclamation[col],char_rexclamation[col],col);
            if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                    //   && option.c_rotate==R_01  ||
                    option.c_rotate==R_02   ) {
                cel_led[i]=( char_exclamation[col]  << 16 | 0xFF << 8) | (1 << col);
            } else {
                cel_led[i]=( char_rexclamation[col] << 16 | 0xFF << 8) | (1 << col);
            }
        } else if (iro[i] == ALL_G ) {
            cel_led[i]=( all[col] << 16 | 0xFF << 8) | (1 << col);
        } else if (iro[i] == ALL_R ) {
            cel_led[i]=( 0xFF << 16 | all[col] << 8) | (1 << col);
        } else if (iro[i] == ALL_Y ) {
            cel_led[i]=( all[col] << 16 | all[col] << 8) | (1 << col);
        } else if (iro[i] == C_OKP) {
            cel_led[i]=0;
            cel_led[2]=( 0xFF << 16 | char_o[col] << 8) | (1 << col);
            cel_led[1]=( 0xFF << 16 | char_k[col] << 8) | (1 << col);
            cel_led[0]=( 0xFF << 16 | char_p[col] << 8) | (1 << col);
        } else if (iro[i] == C_NUSE) {
            cel_led[i]=( 0xFF << 16 | char_NUSE[col] << 8) | (1 << col);
        } else if (iro[i] == C_NUSE_1D) {
            cel_led[i]=( 0xFF << 16 | char_NUSE1d[col] << 8) | (1 << col);
        } else if (iro[i] == C_NUSE_2D) {
            cel_led[i]=( 0xFF << 16 | char_NUSE2d[col] << 8) | (1 << col);
        
        
        
        
        
        
/////シンワオンリー/////////////////////////////////////////////////////////////////////////////            
        } else if (iro[i] == C_1044) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_44[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_44[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_44[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r44[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r44[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r44[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1040) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_40[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_40[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_40[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r40[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r40[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r40[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1036) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_36[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_36[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_36[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r36[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r36[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r36[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1032) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_32[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_32[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_32[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r32[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r32[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r32[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1028) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_28[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_28[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_28[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r28[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r28[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r28[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1024) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_24[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_24[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_24[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r24[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r24[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r24[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1020) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_20[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_20[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_20[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r20[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r20[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r20[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1018) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_18[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_18[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_18[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r18[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r18[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r18[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_1016) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_16[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_16[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_16[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r16[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r16[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r16[col]   << 8) | (1 << col);
                        }
                    }  
            
            
            
            
        } else if (iro[i] == C_0522) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_22[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_22[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_22[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r22[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r22[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r22[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0520) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_20[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_20[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_20[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r20[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r20[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r20[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0518) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_18[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_18[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_18[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r18[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r18[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r18[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0516) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_16[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_16[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_16[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r16[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r16[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r16[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0514) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_14[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_14[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_14[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r14[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r14[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r14[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0512) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_12[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_12[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_12[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r12[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r12[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r12[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0510) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_10[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_10[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_10[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r10[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r10[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r10[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0509) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_09[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_09[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_09[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r09[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r09[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r09[col]   << 8) | (1 << col);
                        }
                    }  
            
        } else if (iro[i] == C_0508) {
                    if((product.Array != 0 || (product.Array == 0 && (i<product.head/2))) && option.c_rotate==R_OFF ||
                            option.c_rotate==R_02   ) {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_antei[col]    << 16 | char_08[col]    << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_08[col]       << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_08[col]    << 8) | (1 << col);
                        }
                    } else {
                        if((comp_rank & oneBit[i]) > 0) {   cel_led[i]=( char_rantei[col]   << 16 | char_r08[col]   << 8) | (1 << col);
                        } else if(led_r==false) {           cel_led[i]=( char_r08[col]      << 16 | 0xFF            << 8) | (1 << col);
                        } else {                            cel_led[i]=( 0xFF               << 16 | char_r08[col]   << 8) | (1 << col);
                        }
                    }  
            
            
            
            
            
            
            
            
            
            
            
            
        } else {
            cel_led[i]=0;
            //led.regis(0);
            //return; //表示なしなら終了
        }
    }
    //printf(":%4d:%4d:%4d:%4d:%4d:%4d\r\n",cel_led[0],cel_led[1],cel_led[2],cel_led[3],cel_led[4],cel_led[5]);
    //led.regis2(cel_led[0],cel_led[1],cel_led[2],cel_led[3],cel_led[4],cel_led[5]);
    led.regis2();
    //wait_us(5);
    col++;
    col %= 8;
}

//*****************************matrix_end**************************************************