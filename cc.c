#include <ncurses.h>
#include <ctype.h>

#include "libitute164.h"

enum itu_t_e164_type_enum itu_t_e164_cc_2_type(int country_code)
{
    switch(country_code) {
        case 0:   return ITU_T_RESERVED;
        case 1:   return ITU_T_NUMBER; // NANP
        case 2:   return ITU_T_INCOMPLETE;
        case 20:  return ITU_T_NUMBER; // Egypt
        case 21:  return ITU_T_INCOMPLETE;
        case 210: return ITU_T_SPARE;
        case 211: return ITU_T_NUMBER; // South Sudan
        case 212: return ITU_T_NUMBER; // Morocco
        case 213: return ITU_T_NUMBER; // Algeria
        case 214: return ITU_T_SPARE;
        case 215: return ITU_T_SPARE;
        case 216: return ITU_T_NUMBER; // Tunisia
        case 217: return ITU_T_SPARE;
        case 218: return ITU_T_NUMBER; // Libya
        case 219: return ITU_T_SPARE;
        case 22:  return ITU_T_INCOMPLETE;
        case 220: return ITU_T_NUMBER; // Gambia
        case 221: return ITU_T_NUMBER; // Senegal
        case 222: return ITU_T_NUMBER; // Mauritania
        case 223: return ITU_T_NUMBER; // Mali
        case 224: return ITU_T_NUMBER; // Guinea
        case 225: return ITU_T_NUMBER; // Côte d'Ivoire
        case 226: return ITU_T_NUMBER; // Burkina Faso
        case 227: return ITU_T_NUMBER; // Niger
        case 228: return ITU_T_NUMBER; // Togolese Republic
        case 229: return ITU_T_NUMBER; // Benin
        case 23:  return ITU_T_INCOMPLETE;
        case 230: return ITU_T_NUMBER; // Mauritius
        case 231: return ITU_T_NUMBER; // Liberia
        case 232: return ITU_T_NUMBER; // Sierra Leone
        case 233: return ITU_T_NUMBER; // Ghana
        case 234: return ITU_T_NUMBER; // Nigeria
        case 235: return ITU_T_NUMBER; // Chad
        case 236: return ITU_T_NUMBER; // Central African Republic
        case 237: return ITU_T_NUMBER; // Cameroon
        case 238: return ITU_T_NUMBER; // Cabo Verde
        case 239: return ITU_T_NUMBER; // Sao Tome and Principe
        case 24:  return ITU_T_INCOMPLETE;
        case 240: return ITU_T_NUMBER; // Equatorial Guinea
        case 241: return ITU_T_NUMBER; // Gabonese Republic
        case 242: return ITU_T_NUMBER; // Congo
        case 243: return ITU_T_NUMBER; // Democratic Republic of the Congo
        case 244: return ITU_T_NUMBER; // Angola
        case 245: return ITU_T_NUMBER; // Guinea-Bissau
        case 246: return ITU_T_NUMBER; // Diego Garcia
        case 247: return ITU_T_NUMBER; // Saint Helena and Tristan da Cunha
        case 248: return ITU_T_NUMBER; // Seychelles
        case 249: return ITU_T_NUMBER; // Sudan
        case 25:  return ITU_T_INCOMPLETE;
        case 250: return ITU_T_NUMBER; // Rwanda
        case 251: return ITU_T_NUMBER; // Ethiopia
        case 252: return ITU_T_NUMBER; // Somalia
        case 253: return ITU_T_NUMBER; // Djibouti
        case 254: return ITU_T_NUMBER; // Kenya
        case 255: return ITU_T_NUMBER; // Tanzania
        case 256: return ITU_T_NUMBER; // Uganda
        case 257: return ITU_T_NUMBER; // Burundi
        case 258: return ITU_T_NUMBER; // Mozambique
        case 259: return ITU_T_SPARE;
        case 26:  return ITU_T_INCOMPLETE;
        case 260: return ITU_T_NUMBER; // Zambia
        case 261: return ITU_T_NUMBER; // Madagascar
        case 262: return ITU_T_NUMBER; // French Departments and Territories in the Indian Ocean
        case 263: return ITU_T_NUMBER; // Zimbabwe
        case 264: return ITU_T_NUMBER; // Namibia
        case 265: return ITU_T_NUMBER; // Malawi
        case 266: return ITU_T_NUMBER; // Lesotho
        case 267: return ITU_T_NUMBER; // Botswana
        case 268: return ITU_T_NUMBER; // Swaziland
        case 269: return ITU_T_NUMBER; // Comoros
        case 27:  return ITU_T_NUMBER; // South Africa
        case 28:  return ITU_T_INCOMPLETE;
        case 280: return ITU_T_SPARE;
        case 281: return ITU_T_SPARE;
        case 282: return ITU_T_SPARE;
        case 283: return ITU_T_SPARE;
        case 284: return ITU_T_SPARE;
        case 285: return ITU_T_SPARE;
        case 286: return ITU_T_SPARE;
        case 287: return ITU_T_SPARE;
        case 288: return ITU_T_SPARE;
        case 289: return ITU_T_SPARE;
        case 29:  return ITU_T_INCOMPLETE;
        case 290: return ITU_T_NUMBER; // Saint Helena and Tristan da Cunha
        case 291: return ITU_T_NUMBER; // Eritrea
        case 292: return ITU_T_SPARE;
        case 293: return ITU_T_SPARE;
        case 294: return ITU_T_SPARE;
        case 295: return ITU_T_SPARE;
        case 296: return ITU_T_SPARE;
        case 297: return ITU_T_NUMBER; // Aruba
        case 298: return ITU_T_NUMBER; // Faroe Islands
        case 299: return ITU_T_NUMBER; // Greenland (Denmark)
        case 3:   return ITU_T_INCOMPLETE;
        case 30:  return ITU_T_NUMBER; // Greece
        case 31:  return ITU_T_NUMBER; // Netherlands
        case 32:  return ITU_T_NUMBER; // Belgium
        case 33:  return ITU_T_NUMBER; // France
        case 34:  return ITU_T_NUMBER; // Spain
        case 35:  return ITU_T_INCOMPLETE;
        case 350: return ITU_T_NUMBER; // Gibraltar
        case 351: return ITU_T_NUMBER; // Portugal
        case 352: return ITU_T_NUMBER; // Luxembourg
        case 353: return ITU_T_NUMBER; // Ireland
        case 354: return ITU_T_NUMBER; // Iceland
        case 355: return ITU_T_NUMBER; // Albania
        case 356: return ITU_T_NUMBER; // Malta
        case 357: return ITU_T_NUMBER; // Cyprus
        case 358: return ITU_T_NUMBER; // Finland
        case 359: return ITU_T_NUMBER; // Bulgaria
        case 36:  return ITU_T_NUMBER; // Hungary
        case 37:  return ITU_T_INCOMPLETE;
        case 370: return ITU_T_NUMBER; // Lithuania
        case 371: return ITU_T_NUMBER; // Latvia
        case 372: return ITU_T_NUMBER; // Estonia
        case 373: return ITU_T_NUMBER; // Moldova
        case 374: return ITU_T_NUMBER; // Armenia
        case 375: return ITU_T_NUMBER; // Belarus
        case 376: return ITU_T_NUMBER; // Andorra
        case 377: return ITU_T_NUMBER; // Monaco
        case 378: return ITU_T_NUMBER; // San Marino
        case 379: return ITU_T_NUMBER; // Vatican City State
        case 38:  return ITU_T_INCOMPLETE;
        case 380: return ITU_T_NUMBER; // Ukraine
        case 381: return ITU_T_NUMBER; // Serbia
        case 382: return ITU_T_NUMBER; // Montenegro
        case 383: return ITU_T_NUMBER; // Kosovo
        case 384: return ITU_T_SPARE;
        case 385: return ITU_T_NUMBER; // Croatia
        case 386: return ITU_T_NUMBER; // Slovenia
        case 387: return ITU_T_NUMBER; // Bosnia and Herzegovina
        case 388: return ITU_T_GROUPS;
        case 389: return ITU_T_NUMBER; // The Former Yugoslav Republic of Macedonia
        case 39:  return ITU_T_NUMBER; // Italy
        case 4:   return ITU_T_INCOMPLETE;
        case 40:  return ITU_T_NUMBER; // Romania
        case 41:  return ITU_T_NUMBER; // Switzerland
        case 42:  return ITU_T_INCOMPLETE;
        case 420: return ITU_T_NUMBER; // Czech Republic
        case 421: return ITU_T_NUMBER; // Slovak Republic
        case 422: return ITU_T_SPARE;
        case 423: return ITU_T_NUMBER; // Liechtenstein
        case 424: return ITU_T_SPARE;
        case 425: return ITU_T_SPARE;
        case 426: return ITU_T_SPARE;
        case 427: return ITU_T_SPARE;
        case 428: return ITU_T_SPARE;
        case 429: return ITU_T_SPARE;
        case 43:  return ITU_T_NUMBER; // Austria
        case 44:  return ITU_T_NUMBER; // United Kingdom of Great Britain and Northern Ireland
        case 45:  return ITU_T_NUMBER; // Denmark
        case 46:  return ITU_T_NUMBER; // Sweden
        case 47:  return ITU_T_NUMBER; // Norway
        case 48:  return ITU_T_NUMBER; // Poland
        case 49:  return ITU_T_NUMBER; // Germany
        case 5:   return ITU_T_INCOMPLETE;
        case 50:  return ITU_T_INCOMPLETE;
        case 500: return ITU_T_NUMBER; // Falkland Islands
        case 501: return ITU_T_NUMBER; // Belize
        case 502: return ITU_T_NUMBER; // Guatemala
        case 503: return ITU_T_NUMBER; // El Salvador
        case 504: return ITU_T_NUMBER; // Honduras
        case 505: return ITU_T_NUMBER; // Nicaragua
        case 506: return ITU_T_NUMBER; // Costa Rica
        case 507: return ITU_T_NUMBER; // Panama
        case 508: return ITU_T_NUMBER; // Saint Pierre and Miquelon
        case 509: return ITU_T_NUMBER; // Haiti
        case 51:  return ITU_T_NUMBER; // Peru
        case 52:  return ITU_T_NUMBER; // Mexico
        case 53:  return ITU_T_NUMBER; // Cuba
        case 54:  return ITU_T_NUMBER; // Argentine Republic
        case 55:  return ITU_T_NUMBER; // Brazil
        case 56:  return ITU_T_NUMBER; // Chile
        case 57:  return ITU_T_NUMBER; // Colombia
        case 58:  return ITU_T_NUMBER; // Venezuela
        case 59:  return ITU_T_INCOMPLETE;
        case 590: return ITU_T_NUMBER; // Guadeloupe
        case 591: return ITU_T_NUMBER; // Bolivia
        case 592: return ITU_T_NUMBER; // Guyana
        case 593: return ITU_T_NUMBER; // Ecuador
        case 594: return ITU_T_NUMBER; // French Guiana
        case 595: return ITU_T_NUMBER; // Paraguay
        case 596: return ITU_T_NUMBER; // Martinique
        case 597: return ITU_T_NUMBER; // Suriname
        case 598: return ITU_T_NUMBER; // Uruguay
        case 599: return ITU_T_NUMBER; // Curaçao
        case 6:   return ITU_T_INCOMPLETE;
        case 60:  return ITU_T_NUMBER; // Malaysia
        case 61:  return ITU_T_NUMBER; // Australia
        case 62:  return ITU_T_NUMBER; // Indonesia
        case 63:  return ITU_T_NUMBER; // Philippines
        case 64:  return ITU_T_NUMBER; // New Zealand
        case 65:  return ITU_T_NUMBER; // Singapore
        case 66:  return ITU_T_NUMBER; // Thailand
        case 67:  return ITU_T_INCOMPLETE;
        case 670: return ITU_T_NUMBER; // Timor-Leste
        case 671: return ITU_T_SPARE;
        case 672: return ITU_T_NUMBER; // Australian External Territories
        case 673: return ITU_T_NUMBER; // Brunei Darussalam
        case 674: return ITU_T_NUMBER; // Nauru
        case 675: return ITU_T_NUMBER; // Papua New Guinea
        case 676: return ITU_T_NUMBER; // Tonga
        case 677: return ITU_T_NUMBER; // Solomon Islands
        case 678: return ITU_T_NUMBER; // Vanuatu
        case 679: return ITU_T_NUMBER; // Fiji
        case 68:  return ITU_T_INCOMPLETE;
        case 680: return ITU_T_NUMBER; // Palau
        case 681: return ITU_T_NUMBER; // Wallis and Futuna
        case 682: return ITU_T_NUMBER; // Cook Islands
        case 683: return ITU_T_NUMBER; // Niue
        case 684: return ITU_T_SPARE;
        case 685: return ITU_T_NUMBER; // Samoa
        case 686: return ITU_T_NUMBER; // Kiribati
        case 687: return ITU_T_NUMBER; // New Caledonia
        case 688: return ITU_T_NUMBER; // Tuvalu
        case 689: return ITU_T_NUMBER; // French Polynesia
        case 69:  return ITU_T_INCOMPLETE;
        case 690: return ITU_T_NUMBER; // Tokelau
        case 691: return ITU_T_NUMBER; // Micronesia
        case 692: return ITU_T_NUMBER; // Marshall Islands
        case 693: return ITU_T_SPARE;
        case 694: return ITU_T_SPARE;
        case 695: return ITU_T_SPARE;
        case 696: return ITU_T_SPARE;
        case 697: return ITU_T_SPARE;
        case 698: return ITU_T_SPARE;
        case 699: return ITU_T_SPARE;
        case 7:   return ITU_T_NUMBER; // Russian Federation
        case 8:   return ITU_T_INCOMPLETE;
        case 80:  return ITU_T_INCOMPLETE;
        case 800: return ITU_T_GLOBAL; // International Freephone Service
        case 801: return ITU_T_SPARE;
        case 802: return ITU_T_SPARE;
        case 803: return ITU_T_SPARE;
        case 804: return ITU_T_SPARE;
        case 805: return ITU_T_SPARE;
        case 806: return ITU_T_SPARE;
        case 807: return ITU_T_SPARE;
        case 808: return ITU_T_GLOBAL; // International Shared Cost Service (ISCS)
        case 809: return ITU_T_SPARE;
        case 81:  return ITU_T_NUMBER; // Japan
        case 82:  return ITU_T_NUMBER; // Korea
        case 83:  return ITU_T_INCOMPLETE;
        case 830: return ITU_T_SPARE;
        case 831: return ITU_T_SPARE;
        case 832: return ITU_T_SPARE;
        case 833: return ITU_T_SPARE;
        case 834: return ITU_T_SPARE;
        case 835: return ITU_T_SPARE;
        case 836: return ITU_T_SPARE;
        case 837: return ITU_T_SPARE;
        case 838: return ITU_T_SPARE;
        case 839: return ITU_T_SPARE;
        case 84:  return ITU_T_NUMBER; // Viet Nam
        case 85:  return ITU_T_INCOMPLETE;
        case 850: return ITU_T_NUMBER; // Democratic People's Republic of Korea
        case 851: return ITU_T_SPARE;
        case 852: return ITU_T_NUMBER; // Hong Kong, China
        case 853: return ITU_T_NUMBER; // Macao, China
        case 854: return ITU_T_SPARE;
        case 855: return ITU_T_NUMBER; // Cambodia
        case 856: return ITU_T_NUMBER; // Lao People's Democratic Republic
        case 857: return ITU_T_SPARE;
        case 858: return ITU_T_SPARE;
        case 859: return ITU_T_SPARE;
        case 86:  return ITU_T_NUMBER; // China
        case 87:  return ITU_T_INCOMPLETE;
        case 870: return ITU_T_GLOBAL; // Inmarsat SNAC
        case 871: return ITU_T_SPARE;
        case 872: return ITU_T_SPARE;
        case 873: return ITU_T_SPARE;
        case 874: return ITU_T_SPARE;
        case 875: return ITU_T_RESERVED; // Reserved - Maritime Mobile Service Applications
        case 876: return ITU_T_RESERVED; // Reserved - Maritime Mobile Service Applications
        case 877: return ITU_T_RESERVED; // Reserved - Maritime Mobile Service Applications
        case 878: return ITU_T_GLOBAL; // Universal Personal Telecommunication Service (UPT)
        case 879: return ITU_T_RESERVED; // Reserved for national non-commercial purposes
        case 88:  return ITU_T_INCOMPLETE;
        case 880: return ITU_T_NUMBER; // Bangladesh
        case 881: return ITU_T_GLOBAL; // Global Mobile Satellite System (GMSS), shared code
        case 882: return ITU_T_GLOBAL; // International Networks, shared code
        case 883: return ITU_T_GLOBAL; // International Networks, shared code
        case 884: return ITU_T_SPARE;
        case 885: return ITU_T_SPARE;
        case 886: return ITU_T_NUMBER; // Taiwan, China
        case 887: return ITU_T_SPARE;
        case 888: return ITU_T_GLOBAL; // Telecommunications for Disaster Relief (TDR)
        case 889: return ITU_T_SPARE;
        case 89:  return ITU_T_INCOMPLETE;
        case 890: return ITU_T_SPARE;
        case 891: return ITU_T_SPARE;
        case 892: return ITU_T_SPARE;
        case 893: return ITU_T_SPARE;
        case 894: return ITU_T_SPARE;
        case 895: return ITU_T_SPARE;
        case 896: return ITU_T_SPARE;
        case 897: return ITU_T_SPARE;
        case 898: return ITU_T_SPARE;
        case 899: return ITU_T_SPARE;
        case 9:   return ITU_T_INCOMPLETE;
        case 90:  return ITU_T_NUMBER; // Turkey
        case 91:  return ITU_T_NUMBER; // India
        case 92:  return ITU_T_NUMBER; // Pakistan
        case 93:  return ITU_T_NUMBER; // Afghanistan
        case 94:  return ITU_T_NUMBER; // Sri Lanka
        case 95:  return ITU_T_NUMBER; // Myanmar
        case 96:  return ITU_T_INCOMPLETE;
        case 960: return ITU_T_NUMBER; // Maldives
        case 961: return ITU_T_NUMBER; // Lebanon
        case 962: return ITU_T_NUMBER; // Jordan
        case 963: return ITU_T_NUMBER; // Syrian Arab Republic
        case 964: return ITU_T_NUMBER; // Iraq
        case 965: return ITU_T_NUMBER; // Kuwait
        case 966: return ITU_T_NUMBER; // Saudi Arabia
        case 967: return ITU_T_NUMBER; // Yemen
        case 968: return ITU_T_NUMBER; // Oman
        case 969: return ITU_T_RESERVED; // Reserved
        case 97:  return ITU_T_INCOMPLETE;
        case 970: return ITU_T_RESERVED;
        case 971: return ITU_T_NUMBER; // United Arab Emirates
        case 972: return ITU_T_NUMBER; // Israel
        case 973: return ITU_T_NUMBER; // Bahrain
        case 974: return ITU_T_NUMBER; // Qatar
        case 975: return ITU_T_NUMBER; // Bhutan
        case 976: return ITU_T_NUMBER; // Mongolia
        case 977: return ITU_T_NUMBER; // Nepal
        case 978: return ITU_T_SPARE;
        case 979: return ITU_T_GLOBAL; // International Premium Rate Service (IPRS)
        case 98:  return ITU_T_NUMBER; // Iran
        case 99:  return ITU_T_INCOMPLETE;
        case 990: return ITU_T_GLOBAL; // international telecommunication public correspondence
        case 991: return ITU_T_TRIALS;
        case 992: return ITU_T_NUMBER; // Tajikistan
        case 993: return ITU_T_NUMBER; // Turkmenistan
        case 994: return ITU_T_NUMBER; // Azerbaijan
        case 995: return ITU_T_NUMBER; // Georgia
        case 996: return ITU_T_NUMBER; // Kyrgyz Republic
        case 997: return ITU_T_SPARE;
        case 998: return ITU_T_NUMBER; // Uzbekistan
        case 999: return ITU_T_RESERVED; // Reserved for future global service
        default:  return ITU_T_UNKNOWN;
    }
}

