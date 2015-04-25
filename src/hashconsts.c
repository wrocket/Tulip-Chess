// The MIT License (MIT)

// Copyright (c) 2015 Brian Wray (brian@wrocket.org)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "tulip.h"
#include "hashconsts.h"
#include "board.h"
#include "piece.h"

const uint64_t HASH_MASK_CASTLE_WK = /*Rh1*/ 0xe9f98801eded53f7 ^ /*Rf1*/ 0x394345456ac4fb80 ^ /*-f1*/ 0x70d7c44806003548 ^ /*-h1*/ 0x381c599bd1a38fd8;
const uint64_t HASH_MASK_CASTLE_WQ = /*Ra1*/ 0xfc0f0e854e8a0fcf ^ /*Rd1*/ 0x17d426030a67bc7c ^ /*-a1*/ 0x82c3e240217eb87c ^ /*-d1*/ 0x0d25aae264535842;
const uint64_t HASH_MASK_CASTLE_BK = /*rh8*/ 0x1c8a15dfafd8d934 ^ /*rf8*/ 0x66c959bbb905a316 ^ /*-h8*/ 0x5e8d1435579d024a ^ /*-f8*/ 0x57cd66545b16183c;
const uint64_t HASH_MASK_CASTLE_BQ = /*ra8*/ 0x04325c381abf132e ^ /*rd8*/ 0xd9653edc79fd29b7 ^ /*-a8*/ 0xf0aa7d665aa3a8fa ^ /*-d8*/ 0xd53bcbf41a7b69ba;

const uint64_t HASH_PIECE_CASTLE[16] = {
    0xdc0b25e9f28ae0dd,     // CASTLE_FLAG_0
    0x7ac3fac33fa2a123,     // CASTLE_FLAG_1
    0x364e1563f20096ad,     // CASTLE_FLAG_2
    0xff102361a4c6027f,     // CASTLE_FLAG_3
    0xf53df923d87bab7e,     // CASTLE_FLAG_4
    0xca97ccc0ddec42ae,     // CASTLE_FLAG_5
    0x220b2a0519e8bde4,     // CASTLE_FLAG_6
    0x9382f71f75bd7982,     // CASTLE_FLAG_7
    0x06e17f8286747b6b,     // CASTLE_FLAG_8
    0x7a9e2be499d7478e,     // CASTLE_FLAG_9
    0x932002aa9d996088,     // CASTLE_FLAG_10
    0x176084049eed89d8,     // CASTLE_FLAG_11
    0x664e9e6fb766488e,     // CASTLE_FLAG_12
    0x23c8cfccca73a4cd,     // CASTLE_FLAG_13
    0x72d91f3373bd2487,     // CASTLE_FLAG_14
    0x08a9bc2c9127f308  // CASTLE_FLAG_15
};

const uint64_t HASH_EP_FILE[9] = {
    0x8d01c42257bf29f2, // EP_FILE_A
    0x6fa938cca254dedb, // EP_FILE_B
    0x3b87c5e82eab951d, // EP_FILE_C
    0x2bfedfdaa3ef7605, // EP_FILE_D
    0x95f7f76baac0573f, // EP_FILE_E
    0x1ab5ac1e2ae62132, // EP_FILE_F
    0x39b14d95c9c2b57e, // EP_FILE_G
    0x515ed17d2e8645e6, // EP_FILE_H
    0x3594db2244a20ea3, // EP_NO_EP_FILE
};

