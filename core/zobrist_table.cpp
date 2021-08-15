// Copyright (C) 2021, by Jay M. Coskey
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "variant.h"
#include "zobrist.h"


namespace hexchess::core {

/// A table of 91 * 2 * 6 = 1092 random values to support board layout hashing
/// and detection of Draws due to board layout repetition during a game.
template<>
Zobrist<Glinski>::ZTable Zobrist<Glinski>::_zobristTable{
    0x6c0271c745ce8842,
    0xcef3bba2e932d1fd,
    0x41e6f7f323c99027,
    0xf64fa74c75c22191,
    0x5f9a99ae392777a9,
    0x7d38c95913878c5b,
    0xc1ee62a8b7fcf28d,
    0x29d59454fac93181,
    0xe49aac1010346012,
    0xd7a03f55af38a7e7,
    0xd42a31bb7909a32f,
    0x802aa074edd484f2,
    0x9ef22a28ae8c39b4,
    0xafcf4679636eb022,
    0xee0ea3811ff28948,
    0x445f27c75e92a1cf,
    0xb80096265ffd8748,
    0x8cb31afeb94e030f,
    0x6eca73a9afb6f697,
    0xe07136f246283a14,
    0xe7dfce9490c0c693,
    0x480dd22d381d1caa,
    0xc8fc18f22d341ef1,
    0x60501aa2d1c4cfb5,
    0xbb1d48d7f5939990,
    0xf6386478a2acc8d6,
    0x385fd3aee847ba7a,
    0x567b3e5b8188276f,
    0x7b797de6a8bf9b50,
    0x84c4d056a9e81a2c,
    0x8b441422db11449f,
    0x02b4d440d1175979,
    0xec99a0ba166ceb6f,
    0xa0c52954bfda3a0d,
    0xf9a5f36adfbeadf3,
    0x2318f8c2dccbd125,
    0x8bfae2957af50f6c,
    0x7a6308b88c6092bc,
    0xc575d3fa3a3987ca,
    0xe42e40f1ebf4c002,
    0xd09955a4932cb95d,
    0xd5dd663becdf3598,
    0x1837649417dab436,
    0x3951aa766f4ea3f6,
    0xa88d6e1954cc9bb3,
    0xc0896de4e8a43f99,
    0xcdb7476ce091f213,
    0x7f10f1c9f81eec73,
    0x295cb7bad8e5d4e2,
    0x3ab306c8d8d0c30e,
    0xef6d78154938766f,
    0x1c164fe14782a6fb,
    0x89baf28c98a5fc10,
    0x58fb71bbf9c3ed28,
    0xa8579517a274e67c,
    0x66ea8004a7e610e5,
    0x925d93ccbc8d648f,
    0x0ff406048f696a95,
    0x0d95efb235bf3863,
    0xd2e0b8e7bc1e49dd,
    0xde9d179cc16eac0d,
    0xff050223a862afcb,
    0xbb7d3fbe6ff467c9,
    0xdf6f8362d0a362b4,
    0x0407a3e2e75c87c1,
    0x64d8548b70457e0c,
    0xce135d9a7af2a583,
    0x9a02aca8cffdfa42,
    0x745610c57f709c4f,
    0xd75755b4fde517f2,
    0xca1605e2f36a2537,
    0xbde5f0553573bf2e,
    0x8bb314e0a25fb91d,
    0x231ef036832848fe,
    0xb7b808fe831e245a,
    0x269adb92294d0409,
    0xa2b47cf55d84c297,
    0x0359c490d8eee979,
    0x8e106b16c281a6f6,
    0x15383507f62494a9,
    0xb7eef3f19931e70e,
    0xd7edff13c0622551,
    0x5f4f8e59c141c2ee,
    0xcc3ba655ea631d70,
    0x13610f56661d9f99,
    0xffb547a08b057671,
    0x06b64c7158b1777d,
    0xbb6ddec821002812,
    0x1b41ed6062edeb21,
    0xdda670e42b6d72c7,
    0x7a9536ac88f81bfb,
    0x3acf056d4167b256,
    0x32c31010f5744291,
    0x4a29e9ff151be7f6,
    0x568af0b030859c3c,
    0xab37b31251723b57,
    0x11349ff30c579cfc,
    0x32bb9a5ff30268d5,
    0xc1eb72be7a7fd0fe,
    0x972da49c36f9dc0f,
    0x88fcfe2f64fc87a4,
    0xbf1e6533ce2207ae,
    0x4cc39d82f0d440ed,
    0xf3cb381407972256,
    0xa72cdcbc879cbc08,
    0xbb6ed42a28391091,
    0x097819d726ff9dd1,
    0xac64eb5089c183a7,
    0x3541f63ac047e59f,
    0xe6ac33242991bcc0,
    0x148ae002135521ea,
    0x65136b953be446d5,
    0x7c620d368a977314,
    0x2f1899d3572b4322,
    0xa3e2419097e5b74e,
    0xf2ce547a766d1acb,
    0xf4eed7aa765f6c19,
    0xbd3dace5cd349304,
    0xaa39df5b1433ad1b,
    0x73b0a8b5b0246d3d,
    0x45122d0f44207a62,
    0x26adce3607cf1d53,
    0x5ea17a484032aaa2,
    0x746d94e4c2ce0f11,
    0x488f7df9a1b6aed3,
    0xbb1b727e9bc90a75,
    0x0f2e0ea76bdfcf18,
    0xecc73591585b4f44,
    0x02cae26bd1f43608,
    0x9608d1cec4e82a57,
    0x2bf98a0c862d13d2,
    0xb70d66babae2f19a,
    0x7b0d7ed2255b0e8a,
    0x31fc6bae20976920,
    0x2525a77101fcbd0b,
    0xfae85e1db27010d7,
    0xd50c488a0dd6d913,
    0xe0da09758580b5d2,
    0xbda147256da7f3bb,
    0x3726fcf995a317f9,
    0x5ebbe81ad6d497fe,
    0xc4b5d62fe63a3b1a,
    0xc96991d4da1fdf43,
    0x9fc133100f32172f,
    0xf27ec59eecef3171,
    0xef67d8ba6d8548d8,
    0x955bec5284fdea87,
    0x027e8e0972805e5b,
    0xc22bd24b8d249271,
    0xcccb1df61ea8f8ce,
    0x35b2925779cf51c4,
    0x3bb6f9bb6cdc4173,
    0x233ece8b5297f94a,
    0x309eddcbf1885489,
    0x62fe9035aacdfbcf,
    0x9718258ef6632658,
    0xa6b3287d334d6951,
    0x1edd880d478805bc,
    0x4aebbac6396320df,
    0x58ab23d2c2494d51,
    0x1a8c6c2f1cde90fa,
    0xe6843c95044277c4,
    0xb5f86ada5f3f0ee4,
    0x43b2f9e9c33f3863,
    0x1d1c14d6e715f938,
    0x0332b5a757bad33f,
    0x40e10438350deb9b,
    0x95ed3ef2306bfb4d,
    0xdac43ae54d169c8c,
    0xc99ce5f1693392b9,
    0x3cd5642a40366f6a,
    0x569d88b22f3d4d8f,
    0xfc6bad73cb677acd,
    0x563038f509a0d428,
    0x9d728c06a525747e,
    0xc30d38c4490965d0,
    0xd69afbbff5bd1199,
    0x9c08e4410dd4f200,
    0xb7df0793a50cb357,
    0x116d9765d1e7dd8e,
    0x5f2e7d6bf377c0d3,
    0x439dfa8b4e8e2a4e,
    0xc67fa2df74197c44,
    0xc9861a7d58913834,
    0x881a53c2c9969563,
    0xdec278a7bb392d90,
    0x48b97d197e510a7b,
    0x9424307acf1c6616,
    0x79074d7e6d2f3f07,
    0x0da06c4fd857f0b7,
    0x5a7a13d63ed30b6c,
    0x1f7e0761cd86efd9,
    0x4e56801b7f93c19b,
    0x5e171e006f56a0de,
    0x620c6699fc132c60,
    0x7ea800d478699eb3,
    0xbbb1444ecab88427,
    0xe3bbfb5c3d6607a5,
    0xb8e0110bf495de57,
    0x56216027b3cc4047,
    0x3040477eb40aa852,
    0x769728fba6295a0b,
    0x931789f62acb2df5,
    0xa9b82a061baa51cb,
    0x766e500c1a591b46,
    0x5bd597c24834b5be,
    0xe16136e6b8fe5faf,
    0xf2b3aa48a524efdc,
    0x1d9ecf27ff56e18d,
    0xe0b524bc91b9cadb,
    0x924cb963b7097e4c,
    0x12bbb078fd25f078,
    0x6c7da973d30375c8,
    0xe6ee7af63ff938c6,
    0x31075eed09ab06a1,
    0xad5bf6bf200908d6,
    0x3883c57eba6f47b0,
    0xd22e9af0c4196862,
    0x69e5eaa660172270,
    0x9aa82ecf398808a9,
    0xd9a4907cf80ad1fe,
    0xba75fd25ed354031,
    0xa556e3d577137808,
    0x253d38445f8d3a77,
    0xcb9df617b72c0595,
    0x7614df78cc489b46,
    0x5a6e2057e85b30f6,
    0x731311adebb9201c,
    0x126cf68a12661519,
    0xd1c1be511d533cc7,
    0x3e8a6c0fe575a56c,
    0xa027bdaf220da160,
    0x422420d89e275688,
    0xddc3a33b45d7be5c,
    0xa6a91103b5b4f334,
    0xfa3f7c7ed8d89323,
    0x509936fb597b823f,
    0xa7f554be79a5c4dd,
    0x654984159a4f1208,
    0xd91203157a346976,
    0xd994e34e7a884252,
    0xed29d1a373b26e15,
    0xb63cbcd7bb58c578,
    0x59b9962996667ee6,
    0x5c724b2406e4737a,
    0x663a2b1a3379d908,
    0x11b658f282663f95,
    0xde06a40612baf8ee,
    0x392f0fff68a232e3,
    0xfaf8eda835862d84,
    0x1017d0e69d989561,
    0xcbe6fb103ed7f8c2,
    0x4e5c59a41a0eb8e2,
    0x0b064b78fb48dd31,
    0x6d64fb5f341e9d83,
    0xcc31d8357864a516,
    0x97850ad797a3aaf1,
    0x1725dc1fa32bf17e,
    0x5210e7ea1d73e810,
    0xfac3a70e1c3770b4,
    0xb7cf0b235405859c,
    0xf85aa279f570098c,
    0x82f69d5a982e8a39,
    0xa525f5fbc7fc2862,
    0x553db215c0fc7eb7,
    0xd4b555b866ef592d,
    0x04f57cdbeca98a38,
    0x2a2d9532f77abd83,
    0x11a6c093dca465be,
    0x0df8465768efd1fa,
    0x3eb9a5e6d5b8f6c5,
    0xaa790e64d6812612,
    0x4328423fe7c0fd01,
    0x7cc61c280651f59c,
    0x63426282956c1067,
    0x0c04a53213460640,
    0xf1a9344ff390d53c,
    0x138aa017e7da6c7e,
    0xc0f8daee46fbf575,
    0x7d922efb4fa1e772,
    0xd55aa69c2c40d974,
    0x20d498360926525a,
    0xb490c9572782a9cb,
    0x1c661b5f21d73dce,
    0x3115a04ef9bab62b,
    0x28a87d33d7033fe7,
    0x3028610f45d52561,
    0x63670ecf532a5e67,
    0xe51c89f092372fe6,
    0x8ba8b81986b3d101,
    0x149cf1d6045e9a29,
    0x71b1a11603256c50,
    0x616105087b094ba7,
    0x442633baa47d9ce9,
    0x92e4b12cccc5368b,
    0xa9566612014c0ca8,
    0xb7f990947e54e4a6,
    0x16996d2a00e9b727,
    0x052891c346496d2d,
    0x102230c5646492e9,
    0xb9430a2b53940449,
    0xa7d56467c5dee3c7,
    0x9b328260213fbfa7,
    0xed422540228a83cd,
    0xf4515ded17e254d5,
    0x06938ed72cb28fea,
    0x0aff2cf9dab04745,
    0xcaf125fb6d2b0f3e,
    0x55fab3ae62a80b67,
    0xc58af45bea1fd10e,
    0xcebe693011c9f361,
    0x5cf056787ac92411,
    0xdc631afb9ea9b69c,
    0x2908eb227597cd2c,
    0xb6705f7f4637d3c6,
    0x1091b4018412ce2f,
    0x9c51c50a1b1127ba,
    0xa169c13e4e0d59de,
    0x6787047d1b033f07,
    0x770b06c3b3146158,
    0x80d5f100802775f4,
    0x93a393f801e348cf,
    0xc68a55d2bdda5f48,
    0x1b309153aea78fdc,
    0x2860cd12cc848e04,
    0x087a9f9bec6e2d16,
    0xc74b55a0a0d61a7d,
    0x3acefc03340efea5,
    0xfc698b669833f37d,
    0x4541fd020bac5a42,
    0xff58c847193b94cb,
    0x8af10d4fa6b81599,
    0x0795725a48df7fe1,
    0x4899deb9d76c4ab3,
    0x593387f9d19a3ab0,
    0x3ab2e1ffdebc242d,
    0xd3463894a84e17c8,
    0xeac21d5c2b509998,
    0xdca60090fe04f74d,
    0xb10cd6d06da6c716,
    0x20d44fa40055e8e8,
    0xd7644b86f4ffd0ef,
    0x8d7c2eb0b4e63d97,
    0xf538d1b05c91f520,
    0x805dcdd9d7557bc9,
    0x33cb3b840887325d,
    0x2abfb3ef3a49bf6e,
    0x84bdc73bc1e8badb,
    0x9bc6d3ad3070dab7,
    0xa9bb5bf64c0faee3,
    0x333d7c5f83f8c0ea,
    0x637580337d8da813,
    0xf3c3824e4d06d55f,
    0xefbe5a1a99173a29,
    0x665e196e4bb6e756,
    0xa068621ff56938f1,
    0xee5b0f07054aa1fb,
    0xd0c83eefa2539ad4,
    0x76ea1d7626c2500f,
    0xfa6f07faf5e6ce94,
    0x9871090e92c43549,
    0x9d58f7fbb3f4c076,
    0xf3a7fce9f10d9a86,
    0xb57224297ea3abff,
    0x9aa64c56c1027532,
    0x0943b840e84946d6,
    0xb51683059fae5f14,
    0x556718366734d838,
    0x7653d3166bd702db,
    0x769fc630ad8e3a7b,
    0x55d2962f22602977,
    0xedede28ad3044ffb,
    0xc527955f6e15da5b,
    0xa0dee9dff3674d26,
    0xcef8cfa49e6e983c,
    0xf1419ee9ebf338ff,
    0xc83ed110d41f1fca,
    0xc295e66200669f1c,
    0x3ad07926ebf922f6,
    0xb42a5f986dec9d05,
    0x789d1013d2cceb6c,
    0xe8baa4ffd34ca7d6,
    0x861d03616b1f2cd6,
    0x9f0b5d65fe810ffa,
    0xb50df4f8fc48d156,
    0x12932180920adc25,
    0x7198c0b0f41611a7,
    0xc28ce31f2646b5ba,
    0xd104dffa5dcaeefd,
    0x326c61049de0e94c,
    0x80b8a5feead4afb7,
    0x09071c8f413d6651,
    0xf0a649f42b0f21f7,
    0x3200a4913156eec6,
    0xedfb0a384dc2f0d8,
    0x33c25d7add332217,
    0x3b61c80b5ba423b0,
    0x51582c324e7453a8,
    0xd4cc52288249d6fe,
    0x9bed6848456635fa,
    0xcc34f020096fb15f,
    0x0f2d8a09779c6462,
    0xb2bbce95d6ba936a,
    0xe68cd29326470357,
    0xe064d4539165549d,
    0x28619df387843369,
    0xaf80a2b9d12f52ec,
    0xb7a298eaf52cb2a9,
    0xf26f338475972b36,
    0xb9b5c8e782afbce2,
    0x576c12ed80d0f927,
    0x84a6b1899465a56b,
    0x5cde7df40bbdf6eb,
    0x458326b9ea52fb2b,
    0x43aa0199bce0021e,
    0x3f63f4c442402773,
    0x0e7eb47307a3209c,
    0xa011cc57870b0d51,
    0xc764a15ae3596175,
    0x27a0e9db968e84dd,
    0x8c5618212bf58c02,
    0x2d64491ca707ff29,
    0x63db7c452948740a,
    0x1ad8c47706db761b,
    0x38188fb32a34cc7a,
    0x1a6707158373882c,
    0xf07d5d737aa19d3c,
    0xe8f44478a14286a5,
    0x21a3403fe62bfd26,
    0x4c7f2a432db73eb4,
    0x9f5370e702605903,
    0x190bf9e7d8365833,
    0xb30c986518844b0f,
    0xb0ce004c4bea2893,
    0x229793e0adbe9803,
    0x29b4998ed7f3e834,
    0x91026a86d0bba4e1,
    0x4277847d5b3526fd,
    0xd7df4f77a22a0b01,
    0x4c6bf32af885c690,
    0x42d6801a0df71178,
    0xe44ee60a163cb829,
    0x7c61dbb4b44ce236,
    0xff7ec199a6779db7,
    0x18f2a949b3edf12a,
    0x114e153652f9d1aa,
    0x91c26d88b521d6b4,
    0xd98375d40b8913c1,
    0xddac8bb3486ea020,
    0x2ff7abc2bc2204b5,
    0x8e0a35c69122a2d9,
    0x5020bfb209f384fb,
    0x0939082a94849663,
    0x0e97427ff868547b,
    0x3fcfa3f00a928f3d,
    0xd0ce485f2eb124fb,
    0x9216e310537a3728,
    0xb3b96ba21c07b9aa,
    0x06ce22f807a128e7,
    0x408a871a7ef0d470,
    0x4573fcbacec7c039,
    0x09f1062bbb8ee9af,
    0x63868974a18c9c93,
    0xddd2f7cbbf53fe08,
    0x5729ca1fbe5f5d34,
    0x13290805befdca21,
    0xb759ccce4fc6bb67,
    0x0f2ad2901b3840e3,
    0xbf19d0a51c885c6d,
    0x8995c1b5c3701c16,
    0xcb61c5378c0e8f26,
    0xf36300d06e77b59c,
    0xa0268b7a959cf31a,
    0x40435bf50aa51bcd,
    0xfd693afdfecd01c7,
    0x5841e60beaf6aac1,
    0x043c6c49cdee69ab,
    0xe1000d551c8cb3b2,
    0x667faa1ce664cb59,
    0xde8bfa82059d1412,
    0xdfc9f0d7daca63f0,
    0x4f7f1d98c2c910ea,
    0x924cf45b02d70192,
    0xf23bf0a0e5fdd92c,
    0x1641b640ed3a225d,
    0xf842eb71a8fae192,
    0x88f832ad170a2b6a,
    0xbf692f66d3b253f3,
    0x6ab8c077bf176e0e,
    0x24c1ba20b69891d8,
    0xc8e66fbfb8303d02,
    0x0432734b1ad54e17,
    0xf2c0711cd1cc2432,
    0x050a154b4e3f3275,
    0xea781f2ae381fb19,
    0x325d6d0b5ace5583,
    0xcf0c5a80a8b8f985,
    0x301217fd733acd2f,
    0xe3595cba97d541e9,
    0x5f16624022032d99,
    0xac8fdd46a543e03f,
    0xc1d675cbea79b1f1,
    0x5188c80f5ef7781f,
    0xaa9c7906a2630345,
    0xe3391a69310c0d6e,
    0x1eab3948e41b2269,
    0x14e8e2a6b14c3b03,
    0x4ce61a49057cd196,
    0x7e18fbe33889319b,
    0xb9d3c78010dc3c18,
    0x79e9c23e73a33c93,
    0x95f3c89cd31453ae,
    0xc6889610d9e38fe1,
    0x29507ba93a2cc0a5,
    0xcf400d696038eda8,
    0x2c4042901559115d,
    0x34dfb5b26d07bfbe,
    0xd4334f874da36921,
    0x8de1361fec605b10,
    0xcf5d2bc4ac2adf51,
    0x3780e23507a25ef4,
    0xb748f2e15b929c1d,
    0xcc8e884820846dc4,
    0x3db8115ae6a7dd8a,
    0xa828d491cd49bf1d,
    0x287d7210471303e1,
    0xaecf180102d821f6,
    0x7cae1fa94f6706c8,
    0xddbfb820b0299c24,
    0xec0997c6bf6d21db,
    0x5f3fc5417311e2f8,
    0xf3bc908e5e26fb2d,
    0x328ed17805dff0d3,
    0x4cf70d165635ce00,
    0xb1ebbed701cb7a93,
    0x4a1c6b719c3ef92a,
    0x590307a2f3704c00,
    0xf701d8dd080974dc,
    0x3a55b5d5d0865c3e,
    0xed1efd5e1d0066a2,
    0x1d00497f80673084,
    0xbc4337fcd1eebba7,
    0xf3a3a039e854e581,
    0x4a17d200d4fa6238,
    0x8a7594160d05514d,
    0xa0a9a41f95d8396c,
    0x4fe46b50b3d1ee71,
    0xeef4d19ef5948950,
    0xa8bb5297d5136707,
    0x1e5c54ecad5bda22,
    0x50f38bcbc7e5cea5,
    0x8a0d5bb202d1c403,
    0x210fb596e312f7fe,
    0x64c44c267ffd838c,
    0xd3062f21394197d7,
    0x279365ef42cb82f9,
    0x08e5a022827b8cae,
    0xe2c6c9783a8c067a,
    0x76956556fcd29f7a,
    0xa536b1690c72603a,
    0xd9384898a533b404,
    0x4ab2f8c135f83932,
    0x39fbc4620d0a9a6a,
    0x8541f9eb24f4245b,
    0x104db4dcc8e0b320,
    0xa9c5626fe1a7a8d9,
    0x9fc2c89aaf393996,
    0x6f90b6144d02bcab,
    0x4fee4b350d8dc041,
    0x915bc04a472fb211,
    0xe967e4c2ea5f1bbe,
    0x46df050e807d5d01,
    0x3baaceda23b0214c,
    0xbc6a6dbfa9d82c50,
    0xe96c1125fa392da7,
    0x50b6878c4e5105b0,
    0x78ba4e2af76b39b5,
    0x93ad2afb6fe36525,
    0x9a47c05709f90499,
    0x6d8a58e6a91da95e,
    0x70eb76f5cd43f68a,
    0xa4eeef5723fc0af2,
    0x333aea6559a38d2e,
    0xd9fa2f82a60a95fe,
    0x32b6e90e213d5c37,
    0xd063fc12cbd1a5c7,
    0x37a8a9ccfc4e9db2,
    0xfcfdf3011bbb9e3f,
    0x7a8a112c5ed81ab5,
    0xe01683238a8d8933,
    0x53f431046e48d7c6,
    0xafc544f453f983a9,
    0xa9b5ae65a52c9fcf,
    0x2efb346b0046275b,
    0x0ba15fc2faef20a4,
    0xdbd4b689a3dd79e0,
    0xd1ae58e9d16c6ff8,
    0x2377284fd4115efb,
    0xdee97a7106417ff1,
    0xa0d19a589bb36ef5,
    0x98782fa05d787867,
    0x49b70e33266eb4b7,
    0x8b7bdbeefaefba1a,
    0x0bda34122552c880,
    0x94b3600f0d4de9c5,
    0xf9bd4117f6c5ed97,
    0x848ede729b56e0e7,
    0x0fde8757090730e2,
    0x94b67f0b4b85264e,
    0x347969b47cfadeeb,
    0xbd9cd531bac5087c,
    0x5a90af8a1a804892,
    0xe5acc6888ba482f3,
    0x2a6bc8c3078c04ee,
    0x9858ce3c09f45396,
    0x8474d7c93d5de64e,
    0xfa4f91a3c622debf,
    0x77617dd76d12c022,
    0x95704c4c5d1e1c8c,
    0x6a78c9c90c2ece97,
    0x0aa6ec8a2392bd50,
    0xa60be848291f586e,
    0x08526c333497bcf2,
    0x02f84274fb0d6a00,
    0xb0ff60bd04dfde6c,
    0x823997481140144b,
    0x08e1efd01b83396f,
    0x2320c9c5e8e5fa2c,
    0x58e40d049c911787,
    0x750344b22dac1ebf,
    0x66389aa23739fcc9,
    0xd7cecec89ccbfc58,
    0xd4b7e8ebe169e42d,
    0xf81cbfd83bdc07f6,
    0x924f904d0d0caa59,
    0x68eff465d6226ff3,
    0x3941d3068f427810,
    0x67b03522b6b1822c,
    0xff631dd42da5e61e,
    0x8adfcba131fe1dcd,
    0x6efda2befd56ee0e,
    0xfe93483cd56e71e9,
    0xe601db2ec27bbc12,
    0x77fee35483fcf765,
    0xcb826f617717e4aa,
    0x7718444ad1b1c645,
    0xa2c7e0dd8888bfee,
    0xb301219b01a89942,
    0x84f3c6f74b14c957,
    0x82ab4172da1e7ea4,
    0x1ea17d5e43680249,
    0xf170f3821edab5ca,
    0x133ab1c7e4a84b68,
    0x71a048469a584673,
    0xb3b8dd9c287222a7,
    0x624b5480e750c7a6,
    0x91f5fef2854cf88c,
    0xc23fae47ffc5e9a5,
    0xbe0aea69dd3f6189,
    0x4fc0c0e07df2b7c9,
    0x710c8e55fafd9da8,
    0x442ef681214b45c4,
    0xc0fe0d4a5685dabc,
    0x449db283e9577680,
    0x4ec56060700a5494,
    0x0c373f97eb7aa0d9,
    0x9d4c38e27bb3aa39,
    0x9287b5cc21187eef,
    0x5da3500c96721777,
    0x0f9976dbae1df363,
    0x49d67dffebc49843,
    0x6aa6da4bf9173e01,
    0x3aa1f662191a08ef,
    0xe16cac14d580430d,
    0x4762b9fd93232d05,
    0xe02da606e9be7532,
    0x1748f2c3f64aff72,
    0xfd6c3e538ee6b40f,
    0xbe6dc3f40f307fa6,
    0x663f95dcd6f3562c,
    0x72ab47928bb3338b,
    0xf742de00758f1953,
    0x89840e96f1c460e2,
    0x287385e5b2d47655,
    0x7d996f86600b073f,
    0x11e0992b4ccb1c3c,
    0xb68d297f1c084359,
    0x9ab9d96ff07d3646,
    0x8b486370e16de9aa,
    0xadcdd424c7395673,
    0x85d97fdc5b71d63c,
    0xdcf9ee6e6dd32c71,
    0xa3ec254f8957d14b,
    0x61a53b089388948d,
    0xd58377c9947b706b,
    0xc153e8440f139f61,
    0x00d49da8fd558314,
    0x82655e7e4357bec7,
    0x0261a785d1235ec0,
    0xd585c07110793047,
    0xe5cc89a5e7d1b268,
    0x715f918dec539f78,
    0xca6988ab5c8d266e,
    0x92ad259becc23988,
    0x22b7417c467c35b0,
    0x90d9b0a1ce7b1231,
    0x3e0cf4dde0a262d9,
    0xe0e8149316f26720,
    0x6930ccae486f51e8,
    0x10a34a4c2c2b79eb,
    0x238fa1ae6385ce5a,
    0x9618a4af85ccaed4,
    0xab261888d6f35108,
    0xb6f1571446b939f9,
    0x33646eb0ea4ce682,
    0xc7efbc33cf974b39,
    0x09e60de107beaa7f,
    0x248e706135eca8a8,
    0xbd3b64ba363bf6b6,
    0x0203710d57b90084,
    0x702e70f50d869c32,
    0x972a678d63ac6d9c,
    0xa6ef458525ce33a7,
    0x3f969716c5e8f6cf,
    0x6f5c760ffe64a961,
    0x26596442fab306ea,
    0x4233a253f90efab3,
    0x349ab74e727e0bac,
    0x1844b982e30252d0,
    0xe50d6a98921eb1e2,
    0xfb8880ecd57c6b84,
    0xfce8448a20b5ba28,
    0xafa947dcf24a0321,
    0xfec70f3747f9e322,
    0x3162d05ea0fdd8af,
    0x3b28b30fd2ec7405,
    0x91a48285bbed85d6,
    0x6b17107d3b26b6eb,
    0xd8fed6726353789d,
    0x58af1b71b43191f6,
    0xcf0256355a7bc788,
    0xc0fa5285d8f92c95,
    0x2d0a7586c2f9f8d6,
    0x33963c21cda7e41d,
    0x5e3e5afe0f7432a4,
    0xc64c537362c655c5,
    0xadb138ef6bbbc749,
    0xba6be4b4284bd42c,
    0x064d8cc4573242bb,
    0x5d76044b93426e84,
    0x0f96326de5a558ea,
    0xe45febdcb158ba9c,
    0x2c051d1477631dc3,
    0x38bc29196c442e80,
    0x0d8dc171be744528,
    0x5cda48130cc43881,
    0x0045ab6f375b2c1c,
    0x621fff6c444669fc,
    0xa3fa3d74ee5c9269,
    0x69878f00bca3b10a,
    0x05193d0248d73fca,
    0x6fe20ac4f5e53629,
    0xe19071d9d89b5d45,
    0x9af57927c3242122,
    0x2bd9bb0d7e4ea3e8,
    0x6aef96490c4b35fa,
    0x0d344c3c4eea1ea3,
    0x6e41c959be1205ff,
    0x55251658f2a65d84,
    0x5c711042d4bd183f,
    0x926373bfff0b1027,
    0xa090120684415e40,
    0xe00be61a13ac5f6f,
    0x05e89e051a0ca366,
    0xd8564d64e3159d83,
    0x85574333d4b5779a,
    0x5e1e3869ce67c688,
    0xa06d58072d7ac2f1,
    0xd69ea183a422bd17,
    0x2c86b13d09db12e9,
    0x798ac271b87f0575,
    0xa87d5de2ebc86319,
    0x3e3c6968f31dd86f,
    0xb6ed4e9b2ca87574,
    0xcf4d91ce6f5ee643,
    0xe3e70d4d2c73fccc,
    0x1e3de2761e2ae45a,
    0xa4f3b30665431491,
    0xd7fc9cc8bbdce1fe,
    0x816afc7d8eb8a3b6,
    0x9a2f06974acfecea,
    0xea2efbeb7ef0a9aa,
    0x52d1434f0c05e566,
    0xc97f940d038e7f89,
    0x9c05b0222791a018,
    0x752452bd1f6636bf,
    0x3abab0842d739742,
    0x637c783255255aed,
    0x79d7e6d43a1cf6c1,
    0x56d5f57f6997a8a6,
    0x46664cb38b0eaf4b,
    0xbe0a013e3e7cb5a8,
    0xe9e44369fec30419,
    0x8436896504091ca4,
    0x8f0c3578d069e443,
    0x398fd88034d37c41,
    0xa57fe308b3aa3f6d,
    0xa3ba6e5d5feb59f5,
    0x85e713c15128f3f9,
    0xbac30b3385452ff8,
    0x5e003f03df6ab196,
    0x832c46835236275f,
    0xe16cc91b2c9e55b3,
    0x35da2230881aa535,
    0x0af1e57c60bf0845,
    0x5d9b4152dcf23b2c,
    0xeeb85d09f55e7734,
    0xbea2a6e946bf691f,
    0x5801cce02fc9c20c,
    0xf118e4f8072cbac2,
    0x642d5d0ca8e203bf,
    0x4d5d13f97bf3c121,
    0x7542933eabfb981b,
    0x5788a35f0ef5a618,
    0xb6b9273f8dfc8b4a,
    0xdf5c003f2b39c575,
    0x302f6f66b961a5eb,
    0xcab0e646793868da,
    0xfe0ededf8f75b878,
    0x897a1f4ee864ee28,
    0x605a1d29f210b9b5,
    0x3005ef1a0e409eee,
    0x908d6af4c129fe06,
    0xcc0f046dfb5051d4,
    0x535037e5a45ce889,
    0x86dfc4f4c24e4d77,
    0x3f37075ec4f45c88,
    0x1798fdc7cbb7c3e6,
    0xebd5efa7fb4e961a,
    0xaf699d6dc4f2241d,
    0x63fbf9f7bbdd5606,
    0xd29b0f4a9fec20f6,
    0xa806ad538b2b7897,
    0xbeee16e02ee5ce6b,
    0x9aee14d931851c3e,
    0x914a3935ceb75ecf,
    0x322220e32eb0d78a,
    0xe08f31e14261a872,
    0x1a3b3da7ae1778a3,
    0xdf2509dc1298f839,
    0xb0fa8d453899c705,
    0x6b41d1d0492cfd6a,
    0xdc2f15bb83a7096f,
    0x1bcb463dcaa1fe74,
    0xa477952817a4dbdd,
    0x1380dbed7f011516,
    0xc3e78a110e0f276e,
    0x7b4b84060466a516,
    0x6aab08268105b3df,
    0xf3d7d7edaef2ab59,
    0x87215723ab6c7bd4,
    0x6b08f314a603ea8b,
    0x1c6acbaad149d906,
    0x694397653ba793a9,
    0x5086406be5cf8fc3,
    0x3512554ccf20fb7f,
    0x130a6a1d93eef0ee,
    0x4db278c725df5044,
    0xbe8303ab4a6e1a90,
    0x94402889ba46804d,
    0x17fd9c56f0d1ab4d,
    0xb499f2f8a1c8dd79,
    0xe0b9c3850158bd2b,
    0x00a0aae2e51b4f7a,
    0xe2680d1f43c6a088,
    0xaa8979cf8deab331,
    0xbc53ce0c54a8aced,
    0x0a79a49aaf6a168a,
    0x313439a967edf084,
    0xb841c5a98fd292f9,
    0x82b173d11aea5034,
    0xf49bfa6e85ec4162,
    0xe9afd71cb9e70df6,
    0x606b60c1bcde1f23,
    0xfc443fb42f436509,
    0x96b019afbbdb1578,
    0xc4c03746fb12ef26,
    0xcf59d7341c2c4206,
    0x33321bf4cee4c43e,
    0x8eefb88a52e4f776,
    0x12f2136ede14df8a,
    0xab7d9bc84d03a1fb,
    0x7535142e38d51066,
    0xfd0384cddbceb1e2,
    0xb01ce86da0474ba1,
    0xa35499a52cfa96db,
    0x4d4e0489a46f609d,
    0xe375378da90d76ab,
    0x85ce7694387575db,
    0x6c4404ae605ec3b2,
    0x1616d5122ad83f5e,
    0x06c2adaf53371e07,
    0xfba41b99b16f005b,
    0x6446e9f9b88ca0b6,
    0x1077b1699df3f591,
    0xb43e48e049a3c6ff,
    0xc26380d9c7e7e53d,
    0xd38b22198a6cbdb8,
    0x901be69814441519,
    0x13aec218ad9f9493,
    0x0c0cb43db10a59e4,
    0x9669bb1f2cf44d1c,
    0x0c71c27318c222d2,
    0xaafdf5c19f285b78,
    0xb2ca5701f78c0966,
    0xfa5d9917a0e022da,
    0xf5a58db6ba9d9486,
    0x0b4c3d11616bfa71,
    0x367de5d28dce7d21,
    0xed863611a265fa4b,
    0x462dca86aef59f93,
    0x9a924c000b312b91,
    0x1e58cb408de4ebb5,
    0xf17b75c6a2d63a5a,
    0x1d90e1c89f8c3eae,
    0x23df62e9662b7cd8,
    0x2b57fbd31f231d2a,
    0xb30ca19824685692,
    0xa07a9246d911de3b,
    0x62e2146d1a63ece5,
    0xbe95221d7abd5c64,
    0xe82110a5f66348b4,
    0x635cf98c690dcf6d,
    0x35360afb36b218a0,
    0xd3fb2e418e2acfdc,
    0xd074b4704daf55d6,
    0x4645fbaa4b68db19,
    0x7edb8ea907c76f4a,
    0x1b97d34e96f2e460,
    0x9a22625cf536dab5,
    0xe2f0a0faa8753e4f,
    0xa303d4a6cd396eef,
    0x8651a16c35dc7791,
    0x330a65c412d1eff2,
    0x470d9f65acea7846,
    0x8b6905fc49cd0ed7,
    0xf109e08bb35d234d,
    0x68774c48556a6171,
    0x7d71398b0e728e7d,
    0x432212cf9d4d2152,
    0xbe82033e508595f2,
    0xe845d1a14343f214,
    0x7b6739bff4230f0d,
    0x6ca2278184583467,
    0xc441dbc0f2288ee1,
    0x1fcd3463c6dca321,
    0xab9e22b129b9b11e,
    0x66c708fd9be2a565,
    0x01329a0ec4380662,
    0x4a660a610a384928,
    0xb6604987c874c6d0,
    0xc5c5f12fc70eae8c,
    0x8594b7920e0a9d78,
    0x6320db4d6c34a82a,
    0x4f62da37f810691d,
    0xdb63cc7424e0d63d,
    0x665e3661f137d26f,
    0x17388d8759c623be,
    0x69bbb25f974cda6b,
    0x28a52b92c8157849,
    0xf937be1b1d1abaa9,
    0xc8ef1293fee4a159,
    0x32413d08947bdcd6,
    0x80078e0439d06cab,
    0x86f36240360cf9af,
    0x0a3c912f047a0934,
    0x91c438550944be82,
    0x6abb63912460a820,
    0x3f1ed530720c622a,
    0xb5d744480c5169b9,
    0x47dcc85718237f8a,
    0x506649db6b453d40,
    0xe8dfbf9d025166bd,
    0x8be508988054c9f8,
    0x6872b53dcbf93c4d,
    0x5c66554905d47a94,
    0xd485084a3729c03c,
    0x193ab4fd3455cc2d,
    0xc4db03f6279cb665,
    0x256e280121aae1ca,
    0x4a483649f8d81c30,
    0x0a36dd77a1fa648c,
    0x72e172f1168f0a58,
    0x537366a67da86b19,
    0x038242bb59f1ea9c,
    0x9837aff9648626db,
    0x369cf094adec7759,
    0x41fe2bdd8e6a5fdc,
    0xe72c26c2098ef570,
    0xb427ae5943c347f0,
    0x55403a94ea566292,
    0x83a90a6a28675053,
    0x138bf7360657ffee,
    0x59f63ac6b907e95e,
    0x92d1b16e402e05c4,
    0x07b0a3d2536f289b,
    0x5fa9dfd3abfde840,
    0x01848dd7cddb3254,
    0xb7f135905d78f7a2,
    0xa853111af9e418e7,
    0x0206303085373c03,
    0xc5e520924887c02f,
    0x055cc96c585b07ac,
    0x2c9747c5a25c6f76,
    0xd9359fe630c15d1e,
    0x99cb82ba6fcc7318,
    0x05f314e57bb0df8d,
    0x2d9bc89dcd08f9b0,
    0xaa2e014f696ab478,
    0x0bc2e9fe30430c40,
    0x83626a270fbb8824,
    0x8654f05138fa6dbc,
    0xee37aaa27f1720ae,
    0xd549b24261bc2d3e,
    0xcb0d24f13d7ea98f,
    0x3df68acf26db7cac,
    0x53d80c43663c7ea2,
    0xd132696de51c83f0,
    0xd8b1335093277866,
    0x4cec2f5e6f173dfc,
    0x7ae84ebda4fa157a,
    0x0efecb64262c7441,
    0x5dacf7eba7922c5f,
    0x6f733ae0df84ee88,
    0xe037df895c6d66bd,
    0x6c2086e29b238f27,
    0x5d0c3afeebe0b613,
    0xe7d9158d43db157a,
    0x8d2b9b6cfb0dffce,
    0x858f1ef0bd6ebfc7,
    0xcbf9d05c9558c301,
    0x1df48eba522026e3,
    0xf01ef645b4c08dfa,
    0x244062f255ab7ec0,
    0xf7ce9a0ca3028943,
    0xb79e67d85c044eff,
    0xa3d611a29973e010,
    0x7af8ac7fd80d3a20,
    0xd95965294177e62b,
    0xdf0b357cdae31369,
    0xff5b3f269afb67d6,
    0xed4dc43f17f541a1,
    0x52a2d36036974a13,
    0xcfe4fc746632805f,
    0xcdc1f4d36cdb3e01,
    0xfd7932b486daed14,
    0xe0a8f83a80d1828c,
    0x4b2dae1c296e87eb,
    0x39a43987fc072583,
    0x23b49f3d723464d8,
    0xf377eba45336dc88,
    0xcd605f496dd37650,
    0x4d8494f26e409c51,
    0x3315076b1c46dbbd,
    0x218b1627f5b1e3c5,
    0x6c7d8e0b64a472cf,
    0x038c88465833e776,
    0x7e65a7b208bd946f,
    0x6e986aac7d50cc1f,
    0xb5318e708b1cc93d,
    0x4c75487023c4e2ef,
    0x066100566ea6a64b,
    0xbeda8749b59413ee,
    0xe4d50b94ca6d65a2,
    0xe8ad715498ef5e00,
    0x066303ee5bf21eb5,
    0x1dab1edc0fe9e10a,
    0xc640d41fbd3837e3,
    0x27982a50f849024f,
    0x13cf85fbb7280a73,
    0x05d24f3da747a2df,
    0x7ec3d538e0c63f63,
    0xe6ab4c511a915e18,
    0x361003d4b0f1f711,
    0xaa3ba6e2d1527ec3,
    0xe8586566277f0dcc,
    0xe740df92e5f3810b,
    0x5670f7a2dce4bcf4,
    0x7689c87e548b8c91,
    0xa5293fee701ec34a,
    0xf0ca991ae91d41b9,
    0x1cd4f5f756e30ae7
};

}  // namespace hexchess::core