struct cc_regex *itu_t_e164_cc_subscriber_regex(int country_code)
{
    static struct cc_regex *codes[1000];
    // NANPA
    static struct cc_regex codes_1[] = {
        { NULL, "^[0-9]{0,7}$", "###-####" },
        { NULL, NULL,           NULL},
    };
    // Argentina
    static struct cc_regex codes_54[] = {
        { "^[0-9]{4}$", "^[0-9]{0,6}$",  "######" },
        { "^[0-9]{3}$", "^[0-9]{0,7}$",  "### ####" },
        { "^[0-9]{2}$", "^[0-9]{0,8}$",  "#### ####" },
        { NULL,         "^$",            "#### ####" },
        { NULL,         NULL,            NULL},
    };
    // Brazil
    static struct cc_regex codes_55[] = {
        { NULL, "^9[0-9]{0,8}$", "#####-####" },
        { NULL, "^[0-9]{0,8}$",  "####-####" },
        { NULL, NULL,            NULL},
    };
    // Uruguay
    static struct cc_regex codes_598[] = {
        { NULL, "^[0-9]{0,8}$", "# ### ####" },
        { NULL, NULL,           NULL},
    };
    codes[1] = codes_1;
    codes[54] = codes_54;
    codes[55] = codes_55;
    codes[598] = codes_598;
    return codes[country_code];
}