const uint64_t HASH_PIECE_SQ[144][ORD_MAX + 1] = {
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A1 */ {/* PA1 */ 0xa6da19eaf54887d4, /* pA1 */ 0x16695de4585e6e04, /* NA1 */ 0x82f51b6073dd9014, /* nA1 */ 0x4752f2651c02a8b5, /* BA1 */ 0xc29a2a79bb86bcb1, /* bA1 */ 0x2f428c56d4cdd52d, /* RA1 */ 0xfc0f0e854e8a0fcf, /* rA1 */ 0x6756e584f583481b, /* QA1 */ 0x6c4c0595e9f8881f, /* qA1 */ 0xe6030c27625b5a08, /* KA1 */ 0x650d7d134baa8196, /* kA1 */ 0x9e095960299db5c5, /* -A1 */ 0x82c3e240217eb87c, 0x0},
    /* B1 */ {/* PB1 */ 0x993dbcb8104134c1, /* pB1 */ 0xf86ea314bcac6602, /* NB1 */ 0xdfa12bd482ca4f4d, /* nB1 */ 0x8df4fbb2a7f1dbac, /* BB1 */ 0x41fc271307101387, /* bB1 */ 0x8cef0e2b7f92ef0d, /* RB1 */ 0x6dad249871f810f5, /* rB1 */ 0xdbc3e41dc4098d28, /* QB1 */ 0xefaf35b584a2b4df, /* qB1 */ 0xa5cf2f9ab3d21c01, /* KB1 */ 0xc1d535a8ac0d8ca4, /* kB1 */ 0x01c8817f294929a9, /* -B1 */ 0x56d0a637f14d1cee, 0x0},
    /* C1 */ {/* PC1 */ 0x11724eabd4e80838, /* pC1 */ 0x91ee55ce7a69756a, /* NC1 */ 0x8c914374e901ae02, /* nC1 */ 0x5663a650ec54525b, /* BC1 */ 0x001f7e7dc6339284, /* bC1 */ 0xdbd766fc1e739655, /* RC1 */ 0xbc82aad43bd48c7e, /* rC1 */ 0x7c52c5b32fde1e03, /* QC1 */ 0xa33503501e3278fb, /* qC1 */ 0xbef93a429ff35917, /* KC1 */ 0x63dab1c7ac4a2570, /* kC1 */ 0x0990f1d570221b92, /* -C1 */ 0x0c5d579da3336099, 0x0},
    /* D1 */ {/* PD1 */ 0x8475f8928dfef443, /* pD1 */ 0xa9717215c792696f, /* ND1 */ 0x2118ffde8c3f091d, /* nD1 */ 0x545788d60197fb28, /* BD1 */ 0xc7e7f761e10b7e32, /* bD1 */ 0x1ecf0cecb1fecf93, /* RD1 */ 0x17d426030a67bc7c, /* rD1 */ 0x14e17a73cfea20e6, /* QD1 */ 0x048481df505b7264, /* qD1 */ 0xe85c63a6dd25b34a, /* KD1 */ 0x30db019b82c01d0b, /* kD1 */ 0xd16f38ee18528c23, /* -D1 */ 0x0d25aae264535842, 0x0},
    /* E1 */ {/* PE1 */ 0xbbcd1025f74580ce, /* pE1 */ 0xe9e037f1c4fa8987, /* NE1 */ 0x280a75c99a41bf55, /* nE1 */ 0x650081b79d8e85bd, /* BE1 */ 0xdb4002c13c6048e6, /* bE1 */ 0xf0d72b1b647590ad, /* RE1 */ 0x3bc70ebdf06da8fa, /* rE1 */ 0x55aab90720833a66, /* QE1 */ 0x8211fcfd39006509, /* qE1 */ 0xfa807c9c4f4ce885, /* KE1 */ 0xc8df942dcdd9d3e3, /* kE1 */ 0xeeb73cecb02d32de, /* -E1 */ 0xca56b19fbc285239, 0x0},
    /* F1 */ {/* PF1 */ 0x8cd12732c9b07b62, /* pF1 */ 0x930ee50da1dc3141, /* NF1 */ 0x12d53176c352fb82, /* nF1 */ 0xc5a83d6ff6e23b44, /* BF1 */ 0xda493eeaaeda3696, /* bF1 */ 0xa08312c3b9bb70f1, /* RF1 */ 0x394345456ac4fb80, /* rF1 */ 0xf25adfe39d0ff179, /* QF1 */ 0xe1ff7fd1d6078ef7, /* qF1 */ 0x11ecd6c73d7d1036, /* KF1 */ 0xd382af026325f8cf, /* kF1 */ 0xdaa89af472956ec5, /* -F1 */ 0x70d7c44806003548, 0x0},
    /* G1 */ {/* PG1 */ 0x99ee23f696f12a22, /* pG1 */ 0x9b82fe2e588bc2a8, /* NG1 */ 0x52197068999fa38f, /* nG1 */ 0x1486f68495f4994e, /* BG1 */ 0x3da4d0c22a824027, /* bG1 */ 0x7a793cd227e0619c, /* RG1 */ 0x641a27dd7909da95, /* rG1 */ 0x0ab784ebeb4c32f8, /* QG1 */ 0x264ac0ff16dc3d0c, /* qG1 */ 0xcb38d077090fc3d7, /* KG1 */ 0x5165bdb57f3e5d48, /* kG1 */ 0xfa43018f1250694f, /* -G1 */ 0xd6a2781d1760be4e, 0x0},
    /* H1 */ {/* PH1 */ 0xc02a2ed672c7c44b, /* pH1 */ 0xaca978f9b4c50101, /* NH1 */ 0x56ba304eab652a74, /* nH1 */ 0xb550ddf83c0a4e47, /* BH1 */ 0xcd42a806f3d30183, /* bH1 */ 0xf8017e9a54a99bba, /* RH1 */ 0xe9f98801eded53f7, /* rH1 */ 0xebf3815a05bf9661, /* QH1 */ 0x1ded8780fe0bd49a, /* qH1 */ 0xbd7b92a33f0a26ea, /* KH1 */ 0x185e3a53004ecd8b, /* kH1 */ 0xcef78ec1af9e8d52, /* -H1 */ 0x381c599bd1a38fd8, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A2 */ {/* PA2 */ 0x9c14c8cb8b16aa7c, /* pA2 */ 0x1275c630c31da04e, /* NA2 */ 0x9b2d77fd3b258f8a, /* nA2 */ 0x8c798466ffbdd7ad, /* BA2 */ 0xe0bc11c7d457c203, /* bA2 */ 0x4df0faaf7787edce, /* RA2 */ 0x6b42ccbfd1b71e51, /* rA2 */ 0x35955d02c2c760d3, /* QA2 */ 0x6776f8eef76cbab3, /* qA2 */ 0xb23cdb1f25529cbb, /* KA2 */ 0xda6d608ffe2fcf06, /* kA2 */ 0xcf9f2425f2e40d9c, /* -A2 */ 0x475bf0f7b7e0c7a9, 0x0},
    /* B2 */ {/* PB2 */ 0x59e81daeb6a9697c, /* pB2 */ 0xcf84da6a889032c3, /* NB2 */ 0xf1606c22bb4c7283, /* nB2 */ 0xc85596d0f1aa2631, /* BB2 */ 0x35d7caee667e1882, /* bB2 */ 0x73a1a2a93b2527d8, /* RB2 */ 0x7e3c95aa09b2bcec, /* rB2 */ 0xd5a42815762c8358, /* QB2 */ 0xa3e7fc823de354d9, /* qB2 */ 0x8a37638d28f3ca3a, /* KB2 */ 0x483c129bf2a6ede0, /* kB2 */ 0x27609e930111fbca, /* -B2 */ 0x817ef412e81c2acd, 0x0},
    /* C2 */ {/* PC2 */ 0xff2626013f088f67, /* pC2 */ 0x95e4cf3f9e6674df, /* NC2 */ 0xb2004d087bfb0c32, /* nC2 */ 0xa688630b0f63dbd8, /* BC2 */ 0x0148250e3ee15cbb, /* bC2 */ 0x540100420b9149d8, /* RC2 */ 0xc41f3ae1e54fc8b8, /* rC2 */ 0xcee3346f8e966e0f, /* QC2 */ 0xa8037660243b1baf, /* qC2 */ 0x4bf1a3a6e7d57775, /* KC2 */ 0x76f2bfe57aa3d217, /* kC2 */ 0xf944f41712733323, /* -C2 */ 0x1a2e477395fab598, 0x0},
    /* D2 */ {/* PD2 */ 0x1088ab4cb112a8b8, /* pD2 */ 0xb49ff0d3912ec419, /* ND2 */ 0xc1d671a953464814, /* nD2 */ 0x6b1f806b0af9bcc1, /* BD2 */ 0xf52fea20298e45fd, /* bD2 */ 0xf00e4004e3c1fcc3, /* RD2 */ 0x9c73257d12184397, /* rD2 */ 0x30a05fe6d39d6851, /* QD2 */ 0x34f3181e9bfd5e97, /* qD2 */ 0xbb49b4e1ade7c273, /* KD2 */ 0x44637d1b8ae5446f, /* kD2 */ 0xc12543fb843d5f0b, /* -D2 */ 0xefabb53e5dd58957, 0x0},
    /* E2 */ {/* PE2 */ 0x5cd2197fed4d1305, /* pE2 */ 0x03e374d326a6f9a9, /* NE2 */ 0x0a1698821e9ba837, /* nE2 */ 0xea0fe2834876e4b4, /* BE2 */ 0xc8893e7b41c2b330, /* bE2 */ 0x31e5ebfe851b4af9, /* RE2 */ 0xa6f0dd00107c67d0, /* rE2 */ 0x5790e9780f4ce792, /* QE2 */ 0xed1f0754d1dd4d69, /* qE2 */ 0x7a17dc1a7c0c6a61, /* KE2 */ 0xe6329546324fa8a9, /* kE2 */ 0x07287dc48aaf613f, /* -E2 */ 0x32bc537ff49882f5, 0x0},
    /* F2 */ {/* PF2 */ 0x0bd9c209acda8429, /* pF2 */ 0xc7e93bcb17fddadd, /* NF2 */ 0x33002beb525ebc18, /* nF2 */ 0xa1efc8ebdcd326bc, /* BF2 */ 0xb69c741786ad1850, /* bF2 */ 0x9c7f81d674448dfa, /* RF2 */ 0xa5574765242bb6c7, /* rF2 */ 0xf848b75931bf7955, /* QF2 */ 0x61679f73e2759cf6, /* qF2 */ 0x3d44f00b8fedb1d5, /* KF2 */ 0xdc9b9a88268986a6, /* kF2 */ 0xe73488671aadc058, /* -F2 */ 0x5dc9d74a7ac8e1a7, 0x0},
    /* G2 */ {/* PG2 */ 0xa83a85a3a32f4ff1, /* pG2 */ 0xae4abb625e69911b, /* NG2 */ 0x61fac134889e199f, /* nG2 */ 0x3d86d9e00878fa80, /* BG2 */ 0x0f2aa063ba924d92, /* bG2 */ 0x2bb2898fa383a698, /* RG2 */ 0xf50018120186f30f, /* rG2 */ 0xc82a2db1c24ada43, /* QG2 */ 0xdf532770ae27a9cf, /* qG2 */ 0x69a5f94ae92d38de, /* KG2 */ 0xb7fe20383d52b383, /* kG2 */ 0xc1be44c30f4be144, /* -G2 */ 0x790cdaf006b20212, 0x0},
    /* H2 */ {/* PH2 */ 0x90929e7068a86475, /* pH2 */ 0x28b158836d81dbe4, /* NH2 */ 0xe40de7371a3604a8, /* nH2 */ 0xa27e7d1473ca8a53, /* BH2 */ 0x572591b7a582eceb, /* bH2 */ 0xd480a3b46938dbf3, /* RH2 */ 0x76ae5b205f2a0ce5, /* rH2 */ 0xdf77294404b97768, /* QH2 */ 0x4f02f40f8de5402a, /* qH2 */ 0x3b343f34008a4fc1, /* KH2 */ 0x94159788a7c6ade6, /* kH2 */ 0xc32d97336a1646fb, /* -H2 */ 0x030bbce86b62bf3a, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A3 */ {/* PA3 */ 0x90a266509f1f9c90, /* pA3 */ 0x386950d06fb38510, /* NA3 */ 0x40e01a760a2bc4cf, /* nA3 */ 0x0ae3ae1278899a14, /* BA3 */ 0x233df36e88258678, /* bA3 */ 0x4aba86d5b44b7128, /* RA3 */ 0x211ef5102899395f, /* rA3 */ 0x103cf8241698c6d4, /* QA3 */ 0x41eaa162197f234a, /* qA3 */ 0x86897e7fd1582cff, /* KA3 */ 0xa7f94e104fefd0be, /* kA3 */ 0x9f538865794598a0, /* -A3 */ 0x8adbc9da3c9b4ee3, 0x0},
    /* B3 */ {/* PB3 */ 0x446c02f9e38cb732, /* pB3 */ 0x84dceb56ee220997, /* NB3 */ 0x1d5261476b6e4fc0, /* nB3 */ 0xa87af7bd9295b83c, /* BB3 */ 0x1281b10ee59fe48d, /* bB3 */ 0xa810e7ce7351a918, /* RB3 */ 0xf3cd7a026bfea5bf, /* rB3 */ 0x5a65d849112eb8f8, /* QB3 */ 0xb23fe2851af11c0b, /* qB3 */ 0x423316ba83a185b5, /* KB3 */ 0xfbcc00204c1100c1, /* kB3 */ 0xa065b03b4eaaee37, /* -B3 */ 0x577f452b5eb3fc01, 0x0},
    /* C3 */ {/* PC3 */ 0x491e28591a641caf, /* pC3 */ 0xe0e93d2427373782, /* NC3 */ 0x014964aa89a85f2a, /* nC3 */ 0x033adefddd797abf, /* BC3 */ 0x2376189562cb8350, /* bC3 */ 0xed51feff097f87f4, /* RC3 */ 0x8ccf3edea9a63c51, /* rC3 */ 0x5943d38d88ce225f, /* QC3 */ 0xdc3eb3774db4a111, /* qC3 */ 0x035b5fc2f89859e5, /* KC3 */ 0x7d1d69b4b5dd9730, /* kC3 */ 0x376e2a5f8202cb43, /* -C3 */ 0x81a208c18cce2da0, 0x0},
    /* D3 */ {/* PD3 */ 0xf8f946e924579a31, /* pD3 */ 0x6f88694ad1072fe3, /* ND3 */ 0x7d94dedbcd9cb861, /* nD3 */ 0x8221c10b70c64c6f, /* BD3 */ 0x3badc2b0ab8c98a7, /* bD3 */ 0x4e2c715ddc924dc2, /* RD3 */ 0xc9022e58e2ae6b67, /* rD3 */ 0x346c4ed0f3cda7b7, /* QD3 */ 0x6117223f687d3f83, /* qD3 */ 0x2d0ca0d7a4fec27f, /* KD3 */ 0x7a71c93b768600ed, /* kD3 */ 0x9d0ceef73d797698, /* -D3 */ 0x1c09df9192cd63da, 0x0},
    /* E3 */ {/* PE3 */ 0x80a7e1b4393b9a8b, /* pE3 */ 0xa57f0cc1cf033eed, /* NE3 */ 0x8455a1bc74940f94, /* nE3 */ 0x64d7fda95b60d0e3, /* BE3 */ 0x02d3163a8a764820, /* bE3 */ 0x0187bd9ca0ed22b0, /* RE3 */ 0x047b42308b2678d4, /* rE3 */ 0xf2752ae3e168f28b, /* QE3 */ 0xe88c205781176323, /* qE3 */ 0xb3a956affa14ad71, /* KE3 */ 0xa50515b134f8c475, /* kE3 */ 0x8bf886d2ad3c11db, /* -E3 */ 0xef687123e75b7a8b, 0x0},
    /* F3 */ {/* PF3 */ 0x7fa1fe6352c95e0d, /* pF3 */ 0xc8c9331ac7c2a8af, /* NF3 */ 0x06b2dd02042ffb61, /* nF3 */ 0xfd25acec9105e1a9, /* BF3 */ 0xed6891661d45f38b, /* bF3 */ 0x1bafc4530a40085d, /* RF3 */ 0x086b8e09536f748a, /* rF3 */ 0xf7e6c7a0ae5715a4, /* QF3 */ 0xe390634e9bfaa100, /* qF3 */ 0x142df48a311ce8ac, /* KF3 */ 0x1e68c197f6d8a235, /* kF3 */ 0x501dd51e1014d8c5, /* -F3 */ 0xae995d324fcacaab, 0x0},
    /* G3 */ {/* PG3 */ 0xfba8a06fbd01db77, /* pG3 */ 0xc3922bb3577cbb5e, /* NG3 */ 0x8b42169bb35598b3, /* nG3 */ 0xf2337a91dd5ad4ab, /* BG3 */ 0x1286305d3f09c603, /* bG3 */ 0xe537ba91aa3b9744, /* RG3 */ 0xa4966d2b9fb8e69b, /* rG3 */ 0x1085bc9fdd0ef2fd, /* QG3 */ 0xcf28d0b49e0fd730, /* qG3 */ 0xde12ca38803ff1ac, /* KG3 */ 0x540e5813c88faf7f, /* kG3 */ 0x702f50a2de68f0a4, /* -G3 */ 0x6b0dcd314331f467, 0x0},
    /* H3 */ {/* PH3 */ 0x21eb7a2c93d9bc13, /* pH3 */ 0x0849ff97c92fe431, /* NH3 */ 0xc758d339326782b8, /* nH3 */ 0x8b7d927bf756d176, /* BH3 */ 0xc8137807a55c64b9, /* bH3 */ 0xc8b1e93b2a44e2e8, /* RH3 */ 0xf59e4c4bd7599a53, /* rH3 */ 0x0b7a7c4327367dff, /* QH3 */ 0x501e1fb31b153b28, /* qH3 */ 0x38a7da7aaad42bd9, /* KH3 */ 0xce93fd681d2c664a, /* kH3 */ 0xa6957faf4789ae3d, /* -H3 */ 0xd1a23ae6427f1f01, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A4 */ {/* PA4 */ 0xfe77fd04da72f2c9, /* pA4 */ 0x1612a942aa0d0ad6, /* NA4 */ 0xf13f57abf2d3b2b1, /* nA4 */ 0x6819b6457867ae68, /* BA4 */ 0x14ea499ddb5e4a2a, /* bA4 */ 0x3f402378c36e4a31, /* RA4 */ 0xe9c75e415cd9c0a6, /* rA4 */ 0x7d106468497b4472, /* QA4 */ 0x1520f983ff0665ae, /* qA4 */ 0xa43f760ed504c449, /* KA4 */ 0x40ac6099dea4a441, /* kA4 */ 0xa842c492eeb78aa0, /* -A4 */ 0x86453125df7a938a, 0x0},
    /* B4 */ {/* PB4 */ 0x0f7beb7525c7f542, /* pB4 */ 0x50a54b9c6911acec, /* NB4 */ 0x807f03f41ddb200b, /* nB4 */ 0x467aa8ffa9d996ad, /* BB4 */ 0x69d76630cbb11a91, /* bB4 */ 0xd49bdb17d4936eb2, /* RB4 */ 0x029d02d14ad1cc22, /* rB4 */ 0x80e9d3ee9254a635, /* QB4 */ 0xe93dc695aaed9c49, /* qB4 */ 0x5db52b62c4ccca95, /* KB4 */ 0xd3ea8ad7f42beafb, /* kB4 */ 0x2a381a9ff8668c9a, /* -B4 */ 0xeb76527c41c7555a, 0x0},
    /* C4 */ {/* PC4 */ 0x02fa64c968dd1116, /* pC4 */ 0xe9c12be79ce902bd, /* NC4 */ 0x240ab1cb296ec605, /* nC4 */ 0x5d11e604b7e2fa8b, /* BC4 */ 0x55395e4c2c1b0e3c, /* bC4 */ 0xf5e3dbb53669a1a3, /* RC4 */ 0x4b82ab177f72855d, /* rC4 */ 0x0187b953771d25e4, /* QC4 */ 0x4cf17ca889590e6e, /* qC4 */ 0x9be32102129dc09e, /* KC4 */ 0x39033265458e579d, /* kC4 */ 0x28ae9e9763ec626f, /* -C4 */ 0x620a3972d8fd6daf, 0x0},
    /* D4 */ {/* PD4 */ 0x76b3f0c7dd40d025, /* pD4 */ 0xab52c40e6263bd9e, /* ND4 */ 0xabc1651a7be06097, /* nD4 */ 0x77c9b70ee2e5db10, /* BD4 */ 0xd52956e40f08dc0d, /* bD4 */ 0x6108f6168afc83bb, /* RD4 */ 0x170f234e6d986074, /* rD4 */ 0x604fa2206d012fdc, /* QD4 */ 0x5bf96884b7e439bf, /* qD4 */ 0x8b5647aa69625926, /* KD4 */ 0x092faa861a83c0fd, /* kD4 */ 0x4d676c05ff7d9d98, /* -D4 */ 0x11768514ceb993d2, 0x0},
    /* E4 */ {/* PE4 */ 0x71e5e0e785b6d05a, /* pE4 */ 0xa4e3fd561eed83c3, /* NE4 */ 0x89b70f2918ffd73d, /* nE4 */ 0xe33ae9838841e551, /* BE4 */ 0x31318f98c1df846e, /* bE4 */ 0xd80130e444756e0e, /* RE4 */ 0x27341f958ac131f3, /* rE4 */ 0x7a7e96b1eaf5d51f, /* QE4 */ 0xffea10d2ef24f075, /* qE4 */ 0x8300b25db7adacb3, /* KE4 */ 0x202286fb731541cb, /* kE4 */ 0x383ca1ef12711fee, /* -E4 */ 0x41fc287803437931, 0x0},
    /* F4 */ {/* PF4 */ 0xc97d1c5c332fcfc0, /* pF4 */ 0xa7fd93aa02210093, /* NF4 */ 0x9960b5b4e180cedd, /* nF4 */ 0x1fc8a8dabde4cfea, /* BF4 */ 0x7dd469cc3e7d30a9, /* bF4 */ 0xa5d855faced97e6f, /* RF4 */ 0x59deea03cf46ed20, /* rF4 */ 0x5a874767b76d979b, /* QF4 */ 0x50c8e1afd0e9b7d3, /* qF4 */ 0xc47eff842ddc9ae7, /* KF4 */ 0x395d3bd769b010cd, /* kF4 */ 0x0ff598f83b2423a0, /* -F4 */ 0xd17afd11e148e2db, 0x0},
    /* G4 */ {/* PG4 */ 0x24a67adafe8ed60d, /* pG4 */ 0x2457f51f39b6e4bd, /* NG4 */ 0x0ab1795f6e27a76e, /* nG4 */ 0x7a72ce4d265595c0, /* BG4 */ 0x2acc0951345d470a, /* bG4 */ 0x33457798e7974e3a, /* RG4 */ 0xf92405e2d186732b, /* rG4 */ 0xde3a9af295628450, /* QG4 */ 0x529cd07b057524bc, /* qG4 */ 0xd6b3f7c424fdbaf3, /* KG4 */ 0x80c051b040dd680e, /* kG4 */ 0x28f24c7329ba082a, /* -G4 */ 0xc5cff12d27aaa77c, 0x0},
    /* H4 */ {/* PH4 */ 0x57c369c39163bc23, /* pH4 */ 0x60998f680aa93d6c, /* NH4 */ 0x2c25f0d8722db832, /* nH4 */ 0x4ccb5641466b6767, /* BH4 */ 0x57bdf89164a752c4, /* bH4 */ 0x6ba34ccd17ca1ad4, /* RH4 */ 0x3dc6291dd402dad3, /* rH4 */ 0x39dbb6a25af25ae4, /* QH4 */ 0xe9b4710674502ec4, /* qH4 */ 0x6d92471310861877, /* KH4 */ 0x89ebf8cfadf154d9, /* kH4 */ 0x29785cc4fc78fcc1, /* -H4 */ 0xdbff79345e71a81e, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A5 */ {/* PA5 */ 0x423e4816140f4ece, /* pA5 */ 0xb3f91b5d044ab00f, /* NA5 */ 0x3439d262b949c465, /* nA5 */ 0xac2ae3ad09ee58a3, /* BA5 */ 0x8e5dc78b970e4137, /* bA5 */ 0x425dc9166b1a48e9, /* RA5 */ 0x3fb8ba763af8a2c6, /* rA5 */ 0x2580a22732107bb3, /* QA5 */ 0x8e1dcc1f24366812, /* qA5 */ 0xabb8cd4802185336, /* KA5 */ 0x4752e633c346e896, /* kA5 */ 0x9e6f023685ecfbcb, /* -A5 */ 0xe3a8e9fcac837aa2, 0x0},
    /* B5 */ {/* PB5 */ 0xc963d77a0d72a711, /* pB5 */ 0xb3e4a00987c50b1c, /* NB5 */ 0xb4a6cd3df66c58c9, /* nB5 */ 0x0e18fcee8c918054, /* BB5 */ 0xfbe671bb26d5300b, /* bB5 */ 0x97d06e35e9a3b535, /* RB5 */ 0x4e79597529e44e29, /* rB5 */ 0x7c42b5057cd9d6cb, /* QB5 */ 0x9e168c08932c4580, /* qB5 */ 0xcaad92079a1bb4bd, /* KB5 */ 0x6f6ea01a6eaa3924, /* kB5 */ 0x9af23494a3df9ab2, /* -B5 */ 0x2c612d97d735e398, 0x0},
    /* C5 */ {/* PC5 */ 0x9841a04cef086198, /* pC5 */ 0x715f95e0695053cb, /* NC5 */ 0x7255bc1ef71bd3fe, /* nC5 */ 0xcbe3cc831f01e489, /* BC5 */ 0x90c449601d0b9b7a, /* bC5 */ 0x715d6b3f8dcdc8c5, /* RC5 */ 0xb5de2ec057d94635, /* rC5 */ 0xd2e41701e9a7b5eb, /* QC5 */ 0x7b173779cba59fa3, /* qC5 */ 0x24bfc3f2c957229e, /* KC5 */ 0x413f8e6634826d57, /* kC5 */ 0x7750a594b98a382a, /* -C5 */ 0xaf0ca2c342b822a5, 0x0},
    /* D5 */ {/* PD5 */ 0xbd3e111d6a21717f, /* pD5 */ 0x717f9b0c15a82bac, /* ND5 */ 0xba43352ab9f3a9b0, /* nD5 */ 0x8e45bdef14b39763, /* BD5 */ 0x919ea256d4d3d030, /* bD5 */ 0xaef96f4a3d416289, /* RD5 */ 0xa37dd5a850aa900b, /* rD5 */ 0x08b62795e4e9cc67, /* QD5 */ 0x0b4dda7608276495, /* qD5 */ 0x85802575371ba679, /* KD5 */ 0xf19f5ba538270459, /* kD5 */ 0x1dbf5b38e075ce2e, /* -D5 */ 0xcf63726a1930d99a, 0x0},
    /* E5 */ {/* PE5 */ 0xc65a04647d473eaf, /* pE5 */ 0xe234a855ebd3ef7a, /* NE5 */ 0xcf8521a38e808995, /* nE5 */ 0x94662c6f1158f14a, /* BE5 */ 0x8ed4ece7c99ef402, /* bE5 */ 0xc8cc701bc61b7d3f, /* RE5 */ 0xc8e39f5deb4811aa, /* rE5 */ 0x7cd91ddc4e872038, /* QE5 */ 0x332c229e70d4c9bb, /* qE5 */ 0x94254b28fef2401f, /* KE5 */ 0x6bd9305663030c71, /* kE5 */ 0x49f227d142efa069, /* -E5 */ 0x787d82c409752495, 0x0},
    /* F5 */ {/* PF5 */ 0xe92ee3e1db499b4e, /* pF5 */ 0xb14aa9d650c44a4d, /* NF5 */ 0x87b459bede04dbaf, /* nF5 */ 0x856200c115561b8f, /* BF5 */ 0xb2e4b29d84855d9e, /* bF5 */ 0xa1cb92d812a629f7, /* RF5 */ 0xed49466d6f00f1aa, /* rF5 */ 0x3ce755c9f5e2e996, /* QF5 */ 0x0e82c21732340d28, /* qF5 */ 0x1f8c8f8745df4ac0, /* KF5 */ 0x66030244966d51f7, /* kF5 */ 0x79ecc80ec955d2ab, /* -F5 */ 0x6ab5bf0c3a028cf6, 0x0},
    /* G5 */ {/* PG5 */ 0x9fed28ab11200941, /* pG5 */ 0x6e910bf8f001a09c, /* NG5 */ 0x977e9e8ff3ee12d1, /* nG5 */ 0xfb1196d34b4825c6, /* BG5 */ 0xbb747e0bcf13b714, /* bG5 */ 0x7616a29918fe227b, /* RG5 */ 0xab1a5c8f666437ef, /* rG5 */ 0x7b9d0cc4b955f63a, /* QG5 */ 0xa75acec894ca59b8, /* qG5 */ 0x94de48855d99bc72, /* KG5 */ 0xdcd75f411a79804b, /* kG5 */ 0x70bca933c0ddb17b, /* -G5 */ 0x2bc6e8929fa6e9f5, 0x0},
    /* H5 */ {/* PH5 */ 0xd67c73ae2df876eb, /* pH5 */ 0xeeb762565e58010d, /* NH5 */ 0xfe985e3e18a3f8c1, /* nH5 */ 0xe7735f29ae0f0e2e, /* BH5 */ 0x3c670981c8ca21cd, /* bH5 */ 0xf7d4735670b70eb4, /* RH5 */ 0x480458e59b1f7e78, /* rH5 */ 0x6f70d1a3b08b83ef, /* QH5 */ 0x511445fcc2f190dc, /* qH5 */ 0x27d7bb5793cdb832, /* KH5 */ 0x70c5f5b47db63325, /* kH5 */ 0xae203089c640f6ce, /* -H5 */ 0x746b84ede2897359, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A6 */ {/* PA6 */ 0xee2c8b749fedbaaa, /* pA6 */ 0x2fca11d28607a738, /* NA6 */ 0x6a3efe90b2340a0e, /* nA6 */ 0x6e7ffe4ebeef9b3b, /* BA6 */ 0x807f04c9106ee74e, /* bA6 */ 0x6327aa762b598dbf, /* RA6 */ 0xc3eb8afc94e75fa4, /* rA6 */ 0x27329b501d437868, /* QA6 */ 0xb67cfcfd2ac67b5a, /* qA6 */ 0xc00b6cd25b61409b, /* KA6 */ 0x1e38b6d6222a673e, /* kA6 */ 0x86467c856bc8a14f, /* -A6 */ 0xa2b5baf21dfb6fce, 0x0},
    /* B6 */ {/* PB6 */ 0x3f58010f07adbafd, /* pB6 */ 0x9a088095ae4526a7, /* NB6 */ 0xa262ea2d98b45eb5, /* nB6 */ 0x6123b4e7898fc42f, /* BB6 */ 0x93d6f6e919953cf9, /* bB6 */ 0x7052be720c2c6e6b, /* RB6 */ 0x3aaa3e3844f7bd26, /* rB6 */ 0x94f54249d40b01b1, /* QB6 */ 0x924e14fe9bba0cdd, /* qB6 */ 0x4c0b280a22c1e541, /* KB6 */ 0x5053db867d5a6c60, /* kB6 */ 0x5500ac4a15830301, /* -B6 */ 0x288833f90b42a2b5, 0x0},
    /* C6 */ {/* PC6 */ 0xa468ced9e07f9321, /* pC6 */ 0x34f1cd4d24613809, /* NC6 */ 0xaaf5a4c9d0cc4b0f, /* nC6 */ 0xe5881bf2620d74f3, /* BC6 */ 0x8fe520a5bba530f6, /* bC6 */ 0xb3d13c7327186a25, /* RC6 */ 0x05b89c28bfc9aa50, /* rC6 */ 0xa6aa92ada3a08981, /* QC6 */ 0x2e7979743122fcdf, /* qC6 */ 0x913c44edb52b1075, /* KC6 */ 0x20fa3c7d926dc357, /* kC6 */ 0x2f47eee58ce9289f, /* -C6 */ 0x6f12d8cfdbda807f, 0x0},
    /* D6 */ {/* PD6 */ 0x297dfd1c804ea475, /* pD6 */ 0xf44d694e86fee616, /* ND6 */ 0x8d53ed7253ebb7c7, /* nD6 */ 0xa9b0bc3ba73f0118, /* BD6 */ 0xec4d0da260ac7b91, /* bD6 */ 0xd6f8628ee8b98566, /* RD6 */ 0x2b27942d6ae0ae55, /* rD6 */ 0x4b5f55a45b00e30f, /* QD6 */ 0x790ecd6e96fb88ac, /* qD6 */ 0x71580dbda99e6735, /* KD6 */ 0x07af1c05be3a4237, /* kD6 */ 0x34bcbd1bc819891d, /* -D6 */ 0x40d7b0b2df5cfe67, 0x0},
    /* E6 */ {/* PE6 */ 0xc5344e158fd9c0d0, /* pE6 */ 0xaadefdc56ac3fb1c, /* NE6 */ 0xc3497c2d10e5442a, /* nE6 */ 0xe0da3e51e2b8080e, /* BE6 */ 0x098b1584905a8ece, /* bE6 */ 0x21d06f7d22ac018e, /* RE6 */ 0x23cd35509c12577e, /* rE6 */ 0x7534626b2d7c2702, /* QE6 */ 0xb14799527eb398c6, /* qE6 */ 0xd745b7a9fbb59b04, /* KE6 */ 0x4e3c3bfcdcc389bb, /* kE6 */ 0xc9871b808f22d1a6, /* -E6 */ 0xdcc479d91e5d708c, 0x0},
    /* F6 */ {/* PF6 */ 0x344769ae84ee9c6b, /* pF6 */ 0x27670514b5c8d3ce, /* NF6 */ 0xbcc67ef227e8ef31, /* nF6 */ 0x42cfe37b3f9f1fd0, /* BF6 */ 0x8f786387626ae8d8, /* bF6 */ 0x95c6cd3fe044fcef, /* RF6 */ 0xecb748ed36238277, /* rF6 */ 0x3dd2cd48fc40c6d2, /* QF6 */ 0x270f7a138aee118a, /* qF6 */ 0x23d70f1f54ef1b76, /* KF6 */ 0x124fa16e6affb220, /* kF6 */ 0x81f364d65057c8b2, /* -F6 */ 0xff8a6de2f5c03d87, 0x0},
    /* G6 */ {/* PG6 */ 0x5fa067f365c73ba0, /* pG6 */ 0xe8cc81387277da75, /* NG6 */ 0x867b7850ac502efe, /* nG6 */ 0x0325d9a5059cd069, /* BG6 */ 0x0667e1ae388d237e, /* bG6 */ 0x5be34d462dfe7479, /* RG6 */ 0xa08804f2af5d4681, /* rG6 */ 0xe5bed2b781a73e53, /* QG6 */ 0xe7d6308d7fc7fc9e, /* qG6 */ 0x939772062e5990da, /* KG6 */ 0xffd6038a105e6b63, /* kG6 */ 0x32b4d8404124832c, /* -G6 */ 0xd644ca6991b1e6fa, 0x0},
    /* H6 */ {/* PH6 */ 0xf36a41aad8962910, /* pH6 */ 0xeade61c954d4f4f9, /* NH6 */ 0x524ce2de62bf67ac, /* nH6 */ 0xb19bc8b08236d33f, /* BH6 */ 0x2ef696b0fa062fb8, /* bH6 */ 0x098b94cd9b7e1a21, /* RH6 */ 0xd99f92eb6762a97f, /* rH6 */ 0x4b59caedb823767a, /* QH6 */ 0xeb2ba601b7ac6216, /* qH6 */ 0xf96dd677cd1ca5b0, /* KH6 */ 0xd12dfc328b76067c, /* kH6 */ 0xf457a3fbbc577073, /* -H6 */ 0x94a141b2f33771ff, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A7 */ {/* PA7 */ 0xfb9e0e0f0ae868a4, /* pA7 */ 0x2dc293f0cc5a2d21, /* NA7 */ 0x50af34fd64951900, /* nA7 */ 0x18f8b5133e42ef73, /* BA7 */ 0xdf1f1da4dd3f7d3e, /* bA7 */ 0x132b36f30cd2fd57, /* RA7 */ 0x1ebc69b4b49994f6, /* rA7 */ 0xcf3ae6995915a1e8, /* QA7 */ 0xf8f2ce213932d464, /* qA7 */ 0x400251432d814a5b, /* KA7 */ 0x286c39b5ffbe0bf9, /* kA7 */ 0x2da35ca681b4e6a2, /* -A7 */ 0xcfa2125d8bc81998, 0x0},
    /* B7 */ {/* PB7 */ 0x48b1cedca37fe2a2, /* pB7 */ 0x0de3ca1d549a6741, /* NB7 */ 0xe7ace8292b7fb0c3, /* nB7 */ 0xcdafe68baa664aa0, /* BB7 */ 0xdf24e7b83b732c98, /* bB7 */ 0x817f9517fdcfdad8, /* RB7 */ 0x92bc14f455947368, /* rB7 */ 0x38fe0b645410016c, /* QB7 */ 0xff1ebb4939070ff9, /* qB7 */ 0xf78989c66d74a750, /* KB7 */ 0x806e8911dcc17d14, /* kB7 */ 0x15d7a18d8d37fb65, /* -B7 */ 0x534da3de25e83c8d, 0x0},
    /* C7 */ {/* PC7 */ 0x7e0b080ede181ccf, /* pC7 */ 0x73d19e54972a054b, /* NC7 */ 0xc7d29bad908738c2, /* nC7 */ 0xc556cf5852cf7324, /* BC7 */ 0x459d20c4899f435c, /* bC7 */ 0x0fb6ad9cbf82d1f2, /* RC7 */ 0x6c4c96894da4f2df, /* rC7 */ 0x80edf576b434d0cf, /* QC7 */ 0xa22de2b6bb9f430a, /* qC7 */ 0x83b9c0a9d8b480aa, /* KC7 */ 0x2a6c39ba43044da7, /* kC7 */ 0xaea92c66d8145b2e, /* -C7 */ 0xae7640233ccb8384, 0x0},
    /* D7 */ {/* PD7 */ 0xf2e81998d7a2b55f, /* pD7 */ 0xc7f4bc21868321dc, /* ND7 */ 0xad594b916a461d9c, /* nD7 */ 0x026860ec23f99e65, /* BD7 */ 0xa60ab927b6b19afc, /* bD7 */ 0x228962d3c2218b66, /* RD7 */ 0x382f6deb3aada583, /* rD7 */ 0xaf270c065f6880d0, /* QD7 */ 0x4ed2ca1c8757be1a, /* qD7 */ 0x75b753aa9757838a, /* KD7 */ 0xada55de0e17f93c8, /* kD7 */ 0x7941ccd4bdfa9692, /* -D7 */ 0x428ae83109059034, 0x0},
    /* E7 */ {/* PE7 */ 0xedd2c82b7bc790d6, /* pE7 */ 0x3506754b49cd20cc, /* NE7 */ 0x8d05518ddb818cbe, /* nE7 */ 0x7dcf7f2a0fcfad84, /* BE7 */ 0x11bc1b6d6459da5e, /* bE7 */ 0x0862c6a9d4c9f017, /* RE7 */ 0x5779aa593e47cdd8, /* rE7 */ 0x861d5f932b37f78c, /* QE7 */ 0xb3d4a9330c10bd14, /* qE7 */ 0x8ecfbff515b4d567, /* KE7 */ 0xa41db3a7f9814eb2, /* kE7 */ 0xe7ff6db1e7ecd4eb, /* -E7 */ 0x0d97e0e096316f07, 0x0},
    /* F7 */ {/* PF7 */ 0x368ea23529ec029b, /* pF7 */ 0xadfbbc8d0f20dc2e, /* NF7 */ 0x4c6b76b7ba21073c, /* nF7 */ 0x5b976d32bfa40db7, /* BF7 */ 0x5c140cc4790b1319, /* bF7 */ 0x34240bb77d1cf9ba, /* RF7 */ 0x958ffec668a87afb, /* rF7 */ 0x1bac5238886762ea, /* QF7 */ 0x41131d42dc241bc4, /* qF7 */ 0xd4be4642b6e91135, /* KF7 */ 0x828ff23f31acecfe, /* kF7 */ 0x2562d26433277331, /* -F7 */ 0x90aea375fadcdc24, 0x0},
    /* G7 */ {/* PG7 */ 0xbb225c9a09b08881, /* pG7 */ 0x088dbf340213573c, /* NG7 */ 0x448265e3ec197ad3, /* nG7 */ 0x7c606f38c7368a3b, /* BG7 */ 0x505a1fd20522c0bd, /* bG7 */ 0x52327ba37ecff4ce, /* RG7 */ 0x6a313fe036a71f50, /* rG7 */ 0x01db5654b7e96f66, /* QG7 */ 0xeb61eb73ba54502d, /* qG7 */ 0x73acbd189c55c5c0, /* KG7 */ 0x7ede54b668f4d6f7, /* kG7 */ 0x1682cc21589077b7, /* -G7 */ 0xa5bb2da973507b10, 0x0},
    /* H7 */ {/* PH7 */ 0x74157d3cf2380d29, /* pH7 */ 0x31f1a5faa46b3010, /* NH7 */ 0xe600637d2f3b59ca, /* nH7 */ 0xc4dc4b35fd6c4b1e, /* BH7 */ 0xb51a6ec0083bf95e, /* bH7 */ 0xde1b4dc2a031d2ab, /* RH7 */ 0x12a2df280c1ac784, /* rH7 */ 0xec99b3b03f612fe3, /* QH7 */ 0x68c2a6805fbd92e7, /* qH7 */ 0xe34df8656e6b2bb3, /* KH7 */ 0x2c0cd3711bfa6372, /* kH7 */ 0x6219a48917bea0de, /* -H7 */ 0xcd241170c085def9, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A8 */ {/* PA8 */ 0x597b551753eec66f, /* pA8 */ 0x506a332c37666eb3, /* NA8 */ 0x2525f592d0ff7dac, /* nA8 */ 0xc3589d636ecb1230, /* BA8 */ 0xdf7fe93ebd515226, /* bA8 */ 0x73c6d32be8900fa5, /* RA8 */ 0xd4bbd390f477f913, /* rA8 */ 0x04325c381abf132e, /* QA8 */ 0x95d16be48fcfa843, /* qA8 */ 0xff86d50aae7f441b, /* KA8 */ 0xaef6a390d10ce0cb, /* kA8 */ 0x63e73e6a08bb9784, /* -A8 */ 0xf0aa7d665aa3a8fa, 0x0},
    /* B8 */ {/* PB8 */ 0x8deb90fc926de591, /* pB8 */ 0x2748b2c553c0733b, /* NB8 */ 0x1174718c83dda23e, /* nB8 */ 0xbcbf7d0a504ea20b, /* BB8 */ 0xd296211a8a00972a, /* bB8 */ 0xe4d33c62df954787, /* RB8 */ 0xc98088b840239579, /* rB8 */ 0x9ff88ffca5360c89, /* QB8 */ 0xbb099e3dc76eabcd, /* qB8 */ 0x59e3fad2e056b18e, /* KB8 */ 0x1cb6ccf53fad8251, /* kB8 */ 0x4f898ae7f00609a9, /* -B8 */ 0x6851bf0971fbc1f9, 0x0},
    /* C8 */ {/* PC8 */ 0xf98c98081db7ec88, /* pC8 */ 0xe17131fe33b984f7, /* NC8 */ 0x050cefa8289a971e, /* nC8 */ 0x29bd65f9dc611b63, /* BC8 */ 0x100c7aa0a69e6768, /* bC8 */ 0xb38c70c6e4c338a8, /* RC8 */ 0xe2b3d3e20ccf903a, /* rC8 */ 0xa1413d41dcb50fd2, /* QC8 */ 0xd6fc5b8e3f482349, /* qC8 */ 0xe85b5625141baed3, /* KC8 */ 0x2759cf9d6df306ec, /* kC8 */ 0x2c09faad8766ee13, /* -C8 */ 0x44e15261d0800493, 0x0},
    /* D8 */ {/* PD8 */ 0x8703eaa6361eb50f, /* pD8 */ 0xa5344cef59242e1b, /* ND8 */ 0x03f96463fcc5afdb, /* nD8 */ 0x68c951049e1ccc07, /* BD8 */ 0xae063cbed6d6e106, /* bD8 */ 0x0a121997ab5a53ab, /* RD8 */ 0x6e69c990e64632c2, /* rD8 */ 0xd9653edc79fd29b7, /* QD8 */ 0xa7ce55e04c12765c, /* qD8 */ 0xc1ac72553ccd9a50, /* KD8 */ 0x4e97816b844cf79f, /* kD8 */ 0x97b919842cafa4e1, /* -D8 */ 0xd53bcbf41a7b69ba, 0x0},
    /* E8 */ {/* PE8 */ 0xcccb65b7f5686aaf, /* pE8 */ 0xe67ebe9c875b0cbd, /* NE8 */ 0xce64d87e7149643e, /* nE8 */ 0x6b713ca03a08000e, /* BE8 */ 0x333fbaa685cecd21, /* bE8 */ 0x092354dc0d6a2f86, /* RE8 */ 0x01e5bb845903871c, /* rE8 */ 0x112bfb6cf0724f63, /* QE8 */ 0xd88bbc88385f1a51, /* qE8 */ 0x7f07f39955c7888d, /* KE8 */ 0x9bc852995b66ea56, /* kE8 */ 0x79ab469f2417a80c, /* -E8 */ 0xe88948c88c965e82, 0x0},
    /* F8 */ {/* PF8 */ 0x858580c16c4d1f18, /* pF8 */ 0x74aa2e24966406b6, /* NF8 */ 0x42184394978a9c92, /* nF8 */ 0x373c1a7ce0df3ecb, /* BF8 */ 0x7e73c0c578bee15e, /* bF8 */ 0xf6e71370b26eb208, /* RF8 */ 0xfd0fc37323a3b009, /* rF8 */ 0x66c959bbb905a316, /* QF8 */ 0xd93d7c6d1fc00ae3, /* qF8 */ 0xab6f2a76ab3ba2ea, /* KF8 */ 0xf7bbefa1ae449444, /* kF8 */ 0xd9b40a625f416f80, /* -F8 */ 0x57cd66545b16183c, 0x0},
    /* G8 */ {/* PG8 */ 0xbf72c16a3873de3c, /* pG8 */ 0x3302d3bdb734320a, /* NG8 */ 0x3ef3f5988bfbc37d, /* nG8 */ 0x58080b01aac8d8fe, /* BG8 */ 0x22dcb4f122db0979, /* bG8 */ 0x345c0b5c76281332, /* RG8 */ 0x1aefbcc29d78d3e8, /* rG8 */ 0xb36a4f93514a904f, /* QG8 */ 0xb76ab29051f47e63, /* qG8 */ 0x66e899c79b4a6df9, /* KG8 */ 0x9c24a0eabe19dfb6, /* kG8 */ 0x498757567d0304f6, /* -G8 */ 0x6e9b57f33fde478c, 0x0},
    /* H8 */ {/* PH8 */ 0x7a006abfafff2410, /* pH8 */ 0x0128750b44223744, /* NH8 */ 0xf1bbbc41412c0ac3, /* nH8 */ 0xad16c2f9ca59e9e0, /* BH8 */ 0x922c82a1715ac48f, /* bH8 */ 0x61ec29194463d014, /* RH8 */ 0x6bc771efef78bf6d, /* rH8 */ 0x1c8a15dfafd8d934, /* QH8 */ 0xdd30f5ff4a552e19, /* qH8 */ 0xc75946734509f7e2, /* KH8 */ 0x48d5e8f1903a023e, /* kH8 */ 0xeda1e0ac1fd26cbe, /* -H8 */ 0x5e8d1435579d024a, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
};

