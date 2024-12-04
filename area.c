#include "libitute164.h"

enum itu_t_area_type_enum itu_t_e164_area_2_type(int country_code, int area_code)
{
    switch(country_code) {
        case 1: // NANPA
            switch(area_code) {
                case 0:   return ITU_T_AREA_INCOMPLETE; // Empty area code
                case 2:   return ITU_T_AREA_INCOMPLETE;
                case 20:  return ITU_T_AREA_INCOMPLETE;
                case 201: return ITU_T_AREA_NUMBER;
                case 202: return ITU_T_AREA_NUMBER;
                case 203: return ITU_T_AREA_NUMBER;
                case 204: return ITU_T_AREA_NUMBER;
                case 205: return ITU_T_AREA_NUMBER;
                case 206: return ITU_T_AREA_NUMBER;
                case 207: return ITU_T_AREA_NUMBER;
                case 208: return ITU_T_AREA_NUMBER;
                case 209: return ITU_T_AREA_NUMBER;
                case 21:  return ITU_T_AREA_INCOMPLETE;
                case 210: return ITU_T_AREA_NUMBER;
                case 212: return ITU_T_AREA_NUMBER;
                case 213: return ITU_T_AREA_NUMBER;
                case 214: return ITU_T_AREA_NUMBER;
                case 215: return ITU_T_AREA_NUMBER;
                case 216: return ITU_T_AREA_NUMBER;
                case 217: return ITU_T_AREA_NUMBER;
                case 218: return ITU_T_AREA_NUMBER;
                case 219: return ITU_T_AREA_NUMBER;
                case 22:  return ITU_T_AREA_INCOMPLETE;
                case 220: return ITU_T_AREA_NUMBER;
                case 223: return ITU_T_AREA_NUMBER;
                case 224: return ITU_T_AREA_NUMBER;
                case 225: return ITU_T_AREA_NUMBER;
                case 226: return ITU_T_AREA_NUMBER;
                case 227: return ITU_T_AREA_NUMBER;
                case 228: return ITU_T_AREA_NUMBER;
                case 229: return ITU_T_AREA_NUMBER;
                case 23:  return ITU_T_AREA_INCOMPLETE;
                case 231: return ITU_T_AREA_NUMBER;
                case 234: return ITU_T_AREA_NUMBER;
                case 235: return ITU_T_AREA_NUMBER;
                case 236: return ITU_T_AREA_NUMBER;
                case 239: return ITU_T_AREA_NUMBER;
                case 24:  return ITU_T_AREA_INCOMPLETE;
                case 240: return ITU_T_AREA_NUMBER;
                case 242: return ITU_T_AREA_NUMBER;
                case 246: return ITU_T_AREA_NUMBER;
                case 248: return ITU_T_AREA_NUMBER;
                case 249: return ITU_T_AREA_NUMBER;
                case 25:  return ITU_T_AREA_INCOMPLETE;
                case 250: return ITU_T_AREA_NUMBER;
                case 251: return ITU_T_AREA_NUMBER;
                case 252: return ITU_T_AREA_NUMBER;
                case 253: return ITU_T_AREA_NUMBER;
                case 254: return ITU_T_AREA_NUMBER;
                case 256: return ITU_T_AREA_NUMBER;
                case 257: return ITU_T_AREA_NUMBER;
                case 26:  return ITU_T_AREA_INCOMPLETE;
                case 260: return ITU_T_AREA_NUMBER;
                case 262: return ITU_T_AREA_NUMBER;
                case 263: return ITU_T_AREA_NUMBER;
                case 264: return ITU_T_AREA_NUMBER;
                case 267: return ITU_T_AREA_NUMBER;
                case 268: return ITU_T_AREA_NUMBER;
                case 269: return ITU_T_AREA_NUMBER;
                case 27:  return ITU_T_AREA_INCOMPLETE;
                case 270: return ITU_T_AREA_NUMBER;
                case 272: return ITU_T_AREA_NUMBER;
                case 274: return ITU_T_AREA_NUMBER;
                case 276: return ITU_T_AREA_NUMBER;
                case 279: return ITU_T_AREA_NUMBER;
                case 28:  return ITU_T_AREA_INCOMPLETE;
                case 281: return ITU_T_AREA_NUMBER;
                case 283: return ITU_T_AREA_NUMBER;
                case 284: return ITU_T_AREA_NUMBER;
                case 289: return ITU_T_AREA_NUMBER;
                case 3:   return ITU_T_AREA_INCOMPLETE;
                case 30:  return ITU_T_AREA_INCOMPLETE;
                case 301: return ITU_T_AREA_NUMBER;
                case 302: return ITU_T_AREA_NUMBER;
                case 303: return ITU_T_AREA_NUMBER;
                case 304: return ITU_T_AREA_NUMBER;
                case 305: return ITU_T_AREA_NUMBER;
                case 306: return ITU_T_AREA_NUMBER;
                case 307: return ITU_T_AREA_NUMBER;
                case 308: return ITU_T_AREA_NUMBER;
                case 309: return ITU_T_AREA_NUMBER;
                case 31:  return ITU_T_AREA_INCOMPLETE;
                case 310: return ITU_T_AREA_NUMBER;
                case 312: return ITU_T_AREA_NUMBER;
                case 313: return ITU_T_AREA_NUMBER;
                case 314: return ITU_T_AREA_NUMBER;
                case 315: return ITU_T_AREA_NUMBER;
                case 316: return ITU_T_AREA_NUMBER;
                case 317: return ITU_T_AREA_NUMBER;
                case 318: return ITU_T_AREA_NUMBER;
                case 319: return ITU_T_AREA_NUMBER;
                case 32:  return ITU_T_AREA_INCOMPLETE;
                case 320: return ITU_T_AREA_NUMBER;
                case 321: return ITU_T_AREA_NUMBER;
                case 323: return ITU_T_AREA_NUMBER;
                case 324: return ITU_T_AREA_NUMBER;
                case 325: return ITU_T_AREA_NUMBER;
                case 326: return ITU_T_AREA_NUMBER;
                case 327: return ITU_T_AREA_NUMBER;
                case 329: return ITU_T_AREA_NUMBER;
                case 33:  return ITU_T_AREA_INCOMPLETE;
                case 330: return ITU_T_AREA_NUMBER;
                case 331: return ITU_T_AREA_NUMBER;
                case 332: return ITU_T_AREA_NUMBER;
                case 334: return ITU_T_AREA_NUMBER;
                case 336: return ITU_T_AREA_NUMBER;
                case 337: return ITU_T_AREA_NUMBER;
                case 339: return ITU_T_AREA_NUMBER;
                case 34:  return ITU_T_AREA_INCOMPLETE;
                case 340: return ITU_T_AREA_NUMBER;
                case 341: return ITU_T_AREA_NUMBER;
                case 343: return ITU_T_AREA_NUMBER;
                case 345: return ITU_T_AREA_NUMBER;
                case 346: return ITU_T_AREA_NUMBER;
                case 347: return ITU_T_AREA_NUMBER;
                case 35:  return ITU_T_AREA_INCOMPLETE;
                case 350: return ITU_T_AREA_NUMBER;
                case 351: return ITU_T_AREA_NUMBER;
                case 352: return ITU_T_AREA_NUMBER;
                case 353: return ITU_T_AREA_NUMBER;
                case 354: return ITU_T_AREA_NUMBER;
                case 357: return ITU_T_AREA_NUMBER;
                case 36:  return ITU_T_AREA_INCOMPLETE;
                case 360: return ITU_T_AREA_NUMBER;
                case 361: return ITU_T_AREA_NUMBER;
                case 363: return ITU_T_AREA_NUMBER;
                case 364: return ITU_T_AREA_NUMBER;
                case 365: return ITU_T_AREA_NUMBER;
                case 367: return ITU_T_AREA_NUMBER;
                case 368: return ITU_T_AREA_NUMBER;
                case 369: return ITU_T_AREA_NUMBER;
                case 38:  return ITU_T_AREA_INCOMPLETE;
                case 380: return ITU_T_AREA_NUMBER;
                case 382: return ITU_T_AREA_NUMBER;
                case 385: return ITU_T_AREA_NUMBER;
                case 386: return ITU_T_AREA_NUMBER;
                case 4:   return ITU_T_AREA_INCOMPLETE;
                case 40:  return ITU_T_AREA_INCOMPLETE;
                case 401: return ITU_T_AREA_NUMBER;
                case 402: return ITU_T_AREA_NUMBER;
                case 403: return ITU_T_AREA_NUMBER;
                case 404: return ITU_T_AREA_NUMBER;
                case 405: return ITU_T_AREA_NUMBER;
                case 406: return ITU_T_AREA_NUMBER;
                case 407: return ITU_T_AREA_NUMBER;
                case 408: return ITU_T_AREA_NUMBER;
                case 409: return ITU_T_AREA_NUMBER;
                case 41:  return ITU_T_AREA_INCOMPLETE;
                case 410: return ITU_T_AREA_NUMBER;
                case 412: return ITU_T_AREA_NUMBER;
                case 413: return ITU_T_AREA_NUMBER;
                case 414: return ITU_T_AREA_NUMBER;
                case 415: return ITU_T_AREA_NUMBER;
                case 416: return ITU_T_AREA_NUMBER;
                case 417: return ITU_T_AREA_NUMBER;
                case 418: return ITU_T_AREA_NUMBER;
                case 419: return ITU_T_AREA_NUMBER;
                case 42:  return ITU_T_AREA_INCOMPLETE;
                case 423: return ITU_T_AREA_NUMBER;
                case 424: return ITU_T_AREA_NUMBER;
                case 425: return ITU_T_AREA_NUMBER;
                case 428: return ITU_T_AREA_NUMBER;
                case 43:  return ITU_T_AREA_INCOMPLETE;
                case 430: return ITU_T_AREA_NUMBER;
                case 431: return ITU_T_AREA_NUMBER;
                case 432: return ITU_T_AREA_NUMBER;
                case 434: return ITU_T_AREA_NUMBER;
                case 435: return ITU_T_AREA_NUMBER;
                case 436: return ITU_T_AREA_NUMBER;
                case 437: return ITU_T_AREA_NUMBER;
                case 438: return ITU_T_AREA_NUMBER;
                case 44:  return ITU_T_AREA_INCOMPLETE;
                case 440: return ITU_T_AREA_NUMBER;
                case 441: return ITU_T_AREA_NUMBER;
                case 442: return ITU_T_AREA_NUMBER;
                case 443: return ITU_T_AREA_NUMBER;
                case 445: return ITU_T_AREA_NUMBER;
                case 447: return ITU_T_AREA_NUMBER;
                case 448: return ITU_T_AREA_NUMBER;
                case 45:  return ITU_T_AREA_INCOMPLETE;
                case 450: return ITU_T_AREA_NUMBER;
                case 457: return ITU_T_AREA_NUMBER;
                case 458: return ITU_T_AREA_NUMBER;
                case 46:  return ITU_T_AREA_INCOMPLETE;
                case 460: return ITU_T_AREA_NUMBER;
                case 463: return ITU_T_AREA_NUMBER;
                case 464: return ITU_T_AREA_NUMBER;
                case 468: return ITU_T_AREA_NUMBER;
                case 469: return ITU_T_AREA_NUMBER;
                case 47:  return ITU_T_AREA_INCOMPLETE;
                case 470: return ITU_T_AREA_NUMBER;
                case 471: return ITU_T_AREA_NUMBER;
                case 472: return ITU_T_AREA_NUMBER;
                case 473: return ITU_T_AREA_NUMBER;
                case 474: return ITU_T_AREA_NUMBER;
                case 475: return ITU_T_AREA_NUMBER;
                case 478: return ITU_T_AREA_NUMBER;
                case 479: return ITU_T_AREA_NUMBER;
                case 48:  return ITU_T_AREA_INCOMPLETE;
                case 480: return ITU_T_AREA_NUMBER;
                case 483: return ITU_T_AREA_NUMBER;
                case 484: return ITU_T_AREA_NUMBER;
                case 5:   return ITU_T_AREA_INCOMPLETE;
                case 50:  return ITU_T_AREA_INCOMPLETE;
                case 500: return ITU_T_AREA_NUMBER;
                case 501: return ITU_T_AREA_NUMBER;
                case 502: return ITU_T_AREA_NUMBER;
                case 503: return ITU_T_AREA_NUMBER;
                case 504: return ITU_T_AREA_NUMBER;
                case 505: return ITU_T_AREA_NUMBER;
                case 506: return ITU_T_AREA_NUMBER;
                case 507: return ITU_T_AREA_NUMBER;
                case 508: return ITU_T_AREA_NUMBER;
                case 509: return ITU_T_AREA_NUMBER;
                case 51:  return ITU_T_AREA_INCOMPLETE;
                case 510: return ITU_T_AREA_NUMBER;
                case 512: return ITU_T_AREA_NUMBER;
                case 513: return ITU_T_AREA_NUMBER;
                case 514: return ITU_T_AREA_NUMBER;
                case 515: return ITU_T_AREA_NUMBER;
                case 516: return ITU_T_AREA_NUMBER;
                case 517: return ITU_T_AREA_NUMBER;
                case 518: return ITU_T_AREA_NUMBER;
                case 519: return ITU_T_AREA_NUMBER;
                case 52:  return ITU_T_AREA_INCOMPLETE;
                case 520: return ITU_T_AREA_NUMBER;
                case 521: return ITU_T_AREA_NUMBER;
                case 522: return ITU_T_AREA_NUMBER;
                case 523: return ITU_T_AREA_NUMBER;
                case 524: return ITU_T_AREA_NUMBER;
                case 525: return ITU_T_AREA_NUMBER;
                case 526: return ITU_T_AREA_NUMBER;
                case 527: return ITU_T_AREA_NUMBER;
                case 528: return ITU_T_AREA_NUMBER;
                case 529: return ITU_T_AREA_NUMBER;
                case 53:  return ITU_T_AREA_INCOMPLETE;
                case 530: return ITU_T_AREA_NUMBER;
                case 531: return ITU_T_AREA_NUMBER;
                case 532: return ITU_T_AREA_NUMBER;
                case 533: return ITU_T_AREA_NUMBER;
                case 534: return ITU_T_AREA_NUMBER;
                case 537: return ITU_T_AREA_NUMBER;
                case 539: return ITU_T_AREA_NUMBER;
                case 54:  return ITU_T_AREA_INCOMPLETE;
                case 540: return ITU_T_AREA_NUMBER;
                case 541: return ITU_T_AREA_NUMBER;
                case 544: return ITU_T_AREA_NUMBER;
                case 548: return ITU_T_AREA_NUMBER;
                case 55:  return ITU_T_AREA_INCOMPLETE;
                case 551: return ITU_T_AREA_NUMBER;
                case 557: return ITU_T_AREA_NUMBER;
                case 559: return ITU_T_AREA_NUMBER;
                case 56:  return ITU_T_AREA_INCOMPLETE;
                case 561: return ITU_T_AREA_NUMBER;
                case 562: return ITU_T_AREA_NUMBER;
                case 563: return ITU_T_AREA_NUMBER;
                case 564: return ITU_T_AREA_NUMBER;
                case 566: return ITU_T_AREA_NUMBER;
                case 567: return ITU_T_AREA_NUMBER;
                case 568: return ITU_T_AREA_NUMBER;
                case 57:  return ITU_T_AREA_INCOMPLETE;
                case 570: return ITU_T_AREA_NUMBER;
                case 571: return ITU_T_AREA_NUMBER;
                case 572: return ITU_T_AREA_NUMBER;
                case 573: return ITU_T_AREA_NUMBER;
                case 574: return ITU_T_AREA_NUMBER;
                case 575: return ITU_T_AREA_NUMBER;
                case 577: return ITU_T_AREA_NUMBER;
                case 579: return ITU_T_AREA_NUMBER;
                case 58:  return ITU_T_AREA_INCOMPLETE;
                case 580: return ITU_T_AREA_NUMBER;
                case 581: return ITU_T_AREA_NUMBER;
                case 582: return ITU_T_AREA_NUMBER;
                case 584: return ITU_T_AREA_NUMBER;
                case 585: return ITU_T_AREA_NUMBER;
                case 586: return ITU_T_AREA_NUMBER;
                case 587: return ITU_T_AREA_NUMBER;
                case 588: return ITU_T_AREA_NUMBER;
                case 6:   return ITU_T_AREA_INCOMPLETE;
                case 60:  return ITU_T_AREA_INCOMPLETE;
                case 600: return ITU_T_AREA_NUMBER;
                case 601: return ITU_T_AREA_NUMBER;
                case 602: return ITU_T_AREA_NUMBER;
                case 603: return ITU_T_AREA_NUMBER;
                case 604: return ITU_T_AREA_NUMBER;
                case 605: return ITU_T_AREA_NUMBER;
                case 606: return ITU_T_AREA_NUMBER;
                case 607: return ITU_T_AREA_NUMBER;
                case 608: return ITU_T_AREA_NUMBER;
                case 609: return ITU_T_AREA_NUMBER;
                case 61:  return ITU_T_AREA_INCOMPLETE;
                case 610: return ITU_T_AREA_NUMBER;
                case 612: return ITU_T_AREA_NUMBER;
                case 613: return ITU_T_AREA_NUMBER;
                case 614: return ITU_T_AREA_NUMBER;
                case 615: return ITU_T_AREA_NUMBER;
                case 616: return ITU_T_AREA_NUMBER;
                case 617: return ITU_T_AREA_NUMBER;
                case 618: return ITU_T_AREA_NUMBER;
                case 619: return ITU_T_AREA_NUMBER;
                case 62:  return ITU_T_AREA_INCOMPLETE;
                case 620: return ITU_T_AREA_NUMBER;
                case 621: return ITU_T_AREA_NUMBER;
                case 622: return ITU_T_AREA_NUMBER;
                case 623: return ITU_T_AREA_NUMBER;
                case 624: return ITU_T_AREA_NUMBER;
                case 626: return ITU_T_AREA_NUMBER;
                case 628: return ITU_T_AREA_NUMBER;
                case 629: return ITU_T_AREA_NUMBER;
                case 63:  return ITU_T_AREA_INCOMPLETE;
                case 630: return ITU_T_AREA_NUMBER;
                case 631: return ITU_T_AREA_NUMBER;
                case 633: return ITU_T_AREA_NUMBER;
                case 636: return ITU_T_AREA_NUMBER;
                case 639: return ITU_T_AREA_NUMBER;
                case 64:  return ITU_T_AREA_INCOMPLETE;
                case 640: return ITU_T_AREA_NUMBER;
                case 641: return ITU_T_AREA_NUMBER;
                case 645: return ITU_T_AREA_NUMBER;
                case 646: return ITU_T_AREA_NUMBER;
                case 647: return ITU_T_AREA_NUMBER;
                case 649: return ITU_T_AREA_NUMBER;
                case 65:  return ITU_T_AREA_INCOMPLETE;
                case 650: return ITU_T_AREA_NUMBER;
                case 651: return ITU_T_AREA_NUMBER;
                case 656: return ITU_T_AREA_NUMBER;
                case 657: return ITU_T_AREA_NUMBER;
                case 658: return ITU_T_AREA_NUMBER;
                case 659: return ITU_T_AREA_NUMBER;
                case 66:  return ITU_T_AREA_INCOMPLETE;
                case 660: return ITU_T_AREA_NUMBER;
                case 661: return ITU_T_AREA_NUMBER;
                case 662: return ITU_T_AREA_NUMBER;
                case 664: return ITU_T_AREA_NUMBER;
                case 667: return ITU_T_AREA_NUMBER;
                case 669: return ITU_T_AREA_NUMBER;
                case 67:  return ITU_T_AREA_INCOMPLETE;
                case 670: return ITU_T_AREA_NUMBER;
                case 671: return ITU_T_AREA_NUMBER;
                case 672: return ITU_T_AREA_NUMBER;
                case 678: return ITU_T_AREA_NUMBER;
                case 679: return ITU_T_AREA_NUMBER;
                case 68:  return ITU_T_AREA_INCOMPLETE;
                case 680: return ITU_T_AREA_NUMBER;
                case 681: return ITU_T_AREA_NUMBER;
                case 682: return ITU_T_AREA_NUMBER;
                case 683: return ITU_T_AREA_NUMBER;
                case 684: return ITU_T_AREA_NUMBER;
                case 686: return ITU_T_AREA_NUMBER;
                case 689: return ITU_T_AREA_NUMBER;
                case 7:   return ITU_T_AREA_INCOMPLETE;
                case 70:  return ITU_T_AREA_INCOMPLETE;
                case 701: return ITU_T_AREA_NUMBER;
                case 702: return ITU_T_AREA_NUMBER;
                case 703: return ITU_T_AREA_NUMBER;
                case 704: return ITU_T_AREA_NUMBER;
                case 705: return ITU_T_AREA_NUMBER;
                case 706: return ITU_T_AREA_NUMBER;
                case 707: return ITU_T_AREA_NUMBER;
                case 708: return ITU_T_AREA_NUMBER;
                case 709: return ITU_T_AREA_NUMBER;
                case 71:  return ITU_T_AREA_INCOMPLETE;
                case 710: return ITU_T_AREA_NUMBER;
                case 712: return ITU_T_AREA_NUMBER;
                case 713: return ITU_T_AREA_NUMBER;
                case 714: return ITU_T_AREA_NUMBER;
                case 715: return ITU_T_AREA_NUMBER;
                case 716: return ITU_T_AREA_NUMBER;
                case 717: return ITU_T_AREA_NUMBER;
                case 718: return ITU_T_AREA_NUMBER;
                case 719: return ITU_T_AREA_NUMBER;
                case 72:  return ITU_T_AREA_INCOMPLETE;
                case 720: return ITU_T_AREA_NUMBER;
                case 721: return ITU_T_AREA_NUMBER;
                case 724: return ITU_T_AREA_NUMBER;
                case 725: return ITU_T_AREA_NUMBER;
                case 726: return ITU_T_AREA_NUMBER;
                case 727: return ITU_T_AREA_NUMBER;
                case 728: return ITU_T_AREA_NUMBER;
                case 729: return ITU_T_AREA_NUMBER;
                case 73:  return ITU_T_AREA_INCOMPLETE;
                case 730: return ITU_T_AREA_NUMBER;
                case 731: return ITU_T_AREA_NUMBER;
                case 732: return ITU_T_AREA_NUMBER;
                case 734: return ITU_T_AREA_NUMBER;
                case 737: return ITU_T_AREA_NUMBER;
                case 738: return ITU_T_AREA_NUMBER;
                case 74:  return ITU_T_AREA_INCOMPLETE;
                case 740: return ITU_T_AREA_NUMBER;
                case 742: return ITU_T_AREA_NUMBER;
                case 743: return ITU_T_AREA_NUMBER;
                case 747: return ITU_T_AREA_NUMBER;
                case 748: return ITU_T_AREA_NUMBER;
                case 75:  return ITU_T_AREA_INCOMPLETE;
                case 753: return ITU_T_AREA_NUMBER;
                case 754: return ITU_T_AREA_NUMBER;
                case 757: return ITU_T_AREA_NUMBER;
                case 758: return ITU_T_AREA_NUMBER;
                case 76:  return ITU_T_AREA_INCOMPLETE;
                case 760: return ITU_T_AREA_NUMBER;
                case 762: return ITU_T_AREA_NUMBER;
                case 763: return ITU_T_AREA_NUMBER;
                case 765: return ITU_T_AREA_NUMBER;
                case 767: return ITU_T_AREA_NUMBER;
                case 769: return ITU_T_AREA_NUMBER;
                case 77:  return ITU_T_AREA_INCOMPLETE;
                case 770: return ITU_T_AREA_NUMBER;
                case 771: return ITU_T_AREA_NUMBER;
                case 772: return ITU_T_AREA_NUMBER;
                case 773: return ITU_T_AREA_NUMBER;
                case 774: return ITU_T_AREA_NUMBER;
                case 775: return ITU_T_AREA_NUMBER;
                case 778: return ITU_T_AREA_NUMBER;
                case 779: return ITU_T_AREA_NUMBER;
                case 78:  return ITU_T_AREA_INCOMPLETE;
                case 780: return ITU_T_AREA_NUMBER;
                case 781: return ITU_T_AREA_NUMBER;
                case 782: return ITU_T_AREA_NUMBER;
                case 784: return ITU_T_AREA_NUMBER;
                case 785: return ITU_T_AREA_NUMBER;
                case 786: return ITU_T_AREA_NUMBER;
                case 787: return ITU_T_AREA_NUMBER;
                case 8:   return ITU_T_AREA_INCOMPLETE;
                case 80:  return ITU_T_AREA_INCOMPLETE;
                case 800: return ITU_T_AREA_NUMBER;
                case 801: return ITU_T_AREA_NUMBER;
                case 802: return ITU_T_AREA_NUMBER;
                case 803: return ITU_T_AREA_NUMBER;
                case 804: return ITU_T_AREA_NUMBER;
                case 805: return ITU_T_AREA_NUMBER;
                case 806: return ITU_T_AREA_NUMBER;
                case 807: return ITU_T_AREA_NUMBER;
                case 808: return ITU_T_AREA_NUMBER;
                case 809: return ITU_T_AREA_NUMBER;
                case 81:  return ITU_T_AREA_INCOMPLETE;
                case 810: return ITU_T_AREA_NUMBER;
                case 812: return ITU_T_AREA_NUMBER;
                case 813: return ITU_T_AREA_NUMBER;
                case 814: return ITU_T_AREA_NUMBER;
                case 815: return ITU_T_AREA_NUMBER;
                case 816: return ITU_T_AREA_NUMBER;
                case 817: return ITU_T_AREA_NUMBER;
                case 818: return ITU_T_AREA_NUMBER;
                case 819: return ITU_T_AREA_NUMBER;
                case 82:  return ITU_T_AREA_INCOMPLETE;
                case 820: return ITU_T_AREA_NUMBER;
                case 821: return ITU_T_AREA_NUMBER;
                case 825: return ITU_T_AREA_NUMBER;
                case 826: return ITU_T_AREA_NUMBER;
                case 828: return ITU_T_AREA_NUMBER;
                case 829: return ITU_T_AREA_NUMBER;
                case 83:  return ITU_T_AREA_INCOMPLETE;
                case 830: return ITU_T_AREA_NUMBER;
                case 831: return ITU_T_AREA_NUMBER;
                case 832: return ITU_T_AREA_NUMBER;
                case 833: return ITU_T_AREA_NUMBER;
                case 835: return ITU_T_AREA_NUMBER;
                case 837: return ITU_T_AREA_NUMBER;
                case 838: return ITU_T_AREA_NUMBER;
                case 839: return ITU_T_AREA_NUMBER;
                case 84:  return ITU_T_AREA_INCOMPLETE;
                case 840: return ITU_T_AREA_NUMBER;
                case 843: return ITU_T_AREA_NUMBER;
                case 844: return ITU_T_AREA_NUMBER;
                case 845: return ITU_T_AREA_NUMBER;
                case 847: return ITU_T_AREA_NUMBER;
                case 848: return ITU_T_AREA_NUMBER;
                case 849: return ITU_T_AREA_NUMBER;
                case 85:  return ITU_T_AREA_INCOMPLETE;
                case 850: return ITU_T_AREA_NUMBER;
                case 851: return ITU_T_AREA_NUMBER;
                case 854: return ITU_T_AREA_NUMBER;
                case 855: return ITU_T_AREA_NUMBER;
                case 856: return ITU_T_AREA_NUMBER;
                case 857: return ITU_T_AREA_NUMBER;
                case 858: return ITU_T_AREA_NUMBER;
                case 859: return ITU_T_AREA_NUMBER;
                case 86:  return ITU_T_AREA_INCOMPLETE;
                case 860: return ITU_T_AREA_NUMBER;
                case 861: return ITU_T_AREA_NUMBER;
                case 862: return ITU_T_AREA_NUMBER;
                case 863: return ITU_T_AREA_NUMBER;
                case 864: return ITU_T_AREA_NUMBER;
                case 865: return ITU_T_AREA_NUMBER;
                case 866: return ITU_T_AREA_NUMBER;
                case 867: return ITU_T_AREA_NUMBER;
                case 868: return ITU_T_AREA_NUMBER;
                case 869: return ITU_T_AREA_NUMBER;
                case 87:  return ITU_T_AREA_INCOMPLETE;
                case 870: return ITU_T_AREA_NUMBER;
                case 872: return ITU_T_AREA_NUMBER;
                case 873: return ITU_T_AREA_NUMBER;
                case 876: return ITU_T_AREA_NUMBER;
                case 877: return ITU_T_AREA_NUMBER;
                case 878: return ITU_T_AREA_NUMBER;
                case 879: return ITU_T_AREA_NUMBER;
                case 88:  return ITU_T_AREA_INCOMPLETE;
                case 888: return ITU_T_AREA_NUMBER;
                case 9:   return ITU_T_AREA_INCOMPLETE;
                case 90:  return ITU_T_AREA_INCOMPLETE;
                case 900: return ITU_T_AREA_NUMBER;
                case 901: return ITU_T_AREA_NUMBER;
                case 902: return ITU_T_AREA_NUMBER;
                case 903: return ITU_T_AREA_NUMBER;
                case 904: return ITU_T_AREA_NUMBER;
                case 905: return ITU_T_AREA_NUMBER;
                case 906: return ITU_T_AREA_NUMBER;
                case 907: return ITU_T_AREA_NUMBER;
                case 908: return ITU_T_AREA_NUMBER;
                case 909: return ITU_T_AREA_NUMBER;
                case 91:  return ITU_T_AREA_INCOMPLETE;
                case 910: return ITU_T_AREA_NUMBER;
                case 912: return ITU_T_AREA_NUMBER;
                case 913: return ITU_T_AREA_NUMBER;
                case 914: return ITU_T_AREA_NUMBER;
                case 915: return ITU_T_AREA_NUMBER;
                case 916: return ITU_T_AREA_NUMBER;
                case 917: return ITU_T_AREA_NUMBER;
                case 918: return ITU_T_AREA_NUMBER;
                case 919: return ITU_T_AREA_NUMBER;
                case 92:  return ITU_T_AREA_INCOMPLETE;
                case 920: return ITU_T_AREA_NUMBER;
                case 924: return ITU_T_AREA_NUMBER;
                case 925: return ITU_T_AREA_NUMBER;
                case 928: return ITU_T_AREA_NUMBER;
                case 929: return ITU_T_AREA_NUMBER;
                case 93:  return ITU_T_AREA_INCOMPLETE;
                case 930: return ITU_T_AREA_NUMBER;
                case 931: return ITU_T_AREA_NUMBER;
                case 934: return ITU_T_AREA_NUMBER;
                case 936: return ITU_T_AREA_NUMBER;
                case 937: return ITU_T_AREA_NUMBER;
                case 938: return ITU_T_AREA_NUMBER;
                case 939: return ITU_T_AREA_NUMBER;
                case 94:  return ITU_T_AREA_INCOMPLETE;
                case 940: return ITU_T_AREA_NUMBER;
                case 941: return ITU_T_AREA_NUMBER;
                case 942: return ITU_T_AREA_NUMBER;
                case 943: return ITU_T_AREA_NUMBER;
                case 945: return ITU_T_AREA_NUMBER;
                case 947: return ITU_T_AREA_NUMBER;
                case 948: return ITU_T_AREA_NUMBER;
                case 949: return ITU_T_AREA_NUMBER;
                case 95:  return ITU_T_AREA_INCOMPLETE;
                case 951: return ITU_T_AREA_NUMBER;
                case 952: return ITU_T_AREA_NUMBER;
                case 954: return ITU_T_AREA_NUMBER;
                case 956: return ITU_T_AREA_NUMBER;
                case 959: return ITU_T_AREA_NUMBER;
                case 97:  return ITU_T_AREA_INCOMPLETE;
                case 970: return ITU_T_AREA_NUMBER;
                case 971: return ITU_T_AREA_NUMBER;
                case 972: return ITU_T_AREA_NUMBER;
                case 973: return ITU_T_AREA_NUMBER;
                case 975: return ITU_T_AREA_NUMBER;
                case 978: return ITU_T_AREA_NUMBER;
                case 979: return ITU_T_AREA_NUMBER;
                case 98:  return ITU_T_AREA_INCOMPLETE;
                case 980: return ITU_T_AREA_NUMBER;
                case 983: return ITU_T_AREA_NUMBER;
                case 984: return ITU_T_AREA_NUMBER;
                case 985: return ITU_T_AREA_NUMBER;
                case 986: return ITU_T_AREA_NUMBER;
                case 989: return ITU_T_AREA_NUMBER;
                default:  return ITU_T_AREA_UNKNOWN;
            }
        case 54: // Argentina
            switch(area_code) {
                case 0:    return ITU_T_AREA_INCOMPLETE; // Empty area code
                case 1:    return ITU_T_AREA_INCOMPLETE;
                case 11:   return ITU_T_AREA_NUMBER;
                case 2:    return ITU_T_AREA_INCOMPLETE;
                case 22:   return ITU_T_AREA_INCOMPLETE;
                case 221:  return ITU_T_AREA_NUMBER;
                case 26:   return ITU_T_AREA_INCOMPLETE;
                case 264:  return ITU_T_AREA_NUMBER;
                case 266:  return ITU_T_AREA_NUMBER;
                case 28:   return ITU_T_AREA_INCOMPLETE;
                case 280:  return ITU_T_AREA_INCOMPLETE;
                case 2804: return ITU_T_AREA_NUMBER;
                case 29:   return ITU_T_AREA_INCOMPLETE;
                case 290:  return ITU_T_AREA_INCOMPLETE;
                case 2901: return ITU_T_AREA_NUMBER;
                case 292:  return ITU_T_AREA_INCOMPLETE;
                case 2920: return ITU_T_AREA_NUMBER;
                case 295:  return ITU_T_AREA_INCOMPLETE;
                case 2954: return ITU_T_AREA_NUMBER;
                case 296:  return ITU_T_AREA_INCOMPLETE;
                case 2966: return ITU_T_AREA_NUMBER;
                case 299:  return ITU_T_AREA_NUMBER;
                case 3:    return ITU_T_AREA_INCOMPLETE;
                case 34:   return ITU_T_AREA_INCOMPLETE;
                case 342:  return ITU_T_AREA_NUMBER;
                case 343:  return ITU_T_AREA_NUMBER;
                case 35:   return ITU_T_AREA_INCOMPLETE;
                case 351:  return ITU_T_AREA_NUMBER;
                case 36:   return ITU_T_AREA_INCOMPLETE;
                case 361:  return ITU_T_AREA_NUMBER;
                case 362:  return ITU_T_AREA_NUMBER;
                case 37:   return ITU_T_AREA_INCOMPLETE;
                case 370:  return ITU_T_AREA_NUMBER;
                case 376:  return ITU_T_AREA_NUMBER;
                case 379:  return ITU_T_AREA_NUMBER;
                case 38:   return ITU_T_AREA_INCOMPLETE;
                case 380:  return ITU_T_AREA_NUMBER;
                case 381:  return ITU_T_AREA_NUMBER;
                case 383:  return ITU_T_AREA_NUMBER;
                case 385:  return ITU_T_AREA_NUMBER;
                case 387:  return ITU_T_AREA_NUMBER;
                case 388:  return ITU_T_AREA_NUMBER;
                default:   return ITU_T_AREA_UNKNOWN;
            }
        case 55: // Brazil
            switch(area_code) {
                case 0:   return ITU_T_AREA_INCOMPLETE; // Empty area code
                case 1:   return ITU_T_AREA_INCOMPLETE;
                case 11:  return ITU_T_AREA_NUMBER;
                case 12:  return ITU_T_AREA_NUMBER;
                case 13:  return ITU_T_AREA_NUMBER;
                case 14:  return ITU_T_AREA_NUMBER;
                case 15:  return ITU_T_AREA_NUMBER;
                case 16:  return ITU_T_AREA_NUMBER;
                case 17:  return ITU_T_AREA_NUMBER;
                case 18:  return ITU_T_AREA_NUMBER;
                case 19:  return ITU_T_AREA_NUMBER;
                case 2:   return ITU_T_AREA_INCOMPLETE;
                case 21:  return ITU_T_AREA_NUMBER;
                case 22:  return ITU_T_AREA_NUMBER;
                case 24:  return ITU_T_AREA_NUMBER;
                case 27:  return ITU_T_AREA_NUMBER;
                case 28:  return ITU_T_AREA_NUMBER;
                case 3:   return ITU_T_AREA_INCOMPLETE;
                case 31:  return ITU_T_AREA_NUMBER;
                case 32:  return ITU_T_AREA_NUMBER;
                case 33:  return ITU_T_AREA_NUMBER;
                case 34:  return ITU_T_AREA_NUMBER;
                case 35:  return ITU_T_AREA_NUMBER;
                case 37:  return ITU_T_AREA_NUMBER;
                case 38:  return ITU_T_AREA_NUMBER;
                case 4:   return ITU_T_AREA_INCOMPLETE;
                case 41:  return ITU_T_AREA_NUMBER;
                case 42:  return ITU_T_AREA_NUMBER;
                case 43:  return ITU_T_AREA_NUMBER;
                case 44:  return ITU_T_AREA_NUMBER;
                case 45:  return ITU_T_AREA_NUMBER;
                case 46:  return ITU_T_AREA_NUMBER;
                case 47:  return ITU_T_AREA_NUMBER;
                case 48:  return ITU_T_AREA_NUMBER;
                case 49:  return ITU_T_AREA_NUMBER;
                case 5:   return ITU_T_AREA_INCOMPLETE;
                case 51:  return ITU_T_AREA_NUMBER;
                case 53:  return ITU_T_AREA_NUMBER;
                case 54:  return ITU_T_AREA_NUMBER;
                case 55:  return ITU_T_AREA_NUMBER;
                case 6:   return ITU_T_AREA_INCOMPLETE;
                case 61:  return ITU_T_AREA_NUMBER;
                case 62:  return ITU_T_AREA_NUMBER;
                case 63:  return ITU_T_AREA_NUMBER;
                case 64:  return ITU_T_AREA_NUMBER;
                case 65:  return ITU_T_AREA_NUMBER;
                case 66:  return ITU_T_AREA_NUMBER;
                case 67:  return ITU_T_AREA_NUMBER;
                case 68:  return ITU_T_AREA_NUMBER;
                case 69:  return ITU_T_AREA_NUMBER;
                case 7:   return ITU_T_AREA_INCOMPLETE;
                case 71:  return ITU_T_AREA_NUMBER;
                case 73:  return ITU_T_AREA_NUMBER;
                case 74:  return ITU_T_AREA_NUMBER;
                case 75:  return ITU_T_AREA_NUMBER;
                case 77:  return ITU_T_AREA_NUMBER;
                case 79:  return ITU_T_AREA_NUMBER;
                case 8:   return ITU_T_AREA_INCOMPLETE;
                case 81:  return ITU_T_AREA_NUMBER;
                case 82:  return ITU_T_AREA_NUMBER;
                case 83:  return ITU_T_AREA_NUMBER;
                case 84:  return ITU_T_AREA_NUMBER;
                case 85:  return ITU_T_AREA_NUMBER;
                case 86:  return ITU_T_AREA_NUMBER;
                case 87:  return ITU_T_AREA_NUMBER;
                case 88:  return ITU_T_AREA_NUMBER;
                case 89:  return ITU_T_AREA_NUMBER;
                case 9:   return ITU_T_AREA_INCOMPLETE;
                case 91:  return ITU_T_AREA_NUMBER;
                case 92:  return ITU_T_AREA_NUMBER;
                case 93:  return ITU_T_AREA_NUMBER;
                case 94:  return ITU_T_AREA_NUMBER;
                case 95:  return ITU_T_AREA_NUMBER;
                case 96:  return ITU_T_AREA_NUMBER;
                case 97:  return ITU_T_AREA_NUMBER;
                case 98:  return ITU_T_AREA_NUMBER;
                case 99:  return ITU_T_AREA_NUMBER;
                default:  return ITU_T_AREA_UNKNOWN;
            }
        case 598: // Uruguai
            switch(area_code) {
                default:  return ITU_T_AREA_NUMBER;
            }
        default:
            return ITU_T_AREA_UNKNOWN;
    }
}
