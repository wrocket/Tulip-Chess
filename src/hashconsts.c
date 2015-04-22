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
    /* A1 */ {0x82c3e240217eb87c, 0xa6da19eaf54887d4, 0x16695de4585e6e04, 0x82f51b6073dd9014, 0x4752f2651c02a8b5, 0xc29a2a79bb86bcb1, 0x2f428c56d4cdd52d, 0xfc0f0e854e8a0fcf, 0x6756e584f583481b, 0x6c4c0595e9f8881f, 0xe6030c27625b5a08, 0x650d7d134baa8196, 0x9e095960299db5c5, 0x0},
    /* B1 */ {0x56d0a637f14d1cee, 0x993dbcb8104134c1, 0xf86ea314bcac6602, 0xdfa12bd482ca4f4d, 0x8df4fbb2a7f1dbac, 0x41fc271307101387, 0x8cef0e2b7f92ef0d, 0x6dad249871f810f5, 0xdbc3e41dc4098d28, 0xefaf35b584a2b4df, 0xa5cf2f9ab3d21c01, 0xc1d535a8ac0d8ca4, 0x01c8817f294929a9, 0x0},
    /* C1 */ {0x0c5d579da3336099, 0x11724eabd4e80838, 0x91ee55ce7a69756a, 0x8c914374e901ae02, 0x5663a650ec54525b, 0x001f7e7dc6339284, 0xdbd766fc1e739655, 0xbc82aad43bd48c7e, 0x7c52c5b32fde1e03, 0xa33503501e3278fb, 0xbef93a429ff35917, 0x63dab1c7ac4a2570, 0x0990f1d570221b92, 0x0},
    /* D1 */ {0x0d25aae264535842, 0x8475f8928dfef443, 0xa9717215c792696f, 0x2118ffde8c3f091d, 0x545788d60197fb28, 0xc7e7f761e10b7e32, 0x1ecf0cecb1fecf93, 0x17d426030a67bc7c, 0x14e17a73cfea20e6, 0x048481df505b7264, 0xe85c63a6dd25b34a, 0x30db019b82c01d0b, 0xd16f38ee18528c23, 0x0},
    /* E1 */ {0xca56b19fbc285239, 0xbbcd1025f74580ce, 0xe9e037f1c4fa8987, 0x280a75c99a41bf55, 0x650081b79d8e85bd, 0xdb4002c13c6048e6, 0xf0d72b1b647590ad, 0x3bc70ebdf06da8fa, 0x55aab90720833a66, 0x8211fcfd39006509, 0xfa807c9c4f4ce885, 0xc8df942dcdd9d3e3, 0xeeb73cecb02d32de, 0x0},
    /* F1 */ {0x70d7c44806003548, 0x8cd12732c9b07b62, 0x930ee50da1dc3141, 0x12d53176c352fb82, 0xc5a83d6ff6e23b44, 0xda493eeaaeda3696, 0xa08312c3b9bb70f1, 0x394345456ac4fb80, 0xf25adfe39d0ff179, 0xe1ff7fd1d6078ef7, 0x11ecd6c73d7d1036, 0xd382af026325f8cf, 0xdaa89af472956ec5, 0x0},
    /* G1 */ {0xd6a2781d1760be4e, 0x99ee23f696f12a22, 0x9b82fe2e588bc2a8, 0x52197068999fa38f, 0x1486f68495f4994e, 0x3da4d0c22a824027, 0x7a793cd227e0619c, 0x641a27dd7909da95, 0x0ab784ebeb4c32f8, 0x264ac0ff16dc3d0c, 0xcb38d077090fc3d7, 0x5165bdb57f3e5d48, 0xfa43018f1250694f, 0x0},
    /* H1 */ {0x381c599bd1a38fd8, 0xc02a2ed672c7c44b, 0xaca978f9b4c50101, 0x56ba304eab652a74, 0xb550ddf83c0a4e47, 0xcd42a806f3d30183, 0xf8017e9a54a99bba, 0xe9f98801eded53f7, 0xebf3815a05bf9661, 0x1ded8780fe0bd49a, 0xbd7b92a33f0a26ea, 0x185e3a53004ecd8b, 0xcef78ec1af9e8d52, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A2 */ {0x475bf0f7b7e0c7a9, 0x9c14c8cb8b16aa7c, 0x1275c630c31da04e, 0x9b2d77fd3b258f8a, 0x8c798466ffbdd7ad, 0xe0bc11c7d457c203, 0x4df0faaf7787edce, 0x6b42ccbfd1b71e51, 0x35955d02c2c760d3, 0x6776f8eef76cbab3, 0xb23cdb1f25529cbb, 0xda6d608ffe2fcf06, 0xcf9f2425f2e40d9c, 0x0},
    /* B2 */ {0x817ef412e81c2acd, 0x59e81daeb6a9697c, 0xcf84da6a889032c3, 0xf1606c22bb4c7283, 0xc85596d0f1aa2631, 0x35d7caee667e1882, 0x73a1a2a93b2527d8, 0x7e3c95aa09b2bcec, 0xd5a42815762c8358, 0xa3e7fc823de354d9, 0x8a37638d28f3ca3a, 0x483c129bf2a6ede0, 0x27609e930111fbca, 0x0},
    /* C2 */ {0x1a2e477395fab598, 0xff2626013f088f67, 0x95e4cf3f9e6674df, 0xb2004d087bfb0c32, 0xa688630b0f63dbd8, 0x0148250e3ee15cbb, 0x540100420b9149d8, 0xc41f3ae1e54fc8b8, 0xcee3346f8e966e0f, 0xa8037660243b1baf, 0x4bf1a3a6e7d57775, 0x76f2bfe57aa3d217, 0xf944f41712733323, 0x0},
    /* D2 */ {0xefabb53e5dd58957, 0x1088ab4cb112a8b8, 0xb49ff0d3912ec419, 0xc1d671a953464814, 0x6b1f806b0af9bcc1, 0xf52fea20298e45fd, 0xf00e4004e3c1fcc3, 0x9c73257d12184397, 0x30a05fe6d39d6851, 0x34f3181e9bfd5e97, 0xbb49b4e1ade7c273, 0x44637d1b8ae5446f, 0xc12543fb843d5f0b, 0x0},
    /* E2 */ {0x32bc537ff49882f5, 0x5cd2197fed4d1305, 0x03e374d326a6f9a9, 0x0a1698821e9ba837, 0xea0fe2834876e4b4, 0xc8893e7b41c2b330, 0x31e5ebfe851b4af9, 0xa6f0dd00107c67d0, 0x5790e9780f4ce792, 0xed1f0754d1dd4d69, 0x7a17dc1a7c0c6a61, 0xe6329546324fa8a9, 0x07287dc48aaf613f, 0x0},
    /* F2 */ {0x5dc9d74a7ac8e1a7, 0x0bd9c209acda8429, 0xc7e93bcb17fddadd, 0x33002beb525ebc18, 0xa1efc8ebdcd326bc, 0xb69c741786ad1850, 0x9c7f81d674448dfa, 0xa5574765242bb6c7, 0xf848b75931bf7955, 0x61679f73e2759cf6, 0x3d44f00b8fedb1d5, 0xdc9b9a88268986a6, 0xe73488671aadc058, 0x0},
    /* G2 */ {0x790cdaf006b20212, 0xa83a85a3a32f4ff1, 0xae4abb625e69911b, 0x61fac134889e199f, 0x3d86d9e00878fa80, 0x0f2aa063ba924d92, 0x2bb2898fa383a698, 0xf50018120186f30f, 0xc82a2db1c24ada43, 0xdf532770ae27a9cf, 0x69a5f94ae92d38de, 0xb7fe20383d52b383, 0xc1be44c30f4be144, 0x0},
    /* H2 */ {0x030bbce86b62bf3a, 0x90929e7068a86475, 0x28b158836d81dbe4, 0xe40de7371a3604a8, 0xa27e7d1473ca8a53, 0x572591b7a582eceb, 0xd480a3b46938dbf3, 0x76ae5b205f2a0ce5, 0xdf77294404b97768, 0x4f02f40f8de5402a, 0x3b343f34008a4fc1, 0x94159788a7c6ade6, 0xc32d97336a1646fb, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A3 */ {0x8adbc9da3c9b4ee3, 0x90a266509f1f9c90, 0x386950d06fb38510, 0x40e01a760a2bc4cf, 0x0ae3ae1278899a14, 0x233df36e88258678, 0x4aba86d5b44b7128, 0x211ef5102899395f, 0x103cf8241698c6d4, 0x41eaa162197f234a, 0x86897e7fd1582cff, 0xa7f94e104fefd0be, 0x9f538865794598a0, 0x0},
    /* B3 */ {0x577f452b5eb3fc01, 0x446c02f9e38cb732, 0x84dceb56ee220997, 0x1d5261476b6e4fc0, 0xa87af7bd9295b83c, 0x1281b10ee59fe48d, 0xa810e7ce7351a918, 0xf3cd7a026bfea5bf, 0x5a65d849112eb8f8, 0xb23fe2851af11c0b, 0x423316ba83a185b5, 0xfbcc00204c1100c1, 0xa065b03b4eaaee37, 0x0},
    /* C3 */ {0x81a208c18cce2da0, 0x491e28591a641caf, 0xe0e93d2427373782, 0x014964aa89a85f2a, 0x033adefddd797abf, 0x2376189562cb8350, 0xed51feff097f87f4, 0x8ccf3edea9a63c51, 0x5943d38d88ce225f, 0xdc3eb3774db4a111, 0x035b5fc2f89859e5, 0x7d1d69b4b5dd9730, 0x376e2a5f8202cb43, 0x0},
    /* D3 */ {0x1c09df9192cd63da, 0xf8f946e924579a31, 0x6f88694ad1072fe3, 0x7d94dedbcd9cb861, 0x8221c10b70c64c6f, 0x3badc2b0ab8c98a7, 0x4e2c715ddc924dc2, 0xc9022e58e2ae6b67, 0x346c4ed0f3cda7b7, 0x6117223f687d3f83, 0x2d0ca0d7a4fec27f, 0x7a71c93b768600ed, 0x9d0ceef73d797698, 0x0},
    /* E3 */ {0xef687123e75b7a8b, 0x80a7e1b4393b9a8b, 0xa57f0cc1cf033eed, 0x8455a1bc74940f94, 0x64d7fda95b60d0e3, 0x02d3163a8a764820, 0x0187bd9ca0ed22b0, 0x047b42308b2678d4, 0xf2752ae3e168f28b, 0xe88c205781176323, 0xb3a956affa14ad71, 0xa50515b134f8c475, 0x8bf886d2ad3c11db, 0x0},
    /* F3 */ {0xae995d324fcacaab, 0x7fa1fe6352c95e0d, 0xc8c9331ac7c2a8af, 0x06b2dd02042ffb61, 0xfd25acec9105e1a9, 0xed6891661d45f38b, 0x1bafc4530a40085d, 0x086b8e09536f748a, 0xf7e6c7a0ae5715a4, 0xe390634e9bfaa100, 0x142df48a311ce8ac, 0x1e68c197f6d8a235, 0x501dd51e1014d8c5, 0x0},
    /* G3 */ {0x6b0dcd314331f467, 0xfba8a06fbd01db77, 0xc3922bb3577cbb5e, 0x8b42169bb35598b3, 0xf2337a91dd5ad4ab, 0x1286305d3f09c603, 0xe537ba91aa3b9744, 0xa4966d2b9fb8e69b, 0x1085bc9fdd0ef2fd, 0xcf28d0b49e0fd730, 0xde12ca38803ff1ac, 0x540e5813c88faf7f, 0x702f50a2de68f0a4, 0x0},
    /* H3 */ {0xd1a23ae6427f1f01, 0x21eb7a2c93d9bc13, 0x0849ff97c92fe431, 0xc758d339326782b8, 0x8b7d927bf756d176, 0xc8137807a55c64b9, 0xc8b1e93b2a44e2e8, 0xf59e4c4bd7599a53, 0x0b7a7c4327367dff, 0x501e1fb31b153b28, 0x38a7da7aaad42bd9, 0xce93fd681d2c664a, 0xa6957faf4789ae3d, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A4 */ {0x86453125df7a938a, 0xfe77fd04da72f2c9, 0x1612a942aa0d0ad6, 0xf13f57abf2d3b2b1, 0x6819b6457867ae68, 0x14ea499ddb5e4a2a, 0x3f402378c36e4a31, 0xe9c75e415cd9c0a6, 0x7d106468497b4472, 0x1520f983ff0665ae, 0xa43f760ed504c449, 0x40ac6099dea4a441, 0xa842c492eeb78aa0, 0x0},
    /* B4 */ {0xeb76527c41c7555a, 0x0f7beb7525c7f542, 0x50a54b9c6911acec, 0x807f03f41ddb200b, 0x467aa8ffa9d996ad, 0x69d76630cbb11a91, 0xd49bdb17d4936eb2, 0x029d02d14ad1cc22, 0x80e9d3ee9254a635, 0xe93dc695aaed9c49, 0x5db52b62c4ccca95, 0xd3ea8ad7f42beafb, 0x2a381a9ff8668c9a, 0x0},
    /* C4 */ {0x620a3972d8fd6daf, 0x02fa64c968dd1116, 0xe9c12be79ce902bd, 0x240ab1cb296ec605, 0x5d11e604b7e2fa8b, 0x55395e4c2c1b0e3c, 0xf5e3dbb53669a1a3, 0x4b82ab177f72855d, 0x0187b953771d25e4, 0x4cf17ca889590e6e, 0x9be32102129dc09e, 0x39033265458e579d, 0x28ae9e9763ec626f, 0x0},
    /* D4 */ {0x11768514ceb993d2, 0x76b3f0c7dd40d025, 0xab52c40e6263bd9e, 0xabc1651a7be06097, 0x77c9b70ee2e5db10, 0xd52956e40f08dc0d, 0x6108f6168afc83bb, 0x170f234e6d986074, 0x604fa2206d012fdc, 0x5bf96884b7e439bf, 0x8b5647aa69625926, 0x092faa861a83c0fd, 0x4d676c05ff7d9d98, 0x0},
    /* E4 */ {0x41fc287803437931, 0x71e5e0e785b6d05a, 0xa4e3fd561eed83c3, 0x89b70f2918ffd73d, 0xe33ae9838841e551, 0x31318f98c1df846e, 0xd80130e444756e0e, 0x27341f958ac131f3, 0x7a7e96b1eaf5d51f, 0xffea10d2ef24f075, 0x8300b25db7adacb3, 0x202286fb731541cb, 0x383ca1ef12711fee, 0x0},
    /* F4 */ {0xd17afd11e148e2db, 0xc97d1c5c332fcfc0, 0xa7fd93aa02210093, 0x9960b5b4e180cedd, 0x1fc8a8dabde4cfea, 0x7dd469cc3e7d30a9, 0xa5d855faced97e6f, 0x59deea03cf46ed20, 0x5a874767b76d979b, 0x50c8e1afd0e9b7d3, 0xc47eff842ddc9ae7, 0x395d3bd769b010cd, 0x0ff598f83b2423a0, 0x0},
    /* G4 */ {0xc5cff12d27aaa77c, 0x24a67adafe8ed60d, 0x2457f51f39b6e4bd, 0x0ab1795f6e27a76e, 0x7a72ce4d265595c0, 0x2acc0951345d470a, 0x33457798e7974e3a, 0xf92405e2d186732b, 0xde3a9af295628450, 0x529cd07b057524bc, 0xd6b3f7c424fdbaf3, 0x80c051b040dd680e, 0x28f24c7329ba082a, 0x0},
    /* H4 */ {0xdbff79345e71a81e, 0x57c369c39163bc23, 0x60998f680aa93d6c, 0x2c25f0d8722db832, 0x4ccb5641466b6767, 0x57bdf89164a752c4, 0x6ba34ccd17ca1ad4, 0x3dc6291dd402dad3, 0x39dbb6a25af25ae4, 0xe9b4710674502ec4, 0x6d92471310861877, 0x89ebf8cfadf154d9, 0x29785cc4fc78fcc1, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A5 */ {0xe3a8e9fcac837aa2, 0x423e4816140f4ece, 0xb3f91b5d044ab00f, 0x3439d262b949c465, 0xac2ae3ad09ee58a3, 0x8e5dc78b970e4137, 0x425dc9166b1a48e9, 0x3fb8ba763af8a2c6, 0x2580a22732107bb3, 0x8e1dcc1f24366812, 0xabb8cd4802185336, 0x4752e633c346e896, 0x9e6f023685ecfbcb, 0x0},
    /* B5 */ {0x2c612d97d735e398, 0xc963d77a0d72a711, 0xb3e4a00987c50b1c, 0xb4a6cd3df66c58c9, 0x0e18fcee8c918054, 0xfbe671bb26d5300b, 0x97d06e35e9a3b535, 0x4e79597529e44e29, 0x7c42b5057cd9d6cb, 0x9e168c08932c4580, 0xcaad92079a1bb4bd, 0x6f6ea01a6eaa3924, 0x9af23494a3df9ab2, 0x0},
    /* C5 */ {0xaf0ca2c342b822a5, 0x9841a04cef086198, 0x715f95e0695053cb, 0x7255bc1ef71bd3fe, 0xcbe3cc831f01e489, 0x90c449601d0b9b7a, 0x715d6b3f8dcdc8c5, 0xb5de2ec057d94635, 0xd2e41701e9a7b5eb, 0x7b173779cba59fa3, 0x24bfc3f2c957229e, 0x413f8e6634826d57, 0x7750a594b98a382a, 0x0},
    /* D5 */ {0xcf63726a1930d99a, 0xbd3e111d6a21717f, 0x717f9b0c15a82bac, 0xba43352ab9f3a9b0, 0x8e45bdef14b39763, 0x919ea256d4d3d030, 0xaef96f4a3d416289, 0xa37dd5a850aa900b, 0x08b62795e4e9cc67, 0x0b4dda7608276495, 0x85802575371ba679, 0xf19f5ba538270459, 0x1dbf5b38e075ce2e, 0x0},
    /* E5 */ {0x787d82c409752495, 0xc65a04647d473eaf, 0xe234a855ebd3ef7a, 0xcf8521a38e808995, 0x94662c6f1158f14a, 0x8ed4ece7c99ef402, 0xc8cc701bc61b7d3f, 0xc8e39f5deb4811aa, 0x7cd91ddc4e872038, 0x332c229e70d4c9bb, 0x94254b28fef2401f, 0x6bd9305663030c71, 0x49f227d142efa069, 0x0},
    /* F5 */ {0x6ab5bf0c3a028cf6, 0xe92ee3e1db499b4e, 0xb14aa9d650c44a4d, 0x87b459bede04dbaf, 0x856200c115561b8f, 0xb2e4b29d84855d9e, 0xa1cb92d812a629f7, 0xed49466d6f00f1aa, 0x3ce755c9f5e2e996, 0x0e82c21732340d28, 0x1f8c8f8745df4ac0, 0x66030244966d51f7, 0x79ecc80ec955d2ab, 0x0},
    /* G5 */ {0x2bc6e8929fa6e9f5, 0x9fed28ab11200941, 0x6e910bf8f001a09c, 0x977e9e8ff3ee12d1, 0xfb1196d34b4825c6, 0xbb747e0bcf13b714, 0x7616a29918fe227b, 0xab1a5c8f666437ef, 0x7b9d0cc4b955f63a, 0xa75acec894ca59b8, 0x94de48855d99bc72, 0xdcd75f411a79804b, 0x70bca933c0ddb17b, 0x0},
    /* H5 */ {0x746b84ede2897359, 0xd67c73ae2df876eb, 0xeeb762565e58010d, 0xfe985e3e18a3f8c1, 0xe7735f29ae0f0e2e, 0x3c670981c8ca21cd, 0xf7d4735670b70eb4, 0x480458e59b1f7e78, 0x6f70d1a3b08b83ef, 0x511445fcc2f190dc, 0x27d7bb5793cdb832, 0x70c5f5b47db63325, 0xae203089c640f6ce, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A6 */ {0xa2b5baf21dfb6fce, 0xee2c8b749fedbaaa, 0x2fca11d28607a738, 0x6a3efe90b2340a0e, 0x6e7ffe4ebeef9b3b, 0x807f04c9106ee74e, 0x6327aa762b598dbf, 0xc3eb8afc94e75fa4, 0x27329b501d437868, 0xb67cfcfd2ac67b5a, 0xc00b6cd25b61409b, 0x1e38b6d6222a673e, 0x86467c856bc8a14f, 0x0},
    /* B6 */ {0x288833f90b42a2b5, 0x3f58010f07adbafd, 0x9a088095ae4526a7, 0xa262ea2d98b45eb5, 0x6123b4e7898fc42f, 0x93d6f6e919953cf9, 0x7052be720c2c6e6b, 0x3aaa3e3844f7bd26, 0x94f54249d40b01b1, 0x924e14fe9bba0cdd, 0x4c0b280a22c1e541, 0x5053db867d5a6c60, 0x5500ac4a15830301, 0x0},
    /* C6 */ {0x6f12d8cfdbda807f, 0xa468ced9e07f9321, 0x34f1cd4d24613809, 0xaaf5a4c9d0cc4b0f, 0xe5881bf2620d74f3, 0x8fe520a5bba530f6, 0xb3d13c7327186a25, 0x05b89c28bfc9aa50, 0xa6aa92ada3a08981, 0x2e7979743122fcdf, 0x913c44edb52b1075, 0x20fa3c7d926dc357, 0x2f47eee58ce9289f, 0x0},
    /* D6 */ {0x40d7b0b2df5cfe67, 0x297dfd1c804ea475, 0xf44d694e86fee616, 0x8d53ed7253ebb7c7, 0xa9b0bc3ba73f0118, 0xec4d0da260ac7b91, 0xd6f8628ee8b98566, 0x2b27942d6ae0ae55, 0x4b5f55a45b00e30f, 0x790ecd6e96fb88ac, 0x71580dbda99e6735, 0x07af1c05be3a4237, 0x34bcbd1bc819891d, 0x0},
    /* E6 */ {0xdcc479d91e5d708c, 0xc5344e158fd9c0d0, 0xaadefdc56ac3fb1c, 0xc3497c2d10e5442a, 0xe0da3e51e2b8080e, 0x098b1584905a8ece, 0x21d06f7d22ac018e, 0x23cd35509c12577e, 0x7534626b2d7c2702, 0xb14799527eb398c6, 0xd745b7a9fbb59b04, 0x4e3c3bfcdcc389bb, 0xc9871b808f22d1a6, 0x0},
    /* F6 */ {0xff8a6de2f5c03d87, 0x344769ae84ee9c6b, 0x27670514b5c8d3ce, 0xbcc67ef227e8ef31, 0x42cfe37b3f9f1fd0, 0x8f786387626ae8d8, 0x95c6cd3fe044fcef, 0xecb748ed36238277, 0x3dd2cd48fc40c6d2, 0x270f7a138aee118a, 0x23d70f1f54ef1b76, 0x124fa16e6affb220, 0x81f364d65057c8b2, 0x0},
    /* G6 */ {0xd644ca6991b1e6fa, 0x5fa067f365c73ba0, 0xe8cc81387277da75, 0x867b7850ac502efe, 0x0325d9a5059cd069, 0x0667e1ae388d237e, 0x5be34d462dfe7479, 0xa08804f2af5d4681, 0xe5bed2b781a73e53, 0xe7d6308d7fc7fc9e, 0x939772062e5990da, 0xffd6038a105e6b63, 0x32b4d8404124832c, 0x0},
    /* H6 */ {0x94a141b2f33771ff, 0xf36a41aad8962910, 0xeade61c954d4f4f9, 0x524ce2de62bf67ac, 0xb19bc8b08236d33f, 0x2ef696b0fa062fb8, 0x098b94cd9b7e1a21, 0xd99f92eb6762a97f, 0x4b59caedb823767a, 0xeb2ba601b7ac6216, 0xf96dd677cd1ca5b0, 0xd12dfc328b76067c, 0xf457a3fbbc577073, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A7 */ {0xcfa2125d8bc81998, 0xfb9e0e0f0ae868a4, 0x2dc293f0cc5a2d21, 0x50af34fd64951900, 0x18f8b5133e42ef73, 0xdf1f1da4dd3f7d3e, 0x132b36f30cd2fd57, 0x1ebc69b4b49994f6, 0xcf3ae6995915a1e8, 0xf8f2ce213932d464, 0x400251432d814a5b, 0x286c39b5ffbe0bf9, 0x2da35ca681b4e6a2, 0x0},
    /* B7 */ {0x534da3de25e83c8d, 0x48b1cedca37fe2a2, 0x0de3ca1d549a6741, 0xe7ace8292b7fb0c3, 0xcdafe68baa664aa0, 0xdf24e7b83b732c98, 0x817f9517fdcfdad8, 0x92bc14f455947368, 0x38fe0b645410016c, 0xff1ebb4939070ff9, 0xf78989c66d74a750, 0x806e8911dcc17d14, 0x15d7a18d8d37fb65, 0x0},
    /* C7 */ {0xae7640233ccb8384, 0x7e0b080ede181ccf, 0x73d19e54972a054b, 0xc7d29bad908738c2, 0xc556cf5852cf7324, 0x459d20c4899f435c, 0x0fb6ad9cbf82d1f2, 0x6c4c96894da4f2df, 0x80edf576b434d0cf, 0xa22de2b6bb9f430a, 0x83b9c0a9d8b480aa, 0x2a6c39ba43044da7, 0xaea92c66d8145b2e, 0x0},
    /* D7 */ {0x428ae83109059034, 0xf2e81998d7a2b55f, 0xc7f4bc21868321dc, 0xad594b916a461d9c, 0x026860ec23f99e65, 0xa60ab927b6b19afc, 0x228962d3c2218b66, 0x382f6deb3aada583, 0xaf270c065f6880d0, 0x4ed2ca1c8757be1a, 0x75b753aa9757838a, 0xada55de0e17f93c8, 0x7941ccd4bdfa9692, 0x0},
    /* E7 */ {0x0d97e0e096316f07, 0xedd2c82b7bc790d6, 0x3506754b49cd20cc, 0x8d05518ddb818cbe, 0x7dcf7f2a0fcfad84, 0x11bc1b6d6459da5e, 0x0862c6a9d4c9f017, 0x5779aa593e47cdd8, 0x861d5f932b37f78c, 0xb3d4a9330c10bd14, 0x8ecfbff515b4d567, 0xa41db3a7f9814eb2, 0xe7ff6db1e7ecd4eb, 0x0},
    /* F7 */ {0x90aea375fadcdc24, 0x368ea23529ec029b, 0xadfbbc8d0f20dc2e, 0x4c6b76b7ba21073c, 0x5b976d32bfa40db7, 0x5c140cc4790b1319, 0x34240bb77d1cf9ba, 0x958ffec668a87afb, 0x1bac5238886762ea, 0x41131d42dc241bc4, 0xd4be4642b6e91135, 0x828ff23f31acecfe, 0x2562d26433277331, 0x0},
    /* G7 */ {0xa5bb2da973507b10, 0xbb225c9a09b08881, 0x088dbf340213573c, 0x448265e3ec197ad3, 0x7c606f38c7368a3b, 0x505a1fd20522c0bd, 0x52327ba37ecff4ce, 0x6a313fe036a71f50, 0x01db5654b7e96f66, 0xeb61eb73ba54502d, 0x73acbd189c55c5c0, 0x7ede54b668f4d6f7, 0x1682cc21589077b7, 0x0},
    /* H7 */ {0xcd241170c085def9, 0x74157d3cf2380d29, 0x31f1a5faa46b3010, 0xe600637d2f3b59ca, 0xc4dc4b35fd6c4b1e, 0xb51a6ec0083bf95e, 0xde1b4dc2a031d2ab, 0x12a2df280c1ac784, 0xec99b3b03f612fe3, 0x68c2a6805fbd92e7, 0xe34df8656e6b2bb3, 0x2c0cd3711bfa6372, 0x6219a48917bea0de, 0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /*    */ {0x0},
    /* A8 */ {0xf0aa7d665aa3a8fa, 0x597b551753eec66f, 0x506a332c37666eb3, 0x2525f592d0ff7dac, 0xc3589d636ecb1230, 0xdf7fe93ebd515226, 0x73c6d32be8900fa5, 0xd4bbd390f477f913, 0x04325c381abf132e, 0x95d16be48fcfa843, 0xff86d50aae7f441b, 0xaef6a390d10ce0cb, 0x63e73e6a08bb9784, 0x0},
    /* B8 */ {0x6851bf0971fbc1f9, 0x8deb90fc926de591, 0x2748b2c553c0733b, 0x1174718c83dda23e, 0xbcbf7d0a504ea20b, 0xd296211a8a00972a, 0xe4d33c62df954787, 0xc98088b840239579, 0x9ff88ffca5360c89, 0xbb099e3dc76eabcd, 0x59e3fad2e056b18e, 0x1cb6ccf53fad8251, 0x4f898ae7f00609a9, 0x0},
    /* C8 */ {0x44e15261d0800493, 0xf98c98081db7ec88, 0xe17131fe33b984f7, 0x050cefa8289a971e, 0x29bd65f9dc611b63, 0x100c7aa0a69e6768, 0xb38c70c6e4c338a8, 0xe2b3d3e20ccf903a, 0xa1413d41dcb50fd2, 0xd6fc5b8e3f482349, 0xe85b5625141baed3, 0x2759cf9d6df306ec, 0x2c09faad8766ee13, 0x0},
    /* D8 */ {0xd53bcbf41a7b69ba, 0x8703eaa6361eb50f, 0xa5344cef59242e1b, 0x03f96463fcc5afdb, 0x68c951049e1ccc07, 0xae063cbed6d6e106, 0x0a121997ab5a53ab, 0x6e69c990e64632c2, 0xd9653edc79fd29b7, 0xa7ce55e04c12765c, 0xc1ac72553ccd9a50, 0x4e97816b844cf79f, 0x97b919842cafa4e1, 0x0},
    /* E8 */ {0xe88948c88c965e82, 0xcccb65b7f5686aaf, 0xe67ebe9c875b0cbd, 0xce64d87e7149643e, 0x6b713ca03a08000e, 0x333fbaa685cecd21, 0x092354dc0d6a2f86, 0x01e5bb845903871c, 0x112bfb6cf0724f63, 0xd88bbc88385f1a51, 0x7f07f39955c7888d, 0x9bc852995b66ea56, 0x79ab469f2417a80c, 0x0},
    /* F8 */ {0x57cd66545b16183c, 0x858580c16c4d1f18, 0x74aa2e24966406b6, 0x42184394978a9c92, 0x373c1a7ce0df3ecb, 0x7e73c0c578bee15e, 0xf6e71370b26eb208, 0xfd0fc37323a3b009, 0x66c959bbb905a316, 0xd93d7c6d1fc00ae3, 0xab6f2a76ab3ba2ea, 0xf7bbefa1ae449444, 0xd9b40a625f416f80, 0x0},
    /* G8 */ {0x6e9b57f33fde478c, 0xbf72c16a3873de3c, 0x3302d3bdb734320a, 0x3ef3f5988bfbc37d, 0x58080b01aac8d8fe, 0x22dcb4f122db0979, 0x345c0b5c76281332, 0x1aefbcc29d78d3e8, 0xb36a4f93514a904f, 0xb76ab29051f47e63, 0x66e899c79b4a6df9, 0x9c24a0eabe19dfb6, 0x498757567d0304f6, 0x0},
    /* H8 */ {0x5e8d1435579d024a, 0x7a006abfafff2410, 0x0128750b44223744, 0xf1bbbc41412c0ac3, 0xad16c2f9ca59e9e0, 0x922c82a1715ac48f, 0x61ec29194463d014, 0x6bc771efef78bf6d, 0x1c8a15dfafd8d934, 0xdd30f5ff4a552e19, 0xc75946734509f7e2, 0x48d5e8f1903a023e, 0xeda1e0ac1fd26cbe, 0x0},
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

